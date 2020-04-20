#include "tinyxml.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <Eigen/Core>
#include <Eigen/Dense>

#include "PointCloud.h"

using namespace std;

int main(int argc, char *argv[])
{
    // preliminaries
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "RBFShapeBuild " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Given an set of points and their normals, this tool builds an RBFShape, which is solved and stored in the output file. " << std::endl;
        std::cerr << "Once built, it can be referenced by evaluate and gradient evaluate by providing the output file name of the build. \n" << std::endl;

        std::cerr << "It uses a parameter file with the following tags:" << std::endl;
        std::cerr << "\t - input: Points and normals for each point (p_x p_y p_z n_x n_y n_z) every line"<< std::endl;
        std::cerr << "\t - output: The first line will have the linear coefficients, and the rest will contain the points and the coefficients (p_x p_y p_z coeff)" << std::endl;
        std::cerr << "\t - dipole_distance: phyical distance from the surface to the dipoles" << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }

    // variables
    std::string infile;
    std::string outfile;
    double dipole_distance;

    // read parameters
    std::string line;
    std::ifstream parfile(argv[1]);
    bool debug = true;

    while (std::getline(parfile, line))  // this does the checking!
    {
        std::istringstream iss(line);
        vector<string> words{istream_iterator<string>{iss},
                          istream_iterator<string>{}};
        if(words[0] == "input:"){infile = words[1];}
        else if(words[0] == "output:"){outfile = words[1];}
        else if(words[0] == "dipole_distance:"){dipole_distance= stod(words[1]);}
    }

    parfile.close();

    if(debug){
        std::cout << "-------------------------------" << std::endl;
        std::cout << "RBFShapeBuild " << std::endl;
        std::cout << "-------------------------------" << std::endl;
        std::cout << "Parameters: " << std::endl;
        std::cout << "\t Input file: " << infile << std::endl;
        std::cout << "\t Output file: " << outfile << std::endl;
        std::cout << "\t Dipole distance: " << dipole_distance << std::endl;
    }

    cout << "Reading file..." << endl;
    //Reading infile
    std::ifstream in(infile);
    std::vector<std::vector<double> > temp_vec;

    while (std::getline(in, line))  // this does the checking!
    {
        std::istringstream iss(line);
        vector<string> words{istream_iterator<string>{iss},
                          istream_iterator<string>{}};
        if(words.size() == 6){
            std::vector<double> temp;
            for(size_t i = 0; i < words.size(); i++){temp.push_back(stod(words[i]));}
            temp_vec.push_back(temp);
        }
        else{
            cerr << "Line did not have 6 elements; line <" << line << endl;
        }
    }

    in.close();

    cout << "Setting up system..." << endl;
    //Getting points and normals. Then calculate dipoles and add them to pts.
    Eigen::MatrixXd raw_pts(temp_vec.size(), 3);
    Eigen::MatrixXd normals(temp_vec.size(), 3);

    Eigen::MatrixXd pts(temp_vec.size()*3, 4);

    for(size_t i = 0; i < temp_vec.size(); i++){

        raw_pts(i, 0) = temp_vec[i][0];
        raw_pts(i, 1) = temp_vec[i][1];
        raw_pts(i, 2) = temp_vec[i][2];

        double norm[3] = {temp_vec[i][3], temp_vec[i][4], temp_vec[i][5]};
        double magnitude = sqrt(norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2]);

        normals(i, 0) = norm[0]/magnitude;
        normals(i, 1) = norm[1]/magnitude;
        normals(i, 2) = norm[2]/magnitude;

        Eigen::Vector3d pos_dipole_pt = raw_pts.row(i) + normals.row(i)*dipole_distance;
        Eigen::Vector3d neg_dipole_pt = raw_pts.row(i) - normals.row(i)*dipole_distance;

        Eigen::Vector4d point = {raw_pts(i, 0),raw_pts(i, 1),raw_pts(i, 2),0.};
        Eigen::Vector4d pos_dipole = {pos_dipole_pt(0),pos_dipole_pt(1),pos_dipole_pt(2),dipole_distance};
        Eigen::Vector4d neg_dipole = {neg_dipole_pt(0),neg_dipole_pt(1),neg_dipole_pt(2),-dipole_distance};

        pts.row(3*i) = point;
        pts.row(3*i+1) = pos_dipole;
        pts.row(3*i+2) = neg_dipole;
    }

    shapeworks::PointCloud shape = shapeworks::PointCloud();

    cout << "Solving RBF system..." << endl;

    shape.solve_system(pts);

    /*
    if(debug){
        std::vector<double> evaluations = shape.batch_evaluate(pts.block(0,0,temp_vec.size()*3,3));

        for(size_t i = 0; i < evaluations.size(); i++){
            cout << pts.row(i) << " " << evaluations[i] << endl;
        }
    }
    */

    shape.writeToEqFile(outfile, 25);

    cout << "Written to file <" << outfile << ">." << endl;

    return EXIT_SUCCESS;
}
