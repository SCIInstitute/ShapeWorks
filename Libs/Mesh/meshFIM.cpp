#include "meshFIM.h"
#include "Vec.h"
#include <math.h>
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

#ifndef LARGENUM
#define LARGENUM 10000000.0
#endif


using namespace std;

typedef pair<unsigned int, float> gDistPair;

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

    for (int currentVert= 0; currentVert< m_meshPtr->vertices.size(); currentVert++ ) {
        // if ((currentVert%100) == 0)
//        std::cout << "vertex " << currentVert+1 << " of " << m_meshPtr->vertices.size() << " vertices ..." << std::endl;

        std::vector<int> seedPointList(1, currentVert);
        SetSeedPoint(seedPointList);

        m_meshPtr->InitializeAttributes(currentVert, m_SeedPoints);

        InitializeLabels();
        InitializeActivePoints();


        while (!m_ActivePoints.empty()) {
            //printf("Size of Activelist is: %d \n", m_ActivePoints.size());
            iter = m_ActivePoints.begin();

            while(iter != m_ActivePoints.end()) {
                tmpIndex1 = *iter;
                nb = m_meshPtr->neighbors[tmpIndex1];
                oldT1 = m_meshPtr->geodesic[tmpIndex1];

                newT1 = Upwind(currentVert,tmpIndex1);

                if (abs(oldT1-newT1)<_EPS)    //if converges {
                    if (oldT1>newT1){
                        m_meshPtr->geodesic[tmpIndex1] = newT1;
                    }

                    if(m_meshPtr->geodesic[tmpIndex1] < m_StopDistance) {
                        for (i=0;i<nb.size();i++) {
                            tmpIndex2 = nb[i];
                            if (m_Label[tmpIndex2]==AlivePoint || m_Label[tmpIndex2]==FarPoint) {
                                oldT2 = m_meshPtr->geodesic[tmpIndex2];

                                newT2 = Upwind(currentVert,tmpIndex2);
                                if (oldT2>newT2) {
                                    m_meshPtr->geodesic[tmpIndex2] = newT2;

                                    if (m_Label[tmpIndex2]!=ActivePoint) {
                                        m_ActivePoints.insert(iter, tmpIndex2);
                                        m_Label[tmpIndex2] = ActivePoint;
                                    }
                                }
                            }
                        }
                    }

                    iter =  m_ActivePoints.erase(iter);
                    m_Label[tmpIndex1] = AlivePoint;
                }
                else   // if not converge {
                    if(newT1 < oldT1){
                        m_meshPtr->geodesic[tmpIndex1] = newT1;
                    }
                    iter++;
                }
            }
        }
        // Loop Through And Copy Only Values < than m_StopDistance
        int nv = m_meshPtr->vertices.size();
        for(int v= 0; v < currentVert; v++){
            if ((m_meshPtr->geodesic[v] <= m_StopDistance) && (m_meshPtr->geodesic[v] > 0))
            {
                (m_meshPtr->geodesicMap[currentVert])[v] = m_meshPtr->geodesic[v];
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
    }
}

