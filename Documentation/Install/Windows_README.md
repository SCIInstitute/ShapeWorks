
Installing ShapeWorks on Windows
=====================

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest) or up-to-date development builds from the master branch for [Windows](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-windows). *Please understand that these are in-progress development builds, not official releases.*

To install:

- Download and install the "Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017 and 2019." :

```
https://aka.ms/vs/16/release/vc_redist.x64.exe
```

- Download and install Anaconda for Windows:

```
https://repo.anaconda.com/archive/Anaconda3-2019.10-Windows-x86_64.exe
```

- Copy "C:\Program Files\ShapeWorks\Examples" to another location

- Open "Anaconda Prompt"

- CD to "C:\Program Files\ShapeWorks"

- Run:

```
$ conda_installs.bat
```

- CD to your copied Examples\Python location

- To run an example:

```
$ cd Examples/Python
$ python RunUseCase.py --use_case [insert name of use case here]
```

- For names for the use cases that are currently release, run:

```
$ python RunUseCase.py --help
```

- More information about running use cases can be found [here](../UseCases/UseCases.md#running-the-use-cases).


Running conda_installs.bat will activate the conda shapeworks environment
into your current anaconda prompt. **For subsequent usage, the shapeworks conda environment must be activated using:**

```
$conda activate shapeworks
```

**Note:** If you have installed ShapeWorks in a different location than
"C:\Program Files\ShapeWorks", please add the path to the "bin"
directory as the final argument to RunUseCase.py so that the path will
be set correctly.
