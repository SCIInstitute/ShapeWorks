#include <Data/Project.h>
#include <Data/Shape.h>
#include <Data/Mesh.h>
#include <Data/MeshManager.h>
#include <Visualization/ShapeworksWorker.h>
#include <QThread>

#include <Visualization/Visualizer.h>

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

const std::string Project::DATA_C("data");
const std::string Project::GROOM_C("groom");
const std::string Project::OPTIMIZE_C("optimize");
const std::string Project::ANALYSIS_C("analysis");

//---------------------------------------------------------------------------
Project::Project(QWidget* parent, Preferences& prefs) : parent_(parent),
  preferences_(prefs)
{
  this->parent_ = NULL;
  this->reset();
  connect(this->mesh_manager_.data(), SIGNAL(new_mesh()), this, SLOT(handle_new_mesh()));
}

//---------------------------------------------------------------------------
Project::~Project()
{}

//---------------------------------------------------------------------------
void Project::handle_new_mesh()
{
  emit update_display();
}

//---------------------------------------------------------------------------
void Project::handle_message(std::string s)
{
  emit message(s);
}

//---------------------------------------------------------------------------
void Project::handle_thread_complete()
{
  emit message("Reconstruction initialization complete.");
  this->calculate_reconstructed_samples();
  emit update_display();
}

//---------------------------------------------------------------------------
void Project::handle_clear_cache()
{
  this->mesh_manager_->clear_cache();
  this->calculate_reconstructed_samples();
}

//---------------------------------------------------------------------------
void Project::calculate_reconstructed_samples()
{
  if (!this->reconstructed_present_) {
    std::cerr << "no reconstructed present\n";
    return;
  }
  this->preferences_.set_preference("cache_enabled", false);
  for (int i = 0; i < this->shapes_.size(); i++) {
    auto shape = this->shapes_.at(i);
    auto pts = shape->get_local_correspondence_points();
    if (pts.size() > 0) {
      shape->set_reconstructed_mesh(this->mesh_manager_->getMesh(pts));
    }
  }
  this->preferences_.set_preference("cache_enabled", true);
}

//---------------------------------------------------------------------------
void Project::set_parent(QWidget* parent)
{
  this->parent_ = parent;
}

