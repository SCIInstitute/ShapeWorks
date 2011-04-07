/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: param.cxx,v $

  Date:      $Date: 2011/03/24 01:17:44 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=======================================================================*/

#include "param.h"

#include <cstdio>



extern FILE *yyin;

extern int yyparse();

extern void yyrestart(FILE *);

extern void reset_yyvalues();

extern int syntax_error_count;

extern param::File OPF_file_parse_result;



namespace param

{

  

const value_type ValueTraits<float>::ValueType       = OPF_DECIMAL;

const value_type ValueTraits<long int>::ValueType    = OPF_INTEGER;

const value_type ValueTraits<std::string>::ValueType = OPF_STRING;



GenericValue Parameter::m_InvalidValue = GenericValue(false);

Parameter parameterFile::m_InvalidParameter = Parameter(false);



GenericValue *ValueFactory::Create( const value_type t)

  {

    if (t==OPF_DECIMAL) return new Value<float>();

    if (t==OPF_INTEGER) return new Value<long int>();

    if (t==OPF_STRING)  return new Value<std::string>();

    else return 0;

  }



GenericValue *ValueFactory::Copy( GenericValue *v)

{

  GenericValue *temp = Create( v->GetValueType() );

  switch (v->GetValueType()) {

  case OPF_DECIMAL:

    *dynamic_cast<Value<float> *>(temp)

      = *dynamic_cast<Value<float> *>(v);

    break;

  case OPF_INTEGER:

    *dynamic_cast<Value<long int> *>(temp)

      = *dynamic_cast<Value<long int> *>(v);

    break;

  case OPF_STRING:

    *dynamic_cast<Value<std::string> *>(temp)

      = *dynamic_cast<Value<std::string> *>(v);

    break;

  default:

    return 0;

  }

  return temp;

}



void ValueFactory::Delete( GenericValue *v)

{

  switch (v->GetValueType()) {

  case OPF_DECIMAL:

    delete dynamic_cast<Value<float> *>(v);

    break;

  case OPF_INTEGER:

    delete dynamic_cast<Value<long int> *>(v);

    break;

  case OPF_STRING:

    delete dynamic_cast<Value<std::string> *>(v);

    break;

  default:

    break;

  }

}



Parameter& parameterFile::operator[](const std::string &s)

{

  if ( m_File.find(s) == m_File.end() )

    {

      return m_InvalidParameter;

      //      std::string temp = "parameterFile::operator[]: Parameter \""

      //        + s + "\" was not found.";

      //      throw Exception(temp);

      //      DIE(temp.c_str());

    }

  return m_File[s];

}



GenericValue * Parameter

::GetElement( std::vector<GenericValue *>::size_type n) const

{

  if (this->valid() == false)

    {

      return &m_InvalidValue;

      //      DIE("OPF::Parameter: request for element in an invalid parameter");

    }

  if (this->size() <= n)

    {

      //      throw Exception(

      //      "OPF::Parameter: request for element past the end of parameter \""

      //      + this->Key()+"\"" );

      return &m_InvalidValue;

      //      DIE("OPF::Parameter: request for element past the end of parameter");

    }

  return std::vector<GenericValue *>::operator[](n);

}

 





void parameterFile::Initialize(const char* fn)

{

  if ((::yyin = ::fopen(fn, "r"))==NULL)

    {

    std::string temp =  "parameterFile::Could not open input file \""

      + std::string(fn) + "\".";

    throw Exception(temp);

    //DIE(temp.c_str());

    }

  else

    {

    ::yyrestart(yyin);

    ::reset_yyvalues();

    ::yyparse();

    ::fclose(yyin);

    

    if (::syntax_error_count > 0)

      {

      std::string temp =  "parameterFile::Input file \""

        + std::string(fn) + "\" contains syntax errors. ";

      throw Exception(temp);

      //DIE(temp.c_str());

      }

    else

      {

      m_File = ::OPF_file_parse_result;

      }

    }

  

}



} // end namespace param

