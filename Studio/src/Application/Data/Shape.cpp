#include <Data/Shape.h>

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

//---------------------------------------------------------------------------
Shape::Shape()
{
  this->id_ = 0;
}

//---------------------------------------------------------------------------
Shape::~Shape()
{}

//---------------------------------------------------------------------------
void Shape::import_original_image(std::string filename, float iso_value)
{
  this->original_mesh_ = QSharedPointer<Mesh>(new Mesh());
  this->original_image_ = this->original_mesh_->create_from_file(filename, iso_value);
  this->original_mesh_filename_ = QString::fromStdString(filename);
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_original_mesh()
{
  return this->original_mesh_;
}

//---------------------------------------------------------------------------
ImageType::Pointer Shape::get_original_image()
{
  return this->original_image_;
}

//---------------------------------------------------------------------------
ImageType::Pointer Shape::get_groomed_image()
{
  return this->groomed_image_;
}

//---------------------------------------------------------------------------
void Shape::import_groomed_file(QString filename, double iso)
{
  this->groomed_mesh_ = QSharedPointer<Mesh>(new Mesh());
  this->groomed_image_ = this->groomed_mesh_->create_from_file(filename.toStdString(), iso);
  this->groomed_filename_ = filename;
}

//---------------------------------------------------------------------------
void Shape::import_groomed_image(ImageType::Pointer img, double iso)
{
  this->groomed_mesh_ = QSharedPointer<Mesh>(new Mesh());
  this->groomed_image_ = img;
  this->groomed_mesh_->create_from_image(img, iso);
  auto name = this->original_mesh_filename_.toStdString();
  name = name.substr(0, name.find_last_of(".")) + "_DT.nrrd";
  this->groomed_filename_ = QString::fromStdString(name);
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_groomed_mesh()
{
  return this->groomed_mesh_;
}

//---------------------------------------------------------------------------
void Shape::set_reconstructed_mesh(vtkSmartPointer<vtkPolyData> poly_data)
{
  if (poly_data) {
    this->reconstructed_mesh_ = QSharedPointer<Mesh> (new Mesh());
    this->reconstructed_mesh_->set_poly_data(poly_data);
  }
}

//---------------------------------------------------------------------------
bool Shape::import_global_point_file(QString filename)
{

  if (!Shape::import_point_file(filename, this->global_correspondence_points_)) {
    return false;
  }

  this->global_point_filename_ = filename;

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
  return true;
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> Shape::get_reconstructed_mesh()
{
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
  QFileInfo qfi(this->original_mesh_filename_);
  return qfi.fileName();
}

//---------------------------------------------------------------------------
QString Shape::get_original_filename_with_path()
{
  return this->original_mesh_filename_;
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
