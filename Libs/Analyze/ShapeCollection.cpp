#include "ShapeCollection.h"
#include <iostream>

ShapeCollection::ShapeCollection() {

}

ShapeCollection::ShapeCollection(const std::string &flist_path) {
    read(flist_path);
}

void ShapeCollection::read(const std::string &flist_path) {
    std::cout << "Reading: " << flist_path << std::endl;
}

void ShapeCollection::evaluate() const {
    std::cout << "Evaluating" << std::endl;
}
