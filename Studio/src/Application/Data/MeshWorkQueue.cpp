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

      if (a.shape.size() < b.shape.size()) {
        return true;
      }
      double eps = 1e-3f;
      //double eps = MeshCache::pref_ref_->get_preference("cache_epsilon", 1e-3f);
      for (unsigned i = 0; i < a.shape.size(); i++) {
        if ((a.shape[i] < b.shape[i]) && ((b.shape[i] - a.shape[i]) > eps)) {
          return true;
        }
        else if (b.shape[i] < a.shape[i] && ((a.shape[i] - b.shape[i]) > eps)) {
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
MeshWorkItem* MeshWorkQueue::pop()
{
  QMutexLocker locker(&this->mutex_);
  MeshWorkItem* item = new MeshWorkItem;

  if (this->work_list_.empty()) {
    return NULL;
  }

  *item = this->work_list_.front();
  this->work_list_.pop_front();
  return item;
}

//---------------------------------------------------------------------------
bool MeshWorkQueue::isInside(const MeshWorkItem &item)
{
  QMutexLocker locker(&this->mutex_);

  for (WorkList::iterator it = this->work_list_.begin(); it != this->work_list_.end(); ++it) {
    if (it->filename != "") {
      if (it->filename == item.filename) {
        return true;
      }
    }
    else {
      if (it->domain == item.domain && it->shape == item.shape) {
        return true;
      }
    }
  }
  return false;
}

//---------------------------------------------------------------------------
void MeshWorkQueue::remove(const MeshWorkItem &item)
{
  QMutexLocker locker(&this->mutex_);

  this->work_list_.remove(item);
}

//---------------------------------------------------------------------------
bool MeshWorkQueue::isEmpty()
{
  QMutexLocker locker(&this->mutex_);
  return this->work_list_.empty();
}
