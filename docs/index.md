# Welcome to ShapeWorks!

<img src="img/shapeworks-logo.png" width="60px" align="left" hspace="20">    
*An Integrated Suite for Shape Representation and Analysis & more...*

## What is ShapeWorks?

<img src="img/about-shapeworks.png" width="1000px" align="right" hspace="20">

<p align="justify">
  ShapeWorks is a free, open-source suite of software tools that uses a flexible method for automated construction of compact statistical landmark-based shape models of ensembles of anatomical shapes that does not rely on any specific surface parameterization. The method requires very little preprocessing or parameter tuning, and is applicable to a wide range of shape analysis problems, including nonmanifold surfaces and objects of arbitrary topology. ShapeWorks includes tools for preprocessing data, computing landmark-based shape models, and visualizing the results.
</p>

## With ShapeWorks You Can!



## ShapeWorks in Action!

<p align="justify">
The underlying scientific premise of ShapeWorks, particle-based shape modeling, is a groupwise approach to placing landmarks (i.e., correspondences) that considers the variability in the entire cohort of images. ShapeWorks uses a set of interacting particle systems, one for each shape, in an ensemble, to produce optimal sets of surface correspondences. Particles interact with one another via mutually repelling forces to optimally cover, and therefore describe, the surface geometry. Particles are positioned on surfaces automatically by optimizing the information content of the model via an entropy optimization scheme. In particular, ShapeWorks explicitly models the inherent trade-off between the statistical simplicity of the model (i.e., compactness) in the shape space and the accuracy of the shape representations (i.e., good surface samplings) in the configuration space. 
</p>

