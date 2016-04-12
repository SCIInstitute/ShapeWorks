#include <iostream>

#include <QXmlStreamWriter>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QThread>
#include <QMessageBox>

#include <Visualization/ShapeWorksStudioApp.h>

#include <Optimize/OptimizeTool.h>
#include <Visualization/ShapeworksWorker.h>
#include <Data/Project.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

#include <ui_OptimizeTool.h>

//---------------------------------------------------------------------------
OptimizeTool::OptimizeTool(Preferences& prefs) : preferences_(prefs) {
  this->ui_ = new Ui_OptimizeTool;
  this->ui_->setupUi( this );
}

//---------------------------------------------------------------------------
OptimizeTool::~OptimizeTool()
{}

//---------------------------------------------------------------------------
void OptimizeTool::on_export_xml_button_clicked()
{
  std::cerr << "Export XML\n";

  QString filename = QFileDialog::getSaveFileName( this, tr( "Save as..." ),
                                                   QString(), tr( "XML files (*.xml)" ) );
  if ( filename.isEmpty() )
  {
    return;
  }

  this->export_xml( filename );
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_error() {
	this->progress_->setValue(100);
  QApplication::processEvents();
	delete this->progress_;
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_progress(int val) {
	if (val < 90 && this->progress_)
		this->progress_->setValue(val);
  QApplication::processEvents();
}

//---------------------------------------------------------------------------
void OptimizeTool::on_run_optimize_button_clicked()
{
  this->progress_ = new QProgressDialog(QString("Running Shapeworks Tool. Please Wait..."), QString(), 0, 100, this);
  this->progress_->setWindowModality(Qt::WindowModal);
  this->progress_->show();
  QApplication::processEvents();
  this->progress_->setValue(5);
  QApplication::processEvents();
  this->app_->set_status_bar("Please wait: running optimize step...");
  auto shapes = this->project_->get_shapes();
  std::vector<ImageType::Pointer> imgs;
  for (auto s : shapes) {
    imgs.push_back(s->get_groomed_image());
  }
  //this->optimize_ = ShapeWorksOptimize(imgs, true);


  QThread *thread = new QThread;
  /*ShapeworksWorker *worker = new ShapeworksWorker(
    ShapeworksWorker::Optimize, this->optimize_, this->project_);
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_thread_complete()));
  connect(worker, SIGNAL(step_made(int)), this, SLOT(handle_progress(int)));
  connect(worker, SIGNAL(error_message(std::string)), this, SLOT(handle_error(std::string)));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();*/

}

//---------------------------------------------------------------------------
void OptimizeTool::handle_thread_complete() {
  //handle failed shape load from project
  QVector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();
  if (shapes.size() == 0) {
        this->progress_->setValue(100);
        QApplication::processEvents();
        delete this->progress_;
        emit optimize_complete();
  }
  //create the list of wpts files
  QFileInfo fi( shapes[0]->get_original_filename_with_path() );
  QString project_path = fi.dir().absolutePath();
  std::string name = this->project_->get_shapes()[0]->get_original_filename().toStdString();
  name = name.substr(0,name.size() - 5);
  QString prefix = project_path + QDir::separator() + QString::fromStdString(name);
  QStringList list;
  int pad = static_cast<int>(std::log10(static_cast<double>(shapes.size())));
  for ( int i = 0; i < shapes.size(); i++ )
  {
      std:: stringstream ss;
      ss << prefix.toStdString() << ".";
      int sz = std::max(i,1);
      int zeros = pad - static_cast<int>(std::log10(static_cast<double>(sz)));
      for (int j = 0; j < zeros; j++)
          ss << "0";
      ss << i << ".wpts";
      QString file(ss.str().c_str());
      std::cout << "Reading points file: " << file.toStdString() << std::endl;
      list << file;
  }
  
  this->progress_->setValue(95);
  QApplication::processEvents();
  this->project_->load_point_files( list );

  this->progress_->setValue(100);
  QApplication::processEvents();
  delete this->progress_;
  emit optimize_complete();
}

//---------------------------------------------------------------------------
bool OptimizeTool::export_xml( QString filename )
{
  std::cerr << "export to " << filename.toStdString() << "\n";

  QFile file( filename );

  if ( !file.open( QIODevice::WriteOnly ) )
  {
    QMessageBox::warning( 0, "Read only", "The file is in read only mode" );
    return false;
  }

  QSharedPointer<QXmlStreamWriter> xml_writer = QSharedPointer<QXmlStreamWriter>( new QXmlStreamWriter() );
  xml_writer->setAutoFormatting( true );
  xml_writer->setDevice( &file );
  xml_writer->writeStartDocument();

  xml_writer->writeTextElement( "number_of_particles",
                                QString::number( this->ui_->number_of_points_->value() ) );

  xml_writer->writeTextElement( "iterations_per_split",
                                QString::number( this->ui_->iterations_per_split_->value() ) );

  xml_writer->writeTextElement( "starting_regularization",
                                QString::number( this->ui_->starting_regularization_->value() ) );

  xml_writer->writeTextElement( "ending_regularization",
                                QString::number( this->ui_->ending_regularization_->value() ) );

  xml_writer->writeTextElement( "optimization_iterations",
                                QString::number( this->ui_->optimization_iterations_->value() ) );

  xml_writer->writeTextElement( "relative_weighting",
                                QString::number( this->ui_->relative_weighting_->value() ) );

  QVector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();
  QFileInfo fi( shapes[0]->get_original_filename_with_path() );
  QString project_path = fi.dir().absolutePath();
  std::string name = this->project_->get_shapes()[0]->get_original_filename().toStdString();
  name = name.substr(0,name.size() - 5);
  xml_writer->writeTextElement( "output_points_prefix",
	  project_path + QDir::separator() + QString::fromStdString(name) );

  // inputs
  xml_writer->writeStartElement( "inputs" );
  xml_writer->writeCharacters( "\n" );

  for ( int i = 0; i < shapes.size(); i++ )
  {
    xml_writer->writeCharacters( shapes[i]->get_groomed_filename_with_path() + "\n" );
  }
  xml_writer->writeEndElement(); // inputs

  xml_writer->writeEndDocument();

  return true;
}

//---------------------------------------------------------------------------
void OptimizeTool::set_project( QSharedPointer<Project> project )
{
  this->project_ = project;
}

//---------------------------------------------------------------------------
void OptimizeTool::set_app( ShapeWorksStudioApp* app )
{
  this->app_ = app;
}
