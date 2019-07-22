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
#ifndef __itkPSMProject_h
#define __itkPSMProject_h

#include "itkMacro.h"
#include "itkPSMDOMNode.h"
#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkEventObject.h"
#include "itkExceptionObject.h"
#include <vnl/vnl_vector_fixed.h>

namespace itk
{

/** \class PSMProject
 *
 * \brief This class is a data object that holds information about a
 * Particle Shape Modeling project.
 *
 * This class is a data object that holds information about a
 * Particle Shape Modeling project.  It contains information such as
 * lists of input and output files, paths, processing parameters, the
 * author of the project, and other metainformation that is useful to
 * keep track of the multiple steps and image files necessary to
 * compute a particle-based shape model.
 *
 * At its core, this class is a wrapper around a PSMDOMNode object
 * that provides a simplified interface to the data in the DOM node.
 */
  class ITK_EXPORT PSMProject : public Object
{
 public:
  /** Standard class typedefs */
  typedef PSMProject Self;
  typedef Object Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMProject, Object);

  /** Reserved keywords for use in XML parameter files. */
  static const std::string                data_tag;
  static const std::string  distance_transform_tag;
  static const std::string distance_transforms_tag;
  static const std::string              domain_tag;
  static const std::string     correspondences_tag;
  static const std::string       cutting_plane_tag;
  static const std::string               model_tag;
  static const std::string                name_tag;
  static const std::string        optimization_tag;
  static const std::string    number_of_scales_tag;
  static const std::string       preprocessing_tag;
  static const std::string          procrustes_tag;
  static const std::string         psm_project_tag;
  static const std::string               scale_tag;
  static const std::string        scale_number_tag;
  static const std::string           variables_tag;


  /** Set the DOM node object, which contains the XML tree of
      parameter data.  This method will throw an exception if the
      DOMNode is not a valid PSMProject file. */
  void SetDOMNode(PSMDOMNode *p);

  /** Get the DOM Node object, which contains the XML tree of
      parameter data.  This method is only intended for special cases
      in which the DOM tree needs to be modified or contains
      nonstandard data objects. */
  PSMDOMNode::Pointer &GetDOMNode() 
    { return m_DOMNode; }


  /** Returns true if the file has specified at least one "domain"
      tag. */
  bool HasDomains() const;

  /** Returns the node associated with the given name */
  const DOMNode *GetDomainNode(const std::string &name) const;

  /** Returns true if the named domain has distance transforms
      defined */
  bool HasDomainDistanceTransform(const std::string &name) const;

  /** Returns the name of the distance transform from the given
      domain */
  const std::vector<std::string > &
    GetDomainDistanceTransform(const std::string &name) const;

  /** Returns true if the named domain has cutting planes defined */
  bool HasDomainCuttingPlanes(const std::string &name) const;

  /** Returns a list of cutting planes for the named domain.  Throws
      an exception if the domain does not contain cutting planes (call
      HasDomainCuttingPlanes first). */
  std::vector<vnl_vector_fixed<double,3> > 
    GetDomainCuttingPlanes(const std::string &name) const;
    
  /** Returns a list of the names of all domains present in the
      project file.  Will throw an exception if no domain names are
      present, so call HasDomains() first to check is the file has
      domains defined. */
  std::vector<std::string> GetDomainNames() const;    
  
  /** The file names of the original segmentations for the project.
      Corresponds to the text found in the XML tag <segmentations> */

  /** The file names for the correspondence points for a model,
      specified by name.  If no name is given, this method just
      returns the first list of names give by a model_tag. */
  const std::vector<std::string> &GetModel(const std::string &name);
  
  /** Returns true if the model with the specified name is given. */
  bool HasModel(const std::string &name) const;

  /** Get the file names of the distance transforms that are to be
      used as input to an optimization process. Corresponds to the
      text found in the XML tag <distance_transforms> */
  const std::vector<std::string> &GetDistanceTransforms() const;

  /** Return the DOM node with the optimization_tag.  User can query
   optimization parameters, for example by making calls such as:
   GetOptimization()->GetAttribute("my_optimization_parameter").  This
   method returns 0 if no optimization_tag is present in the
   file. User should check the return value. */
  const DOMNode *GetOptimization() const
  {
    return m_DOMNode->GetChild(optimization_tag);
  }

  /** Returns the number of optimization scales. */
  unsigned int GetNumberOfOptimizationScales() const;

  /** Returns true if the optimization_tag element includes the given
      parameter at the given scale (optional).  Also returns false if
      no optimization_tag exists.*/
  bool HasOptimizationAttribute(const std::string& name, unsigned int i = 0) const;

  /** Returns an attribute of the optimization_tag element as a
      double-precision floating point number.  To work with attributes
      that are text or integer values, you can call
      GetOptimization->GetAttribute instead, which will return the
      attribute as text, and convert as needed.  This method is a
      convenience for the most common case.  User may optionally
      provide the scale, if there are multiple scales in the
      optimization.*/
  double GetOptimizationAttribute(const std::string &name, unsigned int i = 0) const;

  /** Returns true if the project has a procrustes tag */
  bool HasProcrustes() const;
  
  /** Returns true if the project has a variables_tag with the
      specified name. */
  bool HasVariables(const std::string &name) const;

  /** Returns a vector of doubles for the variable_tag with the
      specified name.  This is a convenience method for the common
      case.  For more complex parsing, the user can call
      GetVariablesText(name). This method will throw an exception if
      the variable name does not exist, so User should first check
      using HasVariables. */
  std::vector<double> GetVariables(const std::string &name) const;

  /** Returns the text for the specified variable. This method will
      throw an exception if the variable does no exist, so User should
      first check HasVariables.*/
  const std::vector<std::string > &GetVariablesText(const std::string &name) const;

  /** Stream the XML content of this project to a standard ostream.
      Note that this method cannot be made const because the necessary
      accessor methods in itkDOMNode are not defined as const. This
      method, however, does not modify the content of the
      PSMProject.*/
  void Stream(std::ostream &os);

 protected:
  PSMProject() {}; 
  void PrintSelf(std::ostream& os, Indent indent) const;
  virtual ~PSMProject() {};

  /** Helper function for Stream. Like Stream, this method cannot be const
      due to nonconst accessor methods in itkDOMNode. */
  void StreamChildren(PSMDOMNode *,std::ostream &os, Indent indent);

  /** Return the parent node of the Data section. */
  const PSMDOMNode *GetDataNode() const;

 private:
  PSMProject(const Self &); // purposely not implemented
  void operator=(const Self &); // purposely not implemented

  /** The DOM node that holds all of the XML data. */
  PSMDOMNode::Pointer m_DOMNode;
};

} // end namespace itk

#endif
