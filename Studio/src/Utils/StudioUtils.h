#pragma once

#include <Libs/Project/Project.h>
class QWidget;

namespace shapeworks {

class StudioUtils {
public:
  static bool ask_multiple_domains_as_single(QWidget *parent, std::shared_ptr<Project> project);
};

} // namespace shapeworks