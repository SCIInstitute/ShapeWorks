#ifndef TRIMESH_H
#define TRIMESH_H
/*
Szymon Rusinkiewicz
Princeton University

TriMesh.h
Class for triangle meshes.
*/

#define  LARGENUM  10000000.0
#define  ONE       1 
#define  CURVATURE 2 
#define  NOISE     3
#define  EPS       1e-6
//#define SPEEDTYPE ONE

#include "Vec.h"
#include "Color.h"
#include "KDtree.h"
#include "math.h"
#include <vector>
#include <list>
#include <map>
#include <limits>
#include <iostream>
#include <fstream>

// SHIREEN
#include <iterator> // -- PM
#include <vnl/vnl_math.h>
#include <vnl/vnl_sparse_matrix.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_sparse_lu.h>

//#define SHOW_WARNING 1

#define NUM_THREADS 8
//#define MP_USE_OPENMP //-- should be in cmake

#ifdef MP_USE_OPENMP
#include <omp.h> // -- PM
#endif

// end SHIREEN

// Praful
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/vgl_conic.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <string>
#include <fstream>
#include <cstdlib>
#include <vcl_list.h>

// Praful end


// itk files to generate Face Index -- PM
#include "itkImage.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageFileWriter.h"
#include "itkTimeProbe.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
// sets for the face index set
#include <set>

// Danny Perry's functor
#include "map.h"

///* Prateep */
//// include alglib headers
//#include "alglib/ap.h"
//#include "alglibinternal.h"
//#include "alglibmisc.h"
//#include "solvers.h"
//#include "optimization.h"
//#include "interpolation.h"

typedef  float  PixelType;

//using std::set; // -- PM

// end SHIREEN

using std::vector;
using std::map;

// SHIREEN
#include <algorithm>

#define  PI 3.141592653589793

#ifndef MIN
#define MIN(a,b) ((a)<(b))?(a):(b)
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b))?(a):(b)
#endif

static int ITER_BLOCK = 0;

/*Prateep */
template<class TIn, class TOut, class Mesh>
struct MapFunctor
{
    typedef typename TIn::ConstPointer TInP;
    typedef typename TOut::Pointer TOutP;
    typedef typename TOut::RegionType OutRegion;
    typedef typename TIn::PixelType TPix;

    // Store size, origin and spacing of super-voxel (Filled in getFaceIndexMap)
    float supVoxelOrigin[3];
    float supVoxelSpacing[3];
    int supVoxelSize[3];

    // super voxel face list
    map<int, vector<int> > superVoxelFaceList;

    Mesh mesh;

    TOutP out_;

    MapFunctor(TOutP out) : out_(out) {}

    void operator()(const TInP &in, const OutRegion & threadRegion)
    {
        ITER_BLOCK = ITER_BLOCK + 1;

        std::cout << "Iteration : " << ITER_BLOCK << std::endl;

        typedef itk::ImageRegionConstIteratorWithIndex<TIn> It;
        It itI(in, threadRegion);

        for(itI.GoToBegin(); !itI.IsAtEnd(); ++itI) {

            if(itI.Get() == 1)
            {
                point tmPoint;
                typename TIn::PointType itkPoint;
                in->TransformIndexToPhysicalPoint(itI.GetIndex(), itkPoint);
                for(int i = 0; i < 3; i++) { tmPoint[i] = itkPoint[i]; }

                // Get neartest k vertices
                vector<int> adjFaces; adjFaces.clear();
                vector<int>::iterator adjFacesIt;
                // find triangles enclosed inside each supervoxel
                int tmpInd = mesh.physicalPointToLinearIndex(tmPoint, supVoxelOrigin, supVoxelSpacing, supVoxelSize);

                for(vector<int>::iterator it = superVoxelFaceList[tmpInd].begin(); it != superVoxelFaceList[tmpInd].end(); it++) {
                    adjFaces.push_back((*it));
                }

                //                std::cout << "Number of neighbors : " << adjFaces.size() << std::endl;
                if(adjFaces.empty() )
                {
                    // We can either abort here or ignore the voxel
                    //                    typename TIn::IndexType ind = itI.GetIndex();
                    //                    std::cout << "-1 : " << ind[0] << ' ' << ind[1] << ' ' << ind[2] << std::endl;
                    out_->SetPixel(itI.GetIndex(), -1);
                } else {

                    //std::cout << "Adjacent faces : " << this->adjacentfaces[imatch].size() << std::endl;
                    double minDist = LARGENUM;
                    int fid = -1;
                    for(adjFacesIt = adjFaces.begin(); adjFacesIt != adjFaces.end(); adjFacesIt++) {
                        point projPoint;
                        double dist = mesh.pointTriangleDistance(tmPoint, mesh.faces[*(adjFacesIt)], projPoint);
                        if(dist + EPS <= minDist) {
                            minDist = dist;
                            fid = *(adjFacesIt);
                        }
                    }
                    out_->SetPixel(itI.GetIndex(), fid);
                    adjFaces.clear();
                }

            } else {
                //                typename TIn::IndexType ind = itI.GetIndex();
                //                std::cout << "-1 : " << ind[0] << ' ' << ind[1] << ' ' << ind[2] << std::endl;
                out_->SetPixel(itI.GetIndex(), -1);
            }

        }
    }
};

/* Prateep */
template<class TIn, class TOut, class Mesh>
struct MapFunctorKDtree
{
    typedef typename TIn::ConstPointer TInP;
    typedef typename TOut::Pointer TOutP;
    typedef typename TOut::RegionType OutRegion;
    typedef typename TIn::PixelType TPix;

    Mesh mesh;
    KDtree *kd;
    TOutP out_;

    MapFunctorKDtree(TOutP out) : out_(out) {}

    void setKD() {
        kd = new KDtree(mesh.vertices);
    }

    void operator()(const TInP &in, const OutRegion & threadRegion)
    {
        ITER_BLOCK = ITER_BLOCK + 1;

        std::cout << "Iteratio : " << ITER_BLOCK << std::endl;

        typedef itk::ImageRegionConstIteratorWithIndex<TIn> It;
        It itI(in, threadRegion);

        for(itI.GoToBegin(); !itI.IsAtEnd(); ++itI)
        {
            if(itI.Get() == 1)
            {
                point tmPoint;
                itk::Image<PixelType, 3>::PointType itkPoint;
                in->TransformIndexToPhysicalPoint(itI.GetIndex(), itkPoint);
                for(int i = 0; i < 3; i++) { tmPoint[i] = itkPoint[i]; }

                // Get neartest vertex
                const float *match = kd->closest_to_pt( tmPoint, 10.0 * sqr( mesh.getMaximumEdgeLength() ) );
                if(!match)
                {
                    out_->SetPixel(itI.GetIndex(), -1);
                } else {
                    int imatch = (match - (const float*) &(mesh.vertices[0][0])) / 3;
                    //std::cout << "Adjacent faces : " << mesh.adjacentfaces[imatch].size() << std::endl;
                    vector<int> adjFaces; adjFaces.clear();
                    vector<int>::iterator adjFacesIt;
                    // Check one-ring to get list of adjacent faces
                    for(size_t f = 0; f < mesh.adjacentfaces[imatch].size(); f++)
                    {
                        adjFaces.push_back(mesh.adjacentfaces[imatch][f]);
                    }

                    int fid = 0;
                    double minDist = LARGENUM;
                    for(adjFacesIt = adjFaces.begin(); adjFacesIt != adjFaces.end(); adjFacesIt++) {
                        point projPoint;
                        double dist = mesh.pointTriangleDistance(tmPoint, mesh.faces[*(adjFacesIt)], projPoint);
                        if(dist + EPS <= minDist) {
                            minDist = dist;
                            fid = *(adjFacesIt);
                        }
                    }
                    out_->SetPixel(itI.GetIndex(), fid);
                    adjFaces.clear();
                }
            } else {
                out_->SetPixel(itI.GetIndex(), -1);
            }

        }
    }
};


class TriMesh {
protected:
    static bool read_helper(const char *filename, TriMesh *mesh);

public:
    // Types
    struct Face {
        int v[3];
        float speedInv;
        float T[3];
        vec3 edgeLens;  // edge length for 01, 12, 20

        Face() {}
        Face(const int &v0, const int &v1, const int &v2)
        {
            v[0] = v0; v[1] = v1; v[2] = v2;
        }
        Face(const int *v_)
        {
            v[0] = v_[0]; v[1] = v_[1]; v[2] = v_[2];
        }
        int &operator[] (int i) { return v[i]; }
        const int &operator[] (int i) const { return v[i]; }
        operator const int * () const { return &(v[0]); }
        operator const int * () { return &(v[0]); }
        operator int * () { return &(v[0]); }
        int indexof(int v_) const
        {
            return (v[0] == v_) ? 0 :
                                  (v[1] == v_) ? 1 :
                                                 (v[2] == v_) ? 2 : -1;
        }
    };

    class BBox {
    public:
        point min, max;
        bool valid;

        // Construct as empty
        BBox() : min(point(std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::max())),
            max(point(std::numeric_limits<float>::min(),
                      std::numeric_limits<float>::min(),
                      std::numeric_limits<float>::min())),
            valid(false)
        {}

        // Initialize to one point or two points
        BBox(const point &p) : min(p), max(p), valid(true)
        {}
        BBox(const point &min_, const point &max_) :
            min(min_), max(max_), valid(true)
        {}

        // Mark invalid
        void clear()
        {
            min = point(std::numeric_limits<float>::max(),
                        std::numeric_limits<float>::max(),
                        std::numeric_limits<float>::max());
            max = point(std::numeric_limits<float>::min(),
                        std::numeric_limits<float>::min(),
                        std::numeric_limits<float>::min());
            valid = false;
        }

        // Return center point and (vector) diagonal
        point center() const { return 0.5f * (min+max); }
        vec size() const { return max - min; }

        // Grow a bounding box to encompass a point
        BBox &operator += (const point &p)
        { min.min(p); max.max(p); return *this; }
        BBox &operator += (const BBox &b)
        { min.min(b.min); max.max(b.max); return *this; }

        // The following appear to be necessary for Visual Studio,
        // despite the fact that the operators shouldn't need
        // to be friends...
        friend const TriMesh::BBox operator + (const TriMesh::BBox &b, const point &p);
        friend const TriMesh::BBox operator + (const point &p, const TriMesh::BBox &b);
        friend const TriMesh::BBox operator + (const TriMesh::BBox &b1, const TriMesh::BBox &b2);
    };
    /*
        struct BBox {
        point min, max;
        point center() const { return 0.5f * (min+max); }
        vec size() const { return max - min; }
        bool valid;
        BBox() : valid(false)
            {}
    };
    */

    struct BSphere {
        point center;
        float r;
        bool valid;
        BSphere() : valid(false)
        {}
    };

    // Enums
    enum tstrip_rep { TSTRIP_LENGTH, TSTRIP_TERM };
    enum { GRID_INVALID = -1 };
    //enum speed_type { ONE = 0, CURVATURE, NOISE };

    // The basics: vertices and faces
    vector< point > vertices;
    vector<Face> faces;

    int speedType;

    // SHIREEN

    // Face Index Map -- PM
    typedef int VoxelIndexType;
    map<VoxelIndexType, vector<int> > faceIndexMap;
    // map< face, ...> didnot work
    //map<Face, double > areaInvPerTri;
    //map<Face, double > areaPerTri; // shireen
    //    map<Face,int> faceids;
    //    std::vector <double> areaPerTri;
    //    std::vector <double> areaInvPerTri;

    // Store the size and index of the image domain (Filled in getFaceIndexMap)
    float imageSpacing[3];
    float imageOrigin[3];
    int   imageSize[3];
    int   imageIndex[3];
    int   number_of_voxels;
    int   number_of_subvoxels;

    // end SHIREEN


    // Triangle strips
    vector<int> tstrips;

    // Grid, if present
    vector<int> grid;
    int grid_width, grid_height;

    // Other per-vertex properties
    vector<Color> colors;
    vector<float> confidences;
    vector<unsigned> flags;
    unsigned flag_curr;

    // Computed per-vertex properties
    vector<vec> normals;
    vector<vec> pdir1, pdir2;
    vector<float> curv1, curv2;
    vector<float> abs_curv;
    vector< Vec<4,float> > dcurv;
    vector<vec> cornerareas;
    vector<float> pointareas;

    KDtree *kd;
    double maxEdgeLength;
    vector< map<unsigned int, float> > geodesicMap;
    float *geodesic;

    vector< vector<float> > features;
    vector < vector<point> > featureGradients; //Praful - load more accurate gradient on vertices using volume for use in shapeworks

    // Bounding structures
    BBox bbox;
    BSphere bsphere;

    // Connectivity structures:
    //  For each vertex, all neighboring vertices
    vector< vector<int> > neighbors;
    //  For each vertex, all neighboring faces
    vector< vector<int> > adjacentfaces;

    vector<double> radiusInscribe;

    vector<int>  getTwoNeighbors(int v){
        vector<int> twoNeighbors;

        // for each neighbor
        for(int i=0; i < this->neighbors[v].size(); i++){
            // add self
            int n = this->neighbors[v][i];
            twoNeighbors.push_back(n);

            // add neighbors
            for(int j=0; j < this->neighbors[n].size(); j++)
                twoNeighbors.push_back( this->neighbors[n][j] );
        }


        return twoNeighbors;
    }


    vector< vector<Face> > vertOneringFaces;
    //  For each face, the three faces attached to its edges
    //  (for example, across_edge[3][2] is the number of the face
    //   that's touching the edge opposite vertex 2 of face 3)
    vector<Face> across_edge;

    vector<float> noiseOnVert;

    int getSpeedType(){
        return speedType;
    }
    //int SPEEDTYPE;
    // Compute all this stuff...
    void setSpeedType(int st)
    {
        //ST = st;
        speedType = st;
        if(st == ONE){
            //iMap = &geoIndex;
            //dMap = &geoMap;
        }
        else if(st == CURVATURE){
            //iMap = &adaptIndex;
            //dMap = &adaptMap;
        }
        else{
            std::cout  << "Impossible SpeedType set" << std::endl;
            throw(1); //exit(1337);
        }
    }
    void need_tstrips();
    void convert_strips(tstrip_rep rep);
    void unpack_tstrips();
    void triangulate_grid();
    void need_faces()
    {
        if (!faces.empty())
            return;
        if (!tstrips.empty())
            unpack_tstrips();
        else if (!grid.empty())
            triangulate_grid();
    }

    void need_faceedges();
    void need_speed();
    void need_noise(int nNoiseIter);
    void need_oneringfaces();
    void need_kdtree();
    void need_maxedgelength();
    void need_normals();
    void need_pointareas();
    void need_curvatures();
    void need_abs_curvatures();
    void need_dcurv();
    void need_bbox();
    void need_bsphere();
    void need_neighbors();
    void need_adjacentfaces();
    void need_across_edge();
    void need_meshinfo();
    void need_Rinscribe();

    // Input and output
    static TriMesh *read(const char *filename);
    bool write(const char *filename);

    // Statistics
    // XXX - Add stuff here
    float feature_size();

    // Useful queries
    // XXX - Add stuff here
    bool is_bdy(int v)
    {
        if (neighbors.empty()) need_neighbors();
        if (adjacentfaces.empty()) need_adjacentfaces();
        return neighbors[v].size() != adjacentfaces[v].size();
    }


    vec trinorm(int f)
    {
        if (faces.empty()) need_faces();
        return ::trinorm(vertices[faces[f][0]], vertices[faces[f][1]],
                vertices[faces[f][2]]);
    }

    // FIM: check angle for at a given vertex, for a given face
    bool IsNonObtuse(int v, Face f)
    {
        int iV = f.indexof(v);

        point A = this->vertices[v];
        point B = this->vertices[f[(iV+1)%3]];
        point C = this->vertices[f[(iV+2)%3]];

        float a = dist(B,C);
        float b = dist(A,C);
        float c = dist(A,B);

        float angA = 0.0; /* = acos( (b*b + c*c - a*a) / (2*b*c) )*/

        if ((a > 0) && (b > 0) && (c > 0))//  Manasi stack overflow
        {//  Manasi stack overflow
            angA = acos( (b*b + c*c - a*a) / (2*b*c) );//  Manasi stack overflow
        }//  Manasi stack overflow

        //return ( angA - PI/2.0f < -0.00001 );
        return ( angA < M_PI/2.0f );
    }

    // FIM: given a vertex, find an all-acute neighborhood of faces
    void SplitFace(vector<Face> &acFaces, int v, Face cf, int nfAdj/*, int currentVert*/)
    {
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
        int v2 = cf[(iV+1)%3];
        int v4 = cf[(iV+2)%3];
        iV = this->faces[nfAdj].indexof(v2);				// get index of v in terms of adjacent face
        int v3 = this->faces[nfAdj][(iV+1)%3];

        // create faces (v1,v3,v4) and (v1,v2,v3), check angle at v1
        Face f1(v1,v3,v4);
        //f1.T[f1.indexof(v1)] = this->vertT[currentVert][v1];
        //f1.T[f1.indexof(v3)] = this->vertT[currentVert][v3];
        //f1.T[f1.indexof(v4)] = this->vertT[currentVert][v4];


        Face f2(v1,v2,v3);
        //f2.T[f2.indexof(v1)] = this->vertT[currentVert][v1];
        //f2.T[f2.indexof(v2)] = this->vertT[currentVert][v2];
        //f2.T[f2.indexof(v3)] = this->vertT[currentVert][v3];


        if (IsNonObtuse(v,f1))
        {
            //switch (SPEEDTYPE)
            switch(speedType)
            {
            case CURVATURE:
                /*
            f1.speedInv = ( abs(curv1[f1[0]] + curv2[f1[0]]) +
                                            abs(curv1[f1[1]] + curv2[f1[1]]) +
                                            abs(curv1[f1[2]] + curv2[f1[2]]) ) / 6;
            */
                f1.speedInv = ( abs_curv[f1[0]] +
                        abs_curv[f1[1]] +
                        abs_curv[f1[2]] ) / 3.0;

                break;
            case ONE:
                f1.speedInv = 1.0;
                break;
            case NOISE:
                f1.speedInv = (noiseOnVert[f1[0]] +
                        noiseOnVert[f1[1]] +
                        noiseOnVert[f1[2]]) / 3;
            default:
                f1.speedInv = 1.0;
                break;
            }

            vec3 edge01 = (vec3)(vertices[f1[1]] - vertices[f1[0]]);
            vec3 edge12 = (vec3)(vertices[f1[2]] - vertices[f1[1]]);
            vec3 edge20 = (vec3)(vertices[f1[0]] - vertices[f1[2]]);
            f1.edgeLens[0] =sqrt(edge01[0]*edge01[0] + edge01[1]*edge01[1] + edge01[2]*edge01[2]);
            f1.edgeLens[1] =sqrt(edge12[0]*edge12[0] + edge12[1]*edge12[1] + edge12[2]*edge12[2]);
            f1.edgeLens[2] =sqrt(edge20[0]*edge20[0] + edge20[1]*edge20[1] + edge20[2]*edge20[2]);
            acFaces.push_back(f1);
        }
        else
        {
            int nfAdj_new = this->across_edge[nfAdj][this->faces[nfAdj].indexof(v2)];
            if (nfAdj_new > -1)
            {
                SplitFace(acFaces,v,f1,nfAdj_new/*, currentVert*/);
            }
            else
            {
                //printf("NO cross edge!!! Maybe a hole!!\n");
            }
            //SplitFace(acFaces,v,f1,nfAdj_new, currentVert);
        }

        if (IsNonObtuse(v,f2))
        {
            //switch (SPEEDTYPE)
            switch(speedType)
            {
            case CURVATURE:
                /*
            f2.speedInv = ( abs(curv1[f2[0]] + curv2[f2[0]]) +
                                                abs( curv1[f2[1]] + curv2[f2[1]]) +
                                                abs(curv1[f2[2]] + curv2[f2[2]]) ) / 6;
            */
                f2.speedInv = ( abs_curv[f2[0]] +
                        abs_curv[f2[1]] +
                        abs_curv[f2[2]] ) / 3.0;

                break;
            case ONE:
                f2.speedInv = 1.0;
                break;
            case NOISE:
                f2.speedInv = (noiseOnVert[f2[0]] +
                        noiseOnVert[f2[1]] +

                        noiseOnVert[f2[2]]) / 3;

                break;
            default:
                f2.speedInv = 1.0;
                break;
            }


            vec3 edge01 = (vec3)(vertices[f2[1]] - vertices[f2[0]]);
            vec3 edge12 = (vec3)(vertices[f2[2]] - vertices[f2[1]]);
            vec3 edge20 = (vec3)(vertices[f2[0]] - vertices[f2[2]]);
            f2.edgeLens[0] =sqrt(edge01[0]*edge01[0] + edge01[1]*edge01[1] + edge01[2]*edge01[2]);
            f2.edgeLens[1] =sqrt(edge12[0]*edge12[0] + edge12[1]*edge12[1] + edge12[2]*edge12[2]);
            f2.edgeLens[2] =sqrt(edge20[0]*edge20[0] + edge20[1]*edge20[1] + edge20[2]*edge20[2]);
            acFaces.push_back(f2);
        }
        else
        {
            int nfAdj_new = this->across_edge[nfAdj][this->faces[nfAdj].indexof(v4)];
            if (nfAdj_new > -1)
            {
                SplitFace(acFaces,v,f2,nfAdj_new/*,currentVert*/);
            }
            else
            {
                //printf("NO cross edge!!! Maybe a hole!!\n");
            }
            //SplitFace(acFaces,v,f2,nfAdj_new,currentVert);
        }
    }


