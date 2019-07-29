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

int computeScaledImage(ImageType::ConstPointer input, ImageType::Pointer output, const double factor = 1.0)
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

int main(int argc, char *argv[])
{
     if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "GenerateFidsFiles " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

	std::cerr << "Compute face index map of a given set of meshes (ply format) along with its signed distance transfrom using spatial hashing" << std::endl;

	std::cerr << "It uses a parameter file with the following tags" << std::endl;
	std::cerr << "\t - dist: a list of approximate distance transforms " << std::endl;	
	std::cerr << "\t - mesh: a list of ply file names for meshes to be processed" << std::endl;
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
    std::vector< std::string > dtFilename;
    std::vector< std::string > meshFilename;
    std::vector< std::string > fidsFilename; // save name as mesh but with fids extension

    float narrowBandWidth     = 3.0f;
    int   number_of_subvoxels = 4;
    int   number_of_voxels    = 4;
    float ballRadiusFactor    = 1.0f; // search for all neighbors
    int num_threads = 6; // 155.98.20.215
    bool visualize_fids = false;

    std::string tmpString;
    if(loadOkay)
    {
        elem = docHandle.FirstChild( "dist" ).Element();
        if (!elem)
        {
            std::cerr << "No DT files have been specified" << std::endl;
            throw 1;
        }
        else
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                dtFilename.push_back(tmpString);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

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

                std::string fullpath(tmpString.c_str());
                int index = fullpath.rfind("/");
                std::string path = fullpath.substr(0, index+1);
                std::string file = fullpath.substr(index+1, fullpath.length()-1);

                std::string curfidsFileName;

                index = file.rfind(".");
                if (index == std::string::npos)
                    curfidsFileName = path + file + ".fids";
                else
                    curfidsFileName = path + file.substr(0, index) + ".fids";

                fidsFilename.push_back (curfidsFileName);

            }
            inputsBuffer.clear();
            inputsBuffer.str("");
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
    }
    else
    {
        std::cerr << "param file can not be opened ..." << std::endl;
        throw 1;
    }

    int numShapes = dtFilename.size();
    ImageWriterType::Pointer writer = ImageWriterType::New();

    for (unsigned int shCount = 0; shCount < numShapes; shCount++)
    {

#if DBG_SAVE_INTERMEDIATE_RESULT || DBG_USE_KDTREE
        size_t ind = meshFilename[shCount].find_last_of('.');
        std::string prefix = meshFilename[shCount].substr(0,ind);
#endif

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

        std::cout << "Read DT File...";
        ImageReaderType::Pointer imageReader = ImageReaderType::New();
        imageReader->SetFileName( dtFilename[shCount].c_str() );
        imageReader->UpdateLargestPossibleRegion();
        std::cout << "Done\n";

        /* Length of the super-voxel = \sqrt(s^2/4 + dimg^2) */
        ImageType::PointType origin    = imageReader->GetOutput()->GetOrigin();
        ImageType::SpacingType spacing = imageReader->GetOutput()->GetSpacing();
        ImageType::SizeType size = imageReader->GetOutput()->GetLargestPossibleRegion().GetSize();

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
        ImageType::Pointer scaledDT = ImageType::New();
        int ret = computeScaledImage(imageReader->GetOutput(), scaledDT, 1.0f/number_of_subvoxels);
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
                                                 number_of_subvoxels, number_of_voxels, ballRadiusFactor, num_threads, prefix, true);
#else
        mesh->generateFaceIndexMapViaSuperVoxel (scaledNarrowBand, scaledDT, q, ldelta,
                                                 number_of_subvoxels, number_of_voxels, ballRadiusFactor, num_threads);
#endif

#if DBG_USE_KDTREE
        mesh->generateFaceIndexMapViaKDtree(scaledNarrowBand, number_of_subvoxels, num_threads, prefix);
#endif

        //map<int, set<int> > faceIndexMap = mesh->faceIndexMap;
        //        mesh->ReadFaceIndexMap( fidsFilename[shCount].c_str() );
        mesh->WriteFaceIndexMap( fidsFilename[shCount].c_str() );
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

            std::string filename = fidsFilename[shCount];
            mesh->ReadFaceIndexMap( filename.c_str() );
            ImageType::PointType origin =  imageReader->GetOutput()->GetOrigin();
            mesh->imageOrigin[0] = origin[0];
            mesh->imageOrigin[1] = origin[1];
            mesh->imageOrigin[2] = origin[2];

            ImageType::SizeType size = imageReader->GetOutput()->GetLargestPossibleRegion().GetSize();
            mesh->imageSize[0] = size[0];
            mesh->imageSize[1] = size[1];
            mesh->imageSize[2] = size[2];

            ImageType::SpacingType spacing = imageReader->GetOutput()->GetSpacing();
            mesh->imageSpacing[0] = spacing[0];
            mesh->imageSpacing[1] = spacing[1];
            mesh->imageSpacing[2] = spacing[2];

            map<int, std::vector<int> > faceIndexMap = mesh->faceIndexMap;

            int len = (int) faceIndexMap.size();
            std::vector<int> indices = randperm(len);

            for(unsigned int i = 0; i < 1; i++)
            {
                // Voxel
                int ind = indices[i]; //(rng.next(len));
                map<int, std::vector<int> >::iterator itInit = faceIndexMap.begin();
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
                ImageType::RegionType region = imageReader->GetOutput()->GetLargestPossibleRegion();
                ImageType::SizeType size = region.GetSize();

                pt[0] = (float) modulo(voxelIndex, size[0]);
                pt[1] = (float) modulo(voxelIndex / size[0], size[1]);
                pt[2] = (float) ((voxelIndex / size[0]) / size[1]);

                ImageType::PointType point;
                ImageType::IndexType index;
                index[0] = pt[0];
                index[1] = pt[1];
                index[2] = pt[2];
                // Transform using original DT
                imageReader->GetOutput()->TransformIndexToPhysicalPoint(index, point);
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
                imageReader->GetOutput()->TransformPhysicalPointToContinuousIndex(point, index);
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
