#include <Data/Session.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/Worker.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Interface/Style.h>
#include <Logging.h>
#include <Python/PythonWorker.h>
#include <Shape.h>
#include <ShapeWorksMONAI/MonaiLabelJob.h>
#include <ShapeWorksMONAI/MonaiLabelTool.h>
#include <ShapeWorksMONAI/MonaiLabelUtils.h>
#include <ui_MonaiLabelTool.h>

#include <QComboBox>
#include <QIcon>
#include <QRegularExpression>
#include <QTextEdit>
#include <QThread>
#include <QValidator>

using namespace shapeworks;
namespace monailabel {

const std::string MonaiLabelTool::MONAI_MODE_SEGMENTATION("segmentation");
const std::string MonaiLabelTool::MONAI_MODE_DEEPGROW("deepgrow");
const std::string MonaiLabelTool::MONAI_MODE_DEEPEDIT("deepedit");
const std::string MonaiLabelTool::MONAI_SAMPLE_STRATEGY_RANDOM("random");

//---------------------------------------------------------------------------
MonaiLabelTool::MonaiLabelTool(Preferences& prefs) : preferences_(prefs) {
  ui_ = new Ui_MonaiLabelTool;
  ui_->setupUi(this);
  tool_is_running_ = false;
  client_id_ = "user-xyz";
  ui_->serverAddressField->setToolTip("Enter the MONAI Label server address.");
  ui_->connectServerButton->setToolTip("Connect or Disconnect to the server.");
  ui_->monaiModelTypeCombo->setToolTip("Select the model type for AI-assisted segmentation.");
  ui_->monaiModelNameCombo->setToolTip("Specify the model name used during server initialization.");
  ui_->currentSampleSpinBox->setToolTip("Select the index of the source volume to be processed.");
  ui_->uploadSampleButton->setToolTip("Upload the selected volume to the server.");
  ui_->runSegmentationButton->setToolTip("Run inference using the selected model.");
  ui_->submitLabelButton->setToolTip("Upload the predicted or edited segmentation label to the server.");

  connect(ui_->monai_label_open_button, &QPushButton::toggled, ui_->monai_panel_content, &QWidget::setVisible);
  connect(ui_->monai_label_header, &QPushButton::clicked, ui_->monai_label_open_button, &QPushButton::toggle);

  connect(ui_->connectServerButton, &QPushButton::clicked, this, &MonaiLabelTool::onConnectServer);
  connect(ui_->currentSampleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(handleSampleNumberChanged()));

  UrlValidator* validator = new UrlValidator(this);
  ui_->serverAddressField->setValidator(validator);
  QObject::connect(ui_->serverAddressField, &QLineEdit::editingFinished, [=]() {
    QString text = ui_->serverAddressField->text();
    int pos = 0;
    if (validator->validate(text, pos) != QValidator::Acceptable) {
      SW_WARN("Invalid Server Address", "Please enter a valid server address.");
    }
  });
  ui_->serverAddressField->setReadOnly(false);
  connect(ui_->serverAddressField, &QLineEdit::textChanged, this, &MonaiLabelTool::onServerAddressChanged);
  connect(ui_->monaiModelTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &MonaiLabelTool::onModelTypeChanged);
}

//---------------------------------------------------------------------------
MonaiLabelTool::~MonaiLabelTool() {}

//---------------------------------------------------------------------------
void MonaiLabelTool::triggerUpdateView() { Q_EMIT update_view(); }

//---------------------------------------------------------------------------
void MonaiLabelTool::set_connect_button() {
  Style::apply_normal_button_style(ui_->connectServerButton);
  ui_->connectServerButton->setText("Connect Server");
  ui_->connectServerButton->setIcon(QIcon(":/Studio/Images/connect.png"));
  ui_->connectServerButton->setEnabled(true);
  ui_->serverAddressField->setEnabled(true);
}

//---------------------------------------------------------------------------
void MonaiLabelTool::onConnectServer() {
  if (tool_is_running_ && monai_label_job_) {
    ui_->connectServerButton->setText("Disconnecting...");
    monai_label_job_->abort();
    shutdown();
    SW_STATUS("Server disconnected successfully.");
    set_connect_button();
    enable_actions();
    session_->get_project()->save();
    monai_label_job_ = nullptr;
    tool_is_running_ = false;
    return;
  }
  if (session_->get_filename() == "") {
    SW_ERROR(
        "Load source volumes as feature images and save project before "
        "establishing connection with MONAI Label server");
    return;
  }
  SW_LOG("⏳ Connecting to MONAI Label Server...")
  Q_EMIT progress(-1);
  ui_->connectServerButton->setText("Connecting...");
  ui_->connectServerButton->setEnabled(false);
  loadParamsFromUi();
  if (model_type_ == MONAI_MODE_SEGMENTATION) {
    runSegmentationTool();
  } else {
    SW_ERROR(
        "The model type is not supported for the currently initialized "
        "server!");
    return;
  }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::enable_actions() {
  if (session_->get_num_shapes() < 1) {
    return;
  }
  ui_->currentSampleSpinBox->setEnabled(true);
  ui_->currentSampleSpinBox->setMaximum(session_->get_num_shapes() - 1);
  ui_->uploadSampleButton->setEnabled(false);
  ui_->runSegmentationButton->setEnabled(false);
  ui_->submitLabelButton->setEnabled(false);
}

//---------------------------------------------------------------------------
void MonaiLabelTool::activate() {
  std::string feature_name = MonaiLabelUtils::getFeatureName(session_);
  if (!feature_name.empty()) {
    session_->set_image_name(feature_name);
  }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handleSampleNumberChanged() {
  SW_LOG("");
  if (monai_label_job_) {
    monai_label_job_->setCurrentSampleNumber(getCurrentSampleNumber());
  }
  if (tool_is_running_) {
    ui_->uploadSampleButton->setEnabled(true);
  } else {
    ui_->uploadSampleButton->setEnabled(false);
  }
  ui_->runSegmentationButton->setEnabled(false);
  ui_->submitLabelButton->setEnabled(false);
  Q_EMIT progress(-1);
  Q_EMIT sampleChanged();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
int MonaiLabelTool::getCurrentSampleNumber() {
  int val = ui_->currentSampleSpinBox->value();
  return val;
}

//---------------------------------------------------------------------------
void MonaiLabelTool::runSegmentationTool() {
  strategy_ = MonaiLabelTool::MONAI_SAMPLE_STRATEGY_RANDOM;
  monai_label_job_ =
      QSharedPointer<MonaiLabelJob>::create(session_, server_address_, client_id_, strategy_, model_type_);
  monai_label_job_->set_quiet_mode(true);
  SW_DEBUG("Monai Label Job initialized!");
  connect(monai_label_job_.data(), &MonaiLabelJob::progress, this, &MonaiLabelTool::handle_progress);
  connect(ui_->uploadSampleButton, &QPushButton::clicked, monai_label_job_.data(),
          &MonaiLabelJob::onUploadSampleClicked);
  connect(ui_->submitLabelButton, &QPushButton::clicked, monai_label_job_.data(), &MonaiLabelJob::onSubmitLabelClicked);
  connect(ui_->runSegmentationButton, &QPushButton::clicked, monai_label_job_.data(),
          &MonaiLabelJob::onRunSegmentationClicked);
  connect(monai_label_job_.data(), &MonaiLabelJob::triggerUpdateView, this, &MonaiLabelTool::triggerUpdateView);

  connect(monai_label_job_.data(), &MonaiLabelJob::triggerClientInitialized, this,
          &MonaiLabelTool::handleClientInitialized);
  connect(monai_label_job_.data(), &MonaiLabelJob::triggerUploadSampleCompleted, this,
          &MonaiLabelTool::handleUploadSampleCompleted);
  connect(monai_label_job_.data(), &MonaiLabelJob::triggerSegmentationCompleted, this,
          &MonaiLabelTool::handleSegmentationCompleted);
  connect(monai_label_job_.data(), &MonaiLabelJob::triggerSubmitLabelCompleted, this,
          &MonaiLabelTool::handleSubmitLabelCompleted);
  app_->get_py_worker()->run_job(monai_label_job_);

  ui_->connectServerButton->setEnabled(false);
  QTimer::singleShot(1000, this, [=]() { ui_->connectServerButton->setEnabled(true); });
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handleClientInitialized(bool success) {
  if (!success) {
    SW_LOG("Connection failed");
    set_connect_button();
    return;
  }
  SW_LOG("✅ Connection successfully established to the server, continue with segmentation!");
  tool_is_running_ = true;
  if (session_->get_shapes().size() > 1) {
    ui_->uploadSampleButton->setEnabled(true);
  } else {
    ui_->uploadSampleButton->setEnabled(false);
  }
  ui_->runSegmentationButton->setEnabled(false);
  ui_->submitLabelButton->setEnabled(false);
  if (monai_label_job_) {
    ui_->monaiModelNameCombo->clear();
    std::vector<std::string> model_names = monai_label_job_->getModelNames(model_type_);
    for (const auto& item : model_names) {
      ui_->monaiModelNameCombo->addItem(QString::fromStdString(item));
    }
  }
  Style::apply_abort_button_style(ui_->connectServerButton);
  ui_->connectServerButton->setText("Disconnect Server");
  ui_->connectServerButton->setIcon(QIcon(":/Studio/Images/disconnect.png"));
  ui_->connectServerButton->setEnabled(true);
  ui_->serverAddressField->setEnabled(false);
  samples_processed_ = 0;
  Q_EMIT progress(100);
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handleUploadSampleCompleted() {
  SW_LOG("✅ Upload complete! Run {} model on the uploaded sample.", model_type_);
  ui_->uploadSampleButton->setEnabled(false);
  ui_->runSegmentationButton->setEnabled(true);
  ui_->submitLabelButton->setEnabled(false);
  Q_EMIT progress(100);
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handleSegmentationCompleted() {
  SW_LOG(
      "Segmentation for the current sample done! Submit the prediction label to server or "
      "proceed with next sample!");
  ui_->uploadSampleButton->setEnabled(false);
  ui_->runSegmentationButton->setEnabled(false);
  ui_->submitLabelButton->setEnabled(true);
  session_->get_project()->save();
  SW_LOG(
      "✅ Segmentation for the current sample done! Submit the prediction label to server or "
      "proceed with next sample!");
  Q_EMIT progress(100);
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handleSubmitLabelCompleted() {
  SW_WARN(
      "The predicted or modified segmentation label will be written to the "
      "server's {} datastore.",
      server_address_);
  ui_->uploadSampleButton->setEnabled(false);
  ui_->runSegmentationButton->setEnabled(false);
  ui_->submitLabelButton->setEnabled(false);
  SW_LOG("✅ Label submitted to the server. Proceed with next sample.")
  samples_processed_++;
  // Q_EMIT
  // progress((int)(samples_processed_/session_->get_shapes().size())*100);
  Q_EMIT progress(100);
}

//---------------------------------------------------------------------------
void MonaiLabelTool::set_session(QSharedPointer<Session> session) {
  session_ = session;
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void MonaiLabelTool::set_app(ShapeWorksStudioApp* app) { app_ = app; }

//---------------------------------------------------------------------------
void MonaiLabelTool::shutdown() { app_->get_py_worker()->abort_job(); }

//---------------------------------------------------------------------------
bool MonaiLabelTool::is_active() { return session_ && session_->get_tool_state() == Session::MONAI_C; }

//---------------------------------------------------------------------------
void MonaiLabelTool::handle_progress(int val, QString message) { SW_PROGRESS(val, message.toStdString()); }

//---------------------------------------------------------------------------
void MonaiLabelTool::handle_error(QString msg) {}

//---------------------------------------------------------------------------
void MonaiLabelTool::loadParamsFromUi() {
  server_address_ = ui_->serverAddressField->text().toStdString();
  model_type_ = ui_->monaiModelTypeCombo->currentText().toStdString();
  std::transform(model_type_.begin(), model_type_.end(), model_type_.begin(), ::tolower);
}

//---------------------------------------------------------------------------
void MonaiLabelTool::onServerAddressChanged() {
  server_address_ = ui_->serverAddressField->text().toStdString();
  SW_DEBUG("Server Address changed to {}", server_address_);
}

//---------------------------------------------------------------------------
void MonaiLabelTool::onModelTypeChanged(int index) {
  model_type_ = ui_->monaiModelTypeCombo->itemText(index).toStdString();
  std::transform(model_type_.begin(), model_type_.end(), model_type_.begin(), ::tolower);
  SW_DEBUG("Model type changed to {}", model_type_);
  if (monai_label_job_) {
    monai_label_job_->setModelType(model_type_);
  } else {
    if (model_type_ == MONAI_MODE_SEGMENTATION) runSegmentationTool();
  }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::resizeEvent(QResizeEvent* event) { QWidget::resizeEvent(event); }
//---------------------------------------------------------------------------
}  // namespace monailabel
