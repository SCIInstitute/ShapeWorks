#include <vtkSmartPointer.h>
class vtkPolyData;

class DataManager
{

public:
  DataManager();
  ~DataManager();

  void importFile( std::string file );

  std::vector<vtkSmartPointer<vtkPolyData> > getMeshes();

private:
  int numShapes_;
  std::vector<vtkSmartPointer<vtkPolyData> > meshes_;
};
