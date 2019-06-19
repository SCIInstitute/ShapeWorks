#ifndef EdgeCheck_h
#define EdgeCheck_h

//namespace common
//{

template<class Iterator>
bool
edgeCheck(const Iterator & it)
{
  typedef typename Iterator::ImageType ImageType;
  typedef typename ImageType::IndexType IndexType;
  typedef typename ImageType::SizeType SizeType;

  IndexType location = it.GetIndex( it.GetCenterNeighborhoodIndex() );
  SizeType radius = it.GetRadius();
  SizeType size = it.GetImagePointer()->GetLargestPossibleRegion().GetSize();

  for(size_t i=0; i<radius.GetSizeDimension(); ++i)
  {
    if( location[i] < radius[i] )
      return true;
    if(  (size[i]-1) - location[i] < radius[i]  )
      return true;
  }
  return false;
}

//} //end namespace

#endif
