/*
 * Shapeworks license
 */

#include <Application/ShapeworksWorker.h>
#include <QProcess>
#include <QMessageBox>
#include <sstream>
#include <fstream>

ShapeworksWorker::ShapeworksWorker(QString exe, QStringList args) : exe_(exe), args_(args){}

ShapeworksWorker::~ShapeworksWorker() {}

void ShapeworksWorker::process()
{
  QProcess* groom = new QProcess( this );
  groom->setProcessChannelMode( QProcess::MergedChannels );
  groom->start( this->exe_, this->args_ );
  if ( !groom->waitForStarted() )
  {
    QMessageBox::critical( 0, "Error", "Failed to start ShapeWorksTool" );
    delete groom;
	emit run_error();
    return;
  }
  std::stringstream output;
  output << "***************************Tool Command*********************************\n";
  output << this->exe_.toStdString();
  for (int i = 0; i < this->args_.size(); i++) {
	  output << " " << this->args_.at(i).toStdString();
  }
  output << std::endl << std::endl;
  output << "***************************Tool Output*********************************\n";
  emit step_made(8);
  int val = 10;
  while ( !groom->waitForFinished( 1000 ) ) {
	  
    QByteArray result = groom->readAll();
    output << "output: " << result.data() << "\n";

    QString strOut = groom->readAllStandardOutput();
    output << strOut.toStdString() << "\n";

    strOut = groom->readAllStandardError();
    output << strOut.toStdString() << "\n";
    emit step_made((val+=2));
  }
  
  QByteArray result = groom->readAll();
  output << "output: " << result.data() << "\n";

  QString strOut = groom->readAllStandardOutput();
  output << strOut.toStdString() << "\n";

  strOut = groom->readAllStandardError();
  output << strOut.toStdString() << "\n";

  output << "Finished running!\n";

  delete groom;

  //write output to file.
  std::ofstream out("ShapeWorksStudioTools.log", std::ios::app);
  out << output.str() << std::endl;
  out << "***************************Tool Complete*********************************\n\n";
  out.close();
  emit result_ready();
}