// SHIREEN - modified the loading to control the generation of geo files (till we add the geo repulsion stuff)
void meshFIM::loadGeodesicFile(TriMesh *mesh, const char *geoFileName)
{
//    cout << "Looking for file: " << geoFileName << " ... " << flush;

    ifstream infile(geoFileName, std::ios::binary);
    if (!infile.is_open())
    {
        if(GENERATE_GEO_FILES == 1)
        {
//            cout << "File Not Found, will generate the geo file now ..." << endl;

            int numVert = mesh->vertices.size();
            //mesh->geoMap.resize(numVert);
            //mesh->geoIndex.resize(numVert);

            mesh->geodesicMap.resize(numVert);

            this->computeFIM(mesh,geoFileName);
        }
//        else
//            cout << "File Not Found and geo file generation is DISABLED ..." << endl;
    }
    else
    {
        int numVert = mesh->vertices.size();
        //mesh->geoMap.resize(numVert);
        //mesh->geoIndex.resize(numVert);

        mesh->geodesicMap.resize(numVert);

        // read stop distance
        float distance;
        infile.read(reinterpret_cast<char *>(&distance), sizeof(float));
        this->SetStopDistance(distance);

        // loop over vertices
        for (int i = 0; i < numVert; i++)
        {
            // read map size for vertex
            unsigned int dLength;
            infile.read( reinterpret_cast<char *>(&dLength), sizeof(unsigned int) );

            // read key and distance pair
            for (int j = 0; j < dLength; j++)
            {
                unsigned int index;
                infile.read( reinterpret_cast<char *>(&index), sizeof(unsigned int) );

                float dist;
                infile.read( reinterpret_cast<char *>(&dist), sizeof(float) );

                (mesh->geodesicMap[i])[index] = dist;
            }
        }

        //int vertex = 0;
        //unsigned int distance;
        //string line;

        //// First Line In File Is Stop Distance
        //getline(infile, line);
        //stringstream str(line);
        //str >> distance;

        //  this->SetStopDistance((float)distance/(float)s);
        //cout << "Found it!" << endl << "Loading " << geoFileName << endl;
        //
        //  distance = 0;
        //for(int i=0; i < numVert; i++)
        //  {
        //    line.clear();
        //  getline(infile, line);
        //    str.clear();
        //    str.str(line);
        //	str >> vertex >> distance;
        //	while(!str.eof())
        //    {
        //		//mesh->geoMap[i].push_back(distance);
        //		//mesh->geoIndex[i].push_back(vertex);
        //      (mesh->geodesicMap[i])[vertex] = distance;
        //		str >> vertex >> distance;
        //	}

        //	//printf("\r                                              \r");
        //	//printf("Progress %.1f%%", (i+1.0f)/(numVert)*100);
        //	//fflush(stdout);
        //
        //}
        //cout << endl;
        infile.close();
    }

}
// end SHIREEN

//Praful
void meshFIM::computeCoordXFiles(TriMesh *mesh, const char *vertT_filename)
{
    unsigned int numVert = mesh->vertices.size();
    this->SetMesh(mesh);

    ofstream outfile(vertT_filename, std::ios::binary);

    cout << "# vertices in mesh: " << numVert << endl;

    outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        float coordVal = mesh->vertices[i][0];
        outfile.write( reinterpret_cast<char *>(&coordVal), sizeof(float) );
    }
    outfile.close();
}

void meshFIM::computeCoordYFiles(TriMesh *mesh, const char *vertT_filename)
{
    unsigned int numVert = mesh->vertices.size();
    this->SetMesh(mesh);

    ofstream outfile(vertT_filename, std::ios::binary);

    cout << "# vertices in mesh: " << numVert << endl;

    outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        float coordVal = mesh->vertices[i][1];
        outfile.write( reinterpret_cast<char *>(&coordVal), sizeof(float) );
    }
    outfile.close();
}

void meshFIM::computeCoordZFiles(TriMesh *mesh, const char *vertT_filename)
{
    unsigned int numVert = mesh->vertices.size();
    this->SetMesh(mesh);

    ofstream outfile(vertT_filename, std::ios::binary);

    cout << "# vertices in mesh: " << numVert << endl;

    outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        float coordVal = mesh->vertices[i][2];
        outfile.write( reinterpret_cast<char *>(&coordVal), sizeof(float) );
    }
    outfile.close();
}

void meshFIM::computeCurvFiles(TriMesh *mesh, const char *vertT_filename)
{
    unsigned int numVert = mesh->vertices.size();
    this->SetMesh(mesh);

    ofstream outfile(vertT_filename, std::ios::binary);

    cout << "# vertices in mesh: " << numVert << endl;

    outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        float absCurvVal = mesh->abs_curv[i];
        outfile.write( reinterpret_cast<char *>(&absCurvVal), sizeof(float) );
    }

    outfile.close();
}

//end Praful


