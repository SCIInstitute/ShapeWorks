
Installing ShapeWorks on Mac
=====================

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest) or up-to-date development builds from the master branch for [Mac](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-mac). *Please understand that these are in-progress development builds, not official releases.*


To use the installer version of ShapeWorks (.pkg):

- Install the ShapeWorks pkg file.

- Open a terminal, change directory to /Applications/ShapeWorks

- Type:

```
$ source conda_installs.sh
```

- Copy "/Applications/ShapeWorks/Examples to another location (e.g., $HOME/ShapeWorks-Examples)

- CD to your copied Examples/Python location

- To run an example:

```
$ cd Examples/Python
$ python3 RunUseCase.py --use_case [insert name of use case here]
```

- For names for the use cases that are currently release, run:

```
$ python3 RunUseCase.py --help
```

- More information about running use cases can be found [here](../UseCases/UseCases.md#running-the-use-cases).



**For subsequent usage, the shapeworks conda environment must be activated using:**

```
$ conda activate shapeworks
```