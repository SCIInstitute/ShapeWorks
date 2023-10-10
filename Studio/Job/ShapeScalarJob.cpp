#include <Common/Logging.h>
#include <Libs/Project/Project.h>
#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;  // to bring in the `_a` literal

#include <Job/ShapeScalarJob.h>

namespace shapeworks {

//---------------------------------------------------------------------------
ShapeScalarJob::ShapeScalarJob(QSharedPointer<Session> session,
                                       QString target_feature)
    : session_(session), target_feature_(target_feature) {}

//---------------------------------------------------------------------------
void ShapeScalarJob::run() {
  SW_DEBUG("Running shape scalar job");


  py::module sw = py::module::import("shapeworks");

  // auto stats = sw.attr("stats");

//  py::type network_analysis_class = sw.attr("ShapeScalar");

  SW_DEBUG("End shape scalar job");
}

//---------------------------------------------------------------------------
QString ShapeScalarJob::name() { return "Shape / Scalar Correlation"; }

//---------------------------------------------------------------------------
}  // namespace shapeworks