void meshFIM::computeFIM(TriMesh *mesh, const char *vertT_filename)
{
    cout << "Trying to load: " << vertT_filename << endl;
    //	FILE* vertTFile = fopen(vertT_filename, "r+");
    ifstream infile(vertT_filename, std::ios::binary);

    unsigned int numVert = mesh->vertices.size();
    //(mesh->dMap)->resize(numVert);
    //(mesh->iMap)->resize(numVert);

    mesh->geodesicMap.resize(numVert);

    this->SetMesh(mesh);

    if (!infile.is_open())
    {
        //vertTFile = fopen(vertT_filename, "w+");
//        ofstream outfile(vertT_filename, std::ios::binary);
        ofstream outfile(vertT_filename, std::ofstream::out);
        cout << "No vertT file!!!\n Writing..." << endl;
        cout << "stop distance = " << this->GetStopDistance() << endl;
        cout << "# vertices in mesh: " << numVert << endl;

        this->GenerateReducedData();

        // write stop distance
        float dStop = this->GetStopDistance();
//        outfile.write( reinterpret_cast<char *>(&dStop), sizeof(float) );

        // loop over each vertex
        for (int i = 0; i < numVert; i++)
        {
            // write map size for vertex
            unsigned int dLength = mesh->geodesicMap[i].size();
//            outfile.write( reinterpret_cast<char *>(&dLength), sizeof(unsigned int) );

            // write key and distance pair
            for (std::map<unsigned int,float>::iterator it= mesh->geodesicMap[i].begin(); it != mesh->geodesicMap[i].end(); it++)
            {
                unsigned int index = (*it).first;
//                outfile.write( reinterpret_cast<char *>(&index), sizeof(unsigned int) );

                float dist = (*it).second;
//                outfile.write( reinterpret_cast<char *>(&dist), sizeof(float) );

                outfile << i << " " << index << " " << dist << std::endl;
            }
        }

        //// First Line In File Is Stop Distance
        //outfile << this->GetStopDistance() * s << endl;

        //// Loop Over Each Vertex
        //for (int i = 0; i < numVert; i++)
        //{
        //    std::map<unsigned int,unsigned int>::iterator mIter;
        //
        //    for(mIter = mesh->geodesicMap[i].begin(); mIter != mesh->geodesicMap[i].end(); mIter++)
        //    {
        //      outfile << (*mIter).first << " " << (*mIter).second << " ";
        //	}

        //	outfile << endl;
        //}

        outfile.close();
    }
    else
    {
        // read stop distance
        float distance;
        infile.read(reinterpret_cast<char *>(&distance), sizeof(float));
        this->SetStopDistance(distance);

        // loop over vertices
        for (int i = 0; i < numVert; i++)
        {
            // read map size for vertex
            unsigned int dLength;
            infile.read( reinterpret_cast<char *>(&dLength), sizeof(unsigned int) );

            // read key and distance pair
            for (int j = 0; j < dLength; j++)
            {
                unsigned int index;
                infile.read( reinterpret_cast<char *>(&index), sizeof(unsigned int) );

                float dist;
                infile.read( reinterpret_cast<char *>(&dist), sizeof(float) );

                (mesh->geodesicMap[i])[index] = dist;
            }
        }

        //unsigned int vertex;
        //unsigned int distance;
        //string line;

        //// First Line In File Is Stop Distance
        //getline(infile, line);
        //stringstream str(line);
        //str >> distance;
        //
        //this->SetStopDistance((float)distance/(float)s);
        //  cout << "Loading " << vertT_filename << endl;

        //for(int i=0; i < numVert; i++){
        //
        //	string line;
        //	getline(infile, line);
        //	stringstream str(line);
        //
        //	str >> vertex >> distance;
        //	while(!str.eof())
        //    {
        //		//mesh->geoMap[i].push_back(distance);
        //		//mesh->geoIndex[i].push_back(vertex);
        //      (mesh->geodesicMap[i])[vertex] = distance;
        //		str >> vertex >> distance;
        //	}

        //	//printf("\r                                              \r");
        //	//printf("progress %.1f%%", (i+1.0f)/(numVert)*100);
        //	//fflush(stdout);
        //
        //}
        //cout << endl;
        infile.close();
    }
}

