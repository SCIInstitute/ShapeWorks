#include <vector>

// qt
#include <Libs/Mesh/MeshUtils.h>
#include <Libs/Utils/StringUtils.h>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QProgressDialog>
#include <QXmlStreamWriter>

#ifdef _WIN32
#include <direct.h>
#define chdir _chdir
#else
#include <unistd.h>
#endif

#include <Data/MeshManager.h>
#include <Data/Session.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>
#include <Libs/Project/Project.h>
#include <Utils/StudioUtils.h>
#include <Visualization/Visualizer.h>
#include <tinyxml.h>

namespace shapeworks {

const std::string Session::DATA_C("data");
const std::string Session::GROOM_C("groom");
const std::string Session::OPTIMIZE_C("optimize");
const std::string Session::ANALYSIS_C("analysis");
const std::string Session::DEEPSSM_C("deepssm");

//---------------------------------------------------------------------------
Session::Session(QWidget* parent, Preferences& prefs)
    : parent_(parent), preferences_(prefs), mesh_manager_(QSharedPointer<MeshManager>(new MeshManager(preferences_))) {
  this->parent_ = nullptr;
  connect(this->mesh_manager_.data(), &MeshManager::new_mesh, this, &Session::handle_new_mesh);
}

//---------------------------------------------------------------------------
Session::~Session() = default;

//---------------------------------------------------------------------------
void Session::handle_new_mesh() { emit new_mesh(); }

//---------------------------------------------------------------------------
void Session::handle_message(QString s) { emit message(s); }

//---------------------------------------------------------------------------
void Session::handle_thread_complete() {
  emit message("Reconstruction initialization complete.");
  this->calculate_reconstructed_samples();
  emit update_display();
}

//---------------------------------------------------------------------------
void Session::handle_clear_cache() {
  this->mesh_manager_->clear_cache();

  for (auto& s : this->shapes_) {
    s->clear_reconstructed_mesh();
  }

  this->calculate_reconstructed_samples();
}

//---------------------------------------------------------------------------
void Session::calculate_reconstructed_samples() {
  if (!this->project_->get_particles_present()) {
    return;
  }
  // this->preferences_.set_preference("Studio/cache_enabled", false);
  for (auto shape : qAsConst(this->shapes_)) {
    auto pts = shape->get_local_correspondence_points();
    if (!(pts.size() == 0)) {
      /// TODO: fix
      // shape->set_reconstructed_mesh(this->mesh_manager_->get_mesh(pts));
    }
  }
  // this->preferences_.set_preference("Studio/cache_enabled", true);
}

//---------------------------------------------------------------------------
void Session::set_parent(QWidget* parent) { this->parent_ = parent; }

//---------------------------------------------------------------------------
bool Session::save_project(std::string fname) {
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
  } else {
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
    // this->session_->set_original_files(original_list);
  }

  // landmarks
  if (project_->get_landmarks_present()) {
    for (int i = 0; i < shapes_.size(); i++) {
      shapes_[i]->store_landmarks();
    }
  }

