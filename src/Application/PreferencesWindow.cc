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

#ifndef POWERCRUST
  this->ui_->power_crust->setVisible(false);
  this->ui_->power_crust->setChecked(false);
  this->preferences_->set_use_powercrust(false);
  this->ui_->neighborhoodSpinBox_2->setEnabled(true);
  this->ui_->neighborhoodSpinBox_2->setEnabled(true);
#endif
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
  emit update_view();
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
  this->ui_->power_crust->setChecked( preferences_.get_use_powercrust() );
  this->ui_->smoothingSlider->setValue( preferences_.get_smoothing_amount() );
  this->ui_->neighborhoodSpinBox->setValue( preferences_.get_neighborhood() );
  this->ui_->spacingSpinBox->setValue( preferences_.get_spacing() );

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
	  this->ui_->neighborhoodSpinBox->setEnabled(false);
	  this->ui_->spacingSpinBox->setEnabled(false);
  } else {
	  this->ui_->neighborhoodSpinBox->setEnabled(true);
	  this->ui_->spacingSpinBox->setEnabled(true);
  }
  preferences_.set_use_powercrust(powercrust);
  emit clear_cache();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_parallel_enabled_toggled(bool b) {
  preferences_.set_parallel_enabled(b);
  this->ui_->num_threads->setEnabled(b);
}
//-----------------------------------------------------------------------------
void PreferencesWindow::on_num_threads_valueChanged(int i) {
  preferences_.set_num_threads(i);
}
//-----------------------------------------------------------------------------
void PreferencesWindow::on_caching_epsilon_valueChanged(int i) {
	preferences_.set_cache_epsilon(std::powf(10.f,static_cast<float>(i)));
    emit clear_cache();
}
  
void PreferencesWindow::on_smoothingSlider_valueChanged(int i) {
	preferences_.set_smoothing_amount(i);
    emit clear_cache();
}
void PreferencesWindow::on_spacingSpinBox_valueChanged(double d) {
	preferences_.set_spacing(static_cast<float>(d));
    emit clear_cache();
}
void PreferencesWindow::on_neighborhoodSpinBox_valueChanged(int i) {
	preferences_.set_neighborhood(i);
    emit clear_cache();
}