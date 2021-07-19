
#include <vector>

// qt
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QXmlStreamWriter>
#include <QProgressDialog>

#include <Libs/Mesh/MeshUtils.h>
#include <Libs/Utils/StringUtils.h>

#ifdef _WIN32
  #include <direct.h>
  #define chdir _chdir
#else
  #include <unistd.h>
#endif

#include <tinyxml.h>

#include <Libs/Project/Project.h>

#include <Data/Session.h>
#include <Data/StudioLog.h>
#include <Data/Shape.h>
#include <Data/MeshManager.h>
#include <Visualization/Visualizer.h>

namespace shapeworks {

const std::string Session::DATA_C("data");
const std::string Session::GROOM_C("groom");
const std::string Session::OPTIMIZE_C("optimize");
const std::string Session::ANALYSIS_C("analysis");

//---------------------------------------------------------------------------
Session::Session(QWidget* parent, Preferences& prefs) : parent_(parent),
                                                        preferences_(prefs),
                                                        mesh_manager_(QSharedPointer<MeshManager>(
                                                          new MeshManager(preferences_)))
{
  this->parent_ = nullptr;
  connect(this->mesh_manager_.data(), &MeshManager::new_mesh, this, &Session::handle_new_mesh);
}

//---------------------------------------------------------------------------
Session::~Session() = default;

//---------------------------------------------------------------------------
void Session::handle_new_mesh()
{
  emit new_mesh();
}

//---------------------------------------------------------------------------
void Session::handle_message(QString s)
{
  emit message(s);
}

//---------------------------------------------------------------------------
void Session::handle_thread_complete()
{
  emit message("Reconstruction initialization complete.");
  this->calculate_reconstructed_samples();
  emit update_display();
}

//---------------------------------------------------------------------------
void Session::handle_clear_cache()
{
  this->mesh_manager_->clear_cache();

  for (auto& s : this->shapes_) {
    s->clear_reconstructed_mesh();
  }

  this->calculate_reconstructed_samples();
}

//---------------------------------------------------------------------------
void Session::calculate_reconstructed_samples()
{
  if (!this->project_->get_particles_present()) {
    return;
  }
  //this->preferences_.set_preference("Studio/cache_enabled", false);
  for (auto shape : this->shapes_) {
    auto pts = shape->get_local_correspondence_points();
    if (!pts.empty()) {
      /// TODO: fix
      //shape->set_reconstructed_mesh(this->mesh_manager_->get_mesh(pts));
    }
  }
  //this->preferences_.set_preference("Studio/cache_enabled", true);
}

//---------------------------------------------------------------------------
void Session::set_parent(QWidget* parent)
{
  this->parent_ = parent;
}

//---------------------------------------------------------------------------
bool Session::save_project(std::string fname)
{
  QString filename = QString::fromStdString(fname);
  if (filename == "") {
    filename = this->filename_;
  }
  this->filename_ = QFileInfo(filename).absoluteFilePath();

  QFileInfo fi(filename);
  if (fi.exists()) {
    if (!fi.isWritable()) {
      QMessageBox::warning(nullptr, "Read only", "The file is in read only mode");
      return false;
    }
  }
  else {
    // open file
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::warning(0, "Read only", "The file is in read only mode");
      return false;
    }
  }

  this->set_project_path(QFileInfo(filename).absolutePath());

