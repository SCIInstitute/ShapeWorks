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

#ifndef __itkPSMDOMNode_h
#define __itkPSMDOMNode_h

#include "itkDOMNode.h"
#include "itkObject.h"
#include "itkObjectFactory.h"

#include <string>
#include <vector>
#include <list>
#include <map>

namespace itk
{

/**
 * \class PSMDOMNode
 * \brief Class to represent a node in the PSM Document Object Model (DOM) tree structure.
 *
 * This class extends the itkDOMNode class to include storage of text
 * strings at each node.  This allows parsing documents that have text
 * between the opening and closing xml tags.  This is useful for
 * specifying lists of file names or other parameters with a tag,
 * e.g. <inputfiles>, <outputfiles>, etc.  
 *
 * \ingroup ITKIOXML PSM
 */
class PSMDOMNode : public DOMNode
{
public:
  /** Standard class typedefs. */
  typedef PSMDOMNode                     Self;
  typedef DOMNode                      Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMDOMNode, DOMNode);

  /** Container to return all or a subset of the children of a DOM node. */
  typedef Superclass::ChildrenListType ChildrenListType;
  typedef Superclass::ConstChildrenListType ConstChildrenListType;

  /** An attribute is a pair of <key,value>, both key and value are strings. */
  typedef Superclass::AttributeKeyType AttributeKeyType;
  typedef Superclass::AttributeValueType AttributeValueType;

  typedef Superclass::AttributeItemType AttributeItemType;

  /** Container to return the attributes of a DOM node. */
  typedef Superclass::AttributesListType AttributesListType;

  typedef Superclass::SizeType SizeType;
  typedef Superclass::IdentifierType IdentifierType;
  typedef Superclass::OffsetType OffsetType;

  /** Get the vector of text lines for this element (node) */
  const std::vector<std::string> &GetText() const
    { 
      return m_TextVector; 
    }
  std::vector<std::string> &GetText()
    { 
      return m_TextVector; 
    }

  /** Add a string to the TextVector. */
  void AddText(const std::string &s)
  {
    m_TextVector.push_back(s);
  }

protected:
  PSMDOMNode() {}
  void PrintSelf(std::ostream &os, Indent indent) const
  { Superclass::PrintSelf(os,indent); }

private:
  /** A vector of strings that holds any text found in this
      element. Text is stored line by line -- i.e., each vector entry
      is a new line.*/
  std::vector<std::string> m_TextVector;

  PSMDOMNode(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented

};

} // namespace itk

#include "itkStringTools.h"
#include "itkFancyString.h"

#endif // __itkPSMDOMNode_h
