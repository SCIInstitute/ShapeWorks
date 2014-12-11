#include <Application/Preferences.h>
#include <Data/MeshManager.h>
#include <Data/Mesh.h>
#include <QThread.h>

//---------------------------------------------------------------------------
MeshManager::MeshManager()
{}

//---------------------------------------------------------------------------
MeshManager::~MeshManager()
{}

//---------------------------------------------------------------------------
void MeshManager::queue_mesh_creation( QString filename, MeshSettings settings )
{}

//---------------------------------------------------------------------------
void MeshManager::queue_mesh_creation( const vnl_vector<double>& vnl_points, MeshSettings settings )
{}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> MeshManager::get_mesh( QString filename, MeshSettings settings, bool wait )
{
  QSharedPointer<Mesh> mesh = QSharedPointer<Mesh>( new Mesh() );

  return mesh;
}

//---------------------------------------------------------------------------
QSharedPointer<Mesh> MeshManager::get_mesh( const vnl_vector<double>& vnl_points, MeshSettings settings, bool wait )
{
  QSharedPointer<Mesh> mesh = QSharedPointer<Mesh>( new Mesh() );

  return mesh;
}

//---------------------------------------------------------------------------
void MeshManager::initialize_threads()
{
  //std::cerr << "shutting down threads\n";
  this->shutdown_threads();
  //std::cerr << "done shutting down threads\n";

  if ( !Preferences::Instance().get_parallel_enabled() )
  {
    threads.resize( 0 );
    workers.resize( 0 );
    return;
  }

  int numThreads = Preferences::Instance().get_num_threads() - 1;
  if ( numThreads > 0 )
  {
    threads.resize( numThreads );
    workers.resize( numThreads );

    //std::cerr << "Starting " << numThreads << " threads\n";

    for ( int i = 0; i < numThreads; i++ )
    {
      threads[i] = new QThread;
      workers[i] = new MeshWorker;
      workers[i]->setWorkQueue( &( this->workQueue ) );
      workers[i]->setWorkingQueue( &( this->workingQueue ) );
      workers[i]->setWorkDoneCondition( &( this->workDoneCondition ) );
      workers[i]->setMeshCache( &( this->meshCache ) );

      workers[i]->getMeshGenerator()->setNeighborhoodSize( this->neighborhoodSize );
      workers[i]->getMeshGenerator()->setSampleSpacing( this->sampleSpacing );
      workers[i]->getMeshGenerator()->setSmoothingAmount( this->smoothingAmount );
      workers[i]->getMeshGenerator()->setUsePowerCrust( this->usePowerCrust );
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

//---------------------------------------------------------------------------
void MeshManager::shutdown_threads()
{
  if ( this->threads.size() != 0 )
  {
    for ( size_t i = 0; i < this->threads.size(); i++ )
    {
      while ( this->threads[i]->isRunning() )
      {
        this->threads[i]->exit();
        this->threads[i]->wait( 1000 );
      }
    }

    for ( size_t i = 0; i < this->threads.size(); i++ )
    {
      delete this->threads[i];
      delete this->workers[i];
    }
  }
}
