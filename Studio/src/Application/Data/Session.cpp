#include <Data/Session.h>
#include <Data/Shape.h>
#include <Data/Mesh.h>
#include <Data/MeshManager.h>
#include <Visualization/ShapeWorksWorker.h>
#include <QThread>

#include <Visualization/Visualizer.h>

#include <Libs/Project/Project.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QProgressDialog>

#include <tinyxml.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyDataReader.h>

const std::string Session::DATA_C("data");
const std::string Session::GROOM_C("groom");
const std::string Session::OPTIMIZE_C("optimize");
const std::string Session::ANALYSIS_C("analysis");

//---------------------------------------------------------------------------
Session::Session(QWidget* parent, Preferences& prefs) : parent_(parent),
  preferences_(prefs)
{
  this->parent_ = NULL;
  this->reset();
  connect(this->mesh_manager_.data(), SIGNAL(new_mesh()), this, SLOT(handle_new_mesh()));
}

//---------------------------------------------------------------------------
Session::~Session()
{}

//---------------------------------------------------------------------------
void Session::handle_new_mesh()
{
  emit new_mesh();
}

//---------------------------------------------------------------------------
void Session::handle_message(std::string s)
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
  this->calculate_reconstructed_samples();
}

//---------------------------------------------------------------------------
void Session::calculate_reconstructed_samples()
{
  if (!this->particles_present_) {
    return;
  }
  //this->preferences_.set_preference("Studio/cache_enabled", false);
  for (int i = 0; i < this->shapes_.size(); i++) {
    auto shape = this->shapes_.at(i);
    auto pts = shape->get_local_correspondence_points();
    if (pts.size() > 0) {
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
bool Session::save_project(std::string fname, std::string data_dir)
{
  QString filename = QString::fromStdString(fname);
  if (filename == "") {
    filename = this->filename_;
  }
  this->filename_ = filename;
  bool defaultDir = data_dir == "";

  // open file
  QFile file(filename);

  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::warning(0, "Read only", "The file is in read only mode");
    return false;
  }

  QProgressDialog progress("Saving Project...", "Abort", 0, 100, this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  //progress.show();
  //progress.setMinimumDuration(2000);

  this->preferences_.set_saved();
  //write out mean info
  auto prefix = this->shapes_[0]->get_original_filename().toStdString();
  prefix = prefix.substr(0, prefix.find_last_of("."));
  auto location = this->shapes_[0]->get_original_filename_with_path().toStdString();
  auto position = location.find_last_of("/");
  location = location.substr(0, position) + "/" + prefix;
  if (!defaultDir) {
    location = data_dir + "/";
  }

  progress.setValue(5);
  QApplication::processEvents();

  // original files
  if (this->original_present()) {
    std::vector<std::string> original_list;
    for (int i = 0; i < this->shapes_.size(); i++) {
      original_list.push_back(this->shapes_[i]->get_original_filename_with_path().toStdString());
    }
    //this->project_->set_original_files(original_list);
  }

  // distance transforms
  if (this->unsaved_groomed_files_) {
    std::cerr << "unsaved groomed files detected, saving...\n";
    std::vector<std::string> groomed_list;
    for (int i = 0; i < this->shapes_.size(); i++) {
      QString loc = this->shapes_[i]->get_groomed_filename_with_path();
      if (!defaultDir) {
        loc = QString::fromStdString(data_dir) + "/" +
              this->shapes_[i]->get_groomed_filename();
      }

      std::string location = loc.toStdString();

      groomed_list.push_back(location);

      //try writing the groomed to file
      WriterType::Pointer writer = WriterType::New();
      writer->SetFileName(location);
      writer->SetInput(this->shapes_[i]->get_groomed_image());
      writer->SetUseCompression(true);
      std::cerr << "Writing distance transform: " << location << "\n";
      writer->Update();
      std::vector<std::string> groomed_filenames {location};   // only single domain supported so far
      this->shapes_[i]->get_subject()->set_groomed_filenames(groomed_filenames);

      QApplication::processEvents();
      if (progress.wasCanceled()) {
        break;
      }
    }

    this->unsaved_groomed_files_ = false;
  }

  // correspondence points
  if (this->unsaved_particle_files_) {
    for (int i = 0; i < this->shapes_.size(); i++) {
      auto global_path = this->shapes_[i]->get_subject()->get_global_particle_filename();
      auto local_path = this->shapes_[i]->get_subject()->get_local_particle_filename();
      this->save_particles_file(global_path, this->shapes_[i]->get_global_correspondence_points());
      this->save_particles_file(local_path, this->shapes_[i]->get_local_correspondence_points());
    }
    this->unsaved_particle_files_ = false;
  }

  this->project_->set_parameters(Parameters::STUDIO_PARAMS, this->params_);

  this->project_->save(filename.toStdString());
  progress.setValue(100);
  return true;
}

//---------------------------------------------------------------------------
void Session::save_particles_file(std::string filename, const vnl_vector<double> &points)
{
  std::ofstream out(filename);
  size_t newline = 1;
  for (auto &a : points) {
    out << a << (newline % 3 == 0 ? "\n" : "    ");
    newline++;
  }
  out.close();
}

//---------------------------------------------------------------------------
bool Session::load_xml_project(QString filename, std::string& planesFile)
{
  if (!QFile::exists(filename)) {
    QMessageBox::critical(NULL, "ShapeWorksStudio", "File does not exist: " + filename,
                          QMessageBox::Ok);
    return false;
  }
  // clear the project out first
  this->reset();
  this->filename_ = filename;

  if (filename.toLower().endsWith(".xlsx")) {
    return this->load_project(filename);
  }

  // open and parse XML
  TiXmlDocument doc(filename.toStdString().c_str());
  bool loadOkay = doc.LoadFile();
  if (!loadOkay) {
    QString message = "Error: Invalid parameter file: " + filename;
    QMessageBox::critical(NULL, "ShapeWorksStudio", message, QMessageBox::Ok);
    return false;
  }

  TiXmlHandle docHandle(&doc);

  TiXmlElement* project_element = docHandle.FirstChild("project").Element();

  bool is_project = project_element != nullptr;

  if (!is_project) {
    return this->load_light_project(filename);
  }

  if (QString(project_element->Attribute("version")) != "2") {
    QString message =
      "Error: This version of ShapeWorksStudio only reads version 2 project files: " +
      filename;
    QMessageBox::critical(NULL, "ShapeWorksStudio", message, QMessageBox::Ok);
    return false;
  }

  // setup XML
  std::vector<std::string> original_files, groom_files, local_point_files, global_point_files;
  std::string sparseFile, denseFile, goodPtsFile;

  TiXmlElement* elem = project_element->FirstChildElement("original_files");
  if (elem) {
    std::string filename;
    std::istringstream inputsBuffer(elem->GetText());
    while (inputsBuffer >> filename) {
      std::cerr << "Found original file: " << filename << "\n";

      if (!QFile::exists(QString::fromStdString(filename))) {
        QMessageBox::critical(NULL, "ShapeWorksStudio", "File does not exist: " +
                              QString::fromStdString(filename), QMessageBox::Ok);
        return false;
      }

      original_files.push_back(filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  elem = project_element->FirstChildElement("distance_transforms");
  if (elem) {
    std::string filename;
    std::istringstream inputsBuffer(elem->GetText());
    while (inputsBuffer >> filename) {
      //std::cerr << "Found distance transform: " << filename << "\n";

      if (!QFile::exists(QString::fromStdString(filename))) {
        QMessageBox::critical(NULL, "ShapeWorksStudio", "File does not exist: " +
                              QString::fromStdString(filename), QMessageBox::Ok);
        return false;
      }

      groom_files.push_back(filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  elem = project_element->FirstChildElement("local_point_files");
  if (elem) {
    local_point_files.clear();
    std::string point_filename;
    std::istringstream inputsBuffer(elem->GetText());
    while (inputsBuffer >> point_filename) {
      local_point_files.push_back(point_filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  elem = project_element->FirstChildElement("world_point_files");
  if (elem) {
    global_point_files.clear();
    std::string point_filename;
    std::istringstream inputsBuffer(elem->GetText());
    while (inputsBuffer >> point_filename) {
      global_point_files.push_back(point_filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  for (auto item = project_element->FirstChildElement(); item != nullptr;
       item = item->NextSiblingElement()) {
    if (QString(item->Value()) != "shapes") {

      QString name = item->Value();
      QString value = item->GetText();

      if (name == "denseMean_file") {
        denseFile = value.toStdString();
      }
      else if (name == "sparseMean_file") {
        sparseFile = value.toStdString();
      }
      else if (name == "goodPoints_file") {
        goodPtsFile = value.toStdString();
      }
      else if (name == "cutPlanes_file") {
        if (value.toStdString().find_last_of(".txt") != std::string::npos) {
          planesFile = value.toStdString();
        }
      }
      else {
        //    this->preferences_.set_preference(name.toStdString(), QVariant(value));
      }
    }
  }

  this->load_original_files(original_files);
  if (groom_files.size() > 0) {
    this->load_groomed_files(groom_files, 0.5);
  }
  this->load_point_files(local_point_files, true);
  this->load_point_files(global_point_files, false);
  if (!denseFile.empty() && !sparseFile.empty() && !goodPtsFile.empty()) {
    this->mesh_manager_->get_surface_reconstructor()->readMeanInfo(denseFile, sparseFile,
                                                                   goodPtsFile);
  }
  this->particles_present_ = local_point_files.size() == global_point_files.size() &&
                             global_point_files.size() > 1;
  return true;
}

//---------------------------------------------------------------------------
bool Session::load_light_project(QString filename)
{
  std::cerr << "Loading light project...\n";
  this->is_light_project_ = true;

  // open and parse XML
  TiXmlDocument doc(filename.toStdString().c_str());
  bool loadOkay = doc.LoadFile();
  if (!loadOkay) {
    QString message = "Error: Invalid parameter file" + filename;
    QMessageBox::critical(NULL, "ShapeWorksStudio", message, QMessageBox::Ok);
    return false;
  }

  TiXmlHandle docHandle(&doc);
  std::istringstream inputsBuffer;

  // determine if groups are available
  this->groups_available_ = (docHandle.FirstChild("group_ids").Element() != nullptr);

  /// TODO
  //this->numDomains = 1;
  //TiXmlElement* elem = docHandle.FirstChild("domains_per_shape").Element();
  //if (elem) {this->numDomains = atoi(elem->GetText()); }

  // setup XML
  std::vector<std::string> import_files, groom_files, local_point_files, global_point_files;
  std::string sparseFile, denseFile, goodPtsFile;

  TiXmlElement* elem = docHandle.FirstChild("distance_transform_files").Element();
  if (elem) {
    std::string distance_transform_filename;
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> distance_transform_filename) {
      //std::cerr << "Found distance transform: " << distance_transform_filename << "\n";

      if (!QFile::exists(QString::fromStdString(distance_transform_filename))) {
        QMessageBox::critical(NULL, "ShapeWorksStudio",
                              "File does not exist: " +
                              QString::fromStdString(distance_transform_filename),
                              QMessageBox::Ok);
        return false;
      }

      groom_files.push_back(distance_transform_filename);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

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

  elem = docHandle.FirstChild("local_point_files").Element();
  if (elem) {
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

  this->load_groomed_files(groom_files, 0.5);

  //this->project_->

  if (!this->load_point_files(local_point_files, true)) {
    return false;
  }

  if (!this->load_point_files(global_point_files, false)) {
    return false;
  }

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

  this->particles_present_ = local_point_files.size() == global_point_files.size() &&
                             global_point_files.size() > 1;

  //this->calculate_reconstructed_samples();

  this->settings().set("view_state", Visualizer::MODE_RECONSTRUCTION_C);
  this->settings().set("tool_state", Session::ANALYSIS_C);

  //this->preferences_.set_preference("display_state",
//                                    QString::fromStdString(Visualizer::MODE_RECONSTRUCTION_C));
//  this->preferences_.set_preference("tool_state", QString::fromStdString(Session::ANALYSIS_C));
  this->renumber_shapes();

  this->project_->store_subjects();

  std::cerr << "light project loaded\n";
  return true;
}

//---------------------------------------------------------------------------
bool Session::load_project(QString filename)
{
  // clear the project out first
  this->reset();
  this->filename_ = filename;

  this->project_->load(filename.toStdString());

  int num_subjects = this->project_->get_number_of_subjects();
  std::cerr << "num_subjects = " << num_subjects << "\n";

  auto subjects = this->project_->get_subjects();

  std::vector<std::string> local_point_files;
  std::vector<std::string> global_point_files;

  for (int i = 0; i < num_subjects; i++) {
    QSharedPointer<Shape> shape = QSharedPointer<Shape>(new Shape());
    shape->set_mesh_manager(this->mesh_manager_);
    shape->set_subject(subjects[i]);
    if (subjects[i]->get_local_particle_filename() != "") {
      local_point_files.push_back(subjects[i]->get_local_particle_filename());
      global_point_files.push_back(subjects[i]->get_global_particle_filename());
    }
    this->shapes_ << shape;
  }

  this->load_point_files(local_point_files, true);
  this->load_point_files(global_point_files, false);

  /*
     if (!denseFile.empty() && !sparseFile.empty() && !goodPtsFile.empty()) {
     this->mesh_manager_->getSurfaceReconstructor()->readMeanInfo(denseFile, sparseFile,
                                                                 goodPtsFile);
     }
   */
  this->particles_present_ = local_point_files.size() == global_point_files.size() &&
                             global_point_files.size() > 1;

  this->params_ = this->project_->get_parameters(Parameters::STUDIO_PARAMS);

  return true;
}

//---------------------------------------------------------------------------
std::shared_ptr<Project> Session::get_project()
{
  return this->project_;
}

//---------------------------------------------------------------------------
void Session::load_original_files(std::vector<std::string> filenames)
{
  QProgressDialog progress("Loading images...", "Abort", 0, filenames.size(), this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  //progress.show();
  //progress.setMinimumDuration(2000);

  for (int i = 0; i < filenames.size(); i++) {
    progress.setValue(i);
    QApplication::processEvents();
    if (progress.wasCanceled()) {
      break;
    }

    QString filename = QString::fromStdString(filenames[i]);
    if (!QFile::exists(filename)) {
      QMessageBox::critical(NULL, "ShapeWorksStudio", "File does not exist: " + filename,
                            QMessageBox::Ok);
      return;
    }

    if (i >= this->shapes_.size()) {
      QSharedPointer<Shape> shape = QSharedPointer<Shape>(new Shape());

      std::shared_ptr<Subject> subject = std::make_shared<Subject>();

      shape->set_mesh_manager(this->mesh_manager_);
      shape->set_subject(subject);
      this->project_->get_subjects().push_back(subject);

      this->shapes_.push_back(shape);
    }
    this->shapes_[i]->import_original_image(filenames[i], 0.5);
  }
  progress.setValue(filenames.size());
  QApplication::processEvents();
  this->renumber_shapes();
  if (filenames.size() > 0) {
    this->original_present_ = true;
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
void Session::load_groomed_images(std::vector<ImageType::Pointer> images, double iso)
{
  QProgressDialog progress("Loading groomed images...", "Abort", 0, images.size(), this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  progress.setMinimumDuration(2000);

  for (int i = 0; i < images.size(); i++) {
    progress.setValue(i);
    QApplication::processEvents();
    if (progress.wasCanceled()) {
      break;
    }
    if (this->shapes_.size() <= i) {
      QSharedPointer<Shape> new_shape = QSharedPointer<Shape>(new Shape);
      new_shape->set_mesh_manager(this->mesh_manager_);
      this->shapes_.push_back(new_shape);
    }
    this->shapes_[i]->import_groomed_image(images[i], iso);
  }
  progress.setValue(images.size());
  QApplication::processEvents();

  this->project_->store_subjects();
  if (images.size() > 0) {
    this->unsaved_groomed_files_ = true;
    this->groomed_present_ = true;
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
void Session::load_groomed_files(std::vector<std::string> file_names, double iso)
{
  //QProgressDialog progress("Loading groomed images...", "Abort", 0,
  //                         file_names.size(), this->parent_);
  //progress.setWindowModality(Qt::WindowModal);

  for (int i = 0; i < file_names.size(); i++) {
    if (this->shapes_.size() <= i) {
      auto shape = QSharedPointer<Shape>(new Shape);
      std::shared_ptr<Subject> subject = std::make_shared<Subject>();
      shape->set_mesh_manager(this->mesh_manager_);
      shape->set_subject(subject);
      this->project_->get_subjects().push_back(subject);
      this->shapes_.push_back(shape);
    }

    std::vector<std::string> groomed_filenames {file_names[i]};   // only single domain supported so far
    this->shapes_[i]->get_subject()->set_groomed_filenames(groomed_filenames);
  }

  if (file_names.size() > 0) {
    this->groomed_present_ = true;
    //emit data_changed();
  }
}

//---------------------------------------------------------------------------
bool Session::update_points(std::vector<std::vector<itk::Point<double>>> points, bool local)
{
  for (int i = 0; i < points.size(); i++) {
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

    if (!shape->import_points(points[i], local)) {
      return false;
    }

    // now update the Subject's filename
    // this needs to be done here so that the Project knows there are particle files
    std::string suffix = local ? "local" : "world";
    auto base_name = this->shapes_[i]->get_original_filename().toStdString();
    auto name =
      base_name.substr(0, base_name.find_last_of(".")) + "." + suffix + ".particles";
    std::string loc = shape->get_original_filename_with_path().toStdString();
    auto pos = loc.find_last_of("/");
    loc = loc.substr(0, pos) + "/";
    auto path = loc + name;
    if (local) {
      shape->get_subject()->set_local_particle_filename(path);
    }
    else {
      shape->get_subject()->set_global_particle_filename(path);
    }
  }

  // update the project now that we have particles
  this->project_->store_subjects();

  if (points.size() > 0) {
    this->unsaved_particle_files_ = true;
    emit points_changed();
  }
  return true;
}

//---------------------------------------------------------------------------
void Session::set_reconstructed_present(bool value)
{
  this->particles_present_ = value;
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
bool Session::load_point_files(std::vector<std::string> list, bool local)
{
  QProgressDialog progress("Loading point files...", "Abort", 0, list.size(), this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  //progress.show();
  //progress.setMinimumDuration(2000);
  for (int i = 0; i < list.size(); i++) {
    progress.setValue(i);
    QApplication::processEvents();
    if (progress.wasCanceled()) {
      break;
    }
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
    auto fname = QString::fromStdString(list[i]);
    QFileInfo fi(fname);
    QString basename = fi.completeBaseName();
    QString ext = fi.suffix();

    if (QFile::exists(fname)) {
      if (!local) {
        if (!shape->import_global_point_file(fname)) {
          return false;
        }
      }
      else {
        if (!shape->import_local_point_file(fname)) {
          return false;
        }
      }
    }
    else {
      QMessageBox::critical(0, "Error", "Unable to open particle file:" + fname);
      return false;
    }
  }
  progress.setValue(list.size());
  QApplication::processEvents();
  if (list.size() > 0) {
    //  emit data_changed();
  }
  return true;
}

//---------------------------------------------------------------------------
QVector<QSharedPointer<Shape>> Session::get_shapes()
{
  return this->shapes_;
}

//---------------------------------------------------------------------------
void Session::remove_shapes(QList<int> list)
{
  std::sort(list.begin(), list.end(), std::greater <>());
  foreach(int i, list) {
    auto subjects = this->project_->get_subjects();
    subjects.erase(subjects.begin() + i);
    this->shapes_.erase(this->shapes_.begin() + i);
  }
  this->renumber_shapes();
  this->project_->store_subjects();
  emit data_changed();
}

//---------------------------------------------------------------------------
void Session::reset()
{
  this->is_light_project_ = false;

  this->filename_ = "";

  this->shapes_.clear();

  this->original_present_ = false;
  this->groomed_present_ = false;
  this->particles_present_ = false;

  this->mesh_manager_ = QSharedPointer<MeshManager>(new MeshManager(preferences_));

  connect(this->mesh_manager_.data(), SIGNAL(new_mesh()), this, SLOT(handle_new_mesh()));
  this->handle_clear_cache();
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
  //return this->groomed_present_;
}

//---------------------------------------------------------------------------
bool Session::particles_present()
{
  return this->project_->get_particles_present();
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
Parameters &Session::settings()
{
  return this->params_;
}
