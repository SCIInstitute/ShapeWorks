#pragma once

#include <spdlog/fmt/fmt.h>

namespace shapeworks {

/**
 * \class Logging
 * \ingroup Group-Common
 *
 * \brief ShapeWorks Logging Library
 *
 * The Logging class supports univeral console and file logging.
 *
 * To use the ShapeWorks logging library, use the pre-defined macros:
 *
 * Normal log message:
 * \code
 * SW_LOG("message")
 * \endcode
 *
 * Warning:
 * \code
 * SW_WARN("warning")
 * \endcode
 *
 * Error:
 * \code
 * SW_ERROR("error")
 * \endcode
 *
 * Debug:
 * \code
 * SW_DEBUG("debug")
 * \endcode
 *
 * Output (assuming it was line 25 of Analyze.cpp and SW_LOG_LEVEL=debug):
 * \code
 * [2022-08-18 11:40:01.021] [debug] [Analyze.cpp|25] debug
 * \endcode
 *
 * The logging macros using the fmt library.  Read more here:
 *
 * https://github.com/fmtlib/fmt
 *
 * Example:
 * \code
 * int mode = 10;
 * double eigen_value = 42.3;
 * SW_LOG("eigen value [{}]: {}", mode, eigen_value);
 * \endcode
 *
 * Output:
 * \code
 * [2022-08-18 11:37:42.613] [info] eigen value [10]: 42.32
 * \endcode
 *
 *
 *
 *
 */
class Logging {
 public:
  //! Return the singleton instance
  static Logging& Instance();

  //! Create a file log
  void open_file_log(std::string filename);

  //! Return if the log is open
  bool check_log_open();

  //! Return the log filename
  std::string get_log_filename();

  //! Log a message, use SW_LOG macro
  void log_message(std::string message, const int line, const char* file);

  //! Log a stack trace message, use SW_LOG_STACK macro
  void log_stack(std::string message);

  //! Log an error, use SW_ERROR macro
  void log_error(std::string message, const int line, const char* file);

  //! Log a message, use SW_MESSAGE macro
  void show_message(std::string message, const int line, const char* file);

  //! Log a message, use SW_STATUS macro
  void show_status(std::string message, const int line, const char* file);

  //! Log a debug message, use SW_DEBUG macro
  void log_debug(std::string message, const int line, const char* file);

  //! Log a warning message, use SW_WARN macro
  void log_warning(std::string message, const int line, const char* file);

  //! Close the log, use SW_CLOSE_LOG macro
  void close_log();

  //! Set an error callback function to be called whenever an error is raised
  void set_error_callback(std::function<void(std::string)> callback);

  //! Set an message callback function to be called whenever an message is posted
  void set_message_callback(std::function<void(std::string)> callback);

 private:
  //! Constructor
  Logging();

  std::string log_filename_;
  bool log_open_ = false;

  std::function<void(std::string)> error_callback_;

  std::function<void(std::string)> message_callback_;
};

//! Log stack macro
#define SW_LOG_STACK(message) shapeworks::Logging::Instance().log_stack(message)

//! Log message macro
#define SW_LOG(message, ...) \
  shapeworks::Logging::Instance().log_message(fmt::format(message, ##__VA_ARGS__), __LINE__, __FILE__)

//! Log warning macro
#define SW_WARN(message, ...) \
  shapeworks::Logging::Instance().log_warning(fmt::format(message, ##__VA_ARGS__), __LINE__, __FILE__)

//! Log error macro
#define SW_ERROR(message, ...) \
  shapeworks::Logging::Instance().log_error(fmt::format(message, ##__VA_ARGS__), __LINE__, __FILE__)

//! Log debug macro
#define SW_DEBUG(message, ...) \
  shapeworks::Logging::Instance().log_debug(fmt::format(message, ##__VA_ARGS__), __LINE__, __FILE__)

//! Log show message macro
#define SW_MESSAGE(message, ...) \
  shapeworks::Logging::Instance().show_message(fmt::format(message, ##__VA_ARGS__), __LINE__, __FILE__)

//! Don't write to log, but set status (e.g. in the Studio statusbar)
#define SW_STATUS(message, ...) \
  shapeworks::Logging::Instance().show_status(fmt::format(message, ##__VA_ARGS__), __LINE__, __FILE__)

//! Close session macro
#define SW_CLOSE_LOG() shapeworks::Logging::Instance().close_log();

//! Close session macro
#define SW_FLUSH_LOG() shapeworks::Logging::Instance().flush_log();
}  // namespace shapeworks
