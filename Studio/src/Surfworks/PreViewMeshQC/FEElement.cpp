// FEElement.cpp: implementation of the FEElement class.
//
//////////////////////////////////////////////////////////////////////

#include "FEElement.h"

//=============================================================================
// FEEdge
//-----------------------------------------------------------------------------
FEEdge::FEEdge()
{ 
	m_elem = -1; 
	n[0] = n[1] = n[2] = -1;
	m_nbr[0] = m_nbr[1] = -1; 
}

//-----------------------------------------------------------------------------
bool FEEdge::operator == (const FEEdge& e)
{
	if ((n[0] == e.n[0]) && (n[1] == e.n[1]) && (n[2] == e.n[2])) return true;
	if ((n[0] == e.n[1]) && (n[1] == e.n[0]) && (n[2] == e.n[2])) return true;
	return false;	
}

//-----------------------------------------------------------------------------
int FEEdge::FindNode(int node)
{
	if (node == n[0]) return 0;
	if (node == n[1]) return 1;
	if (node == n[2]) return 2;
	return -1;
}

//=============================================================================
// FEFace
//-----------------------------------------------------------------------------
FEFace::FEFace()
{
	m_nodes=0; 
	m_elem[0] = m_elem[1] = -1; 
	m_sid = 0;

	n[0] = -1; n[1] = -1; n[2] = -1; n[3] = -1;
	n[4] = -1; n[5] = -1; n[6] = -1; n[7] = -1;
	n[8] = -1;
	m_nbr[0] = -1;
	m_nbr[1] = -1;
	m_nbr[2] = -1;
	m_nbr[3] = -1;
}

//-----------------------------------------------------------------------------
bool FEFace::operator == (const FEFace& f)
{
	if (m_nodes != f.m_nodes) return false;
	switch (m_nodes)
	{
	case 3: // triangle
		{
			if ((n[0] != f.n[0]) && (n[0] != f.n[1]) && (n[0] != f.n[2])) return false;
			if ((n[1] != f.n[0]) && (n[1] != f.n[1]) && (n[1] != f.n[2])) return false;
			if ((n[2] != f.n[0]) && (n[2] != f.n[1]) && (n[2] != f.n[2])) return false;
		}
		break;
	case 4: // quad
		{
			if ((n[0] != f.n[0]) && (n[0] != f.n[1]) && (n[0] != f.n[2]) && (n[0] != f.n[3])) return false;
			if ((n[1] != f.n[0]) && (n[1] != f.n[1]) && (n[1] != f.n[2]) && (n[1] != f.n[3])) return false;
			if ((n[2] != f.n[0]) && (n[2] != f.n[1]) && (n[2] != f.n[2]) && (n[2] != f.n[3])) return false;
			if ((n[3] != f.n[0]) && (n[3] != f.n[1]) && (n[3] != f.n[2]) && (n[3] != f.n[3])) return false;
		}
		break;
	case 6: // quadratic triangle (6-nodes)
		{
			if ((n[0] != f.n[0]) && (n[0] != f.n[1]) && (n[0] != f.n[2])) return false;
			if ((n[1] != f.n[0]) && (n[1] != f.n[1]) && (n[1] != f.n[2])) return false;
			if ((n[2] != f.n[0]) && (n[2] != f.n[1]) && (n[2] != f.n[2])) return false;

			if ((n[3] != f.n[3]) && (n[3] != f.n[4]) && (n[3] != f.n[5])) return false;
			if ((n[4] != f.n[3]) && (n[4] != f.n[4]) && (n[4] != f.n[5])) return false;
			if ((n[5] != f.n[3]) && (n[5] != f.n[4]) && (n[5] != f.n[5])) return false;
		}
		break;
	case 7: // quadratic triangle (7-nodes)
		{
			if ((n[0] != f.n[0]) && (n[0] != f.n[1]) && (n[0] != f.n[2])) return false;
			if ((n[1] != f.n[0]) && (n[1] != f.n[1]) && (n[1] != f.n[2])) return false;
			if ((n[2] != f.n[0]) && (n[2] != f.n[1]) && (n[2] != f.n[2])) return false;

			if ((n[3] != f.n[3]) && (n[3] != f.n[4]) && (n[3] != f.n[5])) return false;
			if ((n[4] != f.n[3]) && (n[4] != f.n[4]) && (n[4] != f.n[5])) return false;
			if ((n[5] != f.n[3]) && (n[5] != f.n[4]) && (n[5] != f.n[5])) return false;

			if ((n[6] != f.n[6])) return false;
		}
		break;
	case 8:	// 2nd-order quad
		{
			if ((n[0] != f.n[0]) && (n[0] != f.n[1]) && (n[0] != f.n[2]) && (n[0] != f.n[3])) return false;
			if ((n[1] != f.n[0]) && (n[1] != f.n[1]) && (n[1] != f.n[2]) && (n[1] != f.n[3])) return false;
			if ((n[2] != f.n[0]) && (n[2] != f.n[1]) && (n[2] != f.n[2]) && (n[2] != f.n[3])) return false;
			if ((n[3] != f.n[0]) && (n[3] != f.n[1]) && (n[3] != f.n[2]) && (n[3] != f.n[3])) return false;

			if ((n[4] != f.n[4]) && (n[4] != f.n[5]) && (n[4] != f.n[6]) && (n[4] != f.n[7])) return false;
			if ((n[5] != f.n[4]) && (n[5] != f.n[5]) && (n[5] != f.n[6]) && (n[5] != f.n[7])) return false;
			if ((n[6] != f.n[4]) && (n[6] != f.n[5]) && (n[6] != f.n[6]) && (n[6] != f.n[7])) return false;
			if ((n[7] != f.n[4]) && (n[7] != f.n[5]) && (n[7] != f.n[6]) && (n[7] != f.n[7])) return false;
		}
		break;
	case 9:	// 2nd-order quad
		{
			if ((n[0] != f.n[0]) && (n[0] != f.n[1]) && (n[0] != f.n[2]) && (n[0] != f.n[3])) return false;
			if ((n[1] != f.n[0]) && (n[1] != f.n[1]) && (n[1] != f.n[2]) && (n[1] != f.n[3])) return false;
			if ((n[2] != f.n[0]) && (n[2] != f.n[1]) && (n[2] != f.n[2]) && (n[2] != f.n[3])) return false;
			if ((n[3] != f.n[0]) && (n[3] != f.n[1]) && (n[3] != f.n[2]) && (n[3] != f.n[3])) return false;

			if ((n[4] != f.n[4]) && (n[4] != f.n[5]) && (n[4] != f.n[6]) && (n[4] != f.n[7])) return false;
			if ((n[5] != f.n[4]) && (n[5] != f.n[5]) && (n[5] != f.n[6]) && (n[5] != f.n[7])) return false;
			if ((n[6] != f.n[4]) && (n[6] != f.n[5]) && (n[6] != f.n[6]) && (n[6] != f.n[7])) return false;
			if ((n[7] != f.n[4]) && (n[7] != f.n[5]) && (n[7] != f.n[6]) && (n[7] != f.n[7])) return false;

			if (n[8] != f.n[8]) return false;
		}
		break;
	default:
		assert(false);
	}

	return true;
}

