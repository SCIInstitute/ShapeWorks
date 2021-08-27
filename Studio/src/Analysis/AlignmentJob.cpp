#include <iostream>

#include <Libs/Project/Project.h>

#include "AlignmentJob.h"
namespace shapeworks {

//-----------------------------------------------------------------------------
AlignmentJob::AlignmentJob(ProjectHandle project, AlignmentType alignment_type)
  : project_(project), alignment_type_(alignment_type)
{
  qRegisterMetaType<Eigen::VectorXd>("Eigen::MatrixXd");
}

//-----------------------------------------------------------------------------
void AlignmentJob::run()
{}

//-----------------------------------------------------------------------------
QString AlignmentJob::name()
{
  return "Alignment";
}
} // namespace shapeworks
