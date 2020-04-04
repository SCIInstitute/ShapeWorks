#include "Utils.h"

#include <vtkMath.h>
#include <cmath>
#include <sstream>
#include <sys/stat.h>

// namespace shapeworks {  //todo (need to make sure everything that calls these functions uses namespace

std::vector<int> Utils::randperm(int n)
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

void Utils::readSparseShape(vtkSmartPointer<vtkPoints>& points, char* filename, int number_of_particles)
{
    points->Reset();

    std::ifstream ifs;
    ifs.open(filename);
    if(!ifs.good())
        throw std::runtime_error("Could not open file for output: " + std::string(filename));

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
}

void Utils::readSparseShape(std::vector<itk::Point<double> > & points, char* filename, int number_of_particles)
{
    points.clear();
    std::ifstream ifs;
    ifs.open(filename);
    if(!ifs.good())
        throw std::runtime_error("Could not open file for output: " + std::string(filename));

    if (number_of_particles>0)
    {
        for (unsigned int ii = 0 ;ii < number_of_particles; ii++)
        {
            itk::Point<double> p;
            ifs >> p[0] >> p[1] >> p[2];

            points.push_back(p);
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

            itk::Point<double> p;
            p[0] = lineData[0]; p[1] = lineData[1]; p[2] = lineData[2];
            points.push_back(p);
        }
    }

    ifs.close();
    std::cout << "total number of correspondences read: " << points.size() << std::endl;
}


void Utils::writeSparseShape(char* filename, vtkSmartPointer<vtkPoints> particles)
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
    std::cout << "total number of correspondences saved: " << particles->GetNumberOfPoints() << std::endl;
}

void Utils::writeSparseShape(char* filename, std::vector<itk::Point<double, 3> > points)
{
    ofstream ofs;
    ofs.open(filename);
    for(unsigned int ii = 0 ; ii < points.size(); ii++)
    {
        itk::Point<double> p = points[ii];
        ofs << p[0] << " " << p[1] << " " << p[2] << std::endl;
    }
    ofs.close();
    std::cout << "total number of correspondences saved: " << points.size() << std::endl;
}


