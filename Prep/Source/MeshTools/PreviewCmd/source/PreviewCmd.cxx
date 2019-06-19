// PreViewCmd.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "FEVTKImport.h"
#include "FEVTKExport.h"
#include "FEFixMesh.h"
#include "FEMeshSmoothingModifier.h"
#include "FECVDDecimationModifier.h"

#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "process meshes (vtk files) using Preview in batch mode...";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFile").action("store").type("string").set_default("").help("the input vtk filename of the mesh to be processed.");
    parser.add_option("--outFile").action("store").type("string").set_default("").help("the output vtk filename.");
    parser.add_option("--fixWinding").action("store").type("bool").set_default(1).help("do element winding fix (default: 1)");
    parser.add_option("--decimate").action("store").type("bool").set_default(1).help("perform mesh decimation (default: 1)");
    parser.add_option("--doLaplacianSmoothingBeforeDecimation").action("store").type("bool").set_default(1).help("perform laplacian smoothing before decimation (default: 1)");
    parser.add_option("--doLaplacianSmoothingAfterDecimation").action("store").type("bool").set_default(1).help("perform laplacian smoothing after decimation (default: 1)");
    parser.add_option("--smoothingLambda").action("store").type("float").set_default(0.5).help("laplacian smoothing lambda  (default: 0.5)");
    parser.add_option("--smoothingIterations").action("store").type("int").set_default(1).help("laplacian smoothing number of iterations  (default: 1)");
    parser.add_option("--decimationPercentage").action("store").type("float").set_default(0.5).help("percentage of target number of clusters/vertices  (default: 0.5)");

    return parser;
}


int main(int argc, char * argv[])
{
    optparse::OptionParser parser = buildParser();
    optparse::Values & options = parser.parse_args(argc,argv);
    std::vector<std::string> args = parser.args();

    if(argc < 2)
    {
        parser.print_help();
        return EXIT_FAILURE;
    }

    std::string inFilename    = (string) options.get("inFile");
    std::string outFilename   = (string) options.get("outFile");
    bool fixWinding           = (bool)   options.get("fixWinding");
    bool decimate             = (bool)   options.get("decimate");
    bool doLaplacianSmoothingBeforeDecimation = (bool)   options.get("doLaplacianSmoothingBeforeDecimation");
    bool doLaplacianSmoothingAfterDecimation = (bool)   options.get("doLaplacianSmoothingAfterDecimation");
    float smoothingLambda      = (float)  options.get("smoothingLambda");
    int   smoothingIterations  = (int)    options.get("smoothingIterations");
    float decimationPercentage = (float)  options.get("decimationPercentage");

    std::cout << "Inputs: " << std ::endl;
    std::cout << "\t input mesh filename: " << inFilename << std ::endl;
    std::cout << "\t output mesh filename: " << outFilename << std ::endl;
    std::cout << "\t fix winding: " << fixWinding << std ::endl;
    std::cout << "\t perform decimation: " << decimate << std ::endl;
    std::cout << "\t perform laplacian smoothing before decimation: " << doLaplacianSmoothingBeforeDecimation << std ::endl;
    std::cout << "\t perform laplacian smoothing after decimation: " << doLaplacianSmoothingAfterDecimation << std ::endl;
    std::cout << "\t laplacian smoothing lambda: " << smoothingLambda << std ::endl;
    std::cout << "\t laplacian smoothing iterations: " << smoothingIterations << std ::endl;
    std::cout << "\t decimation percentage: " << decimationPercentage << std ::endl;

	// read a VTK file
	FEVTKimport vtk_in;
    FEMesh* pm = vtk_in.Load(inFilename.c_str());

	// make sure we were able to read the file
	if (pm == 0)
	{
		fprintf(stderr, "Does not compute ...\n\n");
		return 0;
	}

	// fix the element winding
	FEFixMesh fix;
    FEMesh* pm_fix;
    if (fixWinding)
        pm_fix = fix.FixElementWinding(pm);

    // do a Laplacian smoothing before decimation
    if (doLaplacianSmoothingBeforeDecimation)
    {
        FEMeshSmoothingModifier lap;
        lap.m_threshold1 = smoothingLambda;
        lap.m_iteration = smoothingIterations;
        pm_fix = lap.Apply(pm_fix);
    }

    // do a CVD decimation
    if (decimate)
    {
        FECVDDecimationModifier cvd;
        cvd.m_pct = decimationPercentage;
        cvd.m_gradient = 1; // uniform decimation
        pm_fix = cvd.Apply(pm_fix);

        // do a Laplacian smoothing after decimation
        if (doLaplacianSmoothingAfterDecimation)
        {
            FEMeshSmoothingModifier lap;
            lap.m_threshold1 = smoothingLambda;
            lap.m_iteration = smoothingIterations;
            pm_fix = lap.Apply(pm_fix);
        }

    }

    // export to another vtk file
	FEVTKExport vtk_out;
    if (vtk_out.Export(*pm_fix, outFilename.c_str()) == false)
	{
		fprintf(stderr, "Does not compute ...\n\n");
		return 0;
	}

	// don't forget to clean-up
	delete pm_fix;
	delete pm;
	
	return 0;
}
