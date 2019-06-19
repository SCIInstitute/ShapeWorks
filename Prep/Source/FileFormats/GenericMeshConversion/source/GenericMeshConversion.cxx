#include "math.h"
#include "tinyxml.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <climits>

// VTK libraries

#include <vtkPolyData.h>
#include <vtkPLYWriter.h>
#include <vtkPLYReader.h>
#include <vtkSTLWriter.h>
#include <vtkSTLReader.h>
#include <vtkOBJReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyDataReader.h>


int vtk2ply(std::vector< std::string > inputpaths, std::vector< std::string > outputpaths){
	
	std::string inputname;
	std::string outputname;
	for(int i =0; i < inputpaths.size(); i++){
		inputname = inputpaths[i];
		outputname = outputpaths[i];
		std::cout << "File Input : " << inputname << std::endl;
		std::cout << "File Output : " << outputname << std::endl;
		vtkSmartPointer<vtkPolyDataReader> reader = 
	    vtkSmartPointer<vtkPolyDataReader>::New();
	  
        reader->SetFileName ( inputname.c_str() );

        vtkSmartPointer<vtkPLYWriter> writer =
            vtkSmartPointer<vtkPLYWriter>::New();
	    writer->SetInputConnection(reader->GetOutputPort());
	    writer->SetFileTypeToASCII();
	    writer->SetFileName( outputname.c_str() );
	    writer->Update();
	}

	return EXIT_SUCCESS;
}

int ply2vtk(std::vector< std::string > inputpaths, std::vector< std::string > outputpaths){
	
	std::string inputname;
	std::string outputname;
	for(int i =0; i < inputpaths.size(); i++){
		inputname = inputpaths[i];
		outputname = outputpaths[i];
		std::cout << "File Input : " << inputname << std::endl;
		std::cout << "File Output : " << outputname << std::endl;
		vtkSmartPointer<vtkPLYReader> reader = 
	    vtkSmartPointer<vtkPLYReader>::New();
	  
        reader->SetFileName ( inputname.c_str() );

        vtkSmartPointer<vtkPolyDataWriter> writer =
            vtkSmartPointer<vtkPolyDataWriter>::New();
	    writer->SetInputConnection(reader->GetOutputPort());
	    writer->SetFileTypeToASCII();
	    writer->SetFileName( outputname.c_str() );
	    writer->Update();
	}

	return EXIT_SUCCESS;
}

int ply2stl(std::vector< std::string > inputpaths, std::vector< std::string > outputpaths){
	
	std::string inputname;
	std::string outputname;
	for(int i =0; i < inputpaths.size(); i++){
		inputname = inputpaths[i];
		outputname = outputpaths[i];
		std::cout << "File Input : " << inputname << std::endl;
		std::cout << "File Output : " << outputname << std::endl;
		vtkSmartPointer<vtkPLYReader> reader = 
	    vtkSmartPointer<vtkPLYReader>::New();
	  
        reader->SetFileName ( inputname.c_str() );

        vtkSmartPointer<vtkSTLWriter> writer =
            vtkSmartPointer<vtkSTLWriter>::New();
	    writer->SetInputConnection(reader->GetOutputPort());
	    writer->SetFileTypeToASCII();
	    writer->SetFileName( outputname.c_str() );
	    writer->Update();
	}

	return EXIT_SUCCESS;
}

int vtk2stl(std::vector< std::string > inputpaths, std::vector< std::string > outputpaths){
	
	std::string inputname;
	std::string outputname;
	for(int i =0; i < inputpaths.size(); i++){
		inputname = inputpaths[i];
		outputname = outputpaths[i];
		std::cout << "File Input : " << inputname << std::endl;
		std::cout << "File Output : " << outputname << std::endl;
		vtkSmartPointer<vtkPolyDataReader> reader = 
	    vtkSmartPointer<vtkPolyDataReader>::New();
	  
        reader->SetFileName ( inputname.c_str() );

        vtkSmartPointer<vtkSTLWriter> writer =
            vtkSmartPointer<vtkSTLWriter>::New();
	    writer->SetInputConnection(reader->GetOutputPort());
	    writer->SetFileTypeToASCII();
	    writer->SetFileName( outputname.c_str() );
	    writer->Update();
	}

	return EXIT_SUCCESS;
}

int stl2vtk(std::vector< std::string > inputpaths, std::vector< std::string > outputpaths){
	
	std::string inputname;
	std::string outputname;
	for(int i =0; i < inputpaths.size(); i++){
		inputname = inputpaths[i];
		outputname = outputpaths[i];
		std::cout << "File Input : " << inputname << std::endl;
		std::cout << "File Output : " << outputname << std::endl;
		vtkSmartPointer<vtkSTLReader> reader = 
	    vtkSmartPointer<vtkSTLReader>::New();
	  
        reader->SetFileName ( inputname.c_str() );

        vtkSmartPointer<vtkPolyDataWriter> writer =
            vtkSmartPointer<vtkPolyDataWriter>::New();
	    writer->SetInputConnection(reader->GetOutputPort());
	    writer->SetFileTypeToASCII();
	    writer->SetFileName( outputname.c_str() );
	    writer->Update();
	}

	return EXIT_SUCCESS;
}

