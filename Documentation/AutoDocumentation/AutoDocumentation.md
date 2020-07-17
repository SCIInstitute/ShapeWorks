
Auto Documentation (for developers)
===================

Table of Contents
====================
- [ShapeWorks Command](#shapeworks-commands) 
- [Doxygen](#doxygen)


The `DocumentationUtils` package has APIs for auto-documenting command line tools and *to-come-soon* python APIs. Please use the `Documentation` folder to save generated documentation.

ShapeWorks Commands
=====================

To generate documentation for the `shapeworks` commands, use the `generateShapeWorksCommandDocumentation` function of the `DocumentationUtils` library. Be sure to run `conda_install.sh` as described in [BUILD.md](../Documentation/Build/BUILD.md) to install `DocumentationUtils`.

First, make sure that the `shapeworks` command is in your path (`set PATH=/path/to/shapeworks:$PATH`), then use Python to run the command:

```python
import DocumentationUtils
DocumentationUtils.generateShapeWorksCommandDocumentation(mdFilename = '/path/to/ShapeWorks/Documentation/ShapeWorksCommands/ShapeWorksCommands.md')
```

**Parameters**:   
  - **mdFilename** is the markdown file name for the documentation file to be generated
  
  
Doxygen
=====================
