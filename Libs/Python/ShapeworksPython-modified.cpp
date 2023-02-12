#include <Eigen/Eigen>

#include <Optimize/Optimize.h>

Eigen::MatrixXd optimize_get_particle_system(shapeworks::Optimize *opt)
{
  shapeworks::MatrixContainer container = opt->GetParticleSystem();
  return container.matrix_;
}

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include <bitset>
#include <sstream>

#include "Optimize.h"
#include "Project.h"
#include "pybind_utils.h"

using namespace shapeworks;

PYBIND11_MODULE(shapeworks_py, m)
{
  m.doc() = "ShapeWorks Python API";

  m.attr("Pi") = std::atan(1.0) * 4.0;

  m.def("seed",
        &ShapeworksUtils::setRngSeed,
        "sets the seed for random number generation (internal use)",
        "seed"_a=std::chrono::system_clock::now().time_since_epoch().count());




  // Project 
  // py::class_<Project>(m, "Project")
  py::class_<Project, std::shared_ptr<Project> /* <- holder type */> proj(m, "Project");
  proj.def(py::init<>())

  .def("load",
      &Project::load,
      "Load from XLSX file",
      "filename"_a)

  .def("save",
      &Project::save,
      "Save to XLSX file",
      "filename"_a)

  .def("get_filename",
      &Project::get_filename,
      "Return the filename")

  .def("set_filename",
      &Project::set_filename,
      "Set project filename",
      "filename"_a)

  .def("get_headers",
      &Project::get_headers,
      "Return the headers of the subject sheet")

  .def("get_string_column",
      &Project::get_string_column,
      "Return a column by name",
      "name"_a)

  .def("get_number_of_subjects",
      &Project::get_number_of_subjects,
      "Return the number of subjects in the project")

  .def("get_number_of_domains_per_subject",
      &Project::get_number_of_domains_per_subject,
      "Return the number of domains")

  .def("get_original_domain_types",
      &Project::get_original_domain_types,
      "Return the original domain types")

  .def("set_original_domain_types",
      &Project::set_original_domain_types,
      "Set the original domain types",
       "types"_a)

  .def("get_groomed_domain_types",
      &Project::get_groomed_domain_types,
      "Return the groomed domain types")

  .def("set_groomed_domain_types",
      &Project::set_groomed_domain_types,
      "Set the groomed domain types",
       "types"_a)

  .def("get_domain_names",
      &Project::get_domain_names,
      "Return the domain names (e.g. femur, pelvis, etc)")

  .def("get_subjects",
      [](Project &project) -> decltype(auto) {
        std::vector<Subject> py_subjects;
        for (auto s :project.get_subjects()) {
          py_subjects.push_back(*s);
        }
        return py_subjects;
      },
      "Return the list of Subjects")

  .def("get_originals_present",
      &Project::get_originals_present,
      "Return if original files are present")

  .def("get_groomed_present",
      &Project::get_groomed_present,
      "Return if groomed files are present")

  .def("get_particles_present",
      &Project::get_particles_present,
      "Return if particle files are present")

  .def("get_images_present",
      &Project::get_images_present)

  .def("get_feature_names",
      &Project::get_feature_names)

  .def("get_group_names",
      &Project::get_group_names)

  .def("get_group_values",
      &Project::get_group_values,
      "group_names"_a)

  .def("get_parameters",
      &Project::get_parameters,
      "name"_a,"domain_name"_a="")

  .def("set_parameters",
      &Project::set_parameters,
      "name"_a,"params"_a,"domain_name"_a="")

  .def("clear_parameters",
      &Project::clear_parameters,
      "name"_a)

  .def("store_subjects",
      &Project::update_subjects)

  .def("get_supported_version",
      &Project::get_supported_version)

  .def("get_version",
      &Project::get_version)

  .def("set_subjects",
       [](Project &project, std::vector<Subject> subjects) -> decltype(auto) {
            std::vector<std::shared_ptr<Subject>> sharedSubjects;
            for (auto sub : subjects) {
               std::shared_ptr<Subject> s = std::make_shared<Subject>(sub);
               sharedSubjects.push_back(s);
            }
            return project.set_subjects(sharedSubjects);
       },
       "subjects"_a)
  ; // Project

  // Optimize (TODO)
  py::class_<Optimize>(m, "Optimize")

  .def(py::init<>())

  .def("LoadParameterFile",
       &Optimize::LoadParameterFile)

  .def("LoadPytorchModel",
       &Optimize::LoadPytorchModel)

  .def("LoadXlsxProjectFile",
       &Optimize::LoadXlsxProjectFile)

  .def("SetUpOptimize",
       &Optimize::SetUpOptimize,
       "projectFile"_a )

  .def("Run",
       &Optimize::Run)

  .def("SaveProjectFileAfterOptimize",
       &Optimize::SaveProjectFileAfterOptimize)

  .def("SetIterationCallbackFunction",
       &Optimize::SetIterationCallbackFunction)

  .def("SetNonLinearTrainModelCallbackFunction",
      &Optimize::SetNonLinearTrainModelCallbackFunction)

  .def("GetOptimizing", &Optimize::GetOptimizing)

  .def("GetBaseSpaceInverseCovarianceMatrix",
       [](Optimize *opt){
          shapeworks::MatrixContainer container = opt->GetBaseSpaceInverseCovarianceMatrix();
          return container.matrix_;
     }
     )
     
  .def("GetBaseSpaceMean",
       [](Optimize *opt){
          shapeworks::MatrixContainer container = opt->GetBaseSpaceMean();
          return container.matrix_;
     }
     )

  .def("GetParticleSystem",
       &optimize_get_particle_system)
  ;


} // PYBIND11_MODULE(shapeworks_py)
