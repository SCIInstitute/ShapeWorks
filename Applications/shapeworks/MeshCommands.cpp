#include "Commands.h"
#include "MeshUtils.h"
#include "MeshWarper.h"
#include <boost/filesystem.hpp>

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
  bool centerOfMass = static_cast<bool>(options.get("centerofmass"));
  bool boundingBox = static_cast<bool>(options.get("boundingbox"));
  bool fieldNames = static_cast<bool>(options.get("fieldnames"));

  // by default: print everything
  if (options.num_set() == 0)
    vertices = faces = center = centerOfMass = boundingBox = fieldNames = true;

  if (vertices)
    std::cout << "number of points:      " << sharedData.mesh->numPoints() << std::endl;
  if (faces)
    std::cout << "number of faces:       " << sharedData.mesh->numFaces() << std::endl;
  if (center)
    std::cout << "center:                " << sharedData.mesh->center() << std::endl;
  if (centerOfMass)
    std::cout << "center of mass (0,1]:  " << sharedData.mesh->centerOfMass() << std::endl;
  if (boundingBox)
    std::cout << "bounding box:          " << sharedData.mesh->boundingBox() << std::endl;
  if (fieldNames) {
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
  parser.add_option("--allowbackintersections").action("store").type("bool").set_default(true).help("Allow back-intersections in coverage calculation [default: true].");
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
  bool allowBackIntersections = static_cast<bool>(options.get("allowbackintersections"));
  double angleThreshold = static_cast<double>(options.get("anglethreshold"));
  double backSearchRadius = static_cast<double>(options.get("backsearchradius"));

  if (otherMesh.length() == 0)
  {
    std::cerr << "Must specify path to other mesh\n";
    return false;
  }

  sharedData.mesh->coverage(Mesh(otherMesh), allowBackIntersections, angleThreshold, backSearchRadius);
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
// Smooth Sinc
///////////////////////////////////////////////////////////////////////////////
void SmoothSinc::buildParser()
{
  const std::string prog = "smooth-sinc";
  const std::string desc = "applies windowed sinc smoothing";
  parser.prog(prog).description(desc);

  parser.add_option("--iterations").action("store").type("int").set_default(0).help("Number of iterations [default: %default].");
  parser.add_option("--passband").action("store").type("double").set_default(0.0).help("Set the passband value for the windowed sinc filter [default: %default].");

  Command::buildParser();
}

bool SmoothSinc::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  int iterations = static_cast<int>(options.get("iterations"));
  double passband = static_cast<double>(options.get("passband"));

  sharedData.mesh->smoothSinc(iterations, passband);
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// Decimate
///////////////////////////////////////////////////////////////////////////////
void Decimate::buildParser()
{
  const std::string prog = "decimate";
  const std::string desc = "applies filter to reduce number of triangles in mesh";

  parser.prog(prog).description(desc);

  parser.add_option("--reduction").action("store").type("double").set_default(0.5).help("Percent reduction of total number of polygons [default: %default].");
  parser.add_option("--angle").action("store").type("double").set_default(15.0).help("Necessary angle (in degrees) between two trianges to warrant keeping them separate [default: %default].");
  parser.add_option("--preservetopology").action("store").type("bool").set_default(true).help("Whether to preserve topology [default: true].");

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
  bool preserveTopology = static_cast<bool>(options.get("preservetopology"));

  sharedData.mesh->decimate(reduction, angle, preserveTopology);
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// CVDDecimate
///////////////////////////////////////////////////////////////////////////////
void CVDDecimate::buildParser()
{
  const std::string prog = "cvd-decimate";
  const std::string desc = "applies cvd (centroidal voronoi diagram) decimation filter";

  parser.prog(prog).description(desc);

  parser.add_option("--percentage").action("store").type("double").set_default(0.5).help("Percentage of target number of clusters/vertices [default: %default].");

  Command::buildParser();
}

bool CVDDecimate::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  double percentage = static_cast<double>(options.get("percentage"));

  sharedData.mesh->cvdDecimate(percentage);
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

  std::string axisStr(static_cast<std::string>(options.get("axis")));
  Axis axis(toAxis(axisStr));

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

  if (targetMesh == "")
  {
    std::cerr << "Must specify a target mesh\n";
    return false;
  }
  else
  {
    Mesh target(targetMesh);
    MeshTransform transform(sharedData.mesh->createTransform(target, align, iterations));
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

  parser.add_option("--px").action("store").type("double").set_default(0.0).help("Value of point.x for cutting plane [default: %default].");
  parser.add_option("--py").action("store").type("double").set_default(0.0).help("Value of point.y for cutting plane [default: %default].");
  parser.add_option("--pz").action("store").type("double").set_default(0.0).help("Value of point.z for cutting plane [default: %default].");
  parser.add_option("--nx").action("store").type("double").set_default(0.0).help("Value of normal.x for cutting plane [default: %default].");
  parser.add_option("--ny").action("store").type("double").set_default(0.0).help("Value of normal.y for cutting plane [default: %default].");
  parser.add_option("--nz").action("store").type("double").set_default(0.0).help("Value of normal.z for cutting plane [default: %default].");

  Command::buildParser();
}

bool ClipMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  Point point({static_cast<double>(options.get("px")),
               static_cast<double>(options.get("py")),
               static_cast<double>(options.get("pz"))});

  Vector normal{makeVector({static_cast<double>(options.get("nx")),
                            static_cast<double>(options.get("ny")),
                            static_cast<double>(options.get("nz"))})};

  sharedData.mesh->clip(makePlane(point, normal));
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

  Command::buildParser();
}

bool BoundingBoxMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::vector<std::string> filenames = options.get("names");

  sharedData.region = MeshUtils::boundingBox(filenames);
  std::cout << "Bounding box:\n" << sharedData.region << std::endl;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// MeshBounds
///////////////////////////////////////////////////////////////////////////////
void MeshBounds::buildParser()
{
  const std::string prog = "mesh-bounds";
  const std::string desc = "return physical bounds of mesh";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

bool MeshBounds::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  sharedData.region = sharedData.mesh->boundingBox();

  std::cout << "Bounding box:\n" << sharedData.region << std::endl;

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
  parser.add_option("--summary").action("store").type("bool").set_default(true).help("Print largest distance of any point in mesh to target [default: true].");

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
  auto method{Mesh::PointToPoint};
  if (methodopt == "point-to-point") method = Mesh::PointToPoint;
  else if (methodopt == "point-to-cell") method = Mesh::PointToCell;
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
// ComputeNormals
///////////////////////////////////////////////////////////////////////////////
void ComputeNormals::buildParser()
{
  const std::string prog = "compute-normals";
  const std::string desc = "computes and adds oriented point and cell normals";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

bool ComputeNormals::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  sharedData.mesh->computeNormals();
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// FixElement
///////////////////////////////////////////////////////////////////////////////
void FixElement::buildParser()
{
  const std::string prog = "fix-element";
  const std::string desc = "fix element winding of mesh";
  parser.prog(prog).description(desc);

  Command::buildParser();
}

bool FixElement::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  sharedData.mesh->fixElement();
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

  parser.add_option("--px").action("store").type("double").set_default(0.0).help("Value of point.x for cutting plane [default: %default].");
  parser.add_option("--py").action("store").type("double").set_default(0.0).help("Value of point.y for cutting plane [default: %default].");
  parser.add_option("--pz").action("store").type("double").set_default(0.0).help("Value of point.z for cutting plane [default: %default].");
  parser.add_option("--nx").action("store").type("double").set_default(0.0).help("Value of normal.x for cutting plane [default: %default].");
  parser.add_option("--ny").action("store").type("double").set_default(0.0).help("Value of normal.y for cutting plane [default: %default].");
  parser.add_option("--nz").action("store").type("double").set_default(0.0).help("Value of normal.z for cutting plane [default: %default].");

  Command::buildParser();
}

bool ClipClosedSurface::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  Point point({static_cast<double>(options.get("px")),
               static_cast<double>(options.get("py")),
               static_cast<double>(options.get("pz"))});

  Vector normal{makeVector({static_cast<double>(options.get("nx")),
                            static_cast<double>(options.get("ny")),
                            static_cast<double>(options.get("nz"))})};

  sharedData.mesh->clipClosedSurface(makePlane(point, normal));
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// ClosestPoint
///////////////////////////////////////////////////////////////////////////////
void ClosestPoint::buildParser()
{
  const std::string prog = "closest-point";
  const std::string desc = "returns closest point to given point on mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--x").action("store").type("double").set_default(0.0).help("Value of x for point.");
  parser.add_option("--y").action("store").type("double").set_default(0.0).help("Value of y for point.");
  parser.add_option("--z").action("store").type("double").set_default(0.0).help("Value of z for point.");

  Command::buildParser();
}

bool ClosestPoint::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  Point point({static_cast<double>(options.get("x")),
               static_cast<double>(options.get("y")),
               static_cast<double>(options.get("z"))});

  std::cout << "Closest point to given point on mesh: " << sharedData.mesh->closestPoint(point) << "\n";
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// GeodesicDistance
///////////////////////////////////////////////////////////////////////////////
void GeodesicDistance::buildParser()
{
  const std::string prog = "geodesic-distance";
  const std::string desc = "computes geodesic distance between two vertices on mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--v1").action("store").type("int").set_default(-1).help("Index of first point in mesh.");
  parser.add_option("--v2").action("store").type("int").set_default(-1).help("Index of second point in mesh.");

  Command::buildParser();
}

