#include "Reconstruction.h"
#include <vtkKdTreePointLocator.h>
#include <vtkProbeFilter.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkContourFilter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkDecimatePro.h>
#include "PreViewMeshQC/FEVTKImport.h"
#include "PreViewMeshQC/FEVTKExport.h"
#include "PreViewMeshQC/FEFixMesh.h"
#include "PreViewMeshQC/FEMeshSmoothingModifier.h"
#include "PreViewMeshQC/FECVDDecimationModifier.h"
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <array>

Reconstruction::Reconstruction(float decimationPercent, double maxAngleDegrees) :
    denseDone_(false),
    decimationPercent_(decimationPercent),
    maxAngleDegrees_(maxAngleDegrees),
    numClusters_(5)
{}

Reconstruction::~Reconstruction() {
}

vtkSmartPointer<vtkPolyData> Reconstruction::getDenseMean(
        std::vector<std::vector<itk::Point<float> > > local_pts,
        std::vector<std::vector<itk::Point<float> > > global_pts,
        std::vector<ImageType::Pointer> distance_transform) {
    if (!this->denseDone_ || !local_pts.empty() ||
            !distance_transform.empty() || !global_pts.empty()) {
        this->denseDone_ = false;
        if (local_pts.empty() || distance_transform.empty() ||
                global_pts.empty() || local_pts.size() != distance_transform.size()) {
            throw std::runtime_error("Invalid input for reconstruction!");
        }
        this->computeDenseMean(local_pts, global_pts, distance_transform);
    }
    return this->denseMean_;
}

void Reconstruction::reset() {
    this->denseDone_ = false;
    this->goodPoints_.clear();
    this->denseMean_ = NULL;
    this->sparseMean_ = NULL;
}

void Reconstruction::setDecimation(float dec) {
    this->decimationPercent_ = dec;
}

void Reconstruction::setMaxAngle(double angleDegrees) {
    this->maxAngleDegrees_ = angleDegrees;
}

void Reconstruction::setNumClusters(int num) {
    this->numClusters_ = num;
}

vtkSmartPointer<vtkPolyData> Reconstruction::getMesh(
        std::vector<itk::Point<float> > local_pts) {
    //default reconstruction if no warping to dense mean has occurred yet
    if (!this->denseDone_) {
        return vtkSmartPointer<vtkPolyData>::New();
    }
    //we have a dense mean, so lets warp to subject space
    std::vector<int> particles_indices;
    for (int i = 0; i < this->goodPoints_.size(); i++) {
        if (this->goodPoints_[i]) {
            particles_indices.push_back(i);
        }
    }
    vtkSmartPointer< vtkPoints > subjectPts = vtkSmartPointer<vtkPoints>::New();
    for (auto &a : local_pts) {
        subjectPts->InsertNextPoint(a[0], a[1], a[2]);
    }
    double sigma = this->computeAverageDistanceToNeighbors(subjectPts,
                                                           particles_indices);
    // (3) set up the common source points for all warps
    PointIdType id;
    // NOTE that, since we are not resampling images, this warping is a
    // forward warping from mean space to subject space
    // Define container for source landmarks that corresponds to the mean space, this is
    // the moving mesh which will be warped to each individual subject
    PointSetType::Pointer sourceLandMarks = PointSetType::New();
    PointSetType::PointsContainer::Pointer sourceLandMarkContainer =
            sourceLandMarks->GetPoints();
    PointType ps;
    id = itk::NumericTraits< PointIdType >::Zero;
    int ns = 0;
    for (unsigned int ii = 0; ii < local_pts.size(); ii++) {
        if (std::find(particles_indices.begin(),
                      particles_indices.end(), ii) != particles_indices.end()) {
            double p[3];
            this->sparseMean_->GetPoint(ii, p);
            ps[0] = p[0];
            ps[1] = p[1];
            ps[2] = p[2];
            sourceLandMarkContainer->InsertElement(id++, ps);
            ns++;
        }
    }
    RBFTransformType::Pointer rbfTransform = RBFTransformType::New();
    rbfTransform->SetSigma(sigma); // smaller means more sparse
    rbfTransform->SetStiffness(1e-10);
    rbfTransform->SetSourceLandmarks(sourceLandMarks);
    // Define container for target landmarks corresponds to the subject shape
    PointSetType::Pointer targetLandMarks = PointSetType::New();
    PointType pt;
    PointSetType::PointsContainer::Pointer targetLandMarkContainer =
            targetLandMarks->GetPoints();
    id = itk::NumericTraits< PointIdType >::Zero;
    int nt = 0;
    for (unsigned int ii = 0; ii < local_pts.size(); ii++) {
        if (std::find(particles_indices.begin(),
                      particles_indices.end(), ii) != particles_indices.end()) {
            double p[3];
            subjectPts->GetPoint(ii, p);
            pt[0] = p[0];
            pt[1] = p[1];
            pt[2] = p[2];
            targetLandMarkContainer->InsertElement(id++, pt);
            nt++;
        }
    }
    rbfTransform->SetTargetLandmarks(targetLandMarks);
    // check the mapping (inverse here)
    // this means source points (current sample's space) should
    //  be warped to the target (mean space)
    vtkSmartPointer<vtkPoints> mappedCorrespondences =
            vtkSmartPointer<vtkPoints>::New();
    double rms;
    double rms_wo_mapping;
    double maxmDist;
    this->CheckMapping(this->sparseMean_, subjectPts,
                       rbfTransform, mappedCorrespondences, rms, rms_wo_mapping, maxmDist);
    vtkSmartPointer<vtkPolyData> denseShape = vtkSmartPointer<vtkPolyData>::New();
    denseShape->DeepCopy(this->denseMean_);
    this->generateWarpedMeshes(rbfTransform, denseShape);
    return denseShape;
}

