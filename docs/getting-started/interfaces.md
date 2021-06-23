# ShapeWorks Interfaces


ShapeWorks tools are designed to support different usage scenarios, including execution on a local computing platform through the terminal ([command line](#shapeworks-commands)) and the user-friendly [Studio](#shapeworksstudio) application, APIs including [Python](#shapeworks-in-python), and remote systems such as private or public [clouds](#shapeworks-in-the-cloud). 


## ShapeWorks Command


ShapeWorks consists of a set of independent command line tools for preprocessing binary segmentations and surface meshes ([Groom](../workflow/groom.md)) and computing landmark-based shape models ([Optimize](../workflow/optimize.md)). It also includes an interactive user interface called [ShapeWorks Studio](#shapeworksstudio) to analyze and visualize the optimized shape models ([Analyze](../workflow/analyze.md)). 

We are consolidating these tools into a [single, reusable API](../new/shapeworks-command.md) that is shared across different computational libraries in ShapeWorks, and a standalone `shapeworks` command. Visit [ShapeWorks Command](../new/shapeworks-command.md) for an illustrative example.

The `shapeworks` executable is highly flexible, modular, and loosely coupled, with standardized subcommands and interactive help to perform individual operations needed for a typical shape modeling workflow that includes the Groom, Optimize, and Analyze phases. Please see [shapeworks commands documentation](../tools/ShapeWorksCommands.md) for the list of available commands.


!!! danger "Activate shapeworks environment"
    Each time you use [ShapeWorks from the command line](../tools/ShapeWorksCommands.md), you must first activate its environment using the `conda activate shapeworks` command on the terminal.
    
## ShapeWorks in Python

We are developing Python bindings to enable users to script their customized shape modeling workflows. See [ShapeWorks in Python](../new/shapeworks-python.md) for more details and pointers to step-by-step, hands-on tutorials on different aspects of the shape modeling workflow in a transparent, reproducible, and sharable manner.

!!! important "ShapeWorks with Python support"
    ShapeWorks Python library is currently under active development and is part of our major releases as of **ShapeWorks 6**. 

!!! note "About use cases"
    Exemplar [use cases](../use-cases/use-cases.md) will be updated to use these Python APIs to showcase their use and flexibility.


## ShapeWorks Studio


ShapeWorks Studio is a cross-platform graphical user interface (GUI) to support the standard shape analysis workflow needed by most ShapeWorks users, enabling a wide variety of research scenarios. It is currently supporting image-based grooming and is actively under development to support surface meshes, more sophisticated grooming operations (including user annotations), and offline processing.  ShapeWorks Studio enables real-time parameter tuning and visualization of the optimization process and statistical analyses.


<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/studio_demo.mp4" autoplay muted loop controls style="width:100%"></p>

For more information about ShapeWorks Studio, see [here](../studio/getting-started-with-studio.md)

## ShapeWorks in the Cloud 

*Work in progress ... Stay tuned!*

![In Progress](../img/misc/in-progress.png)
