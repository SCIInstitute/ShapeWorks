#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>

#include <boost/filesystem.hpp>

#include <vector>

#include <Libs/Image/Image.h>
#include <Libs/Mesh/Mesh.h>
#include <Libs/Mesh/MeshUtils.h>
#include <Libs/Utils/StringUtils.h>

#include <Groom.h>
#include <GroomParameters.h>

#include <itkConnectedComponentImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkCastImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkOrientImageFilter.h>

using namespace shapeworks;

typedef float PixelType;
typedef itk::Image<PixelType, 3> ImageType;

//---------------------------------------------------------------------------
Groom::Groom(ProjectHandle project)
{
  this->project_ = project;
}

//---------------------------------------------------------------------------
bool Groom::run()
{
  this->progress_ = 0;
  this->progress_counter_ = 0;
  this->total_ops_ = this->get_total_ops();

  auto subjects = this->project_->get_subjects();

  if (subjects.empty()) {
    throw std::invalid_argument("No subjects to groom");
  }
  tbb::atomic<bool> success = true;

  tbb::parallel_for(
    tbb::blocked_range<size_t>{0, subjects.size()},
    [&](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i < r.end(); ++i) {

      for (int domain = 0; domain < subjects[i]->get_number_of_domains(); domain++) {

        if (this->abort_) {
          success = false;
          continue;
        }

        if (subjects[i]->get_domain_types()[domain] == DomainType::Image) {
          if (!this->image_pipeline(subjects[i], domain)) {
            success = false;
          }
        }

        if (subjects[i]->get_domain_types()[domain] == DomainType::Mesh) {
          if (!this->mesh_pipeline(subjects[i], domain)) {
            success = false;
          }
        }
      }
    }
  });

  if (!this->run_alignment()) {
    success = false;
  }

  // store back to project
  this->project_->store_subjects();
  return success;
}

