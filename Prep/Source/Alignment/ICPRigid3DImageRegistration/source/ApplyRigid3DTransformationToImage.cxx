/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ApplyRigid3DTransformationToImage.cxx
  Language:  C++
  Date:      $Date: 2011/03/23 22:40:15 $
  Version:   $Revision: 1.1 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkCommand.h"
#include "itkImage.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkRigid3DTransform.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"
#include "vtkDataSet.h"
#include "vtkProperty.h"
#include "vtkPolyDataWriter.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkImagePlaneWidget.h"
#include "vtkCellPicker.h"

#include "vtkSmartPointer.h"
#include "vtkTransform.h"
#include "vtkVertexGlyphFilter.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkIterativeClosestPointTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLandmarkTransform.h"
#include "vtkMath.h"

#include "OptionParser.h"

#include <iostream>
#include <fstream>
optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that performs a given transformation matrix to images...";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--targetDistanceMap").action("store").type("string").set_default("").help("The distance map of target image.");
    parser.add_option("--sourceRaw").action("store").type("string").set_default("").help("The raw source image.");
    parser.add_option("--solutionRaw").action("store").type("string").set_default("").help("The filename of the aligned raw source image.");
//    parser.add_option("--m_00").action("store").type("float").set_default("").help("(0,0) element of the tranformation matrix.");
//    parser.add_option("--m_01").action("store").type("float").set_default("").help("(0,1) element of the tranformation matrix.");
//    parser.add_option("--m_02").action("store").type("float").set_default("").help("(0,2) element of the tranformation matrix.");
//    parser.add_option("--m_10").action("store").type("float").set_default("").help("(1,0) element of the tranformation matrix.");
//    parser.add_option("--m_11").action("store").type("float").set_default("").help("(1,1) element of the tranformation matrix.");
//    parser.add_option("--m_12").action("store").type("float").set_default("").help("(1,2) element of the tranformation matrix.");
//    parser.add_option("--m_20").action("store").type("float").set_default("").help("(2,0) element of the tranformation matrix.");
//    parser.add_option("--m_21").action("store").type("float").set_default("").help("(2,1) element of the tranformation matrix.");
//    parser.add_option("--m_22").action("store").type("float").set_default("").help("(2,2) element of the tranformation matrix.");
//    parser.add_option("--m_03").action("store").type("float").set_default("").help("(0,3) element of the tranformation matrix.");
//    parser.add_option("--m_13").action("store").type("float").set_default("").help("(1,3) element of the tranformation matrix.");
//    parser.add_option("--m_23").action("store").type("float").set_default("").help("(2,3) element of the tranformation matrix.");
    parser.add_option("--transformationMatrix").action("store").type("string").set_default("").help("the tranformation matrix.");
    return parser;


    return parser;
}


namespace itk
{
template <class TScalarType>
class Rigid3DTransformSurrogate : public Rigid3DTransform < TScalarType >
{
public:
    /** Standard class typedefs. */
    typedef Rigid3DTransformSurrogate        Self;
    typedef Rigid3DTransform< TScalarType >  Superclass;
    typedef SmartPointer< Self >             Pointer;
    typedef SmartPointer< const Self >       ConstPointer;

    itkNewMacro(Self);

private:
    Rigid3DTransformSurrogate() {}
    ~Rigid3DTransformSurrogate() {}

};

}

/**
 * This function will connect the given itk::VTKImageExport filter to
 * the given vtkImageImport filter.
 */