    // FIM: one ring function
    vector<Face> GetOneRing(int v/*, int currentVert*/)
    {
        // make sure we have the across-edge map
        if (this->across_edge.empty())
            this->need_across_edge();

        // variables required
        vector<Face> oneRingFaces;
        vector<Face> t_faces;

        // get adjacent faces
        int naf = this->adjacentfaces[v].size();

        if (!naf)
        {
            std::cout << "vertex " << v << " has 0 adjacent faces..." << std::endl;
        }
        else
        {
            for (int af = 0; af < naf; af++)
            {
                Face cf = this->faces[adjacentfaces[v][af]];

                t_faces.clear();
                if(IsNonObtuse(v,cf))// check angle: if non-obtuse, return existing face
                {
                    //this->colors[cf[0]] = Color::red();
                    //this->colors[cf[1]] = Color::red();
                    //this->colors[cf[2]] = Color::red();
                    t_faces.push_back(cf);
                }
                else
                {
                    //t_faces.push_back(cf);
                    int nfae = this->across_edge[this->adjacentfaces[v][af]][cf.indexof(v)];
                    if (nfae > -1)
                    {
                        SplitFace(t_faces,v,cf,nfae/*,currentVert*/);// if obtuse, split face till we get all acute angles
                    }
                    else
                    {
                        //printf("NO cross edge!!! Maybe a hole!!\n");
                    }
                    //SplitFace(t_faces,v,cf,nfae,currentVert);// if obtuse, split face till we get all acute angles
                }

                for (int tf = 0; tf < t_faces.size(); tf++)
                {
                    //this->colors[t_faces[tf][0]] = Color::red();
                    //this->colors[t_faces[tf][1]] = Color::red();
                    //this->colors[t_faces[tf][2]] = Color::red();
                    oneRingFaces.push_back(t_faces[tf]);
                }
            }
        }
        //this->colors[v] = Color::green();
        return oneRingFaces;
    }


    // FIM: initialize attributes
    //typedef std::<int> ListType;
    void InitializeAttributes(int currentVert , std::vector<int> seeds = vector<int>() )
    {
        int nv = this->vertices.size();
        this->geodesic = new float[nv];

        for(int v= 0; v < nv; v++){
            geodesic[v] = LARGENUM;
        }

        // initialize seed points if present...
        if (!seeds.empty()){
            int ns = seeds.size();
            for (int s = 0; s < ns; s++){
                //this->vertMap[currentVert][seeds[s]].d = 0;
                geodesic[seeds[s]] = 0;
            }
        }


        // pre-compute faces, normals, and other per-vertex properties that may be needed
        this->need_neighbors();
        this->need_normals();
        this->need_adjacentfaces();
        this->need_across_edge();
        this->need_faces();

        /* HOW DO WE DO THIS USING NEW GEODESIC DATA STRUCTURE?
    // for all faces: initialize per-vertex travel time and face-speed
    int nf = this->faces.size();
    for (int f = 0; f < nf; f++)
    {
        Face cf = this->faces[f];

        // travel time
        faces[f].T[0] = this->vertT[currentVert][cf[0]];
        faces[f].T[1] = this->vertT[currentVert][cf[1]];
        faces[f].T[2] = this->vertT[currentVert][cf[2]];
    }
    */
    }

    // FIM:  Remove data lingering from computation
    void CleanupAttributes(int currentVert)
    {
        delete [] this->geodesic;
    }



    /* Prateep */
    void WriteFaceIndexMap(const char* outfilename)
    {
        std::ofstream fout(outfilename, std::ios::out);
        map<VoxelIndexType, vector<int> >::iterator faceIndexMapIt;
        vector<int>::iterator faceIndexIt;

        for(faceIndexMapIt = this->faceIndexMap.begin(); faceIndexMapIt != this->faceIndexMap.end(); faceIndexMapIt++)
        {
            fout << (int) (*faceIndexMapIt).first << ": ";
            for(faceIndexIt = faceIndexMapIt->second.begin(); faceIndexIt != faceIndexMapIt->second.end(); faceIndexIt++) {
                fout << (*faceIndexIt) << " ";
            }
            fout << std::endl;
        }
        fout.close();
    }

    /* Prateep */
    void ReadFaceIndexMap(const char* infilename)
    {
        std::ifstream infile(infilename);
        if(!infile.is_open())
        {
            std::cout << "File Not Found:" << infilename << std::endl;
        }
        else
        {
            std::cout << "reading face indices from " << infilename << std::endl;

//            map<VoxelIndexType, set<int> > tmpFaceIndexMap;
            std::string line;
            while(infile)
            {
                getline(infile, line);
                std::stringstream ss(line);
                VoxelIndexType index;
                char delim;
                ss >> index >> delim;
                int fid;
                while(ss >> fid) {
                    this->faceIndexMap[index].push_back(fid);
//                    tmpFaceIndexMap[index].insert(fid);
                }
            }

            //            if(tmpFaceIndexMap.size() != 0 )
            //            {
            //                this->faceIndexMap = tmpFaceIndexMap;
            //            }

            //            tmpFaceIndexMap.clear(); // clear memory
            infile.close();
        }
    }

    void ClearFaceIndexMap()
    {
        this->faceIndexMap.clear();
    }

    /* Prateep */
    float getMaximumEdgeLength()
    {
        double s = 0.0f;
        for(unsigned int f = 0; f < this->faces.size(); f++)
        {
            if(s < this->faces[f].edgeLens[0]) { // 01
                s = this->faces[f].edgeLens[0];
            }
            if(s < this->faces[f].edgeLens[1]) { // 12
                s = this->faces[f].edgeLens[1];
            }
            if(s < this->faces[f].edgeLens[2]) { // 20
                s = this->faces[f].edgeLens[2];
            }
        }
        return s;
    }
    
    /* Prateep */
    void physicalPointToXYZ(point x, VoxelIndexType* imageX) // physical to image coordinates
    {
        imageX[0] = static_cast<VoxelIndexType> ( (x[0] - this->imageOrigin[0]) / this->imageSpacing[0] );
        imageX[1] = static_cast<VoxelIndexType> ( (x[1] - this->imageOrigin[1]) / this->imageSpacing[1] );
        imageX[2] = static_cast<VoxelIndexType> ( (x[2] - this->imageOrigin[2]) / this->imageSpacing[2] );
    }

    void physicalPointToXYZ(point x, VoxelIndexType* imageX, float imageOrigin[3], float imageSpacing[3])
    {
        imageX[0] = static_cast<VoxelIndexType> ( (x[0] - imageOrigin[0]) / imageSpacing[0] );
        imageX[1] = static_cast<VoxelIndexType> ( (x[1] - imageOrigin[1]) / imageSpacing[1] );
        imageX[2] = static_cast<VoxelIndexType> ( (x[2] - imageOrigin[2]) / imageSpacing[2] );
    }

    /* Prateep */
    point indexToPhysicalPoint(VoxelIndexType* imageX)
    {
        point ret;
        ret[0] = static_cast<float> ( imageX[0] * this->imageSpacing[0] + this->imageOrigin[0] );
        ret[1] = static_cast<float> ( imageX[1] * this->imageSpacing[1] + this->imageOrigin[1] );
        ret[2] = static_cast<float> ( imageX[2] * this->imageSpacing[2] + this->imageOrigin[2] );
        return ret;
    }

    /* Prateep */
    point indexToPhysicalPoint(VoxelIndexType* imageX, float origin[3], float spacing[3])
    {
        point ret;
        ret[0] = static_cast<float> ( imageX[0] * spacing[0] + origin[0] );
        ret[1] = static_cast<float> ( imageX[1] * spacing[1] + origin[1] );
        ret[2] = static_cast<float> ( imageX[2] * spacing[2] + origin[2] );
        return ret;
    }

    /* Prateep */
    bool isInsideImageBuffer(itk::Image<int,3>::IndexType ind)
    {
        if(ind[0] >= this->imageIndex[0] && ind[0] < this->imageSize[0] &&
                ind[1] >= this->imageIndex[1] && ind[1] < this->imageSize[1] &&
                ind[2] >= this->imageIndex[2] && ind[2] < this->imageSize[2]
                ) {
            return true;
        }
        else {
            return false;
        }
    }

    // SHIREEN

    VoxelIndexType physicalPointToLinearIndex(point x)
    {
        VoxelIndexType imageX[3];
        this->physicalPointToXYZ(x,imageX,this->imageOrigin, this->imageSpacing);

        VoxelIndexType linearIndX = this->indexToLinearIndex(imageX, this->imageSize);
        return linearIndX;
    }

    VoxelIndexType physicalPointToLinearIndex(point x, float imageOrigin[3], float imageSpacing[3], int imageSize[3])
    {
        VoxelIndexType imageX[3];
        this->physicalPointToXYZ(x,imageX,imageOrigin,imageSpacing);

        VoxelIndexType linearIndX = this->indexToLinearIndex(imageX, imageSize);
        return linearIndX;
    }

    void linearIndexToXYZ(VoxelIndexType linearIndX, VoxelIndexType* imageX, int imageSize[3])
    {
        // convert linear index to r, c, s
        imageX[2] = linearIndX / (imageSize[0]*imageSize[1]); // slice value (Note: integer division)
        linearIndX %= (imageSize[0]*imageSize[1]);

        imageX[1] = linearIndX / imageSize[0]; // column value (Note: integer division)

        imageX[0] = linearIndX % imageSize[0]; // row value
    }

    void linearIndexToXYZ(VoxelIndexType linearIndX, VoxelIndexType* imageX)
    {
        // convert linear index to r, c, s
        imageX[2] = linearIndX / (imageSize[0]*imageSize[1]); // slice value (Note: integer division)
        linearIndX %= (imageSize[0]*imageSize[1]);

        imageX[1] = linearIndX / imageSize[0]; // column value (Note: integer division)

        imageX[0] = linearIndX % imageSize[0]; // row value
    }

    VoxelIndexType indexToLinearIndex(VoxelIndexType* imageX, int imageSize[3])
    {
        VoxelIndexType linearIndX = imageX[0] + imageX[1] * imageSize[0] + imageX[2] * imageSize[0] * imageSize[1];
        return linearIndX;
    }

    VoxelIndexType indexToLinearIndex(VoxelIndexType* imageX)
    {
        VoxelIndexType linearIndX = imageX[0] + imageX[1] * imageSize[0] + imageX[2] * imageSize[0] * imageSize[1];
        return linearIndX;
    }

    point linearIndexToPhysicalPoint(VoxelIndexType linearIndX, float imageOrigin[3], float imageSpacing[3], int imageSize[3])
    {
        VoxelIndexType imageX[3];
        this->linearIndexToXYZ(linearIndX, imageX, imageSize);

        point p = this->indexToPhysicalPoint(imageX, imageOrigin, imageSpacing);
        return p;
    }


    point linearIndexToPhysicalPoint(VoxelIndexType linearIndX)
    {
        VoxelIndexType imageX[3];
        this->linearIndexToXYZ(linearIndX, imageX, imageSize);

        point p = this->indexToPhysicalPoint(imageX, imageOrigin, imageSpacing);
        return p;
    }

