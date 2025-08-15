#pragma once

#include <QElapsedTimer>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QString>
#include <QTextStream>
#include <QThread>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

namespace shapeworks {

//---------------------------------------------------------------------------
struct ProfileEntry {
  QString name;
  double exclusive_time_ms;
  double inclusive_time_ms;
  int call_count;
  int subcall_count;
  Qt::HANDLE thread_id;
};

//---------------------------------------------------------------------------
struct TraceEvent {
  QString name;
  QString phase;  // "B" for begin, "E" for end
  qint64 timestamp_us;
  Qt::HANDLE thread_id;
  int process_id;
};

//---------------------------------------------------------------------------
class TimerStackEntry {
 public:
  TimerStackEntry(const QString& name, qint64 start_time_us);

  QString name;
  qint64 start_time_us;
  double accumulated_child_time_ms;
};

//---------------------------------------------------------------------------
class Profiler {
 public:
  static Profiler& instance();

  void start_timer(const QString& name);
  void stop_timer(const QString& name);
  void finalize();

  bool is_profiling_enabled() const { return profiling_enabled_; }
  bool is_tracing_enabled() const { return tracing_enabled_; }

 private:
  Profiler();
  ~Profiler();

  void write_profile_report();
  void write_trace_file();
  QString format_time(double ms);

  bool profiling_enabled_;
  bool tracing_enabled_;
  qint64 start_time_us_;

  QMutex mutex_;
  std::unordered_map<Qt::HANDLE, std::vector<std::unique_ptr<TimerStackEntry>>> timer_stacks_;
  std::unordered_map<QString, ProfileEntry> profile_entries_;
  std::vector<TraceEvent> trace_events_;

  QElapsedTimer elapsed_timer_;
};

//---------------------------------------------------------------------------
class ScopedTimer {
 public:
  ScopedTimer(const QString& name);
  ~ScopedTimer();

 private:
  QString name_;
  bool enabled_;
};

}  // namespace shapeworks

// Profiling macros
#define TIME_START(name) shapeworks::Profiler::instance().start_timer(name);
#define TIME_STOP(name) shapeworks::Profiler::instance().stop_timer(name);
#define TIME_SCOPE(name) shapeworks::ScopedTimer _scoped_timer_##__LINE__(name)
#define TIME_FINALIZE() shapeworks::Profiler::instance().finalize()
