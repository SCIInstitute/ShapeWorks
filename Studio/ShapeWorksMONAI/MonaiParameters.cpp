#include "MonaiParameters.h"

namespace shapeworks {


namespace Keys {
const std::string MONAI_MODEL = "model";
}  // namespace Keys

namespace Defaults {

const std::string STRATEGY = "random";
const std::string MONAI_MODEL = "segmentation";
}  // namespace Defaults

//---------------------------------------------------------------------------
MonaiParameters::MonaiParameters(ProjectHandle project) : project_(project) {
  params_ = project_->get_parameters(Parameters::MONAI_PARAMS);
}

//---------------------------------------------------------------------------
void MonaiParameters::restore_defaults() { params_.reset_parameters(); }

//---------------------------------------------------------------------------
void MonaiParameters::save_to_project() { project_->set_parameters(Parameters::MONAI_PARAMS, params_); }

}  // namespace shapeworks
