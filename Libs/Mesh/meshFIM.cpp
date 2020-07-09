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

#define LARGENUM 10000000.0
#define EPS      1e-6

typedef std::pair<unsigned int, float> gDistPair;





void meshFIM::need_kdtree() {
  if (!kd) {
    kd = new KDtree(m_meshPtr->vertices);
  }
  if (maxEdgeLength == 0.0) {
    need_maxedgelength();
  }
}

void meshFIM::need_oneringfaces() {

  if (vertOneringFaces.empty()) {
    vertOneringFaces.resize(m_meshPtr->vertices.size());
    for (int i = 0; i < m_meshPtr->vertices.size(); i++) {
      vertOneringFaces[i] = GetOneRing(i);

    }

  }

}

void meshFIM::need_speed() {
  need_abs_curvatures();
  int nf = m_meshPtr->faces.size();
  for (int i = 0; i < nf; i++) {
    TriMesh::Face f = m_meshPtr->faces[i];
    float speedInv = 0;
    switch (speedType) {
      case CURVATURE:
        speedInv = (abs_curv[f[0]] + abs_curv[f[1]] + abs_curv[f[2]]) / 3.0;
        break;
      case ONE:
        speedInv = 1.0;
        break;
    }
    speedInvVector.push_back(speedInv);
  }
}

void meshFIM::need_abs_curvatures() {
  m_meshPtr->need_curvatures();
  if (abs_curv.size() == m_meshPtr->vertices.size())
    return;

  abs_curv.clear();
  //compute rms curvature and max/min
  for (int i = 0; i < m_meshPtr->vertices.size(); i++) {
    float crv1 = m_meshPtr->curv1[i];
    float crv2 = m_meshPtr->curv2[i];
    float abs_c = (fabs(crv1) + fabs(crv2)) / 2.0 + 1.0;
    abs_curv.push_back(abs_c);
  }

  std::vector<float> abs_curv_mean;
  float max_abs = -9e99;
  float min_abs = 9e99;

  for (int i = 0; i < abs_curv.size(); i++) {
    float abs_c = abs_curv[i];
    if (abs_c > max_abs) {
      max_abs = abs_c;
    }

    if (abs_c < min_abs) {
      min_abs = abs_c;
    }
  }

  for (int i = 0; i < abs_curv.size(); i++) {
    abs_curv[i] /= max_abs;
    abs_curv[i] = sqrt(abs_curv[i]);
  }
}

//compute the edge length
void meshFIM::need_edge_lengths() {
  if (m_meshPtr->faces.empty()) {
    std::cerr << "No faces to compute face edges!!!\n";
    throw(1);
  }
  int numFaces = m_meshPtr->faces.size();
  for (int i = 0; i < numFaces; i++) {
    TriMesh::Face f = m_meshPtr->faces[i];
    vec3 edge01 = ( vec3) (m_meshPtr->vertices[f[1]] - m_meshPtr->vertices[f[0]]);
    vec3 edge12 = ( vec3) (m_meshPtr->vertices[f[2]] - m_meshPtr->vertices[f[1]]);
    vec3 edge20 = ( vec3) (m_meshPtr->vertices[f[0]] - m_meshPtr->vertices[f[2]]);
    vec3 edgeLengths;
    edgeLengths[0] = sqrt(edge01[0] * edge01[0] + edge01[1] * edge01[1] + edge01[2] * edge01[2]);
    edgeLengths[1] = sqrt(edge12[0] * edge12[0] + edge12[1] * edge12[1] + edge12[2] * edge12[2]);
    edgeLengths[2] = sqrt(edge20[0] * edge20[0] + edge20[1] * edge20[1] + edge20[2] * edge20[2]);
    edgeLengthsVector.push_back(edgeLengths);
  }
}

void meshFIM::SetMesh(TriMesh *mesh) {
  m_meshPtr = mesh;
  this->geodesicMap.resize(m_meshPtr->vertices.size());
  // orient the mesh for consistent vertex ordering...
  orient(m_meshPtr);//  Manasi
  // have to recompute the normals and other attributes required for rendering
  if (!m_meshPtr->normals.empty())
    m_meshPtr->normals.clear();//  Manasi
  m_meshPtr->need_normals();//  Manasi
  if (!m_meshPtr->adjacentfaces.empty())
    m_meshPtr->adjacentfaces.clear();//  Manasi
  m_meshPtr->need_adjacentfaces();//  Manasi
  if (!m_meshPtr->across_edge.empty())
    m_meshPtr->across_edge.clear();//  Manasi
  m_meshPtr->need_across_edge();//  Manasi
  if (!m_meshPtr->tstrips.empty())
    m_meshPtr->tstrips.clear();//  Manasi
  m_meshPtr->need_tstrips();//  Manasi
  need_edge_lengths();
  m_meshPtr->need_curvatures();
  need_speed();
}

void meshFIM::InitializeLabels() {
  if (!m_meshPtr) {
    std::cerr << "Label-vector size unknown, please set the mesh first..." << std::endl;
    throw(1);
  }
  else {
    // initialize all labels to 'Far'
    int nv = m_meshPtr->vertices.size();
    if (m_Label.size() != nv) m_Label.resize(nv);
    for (int l = 0; l < nv; l++) {
      m_Label[l] = FarPoint;
    }

    // if seeed-points are present, treat them differently
    if (!m_SeedPoints.empty()) {
      for (int s = 0; s < m_SeedPoints.size(); s++) {
        m_Label[m_SeedPoints[s]] = SeedPoint;//m_Label[s] = LabelType::SeedPoint;
      }
    }
  }
}

void meshFIM::InitializeActivePoints() {
  if (!m_SeedPoints.empty()) {
    int ns = m_SeedPoints.size();
    std::vector<index> nb;
    for (int s = 0; s < ns; s++) {
      nb = m_meshPtr->neighbors[m_SeedPoints[s]];
      for (int i = 0; i < nb.size(); i++) {
        if (m_Label[nb[i]] != SeedPoint) {
          m_ActivePoints.push_back(nb[i]);
          m_Label[nb[i]] = ActivePoint;
        }
      }
    }
  }
}

float meshFIM::PointLength(point v) {
  float length = 0;
  for (int i = 0; i < 3; i++) {
    length += v[i] * v[i];
  }
  return sqrt(length);
}

// FIM: check angle for at a given vertex, for a given face
bool meshFIM::IsNonObtuse(int v, TriMesh::Face f) {
  int iV = f.indexof(v);
  
  point A = m_meshPtr->vertices[v];
  point B = m_meshPtr->vertices[f[(iV + 1) % 3]];
  point C = m_meshPtr->vertices[f[(iV + 2) % 3]];

  float a = trimesh::dist(B, C);
  float b = trimesh::dist(A, C);
  float c = trimesh::dist(A, B);

  float angA = 0.0; /* = acos( (b*b + c*c - a*a) / (2*b*c) )*/

  if ((a > 0) && (b > 0) && (c > 0))//  Manasi stack overflow
  {//  Manasi stack overflow
    angA = acos((b * b + c * c - a * a) / (2 * b * c));//  Manasi stack overflow
  }//  Manasi stack overflow
  return (angA < M_PI / 2.0f);
}

// FIM: given a vertex, find an all-acute neighborhood of faces
void meshFIM::SplitFace(std::vector<TriMesh::Face> &acFaces, int v, TriMesh::Face cf, int nfAdj/*, int currentVert*/) {
  // get all the four vertices in order
  /* v1         v4
+-------+
\     . \
\   .   \
\ .     \
+-------+
v2         v3 */

  int iV = cf.indexof(v);											// get index of v in terms of cf
  int v1 = v;
  int v2 = cf[(iV + 1) % 3];
  int v4 = cf[(iV + 2) % 3];
  iV = m_meshPtr->faces[nfAdj].indexof(v2);				// get index of v in terms of adjacent face
  int v3 = m_meshPtr->faces[nfAdj][(iV + 1) % 3];

  // create faces (v1,v3,v4) and (v1,v2,v3), check angle at v1
  TriMesh::Face f11(v1, v3, v4);
  TriMesh::Face f22(v1, v2, v3);

  std::vector<TriMesh::Face> f1f2;
  f1f2.push_back(f11);
  f1f2.push_back(f22);
  for (int i = 0; i < f1f2.size(); i++) {
    TriMesh::Face face = f1f2[i];
    if (IsNonObtuse(v, face)) {
      acFaces.push_back(face);
    }
    else {
      int nfAdj_new = m_meshPtr->across_edge[nfAdj][m_meshPtr->faces[nfAdj].indexof(v2)];
      if (nfAdj_new > -1) {
        SplitFace(acFaces, v, face, nfAdj_new/*, currentVert*/);
      }
      else {
        //printf("NO cross edge!!! Maybe a hole!!\n");
      }
    }
  }
}

