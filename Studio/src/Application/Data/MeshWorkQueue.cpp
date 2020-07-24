#include <Data/MeshWorkQueue.h>

//---------------------------------------------------------------------------
bool operator<(const MeshWorkItem &a, const MeshWorkItem &b)
{
  if (a.filename == b.filename) {
    // either they are the same file, or both empty, meaning correspondence points and not a file
    if (a.filename == "") {
      if (a.domain != b.domain) {
        // different domains
        return a.domain < b.domain;
      }

      if (a.points.size() < b.points.size()) {
        return true;
      }
      double eps = 1e-3f;
      //double eps = MeshCache::pref_ref_->get_preference("cache_epsilon", 1e-3f);
      for (unsigned i = 0; i < a.points.size(); i++) {
        if ((a.points[i] < b.points[i]) && ((b.points[i] - a.points[i]) > eps)) {
          return true;
        }
        else if (b.points[i] < a.points[i] && ((a.points[i] - b.points[i]) > eps)) {
          return false;
        }
      }
      return false;
    }
    else {
      // same
      return a.filename < b.filename;
    }
  }
  else {
    return a.filename < b.filename;
  }
}

//---------------------------------------------------------------------------
bool operator==(const MeshWorkItem &a, const MeshWorkItem &b)
{
  //a == b *equals* !(a < b) && !(b < a)

  if (!(a < b) && !(b < a)) {
    return true;
  }

  return false;
}

//---------------------------------------------------------------------------
MeshWorkQueue::MeshWorkQueue()
{}

//---------------------------------------------------------------------------
MeshWorkQueue::~MeshWorkQueue()
{}

//---------------------------------------------------------------------------
void MeshWorkQueue::push(const MeshWorkItem &item)
{
  QMutexLocker locker(&this->mutex_);
  this->work_list_.push_back(item);
}

//---------------------------------------------------------------------------
MeshWorkItem* MeshWorkQueue::get_next_work_item()
{
  QMutexLocker locker(&this->mutex_);

  if (this->work_list_.empty()) {
    return NULL;
  }

  MeshWorkItem item = this->work_list_.back();
  this->work_list_.pop_back();

  this->processing_list_.push_back(item);

  MeshWorkItem* mesh_item = new MeshWorkItem;
  *mesh_item = item;

  return mesh_item;
}

//---------------------------------------------------------------------------
bool MeshWorkQueue::is_inside(const MeshWorkItem &item)
{
  QMutexLocker locker(&this->mutex_);

  return this->in_inside_list(item, this->work_list_) || this->in_inside_list(item,
                                                                              this->processing_list_);
}

//---------------------------------------------------------------------------
void MeshWorkQueue::remove(const MeshWorkItem &item)
{
  QMutexLocker locker(&this->mutex_);

  this->work_list_.remove(item);
  this->processing_list_.remove(item);
}

//---------------------------------------------------------------------------
bool MeshWorkQueue::is_empty()
{
  QMutexLocker locker(&this->mutex_);
  return this->work_list_.empty();
}

//---------------------------------------------------------------------------
int MeshWorkQueue::size()
{
  return this->work_list_.size();
}

//---------------------------------------------------------------------------
bool MeshWorkQueue::in_inside_list(const MeshWorkItem &item, const WorkList &list)
{
  for (WorkList::const_iterator it = list.begin(); it != list.end(); ++it) {
    if (it->filename != "") {
      if (it->filename == item.filename) {
        return true;
      }
    }
    else {
      if (it->domain == item.domain && it->points == item.points) {
        return true;
      }
    }
  }
  return false;
}
