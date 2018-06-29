#pragma once
#include "FEMesh.h"

//-----------------------------------------------------------------------------
// This modifier implements a list of tools to fixing meshes
class FEFixMesh
{
public:
	FEFixMesh() { m_ntask = 0; }
	FEMesh* Apply(FEMesh* pm);

	// set the task to do
	void SetTask(int n);
	//set max thickness in case 6
	void SetThickness(double m);

public:
	FEMesh* RemoveDuplicateElements  (FEMesh* pm);
	FEMesh* RemoveNonManifoldElements(FEMesh* pm);
	FEMesh* FixElementWinding        (FEMesh* pm);
	FEMesh* FillAllHoles			 (FEMesh* pm);
	FEMesh* FixinvertedElements		 (FEMesh* pm);
	FEMesh* FixReferenceSurface  	 (FEMesh* pm);
	FEMesh* InterpolateShellThickness(FEMesh* pm);
private:
	int	m_ntask;
	double m_limitThickness;
};
