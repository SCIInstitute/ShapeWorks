/*
 * Shapeworks license
 */

// qt
#include <QThread>

// vtk
#include <vtkPolyData.h>

#include <Data/MeshManager.h>

MeshManager::MeshManager(Preferences& prefs) : prefs_(prefs), meshCache(prefs), meshGenerator(prefs)
{

  // monitor changes to threading preferences
  QObject::connect(
    &prefs_, SIGNAL( threadingChangedSignal() ),
    this, SLOT( initializeThreads() ) );

  this->initializeThreads();
}

MeshManager::~MeshManager()
{
  this->shutdownThreads();
}

void MeshManager::setNeighborhoodSize( int size )
{
  this->neighborhoodSize = size;
  this->initializeThreads();
  this->meshCache.clear();
  this->meshGenerator.setNeighborhoodSize( size );
}

void MeshManager::setSampleSpacing( double spacing )
{
  this->sampleSpacing = spacing;
  this->initializeThreads();
  this->meshCache.clear();
  this->meshGenerator.setSampleSpacing( spacing );
}

void MeshManager::clear_cache() { 
    this->initializeThreads();
	this->meshCache.clear(); 
	this->meshGenerator.updatePipeline();
}

void MeshManager::generateMesh( const vnl_vector<double>& shape )
{
  // check cache first
  if ( !this->meshCache.getMesh( shape )
       && !this->workingQueue.isInside( shape )
       && !this->workQueue.isInside( shape ) )
  {
    this->workQueue.push( shape );

    // wake up a thread
    static int threadId = 0;
    QMetaObject::invokeMethod( workers[threadId], "threadBegin", Qt::QueuedConnection );

    // wrap
    threadId++;
    threadId = threadId % this->threads.size();
  }
}

vtkSmartPointer<vtkPolyData> MeshManager::getMesh( const vnl_vector<double>& shape )
{
  vtkSmartPointer<vtkPolyData> polyData;

  // remove it from the work queue if it is present
  this->workQueue.remove( shape );

  // if another thread is already working on it, wait here
  while ( this->workingQueue.isInside( shape ) )
  {
    QMutex mutex;
    mutex.lock();
    this->workDoneCondition.wait( &mutex, 5000 );
  }

  // check cache first
  if ( this->meshCache.getMesh( shape ) )
  {
    polyData = this->meshCache.getMesh( shape );
  }
  else
  {
	  if ( (prefs_.get_parallel_enabled()) && (this->threads.size() > 0) && (!this->prefs_.get_use_powercrust()))
	  {
		 this->generateMesh(shape);
	  } else {
		 polyData = this->meshGenerator.buildMesh( shape );
		 this->meshCache.insertMesh( shape, polyData );
	  }
  }
  return polyData;
}

void MeshManager::initializeThreads()
{
  //std::cerr << "shutting down threads\n";
  this->shutdownThreads();
  //std::cerr << "done shutting down threads\n";

  if ( !prefs_.get_parallel_enabled() )
  {
    threads.resize( 0 );
    workers.resize( 0 );
    return;
  }

  int numThreads = prefs_.get_num_threads() - 1;
  if ( numThreads > 0 )
  {
    threads.resize( numThreads );
    workers.resize( numThreads );

    //std::cerr << "Starting " << numThreads << " threads\n";

    for ( int i = 0; i < numThreads; i++ )
    {
      threads[i] = new QThread;
      workers[i] = new MeshWorker(prefs_);
      workers[i]->setWorkQueue( &( this->workQueue ) );
      workers[i]->setWorkingQueue( &( this->workingQueue ) );
      workers[i]->setWorkDoneCondition( &( this->workDoneCondition ) );
      workers[i]->setMeshCache( &( this->meshCache ) );

      workers[i]->getMeshGenerator()->setNeighborhoodSize( this->neighborhoodSize );
      workers[i]->getMeshGenerator()->setSampleSpacing( this->sampleSpacing );
      workers[i]->moveToThread( threads[i] );
      threads[i]->start();
    }
  }
  else
  {
    threads.resize( 0 );
    workers.resize( 0 );
  }
}

void MeshManager::shutdownThreads()
{
  if ( this->threads.size() != 0 )
  {
    for ( size_t i = 0; i < this->threads.size(); i++ )
    {
      while( this->threads[i]->isRunning() )
      {
        this->threads[i]->exit();
        this->threads[i]->wait(1000);
      }
    }

    for ( size_t i = 0; i < this->threads.size(); i++ )
    {
      delete this->threads[i];
      delete this->workers[i];
    }
  }
}
