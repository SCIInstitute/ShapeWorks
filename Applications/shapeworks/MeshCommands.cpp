#include "Commands.h"
#include "MeshUtils.h"

namespace shapeworks {

///////////////////////////////////////////////////////////////////////////////
// ReadMesh
///////////////////////////////////////////////////////////////////////////////
void ReadMesh::buildParser()
{
  const std::string prog = "read-mesh";
  const std::string desc = "reads a mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of file to read.");

  Command::buildParser();
}

bool ReadMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string filename = options["name"];
  if (filename.length() == 0) {
    std::cerr << "readmesh error: no filename specified, must pass `--name <filename>`\n";
    return false;
  }

  try {
    sharedData.mesh = std::make_unique<Mesh>(filename);
    return true;
  } catch (std::exception &e) {
    std::cerr << "exception while reading " << filename << ": " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "unknown exception while reading " << filename << std::endl;
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// WriteMesh
///////////////////////////////////////////////////////////////////////////////
void WriteMesh::buildParser()
{
  const std::string prog = "write-mesh";
  const std::string desc = "writes the current mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of file to write.");

  Command::buildParser();
}

bool WriteMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string filename = options["name"];
  if (filename.length() == 0) {
    std::cerr << "writemesh error: no filename specified, must pass `--name <filename>`\n";
    return false;
  }

  sharedData.mesh->write(filename);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// MeshInfo
///////////////////////////////////////////////////////////////////////////////
void MeshInfo::buildParser()
{
  const std::string prog = "mesh-info";
  const std::string desc = "prints requested mesh center, center of mass, number of vertices, number of faces and bounding box [default: prints everything]";
  parser.prog(prog).description(desc);

  parser.add_option("--vertices").action("store_true").set_default(false).help("Whether to display number of vertices [default: true].");
  parser.add_option("--faces").action("store_true").set_default(false).help("Whether to display number of faces [default: true].");
  parser.add_option("--center").action("store_true").set_default(false).help("Whether to display center [default: true].");
  parser.add_option("--centerofmass").action("store_true").set_default(false).help("Whether to display center of mass [default: true].");
  parser.add_option("--boundingbox").action("store_true").set_default(false).help("Whether to display bounding box [default: true].");
  parser.add_option("--fieldnames").action("store_true").set_default(false).help("Whether to display field names [default: true].");

  Command::buildParser();
}

