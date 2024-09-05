
#include "SharedBoundaryJob.h"

#include <Data/Session.h>
#include <Logging.h>

namespace shapeworks {

//---------------------------------------------------------------------------
SharedBoundaryJob::SharedBoundaryJob(QSharedPointer<Session> session, int first_domain, int second_domain)
    : session_(std::move(session)), first_domain_(first_domain), second_domain_(second_domain) {}

//---------------------------------------------------------------------------
void SharedBoundaryJob::run() {
  SW_DEBUG("Running shared boundary job");
  Q_EMIT progress(0);
  // sleep 5 seconds
  QThread::sleep(5);
  Q_EMIT progress(100);
}

}  // namespace shapeworks
