### ShapeWorks Python module

Our shapeworks module with associated libraries also needs the compiled portion
of the library, which will be in ShapeWorks/bin directory when installed or the
build/bin directory during development.  Environment setup functions
automatically add it, so for development, just ensure PYTHONPATH begins with the
path to this directory and `import shapeworks` will work.

Ensure the module is importable with the following in python:
```
>>> import shapeworks

>>> shapeworks.utils.postive_factors(10)
[1, 2, 5, 10]
```
