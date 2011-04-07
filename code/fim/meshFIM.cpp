#include "meshFIM.h"
#include "Vec.h"
#include <math.h>
#include <GL/glui.h>
#include <sstream>
#include <fstream>
#include <stdio.h>
//#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <termios.h>


#define NB_ENABLE 0
#define NB_DISABLE 1


#ifndef PI
#define  PI 3.1415927
#endif
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

using namespace std;

typedef pair<unsigned int, float> gDistPair;

//void nonblock(int state)
//{
//    struct termios ttystate;
//    //get the terminal state
//    tcgetattr(STDIN_FILENO, &ttystate);
//
//    if (state==NB_ENABLE)
//    {
//        //turn off canonical mode
//        ttystate.c_lflag &= ~ICANON;
//        //minimum of number input read.
//        ttystate.c_cc[VMIN] = 1;
//    }
//    else if (state==NB_DISABLE)
//    {
//        //turn on canonical mode
//        ttystate.c_lflag |= ICANON;
//    }
//    //set the terminal attributes.
//    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
//
//}
//
//
//int kbhit()  
//{
//  struct timeval tv;  
//	fd_set fds;  
//	tv.tv_sec = 0;  
//	tv.tv_usec = 0;  
//	FD_ZERO(&fds);  
//	FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0  
//	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);  
//	return FD_ISSET(STDIN_FILENO, &fds);  
//}  
//
float meshFIM::LocalSolver(index vet, TriMesh::Face triangle, index currentVert)
{

	float a,b, delta, cosA, lamda1, lamda2, TC1, TC2;
	float TAB, TA, TB, TC;
	int A, B, C;
	float squareAB;
	float LenAB, LenBC, LenAC, LenCD, LenAD;
	float EdgeTA, EdgeTB;

 	float speedI;
	//if (SPEEDTYPE == CURVATURE){
	if(m_meshPtr->speedType == CURVATURE){
		speedI = 100 * triangle.speedInv;
	}
	else{
		speedI = triangle.speedInv;
	}

	C = triangle.indexof(vet);
	A = (C + 1) % 3;
	B = (A + 1) % 3;

	TC1 = LARGENUM;
	TC2 = LARGENUM;

	/*
	TA = m_meshPtr->vertT[currentVert][triangle[A]];
	TB = m_meshPtr->vertT[currentVert][triangle[B]];
	TC = m_meshPtr->vertT[currentVert][triangle[C]];
	*/
	/*
	TA = m_meshPtr->vertMap[currentVert][triangle[A]].d;
	TB = m_meshPtr->vertMap[currentVert][triangle[B]].d;
	TC = m_meshPtr->vertMap[currentVert][triangle[C]].d;
	*/
	TA = m_meshPtr->geodesic[triangle[A]];
	TB = m_meshPtr->geodesic[triangle[B]];
	TC = m_meshPtr->geodesic[triangle[C]];


	TAB = TB - TA;

	LenAB = triangle.edgeLens[A];     
	LenBC = triangle.edgeLens[B];   
	LenAC = triangle.edgeLens[C];   

	a = (speedI*speedI*LenAB*LenAB- TAB * TAB)*LenAB*LenAB;

	EdgeTA = TA /*oldValues1 */+ LenAC /*s_triMem[tx*TRIMEMLENGTH + 0]*/ * speedI;
	EdgeTB = TB /*oldValues2*/ + LenBC /*s_triMem[tx*TRIMEMLENGTH + 2]*/ * speedI;

	if (a > 0)
	{
		cosA = (LenAC * LenAC + LenAB * LenAB - LenBC * LenBC) / (2 * LenAC * LenAB);
		b = 2 * LenAB * LenAC * cosA * (TAB * TAB - speedI*speedI*LenAB*LenAB);
		delta = 4 * LenAC * LenAC  * a *  TAB * TAB * (1 - cosA * cosA);

		lamda1 = (-b + sqrt(delta))/(2*a);
		lamda2 = (-b - sqrt(delta))/(2*a);

		if (lamda1>=0&&lamda1<=1)
		{
			LenAD = lamda1*LenAB;
			LenCD = sqrt(LenAC*LenAC+LenAD*LenAD-2*LenAC*LenAD*cosA);
			TC1 = lamda1*TAB+TA+LenCD*speedI;
		}
		if(lamda2>=0&&lamda2<=1)
		{
			LenAD = lamda2*LenAB;
			LenCD = sqrt(LenAC*LenAC+LenAD*LenAD-2*LenAC*LenAD*cosA);
			TC2 = lamda2*TAB+TA+LenCD*speedI;
		}

		TC = MIN(TC, MIN(TC2, MIN(TC1,MIN(EdgeTA,EdgeTB))) );
	}

	else
	{
		TC = MIN(TC, MIN(EdgeTA,EdgeTB) );
	}

	return TC;
}