bool MeshInfo::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  bool vertices = static_cast<bool>(options.get("vertices"));
  bool faces = static_cast<bool>(options.get("faces"));
  bool center = static_cast<bool>(options.get("center"));
  bool centerofmass = static_cast<bool>(options.get("centerofmass"));
  bool boundingbox = static_cast<bool>(options.get("boundingbox"));
  bool fieldnames = static_cast<bool>(options.get("fieldnames"));

  // by default: print everything
  if (options.num_set() == 0)
    vertices = faces = center = centerofmass = boundingbox = fieldnames = true;

  if (vertices)
    std::cout << "number of points:      " << sharedData.mesh->numPoints() << std::endl;
  if (faces)
    std::cout << "number of faces:       " << sharedData.mesh->numFaces() << std::endl;
  if (center)
    std::cout << "center:                " << sharedData.mesh->center() << std::endl;
  if (centerofmass)
    std::cout << "center of mass (0,1]:  " << sharedData.mesh->centerOfMass() << std::endl;
  if (boundingbox)
    std::cout << "bounding box:          " << sharedData.mesh->boundingBox() << std::endl;
  if (fieldnames) {
    auto fields = sharedData.mesh->getFieldNames();
    std::cout << "field names: ";
    for (auto field : fields) std::cout << field << " ";
    std::cout << std::endl;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
/// Coverage
///////////////////////////////////////////////////////////////////////////////
void Coverage::buildParser()
{
  const std::string prog = "coverage";
  const std::string desc = "creates mesh of coverage between two meshes";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Path to other mesh with which to create coverage.");
  parser.add_option("--allowbackintersections").action("store_true").set_default(false).help("Allow back-intersections in coverage calculation [default: true].");
  parser.add_option("--anglethreshold").action("store").type("double").set_default(0.0).help("This checks the cosine between the ray’s direction vector (e1) and the normal at the intersection point (e2) [default: %default].");
  parser.add_option("--backsearchradius").action("store").type("double").set_default(0.0).help("Max distance of a back-intersection [default: %default].");

  Command::buildParser();
}

bool Coverage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  const std::string otherMesh(static_cast<std::string>(options.get("name")));
  bool allowbackintersections = static_cast<bool>(options.get("allowbackintersections"));
  double anglethreshold = static_cast<double>(options.get("anglethreshold"));
  double backsearchradius = static_cast<double>(options.get("backsearchradius"));

  if (otherMesh.length() == 0)
  {
    std::cerr << "Must specify path to other mesh\n";
    return false;
  }

  sharedData.mesh->coverage(Mesh(otherMesh), allowbackintersections, anglethreshold, backsearchradius);
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// Smooth
///////////////////////////////////////////////////////////////////////////////
void Smooth::buildParser()
{
  const std::string prog = "smooth";
  const std::string desc = "applies laplacian smoothing";
  parser.prog(prog).description(desc);

  parser.add_option("--iterations").action("store").type("int").set_default(0).help("Number of iterations [default: %default].");
  parser.add_option("--relaxation").action("store").type("double").set_default(0.0).help("Amount of displacement for a vertex to move in each iteration [default: %default].");

  Command::buildParser();
}

bool Smooth::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  int iterations = static_cast<int>(options.get("iterations"));
  double relaxation = static_cast<double>(options.get("relaxation"));

  sharedData.mesh->smooth(iterations, relaxation);
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// Decimate
///////////////////////////////////////////////////////////////////////////////
void Decimate::buildParser()
{
  const std::string prog = "decimate";
  const std::string desc = "brief description of command"; // TODO: add description
  parser.prog(prog).description(desc);

  parser.add_option("--reduction").action("store").type("double").set_default(0.0).help("Description of optionName [default: %default]."); // TODO: add description
  parser.add_option("--angle").action("store").type("double").set_default(0.0).help("Angle in degrees [default: %default].");
  parser.add_option("--preservetopology").action("store").type("bool").set_default(false).help("Whether to preserve topology [default: false].");

  Command::buildParser();
}

bool Decimate::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  double reduction = static_cast<double>(options.get("reduction"));
  double angle = static_cast<double>(options.get("angle"));
  bool preservetopology = static_cast<bool>(options.get("preservetopology"));

  sharedData.mesh->decimate(reduction, angle, preservetopology);
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// InvertNormals
///////////////////////////////////////////////////////////////////////////////
void InvertNormals::buildParser()
{
  const std::string prog = "invert-normal";
  const std::string desc = "flips the normal";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

bool InvertNormals::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  sharedData.mesh->invertNormals();
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// ReflectMesh
///////////////////////////////////////////////////////////////////////////////
void ReflectMesh::buildParser()
{
  const std::string prog = "reflect-mesh";
  const std::string desc = "reflect meshes with respect to a specified center and specific axis";
  parser.prog(prog).description(desc);

  parser.add_option("--axis").action("store").type("string").set_default("").help("Axis along which to reflect (X, Y, or Z).");
  parser.add_option("--originx", "-x").action("store").type("double").set_default(0.0).help("Origin about which reflection occurs in x-direction [default: %default].");
  parser.add_option("--originy", "-y").action("store").type("double").set_default(0.0).help("Origin about which reflection occurs in y-direction [default: %default].");
  parser.add_option("--originz", "-z").action("store").type("double").set_default(0.0).help("Origin about which reflection occurs in z-direction [default: %default].");

  Command::buildParser();
}

bool ReflectMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string axis_str(static_cast<std::string>(options.get("axis")));
  Axis axis(toAxis(axis_str));

  double originX = static_cast<double>(options.get("originx"));
  double originY = static_cast<double>(options.get("originy"));
  double originZ = static_cast<double>(options.get("originz"));

  if (!axis_is_valid(axis))
  {
    std::cerr << "Must specify a valid axis (X, Y, or Z)\n";
    return false;
  }
  else
  {
    sharedData.mesh->reflect(axis, makeVector({originX, originY, originZ}));
    return sharedData.validMesh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// TransformMesh
///////////////////////////////////////////////////////////////////////////////
void TransformMesh::buildParser()
{
  const std::string prog = "transform-mesh";
  const std::string desc = "transform mesh to target mesh using specified method (default: iterative closest point (ICP) 3D rigid registration)";
  parser.prog(prog).description(desc);

  parser.add_option("--target").action("store").type("string").set_default("").help("Filename of target mesh.");
  std::list<std::string> aligns{"rigid", "similarity", "affine"};
  parser.add_option("--type").action("store").type("choice").choices(aligns.begin(), aligns.end()).set_default("similarity").help("Alignment type to use [default: %default].");
  std::list<std::string> methods{"icp"};
  parser.add_option("--method").action("store").type("choice").choices(methods.begin(), methods.end()).set_default("icp").help("Method used to compute transform [default: %default].");
  parser.add_option("--iterations").action("store").type("unsigned").set_default(10).help("Number of iterations run [default: %default].");

  Command::buildParser();
}

bool TransformMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string targetMesh = static_cast<std::string>(options.get("target"));
  unsigned iterations = static_cast<unsigned>(options.get("iterations"));
  std::string alignopt(options.get("type"));
  Mesh::AlignmentType align;
  if (alignopt == "rigid")
    align = Mesh::Rigid;
  else if (alignopt == "similarity")
    align = Mesh::Similarity;
  else if (alignopt == "affine")
    align = Mesh::Affine;
  else {
    std::cerr << "no such alignment type: " << alignopt << std::endl;
    return false;
  }

  std::string methodopt(options.get("method"));
  XFormType method{IterativeClosestPoint};
  if (methodopt != "icp") {
    std::cerr << "no such transform type: " << methodopt << std::endl;
    return false;
  }

  if (targetMesh == "")
  {
    std::cerr << "Must specify a target mesh\n";
    return false;
  }
  else
  {
    Mesh target(targetMesh);
    MeshTransform transform(sharedData.mesh->createTransform(target, method, align, iterations));
    sharedData.mesh->applyTransform(transform);
    return sharedData.validMesh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// FillHoles
///////////////////////////////////////////////////////////////////////////////
void FillHoles::buildParser()
{
  const std::string prog = "fill-holes";
  const std::string desc = "finds holes in a mesh and closes them";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

bool FillHoles::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }
  
  sharedData.mesh->fillHoles();
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// ProbeVolume
///////////////////////////////////////////////////////////////////////////////
void ProbeVolume::buildParser()
{
  const std::string prog = "probe-volume";
  const std::string desc = "probe feature volumes at each mesh vertex and output vtk meshes with scalar field defined based on such probing process";
  parser.prog(prog).description(desc);

  parser.add_option("--image").action("store").type("string").set_default("").help("Path of image.");

  Command::buildParser();
}

bool ProbeVolume::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string filename = static_cast<std::string>(options.get("image"));

  if (filename == "")
  {
    std::cerr << "Must specify an image\n";
    return false;
  }

  Image img(filename);
  
  sharedData.mesh->probeVolume(img);
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// ClipMesh
///////////////////////////////////////////////////////////////////////////////
void ClipMesh::buildParser()
{
  const std::string prog = "clip-mesh";
  const std::string desc = "clips mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--nx").action("store").type("double").set_default(0.0).help("Value of normal.x for cutting plane [default: %default].");
  parser.add_option("--ny").action("store").type("double").set_default(0.0).help("Value of normal.y for cutting plane [default: %default].");
  parser.add_option("--nz").action("store").type("double").set_default(0.0).help("Value of normal.z for cutting plane [default: %default].");
  parser.add_option("--ox").action("store").type("double").set_default(0.0).help("Value of origin.x for cutting plane [default: %default].");
  parser.add_option("--oy").action("store").type("double").set_default(0.0).help("Value of origin.y for cutting plane [default: %default].");
  parser.add_option("--oz").action("store").type("double").set_default(0.0).help("Value of origin.z for cutting plane [default: %default].");

  Command::buildParser();
}

bool ClipMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  Vector normal{makeVector({static_cast<double>(options.get("nx")),
                            static_cast<double>(options.get("ny")),
                            static_cast<double>(options.get("nz"))})};

  Point origin({static_cast<double>(options.get("ox")),
                static_cast<double>(options.get("oy")),
                static_cast<double>(options.get("oz"))});
  
  sharedData.mesh->clip(makePlane(normal, origin));
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// TranslateMesh
///////////////////////////////////////////////////////////////////////////////
void TranslateMesh::buildParser()
{
  const std::string prog = "translate-mesh";
  const std::string desc = "translates mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--tx", "-x").action("store").type("double").set_default(0).help("X distance.");
  parser.add_option("--ty", "-y").action("store").type("double").set_default(0).help("Y distance.");
  parser.add_option("--tz", "-z").action("store").type("double").set_default(0).help("Z distance.");

  Command::buildParser();
}

bool TranslateMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  double tx = static_cast<double>(options.get("tx"));
  double ty = static_cast<double>(options.get("ty"));
  double tz = static_cast<double>(options.get("tz"));
  
  sharedData.mesh->translate(makeVector({tx, ty, tz}));
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// ScaleMesh
///////////////////////////////////////////////////////////////////////////////
void ScaleMesh::buildParser()
{
  const std::string prog = "scale-mesh";
  const std::string desc = "scales mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--sx", "-x").action("store").type("double").set_default(1.0).help("X scale.");
  parser.add_option("--sy", "-y").action("store").type("double").set_default(1.0).help("Y scale.");
  parser.add_option("--sz", "-z").action("store").type("double").set_default(1.0).help("Z scale.");

  Command::buildParser();
}

bool ScaleMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  double sx = static_cast<double>(options.get("sx"));
  double sy = static_cast<double>(options.get("sy"));
  double sz = static_cast<double>(options.get("sz"));

  if (sx == 0 || sy == 0 || sz == 0)
  {
    std::cerr << "Error: cannot scale by 0 in any dimension\n";
    return false;
  }
  else
  {
    sharedData.mesh->scale(makeVector({sx, sy, sz}));
    return sharedData.validMesh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// BoundingBoxMesh
///////////////////////////////////////////////////////////////////////////////
void BoundingBoxMesh::buildParser()
{
  const std::string prog = "bounding-box-mesh";
  const std::string desc = "compute bounding box of mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--names").action("store").type("multistring").set_default("").help("Paths to meshes (must be followed by `--`), ex: \"bounding-box-mesh --names *.vtk -- --center 1\")");
  parser.add_option("--center").action("store").type("bool").set_default(false).help("Flag for centering [default: false].");

  Command::buildParser();
}

bool BoundingBoxMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::vector<std::string> filenames = options.get("names");
  bool center = static_cast<bool>(options.get("center"));

  sharedData.region = MeshUtils::boundingBox(filenames, center);
  std::cout << "Bounding box:\n" << sharedData.region;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Distance
///////////////////////////////////////////////////////////////////////////////
void Distance::buildParser()
{
  const std::string prog = "distance";
  const std::string desc = "computes the distance between two meshes, printing the largest distance between any point from source to target, target to source, and the Hausdorff distance (the largest of these two)";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Filename of other mesh.");
  std::list<std::string> methods{"point-to-point", "point-to-cell"};
  parser.add_option("--method").action("store").type("choice").choices(methods.begin(), methods.end()).set_default("point-to-point").help("Method used to compute distance [default: %default].");
  parser.add_option("--summary").action("store").type("bool").set_default(false).help("Print largest distance of any point in mesh to target [default: %default].");

  Command::buildParser();
}

bool Distance::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  bool summary = static_cast<bool>(options.get("summary"));

  std::string methodopt(options.get("method"));
  auto method{Mesh::POINT_TO_POINT};
  if (methodopt == "point-to-point") method = Mesh::POINT_TO_POINT;
  else if (methodopt == "point-to-cell") method = Mesh::POINT_TO_CELL;
  else {
    std::cerr << "no such distance method: " << methodopt << std::endl;
    return false;
  }

  std::string otherMesh = static_cast<std::string>(options.get("name"));
  if (otherMesh == "")
  {
    std::cerr << "Must specify a mesh\n";
    return false;
  }

  Mesh other(otherMesh);
  sharedData.mesh->distance(other, method);

  if (summary)
  {
    auto range = sharedData.mesh->getFieldRange("distance");
    auto dist = std::max(range[0], range[1]);
    std::cout << "Maximum distance to target mesh: " << dist << std::endl;
  }

  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// FixMesh
///////////////////////////////////////////////////////////////////////////////
void FixMesh::buildParser()
{
  const std::string prog = "fix-mesh";
  const std::string desc = "quality control meshes";
  parser.prog(prog).description(desc);

  parser.add_option("--smoothbefore").action("store").type("bool").set_default(true).help("Perform laplacian smoothing before decimation [default: true].");
  parser.add_option("--smoothafter").action("store").type("bool").set_default(true).help("Perform laplacian smoothing after decimation [default: true].");
  parser.add_option("--lambda").action("store").type("double").set_default(0.5).help("Laplacian smoothing lambda [default: %default].");
  parser.add_option("--iterations").action("store").type("int").set_default(1).help("Number of laplacian smoothing iterations [default: %default].");
  parser.add_option("--decimate").action("store").type("bool").set_default(true).help("Perform mesh decimation [default: true].");
  parser.add_option("--percentage").action("store").type("double").set_default(0.5).help("Percentage of target number of clusters/vertices [default: %default].");

  Command::buildParser();
}

bool FixMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  bool smoothbefore = static_cast<bool>(options.get("smoothbefore"));
  bool smoothafter = static_cast<bool>(options.get("smoothafter"));
  double lambda = static_cast<double>(options.get("lambda"));
  int iterations = static_cast<int>(options.get("iterations"));
  bool decimate = static_cast<bool>(options.get("decimate"));
  double percentage = static_cast<double>(options.get("percentage"));

  sharedData.mesh->fix(smoothbefore, smoothafter, lambda, iterations, decimate, percentage);
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// ClipClosedSurface
///////////////////////////////////////////////////////////////////////////////
void ClipClosedSurface::buildParser()
{
  const std::string prog = "clip-closed-surface";
  const std::string desc = "clips mesh resulting in a closed surface";
  parser.prog(prog).description(desc);

  parser.add_option("--nx").action("store").type("double").set_default(0.0).help("Value of normal.x for cutting plane [default: %default].");
  parser.add_option("--ny").action("store").type("double").set_default(0.0).help("Value of normal.y for cutting plane [default: %default].");
  parser.add_option("--nz").action("store").type("double").set_default(0.0).help("Value of normal.z for cutting plane [default: %default].");
  parser.add_option("--ox").action("store").type("double").set_default(0.0).help("Value of origin.x for cutting plane [default: %default].");
  parser.add_option("--oy").action("store").type("double").set_default(0.0).help("Value of origin.y for cutting plane [default: %default].");
  parser.add_option("--oz").action("store").type("double").set_default(0.0).help("Value of origin.z for cutting plane [default: %default].");

  Command::buildParser();
}

bool ClipClosedSurface::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  Vector normal{makeVector({static_cast<double>(options.get("nx")),
                            static_cast<double>(options.get("ny")),
                            static_cast<double>(options.get("nz"))})};

  Point origin({static_cast<double>(options.get("ox")),
                static_cast<double>(options.get("oy")),
                static_cast<double>(options.get("oz"))});
  
  sharedData.mesh->clipClosedSurface(makePlane(normal, origin));
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// SetFieldValue
///////////////////////////////////////////////////////////////////////////////
void SetFieldValue::buildParser()
{
  const std::string prog = "set-field-value";
  const std::string desc = "sets value of element at index in given field of mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of scalar field.");
  parser.add_option("--index", "-i").action("store").type("int").set_default(0).help("index of value to return [default: %default].");
  parser.add_option("--value").action("store").type("double").set_default(0).help("value to be set [default: %default].");

  Command::buildParser();
}

