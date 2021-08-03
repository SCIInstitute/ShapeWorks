#include "Style.h"

#include <QPushButton>

namespace shapeworks {

const QString normal_button_ss =
  R"(
  QPushButton{
    background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:0.960227, stop:0 rgba(221, 221, 221, 255), stop:0.155779 rgba(238, 238, 238, 255), stop:1 rgba(192, 194, 194, 255));
    border-radius: 4px;
    border: 1px solid rgb(90, 90, 90);
  }

  QPushButton:hover{
    background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(195, 195, 195, 255), stop:0.253769 rgba(206, 206, 206, 255), stop:1 rgba(185, 185, 185, 255));
    border-radius: 4px;
    border: 1px solid rgb(90, 90, 90);
  }

  QPushButton:pressed{
    background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(150, 150, 150, 255), stop:0.753769 rgba(206, 206, 206, 255), stop:1 rgba(185, 185, 185, 255));
    border-radius: 4px;
    border: 1px solid rgb(90, 90, 90);
  }
  )";

const QString abort_button_ss =
  R"(
  QPushButton#run_button{
    background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(98, 0, 0, 255), stop:0.299435 rgba(128, 0, 0, 255), stop:0.491525 rgba(128, 0, 0, 255), stop:1 rgba(98, 0, 0, 255));
    border-radius: 4px;
    border: 1px solid rgb(90, 90, 90);
    color: white;
  }

  QPushButton#run_button:disabled{
    background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:0.960227, stop:0 rgba(128, 128, 128, 255), stop:0.155779 rgba(96, 96, 96, 255), stop:1 rgba(96, 96, 96, 255));
    border-radius: 4px;
    border: 1px solid rgb(90, 90, 90);
  }

  QPushButton#run_button:hover{
    background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(80, 0, 0, 255), stop:0.299435 rgba(110, 0, 0, 255), stop:0.491525 rgba(110, 0, 0, 255), stop:1 rgba(80, 0, 0, 255));
    border-radius: 4px;
    border: 1px solid rgb(90, 90, 90);
    color: white;
  }

  QPushButton#run_button:pressed{
    background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(50, 0, 0, 255), stop:0.299435 rgba(80, 0, 0, 255), stop:0.491525 rgba(80, 0, 0, 255), stop:1 rgba(50, 0, 0, 255));
    border-radius: 4px;
    border: 1px solid rgb(90, 90, 90);
  }
  )";

//---------------------------------------------------------------------------
Style::Style()
{}

//---------------------------------------------------------------------------
void Style::apply_normal_button_style(QPushButton* button)
{
  button->setStyleSheet(normal_button_ss);
}

//---------------------------------------------------------------------------
void Style::apply_abort_button_style(QPushButton *button)
{
  button->setStyleSheet(abort_button_ss);
}
}