void Reconstruction::readMeanInfo(std::string dense,
                                  std::string sparse, std::string goodPoints) {
    //read out dense mean
    vtkSmartPointer<vtkPolyDataReader> reader1 = vtkPolyDataReader::New();
    reader1->SetFileName(dense.c_str());
    reader1->Update();
    this->denseMean_ = reader1->GetOutput();
    //read out sparse mean
    std::ifstream ptsIn0(sparse.c_str());
    this->sparseMean_ = vtkSmartPointer<vtkPoints>::New();
    while (ptsIn0.good()) {
        double x, y, z;
        ptsIn0 >> x >> y >> z;
        if (ptsIn0.good()) {
            this->sparseMean_->InsertNextPoint(x, y, z);
        }
    }
    ptsIn0.close();
    //read out good points
    std::ifstream ptsIn(goodPoints.c_str());
    this->goodPoints_.clear();
    while (ptsIn.good()) {
        int i;
        ptsIn >> i;
        if (ptsIn.good()) {
            this->goodPoints_.push_back(i == 0 ? false : true);
        }
    }
    ptsIn.close();
    this->denseDone_ = true;
}


bool Reconstruction::denseDone() {
    return this->denseDone_;
}

void Reconstruction::computeDenseMean(
        std::vector<std::vector<itk::Point<float> > > local_pts,
        std::vector<std::vector<itk::Point<float> > > global_pts,
        std::vector<ImageType::Pointer> distance_transform) {
    try {
        //turn the sets of global points to one sparse global mean.
        float init[] = { 0.f,0.f,0.f };
        std::vector<itk::Point<float> > sparseMean =
                std::vector<itk::Point<float> >(global_pts[0].size(), itk::Point<float>(init));
        for (auto &a : global_pts) {
            for (size_t i = 0; i < a.size(); i++) {
                init[0] = a[i][0]; init[1] = a[i][1]; init[2] = a[i][2];
                itk::Vector<float> vec(init);
                sparseMean[i] = sparseMean[i] + vec;
            }
        }
        auto div = static_cast<float>(global_pts.size());
        for (size_t i = 0; i < sparseMean.size(); i++) {
            init[0] = sparseMean[i][0] / div;
            init[1] = sparseMean[i][1] / div;
            init[2] = sparseMean[i][2] / div;
            sparseMean[i] = itk::Point<float>(init);
        }
        std::vector<vnl_matrix<double> > normals;
        std::vector<vtkSmartPointer< vtkPoints > > subjectPts;
        this->sparseMean_ = vtkSmartPointer<vtkPoints>::New();
        for (auto &a : sparseMean) {
            this->sparseMean_->InsertNextPoint(a[0], a[1], a[2]);
        }
        for (size_t shape = 0; shape < local_pts.size(); shape++) {
            subjectPts.push_back(vtkSmartPointer<vtkPoints>::New());
            for (auto &a : local_pts[shape]) {
                subjectPts[shape]->InsertNextPoint(a[0], a[1], a[2]);
            }
            //calculate the normals from the DT
            normals.push_back(this->computeParticlesNormals(
                                  subjectPts[shape], distance_transform[shape]));
        }
        // now decide whether each particle is a good
        // (it normals are in the same direction accross shapes) or
        // bad (there is discrepency in the normal directions across shapes)
        this->goodPoints_.resize(local_pts[0].size(), true);
        for (unsigned int ii = 0; ii < local_pts[0].size(); ii++) {
            bool isGood = true;

            // the normal of the first shape
            double nx_jj = normals[0](ii, 0);
            double ny_jj = normals[0](ii, 1);
            double nz_jj = normals[0](ii, 2);

            // start from the second
            for (unsigned int shapeNo_kk = 1; shapeNo_kk <
                 local_pts.size(); shapeNo_kk++) {
                double nx_kk = normals[shapeNo_kk](ii, 0);
                double ny_kk = normals[shapeNo_kk](ii, 1);
                double nz_kk = normals[shapeNo_kk](ii, 2);

                this->goodPoints_[ii] = this->goodPoints_[ii] &&
                        ((nx_jj*nx_kk + ny_jj*ny_kk + nz_jj*nz_kk) >
                         std::cos(this->maxAngleDegrees_ * M_PI / 180.));
            }
        }
        // decide which correspondences will be used to build the warp
        std::vector<int> particles_indices;
        particles_indices.clear();
        for (unsigned int kk = 0; kk < this->goodPoints_.size(); kk++) {
            if (this->goodPoints_[kk]) {
                particles_indices.push_back(kk);
            }
        }
        std::cout << "There are " << particles_indices.size() << " / " << this->goodPoints_.size() <<
                     " good points." << std::endl;

        const ImageType::SpacingType& spacing = distance_transform[0]->GetSpacing();
        const ImageType::PointType& origin = distance_transform[0]->GetOrigin();
        const ImageType::DirectionType& direction = distance_transform[0]->GetDirection();
        ImageType::SizeType size = distance_transform[0]->GetLargestPossibleRegion().GetSize();
        ImageType::RegionType region = distance_transform[0]->GetBufferedRegion();

        ImageType::Pointer meanDistanceTransform = ImageType::New();
        meanDistanceTransform->SetOrigin(origin);
        meanDistanceTransform->SetSpacing(spacing);
        meanDistanceTransform->SetDirection(direction);
        meanDistanceTransform->SetLargestPossibleRegion(size);

        ImageType::Pointer meanDistanceTransformBeforeWarp = ImageType::New();
        meanDistanceTransformBeforeWarp->SetOrigin(origin);
        meanDistanceTransformBeforeWarp->SetSpacing(spacing);
        meanDistanceTransformBeforeWarp->SetDirection(direction);
        meanDistanceTransformBeforeWarp->SetLargestPossibleRegion(size);

        AddImageFilterType::Pointer sumfilter = AddImageFilterType::New();
        AddImageFilterType::Pointer sumfilterBeforeWarp = AddImageFilterType::New();


        // Define container for source landmarks that corresponds to the mean space, this is
        // fixed where the target (each individual shape) will be warped to
        // NOTE that this is inverse warping to avoid holes in the warped distance transforms
        PointSetType::Pointer sourceLandMarks = PointSetType::New();
        PointSetType::PointsContainer::Pointer sourceLandMarkContainer = sourceLandMarks->GetPoints();
        PointType ps;
        PointIdType id = itk::NumericTraits< PointIdType >::Zero;
        int ns = 0;
        for (unsigned int ii = 0; ii < local_pts[0].size(); ii++) {
            if (std::find(particles_indices.begin(),
                          particles_indices.end(), ii) != particles_indices.end()) {
                double p[3];
                this->sparseMean_->GetPoint(ii, p);
                ps[0] = p[0];
                ps[1] = p[1];
                ps[2] = p[2];
                sourceLandMarkContainer->InsertElement(id++, ps);
                ns++;
            }
        }


        double sigma = computeAverageDistanceToNeighbors(
                    this->sparseMean_, particles_indices);

        // the roles of the source and target are reversed to simulate a reverse warping
        // without explicitly invert the warp in order to avoid holes in the warping result
        RBFTransformType::Pointer rbfTransform = RBFTransformType::New();
        rbfTransform->SetSigma(sigma); // smaller means more sparse
        //rbfTransform->SetStiffness(0.25*sigma);
        rbfTransform->SetStiffness(1e-10);

        rbfTransform->SetSourceLandmarks(sourceLandMarks);


        //////////////////////////////////////////////////////////////////
        //Praful - get the shape indices corresponding to cetroids of
        //kmeans clusters and run the following loop on only those shapes
        // Read input shapes from file list
        std::vector<int> centroidIndices;
        if (this->numClusters_ > 0 && this->numClusters_ < global_pts.size()) {
            this->performKMeansClustering(global_pts, global_pts[0].size(), centroidIndices);

        } else {
            this->numClusters_ = distance_transform.size();
            centroidIndices.resize(distance_transform.size());
            for (size_t shapeNo = 0; shapeNo < distance_transform.size(); shapeNo++) {
                centroidIndices[shapeNo] = shapeNo;
                std::cout << centroidIndices[shapeNo] << std::endl;
            }
        }
        //////////////////////////////////////////////////////////////////
        //Praful - clustering
        for (unsigned int cnt = 0; cnt < centroidIndices.size(); cnt++) {
            unsigned int shape = centroidIndices[cnt];
            auto dt = distance_transform[shape];
            PointSetType::Pointer targetLandMarks = PointSetType::New();
            PointType pt;
            PointSetType::PointsContainer::Pointer
                    targetLandMarkContainer = targetLandMarks->GetPoints();
            id = itk::NumericTraits< PointIdType >::Zero;

            int nt = 0;
            for (unsigned int ii = 0; ii < local_pts[0].size(); ii++) {
                if (std::find(particles_indices.begin(),
                              particles_indices.end(), ii) != particles_indices.end()) {
                    double p[3];
                    subjectPts[shape]->GetPoint(ii, p);
                    pt[0] = p[0];
                    pt[1] = p[1];
                    pt[2] = p[2];
                    targetLandMarkContainer->InsertElement(id++, pt);
                    nt++;
                }
            }
            rbfTransform->SetTargetLandmarks(targetLandMarks);
            // check the mapping (inverse here)
            // this mean source points (mean space) should
            // be warped to the target (current sample's space)
            vtkSmartPointer<vtkPoints> mappedCorrespondences = vtkSmartPointer<vtkPoints>::New();
            double rms;
            double rms_wo_mapping;
            double maxmDist;
            this->CheckMapping(this->sparseMean_, subjectPts[shape], rbfTransform,
                               mappedCorrespondences, rms, rms_wo_mapping, maxmDist);

            ResampleFilterType::Pointer   resampler = ResampleFilterType::New();
            InterpolatorType::Pointer interpolator = InterpolatorType::New();
            interpolator->SetSplineOrder(3);
            resampler->SetInterpolator(interpolator);

            resampler->SetOutputSpacing(spacing);
            resampler->SetOutputDirection(direction);
            resampler->SetOutputOrigin(origin);
            resampler->SetSize(size);
            resampler->SetTransform(rbfTransform);
            resampler->SetDefaultPixelValue((PixelType)-100.0);
            resampler->SetOutputStartIndex(region.GetIndex());
            resampler->SetInput(distance_transform[shape]);
            resampler->Update();

            if (cnt == 0) {
                // after warp
                DuplicatorType::Pointer duplicator = DuplicatorType::New();
                duplicator->SetInputImage(resampler->GetOutput());
                duplicator->Update();
                meanDistanceTransform = duplicator->GetOutput();
                // before warp
                DuplicatorType::Pointer duplicator2 = DuplicatorType::New();
                duplicator2->SetInputImage(distance_transform[shape]);
                duplicator2->Update();
                meanDistanceTransformBeforeWarp = duplicator2->GetOutput();
            } else {
                // after warp
                sumfilter->SetInput1(meanDistanceTransform);
                sumfilter->SetInput2(resampler->GetOutput());
                sumfilter->Update();

                DuplicatorType::Pointer duplicator = DuplicatorType::New();
                duplicator->SetInputImage(sumfilter->GetOutput());
                duplicator->Update();
                meanDistanceTransform = duplicator->GetOutput();

                // before warp
                sumfilterBeforeWarp->SetInput1(meanDistanceTransformBeforeWarp);
                sumfilterBeforeWarp->SetInput2(distance_transform[shape]);
                sumfilterBeforeWarp->Update();

                DuplicatorType::Pointer duplicator2 = DuplicatorType::New();
                duplicator2->SetInputImage(sumfilterBeforeWarp->GetOutput());
                duplicator2->Update();
                meanDistanceTransformBeforeWarp = duplicator2->GetOutput();
            }
        }
        MultiplyByConstantImageFilterType::Pointer multiplyImageFilter =
                MultiplyByConstantImageFilterType::New();
        multiplyImageFilter->SetInput(meanDistanceTransform);
        multiplyImageFilter->SetConstant(1.0 /
                                         static_cast<double>(this->numClusters_));
        multiplyImageFilter->Update();
        MultiplyByConstantImageFilterType::Pointer multiplyImageFilterBeforeWarp =
                MultiplyByConstantImageFilterType::New();
        multiplyImageFilterBeforeWarp->SetInput(meanDistanceTransformBeforeWarp);
        multiplyImageFilterBeforeWarp->SetConstant(1.0 /
                                                   static_cast<double>(this->numClusters_));
        multiplyImageFilterBeforeWarp->Update();
        // going to vtk to extract the template mesh (mean dense shape)
        // to be deformed for each sparse shape
        ITK2VTKConnectorType::Pointer itk2vtkConnector = ITK2VTKConnectorType::New();
        itk2vtkConnector->SetInput(multiplyImageFilter->GetOutput());
        itk2vtkConnector->Update();
        this->denseMean_ =
                this->extractIsosurface(itk2vtkConnector->GetOutput());
        this->denseMean_ = this->MeshQC(this->denseMean_);
    } catch (std::runtime_error e) {
        if (this->denseMean_ != NULL) {
            this->denseDone_ = true;
            throw std::runtime_error("Warning! MeshQC failed, but a dense mean was computed by VTK.");
        }
    } catch (std::exception e) {
        throw e;
    } catch (...) {
        throw std::runtime_error("Optimize failed!");
    }
    this->denseDone_ = true;
}

