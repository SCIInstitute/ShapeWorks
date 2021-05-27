/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#include "MeshDomain.h"

namespace itk
{
  bool MeshDomain::ApplyConstraints(PointType &p, int idx, bool dbg) const {
    if (!meshWrapper) {
      return true;
    }
    p = meshWrapper->SnapToMesh(p, idx);
    return true;
  }

  bool MeshDomain::ApplyVectorConstraints(vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos) const {
    // TODO need to refactor into update particle method
    // TODO reduce magnitude of vector so it doesn't violate constraints
    return true;
  }

  vnl_vector_fixed<double, DIMENSION> MeshDomain::ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos, int idx) const {
    return meshWrapper->ProjectVectorToSurfaceTangent(pos, idx, gradE);
  }

  MeshDomain::PointType MeshDomain::UpdateParticlePosition(const PointType &point, int idx, vnl_vector_fixed<double, DIMENSION> &update) const {
    vnl_vector_fixed<double, DIMENSION> negativeUpdate;
    for (unsigned int i = 0; i < DIMENSION; i++) { negativeUpdate[i] = -update[i]; }
    PointType newPoint = meshWrapper->GeodesicWalk(point, idx, negativeUpdate);
    return newPoint;
  }

  double MeshDomain::GetMaxDiameter() const {
    //todo should this not be the length of the bounding box diagonal?
    PointType boundingBoxSize = meshWrapper->GetMeshUpperBound() - meshWrapper->GetMeshLowerBound();
    double max = 0;
    for (int d = 0; d < 3; d++) {
      max = max > boundingBoxSize[d] ? max : boundingBoxSize[d];
    }
    return max;
  }

void MeshDomain::InvalidateParticlePosition(int idx) const
{
    this->meshWrapper->InvalidateParticle(idx);
}

}
