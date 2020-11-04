#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>

#include <Libs/Image/Image.h>

#include <ShapeWorksGroom2.h>
#include <GroomParameters.h>

#include <utility>
#include <vector>

#include "bounding_box.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkCastImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkThresholdImageFilter.h"

using namespace shapeworks;

typedef float PixelType;
typedef itk::Image<PixelType, 3> ImageType;

//---------------------------------------------------------------------------
ShapeWorksGroom2::ShapeWorksGroom2(shapeworks::ProjectHandle project)
{
  this->project_ = project;
}

//---------------------------------------------------------------------------
void ShapeWorksGroom2::run()
{
  this->progress_ = 0;
  this->progress_counter_ = 0;
  this->total_ops_ = this->get_total_ops();

  auto subjects = this->project_->get_subjects();

  auto params = GroomParameters(this->project_);

  tbb::parallel_for(
    tbb::blocked_range<size_t>{0, subjects.size()},
    [&](const tbb::blocked_range<size_t>& r) {
      for (size_t i = r.begin(); i < r.end(); ++i) {

        auto path = subjects[i]->get_segmentation_filenames()[0];

        Image image(path);

        if (params.get_center_tool()) {
          this->center(image);
          this->increment_progress();
        }

        if (params.get_isolate_tool()) {
          this->isolate(image);
          this->increment_progress();
        }

        if (params.get_fill_holes_tool()) {
          this->hole_fill(image);
          this->increment_progress();
        }

        if (params.get_auto_pad_tool()) {
          this->auto_pad(image, params.get_padding_amount());
          this->increment_progress();
        }

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
        subjects[i]->set_groomed_filenames(groomed_filenames);


        //this->center(static_cast<int>(i));
      }
    });
  this->project_->store_subjects();

}

//---------------------------------------------------------------------------
void ShapeWorksGroom2::isolate(Image& image)
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

  typedef itk::RelabelComponentImageFilter<IsolateType, IsolateType> RelabelType;
  auto relabel = RelabelType::New();
  relabel->SetInput(cc_filter->GetOutput());
  relabel->SortByObjectSizeOn();
  relabel->Update();

  typedef itk::ThresholdImageFilter<IsolateType> ThreshType;
  auto thresh = ThreshType::New();
  thresh->SetInput(relabel->GetOutput());
  thresh->SetOutsideValue(0);
  thresh->ThresholdBelow(0);
  thresh->ThresholdAbove(1);
  thresh->Update();

  typedef itk::CastImageFilter<IsolateType, ImageType> FilterType;
  auto cast_filter = FilterType::New();
  cast_filter->SetInput(thresh->GetOutput());
  cast_filter->Update();

  image = Image(cast_filter->GetOutput());
}

//---------------------------------------------------------------------------
void ShapeWorksGroom2::hole_fill(Image& image)
{
  image.closeHoles();
}

//---------------------------------------------------------------------------
void ShapeWorksGroom2::center(Image& image)
{
  auto com = image.centerOfMass();
  auto diff = image.center() - com;
  Vector3 translation;
  translation[0] = diff[0];
  translation[1] = diff[1];
  translation[2] = diff[2];
  image.translate(translation, Image::InterpolationType::NearestNeighbor);
}

//---------------------------------------------------------------------------
void ShapeWorksGroom2::auto_pad(Image& image, int padding_amount)
{
  image.pad(padding_amount);
}

//---------------------------------------------------------------------------
void ShapeWorksGroom2::antialias(Image& image, int iterations)
{
  image.antialias(iterations);
}

//---------------------------------------------------------------------------
int ShapeWorksGroom2::get_total_ops()
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
void ShapeWorksGroom2::increment_progress()
{
  ++this->progress_counter_;
  this->progress_ = static_cast<float>(this->progress_counter_)
                    / static_cast<float>(this->total_ops_);
}

