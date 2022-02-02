#pragma once

#include <QString>
#include <QSharedPointer>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <vnl/vnl_vector.h>

class MeshSettings;
typedef QSharedPointer< MeshSettings > MeshSettingsHandle;

//! Settings for creating a mesh.
/*!
 * The MeshSettings class represents a set of settings used it creating a mesh.
 *
 */
class MeshSettings
{

public:

  /// Constructor
  MeshSettings();

  /// Destructor
  ~MeshSettings();

private:
};
