#pragma once

#include <QObject>

//! Handle logger callbacks via Qt for thread correctness
class StudioLogger : public QObject {
  Q_OBJECT

 public:
  StudioLogger() {}
  virtual ~StudioLogger() {}

  void register_callbacks();

  // callbacks from logger
  void handle_message(std::string str);
  void handle_error(std::string str);
  void handle_warning(std::string str);
  void handle_debug(std::string str);

 Q_SIGNALS:
  void message(std::string str);
  void error(std::string str);
  void warning(std::string str);
  void debug(std::string str);
};
