#ifndef __MESH_MARCHING_WORKER_H__
#define __MESH_MARCHING_WORKER_H__

#include <Data/Mesh.h>
#include <QThread>
#include <vector>


class MeshThread : public QObject
{
	Q_OBJECT
public:
	MeshThread::MeshThread(MeshHandle m);
	MeshThread::~MeshThread();
	
public slots:
    void process(); 
signals:
    void resultReady();
private:
	MeshHandle mesh_;
};

class MeshMarchingWorker : public QObject
{
	Q_OBJECT
public:
	MeshMarchingWorker::MeshMarchingWorker(size_t max = 8);
	MeshMarchingWorker::~MeshMarchingWorker();
	void queueThread(MeshHandle mesh);
public slots:
    void process(); 
private:
	std::vector<QThread*> threads_;
	size_t max_;
	size_t count_;
};

#endif