std::vector<TriMesh::Face> meshFIM::GetOneRing(int v/*, int currentVert*/) {
  if (m_meshPtr->across_edge.empty())
    m_meshPtr->need_across_edge();

  // variables required
  std::vector<TriMesh::Face> oneRingFaces;
  std::vector<TriMesh::Face> t_faces;

  // get adjacent faces
  int naf = m_meshPtr->adjacentfaces[v].size();

  if (!naf) {
    std::cout << "vertex " << v << " has 0 adjacent faces..." << std::endl;
  }
  else {
    for (int af = 0; af < naf; af++) {
      TriMesh::Face cf = m_meshPtr->faces[m_meshPtr->adjacentfaces[v][af]];

      t_faces.clear();
      if (IsNonObtuse(v, cf))// check angle: if non-obtuse, return existing face
      {
        t_faces.push_back(cf);
      }
      else {
        //t_faces.push_back(cf);
        int nfae = m_meshPtr->across_edge[m_meshPtr->adjacentfaces[v][af]][cf.indexof(v)];
        if (nfae > -1) {
          SplitFace(t_faces, v, cf, nfae/*,currentVert*/);// if obtuse, split face till we get all acute angles
        }
        else {
          //printf("NO cross edge!!! Maybe a hole!!\n");
        }
      }
      for (int tf = 0; tf < t_faces.size(); tf++) {
        oneRingFaces.push_back(t_faces[tf]);
      }
    }
  }
  return oneRingFaces;
}


float meshFIM::LocalSolver(index vet, TriMesh::Face triangle, index currentVert) {
  float a, b, delta, cosA, lamda1, lamda2, TC1, TC2;
  float TAB, TA, TB, TC;
  int A, B, C;
  float squareAB;
  float LenAB, LenBC, LenAC, LenCD, LenAD;
  float EdgeTA, EdgeTB;

  float speedInv;
  switch (speedType) {
    case CURVATURE:
      speedInv = (abs_curv[triangle[0]] +
                  abs_curv[triangle[1]] +
                  abs_curv[triangle[2]]) / 3.0;
      break;
    case ONE:
    default:
      speedInv = 1.0;
      break;
  }
  float speedI;
  if (speedType == CURVATURE) {
    speedI = 100 * speedInv;
  }
  else {
    speedI = speedInv;
  }

  C = triangle.indexof(vet);
  A = (C + 1) % 3;
  B = (A + 1) % 3;

  TC1 = LARGENUM;
  TC2 = LARGENUM;

  TA = this->geodesic[triangle[A]];
  TB = this->geodesic[triangle[B]];
  TC = this->geodesic[triangle[C]];


  TAB = TB - TA;

  vec3 edge01 = ( vec3) (m_meshPtr->vertices[triangle[1]] - m_meshPtr->vertices[triangle[0]]);
  vec3 edge12 = ( vec3) (m_meshPtr->vertices[triangle[2]] - m_meshPtr->vertices[triangle[1]]);
  vec3 edge20 = ( vec3) (m_meshPtr->vertices[triangle[0]] - m_meshPtr->vertices[triangle[2]]);
  vec3 edgeLengths;
  edgeLengths[0] = sqrt(edge01[0] * edge01[0] + edge01[1] * edge01[1] + edge01[2] * edge01[2]);
  edgeLengths[1] = sqrt(edge12[0] * edge12[0] + edge12[1] * edge12[1] + edge12[2] * edge12[2]);
  edgeLengths[2] = sqrt(edge20[0] * edge20[0] + edge20[1] * edge20[1] + edge20[2] * edge20[2]);

  LenAB = edgeLengths[A];
  LenBC = edgeLengths[B];
  LenAC = edgeLengths[C];

  a = (speedI * speedI * LenAB * LenAB - TAB * TAB) * LenAB * LenAB;

  EdgeTA = TA /*oldValues1 */ + LenAC /*s_triMem[tx*TRIMEMLENGTH + 0]*/ * speedI;
  EdgeTB = TB /*oldValues2*/ + LenBC /*s_triMem[tx*TRIMEMLENGTH + 2]*/ * speedI;

  if (a > 0) {
    cosA = (LenAC * LenAC + LenAB * LenAB - LenBC * LenBC) / (2 * LenAC * LenAB);
    b = 2 * LenAB * LenAC * cosA * (TAB * TAB - speedI * speedI * LenAB * LenAB);
    delta = 4 * LenAC * LenAC * a * TAB * TAB * (1 - cosA * cosA);

    lamda1 = (-b + sqrt(delta)) / (2 * a);
    lamda2 = (-b - sqrt(delta)) / (2 * a);

    if (lamda1 >= 0 && lamda1 <= 1) {
      LenAD = lamda1 * LenAB;
      LenCD = sqrt(LenAC * LenAC + LenAD * LenAD - 2 * LenAC * LenAD * cosA);
      TC1 = lamda1 * TAB + TA + LenCD * speedI;
    }
    if (lamda2 >= 0 && lamda2 <= 1) {
      LenAD = lamda2 * LenAB;
      LenCD = sqrt(LenAC * LenAC + LenAD * LenAD - 2 * LenAC * LenAD * cosA);
      TC2 = lamda2 * TAB + TA + LenCD * speedI;
    }

    TC = MIN(TC, MIN(TC2, MIN(TC1, MIN(EdgeTA, EdgeTB))));
  }

  else {
    TC = MIN(TC, MIN(EdgeTA, EdgeTB));
  }

  return TC;
}

float meshFIM::Upwind(index currentVert,index vet)
{
    float result=LARGENUM;
    float tmp;
    std::vector<TriMesh::Face> neighborFaces = this->GetOneRing(vet/*, currentVert*/);
    // WAS THIS LINE BELOW ON Nov17 - Change Back when GetOneRing works for speed!
    //vector<TriMesh::Face> neighborFaces = m_meshPtr->vertOneringFaces[vet];

    //vector<int> neighborFaces = m_meshPtr->adjacentfaces[vet];
    int i;
    for (i = 0; i < neighborFaces.size(); i++)
    {
        tmp = LocalSolver(vet, neighborFaces[i], currentVert);
        //tmp = LocalSolver(vet, m_meshPtr->faces[neighborFaces[i]], currentVert);
        NumComputation++;

        result = MIN(result,tmp );
        //m_meshPtr->vertT[currentVert][vet] = result;
    }

    return result;
}

// FIM: initialize attributes
void meshFIM::InitializeAttributes(int currentVert, std::vector<int> seeds = std::vector<int>()) {

  for (int v = 0; v < m_meshPtr->vertices.size(); v++) {
    this->geodesic.push_back(LARGENUM);
  }
  // initialize seed points if present...
  for (int s = 0; s < seeds.size(); s++) {
    this->geodesic[seeds[s]] = 0;
  }
  // pre-compute faces, normals, and other per-vertex properties that may be needed
  m_meshPtr->need_neighbors();
  m_meshPtr->need_normals();
  m_meshPtr->need_adjacentfaces();
  m_meshPtr->need_across_edge();
  m_meshPtr->need_faces();
}

// FIM:  Remove data lingering from computation
void meshFIM::CleanupAttributes() {
  this->geodesic.clear();
}


void meshFIM::GenerateReducedData() {
  std::list<index>::iterator iter = m_ActivePoints.begin();
  float oldT1, newT1, oldT2, newT2;
  index tmpIndex1, tmpIndex2;
  std::vector<int>  nb;
  NumComputation = 0;
  double total_duration = 0;

  clock_t starttime, endtime;
  starttime = clock();

  char c;
  int i = 0;

  for (int currentVert = 0; currentVert < m_meshPtr->vertices.size(); currentVert++) {

    std::vector<int> seedPointList(1, currentVert);
    SetSeedPoint(seedPointList);

    this->InitializeAttributes(currentVert, m_SeedPoints);

    InitializeLabels();
    InitializeActivePoints();

    while (!m_ActivePoints.empty()) {
      iter = m_ActivePoints.begin();

      while (iter != m_ActivePoints.end()) {
        tmpIndex1 = *iter;
        nb = m_meshPtr->neighbors[tmpIndex1];
        oldT1 = this->geodesic[tmpIndex1];

        newT1 = Upwind(currentVert, tmpIndex1);

        if (abs(oldT1 - newT1) < _EPS)    //if converges
        {
          if (oldT1 > newT1) {
            this->geodesic[tmpIndex1] = newT1;
          }
          if (this->geodesic[tmpIndex1] < m_StopDistance) {
            for (i = 0; i < nb.size(); i++) {
              tmpIndex2 = nb[i];
              if (m_Label[tmpIndex2] == AlivePoint || m_Label[tmpIndex2] == FarPoint) {
                oldT2 = this->geodesic[tmpIndex2];

                newT2 = Upwind(currentVert, tmpIndex2);
                if (oldT2 > newT2) {
                  this->geodesic[tmpIndex2] = newT2;

                  if (m_Label[tmpIndex2] != ActivePoint) {
                    m_ActivePoints.insert(iter, tmpIndex2);
                    m_Label[tmpIndex2] = ActivePoint;
                  }
                }
              }
            }
          }
          iter = m_ActivePoints.erase(iter);
          m_Label[tmpIndex1] = AlivePoint;
        }
        else   // if not converge
        {
          if (newT1 < oldT1) {
            this->geodesic[tmpIndex1] = newT1;
          }
          iter++;
        }
      }
    }
    // Loop Through And Copy Only Values < than m_StopDistance
    int nv = m_meshPtr->vertices.size();
    for (int v = 0; v < currentVert; v++) {
      if ((this->geodesic[v] <= m_StopDistance) && (this->geodesic[v] > 0)) {
        //m_meshPtr->geoMap[currentVert][v] = m_meshPtr->geodesic[v];
        //(*(m_meshPtr->dMap))[currentVert].push_back(m_meshPtr->geodesic[v]);
        //(*(m_meshPtr->iMap))[currentVert].push_back(v);
        (this->geodesicMap[currentVert])[v] = this->geodesic[v];
      }
    }
    // Now Erase the duplicate data
    this->CleanupAttributes();

    endtime = clock();
    total_duration = ( double) (endtime - starttime) / CLOCKS_PER_SEC;
    float percent = (currentVert + 1) / ( float) m_meshPtr->vertices.size();

    double total_time = total_duration / percent;
    double time_left = total_time - total_duration;
    int hours = ( int) (time_left / (60 * 60));
    int minutes = ( int) ((time_left - hours * 60 * 60) / 60);
    int seconds = ( int) (time_left - hours * 60 * 60 - minutes * 60);
    int count = (this->geodesicMap[currentVert]).size();
  }
}

