#include <Data/Shape.h>

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

#include <itkOrientImageFilter.h>

#include <vtkCenterOfMass.h>

#include <Data/MeshGenerator.h>
#include <Data/StudioLog.h>
#include <Visualization/Visualizer.h>

using namespace shapeworks;

//---------------------------------------------------------------------------
Shape::Shape()
{
  this->id_ = 0;

  this->corner_annotations_ << "";
  this->corner_annotations_ << "";
  this->corner_annotations_ << "";
  this->corner_annotations_ << "";
}

//---------------------------------------------------------------------------
Shape::~Shape()
{}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_mesh(std::string display_mode)
{
  if (display_mode == Visualizer::MODE_ORIGINAL_C) {
    return this->get_original_mesh();
  }
  else if (display_mode == Visualizer::MODE_GROOMED_C) {
    return this->get_groomed_mesh();
  }
  return this->get_reconstructed_mesh();
}

//---------------------------------------------------------------------------
void Shape::set_annotations(QStringList annotations)
{
  this->corner_annotations_ = annotations;
}

//---------------------------------------------------------------------------
QStringList Shape::get_annotations()
{
  return this->corner_annotations_;
}

//---------------------------------------------------------------------------
void Shape::set_mesh_manager(QSharedPointer<MeshManager> mesh_manager)
{
  this->mesh_manager_ = mesh_manager;
}

//---------------------------------------------------------------------------
void Shape::set_subject(std::shared_ptr<Subject> subject)
{
  this->subject_ = subject;

  if (this->subject_->get_segmentation_filenames().size() > 0) {
    std::string filename = this->subject_->get_segmentation_filenames()[0];
    this->corner_annotations_[0] = QString::fromStdString(filename);
  }
}

//---------------------------------------------------------------------------
std::shared_ptr<Subject> Shape::get_subject()
{
  return this->subject_;
}

