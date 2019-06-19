/*
Szymon Rusinkiewicz
Princeton University

TriMesh_connectivity.cc
Manipulate data structures that describe connectivity between faces and verts.
*/


#include <stdio.h>
#include "TriMesh.h"
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <math.h>

using std::find;


void TriMesh::need_Rinscribe()
{
	need_faceedges();
	int nf = faces.size();
	if (!radiusInscribe.empty())
	{
		return;
	}

	radiusInscribe.resize(nf);
	
	for (int i=0; i<nf; i++)
	{
		Face f = faces[i];
		double e1 = f.edgeLens[0];
		double e2 = f.edgeLens[1];
		double e3 = f.edgeLens[2];
		double s = (e1+e2+e3)/2.0;
		radiusInscribe[i] = sqrt(s *(s-e1) * (s-e2) * (s - e3)) / s;
	}
	
}

void TriMesh::need_meshinfo()
{
	int nf = faces.size();
	int numObtuse = 0;
	float maxAngle = 0;
	for (int i =0; i< nf; i++)
	{

		Face f = faces[i];
		for (int j=0;j<3; j++)
		{
			point A = vertices[f[j]];
			point B = vertices[f[(j+1)%3]];
			point C = vertices[f[(j+2)%3]];
			point AB = B - A;
			point AC = C - A;
			float angle = acos( AB DOT AC / (sqrt(AB DOT AB) * sqrt(AC DOT AC)));
			maxAngle = MAX(maxAngle, angle);
			if (angle >= M_PI / 2.0)
			{
				numObtuse++;

			}
			

			
		}
		

	}

	printf("number of obtuse triangles/total triangles = %d / %d\n", numObtuse, nf);
	printf("maximum angles = %f\n", maxAngle * 180 / M_PI);
	
}

//compute the edge length
void TriMesh::need_faceedges()
{
	if (faces.empty())
	{
		printf("No faces to compute face edges!!!\n");
		return;
	}
	int numFaces = faces.size();
	for (int i = 0; i < numFaces; i++)
	{
		Face f = faces[i];
		vec3 edge01 = (vec3)(vertices[f[1]] - vertices[f[0]]);
		vec3 edge12 = (vec3)(vertices[f[2]] - vertices[f[1]]);
		vec3 edge20 = (vec3)(vertices[f[0]] - vertices[f[2]]);
		faces[i].edgeLens[0] =sqrt(edge01[0]*edge01[0] + edge01[1]*edge01[1] + edge01[2]*edge01[2]);
		faces[i].edgeLens[1] =sqrt(edge12[0]*edge12[0] + edge12[1]*edge12[1] + edge12[2]*edge12[2]);
		faces[i].edgeLens[2] =sqrt(edge20[0]*edge20[0] + edge20[1]*edge20[1] + edge20[2]*edge20[2]);


	}

}

