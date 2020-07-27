#include <Data/Shape.h>

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

#include <itkOrientImageFilter.h>

#include <vtkCenterOfMass.h>

#include <Data/MeshGenerator.h>
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
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_original_mesh()
{
  if (!this->original_mesh_) {
    this->generate_meshes(this->subject_->get_segmentation_filenames(), this->original_mesh_);
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
    } catch (itk::ExceptionObject & excep) {
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
        reader->SetFileName(filename);
        reader->Update();
        image = reader->GetOutput();
        // don't store to this->groomed_image_ so that we don't hold a pointer to it
      } catch (itk::ExceptionObject & excep) {
        std::cerr << "Exception caught!" << std::endl;
        std::cerr << excep << std::endl;
      }
      return image;
    }
  }
  return this->groomed_image_;
}

//---------------------------------------------------------------------------
void Shape::import_groomed_image(ImageType::Pointer img, double iso)
{
  this->groomed_mesh_ = QSharedPointer<Mesh>(new Mesh());
  this->groomed_image_ = img;
  this->groomed_mesh_->create_from_image(img, iso);
  auto name = this->get_original_filename_with_path().toStdString();
  name = name.substr(0, name.find_last_of(".")) + "_DT.nrrd";
  this->groomed_filename_ = QString::fromStdString(name);
  std::vector<std::string> groomed_filenames {name};   // only single domain supported so far
  this->subject_->set_groomed_filenames(groomed_filenames);
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_groomed_mesh()
{
  if (!this->groomed_mesh_) {

    if (!this->subject_) {
      std::cerr << "Error: asked for groom mesh when none as present!\n";
      //return this->groomed_mesh_;
    }
    else {
      this->generate_meshes(this->subject_->get_groomed_filenames(), this->groomed_mesh_);
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
void Shape::set_global_particles(const vnl_vector<double> &points)
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
  for (auto &a : points) {
    double x = static_cast<double>(a[0]);
    double y = static_cast<double>(a[1]);
    double z = static_cast<double>(a[2]);

    vtk_points->InsertNextPoint(x, y, z);
    num_points++;
  }
  auto & point_list = local ? this->local_correspondence_points_ :
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
void Shape::set_transform(const vnl_vector<double> &transform)
{
  this->transform_ = transform;
}

//---------------------------------------------------------------------------
vnl_vector<double> Shape::get_transform()
{
  return this->transform_;
}

//---------------------------------------------------------------------------
void Shape::generate_original_meshes()
{
  if (this->subject_->get_segmentation_filenames().size() > 0) {
    std::string filename = this->subject_->get_segmentation_filenames()[0];

    MeshWorkItem item;
    item.filename = filename;
    MeshHandle mesh = this->mesh_manager_->get_mesh(item);
    if (mesh) {
      //std::cerr << "mesh was ready from manager!\n";
      this->original_mesh_ = mesh;

      /// Temporarily calculate it here
      auto com = vtkSmartPointer<vtkCenterOfMass>::New();
      com->SetInputData(mesh->get_poly_data());
      com->Update();
      double center[3];
      com->GetCenter(center);

      this->transform_.set_size(3);
      for (unsigned int i = 0; i < 3; i++) {
        this->transform_[i] = center[i];
      }

//      this->set_transform(this->original_mesh_->get_center_transform());
    }
    else {
      //std::cerr << "no mesh yet from manager!\n";
    }
  }
}

//---------------------------------------------------------------------------
void Shape::generate_meshes(std::vector<string> filenames, QSharedPointer<Mesh> &mesh)
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
    //std::cerr << "mesh was ready from manager!\n";
    mesh = new_mesh;

    /// Temporarily calculate it here
    auto com = vtkSmartPointer<vtkCenterOfMass>::New();
    com->SetInputData(mesh->get_poly_data());
    com->Update();
    double center[3];
    com->GetCenter(center);

    this->transform_.set_size(3);
    for (unsigned int i = 0; i < 3; i++) {
      this->transform_[i] = center[i];
    }
  }
}

//---------------------------------------------------------------------------
bool Shape::import_point_file(QString filename, vnl_vector<double> &points)
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
    if (!in.good()) {break;}
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
  vtkSmartPointer<vtkPolyData> poly_data = mesh->get_poly_data();

  std::cerr << "checking if mesh has scalar array for " << feature << "\n";
  auto scalar_array = poly_data->GetPointData()->GetArray(feature.c_str());
  if (scalar_array) {
    std::cerr << "shape: array present!\n";
  }
  else {
    std::cerr << "shape: array NOT present! Loading...\n";

    auto filenames = this->subject_->get_feature_filenames();

    mesh->apply_feature_map(feature, filenames[feature]);

  }


}
