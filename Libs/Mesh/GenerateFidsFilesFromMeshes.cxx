 
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "TriMesh.h"
#include "meshFIM.h"
#include "math.h"

#include "tinyxml.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <climits>

#include "itkIdentityTransform.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"


#include "itkZeroCrossingImageFilter.h"
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

typedef itk::BinaryThresholdImageFilter< ImageType, ImageType >  ThresholdFilterType;
typedef itk::LinearInterpolateImageFunction<ImageType, double> InterpolatorType;
typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;

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
    int xint = (int) vcl_float_round_style(x);
    float res = n - 1.0f*d*xint;
    return res;
}

int computeScaledImage(ImageType::Pointer input, ImageType::Pointer output, const double factor = 1.0)
{
    ImageType::SizeType size = input->GetLargestPossibleRegion().GetSize();
    ImageType::SizeType osize;
    osize[0] = static_cast<ImageType::SizeType::SizeValueType> (size[0] / factor);
    osize[1] = static_cast<ImageType::SizeType::SizeValueType> (size[1] / factor);
    osize[2] = static_cast<ImageType::SizeType::SizeValueType> (size[2] / factor);

    ImageType::SpacingType spacing = input->GetSpacing();
    spacing[0] = spacing[0] * factor;
    spacing[1] = spacing[1] * factor;
    spacing[2] = spacing[2] * factor;

    ImageType::IndexType index;
    index.Fill(0);

    typedef itk::IdentityTransform<double, 3> IdentityTransformType;
    IdentityTransformType::Pointer transform = IdentityTransformType::New();
    transform->SetIdentity();

    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    //interpolator->SetSplineOrder( 3 );

    ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    resampler->SetInput(input);
    resampler->SetTransform( transform );
    resampler->SetInterpolator( interpolator );
    resampler->SetSize( osize );
    resampler->SetOutputOrigin( input->GetOrigin() );
    resampler->SetOutputSpacing( spacing );
    resampler->SetOutputDirection( input->GetDirection() );
    resampler->SetDefaultPixelValue (-100.0f);
    resampler->Update();

    try {
        // inline DeepCopy
        output->SetRegions( resampler->GetOutput()->GetLargestPossibleRegion() );
        output->Allocate();
        output->SetOrigin( resampler->GetOutput()->GetOrigin());
        output->SetSpacing( resampler->GetOutput()->GetSpacing());
        output->SetDirection( resampler->GetOutput()->GetDirection());

        itk::ImageRegionConstIterator < ImageType > inputIt( resampler->GetOutput(), resampler->GetOutput()->GetLargestPossibleRegion() );
        itk::ImageRegionIterator < ImageType > outputIt(output, output->GetLargestPossibleRegion() );

        while( !inputIt.IsAtEnd() )
        {
            outputIt.Set( inputIt.Get() );
            ++inputIt;
            ++outputIt;
        }

    } catch(itk::ExceptionObject &e) {
        std::cerr << "In computeScaledImage(). Exception thrown\n";
        std::cerr << e << std::endl;
        return -1;
    }

    return 1;
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
        triangle->GetPointIds()->SetId(0, mesh->faces[ii].v[0]);
        triangle->GetPointIds()->SetId(1, mesh->faces[ii].v[1]);
        triangle->GetPointIds()->SetId(2, mesh->faces[ii].v[2]);
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
        std::cerr << "GenerateFidsFiles " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Compute face index map of a given set of meshes (ply format) along with its signed distance transfrom using spatial hashing" << std::endl;

        std::cerr << "It uses a parameter file with the following tags" << std::endl;
        std::cerr << "\t - mesh: a list of ply file names for meshes to be processed" << std::endl;
        std::cerr << "\t - origin_x, origin_t, origin_z: the origin in physical units of the resulting distance transform" << std::endl;
        std::cerr << "\t - spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform" << std::endl;
        std::cerr << "\t - size_x, size_y, size_z: the size (rows,cols,slices) of the resulting distance transform" << std::endl;
        std::cerr << "\t - number_of_subvoxels: : number of subvoxels to divid each voxel (higher improve subvoxel accuarcy esp for meshes with high curvature regions)" << std::endl;
        std::cerr << "\t - number_of_voxels: number of voxels to construct a supervoxel" << std::endl;
        std::cerr << "\t - narrow_band: a narrow band defined in phyical units to limit the supvoxel-accuarate distance transform computation, " << std::endl;
        std::cerr << "\t \t distance values of voxels outside this band will be inferred using fids" << std::endl;
        std::cerr << "\t - ball_radius_factor: to reduce the radius(b) at each super-voxel. (At times b is too big and contains the whole mesh. Use < 1)" << std::endl;
        std::cerr << "\t - num_threads: number of thread to be spawned" << std::endl;

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

    float narrowBandWidth     = 3.0f;
    int   number_of_subvoxels = 4;
    int   number_of_voxels    = 4;
    float ballRadiusFactor    = 1.0f; // search for all neighbors
    int num_threads = 6; // 155.98.20.215
    bool visualize_fids = false;
    bool do_raster = true;
    bool do_initDT = true;
    bool do_fids = true;

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

        elem = docHandle.FirstChild( "number_of_subvoxels" ).Element();
        if(elem)
        {
            number_of_subvoxels = atoi( elem->GetText() );
        }

        elem = docHandle.FirstChild( "number_of_voxels" ).Element();
        if(elem)
        {
            number_of_voxels = atoi( elem->GetText() );
        }

        elem = docHandle.FirstChild( "narrow_band" ).Element();
        if(elem)
        {
            narrowBandWidth = atof( elem->GetText() );
        }

        elem = docHandle.FirstChild( "ball_radius_factor" ).Element();
        if(elem)
        {
            ballRadiusFactor = atof( elem->GetText() );
        }

        elem = docHandle.FirstChild( "num_threads" ).Element();
        if(elem)
        {
            num_threads = atoi( elem->GetText() );
        }

        elem = docHandle.FirstChild( "visualize_fids" ).Element();
        if(elem)
        {
            atoi( elem->GetText() ) > 0 ? visualize_fids = true : visualize_fids = false;
        }

        // Praful - debugging pelvis
        elem = docHandle.FirstChild( "do_raster" ).Element();
        if(elem)
        {
            atoi( elem->GetText() ) > 0 ? do_raster = true : do_raster = false;
        }

        elem = docHandle.FirstChild( "do_initDT" ).Element();
        if(elem)
        {
            atoi( elem->GetText() ) > 0 ? do_initDT = true : do_initDT = false;
        }

        elem = docHandle.FirstChild( "do_fids" ).Element();
        if(elem)
        {
            atoi( elem->GetText() ) > 0 ? do_fids = true : do_fids = false;
        }


    }
    else
    {
        std::cerr << "param file can not be opened ..." << std::endl;
        throw 1;
    }

    int numShapes = meshFilename.size();
    ImageWriterType::Pointer writer = ImageWriterType::New();

    std::stringstream ss; ss << ballRadiusFactor;
    std::stringstream sp; sp << spacing_x;
    std::stringstream nb; nb << narrowBandWidth;
    std::string suffix = "_r" + ss.str() +  "_sp" + sp.str() + "_nb" + nb.str() ;

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
        mesh->imageOrigin[0] = origin[0];
        mesh->imageOrigin[1] = origin[1];
        mesh->imageOrigin[2] = origin[2];

        mesh->imageSize[0] = size[0];
        mesh->imageSize[1] = size[1];
        mesh->imageSize[2] = size[2];

        mesh->imageSpacing[0] = spacing[0];
        mesh->imageSpacing[1] = spacing[1];
        mesh->imageSpacing[2] = spacing[2];

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
        ImageType::Pointer binaryImage;
        if(do_raster)
        {
        std::cout << "Rasterizing ..." << std::endl;
//        ImageType::Pointer binaryImage =  RasterizeMesh(polydata, dspacing, dorigin, mesh->imageSize);
        binaryImage =  RasterizeMesh(polydata, dspacing, dorigin, mesh->imageSize);
        std::cout << "Done Rasterizing ..." << std::endl;

//#if DBG_SAVE_INTERMEDIATE_RESULT
        ImageWriterType::Pointer w2 = ImageWriterType::New();
//        std::string f2 = prefix + ".rasterized" + suffix + ".nrrd";
        w2->SetFileName( f2.c_str() );
        w2->SetInput( binaryImage );
        w2->Update();
//#endif
        }
        else
        {
            std::cout<<"Loading Binary Image: "<<f2.c_str()<<std::endl;
            ImageReaderType::Pointer reader = ImageReaderType::New();
            reader->SetFileName(f2.c_str());
            reader->UpdateLargestPossibleRegion();
            binaryImage = reader->GetOutput();
        }

        std::string f3 = prefix + ".DT" + suffix + ".nrrd";
        ImageType::Pointer dtImage;
        if(do_initDT)
        {
        std::cout << "Computing approximate distance transform ..." << std::endl;
//        ImageType::Pointer dtImage = ComputeApproximateDistanceTransform(binaryImage);
        dtImage = ComputeApproximateDistanceTransform(binaryImage);
        std::cout << "Done computing approximate distance transform ..." << std::endl;

//#if DBG_SAVE_INTERMEDIATE_RESULT
        ImageWriterType::Pointer w3 = ImageWriterType::New();
//        std::string f3 = prefix + ".DT" + suffix + ".nrrd";
        w3->SetFileName( f3.c_str() );
        w3->SetInput( dtImage );
        w3->Update();
//#endif
        }
        else
        {
            std::cout<<"Loading Initial DT: "<<f3.c_str()<<std::endl;
            ImageReaderType::Pointer reader = ImageReaderType::New();
            reader->SetFileName(f3.c_str());
            reader->UpdateLargestPossibleRegion();
            dtImage = reader->GetOutput();
        }

        if(!do_fids)
            continue;

        ImageType::Pointer scaledDT = ImageType::New();
        int ret = computeScaledImage(dtImage, scaledDT, 1.0f/number_of_subvoxels);
        if(ret == -1)
        {
            std::cerr << "ERROR : scaled narrow band image empty. Aborting,\n";
            return EXIT_FAILURE;
        }

