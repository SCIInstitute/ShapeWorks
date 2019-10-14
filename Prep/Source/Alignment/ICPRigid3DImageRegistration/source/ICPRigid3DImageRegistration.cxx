/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IcpRegid3DRegistration.cxx
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

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that performs iterative closed point (ICP) 3D rigid registration on a pair of images...";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--targetDistanceMap").action("store").type("string").set_default("").help("The distance map of target image.");
    parser.add_option("--sourceDistanceMap").action("store").type("string").set_default("").help("The distance map of source image.");
    parser.add_option("--sourceSegmentation").action("store").type("string").set_default("").help("The segmentation of source image.");
    parser.add_option("--isoValue").action("store").type("float").set_default(0.0).help("As we need to get point set from surface for ICP, this iso value is required to get the isosurface. The default value is 0.0.");
    parser.add_option("--icpIterations").action("store").type("int").set_default(20).help("The number of iterations user want to run ICP registration.");
    parser.add_option("--visualizeResult").action("store").type("bool").set_default(false).help("A flag to visualize the registration result.");
    parser.add_option("--solutionSegmentation").action("store").type("string").set_default("").help("The filename of the aligned segmentation of source image.");
    parser.add_option("--solutionRaw").action("store").type("string").set_default("").help("The filename of the aligned raw source image.");
    parser.add_option("--sourceRaw").action("store").type("string").set_default("").help("The raw source image.");
    parser.add_option("--solutionTransformation").action("store").type("string").set_default("").help("The filename of the textfile containing the transformation matrix.");
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

    if(argc < 7)
    {
        parser.print_help();
        return EXIT_FAILURE;
    }

    std::string targetDistanceMap    = (std::string) options.get("targetDistanceMap");
    std::string sourceDistanceMap    = (std::string) options.get("sourceDistanceMap");
    std::string sourceSegmentation   = (std::string) options.get("sourceSegmentation");
    std::string solutionSegmentation = (std::string) options.get("solutionSegmentation");
    std::string sourceRaw            = (std::string) options.get("sourceRaw");
    std::string solutionRaw          = (std::string) options.get("solutionRaw");
    std::string solutionTransformation   = (std::string) options.get("solutionTransformation");
    float         isovalue           = (float) options.get("isoValue");
    int         icpIterations        = (int) options.get("icpIterations");
    bool      visualizeResult        = (bool) options.get("visualizeResult");

    try
    {
        typedef float  InputPixelType;
        typedef float  OutputPixelType;
        const unsigned int Dimension = 3;

        typedef itk::Image< InputPixelType, Dimension > InputImageType;
        typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
        typedef itk::Image< InputPixelType, Dimension > InputSegImageType;
        typedef itk::Image< InputPixelType, Dimension > InputRawImageType;
        typedef itk::Image< OutputPixelType, Dimension > OutputSegImageType;
        typedef itk::Image< OutputPixelType, Dimension > OutputRawImageType;

        typedef itk::ImageFileReader< InputImageType > ReaderType;
        typedef itk::ImageFileReader< InputSegImageType > SegReaderType;
        typedef itk::ImageFileReader< InputRawImageType > RawReaderType;


        ReaderType::Pointer targetReader  = ReaderType::New();
        targetReader->SetFileName( targetDistanceMap );
        targetReader->Update();

        InputImageType::Pointer targetInputImage = targetReader->GetOutput();
        InputImageType::SizeType  size  =
                targetInputImage->GetBufferedRegion().GetSize();

        typedef itk::VTKImageExport< InputImageType > ExportFilterType;

        ExportFilterType::Pointer itkTargetExporter = ExportFilterType::New();
        itkTargetExporter->SetInput( targetReader->GetOutput() );

        vtkImageImport* vtkTargetImporter = vtkImageImport::New();
        ConnectPipelines(itkTargetExporter, vtkTargetImporter);

        vtkTargetImporter->Update();

        vtkContourFilter * targetContour = vtkContourFilter::New();
        targetContour->SetInputData( vtkTargetImporter->GetOutput() );
        targetContour->SetValue( 0, isovalue  );
        targetContour->Update();

        ReaderType::Pointer movingReader  = ReaderType::New();
        movingReader->SetFileName( sourceDistanceMap );
        movingReader->Update();

        InputImageType::Pointer movingInputImage = movingReader->GetOutput();

        ExportFilterType::Pointer itkMovingExporter = ExportFilterType::New();

        itkMovingExporter->SetInput( movingReader->GetOutput() );

        vtkImageImport* vtkMovingImporter = vtkImageImport::New();
        ConnectPipelines(itkMovingExporter, vtkMovingImporter);

        vtkMovingImporter->Update();

        vtkContourFilter * movingContour = vtkContourFilter::New();
        movingContour->SetInputData( vtkMovingImporter->GetOutput() );
        movingContour->SetValue( 0, isovalue );
        movingContour->Update();

        vtkSmartPointer<vtkPolyData> target = targetContour->GetOutput();
        vtkSmartPointer<vtkPolyData> moving = movingContour->GetOutput();


        // Setup ICP transform		 // Setup ICP transform
        vtkSmartPointer<vtkIterativeClosestPointTransform> icp =
                vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
        icp->SetSource(moving);
        icp->SetTarget(target);
        icp->GetLandmarkTransform()->SetModeToRigidBody();
        icp->SetMaximumNumberOfIterations(icpIterations);
        //icp->StartByMatchingCentroidsOn();
        icp->Modified();
        icp->Update();


        vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter =
                vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        icpTransformFilter->SetInputData(moving);
        icpTransformFilter->SetTransform(icp);
        icpTransformFilter->Update();


        //Transform Segmentation
        // Get the resulting transformation matrix (this matrix takes the source points to the target points)
        vtkSmartPointer<vtkMatrix4x4> m1 = icp->GetMatrix();
        vtkSmartPointer<vtkMatrix4x4> m  = vtkMatrix4x4::New();

        vtkMatrix4x4::Invert(m1, m);

        std::cout << "The resulting matrix is: " << *m << std::endl;
        // write the matrix m in the solutionTransformation textfile
        std::ofstream ofs;
        std::string fname=solutionTransformation;
        const char * filename = fname.c_str();
        ofs.open (filename); // the file needs to be overwritten in case running the program on the same file several times

        // for ease of automatic parsing the generated param file
       // ofs <<"The Transformation matrix is:\n"<< *m;
        for (int r=0;r<3;r++){
            for (int c =0;c<4;c++){
                ofs<< m->GetElement(r,c)<<"\t";
            }
            ofs << "\n";
        }
        ofs.close();

        typedef itk::Rigid3DTransformSurrogate<double>  TransformType;
        TransformType::Pointer transform = TransformType::New();
        TransformType::ParametersType p;
        p.set_size(12);
        for(int r=0;r<3;r++)
        {
            for(int c=0;c<3;c++)
            {
                p[r*3+c]=m->GetElement(r,c);
            }
        }
        p[ 9]=m->GetElement(0,3);
        p[10]=m->GetElement(1,3);
        p[11]=m->GetElement(2,3);

        //transform->SetParameters( p );

        //std::cout << "parameters:" << std::endl;
        //for(int r=0;r<12;r++)
        //{
        //	std::cout << p[r] << " ";
        //}
        //std::cout << std::endl;
        ///////////////////////////
        SegReaderType::Pointer movingSegReader  = SegReaderType::New();
        movingSegReader->SetFileName( sourceSegmentation );
        movingSegReader->Update();
        InputSegImageType::Pointer movingSegInputImage =
                movingSegReader->GetOutput();

        typedef itk::ResampleImageFilter< InputSegImageType
                , OutputSegImageType > ResampleFilterType;
        ResampleFilterType::Pointer resampler = ResampleFilterType::New();
        resampler->SetTransform( transform );


        typedef itk::NearestNeighborInterpolateImageFunction<
                InputSegImageType, double > InterpolatorType;
        InterpolatorType::Pointer interpolator = InterpolatorType::New();

        resampler->SetInterpolator( interpolator );
        resampler->SetOutputSpacing( targetInputImage->GetSpacing() );
        resampler->SetSize( size );
        resampler->SetOutputOrigin( targetInputImage->GetOrigin() );
        resampler->SetOutputDirection( targetInputImage->GetDirection() );
        resampler->SetInput( movingSegInputImage );
        resampler->Update();

        typedef itk::ImageFileWriter< OutputSegImageType >  ITKWriterType;
        ITKWriterType::Pointer itkImageWriter = ITKWriterType::New();

        itkImageWriter->SetInput( resampler->GetOutput() );
        itkImageWriter->SetFileName( solutionSegmentation );
        itkImageWriter->Update();
        //////////////////////////////////
        // Same transformation applied on the raw images
        // change the variable names for syntax consistency
        if (sourceRaw != "")
        {
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
            resampler->SetOutputSpacing( targetInputImage->GetSpacing() );
            resampler->SetSize( size );
            resampler->SetOutputOrigin( targetInputImage->GetOrigin() );
            resampler->SetOutputDirection( targetInputImage->GetDirection() );
            resampler->SetInput( movingRawInputImage );
            resampler->Update();

            typedef itk::ImageFileWriter< OutputRawImageType >  ITKWriterType;
            ITKWriterType::Pointer itkImageWriter = ITKWriterType::New();

            itkImageWriter->SetInput( resampler->GetOutput() );
            itkImageWriter->SetFileName( solutionRaw );
            itkImageWriter->Update();
         }

        if(visualizeResult){
            //------------------------------------------------------------------------
            // VTK Render pipeline.
            //------------------------------------------------------------------------

            // Create a renderer, render window, and render window interactor to
            // display the results.

            vtkRenderer* renderer = vtkRenderer::New();
            vtkRenderWindow* renWin = vtkRenderWindow::New();
            vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();

            renWin->SetSize(500, 500);
            renWin->AddRenderer(renderer);
            iren->SetRenderWindow(renWin);


            // use cell picker for interacting with the image orthogonal views.
            //
            vtkCellPicker * picker = vtkCellPicker::New();
            picker->SetTolerance(0.005);


            //assign default props to the ipw's texture plane actor
            vtkProperty * ipwProp = vtkProperty::New();


            // Create 3 orthogonal view using the ImagePlaneWidget
            //
            vtkImagePlaneWidget * xImagePlaneWidget =  vtkImagePlaneWidget::New();
            vtkImagePlaneWidget * yImagePlaneWidget =  vtkImagePlaneWidget::New();
            vtkImagePlaneWidget * zImagePlaneWidget =  vtkImagePlaneWidget::New();

            // The 3 image plane widgets are used to probe the dataset.
            //
            xImagePlaneWidget->DisplayTextOn();
            xImagePlaneWidget->SetInputData(vtkTargetImporter->GetOutput());
            xImagePlaneWidget->SetPlaneOrientationToXAxes();
            xImagePlaneWidget->SetSliceIndex(size[0]/2);
            xImagePlaneWidget->SetPicker(picker);
            xImagePlaneWidget->RestrictPlaneToVolumeOn();
            xImagePlaneWidget->SetKeyPressActivationValue('x');
            xImagePlaneWidget->GetPlaneProperty()->SetColor(1, 0, 0);
            xImagePlaneWidget->SetTexturePlaneProperty(ipwProp);
            xImagePlaneWidget->SetResliceInterpolateToNearestNeighbour();

            yImagePlaneWidget->DisplayTextOn();
            yImagePlaneWidget->SetInputData(vtkTargetImporter->GetOutput());
            yImagePlaneWidget->SetPlaneOrientationToYAxes();
            yImagePlaneWidget->SetSliceIndex(size[1]/2);
            yImagePlaneWidget->SetPicker(picker);
            yImagePlaneWidget->RestrictPlaneToVolumeOn();
            yImagePlaneWidget->SetKeyPressActivationValue('y');
            yImagePlaneWidget->GetPlaneProperty()->SetColor(1, 1, 0);
            yImagePlaneWidget->SetTexturePlaneProperty(ipwProp);
            yImagePlaneWidget->SetLookupTable(xImagePlaneWidget->GetLookupTable());

            zImagePlaneWidget->DisplayTextOn();
            zImagePlaneWidget->SetInputData(vtkTargetImporter->GetOutput());
            zImagePlaneWidget->SetPlaneOrientationToZAxes();
            zImagePlaneWidget->SetSliceIndex(size[2]/2);
            zImagePlaneWidget->SetPicker(picker);
            zImagePlaneWidget->SetKeyPressActivationValue('z');
            zImagePlaneWidget->GetPlaneProperty()->SetColor(0, 0, 1);
            zImagePlaneWidget->SetTexturePlaneProperty(ipwProp);
            zImagePlaneWidget->SetLookupTable(xImagePlaneWidget->GetLookupTable());

            xImagePlaneWidget->SetInteractor( iren );
            xImagePlaneWidget->On();

            yImagePlaneWidget->SetInteractor( iren );
            yImagePlaneWidget->On();

            zImagePlaneWidget->SetInteractor( iren );
            zImagePlaneWidget->On();


            // Set the background to something grayish
            renderer->SetBackground(0.4392, 0.5020, 0.5647);

            vtkPolyDataMapper * targetPolyMapper = vtkPolyDataMapper::New();
            vtkActor          * targetPolyActor  = vtkActor::New();

            targetPolyActor->SetMapper( targetPolyMapper );
            targetPolyMapper->SetInputData( targetContour->GetOutput() );
            targetPolyMapper->ScalarVisibilityOff();

            vtkPolyDataMapper * movingPolyMapper = vtkPolyDataMapper::New();
            vtkActor          * movingPolyActor  = vtkActor::New();

            movingPolyActor->SetMapper( movingPolyMapper );
            movingPolyMapper->SetInputData( movingContour->GetOutput() );
            movingPolyMapper->ScalarVisibilityOff();


            vtkPolyDataMapper * solutionPolyMapper = vtkPolyDataMapper::New();
            vtkActor          * solutionPolyActor  = vtkActor::New();

            solutionPolyActor->SetMapper( solutionPolyMapper );
            solutionPolyMapper->SetInputData( icpTransformFilter->GetOutput() );
            solutionPolyMapper->ScalarVisibilityOff();

            vtkProperty * movingProperty = vtkProperty::New();
            movingProperty->SetAmbient(0.1);
            movingProperty->SetDiffuse(0.1);
            movingProperty->SetSpecular(0.5);
            movingProperty->SetColor(1.0,0.0,0.0);
            movingProperty->SetLineWidth(2.0);
            movingProperty->SetRepresentationToSurface();

            vtkProperty * targetProperty = vtkProperty::New();
            targetProperty->SetAmbient(0.1);
            targetProperty->SetDiffuse(0.1);
            targetProperty->SetSpecular(0.5);
            targetProperty->SetColor(0.0,0.0,1.0);
            targetProperty->SetLineWidth(2.0);
            targetProperty->SetRepresentationToSurface();

            vtkProperty * solutionProperty = vtkProperty::New();
            solutionProperty->SetAmbient(0.1);
            solutionProperty->SetDiffuse(0.1);
            solutionProperty->SetSpecular(0.5);
            solutionProperty->SetColor(0.0,1.0,0.0);
            solutionProperty->SetLineWidth(2.0);
            solutionProperty->SetRepresentationToSurface();

            movingPolyActor->SetProperty( movingProperty );
            targetPolyActor->SetProperty( targetProperty );
            solutionPolyActor->SetProperty( solutionProperty );

            renderer->AddActor( targetPolyActor );
            renderer->AddActor( movingPolyActor );
            renderer->AddActor( solutionPolyActor );

            // Bring up the render window and begin interaction.
            renderer->ResetCamera();
            renWin->Render();
            iren->Start();

            // Release all VTK components
            targetPolyActor->Delete();
            movingPolyActor->Delete();
            solutionPolyActor->Delete();
            picker->Delete();
            ipwProp->Delete();
            vtkTargetImporter->Delete();
            vtkMovingImporter->Delete();
            xImagePlaneWidget->Delete();
            yImagePlaneWidget->Delete();
            zImagePlaneWidget->Delete();
            targetContour->Delete();
            movingContour->Delete();
            targetProperty->Delete();
            movingProperty->Delete();
            solutionProperty->Delete();
            targetPolyMapper->Delete();
            movingPolyMapper->Delete();
            solutionPolyMapper->Delete();
            renWin->Delete();
            renderer->Delete();
            iren->Delete();
        }

    }
    catch( itk::ExceptionObject & e )
    {
        std::cerr << "Exception catched !! " << e << std::endl;
    }

    return 0;
}
