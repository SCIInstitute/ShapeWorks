
#include <Data/StudioParticles.h>

namespace shapeworks {

//---------------------------------------------------------------------------
StudioParticles::StudioParticles()
{

}

//---------------------------------------------------------------------------
void StudioParticles::set_local_particles(int domain, std::vector<itk::Point<double>> particles)
{
  this->set_particles(domain, particles, true);
}

//---------------------------------------------------------------------------
void StudioParticles::set_world_particles(int domain, std::vector<itk::Point<double>> particles)
{
  this->set_particles(domain, particles, false);
}

//---------------------------------------------------------------------------
void
StudioParticles::set_particles(int domain, std::vector<itk::Point<double>> particles, bool local)
{
  auto& points = local ? this->local_points_ : this->global_points_;
  if (domain >= points.size()) {
    points.resize(domain + 1);
  }

  vnl_vector<double> vector(particles.size());
  int idx = 0;

  for (int i = 0; i < particles.size(); i++) {
    vector[idx++] = particles[i][0];
    vector[idx++] = particles[i][1];
    vector[idx++] = particles[i][2];
  }
  points[domain] = vector;

}

//---------------------------------------------------------------------------
std::vector<vnl_vector<double>> StudioParticles::get_local_particles()
{
  return this->local_points_;
}

//---------------------------------------------------------------------------
std::vector<vnl_vector<double>> StudioParticles::get_world_particles()
{
  return this->global_points_;
}

//---------------------------------------------------------------------------
std::vector<itk::Point<double>> StudioParticles::get_local_points(int domain)
{
  return this->vnl_to_point_vector(this->local_points_[domain]);
}

//---------------------------------------------------------------------------
std::vector<itk::Point<double>> StudioParticles::get_world_points(int domain)
{
  return this->vnl_to_point_vector(this->global_points_[domain]);
}

//---------------------------------------------------------------------------
std::vector<itk::Point<double>>
StudioParticles::vnl_to_point_vector(const vnl_vector<double>& vnl)
{
  std::vector<itk::Point<double>> points;

  for (size_t i = 0; i < vnl.size(); i += 3) {
    itk::Point<double> pt;
    pt[0] = vnl[i];
    pt[1] = vnl[i + 1];
    pt[2] = vnl[i + 2];
    points.push_back(pt);
  }
  return points;
}

}