#if DBG_SAVE_INTERMEDIATE_RESULT
        //        ImageWriterType::Pointer w2 = ImageWriterType::New();
        //        std::string f2 = prefix + ".scaled.nrrd";
        //        w2->SetFileName( f2.c_str() );
        //        w2->SetInput( scaledDT );
        //        w2->Update();
#endif

        std::cout << "Filter narrow band...";
        ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();
        thresholdFilter->SetInput( scaledDT );
        thresholdFilter->SetOutsideValue( 0.0f );
        thresholdFilter->SetInsideValue(1.0f);
        thresholdFilter->SetLowerThreshold( -1.0*narrowBandWidth );
        thresholdFilter->SetUpperThreshold( 1.0*narrowBandWidth );
        /* PRATEEP
         * inner and outer distance values to maximum positive and negative.
        */
        //        thresholdFilter->SetLowerThreshold( -DBLMAX );
        //        thresholdFilter->SetUpperThreshold( DBLMAX );
        thresholdFilter->Update();
        std::cout << "Done\n";

#if DBG_SAVE_INTERMEDIATE_RESULT
        //        ImageWriterType::Pointer w1 = ImageWriterType::New();
        //        std::string f1 = prefix + ".scaled.narrowband.nrrd";
        //        w1->SetFileName( f1.c_str() );
        //        w1->SetInput( thresholdFilter->GetOutput() );
        //        w1->Update();
