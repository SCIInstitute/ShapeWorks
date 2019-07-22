#include "FEVTKImport.h"
#include "FEMesh.h"
#include "FEAutoMesher.h"
#include <string.h>

FEVTKimport::FEVTKimport()
{
	m_fp = 0;	
}

FEVTKimport::~FEVTKimport(void)
{
	Close();
}

void FEVTKimport::Close()
{
	if (m_fp) fclose(m_fp);
	m_fp = 0;
}

FEMesh* FEVTKimport::errf(const char* szfmt, ...)
{
	Close();
	return 0;
}

FEMesh* FEVTKimport::Load(const char* szfile)
{
	m_fp = fopen(szfile, "rt");
	if (m_fp == 0) return errf("Failed opening file %s.", szfile);

	char szline[256] = {0}, *ch;
	int nread, i,j,k;
	char type[256];
	double temp[9];
	bool isASCII=false, isPOLYDATA = false, isUnstructuredGrid = false, isCellData = false,isScalar=false,isShellThickness=false;
	// read the header
	do
	{
		ch = fgets(szline, 255, m_fp);
		if (ch == 0) return errf("An unexpected error occured while reading the file data.");
		if (strstr(ch, "ASCII") != 0) isASCII = true;
		if (strstr(ch, "POLYDATA") != 0) isPOLYDATA = true;
		if (strstr(ch, "UNSTRUCTURED_GRID") != 0) isUnstructuredGrid = true;
	}
	while(strstr(ch, "POINTS") == 0);
	if (!isASCII)
		return errf("Only ASCII files are read");

	//get number of nodes
	int nodes = atoi(ch+6);
	
	int size = 0;
	int nparts = 0;
	int edges = 0;
	int elems = 0;
	if (nodes <= 0) return errf("Invalid number of nodes.");
	

	// create a new mesh
	FEMesh* pm = new FEMesh();
	pm->Create(nodes, 0);
	
	// read the nodes
	//Check how many nodes are there in each line
	ch = fgets(szline, 255, m_fp);
	if (ch == 0) return errf("An unexpected error occured while reading the file data.");

	nread = sscanf(szline, "%lg%lg%lg%lg%lg%lg%lg%lg%lg%s", &temp[0],&temp[1],&temp[2], &temp[3],&temp[4],&temp[5], &temp[6],&temp[7],&temp[8]);
	if (nread%3 != 0 && nread>9) 
		return errf("An error occured while reading the nodal coordinates.");
	int nodes_each_row = nread/3;
	double temp2 = double(nodes)/nodes_each_row;
	int rows = ceil(temp2);
	for (i=0; i<rows; ++i)
	{	
		for (j=0,k=0;j<nodes_each_row && i*nodes_each_row+j <nodes;j++)
		{
			FENode& n= pm->Node(i*nodes_each_row+j);
			vec3d& r = n.r;
			r.x = temp[k];
			r.y = temp[k+1];
			r.z = temp[k+2];
			k +=3;
		}
		ch = fgets(szline, 255, m_fp);
		if (ch == 0) return errf("An unexpected error occured while reading the file data.");

		nread = sscanf(szline, "%lg%lg%lg%lg%lg%lg%lg%lg%lg%s", &temp[0],&temp[1],&temp[2], &temp[3],&temp[4],&temp[5], &temp[6],&temp[7],&temp[8]);
		if (nread%3 != 0 && nread != -1)
		{ 			
			if (i+1 != nodes/nodes_each_row)
				return errf("An error occured while reading the nodal coordinates.");
		}
		//after reading 1000 rows update the progress bar
//		if ((i)%300==0) CheckProgress();
	}
	//Reading element data
	while(1)
	{		
		if (ch == 0) return errf("An unexpected error occured while reading the file data.");
		if (strstr(ch, "POLYGONS") != 0 || strstr(ch, "CELLS") != 0 ) 
		{
			sscanf(szline, "%s %d %d",type,&elems,&size);
			break;
		}
		ch = fgets(szline, 255, m_fp);
	}

	if(elems == 0||size == 0)
		return errf("Only POLYGON/CELL dataset format is supported.");

	pm->Create(0, elems);

	// read the elements
	int n[5];
	for (i=0; i<elems; ++i)
	{
		FEElement& el = pm->Element(i);
		el.m_gid = 0;
		ch = fgets(szline, 255, m_fp);
		if (ch == 0) return errf("An unexpected error occured while reading the file data.");
		nread = sscanf(szline, "%d%d%d%d%d", &n[0], &n[1], &n[2], &n[3], &n[4]);
		switch (n[0])
		{
		case 3: 
			el.SetType(FE_TRI3);
			el.m_node[0] = n[1];
			el.m_node[1] = n[2];
			el.m_node[2] = n[3];
			break;
		case 4:
			if(isPOLYDATA)
				el.SetType(FE_QUAD4);
			if(isUnstructuredGrid)
				el.SetType(FE_TET4);
			el.m_node[0] = n[1];
			el.m_node[1] = n[2];
			el.m_node[2] = n[3];
			el.m_node[3] = n[4];
			break;
		default:
			delete pm;
			return errf("Only triangular and quadrilateral polygons are supported.");
		}
//		if ((i)%300==0) CheckProgress();
	}
	ch = fgets(szline, 255, m_fp);

	while (ch != NULL) //making sure the file doesn't ends here
	{
		//reading the point data
		do
		{	
			if (ch == NULL) break;
			if (ch == 0) return errf("An unexpected error occured while reading the file data.");
			if (strstr(ch, "POINT_DATA") != 0)
				size = atoi(ch+10);
			if(strstr(ch,"CELL_DATA")!=0)
			{
				size = atoi(ch + 9);
				isCellData = true;
			}
			if (strstr(ch, "SCALARS") != 0) isScalar = true;
			if (strstr(ch, "ShellThickness") != 0) isShellThickness = true;
			ch = fgets(szline, 255, m_fp);
		}
		while(ch == NULL || strstr(ch, "LOOKUP_TABLE") == 0);

		if(!isScalar && ch !=NULL)
			return errf("Only scalar data is supported.");	
		vector<double> data; 
		//reading shell thickness
		if(isShellThickness)
		{			
			data.reserve(size);
			//Check how many nodes are there in each line
			ch = fgets(szline, 255, m_fp);
			if (ch == 0) return errf("An unexpected error occured while reading the file data.");

			nodes_each_row = sscanf(szline, "%lg%lg%lg%lg%lg%lg%lg%lg%lg%s", &temp[0],&temp[1],&temp[2], &temp[3],&temp[4],&temp[5], &temp[6],&temp[7],&temp[8]);
			if (nodes_each_row>9) 
				return errf("An error occured while reading the nodal coordinates.");
			double temp2 = double(size)/nodes_each_row;
			rows = ceil(temp2);
			for (i=0; i<rows; ++i)
			{	
				for (j=0;j<nodes_each_row && i*nodes_each_row+j <size;j++)
				{
					data.push_back(temp[j]);
				}
				ch = fgets(szline, 255, m_fp);
				if (ch == 0 && i+1 != rows) 
					return errf("An unexpected error occured while reading the scalar data.");

				nread = sscanf(szline, "%lg%lg%lg%lg%lg%lg%lg%lg%lg%s", &temp[0],&temp[1],&temp[2], &temp[3],&temp[4],&temp[5], &temp[6],&temp[7],&temp[8]);
				if (nread > 9 && nread != -1)
				{ 			
					if (i+1 != rows)
						return errf("An error occured while reading the scalar.");
				}
			}
			//Assigning the scalar feild to the mesh
			//if scalar feild is shell thickness
			int nn[8];
			for (i=0; i<elems; ++i)
			{
				FEElement& el = pm->Element(i);
				for (int k=0; k<el.Nodes(); ++k) 
					nn[k] = el.m_node[k];
				double	h[4];
				for(int k =0;k<3;k++)
					h[k] = data[nn[k]];
				if(el.IsType(FE_QUAD4))
					h[3] = data[nn[3]];

				el.m_h[0] = h[0];
				el.m_h[1] = h[1];
				el.m_h[2] = h[2];
				if(el.IsType(FE_QUAD4))
					el.m_h[3] = h[3];				
			}
		}
		//reading cell data
		if(isCellData)
		{
			for (i=0; i<size; ++i)
			{
				FEElement& el = pm->Element(i);
				ch = fgets(szline, 255, m_fp);
				if (ch == 0) return errf("An unexpected error occured while reading the file data.");
				nread = sscanf(szline, "%lg", &temp[0]);
				el.m_gid = temp[0];
			}
		}		
		break;
	}

	Close();

	FEAutoMesher mesher;
	mesher.BuildMesh(pm);

	return pm;
}