vnl_matrix<double> Reconstruction::computeParticlesNormals(
        vtkSmartPointer< vtkPoints > particles,
        ImageType::Pointer distance_transform) {
    const ImageType::SpacingType& spacing = distance_transform->GetSpacing();
    const ImageType::PointType& origin = distance_transform->GetOrigin();

    // (2) get normals at each voxel from the implicit representation
    // Create and setup a gradient filter
    GradientFilterType::Pointer gradientFilter = GradientFilterType::New();
    gradientFilter->SetInput(distance_transform);
    gradientFilter->Update();

    GradientMagnitudeFilterType::Pointer gradientMagFilter = GradientMagnitudeFilterType::New();
    gradientMagFilter->SetInput(distance_transform);
    gradientMagFilter->Update();

    ImageType::Pointer gradMagImage = ImageType::New();
    gradMagImage = gradientMagFilter->GetOutput();

    GradientImageType::Pointer  gradientImage = GradientImageType::New();
    gradientImage = gradientFilter->GetOutput();

    // iterate through the gradient to convert it to normal = -gradient/mag(gradient)
    GradientImageIteratorType gradIter(gradientImage, gradientImage->GetRequestedRegion());
    ImageIteratorType magIter(gradMagImage, gradMagImage->GetRequestedRegion());

    // initialize the images that will hold the normal components
    ImageType::Pointer nxImage = ImageType::New();
    nxImage->SetRegions(distance_transform->GetLargestPossibleRegion());
    nxImage->Allocate();
    ImageIteratorType nxIter(nxImage, nxImage->GetRequestedRegion());

    ImageType::Pointer nyImage = ImageType::New();
    nyImage->SetRegions(distance_transform->GetLargestPossibleRegion());
    nyImage->Allocate();
    ImageIteratorType nyIter(nyImage, nyImage->GetRequestedRegion());

    ImageType::Pointer nzImage = ImageType::New();
    nzImage->SetRegions(distance_transform->GetLargestPossibleRegion());
    nzImage->Allocate();
    ImageIteratorType nzIter(nzImage, nzImage->GetRequestedRegion());

    for (gradIter.GoToBegin(), magIter.GoToBegin(), nxIter.GoToBegin(),
         nyIter.GoToBegin(), nzIter.GoToBegin(); !gradIter.IsAtEnd();
         ++gradIter, ++magIter, ++nxIter, ++nyIter, ++nzIter) {
        itk::CovariantVector<float, 3> grad = gradIter.Get();
        float                      gradMag = magIter.Get();

        float nx = -1.0*grad[0] / (1e-10 + gradMag);
        float ny = -1.0*grad[1] / (1e-10 + gradMag);
        float nz = -1.0*grad[2] / (1e-10 + gradMag);

        nxIter.Set(nx);
        nyIter.Set(ny);
        nzIter.Set(nz);
    }

    // going to vtk for probing ...
    ITK2VTKConnectorType::Pointer connector_x = ITK2VTKConnectorType::New();
    connector_x->SetInput(nxImage);
    connector_x->Update();

    vtkSmartPointer<vtkImageData> Nx = vtkSmartPointer<vtkImageData>::New();
    Nx = connector_x->GetOutput();

    ITK2VTKConnectorType::Pointer connector_y = ITK2VTKConnectorType::New();
    connector_y->SetInput(nyImage);
    connector_y->Update();

    vtkSmartPointer<vtkImageData> Ny = vtkSmartPointer<vtkImageData>::New();
    Ny = connector_y->GetOutput();

    ITK2VTKConnectorType::Pointer connector_z = ITK2VTKConnectorType::New();
    connector_z->SetInput(nzImage);
    connector_z->Update();

    vtkSmartPointer<vtkImageData> Nz = vtkSmartPointer<vtkImageData>::New();
    Nz = connector_z->GetOutput();

    vtkSmartPointer<vtkPolyData> particlesData =
            vtkSmartPointer<vtkPolyData>::New();
    particlesData->SetPoints(particles);

    vtkSmartPointer<vtkPoints> pts =
            this->convertToImageCoordinates(particlesData->GetPoints(),
                                            particlesData->GetPoints()->GetNumberOfPoints(), spacing, origin);
    particlesData->SetPoints(pts);

    // (4) get the normals by probing the DT-based normal computation
    vtkSmartPointer<vtkProbeFilter> probe_x = vtkSmartPointer<vtkProbeFilter>::New();
#if (VTK_MAJOR_VERSION < 6)
    probe_x->SetInput(particlesData);
    probe_x->SetSource(Nx);
#else
    probe_x->SetInputData(particlesData);
    probe_x->SetSourceData(Nx);
#endif
    probe_x->Update();

    vtkSmartPointer<vtkProbeFilter> probe_y = vtkSmartPointer<vtkProbeFilter>::New();
#if (VTK_MAJOR_VERSION < 6)
    probe_y->SetInput(particlesData);
    probe_y->SetSource(Ny);
#else
    probe_y->SetInputData(particlesData);
    probe_y->SetSourceData(Ny);
#endif
    probe_y->Update();

    vtkSmartPointer<vtkProbeFilter> probe_z = vtkSmartPointer<vtkProbeFilter>::New();
#if (VTK_MAJOR_VERSION < 6)
    probe_z->SetInput(particlesData);
    probe_z->SetSource(Nz);
#else
    probe_z->SetInputData(particlesData);
    probe_z->SetSourceData(Nz);
#endif
    probe_z->Update();
    vtkFloatArray* nx = vtkFloatArray::SafeDownCast(
                probe_x->GetOutput()->GetPointData()->GetScalars());
    vtkFloatArray* ny = vtkFloatArray::SafeDownCast(
                probe_y->GetOutput()->GetPointData()->GetScalars());
    vtkFloatArray* nz = vtkFloatArray::SafeDownCast(
                probe_z->GetOutput()->GetPointData()->GetScalars());

    // Set point normals
    vtkSmartPointer<vtkDoubleArray> pointNormalsArray =
            vtkSmartPointer<vtkDoubleArray>::New();
    pointNormalsArray->SetNumberOfComponents(3); //3d normals (ie x,y,z)
    pointNormalsArray->SetNumberOfTuples(particlesData->GetNumberOfPoints());

    vnl_matrix<double> particlesNormals(particles->GetNumberOfPoints(), 3);
    for (unsigned int ii = 0; ii < particlesData->GetNumberOfPoints(); ii++) {
        double pN[3];

        // getting the normals at particles
        pN[0] = nx->GetValue(ii);
        pN[1] = ny->GetValue(ii);
        pN[2] = nz->GetValue(ii);

        // making sure this is a unit vector
        double norm = sqrt(pN[0] * pN[0] + pN[1] * pN[1] + pN[2] * pN[2]);
        pN[0] = pN[0] / norm;
        pN[1] = pN[1] / norm;
        pN[2] = pN[2] / norm;

        particlesNormals(ii, 0) = pN[0];
        particlesNormals(ii, 1) = pN[1];
        particlesNormals(ii, 2) = pN[2];

        double p[3];
        particlesData->GetPoint(ii, p);

        // Add the data to the normals array
        pointNormalsArray->SetTuple(ii, pN);

    }

    // Add the normals to the points in the polydata
    particlesData->GetPointData()->SetNormals(pointNormalsArray);

    vtkSmartPointer<vtkPoints> pts2 =
            this->convertToPhysicalCoordinates(particlesData->GetPoints(),
                                               particlesData->GetPoints()->GetNumberOfPoints(), spacing, origin);
    particlesData->SetPoints(pts2);
    return particlesNormals;
}

