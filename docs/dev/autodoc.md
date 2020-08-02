
# How to Auto-generate Documentation?


## MkDocs


To generate static html files for the markdown files, [checkout](build.md#clone-source) the branch where source documentation (`mkdocs.yml`) is maintained and run the following command:

`mkdocs gh-deploy`

Using this command, MkDocs builds the project documentation and uses the ghp-import tool to commit them to the gh-pages branch and push the gh-pages branch to GitHub. *All behind the scenes ...*

Use `mkdocs gh-deploy --help` for a full list of options available for the gh-deploy command.

:fa-info-circle: _**Important:**_ You can not review the built site before pushing it to GitHub. To verify any changes you made locally to the documentation, please use `mkdocs serve`.  


:fa-info-circle: _**Important:**_ Never manually edit files on the `gh-pages` branch because you will lose your work the next time you run the `gh-deploy` command.

## ShapeWorks Commands

The `DocumentationUtils` package has APIs for auto-documenting command line tools and *to-come-soon* python APIs. Please use the `docs` folder to save generated documentation.


To generate documentation for the `shapeworks` commands, use the `generateShapeWorksCommandDocumentation` function of the `DocumentationUtils` library. Be sure to run `conda_install.sh` as described in [How to Build ShapeWorks from Source?](build.md) to install `DocumentationUtils`.

First, make sure that the `shapeworks` command is in your path (`set PATH=/path/to/shapeworks:$PATH`), then use Python to run the command:

```python
import DocumentationUtils
DocumentationUtils.generateShapeWorksCommandDocumentation(mdFilename = '/path/to/ShapeWorks/docs/tools/ShapeWorksCommands.md')
```

**Parameters**:   
  - **mdFilename** is the markdown file name for the documentation file to be generated
  
  
## Doxygen

