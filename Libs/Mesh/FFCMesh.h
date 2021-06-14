#pragma once

#include "Mesh.h"

#include <unordered_map>
#include <unordered_set>
#include <robin_hood.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSelectPolyData.h>
#include <vtkClipPolyData.h>

class vtkCellLocator;

namespace shapeworks {

class FFCMesh{

public:
    enum AlignmentType { Rigid, Similarity, Affine };
    enum DistanceMethod { POINT_TO_POINT, POINT_TO_CELL };

    using MeshType = vtkSmartPointer<vtkPolyData>;

    FFCMesh(MeshType meshPtr, size_t dom, size_t ffcnum){
        m_dom = dom;
        m_ffcnum = ffcnum;
        m_wholeMesh = Mesh(meshPtr);
    }

    bool AddFFC(std::vector< std::vector< Eigen::Vector3d > > boundaries, Eigen::Vector3d query);

private:
    Mesh m_wholeMesh;
    Mesh m_inMesh;
    Mesh m_outMesh;
    size_t m_dom;
    size_t m_ffcnum;
};

}