template <typename ITK_Exporter, typename VTK_Importer>
void ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer)
{
    importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
    importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
    importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
    importer->SetSpacingCallback(exporter->GetSpacingCallback());
    importer->SetOriginCallback(exporter->GetOriginCallback());
    importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
    importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
    importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
    importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
    importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
    importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
    importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

/**
 * This function will connect the given vtkImageExport filter to
 * the given itk::VTKImageImport filter.
 */
template <typename VTK_Exporter, typename ITK_Importer>
void ConnectPipelines(VTK_Exporter* exporter, ITK_Importer importer)
{
    importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
    importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
    importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
    importer->SetSpacingCallback(exporter->GetSpacingCallback());
    importer->SetOriginCallback(exporter->GetOriginCallback());
    importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
    importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
    importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
    importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
    importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
    importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
    importer->SetCallbackUserData(exporter->GetCallbackUserData());
}


/**
 * This program implements an example connection between ITK and VTK
 * pipelines.  The combined pipeline flows as follows:
 *
 * itkImageFileReader ==> itkVTKImageExport ==>
 *    vtkImageImport ==> vtkImagePlaneWidget
 *
 * The resulting vtkImagePlaneWidget is displayed in a vtkRenderWindow.
 * Whenever the VTK pipeline executes, information is propagated
 * through the ITK pipeline.  If the ITK pipeline is out of date, it
 * will re-execute and cause the VTK pipeline to update properly as
 * well.
 */
int main(int argc, char * argv [] )
{

    optparse::OptionParser parser = buildParser();
    optparse::Values & options = parser.parse_args(argc,argv);
    std::vector<std::string> args = parser.args();
/*
    if(argc < 7)
    {
        parser.print_help();
        return EXIT_FAILURE;
    }
*/
    std::string targetDistanceMap    = (std::string) options.get("targetDistanceMap");
    std::string sourceRaw            = (std::string) options.get("sourceRaw");
    std::string solutionRaw          = (std::string) options.get("solutionRaw");
    std::string transformationMatrix    = (std::string) options.get("transformationMatrix");

 /*

    float m_00    = (float) options.get("m_00");
    float m_01    = (float) options.get("m_01");
    float m_02    = (float) options.get("m_02");
    float m_10    = (float) options.get("m_10");
    float m_11    = (float) options.get("m_11");
    float m_12    = (float) options.get("m_12");
    float m_20    = (float) options.get("m_20");
    float m_21    = (float) options.get("m_21");
    float m_22    = (float) options.get("m_22");
    float m_03    = (float) options.get("m_03");
    float m_13    = (float) options.get("m_13");
    float m_23    = (float) options.get("m_23");
*/

    float m[3][4] = {0.0f};

    ifstream info("transformationMatrix");
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 4; j++){
          info >> m[i][j];
        }
    }

    try
    {
        typedef float  InputPixelType;
        typedef float  OutputPixelType;
        const unsigned int Dimension = 3;

        typedef itk::Image< InputPixelType, Dimension > InputImageType;
        typedef itk::Image< InputPixelType, Dimension > InputRawImageType;
        typedef itk::Image< OutputPixelType, Dimension > OutputRawImageType;

        typedef itk::ImageFileReader< InputImageType > ReaderType;
        typedef itk::ImageFileReader< InputRawImageType > RawReaderType;


        ReaderType::Pointer targetReader  = ReaderType::New();
        targetReader->SetFileName( targetDistanceMap );
        targetReader->Update();

        InputImageType::Pointer targetInputImage = targetReader->GetOutput();
        InputImageType::SizeType  size  =
                targetInputImage->GetBufferedRegion().GetSize();

        typedef itk::Rigid3DTransformSurrogate<double>  TransformType;
        TransformType::Pointer transform = TransformType::New();
        TransformType::ParametersType p;
        p.set_size(12);

        p[0] = m[0][0];
        p[1] = m[0][1];
        p[2] = m[0][2];
        p[3] = m[1][0];
        p[4] = m[1][1];
        p[5] = m[1][2];
        p[6] = m[2][0];
        p[7] = m[2][1];
        p[8] = m[2][2];
        p[9] = m[0][3];
        p[10] = m[1][3];
        p[11] = m[2][3];

        //////////////////////////////////
        // transformation applied on the raw images
        // change the variable names for syntax consistency

        RawReaderType::Pointer movingRawReader  = RawReaderType::New();
        movingRawReader->SetFileName( sourceRaw );
        movingRawReader->Update();
        InputRawImageType::Pointer movingRawInputImage =
                movingRawReader->GetOutput();

        typedef itk::ResampleImageFilter< InputRawImageType
                , OutputRawImageType > ResampleFilterType;
        ResampleFilterType::Pointer resampler = ResampleFilterType::New();
        resampler->SetTransform( transform );


        typedef itk::LinearInterpolateImageFunction<
                InputRawImageType, double > InterpolatorType;
        InterpolatorType::Pointer interpolator = InterpolatorType::New();

        resampler->SetInterpolator( interpolator );
        resampler->SetOutputSpacing( targetInputImage ->GetSpacing() );
        resampler->SetSize( size );
        resampler->SetOutputOrigin(targetInputImage ->GetOrigin() );
        resampler->SetOutputDirection(targetInputImage ->GetDirection() );
        resampler->SetInput( movingRawInputImage );
        resampler->Update();

        typedef itk::ImageFileWriter< OutputRawImageType >  ITKWriterType;
        ITKWriterType::Pointer itkImageWriter = ITKWriterType::New();

        itkImageWriter->SetInput( resampler->GetOutput() );
        itkImageWriter->SetFileName( solutionRaw );
        itkImageWriter->Update();


    }
    catch( itk::ExceptionObject & e )
    {
        std::cerr << "Exception catched !! " << e << std::endl;
    }

    return 0;
}
