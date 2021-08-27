#pragma once

#include <Data/Worker.h>
#include <Job/Job.h>
#include <Libs/Project/Project.h>
#include <Eigen/Eigen>

namespace shapeworks {

class AlignmentJob : public Job
{
  Q_OBJECT
public:

  enum AlignmentType {
    Local = -1,
    Global = -2
  };

  AlignmentJob(ProjectHandle project, AlignmentType alignment_type);

  void run() override;

  QString name() override;

private:
  ProjectHandle project_;
  AlignmentType alignment_type_;
};
}

Q_DECLARE_METATYPE(Eigen::MatrixXd);