  // correspondence points
  if (this->unsaved_particle_files_ && this->particles_present()) {
    for (int i = 0; i < this->shapes_.size(); i++) {
      auto local_files = this->shapes_[i]->get_subject()->get_local_particle_filenames();
      auto world_files = this->shapes_[i]->get_subject()->get_world_particle_filenames();
      auto particles = this->shapes_[i]->get_particles();
      for (int i = 0; i < local_files.size(); i++) {
        this->save_particles_file(local_files[i], particles.get_local_particles(i));
        this->save_particles_file(world_files[i], particles.get_raw_world_particles(i));
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
void Session::save_particles_file(std::string filename, const Eigen::VectorXd& points) {
  std::ofstream out(filename);
  size_t newline = 1;
  for (int i = 0; i < points.size(); i++) {
    out << points[i] << (newline % 3 == 0 ? "\n" : "    ");
    newline++;
  }
  out.close();
}

//---------------------------------------------------------------------------
bool Session::load_project(QString filename) {
  if (!QFile::exists(filename)) {
    QMessageBox::critical(nullptr, "ShapeWorksStudio", "File does not exist: " + filename, QMessageBox::Ok);
    return false;
  }

  // clear the project out first
  this->filename_ = QFileInfo(filename).absoluteFilePath();

  if (filename.endsWith(".xlsx", Qt::CaseInsensitive)) {
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

  QString message = "Error: This version of ShapeWorksStudio only reads XLSX and legacy XML files: " + filename;
  QMessageBox::critical(nullptr, "ShapeWorksStudio", message, QMessageBox::Ok);
  return false;
}

//---------------------------------------------------------------------------
bool Session::load_light_project(QString filename) {
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
  if (elem) {
    domains_per_shape = atoi(elem->GetText());
  }
  // this->project_->get_number_of_domains_per_subject()

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
    groom_files.clear();  // if someone specifies both, prefer the distance transforms
    std::string distance_transform_filename;
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> distance_transform_filename) {
      if (!QFile::exists(QString::fromStdString(distance_transform_filename))) {
        QString message = "File does not exist: " + QString::fromStdString(distance_transform_filename);
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
bool Session::load_xl_project(QString filename) {
  filename_ = QFileInfo(filename).absoluteFilePath();

  set_project_path(QFileInfo(filename).absolutePath());

  if (!project_->load(QFileInfo(filename).fileName().toStdString())) {
    return false;
  }

  int num_subjects = project_->get_number_of_subjects();

  auto subjects = project_->get_subjects();

  std::vector<std::string> local_point_files;
  std::vector<std::string> global_point_files;

  auto domain_names = project_->get_domain_names();

  // auto landmark_definitions = project_->get_all_landmark_definitions();
  for (int i = 0; i < num_subjects; i++) {
    QSharedPointer<Shape> shape = QSharedPointer<Shape>(new Shape());
    shape->set_mesh_manager(mesh_manager_);
    shape->set_subject(subjects[i]);

    auto locals = subjects[i]->get_local_particle_filenames();
    auto worlds = subjects[i]->get_world_particle_filenames();
    auto landmark_files = subjects[i]->get_landmarks_filenames();

    if (!shape->import_local_point_files(StudioUtils::to_string_list(locals))) {
      return false;
    }
    if (!shape->import_global_point_files(StudioUtils::to_string_list(worlds))) {
      return false;
    }
    if (!shape->import_landmarks_files(StudioUtils::to_string_list(landmark_files))) {
      return false;
    }

    // add to landmark definitions if landmark files contain more landmarks than the project definitions
    auto landmarks = shape->landmarks();
    for (int row = 0; row < landmarks.rows(); row++) {
      int domain_id = landmarks(row, 0);
      int point_id = landmarks(row, 1);
      auto landmark_definitions = project_->get_landmarks(domain_id);
      while (point_id >= landmark_definitions.size()) {
        project_->new_landmark(domain_id);
        landmark_definitions = project_->get_landmarks(domain_id);
      }
    }
    for (int domain_id = 0; domain_id < domain_names.size(); domain_id++) {
    }
    shapes_ << shape;
  }

  groups_available_ = project_->get_group_names().size() > 0;
  params_ = project_->get_parameters(Parameters::STUDIO_PARAMS);
  return true;
}

//---------------------------------------------------------------------------
void Session::set_project_path(QString relative_path) {
  std::cerr << "Setting project path to " << relative_path.toStdString() << "\n";

  QDir old_path = QDir(this->project_path_);
  QDir new_path = QDir(relative_path);

  auto subjects = this->project_->get_subjects();
  for (auto subject : subjects) {
    // segmentations
    auto paths = subject->get_segmentation_filenames();
    std::vector<std::string> new_paths;
    for (const auto& path : paths) {
      auto full_path = old_path.absoluteFilePath(QString::fromStdString(path));
      new_paths.push_back(new_path.relativeFilePath(full_path).toStdString());
    }
    subject->set_segmentation_filenames(new_paths);

    // groomed
    paths = subject->get_groomed_filenames();
    new_paths.clear();
    for (const auto& path : paths) {
      auto full_path = old_path.absoluteFilePath(QString::fromStdString(path));
      new_paths.push_back(new_path.relativeFilePath(full_path).toStdString());
    }
    subject->set_groomed_filenames(new_paths);

    // local particles
    paths = subject->get_local_particle_filenames();
    new_paths.clear();
    for (const auto& path : paths) {
      auto full_path = old_path.absoluteFilePath(QString::fromStdString(path));
      new_paths.push_back(new_path.relativeFilePath(full_path).toStdString());
    }
    subject->set_local_particle_filenames(new_paths);

    // world particles
    paths = subject->get_world_particle_filenames();
    new_paths.clear();
    for (const auto& path : paths) {
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
std::shared_ptr<Project> Session::get_project() { return this->project_; }

//---------------------------------------------------------------------------
void Session::load_original_files(std::vector<std::string> filenames) {
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
void Session::load_groomed_files(std::vector<std::string> file_names, double iso, int domains_per_shape) {
  assert(file_names.size() % domains_per_shape == 0);
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
bool Session::load_point_files(std::vector<std::string> local, std::vector<std::string> world, int domains_per_shape) {
  assert(local.size() % domains_per_shape == 0);
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

    auto base = QString::fromStdString(world[counter]).remove("_world.particles").toStdString();
    QStringList list;
    list << QString::fromStdString(StringUtils::getFilename(base));
    list << "";
    list << "";
    list << "";

    QStringList local_filenames;
    QStringList world_filenames;
    // only single domain supported so far
    for (int j = 0; j < domains_per_shape; j++) {
      local_filenames << QString::fromStdString(local[counter + j]);
      world_filenames << QString::fromStdString(world[counter + j]);
    }
    counter += domains_per_shape;
    if (!this->shapes_[i]->import_local_point_files(local_filenames)) {
      emit error("Unable to load point files: " + local_filenames.join(", "));
    }
    if (!this->shapes_[i]->import_global_point_files(world_filenames)) {
      emit error("Unable to load point files: " + local_filenames.join(", "));
    }
    this->shapes_[i]->set_annotations(list);
  }

  this->project_->store_subjects();
  if (local.size() > 0) {
    emit data_changed();
  }

  return true;
}

//---------------------------------------------------------------------------
bool Session::update_particles(std::vector<StudioParticles> particles) {
  for (int i = 0; i < particles.size(); i++) {
    QSharedPointer<Shape> shape;
    if (this->shapes_.size() > i) {
      shape = this->shapes_[i];
    } else {
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
void Session::update_procrustes_transforms(std::vector<std::vector<std::vector<double>>> transforms) {
  for (size_t i = 0; i < transforms.size(); i++) {
    if (this->shapes_.size() > i) {
      QSharedPointer<Shape> shape = this->shapes_[i];
      if (shape->get_subject()) {
        shape->get_subject()->set_procrustes_transforms(transforms[i]);
      }
    }
  }
}

//---------------------------------------------------------------------------
double Session::update_auto_glyph_size() {
  auto_glyph_size_ = 1;
  if (shapes_.empty()) {
    return auto_glyph_size_;
  }

  double max_range = std::numeric_limits<double>::min();
  int num_particles = 0;

  for (auto& shape : shapes_) {
    Eigen::VectorXd points = shape->get_global_correspondence_points();
    if (points.size() == 0) {
      continue;
    }
    num_particles = (points.size() / 3);
    double max_x = std::numeric_limits<double>::lowest();
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

  if (num_particles == 0) {
    return auto_glyph_size_;
  }
  auto_glyph_size_ = max_range / std::sqrt(static_cast<double>(num_particles)) / 2;
  auto_glyph_size_ = std::max<double>(0.1, auto_glyph_size_);
  auto_glyph_size_ = std::min<double>(10.0, auto_glyph_size_);

  return this->auto_glyph_size_;
}

//---------------------------------------------------------------------------
bool Session::is_light_project() { return this->is_light_project_; }

//---------------------------------------------------------------------------
bool Session::get_groomed_present() { return this->project_->get_groomed_present(); }

//---------------------------------------------------------------------------
QVector<QSharedPointer<Shape>> Session::get_shapes() { return this->shapes_; }

//---------------------------------------------------------------------------
void Session::remove_shapes(QList<int> list) {
  std::sort(list.begin(), list.end(), std::greater<>());
  foreach (int i, list) {
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
bool Session::original_present() { return this->project_->get_segmentations_present(); }

//---------------------------------------------------------------------------
bool Session::groomed_present() { return this->project_->get_groomed_present(); }

//---------------------------------------------------------------------------
bool Session::particles_present() {
  if (!this->project_->get_particles_present()) {
    return false;
  }

  if (this->shapes_.size() > 0) {
    auto shape = this->shapes_[0];
    return shape->get_global_correspondence_points().size() > 0;
  }

  return true;
}

//---------------------------------------------------------------------------
bool Session::groups_available() { return this->groups_available_; }

//---------------------------------------------------------------------------
void Session::renumber_shapes() {
  for (int i = 0; i < this->shapes_.size(); i++) {
    this->shapes_[i]->set_id(i + 1);
  }
}

//---------------------------------------------------------------------------
void Session::new_landmark(PickResult result) {
  if (result.subject_ < 0 || result.subject_ >= shapes_.size()) {
    return;
  }

  Eigen::MatrixXd& landmarks = shapes_[result.subject_]->landmarks();

  // find the row that matches the placing_landmark and domain
  int row = -1;
  int point_id = 0;
  for (int i = 0; i < landmarks.rows(); i++) {
    if (landmarks(i, 0) == result.domain_ && landmarks(i, 1) == placing_landmark_) {
      row = i;
      point_id = placing_landmark_;
    }
  }

  if (row == -1) {  // not found
    for (int i = 0; i < landmarks.rows(); i++) {
      if (landmarks(i, 0) == result.domain_) {
        point_id = std::max<int>(point_id, landmarks(i, 1) + 1);
      }
    }
  }

  if (row == -1) {
    landmarks.conservativeResize(landmarks.rows() + 1, 5);
    row = landmarks.rows() - 1;
  }
  landmarks(row, 0) = result.domain_;
  landmarks(row, 1) = point_id;
  landmarks(row, 2) = result.pos_.x;
  landmarks(row, 3) = result.pos_.y;
  landmarks(row, 4) = result.pos_.z;
  if (point_id >= project_->get_landmarks(result.domain_).size()) {
    project_->new_landmark(result.domain_);
  }
  update_auto_glyph_size();
  emit landmarks_changed();
}

//---------------------------------------------------------------------------
void Session::new_plane_point(PickResult result) {
  if (result.subject_ < 0 || result.subject_ >= shapes_.size()) {
    return;
  }

  Eigen::Vector3d pos(result.pos_.x, result.pos_.y, result.pos_.z);
  auto& constraints = shapes_[result.subject_]->constraints();
  while (result.domain_ >= constraints.size()) {
    shapes_[result.subject_]->constraints().push_back(Constraints{});
  }
  auto& planes = constraints[result.domain_].getPlaneConstraints();

  bool found = false;
  for (auto& plane : planes) {
    if (plane.points().size() < 3) {
      // found one to add a point to
      plane.points().push_back(pos);
      found = true;
      break;
    }
  }

  if (!found) {  // no planes were found with less than 3 points,
    PlaneConstraint plane;
    plane.points().push_back(pos);
    planes.push_back(plane);
  }
  emit planes_changed();
}

//---------------------------------------------------------------------------
QString Session::get_filename() { return this->filename_; }

//---------------------------------------------------------------------------
int Session::get_num_shapes() { return this->shapes_.size(); }

//---------------------------------------------------------------------------
int Session::get_domains_per_shape() { return this->get_project()->get_number_of_domains_per_subject(); }

//---------------------------------------------------------------------------
Parameters& Session::parameters() { return this->params_; }

//---------------------------------------------------------------------------
QString Session::get_display_name() {
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
std::string Session::get_default_feature_map() {
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
bool Session::is_supported_file_format(std::string filename) {
  for (auto type : Mesh::getSupportedTypes()) {
    if (StringUtils::hasSuffix(filename, type)) {
      return true;
    }
  }

  if (StringUtils::hasSuffix(filename, ".nrrd") || StringUtils::hasSuffix(filename, ".mha") ||
      StringUtils::hasSuffix(filename, ".nii") || StringUtils::hasSuffix(filename, ".nii.gz")) {
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------
std::vector<DomainType> Session::get_domain_types() {
  auto subjects = this->get_project()->get_subjects();
  if (subjects.size() > 0 && subjects[0]->get_domain_types().size() > 0) {
    return subjects[0]->get_domain_types();
  }
  return std::vector<DomainType>();
}

//---------------------------------------------------------------------------
Point3 Session::get_point(const Eigen::VectorXd& points, int i) {
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
double Session::get_auto_glyph_size() { return this->auto_glyph_size_; }

//---------------------------------------------------------------------------
void Session::clear_particles() {
  std::vector<StudioParticles> particles(this->get_num_shapes());
  this->update_particles(particles);
}

//---------------------------------------------------------------------------
bool Session::get_feature_auto_scale() { return this->params_.get("feature_auto_scale", true); }

//---------------------------------------------------------------------------
void Session::set_feature_auto_scale(bool value) {
  this->params_.set("feature_auto_scale", value);
  emit feature_range_changed();
}

//---------------------------------------------------------------------------
double Session::get_feature_range_max() { return this->params_.get("feature_range_max", 0); }

//---------------------------------------------------------------------------
double Session::get_feature_range_min() { return this->params_.get("feature_range_min", 0); }

//---------------------------------------------------------------------------
void Session::set_feature_range(double min, double max) {
  this->set_feature_range_min(min);
  this->set_feature_range_max(max);
}

//---------------------------------------------------------------------------
void Session::set_feature_range_min(double value) {
  this->params_.set("feature_range_min", value);
  emit feature_range_changed();
}

//---------------------------------------------------------------------------
void Session::set_feature_range_max(double value) {
  this->params_.set("feature_range_max", value);
  emit feature_range_changed();
}

//---------------------------------------------------------------------------
void Session::handle_ctrl_click(PickResult result) {
  std::string tool_state = parameters().get("tool_state", Session::DATA_C);

  if (tool_state != Session::DATA_C) {
    return;
  }

  if (landmarks_active_) {
    new_landmark(result);
  } else if (planes_active_) {
    new_plane_point(result);
  }
}

//---------------------------------------------------------------------------
void Session::trigger_landmarks_changed() { emit landmarks_changed(); }

//---------------------------------------------------------------------------
void Session::trigger_planes_changed() { emit planes_changed(); }

//---------------------------------------------------------------------------
void Session::set_active_landmark_domain(int id) { active_landmark_domain_ = id; }

//---------------------------------------------------------------------------
int Session::get_active_landmark_domain() { return active_landmark_domain_; }

//---------------------------------------------------------------------------
void Session::set_placing_landmark(int id) { placing_landmark_ = id; }

//---------------------------------------------------------------------------
int Session::get_placing_landmark() { return placing_landmark_; }

//---------------------------------------------------------------------------
void Session::set_landmarks_active(bool active) { landmarks_active_ = active; }

//---------------------------------------------------------------------------
void Session::set_planes_active(bool active) { planes_active_ = active; }

//---------------------------------------------------------------------------
void Session::set_show_landmark_labels(bool show) {
  show_landmark_labels_ = show;
  emit landmarks_changed();
}

//---------------------------------------------------------------------------
bool Session::get_show_landmark_labels() { return show_landmark_labels_; }

//---------------------------------------------------------------------------
void Session::set_show_planes(bool show) {
  bool old_value = get_show_planes();
  params_.set("show_planes", show);
  if (show != old_value) {
    emit planes_changed();
  }
}

//---------------------------------------------------------------------------
bool Session::get_show_planes() { return params_.get("show_planes", true); }

//---------------------------------------------------------------------------
bool Session::set_image_name(std::string image_name) {
  if (image_name == get_image_name() || is_loading()) {
    return false;
  }
  params_.set("image_name", image_name);
  Q_EMIT image_slice_settings_changed();
  return true;
}

//---------------------------------------------------------------------------
std::string Session::get_image_name() { return params_.get("image_name", "-none-"); }

//---------------------------------------------------------------------------
void Session::set_image_axis(QString axis) {
  if (axis == get_image_axis() || is_loading()) {
    return;
  }
  params_.set("image_axis", axis.toStdString());
  Q_EMIT image_slice_settings_changed();
}

//---------------------------------------------------------------------------
Axis Session::get_image_axis() {
  std::string axis_string = params_.get("image_axis", "Z");
  Axis axis = toAxis(axis_string);
  if (axis == Axis::invalid) {
    axis = Axis::Z;
  }
  return toAxis(axis_string);
}

//---------------------------------------------------------------------------
void Session::set_image_3d_mode(bool mode) {
  if (mode == get_image_3d_mode() || is_loading()) {
    return;
  }
  params_.set("image_3d_mode", mode);
  Q_EMIT image_slice_settings_changed();
}

//---------------------------------------------------------------------------
bool Session::get_image_3d_mode() { return params_.get("image_3d_mode", false); }

//---------------------------------------------------------------------------
void Session::set_image_share_window_and_level(bool enabled) {
  if (enabled == get_image_share_window_and_level() || is_loading()) {
    return;
  }
  params_.set("image_share_window_and_level", enabled);
}

//---------------------------------------------------------------------------
bool Session::get_image_share_window_and_level() { return params_.get("image_share_window_and_level", true); }

//---------------------------------------------------------------------------
void Session::set_loading(bool loading) { is_loading_ = loading; }

//---------------------------------------------------------------------------
bool Session::is_loading() { return is_loading_; }

//---------------------------------------------------------------------------
void Session::set_landmark_drag_mode(bool mode) {
  landmark_drag_mode_ = mode;
  emit landmarks_changed();
}

//---------------------------------------------------------------------------
bool Session::get_landmark_drag_mode() { return landmark_drag_mode_; }
//---------------------------------------------------------------------------
}  // namespace shapeworks
