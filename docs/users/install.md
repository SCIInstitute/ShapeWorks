# How to Install ShapeWorks?

ShapeWorks 6.8 ships with a complete Python environment built in. There is no
conda environment to set up — install the package and you can run Studio, the
`shapeworks` CLI, and Python use cases right away.

## _**First, download ShapeWorks**_

!!! important "Release Builds"
    We provide [official user releases](https://github.com/SCIInstitute/ShapeWorks/releases/latest) for Windows, MacOS, and Linux.

    Their features can be seen at [Release Notes](../about/release-notes.md).

!!! danger "Development Builds"
    We also provide up-to-date development builds from the **master** branch
    [Windows Dev Build](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-windows)
    [Mac Dev Build](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-mac)
    [Linux Dev Build](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-linux)

    Please understand that these are in-progress development builds, not official releases.

## _**Next, follow the instructions for your platform.**_
[Windows](#installing-shapeworks-on-windows)
[Mac](#installing-shapeworks-on-mac)
[Linux](#installing-shapeworks-on-linux)


## Installing ShapeWorks on Windows

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest), or up-to-date development builds from the master branch for [Windows](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-windows) *(remember these are in-progress development builds, not official releases).*

!!! warning "Windows 11: Smart App Control affects Python features"
    On Windows 11, a security feature called **Smart App Control** will block
    the Python libraries that ShapeWorks ships. **ShapeWorks Studio itself
    will still run**, and you can use its grooming, optimization, and analysis
    features normally. However, the following will not work until Smart App
    Control is turned off:

    - Python use cases (`swpython RunUseCase.py ...`)
    - Jupyter notebook examples
    - DeepSSM and other Python-based features inside Studio

    If affected, you will see errors like:

    *"ImportError: DLL load failed while importing vtkWebCore: An Application Control policy has blocked this file."*

    This happens because ShapeWorks bundles common scientific Python libraries
    (VTK, PyTorch, etc.) that are not digitally signed by Microsoft. Smart App
    Control blocks unsigned files, even though they are safe. Most other
    scientific Python tools (PyTorch, SciPy, OpenCV, ...) have the same issue.

    **To turn Smart App Control off:**

    1. Click the **Start** button and type **Smart App Control**.
    2. Click **Smart App Control settings**.
    3. Select **Off**.

    **Important:** Turning Smart App Control off is a one-way change. The only
    way to turn it back on later is to reset Windows.

1. Double-click the ShapeWorks installer that you downloaded and follow the steps.

2. The installer creates a **ShapeWorks Prompt** shortcut in the Start Menu. Open
it whenever you want to run ShapeWorks from the command line — it opens a
terminal with `shapeworks`, `swpython`, and `swpip` already on `PATH`.


### ShapeWorks comes with examples to get you started.

1. Copy the Examples folder to another location of your choosing (you can also use Explorer).
`xcopy /E/H "C:\Program Files\ShapeWorks\Examples" %HOMEPATH%\ShapeWorks-Examples\`

#### Studio

1. Open ShapeWorks Studio from the Start Menu.

2. Select "Open Existing Project".

3. Select `Examples/Studio/Ellipsoid/ellipsoid.xlsx`.

4. Experiment with the Studio interface.

#### Python

1. Open the **ShapeWorks Prompt** from the Start Menu.

2. Change to the Python folder of the Examples directory you copied.
`cd %HOMEPATH%\ShapeWorks-Examples\Python`

3. Run one of the included use cases. To list them all, run: `swpython RunUseCase.py --help`.
`swpython RunUseCase.py ellipsoid`

The first time you run a DeepSSM use case, ShapeWorks downloads and installs
PyTorch into a per-user directory (~3 GB, one-time). Subsequent runs use the
cached install.

#### ShapeWorks also includes interactive Jupyter Python notebook examples.

1. Open the **ShapeWorks Prompt** from the Start Menu.

2. Install Jupyter the first time you use it: `swpip install jupyter`.
This installs to your per-user ShapeWorks site-packages and persists across sessions.

3. Change to the notebook tutorials folder of the Examples directory you copied.
`cd %HOMEPATH%\ShapeWorks-Examples\Python\notebooks\tutorials`

4. Start the Jupyter notebook server. This will open a new tab in your web browser.
`swpython -m jupyter notebook`

5. Click on a notebook to get started.

## Installing ShapeWorks on Mac

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest), or up-to-date development builds from the master branch for [Mac](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-mac) *(remember these are in-progress development builds, not official releases).*

1. Install the ShapeWorks `.pkg` file by double-clicking it. It installs into
`/Applications/ShapeWorks/`.

2. (Optional, for convenience) Put the ShapeWorks command-line tools on your
`PATH`. Add this to your `~/.zshrc` or `~/.bash_profile`:
`export PATH="/Applications/ShapeWorks/bin:$PATH"`

Without this step you can still run the tools using their full paths, e.g.
`/Applications/ShapeWorks/bin/swpython RunUseCase.py ...`.

### ShapeWorks comes with examples to get you started.

#### Studio

1. Open `/Applications/ShapeWorks/ShapeWorksStudio.app` (double-click in Finder,
or `open /Applications/ShapeWorks/ShapeWorksStudio.app` from a terminal).

2. Select "Open Existing Project".

3. Select `Examples/Studio/Ellipsoid/ellipsoid.xlsx`.

4. Experiment with the Studio interface.


#### Python

1. Copy the Examples folder to a writable location.
`cp -r /Applications/ShapeWorks/Examples $HOME/ShapeWorks-Examples`

2. Change to the Python folder of the Examples directory you copied.
`cd $HOME/ShapeWorks-Examples/Python`

3. Run one of the included use cases. To list them all, run: `swpython RunUseCase.py --help`.
`swpython RunUseCase.py <insert name of use case here>`

If you didn't add `/Applications/ShapeWorks/bin` to your `PATH`, use the full
path: `/Applications/ShapeWorks/bin/swpython RunUseCase.py ...`.

The first time you run a DeepSSM use case, ShapeWorks downloads and installs
PyTorch into `~/Library/Application Support/ShapeWorks/<version>/site-packages/`
(~3 GB, one-time). Subsequent runs use the cached install.

- More information about running use cases can be found [here](../use-cases/use-cases.md#running-the-use-cases).


#### ShapeWorks also includes interactive Jupyter Python notebook examples.

1. Install Jupyter the first time you use it: `swpip install jupyter`.
This installs to your per-user ShapeWorks site-packages and persists across sessions.

2. Change to the notebook tutorials folder of the Examples directory you copied.
`cd $HOME/ShapeWorks-Examples/Python/notebooks/tutorials`

3. Start the Jupyter notebook server. This will open a new tab in your web browser.
`swpython -m jupyter notebook`

4. Click on a notebook to get started.



## Installing ShapeWorks on Linux

Please make sure that you download the [latest ShapeWorks binary release](https://github.com/SCIInstitute/ShapeWorks/releases/latest), or up-to-date development builds from the master branch for [Linux](https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-linux) *(remember these are in-progress development builds, not official releases).*

1. Extract the downloaded archive (e.g. `tar xzf ShapeWorks-*.tar.gz`).

2. (Optional, for convenience) Put the ShapeWorks command-line tools on your
`PATH`. Add this to your `~/.bashrc`:
`export PATH="/path/to/ShapeWorks-<version>-linux/bin:$PATH"`

Without this step you can still run the tools using their full paths.

### ShapeWorks comes with examples to get you started.

#### Studio

1. Run `./bin/ShapeWorksStudio` from the extracted directory.

2. Select "Open Existing Project".

3. Select `Examples/Studio/Ellipsoid/ellipsoid.xlsx`.

4. Experiment with the Studio interface.

#### Python

1. Copy the Examples folder to a writable location.
`cp -r /path/to/ShapeWorks-<version>-linux/Examples $HOME/ShapeWorks-Examples`

2. Change to the Python folder of the Examples directory you copied.
`cd $HOME/ShapeWorks-Examples/Python`

3. Run one of the included use cases. To list them all, run: `swpython RunUseCase.py --help`.
`swpython RunUseCase.py <insert name of use case here>`

The first time you run a DeepSSM use case, ShapeWorks downloads and installs
PyTorch into `~/.local/share/ShapeWorks/<version>/site-packages/`
(~3 GB, one-time). Subsequent runs use the cached install.

- More information about running use cases can be found [here](../use-cases/use-cases.md#running-the-use-cases).


#### ShapeWorks also includes interactive Jupyter Python notebook examples.

1. Install Jupyter the first time you use it: `swpip install jupyter`.
This installs to your per-user ShapeWorks site-packages and persists across sessions.

2. Change to the notebook tutorials folder of the Examples directory you copied.
`cd $HOME/ShapeWorks-Examples/Python/notebooks/tutorials`

3. Start the Jupyter notebook server. This will open a new tab in your web browser.
`swpython -m jupyter notebook`

4. Click on a notebook to get started.

---

## Building from source

Developers who want to build ShapeWorks from source — and the conda-based
workflow that comes with it — should see the [build documentation](../dev/build.md).