    // end SHIREEN

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
    double pointTriangleDistance(point P, Face face, point& PP)
    {
        // rewrite vertices in normal form
        point B  = this->vertices[face.v[0]];
        point E0 = this->vertices[face.v[1]] - B;
        point E1 = this->vertices[face.v[2]] - B;

        point D = B - P;
        float a = E0 DOT E0;
        float b = E0 DOT E1;
        float c = E1 DOT E1;
        float d = E0 DOT D;
        float e = E1 DOT D;
        float f = D  DOT D;

        float det = a*c - b*b;
        float s   = b*e - c*d;
        float t   = b*d - a*e;

        float distSqr = 0.0f;

        if(s+t <= det) {
            if(s < 0) {
                if(t < 0) {
                    // region 4
                    if(d < 0) {
                        t = 0;
                        if(-d >= a) {
                            s = 1.0;
                            distSqr = a + 2.0f*d + f;
                        } else {
                            s = -d/a;
                            distSqr = d*s + f;
                        }
                    } else {
                        s = 0.0f;
                        if(e >= 0.0f) {
                            t = 0.0f;
                            distSqr = f;
                        } else {
                            if(-e >= c) {
                                t = 1.0f;
                                distSqr = c + 2.0f*e + f;
                            } else {
                                t = -e/c;
                                distSqr = e*t + f;
                            }
                        }
                    } // end of region 4
                } else {
                    // region 3
                    s = 0.0f;
                    if(e >= 0.0f) {
                        t = 0.0f;
                        distSqr = f;
                    } else {
                        if(-e >= c) {
                            t = 1.0f;
                            distSqr = c + 2.0f*e + f;
                        } else {
                            t = -e/c;
                            distSqr = e*t + f;
                        }
                    }
                } // end of region 3
            } else {
                if(t < 0.0f) {
                    // region 5
                    t = 0.0f;
                    if (d >= 0.0f) {
                        s = 0.0f;
                        distSqr = f;
                    } else {
                        if(-d >= a) {
                            s = 1.0f;
                            distSqr = a + 2*d + f;
                        } else {
                            s = -d/a;
                            distSqr = d*s + f;
                        }
                    }
                    // end of region 5
                } else {
                    // region 0
                    float invDet = 1.0f/det;
                    s *= invDet;
                    t *= invDet;
                    distSqr = s * (a*s + b*t + 2*d) + t*(b*s + c*t + 2*e) + f;
                    // end of region 0
                }
            }
        } else {
            if(s < 0.0f) {
                // region 2
                float tmp0 = b+d;
                float tmp1 = c+e;
                if(tmp1 > tmp0) {
                    float numer = tmp1 - tmp0;
                    float denom = a - 2*b + c;
                    if(numer >= denom) {
                        s = 1.0f;
                        t = 0.0f;
                        distSqr = a + 2*d + f;
                    } else {
                        s = numer / denom;
                        t = 1.0 - s;
                        distSqr = s*(a*s + b*t + 2*d) + t*(b*s + c*t + 2*e) + f;
                    }
                } else {
                    s = 0.0f;
                    if(tmp1 <= 0.0f) {
                        t = 1.0f;
                        distSqr = c + 2*e + f;
                    } else {
                        if(e >= 0.0f) {
                            t = 0.0f;
                            distSqr = f;
                        } else {
                            t = -e/c;
                            distSqr = e*t+f;
                        }
                    }
                }
                // end of region 2
            } else {
                if(t < 0) {
                    // region 6
                    float tmp0 = b + e;
                    float tmp1 = a + d;
                    if(tmp1 > tmp0) {
                        float numer = tmp1 - tmp0;
                        float denom = a-2*b+c;
                        if(numer >= denom) {
                            t = 1.0f;
                            s = 0.0f;
                            distSqr = c + 2*e + f;
                        } else {
                            t = numer / denom;
                            s = 1.0 - t;
                            distSqr = s*(a*s + b*t + 2*d) + t*(b*s + c*t + 2*e) + f;
                        }
                    } else {
                        t = 0.0f;
                        if(tmp1 <= 0.0f) {
                            s = 1.0f;
                            distSqr = a + 2*d + f;
                        } else {
                            if(d >= 0.0f) {
                                s = 0.0f;
                                distSqr = f;
                            } else {
                                s = -d/a;
                                distSqr = d*s + f;
                            }
                        }
                    }
                    // end of region 6
                } else {
                    // region 1
                    float numer = c + e - b - d;
                    if(numer <= 0) {
                        s = 0.0f;
                        t = 1.0f;
                        distSqr = c + 2*e + f;
                    } else {
                        float denom = a - 2*b + c;
                        if(numer >= denom) {
                            s = 1.0f;
                            t = 0.0f;
                            distSqr = a + 2*d + f;
                        } else {
                            s = numer / denom;
                            t = 1.0f-s;
                            distSqr = s*(a*s + b*t + 2*d) + t*(b*s + c*t + 2*e) + f;
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

    /* Prateep */
    int pointTriangleRegion(point P, Face face)
    {
        // rewrite vertices in normal form
        point B  = this->vertices[face.v[0]];
        point E0 = this->vertices[face.v[1]] - B;
        point E1 = this->vertices[face.v[2]] - B;

        point D = B - P;
        float a = E0 DOT E0;
        float b = E0 DOT E1;
        float c = E1 DOT E1;
        float d = E0 DOT D;
        float e = E1 DOT D;
        float f = D  DOT D;

        float det = a*c - b*b;
        float s   = b*e - c*d;
        float t   = b*d - a*e;

        float distSqr = 0.0f;
        int region;

        if(s+t <= det) {
            if(s < 0) {
                if(t < 0) {
                    // region 4
                    region = 4;
                    // end of region 4
                } else {
                    // region 3
                    region = 3;
                    // end of region 3
                }
            } else if(t < 0.0f) {
                // region 5
                region = 5;
                // end of region 5
            } else {
                // region 0
                region = 0;
                // end of region 0
            }
        } else {
            if(s < 0.0f) {
                // region 2
                region = 2;
                // end of region 2
            } else {
                if(t < 0) {
                    // region 6
                    region = 6;
                    // end of region 6
                } else {
                    // region 1
                    region = 1;
                    // end of region 1
                }
            }
        }

        return region;

    }

    /* Prateep */
    void generateFaceIndexMapViaKDtree(itk::Image<PixelType, 3>::ConstPointer narrowBand, int number_of_subvoxels = 1,
                                       int num_threads = 1, std::string debug_prefix = "")
    {
        if( !this->kd ) this->need_kdtree();
        this->faceIndexMap.clear();
        this->number_of_subvoxels = number_of_subvoxels;

        const double eps = 1e-10;

        // inline DeepCopy
        itk::Image<PixelType, 3>::Pointer OutputImage = itk::Image<PixelType, 3>::New();
        OutputImage->SetRegions( narrowBand->GetLargestPossibleRegion() );
        OutputImage->Allocate();
        OutputImage->SetOrigin( narrowBand->GetOrigin() );
        OutputImage->SetSpacing( narrowBand->GetSpacing() );
        OutputImage->SetDirection( narrowBand->GetDirection() );

        itk::ImageRegionConstIteratorWithIndex< itk::Image<PixelType,3> > narrowBandIt(narrowBand, narrowBand->GetLargestPossibleRegion());
        itk::ImageRegionIterator< itk::Image<PixelType,3> > OutputImageIt(OutputImage, narrowBand->GetLargestPossibleRegion() );
        narrowBandIt.GoToBegin();
        OutputImageIt.GoToBegin();

        while(!narrowBandIt.IsAtEnd() ) {
            OutputImageIt.Set( narrowBandIt.Get() );
            ++narrowBandIt;
            ++OutputImageIt;
        }

        narrowBandIt.GoToBegin();
        OutputImageIt.GoToBegin();

        std::cout << "FidsViaKDTree. Starting functor ...\n";
        itk::TimeProbe clock;

        clock.Start();
        {
            typedef MapFunctorKDtree< itk::Image<PixelType,3>, itk::Image<PixelType,3>, TriMesh > FType;
            FType functor(OutputImage);
            // We need kdtree, so input vertices, faces and adjacentfaces. Rest are just function calls for TriMesh::. So, other members need not be past.
            functor.mesh = *this;
            functor.mesh.faces = this->faces;
            functor.mesh.vertices = this->vertices;
            functor.mesh.adjacentfaces = this->adjacentfaces;
            functor.setKD();

            bambam::map< itk::Image<PixelType, 3>, itk::Image<PixelType,3>, FType>::run(narrowBand, functor, num_threads);
        }
        clock.Stop();
        std::cout << "Time taken (functor)\n";
        std::cout << "Mean : " << clock.GetMean() << std::endl;
        std::cout << "Total : " << clock.GetTotal() << std::endl;
        std::cout << "---------------------------\n";

        if(debug_prefix.compare("") != 0)
        {
            itk::ImageFileWriter< itk::Image<PixelType,3> >::Pointer writer = itk::ImageFileWriter< itk::Image<PixelType, 3> >::New();
            std::string f = debug_prefix + ".faceInd.nrrd";
            writer->SetFileName( f.c_str() );
            writer->SetInput( OutputImage );
            writer->Update();

            saveFidsViaKDtreeDistanceMap(OutputImage, debug_prefix);
        }

        // Collect values in faceIndexMap
        itk::Image<PixelType, 3>::IndexType index     = OutputImage->GetLargestPossibleRegion().GetIndex();
        itk::Image<PixelType, 3>::SizeType size       = OutputImage->GetLargestPossibleRegion().GetSize();
        itk::Image<PixelType, 3>::PointType origin    = OutputImage->GetOrigin();
        itk::Image<PixelType, 3>::SpacingType spacing = OutputImage->GetSpacing();

        // Store origin of image domain
        this->imageOrigin[0] = origin[0];
        this->imageOrigin[1] = origin[1];
        this->imageOrigin[2] = origin[2];

        // Store spacing of image domain of the original DT
        this->imageSpacing[0] = spacing[0] * number_of_subvoxels;
        this->imageSpacing[1] = spacing[1] * number_of_subvoxels;
        this->imageSpacing[2] = spacing[2] * number_of_subvoxels;

        // Store size of image domain of the original DT
        this->imageSize[0] = size[0] / number_of_subvoxels;
        this->imageSize[1] = size[1] / number_of_subvoxels;
        this->imageSize[2] = size[2] / number_of_subvoxels;

        // collect the results from the subvoxels to the original voxels
        for(unsigned int i = 0; i < size[0]; i++) { // X
            for(unsigned int j = 0; j < size[1]; j++) { // Y
                for(unsigned int k = 0; k < size[2]; k++) { // Z
                    itk::Image<PixelType, 3>::IndexType idx;
                    idx[0] = i; idx[1] = j; idx[2] = k;
                    //if((i==256) && (j==80) &&(k==256) )
                    //    int p=0;
                    if(OutputImage->GetPixel(idx) > -1)
                    {
                        itk::Image<PixelType, 3>::IndexType idx2;

                        idx2[0] =  (VoxelIndexType)floor((float)i / (float)number_of_subvoxels);
                        idx2[1] =  (VoxelIndexType)floor((float)j / (float)number_of_subvoxels);
                        idx2[2] =  (VoxelIndexType)floor((float)k / (float)number_of_subvoxels);

                        VoxelIndexType idx1 = idx2[0] + idx2[1] * this->imageSize[0] + idx2[2] * this->imageSize[0] * this->imageSize[1];

                        this->faceIndexMap[idx1].push_back( OutputImage->GetPixel(idx) );
                    }
                }
            }
        }

        std::cout << "\nLength of face Index Map " << this->faceIndexMap.size() << std::endl;

        if(debug_prefix.compare("") > 0)
        {
            std::cout << "Now saving distance map...";
            saveFidsViaKDtreeDistanceMap(OutputImage, debug_prefix);

            //            std::cout << "Now saving signed distance map...";
            //            saveFidsSignedDistanceMap(OutputImage, debug_prefix);
            //            std::cout << "Done\n";
        }
    }

    /* Prateep */
    void generateFaceIndexMapViaSuperVoxel(itk::Image<PixelType, 3>::ConstPointer narrowBand, itk::Image<PixelType, 3>::Pointer scaledDT,
                                           float q, float ldelta,
                                           int number_of_subvoxels = 1, int number_of_voxels = 1, float radiusFactor = 1.0, // search for all neighbors
                                           int num_threads = 1, std::string debug_prefix = "", std::string debug_suffix = "", bool saveFaceIndMap = false)
    {
        this->faceIndexMap.clear();
        this->number_of_subvoxels = number_of_subvoxels;
        this->number_of_voxels    = number_of_voxels;

        // inline DeepCopy
        itk::Image<int, 3>::Pointer OutputImage = itk::Image<int, 3>::New();
        OutputImage->SetRegions( narrowBand->GetLargestPossibleRegion() );
        OutputImage->Allocate();
        OutputImage->SetOrigin( narrowBand->GetOrigin() );
        OutputImage->SetSpacing( narrowBand->GetSpacing() );
        OutputImage->SetDirection( narrowBand->GetDirection() );

        itk::ImageRegionConstIteratorWithIndex< itk::Image<PixelType,3> > narrowBandIt(narrowBand, narrowBand->GetLargestPossibleRegion());
        //itk::ImageRegionIteratorWithIndex< itk::Image<int,3> > narrowBandItShared(narrowBand, narrowBand->GetLargestPossibleRegion());
        itk::ImageRegionIterator< itk::Image<int,3> > OutputImageIt(OutputImage, narrowBand->GetLargestPossibleRegion() );

        narrowBandIt.GoToBegin();
        OutputImageIt.GoToBegin();

        while(!narrowBandIt.IsAtEnd() ) {
            OutputImageIt.Set( narrowBandIt.Get() );
            ++narrowBandIt;
            ++OutputImageIt;
        }

        narrowBandIt.GoToBegin();
        //narrowBandItShared.GoToBegin();
        OutputImageIt.GoToBegin();
#ifdef MP_USE_OPENMP
        omp_set_num_threads(6);
#endif

        int nf = this->faces.size();
        itk::Image<int, 3>::PointType ori = OutputImage->GetOrigin();
        itk::Image<int, 3>::SpacingType sp = OutputImage->GetSpacing();
        itk::Image<int, 3>::SizeType si    = OutputImage->GetLargestPossibleRegion().GetSize();

        float supVoxelOrigin[3];
        supVoxelOrigin[0] = (float) (ori[0]);
        supVoxelOrigin[1] = (float) (ori[1]);
        supVoxelOrigin[2] = (float) (ori[2]);

        float supVoxelSpacing[3];
        supVoxelSpacing[0] = (float) (sp[0] * number_of_voxels * number_of_subvoxels);
        supVoxelSpacing[1] = (float) (sp[1] * number_of_voxels * number_of_subvoxels);
        supVoxelSpacing[2] = (float) (sp[2] * number_of_voxels * number_of_subvoxels);

        int supVoxelSize[3];
        supVoxelSize[0] = (int) ((si[0] / (float)number_of_subvoxels) / (float)number_of_voxels);
        supVoxelSize[1] = (int) ((si[1] / (float)number_of_subvoxels) / (float)number_of_voxels);
        supVoxelSize[2] = (int) ((si[2] / (float)number_of_subvoxels) / (float)number_of_voxels);

        // super voxel face list
        map<int, vector<int> > superVoxelFaceList;

        // 1. Compute \sigma --> maximum physical distance for each supVoxel from isosurface
        //int sigmaSV = -LARGENUM;

        //        //        // shireen debug
        //        VoxelIndexType vox_ind = 2524975; //2574480; // vox_ind == 2524975 || vox_ind == 8340369)

        //        //        VoxelIndexType voxX[3], voxXX[3];

        //        //        this->linearIndexToXYZ(vox_ind, voxX, this->imageSize);

        //        //        VoxelIndexType vox_ind22 = this->indexToLinearIndex(voxX, this->imageSize);

        //        //        point vox_p = this->indexToPhysicalPoint(voxX, this->imageOrigin, this->imageSpacing);

        //        //        this->physicalPointToXYZ(vox_p, voxXX, this->imageOrigin, this->imageSpacing);

        //        //        VoxelIndexType vox_ind2 = this->physicalPointToLinearIndex(vox_p, this->imageOrigin, this->imageSpacing, this->imageSize);

        //        point vox_p = linearIndexToPhysicalPoint(vox_ind, this->imageOrigin, this->imageSpacing, this->imageSize);
        //        //        VoxelIndexType vox_ind2;
        //        //        vox_ind2 = this->physicalPointToLinearIndex(vox_p, this->imageOrigin, this->imageSpacing, this->imageSize);


        //        VoxelIndexType imageX[3];
        //        this->physicalPointToXYZ(vox_p, imageX, supVoxelOrigin, supVoxelSpacing);

        //        VoxelIndexType imageX_[3];
        //        this->physicalPointToXYZ(vox_p, imageX_, this->imageOrigin, this->imageSpacing);


        //        int iter2 = 0;
        //        for(int i = 0; i < supVoxelSize[0]; i++) {
        //            for(int j = 0; j < supVoxelSize[1]; j++) {
        //                for(int k = 0; k < supVoxelSize[2]; k++) {

        //                    if(i == imageX[0] && j == imageX[1] && k ==imageX[2])
        //                    {
        //                        int hihi = 0;
        //                    }
        //                    iter2++;

        //                }
        //            }
        //        }

        //        // end shireen debug

        int iter = 0;
        for(int i = 0; i < supVoxelSize[0]; i++) {
            for(int j = 0; j < supVoxelSize[1]; j++) {
                for(int k = 0; k < supVoxelSize[2]; k++) {

                    // the super voxel index
                    VoxelIndexType p[3];
                    p[0] = i; p[1] = j; p[2] = k;

                    // converting the supervoxel index to a physical point in space
                    point supV = this->indexToPhysicalPoint(p, supVoxelOrigin, supVoxelSpacing);

                    // poit to itkpoint
                    itk::Image<int, 3>::PointType supVp;
                    for(int ii = 0; ii < 3; ii++) supVp[ii] = supV[ii];

                    // Get the ball center
                    point supVCent = supV;
                    for(int ii = 0; ii < 3; ii++) {
                        supVCent[ii] += (float) q/2.0;
                    }

                    //                    // shireen debug


                    //                    if(imageX[0] == i && imageX[1] == j && imageX[2] == k)
                    //                    {
                    //                        int test = 0;
                    //                    }

                    //                    VoxelIndexType sind  = this->physicalPointToLinearIndex(vox_p, supVoxelOrigin, supVoxelSpacing, supVoxelSize);
                    //                    VoxelIndexType sind2 = this->physicalPointToLinearIndex(supV , supVoxelOrigin, supVoxelSpacing, supVoxelSize);
                    //                    if (sind == sind2)
                    //                        int test = 0;

                    //                    // end shireen debug

                    // get the subvoxel index of the supervoxel point (where is it lying in the scaled distance transform
                    itk::Image<PixelType,3>::IndexType supVInd;
                    //for(int ii = 0; ii < 3; ii++) supVInd[ii] = supV[ii];
                    scaledDT->TransformPhysicalPointToIndex(supVp, supVInd); // shireen, refer to the center not the corner

                    // get the distance (encoded in the distance transform) of this supervoxel from the mesh surface
                    float sigma = scaledDT->GetPixel(supVInd); // shireen debug float not int
                    // isotropic scaling
                    sigma *= scaledDT->GetSpacing()[0] * this->number_of_subvoxels;

                    // 2. Get ball radius
                    float ballRadiusSV = q + std::sqrt(ldelta*ldelta + sigma*sigma);
                    ballRadiusSV /= radiusFactor;

                    VoxelIndexType ind = this->physicalPointToLinearIndex(supV, supVoxelOrigin, supVoxelSpacing, supVoxelSize);

                    // 3. Get supVoxelFaceList
                    //                    #pragma omp parallel
                    {
                        //                        #pragma omp for
                        for(int f = 0; f < nf; f++) {

                            //                        if (f == 1536 || f == 4608)
                            //                            int tst = 0;

                            //std::cout << "Face # : " << f << std::endl;
                            point pp;
                            double d = this->pointTriangleDistance(supVCent, this->faces[f], pp);

                            if(d < ballRadiusSV + EPS)
                            {
                                //VoxelIndexType ind = this->physicalPointToLinearIndex(supV, supVoxelOrigin, supVoxelSpacing, supVoxelSize);
                                superVoxelFaceList[ind].push_back(f);
                            }
                        }

                    }
                    iter++;
                    if(superVoxelFaceList[ind].size() != 0)
                        std::cout << "iter : " << iter << " ,length(Facelist) = " << superVoxelFaceList[ind].size() << "\n";
                    //else
                    //    std::cout << "iter : " << iter << "ZERO!!!!!!!!! "<< "\n";
                }
            }
        }
        /*
            std::string outfilename = "tmpSize.txt";
            std::ofstream fout( outfilename.c_str(), std::ios::out);
            for(map<int, set<int> >::iterator it = superVoxelFaceList.begin(); it != superVoxelFaceList.end(); it++)
            {
                fout << (int) (*it).first << ": ";
    //            for(set<int>::iterator it1 = it->second.begin(); it1 != it->second.end(); it1++) {
    //                fout << (*it1) << " ";
    //            }
                fout << (int) (*it).second.size();
                fout << std::endl;
            }
            fout.close();
            */

        std::cout << "Starting functor ...\n";
        itk::TimeProbe clock;

        clock.Start();
        {
            typedef MapFunctor< itk::Image<PixelType,3>, itk::Image<int,3>, TriMesh > FType;
            FType functor(OutputImage);
            functor.superVoxelFaceList = superVoxelFaceList;
            for(int i = 0; i < 3; i++) {
                functor.supVoxelOrigin[i]  = supVoxelOrigin[i];
                functor.supVoxelSize[i]    = supVoxelSize[i];
                functor.supVoxelSpacing[i] = supVoxelSpacing[i];
            }
            // We only need faces. Rest are just function calls for TriMesh::. So, other members need not be past.
            functor.mesh = *this;
            functor.mesh.faces = this->faces;

            bambam::map< itk::Image<PixelType, 3>, itk::Image<int,3>, FType>::run(narrowBand, functor, num_threads);
        }
        clock.Stop();
        std::cout << "Time taken (functor)\n";
        std::cout << "Mean : " << clock.GetMean() << std::endl;
        std::cout << "Total : " << clock.GetTotal() << std::endl;
        std::cout << "---------------------------\n";

        if(saveFaceIndMap)
        {
            if(debug_prefix.compare ("") != 0)
            {
                itk::ImageFileWriter< itk::Image<int,3> >::Pointer writer = itk::ImageFileWriter< itk::Image<int, 3> >::New();
                //std::stringstream ss; ss << radiusFactor;
                //std::stringstream sp; sp << this->imageSpacing[0];
                std::string f = debug_prefix + ".faceInd" + debug_suffix + ".nrrd";
                writer->SetFileName( f.c_str() );
                writer->SetInput( OutputImage );
                writer->Update();

                //saveFidsViaSuperVoxelDistanceMap(OutputImage, debug_prefix, radiusFactor); // using the subvoxel resolution not the original one
            }
        }

        std::cout << "Computing .fids ...";

        // Collect values in faceIndexMap
        itk::Image<int, 3>::IndexType index = OutputImage->GetLargestPossibleRegion().GetIndex();
        itk::Image<int, 3>::SizeType size   = OutputImage->GetLargestPossibleRegion().GetSize();
        itk::Image<int, 3>::PointType origin = OutputImage->GetOrigin();
        itk::Image<int, 3>::SpacingType spacing = OutputImage->GetSpacing();

        // Store origin of image domain
        this->imageOrigin[0] = origin[0];
        this->imageOrigin[1] = origin[1];
        this->imageOrigin[2] = origin[2];

        // Store spacing of image domain
        this->imageSpacing[0] = spacing[0] * number_of_subvoxels;
        this->imageSpacing[1] = spacing[1] * number_of_subvoxels;
        this->imageSpacing[2] = spacing[2] * number_of_subvoxels;

        // Store size of image domain
        this->imageSize[0] = size[0] / number_of_subvoxels;
        this->imageSize[1] = size[1] / number_of_subvoxels;
        this->imageSize[2] = size[2] / number_of_subvoxels;

        this->imageIndex[0] = (int) (index[0] / number_of_subvoxels);
        this->imageIndex[1] = (int) (index[1] / number_of_subvoxels);
        this->imageIndex[2] = (int) (index[2] / number_of_subvoxels);

        VoxelIndexType minIndex = this->imageIndex[0] + this->imageIndex[1] * this->imageSize[0] + this->imageIndex[2] * this->imageSize[0] * this->imageSize[1];
        VoxelIndexType maxIndex = (this->imageSize[0]-1) + (this->imageSize[1]-1) * this->imageSize[0] + (this->imageSize[2]-1) * this->imageSize[0] * this->imageSize[1];

        for(unsigned int i = index[0]; i < size[0]; i++) { // X
            for(unsigned int j = index[1]; j < size[1]; j++) { // Y
                for(unsigned int k = index[2]; k < size[2]; k++) { // Z
                    itk::Image<int, 3>::IndexType idx;
                    idx[0] = i; idx[1] = j; idx[2] = k;
                    if(OutputImage->GetPixel(idx) > -1) {
                        itk::Image<int, 3>::IndexType idx2;
                        idx2[0] =  (int) (i / number_of_subvoxels);
                        idx2[1] =  (int) (j / number_of_subvoxels);
                        idx2[2] =  (int) (k / number_of_subvoxels);

                        for(int dx = -2; dx <= 2; dx++) {
                            for(int dy = -2; dy <= 2; dy++) {
                                for(int dz = -2; dz <= 2; dz++) {

                                    itk::Image<int,3>::IndexType idx22;
                                    idx22[0] = idx2[0] + dx;
                                    idx22[1] = idx2[1] + dy;
                                    idx22[2] = idx2[2] + dz;

                                    if( !isInsideImageBuffer(idx22)) continue;

                                    VoxelIndexType idx1 = idx22[0] + idx22[1] * this->imageSize[0] + idx22[2] * this->imageSize[0] * this->imageSize[1];

                                    if(idx1 < 0) {
                                        std::cout << "Index neg " << idx1 << std::endl;
                                        continue;
                                    }

                                    // SHIREEN: when moving from set to vector for memory footprint, we need to make sure taht we are not pushing duplicate candidate faces
                                    // otherwise the fids file will be huge (multiples of gigs)
                                    int curf = OutputImage->GetPixel(idx);
                                    if (std::find(this->faceIndexMap[idx1].begin(), this->faceIndexMap[idx1].end(), curf) == this->faceIndexMap[idx1].end()) // current candidate has not been pushed back before
                                        this->faceIndexMap[idx1].push_back( curf );

                                    //                        VoxelIndexType idx1 = idx2[0] + idx2[1] * this->imageSize[0] + idx2[2] * this->imageSize[0] * this->imageSize[1];

                                    //                        this->faceIndexMap[idx1].insert( OutputImage->GetPixel(idx) );
                                }
                            }
                        }
                    }
                }
            }
        }

        std::cout << "Done";
        std::cout << "\nLength of face Index Map " << this->faceIndexMap.size() << std::endl;

        if(debug_prefix.compare("") > 0)
        {
            //            std::cout << "Now saving distance map...";
            //            saveFidsDistanceMap(OutputImage, debug_prefix, radiusFactor);

            std::cout << "Now saving signed distance map...";
            saveFidsSignedDistanceMap(OutputImage, debug_prefix, debug_suffix, radiusFactor);
            //saveFidsSignedDistanceMap(OutputImage, scaledDT, debug_prefix, debug_suffix, radiusFactor);
            std::cout << "Done\n";
        }
    }

    /* Prateep */
    void generateFaceIndexMapViaSuperVoxelSerial(itk::Image<PixelType, 3>::ConstPointer narrowBand, itk::Image<PixelType, 3>::Pointer scaledDT, float q, float ldelta,
                                                 int number_of_subvoxels = 1, int number_of_voxels = 1, float radiusFactor = 1.0, // search for all neighbors
                                                 std::string debug_prefix = "")
    {
        this->faceIndexMap.clear();
        this->number_of_subvoxels = number_of_subvoxels;
        this->number_of_voxels    = number_of_voxels;

        const double eps = 1e-6;

        // inline DeepCopy
        itk::Image<int, 3>::Pointer OutputImage = itk::Image<int, 3>::New();
        OutputImage->SetRegions( narrowBand->GetLargestPossibleRegion() );
        OutputImage->Allocate();
        OutputImage->SetOrigin( narrowBand->GetOrigin() );
        OutputImage->SetSpacing( narrowBand->GetSpacing() );
        OutputImage->SetDirection( narrowBand->GetDirection() );

        itk::ImageRegionConstIteratorWithIndex< itk::Image<PixelType,3> > narrowBandIt(narrowBand, narrowBand->GetLargestPossibleRegion());
        //itk::ImageRegionIteratorWithIndex< itk::Image<int,3> > narrowBandItShared(narrowBand, narrowBand->GetLargestPossibleRegion());
        itk::ImageRegionIterator< itk::Image<int,3> > OutputImageIt(OutputImage, narrowBand->GetLargestPossibleRegion() );

        narrowBandIt.GoToBegin();
        OutputImageIt.GoToBegin();

        while(!narrowBandIt.IsAtEnd() ) {
            OutputImageIt.Set( (int) narrowBandIt.Get() );
            ++narrowBandIt;
            ++OutputImageIt;
        }

        narrowBandIt.GoToBegin();
        //narrowBandItShared.GoToBegin();
        OutputImageIt.GoToBegin();
#ifdef MP_USE_OPENMP
        omp_set_num_threads(NUM_THREADS);
#endif
        map<int, vector<int> > superVoxelFaceList;

        int nf = this->faces.size();
        itk::Image<int, 3>::PointType ori = OutputImage->GetOrigin();
        itk::Image<int, 3>::SpacingType sp = OutputImage->GetSpacing();
        itk::Image<int, 3>::SizeType si    = OutputImage->GetLargestPossibleRegion().GetSize();

        float supVoxelOrigin[3];
        supVoxelOrigin[0] = (float) (ori[0]);
        supVoxelOrigin[1] = (float) (ori[1]);
        supVoxelOrigin[2] = (float) (ori[2]);

        float supVoxelSpacing[3];
        supVoxelSpacing[0] = (float) (sp[0] * number_of_voxels * number_of_subvoxels);
        supVoxelSpacing[1] = (float) (sp[1] * number_of_voxels * number_of_subvoxels);
        supVoxelSpacing[2] = (float) (sp[2] * number_of_voxels * number_of_subvoxels);

        int supVoxelSize[3];
        supVoxelSize[0] = (int) ((si[0] / (float)number_of_subvoxels) / (float)number_of_voxels);
        supVoxelSize[1] = (int) ((si[1] / (float)number_of_subvoxels) / (float)number_of_voxels);
        supVoxelSize[2] = (int) ((si[2] / (float)number_of_subvoxels) / (float)number_of_voxels);

        /***
             * ver 1. Search each face and map it onto a supervoxell
            for(int f = 0; f < nf; f++)
            {
                // points in physical coordinates
                point v0 = this->vertices[ this->faces[f].v[0] ],
                        v1 = this->vertices[ this->faces[f].v[1] ],
                        v2 = this->vertices[ this->faces[f].v[2] ];

                VoxelType ind0 = this->physicalPointToLinearIndex(v0, supVoxelOrigin, supVoxelSpacing, supVoxelSize);
                VoxelType ind1 = this->physicalPointToLinearIndex(v1, supVoxelOrigin, supVoxelSpacing, supVoxelSize);
                VoxelType ind2 = this->physicalPointToLinearIndex(v2, supVoxelOrigin, supVoxelSpacing, supVoxelSize);

                superVoxelFaceList[ind0].insert(f);
                superVoxelFaceList[ind1].insert(f);
                superVoxelFaceList[ind2].insert(f);
            }
            */
        /***
             * ver 2.
             */
        // 1. Compute \sigma --> maximum physical distance for each supVoxel from isosurface
        //int sigmaSV = -LARGENUM;
        int iter = 0;
        for(int i = 0; i < supVoxelSize[0]; i++) {
            for(int j = 0; j < supVoxelSize[1]; j++) {
                for(int k = 0; k < supVoxelSize[2]; k++) {
                    VoxelIndexType p[3];
                    p[0] = i; p[1] = j; p[2] = k;

                    point supV = this->indexToPhysicalPoint(p, supVoxelOrigin, supVoxelSpacing);
                    point supVCent = supV;
                    itk::Image<int, 3>::IndexType supVInd;
                    for(int ii = 0; ii < 3; ii++) supVInd[ii] = supV[ii];
                    int sigma = scaledDT->GetPixel(supVInd);

                    // 2. Get ball radius
                    float ballRadiusSV = q + std::sqrt(ldelta*ldelta + sigma*sigma);
                    ballRadiusSV /= radiusFactor;
                    // Get center
                    for(int ii = 0; ii < 3; ii++) {
                        supVCent[ii] += (float) q/2;
                    }

                    // 3. Get supVoxelFaceList
                    for(int f = 0; f < nf; f++) {
                        //std::cout << "Face # : " << f << std::endl;
                        point pp;
                        double d = this->pointTriangleDistance(supVCent, this->faces[f], pp);

                        if(d < ballRadiusSV + eps)
                        {
                            VoxelIndexType ind = this->physicalPointToLinearIndex(supV, supVoxelOrigin, supVoxelSpacing, supVoxelSize);
                            superVoxelFaceList[ind].push_back(f);
                        }
                    }

                    iter++;
                    std::cout << "iter : " << iter << "\n";
                }
            }
        }

        /* debug - prateep */
        std::string outfilename = "/home/sci/prateepm/Public/ForShireen/sphere/superVoxelList.txt";
        std::ofstream fout( outfilename.c_str(), std::ios::out);
        for(map<int, vector<int> >::iterator it = superVoxelFaceList.begin(); it != superVoxelFaceList.end(); it++)
        {
            fout << (int) (*it).first << ": ";
            for(vector<int>::iterator it1 = it->second.begin(); it1 != it->second.end(); it1++) {
                fout << (*it1) << " ";
            }
            fout << (int) (*it).second.size();
            fout << std::endl;
        }
        fout.close();
        /* debug - prateep  */

        int stop = 1;
        int fid = -1;
        int tn;

        //#pragma omp parallel private(tn,fid,narrowBandIt)
        {
#ifdef MP_USE_OPENMP
            tn = omp_get_thread_num();
#endif
            std::cout << "\nExecuting thread : " << tn << std::endl;
            while( stop != 0 ) {
                //#pragma omp critical
                {
                    // Voxel is in narrow band
                    if(narrowBandIt.Get() == 1)
                    {
                        point tmPoint;
                        itk::Image<int, 3>::PointType itkPoint;
                        narrowBand->TransformIndexToPhysicalPoint(narrowBandIt.GetIndex(), itkPoint);
                        for(int i = 0; i < 3; i++) { tmPoint[i] = itkPoint[i]; }

                        VoxelIndexType vox_ind = this->physicalPointToLinearIndex(tmPoint, this->imageOrigin, this->imageSpacing, this->imageSize);

                        //                        if (vox_ind == 25744820) // shireen
                        if(vox_ind == 2501770)
                            int tst = 0;

                        // Get neartest k vertices
                        vector<int> adjFaces; adjFaces.clear();
                        vector<int>::iterator adjFacesIt;
                        // find triangles enclosed inside each supervoxel
                        VoxelIndexType tmpInd = this->physicalPointToLinearIndex(tmPoint, supVoxelOrigin, supVoxelSpacing, supVoxelSize);
                        //                  std::cout << "Super Voxel List : " << superVoxelFaceList[tmpInd].size() << std::endl;


                        for(vector<int>::iterator it = superVoxelFaceList[tmpInd].begin(); it != superVoxelFaceList[tmpInd].end(); it++) {
                            adjFaces.push_back((*it));
                        }

                        std::cout << "Number of neighbors : " << adjFaces.size() << std::endl;
                        if(adjFaces.empty() ) //|| adjFaces.size() == nf)
                        {
                            // We can either abort here or ignore the voxel
                            OutputImageIt.Set(-1);
                        } else {

                            //std::cout << "Adjacent faces : " << this->adjacentfaces[imatch].size() << std::endl;
                            double minDist = LARGENUM;
                            for(adjFacesIt = adjFaces.begin(); adjFacesIt != adjFaces.end(); adjFacesIt++) {
                                point projPoint;
                                double dist = this->pointTriangleDistance(tmPoint, this->faces[*(adjFacesIt)], projPoint);
                                int region = this->pointTriangleRegion(tmPoint, this->faces[*(adjFacesIt)]);

                                //                                if (vox_ind == 2501770)
                                //                                    std::cout << "Adjacent faceId : " << *(adjFacesIt) << ", Dist : " << dist << ", Region : "  << region << std::endl;
                                if(dist + eps <= minDist) {
                                    minDist = dist;
                                    fid = *(adjFacesIt);
                                }
                            }
                            OutputImageIt.Set(fid);
                            adjFaces.clear();
                        }

                    } else {
                        OutputImageIt.Set(-1);
                    }

                    ++narrowBandIt;
                    ++OutputImageIt;
                }

                if(narrowBandIt.IsAtEnd() || OutputImageIt.IsAtEnd())
                {
                    stop = 0;
                    //#pragma omp flush(stop)
                }
            }
        }

        if(debug_prefix.compare ("") != 0)
        {
            itk::ImageFileWriter< itk::Image<int,3> >::Pointer writer = itk::ImageFileWriter< itk::Image<int, 3> >::New();
            std::stringstream ss; ss << radiusFactor;
            std::stringstream sp; sp << this->imageSpacing[0];
            std::string f = debug_prefix + ".faceInd_r" + ss.str() +  "_sp" + sp.str() + ".nrrd";
            writer->SetFileName( f.c_str() );
            writer->SetInput( OutputImage );
            writer->Update();
        }
        // Collect values in faceIndexMap
        itk::Image<int, 3>::IndexType index = OutputImage->GetLargestPossibleRegion().GetIndex();
        itk::Image<int, 3>::SizeType size   = OutputImage->GetLargestPossibleRegion().GetSize();
        itk::Image<int, 3>::PointType origin = OutputImage->GetOrigin();
        itk::Image<int, 3>::SpacingType spacing = OutputImage->GetSpacing();

        // Store origin of image domain
        this->imageOrigin[0] = origin[0];
        this->imageOrigin[1] = origin[1];
        this->imageOrigin[2] = origin[2];

        // Store spacing of image domain
        this->imageSpacing[0] = spacing[0] * number_of_subvoxels;
        this->imageSpacing[1] = spacing[1] * number_of_subvoxels;
        this->imageSpacing[2] = spacing[2] * number_of_subvoxels;

        // Store size of image domain
        this->imageSize[0] = size[0] / number_of_subvoxels;
        this->imageSize[1] = size[1] / number_of_subvoxels;
        this->imageSize[2] = size[2] / number_of_subvoxels;

        VoxelIndexType minIndex = this->imageIndex[0] + this->imageIndex[1] * this->imageSize[0] + this->imageIndex[2] * this->imageSize[0] * this->imageSize[1];
        VoxelIndexType maxIndex = (this->imageSize[0]-1) + (this->imageSize[1]-1) * this->imageSize[0] + (this->imageSize[2]-1) * this->imageSize[0] * this->imageSize[1];

        for(unsigned int i = index[0]; i < size[0]; i++) { // X
            for(unsigned int j = index[1]; j < size[1]; j++) { // Y
                for(unsigned int k = index[2]; k < size[2]; k++) { // Z
                    itk::Image<int, 3>::IndexType idx;
                    idx[0] = i; idx[1] = j; idx[2] = k;
                    if(OutputImage->GetPixel(idx) > -1) {
                        itk::Image<int, 3>::IndexType idx2;
                        idx2[0] =  (int) (i / number_of_subvoxels);
                        idx2[1] =  (int) (j / number_of_subvoxels);
                        idx2[2] =  (int) (k / number_of_subvoxels);
                        VoxelIndexType idx1 = idx2[0] + idx2[1] * this->imageSize[0] + idx2[2] * this->imageSize[0] * this->imageSize[1];

                        this->faceIndexMap[idx1].push_back( OutputImage->GetPixel(idx) );
                        if(idx1+1 <= maxIndex)
                            this->faceIndexMap[idx1+1].push_back( OutputImage->GetPixel(idx) );
                        if(idx1-1 >= minIndex)
                            this->faceIndexMap[idx1-1].push_back( OutputImage->GetPixel(idx) );
                    }
                }
            }
        }

        std::cout << "\nLength of face Index Map " << this->faceIndexMap.size() << std::endl;
    }


    int GetTriangleInfoForPoint(point x, Face& triangleX, float& alphaX, float& betaX, float& gammaX)
    {
        int faceID;

        if(this->faceIndexMap.size() > 0) // there is a generated face index map so used it
        {
            // Physical point to Image Index
            VoxelIndexType linearIndX = this->physicalPointToLinearIndex(x);

            // collect face indices for this voxel
            std::map<VoxelIndexType, vector<int> >::iterator it = this->faceIndexMap.find(linearIndX);
            if(it != this->faceIndexMap.end()) // see if the linearIndX already exist in the face index map
            {
//                std::cout << "WOW, fids will be used ... \n" ;
                vector<int> faceList = this->faceIndexMap[linearIndX];

                double minDist = LARGENUM;
                int winnerIndex;

                for(vector<int>::iterator it = faceList.begin();  it != faceList.end(); ++it)
                {
                    triangleX = this->faces[(*it)];

                    // project the point onto the plane of the current triangle
                    point  projPoint;
                    double dist = this->pointTriangleDistance(x, triangleX, projPoint);

                    if (dist < minDist )
                    {
                        minDist      = dist;
                        winnerIndex  = (*it);
                    }
                }

                triangleX       = this->faces[winnerIndex];
                faceID          = winnerIndex;
                point  projPoint;
                double dist = this->pointTriangleDistance(x, triangleX, projPoint);

                vec barycentric = this->ComputeBarycentricCoordinates(projPoint, triangleX);
                alphaX          = barycentric[0];
                betaX           = barycentric[1];
                gammaX          = barycentric[2];

            }
            else //kdtree based
            {
#if SHOW_WARNING
                std::cout << "warning: using kdtree for triangle info because voxel index " << linearIndX <<": "<< x <<" is not found in the face index map !!! ...\n" ;
#endif

                // get vertex closest to first point - x
                int vertX = this->FindNearestVertex(x);

                // scan all adjacent faces to see which face (f) includes point x
                triangleX = this->faces[ this->adjacentfaces[vertX][0] ];
                for (unsigned int fNumber = 0; fNumber < this->adjacentfaces[vertX].size(); fNumber++)
                {
                    // check if face contains x and store barycentric coordinates for x in face f
                    triangleX = this->faces[ this->adjacentfaces[vertX][fNumber] ];
                    faceID    = this->adjacentfaces[vertX][fNumber] ;
                    vec barycentric = this->ComputeBarycentricCoordinates(x,triangleX);
                    alphaX = barycentric[0];
                    betaX  = barycentric[1];
                    gammaX = barycentric[2];

                    if ( ( ( barycentric[0] >= 0 ) && ( barycentric[0] <= 1 ) ) &&
                         ( ( barycentric[1] >= 0 ) && ( barycentric[1] <= 1 ) ) &&
                         ( ( barycentric[2] >= 0 ) && ( barycentric[2] <= 1 ) ) )
                    {
                        fNumber = this->adjacentfaces[vertX].size();
                    }
                }
            }
        }
        else
        {
#if SHOW_WARNING
            std::cout << "warning: using kdtree for triangle info because there is no face index map !!! ...\n" ;
#endif

            // get vertex closest to first point - x
            int vertX = this->FindNearestVertex(x);
            unsigned int fNumber;

            // scan all adjacent faces to see which face (f) includes point x
            triangleX = this->faces[ this->adjacentfaces[vertX][0] ];
            faceID    = this->adjacentfaces[vertX][0];
            for (fNumber = 0; fNumber < this->adjacentfaces[vertX].size(); fNumber++)
            {
                // check if face contains x and store barycentric coordinates for x in face f
                triangleX = this->faces[ this->adjacentfaces[vertX][fNumber] ];
                faceID    = this->adjacentfaces[vertX][fNumber];
                vec barycentric = this->ComputeBarycentricCoordinates(x,triangleX);
                alphaX = barycentric[0];
                betaX  = barycentric[1];
                gammaX = barycentric[2];

                if ( ( ( barycentric[0] >= 0 ) && ( barycentric[0] <= 1 ) ) &&
                     ( ( barycentric[1] >= 0 ) && ( barycentric[1] <= 1 ) ) &&
                     ( ( barycentric[2] >= 0 ) && ( barycentric[2] <= 1 ) ) )
                {
                    fNumber = this->adjacentfaces[vertX].size();
                }
            }

            if(alphaX < 0.0 || betaX < 0.0f || gammaX < 0.0f ) {
                int t = 0;
            }
        }

        return faceID;

    }

    int GetVertexInfoForPoint(point x)
    {
        int vertX;
        Face triangleX;
        float alphaX, betaX, gammaX;

        if(this->faceIndexMap.size() > 0) // there is a generated face index map so used it
        {
            //std::cout << "WOW, fids will be used ... \n" ;
            // Physical point to Image Index
            VoxelIndexType linearIndX = this->physicalPointToLinearIndex(x);

            // collect face indices for this voxel
            std::map<VoxelIndexType, vector<int> >::iterator it = this->faceIndexMap.find(linearIndX);
            if(it != this->faceIndexMap.end())
            {
                vector<int> faceList = this->faceIndexMap[linearIndX];

                double minDist = LARGENUM;
                int winnerIndex;

                for(vector<int>::iterator it = faceList.begin();  it != faceList.end(); ++it)
                {
                    triangleX = this->faces[(*it)];

                    // project the point onto the plane of the current triangle
                    point  projPoint;
                    double dist = this->pointTriangleDistance(x, triangleX, projPoint);

                    if (dist < minDist )
                    {
                        minDist      = dist;
                        winnerIndex  = (*it);
                    }
                }

                triangleX       = this->faces[winnerIndex];

                point  projPoint;
                double dist = this->pointTriangleDistance(x, triangleX, projPoint);

                vec barycentric = this->ComputeBarycentricCoordinates(projPoint, triangleX);
                alphaX          = barycentric[0];
                betaX           = barycentric[1];
                gammaX          = barycentric[2];

                // get vertex closest to first point - x
                vertX = this->FindNearestVertex(projPoint);

            }
            else //kdtree based
            {
#if SHOW_WARNING
                std::cout << "warning: using kdtree for triangle info because voxel index " << linearIndX << " is not found in the face index map !!! ...\n" ;
#endif

                // get vertex closest to first point - x
                vertX = this->FindNearestVertex(x);

                // scan all adjacent faces to see which face (f) includes point x
                triangleX = this->faces[ this->adjacentfaces[vertX][0] ];
                for (unsigned int fNumber = 0; fNumber < this->adjacentfaces[vertX].size(); fNumber++)
                {
                    // check if face contains x and store barycentric coordinates for x in face f
                    triangleX = this->faces[ this->adjacentfaces[vertX][fNumber] ];
                    vec barycentric = this->ComputeBarycentricCoordinates(x,triangleX);
                    alphaX = barycentric[0];
                    betaX  = barycentric[1];
                    gammaX = barycentric[2];

                    if ( ( ( barycentric[0] >= 0 ) && ( barycentric[0] <= 1 ) ) &&
                         ( ( barycentric[1] >= 0 ) && ( barycentric[1] <= 1 ) ) &&
                         ( ( barycentric[2] >= 0 ) && ( barycentric[2] <= 1 ) ) )
                    {
                        fNumber = this->adjacentfaces[vertX].size();
                    }
                }
            }
        }
        else
        {
#if SHOW_WARNING
            std::cout << "warning: using kdtree for triangle info because there is no face index map !!! ...\n" ;
#endif

            // get vertex closest to first point - x
            vertX = this->FindNearestVertex(x);

            // scan all adjacent faces to see which face (f) includes point x
            triangleX = this->faces[ this->adjacentfaces[vertX][0] ];
            for (unsigned int fNumber = 0; fNumber < this->adjacentfaces[vertX].size(); fNumber++)
            {
                // check if face contains x and store barycentric coordinates for x in face f
                triangleX = this->faces[ this->adjacentfaces[vertX][fNumber] ];
                vec barycentric = this->ComputeBarycentricCoordinates(x,triangleX);
                alphaX = barycentric[0];
                betaX  = barycentric[1];
                gammaX = barycentric[2];

                if ( ( ( barycentric[0] >= 0 ) && ( barycentric[0] <= 1 ) ) &&
                     ( ( barycentric[1] >= 0 ) && ( barycentric[1] <= 1 ) ) &&
                     ( ( barycentric[2] >= 0 ) && ( barycentric[2] <= 1 ) ) )
                {
                    fNumber = this->adjacentfaces[vertX].size();
                }
            }
        }

        return vertX;
    }
    // end SHIREEN

    float GetEuclideanDistance(int v1,int v2)
    {
        float d = 0.000001f;
        point p1, p2;

        p1 = this->vertices[v1];
        p2 = this->vertices[v2];

        d = dist(p1,p2);

        return d;
    }
    
    // SHIREEN
    float GetEuclideanDistance(point p1, point p2)
    {
        float d = 0.000001f;
        d       = dist(p1,p2);
        return d;
    }
    // end SHIREEN

    float GetGeodesicDistance(int v1,int v2)
    {
        float gDist = 0.000001f;

        if (v1 == v2) return gDist;

        int vert = v1;
        int key = v2;

        if (v2 > v1)
        {
            vert = v2;
            key = v1;
        }

        std::map<unsigned int,float>::iterator geoIter = this->geodesicMap[vert].find(key);
        if (geoIter != this->geodesicMap[vert].end())
        {
            gDist = geoIter->second;
        }
        else
        {
            gDist = LARGENUM;
        }

        return gDist;
    }

    /* Prateep */
    double GetGeodesicDistance(point x, point y)
    {
        float alphaX, betaX, gammaX;
        Face triangleX;
        GetTriangleInfoForPoint(x,  triangleX, alphaX,  betaX,  gammaX);

        float alphaY, betaY, gammaY;
        Face triangleY;
        GetTriangleInfoForPoint(y,  triangleY, alphaY,  betaY,  gammaY);

        // compute geodesic distance by interpolation
        // level one, interpolate distance from source triangle to distination point (i.e. D(triangleX, y))
        float dx0y = ( alphaY * this->GetGeodesicDistance( triangleX.v[0], triangleY.v[0] ) ) +
                ( betaY  * this->GetGeodesicDistance( triangleX.v[0], triangleY.v[1] ) ) +
                ( gammaY * this->GetGeodesicDistance( triangleX.v[0], triangleY.v[2] ) );

        float dx1y = ( alphaY * this->GetGeodesicDistance( triangleX.v[1], triangleY.v[0] ) ) +
                ( betaY  * this->GetGeodesicDistance( triangleX.v[1], triangleY.v[1] ) ) +
                ( gammaY * this->GetGeodesicDistance( triangleX.v[1], triangleY.v[2] ) );

        float dx2y = ( alphaY * this->GetGeodesicDistance( triangleX.v[2], triangleY.v[0] ) ) +
                ( betaY  * this->GetGeodesicDistance( triangleX.v[2], triangleY.v[1] ) ) +
                ( gammaY * this->GetGeodesicDistance( triangleX.v[2], triangleY.v[2] ) );

        // level 2, interpolate distance between x & y
        float dxy = (alphaX * dx0y) + (betaX * dx1y) + (gammaX * dx2y);

        return dxy;
    }

    /* Praful */
    float TestReport(char* str, int numTri, int numPts)
    {
//        std::cout<<"Generating Test Report..."<<std::endl;
        std::ofstream myfile;
        myfile.open(str);
        int numFaces = this->faces.size();
        char str1[] = "Newton";
        char str2[] = "Bary";
        char str3[] = "LM";
        int counter = 0;
        for(int i=0; i<numTri; i++)
        {
//            std::cout<<"Counter: "<<++counter<<std::endl;
            int f1 = rand() % numFaces;
            int f2 = rand() % numFaces;
//            int tmpf = f1 + 1 + (rand() % 5);
//            std::cout<<"tmpf = "<<tmpf<<std::endl;
//            double param = (double) (tmpf)/ (double) (numFaces);

//            double intpart, fractpart;
//            fractpart = std::modf(param, &intpart);
//            int f2 = (int) std::floor(fractpart*(double)numFaces);
            Face Sa = this->faces[f1];
            Face Sb = this->faces[f2];
            point pta, ptb;
//            std::cout<<"Original: "<<std::endl;
//            std::cout<<"Face a: "<<Sa.v[0]<<" "<<Sa.v[1]<<" "<<Sa.v[2]<<std::endl;
//            std::cout<<"Face b: "<<Sb.v[0]<<" "<<Sb.v[1]<<" "<<Sb.v[2]<<std::endl;
            for(int j=0; j<numPts; j++)
            {

                float alp = (float)(rand() % 50) / 100.0f;
                float bet = (float)(rand() % 50) / 100.0f;
                float gam = 1.0f - alp - bet;
//                std::cout<<"numFaces = "<<numFaces<<std::endl;
                std::cout<<"f1 = "<<f1<<" f2 = "<<f2<<std::endl;
                std::cout<<"Original Barycoordinates: "<<std::endl;
                std::cout<<alp<<" "<<bet<<" "<<gam<<std::endl;
                vnl_vector <float> baryCoord(3);
                baryCoord[0] = alp;
                baryCoord[1] = bet;
                baryCoord[2] = gam;
                for(int ii=0; ii<3; ii++)
                {
                    pta[ii] = alp * (float)(this->vertices[Sa.v[0]][ii]) + bet * (float)(this->vertices[Sa.v[1]][ii]) + gam*(float)(this->vertices[Sa.v[2]][ii]);
                    ptb[ii] = alp * (float)(this->vertices[Sb.v[0]][ii]) + bet * (float)(this->vertices[Sb.v[1]][ii]) + gam*(float)(this->vertices[Sb.v[2]][ii]);
                }
                float ctheta = (pta DOT ptb) / (len(pta) * len(ptb));
                if(ctheta > 1.0f) ctheta = 1.0f;
                else if(ctheta < -1.0f) ctheta = -1.0f;

                float gth = std::acos(ctheta) * this->bsphere.r;
                //              std::cout<<"check1"<<std::endl;

                float valApprox = this->GetBronsteinGeodesicDistance(pta, ptb, str3);

//                float valApprox = this->GetBronsteinGeodesicDistance(pta, ptb, str3, Sa, Sb, baryCoord, baryCoord); %debugging
                //              std::cout<<"check2"<<std::endl;
                float valBary = this->GetBronsteinGeodesicDistance(pta, ptb, str2);//, Sa, Sb, baryCoord, baryCoord);

//                float valNewton = this->GetBronsteinGeodesicDistance(pta, ptb, str1, Sa, Sb, baryCoord, baryCoord);
                myfile<<f1<<"\t"<<f2<<"\t"<<alp<<"\t"<<bet<<"\t"<<gam<<"\t"<<gth<<"\t"<<valApprox<<"\t"<<valBary<<"\n";
                std::cout<<"gth: "<<gth<<"\t"<<"3PtApprox: "<<valApprox<<"\tBary: "<<valBary<<"\n";
            }
        }
        myfile.close();
        return 1.0f;

    }

    /* Praful */
    float TestApproxGeodesic(int f1, int f2)
    {
        Face Sa = this->faces[f1];
        Face Sb = this->faces[f2];
        point pta, ptb;
        for(int i=0; i<3; i++)
        {
            pta[i] = 0.33f*this->vertices[Sa.v[0]][i] + 0.33f*this->vertices[Sa.v[1]][i] + 0.34f*this->vertices[Sa.v[2]][i];
            ptb[i] = 0.33f*this->vertices[Sb.v[0]][i] + 0.33f*this->vertices[Sb.v[1]][i] + 0.34f*this->vertices[Sb.v[2]][i];
        }
        vnl_vector <float> baryCoord(3,0.33);
        float ctheta = (pta DOT ptb) / (len(pta) * len(ptb));
        if(ctheta > 1.0f) ctheta = 1.0f;
        else if(ctheta < -1.0f) ctheta = -1.0f;

        char method[] = "Newton";
        float gth = std::acos(ctheta) * this->bsphere.r;
        float val = this->GetBronsteinGeodesicDistance(pta, ptb, method);//, Sa, Sb, baryCoord, baryCoord);
        std::cout<<"**************"<<std::endl;
        std::cout<<"Approx value = "<<val<<std::endl;
        std::cout<<"Gth value = "<<gth<<std::endl;
        std::cout<<"**************"<<std::endl;

        return val;
    }

    /* Praful */
    float GetBronsteinGeodesicDistance(point a, point b, char* method)//, Face Sa, Face Sb, vnl_vector <float> baryCoord_a, vnl_vector <float> baryCoord_b)
    {
        Face Sa, Sb;
        vnl_vector <float> baryCoord_a(3), baryCoord_b(3);
        float alp_a, alp_b, bet_a, bet_b, gam_a, gam_b;
        GetTriangleInfoForPoint(a, Sa, alp_a, bet_a, gam_a);
        GetTriangleInfoForPoint(b, Sb, alp_b, bet_b, gam_b);

        float dGeo_a_2_b = GetBronsteinGeodesicDistance(Sa, Sb, baryCoord_a, baryCoord_b, method);
        return dGeo_a_2_b;
    }

    /* Praful */
    float GetBronsteinGeodesicDistance( Face Sa, Face Sb, vnl_vector <float> baryCoord_a, vnl_vector <float> baryCoord_b, char* method)
    {
        point a; a.clear();
        point b; b.clear();
        for (int d1 = 0; d1 < 3; d1++)
        {
            a[d1] = 0.0;
            b[d1] = 0.0;
            for (int d2 = 0; d2 < 3; d2++)
            {
                point vt = vertices[Sa.v[d2]];
                a[d1] += baryCoord_a[d2]*vt[d1];
                point vt2 = vertices[Sb.v[d2]];
                b[d1] += baryCoord_b[d2]*vt2[d1];
            }
        }

        float alp_a, alp_b, bet_a, bet_b, gam_a, gam_b;
        alp_a = baryCoord_a[0];
        bet_a = baryCoord_a[1];
        gam_a = baryCoord_a[2];
        alp_b = baryCoord_b[0];
        bet_b = baryCoord_b[1];
        gam_b = baryCoord_b[2];

        if (alp_a<0.000001f)
        {
            alp_a=0.000001f;
        }
        if (bet_a<0.000001f)
        {
            bet_a=0.000001f;
        }
        if (gam_a<0.000001f)
        {
            gam_a=0.000001f;
        }
        if (alp_b<0.000001f)
        {
            alp_b=0.000001f;
        }
        if (bet_b<0.000001f)
        {
            bet_b=0.000001f;
        }
        if (gam_b<0.000001f)
        {
            gam_b=0.000001f;
        }

        alp_a /= (alp_a + bet_a + gam_a);
        bet_a /= (alp_a + bet_a + gam_a);
        gam_a /= (alp_a + bet_a + gam_a);

        alp_b /= (alp_b + bet_b + gam_b);
        bet_b /= (alp_b + bet_b + gam_b);
        gam_b /= (alp_b + bet_b + gam_b);

        baryCoord_a[0]=alp_a;
        baryCoord_a[1]=bet_a;
        baryCoord_a[2]=gam_a;
        baryCoord_b[0]=alp_b;
        baryCoord_b[1]=bet_b;
        baryCoord_b[2]=gam_b;
        vnl_vector<float> xA(2);
        vnl_vector<float> xB(2);
        vnl_matrix<float> Xa(2,3);
        vnl_matrix<float> Xb(2,3);
        if(baryCoord_a.max_value() >1.0f || baryCoord_a.min_value()<0.0f || baryCoord_b.max_value() >1.0f || baryCoord_b.min_value()<0.0f)
        {
            std::cerr<<"incorrect barycentric coordinates...!!"<<std::endl;
            vcl_cerr<<"baryCoord_a: "<<baryCoord_a<<std::endl;
            vcl_cerr<<"baryCoord_b: "<<baryCoord_b<<std::endl;
            return EXIT_FAILURE;
        }

        ComputeCanonicalForm(a, xA, Xa);
        ComputeCanonicalForm(b, xB, Xb);

        vnl_matrix<float> dA_2_B(3,3);
        bool tooFar = false;
        for(int i=0; i<3; i++)
        {
            for(int j=0; j<3; j++)
            {
                dA_2_B(i,j) = this->GetGeodesicDistance(Sa.v[i], Sb.v[j]);

                // SHIREEN: if triangles are too far, don't bother to complete
                if ( dA_2_B(i,j) == LARGENUM)
                {
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
        for(int vertB_id=0; vertB_id<3; vertB_id++)
            geo_approx_2_B[vertB_id] = ComputeThreePointApproximatedGeodesic(xA, baryCoord_a, Xa, dA_2_B.get_column(vertB_id), method);

        float dGeo_a_2_b=0.0f;
        dGeo_a_2_b = ComputeThreePointApproximatedGeodesic(xB, baryCoord_b, Xb, geo_approx_2_B, method);

        return dGeo_a_2_b;
    }


    /* Praful */
    float ComputeGradient(vnl_vector<float> x0, vnl_vector<float> baryCoord, vnl_matrix<float> X, vnl_vector<float> ds, vnl_vector<float> & G)
    {
        G = vnl_vector<float>(2, 0.0f);
        for(int k=0; k<2; k++)
        {
            for(int ii=0; ii<3; ii++)
            {
                vnl_vector<float> xi = X.get_column(ii);
                vnl_vector<float> tmp = x0 - xi;
                float residual = dot_product(tmp,tmp) - ds[ii]*ds[ii];
                G[k] += 4*baryCoord[ii]*residual*tmp[k];
            }
        }
        return 1.0f;
    }

    /* Praful */
    float ComputeHessian(vnl_vector<float> x0, vnl_vector<float> baryCoord, vnl_matrix<float> X, vnl_vector<float> ds, vnl_matrix<float> & H )
    {
        H = vnl_matrix<float>(2,2,0.0f);
        for(int k=0; k<2; k++)
        {
            for(int kp=0; kp<2; kp++)
            {
                for(int ii=0; ii<3; ii++)
                {
                    vnl_vector<float> xi = X.get_column(ii);
                    vnl_vector<float> tmp = x0 - xi;
                    float residual = dot_product(tmp,tmp) - ds[ii]*ds[ii];
                    if(k==kp)
                    {
                        H(k,k) += 4*baryCoord[ii]*(residual + 2*tmp[k]*tmp[k]);
                    }
                    else
                    {
                        H(k,kp) += 8*baryCoord[ii]*tmp[k]*tmp[kp];
                    }
                }
            }
        }
        return 1.0f;
    }

    /* Praful */
    float ComputeThreePointApproximatedGeodesic(vnl_vector<float> x, vnl_vector<float> baryCoord, vnl_matrix<float> X, vnl_vector<float> ds, char* method)
    {

        float geo_approx = -1.0f;
        vnl_vector<float> x0;
//                std::cout<<"check4"<<std::endl;
        float n = GetVirtualSource(baryCoord, X, ds, x0);
//                std::cout<<"check5"<<std::endl;
        char check2[] = "Bary";
        if(n==-1.0f || strcmp(method, check2)==0)
        {
//            std::cout<<"Using Bary..."<<std::endl;
            geo_approx = dot_product(baryCoord, ds);
        }
        else
        {
            char check1[] = "Newton";
            if(strcmp(method, check1)==0) //Newton method
            {
//                                std::cout<<"Using Newton iterations..."<<std::endl;
                //                vcl_cout<<"Initial x0= "<<x0<<std::endl;
                float eta = 1.0f;
                for(int iter=0; iter<10; iter++)
                {
                    vnl_matrix<float> H;
                    vnl_vector<float> G;
                    ComputeGradient(x0, baryCoord, X, ds, G);

                    ComputeHessian(x0, baryCoord, X, ds, H);
                    vnl_matrix<float> Hinv = vnl_matrix_inverse<float>(H);
                    x0 -= eta*Hinv*G;
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
                for(int i = 0; i<m; i++)
                {
                    vnl_vector<float> xi = X.get_column(i);
//                    vcl_cout<<"xi: "<<std::endl<<xi<<std::endl;
                    for(int j = 0; j<n; j++)
                    {
                        J(i,j)=2.0f * (float) (std::sqrt(baryCoord[i])) * (x0[j]-xi[j]);
                    }
                }
//                vcl_cout<<"J: "<<std::endl<<J.extract(m,n,0,0)<<std::endl;

                // computing function values given the current guess
                vnl_vector<float> f(m, 0.0f);
                for(int i=0; i<m; i++)
                {
                    vnl_vector<float> xi = X.get_column(i);
                    float di = ds[i];
                    vnl_vector<float> x0_m_xi;
                    x0_m_xi = x0 - xi;
                    float r_i = dot_product(x0_m_xi, x0_m_xi) - di*di;
                    f[i] = (float) (std::sqrt(baryCoord[i])) * r_i;
                }
                float F;
                F = dot_product(f,f);
                F = 0.5f*F;
                vnl_matrix<float> A(n,n,0.0f);
                A = J.transpose()*J;
                vnl_vector<float> g(n,0.0f);
                g = J.transpose()*f;

                vnl_vector<float> diagA = A.get_diagonal();
                float max_diagA = diagA.max_value();
                float mu = tau * max_diagA;
                float norm_g = g.two_norm();

                vnl_matrix<float> muId(n,n,0.0f);
                vnl_matrix<float> A_mu(n,n,0.0f);
                vnl_matrix<float> A_mu_inv;
                vnl_vector<float> hlm(n,0.0f);
                vnl_vector<float> xnew(n,0.0f);
                vnl_vector<float> fnew(m, 0.0f);
                float Fnew=0.0f, delta_L=0.0f, rho=0.0f;
//                std::cout<<"****************"<<std::endl;
                bool found = norm_g <= eps1;
                while(!found && k<kmax)
                {
                    k = k + 1.0f;

                    muId.set_identity();
                    muId = mu*muId;

                    A_mu = A + muId;
//                    std::cout<<"check4"<<std::endl;
//                    vcl_cout<<"A: "<<std::endl<<A.extract(n,n,0,0)<<std::endl;
//                    std::cout<<"mu: "<<mu<<std::endl;
//                    vcl_cout<<"A_mu: "<<std::endl<<A_mu.extract(n,n,0,0)<<std::endl;
                    A_mu_inv = vnl_matrix_inverse<float>(A_mu);
//                    std::cout<<"check51"<<std::endl;
//                    vcl_cout<<"A_mu_inv: "<<std::endl<<A_mu_inv.extract(n,n,0,0)<<std::endl;
                    A_mu_inv = -1.0f*A_mu_inv;
//                    vcl_cout<<"A_mu_inv: "<<std::endl<<A_mu_inv.extract(n,n,0,0)<<std::endl;
                    hlm = A_mu_inv*g;
                    float norm_hlm = hlm.two_norm();
                    float norm_x0 = x0.two_norm();

                    if(norm_hlm <= (eps1 * (norm_x0 + eps2)))
                    {
                        found = true;
                    }
                    else
                    {
                        xnew = x0 + hlm;
                        for(int i = 0; i<m ; i++)
                        {
                            vnl_vector<float> xi = X.get_column(i);
                            float di = ds[i];
                            vnl_vector<float> x_m_xi;
                            x_m_xi = xnew - xi;
                            float r_i = dot_product(x_m_xi, x_m_xi) - di*di;
                            fnew[i] = (float) (std::sqrt(baryCoord[i])) * r_i;
                        }
                        Fnew = dot_product(fnew,fnew);
                        Fnew = 0.5f*Fnew;

                        delta_L = 0.5f*dot_product(hlm, (mu*hlm-g));
                        rho = (F-Fnew)/delta_L;

                        if(rho>0.0f)
                        {
                            x0 = xnew;

                            // computing Jacobian
                            for(int i = 0; i<m; i++)
                            {
                                vnl_vector<float> xi = X.get_column(i);
                                for(int j = 0; j<n; j++)
                                {
                                    J(i,j)=2.0f * (float) (std::sqrt(baryCoord[i])) * (x0[j]-xi[j]);
                                }
                            }

                            // computing function values given the current guess
                            for(int i=0; i<m; i++)
                            {
                                vnl_vector<float> xi = X.get_column(i);
                                float di = ds[i];
                                vnl_vector<float> x0_m_xi;
                                x0_m_xi = x0 - xi;
                                float r_i = dot_product(x0_m_xi, x0_m_xi) - di*di;
                                f[i] = (float) (std::sqrt(baryCoord[i])) * r_i;
                            }

                            F = dot_product(f,f);
                            F = 0.5f*F;
                            A = J.transpose()*J;
                            g = J.transpose()*f;
                            norm_g = g.two_norm();
                            found = norm_g <= eps1;
//                            std::cout<<"=================="<<std::endl;
//                            std::cout<<"mu= "<<mu<<std::endl;
//                            std::cout<<"=================="<<std::endl;
                            float cmp1 = 1.0f - (2.0f*rho - 1.0f)*(2.0f*rho - 1.0f)*(2.0f*rho - 1.0f);
                            if(0.3f > cmp1)
                            {
                                mu = mu*0.3f;
                            }
                            else
                            {
                                mu = mu*cmp1;
                            }
//                            std::cout<<"=================="<<std::endl;
//                            std::cout<<"cmp1= "<<cmp1<<" mu= "<<mu<<std::endl;
//                            std::cout<<"=================="<<std::endl;
                            v = 2.0f;

                        }
                        else
                        {
                            mu = mu*v;
                            v = 2.0f*v;
                        }
                    }
                }
//                vcl_cout<<x0<<std::endl;
            }
            geo_approx = (x0-x).two_norm();
        } //end else xinit not empty
//        std::cout<<"Returning geo_approx..."<<geo_approx<<std::endl;
        return geo_approx;

    }

    /* Praful */
    float GetVirtualSource(vnl_vector<float> baryCoord, vnl_matrix<float> X, vnl_vector<float> ds, vnl_vector< float > & x0)
    {
//        vcl_cout<<"X:"<<std::endl<<X.extract(2,3,0,0);
//        vcl_cout<<"ds: "<<ds<<std::endl;
        vgl_homg_point_2d<float>  centre1(X(0,0), X(1,0),1);
        vgl_homg_point_2d<float>  centre2(X(0,1), X(1,1),1);
        vgl_homg_point_2d<float>  centre3(X(0,2), X(1,2),1);
        vgl_conic<float> circle1(centre1, ds[0], ds[0], 0.0f);
        vgl_conic<float> circle2(centre2, ds[1], ds[1], 0.0f);
        vgl_conic<float> circle3(centre3, ds[2], ds[2], 0.0f);

//        vcl_cout<<"Circle1: "<<circle1<<std::endl;
//        vcl_cout<<"Circle2: "<<circle2<<std::endl;
//        vcl_cout<<"Circle3: "<<circle3<<std::endl;
        vcl_list<vgl_homg_point_2d<float> > pts1;
        pts1 = vgl_homg_operators_2d<float>::intersection(circle1, circle2);
        int n1 = (int) (pts1.size());

        vcl_list<vgl_homg_point_2d<float> > pts2;
        pts2 = vgl_homg_operators_2d<float>::intersection(circle2, circle3);
        int n2 = (int) (pts2.size());

        vcl_list<vgl_homg_point_2d<float> > pts3;
        pts3 = vgl_homg_operators_2d<float>::intersection(circle1, circle3);
        int n3 = (int) (pts3.size());

        int n = n1+n2+n3;
//        std::cout<<"n= "<<n<<std::endl;
        if(n==0)
        {
            x0 = vnl_vector<float>(2,-1.0f);
            return -1.0f;
        }
        else
        {
            vnl_matrix< float > xinit(2,n,0);
            int i=0;
            typedef vcl_list< vgl_homg_point_2d < float > > container;
            vgl_homg_point_2d<float> temp;
            for(container::iterator p = pts1.begin(); p!=pts1.end(); p++)
            {
//                std::cout<<"n1 = "<<n1<<std::endl;
                temp = *p;
                if (!std::isfinite(temp.x()) || !std::isfinite(temp.y()) || !std::isfinite(temp.w())) continue;
//                std::cout<<"x: "<<temp.x()<<" y: "<<temp.y()<<" w: "<<temp.w()<<std::endl;
                xinit(0,i)=temp.x()/temp.w();
                xinit(1,i)=temp.y()/temp.w();
//                vcl_cout<<"i= "<<i<<" xinit(i)="<<xinit.get_column(i)<<std::endl;
                i++;
            }
            for(container::iterator p = pts2.begin(); p!=pts2.end(); p++)
            {
//                std::cout<<"n2 = "<<n2<<std::endl;
                temp = *p;
                if (!std::isfinite(temp.x()) || !std::isfinite(temp.y()) || !std::isfinite(temp.w())) continue;
//                std::cout<<"x: "<<temp.x()<<" y: "<<temp.y()<<" w: "<<temp.w()<<std::endl;
                xinit(0,i)=temp.x()/temp.w();
                xinit(1,i)=temp.y()/temp.w();
//                vcl_cout<<"i= "<<i<<" xinit(i)="<<xinit.get_column(i)<<std::endl;
                i++;
            }
            for(container::iterator p = pts3.begin(); p!=pts3.end(); p++)
            {
//                std::cout<<"n3 = "<<n3<<std::endl;
                temp = *p;
                if (!std::isfinite(temp.x()) || !std::isfinite(temp.y()) || !std::isfinite(temp.w())) continue;
//                std::cout<<"x: "<<temp.x()<<" y: "<<temp.y()<<" w: "<<temp.w()<<std::endl;
                xinit(0,i)=temp.x()/temp.w();
                xinit(1,i)=temp.y()/temp.w();
//                vcl_cout<<"i= "<<i<<" xinit(i)="<<xinit.get_column(i)<<std::endl;
                i++;
            }
            if (i==0)
            {
                x0 = vnl_vector<float>(2,-1.0f);
                return -1.0f;
            }
//            vcl_cout<<"xinit:"<<std::endl<<xinit.extract(2,n,0,0)<<std::endl;
//            vcl_cout<<"xinit:"<<std::endl<<xinit.extract(2,i,0,0)<<std::endl;
            double minE = 10000000000.0;
            int flag = 0;
            int winner = -1;
            for(int i1=0; i1<i; i1++)
            {
                double energy = 0.0;
                vnl_vector<float> pt = xinit.get_column(i1);
//                                vcl_cout<<"pt= "<<pt<<std::endl;
                for(int j=0; j<3; j++)
                {
                    vnl_vector<float> tmp1 = pt - X.get_column(j);
                    float residual = std::abs(tmp1.squared_magnitude() - ds[j]*ds[j]); //write the dot product in place of tmp1.*tmp1
                    energy += (double)(residual*baryCoord[j]);
//                    float residual = tmp1.squared_magnitude() - ds[j]*ds[j]; //write the dot product in place of tmp1.*tmp1
//                    energy += (double)(residual*residual*baryCoord[j]);
                }
//                                std::cout<<"Energy: "<<energy<<std::endl;
                if(flag==0)
                {
                    minE = energy;
                    winner = i1;
                    flag = 1;
                }
                else
                {
                    if(energy < minE)
                    {
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

    /* Praful */
    float ComputeCanonicalForm(point s, vnl_vector<float> & x, vnl_matrix<float> & X)//, Face S)
    {
        Face S;
        float alpS, betS, gamS;
        GetTriangleInfoForPoint(s, S, alpS, betS, gamS);

        vnl_matrix<float> S_(3,3);
        vnl_vector<float> muS(3,0);
        for(int i = 0; i < 3; i++) {
            point vertex = this->vertices[ S.v[i] ];
            for(int j = 0; j < 3; j++) S_(i,j) = (float)(vertex[j]);
        }
//        std::cout<<"*****************"<<std::endl;
//        vcl_cout<<"Face: "<<std::endl<<S_.extract(3,3,0,0)<<std::endl;
//        std::cout<<"*****************"<<std::endl;
        S_ = S_.transpose();
//        std::cout<<"*****************"<<std::endl;
//        vcl_cout<<"Transposed: "<<std::endl<<S_.extract(3,3,0,0)<<std::endl;
//        std::cout<<"*****************"<<std::endl;
        for(int r = 0; r < 3; r++) {
            for(int c = 0; c < 3; c++) muS[r] += S_(r,c);
            muS[r] /= 3.0f;
        }
//        std::cout<<"*****************"<<std::endl;
//        vcl_cout<<"muS: "<<std::endl<<muS<<std::endl;
//        std::cout<<"*****************"<<std::endl;
        // Scent = S - muS
        vnl_matrix<float> Scent(3,3);
        for(int r = 0; r < 3; r++) {
            for(int c = 0; c < 3; c++) Scent(r,c) = S_(r,c) - muS[r];
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
        vnl_matrix<float> U(3,2);
        for(int r = 0; r < 3; r++) {
            for(int c = 0; c < 2; c++) U(r,c) = U_(r,c);
        }
//        std::cout<<"............................"<<std::endl;
//        vcl_cout<<"U: "<<U.extract(2,3,0,0)<<std::endl;
//        std::cout<<"............................"<<std::endl;
        /*vnl_matrix<point::value_type>*/ X = U.transpose() * Scent;

        vnl_vector<float> sCent(3);
        for(int c = 0; c < 3; c++) sCent[c] = s[c] - muS[c];

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

    // SHIREEN
    void GetPointTriangleVertices(point x, point & v1, point & v2, point & v3)
    {
        Face triangleX;
        float alphaX, betaX, gammaX;

        GetTriangleInfoForPoint(x,  triangleX, alphaX,  betaX,  gammaX);

        v1[0] = this->vertices[triangleX.v[0]][0];
        v1[1] = this->vertices[triangleX.v[0]][1];
        v1[2] = this->vertices[triangleX.v[0]][2];

        v2[0] = this->vertices[triangleX.v[1]][0];
        v2[1] = this->vertices[triangleX.v[1]][1];
        v2[2] = this->vertices[triangleX.v[1]][2];

        v3[0] = this->vertices[triangleX.v[2]][0];
        v3[1] = this->vertices[triangleX.v[2]][1];
        v3[2] = this->vertices[triangleX.v[2]][2];
    }

    void GetPointTriangleVertices(point x, point & v1, point & v2, point & v3, ivec3 & vids)
    {
        Face triangleX;
        float alphaX, betaX, gammaX;

        GetTriangleInfoForPoint(x, triangleX, alphaX,  betaX, gammaX);

        v1[0] = this->vertices[triangleX.v[0]][0];
        v1[1] = this->vertices[triangleX.v[0]][1];
        v1[2] = this->vertices[triangleX.v[0]][2];

        v2[0] = this->vertices[triangleX.v[1]][0];
        v2[1] = this->vertices[triangleX.v[1]][1];
        v2[2] = this->vertices[triangleX.v[1]][2];

        v3[0] = this->vertices[triangleX.v[2]][0];
        v3[1] = this->vertices[triangleX.v[2]][1];
        v3[2] = this->vertices[triangleX.v[2]][2];

        vids[0] = triangleX.v[0];
        vids[1] = triangleX.v[1];
        vids[2] = triangleX.v[2];
    }

    void GetPointBarycentricCoordinates(point x, float& alphaX, float& betaX, float& gammaX)
    {
        Face triangleX;
        GetTriangleInfoForPoint(x,  triangleX, alphaX,  betaX,  gammaX);
    }
    // END-SHIREEN

    /* Prateep */
    void saveFidsDistanceMap(itk::Image<int,3>::Pointer fidsVolume, std::string prefix, double r = 1.0)
    {
        itk::Image<PixelType,3>::Pointer distMap = itk::Image<PixelType,3>::New();
        distMap->SetRegions( fidsVolume->GetLargestPossibleRegion() );
        distMap->Allocate();
        distMap->SetOrigin( fidsVolume->GetOrigin() );
        distMap->SetSpacing( fidsVolume->GetSpacing() );
        distMap->SetDirection( fidsVolume->GetDirection() );

        itk::ImageRegionConstIteratorWithIndex< itk::Image<int,3> > fidsVolumeIt(fidsVolume, fidsVolume->GetLargestPossibleRegion() );
        itk::ImageRegionIterator< itk::Image<PixelType,3> > distMapIt(distMap, distMap->GetLargestPossibleRegion() );

        fidsVolumeIt.GoToBegin();
        distMapIt.GoToBegin();

        while(!fidsVolumeIt.IsAtEnd()) {
            int fid = fidsVolumeIt.Get();

            if(fid == -1) {
                distMapIt.Set(-1.0);
            } else {
                itk::Image<int,3>::IndexType ind = fidsVolumeIt.GetIndex();
                itk::Image<int,3>::PointType pp;
                fidsVolume->TransformIndexToPhysicalPoint(ind, pp);
                point p, junk;
                for(int ii = 0; ii < 3; ii++) p[ii] = pp[ii];
                double d = this->pointTriangleDistance(p, this->faces[fid], junk);
                distMapIt.Set((float) d);
            }

            ++fidsVolumeIt;
            ++distMapIt;
        }

        itk::Image<PixelType,3>::Pointer origDistMap = itk::Image<PixelType,3>::New();
        resampleDistanceMap(distMap, origDistMap);

        itk::ImageFileWriter< itk::Image<PixelType, 3> >::Pointer w =
                itk::ImageFileWriter< itk::Image<PixelType, 3> >::New();
        std::stringstream ss; ss << r;
        std::string f = prefix + ".DistMap_r" + ss.str() + ".nrrd";
        w->SetFileName( f.c_str() );
        w->SetInput( origDistMap );
        w->Update();
    }

    /* Prateep */
    // shireen: provide the approximate distance transform to fix leaking artifacts that might be caused by sharp corners or irregular triangulation
    //void saveFidsSignedDistanceMap(itk::Image<int,3>::Pointer fidsVolume, itk::Image<PixelType, 3>::Pointer scaledDT, std::string prefix, std::string suffix = "", double r = 1.0)
    void saveFidsSignedDistanceMap(itk::Image<int,3>::Pointer fidsVolume, std::string prefix, std::string suffix = "", double r = 1.0)
    {
        if(normals.empty()) this->need_normals();
        //        std::string file = prefix + ".normals.ply";
        //        this->write(file.c_str() );

        itk::Image<PixelType,3>::Pointer distMap = itk::Image<PixelType,3>::New();
        distMap->SetRegions( fidsVolume->GetLargestPossibleRegion() );
        distMap->Allocate();
        distMap->SetOrigin( fidsVolume->GetOrigin() );
        distMap->SetSpacing( fidsVolume->GetSpacing() );
        distMap->SetDirection( fidsVolume->GetDirection() );

        itk::Image<PixelType,3>::PointType origin    = fidsVolume->GetOrigin();
        itk::Image<PixelType,3>::SpacingType spacing = fidsVolume->GetSpacing();

        float m_origin[3];
        m_origin[0] = origin[0];
        m_origin[1] = origin[1];
        m_origin[2] = origin[2];

        float m_spacing[3];
        m_spacing[0] = spacing[0];
        m_spacing[1] = spacing[1];
        m_spacing[2] = spacing[2];

        itk::ImageRegionConstIteratorWithIndex< itk::Image<int,3> > fidsVolumeIt(fidsVolume, fidsVolume->GetLargestPossibleRegion() );
        itk::ImageRegionIterator< itk::Image<PixelType,3> > distMapIt(distMap, distMap->GetLargestPossibleRegion() );

        // shireen
        //itk::ImageRegionIterator< itk::Image<PixelType,3> > approxDistMapIt(scaledDT, scaledDT->GetLargestPossibleRegion() );

        fidsVolumeIt.GoToBegin();
        distMapIt.GoToBegin();
        //approxDistMapIt.GoToBegin();

        point tstp;

        while(!fidsVolumeIt.IsAtEnd()) {
            int fid = fidsVolumeIt.Get();
            double sign;
            double d;

            if(fid == -1) {
                //                distMapIt.Set(-1.0);
                /* Prateep :
                 *   - fix DT to get distance values for every voxel.
                 */
                itk::Image<int,3>::IndexType ind_subv = fidsVolumeIt.GetIndex();
                VoxelIndexType _ind_subv[3];
                _ind_subv[0] =  ind_subv[0];
                _ind_subv[1] =  ind_subv[1];
                _ind_subv[2] =  ind_subv[2];

                point p = this->indexToPhysicalPoint(_ind_subv, m_origin, m_spacing), j;
                tstp = p;

                float alphaP, betaP, gammaP;
                Face triangleP;

                int fidP = this->GetTriangleInfoForPoint(p, triangleP, alphaP, betaP, gammaP );
                d = this->pointTriangleDistance( p, this->faces[fidP], j);

                // (a) get face normal
                vec v0 = this->vertices[this->faces[fidP].v[0]];
                vec nv0 = this->normals[this->faces[fidP].v[0]];
                vec v1 = this->vertices[this->faces[fidP].v[1]];
                vec v2 = this->vertices[this->faces[fidP].v[2]];
                vec facenormal = (v1 - v0) CROSS (v2 - v0);
                float dot1 = facenormal DOT (nv0);
                if(dot1 < 0.0f ) facenormal = -facenormal;
                // (b) get sign
                vec c = (v0 + v1 + v2); c /= 3.f;
                float dot2 = (c - p) DOT facenormal;

                if(dot2 < 0.0f) sign = -1.0f;
                else sign = 1.0f;

            } else {
                itk::Image<int,3>::IndexType ind = fidsVolumeIt.GetIndex();
                itk::Image<int,3>::PointType pp;
                fidsVolume->TransformIndexToPhysicalPoint(ind, pp);
                point p, j;
                for(int ii = 0; ii < 3; ii++) p[ii] = pp[ii];
                d = this->pointTriangleDistance(p, this->faces[fid], j);
                tstp = p;

                // (a) get face normal
                vec v0 = this->vertices[this->faces[fid].v[0]];
                vec nv0 = this->normals[this->faces[fid].v[0]];
                vec v1 = this->vertices[this->faces[fid].v[1]];
                vec v2 = this->vertices[this->faces[fid].v[2]];
                vec facenormal = (v1 - v0) CROSS (v2 - v0);
                float dot1 = facenormal DOT (nv0);
                if(dot1 < 0.0f ) facenormal = -facenormal;
                // (b) get sign
                vec c = (v0 + v1 + v2); c /= 3.f;
                float dot2 = (c - p) DOT facenormal;

                if(dot2 < 0.0f) sign = -1.0f;
                else sign = 1.0f;

            }

            //            if(std::fabs(sign*d + 1.0f) < EPS) {
            //                std::cout << "bug : " << tstp[0] << ' ' << tstp[1] << ' ' << tstp[2] << std::endl;
            //            }

            distMapIt.Set((float)(sign*d));
            //            // shireen
            //            PixelType approx_d = approxDistMapIt.Get();
            //            double approx_sign;

            //            if(approx_d < 0.0f) approx_sign = -1.0f;
            //            else approx_sign = 1.0f;

            //            if (approx_sign == sign)
            //                distMapIt.Set((float)(sign*d));
            //            else
            //            {
            //                std::cout << "WARNING: sign doesn't match for fid = " << fid << ", using approximate distance ..." << std::endl;
            //                distMapIt.Set((float)(approx_d));
            //            }

            ++fidsVolumeIt;
            ++distMapIt;
            //++approxDistMapIt;
        }

        itk::Image<PixelType,3>::Pointer origDistMap = itk::Image<PixelType,3>::New();
        resampleDistanceMap(distMap, origDistMap);

        itk::ImageFileWriter< itk::Image<PixelType, 3> >::Pointer w =
                itk::ImageFileWriter< itk::Image<PixelType, 3> >::New();
        //std::stringstream ss; ss << r;
        //std::stringstream sp; sp << this->imageSpacing[0];
        //std::string f = prefix + ".SignedDistMap_r" + ss.str() +  "_sp" + sp.str() + ".nrrd";
        std::string f = prefix + ".SignedDistMap" + suffix + ".nrrd";
        w->SetFileName( f.c_str() );
        w->SetInput( origDistMap );
        w->Update();
    }

    /* Prateep */
    void resampleDistanceMap(itk::Image<PixelType,3>::Pointer img, itk::Image<PixelType,3>::Pointer output)
    {
        if(this->imageSize[0] == 0 || this->imageSize[1] == 0 || this->imageSize[2] == 0 ||
                this->imageSpacing[0] == 0.f || this->imageSpacing[1] == 0.f || this->imageSpacing[2] == 0.f)
        {
            eprintf("Error!!! invalid image dimensions for resampling\n");
            return;
        }

        typedef itk::ResampleImageFilter< itk::Image<PixelType,3>, itk::Image<PixelType,3> > ResamplerType;
        //        typedef itk::LinearInterpolateImageFunction< itk::Image<PixelType,3>, double> InterpolatorType;
        typedef itk::BSplineInterpolateImageFunction<itk::Image<PixelType,3>, double, double> InterpolatorType;
        typedef itk::IdentityTransform< double, 3> TransformType;
        TransformType::Pointer identityTransform = TransformType::New();
        identityTransform->SetIdentity();
        InterpolatorType::Pointer interpolator = InterpolatorType::New();
        interpolator->SetSplineOrder(3);
        ResamplerType::Pointer resampler = ResamplerType::New();
        resampler->SetTransform( identityTransform );
        resampler->SetInterpolator( interpolator );
        resampler->SetOutputOrigin( this->imageOrigin );
        double spacing[3] = { this->imageSpacing[0], this->imageSpacing[1], this->imageSpacing[2] };
        resampler->SetOutputSpacing( spacing);
        resampler->SetInput( img );
        resampler->SetOutputDirection( img->GetDirection() );
        itk::Size<3> size = { {this->imageSize[0], this->imageSize[1], this->imageSize[2]} };
        resampler->SetSize( size );
        resampler->Update();

        // inline DeepCopy
        output->SetRegions( resampler->GetOutput()->GetLargestPossibleRegion() );
        output->Allocate();
        output->SetOrigin( resampler->GetOutput()->GetOrigin());
        output->SetSpacing( resampler->GetOutput()->GetSpacing());
        output->SetDirection( resampler->GetOutput()->GetDirection());

        itk::ImageRegionConstIterator < itk::Image<PixelType,3> > inputIt( resampler->GetOutput(), resampler->GetOutput()->GetLargestPossibleRegion() );
        itk::ImageRegionIterator < itk::Image<PixelType,3> > outputIt(output, output->GetLargestPossibleRegion() );

        while( !inputIt.IsAtEnd() )
        {
            outputIt.Set( inputIt.Get() );
            ++inputIt;
            ++outputIt;
        }
    }

    /* Prateep */
    void saveFidsViaSuperVoxelDistanceMap(itk::Image<int,3>::Pointer fidsVolume, std::string prefix, double r = 1.0)
    {
        itk::Image<double,3>::Pointer distMap = itk::Image<double,3>::New();
        distMap->SetRegions( fidsVolume->GetLargestPossibleRegion() );
        distMap->Allocate();
        distMap->SetOrigin( fidsVolume->GetOrigin() );
        distMap->SetSpacing( fidsVolume->GetSpacing() );
        distMap->SetDirection( fidsVolume->GetDirection() );

        itk::ImageRegionConstIteratorWithIndex< itk::Image<int,3> > fidsVolumeIt(fidsVolume, fidsVolume->GetLargestPossibleRegion() );
        itk::ImageRegionIterator< itk::Image<double,3> > distMapIt(distMap, distMap->GetLargestPossibleRegion() );

        fidsVolumeIt.GoToBegin();
        distMapIt.GoToBegin();

        while(!fidsVolumeIt.IsAtEnd()) {
            int fid = fidsVolumeIt.Get();

            if(fid == -1) {
                distMapIt.Set(-1.0);
            } else {
                itk::Image<int,3>::IndexType ind = fidsVolumeIt.GetIndex();
                itk::Image<int,3>::PointType pp;
                fidsVolume->TransformIndexToPhysicalPoint(ind, pp);
                point p, junk;
                for(int ii = 0; ii < 3; ii++) p[ii] = pp[ii];
                double d = this->pointTriangleDistance(p, this->faces[fid], junk);
                distMapIt.Set(d);
            }

            ++fidsVolumeIt;
            ++distMapIt;
        }
        itk::ImageFileWriter< itk::Image<double, 3> >::Pointer w =
                itk::ImageFileWriter< itk::Image<double, 3> >::New();
        std::stringstream ss; ss << r;
        std::string f = prefix + ".fidsSV_distMap_r" + ss.str() + ".nrrd";
        w->SetFileName( f.c_str() );
        w->SetInput( distMap );
        w->Update();
    }

    /* Prateep */
    void saveFidsViaKDtreeDistanceMap(itk::Image<PixelType,3>::Pointer fidsVolume, std::string prefix)
    {
        itk::Image<PixelType,3>::Pointer distMap = itk::Image<PixelType,3>::New();
        distMap->SetRegions( fidsVolume->GetLargestPossibleRegion() );
        distMap->Allocate();
        distMap->SetOrigin( fidsVolume->GetOrigin() );
        distMap->SetSpacing( fidsVolume->GetSpacing() );
        distMap->SetDirection( fidsVolume->GetDirection() );

        itk::ImageRegionConstIteratorWithIndex< itk::Image<PixelType,3> > fidsVolumeIt(fidsVolume, fidsVolume->GetLargestPossibleRegion() );
        itk::ImageRegionIterator< itk::Image<PixelType,3> > distMapIt(distMap, distMap->GetLargestPossibleRegion() );

        fidsVolumeIt.GoToBegin();
        distMapIt.GoToBegin();

        while(!fidsVolumeIt.IsAtEnd()) {
            int fid = fidsVolumeIt.Get();

            if(fid == -1) {
                distMapIt.Set(-1.0);
            } else {
                itk::Image<int,3>::IndexType ind = fidsVolumeIt.GetIndex();
                itk::Image<int,3>::PointType pp;
                fidsVolume->TransformIndexToPhysicalPoint(ind, pp);
                point p, junk;
                for(int ii = 0; ii < 3; ii++) p[ii] = pp[ii];
                double d = this->pointTriangleDistance(p, this->faces[fid], junk);

                // (a) get face normal
                vec v0 = this->vertices[this->faces[fid].v[0]];
                vec nv0 = this->normals[this->faces[fid].v[0]];
                vec v1 = this->vertices[this->faces[fid].v[1]];
                vec v2 = this->vertices[this->faces[fid].v[2]];
                vec facenormal = (v1 - v0) CROSS (v2 - v0);
                float dot1 = facenormal DOT (nv0);
                if(dot1 < 0.0f ) facenormal = -facenormal;
                // (b) get sign
                vec c = (v0 + v1 + v2); c /= 3.f;
                float dot2 = (c - p) DOT facenormal;
                double sign;
                if(dot2 < 0.0f) sign = -1.0f;
                else sign = 1.0f;

                distMapIt.Set(sign*d);
            }

            ++fidsVolumeIt;
            ++distMapIt;
        }
        itk::Image<PixelType,3>::Pointer origDistMap = itk::Image<PixelType,3>::New();
        resampleDistanceMap(distMap, origDistMap);

        itk::ImageFileWriter< itk::Image<PixelType, 3> >::Pointer w =
                itk::ImageFileWriter< itk::Image<PixelType, 3> >::New();
        std::string f = prefix + ".fidsKD_distMap" + ".nrrd";
        w->SetFileName( f.c_str() );
        w->SetInput( origDistMap );
        w->Update();
    }

    int FindNearestVertex(point pt)
    {
        if ( !kd )
        {
            kd = new KDtree(this->vertices);
        }

        if (maxEdgeLength == 0.0)
        {
            need_maxedgelength();
        }
        const float *match = kd->closest_to_pt(pt,100000.0*sqr(maxEdgeLength)); // SHIREEN - enlargen the neighborhood size for kdtree to find a match
        int imatch = 0;
        if (!match)
        {
            std::cout << "failed to find vertex within " << maxEdgeLength << " for point " << pt << ". using vertex 0" << std::endl;
            return imatch;
        }

        imatch = (match - (const float *) &(vertices[0][0])) / 3;
        return imatch;
    }

    // SHIREEN

    float cotangent(point a, point b, point c)
    {
        // compute the cotangent of the non-degenerate triangle abc at vertex b

        vec3 ba = (vec3) (a - b);
        vec3 bc = (vec3) (c - b);

        float cot = (bc DOT ba) / (EPS + len(bc CROSS ba));

        return cot;
    }

    vec3 ComputeGeneralizedBarycentricCoordinates(point p, Face f) // suffers from numerical instability with very small faces, even worth than face area way
    {
        // this assumes that p is strictly within the given face
        // this implementation is based on the below paper to handle numerical instability that arise in case of small area triangles:
        // Meyer, Mark, Alan Barr, Haeyoung Lee, and Mathieu Desbrun. "Generalized barycentric coordinates on irregular polygons."
        // Journal of graphics tools 7, no. 1 (2002): 13-22.

        vec3 bCoords; bCoords.clear();
        point v0,v1,v2;
        v0 = this->vertices[ f.v[0] ];
        v1 = this->vertices[ f.v[1] ];
        v2 = this->vertices[ f.v[2] ];

        point n = (v1 - v0) CROSS (v2 - v0);
        normalize(n);
        float area      = ( (v1-v0) CROSS (v2-v0) ) DOT n ;

        if (area < 0.0001) // a small face
        {
            float scale = 1000.0;
            point center = (v0 + v1 + v2);
            center[0] /= 3.0; center[1] /= 3.0; center[2] /= 3.0;
            v0 = v0 - center; v0[0] *= scale; v0[1] *= scale; v0[2] *= scale;
            v1 = v1 - center; v1[0] *= scale; v1[1] *= scale; v1[2] *= scale;
            v2 = v2 - center; v2[0] *= scale; v2[1] *= scale; v2[2] *= scale;
        }

        point vcur, vprev, vnext;
        vec3 curedge;

        // the edge connecting p and v0
        vcur = v0; vprev = v2; vnext = v1;
        curedge = (vec3) (p - vcur);
        bCoords[0] = ( cotangent(p, vcur, vprev) + cotangent(p, vcur, vnext) ) / len2(curedge);

        // the edge connecting p and v1
        vcur = v1; vprev = v0; vnext = v2;
        curedge = (vec3) (p - vcur);
        bCoords[1] = ( cotangent(p, vcur, vprev) + cotangent(p, vcur, vnext) ) / len2(curedge);

        // the edge connecting p and v2
        vcur = v2; vprev = v1; vnext = v0;
        curedge = (vec3) (p - vcur);
        bCoords[2] = ( cotangent(p, vcur, vprev) + cotangent(p, vcur, vnext) ) / len2(curedge);

        float sum = bCoords.sum();
        bCoords[0] /= sum;
        bCoords[1] /= sum;
        bCoords[2] /= sum;

        return bCoords;
    }
    // end SHIREEN

    vec3 ComputeBarycentricCoordinates2(point p, Face f)
    {
        vec3 bCoords; bCoords.clear();
        point a,b,c;
        a = this->vertices[ f.v[0] ];
        b = this->vertices[ f.v[1] ];
        c = this->vertices[ f.v[2] ];

        point n = (b - a) CROSS (c - a);
        normalize(n);

        float denominator = ( (b - a) CROSS (c - a) ) DOT n;

        bCoords[0] = ( ( (c - b) CROSS (p - b) ) DOT n ) / denominator;
        bCoords[1] = ( ( (a - c) CROSS (p - c) ) DOT n ) / denominator;
        bCoords[2] = ( ( (b - a) CROSS (p - a) ) DOT n ) / denominator;

        return bCoords;
    }

    vec3 ComputeBarycentricCoordinates(point p, Face f)//, bool useGeneralized = false)
    {
        //if (useGeneralized)
        //    return ComputeGeneralizedBarycentricCoordinates(p,f);

        vec3 bCoords; bCoords.clear();
        point a,b,c;
        a = this->vertices[ f.v[0] ];
        b = this->vertices[ f.v[1] ];
        c = this->vertices[ f.v[2] ];

        point n = (b - a) CROSS (c - a);
        normalize(n);

        float area      = ( (b-a) CROSS (c-a) ) DOT n ;
        float inv_area  = 1.0f / (area + EPS);

        // shireen
        //        if (area < 0.0001) // a small face
        //        {
        //            float scale = 1000.0;
        //            point center = (a + b + c);
        //            center[0] /= 3.0; center[1] /= 3.0; center[2] /= 3.0;
        //            a = a - center; a[0] *= scale; a[1] *= scale; a[2] *= scale;
        //            b = b - center; b[0] *= scale; b[1] *= scale; b[2] *= scale;
        //            c = c - center; c[0] *= scale; c[1] *= scale; c[2] *= scale;

        //            point n = (b-a) CROSS (c-a);
        //            normalize(n);

        //            area = ( (b-a) CROSS (c-a) ) DOT n;

        //            //vec3 ab = (vec3) (b-a);
        //            //vec3 ac = (vec3) (c-a);
        //            //area     = len(ab CROSS ac)/2.0; // half the area of the parallelogram constructed by ab and ac

        //            inv_area = 1 / (area + EPS);
        //        }

        //        /* Prateep */ // this made the situation worse in case of projecting particles very near or almost on the surface
        //        vec3 pb = (vec3)(p - b); vec3 pa = (vec3)(p-a); vec3 pc = (vec3)(p-c);
        //        float normpb = len2(pb); float normpa = len2(pa); float normpc = len2(pc);
        //        float tot = (normpa + normpb + normpc);
        //        normpa = normpa / tot;
        //        normpb = normpb / tot;
        //        normpc = normpc / tot;

        //        if(IsCloseToAnEdge(p,f)) {
        //            // interpolate
        //            bCoords[0] = normpa;
        //            bCoords[1] = normpb;
        //            bCoords[2] = normpc;
        //            return bCoords;
        //        }

        // shireen
        bCoords[0] = ( ( (c - b) CROSS (p - b) ) DOT n ) * inv_area; // * areaInvPerTri[f];map <face, double> didnot work
        bCoords[1] = ( ( (a - c) CROSS (p - c) ) DOT n ) * inv_area; // * areaInvPerTri[f];map <face, double> didnot work
        bCoords[2] = ( ( (b - a) CROSS (p - a) ) DOT n ) * inv_area; // * areaInvPerTri[f];map <face, double> didnot work

        float sum = bCoords.sum();
        bCoords[0] /= sum;
        bCoords[1] /= sum;
        bCoords[2] /= sum;

        return bCoords;
    }

    //    vec3 ComputeBarycentricCoordinates(point p, Face f)
    //    {
    //        vec3 bCoords; bCoords.clear();
    //        point a,b,c;
    //        a = this->vertices[ f.v[0] ];
    //        b = this->vertices[ f.v[1] ];
    //        c = this->vertices[ f.v[2] ];

    //        point n = (b - a) CROSS (c - a);
    //        normalize(n);
    //        float denominator = ( (b - a) CROSS (c - a) ) DOT n;

    //        //        if (denominator < 0.0001) // small face
    //        //        {
    //        //            float scale = 1000.0;
    //        //            point center = (a + b + c);
    //        //            center[0] /= 3.0; center[1] /= 3.0; center[2] /= 3.0;
    //        //            a = a - center; a[0] *= scale; a[1] *= scale; a[2] *= scale;
    //        //            b = b - center; b[0] *= scale; b[1] *= scale; b[2] *= scale;
    //        //            c = c - center; c[0] *= scale; c[1] *= scale; c[2] *= scale;
    //        //        }

    //        //        denominator = ( (b - a) CROSS (c - a) ) DOT n;

    //        bCoords[0] = ( ( (c - b) CROSS (p - b) ) DOT n ) / (denominator + 1e-7);
    //        bCoords[1] = ( ( (a - c) CROSS (p - c) ) DOT n ) / (denominator + 1e-7);
    //        bCoords[2] = ( ( (b - a) CROSS (p - a) ) DOT n ) / (denominator + 1e-7);

    //        //        // Transcribed from Christer Ericson's Real-Time Collision Detection
    //        //        vec3 v0 = b - a, v1 = c - a, v2 = p - a;
    //        //        float d00 = v0 DOT v0;
    //        //        float d01 = v0 DOT v1;
    //        //        float d11 = v1 DOT v1;
    //        //        float d20 = v2 DOT v0;
    //        //        float d21 = v2 DOT v1;
    //        //        float Denom    = (d00 * d11 - d01 * d01);
    //        //        float invDenom = 1.0 / (d00 * d11 - d01 * d01);

    //        //        bCoords[1] = (d11 * d20 - d01 * d21) * invDenom;
    //        //        bCoords[2] = (d00 * d21 - d01 * d20) * invDenom;
    //        //        bCoords[0] = 1.0f - bCoords[1] - bCoords[2];

    //        return bCoords;
    //    }

    /* Prateep */
    bool IsCloseToAnEdge(point p, Face f)
    {
        point a,b,c;
        a = this->vertices[ f.v[0] ];
        b = this->vertices[ f.v[1] ];
        c = this->vertices[ f.v[2] ];

        vec3 pb = (vec3)(p - b);
        vec3 pa = (vec3)(p - a);
        vec3 pc = (vec3)(p - c);
        float norma; float normb;

        // check if p is \epsilon close to edge BC
        vec3 bc = (vec3)(c-b);
        vec3 tb = (vec3)( bc CROSS pb );
        norma = len2(tb);
        normb = len2(bc);
        if(norma < EPS * normb) {
            return true;
        }
        // check if p is \epsilon close to edge AC
        vec3 ac = (vec3)(a-c);
        vec3 tc = (vec3)( ac CROSS pc );
        norma = len2(tc);
        normb = len2(ac);
        if(norma < EPS * normb) {
            return true;
        }

        // check if p is \epsilon close to edge AB
        vec3 ab = (vec3)(b-a);
        vec3 ta = (vec3)( ab CROSS pa );
        norma = len2(ta);
        normb = len2(ab);
        if(norma < EPS * normb) {
            return true;
        }

        return false;
    }

    //    // map< face, ...> didnot work
    //    /* Prateep */
    //    void CacheFaceIds() // shireen
    //    {
    //        faceids.clear();
    //        for(unsigned int i = 0; i < this->faces.size(); i++)
    //        {
    //            Face f = this->faces[i];
    //            faceids[f] = i;
    //        }
    //    }

    //    void CacheAreaInvPerTriangle()
    //    {
    //        areaInvPerTri.clear();
    //        for(unsigned int i = 0; i < this->faces.size(); i++)
    //        {
    //            Face f = this->faces[i];
    //            point a,b,c;
    //            a = this->vertices[ f.v[0] ];
    //            b = this->vertices[ f.v[1] ];
    //            c = this->vertices[ f.v[2] ];

    //            point n = (b-a) CROSS (c-a);
    //            normalize(n);

    //            float denominator = ( (b-a) CROSS (c-a) ) DOT n;
    //            float inv_area    = 1.0f / (denominator + EPS);
    //            areaInvPerTri.push_back((double)inv_area); // shireen

    //            //            // SHIREEN
    //            //            vec3 ab = (vec3) (b-a);
    //            //            vec3 ac = (vec3) (c-a);

    //            //            float area       = len(ab CROSS ac)/2.0; // half the area of the parallelogram constructed by ab and ac
    //            //            areaInvPerTri[f] = 1.0f / (area + EPS);
    //        }
    //    }

    //    // SHIREEN
    //    void CacheAreaPerTriangle()
    //    {
    //        areaPerTri.clear();
    //        for(unsigned int i = 0; i < this->faces.size(); i++)
    //        {
    //            Face f = this->faces[i];
    //            point a,b,c;
    //            a = this->vertices[ f.v[0] ];
    //            b = this->vertices[ f.v[1] ];
    //            c = this->vertices[ f.v[2] ];

    //            //            // shireen
    //            //            vec3 ab = (vec3) (b-a);
    //            //            vec3 ac = (vec3) (c-a);

    //            //            float area    = len(ab CROSS ac)/2.0; // half the area of the parallelogram constructed by ab and ac
    //            //            areaPerTri[f] = area;


    //            point n = (b-a) CROSS (c-a);
    //            normalize(n);

    //            float denominator = EPS + (( (b-a) CROSS (c-a) ) DOT n);
    //            areaPerTri.push_back((double)denominator); // shireen
    //            //areaPerTri[f] = (double)denominator;
    //        }
    //    }
    //    // end SHIREEN

    void ReadFeatureFromFile(const char *infilename)
    {
        std::ifstream infile(infilename, std::ios::binary);
        if (!infile.is_open())
        {
            std::cerr << "File Not Found: " << infilename << std::endl;
            throw(1);
        }
        else
        {
            // read # vertices
            unsigned int numVert;
            infile.read(reinterpret_cast<char *>(&numVert), sizeof(unsigned int));

            if ( numVert != this->vertices.size() )
            {
                std::cerr << "size of feature vector does not match # vertices in mesh" << std::endl;
                throw(1);
            }
            else
            {
//                std::cout << "reading feature from file " << infilename << std::endl;

                vector< float > tmpFeatureVec;
                // loop over vertices
                for (int i = 0; i < numVert; i++)
                {
                    // read feature value
                    float value;
                    infile.read( reinterpret_cast<char *>(&value), sizeof(float) );
                    tmpFeatureVec.push_back(value);
                }

                this->features.push_back( tmpFeatureVec );
            }

            infile.close();
        }

    }

    void ReadFeatureFromList(const char *infilename)
    {
        std::ifstream infile(infilename);
        if (!infile.is_open())
        {
            std::cerr << "File Not Found" << std::endl;
            throw(1); //exit(1);
        }
        else
        {
//            std::cout << "reading feature from file " << infilename << std::endl;

            vector< float > tmpFeatureVec;
            float value;
            // loop over vertices
            while (infile)
            {
                // read feature value
                infile >> value;
                tmpFeatureVec.push_back(value);
            }
            tmpFeatureVec.pop_back();

            if ( tmpFeatureVec.size() == this->vertices.size() )
            {
                this->features.push_back( tmpFeatureVec );
            }
            else
            {
                std::cerr << "size of feature vector does not match # vertices in the mesh ! Aborting..." << std::endl;
                throw(1);//exit(1);
            }

            infile.close();
        }

    }

    /* Praful */
    void ReadFeatureGradientFromFile(const char *infilename)
    {
        std::ifstream infile(infilename);
        if (!infile.is_open())
        {
            std::cerr << "File Not Found" << std::endl;
            throw(1);//exit(1);
        }
        else
        {
            // read # vertices
            unsigned int numVert;
            infile.read(reinterpret_cast<char *>(&numVert), sizeof(unsigned int));

            if ( numVert != this->vertices.size() )
            {
                std::cerr << "size of feature vector does not match # vertices in mesh" << std::endl;
                throw(1); //exit(1);
            }
            else
            {
//                std::cout << "reading feature gradient from file " << infilename << std::endl;

                vector<point> tempFeatureGradVec;
                // loop over vertices
                for (int i = 0; i < numVert; i++)
                {
                    // read feature gradient
                    point val;
                    float value;
                    for (int j = 0; j < 3; j ++)
                    {
                        infile.read( reinterpret_cast<char *>(&value), sizeof(float) );
                        val[j] = (float) value;
                    }

                    tempFeatureGradVec.push_back(val);
                }
                this->featureGradients.push_back( tempFeatureGradVec );
            }
            infile.close();
        }
    }

    void WriteFeatureToFile(int featureIndex, const char *outfilename)
    {
        //std::ofstream outfile(outfilename, std::ios::binary);

        //// write numVertices to facilitate reading later
        //int numVert = this->vertices.size();
        //outfile.write( reinterpret_cast<char *>(&numVert), sizeof(int) );

        //// loop over each vertex
        //for (int i = 0; i < numVert; i++)
        //{
        //  // write distance to curve
        //  unsigned short value = this->(features[featureIndex])[i];
        //  outfile.write( reinterpret_cast<char *>(&value), sizeof(unsigned short) );
        //}

        //outfile.close();
    }

    /* Praful */
    void GetFeatureValues(point x, std::vector<float> & vals)
    {
        float alphaX, betaX, gammaX;
        Face triangleX;
        GetTriangleInfoForPoint(x,  triangleX, alphaX,  betaX,  gammaX);
        if (alphaX < 0.000001f)
            alphaX = 0.000001f;

        if (betaX < 0.000001f)
            betaX = 0.000001f;

        if (gammaX < 0.000001f)
            gammaX = 0.000001f;

        alphaX /= (alphaX + betaX + gammaX);
        betaX  /= (alphaX + betaX + gammaX);
        gammaX /= (alphaX + betaX + gammaX);

        vals.resize(this->GetNumberOfFeatures());
        for (unsigned int i = 0; i < this->GetNumberOfFeatures(); i++)
        {
            float f0 = this->features[i][ triangleX.v[0] ];
            float f1 = this->features[i][ triangleX.v[1] ];
            float f2 = this->features[i][ triangleX.v[2] ];

            vals[i] = (alphaX * f0) + (betaX * f1) + (gammaX * f2);
        }
    }

    /* Prateep */
    float GetFeatureValue(point x, int featureIndex)
    {
        float alphaX, betaX, gammaX;
        Face triangleX;

        GetTriangleInfoForPoint(x,  triangleX, alphaX,  betaX,  gammaX);

        // SHIREEN
        if (alphaX < 0.000001f)
            alphaX = 0.000001f;

        if (betaX < 0.000001f)
            betaX = 0.000001f;

        if (gammaX < 0.000001f)
            gammaX = 0.000001f;

        alphaX /= (alphaX + betaX + gammaX);
        betaX  /= (alphaX + betaX + gammaX);
        gammaX /= (alphaX + betaX + gammaX);
        // end SHIREEN

        // interpolate feature values on triangle face
        float f0 = this->features[featureIndex][ triangleX.v[0] ];
        float f1 = this->features[featureIndex][ triangleX.v[1] ];
        float f2 = this->features[featureIndex][ triangleX.v[2] ];

        // SHIREEN
        float featureValue = (alphaX * f0) + (betaX * f1) + (gammaX * f2);

        // HACK(01/13/2015) : Prateep ( getting -ve featureValue. Incorrect nearest face).
        //        float featureValue = (f0 + f1 + f2) / 3.0;

//        if(featureValue < 0.0f) {
//            std::cout << "bug\n";
//        }
        return featureValue;
    }

    /* Prateep -- updated Praful */
    point GetFeatureDerivative(point p, int fIndex = 0)
    {
        point dP; dP.clear();
        dP[0] = 0.0f; dP[1] = 0.0f; dP[2] = 0.0f;

        float alphaP, betaP, gammaP;
        Face triangleP;

        GetTriangleInfoForPoint(p,  triangleP, alphaP,  betaP,  gammaP);

        if (alphaP < 0.000001f)
            alphaP = 0.000001f;

        if (betaP < 0.000001f)
            betaP = 0.000001f;

        if (gammaP < 0.000001f)
            gammaP = 0.000001f;

        alphaP /= (alphaP + betaP + gammaP);
        betaP  /= (alphaP + betaP + gammaP);
        gammaP /= (alphaP + betaP + gammaP);

        // compute derivative at 3 vertices (A,B,C)
        int A = triangleP.v[0];
        int B = triangleP.v[1];
        int C = triangleP.v[2];

//        // Get derivatives of Barycentric coordinates
//        vec fNorm    = GetFaceNormal(triangleP);
//        float mag    = fNorm DOT fNorm;
//        mag          = std::sqrt(mag);
//        fNorm[0]     /= mag;
//        fNorm[1]     /= mag;
//        fNorm[2]     /= mag;

//        float fArea  = GetFaceArea(triangleP);
//        vec v0 = this->vertices[triangleP.v[0]];
//        vec v1 = this->vertices[triangleP.v[1]];
//        vec v2 = this->vertices[triangleP.v[2]];
//        vec dAlpha   = GetGradientBaryCentricCoord(fNorm, v2-v1, fArea);
//        vec dBeta    = GetGradientBaryCentricCoord(fNorm, v0-v2, fArea);
//        vec dGamma   = GetGradientBaryCentricCoord(fNorm, v1-v0, fArea);

        point dA = ComputeFeatureDerivative(A,fIndex);
        point dB = ComputeFeatureDerivative(B,fIndex);
        point dC = ComputeFeatureDerivative(C,fIndex);

//        float f0 = this->features[fIndex][A];
//        float f1 = this->features[fIndex][B];
//        float f2 = this->features[fIndex][C];

        // interpolate
        dP[0] = ( alphaP * dA[0] ) + ( betaP * dB[0] ) + ( gammaP * dC[0] );// + ( dAlpha[0] * f0 ) + ( dBeta[0] * f1 ) + ( dGamma[0] * f2 );
        dP[1] = ( alphaP * dA[1] ) + ( betaP * dB[1] ) + ( gammaP * dC[1] );// + ( dAlpha[1] * f0 ) + ( dBeta[1] * f1 ) + ( dGamma[1] * f2 );
        dP[2] = ( alphaP * dA[2] ) + ( betaP * dB[2] ) + ( gammaP * dC[2] );// + ( dAlpha[2] * f0 ) + ( dBeta[2] * f1 ) + ( dGamma[2] * f2 );

        return dP;
    }

    void GetFeatureDerivativeValues(point p, std::vector<point> & vals)
    {
        float alphaP, betaP, gammaP;
        Face triangleP;

        GetTriangleInfoForPoint(p,  triangleP, alphaP,  betaP,  gammaP);

        if (alphaP < 0.000001f)
            alphaP = 0.000001f;

        if (betaP < 0.000001f)
            betaP = 0.000001f;

        if (gammaP < 0.000001f)
            gammaP = 0.000001f;

        alphaP /= (alphaP + betaP + gammaP);
        betaP  /= (alphaP + betaP + gammaP);
        gammaP /= (alphaP + betaP + gammaP);

        // compute derivative at 3 vertices (A,B,C)
        int A = triangleP.v[0];
        int B = triangleP.v[1];
        int C = triangleP.v[2];

//        // Get derivatives of Barycentric coordinates
//        vec fNorm    = GetFaceNormal(triangleP);
//        float fArea  = GetFaceArea(triangleP);
//        vec v0 = this->vertices[triangleP.v[0]];
//        vec v1 = this->vertices[triangleP.v[1]];
//        vec v2 = this->vertices[triangleP.v[2]];
//        vec dAlpha   = GetGradientBaryCentricCoord(fNorm, v2-v1, fArea);
//        vec dBeta    = GetGradientBaryCentricCoord(fNorm, v0-v2, fArea);
//        vec dGamma   = GetGradientBaryCentricCoord(fNorm, v1-v0, fArea);

        // compute final derivatives
        vals.resize(this->GetNumberOfFeatures());
        point dP; dP.clear();
        for (unsigned int fIndex = 0; fIndex < this->GetNumberOfFeatures(); fIndex++)
        {
            point dA = ComputeFeatureDerivative(A,fIndex);
            point dB = ComputeFeatureDerivative(B,fIndex);
            point dC = ComputeFeatureDerivative(C,fIndex);

//            float f0 = this->features[fIndex][A];
//            float f1 = this->features[fIndex][B];
//            float f2 = this->features[fIndex][C];

            dP[0] = ( alphaP * dA[0] ) + ( betaP * dB[0] ) + ( gammaP * dC[0] );// + ( dAlpha[0] * f0 ) + ( dBeta[0] * f1 ) + ( dGamma[0] * f2 );
            dP[1] = ( alphaP * dA[1] ) + ( betaP * dB[1] ) + ( gammaP * dC[1] );// + ( dAlpha[1] * f0 ) + ( dBeta[1] * f1 ) + ( dGamma[1] * f2 );
            dP[2] = ( alphaP * dA[2] ) + ( betaP * dB[2] ) + ( gammaP * dC[2] );// + ( dAlpha[2] * f0 ) + ( dBeta[2] * f1 ) + ( dGamma[2] * f2 );

            vals[fIndex] = dP;
            dP.clear();
        }
    }

    /* Praful */
    vec GetGradientBaryCentricCoord(vec fNorm, vec edge, float fArea)
    {
        vec gradB  = edge CROSS fNorm;
        gradB[0] /= 2.0*fArea;
        gradB[1] /= 2.0*fArea;
        gradB[2] /= 2.0*fArea;
        return gradB;
    }

    float GetFaceArea(int fidP)
    {
        return GetFaceArea(this->faces[fidP]);
    }

    float GetFaceArea(Face fidP)
    {
        vec fn = GetFaceNormal(fidP);
        float val = fn DOT fn;
        val = 0.5*std::sqrt(val);
        return val;
    }

    vec GetFaceNormal(int fidP)
    {
        return GetFaceNormal(this->faces[fidP]);
    }

    vec GetFaceNormal(Face fidP)
    {
        vec v0 = this->vertices[fidP.v[0]];
        vec nv0 = this->normals[fidP.v[0]];
        vec v1 = this->vertices[fidP.v[1]];
        vec v2 = this->vertices[fidP.v[2]];
        vec facenormal = (v1 - v0) CROSS (v2 - v0);
        float dot1 = facenormal DOT (nv0);
        if(dot1 < 0.0f ) facenormal = -facenormal;
        return facenormal;
    }
    /* Praful */

    point ComputeFeatureDerivative(int v,int nFeature = 0)
    {
        if (featureGradients.size() > 0)
            return featureGradients[nFeature][v];
        else
        {
            point df; df.clear();
            df[0] = 0.0f; df[1] = 0.0f; df[2] = 0.0f;

            // feature value at v
            float valueV = this->features[nFeature][v];
            point ptV = this->vertices[v];

            // iterate over neighbors of v to compute derivative as central difference
            for (unsigned int n = 0; n < this->neighbors[v].size(); n++)
            {
                int indexN = this->neighbors[v][n];
                float valueN = this->features[nFeature][indexN];
                point ptN = this->vertices[indexN];

                float valueDiff = valueN - valueV;
                point ptDiff = ptN - ptV;

                df[0] = df[0] + valueDiff / (ptDiff[0] + 0.0001f);
                df[1] = df[1] + valueDiff / (ptDiff[1] + 0.0001f);
                df[2] = df[2] + valueDiff / (ptDiff[2] + 0.0001f);
            }

            df[0] = df[0] / (float) ( this->neighbors[v].size() );
            df[1] = df[1] / (float) ( this->neighbors[v].size() );
            df[2] = df[2] / (float) ( this->neighbors[v].size() );

            return df;
        }
    }

    int GetNumberOfFeatures()
    {
        return this->features.size();
    }

    // Debugging printout, controllable by a "verbose"ness parameter

    static int verbose;
    static void set_verbose(int);
    static void (*dprintf_hook)(const char *);
    static void set_dprintf_hook(void (*hook)(const char *));
    static void dprintf(const char *format, ...);

    // Same as above, but fatal-error printout
    static void (*eprintf_hook)(const char *);
    static void set_eprintf_hook(void (*hook)(const char *));
    static void eprintf(const char *format, ...);

    // Constructor
    TriMesh() : grid_width(-1), grid_height(-1), flag_curr(0), speedType(ONE), maxEdgeLength(0.0)
    {
        //iMap = &geoIndex;
        //dMap = &geoMap;
        kd = NULL;
    }
    virtual ~TriMesh(){}
};


inline const TriMesh::BBox operator + (const TriMesh::BBox &b, const point &p)
{
    return TriMesh::BBox(b) += p;
}


inline const TriMesh::BBox operator + (const point &p, const TriMesh::BBox &b)
{
    return TriMesh::BBox(b) += p;
}


inline const TriMesh::BBox operator + (const TriMesh::BBox &b1, const TriMesh::BBox &b2)
{
    return TriMesh::BBox(b1) += b2;
}

#endif

