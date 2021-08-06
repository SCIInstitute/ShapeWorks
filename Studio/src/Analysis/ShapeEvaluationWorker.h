#pragma once

#include <Data/Worker.h>

namespace shapeworks {

class ShapeEvaluationWorker : public Worker
{
  Q_OBJECT
public:

  ShapeEvaluationWorker();

  static ShapeEvaluationWorker* create_worker();

  void async_run_generalization();

  void async_evaluate_shape();

  void process() override;
};
}
