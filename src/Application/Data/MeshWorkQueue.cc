/*
 * Shapeworks license
 */

#include <Data/MeshWorkQueue.h>

MeshWorkQueue::MeshWorkQueue()
{}

MeshWorkQueue::~MeshWorkQueue()
{}

void MeshWorkQueue::push( const vnl_vector<double> &item )
{
  QMutexLocker locker( &this->mutex );
  this->workList.push_back( item );
}

MeshWorkItem* MeshWorkQueue::pop()
{
  QMutexLocker locker( &this->mutex );
  MeshWorkItem* item = new MeshWorkItem;

  if ( this->workList.empty() )
  {
    return NULL;
  }

  item->shape = this->workList.front();
  this->workList.pop_front();
  return item;
}

bool MeshWorkQueue::isInside( const vnl_vector<double> &item )
{
  QMutexLocker locker( &this->mutex );

  for ( WorkList::iterator it = this->workList.begin(); it != this->workList.end(); ++it )
  {
    if ( *it == item )
    {
      return true;
    }
  }
  return false;
}

void MeshWorkQueue::remove( const vnl_vector<double> &item )
{
  QMutexLocker locker( &this->mutex );

  this->workList.remove( item );
}

bool MeshWorkQueue::isEmpty()
{
  QMutexLocker locker( &this->mutex );
  return this->workList.empty();
}
