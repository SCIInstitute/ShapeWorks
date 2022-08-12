#include "Logging.h"

#include <spdlog/fmt/bundled/chrono.h>

#include <boost/date_time.hpp>
#include <boost/date_time/date_facet.hpp>
#include <iostream>

#include "spdlog/spdlog.h"

namespace shapeworks {

//-----------------------------------------------------------------------------
static std::string get_current_datetime() {
  auto now = boost::posix_time::second_clock::local_time();
  return fmt::format("yyyy-MM-dd HH:mm:ss.zzz", to_tm(now));
}

//-----------------------------------------------------------------------------
static std::string create_header(const int line, const char *filename) {
  const char *name = (strrchr(filename, '/') ? strrchr(filename, '/') + 1 : filename);
  const char *name2 = (strrchr(name, '\\') ? strrchr(name, '\\') + 1 : name);
  std::string header = "[" + get_current_datetime() + "|" + name2 + "|" + std::to_string(line) + "]";
  return header;
}

//-----------------------------------------------------------------------------
Logging::Logging() {}

//-----------------------------------------------------------------------------
Logging &Logging::Instance() {
  static Logging instance;
  return instance;
}

//-----------------------------------------------------------------------------
void Logging::open_file_log(std::string filename) {
  if (log_.is_open()) {
    SW_CLOSE_LOG();
  }
  log_filename_ = filename;
  log_.open(filename);
  log_.flush();
}

//-----------------------------------------------------------------------------
bool Logging::check_log_open() { return log_.is_open(); }

//-----------------------------------------------------------------------------
std::string Logging::get_log_filename() { return log_filename_; }

//-----------------------------------------------------------------------------
void Logging::log_message(std::string message, const int line, const char *file) {
  if (log_.is_open()) {
    log_ << message << "\n";
    log_.flush();
  }
  std::cerr << message << "\n";
}

//-----------------------------------------------------------------------------
void Logging::log_stack(std::string message) {
  log_ << message;
  log_.flush();
  std::cerr << message;
}

//-----------------------------------------------------------------------------
void Logging::log_error(std::string message, const int line, const char *file) {
  std::string str = create_header(line, file) + std::string(" ERROR: ") + message;
  log_ << str << "\n";
  std::cerr << str << "\n";
  log_.flush();
}

//-----------------------------------------------------------------------------
void Logging::show_error(std::string message, const int line, const char *file) {
  log_error(message, line, file);
  if (error_callback_) {
    error_callback_(message);
  }
}

//-----------------------------------------------------------------------------
void Logging::show_message(std::string message, const int line, const char *file) {
  log_message(message, line, file);
  if (message_callback_) {
    message_callback_(message);
  }
}

//-----------------------------------------------------------------------------
void Logging::log_debug(std::string message, const int line, const char *file) {
  std::string str = create_header(line, file) + std::string(" DEBUG: ") + message;
  log_ << str << "\n";
  log_.flush();
}

//-----------------------------------------------------------------------------
void Logging::close_log() {
  if (!log_.is_open()) {
    return;
  }

  SW_LOG_MESSAGE("Log Closed");
}

//-----------------------------------------------------------------------------
void Logging::flush_log() {
  if (!log_.is_open()) {
    return;
  }

  log_.flush();
}

//-----------------------------------------------------------------------------
void Logging::set_error_callback(std::function<void(std::string)> callback) { error_callback_ = callback; }

//-----------------------------------------------------------------------------
void Logging::set_message_callback(std::function<void(std::string)> callback) { message_callback_ = callback; }

}  // namespace shapeworks
