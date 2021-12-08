#pragma once

#include <Data/Preferences.h>

#include <QElapsedTimer>
#include <QObject>
#include <QProgressDialog>
#include <QSharedPointer>
#include <QWidget>
#include <string>

class Ui_DataTool;

namespace shapeworks {

class Session;
class LandmarkTableModel;

class DataTool : public QWidget {
  Q_OBJECT;

 public:
  DataTool(Preferences& prefs);
  ~DataTool();

  //! Set the pointer to the session
  void set_session(QSharedPointer<Session> session);

  //! activate this tool
  void activate();

  void disable_actions();

  void enable_actions();

  void update_table();

  void update_notes();

  std::string get_notes();

  void store_data();


 public Q_SLOTS:

  void new_landmark();

  void delete_button_clicked();

 Q_SIGNALS:
  void import_button_clicked();

 private:
  Preferences& preferences_;

  Ui_DataTool* ui_;
  QSharedPointer<Session> session_;

  std::shared_ptr<LandmarkTableModel> landmark_table_model_;
};
}  // namespace shapeworks
