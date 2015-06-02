/*
 * Shapeworks license
 */

// qt
#include <QThread>

// vtk
#include <vtkPolyData.h>

#include <MeshManager.h>

MeshManager::MeshManager(Preferences& prefs) : prefs_(prefs), meshCache_(prefs), meshGenerator_(prefs)
{
	this->thread_count_ = 0;
}

MeshManager::~MeshManager() {}

void MeshManager::clear_cache() { 
	this->meshCache_.clear(); 
	this->threads_.clear();
	while(!this->workQueue_.isEmpty())
		this->workQueue_.pop();
}

void MeshManager::generateMesh( const vnl_vector<double>& shape )
{
  // check cache first
  if ( !this->meshCache_.getMesh( shape )
       && !this->workQueue_.isInside( shape ) )
  {
      this->workQueue_.push( shape );
	  //todo
	  QThread *thread = new QThread;
	  MeshWorker *worker = new MeshWorker(this->prefs_,shape,&this->workQueue_,&this->meshCache_);
	  worker->moveToThread(thread);
  	  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  	  connect(worker, SIGNAL(result_ready()),  this, SLOT(handle_thread_complete()));
  	  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  	  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	  size_t tmp_max = this->prefs_.getNumThreads();
	  if (this->thread_count_ < tmp_max) {
  		thread->start();
		this->thread_count_++;
	  } else 
		threads_.push_back(thread);
  }
}

vtkSmartPointer<vtkPolyData> MeshManager::getMesh( const vnl_vector<double>& shape )
{
  vtkSmartPointer<vtkPolyData> polyData;
  float tmp = this->prefs_.getCacheEpsilon();
  float tmp2 = this->prefs_.getNumThreads();
  // check cache first
  if ( this->prefs_.getCacheEnabled())
  {
    polyData = this->meshCache_.getMesh( shape );
	if (!polyData) {
	  if (prefs_.getParallelEnabled() && 
		  (this->prefs_.getNumThreads() > 0))
	  {
		 this->generateMesh(shape);
	  } else {
		 polyData = this->meshGenerator_.buildMesh( shape );
		 this->meshCache_.insertMesh( shape, polyData );
	  }
	}
  }
  else
  {
	polyData = this->meshGenerator_.buildMesh( shape );
  }
  return polyData;
}

void MeshManager::handle_thread_complete() {
	this->thread_count_ --;
	size_t tmp_max = this->prefs_.getNumThreads();
	while (!this->threads_.empty() && this->thread_count_ < tmp_max) {
		QThread *thread = this->threads_.back();
		this->threads_.pop_back();
		thread->start();
		this->thread_count_++;
	}
}