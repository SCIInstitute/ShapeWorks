#pragma once

#include <vector>
using namespace std;

#include "FEMesh.h"

class FENodeFaceList
{
public:
	FENodeFaceList(FEMesh* pm);
	~FENodeFaceList(void);

	void Build();
	void BuildSorted();

	int Nodes() { return (int) m_node.size(); }
	FENode& Node(int i) { return m_pm->Node(m_node[i]); }

	int Valence(int i) { return m_val[i]; }
	FEFace* Face(int n, int i) { return m_pface[m_off[n] + i]; }

	bool HasFace(int n, FEFace* pf);

protected:
	void Sort(int node);

protected:
	FEMesh*	m_pm;
	vector<int>	m_node;
	vector<int>	m_val;
	vector<FEFace*>	m_pface;
	vector<int>	m_off;
};