//-----------------------------------------------------------------------------
int FEFace::Edges()
{
	switch (m_nodes)
	{
	case 3: return 3;
	case 4: return 4;
	case 6: return 3;
	case 7: return 3;
	case 8: return 4;
	case 9: return 4;
	default:
		assert(false);
	}
	return 0;
}

//-----------------------------------------------------------------------------
// TODO: Don't know yet how to modify this for quad8 or tri6.
bool FEFace::HasEdge(int n1, int n2)
{
	if ((m_nodes == 4) || (m_nodes == 8) || (m_nodes == 9))
	{
		if (((n[0]==n1) && (n[1]==n2)) || ((n[1]==n1) && (n[0]==n2))) return true;
		if (((n[1]==n1) && (n[2]==n2)) || ((n[2]==n1) && (n[1]==n2))) return true;
		if (((n[2]==n1) && (n[3]==n2)) || ((n[3]==n1) && (n[2]==n2))) return true;
		if (((n[3]==n1) && (n[0]==n2)) || ((n[0]==n1) && (n[3]==n2))) return true;
	}
	else if ((m_nodes == 3) || (m_nodes == 6) || (m_nodes == 7))
	{
		if (((n[0]==n1) && (n[1]==n2)) || ((n[1]==n1) && (n[0]==n2))) return true;
		if (((n[1]==n1) && (n[2]==n2)) || ((n[2]==n1) && (n[1]==n2))) return true;
		if (((n[2]==n1) && (n[0]==n2)) || ((n[0]==n1) && (n[2]==n2))) return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// See if this face has node i
bool FEFace::HasNode(int i)
{
	switch (m_nodes)
	{
	case 3: return ((n[0]==i)||(n[1]==i)||(n[2]==i));
	case 4: return ((n[0]==i)||(n[1]==i)||(n[2]==i)||(n[3]==i));
	case 6: return ((n[0]==i)||(n[1]==i)||(n[2]==i)||(n[3]==i)||(n[4]==i)||(n[5]==i));
	case 7: return ((n[0]==i)||(n[1]==i)||(n[2]==i)||(n[3]==i)||(n[4]==i)||(n[5]==i)||(n[6]==i));
	case 8: return ((n[0]==i)||(n[1]==i)||(n[2]==i)||(n[3]==i)||(n[4]==i)||(n[5]==i)||(n[6]==i)||(n[7]==i));
	case 9: return ((n[0]==i)||(n[1]==i)||(n[2]==i)||(n[3]==i)||(n[4]==i)||(n[5]==i)||(n[6]==i)||(n[7]==i)||(n[8]==i));
	default:
		assert(false);
	}
	return false;
}

//-----------------------------------------------------------------------------
// Find the array index of node with ID i
int FEFace::FindNode(int i)
{
	if (i==n[0]) return 0; if (i==n[1]) return 1; if (i==n[2]) return 2; if (i==n[3]) return 3;
	if (i==n[4]) return 4; if (i==n[5]) return 5; if (i==n[6]) return 6; if (i==n[7]) return 7;
	if (i==n[8]) return 8;
	return -1;
}

//-----------------------------------------------------------------------------
void FEFace::GetEdgeNodes(int i, int* en)
{
	switch (m_nodes)
	{
	case 3: 
		if (i == 0) { en[0] = n[0]; en[1] = n[1]; en[2] = -1; }
		if (i == 1) { en[0] = n[1]; en[1] = n[2]; en[2] = -1; }
		if (i == 2) { en[0] = n[2]; en[1] = n[0]; en[2] = -1; }
		break;
	case 4:
		if (i == 0) { en[0] = n[0]; en[1] = n[1]; en[2] = -1; }
		if (i == 1) { en[0] = n[1]; en[1] = n[2]; en[2] = -1; }
		if (i == 2) { en[0] = n[2]; en[1] = n[3]; en[2] = -1; }
		if (i == 3) { en[0] = n[3]; en[1] = n[0]; en[2] = -1; }
		break;
	case 6:
		if (i == 0) { en[0] = n[0]; en[1] = n[1]; en[2] = 3; }
		if (i == 1) { en[0] = n[1]; en[1] = n[2]; en[2] = 4; }
		if (i == 2) { en[0] = n[2]; en[1] = n[0]; en[2] = 5; }
		break;
	case 7:
		if (i == 0) { en[0] = n[0]; en[1] = n[1]; en[2] = 3; }
		if (i == 1) { en[0] = n[1]; en[1] = n[2]; en[2] = 4; }
		if (i == 2) { en[0] = n[2]; en[1] = n[0]; en[2] = 5; }
		break;
	case 8:
	case 9:
		if (i == 0) { en[0] = n[0]; en[1] = n[1]; en[2] = 4; }
		if (i == 1) { en[0] = n[1]; en[1] = n[2]; en[2] = 5; }
		if (i == 2) { en[0] = n[2]; en[1] = n[3]; en[2] = 6; }
		if (i == 3) { en[0] = n[3]; en[1] = n[0]; en[2] = 7; }
		break;
	default:
		assert(false);
	}
}

//=============================================================================
// FEElement_
//-----------------------------------------------------------------------------
FEElement_::FEElement_()
{
	m_ntype = 0;	// unknown type

	m_node = 0;
	m_nbr = 0;
	m_face = 0;
	m_h = 0;

	m_nodes = 0;
	m_nfaces = 0;
	m_nedges = 0;

	m_gid = 0;
	m_Q.unit(); m_Qactive = false;
	m_a0 = 0;
}

//-----------------------------------------------------------------------------
int FEElement_::GetFace(int i, int n[8])
{
	int nn = 0;
	int* m = m_node;
	switch (m_ntype)
	{
	case FE_HEX8:
		switch (i)
		{
		case 0: nn = 4; n[0] = m_node[0]; n[1] = m_node[1]; n[2] = m_node[5]; n[3] = m_node[4]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 1: nn = 4; n[0] = m_node[1]; n[1] = m_node[2]; n[2] = m_node[6]; n[3] = m_node[5]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 2: nn = 4; n[0] = m_node[2]; n[1] = m_node[3]; n[2] = m_node[7]; n[3] = m_node[6]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 3: nn = 4; n[0] = m_node[3]; n[1] = m_node[0]; n[2] = m_node[4]; n[3] = m_node[7]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 4: nn = 4; n[0] = m_node[3]; n[1] = m_node[2]; n[2] = m_node[1]; n[3] = m_node[0]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 5: nn = 4; n[0] = m_node[4]; n[1] = m_node[5]; n[2] = m_node[6]; n[3] = m_node[7]; n[4] = n[5] = n[6] = n[7] = -1; break;
		}
		break;
	case FE_HEX20:
		switch (i)
		{
		case 0: nn = 8; n[0] = m_node[0]; n[1] = m_node[1]; n[2] = m_node[5]; n[3] = m_node[4]; n[4] = m_node[ 8]; n[5] = m_node[17]; n[6] = m_node[12]; n[7] = m_node[16]; break;
		case 1: nn = 8; n[0] = m_node[1]; n[1] = m_node[2]; n[2] = m_node[6]; n[3] = m_node[5]; n[4] = m_node[ 9]; n[5] = m_node[18]; n[6] = m_node[13]; n[7] = m_node[17]; break;
		case 2: nn = 8; n[0] = m_node[2]; n[1] = m_node[3]; n[2] = m_node[7]; n[3] = m_node[6]; n[4] = m_node[10]; n[5] = m_node[19]; n[6] = m_node[14]; n[7] = m_node[18]; break;
		case 3: nn = 8; n[0] = m_node[3]; n[1] = m_node[0]; n[2] = m_node[4]; n[3] = m_node[7]; n[4] = m_node[11]; n[5] = m_node[16]; n[6] = m_node[15]; n[7] = m_node[19]; break;
		case 4: nn = 8; n[0] = m_node[3]; n[1] = m_node[2]; n[2] = m_node[1]; n[3] = m_node[0]; n[4] = m_node[10]; n[5] = m_node[ 9]; n[6] = m_node[ 8]; n[7] = m_node[11]; break;
		case 5: nn = 8; n[0] = m_node[4]; n[1] = m_node[5]; n[2] = m_node[6]; n[3] = m_node[7]; n[4] = m_node[12]; n[5] = m_node[13]; n[6] = m_node[14]; n[7] = m_node[15]; break;
		}
		break;
	case FE_HEX27:
		switch (i)
		{
		case 0: nn = 9; n[0] = m_node[0]; n[1] = m_node[1]; n[2] = m_node[5]; n[3] = m_node[4]; n[4] = m_node[ 8]; n[5] = m_node[17]; n[6] = m_node[12]; n[7] = m_node[16]; n[8] = m_node[20]; break;
		case 1: nn = 9; n[0] = m_node[1]; n[1] = m_node[2]; n[2] = m_node[6]; n[3] = m_node[5]; n[4] = m_node[ 9]; n[5] = m_node[18]; n[6] = m_node[13]; n[7] = m_node[17]; n[8] = m_node[21]; break;
		case 2: nn = 9; n[0] = m_node[2]; n[1] = m_node[3]; n[2] = m_node[7]; n[3] = m_node[6]; n[4] = m_node[10]; n[5] = m_node[19]; n[6] = m_node[14]; n[7] = m_node[18]; n[8] = m_node[22]; break;
		case 3: nn = 9; n[0] = m_node[3]; n[1] = m_node[0]; n[2] = m_node[4]; n[3] = m_node[7]; n[4] = m_node[11]; n[5] = m_node[16]; n[6] = m_node[15]; n[7] = m_node[19]; n[8] = m_node[23]; break;
		case 4: nn = 9; n[0] = m_node[3]; n[1] = m_node[2]; n[2] = m_node[1]; n[3] = m_node[0]; n[4] = m_node[10]; n[5] = m_node[ 9]; n[6] = m_node[ 8]; n[7] = m_node[11]; n[8] = m_node[24]; break;
		case 5: nn = 9; n[0] = m_node[4]; n[1] = m_node[5]; n[2] = m_node[6]; n[3] = m_node[7]; n[4] = m_node[12]; n[5] = m_node[13]; n[6] = m_node[14]; n[7] = m_node[15]; n[8] = m_node[25]; break;
		}
		break;
	case FE_PENTA6:
		switch(i)
		{
		case 0: nn = 4; n[0] = m_node[0]; n[1] = m_node[1]; n[2] = m_node[4]; n[3] = m_node[3]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 1: nn = 4; n[0] = m_node[1]; n[1] = m_node[2]; n[2] = m_node[5]; n[3] = m_node[4]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 2: nn = 4; n[0] = m_node[0]; n[1] = m_node[3]; n[2] = m_node[5]; n[3] = m_node[2]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 3: nn = 3; n[0] = m_node[0]; n[1] = m_node[2]; n[2] = n[3] = m_node[1]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 4: nn = 3; n[0] = m_node[3]; n[1] = m_node[4]; n[2] = n[3] = m_node[5]; n[4] = n[5] = n[6] = n[7] = -1; break;
		}
		break;
	case FE_TET4:
		switch (i)
		{
		case 0: nn = 3; n[0] = m_node[0]; n[1] = m_node[1]; n[2] = n[3] = m_node[3]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 1: nn = 3; n[0] = m_node[1]; n[1] = m_node[2]; n[2] = n[3] = m_node[3]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 2: nn = 3; n[0] = m_node[0]; n[1] = m_node[3]; n[2] = n[3] = m_node[2]; n[4] = n[5] = n[6] = n[7] = -1; break;
		case 3: nn = 3; n[0] = m_node[0]; n[1] = m_node[2]; n[2] = n[3] = m_node[1]; n[4] = n[5] = n[6] = n[7] = -1; break;
		}
		break;
	case FE_TET10:
		switch(i)
		{
		case 0: nn = 6; n[0] = m[0]; n[1] = m[1]; n[2] = m[3]; n[3] = m[4]; n[4] = m[8]; n[5] = m[7]; n[6] = n[7] = -1; break;
		case 1: nn = 6; n[0] = m[1]; n[1] = m[2]; n[2] = m[3]; n[3] = m[5]; n[4] = m[9]; n[5] = m[8]; n[6] = n[7] = -1; break;
		case 2: nn = 6; n[0] = m[2]; n[1] = m[0]; n[2] = m[3]; n[3] = m[6]; n[4] = m[7]; n[5] = m[9]; n[6] = n[7] = -1; break;
		case 3: nn = 6; n[0] = m[2]; n[1] = m[1]; n[2] = m[0]; n[3] = m[5]; n[4] = m[4]; n[5] = m[6]; n[6] = n[7] = -1; break;
		}
		break;
	case FE_TET15:
		switch(i)
		{
		case 0: nn = 7; n[0] = m[0]; n[1] = m[1]; n[2] = m[3]; n[3] = m[4]; n[4] = m[8]; n[5] = m[7]; n[6] = m[11]; n[7] = -1; break;
		case 1: nn = 7; n[0] = m[1]; n[1] = m[2]; n[2] = m[3]; n[3] = m[5]; n[4] = m[9]; n[5] = m[8]; n[6] = m[12]; n[7] = -1; break;
		case 2: nn = 7; n[0] = m[2]; n[1] = m[0]; n[2] = m[3]; n[3] = m[6]; n[4] = m[7]; n[5] = m[9]; n[6] = m[13]; n[7] = -1; break;
		case 3: nn = 7; n[0] = m[2]; n[1] = m[1]; n[2] = m[0]; n[3] = m[5]; n[4] = m[4]; n[5] = m[6]; n[6] = m[10]; n[7] = -1; break;
		}
		break;
	}
	return nn;
}

//-----------------------------------------------------------------------------
FEEdge FEElement_::GetEdge(int i)
{
	FEEdge e;

	switch (m_ntype)
	{
	case FE_QUAD4:
		switch (i)
		{
		case 0: e.n[0] = m_node[0]; e.n[1] = m_node[1]; break;
		case 1: e.n[0] = m_node[1]; e.n[1] = m_node[2]; break;
		case 2: e.n[0] = m_node[2]; e.n[1] = m_node[3]; break;
		case 3: e.n[0] = m_node[3]; e.n[1] = m_node[0]; break;
		}
		break;
	case FE_TRI3:
		switch (i)
		{
		case 0: e.n[0] = m_node[0]; e.n[1] = m_node[1]; break;
		case 1: e.n[0] = m_node[1]; e.n[1] = m_node[2]; break;
		case 2: e.n[0] = m_node[2]; e.n[1] = m_node[0]; break;
		}
		break;
	}
	return e;
}

//-----------------------------------------------------------------------------
FEFace FEElement_::GetFace(int i)
{
	FEFace f;
	f.m_nodes = GetFace(i, f.n);
	return f;
}

//-----------------------------------------------------------------------------
FEFace FEElement_::GetShellFace()
{
	FEFace f;
	switch (m_ntype)
	{
	case FE_QUAD4: f.m_nodes = 4; f.n[0] = m_node[0]; f.n[1] = m_node[1]; f.n[2] = m_node[2]; f.n[3] = m_node[3]; break;
	case FE_TRI3 : f.m_nodes = 3; f.n[0] = m_node[0]; f.n[1] = m_node[1]; f.n[2] = m_node[2]; f.n[3] = m_node[2]; break;
	default:
		assert(false);
	}
	return f;
}

//-----------------------------------------------------------------------------
//! Is this an exterior element (i.e. an element that is on the outside of the mesh).
//! shells and beams are alwasy exterior. Solids are exterior if they have at least one
//! null neighbor.
bool FEElement_::IsExterior()
{
	if (!IsSolid()) return true;
	int n = Faces();
	for (int i=0; i<n; ++i) if (m_nbr[i] < 0) return true;
	return false;
}

//-----------------------------------------------------------------------------
// TODO: This algorithm assumes that for 2nd order elements, interior nodes
// will only map to interior nodes of the other element. I'm not sure yet if that
// is an acceptable limitation.
bool FEElement_::is_equal(FEElement_& e)
{
	if (m_ntype != e.m_ntype) return false;
	int* n = m_node;
	int* m = e.m_node; 
	switch (m_ntype)
	{
	case FE_BEAM2:
		if ((n[0]!=m[0])&&(n[0]!=m[1])) return false;
		if ((n[1]!=m[0])&&(n[1]!=m[1])) return false;
		break;
	case FE_TRI3:
		if ((n[0]!=m[0])&&(n[0]!=m[1])&&(n[0]!=m[2])) return false;
		if ((n[1]!=m[0])&&(n[1]!=m[1])&&(n[1]!=m[2])) return false;
		if ((n[2]!=m[0])&&(n[2]!=m[1])&&(n[2]!=m[2])) return false;
		break;
	case FE_QUAD4:
	case FE_TET4:
		if ((n[0]!=m[0])&&(n[0]!=m[1])&&(n[0]!=m[2])&&(n[0]!=m[3])) return false;
		if ((n[1]!=m[0])&&(n[1]!=m[1])&&(n[1]!=m[2])&&(n[1]!=m[3])) return false;
		if ((n[2]!=m[0])&&(n[2]!=m[1])&&(n[2]!=m[2])&&(n[2]!=m[3])) return false;
		if ((n[3]!=m[0])&&(n[3]!=m[1])&&(n[3]!=m[2])&&(n[3]!=m[3])) return false;
		break;
	case FE_PENTA6:
		if ((n[0]!=m[0])&&(n[0]!=m[1])&&(n[0]!=m[2])&&(n[0]!=m[3])&&(n[0]!=m[4])&&(n[0]!=m[5])) return false;
		if ((n[1]!=m[0])&&(n[1]!=m[1])&&(n[1]!=m[2])&&(n[1]!=m[3])&&(n[1]!=m[4])&&(n[1]!=m[5])) return false;
		if ((n[2]!=m[0])&&(n[2]!=m[1])&&(n[2]!=m[2])&&(n[2]!=m[3])&&(n[2]!=m[4])&&(n[2]!=m[5])) return false;
		if ((n[3]!=m[0])&&(n[3]!=m[1])&&(n[3]!=m[2])&&(n[3]!=m[3])&&(n[3]!=m[4])&&(n[3]!=m[5])) return false;
		if ((n[4]!=m[0])&&(n[4]!=m[1])&&(n[4]!=m[2])&&(n[4]!=m[3])&&(n[4]!=m[4])&&(n[4]!=m[5])) return false;
		if ((n[5]!=m[0])&&(n[5]!=m[1])&&(n[5]!=m[2])&&(n[5]!=m[3])&&(n[5]!=m[4])&&(n[5]!=m[5])) return false;
		break;
	case FE_HEX8:
		if ((n[0]!=m[0])&&(n[0]!=m[1])&&(n[0]!=m[2])&&(n[0]!=m[3])&&(n[0]!=m[4])&&(n[0]!=m[5])&&(n[0]!=m[6])&&(n[0]!=m[7])) return false;
		if ((n[1]!=m[0])&&(n[1]!=m[1])&&(n[1]!=m[2])&&(n[1]!=m[3])&&(n[1]!=m[4])&&(n[1]!=m[5])&&(n[1]!=m[6])&&(n[1]!=m[7])) return false;
		if ((n[2]!=m[0])&&(n[2]!=m[1])&&(n[2]!=m[2])&&(n[2]!=m[3])&&(n[2]!=m[4])&&(n[2]!=m[5])&&(n[2]!=m[6])&&(n[2]!=m[7])) return false;
		if ((n[3]!=m[0])&&(n[3]!=m[1])&&(n[3]!=m[2])&&(n[3]!=m[3])&&(n[3]!=m[4])&&(n[3]!=m[5])&&(n[3]!=m[6])&&(n[3]!=m[7])) return false;
		if ((n[4]!=m[0])&&(n[4]!=m[1])&&(n[4]!=m[2])&&(n[4]!=m[3])&&(n[4]!=m[4])&&(n[4]!=m[5])&&(n[4]!=m[6])&&(n[4]!=m[7])) return false;
		if ((n[5]!=m[0])&&(n[5]!=m[1])&&(n[5]!=m[2])&&(n[5]!=m[3])&&(n[5]!=m[4])&&(n[5]!=m[5])&&(n[5]!=m[6])&&(n[5]!=m[7])) return false;
		if ((n[6]!=m[0])&&(n[6]!=m[1])&&(n[6]!=m[2])&&(n[6]!=m[3])&&(n[6]!=m[4])&&(n[6]!=m[5])&&(n[6]!=m[6])&&(n[6]!=m[7])) return false;
		if ((n[7]!=m[0])&&(n[7]!=m[1])&&(n[7]!=m[2])&&(n[7]!=m[3])&&(n[7]!=m[4])&&(n[7]!=m[5])&&(n[7]!=m[6])&&(n[7]!=m[7])) return false;
		break;
	case FE_HEX20:
		if ((n[0]!=m[0])&&(n[0]!=m[1])&&(n[0]!=m[2])&&(n[0]!=m[3])&&(n[0]!=m[4])&&(n[0]!=m[5])&&(n[0]!=m[6])&&(n[0]!=m[7])) return false;
		if ((n[1]!=m[0])&&(n[1]!=m[1])&&(n[1]!=m[2])&&(n[1]!=m[3])&&(n[1]!=m[4])&&(n[1]!=m[5])&&(n[1]!=m[6])&&(n[1]!=m[7])) return false;
		if ((n[2]!=m[0])&&(n[2]!=m[1])&&(n[2]!=m[2])&&(n[2]!=m[3])&&(n[2]!=m[4])&&(n[2]!=m[5])&&(n[2]!=m[6])&&(n[2]!=m[7])) return false;
		if ((n[3]!=m[0])&&(n[3]!=m[1])&&(n[3]!=m[2])&&(n[3]!=m[3])&&(n[3]!=m[4])&&(n[3]!=m[5])&&(n[3]!=m[6])&&(n[3]!=m[7])) return false;
		if ((n[4]!=m[0])&&(n[4]!=m[1])&&(n[4]!=m[2])&&(n[4]!=m[3])&&(n[4]!=m[4])&&(n[4]!=m[5])&&(n[4]!=m[6])&&(n[4]!=m[7])) return false;
		if ((n[5]!=m[0])&&(n[5]!=m[1])&&(n[5]!=m[2])&&(n[5]!=m[3])&&(n[5]!=m[4])&&(n[5]!=m[5])&&(n[5]!=m[6])&&(n[5]!=m[7])) return false;
		if ((n[6]!=m[0])&&(n[6]!=m[1])&&(n[6]!=m[2])&&(n[6]!=m[3])&&(n[6]!=m[4])&&(n[6]!=m[5])&&(n[6]!=m[6])&&(n[6]!=m[7])) return false;
		if ((n[7]!=m[0])&&(n[7]!=m[1])&&(n[7]!=m[2])&&(n[7]!=m[3])&&(n[7]!=m[4])&&(n[7]!=m[5])&&(n[7]!=m[6])&&(n[7]!=m[7])) return false;

		if ((n[ 8]!=m[8])&&(n[ 8]!=m[9])&&(n[ 8]!=m[10])&&(n[ 8]!=m[11])&&(n[ 8]!=m[12])&&(n[ 8]!=m[13])&&(n[ 8]!=m[14])&&(n[ 8]!=m[15])&&(n[ 8]!=m[16])&&(n[ 8]!=m[17])&&(n[ 8]!=m[18])&&(n[ 8]!=m[19])) return false;
		if ((n[ 9]!=m[8])&&(n[ 9]!=m[9])&&(n[ 9]!=m[10])&&(n[ 9]!=m[11])&&(n[ 9]!=m[12])&&(n[ 9]!=m[13])&&(n[ 9]!=m[14])&&(n[ 9]!=m[15])&&(n[ 9]!=m[16])&&(n[ 9]!=m[17])&&(n[ 9]!=m[18])&&(n[ 9]!=m[19])) return false;
		if ((n[10]!=m[8])&&(n[10]!=m[9])&&(n[10]!=m[10])&&(n[10]!=m[11])&&(n[10]!=m[12])&&(n[10]!=m[13])&&(n[10]!=m[14])&&(n[10]!=m[15])&&(n[10]!=m[16])&&(n[10]!=m[17])&&(n[10]!=m[18])&&(n[10]!=m[19])) return false;
		if ((n[11]!=m[8])&&(n[11]!=m[9])&&(n[11]!=m[10])&&(n[11]!=m[11])&&(n[11]!=m[12])&&(n[11]!=m[13])&&(n[11]!=m[14])&&(n[11]!=m[15])&&(n[11]!=m[16])&&(n[11]!=m[17])&&(n[11]!=m[18])&&(n[11]!=m[19])) return false;
		if ((n[12]!=m[8])&&(n[12]!=m[9])&&(n[12]!=m[10])&&(n[12]!=m[11])&&(n[12]!=m[12])&&(n[12]!=m[13])&&(n[12]!=m[14])&&(n[12]!=m[15])&&(n[12]!=m[16])&&(n[12]!=m[17])&&(n[12]!=m[18])&&(n[12]!=m[19])) return false;
		if ((n[13]!=m[8])&&(n[13]!=m[9])&&(n[13]!=m[10])&&(n[13]!=m[11])&&(n[13]!=m[12])&&(n[13]!=m[13])&&(n[13]!=m[14])&&(n[13]!=m[15])&&(n[13]!=m[16])&&(n[13]!=m[17])&&(n[13]!=m[18])&&(n[13]!=m[19])) return false;
		if ((n[14]!=m[8])&&(n[14]!=m[9])&&(n[14]!=m[10])&&(n[14]!=m[11])&&(n[14]!=m[12])&&(n[14]!=m[13])&&(n[14]!=m[14])&&(n[14]!=m[15])&&(n[14]!=m[16])&&(n[14]!=m[17])&&(n[14]!=m[18])&&(n[14]!=m[19])) return false;
		if ((n[15]!=m[8])&&(n[15]!=m[9])&&(n[15]!=m[10])&&(n[15]!=m[11])&&(n[15]!=m[12])&&(n[15]!=m[13])&&(n[15]!=m[14])&&(n[15]!=m[15])&&(n[15]!=m[16])&&(n[15]!=m[17])&&(n[15]!=m[18])&&(n[15]!=m[19])) return false;
		if ((n[16]!=m[8])&&(n[16]!=m[9])&&(n[16]!=m[10])&&(n[16]!=m[11])&&(n[16]!=m[12])&&(n[16]!=m[13])&&(n[16]!=m[14])&&(n[16]!=m[15])&&(n[16]!=m[16])&&(n[16]!=m[17])&&(n[16]!=m[18])&&(n[16]!=m[19])) return false;
		if ((n[17]!=m[8])&&(n[17]!=m[9])&&(n[17]!=m[10])&&(n[17]!=m[11])&&(n[17]!=m[12])&&(n[17]!=m[13])&&(n[17]!=m[14])&&(n[17]!=m[15])&&(n[17]!=m[16])&&(n[17]!=m[17])&&(n[17]!=m[18])&&(n[17]!=m[19])) return false;
		if ((n[18]!=m[8])&&(n[18]!=m[9])&&(n[18]!=m[10])&&(n[18]!=m[11])&&(n[18]!=m[12])&&(n[18]!=m[13])&&(n[18]!=m[14])&&(n[18]!=m[15])&&(n[18]!=m[16])&&(n[18]!=m[17])&&(n[18]!=m[18])&&(n[18]!=m[19])) return false;
		if ((n[19]!=m[8])&&(n[19]!=m[9])&&(n[19]!=m[10])&&(n[19]!=m[11])&&(n[19]!=m[12])&&(n[19]!=m[13])&&(n[19]!=m[14])&&(n[19]!=m[15])&&(n[19]!=m[16])&&(n[19]!=m[17])&&(n[19]!=m[18])&&(n[19]!=m[19])) return false;
		break;
	case FE_HEX27:
		if ((n[0]!=m[0])&&(n[0]!=m[1])&&(n[0]!=m[2])&&(n[0]!=m[3])&&(n[0]!=m[4])&&(n[0]!=m[5])&&(n[0]!=m[6])&&(n[0]!=m[7])) return false;
		if ((n[1]!=m[0])&&(n[1]!=m[1])&&(n[1]!=m[2])&&(n[1]!=m[3])&&(n[1]!=m[4])&&(n[1]!=m[5])&&(n[1]!=m[6])&&(n[1]!=m[7])) return false;
		if ((n[2]!=m[0])&&(n[2]!=m[1])&&(n[2]!=m[2])&&(n[2]!=m[3])&&(n[2]!=m[4])&&(n[2]!=m[5])&&(n[2]!=m[6])&&(n[2]!=m[7])) return false;
		if ((n[3]!=m[0])&&(n[3]!=m[1])&&(n[3]!=m[2])&&(n[3]!=m[3])&&(n[3]!=m[4])&&(n[3]!=m[5])&&(n[3]!=m[6])&&(n[3]!=m[7])) return false;
		if ((n[4]!=m[0])&&(n[4]!=m[1])&&(n[4]!=m[2])&&(n[4]!=m[3])&&(n[4]!=m[4])&&(n[4]!=m[5])&&(n[4]!=m[6])&&(n[4]!=m[7])) return false;
		if ((n[5]!=m[0])&&(n[5]!=m[1])&&(n[5]!=m[2])&&(n[5]!=m[3])&&(n[5]!=m[4])&&(n[5]!=m[5])&&(n[5]!=m[6])&&(n[5]!=m[7])) return false;
		if ((n[6]!=m[0])&&(n[6]!=m[1])&&(n[6]!=m[2])&&(n[6]!=m[3])&&(n[6]!=m[4])&&(n[6]!=m[5])&&(n[6]!=m[6])&&(n[6]!=m[7])) return false;
		if ((n[7]!=m[0])&&(n[7]!=m[1])&&(n[7]!=m[2])&&(n[7]!=m[3])&&(n[7]!=m[4])&&(n[7]!=m[5])&&(n[7]!=m[6])&&(n[7]!=m[7])) return false;

		if ((n[ 8]!=m[8])&&(n[ 8]!=m[9])&&(n[ 8]!=m[10])&&(n[ 8]!=m[11])&&(n[ 8]!=m[12])&&(n[ 8]!=m[13])&&(n[ 8]!=m[14])&&(n[ 8]!=m[15])&&(n[ 8]!=m[16])&&(n[ 8]!=m[17])&&(n[ 8]!=m[18])&&(n[ 8]!=m[19])) return false;
		if ((n[ 9]!=m[8])&&(n[ 9]!=m[9])&&(n[ 9]!=m[10])&&(n[ 9]!=m[11])&&(n[ 9]!=m[12])&&(n[ 9]!=m[13])&&(n[ 9]!=m[14])&&(n[ 9]!=m[15])&&(n[ 9]!=m[16])&&(n[ 9]!=m[17])&&(n[ 9]!=m[18])&&(n[ 9]!=m[19])) return false;
		if ((n[10]!=m[8])&&(n[10]!=m[9])&&(n[10]!=m[10])&&(n[10]!=m[11])&&(n[10]!=m[12])&&(n[10]!=m[13])&&(n[10]!=m[14])&&(n[10]!=m[15])&&(n[10]!=m[16])&&(n[10]!=m[17])&&(n[10]!=m[18])&&(n[10]!=m[19])) return false;
		if ((n[11]!=m[8])&&(n[11]!=m[9])&&(n[11]!=m[10])&&(n[11]!=m[11])&&(n[11]!=m[12])&&(n[11]!=m[13])&&(n[11]!=m[14])&&(n[11]!=m[15])&&(n[11]!=m[16])&&(n[11]!=m[17])&&(n[11]!=m[18])&&(n[11]!=m[19])) return false;
		if ((n[12]!=m[8])&&(n[12]!=m[9])&&(n[12]!=m[10])&&(n[12]!=m[11])&&(n[12]!=m[12])&&(n[12]!=m[13])&&(n[12]!=m[14])&&(n[12]!=m[15])&&(n[12]!=m[16])&&(n[12]!=m[17])&&(n[12]!=m[18])&&(n[12]!=m[19])) return false;
		if ((n[13]!=m[8])&&(n[13]!=m[9])&&(n[13]!=m[10])&&(n[13]!=m[11])&&(n[13]!=m[12])&&(n[13]!=m[13])&&(n[13]!=m[14])&&(n[13]!=m[15])&&(n[13]!=m[16])&&(n[13]!=m[17])&&(n[13]!=m[18])&&(n[13]!=m[19])) return false;
		if ((n[14]!=m[8])&&(n[14]!=m[9])&&(n[14]!=m[10])&&(n[14]!=m[11])&&(n[14]!=m[12])&&(n[14]!=m[13])&&(n[14]!=m[14])&&(n[14]!=m[15])&&(n[14]!=m[16])&&(n[14]!=m[17])&&(n[14]!=m[18])&&(n[14]!=m[19])) return false;
		if ((n[15]!=m[8])&&(n[15]!=m[9])&&(n[15]!=m[10])&&(n[15]!=m[11])&&(n[15]!=m[12])&&(n[15]!=m[13])&&(n[15]!=m[14])&&(n[15]!=m[15])&&(n[15]!=m[16])&&(n[15]!=m[17])&&(n[15]!=m[18])&&(n[15]!=m[19])) return false;
		if ((n[16]!=m[8])&&(n[16]!=m[9])&&(n[16]!=m[10])&&(n[16]!=m[11])&&(n[16]!=m[12])&&(n[16]!=m[13])&&(n[16]!=m[14])&&(n[16]!=m[15])&&(n[16]!=m[16])&&(n[16]!=m[17])&&(n[16]!=m[18])&&(n[16]!=m[19])) return false;
		if ((n[17]!=m[8])&&(n[17]!=m[9])&&(n[17]!=m[10])&&(n[17]!=m[11])&&(n[17]!=m[12])&&(n[17]!=m[13])&&(n[17]!=m[14])&&(n[17]!=m[15])&&(n[17]!=m[16])&&(n[17]!=m[17])&&(n[17]!=m[18])&&(n[17]!=m[19])) return false;
		if ((n[18]!=m[8])&&(n[18]!=m[9])&&(n[18]!=m[10])&&(n[18]!=m[11])&&(n[18]!=m[12])&&(n[18]!=m[13])&&(n[18]!=m[14])&&(n[18]!=m[15])&&(n[18]!=m[16])&&(n[18]!=m[17])&&(n[18]!=m[18])&&(n[18]!=m[19])) return false;
		if ((n[19]!=m[8])&&(n[19]!=m[9])&&(n[19]!=m[10])&&(n[19]!=m[11])&&(n[19]!=m[12])&&(n[19]!=m[13])&&(n[19]!=m[14])&&(n[19]!=m[15])&&(n[19]!=m[16])&&(n[19]!=m[17])&&(n[19]!=m[18])&&(n[19]!=m[19])) return false;

		if ((n[20]!=m[20])&&(n[20]!=m[21])&&(n[20]!=m[22])&&(n[20]!=m[23])&&(n[20]!=m[24])&&(n[20]!=m[25])) return false;
		if ((n[21]!=m[20])&&(n[21]!=m[21])&&(n[21]!=m[22])&&(n[21]!=m[23])&&(n[21]!=m[24])&&(n[21]!=m[25])) return false;
		if ((n[22]!=m[20])&&(n[22]!=m[21])&&(n[22]!=m[22])&&(n[22]!=m[23])&&(n[22]!=m[24])&&(n[22]!=m[25])) return false;
		if ((n[23]!=m[20])&&(n[23]!=m[21])&&(n[23]!=m[22])&&(n[23]!=m[23])&&(n[23]!=m[24])&&(n[23]!=m[25])) return false;
		if ((n[24]!=m[20])&&(n[24]!=m[21])&&(n[24]!=m[22])&&(n[24]!=m[23])&&(n[24]!=m[24])&&(n[24]!=m[25])) return false;
		if ((n[25]!=m[20])&&(n[25]!=m[21])&&(n[25]!=m[22])&&(n[25]!=m[23])&&(n[25]!=m[24])&&(n[25]!=m[25])) return false;

		if (n[26]!=m[26]) return false;
		break;
	case FE_TET10:
		if ((n[0]!=m[0])&&(n[0]!=m[1])&&(n[0]!=m[2])&&(n[0]!=m[3])) return false;
		if ((n[1]!=m[0])&&(n[1]!=m[1])&&(n[1]!=m[2])&&(n[1]!=m[3])) return false;
		if ((n[2]!=m[0])&&(n[2]!=m[1])&&(n[2]!=m[2])&&(n[2]!=m[3])) return false;
		if ((n[3]!=m[0])&&(n[3]!=m[1])&&(n[3]!=m[2])&&(n[3]!=m[3])) return false;

		if ((n[4]!=m[4])&&(n[4]!=m[5])&&(n[4]!=m[6])&&(n[4]!=m[7])&&(n[4]!=m[8])&&(n[4]!=m[9])) return false;
		if ((n[5]!=m[4])&&(n[5]!=m[5])&&(n[5]!=m[6])&&(n[5]!=m[7])&&(n[5]!=m[8])&&(n[5]!=m[9])) return false;
		if ((n[6]!=m[4])&&(n[6]!=m[5])&&(n[6]!=m[6])&&(n[6]!=m[7])&&(n[6]!=m[8])&&(n[6]!=m[9])) return false;
		if ((n[7]!=m[4])&&(n[7]!=m[5])&&(n[7]!=m[6])&&(n[7]!=m[7])&&(n[7]!=m[8])&&(n[7]!=m[9])) return false;
		if ((n[8]!=m[4])&&(n[8]!=m[5])&&(n[8]!=m[6])&&(n[8]!=m[7])&&(n[8]!=m[8])&&(n[8]!=m[9])) return false;
		if ((n[9]!=m[4])&&(n[9]!=m[5])&&(n[9]!=m[6])&&(n[9]!=m[7])&&(n[9]!=m[8])&&(n[9]!=m[9])) return false;
		break;
	case FE_TET15:
		if ((n[0]!=m[0])&&(n[0]!=m[1])&&(n[0]!=m[2])&&(n[0]!=m[3])) return false;
		if ((n[1]!=m[0])&&(n[1]!=m[1])&&(n[1]!=m[2])&&(n[1]!=m[3])) return false;
		if ((n[2]!=m[0])&&(n[2]!=m[1])&&(n[2]!=m[2])&&(n[2]!=m[3])) return false;
		if ((n[3]!=m[0])&&(n[3]!=m[1])&&(n[3]!=m[2])&&(n[3]!=m[3])) return false;

		if ((n[4]!=m[4])&&(n[4]!=m[5])&&(n[4]!=m[6])&&(n[4]!=m[7])&&(n[4]!=m[8])&&(n[4]!=m[9])) return false;
		if ((n[5]!=m[4])&&(n[5]!=m[5])&&(n[5]!=m[6])&&(n[5]!=m[7])&&(n[5]!=m[8])&&(n[5]!=m[9])) return false;
		if ((n[6]!=m[4])&&(n[6]!=m[5])&&(n[6]!=m[6])&&(n[6]!=m[7])&&(n[6]!=m[8])&&(n[6]!=m[9])) return false;
		if ((n[7]!=m[4])&&(n[7]!=m[5])&&(n[7]!=m[6])&&(n[7]!=m[7])&&(n[7]!=m[8])&&(n[7]!=m[9])) return false;
		if ((n[8]!=m[4])&&(n[8]!=m[5])&&(n[8]!=m[6])&&(n[8]!=m[7])&&(n[8]!=m[8])&&(n[8]!=m[9])) return false;
		if ((n[9]!=m[4])&&(n[9]!=m[5])&&(n[9]!=m[6])&&(n[9]!=m[7])&&(n[9]!=m[8])&&(n[9]!=m[9])) return false;

		if ((n[10]!=m[10])&&(n[10]!=m[11])&&(n[10]!=m[12])&&(n[10]!=m[13])) return false;
		if ((n[11]!=m[10])&&(n[11]!=m[11])&&(n[11]!=m[12])&&(n[11]!=m[13])) return false;
		if ((n[12]!=m[10])&&(n[12]!=m[11])&&(n[12]!=m[12])&&(n[12]!=m[13])) return false;
		if ((n[13]!=m[10])&&(n[13]!=m[11])&&(n[13]!=m[12])&&(n[13]!=m[13])) return false;

		if (n[14]!=m[14]) return false;

		break;
	default:
		assert(false);
	}

	return true;
}

//-----------------------------------------------------------------------------
// This function is used by derived copy constructors and assignment operators
// to simplify copying the common element data.
void FEElement_::copy(const FEElement_& el)
{
	SetFEState(el.GetFEState());

	m_gid = el.m_gid;
	m_ntype = el.m_ntype;
	m_nodes = el.m_nodes;
	m_nfaces = el.m_nfaces;
	m_nedges = el.m_nedges;

	m_fiber = el.m_fiber;
	m_Q = el.m_Q;
	m_Qactive = el.m_Qactive;
	m_a0 = el.m_a0;
//	m_edata = el.m_edata;

	for (int i=0; i<m_nodes; ++i) m_node[i] = el.m_node[i];
}

//=============================================================================
// FETri3
//-----------------------------------------------------------------------------
FETri3::FETri3()
{
	m_node = _node;
	m_nbr = _nbr;
	m_face = _face;
	m_h = _h;

	m_ntype = FE_TRI3;
	m_nodes = 3;
	m_nfaces = 0;
	m_nedges = 3;

	m_node[0] = m_node[1] = m_node[2] = -1;
	m_nbr [0] = m_nbr [1] = m_nbr [2] = -1;
	m_face[0] = m_face[1] = m_face[2] = -1;
	m_h[0] = m_h[1] = m_h[2] = 0.0;
}

//-----------------------------------------------------------------------------
FETri3::FETri3(FETri3& el)
{
	// set data pointers
	m_node = _node;
	m_nbr = _nbr;
	m_face = _face;
	m_h	= _h;

	// copy common element data
	copy(el);

	// copy additional data
	m_face[0] = el.m_face[0];

	m_nbr[0] = el.m_nbr[0];
	m_nbr[1] = el.m_nbr[1];
	m_nbr[2] = el.m_nbr[2];

	m_h[0] = el.m_h[0];
	m_h[1] = el.m_h[1];
	m_h[2] = el.m_h[2];
}

//-----------------------------------------------------------------------------
FETri3& FETri3::operator = (FETri3& el)
{
	// copy common element data
	copy(el);

	// copy additional data
	m_face[0] = el.m_face[0];

	m_nbr[0] = el.m_nbr[0];
	m_nbr[1] = el.m_nbr[1];
	m_nbr[2] = el.m_nbr[2];

	m_h[0] = el.m_h[0];
	m_h[1] = el.m_h[1];
	m_h[2] = el.m_h[2];

	return *this;
}

//=============================================================================
// FEElement
//-----------------------------------------------------------------------------
FEElement::FEElement()
{ 
	m_node = _node;
	m_nbr = _nbr;
	m_face = _face;
	m_h	= _h;

	for (int i=0; i<MAX_NODES; ++i) m_node[i] = -1;
	for (int i=0; i<6; ++i) { m_nbr[i] = m_face[i] = -1; }
	for (int i=0; i<8; ++i) m_h[i] = 0.0;
}

//-----------------------------------------------------------------------------
FEElement::FEElement(const FEElement& el)
{
	m_node = _node;
	m_nbr = _nbr;
	m_face = _face;
	m_h	= _h;

	copy(el);

	for (int i=0; i<6; ++i) { m_nbr[i] = el.m_nbr[i]; m_face[i] = el.m_face[i]; }
	for (int i=0; i<8; ++i) m_h[i] = el.m_h[i];
}

//-----------------------------------------------------------------------------
FEElement& FEElement::operator = (const FEElement& el)
{
	copy(el);

	for (int i=0; i<6; ++i) { m_nbr[i] = el.m_nbr[i]; m_face[i] = el.m_face[i]; }
	for (int i=0; i<8; ++i) m_h[i] = el.m_h[i];

	return *this;
}

//-----------------------------------------------------------------------------
// Set the element type. This also sets some other type related data
void FEElement::SetType(int ntype)
{ 
	// set the type
	m_ntype = ntype; 

	// set the number of nodes
	const int N[MAX_ELEM] = {8,4,6,4,3,2,20,8,3,10,6,15,27};
	assert((m_ntype>0)&&(m_ntype <= MAX_ELEM));
	m_nodes = N[m_ntype-1];

	// set the number of faces (shells have no faces)
	const int F[MAX_ELEM] = {6,4,5,0,0,0,6,0,0,4,0,4,6};
	assert((m_ntype>0)&&(m_ntype <= MAX_ELEM));
	m_nfaces = F[m_ntype-1];

	// set the number of edges (solids have no edges)
	const int E[MAX_ELEM] = {0,0,0,4,3,1,0,4,1,0,3,0,0};
	assert((m_ntype>0)&&(m_ntype <= MAX_ELEM));
	m_nedges = E[m_ntype-1];
}