bool GeodesicDistance::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  int v1 = static_cast<int>(options.get("v1"));
  int v2 = static_cast<int>(options.get("v2"));

  std::cout << "Geodesic Distance between two points: "
            << sharedData.mesh->geodesicDistance(v1, v2) << std::endl;
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// GeodesicDistanceToLandmark
///////////////////////////////////////////////////////////////////////////////
void GeodesicDistanceToLandmark::buildParser()
{
  const std::string prog = "geodesic-distance-landmark";
  const std::string desc = "computes geodesic distance between a point (landmark) and each vertex on mesh";
  parser.prog(prog).description(desc);

  parser.add_option("--x").action("store").type("double").set_default(0.0).help("Value of x for landmark point.");
  parser.add_option("--y").action("store").type("double").set_default(0.0).help("Value of y for landmark point.");
  parser.add_option("--z").action("store").type("double").set_default(0.0).help("Value of z for landmark point.");

  Command::buildParser();
}

bool GeodesicDistanceToLandmark::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  Point landmark({static_cast<double>(options.get("x")),
                  static_cast<double>(options.get("y")),
                  static_cast<double>(options.get("z"))});

  sharedData.field = sharedData.mesh->geodesicDistance(landmark);
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// MeanNormals
///////////////////////////////////////////////////////////////////////////////
void MeanNormals::buildParser()
{
  const std::string prog = "mean-normals";
  const std::string desc = "computes average normals for each point in given set of meshes";
  parser.prog(prog).description(desc);

  parser.add_option("--names").action("store").type("multistring").set_default("").help("Paths to meshes (must be followed by `--`), ex: \"mean-normals --names *.vtk --\")");
  parser.add_option("--generatenormals").action("store").type("bool").set_default(true).help("Auto generate normals if the mesh does not have normals [default: true].");

  Command::buildParser();
}

