# Welcome to ShapeWorks!

<img src="img/home/shapeworks-logo.png" width="60px" align="left" hspace="20">    
*An Integrated Suite for Shape Representation and Analysis & more...*

## What is ShapeWorks?

<img src="img/home/about-shapeworks.png" width="1000px" align="right" hspace="20">

ShapeWorks is a free, open-source suite of software tools that uses a flexible method for automated construction of compact statistical landmark-based shape models of ensembles of anatomical shapes that do not rely on any specific surface parameterization. The method requires very little preprocessing or parameter tuning and applies to a wide range of shape analysis problems, including nonmanifold surfaces and arbitrary topology objects. ShapeWorks includes tools for preprocessing data, computing landmark-based shape models, and visualizing the results.


## Why use ShapeWorks?

ShapeWorks is the only publicly available tool that learns a population-specific anatomical mapping in a way that respects population variability without relying on a user-defined template/atlas. 
    
It produces more detailed surface-to-surface correspondences than traditional landmark-based approaches. 

Its optimized approach offers increased power for testing biological hypotheses of shape and shape differences, captures subtle shape variations, and decreases observer bias for reproducible scientific results. 
    

*ShapeWorks can discover the underlying mode of variation in the box-bump ensemble in comparison to existing publicly available shape modeling software packages*
![](img/home/shapeworks_vs_others4.png)


## ShapeWorks Get-Togethers

!!! note "Where to start? What are the latest features? How to customize existing workflows?"
    A monthly virtual get-together meeting for ShapeWorks users is organized to demonstrate new features, discuss the customization of the shape modeling workflow to users' own use cases, and get suggestions to improve the software and documentation. **If you would like to be added to the invitation list, please send an email to `shireen-at-sci-dot-utah-dot-edu`.**


## ShapeWorks in Action!


The underlying scientific premise of ShapeWorks, particle-based shape modeling, is a groupwise approach to placing landmarks (i.e., correspondences) that consider variability in the entire cohort of images. 

<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/studio_ellipsoid.mp4" autoplay muted loop controls style="width:100%"></p>

ShapeWorks uses a set of interacting particle systems, one for each shape, to produce optimal sets of surface correspondences in an ensemble. Particles interact with one another via mutually repelling forces to cover and, therefore, describe surface geometry optimally. Particles are positioned on surfaces automatically by optimizing the model's information content via an entropy optimization scheme. 

In particular, ShapeWorks explicitly models the inherent trade-off between the model's statistical simplicity (i.e., compactness) in the shape space and the accuracy of the shape representations (i.e., good surface samplings) in the configuration space. 


ShapeWorks has been effective in various applications, including psychology, biological phenotyping, cardiology, and orthopedics. [See relevant papers](users/papers.md) and [ShapeWorks Success Stories](getting-started/sw-stories.md). 






