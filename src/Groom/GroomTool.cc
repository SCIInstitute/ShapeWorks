#include <iostream>

#include <Groom/GroomTool.h>

#include <ui_GroomTool.h>

GroomTool::GroomTool()
{

  this->ui = new Ui_GroomTool;
  this->ui->setupUi( this );


  this->ui->toolListWidget->setCurrentRow(0);

}


GroomTool::~GroomTool()
{

}


void GroomTool::on_toolListWidget_currentRowChanged( int row )
{
  std::cerr << "row: " << row << "\n";
  if ( row < 3 )
  {
    this->ui->toolStackedWidget->setCurrentIndex( row );
  }
}

void GroomTool::on_addToolButton_clicked()
{

  QListWidgetItem* selected_tool = this->ui->toolListWidget->currentItem();
  QString tool_string = selected_tool->text();

  if (selected_tool->text() == "Antialias")
  {
    tool_string = tool_string + " (iterations: " + QString::number(this->ui->antialiasIterations->value()) + ")";
  }

  if (selected_tool->text() == "Blur")
  {
    tool_string = tool_string + " (sigma: " + QString::number(this->ui->blurSigma->value()) + ")";
  }



  QListWidgetItem* item = new QListWidgetItem( tool_string );

  this->ui->groomPipelineListWidget->addItem( item );
}

void GroomTool::on_deleteToolButton_clicked()
{
  qDeleteAll(this->ui->groomPipelineListWidget->selectedItems());
}


