
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "TriMesh.h"
#include "meshFIM.h"
#include "math.h"

#include "tinyxml.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <climits>

#include "itksys/SystemTools.hxx"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkReinitializeLevelSetImageFilter.h" // for distance transform computation


// VTK
#include "vtkPolyData.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "vtkSmartPointer.h"
#include "vtkContourFilter.h"
#include "vtkDecimatePro.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkPLYWriter.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataMapper.h"
#include "vtkVertexGlyphFilter.h"
#include "vtkPLYReader.h"
#include "vtkTriangleFilter.h"
#include "vtkProperty.h"
#include "vtkImageCast.h"
#include "vtkExtractSelection.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDataSetMapper.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkImageStencil.h"
#include "vtkImageData.h"
#include <vtkPointData.h>
#include "vtkTriangle.h"
#include "vtkCellArray.h"

// C++/STL
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <map>
#include <set>
#include <iterator>
#include "math.h"

// Etc
#include "tinyxml.h"
#include "vnl/vnl_vector.h"

// debug -- prateep
//#include "itkParticlePositionReader.h"
// debug -- prateep

#define _USE_MATH_DEFINES
#define DBG_SAVE_INTERMEDIATE_RESULT 0
#define DBG_USE_KDTREE 0

#define SQRT_3 1.732050807
#define DBLMAX std::numeric_limits<double>::max()

typedef  float  PixelType;
typedef itk::Image< PixelType,  3 >   ImageType;
typedef itk::Image< int,  3 >   IntImageType;
typedef itk::ImageFileReader< ImageType >  ImageReaderType;
typedef itk::ImageFileWriter< ImageType >  ImageWriterType;

typedef itk::ImageFileReader< IntImageType >  IntImageReaderType;
typedef itk::ImageFileWriter< IntImageType >  IntImageWriterType;

std::vector<int> randperm(int n)
{
    std::vector<int> perm;
    perm.clear();

    int i, j, t;

    for(i=0; i<n; i++)
        perm.push_back(i);

    for(i=0; i<n; i++) {
        j = rand()%(n-i)+i;
        t = perm[j];
        perm[j] = perm[i];
        perm[i] = t;
    }

    return perm;
}

float modulo(float n, unsigned int d)
{
    float x = 1.0f * n / d;
    int xint = (int) std::round(x);
    float res = n - 1.0f*d*xint;
    return res;
}

ImageType::Pointer VTK2ITK ( vtkImageData *vtkImage, int *dims, double *origin, double* spacing )
{
    // convert the vtk image to an itk image
    ImageType::SizeType size ;
    size[0] = dims[0] ;
    size[1] = dims[1] ;
    size[2] = dims[2] ;

    ImageType::Pointer itkImage = ImageType::New () ;

    ImageType::RegionType region;
    region.SetSize( size );
    ImageType::IndexType start;
    start[0] = start[1] = start[2] = 0;
    region.SetIndex( start );
    itkImage->SetRegions( region );
    itkImage->SetOrigin ( origin ) ;

    itkImage->SetSpacing ( spacing ) ;
    itkImage->Allocate () ;

    ImageType::IndexType index ;
    int pixel ;

    for ( index[0] = 0 ; index[0] < dims[0] ; index[0]++ )
    {
        for ( index[1] = 0 ; index[1] < dims[1] ; index[1]++ )
        {
            for ( index[2] = 0 ; index[2] < dims[2] ; index[2]++ )
            {
                //pixel = vtkImage->GetScalarComponentAsFloat ( dims[0] - 1 - index[0], dims[1] - 1 - index[1], index[2], 0 );
                pixel = vtkImage->GetScalarComponentAsFloat ( index[0], index[1], index[2], 0 ) ;
                //if ( pixel != 128 )
                //    pixel = 0 ;
                itkImage->SetPixel ( index, pixel ) ;
            }
        }
    }

    //std::cout << "VTK to ITK conversion completed." << std::endl ;
    return itkImage ;
}