//---------------------------------------------------------------------------
void Shape::import_original_image(std::string filename, float iso_value)
{
  this->subject_->set_segmentation_filenames(std::vector<std::string>{filename});
  this->corner_annotations_[0] = QString::fromStdString(filename);
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_original_mesh()
{
  if (!this->original_mesh_) {
    if (!this->subject_) {
      // There is no original mesh for a computed shape (e.g. mean)
      std::cerr << "Error: asked for original mesh when none is present!\n";
    }
    else {
      this->generate_meshes(this->subject_->get_segmentation_filenames(), this->original_mesh_,
                            true);
    }
  }
  return this->original_mesh_;
}

//---------------------------------------------------------------------------
ImageType::Pointer Shape::get_original_image()
{
  ImageType::Pointer image;
  std::string filename = this->subject_->get_segmentation_filenames()[0];
  if (filename != "") {
    try {
      // read file using ITK
      ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(filename);
      reader->Update();
      image = reader->GetOutput();

      // set orientation to RAI
      itk::OrientImageFilter<ImageType, ImageType>::Pointer orienter =
        itk::OrientImageFilter<ImageType, ImageType>::New();
      orienter->UseImageDirectionOn();
      orienter->SetDesiredCoordinateOrientation(
        itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
      orienter->SetInput(image);
      orienter->Update();
      image = orienter->GetOutput();
    } catch (itk::ExceptionObject& excep) {
      std::cerr << "Exception caught!" << std::endl;
      std::cerr << excep << std::endl;
    }
  }
  return image;
}

//---------------------------------------------------------------------------
ImageType::Pointer Shape::get_groomed_image()
{
  if (!this->groomed_image_) {
    std::string filename = this->subject_->get_groomed_filenames()[0]; // single domain supported
    if (filename != "") {
      ImageType::Pointer image;
      try {
        // read file using ITK
        ReaderType::Pointer reader = ReaderType::New();
        STUDIO_LOG_MESSAGE(QString::fromStdString("Loading groomed file: " + filename));
        reader->SetFileName(filename);
        reader->Update();
        image = reader->GetOutput();
        // don't store to this->groomed_image_ so that we don't hold a pointer to it
      } catch (itk::ExceptionObject& excep) {
        STUDIO_LOG_ERROR(QString::fromStdString("Failed to loading groomed file: " + filename));
        std::cerr << "Exception caught!" << std::endl;
        std::cerr << excep << std::endl;
      }
      return image;
    }
  }
  return this->groomed_image_;
}

//---------------------------------------------------------------------------
void Shape::import_groomed_image(ImageType::Pointer img, double iso, TransformType transform)
{
  this->groomed_mesh_ = QSharedPointer<Mesh>(new Mesh());
  this->groomed_image_ = img;
  this->groomed_mesh_->create_from_image(img, iso);
  this->groomed_transform_ = transform;
  auto name = this->get_original_filename_with_path().toStdString();
  name = name.substr(0, name.find_last_of(".")) + "_DT.nrrd";
  this->groomed_filename_ = QString::fromStdString(name);
  std::vector<std::string> groomed_filenames{name};   // only single domain supported so far
  this->subject_->set_groomed_filenames(groomed_filenames);

  // single domain so far
  std::vector<std::vector<double>> groomed_transforms;
  std::vector<double> groomed_transform;
  for (int i = 0; i < transform.size(); i++) {
    groomed_transform.push_back(transform[i]);
  }
  groomed_transforms.push_back(groomed_transform);
  this->subject_->set_groomed_transforms(groomed_transforms);

}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_groomed_mesh()
{
  if (!this->groomed_mesh_) {
    if (!this->subject_) {
      std::cerr << "Error: asked for groomed mesh when none is present!\n";
    }
    else {
      this->generate_meshes(this->subject_->get_groomed_filenames(), this->groomed_mesh_, false);
    }
  }

  return this->groomed_mesh_;
}

//---------------------------------------------------------------------------
void Shape::set_reconstructed_mesh(MeshHandle mesh)
{
  this->reconstructed_mesh_ = mesh;
}

//---------------------------------------------------------------------------
void Shape::clear_reconstructed_mesh()
{
  this->reconstructed_mesh_ = nullptr;
}

//---------------------------------------------------------------------------
void Shape::set_global_particles(const vnl_vector<double>& points)
{
  this->global_correspondence_points_ = points;
}

//---------------------------------------------------------------------------
bool Shape::import_global_point_file(QString filename)
{

  if (!Shape::import_point_file(filename, this->global_correspondence_points_)) {
    return false;
  }

  this->global_point_filename_ = filename;

  this->subject_->set_global_particle_filename(filename.toStdString());

  return true;
}

//---------------------------------------------------------------------------
bool Shape::import_points(std::vector<itk::Point<double>> points, bool local)
{
  vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();
  size_t num_points = 0;
  for (auto& a : points) {
    double x = static_cast<double>(a[0]);
    double y = static_cast<double>(a[1]);
    double z = static_cast<double>(a[2]);

    vtk_points->InsertNextPoint(x, y, z);
    num_points++;
  }
  auto& point_list = local ? this->local_correspondence_points_ :
                     this->global_correspondence_points_;

  point_list.clear();
  point_list.set_size(num_points * 3);

  int idx = 0;
  for (int i = 0; i < num_points; i++) {
    double* pos = vtk_points->GetPoint(i);
    point_list[idx++] = pos[0];
    point_list[idx++] = pos[1];
    point_list[idx++] = pos[2];
  }
  return true;
}

//---------------------------------------------------------------------------
bool Shape::import_local_point_file(QString filename)
{
  if (!Shape::import_point_file(filename, this->local_correspondence_points_)) {
    return false;
  }

  this->local_point_filename_ = filename;

  this->subject_->set_local_particle_filename(filename.toStdString());

  return true;
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_reconstructed_mesh()
{
  if (!this->reconstructed_mesh_) {
    this->reconstructed_mesh_ = this->mesh_manager_->get_mesh(this->global_correspondence_points_);
  }

  return this->reconstructed_mesh_;
}

//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_global_correspondence_points()
{
  return this->global_correspondence_points_;
}

//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_local_correspondence_points()
{
  return this->local_correspondence_points_;
}

//---------------------------------------------------------------------------
int Shape::get_id()
{
  return this->id_;
}

//---------------------------------------------------------------------------
void Shape::set_id(int id)
{
  this->id_ = id;
}

//---------------------------------------------------------------------------
QString Shape::get_original_filename()
{
  if (this->subject_->get_segmentation_filenames().size() < 1) {
    return "";
  }
  auto string = QString::fromStdString(this->subject_->get_segmentation_filenames()[0]);
  QFileInfo info(string);
  return info.fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_original_filename_with_path()
{
  if (this->subject_->get_segmentation_filenames().size() < 1) {
    return "";
  }
  return QString::fromStdString(this->subject_->get_segmentation_filenames()[0]);
}

//---------------------------------------------------------------------------
QString Shape::get_groomed_filename()
{
  QFileInfo qfi(this->groomed_filename_);
  return qfi.fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_groomed_filename_with_path()
{
  return this->groomed_filename_;
}

//---------------------------------------------------------------------------
QString Shape::get_global_point_filename()
{
  return QFileInfo(this->global_point_filename_).fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_global_point_filename_with_path()
{
  return this->global_point_filename_;
}

//---------------------------------------------------------------------------
QString Shape::get_local_point_filename()
{
  return QFileInfo(this->local_point_filename_).fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_local_point_filename_with_path()
{
  return this->local_point_filename_;
}

//---------------------------------------------------------------------------
QList<Point> Shape::get_exclusion_sphere_centers()
{
  return this->exclusion_sphere_centers_;
}

//---------------------------------------------------------------------------
void Shape::set_exclusion_sphere_centers(QList<Point> centers)
{
  this->exclusion_sphere_centers_ = centers;
}

//---------------------------------------------------------------------------
QList<double> Shape::get_exclusion_sphere_radii()
{
  return this->exclusion_sphere_radii_;
}

//---------------------------------------------------------------------------
void Shape::set_exclusion_sphere_radii(QList<double> radii)
{
  this->exclusion_sphere_radii_ = radii;
}

//---------------------------------------------------------------------------
int Shape::get_group_id()
{
  return this->group_id_;
}

//---------------------------------------------------------------------------
void Shape::set_group_id(int id)
{
  if (this->subject_) {
    this->subject_->set_group_values({{"group", std::to_string(id)}});
  }
  this->group_id_ = id;
}

//---------------------------------------------------------------------------
std::vector<Point> Shape::get_vectors()
{
  return this->vectors_;
}

//---------------------------------------------------------------------------
void Shape::set_vectors(std::vector<Point> vectors)
{
  this->vectors_ = vectors;
}

//---------------------------------------------------------------------------
void Shape::set_transform(const vnl_vector<double>& transform)
{
  this->transform_ = transform;
}

//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_transform()
{
  return this->transform_;
}

//---------------------------------------------------------------------------
void Shape::generate_meshes(std::vector<string> filenames, QSharedPointer<Mesh>& mesh,
                            bool save_transform)
{
  if (filenames.size() < 1) {
    return;
  }

  // single domain supported right now
  std::string filename = filenames[0];

  MeshWorkItem item;
  item.filename = filename;
  MeshHandle new_mesh = this->mesh_manager_->get_mesh(item);
  if (new_mesh) {
    mesh = new_mesh;

    /// Temporarily calculate the COM here
    auto com = vtkSmartPointer<vtkCenterOfMass>::New();
    com->SetInputData(mesh->get_poly_data());
    com->Update();
    double center[3];
    com->GetCenter(center);

    if (save_transform) {
      this->transform_.set_size(12);
      for (unsigned int i = 0; i < 3; i++) {
        this->transform_[9 + i] = center[i];
      }
    }
  }
}

//---------------------------------------------------------------------------
bool Shape::import_point_file(QString filename, vnl_vector<double>& points)
{
  std::ifstream in(filename.toStdString().c_str());
  if (!in.good()) {
    QMessageBox::warning(0, "Unable to open file", "Error opening file: " + filename);
    return false;
  }
  vtkSmartPointer<vtkPoints> vtk_points = vtkSmartPointer<vtkPoints>::New();
  auto test = filename.toStdString();
  int num_points = 0;
  while (in.good()) {
    double x, y, z;
    in >> x >> y >> z;
    if (!in.good()) { break; }
    vtk_points->InsertNextPoint(x, y, z);
    num_points++;
  }
  in.close();
  points.clear();
  points.set_size(num_points * 3);

  int idx = 0;
  for (int i = 0; i < num_points; i++) {
    double* pos = vtk_points->GetPoint(i);
    points[idx++] = pos[0];
    points[idx++] = pos[1];
    points[idx++] = pos[2];
  }
  return true;
}

//---------------------------------------------------------------------------
void Shape::load_feature(std::string display_mode, std::string feature)
{
  auto mesh = this->get_mesh(display_mode);
  if (!mesh) {
    // not ready yet
    return;
  }
  vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();

  auto scalar_array = poly_data->GetPointData()->GetArray(feature.c_str());
  if (!scalar_array) {

    if (!this->subject_) {
      return;
    }

    auto filenames = this->subject_->get_feature_filenames();

    vnl_vector<double> transform;
    if (display_mode != Visualizer::MODE_ORIGINAL_C) {
      transform = this->get_groomed_transform();
    }

    // read the feature
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filenames[feature]);
    reader->Update();
    ImageType::Pointer image = reader->GetOutput();

    mesh->apply_feature_map(feature, image, transform);

    this->apply_feature_to_points(feature, image);
  }
}

//---------------------------------------------------------------------------
void Shape::apply_feature_to_points(std::string feature, ImageType::Pointer image)
{
  using LinearInterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;

  LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
  interpolator->SetInputImage(image);

  auto region = image->GetLargestPossibleRegion();

  vnl_vector<double> transform = this->get_groomed_transform();

  int num_points = this->local_correspondence_points_.size() / 3;

  Eigen::VectorXf values(num_points);

  int idx = 0;
  for (int i = 0; i < num_points; ++i) {

    ImageType::PointType pitk;
    pitk[0] = this->local_correspondence_points_[idx++];
    pitk[1] = this->local_correspondence_points_[idx++];
    pitk[2] = this->local_correspondence_points_[idx++];

    if (transform.size() == 12) {
      pitk[0] = pitk[0] + transform[9];
      pitk[1] = pitk[1] + transform[10];
      pitk[2] = pitk[2] + transform[11];
    }

    LinearInterpolatorType::ContinuousIndexType index;
    image->TransformPhysicalPointToContinuousIndex(pitk, index);

    auto pixel = 0;
    if (region.IsInside(index)) {
      pixel = interpolator->EvaluateAtContinuousIndex(index);
    }

    values[i] = pixel;
  }
  this->point_features_[feature] = values;

}

//---------------------------------------------------------------------------
Eigen::VectorXf Shape::get_point_features(std::string feature)
{
  auto it = this->point_features_.find(feature);
  if (it == this->point_features_.end()) {
    return Eigen::VectorXf();
  }

  return it->second;
}

//---------------------------------------------------------------------------
void Shape::set_point_features(std::string feature, Eigen::VectorXf values)
{
  this->point_features_[feature] = values;

  auto mesh = this->get_mesh(Visualizer::MODE_RECONSTRUCTION_C);

  if (mesh) {
    mesh->interpolate_scalars_to_mesh(feature, this->global_correspondence_points_, values);
  }
}

//---------------------------------------------------------------------------
TransformType Shape::get_groomed_transform()
{
  if (this->groomed_transform_.size() == 0) {
    // single domain support
    auto transforms = this->subject_->get_groomed_transforms();
    if (transforms.size() > 0) {
      this->groomed_transform_.set_size(transforms[0].size());
      for (int i = 0; i < transforms[0].size(); i++) {
        this->groomed_transform_[i] = transforms[0][i];
      }
    }
  }
  return this->groomed_transform_;
}
