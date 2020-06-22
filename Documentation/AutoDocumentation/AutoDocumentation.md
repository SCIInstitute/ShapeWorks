
Auto Documentation (for developers)
===================

Table of Contents
====================
- [ShapeWorks Command](#shapeworks-commands) 
- [Doxygen](#doxygen)


The `DocumentationUtils` package has APIs for auto-documenting command line tools and *to-come-soon* python APIs. Please use the `Documentation` folder to save generated documentation.

ShapeWorks Commands
=====================

To generate documentation for the `shapeworks` commands, use the `generateShapeWorksCommandDocumentation` API as follows.

```python
import DocumentationUtils
DocumentationUtils.generateShapeWorksCommandDocumentation(mdFilename = '/path/to/documentation/ShapeWorksCommands/ShapeWorksCommands.md')
```

**Parameters**:   
  - **mdFilename** is the markdown file name for the documentation file to be generated
  
  
Doxygen
=====================