// SHIREEN - modified the loading to control the generation of geo files (till we add the geo repulsion stuff)
void meshFIM::loadGeodesicFile(TriMesh *mesh, const char *geoFileName)
{
//    cout << "Looking for file: " << geoFileName << " ... " << flush;

    std::ifstream infile(geoFileName, std::ios::binary);
    if (!infile.is_open())
    {
        if(GENERATE_GEO_FILES == 1)
        {
//            cout << "File Not Found, will generate the geo file now ..." << endl;
            int numVert = mesh->vertices.size();
            this->geodesicMap.resize(numVert);
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

        this->geodesicMap.resize(numVert);

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

                (this->geodesicMap[i])[index] = dist;
            }
        }

        infile.close();
    }

}
// end SHIREEN

//Praful
void meshFIM::computeCoordXFiles(TriMesh *mesh, const char *vertT_filename)
{
    unsigned int numVert = mesh->vertices.size();
    this->SetMesh(mesh);

    std::ofstream outfile(vertT_filename, std::ios::binary);

    std::cout << "# vertices in mesh: " << numVert << std::endl;

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

    std::ofstream outfile(vertT_filename, std::ios::binary);

    std::cout << "# vertices in mesh: " << numVert << std::endl;

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

    std::ofstream outfile(vertT_filename, std::ios::binary);

    std::cout << "# vertices in mesh: " << numVert << std::endl;

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

    std::ofstream outfile(vertT_filename, std::ios::binary);

    std::cout << "# vertices in mesh: " << numVert << std::endl;

    outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        float absCurvVal = this->abs_curv[i];
        outfile.write( reinterpret_cast<char *>(&absCurvVal), sizeof(float) );
    }

    outfile.close();
}

//end Praful


