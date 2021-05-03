#pragma once

#include <stddef.h>
#include "Shapeworks.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <algorithm>

#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/features/integral_image_normal.h>
#include <pcl/visualization/cloud_viewer.h>

#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/surface/mls.h>
#include <pcl/features/normal_3d.h>

namespace shapeworks {

class pc_aux
{
public:
  /** This functions computes the normals from a point could */
  Eigen::MatrixXd compute_normals(const Eigen::MatrixXd & pts, double r);
};

}