ImageType::Pointer RasterizeMesh(vtkSmartPointer<vtkPolyData> polydata, double spacing[3], double origin[3], int size[3])
{
    int inval         = 1; // voxel value inside the shape (mesh)
    int outval        = 0; // voxel value outside the shape (mesh)

    vtkSmartPointer<vtkImageData> whiteImage = vtkSmartPointer<vtkImageData>::New();

    whiteImage->SetSpacing(spacing);
    whiteImage->SetDimensions(size);
    whiteImage->SetExtent(0, size[0] - 1, 0, size[1] - 1, 0, size[2] - 1);
    whiteImage->SetOrigin(origin);

#if VTK_MAJOR_VERSION <= 5
    whiteImage->SetScalarTypeToUnsignedChar();
    whiteImage->AllocateScalars();
#else
    whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR,1);
#endif
    //whiteImage->Update();

    // fill the image with foreground voxels:
    //unsigned char inval = 255;
    //unsigned char outval = 0;
    vtkIdType count = whiteImage->GetNumberOfPoints();
    for (vtkIdType i = 0; i < count; ++i)
    {
        whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
    }

    // polygonal data --> image stencil:
    vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
#if VTK_MAJOR_VERSION <= 5
    pol2stenc->SetInput(polydata);
#else
    pol2stenc->SetInputData(polydata);
#endif
    pol2stenc->SetOutputOrigin(origin);
    pol2stenc->SetOutputSpacing(spacing);
    pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
    pol2stenc->Update();

    // cut the corresponding white image and set the background:
    vtkSmartPointer<vtkImageStencil> imgstenc = vtkSmartPointer<vtkImageStencil>::New();
#if VTK_MAJOR_VERSION <= 5
    imgstenc->SetInput(whiteImage);
    imgstenc->SetStencil(pol2stenc->GetOutput());
#else
    imgstenc->SetInputData(whiteImage);
    imgstenc->SetStencilConnection(pol2stenc->GetOutputPort());
#endif
    imgstenc->ReverseStencilOff();
    imgstenc->SetBackgroundValue(outval);
    imgstenc->Update();

    ImageType::Pointer binaryImage = VTK2ITK ( imgstenc->GetOutput(), size, origin, spacing );
    //    IntImageWriterType::Pointer writer = ImageWriterType::New();
    //    writer->SetFileName(outFilenames[shapeNo].c_str());
    //    writer->SetInput(binaryImage);
    //    writer->Update();
    //    writer->Write();

    return binaryImage;

}

ImageType::Pointer ComputeApproximateDistanceTransform(ImageType::Pointer binaryImage)
{
    int AntialiasIterations = 30;

    typedef itk::BinaryThresholdImageFilter <ImageType, ImageType> ScalarThresholdImageFilterType;
    typedef itk::AntiAliasBinaryImageFilter <ImageType, ImageType> AntiAliasBinaryImageFilterType;
    typedef itk::ReinitializeLevelSetImageFilter< ImageType > ReinitializeLevelSetImageFilterType;


    typename AntiAliasBinaryImageFilterType::Pointer antiAliasFilter = AntiAliasBinaryImageFilterType::New ();
    antiAliasFilter->SetInput(binaryImage);
    antiAliasFilter->SetNumberOfIterations(AntialiasIterations);
    antiAliasFilter->SetMaximumRMSError(0.0);
    antiAliasFilter->Update();

    typename ReinitializeLevelSetImageFilterType::Pointer distanceMapImageFilter = ReinitializeLevelSetImageFilterType::New();
    distanceMapImageFilter->SetInput(antiAliasFilter->GetOutput());
    distanceMapImageFilter->NarrowBandingOff();
    distanceMapImageFilter->SetLevelSetValue(0.0);
    distanceMapImageFilter->Update();

    ImageType::Pointer dtImage  = distanceMapImageFilter->GetOutput();

    return dtImage;
}

