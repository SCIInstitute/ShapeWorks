#include <Data/MeshMarchingWorker.h>
#include <algorithm>

MeshMarchingWorker::MeshMarchingWorker(size_t max) 
	:  max_(max), count_(0) {}

MeshMarchingWorker::~MeshMarchingWorker(void) {}

void MeshMarchingWorker::process() {
	count_--;
	while (!threads_.empty()) {
		QThread *thread = this->threads_.back();
		this->threads_.pop_back();
		thread->start();
		this->count_++;
		if (this->count_ >= this->max_) break;
	}
}

void MeshMarchingWorker::queueThread(MeshHandle mesh) {
	  QThread *thread = new QThread;
	  MeshThread *worker = new MeshThread(mesh);
	  worker->moveToThread(thread);
  	  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  	  connect(worker, SIGNAL(resultReady()), this, SLOT(process()));
  	  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  	  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	  if (this->count_ < this->max_) {
  		thread->start();
		this->count_++;
	  } else 
		threads_.push_back(thread);
}

MeshThread::MeshThread(MeshHandle mesh) : mesh_(mesh) {}

MeshThread::~MeshThread() {}

void MeshThread::process() {
	this->mesh_->Update();
	emit resultReady();
}