float meshFIM::Upwind(index currentVert,index vet)
{
	float result=LARGENUM;
	float tmp;
	vector<TriMesh::Face> neighborFaces = m_meshPtr->GetOneRing(vet/*, currentVert*/);
	// WAS THIS LINE BELOW ON Nov17 - Change Back when GetOneRing works for speed!
	//vector<TriMesh::Face> neighborFaces = m_meshPtr->vertOneringFaces[vet];

	//vector<int> neighborFaces = m_meshPtr->adjacentfaces[vet];
	int i;
	for (i=0;i< neighborFaces.size();i++)
	{
		tmp = LocalSolver(vet, neighborFaces[i], currentVert);
		//tmp = LocalSolver(vet, m_meshPtr->faces[neighborFaces[i]], currentVert);
		NumComputation++;

		result = MIN(result,tmp );
	    //m_meshPtr->vertT[currentVert][vet] = result;
	}

	return result;
}


/*
void meshFIM::GenerateData()
{	
	std::list<index>::iterator iter = m_ActivePoints.begin();
	float oldT1 , newT1, oldT2, newT2;
	index tmpIndex1, tmpIndex2;
	vector<int>  nb; 
	int i;
	NumComputation = 0;


	for (int currentVert= 0; currentVert< m_meshPtr->vertices.size(); currentVert++ )
	{
		std::vector<int> seedPointList(1, currentVert);
		
		SetSeedPoint(seedPointList);

		m_meshPtr->InitializeAttributes(currentVert, m_SeedPoints);
	
		InitializeLabels();
		InitializeActivePoints();

		clock_t starttime, endtime;
		starttime = clock ();

		while (!m_ActivePoints.empty())
		{
			//printf("Size of Activelist is: %d \n", m_ActivePoints.size());
			iter = m_ActivePoints.begin();
			
			while(iter != m_ActivePoints.end()) {
				tmpIndex1 = *iter;
				nb = m_meshPtr->neighbors[tmpIndex1];
				oldT1 = m_meshPtr->vertT[currentVert][tmpIndex1];
				newT1 = Upwind(currentVert,tmpIndex1);
				//			m_meshPtr->vertT[tmpIndex1] = newT1;

				if (abs(oldT1-newT1)<_EPS)    //if converges
				{
					if (oldT1>newT1)
						m_meshPtr->vertT[currentVert][tmpIndex1] = newT1;

					if (m_meshPtr->vertT[currentVert][tmpIndex1] < m_StopDistance)
					{
						for (i=0;i<nb.size();i++)
						{
							tmpIndex2 = nb[i];
							if (m_Label[tmpIndex2]==AlivePoint || m_Label[tmpIndex2]==FarPoint)
							{
								oldT2 = m_meshPtr->vertT[currentVert][tmpIndex2]; 
								newT2 = Upwind(currentVert,tmpIndex2);
								if (oldT2>newT2)
								{
									m_meshPtr->vertT[currentVert][tmpIndex2] = newT2;
									if (m_Label[tmpIndex2]!=ActivePoint)
									{
										m_ActivePoints.insert(iter, tmpIndex2);
										//iter++;
										m_Label[tmpIndex2] = ActivePoint;
									}
								}
							}
						}
					}
				
					iter =  m_ActivePoints.erase(iter);
					m_Label[tmpIndex1] = AlivePoint;

				}
				else   // if not converge
				{
					if(newT1 < oldT1)
						m_meshPtr->vertT[currentVert][tmpIndex1] = newT1;

					iter++;
					
				}
			}
		}

		endtime = clock();
		double duration = (double)(endtime - starttime) *1000 / CLOCKS_PER_SEC;
		printf("Processing time : %.10lf ms\n",duration);
	}

	printf("Number of computation is : %d\n", NumComputation);
}
*/

