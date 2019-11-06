/*
 * Copyright (c) 2013 University of Utah
 */

#ifndef __ReduceFilter_H
#define __ReduceFilter_H

#include <itkImageToImageFilter.h>

template< 
          class TFunctor,
          class TInput,
          class TOutput
        >
class ReduceFilter : public itk::ImageToImageFilter<TInput,TInput>
{
public:
  /** types */
  typedef TInput                  InType;
  typedef typename InType::Pointer InTypeP;
  typedef typename InType::RegionType InRegion;
  typedef TOutput                 OutType;
  typedef std::vector<OutType>    OutList;
  typedef ReduceFilter           Self;
  typedef typename itk::ImageToImageFilter<InType,OutType> Superclass;
  typedef typename itk::SmartPointer<Self>                                  Pointer;
  typedef typename itk::SmartPointer<const Self>                            ConstPointer;
  typedef TFunctor FType;

  itkNewMacro(Self);
  itkTypeMacro(ReduceFilter, ImageToImageFilter);

  void SetFunctor(FType * f) { m_functor = f; }
  const FType * GetFunctor() const { return m_functor; }
  FType * GetFunctor() { return m_functor; }

  const OutType & GetResult() const { return m_result; }
  OutType & GetResult() { return m_result; }

protected:
  ReduceFilter();

  virtual void BeforeThreadedGenerateData();
  virtual void AllocateOutputs();
  virtual void ThreadedGenerateData(const InRegion & outputRegionForThread,
                                    itk::ThreadIdType threadId);
  virtual void AfterThreadedGenerateData();

private:
  FType * m_functor;
  OutList m_list;
  OutType m_result;
};

#include "ReduceFilter.hxx" // implementation

#endif