void meshFIM::computeFIM(TriMesh *mesh, const char *vertT_filename)
{
  std::cout << "Trying to load: " << vertT_filename << std::endl;
    //	FILE* vertTFile = fopen(vertT_filename, "r+");
  std::ifstream infile(vertT_filename, std::ios::binary);

    unsigned int numVert = mesh->vertices.size();
    //(mesh->dMap)->resize(numVert);
    //(mesh->iMap)->resize(numVert);

    this->geodesicMap.resize(numVert);

    this->SetMesh(mesh);

    if (!infile.is_open())
    {
        //vertTFile = fopen(vertT_filename, "w+");
//        ofstream outfile(vertT_filename, std::ios::binary);
      std::ofstream outfile(vertT_filename, std::ofstream::out);
      std::cout << "No vertT file!!!\n Writing..." << std::endl;
      std::cout << "stop distance = " << this->GetStopDistance() << std::endl;
      std::cout << "# vertices in mesh: " << numVert << std::endl;

        this->GenerateReducedData();

        // write stop distance
        float dStop = this->GetStopDistance();
//        outfile.write( reinterpret_cast<char *>(&dStop), sizeof(float) );

        // loop over each vertex
        for (int i = 0; i < numVert; i++)
        {
            // write map size for vertex
            unsigned int dLength = this->geodesicMap[i].size();
//            outfile.write( reinterpret_cast<char *>(&dLength), sizeof(unsigned int) );

            // write key and distance pair
            for (std::map<unsigned int,float>::iterator it= this->geodesicMap[i].begin(); it != this->geodesicMap[i].end(); it++)
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

                (this->geodesicMap[i])[index] = dist;
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


void meshFIM::physicalPointToXYZ(point x, VoxelIndexType *imageX, float imageOrigin[3], float imageSpacing[3]) {
  imageX[0] = static_cast<VoxelIndexType> ((x[0] - imageOrigin[0]) / imageSpacing[0]);
  imageX[1] = static_cast<VoxelIndexType> ((x[1] - imageOrigin[1]) / imageSpacing[1]);
  imageX[2] = static_cast<VoxelIndexType> ((x[2] - imageOrigin[2]) / imageSpacing[2]);
}

meshFIM::VoxelIndexType meshFIM::indexToLinearIndex(VoxelIndexType *imageX, int imageSize[3]) {
  VoxelIndexType linearIndX = imageX[0] + imageX[1] * imageSize[0] + imageX[2] * imageSize[0] * imageSize[1];
  return linearIndX;
}

meshFIM::VoxelIndexType meshFIM::physicalPointToLinearIndex(point x) {
  VoxelIndexType imageX[3];
  this->physicalPointToXYZ(x, imageX, this->imageOrigin, this->imageSpacing);

  VoxelIndexType linearIndX = this->indexToLinearIndex(imageX, this->imageSize);
  return linearIndX;
}

meshFIM::VoxelIndexType meshFIM::physicalPointToLinearIndex(point x, float imageOrigin[3], float imageSpacing[3], int imageSize[3]) {
  VoxelIndexType imageX[3];
  this->physicalPointToXYZ(x, imageX, imageOrigin, imageSpacing);

  VoxelIndexType linearIndX = this->indexToLinearIndex(imageX, imageSize);
  return linearIndX;
}


/* Prateep
 * http://www.geometrictools.com/Documentation/DistancePoint3Triangle3.pdf
 *        ^t
 *  \     |
 *   \reg2|
 *    \   |
 *     \  |
 *      \ |
 *       \|
 *        *P2
 *        |\
 *        | \
 *  reg3  |  \ reg1
 *        |   \
 *        |reg0\
 *        |     \
 *        |      \ P1
 * -------*-------*------->s
 *        |P0      \
 *  reg4  | reg5    \ reg6
 */
double meshFIM::pointTriangleDistance(point P, TriMesh::Face face, point &PP) {
  // rewrite vertices in normal form
  point B = m_meshPtr->vertices[face[0]];
  point E0 = m_meshPtr->vertices[face[1]] - B;
  point E1 = m_meshPtr->vertices[face[2]] - B;

  point D = B - P;
  float a = E0 DOT E0;
  float b = E0 DOT E1;
  float c = E1 DOT E1;
  float d = E0 DOT D;
  float e = E1 DOT D;
  float f = D  DOT D;

  float det = a * c - b * b;
  float s = b * e - c * d;
  float t = b * d - a * e;

  float distSqr = 0.0f;

  if (s + t <= det) {
    if (s < 0) {
      if (t < 0) {
        // region 4
        if (d < 0) {
          t = 0;
          if (-d >= a) {
            s = 1.0;
            distSqr = a + 2.0f * d + f;
          }
          else {
            s = -d / a;
            distSqr = d * s + f;
          }
        }
        else {
          s = 0.0f;
          if (e >= 0.0f) {
            t = 0.0f;
            distSqr = f;
          }
          else {
            if (-e >= c) {
              t = 1.0f;
              distSqr = c + 2.0f * e + f;
            }
            else {
              t = -e / c;
              distSqr = e * t + f;
            }
          }
        } // end of region 4
      }
      else {
        // region 3
        s = 0.0f;
        if (e >= 0.0f) {
          t = 0.0f;
          distSqr = f;
        }
        else {
          if (-e >= c) {
            t = 1.0f;
            distSqr = c + 2.0f * e + f;
          }
          else {
            t = -e / c;
            distSqr = e * t + f;
          }
        }
      } // end of region 3
    }
    else {
      if (t < 0.0f) {
        // region 5
        t = 0.0f;
        if (d >= 0.0f) {
          s = 0.0f;
          distSqr = f;
        }
        else {
          if (-d >= a) {
            s = 1.0f;
            distSqr = a + 2 * d + f;
          }
          else {
            s = -d / a;
            distSqr = d * s + f;
          }
        }
        // end of region 5
      }
      else {
        // region 0
        float invDet = 1.0f / det;
        s *= invDet;
        t *= invDet;
        distSqr = s * (a * s + b * t + 2 * d) + t * (b * s + c * t + 2 * e) + f;
        // end of region 0
      }
    }
  }
  else {
    if (s < 0.0f) {
      // region 2
      float tmp0 = b + d;
      float tmp1 = c + e;
      if (tmp1 > tmp0) {
        float numer = tmp1 - tmp0;
        float denom = a - 2 * b + c;
        if (numer >= denom) {
          s = 1.0f;
          t = 0.0f;
          distSqr = a + 2 * d + f;
        }
        else {
          s = numer / denom;
          t = 1.0 - s;
          distSqr = s * (a * s + b * t + 2 * d) + t * (b * s + c * t + 2 * e) + f;
        }
      }
      else {
        s = 0.0f;
        if (tmp1 <= 0.0f) {
          t = 1.0f;
          distSqr = c + 2 * e + f;
        }
        else {
          if (e >= 0.0f) {
            t = 0.0f;
            distSqr = f;
          }
          else {
            t = -e / c;
            distSqr = e * t + f;
          }
        }
      }
      // end of region 2
    }
    else {
      if (t < 0) {
        // region 6
        float tmp0 = b + e;
        float tmp1 = a + d;
        if (tmp1 > tmp0) {
          float numer = tmp1 - tmp0;
          float denom = a - 2 * b + c;
          if (numer >= denom) {
            t = 1.0f;
            s = 0.0f;
            distSqr = c + 2 * e + f;
          }
          else {
            t = numer / denom;
            s = 1.0 - t;
            distSqr = s * (a * s + b * t + 2 * d) + t * (b * s + c * t + 2 * e) + f;
          }
        }
        else {
          t = 0.0f;
          if (tmp1 <= 0.0f) {
            s = 1.0f;
            distSqr = a + 2 * d + f;
          }
          else {
            if (d >= 0.0f) {
              s = 0.0f;
              distSqr = f;
            }
            else {
              s = -d / a;
              distSqr = d * s + f;
            }
          }
        }
        // end of region 6
      }
      else {
        // region 1
        float numer = c + e - b - d;
        if (numer <= 0) {
          s = 0.0f;
          t = 1.0f;
          distSqr = c + 2 * e + f;
        }
        else {
          float denom = a - 2 * b + c;
          if (numer >= denom) {
            s = 1.0f;
            t = 0.0f;
            distSqr = a + 2 * d + f;
          }
          else {
            s = numer / denom;
            t = 1.0f - s;
            distSqr = s * (a * s + b * t + 2 * d) + t * (b * s + c * t + 2 * e) + f;
          }
        }
        // end of region 1
      }
    }
  }

  if (distSqr < 0.0f) distSqr = 0.0f;

  float dist = std::sqrt(distSqr);

  PP = B + s * E0 + t * E1;

  return dist;

}

vec3 meshFIM::ComputeBarycentricCoordinates(point p, TriMesh::Face f) {

  vec3 bCoords; bCoords.clear();
  point a, b, c;
  a = m_meshPtr->vertices[f[0]];
  b = m_meshPtr->vertices[f[1]];
  c = m_meshPtr->vertices[f[2]];

  point n = (b - a) CROSS(c - a);
  normalize(n);

  float area = ((b - a) CROSS(c - a)) DOT n;
  float inv_area = 1.0f / (area + EPS);

  // shireen
  bCoords[0] = (((c - b) CROSS(p - b)) DOT n) * inv_area; // * areaInvPerTri[f];map <face, double> didnot work
  bCoords[1] = (((a - c) CROSS(p - c)) DOT n) * inv_area; // * areaInvPerTri[f];map <face, double> didnot work
  bCoords[2] = (((b - a) CROSS(p - a)) DOT n) * inv_area; // * areaInvPerTri[f];map <face, double> didnot work

  float sum = bCoords.sum();
  bCoords[0] /= sum;
  bCoords[1] /= sum;
  bCoords[2] /= sum;

  return bCoords;
}

void meshFIM::need_maxedgelength() {
  this->need_edge_lengths();
  for (int f = 0; f < m_meshPtr->faces.size(); f++) {
    for (int d = 0; d < 3; d++) {
      if (this->edgeLengthsVector[f][d] >= maxEdgeLength) {
        maxEdgeLength = this->edgeLengthsVector[f][d];
      }
    }
  }
}

int meshFIM::FindNearestVertex(point pt) {
  //std::cerr << "FindNearestVertexA\n";
  if (!kd) {
    kd = new KDtree(m_meshPtr->vertices);
  }
  //std::cerr << "FindNearestVertexB\n";
  if (maxEdgeLength == 0.0) {
    need_maxedgelength();
  }
  //std::cerr << "FindNearestVertexC\n";
  const float *match = kd->closest_to_pt(pt, 100000.0 * maxEdgeLength * maxEdgeLength); // SHIREEN - enlargen the neighborhood size for kdtree to find a match
  int imatch = 0;
  if (!match) {
    std::cout << "failed to find vertex within " << maxEdgeLength << " for point " << pt << ". using vertex 0" << std::endl;
    return imatch;
  }
  //std::cerr << "FindNearestVertexD\n";

  imatch = (match - (const float *) & (m_meshPtr->vertices[0][0])) / 3;
  return imatch;
}

int meshFIM::GetTriangleInfoForPoint(point x, TriMesh::Face &triangleX, float &alphaX, float &betaX, float &gammaX) {
  int faceID;
  //std::cerr << "GetTriangleInfoForPoint\n";

  if (this->faceIndexMap.size() > 0) // there is a generated face index map so used it
  {
    std::cerr << "Doing face index stuff\n";
    // Physical point to Image Index
    VoxelIndexType linearIndX = this->physicalPointToLinearIndex(x);

    // collect face indices for this voxel
    std::map<VoxelIndexType, std::vector<int> >::iterator it = this->faceIndexMap.find(linearIndX);
    if (it != this->faceIndexMap.end()) // see if the linearIndX already exist in the face index map
    {
      //                std::cout << "WOW, fids will be used ... \n" ;
      std::vector<int> faceList = this->faceIndexMap[linearIndX];

      double minDist = LARGENUM;
      int winnerIndex;

      for (std::vector<int>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
        triangleX = m_meshPtr->faces[(*it)];

        // project the point onto the plane of the current triangle
        point  projPoint;
        double dist = this->pointTriangleDistance(x, triangleX, projPoint);

        if (dist < minDist) {
          minDist = dist;
          winnerIndex = (*it);
        }
      }

      triangleX = m_meshPtr->faces[winnerIndex];
      faceID = winnerIndex;
      point  projPoint;
      double dist = this->pointTriangleDistance(x, triangleX, projPoint);

      vec3 barycentric = this->ComputeBarycentricCoordinates(projPoint, triangleX);
      alphaX = barycentric[0];
      betaX = barycentric[1];
      gammaX = barycentric[2];
      return faceID;
    }
  }
#if SHOW_WARNING
    std::cerr << "warning: using kdtree for triangle info because there is no face index map !!! ...\n";
#endif
    //std::cerr << "warning: using kdtree for triangle info because there is no face index map !!! ...\n";

    // get vertex closest to first point - x
    int vertX = this->FindNearestVertex(x);
    //std::cerr << "FindNearestVertex finished\n";
    unsigned int fNumber;

    // scan all adjacent faces to see which face (f) includes point x
    triangleX = m_meshPtr->faces[m_meshPtr->adjacentfaces[vertX][0]];
    faceID = m_meshPtr->adjacentfaces[vertX][0];
    for (fNumber = 0; fNumber < m_meshPtr->adjacentfaces[vertX].size(); fNumber++) {
      // check if face contains x and store barycentric coordinates for x in face f
      triangleX = m_meshPtr->faces[m_meshPtr->adjacentfaces[vertX][fNumber]];
      faceID = m_meshPtr->adjacentfaces[vertX][fNumber];
      vec3 barycentric = this->ComputeBarycentricCoordinates(x, triangleX);
      alphaX = barycentric[0];
      betaX = barycentric[1];
      gammaX = barycentric[2];

      if (((barycentric[0] >= 0) && (barycentric[0] <= 1)) &&
        ((barycentric[1] >= 0) && (barycentric[1] <= 1)) &&
          ((barycentric[2] >= 0) && (barycentric[2] <= 1))) {
        fNumber = m_meshPtr->adjacentfaces[vertX].size();
      }
    }

    if (alphaX < 0.0 || betaX < 0.0f || gammaX < 0.0f) {
      int t = 0;
    }

  return faceID;

}

float meshFIM::GetVirtualSource(vnl_vector<float> baryCoord, vnl_matrix<float> X, vnl_vector<float> ds, vnl_vector< float > &x0) {
  //        vcl_cout<<"X:"<<std::endl<<X.extract(2,3,0,0);
  //        vcl_cout<<"ds: "<<ds<<std::endl;
  vgl_homg_point_2d<float>  centre1(X(0, 0), X(1, 0), 1);
  vgl_homg_point_2d<float>  centre2(X(0, 1), X(1, 1), 1);
  vgl_homg_point_2d<float>  centre3(X(0, 2), X(1, 2), 1);
  vgl_conic<float> circle1(centre1, ds[0], ds[0], 0.0f);
  vgl_conic<float> circle2(centre2, ds[1], ds[1], 0.0f);
  vgl_conic<float> circle3(centre3, ds[2], ds[2], 0.0f);

  //        vcl_cout<<"Circle1: "<<circle1<<std::endl;
  //        vcl_cout<<"Circle2: "<<circle2<<std::endl;
  //        vcl_cout<<"Circle3: "<<circle3<<std::endl;
  vcl_list<vgl_homg_point_2d<float> > pts1;
  pts1 = vgl_homg_operators_2d<float>::intersection(circle1, circle2);
  int n1 = ( int) (pts1.size());

  vcl_list<vgl_homg_point_2d<float> > pts2;
  pts2 = vgl_homg_operators_2d<float>::intersection(circle2, circle3);
  int n2 = ( int) (pts2.size());

  vcl_list<vgl_homg_point_2d<float> > pts3;
  pts3 = vgl_homg_operators_2d<float>::intersection(circle1, circle3);
  int n3 = ( int) (pts3.size());

  int n = n1 + n2 + n3;
  //        std::cout<<"n= "<<n<<std::endl;
  if (n == 0) {
    x0 = vnl_vector<float>(2, -1.0f);
    return -1.0f;
  }
  else {
    vnl_matrix< float > xinit(2, n, 0);
    int i = 0;
    typedef vcl_list< vgl_homg_point_2d < float > > container;
    vgl_homg_point_2d<float> temp;
    for (container::iterator p = pts1.begin(); p != pts1.end(); p++) {
      //                std::cout<<"n1 = "<<n1<<std::endl;
      temp = *p;
      if (!std::isfinite(temp.x()) || !std::isfinite(temp.y()) || !std::isfinite(temp.w())) continue;
      //                std::cout<<"x: "<<temp.x()<<" y: "<<temp.y()<<" w: "<<temp.w()<<std::endl;
      xinit(0, i) = temp.x() / temp.w();
      xinit(1, i) = temp.y() / temp.w();
      //                vcl_cout<<"i= "<<i<<" xinit(i)="<<xinit.get_column(i)<<std::endl;
      i++;
    }
    for (container::iterator p = pts2.begin(); p != pts2.end(); p++) {
      //                std::cout<<"n2 = "<<n2<<std::endl;
      temp = *p;
      if (!std::isfinite(temp.x()) || !std::isfinite(temp.y()) || !std::isfinite(temp.w())) continue;
      //                std::cout<<"x: "<<temp.x()<<" y: "<<temp.y()<<" w: "<<temp.w()<<std::endl;
      xinit(0, i) = temp.x() / temp.w();
      xinit(1, i) = temp.y() / temp.w();
      //                vcl_cout<<"i= "<<i<<" xinit(i)="<<xinit.get_column(i)<<std::endl;
      i++;
    }
    for (container::iterator p = pts3.begin(); p != pts3.end(); p++) {
      //                std::cout<<"n3 = "<<n3<<std::endl;
      temp = *p;
      if (!std::isfinite(temp.x()) || !std::isfinite(temp.y()) || !std::isfinite(temp.w())) continue;
      //                std::cout<<"x: "<<temp.x()<<" y: "<<temp.y()<<" w: "<<temp.w()<<std::endl;
      xinit(0, i) = temp.x() / temp.w();
      xinit(1, i) = temp.y() / temp.w();
      //                vcl_cout<<"i= "<<i<<" xinit(i)="<<xinit.get_column(i)<<std::endl;
      i++;
    }
    if (i == 0) {
      x0 = vnl_vector<float>(2, -1.0f);
      return -1.0f;
    }
    //            vcl_cout<<"xinit:"<<std::endl<<xinit.extract(2,n,0,0)<<std::endl;
    //            vcl_cout<<"xinit:"<<std::endl<<xinit.extract(2,i,0,0)<<std::endl;
    double minE = 10000000000.0;
    int flag = 0;
    int winner = -1;
    for (int i1 = 0; i1 < i; i1++) {
      double energy = 0.0;
      vnl_vector<float> pt = xinit.get_column(i1);
      //                                vcl_cout<<"pt= "<<pt<<std::endl;
      for (int j = 0; j < 3; j++) {
        vnl_vector<float> tmp1 = pt - X.get_column(j);
        float residual = std::abs(tmp1.squared_magnitude() - ds[j] * ds[j]); //write the dot product in place of tmp1.*tmp1
        energy += ( double) (residual * baryCoord[j]);
        //                    float residual = tmp1.squared_magnitude() - ds[j]*ds[j]; //write the dot product in place of tmp1.*tmp1
        //                    energy += (double)(residual*residual*baryCoord[j]);
      }
      //                                std::cout<<"Energy: "<<energy<<std::endl;
      if (flag == 0) {
        minE = energy;
        winner = i1;
        flag = 1;
      }
      else {
        if (energy < minE) {
          minE = energy;
          winner = i1;
        }
      }
    }
    //                        std::cout<<winner<<std::endl;
    x0 = xinit.get_column(winner);
    //            vcl_cout<<"x0: "<<x0<<std::endl;
    return 1.0f;
  }

}

namespace {
  /* Praful */
  float ComputeGradient(vnl_vector<float> x0, vnl_vector<float> baryCoord, vnl_matrix<float> X, vnl_vector<float> ds, vnl_vector<float> &G) {
    G = vnl_vector<float>(2, 0.0f);
    for (int k = 0; k < 2; k++) {
      for (int ii = 0; ii < 3; ii++) {
        vnl_vector<float> xi = X.get_column(ii);
        vnl_vector<float> tmp = x0 - xi;
        float residual = dot_product(tmp, tmp) - ds[ii] * ds[ii];
        G[k] += 4 * baryCoord[ii] * residual * tmp[k];
      }
    }
    return 1.0f;
  }

  /* Praful */
  float ComputeHessian(vnl_vector<float> x0, vnl_vector<float> baryCoord, vnl_matrix<float> X, vnl_vector<float> ds, vnl_matrix<float> &H) {
    H = vnl_matrix<float>(2, 2, 0.0f);
    for (int k = 0; k < 2; k++) {
      for (int kp = 0; kp < 2; kp++) {
        for (int ii = 0; ii < 3; ii++) {
          vnl_vector<float> xi = X.get_column(ii);
          vnl_vector<float> tmp = x0 - xi;
          float residual = dot_product(tmp, tmp) - ds[ii] * ds[ii];
          if (k == kp) {
            H(k, k) += 4 * baryCoord[ii] * (residual + 2 * tmp[k] * tmp[k]);
          }
          else {
            H(k, kp) += 8 * baryCoord[ii] * tmp[k] * tmp[kp];
          }
        }
      }
    }
    return 1.0f;
  }
}

float meshFIM::ComputeThreePointApproximatedGeodesic(vnl_vector<float> x, vnl_vector<float> baryCoord, vnl_matrix<float> X, vnl_vector<float> ds, char *method) {

  float geo_approx = -1.0f;
  vnl_vector<float> x0;
  //                std::cout<<"check4"<<std::endl;
  float n = this->GetVirtualSource(baryCoord, X, ds, x0);
  //                std::cout<<"check5"<<std::endl;
  char check2[] = "Bary";
  if (n == -1.0f || strcmp(method, check2) == 0) {
    //            std::cout<<"Using Bary..."<<std::endl;
    geo_approx = dot_product(baryCoord, ds);
  }
  else {
    char check1[] = "Newton";
    if (strcmp(method, check1) == 0) //Newton method
    {
      //                                std::cout<<"Using Newton iterations..."<<std::endl;
                      //                vcl_cout<<"Initial x0= "<<x0<<std::endl;
      float eta = 1.0f;
      for (int iter = 0; iter < 10; iter++) {
        vnl_matrix<float> H;
        vnl_vector<float> G;
        ComputeGradient(x0, baryCoord, X, ds, G);

        ComputeHessian(x0, baryCoord, X, ds, H);
        vnl_matrix<float> Hinv = vnl_matrix_inverse<float>(H);
        x0 -= eta * Hinv * G;
      }
      //                vcl_cout<<"Final x0= "<<x0<<std::endl;

    }
    else //LM method
    {
      //                std::cout<<"LM..coming soon.."<<std::endl;
      //                std::cout<<"Using LM..."<<std::endl;
      float v = 2.0f;
      float eps1 = 0.000001f;
      float eps2 = 0.000001f;
      float tau = 0.001f;
      int m = 3;
      int n = 2;
      float k = 0.0f;
      float kmax = 10.0f;

      // computing Jacobian
//                vcl_cout<<"x0: "<<std::endl<<x0<<std::endl;
//                vcl_cout<<"baryCoord: "<<std::endl<<baryCoord<<std::endl;
      vnl_matrix<float> J(m, n, 0.0f);
      for (int i = 0; i < m; i++) {
        vnl_vector<float> xi = X.get_column(i);
        //                    vcl_cout<<"xi: "<<std::endl<<xi<<std::endl;
        for (int j = 0; j < n; j++) {
          J(i, j) = 2.0f * ( float) (std::sqrt(baryCoord[i])) * (x0[j] - xi[j]);
        }
      }
      //                vcl_cout<<"J: "<<std::endl<<J.extract(m,n,0,0)<<std::endl;

                      // computing function values given the current guess
      vnl_vector<float> f(m, 0.0f);
      for (int i = 0; i < m; i++) {
        vnl_vector<float> xi = X.get_column(i);
        float di = ds[i];
        vnl_vector<float> x0_m_xi;
        x0_m_xi = x0 - xi;
        float r_i = dot_product(x0_m_xi, x0_m_xi) - di * di;
        f[i] = ( float) (std::sqrt(baryCoord[i])) * r_i;
      }
      float F;
      F = dot_product(f, f);
      F = 0.5f * F;
      vnl_matrix<float> A(n, n, 0.0f);
      A = J.transpose() * J;
      vnl_vector<float> g(n, 0.0f);
      g = J.transpose() * f;

      vnl_vector<float> diagA = A.get_diagonal();
      float max_diagA = diagA.max_value();
      float mu = tau * max_diagA;
      float norm_g = g.two_norm();

      vnl_matrix<float> muId(n, n, 0.0f);
      vnl_matrix<float> A_mu(n, n, 0.0f);
      vnl_matrix<float> A_mu_inv;
      vnl_vector<float> hlm(n, 0.0f);
      vnl_vector<float> xnew(n, 0.0f);
      vnl_vector<float> fnew(m, 0.0f);
      float Fnew = 0.0f, delta_L = 0.0f, rho = 0.0f;
      //                std::cout<<"****************"<<std::endl;
      bool found = norm_g <= eps1;
      while (!found && k < kmax) {
        k = k + 1.0f;

        muId.set_identity();
        muId = mu * muId;

        A_mu = A + muId;
        //                    std::cout<<"check4"<<std::endl;
        //                    vcl_cout<<"A: "<<std::endl<<A.extract(n,n,0,0)<<std::endl;
        //                    std::cout<<"mu: "<<mu<<std::endl;
        //                    vcl_cout<<"A_mu: "<<std::endl<<A_mu.extract(n,n,0,0)<<std::endl;
        A_mu_inv = vnl_matrix_inverse<float>(A_mu);
        //                    std::cout<<"check51"<<std::endl;
        //                    vcl_cout<<"A_mu_inv: "<<std::endl<<A_mu_inv.extract(n,n,0,0)<<std::endl;
        A_mu_inv = -1.0f * A_mu_inv;
        //                    vcl_cout<<"A_mu_inv: "<<std::endl<<A_mu_inv.extract(n,n,0,0)<<std::endl;
        hlm = A_mu_inv * g;
        float norm_hlm = hlm.two_norm();
        float norm_x0 = x0.two_norm();

        if (norm_hlm <= (eps1 * (norm_x0 + eps2))) {
          found = true;
        }
        else {
          xnew = x0 + hlm;
          for (int i = 0; i < m; i++) {
            vnl_vector<float> xi = X.get_column(i);
            float di = ds[i];
            vnl_vector<float> x_m_xi;
            x_m_xi = xnew - xi;
            float r_i = dot_product(x_m_xi, x_m_xi) - di * di;
            fnew[i] = ( float) (std::sqrt(baryCoord[i])) * r_i;
          }
          Fnew = dot_product(fnew, fnew);
          Fnew = 0.5f * Fnew;

          delta_L = 0.5f * dot_product(hlm, (mu * hlm - g));
          rho = (F - Fnew) / delta_L;

          if (rho > 0.0f) {
            x0 = xnew;

            // computing Jacobian
            for (int i = 0; i < m; i++) {
              vnl_vector<float> xi = X.get_column(i);
              for (int j = 0; j < n; j++) {
                J(i, j) = 2.0f * ( float) (std::sqrt(baryCoord[i])) * (x0[j] - xi[j]);
              }
            }

            // computing function values given the current guess
            for (int i = 0; i < m; i++) {
              vnl_vector<float> xi = X.get_column(i);
              float di = ds[i];
              vnl_vector<float> x0_m_xi;
              x0_m_xi = x0 - xi;
              float r_i = dot_product(x0_m_xi, x0_m_xi) - di * di;
              f[i] = ( float) (std::sqrt(baryCoord[i])) * r_i;
            }

            F = dot_product(f, f);
            F = 0.5f * F;
            A = J.transpose() * J;
            g = J.transpose() * f;
            norm_g = g.two_norm();
            found = norm_g <= eps1;
            //                            std::cout<<"=================="<<std::endl;
            //                            std::cout<<"mu= "<<mu<<std::endl;
            //                            std::cout<<"=================="<<std::endl;
            float cmp1 = 1.0f - (2.0f * rho - 1.0f) * (2.0f * rho - 1.0f) * (2.0f * rho - 1.0f);
            if (0.3f > cmp1) {
              mu = mu * 0.3f;
            }
            else {
              mu = mu * cmp1;
            }
            //                            std::cout<<"=================="<<std::endl;
            //                            std::cout<<"cmp1= "<<cmp1<<" mu= "<<mu<<std::endl;
            //                            std::cout<<"=================="<<std::endl;
            v = 2.0f;

          }
          else {
            mu = mu * v;
            v = 2.0f * v;
          }
        }
      }
      //                vcl_cout<<x0<<std::endl;
    }
    geo_approx = (x0 - x).two_norm();
  } //end else xinit not empty
//        std::cout<<"Returning geo_approx..."<<geo_approx<<std::endl;
  return geo_approx;

}

float meshFIM::ComputeCanonicalForm(point s, vnl_vector<float> &x, vnl_matrix<float> &X)//, Face S)
{
  TriMesh::Face S;
  float alpS, betS, gamS;
  GetTriangleInfoForPoint(s, S, alpS, betS, gamS);

  vnl_matrix<float> S_(3, 3);
  vnl_vector<float> muS(3, 0);
  for (int i = 0; i < 3; i++) {
    point vertex = m_meshPtr->vertices[S[i]];
    for (int j = 0; j < 3; j++) S_(i, j) = ( float) (vertex[j]);
  }
  //        std::cout<<"*****************"<<std::endl;
  //        vcl_cout<<"Face: "<<std::endl<<S_.extract(3,3,0,0)<<std::endl;
  //        std::cout<<"*****************"<<std::endl;
  S_ = S_.transpose();
  //        std::cout<<"*****************"<<std::endl;
  //        vcl_cout<<"Transposed: "<<std::endl<<S_.extract(3,3,0,0)<<std::endl;
  //        std::cout<<"*****************"<<std::endl;
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) muS[r] += S_(r, c);
    muS[r] /= 3.0f;
  }
  //        std::cout<<"*****************"<<std::endl;
  //        vcl_cout<<"muS: "<<std::endl<<muS<<std::endl;
  //        std::cout<<"*****************"<<std::endl;
          // Scent = S - muS
  vnl_matrix<float> Scent(3, 3);
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) Scent(r, c) = S_(r, c) - muS[r];
  }
  //        vcl_cout<<"Scent: "<<Scent.extract(3,3,0,0)<<std::endl;
  vnl_svd<float> svd(Scent);
  //        bool vld_svd = vnl_svd<float>::valid();
  //        std::cout<<"Valid SVD? "<<vld_svd<<std::endl;
  //        std::cout<<"checkpoint SVD"<<std::endl;
          //        vnl_diag_matrix<point::value_type> W_ =  svd.W();
  vnl_matrix<float> U_ = svd.U();
  //        vcl_cout<<"U_: "<<U_.extract(3,2,0,0)<<std::endl;
  //        std::cout<<"check32"<<std::endl;
          //        vnl_matrix<point::value_type> V_ = svd.V();
          /* top 2 eigen vectors */
  vnl_matrix<float> U(3, 2);
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 2; c++) U(r, c) = U_(r, c);
  }
  //        std::cout<<"............................"<<std::endl;
  //        vcl_cout<<"U: "<<U.extract(2,3,0,0)<<std::endl;
  //        std::cout<<"............................"<<std::endl;
  /*vnl_matrix<point::value_type>*/ X = U.transpose() * Scent;

  vnl_vector<float> sCent(3);
  for (int c = 0; c < 3; c++) sCent[c] = s[c] - muS[c];

  /*vnl_vector<point::value_type>*/ x = U.transpose() * sCent;
  //        std::cout<<"-----------------------------"<<std::endl;
  //        vcl_cout<<x<<std::endl;
  //        std::cout<<"-----------------------------"<<std::endl;
  return 1.0f;
  // printing for debugging
  //        std::cout<<std::endl<<std::endl<<"Canonical form computed..."<<std::endl;
  //        vcl_cerr<<x;
  //        std::cout<<std::endl;
  //        vcl_cerr<<X.extract(2,3,0,0);
}