void Reconstruction::generateWarpedMeshes(
        RBFTransformType::Pointer transform,
        vtkSmartPointer<vtkPolyData>& outputMesh) {
    // generate warped meshes
    vtkSmartPointer<vtkPoints> vertices = vtkSmartPointer<vtkPoints>::New();
    vertices->DeepCopy(outputMesh->GetPoints());
    unsigned int numPointsToTransform = vertices->GetNumberOfPoints();
    for (unsigned int i = 0; i < numPointsToTransform; i++)
    {
        double meshPoint[3];
        vertices->GetPoint(i, meshPoint);

        itk::Point<double, 3> pm_;
        itk::Point<double, 3> pw_;

        pm_[0] = meshPoint[0]; pm_[1] = meshPoint[1]; pm_[2] = meshPoint[2];

        pw_ = transform->TransformPoint(pm_);

        vertices->SetPoint(i, pw_[0], pw_[1], pw_[2]);
    }
    outputMesh->SetPoints(vertices);
    outputMesh->Modified();
}

double Reconstruction::computeAverageDistanceToNeighbors(
        vtkSmartPointer<vtkPoints> points, std::vector<int> particles_indices) {
    int K = 6; // hexagonal ring - one jump
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);

    // Create the tree
    vtkSmartPointer<vtkKdTreePointLocator> kDTree =
            vtkSmartPointer<vtkKdTreePointLocator>::New();
    kDTree->SetDataSet(polydata);
    kDTree->BuildLocator();

    // Find the closest points to each particle
    double avgDist = 0;
    for (unsigned int ii = 0; ii < particles_indices.size(); ii++)
    {
        double p[3];
        points->GetPoint(particles_indices[ii], p);

        vtkSmartPointer<vtkIdList> result = vtkSmartPointer<vtkIdList>::New();
        kDTree->FindClosestNPoints(K + 1, p, result); // +1 to exclude myself

        double meanDist = 0;
        for (vtkIdType k = 0; k < K + 1; k++)
        {
            vtkIdType pid = result->GetId(k);

            if (pid == particles_indices[ii]) // close to myself
                continue;

            double pk[3];
            points->GetPoint(pid, pk);

            double curDist = sqrt(pow(p[0] - pk[0], 2.0) + pow(p[1] - pk[1], 2.0) +
                    pow(p[2] - pk[2], 2.0));

            meanDist += curDist;
        }
        meanDist /= K;

        avgDist += meanDist;
    }
    avgDist /= particles_indices.size();
    return avgDist;
}

