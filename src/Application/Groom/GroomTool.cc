#include <iostream>

#include <QXmlStreamWriter>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>

#include <Groom/GroomTool.h>
#include <Visualization/ShapeworksWorker.h>
#include <Data/Project.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

#include <Visualization/ShapeWorksStudioApp.h>

#include <ui_GroomTool.h>

//---------------------------------------------------------------------------
GroomTool::GroomTool(Preferences& prefs,std::vector<std::string>& files) 
  : preferences_(prefs), files_(files) {
  this->progress_ = nullptr;
  this->ui_ = new Ui_GroomTool;
  this->ui_->setupUi( this );
}

//---------------------------------------------------------------------------
GroomTool::~GroomTool()
{}

//---------------------------------------------------------------------------
void GroomTool::on_antialias_checkbox_stateChanged( int state )
{
  this->ui_->antialias_groupbox->setEnabled( state );
}

//---------------------------------------------------------------------------
void GroomTool::on_blur_checkbox_stateChanged( int state )
{
  this->ui_->blur_groupbox->setEnabled( state );
}

//---------------------------------------------------------------------------
void GroomTool::on_export_xml_button_clicked()
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
void GroomTool::handle_error() {
	this->progress_->setValue(100);
  QApplication::processEvents();
	delete this->progress_;
}

//---------------------------------------------------------------------------
void GroomTool::handle_progress(int val) {
	if (val < 90)
		this->progress_->setValue(val);
  QApplication::processEvents();
}

//---------------------------------------------------------------------------
void GroomTool::on_run_groom_button_clicked()
{
  this->progress_ = new QProgressDialog(QString("Running Shapeworks Tool. Please Wait..."),QString(),0,100,this);
  this->progress_->setWindowModality(Qt::WindowModal);
  this->progress_->show();
  QApplication::processEvents();
  this->progress_->setValue(5);
  QApplication::processEvents();
  this->app_->set_status_bar( "Please wait: running groom step..." );
  auto shapes = this->project_->get_shapes();
  std::vector<ImageType::Pointer> imgs;
  std::vector<std::string> names;
  for (auto s : shapes) {
    imgs.push_back(s->get_image());
    names.push_back(s->get_original_filename_with_path().toStdString());
  }
  this->groom_ = ShapeWorksGroom(imgs, names, 0, 1, this->ui_->blur_sigma->value(),
    this->ui_->padding_amount->value(), this->ui_->antialias_iterations->value(), 0, true);

  if ( this->ui_->center_checkbox->isChecked() ) {
    this->groom_.queueTool("center");
  }
  if ( this->ui_->autocrop_checkbox->isChecked() ) {
    this->groom_.queueTool("auto_crop");
  }
  if ( this->ui_->auto_pad_checkbox_->isChecked() ) {
    this->groom_.queueTool("auto_pad");
  }
  if ( this->ui_->antialias_checkbox->isChecked() ) {
    this->groom_.queueTool("antialias");
  }
  if ( this->ui_->fastmarching_checkbox->isChecked() ) {   
    this->groom_.queueTool("fastmarching");
  }
  if ( this->ui_->blur_checkbox->isChecked() ) {
    this->groom_.queueTool("blur");
  }
  if (this->ui_->fill_holes_checkbox->isChecked()) {
    this->groom_.queueTool("hole_fill");
  }
  if (this->ui_->isolate_checkbox->isChecked()) {
    this->groom_.queueTool("isolate");
  }

  QThread *thread = new QThread;
  ShapeworksWorker *worker = new ShapeworksWorker(
    ShapeworksWorker::Groom, this->groom_, this->project_);
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()),  this, SLOT(handle_thread_complete()));
  connect(worker, SIGNAL(step_made(int)),  this, SLOT(handle_progress(int)));
  connect(worker, SIGNAL(run_error()),  this, SLOT(handle_error()));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  thread->start();
  // now lets hope this thread does it's job.
}