vector<float> rescale(vector<float> scalar, float rangemin, float rangemax )
{
	float orimax = 0;
	float orimin = 100000000;
	int nv = scalar.size();
	vector<float> result;
	result.resize(nv);
	for (int i =0; i< nv; i++)
	{
		orimin = MIN(orimin, scalar[i]);
		orimax = MAX(orimax, scalar[i]);
	}

	for ( int i=0; i<nv; i++)
	{
		result[i] = ( rangemax*(scalar[i] - orimin) + rangemin*(orimax-scalar[i]) )/ (orimax -orimin) ;
	}

	return result;
	
	
}
void TriMesh::need_noise(int nNoiseIter)
{
	if (!noiseOnVert.empty())
		return;


	FILE* file = fopen("noise_16.txt","r");
	//FILE* file = fopen("noiseSquare.1.txt","r");

	need_neighbors();
	int nv = vertices.size();
	noiseOnVert.resize(nv);
	srand( (unsigned)time( NULL ) );

	for (int i = 0;i<nv; i++)
	{
		//float up = 2;
		//float down = 0;
		//noiseOnVert[i] = (float)rand() / (RAND_MAX + 1)*(up - down) + down;  //random number between [donw,up)
		float wgn; 
		fscanf(file,"%f",&wgn); 
		noiseOnVert[i] = wgn;


	}

	fclose(file);

	vector< float> tmpNoiseOnVert;
	tmpNoiseOnVert.resize(nv);

	float dt = 1.0 / 20.0;
	//iterate 
	int maxIterNum = 1;
	int iterStep = 10;

	//FILE* diffnoisefile = fopen("diffnoise.txt", "w+");

	//fprintf(diffnoisefile,"%d\n", nv);
	//fprintf(diffnoisefile,"%d\n", maxIterNum / iterStep + 1 );
	//for (int maxiternum = 0; maxiternum <= maxIterNum; maxiternum +=iterStep)
	//{


	//	for (int i=0;i<nNoiseIter/*maxiternum*/; i++)
	//	{
	//		for (int j=0; j<nv;j++)
	//		{

	//			//noiseOnVert[j] = 0;
	//			vector<int> nbs = neighbors[j];
	//			float upvalue = 0;
	//			float downvalue = 0;

	//			for (int k=0;k<nbs.size();k++)
	//			{
	//				int nb = nbs[k];
	//				vector<int> nbnbs = neighbors[nb];
	//				vector<int> samenbs;
	//				//float areaPoly = 0;
	//				samenbs.clear();


	//				///////////////find the A and B//////////////////
	//				for (int numnbs =0; numnbs < nbnbs.size(); numnbs++)
	//				{
	//					for (int numCnbs = 0; numCnbs < nbs.size(); numCnbs++)
	//					{
	//						if (nbnbs[numnbs] == nbs[numCnbs])
	//						{
	//							samenbs.push_back(nbnbs[numnbs]);
	//						}

	//					}

	//				}

	//				//////////////////////////////////////////////////////
	//				if (samenbs.size() == 1)
	//				{
	//					point AC = vertices[j] - vertices[samenbs[0]];
	//					point AN = vertices[nb] - vertices[samenbs[0]];
	//					float alpha = acos( (AC DOT AN) / (sqrt(AC DOT AC) * sqrt( AN DOT AN)) );
	//					alpha = MIN(alpha,(float) 85.0*PI/180.0);
	//					alpha = MAX(alpha, (float)5.0*PI/180.0);
	//					float w = 1.0 / tan(alpha);
	//					downvalue += w;
	//					upvalue += w*noiseOnVert[nb];
	//					// upvalue +=0.5 * w* (noiseOnVert[j] - noiseOnVert[nb]);

	//				}
	//				else if (samenbs.size()==2)
	//				{
	//					point AC = vertices[j] - vertices[samenbs[0]];
	//					point AN = vertices[nb] - vertices[samenbs[0]];
	//					point BC = vertices[j] - vertices[samenbs[1]];
	//					point BN = vertices[nb] - vertices[samenbs[1]];
	//					float alpha = acos( (AC DOT AN) / (sqrt(AC DOT AC) * sqrt( AN DOT AN)) );
	//					alpha = MIN(alpha, (float)85.0*M_PI/180.0);
	//					alpha = MAX(alpha, (float)5.0*M_PI/180.0);

	//					float beta  = acos( (BC  DOT BN) / (sqrt(BC  DOT BC ) * sqrt( BN DOT BN)) );

	//					beta = MIN(beta, (float)85.0*M_PI/180.0);
	//					beta = MAX(beta,(float) 5.0*M_PI/180.0);
	//					float w = 1.0 / tan(alpha) + 1.0 / tan(beta);
	//					downvalue += w;
	//					upvalue += w*noiseOnVert[nb];
	//					//upvalue +=0.5 * w* (noiseOnVert[j] - noiseOnVert[nb]);

	//				}
	//				else
	//				{
	//					//printf("same nbs ERROR : not 1 or 2!!\n");
	//				}



	//				//noiseOnVert[j] +=noiseOnVert[neighbors[j][k]];

	//			}
	//			//noiseOnVert[j] /= nb.size();


	//			float delta = noiseOnVert[j] - upvalue / downvalue;
	//			tmpNoiseOnVert[j] = noiseOnVert[j] - dt * delta;
	//		}

	//		for (int buzhidao = 0; buzhidao < nv; buzhidao++)
	//		{
	//			noiseOnVert[buzhidao] = tmpNoiseOnVert[buzhidao];
	//		}



	//	}


		//for (int buzhidao = 0; buzhidao < nv; buzhidao++)
		//{
		//	noiseOnVert[buzhidao] = noiseOnVert[buzhidao]+1;
		//}

		//noiseOnVert = rescale(noiseOnVert, 0, 2);

		//fprintf(diffnoisefile,"%d\n", maxiternum);
		//for (int j=0; j<nv; j++)
		//{
		//	fprintf(diffnoisefile,"%f\n",noiseOnVert[j]);
		//}

		//colors.resize(nv);
		//for (int i =0; i< nv; i++)
		//{
		//	colors[i] = Color(noiseOnVert[i]/2.0,noiseOnVert[i]/2.0,noiseOnVert[i]/2.0);
		//}
		

		
		
		

	//}

	//fclose(diffnoisefile);



}