float meshFIM::GetGeodesicDistance(int v1, int v2) {
  float gDist = 0.000001f;

  if (v1 == v2) return gDist;

  int vert = v1;
  int key = v2;

  if (v2 > v1) {
    vert = v2;
    key = v1;
  }

  std::map<unsigned int, float>::iterator geoIter = this->geodesicMap[vert].find(key);
  if (geoIter != this->geodesicMap[vert].end()) {
    gDist = geoIter->second;
  }
  else {
    gDist = LARGENUM;
  }

  return gDist;
}

float meshFIM::GetBronsteinGeodesicDistance(TriMesh::Face Sa, TriMesh::Face Sb, vnl_vector <float> baryCoord_a, vnl_vector <float> baryCoord_b, char *method) {
  point a; a.clear();
  point b; b.clear();
  for (int d1 = 0; d1 < 3; d1++) {
    a[d1] = 0.0;
    b[d1] = 0.0;
    for (int d2 = 0; d2 < 3; d2++) {
      point vt = m_meshPtr->vertices[Sa[d2]];
      a[d1] += baryCoord_a[d2] * vt[d1];
      point vt2 = m_meshPtr->vertices[Sb[d2]];
      b[d1] += baryCoord_b[d2] * vt2[d1];
    }
  }

  float alp_a, alp_b, bet_a, bet_b, gam_a, gam_b;
  alp_a = baryCoord_a[0];
  bet_a = baryCoord_a[1];
  gam_a = baryCoord_a[2];
  alp_b = baryCoord_b[0];
  bet_b = baryCoord_b[1];
  gam_b = baryCoord_b[2];

  if (alp_a < 0.000001f) {
    alp_a = 0.000001f;
  }
  if (bet_a < 0.000001f) {
    bet_a = 0.000001f;
  }
  if (gam_a < 0.000001f) {
    gam_a = 0.000001f;
  }
  if (alp_b < 0.000001f) {
    alp_b = 0.000001f;
  }
  if (bet_b < 0.000001f) {
    bet_b = 0.000001f;
  }
  if (gam_b < 0.000001f) {
    gam_b = 0.000001f;
  }

  alp_a /= (alp_a + bet_a + gam_a);
  bet_a /= (alp_a + bet_a + gam_a);
  gam_a /= (alp_a + bet_a + gam_a);

  alp_b /= (alp_b + bet_b + gam_b);
  bet_b /= (alp_b + bet_b + gam_b);
  gam_b /= (alp_b + bet_b + gam_b);

  baryCoord_a[0] = alp_a;
  baryCoord_a[1] = bet_a;
  baryCoord_a[2] = gam_a;
  baryCoord_b[0] = alp_b;
  baryCoord_b[1] = bet_b;
  baryCoord_b[2] = gam_b;
  vnl_vector<float> xA(2);
  vnl_vector<float> xB(2);
  vnl_matrix<float> Xa(2, 3);
  vnl_matrix<float> Xb(2, 3);
  if (baryCoord_a.max_value() > 1.0f || baryCoord_a.min_value() < 0.0f || baryCoord_b.max_value() > 1.0f || baryCoord_b.min_value() < 0.0f) {
    std::cerr << "incorrect barycentric coordinates...!!" << std::endl;
    std::cerr << "baryCoord_a: " << baryCoord_a << std::endl;
    std::cerr << "baryCoord_b: " << baryCoord_b << std::endl;
    return EXIT_FAILURE;
  }

  ComputeCanonicalForm(a, xA, Xa);
  ComputeCanonicalForm(b, xB, Xb);

  vnl_matrix<float> dA_2_B(3, 3);
  bool tooFar = false;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      dA_2_B(i, j) = this->GetGeodesicDistance(Sa[i], Sb[j]);

      // SHIREEN: if triangles are too far, don't bother to complete
      if (dA_2_B(i, j) == LARGENUM) {
        tooFar = true;
        break;
      }
    }
    if (tooFar)
      break;
  }

  if (tooFar)
    return LARGENUM;

  vnl_vector<float> geo_approx_2_B(3);
  for (int vertB_id = 0; vertB_id < 3; vertB_id++)
    geo_approx_2_B[vertB_id] = ComputeThreePointApproximatedGeodesic(xA, baryCoord_a, Xa, dA_2_B.get_column(vertB_id), method);

  float dGeo_a_2_b = 0.0f;
  dGeo_a_2_b = ComputeThreePointApproximatedGeodesic(xB, baryCoord_b, Xb, geo_approx_2_B, method);

  return dGeo_a_2_b;
}

