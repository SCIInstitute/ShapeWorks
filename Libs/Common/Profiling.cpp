#include "Profiling.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTextStream>
#include <QThread>
#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace shapeworks {

//---------------------------------------------------------------------------
TimerStackEntry::TimerStackEntry(const QString& name, qint64 start_time_us)
    : name(name), start_time_us(start_time_us), accumulated_child_time_ms(0.0) {}

//---------------------------------------------------------------------------
Profiler::Profiler() : profiling_enabled_(false), tracing_enabled_(false), start_time_us_(0) {
  // Check environment variables
  const char* profile_env = std::getenv("SW_TIME_PROFILE");
  if (profile_env) {
    QString profile_str = QString(profile_env).toLower();
    profiling_enabled_ = (profile_str == "on" || profile_str == "1" || profile_str == "true");
  }

  const char* trace_env = std::getenv("SW_TIME_TRACE");
  if (trace_env) {
    QString trace_str = QString(trace_env).toLower();
    tracing_enabled_ = (trace_str == "on" || trace_str == "1" || trace_str == "true");
  }

  if (profiling_enabled_ || tracing_enabled_) {
    elapsed_timer_.start();
    start_time_us_ = std::chrono::duration_cast<std::chrono::microseconds>(
                         std::chrono::high_resolution_clock::now().time_since_epoch())
                         .count();
  }
}

//---------------------------------------------------------------------------
Profiler::~Profiler() {}

//---------------------------------------------------------------------------
Profiler& Profiler::instance() {
  static Profiler instance;
  return instance;
}

//---------------------------------------------------------------------------
void Profiler::start_timer(const QString& name) {
  if (!profiling_enabled_ && !tracing_enabled_) {
    return;
  }

  QMutexLocker locker(&mutex_);
  Qt::HANDLE thread_id = QThread::currentThreadId();
  qint64 current_time_us = elapsed_timer_.nsecsElapsed() / 1000;

  // Add to timer stack
  auto& stack = timer_stacks_[thread_id];
  stack.push_back(std::make_unique<TimerStackEntry>(name, current_time_us));

  // Add trace event if tracing enabled
  if (tracing_enabled_) {
    TraceEvent event;
    event.name = name;
    event.phase = "B";
    event.timestamp_us = start_time_us_ + current_time_us;
    event.thread_id = thread_id;
    event.process_id = QCoreApplication::applicationPid();
    trace_events_.push_back(event);
  }
}

//---------------------------------------------------------------------------
void Profiler::stop_timer(const QString& name) {
  if (!profiling_enabled_ && !tracing_enabled_) {
    return;
  }

  QMutexLocker locker(&mutex_);
  Qt::HANDLE thread_id = QThread::currentThreadId();
  qint64 current_time_us = elapsed_timer_.nsecsElapsed() / 1000;

  auto& stack = timer_stacks_[thread_id];
  if (stack.empty()) {
    qWarning() << "Timer stack underflow for" << name;
    return;
  }

  // Find matching timer in stack (should be at top, but handle mismatched calls)
  auto it = std::find_if(stack.rbegin(), stack.rend(),
                         [&name](const std::unique_ptr<TimerStackEntry>& entry) { return entry->name == name; });

  if (it == stack.rend()) {
    qWarning() << "No matching start timer found for" << name;
    return;
  }

  auto& entry = **it;
  double elapsed_ms = (current_time_us - entry.start_time_us) / 1000.0;

  if (profiling_enabled_) {
    // Update profile entry
    auto& profile_entry = profile_entries_[name];
    profile_entry.name = name;
    profile_entry.thread_id = thread_id;
    profile_entry.call_count++;
    profile_entry.inclusive_time_ms += elapsed_ms;
    profile_entry.exclusive_time_ms += (elapsed_ms - entry.accumulated_child_time_ms);

    // Update parent's subcall count and child time
    if (stack.size() > 1) {
      auto parent_it = stack.end() - 2;
      if (std::distance(stack.begin(), parent_it) >= 0) {
        (*parent_it)->accumulated_child_time_ms += elapsed_ms;
        auto& parent_profile = profile_entries_[(*parent_it)->name];
        parent_profile.subcall_count++;
      }
    }
  }

  // Add trace event if tracing enabled
  if (tracing_enabled_) {
    TraceEvent event;
    event.name = name;
    event.phase = "E";
    event.timestamp_us = start_time_us_ + current_time_us;
    event.thread_id = thread_id;
    event.process_id = QCoreApplication::applicationPid();
    trace_events_.push_back(event);
  }

  // Remove from stack
  stack.erase(it.base() - 1);
}

