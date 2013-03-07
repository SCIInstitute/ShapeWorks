#ifndef PREFERENCES_H
#define PREFERENCES_H


#include <QSettings>
#include <PreferencesWindow.h>

class Preferences
{
public:

  static Preferences& Instance();

  QSettings& getSettings();

  void showWindow();

private:

  // private constructor
  Preferences();
  // Stop the compiler generating methods of copy the object
  Preferences( Preferences const& copy );            // Not Implemented
  Preferences& operator=( Preferences const& copy ); // Not Implemented
  static Preferences instance;

  QSettings settings;

  PreferencesWindow preferencesWindow;
};

#endif