bool MeanNormals::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::vector<std::string> filenames = options.get("names");
  bool generateNormals = static_cast<bool>(options.get("generatenormals"));

  sharedData.field = MeshUtils::computeMeanNormals(filenames, generateNormals);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// SetField
///////////////////////////////////////////////////////////////////////////////
void SetField::buildParser()
{
  const std::string prog = "set-field";
  const std::string desc = "adds the current field to the current mesh with the given name.";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of scalar field.");

  Command::buildParser();
}

bool SetField::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  if (!sharedData.field)
  {
    std::cerr << "No field initialized to set\n";
    return false;
  }

  std::string name = static_cast<std::string>(options.get("name"));

  sharedData.mesh->setField(name, sharedData.field);
  return sharedData.validMesh();
}

///////////////////////////////////////////////////////////////////////////////
// GetField
///////////////////////////////////////////////////////////////////////////////
void GetField::buildParser()
{
  const std::string prog = "get-field";
  const std::string desc = "gets field of mesh with given name";
  parser.prog(prog).description(desc);

  parser.add_option("--name").action("store").type("string").set_default("").help("Name of scalar field.");

  Command::buildParser();
}

bool GetField::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string name = static_cast<std::string>(options.get("name"));

  sharedData.field = sharedData.mesh->getField<vtkDataArray>(name);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Curvature
