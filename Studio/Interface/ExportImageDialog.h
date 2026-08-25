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

  //! Render the composed image; reduced size for the preview, full requested size for the export
  QPixmap render_image(QSize size, bool& all_ready);

  //! The size the export will actually be rendered at
  QSize get_export_size();

  //! Set the export size to a multiple of the size the view is currently rendered at
  void apply_size_preset(int multiplier);

  //! Show the progress widget and paint it before a render blocks the GUI thread; maximum 0 is busy
  void begin_progress(int maximum, QString message);

  //! Advance the progress bar and repaint between renders
  void step_progress(int value);

  void end_progress();

  void drawRotatedText(QPainter& painter, QString text, QPointF point, qreal angle, QRect rect);

  QVector<int> get_modes(QString string);

  Ui_ExportImageDialog* ui_;
  QSharedPointer<Visualizer> visualizer_;
  Preferences& prefs_;
  QPixmap pixmap_;
  bool pca_mode_ = false;
  QTimer update_preview_timer_;
  QSharedPointer<AnalysisTool> analysis_tool_;
  //! guards update_preview() against re-entering itself via the repaint it processes events for
  bool updating_preview_ = false;
};

}  // namespace shapeworks
