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

#include "itkPSMProjectReader.h"

namespace itk
{

PSMProjectReader::PSMProjectReader()
{
}

void PSMProjectReader::Update()
{
  // Have the parent class parse the XML file
  Superclass::Update();

  DOMNodeType* dom = this->GetDOMNode();

  if (dom == 0)
    {
      itkExceptionMacro( "Failed to parse the XML file." );
    }
  else
    { 
      // Allocate a new PSM Project.
      m_Output = PSMProject::New();
      m_Output->SetDOMNode(dom);
    }
}

} // end namespace itk
