REM update anaconda

call conda config --add channels anaconda
call conda config --add channels conda-forge

call conda activate base
call conda update --yes -n base -c defaults conda
call conda update --yes --all

REM create and activate shapeworks env
call conda create --yes --name shapeworks python=3.7
call conda activate shapeworks

REM install shapeworks deps
call conda install --yes colorama=0.4.3
call conda install --yes requests=2.22.0
call conda install --yes openblas=0.3.3
call conda install --yes numpy=1.17.4
call conda install --yes scikit-learn=0.22.1
call conda install --yes gtest=1.10.0 cmake-3.15.5
call conda install --yes pytorch torchvision cpuonly -c pytorch
call conda install --yes notebook=6.1.5

REM reactivate shapeworks environment
call conda activate base
call conda activate shapeworks

call pip install --upgrade pip
call pip install termcolor==1.1.0
call pip install matplotlib==3.1.2
call pip install itk==5.0.1
call pip install vtk==8.1.2
call pip install bokeh==2.2.0
call pip install mdutils
call pip install mkdocs
call pip install python-markdown-math
call pip install fontawesome-markdown
call pip install pymdown-extensions

REM install the local GirderConnector code as a package
call pip install Python/DatasetUtilsPackage
call pip install Python/DocumentationUtilsPackage
call pip install Python/DataAugmentationUtilsPackage
call pip install Python/DeepSSMUtilsPackage
  
REM installs for jupyter notebooks
call pip install nbstripout
call pip install pyvista
call pip install ipyvtk_simple
call pip install ipywidgets
call pip install itkwidgets
call pip install mkdocs-jupyter

REM for spell check markdown cells in jupyter notebooks and table of content
call conda install --yes jupyter_contrib_nbextensions
call jupyter contrib nbextension install --user
call jupyter nbextension enable spellchecker/main
call jupyter nbextension enable toc2/main

REM installing nbstripout to strip out notebooks cell outputs before committing 
call nbstripout --install
call nbstripout --install --attributes .gitattributes

call conda info




