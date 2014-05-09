#include <iostream>

#include <QXmlStreamWriter>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>

#include <Groom/GroomTool.h>
#include <Data/DataManager.h>

#include <ui_GroomTool.h>

//---------------------------------------------------------------------------
GroomTool::GroomTool()
{

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
void GroomTool::on_run_groom_button_clicked()
{

  QTemporaryFile file;
  file.open();

  QString temp_file_name = file.fileName() + ".xml";

  this->export_xml( temp_file_name );
  file.close();

  QStringList args;

  args << temp_file_name;

  if ( this->ui_->center_checkbox->isChecked() )
  {
    args << "center";
  }
  if ( this->ui_->antialias_checkbox->isChecked() )
  {
    args << "antialias";
  }

  QProcess* groom = new QProcess( this );
  groom->setProcessChannelMode( QProcess::MergedChannels );
  groom->start("C:/Users/amorris/carma/shapeworks/bin/ShapeWorksGroom/Release/ShapeWorksGroom.exe", args);
  //groom->start( "C:/Users/amorris/carma/shapeworks/build-x86/ShapeWorksGroom/Release/ShapeWorksGroom.exe", args );
  if ( !groom->waitForStarted() )
  {
    std::cerr << "Error: failed to start ShapeWorksGroom\n";
    QMessageBox::critical( 0, "Error", "Failed to start ShapeWorksGroom" );
    return;
  }


  //groom.closeWriteChannel();

  std::cerr << "running...";

  if ( !groom->waitForFinished() )
  {
    std::cerr << "Error running ShapeWorksGroom\n";
    QMessageBox::critical( 0, "Error", "Error running ShapeWorksGroom" );

    return;
  }

  QByteArray result = groom->readAll();
  std::cerr << "output: " << result.data() << "\n";

  QString strOut = groom->readAllStandardOutput();
  std::cerr << strOut.toStdString() << "\n";

  strOut = groom->readAllStandardError();
  std::cerr << strOut.toStdString() << "\n";

  std::cerr << "Finished running!\n";

  delete groom;
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
  xml_writer->writeStartElement( "background" );
  xml_writer->writeCharacters( "0.0" );
  xml_writer->writeEndElement();

  // foreground
  xml_writer->writeComment( "Value of foreground pixels in image" );
  xml_writer->writeStartElement( "foreground" );
  xml_writer->writeCharacters( "1.0" );
  xml_writer->writeEndElement();

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
  std::vector<QSharedPointer<Shape> > shapes = this->data_manager_->get_shapes();

  for ( int i = 0; i < shapes.size(); i++ )
  {
    QSharedPointer<Mesh> initial_mesh = shapes[i]->get_initial_mesh();

    xml_writer->writeCharacters( initial_mesh->get_filename_with_path() + "\n" );
  }
  xml_writer->writeEndElement();

  // outputs
  xml_writer->writeStartElement( "outputs" );
  xml_writer->writeCharacters( "\n" );
  shapes = this->data_manager_->get_shapes();

  for ( int i = 0; i < shapes.size(); i++ )
  {
    QSharedPointer<Mesh> initial_mesh = shapes[i]->get_initial_mesh();

    QString path = initial_mesh->get_filename_with_path();

    QFileInfo fi( path );
    QString outfile = fi.dir().absolutePath() + QDir::separator() + fi.completeBaseName() + "_DT.nrrd";

    xml_writer->writeCharacters( outfile + "\n" );
  }
  xml_writer->writeEndElement();

  xml_writer->writeEndDocument();

  return true;
}

//---------------------------------------------------------------------------
void GroomTool::set_data_manager( QSharedPointer<DataManager> data_manager )
{
  this->data_manager_ = data_manager;
}
