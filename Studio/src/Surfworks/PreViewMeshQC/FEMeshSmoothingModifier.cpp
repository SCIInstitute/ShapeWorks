#include "stdafx.h"
#include "FEMeshSmoothingModifier.h"
#include "FEAutoMesher.h"
#include "FENodeFaceList.h"
#include "FENodeNodeList.h"
#include "FEMesh.h"
#include <exception>
#include <stdexcept>
#include <algorithm>

//-----------------------------------------------------------------------------
//! Constructor
FEMeshSmoothingModifier::FEMeshSmoothingModifier()
{
	m_threshold1 = 0.0;
	m_threshold2 = 0.0;
	m_iteration = 1;
	m_method = 1;
}


//-----------------------------------------------------------------------------
//! Create the decimate mesh. 
//! \todo This implementation will only work with closed surfaces. 

FEMesh* FEMeshSmoothingModifier::Apply(FEMesh* pm)
{
  if (pm == NULL) {
    throw std::runtime_error("FEMesher failed.");
  }
	// make sure this is a triangle mesh
  if (pm->IsType(FE_TRI3) == false) {
    throw std::runtime_error("FEMesher failed.");
  }

	// make a copy of this mesh
	FEMesh* pnew = new FEMesh(*pm);

	//marking the edge nodes.
	vector<int> hashmap; 
	hashmap.reserve(pm->Nodes());
	for (int i = 0 ; i< pm->Nodes();i++)
		hashmap.push_back(0);	
	for (int i =0;i<pm->Edges();++i)
	{
		FEEdge& ed = pm->Edge(i);
		hashmap[ed.n[0]] = -1;
		hashmap[ed.n[1]] = -1;
	}
	switch(m_method)
	{
		case 0:
			Laplacian_Smoothing(pnew,hashmap);
			break;
		case 1:
			Laplacian_Smoothing2(pnew,hashmap);
			break;
		case 2:
			Taubin_Smoothing(pnew,hashmap);
			break;
		case 3:
			Crease_Enhancing_Diffusion(pnew,hashmap);
			break;
		case 4:	
			Add_Noise(pnew,hashmap);
			break;
		default:
			break;
	}
	FEAutoMesher mesher;
	mesher.BuildMesh(pnew);
	return pnew;
}

void FEMeshSmoothingModifier::Laplacian_Smoothing(FEMesh* pnew,vector<int> hashmap)
{
	//Creating a node node list
	FENodeNodeList NNL(pnew);
	NNL.Build();
	for(int j =0 ;j<m_iteration;j++)
	{
		for(int i = 0; i < pnew->Nodes() ; i++)
		{
			if(hashmap[i] == 0)
			{
				FENode& ni = pnew->Node(i);
				vec3d r_new; 
				for (int k = 0; k<NNL.Valence(i);k++)
				{
					vec3d x = pnew->Node(NNL.Node(i, k)).r;
					r_new = r_new + x;
				}
				r_new = r_new/NNL.Valence(i);
				r_new =(r_new * m_threshold1) + (ni.r * (1-m_threshold1));
				ni.r = r_new;
			}
		}
	}
}

void FEMeshSmoothingModifier::Laplacian_Smoothing2(FEMesh* pnew,vector<int> hashmap)
{
	//Creating a node node list
	FENodeNodeList NNL(pnew);
	NNL.Build();
	for(int j =0 ;j<m_iteration;j++)
	{
		for(int i = 0; i < pnew->Nodes() ; i++)
		{
			if(hashmap[i] == 0)
			{
				FENode& ni = pnew->Node(i);
				vec3d r_new; 
				double sum_dist=0;
				for (int k = 0; k<NNL.Valence(i);k++)
				{
					vec3d x = pnew->Node(NNL.Node(i, k)).r;
					double dist = distance(x,ni.r);
					r_new = r_new + (x * dist);
					sum_dist += dist;
				}
				r_new = r_new/sum_dist;
				r_new =(r_new * m_threshold1) + (ni.r * (1-m_threshold1));
				ni.r = r_new;
			}
		}
	}
}

void FEMeshSmoothingModifier::Taubin_Smoothing(FEMesh* pnew,vector<int> hashmap)
{
	//Creating a node node list
	FENodeNodeList NNL(pnew);
	NNL.Build();
	vector<vec3d>phi_node;
	phi_node.reserve(pnew->Nodes());
	for(int j =0 ;j<m_iteration;j++)
	{		
		for(int i = 0; i < pnew->Nodes() ; i++)
		{
			FENode& ni = pnew->Node(i);
			vec3d r_sum;
			for (int k = 0; k<NNL.Valence(i);k++)
			{
				vec3d x = pnew->Node(NNL.Node(i, k)).r;
				r_sum += x;
			}
			r_sum = r_sum/NNL.Valence(i);
			r_sum -= ni.r;
			phi_node.push_back(r_sum);
		}		
		for(int i = 0; i < pnew->Nodes() ; i++)
		{
			if(hashmap[i] == 0)
			{
				FENode& ni = pnew->Node(i);
				vec3d phi_old = phi_node[i];

				vec3d r_sq_sum,phi_sq_old; 
				for (int k = 0; k<NNL.Valence(i);k++)
				{
					int neigh_node = NNL.Node(i, k);
					r_sq_sum += phi_node[neigh_node];
				}
				phi_sq_old = r_sq_sum/NNL.Valence(i);
				phi_sq_old -= phi_old;

				ni.r = ni.r - (phi_old * (m_threshold2 - m_threshold1)) - (phi_sq_old *(m_threshold1*m_threshold2));
			}
		}
		phi_node.clear();
	}
}

