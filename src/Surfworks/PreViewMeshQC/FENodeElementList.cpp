#include "stdafx.h"
#include "FENodeElementList.h"
#include "FEMesh.h"

FENodeElementList::FENodeElementList(FEMesh* pm)
{
	m_pm = pm;
}

FENodeElementList::~FENodeElementList()
{
}

void FENodeElementList::Build()
{
	assert(m_pm);
	int i, j;
	int NN = m_pm->Nodes();
	int NE = m_pm->Elements();

	m_val.assign(NN, 0);
	int nsize = 0;
	for (i=0; i<NE; ++i)
	{
		FEElement& el = m_pm->Element(i);
		int ne = el.Nodes();
		for (j=0; j<ne; ++j) m_val[el.m_node[j]]++;
		nsize += ne;
	}

	m_off.resize(NN);
	m_off[0] = 0;
	for (i=1; i<NN; ++i) m_off[i] = m_off[i-1] + m_val[i-1];

	for (i=0; i<NN; ++i) m_val[i] = 0;

	m_pelem.resize(nsize);
	for (i=0; i<NE; ++i)
	{
		FEElement& el = m_pm->Element(i);
		int ne = el.Nodes();
		for (j=0; j<ne; ++j) 
		{
			int n = el.m_node[j];
			int noff = m_off[n] + m_val[n];
			m_pelem[noff] = &el;
			m_val[n]++;
		}
	}
}