vtkSmartPointer< vtkPolyData > TriMesh2PolyData(TriMesh * mesh)
{

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (unsigned int ii = 0 ; ii < mesh->vertices.size(); ii++)
    {
        points->InsertNextPoint(mesh->vertices[ii][0], mesh->vertices[ii][1], mesh->vertices[ii][2]);
    }

    vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
    for(unsigned int ii = 0 ; ii < mesh->faces.size(); ii++)
    {
        triangle->GetPointIds()->SetId(0, mesh->faces[ii][0]);
        triangle->GetPointIds()->SetId(1, mesh->faces[ii][1]);
        triangle->GetPointIds()->SetId(2, mesh->faces[ii][2]);
        triangles->InsertNextCell(triangle);
    }

    // Create a polydata object and add everything to it
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetPolys(triangles);

    return polydata;
}

int main(int argc, char *argv[])
{
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "GenerateBinaryAndDTImagesFromMeshes " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Compute binary images (i.e. segmentation) and their distance transforms of a given set of meshes (ply format)" << std::endl;

        std::cerr << "It uses a parameter file with the following tags" << std::endl;
        std::cerr << "\t - mesh: a list of ply file names for meshes to be processed" << std::endl;
        std::cerr << "\t - origin_x, origin_y, origin_z: the origin in physical units of the resulting distance transform" << std::endl;
        std::cerr << "\t - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform" << std::endl;
        std::cerr << "\t - size_x, size_y, size_z: the size (rows,cols,slices) of the resulting distance transform" << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;

        return EXIT_FAILURE;
    }
    
    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
    std::istringstream inputsBuffer;

    // Collect a list of point file names and output file names
    std::vector< std::string > meshFilename;

    // the resulting distance tranform (image grid) properties
    int size_x, size_y, size_z;
    float origin_x, origin_y, origin_z;
    float spacing_x, spacing_y,  spacing_z;

    std::string tmpString;
    if(loadOkay)
    {
        elem = docHandle.FirstChild( "mesh" ).Element();
        if (!elem)
        {
            std::cerr << "No mesh files have been specified" << std::endl;
            throw 1;
        }
        else
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                meshFilename.push_back(tmpString);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

        // voxel spacing in the x-direction
        elem = docHandle.FirstChild( "spacing_x" ).Element();
        if (elem)
        {
            spacing_x = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No spacing_x specified!" << std::endl;
            return EXIT_FAILURE;
        }


        // voxel spacing in the y-direction
        elem = docHandle.FirstChild( "spacing_y" ).Element();
        if (elem)
        {
            spacing_y = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No spacing_y specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // voxel spacing in the z-direction
        elem = docHandle.FirstChild( "spacing_z" ).Element();
        if (elem)
        {
            spacing_z = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No spacing_z specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume size in the x-direction
        elem = docHandle.FirstChild( "size_x" ).Element();
        if (elem)
        {
            size_x = atoi( elem->GetText() );
        }
        else
        {
            std::cerr << "No size_x specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume size in the y-direction
        elem = docHandle.FirstChild( "size_y" ).Element();
        if (elem)
        {
            size_y = atoi( elem->GetText() );
        }
        else
        {
            std::cerr << "No size_y specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume size in the z-direction
        elem = docHandle.FirstChild( "size_z" ).Element();
        if (elem)
        {
            size_z = atoi( elem->GetText() );
        }
        else
        {
            std::cerr << "No size_z specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume orign in the x-direction
        elem = docHandle.FirstChild( "origin_x" ).Element();
        if (elem)
        {
            origin_x = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No origin_x specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume orign in the y-direction
        elem = docHandle.FirstChild( "origin_y" ).Element();
        if (elem)
        {
            origin_y = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No origin_y specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // volume orign in the z-direction
        elem = docHandle.FirstChild( "origin_z" ).Element();
        if (elem)
        {
            origin_z = atof( elem->GetText() );
        }
        else
        {
            std::cerr << "No origin_z specified!" << std::endl;
            return EXIT_FAILURE;
        }
    }
    else
    {
        std::cerr << "param file can not be opened ..." << std::endl;
        throw 1;
    }

    int numShapes = meshFilename.size();
    ImageWriterType::Pointer writer = ImageWriterType::New();

    std::stringstream sp; sp << spacing_x;
    std::string suffix =  "_sp" + sp.str()  ;

    for (unsigned int shCount = 0; shCount < numShapes; shCount++)
    {

        //#if DBG_SAVE_INTERMEDIATE_RESULT || DBG_USE_KDTREE
        size_t ind = meshFilename[shCount].find_last_of('.');
        std::string prefix = meshFilename[shCount].substr(0,ind);
        std::cout << "prefix: " << prefix << std::endl;
        //#endif

        // read triangle mesh and set feature values at vertices
        TriMesh *mesh = TriMesh::read( meshFilename[shCount].c_str() );
        orient(mesh);

        if (mesh->neighbors.empty()) mesh->need_neighbors();
        if (mesh->adjacentfaces.empty()) mesh->need_adjacentfaces();
        if (mesh->normals.empty()) mesh->need_normals();
        //        if (mesh->areaInvPerTri.empty()) mesh->CacheAreaInvPerTriangle(); // Prateep

        // bounding sphere
        mesh->need_bsphere();

        float stopDist = 0.1f * 2.0f * M_PI * mesh->bsphere.r;

        meshFIM *fim = new meshFIM;
        fim->SetMesh(mesh);
        fim->SetStopDistance( stopDist );
        std::cout << "Done\n";

        /* Length of the super-voxel = \sqrt(s^2/4 + dimg^2) */
        ImageType::PointType origin;
        origin[0] = origin_x;
        origin[1] = origin_y;
        origin[2] = origin_z;

        ImageType::SpacingType spacing;
        spacing[0] = spacing_x;
        spacing[1] = spacing_y;
        spacing[2] = spacing_z;

        ImageType::SizeType size;
        size[0] = size_x;
        size[1] = size_y;
        size[2] = size_z;

        // setting domain characterestics
        fim->imageOrigin[0] = origin[0];
        fim->imageOrigin[1] = origin[1];
        fim->imageOrigin[2] = origin[2];

        fim->imageSize[0] = size[0];
        fim->imageSize[1] = size[1];
        fim->imageSize[2] = size[2];

        fim->imageSpacing[0] = spacing[0];
        fim->imageSpacing[1] = spacing[1];
        fim->imageSpacing[2] = spacing[2];

        /* narrowband = original x number_of_subvoxels -- (1)
         * narrowband = orginal x factor (length of the supervoxel)
         */
        vtkSmartPointer< vtkPolyData > polydata = TriMesh2PolyData(mesh);

#if DBG_SAVE_INTERMEDIATE_RESULT
        vtkPolyDataWriter* polywriter = vtkPolyDataWriter::New();
        std::string vtkfilename = prefix + "_polydata.vtk";
        polywriter->SetFileName((const char*) vtkfilename.c_str());
        polywriter->SetInput(polydata);
        polywriter->Update();
#endif

        double dorigin[3]; dorigin[0] = origin_x; dorigin[1] = origin_y; dorigin[2] = origin_z;
        double dspacing[3]; dspacing[0] = spacing_x; dspacing[1] = spacing_y; dspacing[2] = spacing_z;

        std::string f2 = prefix + ".rasterized" + suffix + ".nrrd";

        std::cout << "Rasterizing ..." << std::endl;
        ImageType::Pointer binaryImage =  RasterizeMesh(polydata, dspacing, dorigin, fim->imageSize);
        std::cout << "Done Rasterizing ..." << std::endl;

        ImageWriterType::Pointer w2 = ImageWriterType::New();
        w2->SetFileName( f2.c_str() );
        w2->SetInput( binaryImage );
        w2->SetUseCompression(true);
        w2->Update();

        std::string f3 = prefix + ".DT" + suffix + ".nrrd";

        std::cout << "Computing approximate distance transform ..." << std::endl;
        ImageType::Pointer dtImage = ComputeApproximateDistanceTransform(binaryImage);
        std::cout << "Done computing approximate distance transform ..." << std::endl;

        ImageWriterType::Pointer w3 = ImageWriterType::New();
        w3->SetFileName( f3.c_str() );
        w3->SetInput( dtImage );
        w3->SetUseCompression(true);
        w3->Update();

    }

    return EXIT_SUCCESS;
}
