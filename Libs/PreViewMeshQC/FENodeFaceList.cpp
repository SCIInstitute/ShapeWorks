#include "stdafx.h"
#include "FENodeFaceList.h"
#include "FEMesh.h"

FENodeFaceList::FENodeFaceList(FEMesh* pm)
{
	m_pm = pm;
}

FENodeFaceList::~FENodeFaceList(void)
{
}

void FENodeFaceList::BuildSorted()
{
	Build();

	// sort the faces
	int N = m_pm->Nodes();
	for (int i=0; i<N; ++i)
	{
		Sort(i);
	}
}

void FENodeFaceList::Build()
{
	assert(m_pm);
	FEMesh& m = *m_pm;

	int i, j;

	int N = m.Nodes();
	vector<int> tag;
	tag.assign(N, 0);

	int NF = 0;
	for (i=0; i<m.Faces(); ++i)
	{
		FEFace& f = m.Face(i);
		for (j=0; j<f.m_nodes; ++j) tag[f.n[j]]++;
		NF += f.m_nodes;
	}

	int n = 0;
	for (i=0; i<N; ++i) if (tag[i] > 0) ++n;

	m_node.resize(n);
	m_val.assign(n, 0);
	m_off.assign(n, 0);
	n = 0;
	for (i=0; i<N; ++i)
		if (tag[i] > 0)
		{
			m_node[n] = i;
			if (n != 0) m_off[n] = m_off[n-1] + tag[n-1];
			n++;
		}

	n = 0;
	for (i=0; i<N; ++i)
		if (tag[i] > 0)
		{
			tag[i] = n++;
		}
		else tag[i] = -1;


	m_pface.resize(NF);
	for (i=0; i<m.Faces(); ++i)
	{
		FEFace& f = m.Face(i);
		for (j=0; j<f.m_nodes; ++j) 
		{
			n = tag[ f.n[j] ];
			assert(n != -1);
			int& nval = m_val[n];
			int& noff = m_off[n];
			m_pface[noff + nval] = &f;
			nval++;
		}
	}
}

bool FENodeFaceList::HasFace(int n, FEFace* pf)
{
	int nval = Valence(n);
	for (int i=0; i<nval; ++i) if (Face(n, i) == pf) return true;
	return false;
}

void FENodeFaceList::Sort(int node)
{
	int nval = Valence(node);
	vector<FEFace*> fl; fl.reserve(nval);

	for (int i=0; i<nval; ++i) Face(node, i)->m_ntag = 0;

	FEFace* pf = Face(node, 0);
	pf->m_ntag = 1;
	fl.push_back(pf);
	bool bdone = false;
	do
	{
		bdone = true;

		int m = -1;
		if (pf->n[0] == node) m = 0;
		else if (pf->n[1] == node) m = 1;
		else if (pf->n[2] == node) m = 2;

		int nj = pf->m_nbr[(m+2)%3];
		if (nj >= 0)
		{
			FEFace* pf2 = &m_pm->Face(nj);
			assert(HasFace(node, pf2));
			if (pf2->m_ntag == 0)
			{
				pf2->m_ntag = 1;
				fl.push_back(pf2);
				pf = pf2;
				bdone = false;
			}
		}
	}
	while (bdone == false);
	assert((int)fl.size() == nval);

	for (int i=0; i<nval; ++i)
	{
		m_pface[m_off[node] + i] = fl[i];
	}
}
