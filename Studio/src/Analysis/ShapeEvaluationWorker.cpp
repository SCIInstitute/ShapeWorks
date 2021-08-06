#include <iostream>

#include "ShapeEvaluationWorker.h"
namespace shapeworks {

//-----------------------------------------------------------------------------
ShapeEvaluationWorker::ShapeEvaluationWorker()
{}

//-----------------------------------------------------------------------------
ShapeEvaluationWorker* ShapeEvaluationWorker::create_worker()
{
  ShapeEvaluationWorker* shape_evaluation_worker = new ShapeEvaluationWorker();
  return shape_evaluation_worker;
}

//-----------------------------------------------------------------------------
void ShapeEvaluationWorker::async_run_generalization()
{

}

//-----------------------------------------------------------------------------
void ShapeEvaluationWorker::async_evaluate_shape()
{
  this->run_job();
}

//-----------------------------------------------------------------------------
void ShapeEvaluationWorker::process()
{
  std::cerr << "doing stuff on another thread\n";
  emit finished();
}
}