#endif

        ImageType::ConstPointer scaledNarrowBand = thresholdFilter->GetOutput();

        /* Length of the super-voxel = \sqrt(s^2/4 + dimg^2) */
        std::cout << "Computing Super voxel dims...";
        float s = mesh->getMaximumEdgeLength();
        std::cout << "\nMaximum edge length : " << s << std::endl;

        float ldelta = (float) (s / SQRT_3); // sqrt(3) = 1.732050807
        float q      = SQRT_3 * number_of_voxels * spacing[0];

        std::cout << "Generating Face Indices Map...";

#if DBG_SAVE_INTERMEDIATE_RESULT
        mesh->generateFaceIndexMapViaSuperVoxel (scaledNarrowBand, scaledDT, q, ldelta,
                                                 number_of_subvoxels, number_of_voxels, ballRadiusFactor, num_threads, prefix, suffix, true);
#else
        mesh->generateFaceIndexMapViaSuperVoxel (scaledNarrowBand, scaledDT, q, ldelta,
                                                 number_of_subvoxels, number_of_voxels, ballRadiusFactor, num_threads, prefix, suffix);
#endif

#if DBG_USE_KDTREE
        mesh->generateFaceIndexMapViaKDtree(scaledNarrowBand, number_of_subvoxels, num_threads, prefix);
