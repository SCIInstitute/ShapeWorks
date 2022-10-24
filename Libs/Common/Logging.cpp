#include "Logging.h"

#include <spdlog/cfg/helpers.h>
#include <spdlog/details/os.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace spd = spdlog;

namespace shapeworks {

//-----------------------------------------------------------------------------
static std::string create_header(const int line, const char *filename) {
  const char *name = (strrchr(filename, '/') ? strrchr(filename, '/') + 1 : filename);
  const char *name2 = (strrchr(name, '\\') ? strrchr(name, '\\') + 1 : name);
  std::string header = "[" + std::string(name2) + "|" + std::to_string(line) + "]";
  return header;
}

//-----------------------------------------------------------------------------
Logging::Logging() {
  auto env_val = spd::details::os::getenv("SW_LOG_LEVEL");
  if (!env_val.empty()) {
    spd::cfg::helpers::load_levels(env_val);
  }
  spd::flush_every(std::chrono::seconds(3));
}

//-----------------------------------------------------------------------------
Logging& Logging::Instance() {
  static Logging instance;
  return instance;
}

//-----------------------------------------------------------------------------
void Logging::open_file_log(const std::string& filename) {
  try {
    auto logger = spd::basic_logger_mt("file", filename);
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
    logger->set_level(spd::get_level());
    log_open_ = true;
  } catch (const spd::spdlog_ex& ex) {
    spd::error(std::string("Log file init failed: ") + ex.what());
  }

  log_filename_ = filename;
}

//-----------------------------------------------------------------------------
bool Logging::check_log_open() const { return log_open_; }

//-----------------------------------------------------------------------------
std::string Logging::get_log_filename() const { return log_filename_; }

//-----------------------------------------------------------------------------
void Logging::log_message(const std::string& message, const int line, const char *file) const {
  spd::info(message);
  if (log_open_) {
    spd::get("file")->info(message);
  }
  if (message_callback_) {
    message_callback_(message);
  }
}

//-----------------------------------------------------------------------------
void Logging::log_stack(const std::string& message) const {
  spd::error(message);
  if (log_open_) {
    spd::get("file")->error(message);
  }
}

//-----------------------------------------------------------------------------
void Logging::log_error(const std::string& message, const int line, const char *file) const {
  spd::error(message);
  if (log_open_) {
    spd::get("file")->error(message);
  }
  if (error_callback_) {
    error_callback_(message);
  }
}

//-----------------------------------------------------------------------------
void Logging::show_message(const std::string& message, const int line, const char *file) const {
  log_message(message, line, file);
  if (message_callback_) {
    message_callback_(message);
  }
}

//-----------------------------------------------------------------------------
void Logging::show_status(const std::string& message, const int line, const char *file) const {
  log_message(message, line, file);
  if (message_callback_) {
    message_callback_(message);
  }
}

//-----------------------------------------------------------------------------
void Logging::log_debug(const std::string& message, const int line, const char *file) const {
  std::string str = create_header(line, file) + " " + message;
  spd::debug(str);
  if (log_open_) {
    spd::get("file")->debug(str);
  }
  if (debug_callback_) {
    debug_callback_(str);
  }
}

//-----------------------------------------------------------------------------
void Logging::log_warning(const std::string& message, const int line, const char *file) const {
  spd::warn(message);
  if (log_open_) {
    spd::get("file")->warn(message);
  }
  if (warning_callback_) {
    warning_callback_(message);
  }
}

//-----------------------------------------------------------------------------
void Logging::close_log() {
  if (!log_open_) {
    return;
  }

  SW_LOG("Log Closed");
  log_open_ = false;
}

//-----------------------------------------------------------------------------
void Logging::set_error_callback(const std::function<void(std::string)>& callback) { error_callback_ = callback; }

//-----------------------------------------------------------------------------
void Logging::set_message_callback(const std::function<void(std::string)>& callback) { message_callback_ = callback; }

//-----------------------------------------------------------------------------
void Logging::set_warning_callback(const std::function<void(std::string)>& callback) { warning_callback_ = callback; }

//-----------------------------------------------------------------------------
void Logging::set_debug_callback(const std::function<void(std::string)>& callback) { debug_callback_ = callback; }

}  // namespace shapeworks