// Praful - compute distance to landmarks based on geodesic approximation with given triangle info
void meshFIM::ComputeDistanceToLandmarksGivenTriangleInfo(TriMesh *mesh, const char *infilename , const char *outfilename)
{
    // initialize the geodesic map to hold the geodesics from the triangle vertices of the given landmark to all other mesh vertices
    SetStopDistance(1e7);
    int numVert = mesh->vertices.size();
    this->geodesicMap.resize(numVert);
    SetMesh(mesh);

    std::ifstream pointsFile(infilename);
    std::ofstream outfile(outfilename);

    if (!pointsFile)
    {
        std::cerr << "points file not found: " << infilename << std::endl;
    }
    int count = 0;
    while(pointsFile)
    {
        point tmpPt; tmpPt.clear();

        vnl_vector<float> baryCoords(3, 0.0);

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
        vertexIdlist.push_back(triangleX[0]);
        vertexIdlist.push_back(triangleX[1]);
        vertexIdlist.push_back(triangleX[2]);

        // update the geodesic map with geodesic distances from each triangle vertex to all other mesh vertices
        UpdateGeodesicMapWithDistancesFromVertices(vertexIdlist);
        std::cout << "Point# " << count++ << " fid: " << faceId << " baryCoords: " << baryCoords[0] << " " << baryCoords[1] << " " << baryCoords[2] << std::endl;

        for (int i = 0; i < numVert; i++)
        {
//            std::cout << "Vertex: " << i << std::endl;
            point curVertex = mesh->vertices[i];
            TriMesh::Face vertFace = mesh->faces[mesh->adjacentfaces[i][0]];
            vec3 barycentric = this->ComputeBarycentricCoordinates(curVertex, vertFace);
            vnl_vector<float> baryVert(3, 0.0);
            for (int d = 0; d < 3; d++)
                baryVert[d] = barycentric[d];
            float distToLandmark = this->GetBronsteinGeodesicDistance(triangleX, vertFace, baryCoords, baryVert, (char*) "LM");
            outfile << distToLandmark << " ";
        }
        outfile << std::endl;
    }
    std::cout << "Total number of points: " << count+1 << std::endl;
    pointsFile.close();
    outfile.close();
}

