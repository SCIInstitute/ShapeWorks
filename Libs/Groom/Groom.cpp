#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>

#include <vector>

#include <Libs/Image/Image.h>
#include <Libs/Mesh/Mesh.h>

#include <Groom.h>
#include <GroomParameters.h>

#include "itkConnectedComponentImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkCastImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkThresholdImageFilter.h"

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

        if (this->abort_) { success = false; continue; }

        if (subjects[i]->get_domain_types()[0] == DomainType::Image) {
          if (!this->image_pipeline(subjects[i])) {
            success = false;
          }
        }

        if (subjects[i]->get_domain_types()[0] == DomainType::Mesh) {
          if (!this->mesh_pipeline(subjects[i])) {
            success = false;
          }
        }

      }
    });

  // store back to project
  this->project_->store_subjects();
  return success;
}

//---------------------------------------------------------------------------
bool Groom::image_pipeline(std::shared_ptr<Subject> subject)
{
  // grab parameters
  auto params = GroomParameters(this->project_);

  // single domain support right now
  auto path = subject->get_segmentation_filenames()[0];

  // load the image
  Image image(path);

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

    // only single domain supported so far
    std::vector<std::string> groomed_filenames{path};
    // store filename back to subject
    subject->set_groomed_filenames(groomed_filenames);
    return true;
  }

  // centering
  if (params.get_center_tool()) {
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
  std::vector<std::vector<double>> groomed_transforms;
  std::vector<double> groomed_transform;
  auto transform_params = transform->GetParameters();
  for (int i = 0; i < transform_params.size(); i++) {
    groomed_transform.push_back(transform_params[i]);
  }
  groomed_transforms.push_back(groomed_transform);
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
  std::string dt_name = path;
  dt_name = dt_name.substr(0, dt_name.find_last_of(".")) + "_DT.nrrd";

  // save image
  image.write(dt_name);

  // only single domain supported so far
  std::vector<std::string> groomed_filenames{dt_name};
  // store filename back to subject
  subject->set_groomed_filenames(groomed_filenames);

  return true;
}

//---------------------------------------------------------------------------
bool Groom::mesh_pipeline(std::shared_ptr<Subject> subject)
{
  // grab parameters
  auto params = GroomParameters(this->project_);

  auto path = subject->get_segmentation_filenames()[0];

  // groomed mesh name
  std::string groom_name = path;
  groom_name = groom_name.substr(0, groom_name.find_last_of('.')) + "_groomed.ply";

  try {
    // load the image
    std::shared_ptr<Mesh> mesh = this->load_mesh(path);
    if (!mesh) {
      return false;
    }

    // define a groom transform
    auto transform = itk::AffineTransform<double, 3>::New();
    transform->SetIdentity();

    if (!this->skip_grooming_) {

      // centering
      if (params.get_center_tool()) {
        auto com = mesh->centerOfMass();

        Vector3 vector;
        vector[0] = -com[0];
        vector[1] = -com[1];
        vector[2] = -com[2];
        mesh->translate_mesh(vector);

        itk::MatrixOffsetTransformBase<double, 3, 3>::OutputVectorType tform;
        tform[0] = com[0];
        tform[1] = com[1];
        tform[2] = com[2];
        transform->SetTranslation(tform);
        this->increment_progress();
      }
    }

    // store transform
    std::vector<std::vector<double>> groomed_transforms;
    std::vector<double> groomed_transform;
    auto transform_params = transform->GetParameters();
    for (int i = 0; i < transform_params.size(); i++) {
      groomed_transform.push_back(transform_params[i]);
    }
    groomed_transforms.push_back(groomed_transform);
    subject->set_groomed_transforms(groomed_transforms);

    // save the groomed mesh
    this->save_mesh(mesh, groom_name);

    // only single domain supported so far
    std::vector<std::string> groomed_filenames{groom_name};
    // store filename back to subject
    subject->set_groomed_filenames(groomed_filenames);

  } catch (std::exception e) {
    std::cerr << "Exception: " << e.what() << "\n";
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------
void Groom::isolate(Image &image)
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
Vector3 Groom::center(Image &image)
{
  image.recenter();
  auto com = image.centerOfMass();
  auto diff = image.center() - com;
  Vector3 translation;
  translation[0] = -diff[0];
  translation[1] = -diff[1];
  translation[2] = -diff[2];

  //image.translate(translation, Image::InterpolationType::NearestNeighbor);

  AffineTransformPtr xform(AffineTransform::New());
  xform->Translate(-translation);
  image.applyTransform(xform,Image::NearestNeighbor);


  translation[0] = com[0];
  translation[1] = com[1];
  translation[2] = com[2];
  return translation;
}

//---------------------------------------------------------------------------
int Groom::get_total_ops()
{
  int num_subjects = this->project_->get_subjects().size();
  auto params = GroomParameters(this->project_);

  int num_tools = 0;
  num_tools += params.get_center_tool() ? 1 : 0;
  num_tools += params.get_isolate_tool() ? 1 : 0;
  num_tools += params.get_fill_holes_tool() ? 1 : 0;
  num_tools += params.get_auto_pad_tool() ? 1 : 0;
  num_tools += params.get_antialias_tool() ? 1 : 0;
  num_tools += params.get_fast_marching() ? 10 : 0;
  num_tools += params.get_blur_tool() ? 1 : 0;

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
std::shared_ptr<Mesh> Groom::load_mesh(std::string filename)
{
  tbb::mutex::scoped_lock lock(this->mutex_);
  try {
    auto mesh = std::make_shared<Mesh>(filename);
    return mesh;
  } catch (std::exception e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return nullptr;
}

//---------------------------------------------------------------------------
bool Groom::save_mesh(std::shared_ptr<Mesh> mesh, std::string filename)
{
  tbb::mutex::scoped_lock lock(this->mutex_);
  try {
    mesh->write(filename);
    return true;
  } catch (std::exception e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return false;
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