// Praful - compute distance to landmarks based on geodesic approximation with given triangle info
void meshFIM::ComputeDistanceToLandmarksGivenTriangleInfo(TriMesh *mesh, const char *infilename , const char *outfilename)
{
    // initialize the geodesic map to hold the geodesics from the triangle vertices of the given landmark to all other mesh vertices
    SetStopDistance(1e7);
    int numVert = mesh->vertices.size();
    mesh->geodesicMap.resize(numVert);
    SetMesh(mesh);

    std::ifstream pointsFile(infilename);
    ofstream outfile(outfilename);

    if (!pointsFile)
    {
        std::cerr << "points file not found: " << infilename << std::endl;
    }
    int count = 0;
    while(pointsFile)
    {
        point tmpPt; tmpPt.clear();

        vnl_vector<float> baryCoords(3,0.0);

        int faceId;

        for (int d=0; d<3; d++)
            pointsFile >> tmpPt[d];
        if (!pointsFile)
        {
            count--;
            break;
        }
        pointsFile >> faceId;
        if (!pointsFile)
        {
            count--;
            break;
        }
        for (int d=0; d<3; d++)
            pointsFile >> baryCoords[d];
        if (!pointsFile)
        {
            count--;
            break;
        }
        TriMesh::Face triangleX = mesh->faces[faceId];
        std::vector<int> vertexIdlist;
        vertexIdlist.clear();
        vertexIdlist.push_back(triangleX.v[0]);
        vertexIdlist.push_back(triangleX.v[1]);
        vertexIdlist.push_back(triangleX.v[2]);

        // update the geodesic map with geodesic distances from each triangle vertex to all other mesh vertices
        UpdateGeodesicMapWithDistancesFromVertices(vertexIdlist);
        std::cout << "Point# " << count++ << " fid: " << faceId << " baryCoords: " << baryCoords[0] << " " << baryCoords[1] << " " << baryCoords[2] << std::endl;

        for (int i = 0; i < numVert; i++)
        {
//            std::cout << "Vertex: " << i << std::endl;
            point curVertex = mesh->vertices[i];
            TriMesh::Face vertFace = mesh->faces[mesh->adjacentfaces[i][0]];
            vec barycentric = mesh->ComputeBarycentricCoordinates(curVertex, vertFace);
            vnl_vector<float> baryVert(3, 0.0);
            for (int d = 0; d < 3; d++)
                baryVert[d] = barycentric[d];
            float distToLandmark = mesh->GetBronsteinGeodesicDistance(triangleX, vertFace, baryCoords, baryVert, (char*) "LM");
            outfile << distToLandmark << " ";
        }
        outfile << std::endl;
    }
    std::cout << "Total number of points: " << count+1 << std::endl;
    pointsFile.close();
    outfile.close();
}


// SHIREEN - compute distance to landmarks based on geodesic approximation
void meshFIM::ComputeDistanceToLandmark(TriMesh *mesh, point landmark, bool apply_log, const char *outfilename)
{
    // initialize the geodesic map to hold the geodesics from the triangle vertices of the given landmark to all other mesh vertices
    int numVert = mesh->vertices.size();
    mesh->geodesicMap.resize(numVert);
    SetMesh(mesh);

    // get which triangle the given landmark should belong to
    TriMesh::Face triangleX;
    float alphaX, betaX, gammaX;

    int faceId = mesh->GetTriangleInfoForPoint(landmark, triangleX, alphaX, betaX, gammaX);

    std::vector<int> vertexIdlist;
    vertexIdlist.clear();
    vertexIdlist.push_back(triangleX.v[0]);
    vertexIdlist.push_back(triangleX.v[1]);
    vertexIdlist.push_back(triangleX.v[2]);

    // update the geodesic map with geodesic distances from each triangle vertex to all other mesh vertices
    UpdateGeodesicMapWithDistancesFromVertices(vertexIdlist);

    // now start approximating the geodesics to the given landmarks based on the geodesics to its triangle vertices
    // write out distance to curve
    ofstream outfile(outfilename, std::ios::binary);

    // write numVertices to facilitate reading later
    outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

    //std::cout << "vertices (" << numVert << "): ";
    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        //if ((i % 50) == 0)
        //    std::cout << std::endl;

        //std::cout << i << ", ";
        point curVertex = mesh->vertices[i];
        float distToLandmark = mesh->GetBronsteinGeodesicDistance(landmark, curVertex, (char*) "LM");
        distToLandmark      += 0.00001f;
        if (apply_log)
            distToLandmark = std::log(distToLandmark);
        // write distance to curve
        outfile.write( reinterpret_cast<char *>(&distToLandmark), sizeof(float) );
    }
    //std::cout << std::endl;
    outfile.close();

}

