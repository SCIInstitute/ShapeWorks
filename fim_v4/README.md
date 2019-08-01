# FIM_V4 Library

--
**IMPORTANT:**  
This repository has been merged with [ShapeWorks](https://github.com/SCIInstitute/shapeworks).  
Please find the most up-to-date code there.

--
###Overview
_TODO_

--
###License
_TODO_  

THIS IS BASED ON FIM LIBRARY FROM FIDS AND GEODESICS REPOS JANUARY 2016 (fim_v4_10_17_2014)

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
