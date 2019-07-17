#include "stdafx.h"
#include "FENodeNodeList.h"
#include "FENodeElementList.h"
#include "FEMesh.h"

FENodeNodeList::FENodeNodeList(FEMesh* pm)
{
	m_pm = pm;
}

FENodeNodeList::~FENodeNodeList()
{
}

void FENodeNodeList::Build()
{
	assert(m_pm);
	FENodeElementList NEL(m_pm);
	NEL.Build();

	int i, j, k, n;
	int NN = m_pm->Nodes();
	vector<int> tag; tag.assign(NN, -1);

	m_val.resize(NN);
	int nsize = 0;

	for (i=0; i<NN; ++i)
	{
		int nv = NEL.Valence(i);
		for (j=n=0; j<nv; ++j)
		{
			FEElement* pe = NEL.Element(i, j);
			int ne = pe->Nodes();
			for (k=0; k<ne; ++k) 
			{
				int nn = pe->m_node[k];
				if ((nn != i) && (tag[nn] != i))
				{
					tag[nn] = i;
					n++;
				}
			}

			m_val[i] = n;
			nsize += n;
		}
	}

	m_off.resize(NN);
	m_off[0] = 0;
	for (i=1; i<NN; ++i) m_off[i] = m_off[i-1] + m_val[i-1];

	for (i=0; i<NN; ++i) tag[i] = -1;

	m_node.resize(nsize);

	for (i=0; i<NN; ++i)
	{
		int nv = NEL.Valence(i);
		int noff = m_off[i];
		for (j=n=0; j<nv; ++j)
		{
			FEElement* pe = NEL.Element(i, j);
			int ne = pe->Nodes();
			for (k=0; k<ne; ++k) 
			{
				int nn = pe->m_node[k];
				if ((nn != i) && (tag[nn] != i))
				{
					tag[nn] = i;
					m_node[noff + n] = nn;
					n++;
				}
			}
		}
	}
}
