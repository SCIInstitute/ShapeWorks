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
PreferencesWindow::PreferencesWindow( QWidget* parent /*= 0 */ )
{
  this->ui_ = new Ui_PreferencesWindow;
  this->ui_->setupUi( this );

  this->ui_->num_threads->setMaximum( QThread::idealThreadCount() );
  this->ui_->num_threads_max_label->setText( QString::number( QThread::idealThreadCount() ) );

  QPushButton* reset_button = this->ui_->button_box->button( QDialogButtonBox::RestoreDefaults );
  QObject::connect( reset_button, SIGNAL( clicked() ), this, SLOT( restore_defaults() ) );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::update_labels()
{
  this->ui_->glyphQualityLabel->setText( QString::number( Preferences::Instance().get_glyph_quality() ) );
  this->ui_->glyphSizeLabel->setText( QString::number( Preferences::Instance().get_glyph_size() ) );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_enabled_stateChanged( int state )
{
  Preferences::Instance().set_cache_enabled( this->ui_->mesh_cache_enabled->isChecked() );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_mesh_cache_memory_valueChanged( int value )
{
  Preferences::Instance().setCacheMemory( value );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_color_scheme_currentIndexChanged( int index )
{
  Preferences::Instance().set_color_scheme( index );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_glyph_size_valueChanged( int value )
{
  Preferences::Instance().set_glyph_size( value / 10.0 );
  this->update_labels();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_glyph_quality_valueChanged( int value )
{
  Preferences::Instance().set_glyph_quality( value );
  this->update_labels();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_num_threads_valueChanged( int value )
{
  Preferences::Instance().set_num_threads( value );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_parallel_enabled_stateChanged( int state )
{
  Preferences::Instance().set_parallel_enabled( this->ui_->parallel_enabled->isChecked() );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_range_valueChanged( double value )
{
  Preferences::Instance().set_pca_range( this->ui_->pca_range->value() );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_pca_steps_valueChanged( int value )
{
  Preferences::Instance().set_num_pca_steps( this->ui_->pca_steps->value() );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_regression_steps_valueChanged( int value )
{
  Preferences::Instance().set_num_regression_steps( this->ui_->regression_steps->value() );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::restore_defaults()
{
  Preferences::Instance().restore_defaults();
  this->set_values_from_preferences();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::set_values_from_preferences()
{
  this->ui_->groom_location->setText( Preferences::Instance().get_groom_location() );
  this->ui_->optimize_location->setText( Preferences::Instance().get_optimize_location() );

  this->ui_->mesh_cache_enabled->setChecked( Preferences::Instance().get_cache_enabled() );
  this->ui_->mesh_cache_memory->setValue( Preferences::Instance().get_cache_memory() );
  this->ui_->color_scheme->setCurrentIndex( Preferences::Instance().get_color_scheme() );

  this->ui_->glyph_quality->setValue( Preferences::Instance().get_glyph_quality() );
  this->ui_->glyph_size->setValue( Preferences::Instance().get_glyph_size() * 10 );

  this->ui_->num_threads->setValue( Preferences::Instance().get_num_threads() );
  this->ui_->parallel_enabled->setChecked( Preferences::Instance().get_parallel_enabled() );

  this->ui_->pca_range->setValue( Preferences::Instance().get_pca_range() );
  this->ui_->pca_steps->setValue( Preferences::Instance().get_num_pca_steps() );
  this->ui_->regression_steps->setValue( Preferences::Instance().get_num_regression_rteps() );

  this->update_labels();
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_groom_location_textChanged( const QString& text )
{
  Preferences::Instance().set_groom_location( text );
}

//-----------------------------------------------------------------------------
void PreferencesWindow::on_optimize_location_textChanged( const QString& text )
{
  Preferences::Instance().set_optimize_location( text );
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