//---------------------------------------------------------------------------
bool Project::save_project(std::string fname, std::string dataDir, std::string cutPlanesFile)
{
  QString filename = QString::fromStdString(fname);
  if (filename == "") {
    filename = this->filename_;
  }
  this->filename_ = filename;
  bool defaultDir = dataDir == "";

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

  // setup XML
  QSharedPointer<QXmlStreamWriter> xml = QSharedPointer<QXmlStreamWriter>(new QXmlStreamWriter());
  xml->setAutoFormatting(true);
  xml->setDevice(&file);
  xml->writeStartDocument();

  xml->writeStartElement("project");
  xml->writeAttribute("version", "1");
  auto prefs = this->preferences_.getAllPreferences();
  for (auto &a : prefs) {
    xml->writeTextElement(
      QString::fromStdString(a.first), a.second.toString());
  }
  this->preferences_.set_saved();
  //write out mean info
  auto prefix = this->shapes_[0]->get_original_filename().toStdString();
  prefix = prefix.substr(0, prefix.find_last_of("."));
  auto location = this->shapes_[0]->get_original_filename_with_path().toStdString();
  auto position = location.find_last_of("/");
  location = location.substr(0, position) + "/" + prefix;
  if (!defaultDir) {
    location = dataDir + "/";
  }
  if (this->reconstructed_present() &&
      this->get_mesh_manager()->getSurfaceReconstructor()->hasDenseMean()) {
    this->mesh_manager_->getSurfaceReconstructor()->writeMeanInfo(location);
    xml->writeTextElement("denseMean_file", QString::fromStdString(location + ".dense.vtk"));
    xml->writeTextElement("sparseMean_file", QString::fromStdString(location + ".sparse.txt"));
    xml->writeTextElement("goodPoints_file", QString::fromStdString(location + ".goodPoints.txt"));
  }
  xml->writeTextElement("cutPlanes_file", QString::fromStdString(cutPlanesFile));
  progress.setValue(5);
  QApplication::processEvents();

  // shapes
  xml->writeStartElement("shapes");
  for (int i = 0; i < this->shapes_.size(); i++) {
    xml->writeStartElement("shape");
    xml->writeAttribute("id", QString::number(i));

    if (this->original_present()) {
      xml->writeTextElement("initial_mesh", this->shapes_[i]->get_original_filename_with_path());
    }

    if (this->groomed_present()) {
      QString loc = this->shapes_[i]->get_groomed_filename_with_path();
      if (!defaultDir) {
        loc = QString::fromStdString(dataDir) + "/" +
              this->shapes_[i]->get_groomed_filename();
      }
      xml->writeTextElement("groomed_mesh", loc);
      //try writing the groomed to file
      WriterType::Pointer writer = WriterType::New();
      writer->SetFileName(loc.toStdString());
      writer->SetInput(this->shapes_[i]->get_groomed_image());
      writer->SetUseCompression(true);
      writer->Update();
    }

    if (this->reconstructed_present()) {
      auto name = this->shapes_[i]->get_original_filename().toStdString();
      name = name.substr(0, name.find_last_of(".")) + ".wpts";
      auto name2 = name.substr(0, name.find_last_of(".")) + ".lpts";
      auto loc = this->shapes_[i]->get_original_filename_with_path().toStdString();
      auto pos = loc.find_last_of("/");
      loc = loc.substr(0, pos) + "/";
      auto path = loc + name;
      auto path2 = loc + name2;
      if (!defaultDir) {
        path = dataDir + "/" + name;
        path2 = dataDir + "/" + name2;
      }
      xml->writeTextElement("point_file", QString::fromStdString(path));
      xml->writeTextElement("point_file", QString::fromStdString(path2));
      //try writing to file
      std::ofstream out(path);
      auto points = this->shapes_[i]->get_global_correspondence_points();
      size_t newline = 1;
      for (auto &a : points) {
        out << a << (newline % 3 == 0 ? "\n" : "    ");
        newline++;
      }
      out.close();
      //try writing to file
      std::ofstream out2(path2);
      points = this->shapes_[i]->get_local_correspondence_points();
      newline = 1;
      for (auto &a : points) {
        out2 << a << (newline % 3 == 0 ? "\n" : "    ");
        newline++;
      }
      out2.close();
    }

    xml->writeEndElement(); // shape
    progress.setValue(5 + static_cast<int>(static_cast<double>(i) * 95. /
                                           static_cast<double>(this->shapes_.size())));
    QApplication::processEvents();
    if (progress.wasCanceled()) {
      break;
    }
  }
  xml->writeEndElement(); // shapes

  xml->writeEndElement(); // project
  progress.setValue(100);
  return true;
}

//---------------------------------------------------------------------------
bool Project::load_project(QString filename, std::string& planesFile)
{
  if (!QFile::exists(filename)) {
    QMessageBox::critical(NULL, "ShapeWorksStudio", "File does not exist: " + filename,
                          QMessageBox::Ok);
    return false;
  }
  // clear the project out first
  this->reset();
  this->filename_ = filename;

  // open and parse XML
  TiXmlDocument doc(filename.toStdString().c_str());
  bool loadOkay = doc.LoadFile();
  if (!loadOkay) {
    QString message = "Error: Invalid parameter file" + filename;
    QMessageBox::critical(NULL, "ShapeWorksStudio", message, QMessageBox::Ok);
    return false;
  }

  TiXmlHandle docHandle(&doc);

  TiXmlElement* project_element = docHandle.FirstChild("project").Element();

  bool is_project = project_element != nullptr;

  if (!is_project) {
    return this->load_light_project(filename, planesFile);
  }

  TiXmlNode* shapes_node = project_element->FirstChild("shapes");

  // setup XML
  std::vector<std::string> import_files, groom_files, local_point_files, global_point_files;
  std::string sparseFile, denseFile, goodPtsFile;

  for (TiXmlElement* e = shapes_node->FirstChildElement("shape"); e != NULL;
       e = e->NextSiblingElement("shape")) {
    TiXmlElement* initial_mesh_element = e->FirstChildElement("initial_mesh");
    import_files.push_back(initial_mesh_element->GetText());
    TiXmlElement* groomed_mesh_element = e->FirstChildElement("groomed_mesh");
    if (groomed_mesh_element) {
      groom_files.push_back(groomed_mesh_element->GetText());
    }
    TiXmlElement* point_file_element = e->FirstChildElement("point_file");

    if (point_file_element) {
      std::string filename = point_file_element->GetText();
      if (filename.find(".lpts") != std::string::npos) {
        local_point_files.push_back(filename);
      }
      else if (filename.find(".wpts") != std::string::npos) {
        global_point_files.push_back(filename);
      }

      auto next_point_file = point_file_element->NextSiblingElement("point_file");
      if (next_point_file) {
        filename = next_point_file->GetText();
        if (filename.find(".lpts") != std::string::npos) {
          local_point_files.push_back(filename);
        }
        else if (filename.find(".wpts") != std::string::npos) {
          global_point_files.push_back(filename);
        }
      }
    }
  }

  // now read the preferences and other elements
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
        this->preferences_.set_preference(name.toStdString(), QVariant(value));
      }
    }
  }

  auto display_state = this->preferences_.get_preference(
    "display_state", QString::fromStdString(Visualizer::MODE_ORIGINAL_C)).toStdString();

  this->load_original_files(import_files);
  if (groom_files.size() > 0) {
    this->load_groomed_files(groom_files, 0.5);
  }
  this->load_point_files(local_point_files, true);
  this->load_point_files(global_point_files, false);
  if (!denseFile.empty() && !sparseFile.empty() && !goodPtsFile.empty()) {
    this->mesh_manager_->getSurfaceReconstructor()->readMeanInfo(denseFile, sparseFile,
                                                                 goodPtsFile);
  }
  this->reconstructed_present_ = local_point_files.size() == global_point_files.size() &&
                                 global_point_files.size() > 1;
  this->preferences_.set_preference("display_state", QString::fromStdString(display_state));
  return true;
}