int stl2ply(std::vector< std::string > inputpaths, std::vector< std::string > outputpaths){
	
	std::string inputname;
	std::string outputname;
	for(int i =0; i < inputpaths.size(); i++){
		inputname = inputpaths[i];
		outputname = outputpaths[i];
		std::cout << "File Input : " << inputname << std::endl;
		std::cout << "File Output : " << outputname << std::endl;
		vtkSmartPointer<vtkSTLReader> reader = 
	    vtkSmartPointer<vtkSTLReader>::New();
	  
        reader->SetFileName ( inputname.c_str() );

        vtkSmartPointer<vtkPLYWriter> writer =
            vtkSmartPointer<vtkPLYWriter>::New();
	    writer->SetInputConnection(reader->GetOutputPort());
	    writer->SetFileTypeToASCII();
	    writer->SetFileName( outputname.c_str() );
	    writer->Update();
	}

	return EXIT_SUCCESS;
}


int obj2vtk(std::vector< std::string > inputpaths, std::vector< std::string > outputpaths){
	
	std::string inputname;
	std::string outputname;
	for(int i =0; i < inputpaths.size(); i++){
		inputname = inputpaths[i];
		outputname = outputpaths[i];
		std::cout << "File Input : " << inputname << std::endl;
		std::cout << "File Output : " << outputname << std::endl;
		vtkSmartPointer<vtkOBJReader> reader = 
	    vtkSmartPointer<vtkOBJReader>::New();
	  
        reader->SetFileName ( inputname.c_str() );

        vtkSmartPointer<vtkPolyDataWriter> writer =
            vtkSmartPointer<vtkPolyDataWriter>::New();
	    writer->SetInputConnection(reader->GetOutputPort());
	    writer->SetFileTypeToASCII();
	    writer->SetFileName( outputname.c_str() );
	    writer->Update();
	}

	return EXIT_SUCCESS;
}

int obj2ply(std::vector< std::string > inputpaths, std::vector< std::string > outputpaths){
	
	std::string inputname;
	std::string outputname;
	for(int i =0; i < inputpaths.size(); i++){
		inputname = inputpaths[i];
		outputname = outputpaths[i];
		std::cout << "File Input : " << inputname << std::endl;
		std::cout << "File Output : " << outputname << std::endl;
		vtkSmartPointer<vtkOBJReader> reader = 
	    vtkSmartPointer<vtkOBJReader>::New();
	  
        reader->SetFileName ( inputname.c_str() );

        vtkSmartPointer<vtkPLYWriter> writer =
            vtkSmartPointer<vtkPLYWriter>::New();
	    writer->SetInputConnection(reader->GetOutputPort());
	    writer->SetFileTypeToASCII();
	    writer->SetFileName( outputname.c_str() );
	    writer->Update();
	}

	return EXIT_SUCCESS;
}

