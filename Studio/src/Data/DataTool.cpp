#include <iostream>

#include <QMessageBox>
#include <QThread>

#include <Data/Session.h>
#include <Data/StudioMesh.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/DataTool.h>

#include <Libs/Groom/GroomParameters.h>

#include <ui_DataTool.h>

namespace shapeworks {

//---------------------------------------------------------------------------
DataTool::DataTool(Preferences& prefs) : preferences_(prefs)
{
  ui_ = new Ui_DataTool;
  ui_->setupUi(this);
}

//---------------------------------------------------------------------------
DataTool::~DataTool()
{}

//---------------------------------------------------------------------------
void DataTool::set_session(QSharedPointer<Session> session)
{
  this->session_ = session;
}

//---------------------------------------------------------------------------
}
