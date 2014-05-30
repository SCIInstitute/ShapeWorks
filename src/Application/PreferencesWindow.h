#ifndef STUDIO_APPLICATION_PREFERENCES_WINDOW_H
#define STUDIO_APPLICATION_PREFERENCES_WINDOW_H

#include <QDialog>

class Ui_PreferencesWindow;
class QAbstractButton;

//! Qt UI dialog to control preferences
/*!
 * The PreferenceWindow provides controls over preferences for the
 * application.  It is synchronized with the singleton Preference object
 */
class PreferencesWindow : public QDialog
{
  Q_OBJECT

public:
  PreferencesWindow( QWidget* parent = 0 );

  void set_values_from_preferences();

public Q_SLOTS:

  void on_groom_location_textChanged( const QString& text );
  void on_optimize_location_textChanged( const QString& text );
  void on_locate_groom_button_clicked();
  void on_locate_optimize_button_clicked();

  void on_mesh_cache_enabled_stateChanged( int state );
  void on_mesh_cache_memory_valueChanged( int value );
  void on_color_scheme_currentIndexChanged( int index );
  void on_glyph_size_valueChanged( int value );
  void on_glyph_quality_valueChanged( int value );
  void on_num_threads_valueChanged( int value );
  void on_parallel_enabled_stateChanged( int state );

  void on_pca_range_valueChanged( double value );
  void on_pca_steps_valueChanged( int value );
  void on_regression_steps_valueChanged( int value );

  void restore_defaults();

private:
  void update_labels();
  Ui_PreferencesWindow* ui_;
};

#endif // STUDIO_APPLICATION_PREFERENCES_WINDOW_H
