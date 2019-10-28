
#include "MeshFromDT_InputParams.h"
#include "Reconstruction.h"
//#include "Reconstruction.cpp"  //need to include template definition in order for it to be instantiated

int main( int argc , char* argv[] )
{
    if( argc < 2 )
    {
      std::cerr << "Usage: " << std::endl;
      std::cerr << argv[0] << " paramfile " << std::endl;
      return EXIT_FAILURE;
    }

    InputParams params;

    params.readParams(argv[1]);

    std::cout << "Number of input DTs: " << params.distanceTransformFilenames.size() << std::endl;
    std::cout << "subdivision: " << params.subdivision << std::endl;
    std::cout << "butterfly: " << params.butterfly_subdivision << std::endl;

    typedef Reconstruction <> ReconstructionType;
    ReconstructionType reconstructor(params.out_prefix,
                                     params.qcDecimationPercentage, 45, params.K,
                                     params.qcFixWinding,
                                     params.qcDoLaplacianSmoothingBeforeDecimation,
                                     params.qcDoLaplacianSmoothingAfterDecimation,
                                     params.qcSmoothingLambda, params.qcSmoothingIterations);
    reconstructor.reset();

    for (unsigned int shapeNo = 0; shapeNo < params.distanceTransformFilenames.size(); shapeNo++)
    {
        std::cout<< params.distanceTransformFilenames[shapeNo] + " -> " + params.meshFilenames[shapeNo] << std::endl;
        reconstructor.MeshFromDT(params.distanceTransformFilenames[shapeNo], params.meshFilenames[shapeNo], params.subdivision, params.butterfly_subdivision);
    }

    return 0;
}

