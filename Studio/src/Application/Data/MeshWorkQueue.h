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
  std::string filename;
  vnl_vector<double> shape;
  int domain;

  size_t memory_size;

  friend bool operator< (const MeshWorkItem &a, const MeshWorkItem &b);

  friend bool operator== (const MeshWorkItem &a, const MeshWorkItem &b);

};

using WorkList = std::list<MeshWorkItem>;

class MeshWorkQueue
{

public:
  MeshWorkQueue();
  ~MeshWorkQueue();

  void push(const MeshWorkItem &item);

  MeshWorkItem* pop();

  bool isInside(const MeshWorkItem &item);

  void remove(const MeshWorkItem &item);

  bool isEmpty();

private:

  // for concurrent access
  QMutex mutex_;

  WorkList work_list_;
};
