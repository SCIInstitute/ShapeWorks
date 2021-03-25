### ShapeWorks Python module

This contains first draft of the shapeworks module with associated libraries and conda install scripts (meta.yaml, build.sh, bld.bat). 

DEVELOPERS: Follow these steps to use this today:

1. Import the python path as `export PYTHONPATH="<path/to/Shapeworks/Python/shapeworks>:$PYTHONPATH`
2. Navigate to the python shapeworks directory `cd <path/to/Shapeworks/Python/shapeworks>`
3. Create a symbolic link in this directory as, `ln -s <path/to/your/build/bin/shapeworks.cpython[hit-tab] ./shapeworks`.
4. Test the following in python, and you are good to go.

```
>>> import shapeworks

Python path:
 > info

System path:
 > info 

>>> shapeworks.utils.postive_factors(10)
[1, 2, 5, 10]
```

TODO: deploy shapeworks with all the binaries inside the module (it's okay if other stuff like Studio is in there, too)

``` python
import shapeworks as sw
sw.paaaaaaaaaarrrrtaaayyyy(365, 7, 24)
```