bool SetFieldValue::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string name = static_cast<std::string>(options.get("name"));
  int index = static_cast<int>(options.get("index"));
  double value = static_cast<double>(options.get("value"));

  sharedData.mesh->setFieldValue(name, index, value);
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// GetFieldValue
///////////////////////////////////////////////////////////////////////////////
void GetFieldValue::buildParser()
{
  const std::string prog = "get-field-value";
  const std::string desc = "prints value of element at index in given field of mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of scalar field.");
  parser.add_option("--index", "-i").action("store").type("int").set_default(0).help("index of value to return [default: %default].");

  Command::buildParser();
}

bool GetFieldValue::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string name = static_cast<std::string>(options.get("name"));
  int index = static_cast<int>(options.get("index"));

  std::cout << sharedData.mesh->getFieldValue(name, index) << "\n";
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// FieldRange
///////////////////////////////////////////////////////////////////////////////
void FieldRange::buildParser()
{
  const std::string prog = "field-range";
  const std::string desc = "prints the range of the given field";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of scalar field.");

  Command::buildParser();
}

bool FieldRange::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string name = static_cast<std::string>(options.get("name"));

  std::vector<double> range = sharedData.mesh->getFieldRange(name);
  std::cout << "[" << range[0] << "," << range[1] << "]\n";
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// FieldMean
///////////////////////////////////////////////////////////////////////////////
void FieldMean::buildParser()
{
  const std::string prog = "field-mean";
  const std::string desc = "prints the mean of the given field";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of scalar field.");

  Command::buildParser();
}

