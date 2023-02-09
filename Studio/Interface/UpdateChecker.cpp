#include "UpdateChecker.h"

#include <Applications/Configuration.h>
#include <Data/Preferences.h>
#include <Logging.h>
#include <Utils/StudioUtils.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <nlohmann/json.hpp>

#include "ui_UpdateChecker.h"

using json = nlohmann::ordered_json;

namespace shapeworks {

UpdateChecker::UpdateChecker(Preferences& prefs, QWidget* parent)
    : QDialog(parent), prefs_(prefs), ui_(new Ui::UpdateChecker) {
  ui_->setupUi(this);
  connect(&network_, &QNetworkAccessManager::finished, this, &UpdateChecker::handleNetworkReply);
  connect(ui_->ok_button, &QPushButton::clicked, this, &UpdateChecker::hide);
  ui_->auto_update_checkbox->setChecked(prefs_.get_auto_update_check());
  connect(ui_->auto_update_checkbox, &QCheckBox::toggled, &prefs_, &Preferences::set_auto_update_check);
  connect(ui_->snooze_button, &QPushButton::clicked, this, [this]() {
    prefs_.set_update_snooze_until(QDateTime::currentDateTime().addDays(1));
    accept();
  });
}

UpdateChecker::~UpdateChecker() { delete ui_; }

void UpdateChecker::run_manual_update_check() {
  manual_trigger_ = true;
  run_update_check();
}

void UpdateChecker::run_auto_update_check() {
  if (prefs_.get_auto_update_check()) {
    QDateTime snooze_time = prefs_.get_update_snooze_until();
    if (!snooze_time.isValid() || QDateTime::currentDateTime() > snooze_time) {
      manual_trigger_ = false;
      run_update_check();
    }
  }
}

void UpdateChecker::run_update_check() {
  // check if a new version is available
  QNetworkRequest request(QUrl("http://www.sci.utah.edu/~shapeworks/version.json"));
  network_.get(request);
}

void UpdateChecker::handleNetworkReply(QNetworkReply* reply) {
  std::string response = QString(reply->readAll()).toStdString();

  // get the json response
  auto j = json::parse(response);

  std::string platform = StudioUtils::get_platform_string().toStdString();

  int major = j[platform]["major"].get<int>();
  int minor = j[platform]["minor"].get<int>();
  int patch = j[platform]["patch"].get<int>();
  QString message = QString::fromStdString(j[platform]["message"].get<std::string>());

  bool update_available = false;
  if (major > SHAPEWORKS_MAJOR_VERSION) {
    update_available = true;
  } else if (major == SHAPEWORKS_MAJOR_VERSION && minor > SHAPEWORKS_MINOR_VERSION) {
    update_available = true;
  } else if (major == SHAPEWORKS_MAJOR_VERSION && minor == SHAPEWORKS_MINOR_VERSION &&
             patch > SHAPEWORKS_PATCH_VERSION) {
    update_available = true;
  }

  if (update_available) {
    auto url = QString("https://github.com/SCIInstitute/ShapeWorks/releases/latest");

    auto title = QString("New version available");
    setWindowTitle(title);
    ui_->label->setTextFormat(Qt::RichText);
    ui_->label->setOpenExternalLinks(true);
    ui_->label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui_->label->setText(QString("A new version of ShapeWorks is available.<br><br>"
                                "To download the latest version, please visit:<br><br><a href=\"%1\">%1</a><br><br>" +
                                message)
                            .arg(url));

    show();
    raise();

  } else {
    if (manual_trigger_) {
      // show a messagebox saying there is no update
      auto title = QString("No update available");
      auto info = QString("You are running the latest version of ShapeWorks.");
      QMessageBox::information(nullptr, title, info, QMessageBox::Ok);
    }
  }
}
}  // namespace shapeworks