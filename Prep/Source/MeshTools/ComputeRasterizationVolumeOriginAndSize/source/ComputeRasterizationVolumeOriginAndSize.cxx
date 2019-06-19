 
#include "vtkSmartPointer.h"
#include "tinyxml.h"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>

#include <vtkVersion.h>
#include <vtkImageData.h>

int main(int argc, char *argv[])
{
    // preliminaries
    if( argc < 2 )
    {
	std::cerr << "-------------------------------" << std::endl;
	std::cerr << "ComputeRasterizationVolumeOriginAndSize " << std::endl;
	std::cerr << "-------------------------------" << std::endl;

	std::cerr << "Given a set of meshes (vtk), compute the origin and size of a volume that would contain the rasterization of each mesh " << std::endl;
	std::cerr << "so that all meshes will be embedded within the same volume characteristics" << std::endl;

	std::cerr << "It uses a parameter file with the following tags" << std::endl;
	std::cerr << "\t- mesh: a list of vtk file names for meshes to be processed" << std::endl;
	std::cerr << "\t- centering: a flag to indicate whether build the to-be built rasterization is centered on the shape's center" << std::endl;
	std::cerr << "\t- spacing_x, spacing_y, spacing_z: voxel spacing of the resulting distance transform" << std::endl;
	std::cerr << "\t- pad_pixels: number of pixels to be padded in each dimension" << std::endl;
	std::cerr << "\t- out_origin_filename: the filename to save the origin information" << std::endl;
	std::cerr << "\t- out_size_filename: the filename to save the size information" << std::endl;
  
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }

    // variables
    std::vector< std::string > inFilenames; inFilenames.clear();
    std::string out_origin_filename;
    std::string out_size_filename;

    bool centering    = false; // a flag to indicate whether build the rasterization being centered on the shape's center
    float spacing_x = 1.0; // voxel spacing in the x-direction
    float spacing_y = 1.0; // voxel spacing in the y-direction
    float spacing_z = 1.0; // voxel spacing in the z-direction

    int pad_pixels         = 0; // number of voxels to pad in each dimension

    // read parameters
    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();

    if (loadOkay)
    {
        TiXmlHandle docHandle( &doc );
        TiXmlElement *elem;
        std::istringstream inputsBuffer;
        std::string filename("/dev/null\0");

        // Compile the list of input files.
        elem = docHandle.FirstChild( "mesh" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                inFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No inputs to process!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "out_origin_filename" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            inputsBuffer >> out_origin_filename;
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No out_origin_filename is specified!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "out_size_filename" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            inputsBuffer >> out_size_filename;
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No out_size_filename is specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // centeralize the rasterization
        elem = docHandle.FirstChild( "centering" ).Element();
        if (elem)
        {
            atoi(elem->GetText()) > 0 ? centering = true : centering = false;
        }

        // voxel spacing in the x-direction
        elem = docHandle.FirstChild( "spacing_x" ).Element();
        if (elem)
        {
            spacing_x = atof( elem->GetText() );
        }

        // voxel spacing in the y-direction
        elem = docHandle.FirstChild( "spacing_y" ).Element();
        if (elem)
        {
            spacing_y = atof( elem->GetText() );
        }

        // voxel spacing in the z-direction
        elem = docHandle.FirstChild( "spacing_z" ).Element();
        if (elem)
        {
            spacing_z = atof( elem->GetText() );
        }

        elem = docHandle.FirstChild( "pad_pixels" ).Element();
        if (elem)
        {
            pad_pixels = atof( elem->GetText() );
        }

    }

    std::vector<vtkSmartPointer<vtkPolyData> > meshes;
    meshes.resize(inFilenames.size());

    // we'll need to compute the bounding box for the entire population
    double largestBoundaries[6] ;
    largestBoundaries[0] = largestBoundaries[2] = largestBoundaries[4] = 999999 ;
    largestBoundaries[1] = largestBoundaries[3] = largestBoundaries[5] = -999999 ;

    for (unsigned int shapeNo = 0; shapeNo < inFilenames.size(); shapeNo++)
    {
        vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
        reader->SetFileName(inFilenames[shapeNo].c_str());
        reader->Update();

        // get the current surface mesh
        meshes[shapeNo] = reader->GetOutput();

        std::cout << "ReadMesh completed for " << inFilenames[shapeNo].c_str()  << std::endl ;

        // the bounding box of the current mesh
        double bb[6] ;
        meshes[shapeNo]->GetBounds ( bb ) ;

        if ( centering )
        {
            double bbSize[3], centerOffset[3] ;
            bbSize[0] = bb[1] - bb[0] ;
            bbSize[1] = bb[3] - bb[2] ;
            bbSize[2] = bb[5] - bb[4] ;
            centerOffset[0] = -bbSize[0] * 0.5 - bb[0] ;
            centerOffset[1] = -bbSize[1] * 0.5 - bb[2] ;
            centerOffset[2] = -bbSize[2] * 0.5 - bb[4] ;
            bb[0] = -bbSize[0] * 0.5 ;
            bb[1] = bbSize[0] * 0.5 ;
            bb[2] = -bbSize[1] * 0.5 ;
            bb[3] = bbSize[1] * 0.5 ;
            bb[4] = -bbSize[2] * 0.5 ;
            bb[5] = bbSize[2] * 0.5 ;

            std::cout << "Offsetting by " << centerOffset[0] << " " << centerOffset[1] << " " << centerOffset[2] << std::endl ;
        }

        if ( bb[0] < largestBoundaries[0] )
            largestBoundaries[0] = bb[0] ;
        if ( bb[2] < largestBoundaries[2] )
            largestBoundaries[2] = bb[2] ;
        if ( bb[4] < largestBoundaries[4] )
            largestBoundaries[4] = bb[4] ;
        if ( bb[1] > largestBoundaries[1] )
            largestBoundaries[1] = bb[1] ;
        if ( bb[3] > largestBoundaries[3] )
            largestBoundaries[3] = bb[3] ;
        if ( bb[5] > largestBoundaries[5] )
            largestBoundaries[5] = bb[5] ;

    }

    largestBoundaries[0] = largestBoundaries[0] - pad_pixels*spacing_x;
    largestBoundaries[1] = largestBoundaries[1] + pad_pixels*spacing_x;

    largestBoundaries[2] = largestBoundaries[2] - pad_pixels*spacing_y;
    largestBoundaries[3] = largestBoundaries[3] + pad_pixels*spacing_y;

    largestBoundaries[4] = largestBoundaries[4] - pad_pixels*spacing_z;
    largestBoundaries[5] = largestBoundaries[5] + pad_pixels*spacing_z;

    std::cout << "Largest boundaries: " << largestBoundaries[0] << " " << largestBoundaries[1] << " "
              << largestBoundaries[2] << " " << largestBoundaries[3] << " " << largestBoundaries[4] << " "
              << largestBoundaries[5] << std::endl ;

    // compute image size, origin and spacing given the common bounding box
    double  origin[3], spacing[3] ;
    int size[3] ;
    spacing[0] = spacing_x;
    spacing[1] = spacing_y;
    spacing[2] = spacing_z;

    for ( int i = 0 ; i < 3 ; i++ )
    {
        origin[i] = floor ( largestBoundaries[2*i] ) - 1  ;
        size[i]   = ceil ( ( ceil ( largestBoundaries[2*i+1] ) - origin[i] ) / spacing[i] ) + 1 ;
    }

    std::cout << "Origin: " << origin[0] << " " << origin[1] << " " << origin[2] << std::endl ;
    std::cout << "Size: " << size[0] << " " << size[1] << " " << size[2] << std::endl ;


    std::ofstream ofs_origin(out_origin_filename.c_str());
    ofs_origin << origin[0] << std::endl << origin[1] << std::endl << origin[2] << std::endl;
    ofs_origin.close();

    std::ofstream ofs_size(out_size_filename.c_str());
    ofs_size << size[0] << std::endl << size[1] << std::endl << size[2] << std::endl;
    ofs_size.close();

    return EXIT_SUCCESS;
}


