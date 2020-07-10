#ifndef MESHFIM_H
#define MESHFIM_H


#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "KDtree.h"
#include "Color.h"


//#include "itkImageToImageFilter.h"
//#include "itkLevelSet.h"
//#include "itkIndex.h"
//#include "vnl/vnl_math.h"
//#include "itkDiffusionTensor3D.h"
#include <iostream>
#include <typeinfo>
#include <functional>
#include <queue>
#include <list>
#include <map>
#include <time.h>

#include <iterator>
#include <vnl/vnl_math.h>
#include <vnl/vnl_sparse_matrix.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_sparse_lu.h>
#include <vcl_legacy_aliases.h>


#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/vgl_conic.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vcl_compiler.h>

#ifndef _EPS
#define _EPS 1e-06
#endif

#define ONE 1 
#define CURVATURE 2 


#define  PI 3.141592653589793

// SHIREEN
#define GENERATE_GEO_FILES 1 // now disable till we incorporate the geodesic repulsion
// end SHIREEN

using trimesh::TriMesh;
using trimesh::vec3;
using trimesh::point;
using trimesh::KDtree;
using trimesh::Color;

class meshFIM {

public:
  typedef int VoxelIndexType;

    typedef int index;
    enum LabelType { MaskPoint, SeedPoint, ActivePoint, FarPoint, StopPoint, AlivePoint,ToBeAlivePoint };

    TriMesh *m_meshPtr;
    int NumComputation;
    float imageOrigin[3];
    float imageSpacing[3];
    int imageSize[3];
    int imageIndex[3];

    std::vector<Color> colors;



    void computeFIM(TriMesh *mesh, const char *vertT_filename);
    void GetFeatureValues(point x, std::vector<float> &vals);
    void ReadFaceIndexMap(const char *infilename);
    void ReadFeatureFromFile(const char *infilename);
    void ReadFeatureGradientFromFile(const char *infilename);
    point GetFeatureDerivative(point p, int fIndex);


    void need_abs_curvatures();
    void need_edge_lengths();
    void need_speed();
    void need_oneringfaces();
    void need_kdtree();


    void SetMesh(TriMesh *mesh);
    void SetStopDistance(float d) {
      m_StopDistance = d;
    }
    void setSpeedType(int st) {
      speedType = st;
      if (st != ONE && st != CURVATURE) {
        std::cout << "Impossible SpeedType set" << std::endl;
        throw(1);
      }
    }


    meshFIM() {
      m_meshPtr = NULL;
    };
    ~meshFIM() {};

protected:


private:

  std::list<index>                             m_ActivePoints;
  std::vector<index>                           m_SeedPoints;
  std::vector<LabelType>                       m_Label;
  float                                        m_StopDistance;

  TriMesh *GetOutputMesh() {
    return m_meshPtr;
  }
  void MeshReader(char *filename);

  bool IsNonObtuse(int v, TriMesh::Face f);
  void SplitFace(std::vector<TriMesh::Face> &acFaces, int v, TriMesh::Face cf, int nfAdj);
  std::vector<TriMesh::Face> GetOneRing(int v);
  float Upwind(index currentVert, index vet);
  void InitializeAttributes(int currentVert, std::vector<int> seeds);
  void CleanupAttributes();
  float LocalSolver(index C, TriMesh::Face triangle, index currentVert);

  void SetSeedPoint(std::vector<index> SeedPoints) {
    m_SeedPoints = SeedPoints;
  }

  int getSpeedType() {
    return speedType;
  }
  float GetStopDistance() {
    return m_StopDistance;
  }


  void InitializeLabels();
  void InitializeActivePoints();
  float PointLength(point v);

  void GenerateReducedData();

  void loadGeodesicFile(TriMesh *mesh, const char *geoFilename);

