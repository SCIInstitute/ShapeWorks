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
void Groom::run()
{
  this->progress_ = 0;
  this->progress_counter_ = 0;
  this->total_ops_ = this->get_total_ops();

  auto subjects = this->project_->get_subjects();

  tbb::parallel_for(
    tbb::blocked_range<size_t>{0, subjects.size()},
    [&](const tbb::blocked_range<size_t>& r) {
      for (size_t i = r.begin(); i < r.end(); ++i) {

        if (subjects[i]->get_domain_types()[0] == DomainType::Image) {
          this->image_groom_pipeline(subjects[i]);
        }

        if (subjects[i]->get_domain_types()[0] == DomainType::Mesh) {
          this->image_groom_pipeline(subjects[i]);
        }

      }
    });
  this->project_->store_subjects();

}

//---------------------------------------------------------------------------
void Groom::image_groom_pipeline(std::shared_ptr<Subject> subject)
{
  auto params = GroomParameters(this->project_);

  auto path = subject->get_segmentation_filenames()[0];

  // load the image
  Image image(path);

  // define a groom transform
  auto transform = itk::AffineTransform<double, 3>::New();
  transform->SetIdentity();

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

  // store transform
  std::vector<std::vector<double>> groomed_transforms;
  std::vector<double> groomed_transform;
  auto tform_params = transform->GetParameters();
  for (int i = 0; i < tform_params.size(); i++) {
    groomed_transform.push_back(tform_params[i]);
  }
  groomed_transforms.push_back(groomed_transform);
  subject->set_groomed_transforms(groomed_transforms);

  // isolate
  if (params.get_isolate_tool()) {
    this->isolate(image);
    this->increment_progress();
  }

  // fill holes
  if (params.get_fill_holes_tool()) {
    this->hole_fill(image);
    this->increment_progress();
  }

  // autopad
  if (params.get_auto_pad_tool()) {
    this->auto_pad(image, params.get_padding_amount());
    this->increment_progress();
  }

  // antialias
  if (params.get_antialias_tool()) {
    this->antialias(image, params.get_antialias_iterations());
    this->increment_progress();
  }

  // fastmarching
  if (params.get_fast_marching()) {
    image.computeDT();
    this->increment_progress();
  }

  // blur
  if (params.get_blur_tool()) {
    image.gaussianBlur(params.get_blur_amount());
    this->increment_progress();
  }

  std::string dt_name = path;
  dt_name = dt_name.substr(0, dt_name.find_last_of(".")) + "_DT.nrrd";

  image.write(dt_name);

  // only single domain supported so far
  std::vector<std::string> groomed_filenames{dt_name};
  // store filename back to subject
  subject->set_groomed_filenames(groomed_filenames);
}

//---------------------------------------------------------------------------
void Groom::image_mesh_pipeline(std::shared_ptr<Subject> subject)
{
  auto path = subject->get_segmentation_filenames()[0];

  // load the image
  Mesh mesh(path);

  std::string groom_name = path;
  groom_name = groom_name.substr(0, groom_name.find_last_of(".")) + "_groomed.ply";

  mesh.write(groom_name);
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
void Groom::hole_fill(Image& image)
{
  image.closeHoles();
}

//---------------------------------------------------------------------------
Vector3 Groom::center(Image& image)
{
  auto com = image.centerOfMass();
  auto diff = image.center() - com;
  Vector3 translation;
  translation[0] = diff[0];
  translation[1] = diff[1];
  translation[2] = diff[2];
  image.translate(translation, Image::InterpolationType::NearestNeighbor);

  return translation;
}

//---------------------------------------------------------------------------
void Groom::auto_pad(Image& image, int padding_amount)
{
  image.pad(padding_amount);
}

//---------------------------------------------------------------------------
void Groom::antialias(Image& image, int iterations)
{
  image.antialias(iterations);
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
  num_tools += params.get_fast_marching() ? 1 : 0;
  num_tools += params.get_blur_tool() ? 1 : 0;

  return num_subjects * num_tools;
}

//---------------------------------------------------------------------------
void Groom::increment_progress()
{
  ++this->progress_counter_;
  this->progress_ = static_cast<float>(this->progress_counter_)
                    / static_cast<float>(this->total_ops_);
}


