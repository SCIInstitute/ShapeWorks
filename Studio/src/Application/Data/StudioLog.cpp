
#include "StudioLog.h"
#include <iostream>
#include <QDateTime>
#include <QString>
#include <QDir>

using namespace shapeworks;

const QString StudioLog::log_datetime_format_ = "yyyy-MM-dd_HH_mm_ss";
const QString StudioLog::datetime_format_ = "yyyy-MM-dd HH:mm:ss.zzz";

//-----------------------------------------------------------------------------
StudioLog::StudioLog()
{
  this->new_log();
}

//-----------------------------------------------------------------------------
StudioLog& StudioLog::Instance()
{
  static StudioLog instance;
  return instance;
}

//-----------------------------------------------------------------------------
void StudioLog::new_log()
{
  // close out old session if it exists
  if (this->log_.is_open()) {
    STUDIO_CLOSE_LOG();
  }

  QDateTime date_time = QDateTime::currentDateTime();
  QString session_name = date_time.toString(StudioLog::log_datetime_format_);
  QString path = QDir::homePath() + QDir::separator() + ".shapeworks";

  QDir dir(path);
  if (!dir.exists()) {
    dir.mkpath(".");
  }

  QString logfile = path + QDir::separator() + "studio-" + session_name + ".txt";

  std::cerr << "filename = " << logfile.toStdString() << "\n";
  this->log_.open(logfile.toStdString());
  this->log_.flush();

}

//-----------------------------------------------------------------------------
void StudioLog::log_message(QString message, const int line, const char* file)
{
  QString str = this->generate_header(line, file) + QString(" MESSAGE: ") + message;
  this->log_ << str.toStdString() << "\n";
  std::cerr << str.toStdString() << "\n";
}

//-----------------------------------------------------------------------------
void StudioLog::log_stack(std::string message)
{
  this->log_ << message;
  this->log_.flush();
  std::cerr << message;
}

//-----------------------------------------------------------------------------
void StudioLog::log_error(QString message, const int line, const char* file)
{
  QString str = this->generate_header(line, file) + QString(" ERROR: ") + message;
  this->log_ << str.toStdString() << "\n";
  std::cerr << str.toStdString() << "\n";
  this->log_.flush();
}

//-----------------------------------------------------------------------------
void StudioLog::close_log()
{
  if (!this->log_.is_open()) {
    return;
  }

  STUDIO_LOG_MESSAGE("Log Closed");
}

//-----------------------------------------------------------------------------
void StudioLog::flush_log()
{
  if (!this->log_.is_open()) {
    return;
  }

  this->log_.flush();
}

//-----------------------------------------------------------------------------
void StudioLog::log_debug(QString message, const int line, const char* file)
{
  QString str = this->generate_header(line, file) + QString(" DEBUG: ") + message;
  this->log_ << str.toStdString() << "\n";
  this->log_.flush();
}

//-----------------------------------------------------------------------------
QString StudioLog::generate_header(const int line, const char* filename)
{
  const char* name = (strrchr(filename, '/') ? strrchr(filename, '/') + 1 : filename);
  const char* name2 = (strrchr(name, '\\') ? strrchr(name, '\\') + 1 : name);
  QString header =
    "[" + this->get_current_datetime() + "|" + name2 + "|" + QString::number(line) + "]";
  return header;
}

//-----------------------------------------------------------------------------
QString StudioLog::get_current_datetime()
{
  QDateTime date_time = QDateTime::currentDateTime();
  return date_time.toString(StudioLog::datetime_format_);
}

