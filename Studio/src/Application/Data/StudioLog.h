#pragma once

#include <string>
#include <fstream>
#include <QString>

namespace shapeworks {

class StudioLog {

public:

  StudioLog();

  //! Return the singleton instance
  static StudioLog& Instance();

  //! Create a new session
  void new_log();

  //! Log a message, use STUDIO_LOG_MESSAGE macro
  void log_message(QString message, const int line, const char* file);

  //! Log a stack trace message, use STUDIO_LOG_STACK macro
  void log_stack(std::string message);

  //! Log an error, use STUDIO_LOG_ERROR macro
  void log_error(QString message, const int line, const char* file);

  //! Log a debug message, use STUDIO_LOG_DEBUG macro
  void log_debug(QString message, const int line, const char* file);

  //! Close the log, use STUDIO_CLOSE_LOG macro
  void close_log();

  //! Flush log to disk
  void flush_log();

  //! Return if the log is open
  bool check_log_open();

  //! Return the log filename
  QString get_log_filename();

private:

  QString create_header(const int line, const char* filename );

  QString get_current_datetime();

  std::ofstream log_; //! log file

  QString log_filename_;

  //! Datetime format used
  static const QString datetime_format_;

  //! Datetime format used
  static const QString log_datetime_format_;
};



//! Log stack macro
#define STUDIO_LOG_STACK(message) \
  shapeworks::StudioLog::Instance().log_stack( message )

//! Log message macro
#define STUDIO_LOG_MESSAGE(message) \
  shapeworks::StudioLog::Instance().log_message( message, __LINE__, __FILE__ )

//! Log error macro
#define STUDIO_LOG_ERROR(message) \
  shapeworks::StudioLog::Instance().log_error( message, __LINE__, __FILE__ )

//! Close session macro
#define STUDIO_CLOSE_LOG() \
  shapeworks::StudioLog::Instance().close_log();

//! Close session macro
#define STUDIO_FLUSH_LOG() \
  shapeworks::StudioLog::Instance().flush_log();
}