void FEMeshSmoothingModifier::Crease_Enhancing_Diffusion(FEMesh* pnew,vector<int> hashmap)
{
	//creating Node Element list
	FENodeFaceList NFL(pnew);
	NFL.Build();

	//calculating m(R) for each face i.e for each triangle	
	vector<FEFace*> mR ;
	vector<vec3d>m_R;
	vector<vec3d>m_R_new;	
	mR.reserve(pnew->Faces());
	m_R.reserve(pnew->Faces());
	m_R_new.reserve(pnew->Faces());

	//for first iteration m_R are normals
	for(int i =0; i< pnew->Faces();i++)
	{
		FEFace& fa = pnew->Face(i);
		m_R.push_back(fa.m_fn);
	}
	for (int iter = 0 ; iter< m_iteration;iter++)
	{				
		//for each face calculate m_R
		for(int i =0;i<pnew->Faces();i++)
		{
			FEFace& fa = pnew->Face(i);				
			vec3d centroid_R = (pnew->Node(fa.n[0]).r + pnew->Node(fa.n[1]).r + pnew->Node(fa.n[2]).r )/3;
			//finding the neighbouring faces
			for(int j =0 ;j<3 ;j++)
			{
				int nodeID = fa.n[j];
				for (int k = 0; k<NFL.Valence(nodeID);k++)
				{
					FEFace *fa1 = NFL.Face(nodeID,k);
					if(fa1->m_ntag != 1 && fa1->m_elem[0] != i)
					{
						fa1->m_ntag = 1;
						mR.push_back(fa1);
					}
				}					
			}
			//now we have the neighbouring faces in mR list
			double weight =0;
			m_R_new.push_back(vec3d(0,0,0));
			for(int k =0;k<mR.size();k++)
			{
				FEFace *fa1 = mR[k];
				vec3d r[3]; //three nodes of the face
				r[0] = pnew->Node(fa1->n[0]).r;
				r[1] = pnew->Node(fa1->n[1]).r;
				r[2] = pnew->Node(fa1->n[2]).r;
				vec3d centroid_S = (r[0]+r[1]+r[2])/3;
				double dist = distance(centroid_S,centroid_R);
				double angle = acos((fa.m_fn * fa1->m_fn)/(fa.m_fn.Length() * fa1->m_fn.Length()));//angle between the normals
				double weight1 = area_triangle(r) * exp(-m_threshold1 * angle*angle*dist*dist);
				weight += weight1;
				m_R_new[i] += m_R[fa1->m_elem[0]] * weight1;	
				fa1->m_ntag = 0;
			}
			m_R_new[i] = m_R_new[i]/weight;	
			mR.clear();
		}
		//we have m_R_new for each face.
		for(int i =0 ;i<m_R_new.size();i++)
		{
			m_R[i] = m_R_new[i];
		}
		m_R_new.clear();
		//For each node modify its coodinates
		for(int i = 0 ;i < pnew->Nodes();i++)
		{
			if(hashmap[i] == 0) //not the edge node
			{
				FENode& ni = pnew->Node(i);
				vec3d vR; 
				double weight=0;
				for (int k = 0; k<NFL.Valence(i);k++)
				{
					FEFace *fa1 = NFL.Face(i,k);
					vec3d r[3]; //three nodes of the face
					r[0] = pnew->Node(fa1->n[0]).r;
					r[1] = pnew->Node(fa1->n[1]).r;
					r[2] = pnew->Node(fa1->n[2]).r;
					vec3d centroid = (r[0]+r[1]+r[2])/3;
					weight += area_triangle(r);
					vec3d PC = centroid - ni.r;
					double temp = PC * m_R[fa1->m_elem[0]];
					vR += (m_R[fa1->m_elem[0]] * temp)*area_triangle(r);
				}	
				vR = vR/weight;
				ni.r = ni.r + vR;
			}				
		}
	}//end of one iteration
}

void FEMeshSmoothingModifier::Add_Noise(FEMesh* pnew,vector<int> hashmap)
{
	for(int j =0 ;j<m_iteration;j++)
	{
		for(int i = 0; i < pnew->Nodes() ; i++)
		{
			if(hashmap[i] == 0)
			{
				FENode& ni = pnew->Node(i); 
				int temp = 1;
				if(int(random()*10000)%2)temp=-1; else temp = 1;
				ni.r.x += temp*random();
				if(int(random()*10000)%2)temp=-1; else temp = 1;
				ni.r.y += temp*random();
				if(int(random()*10000)%2)temp=-1; else temp = 1;
				ni.r.z += temp*random();					
			}
		}
	}
}

double FEMeshSmoothingModifier::area_triangle(vec3d r[3])
{
	vec3d e1 = r[1] - r[0];
	vec3d e2 = r[2] - r[0];

	return (e1 ^ e2).Length()*0.5;
}

double FEMeshSmoothingModifier::distance(vec3d x,vec3d y )
{
	double dist = sqrt((x.x - y.x)*(x.x - y.x) + (x.y - y.y)*(x.y - y.y) + (x.z-y.z)*(x.z-y.z));
							

	return dist;
}

double FEMeshSmoothingModifier::random()
{
	static unsigned int nseed = 47856987;
	nseed = 789789812*nseed + 38569741 + rand();
	nseed =  (int)(nseed%100);						
	double temp = nseed;
	return (temp/10000);
}