void Reconstruction::CheckMapping(vtkSmartPointer<vtkPoints> sourcePts,
                                  vtkSmartPointer<vtkPoints> targetPts, RBFTransformType::Pointer rbfTransform,
                                  vtkSmartPointer<vtkPoints>& mappedCorrespondences, double & rms,
                                  double & rms_wo_mapping, double & maxmDist) {
    // source should be warped to the target
    rms = 0.0;
    rms_wo_mapping = 0.0;
    for (unsigned int ii = 0; ii < sourcePts->GetNumberOfPoints(); ii++)
    {
        double ps[3]; // source
        double pt[3]; // target
        double pw[3]; // warped

        sourcePts->GetPoint(ii, ps);
        targetPts->GetPoint(ii, pt);

        itk::Point<double, 3> ps_;
        itk::Point<double, 3> pt_;
        itk::Point<double, 3> pw_;

        ps_[0] = ps[0]; ps_[1] = ps[1]; ps_[2] = ps[2];
        pt_[0] = pt[0]; pt_[1] = pt[1]; pt_[2] = pt[2];

        pw_ = rbfTransform->TransformPoint(ps_);

        double cur_rms = pw_.EuclideanDistanceTo(pt_);
        double cur_rms_wo_mapping = ps_.EuclideanDistanceTo(pt_);

        rms += cur_rms;
        rms_wo_mapping += cur_rms_wo_mapping;

        pw[0] = pw_[0]; pw[1] = pw_[1]; pw[2] = pw_[2];
        mappedCorrespondences->InsertNextPoint(pw);

    }
    maxmDist = -10000.0f;
    for (unsigned int ii = 0; ii < mappedCorrespondences->GetNumberOfPoints(); ii++) {
        double pi[3];
        mappedCorrespondences->GetPoint(ii, pi);
        itk::Point<double, 3> pi_;
        pi_[0] = pi[0]; pi_[1] = pi[1]; pi_[2] = pi[2];
        for (unsigned int jj = 0; jj < mappedCorrespondences->GetNumberOfPoints(); jj++) {
            double pj[3];
            mappedCorrespondences->GetPoint(jj, pj);
            itk::Point<double, 3> pj_;
            pj_[0] = pj[0]; pj_[1] = pj[1]; pj_[2] = pj[2];
            double dist = pi_.EuclideanDistanceTo(pj_);
            if (dist > maxmDist) {
                maxmDist = dist;
            }
        }
    }
    rms /= sourcePts->GetNumberOfPoints();
    rms_wo_mapping /= sourcePts->GetNumberOfPoints();
}

