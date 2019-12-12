/*
 * Copyright (c) 2013 University of Utah
 */

#ifndef __MapFilter_HXX
#define __MapFilter_HXX

// constructor
template<class TFunctor,class TInput,class TOutput>
MapFilter<TFunctor,TInput,TOutput>::MapFilter()
{
  // disable dynamic multithreading (ITKv5)
  this->DynamicMultiThreadingOff();
}


template< class TFunctor,
          class TInput,
          class TOutput
        >
void
MapFilter<TFunctor,TInput,TOutput>
::AllocateOutputs()
{
  InTypeP outputPtr;

  // Allocate the output memory
  for ( typename itk::OutputDataObjectIterator it(this); !it.IsAtEnd(); it++ )
  {
    // Check whether the output is an image of the appropriate
    // dimension (use ProcessObject's version of the GetInput()
    // method since it returns the input as a pointer to a
    // DataObject as opposed to the subclass version which
    // static_casts the input to an TInputValue).
    outputPtr = dynamic_cast< InType * >( it.GetOutput() );

    if ( outputPtr )
    {
      typename InType::ConstPointer input = this->GetInput();
      outputPtr->SetLargestPossibleRegion( input->GetLargestPossibleRegion() );
      outputPtr->SetBufferedRegion( outputPtr->GetRequestedRegion() );
      //outputPtr->Allocate(); // don't allocate, just need region definition for thread work division.
    }
  }
}

template< class TFunctor,
          class TInput,
          class TOutput
        >
void
MapFilter<TFunctor,TInput,TOutput>
::ThreadedGenerateData(const OutRegion & outRegion, itk::ThreadIdType threadId)
{
  typename InType::ConstPointer in = this->GetInput();
  (*m_functor)( in, outRegion );
}


#endif