///////////////////////////////////////////////////////////////////////////////
void Curvature::buildParser()
{
  const std::string prog = "mesh-curvature";
  const std::string desc = "computes and adds curvature";
  parser.prog(prog).description(desc);

  std::list<std::string> curvs{"principal", "gaussian", "mean"};
  parser.add_option("--type").action("store").type("choice").choices(curvs.begin(), curvs.end()).set_default("principal").help("Curvature type to use [default: %default].");

  Command::buildParser();
}

bool Curvature::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  std::string curvopt(options.get("type"));

  Mesh::CurvatureType curv;
  if (curvopt == "principal")
    curv = Mesh::Principal;
  else if (curvopt == "gaussian")
    curv = Mesh::Gaussian;
  else if (curvopt == "mean")
    curv = Mesh::Mean;
  else {
    std::cerr << "no such curvature type: " << curvopt << std::endl;
    return false;
  }

  sharedData.field = sharedData.mesh->curvature(curv);
  return true;
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
  const std::string desc = "converts mesh to a binary segmentation image, using unit spacing by default";
  parser.prog(prog).description(desc);

  parser.add_option("--sx").action("store").type("double").set_default(1.0).help("Spacing of output image in x-direction [default: unit spacing].");
  parser.add_option("--sy").action("store").type("double").set_default(1.0).help("Spacing of output image in y-direction [default: unit spacing].");
  parser.add_option("--sz").action("store").type("double").set_default(1.0).help("Spacing of output image in z-direction [default: unit spacing].");
  parser.add_option("--pad").action("store").type("double").set_default(0.0).help("Pad the region to extract [default: 0.0].");

  Command::buildParser();
}

bool MeshToImage::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  double x = static_cast<double>(options.get("sx"));
  double y = static_cast<double>(options.get("sy"));
  double z = static_cast<double>(options.get("sz"));
  double pad = static_cast<double>(options.get("pad"));

  Point3 spacing({x,y,z});
  auto region = sharedData.mesh->boundingBox().pad(pad);
  
  sharedData.image = sharedData.mesh->toImage(region, spacing);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// MeshToDT
///////////////////////////////////////////////////////////////////////////////
void MeshToDT::buildParser()
{
  const std::string prog = "mesh-to-dt";
  const std::string desc = "converts mesh to a distance transform, using unit spacing by default";
  parser.prog(prog).description(desc);

  parser.add_option("--sx").action("store").type("double").set_default(1.0).help("Spacing of output image in x-direction [default: unit spacing].");
  parser.add_option("--sy").action("store").type("double").set_default(1.0).help("Spacing of output image in y-direction [default: unit spacing].");
  parser.add_option("--sz").action("store").type("double").set_default(1.0).help("Spacing of output image in z-direction [default: unit spacing].");
  parser.add_option("--pad").action("store").type("double").set_default(0.0).help("Pad the region to extract [default: 0.0].");

  Command::buildParser();
}