vtkSmartPointer<vtkPoints> Reconstruction::convertToImageCoordinates(
        vtkSmartPointer<vtkPoints> particles, int number_of_particles,
        const itk::Image< float, 3 >::SpacingType& spacing,
        const itk::Image< float, 3 >::PointType& origin) {
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (unsigned int ii = 0; ii < number_of_particles; ii++) {
        double p[3];
        particles->GetPoint(ii, p);
        points->InsertNextPoint((p[0] - origin[0]) / ((double)spacing[0]),
                (p[1] - origin[1]) / ((double)spacing[1]),
                (p[2] - origin[2]) / ((double)spacing[2]));
    }
    return points;
}

vtkSmartPointer<vtkPoints> Reconstruction::convertToPhysicalCoordinates(
        vtkSmartPointer<vtkPoints> particles, int number_of_particles,
        const itk::Image< float, 3 >::SpacingType& spacing,
        const itk::Image< float, 3 >::PointType& origin) {
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (unsigned int ii = 0; ii < number_of_particles; ii++) {
        double p[3];
        particles->GetPoint(ii, p);
        points->InsertNextPoint(p[0] * spacing[0] + origin[0],
                p[1] * spacing[1] + origin[1],
                p[2] * spacing[2] + origin[2]);
    }
    return points;
}

