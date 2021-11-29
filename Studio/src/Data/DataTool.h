#pragma once

#include <string>

#include <QWidget>
#include <QSharedPointer>
#include <QProgressDialog>
#include <QElapsedTimer>
#include <QObject>

#include <Data/Preferences.h>
#include <Groom/QGroom.h>
#include <Libs/Groom/GroomParameters.h>

class Ui_DataTool;

Q_DECLARE_METATYPE(std::string)

namespace shapeworks {

class Session;
class ShapeWorksStudioApp;

class DataTool : public QWidget {
  Q_OBJECT;
public:

  DataTool(Preferences& prefs);
  ~DataTool();

  //! Set the pointer to the session
  void set_session(QSharedPointer<Session> session);

  //! activate this tool
  void activate();

private:

  Preferences& preferences_;

  Ui_DataTool* ui_;
  QSharedPointer<Session> session_;
};
}
