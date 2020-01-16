#ifndef SHAPEWORKS_SHAPECOLLECTION_H
#define SHAPEWORKS_SHAPECOLLECTION_H

#include <string>

//TODO: Namespace?
//TODO: This should be just a list of meshes, but its unclear to me what the correct Mesh class is
class ShapeCollection {
public:
    ShapeCollection();
    ShapeCollection(const std::string &flist_path);
    void read(const std::string &flist_path);
    void evaluate() const;
};


#endif //SHAPEWORKS_SHAPECOLLECTION_H
