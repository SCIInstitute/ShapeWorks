# Getting Started with Documentation



## Naming and Organization

:fa-info-circle: _**Where are the documentation files?**_ ShapeWorks documentation is written using [Markdown](../dev/markdown.md#markdown-basics), a text layout language that enables simple formatting for section headers, code samples, weblinks, and images, yet is still readable as plain text. All documentation markdown files are located in the `docs/` folder. Documentation settings are configured by using the `mkdocs.yml` configuration file in the ShapeWorks directory.


:fa-info-circle: **Naming convention:** To maintain consistency, please use lower-case letters and dashes for your markdown files, e.g., `fixed-domain-ellipsoid.md`.  

:fa-info-circle: **Docs organization:** ShapeWorks documentation is organized into main sections (e.g., "Getting Started", "Use Cases", ... etc), which map to subfolders in the `docs/` folder (e.g., "getting-started", "use-cases", ... etc) and images used in their markdown files are located in the corresponding subfolders in the `docs/img` folder.


## Locally Building Documentation
  
To see local changes to documentation in realtime, `cd` to where source documentation (i.e., `mkdocs.yml`) is maintained and run the following command:

`mkdocs serve`

This command builds markdown files into HTML and starts a development server to browse the documentation. 

Open up `http://127.0.0.1:8000/` in your favorite browser to see the documentation. Changes you make to the markdown files are automatically rebuilt.


## Deploying on GitHub Pages

As ShapeWorks is hosted on [GitHub](https://github.com/SCIInstitute/ShapeWorks), we use GitHub Pages to host the documentation for ShapeWorks. 

We use Project Pages sites for documentation deployment. The site files are deployed to the `gh-pages` branch within the ShapeWorks repository.

To generate static HTML files for the markdown files, [checkout](build.md#clone-source) the branch where source documentation (`mkdocs.yml`) is maintained and run the following command:

`mkdocs gh-deploy`

Using this command, MkDocs builds the project documentation and uses the ghp-import tool to commit them to the gh-pages branch and push the gh-pages branch to GitHub. *All behind the scenes ...*


:fa-info-circle: _**WARNING:**_  Running `mkdocs gh-deploy` will update the documentation seen by everyone on [GitHub](https://github.com/SCIInstitute/ShapeWorks). It is recommented that deployed documentation on [GitHub](https://github.com/SCIInstitute/ShapeWorks) should be synced with documentation in the `master` branch.

:fa-info-circle: _**Important:**_ To deploy the documentation associated with software releases, please use `mkdocs gh-deploy -m "Release Number"`.


Use `mkdocs gh-deploy --clean` to start a fresh deployment.   

Use `mkdocs gh-deploy --help` for a full list of options available for the gh-deploy command.

:fa-info-circle: _**Important:**_ You can not review the built site before pushing it to GitHub. To verify any changes you made locally to the documentation, please use ``mkdocs build`` or `mkdocs serve`.  
 
:fa-info-circle: _**Important:**_ Never manually edit files on the `gh-pages` branch because you will lose your work the next time you run the `gh-deploy` command.


## Contributing to Documentation

:fa-info-circle: _**Important:**_ If you added a new markdown file to `docs/`, please make sure to include it in it relevant section in `mkdocs.yml` to make it accessible.

We use GitHub to keep track of issues pertaining to ShapeWorks documentation. For an internal list of todos, which will be turned to issues, visit [Documentation ToDo List](../todo.md).

## Inserting Videos in Documentation

[Request on GitHub](https://github.com/SCIInstitute/ShapeWorks/issues/new) for your video to be uploaded to the SCI ShapeWorks server.

Insert it in the markdown file using `<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/video.mp4" autoplay muted loop controls style="width:100%"></p>`.

:fa-info-circle: _**Where are the videos stored?**_ On SCI servers, at `/usr/sci/www/shapeworks/doc-resources/mp4s/`.



## Auto-generating ShapeWorks Commands Documentation

The `DocumentationUtils` package in `Python` has APIs for auto-documenting command-line tools and *to-come-soon* python APIs. We use the `docs` folder to save the generated documentation.


To generate documentation for the `shapeworks` commands, use the `generateShapeWorksCommandDocumentation` function of the `DocumentationUtils` library. Be sure to run `conda_install.sh` as described in [How to Build ShapeWorks from Source?](build.md) to install `DocumentationUtils`.

First, make sure that the `shapeworks` command is in your path (`set PATH=/path/to/shapeworks:$PATH`), then use Python to run the command:

```python
import DocumentationUtils
DocumentationUtils.generateShapeWorksCommandDocumentation(mdFilename = '/path/to/ShapeWorks/docs/tools/ShapeWorksCommands.md')
```

**Parameters**:   
  - **mdFilename** is the markdown file name for the documentation file to be generated


## See Also
- [MkDocs documentation](https://mkdocs.readthedocs.io/en/stable/)
- [Getting started with MkDocs](https://docs.readthedocs.io/en/stable/intro/getting-started-with-mkdocs.html)
- [Setting up MkDocs](https://mikedemaso.com/tech/2019-06-20-setting-up-mkdocs/)
- [Projects documentation](https://netgen.io/blog/the-most-overlooked-part-in-software-development-writing-project-documentation)
- [MkDocs recipes](https://github.com/mkdocs/mkdocs/wiki/MkDocs-Recipes)


