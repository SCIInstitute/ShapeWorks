# Getting Started with MkDocs

## Install MkDocs and Themes 

:fa-info-circle: _**Important:**_ The below installations are added to `conda_insalls` scripts.

Install mkdocs.  
`pip install mkdocs`

Next install the theme (if needed). 
For example, `pip install mkdocs-bootswatch`

See [this link](https://mkdocs.github.io/mkdocs-bootswatch/) for list of bootswatch themes.    


Next install font awesome for icons to display in the documentation.   
`pip install fontawesome-markdown`


## Setting up the MkDocs project

Go to the directory for the MkDocs project.   

Then    

`mkdocs new .`

This command creates mkdocs.yml that will be used for MkDocs configuration, and docs/index.md, which is the Markdown file that is the entry point for the documentation.

## Naming and Organization

:fa-info-circle: **Naming convention:** To maintain consistency, please use lower-case letters and dashes for your markdown files, e.g., `fixed-domain-ellipsoid.md`.  

:fa-info-circle: **Docs organization:** ShapeWorks documentation is organized into main sections (e.g., "Getting Started", "Use Cases", ... etc), which map to subfolders in the `docs/` folder (e.g., "getting-started", "use-cases", ... etc) and images used in their markdown files are located in the corresponding subfolders in the `docs/img` folder.

## Building Documentation
  
`mkdocs serve`

This command builds markdown files into html and starts a development server to browse the documentation. 

Open up `http://127.0.0.1:8000/` in your favorite browser to see the documentation. Changes you make to the markdown files are automatically rebuild.

## Deploying on GitHub Pages

As ShapeWorks is hosted on [GitHub](https://github.com/SCIInstitute/ShapeWorks), we use GitHub Pages to host the documentation for ShapeWorks. 

We use Project Pages sites for documentation deployment. The site files are deployed to the `gh-pages' branch within the ShapeWorks repository.

To generate static html files for the markdown files, [checkout](build.md#clone-source) the branch where source documentation (`mkdocs.yml`) is maintained and run the following command:

`mkdocs gh-deploy`

Using this command, MkDocs builds the project documentation and uses the ghp-import tool to commit them to the gh-pages branch and push the gh-pages branch to GitHub. *All behind the scenes ...*

Use `mkdocs gh-deploy --clean` to start a fresh deployment.   

Use `mkdocs gh-deploy --help` for a full list of options available for the gh-deploy command.

:fa-info-circle: _**Important:**_ You can not review the built site before pushing it to GitHub. To verify any changes you made locally to the documentation, please use ``mkdocs build`` or `mkdocs serve`.  
 
:fa-info-circle: _**Important:**_ Never manually edit files on the `gh-pages` branch because you will lose your work the next time you run the `gh-deploy` command.

## Contributing to Documentation

We use github issues to keep track of hanging fruits to improve ShapeWorks documentation. For an internal list of todos, which will be turned to issues, visit [Documentation ToDo List](../todo.md).

## See Also
- [MkDocs documentation](https://mkdocs.readthedocs.io/en/stable/)
- [Getting started with MkDocs](https://docs.readthedocs.io/en/stable/intro/getting-started-with-mkdocs.html)
- [Setting up MkDocs](https://mikedemaso.com/tech/2019-06-20-setting-up-mkdocs/)
- [Projects documentation](https://netgen.io/blog/the-most-overlooked-part-in-software-development-writing-project-documentation)
- [MkDocs recipes](https://github.com/mkdocs/mkdocs/wiki/MkDocs-Recipes)

