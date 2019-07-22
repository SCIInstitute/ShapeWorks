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
#ifndef __itkPSMDomain_hxx
#define __itkPSMDomain_hxx
#include "itkPSMDomain.h"

namespace itk
{

template <unsigned int VDimension>
PSMDomain<VDimension>::PSMDomain()  : m_ConstraintsEnabled(true)
{
}

template <unsigned int VDimension>
void PSMDomain<VDimension>::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace

#endif //__itkPSMDomain_hxx