  void computeCoordXFiles(TriMesh *mesh, const char *vertT_filename);
  void computeCoordYFiles(TriMesh *mesh, const char *vertT_filename);
  void computeCoordZFiles(TriMesh *mesh, const char *vertT_filename);
  void computeCurvFiles(TriMesh *mesh, const char *vertT_filename);

  void ComputeDistanceToCurve(TriMesh *mesh, std::vector< point > curvePoints, const char *outfilename);

  void physicalPointToXYZ(point x, VoxelIndexType *imageX, float imageOrigin[3], float imageSpacing[3]);
  VoxelIndexType indexToLinearIndex(VoxelIndexType *imageX, int imageSize[3]);
  VoxelIndexType physicalPointToLinearIndex(point x);
  VoxelIndexType physicalPointToLinearIndex(point x, float imageOrigin[3], float imageSpacing[3], int imageSize[3]);
  double pointTriangleDistance(point P, TriMesh::Face face, point &PP);
  vec3 ComputeBarycentricCoordinates(point p, TriMesh::Face f);
  void need_maxedgelength();
  int FindNearestVertex(point pt);
  int GetTriangleInfoForPoint(point x, TriMesh::Face &triangleX, float &alphaX, float &betaX, float &gammaX);

  //Praful - for Riddhish project
  float GetVirtualSource(vnl_vector<float> baryCoord, vnl_matrix<float> X, vnl_vector<float> ds, vnl_vector< float > &x0);
  float ComputeThreePointApproximatedGeodesic(vnl_vector<float> x, vnl_vector<float> baryCoord, vnl_matrix<float> X, vnl_vector<float> ds, char *method);
  float ComputeCanonicalForm(point s, vnl_vector<float> &x, vnl_matrix<float> &X);
  float GetGeodesicDistance(int v1, int v2);
  float GetBronsteinGeodesicDistance(TriMesh::Face Sa, TriMesh::Face Sb, vnl_vector <float> baryCoord_a, vnl_vector <float> baryCoord_b, char *method);
  void ComputeDistanceToLandmarksGivenTriangleInfo(TriMesh *mesh, const char *infilename, const char *outfilename);

  // SHIREEN - compute distance to landmarks based on geodesic approximation
  float GetBronsteinGeodesicDistance(point a, point b, char *method);
  void ComputeDistanceToLandmark(TriMesh *mesh, point landmark, bool apply_log, const char *outfilename);
  void UpdateGeodesicMapWithDistancesFromVertices(std::vector<int> vertexIdlist);
  // end SHIREEN

  // SHIREEN - computing geo distance on the fly for fuzzy geodesics
  int GetVertexInfoForPoint(point x);
  std::vector<float> ComputeDistanceToCurve(TriMesh *mesh, std::vector< point > curvePoints);
  void WriteFeaFile(TriMesh *mesh, char *outfilename);
  void WriteFeaFile(std::vector<float> fea, char *outfilename);
  // end SHIREEN

  point ComputeFeatureDerivative(int v, int nFeature);



  int speedType;

  // maps face index to speedInv
  std::vector<float> speedInvVector;

  // maps vertex index to abs_curv
  std::vector<float> abs_curv;

  // maps face index to vec3 of edge lengths with edges in this order: {01, 12, 20}
  std::vector<vec3> edgeLengthsVector;

  // maps vertex index to vertex index to distance?
  std::vector< std::map<unsigned int, float> > geodesicMap;

  // maps vertex index to something to do with the geodesic computation
  std::vector<float> geodesic;

  // maps something to something
  std::map<VoxelIndexType, std::vector<int> > faceIndexMap;

  // Used for ComputeBaryCentricCoordinates when faceIndexMap is unavailable.
  KDtree *kd;

  // Used for GetNearestVertex;
  double maxEdgeLength;

  std::vector< std::vector<TriMesh::Face> > vertOneringFaces;

  std::vector< std::vector<float> > features;

  std::vector < std::vector<point> > featureGradients;

};


#endif
