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

#include "RBFShape.h"

using namespace std;

int main(int argc, char *argv[])
{
    // preliminaries
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "RBFShapeGradient " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Given an set of points and a built RBFShape file, it evaluates the gradient at a point. \n" << std::endl;

        std::cerr << "It uses a parameter file with the following tags:" << std::endl;
        std::cerr << "\t - input: File with points to be evaluated against the shape (p_x p_y p_z) every line"<< std::endl;
        std::cerr << "\t - input_rbfshape: File outputted by RBFShapeBuild"<< std::endl;
        std::cerr << "\t - output: A file with the magnitude of the gradient at each point, then the gradient point." << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }

    // variables
    std::string infile;
    std::string rbfshape_file;
    std::string outfile;

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
        else if(words[0] == "input_rbfshape:"){rbfshape_file= words[1];}
    }

    parfile.close();

    if(debug){
        std::cout << "-------------------------------" << std::endl;
        std::cout << "RBFShapeGradient " << std::endl;
        std::cout << "-------------------------------" << std::endl;
        std::cout << "Parameters: " << std::endl;
        std::cout << "\t Input file: " << infile << std::endl;
        std::cout << "\t Input rbfshape file: " << rbfshape_file << std::endl;
        std::cout << "\t Output file: " << outfile << std::endl;
    }

    cout << "Reading files..." << endl;
    //Reading infile
    std::ifstream in(infile);
    std::vector<std::vector<double> > temp_vec;

    while (std::getline(in, line))  // this does the checking!
    {
        std::istringstream iss(line);
        vector<string> words{istream_iterator<string>{iss},
                          istream_iterator<string>{}};
        if(words.size() >= 3){
            std::vector<double> temp;
            for(size_t i = 0; i < words.size(); i++){temp.push_back(stod(words[i]));}
            temp_vec.push_back(temp);
        }
        else{
            cerr << "Line did not have 6 elements; line <" << line << endl;
        }
    }

    in.close();

    cout << "Setting up RBF..." << endl;
    Eigen::MatrixXd ev_pts(temp_vec.size(), 3);

    for(size_t i = 0; i < temp_vec.size(); i++){
        ev_pts(i, 0) = temp_vec[i][0];
        ev_pts(i, 1) = temp_vec[i][1];
        ev_pts(i, 2) = temp_vec[i][2];
    }

    shapeworks::RBFShape shape = shapeworks::RBFShape();

    shape.loadFromEqFile(rbfshape_file);

    cout << "Evaluating Gradients..." << endl;

    std::vector<Eigen::Vector3d> gradients;
    std::vector<double> mag_gradients;

    std::ofstream file;
    file.open(outfile);
    file << std::fixed;
    file << std::setprecision(25);

    for(size_t i = 0; i < temp_vec.size(); i++){
        gradients.push_back(shape.gradient(ev_pts.row(i)));
        mag_gradients.push_back(shape.mag_gradient(ev_pts.row(i)));
        file << mag_gradients[i] << " " << gradients[i](0) << " " << gradients[i](1) << " " << gradients[i](2) << "\n";
    }

    file.close();

    cout << "Written to file <" << outfile << ">." << endl;

    return EXIT_SUCCESS;
}