/* Praful */
float meshFIM::GetBronsteinGeodesicDistance(point a, point b, char *method)//, Face Sa, Face Sb, vnl_vector <float> baryCoord_a, vnl_vector <float> baryCoord_b)
{
  TriMesh::Face Sa, Sb;
  vnl_vector <float> baryCoord_a(3), baryCoord_b(3);
  float alp_a, alp_b, bet_a, bet_b, gam_a, gam_b;
  GetTriangleInfoForPoint(a, Sa, alp_a, bet_a, gam_a);
  GetTriangleInfoForPoint(b, Sb, alp_b, bet_b, gam_b);

  float dGeo_a_2_b = GetBronsteinGeodesicDistance(Sa, Sb, baryCoord_a, baryCoord_b, method);
  return dGeo_a_2_b;
}

// SHIREEN - compute distance to landmarks based on geodesic approximation
void meshFIM::ComputeDistanceToLandmark(TriMesh *mesh, point landmark, bool apply_log, const char *outfilename)
{
    // initialize the geodesic map to hold the geodesics from the triangle vertices of the given landmark to all other mesh vertices
    int numVert = mesh->vertices.size();
    this->geodesicMap.resize(numVert);
    SetMesh(mesh);

    // get which triangle the given landmark should belong to
    TriMesh::Face triangleX;
    float alphaX, betaX, gammaX;

    int faceId = this->GetTriangleInfoForPoint(landmark, triangleX, alphaX, betaX, gammaX);

    std::vector<int> vertexIdlist;
    vertexIdlist.clear();
    vertexIdlist.push_back(triangleX[0]);
    vertexIdlist.push_back(triangleX[1]);
    vertexIdlist.push_back(triangleX[2]);

    // update the geodesic map with geodesic distances from each triangle vertex to all other mesh vertices
    UpdateGeodesicMapWithDistancesFromVertices(vertexIdlist);

    // now start approximating the geodesics to the given landmarks based on the geodesics to its triangle vertices
    // write out distance to curve
    std::ofstream outfile(outfilename, std::ios::binary);

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
        float distToLandmark = this->GetBronsteinGeodesicDistance(landmark, curVertex, (char*) "LM");
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
    std::vector<int>  nb;
    int i = 0;

    for (int ii = 0 ; ii < vertexIdlist.size(); ii++)
    {
        int currentVert = vertexIdlist[ii];

        std::vector<int> seedPointList(1, currentVert);
        SetSeedPoint(seedPointList);

        this->InitializeAttributes(currentVert, m_SeedPoints);

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
                oldT1 = this->geodesic[tmpIndex1];

                newT1 = Upwind(currentVert,tmpIndex1);

                if (abs(oldT1-newT1)<_EPS)    //if converges
                {
                    if (oldT1>newT1){
                      this->geodesic[tmpIndex1] = newT1;
                    }

                    if(this->geodesic[tmpIndex1] < m_StopDistance)
                    {
                        for (i=0;i<nb.size();i++)
                        {
                            tmpIndex2 = nb[i];
                            if (m_Label[tmpIndex2]==AlivePoint || m_Label[tmpIndex2]==FarPoint)
                            {
                                oldT2 = this->geodesic[tmpIndex2];

                                newT2 = Upwind(currentVert,tmpIndex2);
                                if (oldT2>newT2)
                                {
                                  this->geodesic[tmpIndex2] = newT2;

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
                      this->geodesic[tmpIndex1] = newT1;
                    }

                    iter++;
                }
            }
        }

        // Loop Through And Copy Only Values < than
        int nv = m_meshPtr->vertices.size();
        for(int v = 0; v < nv; v++){
            if ((this->geodesic[v] <= m_StopDistance) && (this->geodesic[v] > 0))
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
                (this->geodesicMap[vert])[key] = this->geodesic[v];
            }
        }

        // Now Erase the duplicate data
        this->CleanupAttributes();
    }

}

// end SHIREEN