void meshFIM::GenerateReducedData()
{	
	std::list<index>::iterator iter = m_ActivePoints.begin();
	float oldT1 , newT1, oldT2, newT2;
	index tmpIndex1, tmpIndex2;
	vector<int>  nb; 
	NumComputation = 0;
	double total_duration = 0;

	clock_t starttime, endtime;
	starttime = clock ();

	char c;  
  int i=0;  
  
	//nonblock(NB_ENABLE);  

	for (int currentVert= 0; currentVert< m_meshPtr->vertices.size(); currentVert++ )
	{	
		std::vector<int> seedPointList(1, currentVert);
		SetSeedPoint(seedPointList);

		m_meshPtr->InitializeAttributes(currentVert, m_SeedPoints);
	
		InitializeLabels();
		InitializeActivePoints();


		while (!m_ActivePoints.empty())
		{
			//printf("Size of Activelist is: %d \n", m_ActivePoints.size());
			iter = m_ActivePoints.begin();
			
			while(iter != m_ActivePoints.end()) {
				tmpIndex1 = *iter;
				nb = m_meshPtr->neighbors[tmpIndex1];
				//oldT1 = m_meshPtr->vertT[currentVert][tmpIndex1];
				//oldT1 = m_meshPtr->vertMap[currentVert][tmpIndex1].d;
				oldT1 = m_meshPtr->geodesic[tmpIndex1];

				newT1 = Upwind(currentVert,tmpIndex1);

				if (abs(oldT1-newT1)<_EPS)    //if converges
				{
					if (oldT1>newT1){
						//m_meshPtr->vertT[currentVert][tmpIndex1] = newT1;
						//m_meshPtr->vertMap[currentVert][tmpIndex1].d = newT1;
						m_meshPtr->geodesic[tmpIndex1] = newT1;
					}

					//if (m_meshPtr->vertT[currentVert][tmpIndex1] < m_StopDistance)
					//if (m_meshPtr->vertMap[currentVert][tmpIndex1].d < m_StopDistance)
					if(m_meshPtr->geodesic[tmpIndex1] < m_StopDistance)
					{
						for (i=0;i<nb.size();i++)
						{
							tmpIndex2 = nb[i];
							if (m_Label[tmpIndex2]==AlivePoint || m_Label[tmpIndex2]==FarPoint)
							{
								//oldT2 = m_meshPtr->vertT[currentVert][tmpIndex2]; 
								//oldT2 = m_meshPtr->vertMap[currentVert][tmpIndex2].d;
								oldT2 = m_meshPtr->geodesic[tmpIndex2];

								newT2 = Upwind(currentVert,tmpIndex2);
								if (oldT2>newT2)
								{
									//m_meshPtr->vertT[currentVert][tmpIndex2] = newT2;
									//m_meshPtr->vertMap[currentVert][tmpIndex2].d = newT2;
									m_meshPtr->geodesic[tmpIndex2] = newT2;

									if (m_Label[tmpIndex2]!=ActivePoint)
									{
										m_ActivePoints.insert(iter, tmpIndex2);
										//iter++;
										m_Label[tmpIndex2] = ActivePoint;
									}
								}
							}
						}
					}
								
					iter =  m_ActivePoints.erase(iter);
					m_Label[tmpIndex1] = AlivePoint;

				}
				else   // if not converge
				{
					if(newT1 < oldT1){
						//m_meshPtr->vertT[currentVert][tmpIndex1] = newT1;
						//m_meshPtr->vertMap[currentVert][tmpIndex1].d = newT1;
						m_meshPtr->geodesic[tmpIndex1] = newT1;
					}

					iter++;
					
				}
			}
		}

		// Loop Through And Copy Only Values < than m_StopDistance
		int nv = m_meshPtr->vertices.size();
    int s = m_meshPtr->scaleFactor;
		for(int v= 0; v < currentVert; v++){
			if ((m_meshPtr->geodesic[v] <= m_StopDistance) && (m_meshPtr->geodesic[v] > 0))
      {
				//m_meshPtr->geoMap[currentVert][v] = m_meshPtr->geodesic[v];
				//(*(m_meshPtr->dMap))[currentVert].push_back(m_meshPtr->geodesic[v]);
				//(*(m_meshPtr->iMap))[currentVert].push_back(v);
        (m_meshPtr->geodesicMap[currentVert])[v] = (int)(m_meshPtr->geodesic[v]*s);       
			}
		}

		// Now Erase the duplicate data
		m_meshPtr->CleanupAttributes(currentVert);
		
		

		endtime = clock();
		total_duration = (double)(endtime - starttime) / CLOCKS_PER_SEC;
		float percent = (currentVert+1) / (float)m_meshPtr->vertices.size();

		
		double total_time = total_duration / percent;
		double time_left = total_time - total_duration;
		int hours = (int)(time_left / (60*60));
		int minutes = (int)((time_left - hours*60*60) / 60);
		int seconds = (int)(time_left - hours*60*60 - minutes*60);
		int count = (m_meshPtr->geodesicMap[currentVert]).size();
		
		//printf("\r                                              \r");
		//printf("Progress %.1f%%", 100 * percent);
		//printf("     ETA: %dh:%dm:%ds ", hours, minutes, seconds);
		//printf(" v: %d", count);
		//fflush(stdout);

		//i=kbhit();  
		//if (i!=0)
		//{  
		//	c=fgetc(stdin);  
		//	if (c=='c')  
		//		i=1;  
  //    else  
		//		i=0;  
		//	
		//}
		//
		//// Check to see if user has canceled this
		//if(i){
		//	cout << endl << "User Canceled Computation" << endl << endl;
		//	break;
		//}
	}

	//nonblock(NB_DISABLE);
}

