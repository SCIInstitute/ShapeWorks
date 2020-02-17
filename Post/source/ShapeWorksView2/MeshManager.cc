/*
 * Shapeworks license
 */

// qt
#include <QThread>

// vtk
#include <vtkPolyData.h>

#include <MeshManager.h>
#include <Preferences.h>

MeshManager::MeshManager()
  :
  surfaceReconstructor_(new SurfaceReconstructor())
{
  this->smoothingAmount = 0;

  // monitor changes to threading preferences
  QObject::connect(
    &Preferences::Instance(), SIGNAL(threadingChangedSignal()),
    this, SLOT(initializeThreads()));

  this->initializeThreads();

  this->meshGenerator.set_surface_reconstructor(this->surfaceReconstructor_);
}

MeshManager::~MeshManager()
{}

void MeshManager::setNeighborhoodSize(int size)
{
  this->neighborhoodSize = size;
  this->initializeThreads();
  this->meshCache.clear();
  this->meshGenerator.setNeighborhoodSize(size);
}

int MeshManager::getNeighborhoodSize()
{
  return this->neighborhoodSize;
}

void MeshManager::setSampleSpacing(double spacing)
{
  this->sampleSpacing = spacing;
  this->initializeThreads();
  this->meshCache.clear();
  this->meshGenerator.setSampleSpacing(spacing);
}

double MeshManager::getSampleSpacing()
{
  return this->sampleSpacing;
}

void MeshManager::setSmoothingAmount(float amount)
{
  this->smoothingAmount = amount;
  this->initializeThreads();
  this->meshCache.clear();
  this->meshGenerator.setSmoothingAmount(amount);
}

float MeshManager::getSmoothing()
{
  return this->smoothingAmount;
}

void MeshManager::setUsePowerCrust(bool enabled)
{
  this->usePowerCrust = enabled;
  this->initializeThreads();
  this->meshCache.clear();
  this->meshGenerator.setUsePowerCrust(enabled);
}

void MeshManager::generateMesh(const vnl_vector<double>& shape)
{
  /// disable pre-generation for the moment
  //return;

  if (this->usePowerCrust) {
    // the powercrust code uses all manner of global variables and cannot be run in parallel right now
    return;
  }

  if (!Preferences::Instance().getParallelEnabled() || this->threads.size() <= 0) {
    return;
  }

  // check cache first
  if (!this->meshCache.getMesh(shape)
      && !this->workingQueue.isInside(shape)
      && !this->workQueue.isInside(shape)) {
    this->workQueue.push(shape);

    // wake up a thread
    static int threadId = 0;
    QMetaObject::invokeMethod(workers[threadId], "threadBegin", Qt::QueuedConnection);

    // wrap
    threadId++;
    threadId = threadId % this->threads.size();
  }
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshManager::getMesh(const vnl_vector<double>& shape)
{
  vtkSmartPointer<vtkPolyData> polyData;

  // remove it from the work queue if it is present
  this->workQueue.remove(shape);

  // if another thread is already working on it, wait here
  while (this->workingQueue.isInside(shape)) {
    QMutex mutex;
    mutex.lock();
    this->workDoneCondition.wait(&mutex, 5000);
  }

  // check cache first
  if (this->meshCache.getMesh(shape)) {
    polyData = this->meshCache.getMesh(shape);
  }
  else {
    polyData = this->meshGenerator.buildMesh(shape);
    this->meshCache.insertMesh(shape, polyData);
  }

  return polyData;
}

//---------------------------------------------------------------------------
QSharedPointer<SurfaceReconstructor> MeshManager::getSurfaceReconstructor()
{
  return this->surfaceReconstructor_;
}

//---------------------------------------------------------------------------
void MeshManager::initializeThreads()
{
  this->shutdownThreads();

  if (!Preferences::Instance().getParallelEnabled()) {
    threads.resize(0);
    workers.resize(0);
    return;
  }

  unsigned int numThreads = Preferences::Instance().getNumThreads();

  if (numThreads > 0) {
    threads.resize(numThreads);
    workers.resize(numThreads);

    for (unsigned int i = 0; i < numThreads; i++) {
      threads[i] = new QThread;
      workers[i] = new MeshWorker;
      workers[i]->setWorkQueue(&(this->workQueue));
      workers[i]->setWorkingQueue(&(this->workingQueue));
      workers[i]->setWorkDoneCondition(&(this->workDoneCondition));
      workers[i]->setMeshCache(&(this->meshCache));

      workers[i]->getMeshGenerator()->setNeighborhoodSize(this->neighborhoodSize);
      workers[i]->getMeshGenerator()->setSampleSpacing(this->sampleSpacing);
      workers[i]->getMeshGenerator()->setSmoothingAmount(this->smoothingAmount);
      workers[i]->getMeshGenerator()->setUsePowerCrust(this->usePowerCrust);
      workers[i]->getMeshGenerator()->set_surface_reconstructor(this->surfaceReconstructor_);

      workers[i]->moveToThread(threads[i]);
      threads[i]->start();
    }
  }
  else {
    threads.resize(0);
    workers.resize(0);
  }
}

void MeshManager::shutdownThreads()
{
  if (this->threads.size() != 0) {
    for (size_t i = 0; i < this->threads.size(); i++) {
      while (this->threads[i]->isRunning()) {
        this->threads[i]->exit();
        this->threads[i]->wait(1000);
      }
    }

    for (size_t i = 0; i < this->threads.size(); i++) {
      delete this->threads[i];
      delete this->workers[i];
    }
  }
  std::cerr << "done shutting down threads\n";
}
