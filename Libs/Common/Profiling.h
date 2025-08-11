#pragma once

#include <QMutex>
#include <QString>
#include <QElapsedTimer>
#include <QThread>
#include <QTextStream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>

namespace shapeworks {

struct Profile_Entry {
  QString name;
  double exclusive_time_ms;
  double inclusive_time_ms;
  int call_count;
  int subcall_count;
  Qt::HANDLE thread_id;
};

struct Trace_Event {
  QString name;
  QString phase;  // "B" for begin, "E" for end
  qint64 timestamp_us;
  Qt::HANDLE thread_id;
  int process_id;
};

class Timer_Stack_Entry {
public:
  Timer_Stack_Entry(const QString& name, qint64 start_time_us);

  QString name;
  qint64 start_time_us;
  double accumulated_child_time_ms;
};

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
  std::unordered_map<Qt::HANDLE, std::vector<std::unique_ptr<Timer_Stack_Entry>>> timer_stacks_;
  std::unordered_map<QString, Profile_Entry> profile_entries_;
  std::vector<Trace_Event> trace_events_;

  QElapsedTimer elapsed_timer_;
};

class Scoped_Timer {
public:
  Scoped_Timer(const QString& name);
  ~Scoped_Timer();

private:
  QString name_;
  bool enabled_;
};

} // namespace shapeworks

// Profiling macros
#define TIME_START(name) \
do { \
      if (shapeworks::Profiler::instance().is_profiling_enabled() || shapeworks::Profiler::instance().is_tracing_enabled()) { \
        shapeworks::Profiler::instance().start_timer(name); \
  } \
} while(0)

#define TIME_STOP(name) \
    do { \
      if (shapeworks::Profiler::instance().is_profiling_enabled() || shapeworks::Profiler::instance().is_tracing_enabled()) { \
        shapeworks::Profiler::instance().stop_timer(name); \
  } \
} while(0)

#define TIME_SCOPE(name) \
shapeworks::Scoped_Timer _scoped_timer_##__LINE__(name)

#define TIME_FINALIZE() \
    shapeworks::Profiler::instance().finalize()
