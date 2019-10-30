#include "stdafx.h"
#include "FEFixMesh.h"
#include "FEAutoMesher.h"
#include "FEFillHole.h"

//-----------------------------------------------------------------------------
FEMesh* FEFixMesh::Apply(FEMesh* pm)
{
	// apply the task on this mesh
	switch (m_ntask)
	{
	case 0: return RemoveDuplicateElements  (pm); break;
	case 1: return RemoveNonManifoldElements(pm); break;
	case 2: return FixElementWinding        (pm); break;
	case 3: return FillAllHoles             (pm); break;
	case 4: return FixinvertedElements		(pm); break;
	case 5: return FixReferenceSurface		(pm); break;
	case 6: return InterpolateShellThickness(pm); break;
	}
	return 0;
}

//-----------------------------------------------------------------------------
void FEFixMesh::SetTask(int n)
{
	m_ntask = n;
}

//-----------------------------------------------------------------------------
void FEFixMesh::SetThickness(double  m)
{
	m_limitThickness = m;
}

//-----------------------------------------------------------------------------
FEMesh* FEFixMesh::RemoveDuplicateElements(FEMesh* pm)
{
	// create a copy of the mesh
	FEMesh* pnew = new FEMesh(*pm);

	// remove the duplicate elements
	pnew->RemoveDuplicateElements();

	// rebuild the mesh
	FEAutoMesher mesher;
	mesher.BuildMesh(pnew);

	// done
	return pnew;
}

//-----------------------------------------------------------------------------
FEMesh* FEFixMesh::RemoveNonManifoldElements(FEMesh* pm)
{
	// create a copy of the mesh
	FEMesh* pnew = new FEMesh(*pm);

	// remove the non-manifold elements
	pnew->RemoveNonManifoldElements();

	// rebuild the mesh
	FEAutoMesher mesher;
	mesher.BuildMesh(pnew);

	// done
	return pnew;
}

//-----------------------------------------------------------------------------
FEMesh* FEFixMesh::FixElementWinding(FEMesh* pm)
{
	// create a copy of the mesh
	FEMesh* pnew = new FEMesh(*pm);

	// fix element winding
	pnew->FixElementWinding2();

	// rebuild the mesh
	FEAutoMesher mesher;
	mesher.BuildMesh(pnew);

	// done
	return pnew;
}

//-----------------------------------------------------------------------------
FEMesh* FEFixMesh::FillAllHoles(FEMesh* pm)
{
	// create a copy of the mesh
	FEMesh* pnew = new FEMesh(*pm);

	// fill all the holes
	FEFillHole fill;
	fill.FillAllHoles(pnew);

	// all done
	return pnew;
}
//-----------------------------------------------------------------------------
FEMesh* FEFixMesh::FixinvertedElements(FEMesh* pm)
{
	// create a copy of the mesh
	FEMesh* pnew = new FEMesh(*pm);
	pnew->FixinvertedElements();
	//all done
	return pnew;
}

//-----------------------------------------------------------------------------
FEMesh* FEFixMesh::FixReferenceSurface(FEMesh* pm)
{
	// create a copy of the mesh
	FEMesh* pnew = new FEMesh(*pm);
	pnew->FixReferenceSurface();
	//all done
	return pnew;
}

//-----------------------------------------------------------------------------
FEMesh* FEFixMesh::InterpolateShellThickness(FEMesh* pm)
{
	// create a copy of the mesh
	FEMesh* pnew = new FEMesh(*pm);
	pnew->InterpolateShellThickness(m_limitThickness);
	//all done
	return pnew;
}