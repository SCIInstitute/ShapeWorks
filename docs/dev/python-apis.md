# How to Add Python APIs?

Python APIs in ShapeWorks are pybind wrappers of C++ functions. 

1. Header file must be included.

2. New class can be added like this:

```
py:class_<c++ClassName> objectName(m, "pythonClassName);
```

3. New function can be added like this:

```
objectName.def("pythonFunctionName", &c++ClassName::c++FunctionName, "description/help", "argument1"_a, "argument2"_a=defaultValue)
```
