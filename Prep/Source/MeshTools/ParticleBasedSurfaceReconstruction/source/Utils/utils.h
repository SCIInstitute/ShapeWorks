#ifndef UTILS_H
#define UTILS_H

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <itkImage.h>

namespace utils
{


std::vector<int> randperm(int n);

//--------------------------- IO-----------------------------------
vtkSmartPointer<vtkPoints> readSparseShape(char* filename, int number_of_particles = -1);
void writeSparseShape(char* filename, vtkSmartPointer<vtkPoints> particles);

std::vector<int> readParticleIds(char* filename);
void writeParticleIds(char* filename, std::vector<int> ids);

//------------------------- string manipulation-------------------------------
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '\\' || ch == '/';
    }
};
#else
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};
#endif

std::string removeExtension( std::string const& filename );
std::string getPath( std::string const& filename );
std::string getFilename( std::string const& pathname );

//--------------- coordinates transformations --------------------------------
void spherical2cartesian(const double inPoint[3], double outPoint[3]);
void cartesian2spherical(const double inPoint[3], double outPoint[3]);

vtkSmartPointer<vtkPoints> convertToPhysicalCoordinates(vtkSmartPointer<vtkPoints> particles, int number_of_particles,
                                                        const itk::Image< float,  3 >::SpacingType& spacing ,
                                                        const itk::Image< float,  3 >::PointType& origin);

vtkSmartPointer<vtkPoints> convertToImageCoordinates(vtkSmartPointer<vtkPoints> particles, int number_of_particles,
                                                     const itk::Image< float,  3 >::SpacingType& spacing ,
                                                     const itk::Image< float,  3 >::PointType& origin);

std::string num2str(float num);
std::vector<double> linspace(double a, double b, size_t N);
std::string int2str(int n, int number_of_zeros);
}
#endif // UTILS_H