//---------------------------------------------------------------------------
void GroomTool::handle_thread_complete() {
  if (this->progress_) {
    this->progress_->setValue(95);
    QApplication::processEvents();
  }
  this->project_->load_groomed_files(this->groom_.getGroomFileNames());
  if (this->progress_) {
    this->progress_->setValue(100);
    QApplication::processEvents();
    delete this->progress_;
    this->progress_ = nullptr;
  }
  emit groom_complete();
}

//---------------------------------------------------------------------------
bool GroomTool::export_xml( QString filename )
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

  // background
  xml_writer->writeComment( "Value of background pixels in image" );
  xml_writer->writeTextElement( "background", "0.0" );

  // foreground
  xml_writer->writeComment( "Value of foreground pixels in image" );
  xml_writer->writeTextElement( "foreground", "1.0" );

  // pad
  /// TODO: hook up a UI element to control
  xml_writer->writeTextElement( "pad", QString::number( this->ui_->padding_amount->value() ) );

  QVector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();

  QFileInfo fi( shapes[0]->get_original_filename_with_path() );
  QString project_path = fi.dir().absolutePath();

  // output transform
  xml_writer->writeTextElement( "transform_file", project_path + QDir::separator() + "studio.transform" );

  if ( this->ui_->antialias_checkbox->isChecked() )
  {
    xml_writer->writeComment( "Number of anti-aliasing iterations" );
    xml_writer->writeStartElement( "antialias_iterations" );
    xml_writer->writeCharacters( QString::number( this->ui_->antialias_iterations->value() ) );
    xml_writer->writeEndElement();
  }

  if ( this->ui_->blur_checkbox->isChecked() )
  {
    xml_writer->writeComment( "Size of Gaussian blurring kernel for smoothing" );
    xml_writer->writeStartElement( "blur_sigma" );
    xml_writer->writeCharacters( QString::number( this->ui_->blur_sigma->value() ) );
    xml_writer->writeEndElement();
  }

  // inputs
  xml_writer->writeStartElement( "inputs" );
  xml_writer->writeCharacters( "\n" );

  for ( int i = 0; i < shapes.size(); i++ )
  {
    xml_writer->writeCharacters( shapes[i]->get_original_filename_with_path() + "\n" );
  }
  xml_writer->writeEndElement();

  // outputs
  xml_writer->writeStartElement( "outputs" );
  xml_writer->writeCharacters( "\n" );
  shapes = this->project_->get_shapes();

  for ( int i = 0; i < shapes.size(); i++ )
  {
    QString path = shapes[i]->get_original_filename_with_path();

    QFileInfo fi( path );
    QString outfile = fi.dir().absolutePath() + QDir::separator() + fi.completeBaseName() + "_DT.nrrd";

    xml_writer->writeCharacters( outfile + "\n" );
  }
  xml_writer->writeEndElement();

  xml_writer->writeEndDocument();

  return true;
}

//---------------------------------------------------------------------------
void GroomTool::set_project( QSharedPointer<Project> project )
{
  this->project_ = project;
}

//---------------------------------------------------------------------------
void GroomTool::set_app( ShapeWorksStudioApp* app )
{
  this->app_ = app;
}

void GroomTool::on_skipButton_clicked() {
  auto shapes = this->project_->get_shapes();
  std::vector<ImageType::Pointer> imgs;
  std::vector<std::string> names;
  for (auto s : shapes) {
    imgs.push_back(s->get_image());
    names.push_back(s->get_original_filename_with_path().toStdString());
  }
  this->groom_ = ShapeWorksGroom(imgs, names, 0, 1, 0, 0, 0, 0, true);
  QThread *thread = new QThread;
  ShapeworksWorker *worker = new ShapeworksWorker(
    ShapeworksWorker::Groom, this->groom_, this->project_);
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_thread_complete()));
  connect(worker, SIGNAL(step_made(int)), this, SLOT(handle_progress(int)));
  connect(worker, SIGNAL(run_error()), this, SLOT(handle_error()));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  thread->start();
  // now lets hope this thread does it's job.
}