void meshFIM::loadGeodesicFile(TriMesh *mesh, const char *geoFileName)
{
	cout << "Looking for file: " << geoFileName << " ... " << flush;

	int numVert = mesh->vertices.size();
	//mesh->geoMap.resize(numVert);
	//mesh->geoIndex.resize(numVert);
  unsigned int s = mesh->scaleFactor;

  mesh->geodesicMap.resize(numVert);

	ifstream infile(geoFileName, ifstream::in);
	if (!infile.is_open())
  {
		cout << "File Not Found" << endl;
    this->computeFIM(mesh,geoFileName);
	}
	else
  {
		int vertex = 0;
		unsigned int distance;
		string line;

		// First Line In File Is Stop Distance
		getline(infile, line);
		stringstream str(line);
		str >> distance;

    this->SetStopDistance((float)distance/(float)s);
		cout << "Found it!" << endl << "Loading " << geoFileName << endl;
    
    distance = 0;
		for(int i=0; i < numVert; i++)
    {
      line.clear();
		  getline(infile, line);
      str.clear();
      str.str(line);
			str >> vertex >> distance;
			while(!str.eof())
      {
				//mesh->geoMap[i].push_back(distance);
				//mesh->geoIndex[i].push_back(vertex);
        (mesh->geodesicMap[i])[vertex] = distance;
				str >> vertex >> distance;
			}

			//printf("\r                                              \r");
			//printf("Progress %.1f%%", (i+1.0f)/(numVert)*100);
			//fflush(stdout);
				
		}
		cout << endl;
		infile.close();
	}	
	
}

void meshFIM::computeFIM(TriMesh *mesh, const char *vertT_filename)
{
	cout << "Trying to load: " << vertT_filename << endl;
  //	FILE* vertTFile = fopen(vertT_filename, "r+");
	ifstream infile(vertT_filename, ifstream::in);
	
	int numVert = mesh->vertices.size();
	//(mesh->dMap)->resize(numVert);
	//(mesh->iMap)->resize(numVert);

  mesh->geodesicMap.resize(numVert);

	this->SetMesh(mesh);

  unsigned int s = mesh->scaleFactor;
	
	if (!infile.is_open()){
		//vertTFile = fopen(vertT_filename, "w+");
		ofstream outfile(vertT_filename, ifstream::out);
		cout << "No vertT file!!!\n Writing..." << endl;
		cout << "stop distance = " << this->GetStopDistance() << endl;
		cout << "# vertices in mesh: " << numVert << endl;
				
		this->GenerateReducedData();   

		// First Line In File Is Stop Distance
		outfile << this->GetStopDistance() * s << endl;

		//// Loop Over Each Vertex
		//for (int i = 0; i < numVert; i++)
		//{
		//	// Loop Over Each (Vertex,Distance) pair			
		//	for(int j=0; j < (*(mesh->dMap))[i].size(); j++){
		//		outfile << (*(mesh->iMap))[i][j] << " " << (*(mesh->dMap))[i][j] << " ";
		//	}

		//	outfile << endl;
		//}

		// Loop Over Each Vertex
		for (int i = 0; i < numVert; i++)
		{
      std::map<unsigned int,unsigned int>::iterator mIter; 
					
      for(mIter = mesh->geodesicMap[i].begin(); mIter != mesh->geodesicMap[i].end(); mIter++)
      {
        outfile << (*mIter).first << " " << (*mIter).second << " ";
			}

			outfile << endl;
		}

		outfile.close();
	}
	else{
		unsigned int vertex;
		unsigned int distance;
		string line;

		// First Line In File Is Stop Distance
		getline(infile, line);
		stringstream str(line);
		str >> distance;
    		
		this->SetStopDistance((float)distance/(float)s);
    cout << "Loading " << vertT_filename << endl;

		for(int i=0; i < numVert; i++){
			
			string line;
			getline(infile, line);
			stringstream str(line);
			
			str >> vertex >> distance;
			while(!str.eof())
      {
				//mesh->geoMap[i].push_back(distance);
				//mesh->geoIndex[i].push_back(vertex);
        (mesh->geodesicMap[i])[vertex] = distance;
				str >> vertex >> distance;
			}

			//printf("\r                                              \r");
			//printf("progress %.1f%%", (i+1.0f)/(numVert)*100);
			//fflush(stdout);
				
		}
		cout << endl;
		infile.close();
	}	
	
	
}
