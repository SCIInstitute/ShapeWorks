#pragma once

#include <Libs/Project/Project.h>
class QWidget;

#include <QStringList>

namespace shapeworks {

class StudioUtils {
public:

  static bool ask_multiple_domains_as_single(QWidget *parent, std::shared_ptr<Project> project);

  static QStringList to_string_list(std::vector<std::string> vector);

};

} // namespace shapeworks