  QProgressDialog progress("Saving Project...", "Abort", 0, 100, this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(2000);
  progress.show();

  this->preferences_.set_saved();

  progress.setValue(5);
  QApplication::processEvents();

  // original files
  if (this->original_present()) {
    std::vector<std::string> original_list;
    for (int i = 0; i < this->shapes_.size(); i++) {
      original_list.push_back(this->shapes_[i]->get_original_filename_with_path().toStdString());
    }
    //this->session_->set_original_files(original_list);
  }

  // correspondence points
  if (this->unsaved_particle_files_ && this->particles_present()) {
    for (int i = 0; i < this->shapes_.size(); i++) {
      auto local_files = this->shapes_[i]->get_subject()->get_local_particle_filenames();
      auto world_files = this->shapes_[i]->get_subject()->get_world_particle_filenames();
      auto particles = this->shapes_[i]->get_particles();
      for (int i = 0; i < local_files.size(); i++) {
        this->save_particles_file(local_files[i], particles.get_local_particles(i));
        this->save_particles_file(world_files[i], particles.get_world_particles(i));
      }
    }
    this->unsaved_particle_files_ = false;
  }

  this->project_->set_parameters(Parameters::STUDIO_PARAMS, this->params_);

  this->project_->save(filename.toStdString());
  progress.setValue(100);
  return true;
}

//---------------------------------------------------------------------------
void Session::save_particles_file(std::string filename, const vnl_vector<double>& points)
{
  std::ofstream out(filename);
  size_t newline = 1;
  for (auto& a : points) {
    out << a << (newline % 3 == 0 ? "\n" : "    ");
    newline++;
  }
  out.close();
}

//---------------------------------------------------------------------------
bool Session::load_project(QString filename)
{
  if (!QFile::exists(filename)) {
    QMessageBox::critical(nullptr, "ShapeWorksStudio", "File does not exist: " + filename,
                          QMessageBox::Ok);
    return false;
  }

  // clear the project out first
  this->filename_ = QFileInfo(filename).absoluteFilePath();

  if (filename.toLower().endsWith(".xlsx")) {
    return this->load_xl_project(filename);
  }

  // open and parse XML
  TiXmlDocument doc(filename.toStdString().c_str());
  bool loadOkay = doc.LoadFile();
  if (!loadOkay) {
    QString message = "Error: Invalid parameter file: " + filename;
    QMessageBox::critical(nullptr, "ShapeWorksStudio", message, QMessageBox::Ok);
    return false;
  }

  TiXmlHandle docHandle(&doc);

  TiXmlElement* project_element = docHandle.FirstChild("project").Element();

  bool is_project = project_element != nullptr;

  if (!is_project) {
    return this->load_light_project(filename);
  }

  QString message =
    "Error: This version of ShapeWorksStudio only reads XLSX and legacy XML files: " + filename;
  QMessageBox::critical(nullptr, "ShapeWorksStudio", message, QMessageBox::Ok);
  return false;
}

//---------------------------------------------------------------------------
bool Session::load_light_project(QString filename)
{
  std::cerr << "Loading old XML parameter file...\n";
  this->is_light_project_ = true;

  // open and parse XML
  TiXmlDocument doc(filename.toStdString().c_str());
  bool loadOkay = doc.LoadFile();
  if (!loadOkay) {
    QString message = "Error: Invalid parameter file" + filename;
    QMessageBox::critical(nullptr, "ShapeWorksStudio", message, QMessageBox::Ok);
    return false;
  }

  TiXmlHandle docHandle(&doc);
  std::istringstream inputsBuffer;

  // determine if groups are available
  this->groups_available_ = (docHandle.FirstChild("group_ids").Element() != nullptr);

  int domains_per_shape = 1;
  TiXmlElement* elem = docHandle.FirstChild("domains_per_shape").Element();
  if (elem) { domains_per_shape = atoi(elem->GetText()); }
  //this->project_->get_number_of_domains_per_subject()

  // setup XML
  std::vector<std::string> import_files, groom_files, local_point_files, global_point_files;
  std::string sparseFile, denseFile, goodPtsFile;

  elem = docHandle.FirstChild("point_files").Element();
  if (elem) {
    std::string point_filename;
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> point_filename) {
      local_point_files.push_back(point_filename);
      global_point_files.push_back(point_filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  bool local_particles_found = false;
  elem = docHandle.FirstChild("local_point_files").Element();
  if (elem) {
    local_particles_found = true;
    local_point_files.clear();
    std::string point_filename;
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> point_filename) {
      local_point_files.push_back(point_filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  elem = docHandle.FirstChild("world_point_files").Element();
  if (elem) {
    global_point_files.clear();
    std::string point_filename;
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> point_filename) {
      global_point_files.push_back(point_filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  elem = docHandle.FirstChild("distance_transform_files").Element();
  if (elem && elem->GetText()) {
    if (!local_particles_found) {
      QString message = "Error, distance_transform_files specified, but not local_particle_files";
      STUDIO_LOG_ERROR(message);
      QMessageBox::critical(nullptr, "ShapeWorksStudio", message, QMessageBox::Ok);
      return false;
    }
    groom_files.clear(); // if someone specifies both, prefer the distance transforms
    std::string distance_transform_filename;
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> distance_transform_filename) {
      if (!QFile::exists(QString::fromStdString(distance_transform_filename))) {
        QString message = "File does not exist: " +
                          QString::fromStdString(distance_transform_filename);
        STUDIO_LOG_ERROR(message);
        QMessageBox::critical(nullptr, "ShapeWorksStudio", message, QMessageBox::Ok);
        return false;
      }

      groom_files.push_back(distance_transform_filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  elem = docHandle.FirstChild("mesh_files").Element();
  if (elem && elem->GetText()) {
    if (!local_particles_found) {
      QString message = "Error, mesh_files specified, but not local_particle_files";
      STUDIO_LOG_ERROR(message);
      QMessageBox::critical(nullptr, "ShapeWorksStudio", message, QMessageBox::Ok);
      return false;
    }
    std::string mesh_filename;
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> mesh_filename) {
      if (!QFile::exists(QString::fromStdString(mesh_filename))) {
        QString message = "File does not exist: " + QString::fromStdString(mesh_filename);
        STUDIO_LOG_ERROR(message);
        QMessageBox::critical(nullptr, "ShapeWorksStudio", message, QMessageBox::Ok);
        return false;
      }
      groom_files.push_back(mesh_filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  if (!groom_files.empty()) {
    if (groom_files.size() != local_point_files.size()) {
      QString message = "Error, mismatch in number of distance_transforms and particle files";
      STUDIO_LOG_ERROR(message);
      QMessageBox::critical(nullptr, "ShapeWorksStudio", message, QMessageBox::Ok);
      return false;
    }
  }

  if (local_point_files.size() != global_point_files.size()) {
    QString message = "Error, mismatch in number of local and world particle files";
    STUDIO_LOG_ERROR(message);
    QMessageBox::critical(nullptr, "ShapeWorksStudio", message, QMessageBox::Ok);
    return false;
  }

  if (!this->load_point_files(local_point_files, global_point_files, domains_per_shape)) {
    return false;
  }

  this->load_groomed_files(groom_files, 0.5, domains_per_shape);

  // read group ids
  std::vector<int> group_ids;
  elem = docHandle.FirstChild("group_ids").Element();
  if (elem) {
    inputsBuffer.str(elem->GetText());
    int group_id;
    while (inputsBuffer >> group_id) {
      group_ids.push_back(group_id);
    }

    for (int i = 0; i < this->shapes_.size(); i++) {
      this->shapes_[i]->set_group_id(group_ids[i]);
    }
  }

  this->parameters().set("view_state", Visualizer::MODE_RECONSTRUCTION_C);
  this->parameters().set("tool_state", Session::ANALYSIS_C);

  this->renumber_shapes();

  this->project_->store_subjects();

  std::cerr << "Old XML parameter file loaded\n";
  return true;
}

//---------------------------------------------------------------------------
bool Session::load_xl_project(QString filename)
{
  this->filename_ = QFileInfo(filename).absoluteFilePath();

  this->set_project_path(QFileInfo(filename).absolutePath());

  if (!this->project_->load(QFileInfo(filename).fileName().toStdString())) {
    return false;
  }

  int num_subjects = this->project_->get_number_of_subjects();

  auto subjects = this->project_->get_subjects();

  std::vector<std::string> local_point_files;
  std::vector<std::string> global_point_files;

  for (int i = 0; i < num_subjects; i++) {
    QSharedPointer<Shape> shape = QSharedPointer<Shape>(new Shape());
    shape->set_mesh_manager(this->mesh_manager_);
    shape->set_subject(subjects[i]);

    auto locals = subjects[i]->get_local_particle_filenames();
    auto worlds = subjects[i]->get_world_particle_filenames();

    if (!shape->import_local_point_files(locals)) {
      return false;
    }
    if (!shape->import_global_point_files(worlds)) {
      return false;
    }

    this->shapes_ << shape;
  }

  this->groups_available_ = this->project_->get_group_names().size() > 0;
  this->params_ = this->project_->get_parameters(Parameters::STUDIO_PARAMS);
  return true;
}

//---------------------------------------------------------------------------
void Session::set_project_path(QString relative_path)
{
  std::cerr << "Setting project path to " << relative_path.toStdString() << "\n";

  QDir old_path = QDir(this->project_path_);
  QDir new_path = QDir(relative_path);

  auto subjects = this->project_->get_subjects();
  for (auto subject: subjects) {

    // segmentations
    auto paths = subject->get_segmentation_filenames();
    std::vector<std::string> new_paths;
    for (auto path: paths) {
      auto full_path = old_path.absoluteFilePath(QString::fromStdString(path));
      new_paths.push_back(new_path.relativeFilePath(full_path).toStdString());
    }
    subject->set_segmentation_filenames(new_paths);

    // groomed
    paths = subject->get_groomed_filenames();
    new_paths.clear();
    for (auto path: paths) {
      auto full_path = old_path.absoluteFilePath(QString::fromStdString(path));
      new_paths.push_back(new_path.relativeFilePath(full_path).toStdString());
    }
    subject->set_groomed_filenames(new_paths);

    // local particles
    paths = subject->get_local_particle_filenames();
    new_paths.clear();
    for (auto path: paths) {
      auto full_path = old_path.absoluteFilePath(QString::fromStdString(path));
      new_paths.push_back(new_path.relativeFilePath(full_path).toStdString());
    }
    subject->set_local_particle_filenames(new_paths);

    // world particles
    paths = subject->get_world_particle_filenames();
    new_paths.clear();
    for (auto path: paths) {
      auto full_path = old_path.absoluteFilePath(QString::fromStdString(path));
      new_paths.push_back(new_path.relativeFilePath(full_path).toStdString());
    }
    subject->set_world_particle_filenames(new_paths);

    // features
    auto features = subject->get_feature_filenames();
    std::map<std::string, std::string> new_features;
    for (auto const& x : features) {
      auto full_path = old_path.absoluteFilePath(QString::fromStdString(x.second));
      new_features[x.first] = new_path.relativeFilePath(full_path).toStdString();
    }
    subject->set_feature_filenames(new_features);
  }

  this->project_path_ = relative_path;
  chdir(this->project_path_.toStdString().c_str());
}

//---------------------------------------------------------------------------
std::shared_ptr<Project> Session::get_project()
{
  return this->project_;
}

//---------------------------------------------------------------------------
void Session::load_original_files(std::vector<std::string> filenames)
{
  // rewrite the paths based on the project path
  std::vector<std::string> new_filenames;
  for (int i = 0; i < filenames.size(); i++) {
    QDir dir(".");
    QString new_filename = dir.relativeFilePath(QString::fromStdString(filenames[i]));
    new_filenames.push_back(new_filename.toStdString());
  }
  filenames = new_filenames;

  for (int i = 0; i < filenames.size(); i++) {

    QString filename = QString::fromStdString(filenames[i]);
    if (!QFile::exists(filename)) {
      QString message = "File does not exist: " + filename;
      STUDIO_LOG_MESSAGE(message);
      QMessageBox::critical(NULL, "ShapeWorksStudio", message, QMessageBox::Ok);
      return;
    }

    QSharedPointer<Shape> shape = QSharedPointer<Shape>(new Shape());

    std::shared_ptr<Subject> subject = std::make_shared<Subject>();
    subject->set_number_of_domains(1);
    shape->set_mesh_manager(this->mesh_manager_);
    shape->set_subject(subject);
    this->project_->get_subjects().push_back(subject);
    shape->import_original_image(filenames[i]);

    this->shapes_.push_back(shape);
  }

  this->renumber_shapes();
  this->project_->store_subjects();
  if (filenames.size() > 0) {
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
void
Session::load_groomed_files(std::vector<std::string> file_names, double iso, int domains_per_shape)
{
  assert (file_names.size() % domains_per_shape == 0);
  int num_subjects = file_names.size() / domains_per_shape;
  int counter = 0;
  for (int i = 0; i < num_subjects; i++) {
    if (this->shapes_.size() <= i) {
      auto shape = QSharedPointer<Shape>(new Shape);
      std::shared_ptr<Subject> subject = std::make_shared<Subject>();
      shape->set_mesh_manager(this->mesh_manager_);
      shape->set_subject(subject);
      this->project_->get_subjects().push_back(subject);
      this->shapes_.push_back(shape);
    }

    QStringList list;
    list << QFileInfo(QString::fromStdString(file_names[counter])).fileName();
    list << "";
    list << "";
    list << "";
    this->shapes_[i]->set_annotations(list);

    std::vector<std::string> groomed_filenames;
    for (int j = 0; j < domains_per_shape; j++) {
      groomed_filenames.push_back(file_names[counter + j]);
    }
    counter += domains_per_shape;

    this->shapes_[i]->get_subject()->set_groomed_filenames(groomed_filenames);
  }

  this->project_->store_subjects();
  if (file_names.size() > 0) {
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
bool Session::load_point_files(std::vector<std::string> local, std::vector<std::string> world,
                               int domains_per_shape)
{
  assert (local.size() % domains_per_shape == 0);
  int num_subjects = local.size() / domains_per_shape;
  int counter = 0;
  for (int i = 0; i < num_subjects; i++) {
    if (this->shapes_.size() <= i) {
      auto shape = QSharedPointer<Shape>(new Shape);
      std::shared_ptr<Subject> subject = std::make_shared<Subject>();
      subject->set_number_of_domains(domains_per_shape);
      shape->set_mesh_manager(this->mesh_manager_);
      shape->set_subject(subject);
      this->project_->get_subjects().push_back(subject);
      this->shapes_.push_back(shape);
    }

    QStringList list;
    list << QFileInfo(QString::fromStdString(world[counter])).fileName();
    list << "";
    list << "";
    list << "";

    std::vector<std::string> local_filenames;
    std::vector<std::string> world_filenames;
    // only single domain supported so far
    for (int j = 0; j < domains_per_shape; j++) {
      local_filenames.push_back(local[counter + j]);
      world_filenames.push_back(world[counter + j]);
    }
    counter += domains_per_shape;
    this->shapes_[i]->import_local_point_files(local_filenames);
    this->shapes_[i]->import_global_point_files(world_filenames);
    this->shapes_[i]->set_annotations(list);
  }

  this->project_->store_subjects();
  if (local.size() > 0) {
    emit data_changed();
  }

  return true;
}

//---------------------------------------------------------------------------
bool Session::update_particles(std::vector<StudioParticles> particles)
{
  for (int i = 0; i < particles.size(); i++) {
    QSharedPointer<Shape> shape;
    if (this->shapes_.size() > i) {
      shape = this->shapes_[i];
    }
    else {
      shape = QSharedPointer<Shape>(new Shape);
      std::shared_ptr<Subject> subject = std::make_shared<Subject>();
      shape->set_mesh_manager(this->mesh_manager_);
      shape->set_subject(subject);
      this->project_->get_subjects().push_back(subject);
      this->shapes_.push_back(shape);
    }
    shape->set_particles(particles[i]);
  }

  this->unsaved_particle_files_ = true;
  emit points_changed();
  return true;
}

//---------------------------------------------------------------------------
double Session::update_auto_glyph_size()
{
  this->auto_glyph_size_ = 1;
  if (this->shapes_.empty()) {
    return this->auto_glyph_size_;
  }

  double max_range = std::numeric_limits<double>::min();
  int num_particles = 0;
  for (auto& shape : this->shapes_) {
    vnl_vector<double> points = shape->get_global_correspondence_points();
    if (points.empty()) {
      return this->auto_glyph_size_;
    }
    num_particles = points.size() / 3;
    double max_x = std::numeric_limits<double>::min();
    double min_x = std::numeric_limits<double>::max();
    double max_y = max_x;
    double min_y = min_x;
    double max_z = max_x;
    double min_z = min_x;
    for (int i = 0; i < points.size() / 3; i++) {
      Point3 p1 = Session::get_point(points, i);
      max_x = std::max<double>(max_x, p1[0]);
      min_x = std::min<double>(min_x, p1[0]);
      max_y = std::max<double>(max_y, p1[1]);
      min_y = std::min<double>(min_y, p1[1]);
      max_z = std::max<double>(max_z, p1[2]);
      min_z = std::min<double>(min_z, p1[2]);
    }

    double range_x = max_x - min_x;
    double range_y = max_y - min_y;
    double range_z = max_z - min_z;

    max_range = std::max<double>({max_range, range_x, range_y, range_z});
  }

  this->auto_glyph_size_ = max_range / std::sqrt(static_cast<double>(num_particles)) / 2;
  this->auto_glyph_size_ = std::max<double>(0.1, this->auto_glyph_size_);
  this->auto_glyph_size_ = std::min<double>(10.0, this->auto_glyph_size_);

  return this->auto_glyph_size_;
}

//---------------------------------------------------------------------------
bool Session::is_light_project()
{
  return this->is_light_project_;
}

//---------------------------------------------------------------------------
bool Session::get_groomed_present()
{
  return this->project_->get_groomed_present();
}

//---------------------------------------------------------------------------
QVector<QSharedPointer<Shape>> Session::get_shapes()
{
  return this->shapes_;
}

//---------------------------------------------------------------------------
void Session::remove_shapes(QList<int> list)
{
  std::sort(list.begin(), list.end(), std::greater<>());
    foreach(int i, list) {
      std::vector<std::shared_ptr<Subject>>& subjects = this->project_->get_subjects();
      subjects.erase(subjects.begin() + i);
      this->shapes_.erase(this->shapes_.begin() + i);
    }

  this->project_->get_subjects();
  this->renumber_shapes();
  this->project_->store_subjects();
  emit data_changed();
}

//---------------------------------------------------------------------------
bool Session::original_present()
{
  return this->project_->get_segmentations_present();
}

//---------------------------------------------------------------------------
bool Session::groomed_present()
{
  return this->project_->get_groomed_present();
}

//---------------------------------------------------------------------------
bool Session::particles_present()
{
  if (!this->project_->get_particles_present()) {
    return false;
  }

  if (this->shapes_.size() > 0) {
    auto shape = this->shapes_[0];
    return (shape->get_global_correspondence_points().size() > 0);
  }

  return true;
}

//---------------------------------------------------------------------------
bool Session::groups_available()
{
  return this->groups_available_;
}

//---------------------------------------------------------------------------
void Session::renumber_shapes()
{
  for (int i = 0; i < this->shapes_.size(); i++) {
    this->shapes_[i]->set_id(i + 1);
  }
}

//---------------------------------------------------------------------------
QString Session::get_filename()
{
  return this->filename_;
}

//---------------------------------------------------------------------------
int Session::get_num_shapes()
{
  return this->shapes_.size();
}

//---------------------------------------------------------------------------
int Session::get_domains_per_shape()
{
  return this->get_project()->get_number_of_domains_per_subject();
}

//---------------------------------------------------------------------------
Parameters& Session::parameters()
{
  return this->params_;
}

//---------------------------------------------------------------------------
QString Session::get_display_name()
{
  if (this->filename_ == "") {
    return "New Project";
  }

  QFileInfo fi(this->filename_);
  QString name = fi.baseName();
  if (!fi.isWritable()) {
    name = name + " (read-only)";
  }

  return name;
}

//---------------------------------------------------------------------------
std::string Session::get_default_feature_map()
{
  if (!this->get_project()->get_subjects().empty()) {
    auto subject = this->get_project()->get_subjects()[0];
    if (!subject->get_segmentation_filenames().empty()) {
      if (subject->get_domain_types()[0] == DomainType::Mesh) {
        Mesh m = MeshUtils::threadSafeReadMesh(subject->get_segmentation_filenames()[0]);
        auto poly_data = m.getVTKMesh();
        if (poly_data) {
          auto scalars = poly_data->GetPointData()->GetScalars();
          if (scalars) {
            return scalars->GetName();
          }
        }
      }
    }
  }
  return "";
}

//---------------------------------------------------------------------------
bool Session::is_supported_file_format(std::string filename)
{
  for (auto type : Mesh::getSupportedTypes()) {
    if (StringUtils::hasSuffix(filename, type)) {
      return true;
    }
  }

  if (StringUtils::hasSuffix(filename, ".nrrd")
      || StringUtils::hasSuffix(filename, ".mha")
      || StringUtils::hasSuffix(filename, ".nii")
      || StringUtils::hasSuffix(filename, ".nii.gz")) {
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------
std::vector<DomainType> Session::get_domain_types()
{
  auto subjects = this->get_project()->get_subjects();
  if (subjects.size() > 0 && subjects[0]->get_domain_types().size() > 0) {
    return subjects[0]->get_domain_types();
  }
  return std::vector<DomainType>();
}

//---------------------------------------------------------------------------
Point3 Session::get_point(const vnl_vector<double>& points, int i)
{
  if ((i * 3) + 2 > points.size() - 1) {
    return Point3();
  }
  int pos = i * 3;
  Point3 point;
  point[0] = points[pos];
  point[1] = points[pos + 1];
  point[2] = points[pos + 2];
  return point;
}

//---------------------------------------------------------------------------
double Session::get_auto_glyph_size()
{
  return this->auto_glyph_size_;
}

//---------------------------------------------------------------------------
void Session::clear_particles()
{
  std::vector<StudioParticles> particles(this->get_num_shapes());
  this->update_particles(particles);
}

}