//---------------------------------------------------------------------------
void Profiler::finalize() {
  if (!profiling_enabled_ && !tracing_enabled_) {
    return;
  }

  QMutexLocker locker(&mutex_);

  if (profiling_enabled_) {
    write_profile_report();
  }

  if (tracing_enabled_) {
    write_trace_file();
  }
}
void Profiler::write_profile_report() {
  // Calculate total time
  double total_time_ms = elapsed_timer_.elapsed();

  // Group entries by thread
  std::unordered_map<Qt::HANDLE, std::vector<ProfileEntry*>> entries_by_thread;
  for (auto& pair : profile_entries_) {
    entries_by_thread[pair.second.thread_id].push_back(&pair.second);
  }

  QString report;
  QTextStream stream(&report);

  int tid = 0;
  for (const auto& thread_pair : entries_by_thread) {
    Qt::HANDLE thread_id = thread_pair.first;
    auto entries = thread_pair.second;

    // Sort by inclusive time (descending)
    std::sort(entries.begin(), entries.end(),
              [](const ProfileEntry* a, const ProfileEntry* b) { return a->inclusive_time_ms > b->inclusive_time_ms; });

    stream << QString("THREAD %1: ").arg(tid++) << "\n";
    stream
        << "------------------------------------------------------------------------------------------------------\n";
    stream << QString("%1 %2 %3 %4 %5 %6 %7 %8\n")
                  .arg("   %Time", 8)
                  .arg("Exclusive", 12)
                  .arg("Inclusive", 12)
                  .arg("#Calls", 8)
                  .arg("#Child", 8)
                  .arg("Exclusive", 12)
                  .arg("Inclusive", 12)
                  .arg("Name");
    stream << QString("%1 %2 %3 %4 %5 %6 %7 %8\n")
                  .arg("", 8)
                  .arg("total ms", 12)
                  .arg("total ms", 12)
                  .arg("", 8)
                  .arg("", 8)
                  .arg("ms/call", 12)
                  .arg("ms/call", 12)
                  .arg("");
    stream
        << "------------------------------------------------------------------------------------------------------\n";

    for (const auto* entry : entries) {
      double percent = (entry->inclusive_time_ms / total_time_ms) * 100.0;
      double exclusive_msec_per_call = entry->call_count > 0 ? entry->exclusive_time_ms / entry->call_count : 0.0;
      double inclusive_msec_per_call = entry->call_count > 0 ? entry->inclusive_time_ms / entry->call_count : 0.0;

      stream << QString("%1 %2 %3 %4 %5 %6 %7 %8\n")
                    .arg(QString::number(percent, 'f', 1), 8)
                    .arg(QString::number(entry->exclusive_time_ms, 'f', 0), 12)
                    .arg(QString::number(entry->inclusive_time_ms, 'f', 0), 12)
                    .arg(entry->call_count, 8)
                    .arg(entry->subcall_count, 8)
                    .arg(QString::number(exclusive_msec_per_call, 'f', 0), 12)
                    .arg(QString::number(inclusive_msec_per_call, 'f', 0), 12)
                    .arg(entry->name);
    }
    stream
        << "------------------------------------------------------------------------------------------------------\n\n";
  }

  // Output to console
  std::cout << report.toStdString() << std::endl;

  // Write to file
  QFile file("profile.txt");
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream file_stream(&file);
    file_stream << report;
    file.close();
    std::cout << "Profile written to profile.txt" << std::endl;
  } else {
    qWarning() << "Failed to write profile.txt";
  }
}
//---------------------------------------------------------------------------
void Profiler::write_trace_file() {
  QJsonObject root;
  QJsonArray events;

  for (const auto& event : trace_events_) {
    QJsonObject json_event;
    json_event["name"] = event.name;
    json_event["ph"] = event.phase;
    json_event["ts"] = event.timestamp_us;
    json_event["tid"] = static_cast<qint64>(reinterpret_cast<quintptr>(event.thread_id));
    json_event["pid"] = event.process_id;
    events.append(json_event);
  }

  root["traceEvents"] = events;

  QJsonDocument doc(root);

  QFile file("trace.json");
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    file.write(doc.toJson());
    file.close();
    std::cout << "Trace written to trace.json" << std::endl;
  } else {
    qWarning() << "Failed to write trace.json";
  }
}

//---------------------------------------------------------------------------
QString Profiler::format_time(double ms) {
  if (ms >= 1000.0) {
    return QString::number(ms / 1000.0, 'f', 3) + "s";
  } else {
    return QString::number(ms, 'f', 3) + "ms";
  }
}

//---------------------------------------------------------------------------
ScopedTimer::ScopedTimer(const QString& name)
    : name_(name), enabled_(Profiler::instance().is_profiling_enabled() || Profiler::instance().is_tracing_enabled()) {
  if (enabled_) {
    Profiler::instance().start_timer(name_);
  }
}

//---------------------------------------------------------------------------
ScopedTimer::~ScopedTimer() {
  if (enabled_) {
    Profiler::instance().stop_timer(name_);
  }
}

}  // namespace shapeworks
