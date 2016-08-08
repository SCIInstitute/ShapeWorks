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

#ifndef __itkPSMProjectWriter_h
#define __itkPSMProjectWriter_h

#include "itkPSMProject.h"

namespace itk
{

/**
 * \class PSMProjectWriter
 *
 * \brief This class is a writer for PSMProjects. 
 *
 * The following code illustrates how to use this class to write a PSMProject to a file.
 *
 */
class ITK_EXPORT PSMProjectWriter : public Object
{
public:
  /** Standard class typedefs. */
  typedef PSMProjectWriter Self;
  typedef Object           Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMProjectWriter, Object);

  /** Set the input XML filename. */
  itkSetStringMacro(FileName);
  
  /** Get the input XML filename. */
  itkGetStringMacro(FileName);
  
  /** Get the project data structure generated after calling
      Update(). */
  itkGetObjectMacro( Input, PSMProject);
  itkSetObjectMacro( Input, PSMProject);
  itkGetConstObjectMacro( Input, PSMProject);

  /** Execute the reader. */
  virtual void Update();

 protected:
  PSMProjectWriter();

 private:
  PSMProjectWriter(const Self &); //purposely not implemented
  void operator=(const Self &);   //purposely not implemented

  /** The PSMProject object that will be created by the reader. */
  PSMProject::Pointer m_Input;

  /** The filename of the project to be written. */
  std::string m_FileName;
};

} // namespace itk

#endif