#endif

        //map<int, set<int> > faceIndexMap = mesh->faceIndexMap;
        //        mesh->ReadFaceIndexMap( fidsFilename[shCount].c_str() );

        std::string fullpath(meshFilename[shCount].c_str());
        int index = fullpath.rfind("/");
        std::string path = fullpath.substr(0, index+1);
        std::string file = fullpath.substr(index+1, fullpath.length()-1);

        std::string curfidsFileName;

        index = file.rfind(".");
        if (index == std::string::npos)
            curfidsFileName = path + file +  suffix + ".fids";
        else
            curfidsFileName = path + file.substr(0, index) + suffix + ".fids";

        std::cout << "Writing face index map ...";
        mesh->WriteFaceIndexMap( curfidsFileName.c_str() );
        std::cout << "Done\n";

        //        // debug -- prateep
        //        // sphere
        //        std::string particleFilename = "/home/sci/prateepm/Public/ForShireen/sphere/sphere.s4.epsOut0.555556.txt";
        //        std::string truefaceFilename = "/home/sci/prateepm/Public/ForShireen/sphere/sphere.s4.epsOut0.555556_trueFaces.txt";
        //        std::string opFilename = "/home/sci/prateepm/Public/ForShireen/sphere/sphere.s4.epsOut0.555556_recoveredFaces.txt";

        //        // endo-cardium
        ////        std::string particleFilename = "/home/sci/prateepm/Public/ForShireen/endocardium_sample/particles_in_narrowband/postendo01_rbtc_eps.In0.666667.txt";
        ////        std::string truefaceFilename = "/home/sci/prateepm/Public/ForShireen/endocardium_sample/particles_in_narrowband/postendo01_rbtc_eps.In0.666667_trueFaces.txt";
        ////        std::string opFilename = "/home/sci/prateepm/Public/ForShireen/endocardium_sample/particles_in_narrowband/postendo01_rbtc_eps.In0.666667_recoveredFaces.txt";

        //        std::ofstream fout( opFilename.c_str(), std::ios::out);

        //        itk::ParticlePositionReader<3>::Pointer reader = itk::ParticlePositionReader<3>::New();
        //        reader->SetFileName( particleFilename.c_str());
        //        reader->Update();

        //        itk::ParticlePositionReader<1>::Pointer r = itk::ParticlePositionReader<1>::New();
        //        r->SetFileName( truefaceFilename.c_str() );
        //        r->Update();

        //        std::vector< itk::Point<double,3> > particles = reader->GetOutput();
        //        std::vector< itk::Point<double,1> > trueFaces = r->GetOutput();

        //        for (unsigned int i = 0; i < particles.size(); i++) {
        //            itk::Point<double, 3> p = particles[i];
        //            int trueface = (int) (trueFaces[i][0]);
        //            point pp;
        //            for(int ii = 0; ii < 3; ii++) pp[ii] = (float) p[ii];
        //            TriMesh::VoxelIndexType voxp = mesh->physicalPointToLinearIndex(pp);
        //            point projp;
        //            double dist = mesh->pointTriangleDistance(pp, mesh->faces[ trueface ], projp);

        //            if ( mesh->faceIndexMap[voxp].find( trueface ) == mesh->faceIndexMap[voxp].end() )
        //                fout << voxp << ' ' << i << ' ' << trueface << ' ' << dist << " 0" << std::endl;
        //        }
        //        fout.close();

        //        // debug -- prateep

        // Visualize face Index Map
        if(visualize_fids)
        {
            // Mesh
            vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
            reader->SetFileName( meshFilename[shCount].c_str() );

            vtkSmartPointer<vtkPolyDataMapper> mapperMesh = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapperMesh->SetInputConnection( reader->GetOutputPort() );

            vtkSmartPointer<vtkActor> actorMesh = vtkSmartPointer<vtkActor>::New();
            actorMesh->SetMapper(mapperMesh);
            actorMesh->GetProperty()->SetRepresentationToWireframe();

            vtkSmartPointer<vtkIdTypeArray> ids = vtkSmartPointer<vtkIdTypeArray>::New();
            ids->SetNumberOfComponents(1);

            vtkSmartPointer<vtkIdTypeArray> ids1 = vtkSmartPointer<vtkIdTypeArray>::New();
            ids1->SetNumberOfComponents(1);

            vector< vtkSmartPointer<vtkActor> > actorsPoint;
            vector< vtkSmartPointer<vtkActor> > actorsCell;

            std::string filename = curfidsFileName;
            mesh->ReadFaceIndexMap( filename.c_str() );
            mesh->imageOrigin[0] = origin[0];
            mesh->imageOrigin[1] = origin[1];
            mesh->imageOrigin[2] = origin[2];

            mesh->imageSize[0] = size[0];
            mesh->imageSize[1] = size[1];
            mesh->imageSize[2] = size[2];

            mesh->imageSpacing[0] = spacing[0];
            mesh->imageSpacing[1] = spacing[1];
            mesh->imageSpacing[2] = spacing[2];

            std::map<int, std::vector<int> > faceIndexMap = mesh->faceIndexMap;

            int len = (int) faceIndexMap.size();
            std::vector<int> indices = randperm(len);

            for(unsigned int i = 0; i < 1; i++)
            {
                // Voxel
                int ind = indices[i]; //(rng.next(len));
                std::map<int, std::vector<int> >::iterator itInit = faceIndexMap.begin();
                std::advance(itInit, ind);
                float voxelIndex = itInit->first;
                std::cout << "Viewing voxel # " << ind << std::endl;

                //                itk::Point<double, 3> p = particles[2318];
                //                point pp;
                //                for(int ii = 0; ii < 3; ii++) pp[ii] = (float) p[ii];
                //                float voxelIndex = mesh->physicalPointToLinearIndex(pp);

                std::vector<int> faceList = faceIndexMap[voxelIndex];
                TriMesh::Face f = mesh->faces[ *(faceList.begin())];
                voxelIndex = mesh->physicalPointToLinearIndex( mesh->vertices[f.v[0]] );

                // ind = x + y * size[0] + z * size[0] * size[1]
                float pt[3];

                pt[0] = (float) modulo(voxelIndex, size[0]);
                pt[1] = (float) modulo(voxelIndex / size[0], size[1]);
                pt[2] = (float) ((voxelIndex / size[0]) / size[1]);

                ImageType::PointType point;
                ImageType::IndexType index;
                index[0] = pt[0];
                index[1] = pt[1];
                index[2] = pt[2];
                // Transform using original DT
                dtImage->TransformIndexToPhysicalPoint(index, point);
                //                point[0] = mesh->vertices[f.v[0]][0];
                //                point[1] = mesh->vertices[f.v[0]][1];
                //                point[2] = mesh->vertices[f.v[0]][2];

#if 0
                float pt[3];
                set<int>::iterator it = faceIndexMap[voxelIndex].begin();
                std::advance(it,2);
                int fid = (*it);
                std::cout << "Face Index " << fid << std::endl;
                pt[0] = mesh->vertices[mesh->faces[fid].v[0]][0];
                pt[1] = mesh->vertices[mesh->faces[fid].v[0]][1];
                pt[2] = mesh->vertices[mesh->faces[fid].v[0]][2];
                std::cout << "Coordinates of physical point : " << pt[0] << ' ' << pt[1] << ' ' << pt[2] << std::endl;
                ImageType::PointType point;
                itk::ContinuousIndex<double,3> index;
                point[0] = pt[0];
                point[1] = pt[1];
                point[2] = pt[2];
                // Transform using original DT
                dtImage->TransformPhysicalPointToContinuousIndex(point, index);
                std::cout << "Coordinates of index : " << index[0] << ' ' << index[1] << ' ' << index[2] << std::endl;
#endif

                vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
                points->InsertNextPoint( point[0], point[1], point[2] );

                vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
                polydata->SetPoints( points );

                vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
                glyphFilter->SetInputData( polydata );
                glyphFilter->Update();

                vtkSmartPointer<vtkPolyDataMapper> mapperPoint = vtkSmartPointer<vtkPolyDataMapper>::New();
                mapperPoint->SetInputConnection( glyphFilter->GetOutputPort() );

                vtkSmartPointer<vtkActor> actorPoint = vtkSmartPointer<vtkActor>::New();
                actorPoint->SetMapper( mapperPoint );
                actorPoint->GetProperty()->SetPointSize(10 );
                actorPoint->GetProperty()->SetColor(0,0,1);

                actorsPoint.push_back(actorPoint);

                // Cells
                //                for(set<int>::iterator it = faceIndexMap[voxelIndex].begin(); it != faceIndexMap[voxelIndex].end(); it++)
                //                {
                //                    if((*it) == 2318)
                ids->InsertNextValue( 2318 );
                ids1->InsertNextValue( 2314 );
                //                }

                std::cout << "Number of faces : " <<  faceIndexMap[voxelIndex].size() << std::endl;

                vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
                selectionNode->SetFieldType(vtkSelectionNode::CELL);
                selectionNode->SetContentType(vtkSelectionNode::INDICES);
                selectionNode->SetSelectionList( ids );

                vtkSmartPointer<vtkSelection> sel = vtkSmartPointer<vtkSelection>::New();
                sel->AddNode( selectionNode );

                vtkSmartPointer<vtkExtractSelection> extractSel = vtkSmartPointer<vtkExtractSelection>::New();
                extractSel->SetInputConnection(0, reader->GetOutputPort());
                extractSel->SetInputData(1, sel);
                extractSel->Update();

                vtkSmartPointer<vtkUnstructuredGrid> selected = vtkSmartPointer<vtkUnstructuredGrid>::New();
                selected->ShallowCopy( extractSel->GetOutput() );

                vtkSmartPointer<vtkProperty> backfaces = vtkSmartPointer<vtkProperty>::New();
                backfaces->SetColor(1, 0, 0);
                vtkSmartPointer<vtkDataSetMapper> mapperCells = vtkSmartPointer<vtkDataSetMapper>::New();
                mapperCells->SetInputData( selected );
                vtkSmartPointer<vtkActor> actorCells = vtkSmartPointer<vtkActor>::New();
                actorCells->SetMapper( mapperCells );
                actorCells->SetBackfaceProperty( backfaces );
                actorCells->GetProperty()->SetColor(1.0, 0.0, 0.0);

                actorsCell.push_back(actorCells);

                // ids1
                vtkSmartPointer<vtkSelectionNode> selectionNode1 = vtkSmartPointer<vtkSelectionNode>::New();
                selectionNode1->SetFieldType(vtkSelectionNode::CELL);
                selectionNode1->SetContentType(vtkSelectionNode::INDICES);
                selectionNode1->SetSelectionList( ids1 );

                vtkSmartPointer<vtkSelection> sel1 = vtkSmartPointer<vtkSelection>::New();
                sel1->AddNode( selectionNode1 );

                vtkSmartPointer<vtkExtractSelection> extractSel1 = vtkSmartPointer<vtkExtractSelection>::New();
                extractSel1->SetInputConnection(0, reader->GetOutputPort());
                extractSel1->SetInputData(1, sel1);
                extractSel1->Update();

                vtkSmartPointer<vtkUnstructuredGrid> selected1 = vtkSmartPointer<vtkUnstructuredGrid>::New();
                selected1->ShallowCopy( extractSel1->GetOutput() );

                vtkSmartPointer<vtkProperty> backfaces1 = vtkSmartPointer<vtkProperty>::New();
                backfaces1->SetColor(1, 1, 0);
                vtkSmartPointer<vtkDataSetMapper> mapperCells1 = vtkSmartPointer<vtkDataSetMapper>::New();
                mapperCells1->SetInputData( selected1 );
                vtkSmartPointer<vtkActor> actorCells1 = vtkSmartPointer<vtkActor>::New();
                actorCells1->SetMapper( mapperCells1 );
                actorCells1->SetBackfaceProperty( backfaces1 );
                actorCells1->GetProperty()->SetColor(1.0, 1.0, 0.0);

                actorsCell.push_back(actorCells1);

            }

            vtkSmartPointer<vtkRenderWindow> renderWin = vtkSmartPointer<vtkRenderWindow>::New();
            vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
            interactor->SetRenderWindow( renderWin );

            vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
            renderWin->AddRenderer( renderer );

            renderer->AddActor(actorMesh);
            for(unsigned int i = 0; i < actorsCell.size(); i++)
            {
                renderer->AddActor(actorsCell[i]);
            }
            for(unsigned int i = 0; i < actorsPoint.size(); i++)
            {
                renderer->AddActor(actorsPoint[i]);
            }
            renderer->SetBackground(.3, .6, .3);

            renderWin->Render();
            interactor->Start();

        }

    }

    return EXIT_SUCCESS;
}
