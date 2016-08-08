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

#ifndef __itkPSMProjectReader_h
#define __itkPSMProjectReader_h

#include "itkPSMProject.h"
#include "itkPSMDOMNodeXMLReader.h"

namespace itk
{

/**
 * \class PSMProjectReader
 *
 * \brief This class is a reader for PSMProjects. 
 *
 * The following code illustrates how to use this class to read a PSMProject from a file:
 *
 * 
 * Note: Though this class behaves similar to ProcessObject, it is not
 *       derived from ProcessObject. This is because the output of
 *       this class, i.e. a PSMProject object, is not an image and
 *       cannot be connected to an ITK process pipeline.
 */
class ITK_EXPORT PSMProjectReader : public PSMDOMNodeXMLReader
{
public:
  /** Standard class typedefs. */
  typedef PSMProjectReader    Self;
  typedef PSMDOMNodeXMLReader Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMProjectReader, PSMDOMNodeXMLReader);

  /** Some convenient typedefs */
  typedef Superclass::DOMNodeType    DOMNodeType;
  typedef Superclass::DOMNodePointer DOMNodePointer;

  /** Get the project data structure generated after calling
      Update(). */
  itkGetObjectMacro( Output, PSMProject);
  itkSetObjectMacro( Output, PSMProject);
  itkGetConstObjectMacro( Output, PSMProject);

  /** Execute the reader. */
  virtual void Update();

 protected:
  PSMProjectReader();

 private:
  PSMProjectReader(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  /** The PSMProject object that will be created by the reader. */
  PSMProject::Pointer m_Output;
};

} // namespace itk

#endif
