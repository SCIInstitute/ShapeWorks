#ifdef _WIN32

#include <Data/StudioLog.h>
#include "StackWalker.h"
#include <tchar.h>
#include <stdio.h>

#define UNHANDLED_EXCEPTION_TEST
#define EXCEPTION_FILTER_TEST

// secure-CRT_functions are only available starting with VC8
#if _MSC_VER < 1400
#define _tcscpy_s _tcscpy
#define _tcscat_s _tcscat
#define _stprintf_s _stprintf
#define strcpy_s(a, b, c) strcpy(a, c)
#endif

// Specialized stackwalker-output classes
// Console (printf):
class StackWalkerToConsole : public StackWalker
{
protected:
  virtual void OnOutput(LPCSTR szText) {
	  STUDIO_LOG_STACK(std::string(szText));
  }
};

void Func5()
{
  StackWalkerToConsole sw;
  sw.ShowCallstack();
}
void Func4()
{
  Func5();
}
void Func3()
{
  Func4();
}
void Func2()
{
  Func3();
}
void Func1()
{
  Func2();
}

void StackWalkTest()
{
  Func1();
}

int init_crash_handler()
{
  STUDIO_LOG_MESSAGE("Showing a simple callstack of the current thread:\n\n\n");
  StackWalkTest();

  return 0;
}

#endif

