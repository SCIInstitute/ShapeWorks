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
#include <QMetaType>

// vnl
#include "vnl/vnl_vector.h"

class MeshWorkItem
{
public:
  std::string filename;
  vnl_vector<double> points;
  int domain{0};

  size_t memory_size{0};

  friend bool operator<(const MeshWorkItem &a, const MeshWorkItem &b);

  friend bool operator==(const MeshWorkItem &a, const MeshWorkItem &b);
};

Q_DECLARE_METATYPE(MeshWorkItem);

using WorkList = std::list<MeshWorkItem>;

class MeshWorkQueue
{

public:
  MeshWorkQueue();
  ~MeshWorkQueue();

  void push(const MeshWorkItem &item);

  MeshWorkItem* pop();

  MeshWorkItem* get_next_work_item();

  bool isInside(const MeshWorkItem &item);

  void remove(const MeshWorkItem &item);

  bool isEmpty();

  int size();

private:

  bool in_inside_list(const MeshWorkItem &item, const WorkList& list);

  // for concurrent access
  QMutex mutex_;

  WorkList work_list_;

  WorkList processing_list_;
};
