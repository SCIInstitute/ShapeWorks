/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMPointIndexPair_h
#define __itkPSMPointIndexPair_h

#include "itkPoint.h"
namespace itk {
/**  
 * \class PSMPointIndexPair
 * \brief
 * Struct containing a Point and an index value associated with a
 * point.  This object is used mainly by PSMNeighborhood classes and
 * the PowerOfTwoPointTree.
 * 
 * \ingroup PSM
 *
*/
template <unsigned int VDimension>
struct PSMPointIndexPair
{
  typedef itk::Point<double, VDimension> PointType;
  PSMPointIndexPair() {}
  PSMPointIndexPair(const PointType &p, unsigned int i=0)
  {
    Point = p;
    Index = i;
  }
  PSMPointIndexPair(const PSMPointIndexPair &o)
  { this->operator=(o); }
  const PSMPointIndexPair &operator=(const PSMPointIndexPair &o)
  {
    Point = o.Point;
    Index = o.Index;
	return *this;
  }
  PointType Point;
  unsigned int Index;
};

}
#endif

