#include <iostream>
#include <string>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "itkGradientImageFilter.h"
#include "itkPoint.h"
#include <sstream>
#include <fstream>
#include "itkFixedArray.h"
#include "utils.h"
#include "vnl/vnl_vector_fixed.h"
#include "tinyxml.h"
#include <vector>

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cout << "-------------------------------" << std::endl;
        std::cout << "ComputeMeanNormals " << std::endl;
        std::cout << "-------------------------------" << std::endl;

        std::cout << "Compute mean normals using spherical coordinates for given normals for a set of shapes " << std::endl;

        std::cout << "It uses a parameter file with the following tags" << std::endl;
        std::cout << "\t - normals:     a list of files containing normals at a set of points on each shape (output files from ProbeNormals)" << std::endl;
        std::cout << "\t - pointsCount: number of points in every shape file" << std::endl;
        std::cout << "\t - outFileName: full filename (with path) to save resulting file (default: mean.normals.txt)" << std::endl;
        std::cout << "Usage: " << argv[0] << "  parameter_file" << std::endl;
        return EXIT_FAILURE;
    }
    
    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();

    std::vector< std::string > nrmFilename;
    std::string outFilename;
    std::vector< std::vector< double > > thetas ;
    std::vector< std::vector< double > > phis;
    int numPoints;
    if (loadOkay)
    {
        TiXmlHandle docHandle( &doc );
        TiXmlElement *elem;
        std::istringstream inputsBuffer;
        std::string tmpString;

        elem = docHandle.FirstChild( "normals" ).Element();
        if (!elem)
        {
            std::cerr << "No normal files have been specified" << std::endl;
            throw 1;
        }
        else
        {
            nrmFilename.clear();
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                nrmFilename.push_back(tmpString);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        elem = docHandle.FirstChild("pointsCount").Element();
        if(!elem)
        {
            std::cerr << "Number of points not set!!!!!!" << std::endl;
            throw 1;
        }
        else
        {
            numPoints = atoi(elem->GetText());
        }
        elem = docHandle.FirstChild("outFileName").Element();
        if(!elem)
        {
            std::cerr << "No out file name.. default name mean.normals.txt being set" << std::endl;
            outFilename = "mean.normals.txt";
        }
        else
        {
            inputsBuffer.str(elem->GetText());
            inputsBuffer >> outFilename;
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
    }
    else
    {
        std::cerr << "Failed at loading parameter file: " << argv[1] <<std::endl;
        throw 1;
    }

    int numFiles = nrmFilename.size();
    thetas.resize(numPoints);
    phis.resize(numPoints);
    for (int i = 0; i < numPoints; i++)
    {
        thetas[i].resize(numFiles);
        phis[i].resize(numFiles);
    }

    for (int i = 0; i < numFiles; i++)
    {
        std::ifstream normalFile(nrmFilename[i].c_str());
        if (!normalFile)
        {
            std::cout << "normal file does not exist... " << normalFile << std::endl;
            return EXIT_FAILURE;
        }
        else
        {
            int j = 0;
            while(normalFile)
            {
                double in[3];
                normalFile >> in[0];
                if (!normalFile)
                    break;
                normalFile >> in[1];
                normalFile >> in[2];

                double out[3];
                utils::cartesian2spherical(in, out);
                phis[j][i]   = out[1];
                thetas[j][i] = out[2];
                j++;
            }
        }
        normalFile.close();
    }

    std::ofstream outfile(outFilename.c_str());

    for (int i = 0; i < numPoints; i++)
    {
        double avgNormal_sph[3];
        double avgNormal_cart[3];
        avgNormal_sph[0] = 1;
        avgNormal_sph[1] = utils::averageThetaArc(phis[i]);
        avgNormal_sph[2] = utils::averageThetaArc(thetas[i]);
        utils::spherical2cartesian(avgNormal_sph, avgNormal_cart);
        outfile << avgNormal_cart[0] << " " << avgNormal_cart[1] << " " << avgNormal_cart[2] << std::endl;
    }
    outfile.close();
    return 0;
}