bool FieldMean::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string name = static_cast<std::string>(options.get("name"));

  std::cout << sharedData.mesh->getFieldMean(name) << "\n";
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// FieldStd
///////////////////////////////////////////////////////////////////////////////
void FieldStd::buildParser()
{
  const std::string prog = "field-std";
  const std::string desc = "prints the standard deviation of the given field";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of scalar field.");

  Command::buildParser();
}

bool FieldStd::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string name = static_cast<std::string>(options.get("name"));

  std::cout << sharedData.mesh->getFieldStd(name) << "\n";
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// FieldNames
///////////////////////////////////////////////////////////////////////////////
void FieldNames::buildParser()
{
  const std::string prog = "field-names";
  const std::string desc = "prints all the field names present in mesh";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

bool FieldNames::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::vector<std::string> fields = sharedData.mesh->getFieldNames();
  for (int i=0; i<fields.size(); i++)
    std::cout << fields[i] << "\n";
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// MeshToImage
///////////////////////////////////////////////////////////////////////////////
void MeshToImage::buildParser()
{
  const std::string prog = "mesh-to-image";
  const std::string desc = "converts current mesh to a binary segmentation image";
  parser.prog(prog).description(desc);

  parser.add_option("--spacex").action("store").type("double").set_default(1.0).help("Spacing of output image in x-direction [default: %default].");
  parser.add_option("--spacey").action("store").type("double").set_default(1.0).help("Spacing of output image in y-direction [default: %default].");
  parser.add_option("--spacez").action("store").type("double").set_default(1.0).help("Spacing of output image in z-direction [default: %default].");
  parser.add_option("--sizex").action("store").type("unsigned").set_default(0).help("Size of output image in x-direction [default: one pixel per unit of distance in mesh].");
  parser.add_option("--sizey").action("store").type("unsigned").set_default(0).help("Size of output image in y-direction [default: one pixel per unit of distance in mesh].");
  parser.add_option("--sizez").action("store").type("unsigned").set_default(0).help("Size of output image in z-direction [default: one pixel per unit of distance in mesh].");
  parser.add_option("--originx").action("store").type("double").set_default(-1.0).help("Origin of output image in x-direction [default: current origin].");
  parser.add_option("--originy").action("store").type("double").set_default(-1.0).help("Origin of output image in y-direction [default: current origin].");
  parser.add_option("--originz").action("store").type("double").set_default(-1.0).help("Origin of output image in z-direction [default: current origin].");

  Command::buildParser();
}

bool MeshToImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  double spaceX = static_cast<double>(options.get("spacex"));
  double spaceY = static_cast<double>(options.get("spacey"));
  double spaceZ = static_cast<double>(options.get("spacez"));
  unsigned sizeX = static_cast<unsigned>(options.get("sizex"));
  unsigned sizeY = static_cast<unsigned>(options.get("sizey"));
  unsigned sizeZ = static_cast<unsigned>(options.get("sizez"));
  double originX = static_cast<double>(options.get("originx"));
  double originY = static_cast<double>(options.get("originy"));
  double originZ = static_cast<double>(options.get("originz"));

  Vector3 spacing(makeVector({spaceX,spaceY,spaceZ}));
  Dims size({sizeX,sizeY, sizeZ});
  Point3 origin({originX,originY,originZ});

  sharedData.image = sharedData.mesh->toImage(spacing, size, origin);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// MeshToDT
///////////////////////////////////////////////////////////////////////////////
void MeshToDT::buildParser()
{
  const std::string prog = "mesh-to-dt";
  const std::string desc = "converts current mesh to distance transform";
  parser.prog(prog).description(desc);

  parser.add_option("--spacex").action("store").type("double").set_default(1.0).help("Spacing of output image in x-direction [default: %default].");
  parser.add_option("--spacey").action("store").type("double").set_default(1.0).help("Spacing of output image in y-direction [default: %default].");
  parser.add_option("--spacez").action("store").type("double").set_default(1.0).help("Spacing of output image in z-direction [default: %default].");
  parser.add_option("--sizex").action("store").type("unsigned").set_default(0).help("Size of output image in x-direction [default: one pixel per unit of distance in mesh].");
  parser.add_option("--sizey").action("store").type("unsigned").set_default(0).help("Size of output image in y-direction [default: one pixel per unit of distance in mesh].");
  parser.add_option("--sizez").action("store").type("unsigned").set_default(0).help("Size of output image in z-direction [default: one pixel per unit of distance in mesh].");
  parser.add_option("--originx").action("store").type("double").set_default(-1.0).help("Origin of output image in x-direction [default: current origin].");
  parser.add_option("--originy").action("store").type("double").set_default(-1.0).help("Origin of output image in y-direction [default: current origin].");
  parser.add_option("--originz").action("store").type("double").set_default(-1.0).help("Origin of output image in z-direction [default: current origin].");

  Command::buildParser();
}

bool MeshToDT::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  double spaceX = static_cast<double>(options.get("spacex"));
  double spaceY = static_cast<double>(options.get("spacey"));
  double spaceZ = static_cast<double>(options.get("spacez"));
  unsigned sizeX = static_cast<unsigned>(options.get("sizex"));
  unsigned sizeY = static_cast<unsigned>(options.get("sizey"));
  unsigned sizeZ = static_cast<unsigned>(options.get("sizez"));
  double originX = static_cast<double>(options.get("originx"));
  double originY = static_cast<double>(options.get("originy"));
  double originZ = static_cast<double>(options.get("originz"));

  Vector3 spacing(makeVector({spaceX,spaceY,spaceZ}));
  Dims size({sizeX,sizeY, sizeZ});
  Point3 origin({originX,originY,originZ});

  sharedData.image = sharedData.mesh->toDistanceTransform(spacing, size, origin);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// CompareMesh
///////////////////////////////////////////////////////////////////////////////
void CompareMesh::buildParser()
{
  const std::string prog = "compare-mesh";
  const std::string desc = "compare two meshes";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Compare this mesh with another.");

  Command::buildParser();
}

bool CompareMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string filename = options["name"];
  if (filename.length() == 0) {
    std::cerr << "compare-mesh error: no filename specified with which to compare, must pass `--name <filename>`\n";
    return false;
  }

  if (sharedData.mesh->compare(Mesh(filename)))
  {
    std::cout << "compare success\n";
    return true;
  }
  else
  {
    std::cout << "compare failure\n";
    return false;
  }
}

} //shapeworks
