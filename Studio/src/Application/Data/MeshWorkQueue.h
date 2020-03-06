#pragma once

/**
 * @file MeshWorkQueue.h
 * @brief Provides concurrent access to a list of shapes to work needing reconstruction
 *
 */

// stl
#include <list>

// qt
#include <QMutex>

// vnl
#include "vnl/vnl_vector.h"

class MeshWorkItem
{
public:
  vnl_vector<double> shape;
};

class MeshWorkQueue
{

public:
  MeshWorkQueue();
  ~MeshWorkQueue();

  void push(const vnl_vector<double> &item);

  MeshWorkItem* pop();

  bool isInside(const vnl_vector<double> &item);

  void remove(const vnl_vector<double> &item);

  bool isEmpty();

private:

  // for concurrent access
  QMutex mutex;

  typedef std::list< vnl_vector<double>> WorkList;

  WorkList workList;
};
