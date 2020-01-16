#include "ShapeCollection.h"
#include <iostream>
#include <fstream>

ShapeCollection::ShapeCollection() {

}

ShapeCollection::ShapeCollection(const std::string &flistPath) {
    read(flistPath);
}

void ShapeCollection::read(const std::string &flistPath) {
    std::cout << "Reading: " << flistPath << std::endl;

    std::ifstream shapeFile(flistPath);
    if(!shapeFile) {
        //TODO: Handle error better
        std::cerr << "Failed to open " << flistPath << std::endl;
        std::exit(1);
    }

    std::string line;
    while(std::getline(shapeFile, line)) {
        if(line.empty() || line[0] == '#') {
            continue;
        }
    }
    shapeFile.close();

}

void ShapeCollection::evaluate() const {
    std::cout << "Evaluating" << std::endl;
}