//---------------------------------------------------------------------------
bool Groom::image_pipeline(std::shared_ptr<Subject> subject, size_t domain)
{
  // grab parameters
  auto params = GroomParameters(this->project_, this->project_->get_domain_names()[domain]);

  auto path = subject->get_segmentation_filenames()[domain];

  // load the image
  Image image(path);

  // set orientation to RAI
  using Orienter = itk::OrientImageFilter<ImageType, ImageType>;
  Orienter::Pointer orienter = Orienter::New();
  orienter->UseImageDirectionOn();
  orienter->SetDesiredCoordinateOrientation(
    itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
  orienter->SetInput(image.getITKImage());
  orienter->Update();
  image = Image(orienter->GetOutput());

  // define a groom transform
  auto transform = itk::AffineTransform<double, 3>::New();
  transform->SetIdentity();

  if (this->skip_grooming_) {
    std::vector<std::vector<double>> groomed_transforms;
    std::vector<double> groomed_transform;
    auto transform_params = transform->GetParameters();
    for (int i = 0; i < transform_params.size(); i++) {
      groomed_transform.push_back(transform_params[i]);
    }
    groomed_transforms.push_back(groomed_transform);
    subject->set_groomed_transforms(groomed_transforms);

    {
      // lock for project data structure
      tbb::mutex::scoped_lock lock(mutex_);

      // update groomed filenames
      std::vector<std::string> groomed_filenames = subject->get_groomed_filenames();
      if (domain >= groomed_filenames.size()) {
        groomed_filenames.resize(domain + 1);
      }
      groomed_filenames[domain] = path;

      // store filenames back to subject
      subject->set_groomed_filenames(groomed_filenames);
    }

    return true;
  }

  // centering
  if (params.get_use_center()) {
    auto centering = this->center(image);

    itk::MatrixOffsetTransformBase<double, 3, 3>::OutputVectorType tform;
    tform[0] = centering[0];
    tform[1] = centering[1];
    tform[2] = centering[2];
    transform->SetTranslation(tform);

    this->increment_progress();
  }

  if (this->abort_) { return false; }

  // store transform
  std::vector<std::vector<double>> groomed_transforms = subject->get_groomed_transforms();
  std::vector<double> groomed_transform;
  auto transform_params = transform->GetParameters();
  for (int i = 0; i < transform_params.size(); i++) {
    groomed_transform.push_back(transform_params[i]);
  }
  if (domain >= groomed_transforms.size()) {
    groomed_transforms.resize(domain + 1);
  }
  groomed_transforms[domain] = groomed_transform;
  subject->set_groomed_transforms(groomed_transforms);

  // isolate
  if (params.get_isolate_tool()) {
    this->isolate(image);
    this->increment_progress();
  }

  if (this->abort_) { return false; }

  // fill holes
  if (params.get_fill_holes_tool()) {
    image.closeHoles();
    this->increment_progress();
  }

  if (this->abort_) { return false; }

  // autopad
  if (params.get_auto_pad_tool()) {
    image.pad(params.get_padding_amount());
    this->increment_progress();
  }

  if (this->abort_) { return false; }

  // antialias
  if (params.get_antialias_tool()) {
    image.antialias(params.get_antialias_iterations());
    this->increment_progress();
  }

  if (this->abort_) { return false; }

  // create distance transform
  if (params.get_fast_marching()) {
    image.computeDT();
    this->increment_progress(10);
  }

  if (this->abort_) { return false; }

  // blur
  if (params.get_blur_tool()) {
    image.gaussianBlur(params.get_blur_amount());
    this->increment_progress();
  }

  if (this->abort_) { return false; }

  // groomed filename
  std::string dt_name = this->get_output_filename(path, DomainType::Image);

  // save image
  image.write(dt_name);

  {
    // lock for project data structure
    tbb::mutex::scoped_lock lock(mutex_);

    // update groomed filenames
    std::vector<std::string> groomed_filenames = subject->get_groomed_filenames();
    if (domain >= groomed_filenames.size()) {
      groomed_filenames.resize(domain + 1);
    }
    groomed_filenames[domain] = dt_name;

    // store filenames back to subject
    subject->set_groomed_filenames(groomed_filenames);
  }

  return true;
}

//---------------------------------------------------------------------------
bool Groom::mesh_pipeline(std::shared_ptr<Subject> subject, size_t domain)
{
  // grab parameters
  auto params = GroomParameters(this->project_, this->project_->get_domain_names()[domain]);

  auto path = subject->get_segmentation_filenames()[domain];

  // groomed mesh name
  std::string groom_name = this->get_output_filename(path, DomainType::Mesh);

  Mesh mesh = MeshUtils::threadSafeReadMesh(path);

  // define a groom transform
  auto transform = itk::AffineTransform<double, 3>::New();
  transform->SetIdentity();

  if (!this->skip_grooming_) {

    if (params.get_fill_holes_tool()) {
      mesh.fillHoles();
      this->increment_progress();
    }

    if (params.get_mesh_smooth()) {
      if (params.get_mesh_smoothing_method() == GroomParameters::GROOM_SMOOTH_VTK_LAPLACIAN_C) {
        mesh.smooth(params.get_mesh_vtk_laplacian_iterations(),
                    params.get_mesh_vtk_laplacian_relaxation());
      }
      else if (params.get_mesh_smoothing_method() ==
               GroomParameters::GROOM_SMOOTH_VTK_WINDOWED_SINC_C) {
        mesh.smoothSinc(params.get_mesh_vtk_windowed_sinc_iterations(),
                        params.get_mesh_vtk_windowed_sinc_passband());
      }
      this->increment_progress();
    }

    // centering
    if (params.get_use_center()) {
      auto diff = mesh.centerOfMass();
      itk::MatrixOffsetTransformBase<double, 3, 3>::OutputVectorType tform;
      tform[0] = -diff[0];
      tform[1] = -diff[1];
      tform[2] = -diff[2];
      transform->SetTranslation(tform);
      this->increment_progress();
    }
  }

  // store transform
  std::vector<std::vector<double>> groomed_transforms = subject->get_groomed_transforms();
  std::vector<double> groomed_transform;
  auto transform_params = transform->GetParameters();
  for (size_t i = 0; i < transform_params.size(); i++) {
    groomed_transform.push_back(transform_params[i]);
  }
  if (domain >= groomed_transforms.size()) {
    groomed_transforms.resize(domain + 1);
  }
  groomed_transforms[domain] = groomed_transform;
  subject->set_groomed_transforms(groomed_transforms);

  // save the groomed mesh
  MeshUtils::threadSafeWriteMesh(groom_name, mesh);

  {
    // lock for project data structure
    tbb::mutex::scoped_lock lock(mutex_);

    // update groomed filenames
    std::vector<std::string> groomed_filenames = subject->get_groomed_filenames();
    if (domain >= groomed_filenames.size()) {
      groomed_filenames.resize(domain + 1);
    }
    groomed_filenames[domain] = groom_name;

    // store filename back to subject
    subject->set_groomed_filenames(groomed_filenames);
  }

  return true;
}

//---------------------------------------------------------------------------
void Groom::isolate(Image& image)
{
  ImageType::Pointer img = image;

  typedef itk::Image<unsigned char, 3> IsolateType;
  typedef itk::CastImageFilter<ImageType, IsolateType> ToIntType;
  ToIntType::Pointer filter = ToIntType::New();
  filter->SetInput(img);
  filter->Update();

  // Find the connected components in this image.
  auto cc_filter = itk::ConnectedComponentImageFilter<IsolateType, IsolateType>::New();
  cc_filter->SetInput(filter->GetOutput());
  cc_filter->FullyConnectedOn();
  cc_filter->Update();

  auto relabel = itk::RelabelComponentImageFilter<IsolateType, IsolateType>::New();
  relabel->SetInput(cc_filter->GetOutput());
  relabel->SortByObjectSizeOn();
  relabel->Update();

  auto thresh = itk::ThresholdImageFilter<IsolateType>::New();
  thresh->SetInput(relabel->GetOutput());
  thresh->SetOutsideValue(0);
  thresh->ThresholdBelow(0);
  thresh->ThresholdAbove(1);
  thresh->Update();

  auto cast_filter = itk::CastImageFilter<IsolateType, ImageType>::New();
  cast_filter->SetInput(thresh->GetOutput());
  cast_filter->Update();

  image = Image(cast_filter->GetOutput());
}

//---------------------------------------------------------------------------
Vector3 Groom::center(Image& image)
{
  auto diff = image.centerOfMass();
  Vector3 translation;
  translation[0] = -diff[0];
  translation[1] = -diff[1];
  translation[2] = -diff[2];
  return translation;

/*
   // capture full translation
   auto com = image.centerOfMass();
   // set center to origin
   image.recenter();
   // calculate translation for center of mass to new center
   auto diff = image.centerOfMass();
   Vector3 translation;
   translation[0] = -diff[0];
   translation[1] = -diff[1];
   translation[2] = -diff[2];

   AffineTransformPtr xform(AffineTransform::New());
   xform->Translate(-translation);
   image.applyTransform(xform, Image::NearestNeighbor);

   translation[0] = com[0];
   translation[1] = com[1];
   translation[2] = com[2];
   return translation;
 */
}

//---------------------------------------------------------------------------
int Groom::get_total_ops()
{
  int num_subjects = this->project_->get_subjects().size();

  int num_tools = 0;

  auto domains = this->project_->get_domain_names();

  for (int i = 0; i < domains.size(); i++) {

    auto params = GroomParameters(this->project_, domains[i]);

    num_tools += params.get_use_center() ? 1 : 0;
    num_tools += params.get_isolate_tool() ? 1 : 0;
    num_tools += params.get_fill_holes_tool() ? 1 : 0;
    num_tools += params.get_auto_pad_tool() ? 1 : 0;
    num_tools += params.get_antialias_tool() ? 1 : 0;
    num_tools += params.get_fast_marching() ? 10 : 0;
    num_tools += params.get_blur_tool() ? 1 : 0;
  }

  return num_subjects * num_tools;
}

//---------------------------------------------------------------------------
void Groom::increment_progress(int amount)
{
  this->progress_counter_ += amount;
  this->progress_ = static_cast<float>(this->progress_counter_)
                    / static_cast<float>(this->total_ops_) * 100.0;
  this->update_progress();
}

//---------------------------------------------------------------------------
void Groom::set_skip_grooming(bool skip)
{
  this->skip_grooming_ = skip;
}

//---------------------------------------------------------------------------
void Groom::abort()
{
  this->abort_ = true;
}

//---------------------------------------------------------------------------
bool Groom::get_aborted()
{
  return this->abort_;
}

//---------------------------------------------------------------------------
bool Groom::run_alignment()
{
  size_t num_domains = this->project_->get_number_of_domains_per_subject();
  auto subjects = this->project_->get_subjects();

  auto base_params = GroomParameters(this->project_);
  if (num_domains > 1 && base_params.get_global_alignment()) { // global alignment for multiple domains
    std::vector<Mesh> meshes;

    for (size_t i = 0; i < subjects.size(); i++) {
      Mesh mesh = this->get_mesh(i, 0);
      for (size_t domain = 1; domain < num_domains; domain++) {
        mesh += this->get_mesh(i, domain); // combine
      }
      meshes.push_back(mesh);
    }

    size_t reference_mesh = MeshUtils::findReferenceMesh(meshes);

    auto transforms = Groom::get_transforms(meshes, reference_mesh);

    for (size_t i = 0; i < subjects.size(); i++) {
      auto subject = subjects[i];
      // store transform
      std::vector<std::vector<double>> groomed_transforms = subject->get_groomed_transforms();

      for (size_t domain = 0; domain < num_domains; domain++) {
        if (domain >= groomed_transforms.size()) {
          groomed_transforms.resize(domain + 1);
        }
        groomed_transforms[domain] = transforms[i];
      }
      subject->set_groomed_transforms(groomed_transforms);
    }
  }
  else {

    // per-domain alignment
    for (size_t domain = 0; domain < num_domains; domain++) {
      if (this->abort_) {
        return false;
      }

      auto params = GroomParameters(this->project_, this->project_->get_domain_names()[domain]);

      if (params.get_use_icp()) {
        std::vector<Mesh> meshes;
        for (size_t i = 0; i < subjects.size(); i++) {
          meshes.push_back(this->get_mesh(i, domain));
        }

        size_t reference_mesh = MeshUtils::findReferenceMesh(meshes);

        auto transforms = Groom::get_transforms(meshes, reference_mesh);

        for (size_t i = 0; i < subjects.size(); i++) {
          auto subject = subjects[i];
          // store transform
          std::vector<std::vector<double>> groomed_transforms = subject->get_groomed_transforms();

          if (domain >= groomed_transforms.size()) {
            groomed_transforms.resize(domain + 1);
          }
          groomed_transforms[domain] = transforms[i];
          subject->set_groomed_transforms(groomed_transforms);
        }
      }
    }
  }
  return true;
}

//---------------------------------------------------------------------------
std::string Groom::get_output_filename(std::string input, DomainType domain_type)
{
  // grab parameters
  auto params = GroomParameters(this->project_);

  // if the project is not saved, use the path of the input filename
  auto filename = this->project_->get_filename();
  if (filename == "") {
    filename = input;
  }

  auto base = StringUtils::getPath(filename);
  if (base == filename) {
    base = ".";
  }

  auto prefix = params.get_groom_output_prefix();

  std::string suffix = "_DT.nrrd";
  if (domain_type == DomainType::Mesh) {
    suffix = "_groomed.vtk";
  }

  auto path = base + "/" + prefix;

  boost::filesystem::create_directories(path);

  auto output = path + "/" + StringUtils::getFileNameWithoutExtension(input) + suffix;

  return output;
}

//---------------------------------------------------------------------------
Mesh Groom::get_mesh(int subject, int domain)
{
  auto subjects = this->project_->get_subjects();
  auto path = subjects[subject]->get_segmentation_filenames()[domain];

  if (subjects[subject]->get_domain_types()[domain] == DomainType::Image) {
    Image image(path);
    return image.toMesh(0.5);
  }
  else if (subjects[subject]->get_domain_types()[domain] == DomainType::Mesh) {
    Mesh mesh = MeshUtils::threadSafeReadMesh(path);
    return mesh;
  }
  throw std::invalid_argument("invalid domain type");
}

//---------------------------------------------------------------------------
std::vector<std::vector<double>> Groom::get_transforms(const std::vector<Mesh> meshes, size_t reference)
{
  std::vector<std::vector<double>> transforms(meshes.size());

  tbb::parallel_for(
    tbb::blocked_range<size_t>{0, meshes.size()},
    [&](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i < r.end(); ++i) {

      vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
      matrix->Identity();

      if (i != reference) {
        Mesh source = meshes[i];
        Mesh target = meshes[reference];
        matrix = MeshUtils::createICPTransform(source.getVTKMesh(),
                                               target.getVTKMesh(), Mesh::Rigid, 100, true);
      }

      std::vector<double> groomed_transform;

      for (int i = 0; i < 16; i++) {
        groomed_transform.push_back(matrix->GetData()[i]);
      }
      transforms[i] = groomed_transform;
    }
  });
  return transforms;
}
