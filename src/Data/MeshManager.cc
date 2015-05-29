/*
 * Shapeworks license
 */

// qt
#include <QThread>

// vtk
#include <vtkPolyData.h>

#include <Data/MeshManager.h>

MeshManager::MeshManager(Preferences& prefs) : prefs_(prefs), meshCache_(prefs), meshGenerator_(prefs)
{
	this->thread_count_ = 0;
}

MeshManager::~MeshManager() {}

void MeshManager::clear_cache() { 
	this->meshCache_.clear(); 
	this->meshGenerator_.updatePipeline();
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
	  size_t tmp_max = this->prefs_.get_num_threads();
	  if (prefs_.get_use_powercrust())  //powercrust breaks on > 1 additional thread. 
		tmp_max = 1;
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
  float tmp = this->prefs_.get_cache_epsilon();
  float tmp2 = this->prefs_.get_num_threads();
  // check cache first
  if ( this->meshCache_.getMesh( shape ) )
  {
    polyData = this->meshCache_.getMesh( shape );
  }
  else
  {
	  if ( (prefs_.get_parallel_enabled()) && 
		  (this->prefs_.get_num_threads() > 0))
	  {
		 this->generateMesh(shape);
	  } else {
		 polyData = this->meshGenerator_.buildMesh( shape );
		 this->meshCache_.insertMesh( shape, polyData );
	  }
  }
  return polyData;
}

void MeshManager::handle_thread_complete() {
	this->thread_count_ --;
	size_t tmp_max = this->prefs_.get_num_threads();
	if (prefs_.get_use_powercrust())  //powercrust breaks on > 1 additional thread. 
		tmp_max = 1;
	while (!this->threads_.empty() && this->thread_count_ < tmp_max) {
		QThread *thread = this->threads_.back();
		this->threads_.pop_back();
		thread->start();
		this->thread_count_++;
	}
}