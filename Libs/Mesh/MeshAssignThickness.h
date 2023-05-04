#include <Image.h>
#include <Mesh.h>

namespace shapeworks {

namespace mesh {

void assign_thickness(Mesh& mesh, const Image& image, const Image& dt, double threshold);

}

}  // namespace shapeworks