bool MeshToDT::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  if (!sharedData.validMesh())
  {
    std::cerr << "No mesh to operate on\n";
    return false;
  }

  double x = static_cast<double>(options.get("sx"));
  double y = static_cast<double>(options.get("sy"));
  double z = static_cast<double>(options.get("sz"));
  double pad = static_cast<double>(options.get("pad"));

  Point3 spacing({x,y,z});
  auto region = sharedData.mesh->boundingBox().pad(pad);
  
  sharedData.image = sharedData.mesh->toDistanceTransform(region, spacing);
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
  parser.add_option("--epsilon").action("store").type("double").set_default(-1.0).help("Epsilon [default: %default].");

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

  double eps = static_cast<double>(options.get("epsilon"));

  if (sharedData.mesh->compare(Mesh(filename), eps))
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

///////////////////////////////////////////////////////////////////////////////
// WarpMesh
///////////////////////////////////////////////////////////////////////////////
void WarpMesh::buildParser()
{
  const std::string prog = "warp-mesh";
  const std::string desc = "warps a mesh given reference and target particles";
  parser.prog(prog).description(desc);
  parser.add_option("--reference_mesh").action("store").type("string").set_default("").help("Name of reference mesh.");
  parser.add_option("--reference_points").action("store").type("string").set_default("").help("Name of reference points.");
  parser.add_option("--target_points").action("store").type("multistring").set_default("").help("Names of target points (must be followed by `--`), ex: \"... --target_points *.particles -- ...");
  parser.add_option("--save_dir").action("store").type("string").set_default("").help("Optional: Path to the directory where the mesh files will be saved");
  Command::buildParser();
}
bool WarpMesh::execute(const optparse::Values &options, SharedCommandData &sharedData)
{
  std::string inputMeshFilename = options["reference_mesh"];
  if (inputMeshFilename.length() == 0) {
    std::cerr << "warpmesh error: no reference mesh specified, must pass `--reference_mesh <filename>`\n";
    return false;
  }
  std::string inputPointsFilename = options["reference_points"];
  if (inputPointsFilename.length() == 0) {
    std::cerr << "warpmesh error: no reference points specified, must pass `--reference_points <filename>`\n";
    return false;
  }
  std::vector<std::string> targetPointsFilenames = options.get("target_points");
  if (targetPointsFilenames.size() == 0) {
    std::cerr << "warpmesh error: no target points specified, must pass `--target_points <filenames> --`\n";
    return false;
  }

  std::string saveDir = options["save_dir"];
  try {
    Mesh inputMesh(inputMeshFilename);
    targetPointsFilenames.push_back(inputPointsFilename);
    ParticleSystem particlesystem(targetPointsFilenames);
    Eigen::MatrixXd allPts = particlesystem.Particles();
    Eigen::MatrixXd staticPoints = allPts.col(targetPointsFilenames.size() - 1);
    int numParticles = staticPoints.rows() / 3;
    staticPoints.resize(3, numParticles);
    staticPoints.transposeInPlace();
    MeshWarper warper;
    warper.set_reference_mesh(inputMesh.getVTKMesh(), staticPoints);
    std::string filenm;

    if (saveDir.length() > 0) {
      boost::filesystem::create_directories(saveDir);
    }

    for (int i = 0; i < targetPointsFilenames.size() - 1; i++) {
      filenm = targetPointsFilenames[i];
      filenm.replace(static_cast<int>(filenm.rfind('.')) + 1, filenm.length(), "vtk");
      if (saveDir.length() > 0) {
        int idx = static_cast<int>(filenm.rfind('/'));
        filenm.replace(0, idx, saveDir);
      }
      
      Eigen::MatrixXd movingPoints = allPts.col(i);
      movingPoints.resize(3, numParticles);
      movingPoints.transposeInPlace();
      Mesh output = warper.build_mesh(movingPoints);
      output.write(filenm);
    }
    return true;
  } catch (std::exception &e) {
    std::cerr << "exception during mesh warp: " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "unknown exception while mesh warping" << std::endl;
    return false;
  }
}

} //shapeworks
