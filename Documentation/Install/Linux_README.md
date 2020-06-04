
Installing ShapeWorks on Linux
=====================

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest) or up-to-date development builds from the master branch for [Linux](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-linux). *Please understand that these are in-progress development builds, not official releases.*


ShapeWorks comes with [python examples](../UseCases/UseCases.md) to get you started. To run them you will
need python3 with a few packages.  The easist way to **install** them is to run:

```
$ source ./conda_installs.sh
```

Then, to run the example:

```
$ cd Examples/Python
$ python3 RunUseCase.py --use_case [insert name of use case here]
```

For names for the use cases that are currently release, run:

```
$ python3 RunUseCase.py --help
```

More information about running use cases can be found [here](../UseCases/UseCases.md#running-the-use-cases).


For subsequent usage, the shapeworks conda environment must be activated using:

```
$ conda activate shapeworks
```