<p align="justify">
ShapeWorks has been effective in a range of applications, including psychology, biological phenotyping, cardiology, and orthopedics. [See relevant papers](#relevant-papers). 
</p>

**TODO: use case with relevant paper, see the U24 proposal**

## How to Cite ShapeWorks?

### Acknowledgements

If you use ShapeWorks in work that leads to published research, we humbly ask that you to [cite ShapeWorks](#citation), add the following to the 'Acknowledgments' section of your paper: 
<!--
"This project was supported by the 
National Institute of General Medical Sciences of the National Institutes of Health under 
grant number P41GM103545."
-->
>"This work was supported by the National Institutes of Health under grant numbers NIBIB-U24EB029011, NIAMS-R01AR076120, NHLBI-R01HL135568, NIBIB-R01EB016701, and NIGMS-P41GM103545."

and add the following 'disclaimer':

>"The content is solely the responsibility of the authors and does not necessarily represent the official views of the National Institutes of Health."

### Citation

When referencing ShapeWorks, please include a bibliographical reference to the paper below, and, if possible, include a link to [shapeworks.sci.utah.edu](http://www.sci.utah.edu/software/shapeworks.html/). 

---

<p align="justify">
Joshua Cates, Shireen Elhabian, Ross Whitaker. "Shapeworks: particle-based shape correspondence and visualization software." Statistical Shape and Deformation Analysis. Academic Press, 2017. 257-298.
</p>

>@incollection{cates2017shapeworks,   
title = {Shapeworks: particle-based shape correspondence and visualization software},   
  author = {Cates, Joshua and Elhabian, Shireen and Whitaker, Ross},   
  booktitle = {Statistical Shape and Deformation Analysis},   
  pages = {257--298},   
  year = {2017},   
  publisher = {Elsevier}  
}
---


## Relevant Papers

**TODO: add links to the papers and update the list of papers with recent ones**

**If you would like to have your publication listed here: e-mail us!**

<p align="justify">
The following selected papers give background on the methods used in ShapeWorks and a sample of the areas ShapeWorks has been applied:
</p>

<p align="justify">
M. Jacxsens, S. Elhabian, S. Brady, P. Chalmers, R. Tashjian, and H. Henninger. "Coracoacromial morphology: a contributor to recurrent traumatic anterior glenohumeral instability," Journal of Shoulder and Elbow Surgery, 1:1316-1325, 2019.
</p>

<p align="justify">
P. R. Atkins, Y. Shin, P. Agrawal, S. Y. Elhabian, R. T. Whitaker, J. A. Weiss, S. K. Aoki, C. L. Peters ,and A. E.  Anderson. "Which Two-dimensional Radiographic Measurements of Cam Femoroacetabular Impingement Best Describe the Three-dimensional Shape of the Proximal Femur?," Clinical Orthopaedics and Related Research, 477(1):242–253, 2019.
</p>

<p align="justify">
T. Sodergren, R. Bhalodia, R. Whitaker, J. Cates, N. Marrouche, and S. Elhabian. "Mixture modeling of global shape priors and autoencoding local intensity priors for left atrium segmentation," In STACOM- MICCAI: Statistical Atlases and Computational Modeling of the Heart workshop, page in press. Springer, 2018.
</p>

<p align="justify">
A. Goparaju, I. Csecs, A. Morris, E. Kholmovski, N. Marrouche, R. Whitaker, and S. Elhabian. "On the Evaluation and Validation of Off-the-Shelf Statistical Shape Modeling Tools: A Clinical Application," In International Workshop on Shape in Medical Imaging, pages 14–27. Springer, 2018.
</p>

<p align="justify">
E. T. Bieging, A. Morris, B. D. Wilson, C. J. McGann, N. F. Marrouche, and J. Cates. "Left atrial shape predicts recurrence after atrial fibrillation catheter ablation," Journal of Cardiovascular Electrophysiology, 2018.
</p>

<p align="justify">
P. R. Atkins, S. Y. Elhabian, P. Agrawal, M. D. Harris, R. T. Whitaker, J. A. Weiss, C. L. Peters, and A. E. Anderson. "Quantitative comparison of cortical bone thickness using correspondence-based shape modeling in patients with cam femoroacetabular impingement," Journal of Orthopaedic Research, 35(8):1743–1753, 2017.
</p>

<p align="justify">
J. Cates, L. Nevell, S. I. Prajapati, L. D. Nelon, J. Y. Chang, M. E. Randolph, B. Wood, C. Keller, and R. T. Whitaker. "Shape analysis of the basioccipital bone in Pax7-deficient mice," Scientific Reports, 7(1):17955, 2017.
</p>

<p align="justify">
J. Cates, S. Elhabian, and R. Whitaker. "ShapeWorks: particle-based shape correspondence and visualization software," In G. Zheng, S. Li, and G. Szekely, editors, Statistical Shape and Deformation Analysis: Methods, Implementation and Applications, 1st Edition, chapter 10. Academic Press, 2017.
</p>

<p align="justify">
P. Agrawal, S.Y. Elhabian, R.T. Whitaker, "Learning Deep Features for Automated Placement of Correspondence Points on Ensembles of Complex Shapes," In International Conference on Medical Image Computing and Computer-Assisted Intervention, pp. 185-193. Springer, Cham, 2017.
</p>

<p align="justify">
S. Sultana, P. Agrawal, S. Elhabian, R. Whitaker, T. Rashid, J. Blatt, J. Cetas, and M. Audette. "Towards a statistical shape-aware deformable contour model for cranial nerve identification," In Workshop on Clinical Image-Based Procedures, pages 68–76. Springer, 2016.
</p>

<p align="justify">
J. Cates, E. Bieging, A. Morris, G. Gardner, N. Akoum, E. Kholmovski, N. Marrouche, C. McGann, and R. S. MacLeod. "Computational shape models characterize shape change of the left atrium in atrial fibrillation," Clinical Medicine Insights. Cardiology, 8(Suppl 1):99, 2015.
</p>

<p align="justify">
Ken Museth. "VDB: High-resolution sparse volumes with dynamic topology" ACM Transactions on Graphics, July 2013, Article No.: 27
</p>

<p align="justify">
M. Datar, I. Lyu, S. Kim, J. Cates, M. Styner, R. Whitaker. "Geodesic distances to landmarks for dense correspondence on ensembles of complex shapes," In International Conference on Medical Image Computing and Computer-Assisted Intervention, Springer, Berlin, Heidelberg, pp. 19-26, 2013.
</p>

<p align="justify">
M.D. Harris, M. Datar, R.T. Whitaker, E.R. Jurrus, C.L. Peters, A.E. Anderson. 
"Statistical Shape Modeling of Cam Femoroacetabular Impingement," In Journal of Orthopaedic Research, 
Vol. 31, No. 10, pp. 1620--1626. 2013.
</p>

<p align="justify">
K.B. Jones, M. Datar, S. Ravichandran, H. Jin, E. Jurrus, R.T. Whitaker, M.R. Capecchi. 
"Toward an Understanding of the Short Bone Phenotype Associated with Multiple Osteochondromas," 
In Journal of Orthopaedic Research, Vol. 31, No. 4, pp. 651--657. 2013.
</p>

<p align="justify">
J. Cates, P.T. Fletcher, Z. Warnock, R.T. Whitaker. 
"A Shape Analysis Framework for Small Animal Phenotyping with Application to Mice with 
a Targeted Disruption of Hoxd11," In Proceedings of the 5th IEEE International Symposium 
on Biomedical Imaging (ISBI '08), pp. 512--516. 2008. DOI: 10.1109/ISBI.2008.4541045
</p>

<p align="justify">
J. Cates, P.T. Fletcher, M. Styner, H. Hazlett, R.T. Whitaker. 
"Particle-Based Shape Analysis of Multi-Object Complexes," 
In Proceedings of the 11th International Conference on Medical Image Computing and 
Computer Assisted Intervention (MICCAI '08), Lecture Notes In Computer Science (LCNS), 
pp. 477--485. 2008. ISBN: 978-3-540-85987-1
</p>

<p align="justify">
J. Cates, P. T. Fletcher, M. Styner, M. Shenton, and R. Whitaker. "Shape modeling and analysis with entropy-based particle systems," In Information Processing in Medical Imaging, pages 333–345. Springer, 2007.
</p>

