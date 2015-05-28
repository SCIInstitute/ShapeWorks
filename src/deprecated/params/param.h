/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: param.h,v $
  Date:      $Date: 2011/03/24 15:29:48 $
  Version:   $Revision: 1.4 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=======================================================================*/
#ifndef _param_h_
#define _param_h_

#include <iostream>
#include <string.h>
#include <vector>
#include <map>

#ifdef _WIN32
#include <string>
#pragma warning( disable: 4541 )
#pragma warning( disable: 4244 )
#pragma warning( disable: 4996 )
#pragma warning( disable: 4273 )
#endif

namespace param 
{

//inline void DIE(const char *s)
//{
// std::cerr << "OPF fatal error: " << s << std::endl;
//  exit(-1);
//}
  
typedef enum { VALID, INVALID } ReturnStatusType;
   
class Exception
{
public:
  Exception(std::string s) : info(s) {}
  Exception() { info = "no information"; }
  std::string info;
};

inline std::ostream& operator<<(std::ostream& s, const Exception &f)
{ s << "OPF::Exception: " << f.info; return s; }
  
struct ltstr
{
  bool operator()(const std::string& s1, const std::string &s2) const
  {
    return strcmp(s1.c_str(), s2.c_str()) < 0;
  }
};
  
typedef enum{
  OPF_DECIMAL=0, OPF_INTEGER=1, OPF_STRING=2, OPF_UNKNOWN=3 } value_type;

class GenericValue
{
public:
  GenericValue(bool b) {m_Valid = b;}
  GenericValue() : m_Valid(true) {}
  virtual value_type GetValueType() const    { return OPF_UNKNOWN; }
  virtual void PrintSelf(std::ostream &s) const { s << "OPF_UNKNOWN_VALUE"; }
  bool valid() const { return m_Valid; }
  void valid(bool b) { m_Valid = b; }
  bool m_Valid;
};

template<class T> struct ValueTraits
{
  static const value_type ValueType;
  static void PrintValue(std::ostream &s, const T &v) { s << v; }
};
template<> struct ValueTraits<float>
{
  static const value_type ValueType;
  static void PrintValue(std::ostream &s, const float &v) { s << v; }
};
template<> struct ValueTraits<long int>
{
  static const value_type ValueType;
  static void PrintValue(std::ostream &s, const long int &v){ s << v; }
};
template<> struct ValueTraits<std::string>
{
  static const value_type ValueType;
  static void PrintValue(std::ostream &s, const std::string &v)
    { s << "\"" << v << "\""; }
};

template<class T>
class Value : public GenericValue
{
  T m_Value;
public:
  T GetValue() const { return m_Value; }
  T getValue() const { return m_Value; }
  void SetValue( const T& v ) { m_Value = v; }
  virtual value_type GetValueType() const { return ValueTraits<T>::ValueType; }
  virtual void PrintSelf(std::ostream &s) const
  { ValueTraits<T>::PrintValue(s, m_Value); }
};

class ValueFactory
{
public:
  static GenericValue *Create( const value_type );
  static GenericValue *Copy( GenericValue *v);
  static void Delete( GenericValue *v );
};

/**
 * \class Parameter
 * \brief Holds a lists of generic values and a keyword associated with those
 * values.
 *
 * Parameter is implemented as an array of pointers to the value superclass
 * because each value element's specific type is unknown, but must be
 * preserved.
 */
class Parameter : public std::vector<GenericValue *>
{
  bool m_Valid;
  std::string m_Key;
  static GenericValue m_InvalidValue;

