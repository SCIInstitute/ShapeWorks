#include "Logging.h"

#include <spdlog/fmt/bundled/chrono.h>

#include <boost/date_time.hpp>
#include <boost/date_time/date_facet.hpp>
#include <iostream>

#include "spdlog/spdlog.h"

const std::string log_datetime_format_ = "yyyy-MM-dd_HH_mm_ss";

namespace shapeworks {

Logging::Logging() {}

Logging &Logging::Instance() {
  static Logging instance;
  return instance;
}

void Logging::open_file_log(std::string filename) {
  if (log_.is_open()) {
    SW_CLOSE_LOG();
  }
  log_filename_ = filename;
  log_.open(filename);
  log_.flush();
}

void Logging::log_message(std::string message, const int line, const char *file) {
  if (log_.is_open()) {
    log_ << message;
    log_.flush();
  }
  std::cerr << message;
}

}  // namespace shapeworks
