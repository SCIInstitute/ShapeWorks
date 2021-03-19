#pragma once

#include <vector>
#include <vnl_vector.h>

namespace shapeworks {

//! Representation of correspondence points for a shape including multiple domains
/*!
 * The StudioPoints class encapsulates the correspondence points
 * for a shape, including multiple domains, local and global points
 *
 */
class StudioPoints {

public:
  StudioPoints();

private:
  std::vector <vnl_vector<double>> local_points_;
  std::vector <vnl_vector<double>> global_points_;

};
}