  void PushBack( GenericValue *v )  // Private versions that use pointers.
  {                                 // Necessary for object copying because
    this->push_back( ValueFactory::Copy( v ) ); // const references cannot
  }                                             // be dynamically downcast.
  void PushFront( GenericValue *v )
  {
    this->insert( this->begin(),  ValueFactory::Copy( v ) );
  }
  
public:
  Parameter(bool b) { m_Valid = b; }
  Parameter() : m_Valid(true) { }
  ~Parameter() { this->Clear(); }
  const Parameter& operator=(const Parameter &p)
  {
    this->Clear();  m_Valid = p.m_Valid;  m_Key = p.m_Key;
    for (const_iterator it = p.begin(); it < p.end(); ++it)
      { this->PushBack(*it); }
    return *this;
  }
  Parameter(const Parameter& p) { *this = p; }
  GenericValue *operator[]( std::vector<GenericValue *>::size_type n) const
    {
      if (this->valid() == false) return &m_InvalidValue;
      else return this->GetElement(n);
    }  
  GenericValue *GetElement( std::vector<GenericValue *>::size_type n) const ;
  void Clear()
  {
    for (iterator it = this->begin(); it < this->end(); ++it)
      { ValueFactory::Delete(*it); }
    this->clear();
  }  
  void PushBack( GenericValue &v )  // Passing by reference allows us
  {                                 // to keep the subtype info
    this->push_back( ValueFactory::Copy( &v ) );
  }
  void PushFront( GenericValue &v )
  {
    this->insert( this->begin(),  ValueFactory::Copy( &v ) );
  }
  bool valid() const { return m_Valid; }
  void valid(const bool b) { m_Valid = b; }
  void Key(const std::string &k) { m_Key = k; }
  const char *getName() const { return m_Key.c_str();}
  std::string Key() const { return m_Key; };
  void PrintSelf(std::ostream &s) const
  {
    s << "( " << this->Key() << " ";
    for (const_iterator it = this->begin(); it < this->end(); ++it)
      { (*it)->PrintSelf(s); s << " ";}
    s << ")" << std::endl;
  }
};

typedef std::map<std::string, Parameter, ltstr> File;

class parameterFile
{
  static Parameter m_InvalidParameter;
  File m_File;
public:
  parameterFile() {}
  parameterFile(const char *s)        { this->Initialize(s);         }
  parameterFile(const std::string &s) { this->Initialize(s.c_str()); }
  void PrintSelf(std::ostream &s) const
  {
    for (std::map<std::string, Parameter, ltstr>::const_iterator
           it = m_File.begin(); it != m_File.end(); ++it )
      { it->second.PrintSelf(s); }
  }
  void Initialize(const char *);
  bool valid()    const { return m_File.empty(); }
  ::size_t size() const {  return m_File.size(); }
  bool empty()    const { return m_File.empty(); }
  Parameter &operator[](const std::string &s);
  Parameter &operator[](const char *s)
  {  std::string temp = s; return this->operator[](temp);  }

};

inline ReturnStatusType set(long &operand, GenericValue *value)
{
  if (value->valid() == false) return INVALID;

  if (value->GetValueType() != ValueTraits<long>::ValueType)
    {
      return INVALID;
      //      DIE("OPF::set: Operand is of the wrong type");
      //  throw Exception("OPF::set: Operand is of the wrong type");
    }
  operand = dynamic_cast<Value<long> *>(value)->GetValue();
  return VALID;
}

inline ReturnStatusType set(unsigned long &operand, GenericValue *value)
{
  if (value->valid() == false) return INVALID;
  
  if (value->GetValueType() != ValueTraits<long>::ValueType)
    {
      return INVALID;
      //      DIE("OPF::set: Operand is of the wrong type");
      //    throw Exception("OPF::set: Operand is of the wrong type");
    }

  operand = dynamic_cast<Value<long> *>(value)->GetValue();
  return VALID;
}

inline ReturnStatusType set(short &operand, GenericValue *value)
{
  if (value->valid() == false) return INVALID;
  
  if (value->GetValueType() != ValueTraits<long>::ValueType)
    {
      return INVALID;
      //      DIE("OPF::set: Operand is of the wrong type");
      //  throw Exception("OPF::set: Operand is of the wrong type");
    }

  operand = dynamic_cast<Value<long> *>(value)->GetValue();
  return VALID;
}

inline ReturnStatusType set(unsigned short &operand, GenericValue *value)
{
  if (value->valid() == false) return INVALID;

  if (value->GetValueType() != ValueTraits<long>::ValueType)
    {
      return INVALID;
      //      DIE("OPF::set: Operand is of the wrong type");
      //  throw Exception("OPF::set: Operand is of the wrong type");
    }

  operand = dynamic_cast<Value<long> *>(value)->GetValue();
  return VALID;
}

inline ReturnStatusType set(int &operand, GenericValue *value)
{
  if (value->valid() == false) return INVALID;

  if (value->GetValueType() != ValueTraits<long int>::ValueType)
    {
      return INVALID;
      //      DIE("OPF::set: Operand is of the wrong type");
      //        throw Exception("OPF::set: Operand is of the wrong type");
    }

  operand = dynamic_cast<Value<long int> *>(value)->GetValue();
  return VALID;
}

inline ReturnStatusType set(unsigned int &operand, GenericValue *value)
{
  if (value->valid() == false) return INVALID;

  if (value->GetValueType() != ValueTraits<long int>::ValueType)
    {
      return INVALID;
      //      DIE("OPF::set: Operand is of the wrong type");
      //        throw Exception("OPF::set: Operand is of the wrong type");
    }

  operand = dynamic_cast<Value<long int> *>(value)->GetValue();
  return VALID;
}

inline ReturnStatusType set(float &operand, GenericValue *value)
{
  if (value->valid() == false) return INVALID;

  if (value->GetValueType() != ValueTraits<float>::ValueType)
    {
      return INVALID;
      //      DIE("OPF::set: Operand is of the wrong type");
      //      throw Exception("OPF::set: Operand is of the wrong type");
    }

  operand = dynamic_cast<Value<float> *>(value)->GetValue();
  return VALID;
}

inline ReturnStatusType set(double &operand, GenericValue *value)
{
  if (value->valid() == false) return INVALID;

  if (value->GetValueType() != ValueTraits<float>::ValueType)
    {
      return INVALID;
      //      DIE("OPF::set: Operand is of the wrong type");
      //      throw Exception("OPF::set: Operand is of the wrong type");
    }

  operand = (double)(dynamic_cast<Value<float> *>(value)->GetValue());
  return VALID;
}

inline ReturnStatusType set(char *operand, GenericValue *value)
{
  if (value->valid() == false) return INVALID;

  if (value->GetValueType() != ValueTraits<std::string>::ValueType)
    {
    //DIE("OPF::set: Operand is of the wrong type");
      throw Exception("OPF::set: Operand is of the wrong type");
    }

  strcpy(operand, dynamic_cast<Value<std::string> *>(value)->GetValue().c_str());
  return VALID;
}

inline ReturnStatusType set(std::string &operand, GenericValue *value)
{
  if (value->valid() == false) return INVALID;

  if (value->GetValueType() != ValueTraits<std::string>::ValueType)
    {
    //DIE("OPF::set: Operand is of the wrong type");
    throw Exception("OPF::set: Operand is of the wrong type");
    }

  operand = dynamic_cast<Value<std::string> *>(value)->GetValue();
  return VALID;
}

template <class T>
inline ReturnStatusType set(GenericValue *value, T &operand)
  { return set(operand, value); }

inline std::ostream& operator<<(std::ostream& s, const parameterFile &f)
 { f.PrintSelf(s); return s; }

} // end namespace param

// Parameter set macro
#define PARAMSET(a,b,c,d,e,f) if (param::set(b, a[c][d]) != param::VALID) { e = false; b = f; }


#endif