std::vector<int> Utils::readParticleIds(char* filename)
{
    std::ifstream ifs;
    ifs.open(filename);
    if(!ifs.good())
        throw std::runtime_error("Could not open file for output: " + std::string(filename));

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

void Utils::writeParticleIds(char* filename, std::vector<int> ids)
{
    ofstream ofs;
    ofs.open(filename);
    for(unsigned int ii = 0 ; ii < ids.size(); ii++)
    {
        ofs << ids[ii] << std::endl;
    }
    ofs.close();
}

//--------------- point cloud queries --------------------------------
void Utils::computeCenterOfMassForShapeEnsemble (std::vector< std::vector< itk::Point< double, 3 > > > points_list, itk::Point< double, 3 > & center)
{
    size_t numShapes = points_list.size();
    center[0] = 0.0; center[1] = 0.0; center[2] = 0.0;
    for (size_t shapeNo = 0; shapeNo < numShapes; shapeNo++)
    {
        itk::Point< double, 3 >  cur_center;
        std::vector< itk::Point< double, 3 > > cur_points = points_list[shapeNo];
        computeCenterOfMassForShape (cur_points, cur_center);

        center[0] += cur_center[0];
        center[1] += cur_center[1];
        center[2] += cur_center[2];
    }

    center[0] /= double(numShapes);
    center[1] /= double(numShapes);
    center[2] /= double(numShapes);
}

void Utils::computeCenterOfMassForShape (std::vector< itk::Point< double, 3 > >  points, itk::Point< double, 3 > & center)
{
    size_t numPoints = points.size();
    center[0] = 0.0; center[1] = 0.0; center[2] = 0.0;
    for (size_t ii = 0; ii < numPoints; ii++)
    {
        itk::Point< double, 3 > cur_point = points[ii];
        center[0] += cur_point[0];
        center[1] += cur_point[1];
        center[2] += cur_point[2];
    }

    center[0] /= double(numPoints);
    center[1] /= double(numPoints);
    center[2] /= double(numPoints);
}

void Utils::updateMin(double curVal, double& minVal)
{
    if(curVal < minVal)
        minVal = curVal;
}

void Utils::updateMax(double curVal, double& maxVal)
{
    if(curVal > maxVal)
        maxVal = curVal;
}

void Utils::getBoundingBoxForShapeEnsemble (std::vector< std::vector< itk::Point< double, 3 > > > points_list,
                                            double& min_x, double& min_y, double& min_z,
                                            double& max_x, double& max_y, double& max_z)
{
    size_t numShapes = points_list.size();

    min_x = 1e10;  min_y = 1e10;  min_z = 1e10;
    max_x = -1e10; max_y = -1e10; max_z = -1e10;

    for (size_t shapeNo = 0; shapeNo < numShapes; shapeNo++)
    {
        double cur_min_x, cur_min_y, cur_min_z;
        double cur_max_x, cur_max_y, cur_max_z;

        std::vector< itk::Point< double, 3 > > cur_points = points_list[shapeNo];
        getBoundingBoxForShape (cur_points,
                                cur_min_x, cur_min_y, cur_min_z,
                                cur_max_x, cur_max_y, cur_max_z);

        updateMax(cur_max_x, max_x);
        updateMax(cur_max_y, max_y);
        updateMax(cur_max_z, max_z);

        updateMin(cur_min_x, min_x);
        updateMin(cur_min_y, min_y);
        updateMin(cur_min_z, min_z);

    }

}

void Utils::getBoundingBoxForShape (std::vector< itk::Point< double, 3 > >  points,
                                    double& min_x, double& min_y, double& min_z,
                                    double& max_x, double& max_y, double& max_z)
{
    size_t numPoints = points.size();

    min_x = 1e10;  min_y = 1e10;  min_z = 1e10;
    max_x = -1e10; max_y = -1e10; max_z = -1e10;

    for (size_t ii = 0; ii < numPoints; ii++)
    {
        itk::Point< double, 3 > cur_point = points[ii];

        updateMax(cur_point[0], max_x);
        updateMax(cur_point[1], max_y);
        updateMax(cur_point[2], max_z);

        updateMin(cur_point[0], min_x);
        updateMin(cur_point[1], min_y);
        updateMin(cur_point[2], min_z);
    }
}


// ------------------- string manipulation ------------------------------------
std::string Utils::removeExtension( std::string const& filename )
{
    std::string::const_reverse_iterator
            pivot
            = std::find( filename.rbegin(), filename.rend(), '.' );
    return pivot == filename.rend()
            ? filename
            : std::string( filename.begin(), pivot.base() - 1 );
}

std::string Utils::getPath( std::string const& filename )
{
    std::string::const_reverse_iterator
            pivot
            = std::find( filename.rbegin(), filename.rend(), '/' );
    return pivot == filename.rend()
            ? filename
            : std::string( filename.begin(), pivot.base() - 1 );
}

std::string Utils::getFilename( std::string const& pathname )
{
    return std::string(
                std::find_if( pathname.rbegin(), pathname.rend(),
                              utils::MatchPathSeparator() ).base(),
                pathname.end() );
}

// ------------------- coordinates transformations -----------------------------
void Utils::spherical2cartesian(const double inPoint[3], double outPoint[3])
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

void Utils::cartesian2spherical(const double inPoint[3], double outPoint[3])
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


vtkSmartPointer<vtkPoints> Utils::convertToPhysicalCoordinates(vtkSmartPointer<vtkPoints> particles, int number_of_particles,
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

vtkSmartPointer<vtkPoints> Utils::convertToImageCoordinates(vtkSmartPointer<vtkPoints> particles, int number_of_particles,
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


std::string Utils::num2str(float num)
{
    std::string Result;          // string which will contain the result
    std::ostringstream convert;   // stream used for the conversion
    convert << num;      // insert the textual representation of 'Number' in the characters in the stream
    Result = convert.str(); // set 'Result' to the contents of the stream
    return Result;
}

std::string Utils::num2str(int num)
{
    std::string Result;          // string which will contain the result
    std::ostringstream convert;   // stream used for the conversion
    convert << num;      // insert the textual representation of 'Number' in the characters in the stream
    Result = convert.str(); // set 'Result' to the contents of the stream
    return Result;
}

std::vector<double> Utils::linspace(double a, double b, size_t N)
{
    double h = (b - a) / static_cast<double>(N-1);
    std::vector<double> xs(N);
    typename std::vector<double>::iterator x;
    double val;
    for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
        *x = val;
    return xs;
}

std::string Utils::int2str(int n, int number_of_zeros)
{
    std::stringstream ss;
    ss << setw(number_of_zeros) << setfill('0') << n;
    std::string s = ss.str();

    return s;
}


//--------------- average normal directions --------------------------------

// only good for positive numbers.
double Utils::mod2pi_pos(double vin)
{
    double q = vin * twopi_inv + 0.5;
    int qi = (int) q;

    return vin - qi*twopi;
}

// Ensure that v is [-PI, PI]
double Utils::mod2pi(double vin)
{
    double v;

    if (vin < 0)
        v = -1.0*mod2pi_pos(-1.0*vin);
    else
        v = mod2pi_pos(vin);

    // Validation test:
    //	if (v < -Math.PI || v > Math.PI)
    //		System.out.printf("%10.3f -> %10.3f\n", vin, v);

    return v;
}

/* Returns a value of v wrapped such that ref and v differ by no
     * more +/-PI
     */
double Utils::mod2pi(double ref, double v)
{
    return ref + mod2pi(v-ref);
}

/* For a given theta, compute the MSE. A simple O(N) method used for testing. */
double Utils::computeMSE(std::vector<double> thetas, double theta)
{
    double sqerr = 0;
    for(std::vector<double>::iterator it = thetas.begin(); it != thetas.end(); ++it)
    {
        double err = mod2pi((*it) - theta);
        sqerr += (err * err);
    }
    return (sqerr / ((double)thetas.size()));
}


/* Trying every theta (with a step size of dtheta), find the theta that results in the smallest MSE.
     */
double Utils::averageThetaBruteForce(std::vector<double> thetas, double dtheta)
{
    double bestmse = 1e10;
    double besttheta = 0;

    for (double theta = 0; theta < 2 * M_PI; theta += dtheta)
    {
        double mse = computeMSE(thetas, theta);

        if (mse < bestmse) {
            bestmse = mse;
            besttheta = theta;
        }
    }
    return mod2pi(besttheta);
}

// the chord method
double Utils::averageThetaChord(std::vector<double> thetas)
{
    double M = 0, N = 0;

    for(std::vector<double>::iterator it = thetas.begin(); it != thetas.end(); ++it)
    {
        double x0 = cos(*it);
        double y0 = sin(*it);
        M += y0;
        N += x0;

    }

    return atan2(M, N);
}

// the exact method
double Utils::averageThetaArc(std::vector<double> thetas)
{
    std::vector<double> x;

    for(std::vector<double>::iterator it = thetas.begin(); it != thetas.end(); ++it)
        x.push_back(mod2pi(*it)); // map to [-PI, PI]
    std::sort (x.begin(), x.end()); // ascending numerical order.

    // compute first and second moments without adding 2PI
    double A1 = 0, A2 = 0;

    size_t sz = x.size();
    for (size_t i = 0; i < sz; i++)
    {
        A1 += x[i];
        A2 += x[i]*x[i];
    }

    // now, go back through again, converting elements one-by-one
    // into their +2PI versions, recomputing the error and picking
    // the best one.

    // initialize with case where all points are the non-2PI
    // version.
    double bestError = 1e10;
    double bestTheta = -1;

    // note: i=-1 iteration tests case where none are flipped.
    for (int i = -1; i < int(sz); i++)
    {
        if (i >= 0) {
            // flip the i'th component into the +2PI version
            A1 += 2*M_PI;
            // A2 += (x[i] + 2*Math.PI)^2 - x[i]*x[i]
            A2 += 4*M_PI*x[i] + 4*M_PI*M_PI;
        }

        double theta = A1 / ((double)x.size());
        double error = A2 - 2*A1*theta + ((double)x.size())*theta*theta;

        if (error < bestError) {
            bestError = error;
            bestTheta = theta;
        }
    }

    return mod2pi(bestTheta);
}

// } //shapeworks
