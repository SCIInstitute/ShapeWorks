// This filter takes a vtkPolyData input and produces a vtkImageData output.
// It is intended to take a closed surface mesh and produce a binary rasterization.
//
// Options are:
//
//  Border: optional border around object (useful for isosurfacing)
//
//  Spacing: output image spacing
//
//  SampleDistance: sample distance for point sampling along surface
//                  if you get a blank/full image, try reducing this value
//
//  InitialBallSize: dilate/erode ball size for morphological closing
//                  if you get a blank/full image, try increasing this value
//
//  FinalBallSize: dilate/erode ball size step at the end

#ifndef __vtkPolyDataToImageData_h
#define __vtkPolyDataToImageData_h

#include "vtkImageAlgorithm.h"

class vtkPolyDataToImageData : public vtkImageAlgorithm
{
public:

  vtkTypeMacro( vtkPolyDataToImageData, vtkImageAlgorithm );
  void PrintSelf( ostream& os, vtkIndent indent );

  static vtkPolyDataToImageData* New();

  // Set the number of voxels bordering the object
  vtkSetMacro( Border, unsigned int );
  vtkGetMacro( Border, unsigned int );

  // Set the spacing of the produced image
  vtkSetMacro( Spacing, double );
  vtkGetMacro( Spacing, double );

  // Set the sample distance of the point sampler
  vtkSetMacro( SampleDistance, double );
  vtkGetMacro( SampleDistance, double );

  // Set the initial dilate/erode ball size for morphological closing
  vtkSetMacro( InitialBallSize, unsigned int );
  vtkGetMacro( InitialBallSize, unsigned int );

  // Set the final dilate/erode ball size for morphological closing
  vtkSetMacro( FinalBallSize, unsigned int );
  vtkGetMacro( FinalBallSize, unsigned int );

  virtual int FillInputPortInformation( int port, vtkInformation* info );

protected:
  vtkPolyDataToImageData();
  ~vtkPolyDataToImageData() {}

  virtual int RequestInformation( vtkInformation* request,
                                  vtkInformationVector** inputVector,
                                  vtkInformationVector* outputVector );

  virtual int RequestData( vtkInformation* request,
                           vtkInformationVector** inputVector,
                           vtkInformationVector* outputVector );

  virtual int RequestUpdateExtent( vtkInformation* request,
                                   vtkInformationVector** inputVector,
                                   vtkInformationVector* outputVector );

private:
  vtkPolyDataToImageData( const vtkPolyDataToImageData& );  // Not implemented.
  void operator=( const vtkPolyDataToImageData& );  // Not implemented.

  void GetOutputInfo( vtkInformationVector * *inputVector,
                      int dim[3], double origin[3], double spacing[3] );

  unsigned int Border;
  double Spacing;
  double SampleDistance;

  unsigned int InitialBallSize;
  unsigned int FinalBallSize;
};

#endif // ifndef __vtkPolyDataToImageData_h
