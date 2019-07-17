#include "math.h"
#include "tinyxml.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <fstream>

#define _USE_MATH_DEFINES

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_cross.h>

// Convenient typedef for storing cutting plane information
const int Dimension = 3;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "-------------------------------" << std::endl;
        std::cout << "FixCuttingPlanes " << std::endl;
        std::cout << "-------------------------------" << std::endl;

        std::cout << "Given bunch of curves/landmarks for each shape, fix the orientation of the provided cutting plane to point to the side where the curves/landmarks exist" << std::endl;

        std::cout << "It uses a parameter file with the following tags" << std::endl;
        std::cout << "\t - mesh:      a list of vtk/ply file names for meshes to be processed" << std::endl;
        std::cout << "\t - input_planes:      a list of txt file names for the planes to be processed" << std::endl;
        std::cout << "\t - output_planes:      a list of txt file names for the fixed planes to be saved" << std::endl;
        std::cout << "\t - curves_prefix: where we can find the point files of the curves, this should be the common part of the filename for all given mesh files" << std::endl;
        std::cout << "\t - curves_ext:    the extension of the curve files" << std::endl;
        std::cout << "\t - curve_labels:  the name of each curve according to the suffix given for each curve pts file" << std::endl;

        std::cout << "Usage: " << argv[0] << "  parameter file" << std::endl;
        return EXIT_FAILURE;
    }


    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
    std::istringstream inputsBuffer;

    // Collect a list of point file names and output file names
    std::vector< std::string > meshFilename;
    std::vector< std::string > inFilename;
    std::vector< std::string > outFilename;

    std::vector< std::string > curveLabels;
    std::string curves_prefix;
    std::string curves_ext = "lpts";

    std::string tmpString;

    elem = docHandle.FirstChild( "mesh" ).Element();
    if (!elem)
    {
        std::cerr << "No mesh files have been specified" << std::endl;
        throw 1;
    }
    else
    {
        meshFilename.clear();
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            meshFilename.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    elem = docHandle.FirstChild( "input_planes" ).Element();
    if (!elem)
    {
        std::cerr << "No input_planes files have been specified" << std::endl;
        throw 1;
    }
    else
    {
        inFilename.clear();
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            inFilename.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    elem = docHandle.FirstChild( "output_planes" ).Element();
    if (!elem)
    {
        std::cerr << "No output_planes files have been specified" << std::endl;
        throw 1;
    }
    else
    {
        outFilename.clear();
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            outFilename.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }


    elem = docHandle.FirstChild( "curve_labels" ).Element();
    if (!elem)
    {
        std::cerr << "No curve labels have been specified" << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            curveLabels.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    elem = docHandle.FirstChild( "curves_prefix" ).Element();
    if (!elem)
    {
        std::cerr << "No curves_prefix have been specified" << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        inputsBuffer.str(elem->GetText());
        inputsBuffer >> curves_prefix;
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    elem = docHandle.FirstChild( "curves_ext" ).Element();
    if (elem)
    {
        inputsBuffer.str(elem->GetText());
        inputsBuffer >> curves_ext;
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    for (int s = 0; s < inFilename.size(); s++)
    {
        // cutting plane points
        vnl_vector_fixed<double,Dimension> a,b,c;

        std::ifstream ifs;
        ifs.open(inFilename[s].c_str());
        ifs >> a[0] >> a[1] >> a[2] >> b[0] >> b[1] >> b[2] >> c[0] >> c[1] >> c[2] ;
        ifs.close();

        // compute cutting plane normal
        vnl_vector<double> q = vnl_cross_3d((b-a),(c-a));

        vnl_vector_fixed<double, Dimension> normal;
        vnl_vector_fixed<double, Dimension> origin;

        normal = q / (q.magnitude() + 1e-20);
        origin = a;

        bool flipPlane = false;

        for (int c = 0; c < curveLabels.size(); c++)
        {
            // read curve points
            int index = meshFilename[s].rfind("/");
            std::string path = meshFilename[s].substr(0, index+1);
            std::string file = meshFilename[s].substr(index+1, meshFilename[s].length()-1);
            std::string curveFileName;

            index = file.rfind(".");
            if (index == std::string::npos)
                curveFileName = curves_prefix + file + curveLabels[c] + "." + curves_ext;
            else
                curveFileName = curves_prefix + file.substr(0, index) + curveLabels[c] + "." + curves_ext;
            std::cout << "curve filename: " << curveFileName << std::endl;

            std::ifstream curveFile(curveFileName.c_str());
            std::vector< vnl_vector_fixed<double, Dimension> > curvePts;

            if (!curveFile)
            {
                std::cout << "Curve file not found" << std::endl;
                return EXIT_FAILURE;
            }

            while (curveFile)
            {
                vnl_vector_fixed<double, Dimension> tmpPt;

                for (int d = 0; d < Dimension; d++)
                {
                    curveFile >> tmpPt[d];
                }
                curvePts.push_back(tmpPt);
            }
            curvePts.pop_back();

            for (int ii = 0; ii < curvePts.size(); ii++)
            {
                vnl_vector_fixed<double, Dimension> p = curvePts[ii];
                double D = dot_product( normal, p - origin);

                if (D < 0 ) // plane is not oriented towards the current curve
                {
                    flipPlane = true;
                    break;
                }
            }

            if (flipPlane)
                break;
        }

        std::ofstream ofs;
        ofs.open(outFilename[s].c_str());
        if (flipPlane)
        {
            std::cout<< "Plane FIXED for: " << inFilename[s] << std::endl;
            ofs << c[0] << " " << c[1] << " " << c[2] << " " << b[0] << " " << b[1] << " " << b[2] << " " << a[0] << " " << a[1] << " " << a[2] ;
        }
        else
        {
            ofs << a[0] << " " << a[1] << " " << a[2] << " " << b[0] << " " << b[1] << " " << b[2] << " " << c[0] << " " << c[1] << " " << c[2] ;
        }
        ofs.close();
    }

    return EXIT_SUCCESS;
}

