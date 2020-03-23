#include <RBFShape.h>

namespace shapeworks {

RBFShape::RBFShape(Eigen::Vector4d coeff,
                   Eigen::Vector3d pows,
                   Eigen::MatrixXd points)
: coeff_(coeff), pows_(pows),
id_(0), material_(0), points_(points) {
    if(this->points_.size() == 0)
        this->isEmpty_ = true;
    else
        this->isEmpty_ = false;
    this->kernel = new ThinPlateKernel();
}

RBFShape::~RBFShape(){}

std::string RBFShape::ToDebugString() {
    return "name() = " + name() + ", id = " + std::to_string(getID()) + ", isEmtpy = " + std::to_string(isEmpty());
}

void RBFShape::setID(size_t id) { this->id_ = id; }

size_t RBFShape::getID() { return this->id_; }

double RBFShape::evaluate(const Eigen::Vector3d & point){
    CPURBFEvaluator eval = CPURBFEvaluator();

    return eval.evaluate(this->kernel, point, this->points_, this->TPSWeights, this->coeff_.head<3>(), this->coeff_(3));
}

Eigen::Vector3d RBFShape::gradient(const Eigen::Vector3d &point){

    Eigen::Vector3d grad = Eigen::Vector3d(0.,0.,0.);

    for (size_t i = 0; i < this->points_.size(); i++) {
      Eigen::Vector3d p = this->points_.row(i).head<3>();
      Eigen::Vector3d s = (point - p);
      double r = sqrt(pow(s(0), 2) + pow(s(1), 2) + pow(s(2), 2));
      grad = grad + (s * (this->TPSWeights[i] * (this->kernel)->compute_gradient(r)));
    }

    return Eigen::Vector3d(1.,1.,1.);
}

void RBFShape::solve_system(const Eigen::MatrixXd & points){
    EigenRBFSolver solver = EigenRBFSolver();
    this->points_ = points;

    this->TPSWeights = solver.solveForCoefficients(this->kernel, this->points_);

    size_t n = this->TPSWeights.size();
    this->coeff_ = {this->TPSWeights[n - 3], this->TPSWeights[n - 2], this->TPSWeights[n - 1], this->TPSWeights[n - 4]};
}

//Writes RBFShape into a Eq file format
void RBFShape::writeToEqFile(const std::string& filename, int precision){

    //Create a file and set decimal point precision
    std::ofstream file;
    file.open(filename);
    file << std::fixed;
    file << std::setprecision(precision);

    // by default all rbfs have tps part regardless whether csrbf is enabled or not
    //Get TPS part
    const auto& TPSPoints = getPoints();
    int TPSPointCount = TPSPoints.rows();
    const auto& TPSWeights = this->TPSWeights;
    int TPSWeightsCount = this->TPSWeights.size();

    //Check if vectors are empty
    if (TPSPointCount == 0)
    {
        std::cerr << "Error: RBF solutions were not completed. TPS points std::vector is empty.\n";
        return;
    }

    //Check if vectors are empty
    if (TPSWeights.empty())
    {
        std::cerr << "Error: RBF solutions were not completed. TPS weights std::vector is empty.\n";
        return;
    }

    //Check if the vectors are compatiable sizes
    const int linearCoeffNum = 4;
    if (TPSWeights.size() != TPSPointCount)
    {
        std::cout << "TPSWeights.size() = " << TPSWeights.size() << ", TPSPointCount = " << TPSPointCount << std::endl;
        std::cerr << "Error: TPS points and TPS weights vectors are not compatible lengths.\n";
        return;
    }
    //std::cout << "Num TPS points: " << TPSPointCount << "\n";
    //std::cout << "Num TPS weights: " << TPSWeightsCount << "\n\n";

    // Write the TPS to the file
    writeTPSdata(file);

    /*
    // write csrbf if it is enabled
    if (m_Parameters->m_bCompactSupported)
    {
        const auto& CSRBFPoints = GetCSRBFPoints();
        int CSRBFPointCount = GetCSRBFPoints().size();
        const auto& CSRBFWeights = GetCSRBFPoints();
        int CSRBFWeightsCount = GetCSRBFPoints().size();

        //Check if vectors are empty
        if (CSRBFPoints.empty())
        {
            std::cerr << "Error: RBF solutions were not completed. CSRBF points std::vector is empty while compact support is enabled.\n";
            return;
        }

        //Check if vectors are empty
        if (CSRBFWeights.empty())
        {
            std::cerr << "Error: RBF solutions were not completed. CSRBF weights std::vector is empty while compact support is enabled.\n";
            return;
        }

        if (CSRBFWeightsCount != CSRBFPointCount)
        {
            std::cerr << "Error: Number of CSRBF points and CSRBF weights are not compatible.\n";
            return;
        }

        std::cout << "\nNum CSRBF points: " << CSRBFPointCount << "\n";
        std::cout << "Num CSRBF weights: " << CSRBFWeightsCount << "\n";

        // Write the CSRBFs to the file
        writeCSRBFdata(file);
    }
    */

    file.close();
}

//Writes RBFShape into a Raw file format
void RBFShape::writeToRawFile(const std::string& filename, int precision){
    //Create a file and set decimal point precision
    std::ofstream file;
    file.open(filename);
    file << std::fixed;
    file << std::setprecision(precision);

    const Eigen::MatrixXd RawPoints = this->raw_points_;
    const Eigen::MatrixXd RawNormals = this->raw_normals_;

    //Check if vectors are empty
    if (RawPoints.rows() == 0)
    {
        std::cerr << "Error: Raw points std::vector is empty.\n";
        return;
    }

    if (RawPoints.rows() != RawNormals.rows())
    {
        std::cerr << "Error: Raw points and raw normals do not have the same number of elements.\n";
        return;
    }

    file << "NumberOfPoints " << RawPoints.rows() << "\n";
    for (int i = 0; i < RawPoints.rows(); i++)
    {
        Eigen::Vector4d pt = RawPoints.row(i);
        Eigen::Vector3d ptn = RawNormals.row(i);

        file << pt(0) << " " << pt(1) << " " << pt(2) << " "
            << ptn(0) << " " << ptn(1) << " " << ptn(2) << " "
            << pt(3) << "\n";
    }

    file.close();
}

void RBFShape::writeTPSdata(std::ofstream& file)
{
    const auto& TPSPoints = getPoints();
    int TPSPointCount = TPSPoints.rows();
    const auto& TPSWeights = this->TPSWeights;
    Eigen::Vector4d TPSCoeff = coeff();

    file << "KernelType ThinPlateSpline \n";

    //Write linear coefficients
    file << "Coeff ";
    file << TPSCoeff(0) << " " << TPSCoeff(1) << " " << TPSCoeff(2) << " " << TPSCoeff(3);
    file << "\n";

    //Write TPS points
    file << "NumberOfPoints " << TPSPointCount << "\n";
    for (int i = 0; i < TPSPointCount; i++)
    {
        file << TPSPoints(i,0) << " " << TPSPoints(i,1) << " " << TPSPoints(i,2) << " " << TPSWeights[i] << "\n";
    }
}

void RBFShape::write_csv(const std::string& filename, int precision){
    //Create a file and set decimal point precision
    std::ofstream file;
    file.open(filename);
    file << std::fixed;
    file << std::setprecision(precision);

    // by default all rbfs have tps part regardless whether csrbf is enabled or not
    //Get TPS part
    const auto& TPSPoints = getPoints();
    int TPSPointCount = TPSPoints.rows();
    const auto& TPSWeights = this->TPSWeights;
    int TPSWeightsCount = this->TPSWeights.size();

    for(size_t i = 0; i < TPSPointCount; i++){
        file << TPSPoints(i,0) << "," << TPSPoints(i,1) << "," << TPSPoints(i,2) << "," << TPSWeights[i] << "\n";
    }

    file.close();
}

} // shapeworks