vtkSmartPointer<vtkPolyData> Reconstruction::extractIsosurface(
        vtkSmartPointer<vtkImageData> volData) {
    // (1) isosurface generation
    vtkSmartPointer<vtkContourFilter> ls = vtkSmartPointer<vtkContourFilter>::New();
    //vtkSmartPointer<vtkMarchingCubes> ls = vtkSmartPointer<vtkMarchingCubes>::New();
    //ls->SetInputConnection( reader->GetOutputPort() );
#if (VTK_MAJOR_VERSION < 6)
    ls->SetInput(volData);
#else
    ls->SetInputData(volData);
#endif
    ls->SetValue(0, 0.);
    ls->Update();

    // (2) laplacian smoothing
    // Description:
    // Construct object with number of iterations as input; relaxation factor .01;
    // feature edge smoothing turned off; feature
    // angle 45 degrees; edge angle 15 degrees; and boundary smoothing turned
    // on. The convergence criterion is 0.0 of the bounding box diagonal.
    vtkSmartPointer<vtkSmoothPolyDataFilter> lsSmoother =
            vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
    lsSmoother->SetInputConnection(ls->GetOutputPort());
    lsSmoother->SetNumberOfIterations(1);
    lsSmoother->Update();
    std::cout << "..";

    // (3) largest connected component (assuming only a single domain shape)
    vtkSmartPointer<vtkPolyDataConnectivityFilter> conn =
            vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
    conn->SetExtractionModeToLargestRegion();
#if (VTK_MAJOR_VERSION < 6)
    conn->SetInput(lsSmoother->GetOutput());
#else
    conn->SetInputData(lsSmoother->GetOutput());
#endif
    conn->Update();

    // (4) mesh decimation
    vtkSmartPointer<vtkDecimatePro> decimator =
            vtkSmartPointer<vtkDecimatePro>::New();
    decimator->SetInputConnection(conn->GetOutputPort());
    decimator->SetTargetReduction(0.1);
    decimator->SetFeatureAngle(30.);
    decimator->PreserveTopologyOn();
    decimator->BoundaryVertexDeletionOn();
    decimator->Update();

    // (5) mesh relaxation
    vtkSmartPointer<vtkSmoothPolyDataFilter> smoother =
            vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
    smoother->SetInputConnection(decimator->GetOutputPort());
    smoother->SetNumberOfIterations(1);
    smoother->Update();

    vtkSmartPointer<vtkPolyData> denseShape = vtkSmartPointer<vtkPolyData>::New();
    denseShape = smoother->GetOutput();

    return denseShape;
}

