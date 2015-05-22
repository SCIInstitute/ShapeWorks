/*
 * Shapeworks license
 */

// qt
#include <QtGui>

#include <Application/PreferencesWindow.h>
#include <Application/Preferences.h>

// The interface from the designer
#include "ui_PreferencesWindow.h"

//-----------------------------------------------------------------------------
PreferencesWindow::PreferencesWindow( QWidget* parent, Preferences& prefs ) : preferences_(prefs)
{
  this->ui_ = new Ui_PreferencesWindow;
  this->ui_->setupUi( this );

  this->ui_->num_threads->setMaximum( QThread::idealThreadCount() );
  this->ui_->num_threads_max_label->setText( QString::number( QThread::idealThreadCount() ) );

  QPushButton* reset_button = this->ui_->button_box->button( QDialogButtonBox::RestoreDefaults );
  QObject::connect( reset_button, SIGNAL( clicked() ), this, SLOT( restore_defaults() ) );
}

//-----------------------------------------------------------------------------
bool PreferencesWindow::get_use_powercrust() {
	return this->ui_->power_crust->isChecked();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::update_labels()
{
  this->ui_->glyphQualityLabel->setText( QString::number( preferences_.get_glyph_quality() ) );
  this->ui_->glyphSizeLabel->setText( QString::number( preferences_.get_glyph_size() ) );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_enabled_stateChanged( int state )
{
  preferences_.set_cache_enabled( this->ui_->mesh_cache_enabled->isChecked() );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_memory_valueChanged( int value )
{
  preferences_.setCacheMemory( value );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_color_scheme_currentIndexChanged( int index )
{
  preferences_.set_color_scheme( index );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_glyph_size_valueChanged( int value )
{
  preferences_.set_glyph_size( value / 10.0 );
  this->update_labels();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_glyph_quality_valueChanged( int value )
{
  preferences_.set_glyph_quality( value );
  this->update_labels();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_num_threads_valueChanged( int value )
{
  preferences_.set_num_threads( value );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_parallel_enabled_stateChanged( int state )
{
  preferences_.set_parallel_enabled( this->ui_->parallel_enabled->isChecked() );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_range_valueChanged( double value )
{
  preferences_.set_pca_range( this->ui_->pca_range->value() );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_steps_valueChanged( int value )
{
  preferences_.set_num_pca_steps( this->ui_->pca_steps->value() );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_regression_steps_valueChanged( int value )
{
  preferences_.set_num_regression_steps( this->ui_->regression_steps->value() );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::restore_defaults()
{
  preferences_.restore_defaults();
  this->set_values_from_preferences();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::set_values_from_preferences()
{
  this->ui_->groom_location->setText( preferences_.get_groom_location() );
  this->ui_->optimize_location->setText( preferences_.get_optimize_location() );

  this->ui_->mesh_cache_enabled->setChecked( preferences_.get_cache_enabled() );
  this->ui_->mesh_cache_memory->setValue( preferences_.get_cache_memory() );
  this->ui_->color_scheme->setCurrentIndex( preferences_.get_color_scheme() );

  this->ui_->glyph_quality->setValue( preferences_.get_glyph_quality() );
  this->ui_->glyph_size->setValue( preferences_.get_glyph_size() * 10 );

  this->ui_->num_threads->setValue( preferences_.get_num_threads() );
  this->ui_->parallel_enabled->setChecked( preferences_.get_parallel_enabled() );

  this->ui_->pca_range->setValue( preferences_.get_pca_range() );
  this->ui_->pca_steps->setValue( preferences_.get_num_pca_steps() );
  this->ui_->regression_steps->setValue( preferences_.get_num_regression_rteps() );

  this->update_labels();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_groom_location_textChanged( const QString& text )
{
  preferences_.set_groom_location( text );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_optimize_location_textChanged( const QString& text )
{
  preferences_.set_optimize_location( text );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_locate_groom_button_clicked()
{
  QString filename = QFileDialog::getOpenFileName( this, tr( "Locate ShapeWorksGroom..." ),
                                                   QString(), tr( "File (*)" ) );
  if ( filename.isEmpty() )
  {
    return;
  }

  this->ui_->groom_location->setText( filename );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_locate_optimize_button_clicked()
{
  QString filename = QFileDialog::getOpenFileName( this, tr( "Locate ShapeWorksGroom..." ),
                                                   QString(), tr( "File (*)" ) );
  if ( filename.isEmpty() )
  {
    return;
  }

  this->ui_->optimize_location->setText( filename );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_power_crust_toggled(bool powercrust)
{
  if (powercrust) {
	  this->ui_->neighborhoodSpinBox_2->setEnabled(false);
	  this->ui_->spacingSpinBox_2->setEnabled(false);
  } else {
	  this->ui_->neighborhoodSpinBox_2->setEnabled(true);
	  this->ui_->spacingSpinBox_2->setEnabled(true);
  }
  preferences_.set_use_powercrust(powercrust);
  emit clear_cache();
}
