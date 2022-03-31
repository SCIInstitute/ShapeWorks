![](https://github.com/SCIInstitute/ShapeWorks/workflows/Mac%20Build/badge.svg)
![](https://github.com/SCIInstitute/ShapeWorks/workflows/Linux%20Build/badge.svg)
![](https://github.com/SCIInstitute/ShapeWorks/workflows/Windows%20Build/badge.svg)

<!-- update docs -->
This branch exists in order to update the 6.2.1 docs if necessary.
Instructions:
```
git fetch origin gh-pages
pip install mkdocs==1.1.2
pip install mkdocs-material==8.2.8
pip install mike==1.1.2
pip install jinja2==3.0.3
# build shapeworks with -DBUILD_DOCUMENTATION=ON
# from shapeworks root:
./build_docs.sh <install_or_build_dir>
mike deploy --config-file ./mkdocs.yml --title "6.2.1" 6.2 --branch gh-pages --update-aliases
# push updates:
git push origin gh-pages
```
Pushing source to github should simply update 6.2, not overwrite what is currently there.

<!--ShapeWorks
=====================-->
![](docs/img/home/sw-logo-w-text.png)


<!--<img src="docs/img/home/shapeworks-logo.png" width="300px" align="left" hspace="20">-->

ShapeWorks is an open-source software that learns population-specific shape parameterization directly from user data. It enables the automated placement of dense corresponding landmarks on a group of shapes given as either binary volumes or surface meshes. Please visit [ShapeWorks website](http://sciinstitute.github.io/ShapeWorks/) for more information. ShapeWorks has been effective in a range of applications, including psychology, biological phenotyping, cardiology, and orthopedics. [See relevant papers](http://sciinstitute.github.io/ShapeWorks/users/papers). 


<img src="docs/img/home/about-shapeworks.png" width="1000px" align="right" hspace="20">

Latest Release
=====================

<!--![](docs/img/about/release5.5.png)-->
<!--![](docs/img/about/release6.0.png)-->
<!--![](docs/img/about/release6.1.png)-->
![](docs/img/about/release6.2.png)

Quick Links
=====================
- [How to install ShapeWorks?](http://sciinstitute.github.io/ShapeWorks/users/install)
- [How to cite ShapeWorks?](http://sciinstitute.github.io/ShapeWorks/users/citation)
- [How to get started with shape modeling?](http://sciinstitute.github.io/ShapeWorks/getting-started/shapes)
- See [ShapeWorks in action](http://sciinstitute.github.io/ShapeWorks/getting-started/interfaces)!
- How to run and see ShapeWorks in action on exemplar [use cases](http://sciinstitute.github.io/ShapeWorks/use-cases/use-cases). 
- [How to contact us?](http://sciinstitute.github.io/ShapeWorks/about/contact)
- [Meet ShapeWorkers!](http://sciinstitute.github.io/ShapeWorks/about/team)
- [...and more](http://sciinstitute.github.io/ShapeWorks/getting-started/how-tos)


Using
=====================
[ShapeWorks Commands](http://sciinstitute.github.io/ShapeWorks/tools/ShapeWorksCommands)  
[ShapeWorks Studio](http://sciinstitute.github.io/ShapeWorks/getting-started/interfaces#shapeworksstudio)  
[Python API](http://sciinstitute.github.io/ShapeWorks/getting-started/interfaces#shapeworks-in-python) _(New!)_  
[Clouds Deployments](http://sciinstitute.github.io/ShapeWorks/getting-started/interfaces#shapeworks-in-the-cloud) _(Coming Soon!)_  