// SHIREEN - compute distance to curve based on geodesic approximation
void meshFIM::ComputeDistanceToCurve(TriMesh *mesh, std::vector< point > curvePoints, const char *outfilename)
{
    // initialize the geodesic map to hold the geodesics from the triangle vertices of the given landmark to all other mesh vertices
    int numVert = mesh->vertices.size();
    this->geodesicMap.resize(numVert);
    SetMesh(mesh);

    // get which triangle the given landmark should belong to
    TriMesh::Face triangleX;
    float alphaX, betaX, gammaX;

    std::vector<int> vertexIdlist;
    vertexIdlist.clear();
    for (int pIndex = 0; pIndex < curvePoints.size(); pIndex++)
    {
        int faceId = this->GetTriangleInfoForPoint(curvePoints[pIndex], triangleX, alphaX, betaX, gammaX);
        
        vertexIdlist.push_back(triangleX[0]);
        vertexIdlist.push_back(triangleX[1]);
        vertexIdlist.push_back(triangleX[2]);
    }
    
    // update the geodesic map with geodesic distances from each triangle vertex of each curve point to all other mesh vertices
    UpdateGeodesicMapWithDistancesFromVertices(vertexIdlist);

    // now start approximating the geodesics to the given curve points based on the geodesics to its triangle vertices
    // write out distance to curve
    std::ofstream outfile(outfilename, std::ios::binary);

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
            float curDist  = this->GetBronsteinGeodesicDistance(landmark, curVertex, (char*) "LM");
            
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


int meshFIM::GetVertexInfoForPoint(point x) {
  int vertX;
  TriMesh::Face triangleX;
  float alphaX, betaX, gammaX;

  if (this->faceIndexMap.size() > 0) // there is a generated face index map so used it
  {
    //std::cout << "WOW, fids will be used ... \n" ;
    // Physical point to Image Index
    VoxelIndexType linearIndX = this->physicalPointToLinearIndex(x);

    // collect face indices for this voxel
    std::map<VoxelIndexType, std::vector<int> >::iterator it = this->faceIndexMap.find(linearIndX);
    if (it != this->faceIndexMap.end()) {
      std::vector<int> faceList = this->faceIndexMap[linearIndX];

      double minDist = LARGENUM;
      int winnerIndex;

      for (std::vector<int>::iterator it = faceList.begin(); it != faceList.end(); ++it) {
        triangleX = m_meshPtr->faces[(*it)];

        // project the point onto the plane of the current triangle
        point  projPoint;
        double dist = this->pointTriangleDistance(x, triangleX, projPoint);

        if (dist < minDist) {
          minDist = dist;
          winnerIndex = (*it);
        }
      }

      triangleX = m_meshPtr->faces[winnerIndex];

      point  projPoint;
      double dist = this->pointTriangleDistance(x, triangleX, projPoint);

      vec3 barycentric = this->ComputeBarycentricCoordinates(projPoint, triangleX);
      alphaX = barycentric[0];
      betaX = barycentric[1];
      gammaX = barycentric[2];

      // get vertex closest to first point - x
      vertX = this->FindNearestVertex(projPoint);

    }
    else //kdtree based
    {
#if SHOW_WARNING
      std::cout << "warning: using kdtree for triangle info because voxel index " << linearIndX << " is not found in the face index map !!! ...\n";
#endif

      // get vertex closest to first point - x
      vertX = this->FindNearestVertex(x);

      // scan all adjacent faces to see which face (f) includes point x
      triangleX = m_meshPtr->faces[m_meshPtr->adjacentfaces[vertX][0]];
      for (unsigned int fNumber = 0; fNumber < m_meshPtr->adjacentfaces[vertX].size(); fNumber++) {
        // check if face contains x and store barycentric coordinates for x in face f
        triangleX = m_meshPtr->faces[m_meshPtr->adjacentfaces[vertX][fNumber]];
        vec3 barycentric = this->ComputeBarycentricCoordinates(x, triangleX);
        alphaX = barycentric[0];
        betaX = barycentric[1];
        gammaX = barycentric[2];

        if (((barycentric[0] >= 0) && (barycentric[0] <= 1)) &&
          ((barycentric[1] >= 0) && (barycentric[1] <= 1)) &&
            ((barycentric[2] >= 0) && (barycentric[2] <= 1))) {
          fNumber = m_meshPtr->adjacentfaces[vertX].size();
        }
      }
    }
  }
  else {
#if SHOW_WARNING
    std::cout << "warning: using kdtree for triangle info because there is no face index map !!! ...\n";
#endif

    // get vertex closest to first point - x
    vertX = this->FindNearestVertex(x);

    // scan all adjacent faces to see which face (f) includes point x
    triangleX = m_meshPtr->faces[m_meshPtr->adjacentfaces[vertX][0]];
    for (unsigned int fNumber = 0; fNumber < m_meshPtr->adjacentfaces[vertX].size(); fNumber++) {
      // check if face contains x and store barycentric coordinates for x in face f
      triangleX = m_meshPtr->faces[m_meshPtr->adjacentfaces[vertX][fNumber]];
      vec3 barycentric = this->ComputeBarycentricCoordinates(x, triangleX);
      alphaX = barycentric[0];
      betaX = barycentric[1];
      gammaX = barycentric[2];

      if (((barycentric[0] >= 0) && (barycentric[0] <= 1)) &&
        ((barycentric[1] >= 0) && (barycentric[1] <= 1)) &&
          ((barycentric[2] >= 0) && (barycentric[2] <= 1))) {
        fNumber = m_meshPtr->adjacentfaces[vertX].size();
      }
    }
  }

  return vertX;
}

// SHIREEN - computing geo distance on the fly for fuzzy geodesics
std::vector<float> meshFIM::ComputeDistanceToCurve(TriMesh *mesh, std::vector< point > curvePoints)
{
    int numVert = mesh->vertices.size();
    this->geodesicMap.resize(numVert);
    SetMesh(mesh);

    std::list<index>::iterator iter = m_ActivePoints.begin();
    float oldT1 , newT1, oldT2, newT2;
    index tmpIndex1, tmpIndex2;
    std::vector<int>  nb;
    NumComputation = 0;

    double total_duration = 0;
    char c;
    int i=0;

    std::vector<int> seedPointList;
    for (int pIndex = 0; pIndex < curvePoints.size(); pIndex++)
    {
        // SHIREEN
        seedPointList.push_back(this->GetVertexInfoForPoint(curvePoints[pIndex]) );
        //seedPointList.push_back( mesh->FindNearestVertex(curvePoints[pIndex]) );
        // end SHIREEN
    }
    SetSeedPoint(seedPointList);

    this->InitializeAttributes(0, m_SeedPoints);

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
            oldT1 = this->geodesic[tmpIndex1];

            newT1 = Upwind(0,tmpIndex1);

            if (abs(oldT1-newT1)<_EPS)    //if converges
            {
                if (oldT1>newT1)
                {
                  this->geodesic[tmpIndex1] = newT1;
                }

                if(this->geodesic[tmpIndex1] < m_StopDistance)
                {
                    for (i=0;i<nb.size();i++)
                    {
                        tmpIndex2 = nb[i];
                        if (m_Label[tmpIndex2]==AlivePoint || m_Label[tmpIndex2]==FarPoint)
                        {
                            oldT2 = this->geodesic[tmpIndex2];

                            newT2 = Upwind(0,tmpIndex2);
                            if (oldT2>newT2)
                            {
                              this->geodesic[tmpIndex2] = newT2;

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
                  this->geodesic[tmpIndex1] = newT1;
                }

                iter++;
            }
        }
    }

    std::vector<float> geodesics;geodesics.clear();
    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        geodesics.push_back(this->geodesic[i] + 0.0001f);
    }
    // Now Erase the duplicate data
    this->CleanupAttributes();

    return geodesics;
}

void meshFIM::WriteFeaFile(TriMesh *mesh, char* outfilename)
{
    int numVert = mesh->vertices.size();

    // write out distance to curve
    std::ofstream outfile(outfilename, std::ios::binary);

    // write numVertices to facilitate reading later
    outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

    // loop over each vertex
    for (int i = 0; i < numVert; i++)
    {
        // write distance to curve
        float distToCurve;
        distToCurve = this->geodesic[i] + 0.0001f;
        outfile.write( reinterpret_cast<char *>(&distToCurve), sizeof(float) );
    }

    outfile.close();
}

void meshFIM::WriteFeaFile(std::vector<float> fea, char* outfilename)
{
    int numVert = fea.size();

    // write out distance to curve
    std::ofstream outfile(outfilename, std::ios::binary);

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

/* Praful */
void meshFIM::GetFeatureValues(point x, std::vector<float> &vals) {
  float alphaX, betaX, gammaX;
  TriMesh::Face triangleX;
  GetTriangleInfoForPoint(x, triangleX, alphaX, betaX, gammaX);
  if (alphaX < 0.000001f)
    alphaX = 0.000001f;

  if (betaX < 0.000001f)
    betaX = 0.000001f;

  if (gammaX < 0.000001f)
    gammaX = 0.000001f;

  alphaX /= (alphaX + betaX + gammaX);
  betaX /= (alphaX + betaX + gammaX);
  gammaX /= (alphaX + betaX + gammaX);

  vals.resize(this->features.size());
  for (unsigned int i = 0; i < this->features.size(); i++) {
    float f0 = this->features[i][triangleX[0]];
    float f1 = this->features[i][triangleX[1]];
    float f2 = this->features[i][triangleX[2]];

    vals[i] = (alphaX * f0) + (betaX * f1) + (gammaX * f2);
  }
}

