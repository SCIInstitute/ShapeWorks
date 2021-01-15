#include <pybind11/pybind11.h>

namespace py = pybind11;

class Ball {
public:
  int foo{42};
  Ball& setFoo(int val) { foo = val; return *this; }
};

PYBIND11_MODULE(ballandchain, m)
{
  py::class_<Ball>(m, "Ball")
  .def(py::init<>())
  .def_readonly("foo", &Ball::foo) 
  .def("setFoo", &Ball::setFoo) //successfully chains in Python
  .def("setFooLambdaImplicitRet", [](Ball& self, int val) { return self.setFoo(val); }) //fails to chain
  .def("setFooLambdaExplicitRet", [](Ball& self, int val) -> Ball& { return self.setFoo(val); }) //fails to chain?
  .def("setFooLambdaAutoRet", [](Ball& self, int val) -> decltype(auto) { return self.setFoo(val); }) //fails to chain?
  ;
}