int obj2stl(std::vector< std::string > inputpaths, std::vector< std::string > outputpaths){
	
	std::string inputname;
	std::string outputname;
	for(int i =0; i < inputpaths.size(); i++){
		inputname = inputpaths[i];
		outputname = outputpaths[i];
		std::cout << "File Input : " << inputname << std::endl;
		std::cout << "File Output : " << outputname << std::endl;
		vtkSmartPointer<vtkOBJReader> reader = 
	    vtkSmartPointer<vtkOBJReader>::New();
	  
        reader->SetFileName ( inputname.c_str() );

        vtkSmartPointer<vtkSTLWriter> writer =
            vtkSmartPointer<vtkSTLWriter>::New();
	    writer->SetInputConnection(reader->GetOutputPort());
	    writer->SetFileTypeToASCII();
	    writer->SetFileName( outputname.c_str() );
	    writer->Update();
	}

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "GENERIC MESH FORMAT CONVERSION" << std::endl;
        std::cerr << "-------------------------------" << std::endl;

		std::cerr << "Converts different mesh formats (vtk, ply, stl, obj) to other formats" << std::endl;

		std::cerr << "It uses a parameter file with the following tags" << std::endl;
		std::cerr << "- input_format: integer value specifying the input mesh file format" << std::endl;
		std::cerr << "\t (1) .vtk " << std::endl;
		std::cerr << "\t (2) .ply " << std::endl;
		std::cerr << "\t (3) .stl " << std::endl;
		std::cerr << "\t (4) .obj " << std::endl;
		std::cerr << "- output_format: integer value specifying the output mesh file format" << std::endl;
		std::cerr << "\t (1) .vtk " << std::endl;
		std::cerr << "\t (2) .ply " << std::endl;
		std::cerr << "\t (3) .stl " << std::endl;
		std::cerr << "- input_mesh: paths of the input meshes" << std::endl;
		std::cerr << "- output_mesh: paths of the output meshes" << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;

        return EXIT_FAILURE;
    }

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
    std::istringstream inputsBuffer;
    std::string tmpString;
    std::vector< std::string > inputMeshPaths;
    std::vector< std::string > outputMeshPaths;

    int input_format = 0;
    int output_format = 0;

    if(loadOkay)
    {	

    	// read input format
    	elem = docHandle.FirstChild( "input_format" ).Element();
        if (elem)
        {
            input_format = atoi( elem->GetText() );
        }
        else
        {
            std::cerr << "No input mesh format specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // read output format
        elem = docHandle.FirstChild( "output_format" ).Element();
        if (elem)
        {
            output_format = atoi( elem->GetText() );
        }
        else
        {
            std::cerr << "No output mesh format specified!" << std::endl;
            return EXIT_FAILURE;
        }

        // read input mesh paths
        elem = docHandle.FirstChild( "input_mesh" ).Element();
        if (!elem)
        {
            std::cerr << "No input mesh files have been specified" << std::endl;
            throw 1;
        }
        else
        {	
        	
            // mesh input path read
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                inputMeshPaths.push_back(tmpString);
                // std::cout << tmpString <<std::endl;
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

        // read input mesh paths
        elem = docHandle.FirstChild( "output_mesh" ).Element();
        if (!elem)
        {
            std::cerr << "No input mesh files have been specified" << std::endl;
            throw 1;
        }
        else
        {	
        	
            // mesh input path read
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                outputMeshPaths.push_back(tmpString);
                // std::cout << tmpString <<std::endl;
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
    }
    else
    {
        std::cerr << "param file can not be opened ..." << std::endl;
        throw 1;
    }

    if( input_format == output_format){
    	std::cout << "No need for conversion!" <<std::endl;
    }

    if( output_format == 4 ){
    	// convert from vtk to ply
    	std::cerr << "No OBJWriter in VTK ... Failed!" <<std::endl;
    	return EXIT_FAILURE;
    }

    if( input_format == 1 && output_format == 2){
    	// convert from vtk to ply
    	std::cout << "Converting From VTK to PLY ..." << std::endl;
    	std::cout << "  " << std::endl;
    	vtk2ply(inputMeshPaths, outputMeshPaths);
    }

    if( input_format == 1 && output_format == 3){
    	// convert from vtk to stl
    	std::cout << "Converting From VTK to STL ..." << std::endl;
    	std::cout << "  " << std::endl;
    	vtk2stl(inputMeshPaths, outputMeshPaths);
    }

    if( input_format == 2 && output_format == 1){
    	// convert from ply to vtk
    	std::cout << "Converting From PLY to VTK ..." << std::endl;
    	std::cout << "  " << std::endl;
    	ply2vtk(inputMeshPaths, outputMeshPaths);
    }

    if( input_format == 2 && output_format == 3){
    	// convert from ply to stl
    	std::cout << "Converting From PLY to STL ..." << std::endl;
    	std::cout << "  " << std::endl;
    	ply2stl(inputMeshPaths, outputMeshPaths);
    }

    if( input_format == 3 && output_format == 1){
    	// convert from stl to vtk
    	std::cout << "Converting From STL to VTK ..." << std::endl;
    	std::cout << "  " << std::endl;
    	stl2vtk(inputMeshPaths, outputMeshPaths);
    }

    if( input_format == 3 && output_format == 2){
    	// convert from stl to ply
    	std::cout << "Converting From STL to PLY ..." << std::endl;
    	std::cout << "  " << std::endl;
    	stl2ply(inputMeshPaths, outputMeshPaths);
    }

    if( input_format == 4 && output_format == 1){
    	// convert from obj to vtk
    	std::cout << "Converting From OBJ to VTK ..." << std::endl;
    	std::cout << "  " << std::endl;
    	obj2vtk(inputMeshPaths, outputMeshPaths);
    }

    if( input_format == 4 && output_format == 2){
    	// convert from obj to ply
    	std::cout << "Converting From OBJ to PLY ..." << std::endl;
    	std::cout << "  " << std::endl;
    	obj2ply(inputMeshPaths, outputMeshPaths);
    }

    if( input_format == 4 && output_format == 3){
    	// convert from obj to stl
    	std::cout << "Converting From OBJ to STL ..." << std::endl;
    	std::cout << "  " << std::endl;
    	obj2stl(inputMeshPaths, outputMeshPaths);
    }


    return EXIT_SUCCESS;
}
