#pragma once

#include <string>
#include <fstream>

namespace shapeworks {

class Logging {
 public:
  Logging();

  //! Return the singleton instance
  static Logging& Instance();

  //! Create a new session
  void open_file_log(std::string filename);

  //! Log a message, use SW_LOG_MESSAGE macro
  void log_message(std::string message, const int line, const char* file);

  //! Log a stack trace message, use SW_LOG_STACK macro
  void log_stack(std::string message);

  //! Log an error, use SW_LOG_ERROR macro
  void log_error(std::string message, const int line, const char* file);

  //! Log an error, use SW_SHOW_ERROR macro
  void show_error(std::string message, const int line, const char* file);

  //! Log a message, use SW_SHOW_MESSAGE macro
  void show_message(std::string message, const int line, const char* file);

  //! Log a debug message, use SW_LOG_DEBUG macro
  void log_debug(std::string message, const int line, const char* file);

  //! Close the log, use SW_CLOSE_LOG macro
  void close_log();

  //! Flush log to disk
  void flush_log();

  //! Return if the log is open
  bool check_log_open();

  //! Return the log filename
  std::string get_log_filename();

 private:
  std::string create_header(const int line, const char* filename);

  std::string get_current_datetime();

  std::ofstream log_;  //! log file

  std::string log_filename_;

  //! Datetime format used
  static const std::string datetime_format_;

  //! Datetime format used
  static const std::string log_datetime_format_;
};

//! Log stack macro
#define SW_LOG_STACK(message) shapeworks::Logging::Instance().log_stack(message)

//! Log message macro
#define SW_LOG_MESSAGE(message) shapeworks::Logging::Instance().log_message(message, __LINE__, __FILE__)

//! Log error macro
#define SW_LOG_ERROR(message) shapeworks::Logging::Instance().log_error(message, __LINE__, __FILE__)

//! Log show error macro
#define SW_SHOW_ERROR(message) shapeworks::Logging::Instance().show_error(message, __LINE__, __FILE__)

//! Log show message macro
#define SW_SHOW_MESSAGE(message) shapeworks::Logging::Instance().show_message(message, __LINE__, __FILE__)

//! Close session macro
#define SW_CLOSE_LOG() shapeworks::Logging::Instance().close_log();

//! Close session macro
#define SW_FLUSH_LOG() shapeworks::Logging::Instance().flush_log();
}  // namespace shapeworks
