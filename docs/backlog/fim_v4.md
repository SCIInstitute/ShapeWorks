# FIM_V4 Library

###Overview
_TODO_

THIS WORK IS BASED ON FIM LIBRARY FROM FIDS AND GEODESICS REPOS JANUARY 2016 (fim_v4_10_17_2014)

--
###License
fim: Fast Eikonal Solver for Triangular Meshes
Zhisong Fu zhisong@cs.utah.edu

The MIT License

Copyright (c) 2009 Scientific Computing and Imaging Institute,
University of Utah.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.


## Building
To build and link with ShapeWorks:

* Configure with CMake
* Make sure to specify the CMAKE_INSTALL_PREFIX variable if you don't want it to install in a standard system location.
* Run "make install"

* Select "BUILD_Mesh_Support" when configuring ShapeWorks in CMake
* If you have installed fim in a reasonable location, then the ShapeWorks config will automatically find it.  Otherwise, you'll need to specify the FIM_LIB and FIM_INCLUDE_PATH variables.


## Library Export
This library is deployed as part of ShapeWorks, but at some point in the future it may be desirable to make it completely independent by moving it to a separate repository. This can be achieved, maintaining the git commit history, by using the following steps from a clone of the ShapeWorks code.

```
git filter-branch --tag-name-filter <tag>  
  --index-filter 'git rm -r --cached  
  --ignore-unmatch <unwanted> <dirs> <files>'  
  --prune-empty -f -- --all
git gc --aggressive
```

Some resources if you want to better understand the above commands include:

* [add folder from one repo to another](https://github.community/t5/How-to-use-Git-and-GitHub/Adding-a-folder-from-one-repo-to-another/m-p/5574)
* [detach part of a git repository](https://stackoverflow.com/questions/359424/detach-move-subdirectory-into-separate-git-repository/17864475#17864475)
* [follow renames when splitting git subtree](https://stackoverflow.com/questions/26304941/follow-renames-when-performing-git-subtree-split)
