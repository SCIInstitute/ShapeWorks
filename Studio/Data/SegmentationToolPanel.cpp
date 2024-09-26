// qt
#include <QMessageBox>
#include <QThread>

// shapeworks
#include <Data/Session.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/Worker.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Interface/Style.h>
#include <Job/GroupPvalueJob.h>
#include <Job/ParticleNormalEvaluationJob.h>
#include <Job/StatsGroupLDAJob.h>
#include <Logging.h>
#include <QMeshWarper.h>
#include <Shape.h>
#include <StudioMesh.h>
#include <Utils/AnalysisUtils.h>
#include <jkqtplotter/graphs/jkqtpboxplot.h>
#include <jkqtplotter/graphs/jkqtpstatisticsadaptors.h>
#include <jkqtplotter/jkqtplotter.h>
#include <ui_SegmentationToolPanel.h>

#include "SegmentationToolPanel.h"

namespace shapeworks {

//---------------------------------------------------------------------------
SegmentationToolPanel::SegmentationToolPanel(QWidget* parent) : QWidget(parent), ui_(new Ui_SegmentationToolPanel) {
  ui_->setupUi(this);

  connect(ui_->open_button, &QPushButton::toggled, ui_->content, &QWidget::setVisible);
  connect(ui_->header, &QPushButton::clicked, ui_->open_button, &QPushButton::toggle);

  connect(ui_->erase_mode_, &QRadioButton::toggled, this, &SegmentationToolPanel::update_paint_value);
  connect(ui_->segmentation_mode_, &QRadioButton::toggled, this, &SegmentationToolPanel::update_paint_value);
  connect(ui_->included_mode_, &QRadioButton::toggled, this, &SegmentationToolPanel::update_paint_value);
  connect(ui_->excluded_mode_, &QRadioButton::toggled, this, &SegmentationToolPanel::update_paint_value);

  connect(ui_->recompute_surface_, &QPushButton::clicked, this, &SegmentationToolPanel::recompute_surface);
  ui_->header_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->open_button->setChecked(false);
}

//---------------------------------------------------------------------------
SegmentationToolPanel::~SegmentationToolPanel() {}

//---------------------------------------------------------------------------
void SegmentationToolPanel::set_session(QSharedPointer<Session> session) {
  session_ = session;

  connect(ui_->painting_active_, &QCheckBox::toggled, session.data(), &Session::set_seg_paint_active);
  connect(ui_->brush_size_, &CustomSlider::valueChanged, session.data(), &Session::set_seg_paint_size);

  reset();
}

//---------------------------------------------------------------------------
void SegmentationToolPanel::reset() {}

//---------------------------------------------------------------------------
void SegmentationToolPanel::update_paint_value() {
  int paint_value = 0;
  if (ui_->segmentation_mode_->isChecked()) {
    paint_value = 1;
  } else if (ui_->included_mode_->isChecked()) {
    paint_value = 2;
  } else if (ui_->excluded_mode_->isChecked()) {
    paint_value = 3;
  }
  session_->set_seg_paint_value(paint_value);
}

//---------------------------------------------------------------------------
void SegmentationToolPanel::recompute_surface() { session_->recompute_surfaces(); }

}  // namespace shapeworks