void TriMesh::need_speed()
{
	int nf = faces.size();
	//need_noise();

	//FILE* file = fopen("noiseSphereR40.txt","r");
	//FILE* file = fopen("noiseSquare.1.txt","r");
	
	for (int i =0; i<nf;i++)
	{
		Face f = faces[i];
		//switch (SPEEDTYPE)
		switch(speedType)
		{		
			case CURVATURE:
				//faces[i].speedInv = ( abs(curv1[f[0]] + curv2[f[0]]) +
				//											abs(curv1[f[1]] + curv2[f[1]]) + 
				//											abs(curv1[f[2]] + curv2[f[2]]) )/ 6.0;
            faces[i].speedInv = ( abs_curv[f[0]] + 
               abs_curv[f[1]] + 
               abs_curv[f[2]] ) / 3.0;

				break;
			case ONE:
				faces[i].speedInv = 1.0;
				break;
			case NOISE:
				faces[i].speedInv =( noiseOnVert[faces[i][0]] + noiseOnVert[faces[i][1]] + noiseOnVert[faces[i][2]] )/ 3;
				//float wgn; 
				//fscanf(file,"%f",&wgn); 
				//faces[i].speedInv =1.0 / wgn;//( noiseOnVert[faces[i][0]] + noiseOnVert[faces[i][1]] + noiseOnVert[faces[i][2]] )/ 3;
				//
				break;


		}

		
	}

	//fclose(file);
	
	

	
}


// Find the direct neighbors of each vertex
void TriMesh::need_neighbors()
{
	if (!neighbors.empty())
		return;
	need_faces();

	dprintf("Finding vertex neighbors... ");
	int nv = vertices.size(), nf = faces.size();

	vector<int> numneighbors(nv);
	for (int i = 0; i < nf; i++) {
		numneighbors[faces[i][0]]++;
		numneighbors[faces[i][1]]++;
		numneighbors[faces[i][2]]++;
	}

	neighbors.resize(nv);
	for (int i = 0; i < nv; i++)
		neighbors[i].reserve(numneighbors[i]+2); // Slop for boundaries

	for (int i = 0; i < nf; i++) {
		for (int j = 0; j < 3; j++) {
			vector<int> &me = neighbors[faces[i][j]];
			int n1 = faces[i][(j+1)%3];
			int n2 = faces[i][(j+2)%3];
			if (find(me.begin(), me.end(), n1) == me.end())
				me.push_back(n1);
			if (find(me.begin(), me.end(), n2) == me.end())
				me.push_back(n2);
		}
	}

	dprintf("Done.\n");
}

void TriMesh::need_oneringfaces()
{

	if (vertOneringFaces.empty())
	{
		vertOneringFaces.resize(vertices.size());
		for (int i=0; i< vertices.size();i++)
		{
			vertOneringFaces[i] = GetOneRing(i);

		}
		
	}
	
}

void TriMesh::need_kdtree()
{
	if ( !kd )
	{
    kd = new KDtree(this->vertices);
  }	

  if (maxEdgeLength == 0.0)
  {
    need_maxedgelength();
  }
}

void TriMesh::need_maxedgelength()
{
  need_faceedges();
  for (unsigned int f = 0; f < faces.size(); f++)
  {
    for (unsigned int d = 0; d < 3; d++)
    {
      if (faces[f].edgeLens[d] >= maxEdgeLength)
      {
        maxEdgeLength = faces[f].edgeLens[d];
      }
    }
  }
}


// Find the faces touching each vertex
void TriMesh::need_adjacentfaces()
{
	if (!adjacentfaces.empty())
		return;
	need_faces();

	dprintf("Finding vertex to triangle maps... ");
	int nv = vertices.size(), nf = faces.size();

	vector<int> numadjacentfaces(nv);
	for (int i = 0; i < nf; i++) {
		numadjacentfaces[faces[i][0]]++;
		numadjacentfaces[faces[i][1]]++;
		numadjacentfaces[faces[i][2]]++;
	}

	adjacentfaces.resize(vertices.size());
	for (int i = 0; i < nv; i++)
		adjacentfaces[i].reserve(numadjacentfaces[i]);

	for (int i = 0; i < nf; i++) {
		for (int j = 0; j < 3; j++)
			adjacentfaces[faces[i][j]].push_back(i);
	}

	dprintf("Done.\n");
}


// Find the face across each edge from each other face (-1 on boundary)
// If topology is bad, not necessarily what one would expect...
void TriMesh::need_across_edge()
{
	if (!across_edge.empty())
		return;
	need_adjacentfaces();

	dprintf("Finding across-edge maps... ");

	int nf = faces.size();
	across_edge.resize(nf, Face(-1,-1,-1));

	for (long long i = 0; i < nf; i++) {
		for (int j = 0; j < 3; j++) {
			if (-1 != across_edge[i][j])
				continue;
			int v1 = faces[i][(j+1)%3];
			int v2 = faces[i][(j+2)%3];
			const vector<int> &a1 = adjacentfaces[v1];
			const vector<int> &a2 = adjacentfaces[v2];
			for (int k1 = 0; k1 < a1.size(); k1++) {
				int other = a1[k1];
				if (other == i)
					continue;
				vector<int>::const_iterator it =
					find(a2.begin(), a2.end(), other);
				if (it == a2.end())
					continue;
				int ind = (faces[other].indexof(v1)+1)%3;
				if (faces[other][(ind+1)%3] != v2)
					continue;
				across_edge[i][j] = other;
				across_edge[other][ind] = i;
				break;
			}
		}
	}

	dprintf("Done.\n");
}

