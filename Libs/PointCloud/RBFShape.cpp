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

std::vector<double> RBFShape::batch_evaluate(const Eigen::MatrixXd & points){
    CPURBFEvaluator eval = CPURBFEvaluator();

    return eval.batchEvaluate(this->kernel, points, this->points_, this->TPSWeights, this->coeff_.head<3>(), this->coeff_(3));
}

Eigen::Vector3d RBFShape::gradient(const Eigen::Vector3d & point){

    Eigen::Vector3d grad = Eigen::Vector3d(0.,0.,0.);

    for (size_t i = 0; i < this->points_.rows(); i++) {
      Eigen::Vector3d p = this->points_.row(i).head<3>();
      Eigen::Vector3d s = (point - p);
      double r = s.norm();
      grad = grad + (s * (this->TPSWeights[i] * (this->kernel)->compute_gradient(r)));
    }

    return grad;
}

double RBFShape::mag_gradient(const Eigen::Vector3d & point){
    Eigen::Vector3d grad = gradient(point);

    double mag_gradient = (grad + coeff_.head<3>()).norm();

    return mag_gradient;
}

void RBFShape::solve_system(const Eigen::MatrixXd & points){
    EigenRBFSolver solver = EigenRBFSolver();
    this->points_ = points;

    this->PointSolutions = solver.solveForCoefficients(this->kernel, this->points_);

    size_t n = this->PointSolutions.size();
    auto first = this->PointSolutions.begin();
    auto last = this->PointSolutions.begin() + n - 4;
    this->TPSWeights = std::vector<double>(first, last);

    this->coeff_ = {this->PointSolutions[n - 3], this->PointSolutions[n - 2], this->PointSolutions[n - 1], this->PointSolutions[n - 4]};
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
        Eigen::Vector3d ptn = RawNormals.row(i).head<3>();

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
    Eigen::Vector4d TPSCoeff = this->coeff_;

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

//TODO? Handle a more complete loading
bool RBFShape::loadFromEqFile(const std::string& filename){
    std::cout << "Reading file..." << std::endl;
    //Reading infile
    std::string line;
    std::ifstream in(filename);
    std::vector<std::vector<double> > temp_vec;

    while (std::getline(in, line))  // this does the checking!
    {
        std::istringstream iss(line);
        std::vector<std::string> words{std::istream_iterator<std::string>{iss},
                          std::istream_iterator<std::string>{}};

        this->pows_ = Eigen::Vector3d(1.,1.,1.);
        if(words.size() == 4){
            std::vector<double> temp;
            for(size_t i = 0; i < words.size(); i++){temp.push_back(stod(words[i]));}
            temp_vec.push_back(temp);
        }
        else if (words.size() > 0 && words[0] == "Coeff" ){
            this->coeff_(0) = stod(words[1]);
            this->coeff_(1) = stod(words[2]);
            this->coeff_(2) = stod(words[3]);
            this->coeff_(3) = stod(words[4]);
            //std::cout << "Coeffs: " << this->coeff_ << std::endl;
        }
        else if (words.size() > 0 && words[0] == "KernelType" ){
            std::cout << "Kernel type: " << words[1] << std::endl;
            if (words[1] == "ThinPlateSpline "){
                this->kernel = new ThinPlateKernel();
            }
        }
        else if (words.size() > 0 && words[0] == "NumberOfPoints" ){
            std::cout << "Number of Points: " << words[1] << std::endl;
        }
        else{
            std::cerr << "Line did not comform the format; line <" << line << std::endl;
        }
    }

    in.close();

    this->points_ = Eigen::MatrixXd(temp_vec.size(), 4);

    for(size_t i = 0; i < temp_vec.size(); i++){

        this->points_(i, 0) = temp_vec[i][0];
        this->points_(i, 1) = temp_vec[i][1];
        this->points_(i, 2) = temp_vec[i][2];
        this->points_(i, 3) = 0.;

        this->TPSWeights.push_back(temp_vec[i][3]);
    }

    return true;
}

} // shapeworks
