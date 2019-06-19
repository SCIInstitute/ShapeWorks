/*
 * Shapeworks license
 */

// vtk
#include <vtkPolyData.h>

#include <MeshWorker.h>

void MeshWorker::threadBegin()
{
  MeshWorkItem* workItem = this->workQueue->pop();

  // check that the queue wasn't empty
  if ( !workItem ) { return; }

  // make sure it's not already in the check the cache
  if ( !this->meshCache->getMesh( workItem->shape ) )
  {
    // add to the list of meshes being worked on
    this->workingQueue->push( workItem->shape );

    // build the mesh using our MeshGenerator
    vtkSmartPointer<vtkPolyData> mesh = this->meshGenerator.buildMesh( workItem->shape );

    // insert into the cache
    this->meshCache->insertMesh( workItem->shape, mesh );

    // remove from the list of meshes being worked on
    this->workingQueue->remove( workItem->shape );
 
    // wake up the main thread in case it was waiting for us to complete
    this->workDoneCondition->wakeAll();
  }
}

void MeshWorker::setWorkQueue( MeshWorkQueue* queue )
{
  this->workQueue = queue;
}

void MeshWorker::setMeshCache( MeshCache* cache )
{
  this->meshCache = cache;
}

MeshGenerator* MeshWorker::getMeshGenerator()
{
  return &( this->meshGenerator );
}

void MeshWorker::setWorkingQueue( MeshWorkQueue* queue )
{
  this->workingQueue = queue;
}

void MeshWorker::setWorkDoneCondition( QWaitCondition* condition )
{
  this->workDoneCondition = condition;
}
