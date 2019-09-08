/*
 * Shapeworks license
 */

/**
 * @file SurfaceReconstructor.h
 * @brief Surface Reconstruction Layer
 *
 * The SurfaceReconstructor wraps the surface reconstruction method
 */

#pragma once

#include <vector>
#include <string>

class SurfaceReconstructor
{

public:

  SurfaceReconstructor();

  void set_filenames(std::vector< std::string > distance_transform_filenames,
                     std::vector< std::string > local_point_filenames,
                     std::vector< std::string > world_point_filenames);

  void generate_mean_dense();

private:

  std::vector< std::string > distance_transform_filenames_;
  std::vector< std::string > world_point_filenames_;
  std::vector< std::string > local_point_filenames_;
};
