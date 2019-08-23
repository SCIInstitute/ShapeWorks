#include "stdafx.h"
#include "FEVTKExport.h"
#include "FEMesh.h"

FEVTKExport::FEVTKExport(void)
{
	m_fp = 0;
	m_ops.bscalar_data = false;
	m_ops.bshellthick = false;
}

FEVTKExport::~FEVTKExport(void)
{
	Close();
}

void FEVTKExport::Close()
{
	if (m_fp) fclose(m_fp);
	m_fp = 0;
}

bool FEVTKExport::Export(FEMesh& mesh, const char* szfile)
{
	int j, n;

	m_fp = fopen(szfile, "wt");
	if (m_fp == 0) return false;

	// for now we put everything in one part
	int parts = 1;

	// count nr of nodes
	int nodes = mesh.Nodes();

	// --- H E A D E R ---
	fprintf(m_fp, "%s\n" , "# vtk DataFile Version 3.0");
	fprintf(m_fp, "%s\n", "vtk output");
	fprintf(m_fp, "%s\n", "ASCII");
	fprintf(m_fp, "%s\n", "DATASET POLYDATA");
	fprintf(m_fp, "%s %d %s\n", "POINTS", nodes, "float");

	//fprintf(fp, "%d %d %d %d\n", parts, nodes, faces, edges);

	// --- N O D E S ---
	FEMesh& m = mesh;
	for (j=0; j<m.Nodes(); ++j)
	{
		FENode& n = m.Node(j);
		fprintf(m_fp, "%g %g %g\n", n.r.x, n.r.y, n.r.z);
	}
	
	// --- F A C E S ---
	int faces = m.Faces();
	for (j=0; j<faces; ++j)
	{
		FEFace& f = m.Face(j);
		n = f.Nodes();
		if(j==0)
			fprintf(m_fp, "%s %d %d\n" ,"POLYGONS", faces, faces * (n+1));
		if (n == 3)
			fprintf(m_fp, "%d %d %d %d\n", n, f.n[0], f.n[1], f.n[2]);
		else if (n == 4)
			fprintf(m_fp, "%d %d %d %d %d\n", n, f.n[0], f.n[1], f.n[2], f.n[3]);
		else 
			assert(false);
	}

	//----Shell Thickness ----
	if (m_ops.bshellthick)
	{
		fprintf(m_fp, "%s\n", "");
		fprintf(m_fp, "%s %d\n", "POINT_DATA", nodes);
		fprintf(m_fp, "%s %s %s\n", "SCALARS", "ShellThickness", "float");
		fprintf(m_fp, "%s\n", "LOOKUP_TABLE default");

		FEMesh& m = mesh;

		vector<double> nodeShellThickness; 
		nodeShellThickness.reserve(nodes);
		int nn[8];
		for (int k = 0 ; k< nodes;k++)
			nodeShellThickness.push_back(0);

		for (j=0; j<m.Elements(); ++j)
		{
			FEElement& el = m.Element(j);
			if (!el.IsType(FE_TRI3) && !el.IsType(FE_QUAD4))
				break;

			for (int k=0; k<el.Nodes(); ++k) 
				nn[k] = el.m_node[k];

			double* h = el.m_h;
			
			for(int k =0;k<el.Nodes();k++)
				nodeShellThickness[nn[k]] = h[k];
		}

		for (j=0; j<m.Nodes();)
		{
			for (int k =0; k<9 && j+k<m.Nodes();k++)
				fprintf(m_fp, "%15.10lg ", nodeShellThickness[j+k]);	
			fprintf(m_fp, "\n");
			j = j + 9;	
		}
	}

	//-----Nodal Data-----------
	if (m_ops.bscalar_data)
	{
		fprintf(m_fp, "%s\n", "");
		fprintf(m_fp, "%s %d\n", "POINT_DATA", nodes);
		fprintf(m_fp, "%s %s %s\n", "SCALARS", "ScalarData", "float");
		fprintf(m_fp, "%s\n", "LOOKUP_TABLE default");

		for (j=0; j<m.Nodes();)
		{
			for (int k =0; k<9 && j+k<m.Nodes();k++)
			{
				FENode& n = m.Node(j+k);
					fprintf(m_fp, "%15.10lg ", n.m_ndata);
			}
			fprintf(m_fp, "\n");
			j = j + 9;	
		}
	}

	Close();

	return true;
}
