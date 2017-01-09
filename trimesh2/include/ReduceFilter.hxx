/*
 * Copyright (c) 2013 University of Utah
 */

#ifndef __ReduceFilter_HXX
#define __ReduceFilter_HXX

template< class TFunctor,
          class TInput,
          class TOutput
        >
void
ReduceFilter<TFunctor,TInput,TOutput>
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
      InRegion outputSize = input->GetLargestPossibleRegion();
      outputPtr->SetRegions( outputSize );
      //outputPtr->Allocate(); // don't allocate, just need region definition for thread work division.
    }
  }
}

template< class TFunctor,
          class TInput,
          class TOutput
        >
void
ReduceFilter<TFunctor,TInput,TOutput>
::BeforeThreadedGenerateData()
{
  m_list.resize( this->GetNumberOfThreads() );
}


template< class TFunctor,
          class TInput,
          class TOutput
        >
void
ReduceFilter<TFunctor,TInput,TOutput>
::ThreadedGenerateData(const InRegion & outRegion, itk::ThreadIdType threadId)
{
  typename InType::ConstPointer in = this->GetInput();
  m_list[threadId] = (*m_functor)( in, outRegion );
}

template< class TFunctor,
          class TInput,
          class TOutput
        >
void
ReduceFilter<TFunctor,TInput,TOutput>
::AfterThreadedGenerateData()
{
  m_result = (*m_functor)( m_list );
}



#endif
