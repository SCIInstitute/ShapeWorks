#pragma once

class FEMesh;

//-----------------------------------------------------------------------------
struct VTKEXPORT
{
	bool	bshellthick;	// shell thickness
	bool	bscalar_data;   //user scalar data
};


class FEVTKExport
{
public:
	FEVTKExport(void);
	~FEVTKExport(void);

	bool Export(FEMesh& mesh, const char* szfile);
	void SetOptions(VTKEXPORT o) { m_ops = o; }

protected:
	void Close();

private:
	FILE* m_fp;
	VTKEXPORT m_ops;
};
