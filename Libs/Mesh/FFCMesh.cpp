#include "FFCMesh.h"

namespace shapeworks {

bool FFCMesh::AddFFC(std::vector< std::vector< Eigen::Vector3d > > boundaries, Eigen::Vector3d query){

    vtkPoints *selectionPoints = vtkPoints::New();
    for(size_t bound; bound < boundaries.size(); bound++){
        for(size_t i; i < boundaries[bound].size(); i++){
            Eigen::Vector3d pt = boundaries[bound][i];
            selectionPoints->InsertNextPoint(pt[0],pt[1],pt[2]);
        }
    }

    vtkSelectPolyData *select = vtkSelectPolyData::New();
    select->SetLoop(selectionPoints);
    select->GenerateSelectionScalarsOn();
    select->SetSelectionModeToLargestRegion();

    vtkClipPolyData *selectclip = vtkClipPolyData::New();
    selectclip->SetInputData(this->m_wholeMesh.getVTKMesh());
    selectclip->SetInputConnection(select->GetOutputPort());
    selectclip->SetValue(0.0);

    this->m_inMesh = Mesh(selectclip->GetOutput());

    std::string fnin = "dev/mesh_" + std::to_string(m_dom) + "_" + std::to_string(m_ffcnum) + ".vtk";
    this->m_inMesh.write(fnin);

    return true;
}

}
