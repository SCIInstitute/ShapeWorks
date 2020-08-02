![](https://github.com/SCIInstitute/ShapeWorks/workflows/Mac%20Build/badge.svg)
![](https://github.com/SCIInstitute/ShapeWorks/workflows/Linux%20Build/badge.svg)
![](https://github.com/SCIInstitute/ShapeWorks/workflows/Windows%20Build/badge.svg)

ShapeWorks
=====================

<img src="Images/shapeworks-logo.png" width="300px" align="left" hspace="20">

<p align="justify">
ShapeWorks is an open-source software that learns population-specific shape parameterization directly from user data. It enables the automated placement of dense corresponding landmarks on a group of shapes given as either binary volumes or surface meshes. Please visit [ShapeWorks website](http://sciinstitute.github.io/ShapeWorks/) for more information. ShapeWorks has been effective in a range of applications, including psychology, biological phenotyping, cardiology, and orthopedics. [See relevant papers](http://sciinstitute.github.io/ShapeWorks/users/papers). 
</p>


<img src="Images/about-shapeworks.png" width="1000px" align="right" hspace="20">

   
    


Table of Contents
====================
- [Contact Us](#contact-us)  
- [Installation](#installation)  
- [Getting Started](#getting-started)  
- [Acknowledgements](#acknowledgements)
- [Contributors](#contributors)
- [Relevant Papers](#relevant-papers)


Contact Us
=====================

Users Forum
----------
Our user forum is located on [Gitter ShapeWorks User Forum](https://gitter.im/ShapeWorks). 

You can access the ShapeWorks Gitter using the [![Gitter](https://badges.gitter.im/ShapeWorks/ShapeWorks.svg)](https://gitter.im/ShapeWorks/ShapeWorks?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge) link above or download the [Gitter app](https://gitter.im/apps) for any platform or your phone. It provides easy integration with [GitHub](https://github.com) and has no limitation on the number of messages as is the case with other services. 


Users Mailing List
------------
Please join our mailing list by sending a message to sympa@sci.utah.edu with the subject `subscribe shapeworks-users` and empty body.
You can also email any questions, bugs, or feature requests to shapeworks-users@sci.utah.edu.

Developers Mailing List
------------
You can join our developer support mailing list by sending a message to sympa@sci.utah.edu with the subject `subscribe shapeworks-dev-support` and empty body.

As a developer, if you encounter any problems or bugs, please report them using [the issue tracker on GitHub](https://github.com/SCIInstitute/ShapeWorks/issues). This includes feature requests. Feel free to add improvements using git pull requests. You can also email ShapeWorkers at shapeworks-dev-support@sci.utah.edu.


Installation
=====================


ShapeWorks provides official user releases, the features of which can be seen at [ChangeLog](Documentation/ChangeLog.md).

First, download the latest ShapeWorks binary release:  
https://github.com/SCIInstitute/ShapeWorks/releases/latest

We also provide up-to-date development builds from the master branch for Windows, Mac, and Linux:

https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-windows

https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-mac

https://github.com/SCIInstitute/ShapeWorks/releases/tag/dev-linux

Please understand that these are in-progress development builds, not official releases.

Then follow the appropriate instructions for their platform:  
[Windows](https://raw.githubusercontent.com/SCIInstitute/ShapeWorks/master/Documentation/Install/Windows_README.md)  
[Mac](https://raw.githubusercontent.com/SCIInstitute/ShapeWorks/master/Documentation/Install/Mac_README.md)  
[Linux](https://raw.githubusercontent.com/SCIInstitute/ShapeWorks/master/Documentation/Install/Linux_README.md)  

_**Important**_  
Each time you use ShapeWorks from the command line you must first activate its environment:
```
$ conda activate shapeworks
```

Documentation
=====================

Getting Started
---------------------
[GettingStarted](Documentation/GettingStarted.md) provides overviews of dataset grooming, optimization, and analysis, ShapeWorks Studio and command line instructions, and practical use cases. See [ShapeWorks in action](Documentation/GettingStarted.md#shapeworks-interfaces)!

Using
---------------------
[ShapeWorks Commands](Documentation/GettingStarted.md#shapeworks-command)  
[ShapeWorks Studio](Documentation/GettingStarted.md#shapeworksstudio)  
[Python API](Documentation/GettingStarted.md#shapeworks-in-python) _(Coming Soon!)_  
[Clouds Deployments](Documentation/GettingStarted.md#shapeworks-in-the-cloud) _(Coming Soon!)_  

How Tos
---------------------
- How to run and see ShapeWorks in action on exemplar [use cases](Documentation/UseCases/UseCases.md). 
- How to preprocess or [groom](Documentation/Workflow/Groom.md) your dataset.
- How to [optimize](Documentation/Workflow/Optimize.md) your shape model.
- How to [visualize and analyze](Documentation/Workflow/Analyze.md) your optimized shape model.
- [...and more](Documentation/GettingStarted.md#how-tos)


Acknowledgements
=====================

If you use ShapeWorks in work that leads to published research, we humbly ask that you to [cite ShapeWorks](#how-to-cite-shapeworks), add the following to the 'Acknowledgments' section of your paper: 
<!--
"This project was supported by the 
National Institute of General Medical Sciences of the National Institutes of Health under 
grant number P41GM103545."
-->
>"This work was supported by the National Institutes of Health under grant numbers NIBIB-U24EB029011, NIAMS-R01AR076120, NHLBI-R01HL135568, NIBIB-R01EB016701, and NIGMS-P41GM103545."

and add the following 'disclaimer':

>"The content is solely the responsibility of the authors and does not necessarily represent the official views of the National Institutes of Health."


How to Cite ShapeWorks
=====================

When referencing ShapeWorks, please include a bibliographical reference to the paper below, and, if possible, include a link to [shapeworks.sci.utah.edu](http://www.sci.utah.edu/software/shapeworks.html/). 

---
<p align="justify">
Joshua Cates, Shireen Elhabian, Ross Whitaker. "Shapeworks: particle-based shape correspondence and visualization software." Statistical Shape and Deformation Analysis. Academic Press, 2017. 257-298.
</p>

>@incollection{cates2017shapeworks,
title={Shapeworks: particle-based shape correspondence and visualization software},
  author={Cates, Joshua and Elhabian, Shireen and Whitaker, Ross},
  booktitle={Statistical Shape and Deformation Analysis},
  pages={257--298},
  year={2017},
  publisher={Elsevier}
}

---

Contributors
=====================

Principal Investigators
---------------------

- [Shireen Elhabian](http://www.sci.utah.edu/~shireen/)
- [Ross Whitaker](http://www.cs.utah.edu/~whitaker/)

Software Developers
---------------------
- [Alan Morris](https://www.sci.utah.edu/people/amorris.html)
- [Cameron Christensen](https://www.sci.utah.edu/people/cam.html)
- [Archanasri Subramanian](https://www.sci.utah.edu/people/archanasri.html)

Researchers
---------------------
- [Riddhish Bhalodia](https://www.sci.utah.edu/people/riddhishb.html)
- [Atefeh Ghanaatikashani](https://www.sci.utah.edu/people/atefeh.gk.html)
- [Jadie Adams](https://www.sci.utah.edu/people/jadie.html)
- [Hong Xu](http://www.sci.utah.edu/people/hxu.html)
- [Oleks Korshak](http://www.sci.utah.edu/people/oleks.html)
- [Karthik Karanth](http://www.sci.utah.edu/people/karthik.html)

Past Contributors
---------------------
- [Joshua Cates](http://www.sci.utah.edu/~cates/)
- [Manasi Datar](https://manasidatar.github.io/index.html)
- [Brig Bagley](http://www.cs.utah.edu/~bbagley/)
- [Praful Agrawal](https://www.sci.utah.edu/people/prafulag.html)



