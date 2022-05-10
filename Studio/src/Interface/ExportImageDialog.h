#pragma once

// qt
#include <QDialog>
#include <QTimer>

// studio
#include <Data/Preferences.h>

class Ui_ExportImageDialog;

namespace shapeworks {

class Visualizer;
class AnalysisTool;

/*!
 * The ExportImageDialog is a QDialog that implements the user interface for exporting images
 */
class ExportImageDialog : public QDialog {
  Q_OBJECT

 public:
  ExportImageDialog(QWidget* parent, Preferences& prefs, QSharedPointer<AnalysisTool> analysis_tool,
                    QSharedPointer<Visualizer> visualizer, bool pca_mode);

 private Q_SLOTS:

  void export_clicked();

 Q_SIGNALS:

 private:
  void update_preview();

  void drawRotatedText(QPainter &painter, QString text, QPointF point, qreal angle, QRect rect);

  Ui_ExportImageDialog* ui_;

  QSharedPointer<Visualizer> visualizer_;

  Preferences& prefs_;

  QPixmap pixmap_;

  bool pca_mode_ = false;

  QTimer update_preview_timer_;

  QSharedPointer<AnalysisTool> analysis_tool_;
};

}  // namespace shapeworks
