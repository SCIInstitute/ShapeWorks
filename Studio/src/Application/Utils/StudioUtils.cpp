#include <Utils/StudioUtils.h>

#include <QMessageBox>

namespace shapeworks {

bool StudioUtils::ask_multiple_domains_as_single(QWidget* parent, std::shared_ptr<Project> project)
{
  bool single = true;
  if (project->get_number_of_domains_per_subject() > 0) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(parent, "Multiple Domains",
                                  "This export contains multiple domains.\n\n"
                                  "Would you like each domain exported separately?\n\n"
                                  "Each will be suffixed with the domain name.",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
      single = false;
    }
  }
  return single;
}

}