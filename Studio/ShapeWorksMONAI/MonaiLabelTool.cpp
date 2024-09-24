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
#include <ui_MonaiLabelTool.h>

#include <QComboBox>
#include <QRegularExpression>
#include <QTextEdit>
#include <QThread>

using namespace shapeworks;
namespace monailabel {

const std::string MonaiLabelTool::MONAI_MODE_SEGMENTATION("segmentation");
const std::string MonaiLabelTool::MONAI_MODE_DEEPGROW("deepgrow");
const std::string MonaiLabelTool::MONAI_MODE_DEEPEDIT("deepedit");


//---------------------------------------------------------------------------
MonaiLabelTool::MonaiLabelTool(Preferences& prefs) : preferences_(prefs) {
  ui_ = new Ui_MonaiLabelTool;
  ui_->setupUi(this);
  client_id_ = "user-xyz";

  ui_->serverAddressField->setToolTip("Enter MONAI Label server address");

  // MONAI params panel click behavior
  connect(ui_->monai_label_open_button, &QPushButton::toggled,
          ui_->monai_panel_content, &QWidget::setVisible);
  connect(ui_->monai_label_header, &QPushButton::clicked,
          ui_->monai_label_open_button, &QPushButton::toggle);

  connect(ui_->connectServerButton, &QPushButton::clicked, this,
          &MonaiLabelTool::onConnectServer);
  connect(ui_->currentSampleSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(handleSampleView()));

  // QRegularExpression urlRegex(R"(https?://(?:www\.)?[^\s/$.?#].[^\s]*)",
  //                             QRegularExpression::CaseInsensitiveOption);
  // QRegularExpressionValidator* urlValidator =
  //     new QRegularExpressionValidator(urlRegex, &lineEdit);
  // ui_->serverAddressField->setValidator(urlValidator);
  // when the subject_notes changes
  connect(ui_->serverAddressField, &QLineEdit::textChanged, this,
          &MonaiLabelTool::onServerAddressChanged);
  connect(ui_->monaiModelTypeCombo,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &MonaiLabelTool::onModelTypeChanged);
  update_panels();
}

//---------------------------------------------------------------------------
MonaiLabelTool::~MonaiLabelTool() {}

void MonaiLabelTool::triggerUpdateView() { Q_EMIT update_view(); }

//---------------------------------------------------------------------------
void MonaiLabelTool::updateDisplayPanels() {
  ui_->connectServerButton->setEnabled(!tool_is_running_);
  if (!session_ || !session_->get_project()) {
    return;
  }
  // TODO: switch later for different model types: DeepGrow, DeepEdits, etc
}

//---------------------------------------------------------------------------
void MonaiLabelTool::onConnectServer() {
  // if (tool_is_running_) {
  //   ui_->connectServerButton->setText("Disconnecting...");
  //   ui_->connectServerButton->setEnabled(false);
  //   monai_label_logic_->abort();
  //   app_->get_py_worker()->abort_job();
  // } else {
  // if (session_->get_filename() == "") {
  //   SW_ERROR("Save project before starting connection");
  //   ui_->connectServerButton->setEnabled(true);
  //   return;
  // } else
  //   session_->trigger_save();
  // loadParamsFromUi();
  server_address_ = "http://0.0.0.0:8000";
  model_type_ = "segmentation";

  runSegmentationTool();

  //   SW_DEBUG("Params from UI {} {}", server_address_, model_type_);

  //   if (model_type_ ==
  //       getModelType(MonaiLabelTool::MonaiModelType::SEGMENTATION)) {
  //     SW_DEBUG("Starting MONAI Label with model segmentation ");
  //     runSegmentationTool();
  //   } else {
  //     SW_ERROR("Invalid MONAI model type");
  //     return;
  //   }
  // // }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::enable_actions() {
  if (session_->get_num_shapes() < 1) {
    return;
  }
  ui_->currentSampleSpinBox->setEnabled(true);
  ui_->currentSampleSpinBox->setMaximum(session_->get_num_shapes() - 1);
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handleSampleView() {
  if (monai_label_logic_) {
    monai_label_logic_->setCurrentSampleNumber(getCurrentSampleNumber());
  }
  Q_EMIT sampleChanged();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
int MonaiLabelTool::getCurrentSampleNumber() {
  int val = ui_->currentSampleSpinBox->value();
  std::cout << "Setting Current sample to " << val << std::endl;
  return val;
}

//---------------------------------------------------------------------------
void MonaiLabelTool::runSegmentationTool() {
  tool_is_running_ = true;
  strategy_ = "random";
  std::cout << "Initializing Interpreter" << std::endl;
  app_->get_py_worker()->init();
  std::cout << "initializing MonaiLabelJob object" << std::endl;
  monai_label_logic_ = QSharedPointer<MonaiLabelJob>::create(
      session_, server_address_, client_id_, strategy_, model_type_);
  SW_DEBUG("Monai Label Job initialized!");
  connect(monai_label_logic_.data(), &MonaiLabelJob::progress, this,
          &MonaiLabelTool::handle_progress);
  connect(monai_label_logic_.data(), &MonaiLabelJob::finished, this,
          &MonaiLabelTool::handle_thread_complete);
  connect(ui_->uploadSampleButton, &QPushButton::clicked,
          monai_label_logic_.data(), &MonaiLabelJob::onUploadSampleClicked);
  connect(ui_->submitLabelButton, &QPushButton::clicked,
          monai_label_logic_.data(), &MonaiLabelJob::onSubmitLabelClicked);
  connect(ui_->runSegmentationButton, &QPushButton::clicked,
          monai_label_logic_.data(), &MonaiLabelJob::onRunSegmentationClicked);
  connect(monai_label_logic_.data(), &MonaiLabelJob::triggerUpdateView, this,
          &MonaiLabelTool::triggerUpdateView);

  SW_DEBUG("Starting Job with python worker");
  monai_label_logic_->run();
  ui_->connectServerButton->setEnabled(false);
  ui_->connectServerButton->setVisible(false);
  ui_->serverAddressField->setEnabled(false);
  QTimer::singleShot(1000, this,
                     [=]() { ui_->connectServerButton->setEnabled(true); });
}

//---------------------------------------------------------------------------
void MonaiLabelTool::set_session(QSharedPointer<Session> session) {
  session_ = session;
  load_params();
  update_panels();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void MonaiLabelTool::set_app(ShapeWorksStudioApp* app) { app_ = app; }

//---------------------------------------------------------------------------
void MonaiLabelTool::load_params() {
  updateDisplayPanels();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void MonaiLabelTool::shutdown() { app_->get_py_worker()->abort_job(); }

//---------------------------------------------------------------------------
bool MonaiLabelTool::is_active() {
  return session_ && session_->get_tool_state() == Session::MONAI_C;
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handle_thread_complete() {
  // try {
  //   if (!monai_label_logic_->is_aborted()) } catch (std::exception& e) {
  //   SW_ERROR("{}", e.what());
  // }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handle_progress(int val, QString message) {
  SW_PROGRESS(val, message.toStdString());
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handle_error(QString msg) {}

//---------------------------------------------------------------------------
void MonaiLabelTool::update_panels() {
  ui_->connectServerButton->setEnabled(!tool_is_running_);

  // if (!session_ || !session_->get_project()) {
  //   return;
  // }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::loadParamsFromUi() {
  server_address_ = ui_->serverAddressField->text().toStdString();
  model_type_ = ui_->monaiModelTypeCombo->currentText().toStdString();
  std::transform(model_type_.begin(), model_type_.end(), model_type_.begin(),
                 ::tolower);
}

//---------------------------------------------------------------------------
void MonaiLabelTool::onServerAddressChanged() {
  server_address_ = ui_->serverAddressField->text().toStdString();
  SW_DEBUG("Server Address changed to {}", server_address_);
  if (monai_label_logic_) {
    monai_label_logic_->setServer(server_address_);
  }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::onModelTypeChanged(int index) {
  model_type_ = ui_->monaiModelTypeCombo->itemText(index).toStdString();
  std::transform(model_type_.begin(), model_type_.end(), model_type_.begin(),
                 ::tolower);
  SW_DEBUG("Model type changed to {}", model_type_);
  if (monai_label_logic_) {
    monai_label_logic_->setModelType(model_type_);
  }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
}
//---------------------------------------------------------------------------
}  // namespace monailabel
