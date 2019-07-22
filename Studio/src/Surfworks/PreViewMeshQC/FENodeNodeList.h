#pragma once

#include <vector>
using namespace std;

class FEMesh;

class FENodeNodeList
{
public:
	FENodeNodeList(FEMesh* pm);
	~FENodeNodeList();

	void Build();

	int Valence(int n) { return m_val[n]; }
	int Node(int n, int j) { return m_node[ m_off[n] + j]; }

protected:
	FEMesh* m_pm;

	vector<int>	m_val;
	vector<int>	m_off;
	vector<int>	m_node;
};
