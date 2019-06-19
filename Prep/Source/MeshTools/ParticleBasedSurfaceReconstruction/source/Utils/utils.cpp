
#include "utils.h"

#include <vtkMath.h>
#include <cmath>
#include <sstream>      // std::istringstream

namespace utils
{
std::vector<int> randperm(int n)
{
    std::vector<int> perm;
    perm.clear();

    int i, j, t;

    for(i=0; i<n; i++)
        perm.push_back(i);

    for(i=0; i<n; i++) {
        j = rand()%(n-i)+i;
        t = perm[j];
        perm[j] = perm[i];
        perm[i] = t;
    }

    return perm;
}


// ------------------- IO ------------------------------------

vtkSmartPointer<vtkPoints> readSparseShape(char* filename, int number_of_particles)
{
    std::ifstream ifs;
    ifs.open(filename);

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    if (number_of_particles>0)
    {
        for (unsigned int ii = 0 ;ii < number_of_particles; ii++)
        {
            double p[3];
            ifs >> p[0] >> p[1] >> p[2];

            points->InsertNextPoint(p[0], p[1], p[2]);
            //ofs << p[0] << "," << p[1] << ","<< p[2] << std::endl;
        }
    }
    else
    {
        std::string line;
        while(!std::getline(ifs, line, '\n').eof())
        {
            std::istringstream reader(line);

            std::string::const_iterator i = line.begin();
            std::vector<double> lineData;
            while(!reader.eof()) {
                double val;
                reader >> val;

                lineData.push_back(val);
            }
            points->InsertNextPoint(lineData[0], lineData[1], lineData[2]);
        }
    }


    ifs.close();
    std::cout << "total number of correspondences read: " << points->GetNumberOfPoints() << std::endl;
    return points;
}

void writeSparseShape(char* filename, vtkSmartPointer<vtkPoints> particles)
{
    ofstream ofs;
    ofs.open(filename);
    for(unsigned int ii = 0 ; ii < particles->GetNumberOfPoints(); ii++)
    {
        double p[3];
        particles->GetPoint(ii, p);
        ofs << p[0] << " " << p[1] << " " << p[2] << std::endl;
    }
    ofs.close();
    std::cout << "total number of correspondences save: " << particles->GetNumberOfPoints() << std::endl;
}

std::vector<int> readParticleIds(char* filename)
{
    std::ifstream ifs;
    ifs.open(filename);

    std::vector<int> ids;

    std::string line;
    while(!std::getline(ifs, line, '\n').eof())
    {
        std::istringstream reader(line);
        int id;
        reader >> id;
        ids.push_back(id);
    }

    ifs.close();
    return ids;
}

void writeParticleIds(char* filename, std::vector<int> ids)
{
    ofstream ofs;
    ofs.open(filename);
    for(unsigned int ii = 0 ; ii < ids.size(); ii++)
    {
        ofs << ids[ii] << std::endl;
    }
    ofs.close();
}

// ------------------- string manipulation ------------------------------------
std::string removeExtension( std::string const& filename )
{
    std::string::const_reverse_iterator
            pivot
            = std::find( filename.rbegin(), filename.rend(), '.' );
    return pivot == filename.rend()
            ? filename
            : std::string( filename.begin(), pivot.base() - 1 );
}

std::string getPath( std::string const& filename )
{
    std::string::const_reverse_iterator
            pivot
            = std::find( filename.rbegin(), filename.rend(), '/' );
    return pivot == filename.rend()
            ? filename
            : std::string( filename.begin(), pivot.base() - 1 );
}

std::string getFilename( std::string const& pathname )
{
    return std::string(
                std::find_if( pathname.rbegin(), pathname.rend(),
                              MatchPathSeparator() ).base(),
                pathname.end() );
}

// ------------------- coordinates transformations -----------------------------
void spherical2cartesian(const double inPoint[3], double outPoint[3])
{
    double r        = inPoint[0];
    double sinphi   = sin(inPoint[1]);
    double cosphi   = cos(inPoint[1]);
    double sintheta = sin(inPoint[2]);
    double costheta = cos(inPoint[2]);

    outPoint[0] = r*sinphi*costheta;
    outPoint[1] = r*sinphi*sintheta;
    outPoint[2] = r*cosphi;
}

void cartesian2spherical(const double inPoint[3], double outPoint[3])
{
    double x = inPoint[0];
    double y = inPoint[1];
    double z = inPoint[2];

    double RR = x*x + y*y;
    double r  = sqrt(RR + z*z);

    outPoint[0] = r;
    if (r == 0)
        outPoint[1] = 0;
    else
        outPoint[1] = acos(z/r);

    if (RR == 0)
        outPoint[2] = 0;
    else
    {
        // Change range to [0, 2*Pi], otherwise the same as atan2(y, x)
        outPoint[2] = vtkMath::Pi() + std::atan2(-y, -x);
    }
}


vtkSmartPointer<vtkPoints> convertToPhysicalCoordinates(vtkSmartPointer<vtkPoints> particles, int number_of_particles,
                                                        const itk::Image< float,  3 >::SpacingType& spacing ,
                                                        const itk::Image< float,  3 >::PointType& origin)
{
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (unsigned int ii = 0 ;ii < number_of_particles; ii++)
    {
        double p[3];

        particles->GetPoint(ii, p);

        points->InsertNextPoint(p[0] * spacing[0] + origin[0],
                p[1] * spacing[1] + origin[1],
                p[2] * spacing[2] + origin[2]);
        //ofs << p[0] << "," << p[1] << ","<< p[2] << std::endl;
    }
    //std::cout << "total number of correspondences used in TPS warping: " << points->GetNumberOfPoints() << std::endl;
    return points;
}

vtkSmartPointer<vtkPoints> convertToImageCoordinates(vtkSmartPointer<vtkPoints> particles, int number_of_particles,
                                                     const itk::Image< float,  3 >::SpacingType& spacing ,
                                                     const itk::Image< float,  3 >::PointType& origin)
{
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (unsigned int ii = 0 ;ii < number_of_particles; ii++)
    {
        //std::cout << ii << std::endl;
        double p[3];

        particles->GetPoint(ii, p);

        points->InsertNextPoint((p[0] - origin[0])/((double)spacing[0]),
                (p[1] - origin[1])/((double)spacing[1]),
                (p[2] - origin[2])/((double)spacing[2]));
        //ofs << p[0] << "," << p[1] << ","<< p[2] << std::endl;
    }
    //std::cout << "total number of correspondences used in TPS warping: " << points->GetNumberOfPoints() << std::endl;
    return points;
}


std::string num2str(float num)
{
    std::string Result;          // string which will contain the result
    std::ostringstream convert;   // stream used for the conversion
    convert << num;      // insert the textual representation of 'Number' in the characters in the stream
    Result = convert.str(); // set 'Result' to the contents of the stream
    return Result;
}

std::vector<double> linspace(double a, double b, size_t N)
{
    double h = (b - a) / static_cast<double>(N-1);
    std::vector<double> xs(N);
    typename std::vector<double>::iterator x;
    double val;
    for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
        *x = val;
    return xs;
}

std::string int2str(int n, int number_of_zeros)
{
    std::stringstream ss;
    ss << setw(number_of_zeros) << setfill('0') << n;
    std::string s = ss.str();

    return s;
}

}


