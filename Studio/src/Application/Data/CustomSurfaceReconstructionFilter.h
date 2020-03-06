/*=========================================================================

  Program:   Visualization Toolkit
  Module:    CustomSurfaceReconstructionFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME CustomSurfaceReconstructionFilter - reconstructs a surface from unorganized points
// .SECTION Description
// CustomSurfaceReconstructionFilter takes a list of points assumed to lie on
// the surface of a solid 3D object. A signed measure of the distance to the
// surface is computed and sampled on a regular grid. The grid can then be
// contoured at zero to extract the surface. The default values for
// neighborhood size and sample spacing should give reasonable results for
// most uses but can be set if desired. This procedure is based on the PhD
// work of Hugues Hoppe: http://www.research.microsoft.com/~hoppe

#ifndef __CustomSurfaceReconstructionFilter_h
#define __CustomSurfaceReconstructionFilter_h

#include "vtkImageAlgorithm.h"

class CustomSurfaceReconstructionFilter : public vtkImageAlgorithm
{
public:
  vtkTypeMacro(CustomSurfaceReconstructionFilter,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Construct with NeighborhoodSize=20.
  static CustomSurfaceReconstructionFilter* New();

  // Description: 
  // Specify the number of neighbors each point has, used for estimating the
  // local surface orientation.  The default value of 20 should be OK for
  // most applications, higher values can be specified if the spread of
  // points is uneven. Values as low as 10 may yield adequate results for
  // some surfaces. Higher values cause the algorithm to take longer. Higher
  // values will cause errors on sharp boundaries.
  vtkGetMacro(NeighborhoodSize,int);
  vtkSetMacro(NeighborhoodSize,int);

  // Description: 
  // Specify the spacing of the 3D sampling grid. If not set, a
  // reasonable guess will be made.
  vtkGetMacro(SampleSpacing,double);
  vtkSetMacro(SampleSpacing,double);

protected:
  CustomSurfaceReconstructionFilter();
  ~CustomSurfaceReconstructionFilter() {};

  virtual int RequestInformation (vtkInformation *, 
                                  vtkInformationVector **, 
                                  vtkInformationVector *);
  virtual int RequestData (vtkInformation *, 
                           vtkInformationVector **, 
                           vtkInformationVector *);

  int NeighborhoodSize;
  double SampleSpacing;

  virtual int FillInputPortInformation(int, vtkInformation*);

private:
  CustomSurfaceReconstructionFilter(const CustomSurfaceReconstructionFilter&);  // Not implemented.
  void operator=(const CustomSurfaceReconstructionFilter&);  // Not implemented.
};

#endif

