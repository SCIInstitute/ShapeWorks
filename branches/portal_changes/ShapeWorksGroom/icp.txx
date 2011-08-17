/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: icp.txx,v $
  Date:      $Date: 2011/03/23 22:40:11 $
  Version:   $Revision: 1.1 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_icp_txx
#define __st_icp_txx

#include "icp.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "itkImageRegionIterator.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkSmartPointer.h"
#include "vtkIterativeClosestPointTransform.h"
#include "vtkTransform.h"
#include "vtkLandmarkTransform.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"


namespace shapetools
{

template <class T, unsigned int D>
icp<T,D>::icp(const char *fname)
{
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay)
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;

    //PARAMSET(pf, m_iterations, "icp_iterations", 0, ok, 50);
    this->m_iterations = 50;
    elem = docHandle.FirstChild( "icp_iterations" ).Element();
    if (elem) this->m_iterations = atoi(elem->GetText());
  }
}


template <class T, unsigned int D> 
void icp<T,D>::operator()()
{
	float isovalue = 0.0;

	typename itk::ImageFileReader<image_type>::Pointer reader =
			itk::ImageFileReader<image_type>::New();

	typename std::vector<std::string>::const_iterator it
		= this->input_filenames().begin();		//Here is the target distance map

	//std::cout << "in icp filter: " << "target file: " << (*it).c_str() << std::endl;
	reader->SetFileName( (*it).c_str() );
	reader->Update();

	typename image_type::Pointer targetImage = reader->GetOutput();
	typename image_type::SizeType  size  =
		targetImage->GetBufferedRegion().GetSize();

	typedef itk::VTKImageExport<image_type> ExportFilterType;
	typename ExportFilterType::Pointer itkTargetExporter = ExportFilterType::New();
	itkTargetExporter->SetInput( reader->GetOutput() );
	vtkImageImport* vtkTargetImporter = vtkImageImport::New();  
	ConnectPipelines(itkTargetExporter, vtkTargetImporter);

	vtkTargetImporter->Update();
	 
	vtkContourFilter * targetContour = vtkContourFilter::New();
	targetContour->SetInput( vtkTargetImporter->GetOutput() );
	targetContour->SetValue( 0, isovalue  );
	targetContour->Update();

	typename std::vector<std::string>::const_iterator oit
		= this->output_filenames().begin();		//Here is distance map

	typename std::vector<std::string>::const_iterator segit
		= this->input_seg_filenames().begin();		//Here is binary segmentation

	typename std::vector<std::string>::const_iterator segoit
		= this->output_seg_filenames().begin();		//Here is binary segmentation

	it++;
	oit++;
	segit++;
	segoit++;

	for (; it != this->input_filenames().end(); it++, oit++, segit++, segoit++)
	{
		//std::cout << "in icp filter: " << "source file: " << (*it).c_str() << std::endl;
		reader->SetFileName( (*it).c_str() );
		reader->Update();
		
		typename ExportFilterType::Pointer itkSourceExporter = ExportFilterType::New();
		itkSourceExporter->SetInput( reader->GetOutput() );

		vtkImageImport* vtkSourceImporter = vtkImageImport::New();  
		ConnectPipelines(itkSourceExporter, vtkSourceImporter);

		vtkSourceImporter->Update();

		vtkContourFilter * sourceContour = vtkContourFilter::New();
		sourceContour->SetInput( vtkSourceImporter->GetOutput() );
		sourceContour->SetValue( 0, isovalue );
		sourceContour->Update();

		//image_type::Pointer sourceInputImage = reader->GetOutput();

		vtkSmartPointer<vtkPolyData> target = targetContour->GetOutput();
		vtkSmartPointer<vtkPolyData> source = sourceContour->GetOutput();

		vtkSmartPointer<vtkIterativeClosestPointTransform> icp =
				vtkSmartPointer<vtkIterativeClosestPointTransform>::New();		

		icp->SetSource(source);
		icp->SetTarget(target);
		icp->GetLandmarkTransform()->SetModeToRigidBody();
		icp->SetMaximumNumberOfIterations(m_iterations);
		icp->StartByMatchingCentroidsOn();

		icp->Modified();
		icp->Update();

		vtkSmartPointer<vtkMatrix4x4> m1 = icp->GetMatrix();
		vtkSmartPointer<vtkMatrix4x4> m  = vtkMatrix4x4::New();

		vtkMatrix4x4::Invert(m1, m);

		//std::cout << "The resulting matrix is: " << *m << std::endl;

		typedef Rigid3DTransformSurrogate<double>  TransformType;

		typename TransformType::Pointer transform = TransformType::New();
		typename TransformType::ParametersType p;
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

		transform->SetParameters( p );

		//std::cout << "parameters:" << std::endl;
		//for(int r=0;r<12;r++)
		//{
		//	std::cout << p[r] << " ";
		//}
		//std::cout << std::endl;

		typename itk::ImageFileReader<image_type>::Pointer source_seg_reader =
			itk::ImageFileReader<image_type>::New();

		//std::cout << "in icp filter: " << "input source seg file: " << (*segit).c_str() << std::endl;
		source_seg_reader->SetFileName( (*segit).c_str() );
		source_seg_reader->Update();
		
		typename image_type::Pointer source_seg_img = 
			source_seg_reader->GetOutput();

		typedef itk::ResampleImageFilter< image_type
			, image_type > ResampleFilterType;
		typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();
		resampler->SetTransform( transform );

		typedef itk::NearestNeighborInterpolateImageFunction<
			image_type, double > InterpolatorType;
		typename InterpolatorType::Pointer interpolator = InterpolatorType::New();

		resampler->SetInterpolator( interpolator );
		resampler->SetOutputSpacing( source_seg_img->GetSpacing() );
		resampler->SetSize( size );
		resampler->SetOutputOrigin( source_seg_img->GetOrigin() );
		resampler->SetOutputDirection( source_seg_img->GetDirection() );
		resampler->SetInput( source_seg_img );
		resampler->Update();

		typename itk::ImageFileWriter< image_type >::Pointer writer
			= itk::ImageFileWriter< image_type >::New();

		//std::cout << "in icp filter: " << "result source seg file: " << (*segoit).c_str() << std::endl;
		writer->SetInput( resampler->GetOutput() );
		writer->SetFileName( (*segoit).c_str()  );
		writer->Update();
	}
}

} // end namespace

#endif