vtkSmartPointer<vtkPolyData> Reconstruction::MeshQC(
        vtkSmartPointer<vtkPolyData> meshIn) {
    //for now, write formats and read them in
    vtkSmartPointer<vtkPolyDataWriter>  polywriter =
            vtkSmartPointer<vtkPolyDataWriter>::New();
    polywriter->SetFileName("tmp.vtk");
#if (VTK_MAJOR_VERSION < 6)
    polywriter->SetInput(meshIn);
#else
    polywriter->SetInputData(meshIn);
#endif
    polywriter->Update();
    // read a VTK file
    FEVTKimport vtk_in;
    FEMesh* pm = vtk_in.Load("tmp.vtk");

    // make sure we were able to read the file
    if (pm == 0) {
        throw std::runtime_error("Could not read file tmp.vtk!");
    }

    // fix the element winding
    FEFixMesh fix;
    FEMesh* pm_fix;
    pm_fix = fix.FixElementWinding(pm);
    // do a Laplacian smoothing before decimation
    FEMeshSmoothingModifier lap;
    lap.m_threshold1 = 0.5;
    lap.m_iteration = 1;
    pm_fix = lap.Apply(pm_fix);

    // do a CVD decimation
    FECVDDecimationModifier cvd;
    cvd.m_pct = this->decimationPercent_;
    cvd.m_gradient = 1; // uniform decimation
    pm_fix = cvd.Apply(pm_fix);

    // do a Laplacian smoothing after decimation
    FEMeshSmoothingModifier lap2;
    lap2.m_threshold1 = 0.5;
    lap2.m_iteration = 1;
    pm_fix = lap2.Apply(pm_fix);

    // export to another vtk file
    FEVTKExport vtk_out;
    if (vtk_out.Export(*pm_fix, "tmp2.vtk") == false) {
        throw std::runtime_error("Could not write file tmp2.vtk!");
    }
    // don't forget to clean-up
    delete pm_fix;
    delete pm;
    //read back in new mesh
    vtkSmartPointer<vtkPolyDataReader> polyreader =
            vtkSmartPointer<vtkPolyDataReader>::New();
    polyreader->SetFileName("tmp2.vtk");
    polyreader->Update();
    return polyreader->GetOutput();
    return meshIn;
}

void Reconstruction::writeMeanInfo(std::string nameBase) {
    //write out dense mean
    vtkSmartPointer<vtkPolyDataWriter> writer1 = vtkPolyDataWriter::New();
    writer1->SetFileName((nameBase + ".dense.vtk").c_str());
    writer1->SetInputData(this->denseMean_);
    writer1->Update();
    //write out sparse mean
    std::ofstream ptsOut((nameBase + ".sparse.txt").c_str());
    auto sparsePts = this->sparseMean_;
    for (size_t i = 0; i < goodPoints_.size(); i++) {
        auto pt = sparsePts->GetPoint(i);
        ptsOut << pt[0] << " " << pt[1] << " " << pt[2] << std::endl;
    }
    ptsOut.close();
    //write out good points
    std::ofstream ptsOut1((nameBase + ".goodPoints.txt").c_str());
    auto goodPts = this->goodPoints_;
    for (auto a : goodPts) {
        ptsOut1 << a << std::endl;
    }
    ptsOut1.close();
}

void Reconstruction::performKMeansClustering(
        std::vector<std::vector<itk::Point<float> > > global_pts,
        unsigned int number_of_particles,
        std::vector<int> & centroidIndices) {
    unsigned int number_of_shapes = global_pts.size();
    if (this->numClusters_ > number_of_shapes) {
        this->numClusters_ = number_of_shapes;
    }
    std::vector<vnl_matrix<double>> shapeList;
    vnl_matrix<double> shapeVector(number_of_particles, 3, 0.0);

    // Read input shapes from file list
    for (unsigned int shapeNo = 0; shapeNo < number_of_shapes; shapeNo++) {
        for (unsigned int ii = 0; ii < number_of_particles; ii++) {
            double p[3];
            p[0] = global_pts[shapeNo][ii][0];
            p[1] = global_pts[shapeNo][ii][1];
            p[2] = global_pts[shapeNo][ii][2];

            shapeVector[ii][0] = p[0];
            shapeVector[ii][1] = p[1];
            shapeVector[ii][2] = p[2];
        }
        shapeList.push_back(shapeVector);
    }
    std::vector<int> centers(this->numClusters_, 0);
    unsigned int seed = unsigned(std::time(0));
    std::srand(seed);
    centers[0] = rand() % number_of_shapes;
    std::cout << "Setting center[0] to shape #" << centers[0] << std::endl;
    int countCenters = 1;
    while (countCenters < this->numClusters_) {
        vnl_matrix<double> distMat(number_of_shapes, countCenters, 0.0);
        vnl_vector<double> minDists(number_of_shapes, 0.0);
        vnl_vector<double> probs(number_of_shapes, 0.0);
        for (int s = 0; s < number_of_shapes; s++) {
            for (int c = 0; c < countCenters; c++) {
                if (s == centers[c]) {
                    distMat.set_row(s, 0.0);
                    break;
                }
                shapeVector = shapeList[s] - shapeList[centers[c]];
                distMat(s, c) = shapeVector.fro_norm();
            }
            minDists(s) = distMat.get_row(s).min_value();
            probs(s) = minDists(s)*minDists(s);
        }
        probs.operator /=(probs.sum());
        vnl_vector<double> cumProbs(number_of_shapes, 0.0);

        for (int s = 0; s < number_of_shapes; s++) {
            cumProbs[s] = probs.extract(s + 1, 0).sum();
        }
        double r = (double)(rand() % 10000);
        r = r / 10000.0;
        for (int s = 0; s < number_of_shapes; s++) {
            if (r < cumProbs[s]) {
                if (probs[s] == 0.0) {
                    continue;
                } else {
                    centers[countCenters] = s;
                    countCenters += 1;
                    break;
                }
            }
        }
        std::cout << "Setting center[" << countCenters - 1 <<
                     "] to shape #" << centers[countCenters - 1] << std::endl;
    }
    std::cout << "KMeans++ finished...." << std::endl;
    centroidIndices = centers;
}
