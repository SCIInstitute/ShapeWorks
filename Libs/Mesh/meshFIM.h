#ifndef MESHFIM_H
#define MESHFIM_H


#include "TriMesh.h"
#include "TriMesh_algo.h"


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

#ifndef _EPS
#define _EPS 1e-06
#endif

#define ONE 1 
#define CURVATURE 2 


// SHIREEN
#define GENERATE_GEO_FILES 1 // now disable till we incorporate the geodesic repulsion
// end SHIREEN

using trimesh::TriMesh;

class meshFIM {

public:

    typedef int index;
    enum LabelType { MaskPoint, SeedPoint, ActivePoint, FarPoint, StopPoint, AlivePoint,ToBeAlivePoint };

    TriMesh* GetOutputMesh() {
        return m_meshPtr;
    }

    float Upwind(index currentVert, index vet);
    void MeshReader(char * filename);

    float LocalSolver(index C, TriMesh::Face triangle,  index currentVert);

    void SetSeedPoint(std::vector<index> SeedPoints) {
        m_SeedPoints = SeedPoints;
    }

    int getSpeedType() {
      return speedType;
    }
    void setSpeedType(int st) {
      speedType = st;
      if (st != ONE && st != CURVATURE) {
        std::cout << "Impossible SpeedType set" << std::endl;
        throw(1);
      }
    }

    void need_speed() {
      int nf = m_meshPtr->faces.size();
      for (int i = 0; i < nf; i++) {
        trimesh::TriMesh::Face f = m_meshPtr->faces[i];
        float speedInv = 0;
        switch (speedType) {
          case CURVATURE:
            speedInv = (abs_curv[f[0]] +
                                 abs_curv[f[1]] +
                                 abs_curv[f[2]]) / 3.0;
            break;
          case ONE:
            speedInv = 1.0;
            break;
        }
        speedInvVector.push_back(speedInv);
      }
    }

    void SetMesh(TriMesh* mesh)
    {
        m_meshPtr = mesh;
        m_meshPtr->geodesicMap.resize(m_meshPtr->vertices.size());

        // orient the mesh for consistent vertex ordering...
        orient(m_meshPtr);//  Manasi

        // have to recompute the normals and other attributes required for rendering
        if (!m_meshPtr->normals.empty())
            m_meshPtr->normals.clear();//  Manasi
        m_meshPtr->need_normals();//  Manasi
        if (!m_meshPtr->adjacentfaces.empty())
            m_meshPtr->adjacentfaces.clear();//  Manasi
        m_meshPtr->need_adjacentfaces();//  Manasi
        if (!m_meshPtr->across_edge.empty())
            m_meshPtr->across_edge.clear();//  Manasi
        m_meshPtr->need_across_edge();//  Manasi
        if (!m_meshPtr->tstrips.empty())
            m_meshPtr->tstrips.clear();//  Manasi
        m_meshPtr->need_tstrips();//  Manasi
        m_meshPtr->need_faceedges();
        m_meshPtr->need_curvatures();
        m_meshPtr->need_speed();

    }

    void InitializeLabels()
    {
        if (!m_meshPtr)
        {
            std::cout << "Label-vector size unknown, please set the mesh first..." << std::endl;
        }
        else
        {
            // initialize all labels to 'Far'
            int nv = m_meshPtr->vertices.size();
            if (m_Label.size() != nv) m_Label.resize(nv);
            for (int l = 0; l < nv; l++)
            {
                m_Label[l] = FarPoint;
            }

            // if seeed-points are present, treat them differently
            if (!m_SeedPoints.empty())
            {
                for (int s = 0; s < m_SeedPoints.size(); s++)
                {
                    m_Label[m_SeedPoints[s]] = SeedPoint;//m_Label[s] = LabelType::SeedPoint;
                }
            }

        }
    }

    void InitializeActivePoints()
    {
        if (!m_SeedPoints.empty())
        {
            int ns = m_SeedPoints.size();
            vector<index> nb;
            for (int s = 0; s < ns; s++)
            {

                nb = m_meshPtr->neighbors[m_SeedPoints[s]];
                for (int i = 0; i<nb.size();i++)
                {
                    if (m_Label[nb[i]]!=SeedPoint)
                    {
                        m_ActivePoints.push_back(nb[i]);
                        m_Label[nb[i]] = ActivePoint;
                    }

                }


            }


        }

    }

    float PointLength(point v)
    {
        float length = 0;
        for (int i = 0; i<3;i++)
        {
            length += v[i]*v[i];
        }

        return sqrt(length);

    }

    void SetStopDistance(float d)
    {
        m_StopDistance = d;
    }

    float GetStopDistance()
    {
        return m_StopDistance;
    }

    //void GenerateData();
    void GenerateReducedData();

    void loadGeodesicFile(TriMesh *mesh, const char *geoFilename);
    void computeFIM(TriMesh *mesh, const char *vertT_filename);

    void computeCoordXFiles(TriMesh *mesh, const char *vertT_filename);
    void computeCoordYFiles(TriMesh *mesh, const char *vertT_filename);
    void computeCoordZFiles(TriMesh *mesh, const char *vertT_filename);
    void computeCurvFiles(TriMesh *mesh, const char *vertT_filename);

    void ComputeDistanceToCurve(TriMesh *mesh, std::vector< point > curvePoints, const char *outfilename);

    //Praful - for Riddhish project
    void ComputeDistanceToLandmarksGivenTriangleInfo(TriMesh *mesh, const char *infilename , const char *outfilename);

    // SHIREEN - compute distance to landmarks based on geodesic approximation
    void ComputeDistanceToLandmark(TriMesh *mesh, point landmark, bool apply_log, const char *outfilename);
    void UpdateGeodesicMapWithDistancesFromVertices(std::vector<int> vertexIdlist);
    // end SHIREEN

    // SHIREEN - computing geo distance on the fly for fuzzy geodesics
    std::vector<float> ComputeDistanceToCurve(TriMesh *mesh, std::vector< point > curvePoints);
    void WriteFeaFile(TriMesh *mesh, char* outfilename);
    void WriteFeaFile(std::vector<float> fea, char* outfilename);
    // end SHIREEN
    meshFIM(){
        m_meshPtr = NULL;
    };
    ~meshFIM(){};

    TriMesh*                                     m_meshPtr;
    int                                          NumComputation;

protected:

    std::list<index>                             m_ActivePoints;
    std::vector<index>                           m_SeedPoints;
    std::vector<LabelType>                       m_Label;
    float                                        m_StopDistance;


private:
  int speedType;

  std::vector<float> speedInvVector;

};


#endif
