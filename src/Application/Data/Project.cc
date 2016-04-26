#include <Data/Project.h>
#include <Data/Shape.h>
#include <Data/Mesh.h>
#include <Data/MeshManager.h>

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

const std::string Project::DATA_C("data");
const std::string Project::GROOM_C("groom");
const std::string Project::OPTIMIZE_C("optimize");
const std::string Project::ANALYSIS_C("analysis");

//---------------------------------------------------------------------------
Project::Project(Preferences& prefs) : preferences_(prefs)
{
  this->parent_ = NULL;
  this->reset();
  connect(this->mesh_manager_.data(), SIGNAL(new_mesh()), this, SLOT(handle_new_mesh()));
}

//---------------------------------------------------------------------------
Project::~Project()
{
}

//---------------------------------------------------------------------------
void Project::handle_new_mesh() {
  emit update_display();
}

//---------------------------------------------------------------------------
void Project::handle_clear_cache() {
  this->mesh_manager_->clear_cache();
  this->calculate_reconstructed_samples();
}

//---------------------------------------------------------------------------
void Project::calculate_reconstructed_samples() {
  if (!this->reconstructed_present_) return;
  this->preferences_.set_preference("cache_enabled", false);
  for (int i = 0; i < this->shapes_.size(); i++) {
    if (this->shapes_.at(i)->get_local_correspondence_points().size() > 0) {
      this->shapes_.at(i)->set_reconstructed_mesh(
        this->mesh_manager_->getMesh(
        this->shapes_.at(i)->get_local_correspondence_points()));
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
bool Project::save_project(std::string fname, std::string dataDir) {
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

  // shapes
  xml->writeStartElement("shapes");
  for (int i = 0; i < this->shapes_.size(); i++) {
    xml->writeStartElement("shape");
    xml->writeAttribute("id", QString::number(i));

    if (this->original_present()) {
      xml->writeTextElement("initial_mesh", this->shapes_[i]->get_original_filename_with_path());
    }

    if (this->groomed_present())  {
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
        out << a << (newline % 3 == 0 ? "\n" : "    ");
        newline++;
      }
      out2.close();
    }

    xml->writeEndElement(); // shape
  }
  xml->writeEndElement(); // shapes

  xml->writeEndElement(); // project

  return true;
}

//---------------------------------------------------------------------------
bool Project::load_project(QString filename) {
  if (!QFile::exists(filename)) {
    QMessageBox::critical(NULL, "ShapeWorksStudio", "File does not exist: " + filename, QMessageBox::Ok);
    return false;
  }
  // clear the project out first
  this->reset();
  this->filename_ = filename;
  // open file
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::warning(0, "Unable to open file", "Error opening file: " + filename);
    return false;
  }
  // setup XML
  QSharedPointer<QXmlStreamReader> xml = QSharedPointer<QXmlStreamReader>(new QXmlStreamReader());
  xml->setDevice(&file);
  QStringList import_files;
  QStringList groomed_files;
  QStringList point_files;
  QString preference_file;

  while (!xml->atEnd() && !xml->hasError()){
    QXmlStreamReader::TokenType token = xml->readNext();
    if (token == QXmlStreamReader::StartDocument) {
      continue;
    }
    if (token == QXmlStreamReader::StartElement)  {
      auto name = xml->name().toString().toStdString();
      if (xml->name() == "project" ||
        xml->name() == "shapes" ||
        xml->name() == "shape") {
        continue;
      }
      auto val = xml->readElementText().toStdString();
      if (xml->name() == "initial_mesh"){
        import_files << QString::fromStdString(val);
      } else if (xml->name() == "groomed_mesh") {
        groomed_files << QString::fromStdString(val);
      } else if (xml->name() == "point_file")  {
        point_files << QString::fromStdString(val);
      } else {
        this->preferences_.set_preference(name, QVariant(QString::fromStdString(val)));
      }
    }
  }
  auto display_state = this->preferences_.get_preference(
    "display_state", QString::fromStdString(Visualizer::MODE_ORIGINAL_C)).toStdString();

  /* Error handling. */
  if (xml->hasError()) {
    QMessageBox::critical(NULL, "ShapeWorksStudio", xml->errorString(), QMessageBox::Ok);
    return false;
  }
  this->load_original_files(import_files);
  std::vector<std::string> groom_list;
  for (auto a : groomed_files) {
    groom_list.push_back(a.toStdString());
  }
  this->load_groomed_files(groom_list, 0.5);
  std::vector<std::string> localpointlist, globalpointlist;
  for (auto a : point_files) {
    if (a.toStdString().find(".lpts") != std::string::npos) {
      localpointlist.push_back(a.toStdString());
    } else if (a.toStdString().find(".wpts") != std::string::npos) {
      globalpointlist.push_back(a.toStdString());
    }
  }
  this->load_point_files(localpointlist, true);
  this->load_point_files(globalpointlist, false);
  this->preferences_.set_preference("display_state", QString::fromStdString(display_state));
  return true;
}

//---------------------------------------------------------------------------
void Project::load_original_files(QStringList file_names) {
  QProgressDialog progress("Loading images...", "Abort", 0, file_names.size(), this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  progress.show();
  progress.setMinimumDuration(2000);

  for (int i = 0; i < file_names.size(); i++) {
    progress.setValue(i);
    QApplication::processEvents();
    if (progress.wasCanceled()) {
      break;
    }
    QSharedPointer<Shape> new_shape = QSharedPointer<Shape>(new Shape);
    auto test = file_names[i].toStdString();
    new_shape->import_original_image(file_names[i], 0.5);
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
void Project::load_groomed_images(std::vector<ImageType::Pointer> images, double iso) {
  QProgressDialog progress("Loading groomed images...", "Abort", 0, images.size(), this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  progress.show();
  progress.setMinimumDuration(2000);

  for (int i = 0; i < images.size(); i++) {
    progress.setValue(i);
    QApplication::processEvents();
    if (progress.wasCanceled())  {
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
  QProgressDialog progress("Loading groomed images...", "Abort", 0, file_names.size(), this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  progress.show();
  progress.setMinimumDuration(2000);

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
bool Project::load_points(std::vector<std::vector<itk::Point<float> > > points, bool local)
{
  QProgressDialog progress("Loading points...", "Abort", 0, points.size(), this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  progress.show();
  progress.setMinimumDuration(2000);
  std::cerr << "num file = " << points.size() << "\n";
  for (int i = 0; i < points.size(); i++) {
    QSharedPointer<Shape> shape;
    if (this->shapes_.size() > i) {
      shape = this->shapes_[i];
    } else {
      shape = QSharedPointer<Shape>(new Shape);
      this->shapes_.push_back(shape);
    }
    std::cerr << "Loading points from shape " << i << "\n";
    progress.setValue(i);
    QApplication::processEvents();
    if (!shape->import_points(points[i], local)) {
      return false;
    }
    if (progress.wasCanceled()) {
      break;
    }
  }
  progress.setValue(points.size());
  QApplication::processEvents();

  if (points.size() > 0) {
    this->reconstructed_present_ = true;
    emit data_changed();
  }
  return true;
}

//---------------------------------------------------------------------------
bool Project::load_point_files(std::vector<std::string> list, bool local )
{
  QProgressDialog progress("Loading point files...", "Abort", 0, list.size(), this->parent_);
  progress.setWindowModality(Qt::WindowModal);
  progress.show();
  progress.setMinimumDuration(2000);
  for (int i = 0; i < list.size(); i++) {
    progress.setValue(i);
    QApplication::processEvents();
    if (progress.wasCanceled()) {
      break;
    }
    QSharedPointer<Shape> shape;
    if (this->shapes_.size() > i) {
      shape = this->shapes_[i];
    } else {
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
      } else {
        if (!shape->import_local_point_file(fname)) {
          return false;
        }
      }
    }
  }
  progress.setValue(list.size());
  QApplication::processEvents();
  if (list.size() > 0) {
    this->reconstructed_present_ = true;
    emit data_changed();
  }
  return true;
}

//---------------------------------------------------------------------------
QVector<QSharedPointer<Shape> > Project::get_shapes()
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
void Project::reset() {
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
  for (int i = 0; i < this->shapes_.size(); i++)
  {
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