void meshFIM::UpdateGeodesicMapWithDistancesFromVertices(std::vector<int> vertexIdlist)
{
    std::list<index>::iterator iter = m_ActivePoints.begin();
    float oldT1 , newT1, oldT2, newT2;
    index tmpIndex1, tmpIndex2;
    vector<int>  nb;
    int i = 0;

    for (int ii = 0 ; ii < vertexIdlist.size(); ii++)
    {
        int currentVert = vertexIdlist[ii];

        std::vector<int> seedPointList(1, currentVert);
        SetSeedPoint(seedPointList);

        m_meshPtr->InitializeAttributes(currentVert, m_SeedPoints);

        InitializeLabels();
        InitializeActivePoints();

        // to enable computing geodesic from the current vertex to all mesh vertices
        SetStopDistance(LARGENUM);
        while (!m_ActivePoints.empty())
        {
            //printf("Size of Activelist is: %d \n", m_ActivePoints.size());
            iter = m_ActivePoints.begin();

            while(iter != m_ActivePoints.end()) {
                tmpIndex1 = *iter;
                nb = m_meshPtr->neighbors[tmpIndex1];
                oldT1 = m_meshPtr->geodesic[tmpIndex1];

                newT1 = Upwind(currentVert,tmpIndex1);

                if (abs(oldT1-newT1)<_EPS)    //if converges
                {
                    if (oldT1>newT1){
                        m_meshPtr->geodesic[tmpIndex1] = newT1;
                    }

                    if(m_meshPtr->geodesic[tmpIndex1] < m_StopDistance)
                    {
                        for (i=0;i<nb.size();i++)
                        {
                            tmpIndex2 = nb[i];
                            if (m_Label[tmpIndex2]==AlivePoint || m_Label[tmpIndex2]==FarPoint)
                            {
                                oldT2 = m_meshPtr->geodesic[tmpIndex2];

                                newT2 = Upwind(currentVert,tmpIndex2);
                                if (oldT2>newT2)
                                {
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
                        m_meshPtr->geodesic[tmpIndex1] = newT1;
                    }

                    iter++;
                }
            }
        }

        // Loop Through And Copy Only Values < than
        int nv = m_meshPtr->vertices.size();
        for(int v = 0; v < nv; v++){
            if ((m_meshPtr->geodesic[v] <= m_StopDistance) && (m_meshPtr->geodesic[v] > 0))
            {
                int v1 = currentVert;
                int v2 = v;

                int vert = v1;
                int key = v2;

                if (v2 > v1)
                {
                    vert = v2;
                    key = v1;
                }
                //(m_meshPtr->geodesicMap[currentVert])[v] = m_meshPtr->geodesic[v];
                (m_meshPtr->geodesicMap[vert])[key] = m_meshPtr->geodesic[v];
            }
        }

        // Now Erase the duplicate data
        m_meshPtr->CleanupAttributes(currentVert);
    }

}

// end SHIREEN

// SHIREEN - compute distance to curve based on geodesic approximation
void meshFIM::ComputeDistanceToCurve(TriMesh *mesh, std::vector< point > curvePoints, const char *outfilename)
{
    // initialize the geodesic map to hold the geodesics from the triangle vertices of the given landmark to all other mesh vertices
    int numVert = mesh->vertices.size();
    mesh->geodesicMap.resize(numVert);
    SetMesh(mesh);

    // get which triangle the given landmark should belong to
    TriMesh::Face triangleX;
    float alphaX, betaX, gammaX;

    std::vector<int> vertexIdlist;
    vertexIdlist.clear();
    for (int pIndex = 0; pIndex < curvePoints.size(); pIndex++)
    {
        int faceId = mesh->GetTriangleInfoForPoint(curvePoints[pIndex], triangleX, alphaX, betaX, gammaX);
        
        vertexIdlist.push_back(triangleX.v[0]);
        vertexIdlist.push_back(triangleX.v[1]);
        vertexIdlist.push_back(triangleX.v[2]);
    }
    
    // update the geodesic map with geodesic distances from each triangle vertex of each curve point to all other mesh vertices
    UpdateGeodesicMapWithDistancesFromVertices(vertexIdlist);

    // now start approximating the geodesics to the given curve points based on the geodesics to its triangle vertices
    // write out distance to curve
    ofstream outfile(outfilename, std::ios::binary);

    // write numVertices to facilitate reading later
    outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

    //std::cout << "vertices (" << numVert << "): ";
    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        float distToCurve = 1e20;
        
        point curVertex = mesh->vertices[i];
        for (int pIndex = 0; pIndex < curvePoints.size(); pIndex++)
        {
            point landmark = curvePoints[pIndex];
            float curDist  = mesh->GetBronsteinGeodesicDistance(landmark, curVertex, (char*) "LM");
            
            if (curDist < distToCurve)
                distToCurve = curDist;
        }
        distToCurve      += 0.00001f;

        // write distance to curve
        outfile.write( reinterpret_cast<char *>(&distToCurve), sizeof(float) );
    }
    //std::cout << std::endl;
    outfile.close();
}
// end SHIREEN

// SHIREEN - computing geo distance on the fly for fuzzy geodesics
std::vector<float> meshFIM::ComputeDistanceToCurve(TriMesh *mesh, std::vector< point > curvePoints)
{
    int numVert = mesh->vertices.size();
    mesh->geodesicMap.resize(numVert);
    SetMesh(mesh);

    std::list<index>::iterator iter = m_ActivePoints.begin();
    float oldT1 , newT1, oldT2, newT2;
    index tmpIndex1, tmpIndex2;
    vector<int>  nb;
    NumComputation = 0;

    double total_duration = 0;
    char c;
    int i=0;

    std::vector<int> seedPointList;
    for (int pIndex = 0; pIndex < curvePoints.size(); pIndex++)
    {
        // SHIREEN
        seedPointList.push_back( mesh->GetVertexInfoForPoint(curvePoints[pIndex]) );
        //seedPointList.push_back( mesh->FindNearestVertex(curvePoints[pIndex]) );
        // end SHIREEN
    }
    SetSeedPoint(seedPointList);

    m_meshPtr->InitializeAttributes(0, m_SeedPoints);

    InitializeLabels();
    InitializeActivePoints();

    SetStopDistance(LARGENUM);

    while (!m_ActivePoints.empty())
    {
        //printf("Size of Activelist is: %d \n", m_ActivePoints.size());
        iter = m_ActivePoints.begin();

        while(iter != m_ActivePoints.end())
        {
            tmpIndex1 = *iter;
            nb = m_meshPtr->neighbors[tmpIndex1];
            oldT1 = m_meshPtr->geodesic[tmpIndex1];

            newT1 = Upwind(0,tmpIndex1);

            if (abs(oldT1-newT1)<_EPS)    //if converges
            {
                if (oldT1>newT1)
                {
                    m_meshPtr->geodesic[tmpIndex1] = newT1;
                }

                if(m_meshPtr->geodesic[tmpIndex1] < m_StopDistance)
                {
                    for (i=0;i<nb.size();i++)
                    {
                        tmpIndex2 = nb[i];
                        if (m_Label[tmpIndex2]==AlivePoint || m_Label[tmpIndex2]==FarPoint)
                        {
                            oldT2 = m_meshPtr->geodesic[tmpIndex2];

                            newT2 = Upwind(0,tmpIndex2);
                            if (oldT2>newT2)
                            {
                                m_meshPtr->geodesic[tmpIndex2] = newT2;

                                if (m_Label[tmpIndex2]!=ActivePoint)
                                {
                                    m_ActivePoints.insert(iter, tmpIndex2);
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
                {
                    m_meshPtr->geodesic[tmpIndex1] = newT1;
                }

                iter++;
            }
        }
    }

    std::vector<float> geodesics;geodesics.clear();
    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        geodesics.push_back(mesh->geodesic[i] + 0.0001f);
    }
    // Now Erase the duplicate data
    m_meshPtr->CleanupAttributes(0);

    return geodesics;
}

void meshFIM::WriteFeaFile(TriMesh *mesh, char* outfilename)
{
    int numVert = mesh->vertices.size();

    // write out distance to curve
    ofstream outfile(outfilename, std::ios::binary);

    // write numVertices to facilitate reading later
    outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        // write distance to curve
        float distToCurve;
        distToCurve = mesh->geodesic[i] + 0.0001f;
        outfile.write( reinterpret_cast<char *>(&distToCurve), sizeof(float) );
    }

    outfile.close();
}

void meshFIM::WriteFeaFile(std::vector<float> fea, char* outfilename)
{
    int numVert = fea.size();

    // write out distance to curve
    ofstream outfile(outfilename, std::ios::binary);

    // write numVertices to facilitate reading later
    outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        // write distance to curve
        float distToCurve;
        distToCurve = fea[i];
        outfile.write( reinterpret_cast<char *>(&distToCurve), sizeof(float) );
    }

    outfile.close();
}

// end SHIREEN
