#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;

class FEMesh;

class FEVTKimport
{
public:
	FEVTKimport();
	~FEVTKimport(void);

	FEMesh* Load(const char* szfile);

protected:
	bool BuildMesh();
	FEMesh* errf(const char* sz, ...);
	void Close();

private:
	FILE*	m_fp;
};
