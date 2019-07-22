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

#ifndef __itkPSMDomNodeXMLReader_h
#define __itkPSMDomNodeXMLReader_h

#include "itkPSMDOMNode.h"
#include "itkObject.h"

#include <istream>

namespace itk
{

/**
 * \class PSMDOMNodeXMLReader 
 *
 * \brief Class to read a special PSM DOM object from an XML file or
 * an input stream.
 *
 * This class is used by the PSMProjectReader to produce the DOM tree
 * data structure that is passed to a PSMProject.
 *
 * \sa DOMReader
 * \sa PSMDOMNode
 * \sa PSMProject
 *
 * \ingroup ITKIOXML
 */
class PSMDOMNodeXMLReader : public Object
{
public:
  /** Standard class typedefs. */
  typedef PSMDOMNodeXMLReader         Self;
  typedef Object                      Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMDOMNodeXMLReader, Object);

  typedef PSMDOMNode             DOMNodeType;
  typedef DOMNodeType::Pointer DOMNodePointer;

  /** Set the input XML filename. */
  itkSetStringMacro(FileName);

  /** Get the input XML filename. */
  itkGetStringMacro(FileName);

  /**
   * The output DOM object will be created automatically, but the user
   * can appoint a user DOM object as the output by calling this function.
   */
  itkSetObjectMacro( DOMNode, DOMNodeType );

  /** Get the output DOM object for full access. */
  itkGetObjectMacro( DOMNode, DOMNodeType );

  /** Get the output DOM object for read-only access. */
  itkGetConstObjectMacro( DOMNode, DOMNodeType );

  /**
   * Function called by Update() or end-users to generate the output DOM object
   * from an input stream such as file, string, etc.
   */
  void Update( std::istream& is );

  /**
   * Function called by end-users to generate the output DOM object from the input XML file.
   */
  virtual void Update();

  /** Callback function -- called from XML parser with start-of-element
   * information.
   */
  virtual void StartElement( const char* name, const char** atts );

  /** Callback function -- called from XML parser when ending tag
   * encountered.
   */
  virtual void EndElement( const char* name );

  /** Callback function -- called from XML parser with the character data
   * for an XML element.
   */
  virtual void CharacterDataHandler( const char* text, int len );

protected:
  PSMDOMNodeXMLReader();

private:
  PSMDOMNodeXMLReader(const Self &); //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  /** Variable to hold the input XML file name. */
  std::string m_FileName;

  /** Variable to hold the output DOM object, created internally or supplied by the user. */
  DOMNodePointer m_DOMNode;

  /** Variable to keep the current context during XML parsing. */
  DOMNodeType* m_Context;
};

} // namespace itk

/** The operator ">>" is overloaded such that a DOM object can be conveniently read from an input stream. */
inline std::istream& operator>>( std::istream& is, itk::PSMDOMNode& object )
{
  itk::PSMDOMNodeXMLReader::Pointer reader = itk::PSMDOMNodeXMLReader::New();
  reader->SetDOMNode( &object );
  reader->Update( is );
  return is;
}

#endif // __itkPSMDOMNodeXMLReader_h