//---------------------------------------------------------------------------
bool Project::load_light_project(QString filename, string &planesFile)
{
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

  /// TODO
  ///this->groupsAvailable = (docHandle.FirstChild("group_ids").Element() != nullptr);

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
      std::cerr << "Found distance transform: " << distance_transform_filename << "\n";
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
  this->load_point_files(local_point_files, true);
  this->load_point_files(global_point_files, false);

  this->reconstructed_present_ = local_point_files.size() == global_point_files.size() &&
                                 global_point_files.size() > 1;


  this->calculate_reconstructed_samples();

  this->preferences_.set_preference("display_state", QString::fromStdString(Visualizer::MODE_RECONSTRUCTION_C));
  this->preferences_.set_preference("tool_state", QString::fromStdString(Project::ANALYSIS_C));
  this->renumber_shapes();

  return true;
}

//---------------------------------------------------------------------------
void Project::load_original_files(std::vector<std::string> file_names)
{
  QProgressDialog progress("Loading images...", "Abort", 0, file_names.size(), this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  //progress.show();
  //progress.setMinimumDuration(2000);

  for (int i = 0; i < file_names.size(); i++) {
    progress.setValue(i);
    QApplication::processEvents();
    if (progress.wasCanceled()) {
      break;
    }
    QSharedPointer<Shape> new_shape = QSharedPointer<Shape>(new Shape);
    new_shape->import_original_image(file_names[i], 0.5);
    if (!this->shapes_.empty()) {
      auto spacing = this->shapes_[0]->get_original_image()->GetSpacing();
      auto spacing_new = new_shape->get_original_image()->GetSpacing();
      if (spacing != spacing_new) {
        emit data_changed();
        this->renumber_shapes();
        this->original_present_ = true;
        throw std::runtime_error(file_names[i] + " does not match spacing with " +
                                 this->shapes_[0]->get_original_filename().toStdString() + "!!!");
      }
      auto sizing = this->shapes_[0]->get_original_image()->GetLargestPossibleRegion();
      auto sizing_new = new_shape->get_original_image()->GetLargestPossibleRegion();
      if (sizing != sizing_new) {
        emit data_changed();
        this->renumber_shapes();
        this->original_present_ = true;
        throw std::runtime_error(file_names[i] + " does not match voxel space with " +
                                 this->shapes_[0]->get_original_filename().toStdString() + "!!!");
      }
    }
    this->shapes_.push_back(new_shape);
  }
  progress.setValue(file_names.size());
  QApplication::processEvents();
  this->renumber_shapes();
  if (file_names.size() > 0) {
    this->original_present_ = true;
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
void Project::load_groomed_images(std::vector<ImageType::Pointer> images, double iso)
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
      this->shapes_.push_back(new_shape);
    }
    this->shapes_[i]->import_groomed_image(images[i], iso);
  }
  progress.setValue(images.size());
  QApplication::processEvents();
  if (images.size() > 0) {
    this->groomed_present_ = true;
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
void Project::load_groomed_files(std::vector<std::string> file_names, double iso)
{
  QProgressDialog progress("Loading groomed images...", "Abort", 0,
                           file_names.size(), this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  //progress.show();
  //progress.setMinimumDuration(2000);

  for (int i = 0; i < file_names.size(); i++) {
    progress.setValue(i);
    QApplication::processEvents();
    if (progress.wasCanceled()) {
      break;
    }
    if (this->shapes_.size() <= i) {
      QSharedPointer<Shape> new_shape = QSharedPointer<Shape>(new Shape);
      this->shapes_.push_back(new_shape);
    }
    this->shapes_[i]->import_groomed_file(QString::fromStdString(file_names[i]), iso);
  }
  progress.setValue(file_names.size());
  QApplication::processEvents();

  if (file_names.size() > 0) {
    this->groomed_present_ = true;
    emit data_changed();
  }
}

//---------------------------------------------------------------------------
bool Project::load_points(std::vector<std::vector<itk::Point<double>>> points, bool local)
{
  //QProgressDialog progress("Loading points...", "Abort", 0, points.size(), this->parent_);
  //progress.setWindowModality(Qt::WindowModal);
  //progress.show();
  //progress.setMinimumDuration(2000);
  //std::cerr << "num file = " << points.size() << "\n";
  for (int i = 0; i < points.size(); i++) {
    QSharedPointer<Shape> shape;
    if (this->shapes_.size() > i) {
      shape = this->shapes_[i];
    }
    else {
      shape = QSharedPointer<Shape>(new Shape);
      this->shapes_.push_back(shape);
    }
    //std::cerr << "Loading points from shape " << i << "\n";
    //progress.setValue(i);
    QApplication::processEvents();
    if (!shape->import_points(points[i], local)) {
      return false;
    }
    //if (progress.wasCanceled()) {
    //  break;
    //}
  }
  //progress.setValue(points.size());
  QApplication::processEvents();

  if (points.size() > 0) {
    emit data_changed();
  }
  return true;
}

//---------------------------------------------------------------------------
void Project::set_reconstructed_present(bool value)
{
  this->reconstructed_present_ = value;
}

//---------------------------------------------------------------------------
bool Project::load_point_files(std::vector<std::string> list, bool local)
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
  }
  progress.setValue(list.size());
  QApplication::processEvents();
  if (list.size() > 0) {
    emit data_changed();
  }
  return true;
}

//---------------------------------------------------------------------------
QVector<QSharedPointer<Shape>> Project::get_shapes()
{
  return this->shapes_;
}

//---------------------------------------------------------------------------
void Project::remove_shapes(QList<int> list)
{
  foreach(int i, list) {
    this->shapes_.erase(this->shapes_.begin() + i);
  }
  this->renumber_shapes();

  emit data_changed();
}

//---------------------------------------------------------------------------
void Project::reset()
{
  this->filename_ = "";

  this->shapes_.clear();

  this->original_present_ = false;
  this->groomed_present_ = false;
  this->reconstructed_present_ = false;

  this->mesh_manager_ = QSharedPointer<MeshManager>(new MeshManager(preferences_));

  connect(this->mesh_manager_.data(), SIGNAL(new_mesh()), this, SLOT(handle_new_mesh()));
  this->handle_clear_cache();
  emit data_changed();
}

//---------------------------------------------------------------------------
bool Project::original_present()
{
  return this->original_present_;
}

//---------------------------------------------------------------------------
bool Project::groomed_present()
{
  return this->groomed_present_;
}

//---------------------------------------------------------------------------
bool Project::reconstructed_present()
{
  return this->reconstructed_present_;
}

//---------------------------------------------------------------------------
void Project::renumber_shapes()
{
  for (int i = 0; i < this->shapes_.size(); i++) {
    this->shapes_[i]->set_id(i + 1);
  }
}

//---------------------------------------------------------------------------
QString Project::get_filename()
{
  return this->filename_;
}

//---------------------------------------------------------------------------
int Project::get_num_shapes()
{
  return this->shapes_.size();
}
