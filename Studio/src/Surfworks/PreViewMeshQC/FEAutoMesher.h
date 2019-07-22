#pragma once

class FEMesh;

//-----------------------------------------------------------------------------
class FEAutoMesher
{
public:
	FEAutoMesher();
	void BuildMesh(FEMesh* pm);

public:
	void AutoPartitionSurface(FEMesh& m);
	void AutoPartitionEdges(FEMesh& m);
	void AutoPartitionNodes(FEMesh& m);

	void Repartition(FEMesh& m);

	void SetSmoothingAngle(double w) { m_wsmooth = w; }
	double GetSmoothingAngle() { return m_wsmooth; }

protected:
	void BuildFaces(FEMesh& m);
	void BuildEdges(FEMesh& m);

protected:
	FEMesh*		m_pm;

	double	m_wsmooth;
};
