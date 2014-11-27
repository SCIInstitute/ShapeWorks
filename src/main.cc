#include <QApplication>
#include <Application/ShapeWorksStudioApp.h>
#include <iostream>

#ifdef _WIN32

#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

#ifndef _USE_OLD_IOSTREAMS
using namespace std;
#endif

// maximum number of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

void RedirectIOToConsole2()
{
  int hConHandle;
  long lStdHandle;

  CONSOLE_SCREEN_BUFFER_INFO coninfo;
  FILE* fp;

  // allocate a console for this app
  AllocConsole();

  // set the screen buffer to be big enough to let us scroll text
  GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &coninfo );
  //coninfo.dwSize.Y = MAX_CONSOLE_LINES;

  coninfo.dwSize.X = 10000;
  coninfo.dwSize.Y = 10000;
  SetConsoleScreenBufferSize( GetStdHandle( STD_OUTPUT_HANDLE ), coninfo.dwSize );

  SMALL_RECT windowSize = {0, 0, 79, 49};

  // Change the console window size:
  SetConsoleWindowInfo( GetStdHandle( STD_OUTPUT_HANDLE ), TRUE, &windowSize );

  //  SetConsoleWindowInfo()

  // redirect unbuffered STDOUT to the console
  lStdHandle = (long)GetStdHandle( STD_OUTPUT_HANDLE );
  hConHandle = _open_osfhandle( lStdHandle, _O_TEXT );
  fp = _fdopen( hConHandle, "w" );
  *stdout = *fp;
  setvbuf( stdout, NULL, _IONBF, 0 );

  // redirect unbuffered STDIN to the console
  lStdHandle = (long)GetStdHandle( STD_INPUT_HANDLE );
  hConHandle = _open_osfhandle( lStdHandle, _O_TEXT );
  fp = _fdopen( hConHandle, "r" );
  *stdin = *fp;
  setvbuf( stdin, NULL, _IONBF, 0 );

  // redirect unbuffered STDERR to the console
  lStdHandle = (long)GetStdHandle( STD_ERROR_HANDLE );
  hConHandle = _open_osfhandle( lStdHandle, _O_TEXT );
  fp = _fdopen( hConHandle, "w" );
  *stderr = *fp;
  setvbuf( stderr, NULL, _IONBF, 0 );

  // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
  // point to console as well
  ios::sync_with_stdio( true );
}

#endif // _WIN32

int main( int argc, char** argv )
{

#ifdef WIN32
  ::SetErrorMode( 0 );
  RedirectIOToConsole2();
#endif

  std::cerr << "ShapeWorksStudio initializing...\n";

  QApplication app( argc, argv );

  QSharedPointer<ShapeWorksStudioApp> studio_app =
    QSharedPointer<ShapeWorksStudioApp>( new ShapeWorksStudioApp( argc, argv ) );

  studio_app->show();

  // do this after "show" for mac initialization
  studio_app->initialize_vtk();

  QStringList files;

/*
   files << "z:/shared/laatee/laa_0_DT.nrrd";
   files << "z:/shared/laatee/laa_1_DT.nrrd";
   files << "z:/shared/laatee/laa_2_DT.nrrd";
   files << "z:/shared/laatee/laa_3_DT.nrrd";
   files << "z:/shared/laatee/laa_4_DT.nrrd";
 */

/*
   files << "h:/projects/laa_tee/groomed/interface_0_DT.nrrd";
   files << "h:/projects/laa_tee/groomed/interface_1_DT.nrrd";
   files << "h:/projects/laa_tee/groomed/interface_2_DT.nrrd";
 */
  files << "h:/projects/laa_tee/data/interface_0.nrrd";
  files << "h:/projects/laa_tee/data/interface_1.nrrd";
  files << "h:/projects/laa_tee/data/interface_2.nrrd";
  files << "h:/projects/laa_tee/data/interface_3.nrrd";
  files << "h:/projects/laa_tee/data/interface_4.nrrd";
  files << "h:/projects/laa_tee/data/interface_5.nrrd";

  //studio_app.import_files( files );
#ifdef LOAD_DEFAULT_PROJECT
  studio_app->open_project( "h:/projects/studio/studio.xml" );
#endif

  //  studio_app->open_project( "c:/Users/amorris/Desktop/studio.xml" );
  //studio_app->import_legacy( "c:/Users/amorris/Desktop/groom.xml" );
  //studio_app->import_legacy( "c:/Users/amorris/Desktop/optimize.xml" );
  studio_app->import_legacy( "c:/Users/amorris/Desktop/analysis.xml" );





  return app.exec();
}
