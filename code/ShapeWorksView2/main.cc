#include <QApplication>
#include <ShapeWorksView2.h>

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

  try {

#ifdef WIN32
    ::SetErrorMode( 0 );
#endif

    if ( argc < 2 )
    {
      std::cerr << "Usage: " << argv[0] << " parameterfile.xml [--pca pca.csv]" << std::endl;
      return 1;
    }

    if ( argc == 4 )
    {
      QString opt = argv[2];
      QString file = argv[3];
      if ( opt == "--pca" )
      {
        std::cout << "Writing PCA values to: " << file.toStdString() << "\n";

        ParticleShapeStatistics<3> stats;
        stats.ReadPointFiles( argv[1] );
        stats.ComputeModes();
        stats.PrincipalComponentProjections();
        stats.WriteCSVFile2( file.toStdString() );
        return 0;
      }
    }

    
    QApplication app( argc, argv );
    ShapeWorksView2 shapeWorksView2( argc, argv );
    shapeWorksView2.show();
    return app.exec();
  }
  catch ( itk::ExceptionObject & excep )
  {
    std::cerr << excep << std::endl;
  }
  catch ( std::exception e )
  {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << e.what() << "\n";
  }
}
