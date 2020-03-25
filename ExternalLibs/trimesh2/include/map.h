/*
 * Copyright (c) 2013 University of Utah
 */

#ifndef __map_H
#define __map_H

#include "MapFilter.h"
#include "ReduceFilter.h"

namespace bambam
{

/**
 * map is an abstraction of mapping over threads to simplify (and restrict) things a bit.
 *
 * the passed in functor should have an 
 *   void operator()(TInputImage::ConstPointer in, const TOutputImage::RegionType & threadRegion)
 *
 *   with the idea of updating out with the results of the map and the functor will hold any state information you need as you go.
 */
template<class TInputImage,
         class TOutputImage,
         class TFunctor>
struct map
{
private:
  typedef TInputImage InType;
  //typedef typename InType::ConstPointer InTypeP;
    typedef typename InType::ConstPointer InTypeP;
  typedef typename InType::IndexType InIndex;
  typedef typename InType::SizeType InSize;
  typedef typename InType::RegionType InRegion;

  typedef TOutputImage OutType;
  typedef typename OutType::Pointer OutTypeP;
  typedef typename OutType::IndexType OutOutdex;
  typedef typename OutType::SizeType OutSize;
  typedef typename OutType::RegionType OutRegion;

  typedef TFunctor FType;

public:
  static
  void
  run( const InTypeP in, FType & functor , size_t numThreads = 1, OutRegion outRegion = OutRegion())
  {
        typedef MapFilter<FType,InType,OutType> MF;
        typename MF::Pointer mf = MF::New();
        mf->SetFunctor(&functor); // handles output
        // if(numThreads>0)
        //     mf->SetNumberOfThreads(numThreads);
        mf->SetInput(in);
        if(outRegion.GetSize()[0] > 0)
        {
            mf->GetOutput()->SetRequestedRegion(outRegion);
        }
        else
        {
            mf->GetOutput()->SetRequestedRegion(in->GetLargestPossibleRegion());
        }

        mf->Update(); // throws
  }
};

/**
 * reduce is an abstraction of mapping over threads to simplify (and restrict) things a bit.
 *
 * the passed in functor should have an:
 *   TOutput operator()(TInputImage::ConstPointer in, const TOutputImage::RegionType & threadRegion)
 *   and
 *   TOutput operator()(const std::vector<TOutput> & in)
 *
 *   where the reduce is two steps, first each thread reduces it's region to a single output object,
 *     and the second step reduces a list (std::vector) of those objects to a single output object.
 *
 *   For example to compute the mean,  you might use a std::pair<float,size_t> obj, where obj.first = sum, obj.second = count,
 *     then in the second step you would sum all the obj.first and obj.second and divide first by the scond to get the mean.
 */
template<class TInputImage,
         class TOutput,
         class TFunctor>
struct reduce
{
public:
  typedef TFunctor FType;
  typedef TInputImage InType;
  typedef typename InType::ConstPointer InTypeP;
  typedef TOutput OutType;

  static
  TOutput
  run( const InTypeP in, TFunctor & functor , size_t numThreads = 1)
  {
    typedef ReduceFilter<FType,InType,OutType> RF;
    typename RF::Pointer rf = RF::New();
    rf->SetFunctor(&functor);
    // if(numThreads>0)
    //   rf->SetNumberOfThreads(numThreads);
    rf->SetInput(in);

    rf->Update(); // throws

    return rf->GetResult();
  }
};

} // end namespace bambam


#endif
