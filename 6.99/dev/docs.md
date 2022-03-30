# Getting Started with Documentation

!!! danger "Which branch to use?"
    Please use the `mkdocs` branch to add/edit your documentation according to the guidelines detailed in this page. Once you are done with your edits, please submit a Pull Request (PR) to get your changes reviewed and merged to the master branch.
    
!!! danger "For PRs that include documentation changes"
    At the beginning of the PR, please add instructions on how one could compile the documentation and what pages should be reviewed. For example, these instructions could include the following: 
    
    - Checkout the `mkdocs` branch
    - Run `mkdocs serve` from the root directory (containing mkdocs.yml)
    - Open and review `path/to/edited-or-added-md-files`

## Naming and Organization

!!! note "Where are the documentation files?" 
    ShapeWorks documentation is written using [Markdown](../dev/markdown.md#markdown-basics), a text layout language that enables simple formatting for section headers, code samples, weblinks, and images, yet is still readable as plain text. All documentation markdown files are located in the `docs/` folder. Documentation settings are configured by using the `mkdocs.yml` configuration file in the ShapeWorks directory.


!!! important "Naming convention"
    To maintain consistency, please use lower-case letters and dashes for your markdown files, e.g., `fixed-domain-ellipsoid.md`.  

!!! note "Docs organization"
    ShapeWorks documentation is organized into main sections (e.g., "Getting Started", "Use Cases", ... etc), which map to subfolders in the `docs/` folder (e.g., "getting-started", "use-cases", ... etc) and images used in their markdown files are located in the corresponding subfolders in the `docs/img` folder.

## Adding Call-outs

We are now using the [Admonition](https://python-markdown.github.io/extensions/admonition/) extension to include side content that is minimally disruptive to the document flow. It is also very useful to a reader's draw attention. Below are some examples.

To add a note block with a title, use the following markdown syntax.

```
!!! note "Title of the note"
    Here is a note to keep in mind.
```

It will render as follows.

!!! note "Title of the note"
    Here is a note to keep in mind.
    

You can also add a note without a title.

```
!!! note 
    This is a note without a title. This also applies to other call-outs.
```

It will render as follows.

!!! note 
    This is a note without a title

To make a note (or any call-out collapsable, use `???` as follows.

```
??? note 
    This is a collapsable note without a title. This also applies to other call-outs.
```

It will render as follows.

??? note 
    This is a collapsable note without a title. This also applies to other call-outs.


To add a danger block with a title, use the following markdown syntax.

```
!!! danger "Don't try this at home"
    Seriously don't try this; this is a dangerous step to take.
```

It will render as follows.

!!! danger "Don't try this at home"
    Seriously don't try this; this is a dangerous step to take.
  
  
To add an important block without a title, use the following markdown syntax.   
 
``` 
!!! important 
    This is an admonition box without a title.
```    
   
It will render as follows.
 
!!! important 
    This is an admonition box without a title.
    
## Locally Building Documentation
  
To see local changes to documentation in realtime, `cd` to where source documentation (i.e., `mkdocs.yml`) is maintained and run the following command:

```
mkdocs serve
```

This command builds markdown files into HTML and starts a development server to browse the documentation. 

Open up `http://127.0.0.1:8000/` in your favorite browser to see the documentation. Changes you make to the markdown files are automatically rebuilt.


## Deploying on GitHub Pages

As ShapeWorks is hosted on [GitHub](https://github.com/SCIInstitute/ShapeWorks), we use GitHub Pages to host the documentation for ShapeWorks. 

We use Project Pages sites for documentation deployment. The site files are deployed to the `gh-pages` branch within the ShapeWorks repository.

Deployment is taken care of automatically by GitHub Actions using the script `Support/deploy_docs.sh`
 
!!! danger "Do not edit gh-pages"
    Never manually edit files on the `gh-pages` branch because you will lose your work the next time the docs are deployed.

## Contributing to Documentation

!!! important
    If you added a new markdown file to `docs/`, please make sure to include it in it relevant section in `mkdocs.yml` to make it accessible.

We use GitHub to keep track of issues pertaining to ShapeWorks documentation. For an internal list of todos, which will be turned to issues, visit [Documentation ToDo List](../todo.md).

## Inserting Videos in Documentation

[Request on GitHub](https://github.com/SCIInstitute/ShapeWorks/issues/new) for your video to be uploaded to the SCI ShapeWorks server.

Insert it in the markdown file using `<p><video src="https://sci.utah.edu/~shapeworks/doc-resources/mp4s/video.mp4" autoplay muted loop controls style="width:100%"></p>`.

!!! note "Where are the videos stored?" 
    On SCI servers, at `/usr/sci/www/shapeworks/doc-resources/mp4s/`.



## Auto-generating ShapeWorks Commands Documentation

The `DocumentationUtils` package in `Python` has APIs for auto-documenting command-line tools and *to-come-soon* python APIs. We use the `docs` folder to save the generated documentation.


To generate documentation for the `shapeworks` commands, first be sure to run `install_shapeworks.sh` as described in [How to Build ShapeWorks from Source?](build.md) to install `DocumentationUtils`.


Then, make sure that the `shapeworks` command is in your path (`set PATH=/path/to/shapeworks:$PATH`), then use Python to run the following command:


```shell
$ python Python/RunShapeWorksAutoDoc.py --md_filename docs/tools/ShapeWorksCommands.md
```

**Parameters**:    

  - `md_filename` is the markdown file name for the documentation file to be generated

## Auto-generating C++ Doxygen API Documentation

To generate C++ Doxygen API output into mkdocs, configure ShapeWorks with BUILD_DOCUMENTATION=ON.

After building, run (from the root source directory):

```shell
$ ./Support/build_docs.sh ${BUILD_DIR}/Documentation/Doxygen/xml
```


## See Also
- [MkDocs documentation](https://mkdocs.readthedocs.io/en/stable/)
- [Getting started with MkDocs](https://docs.readthedocs.io/en/stable/intro/getting-started-with-mkdocs.html)
- [Setting up MkDocs](https://mikedemaso.com/tech/2019-06-20-setting-up-mkdocs/)
- [Projects documentation](https://netgen.io/blog/the-most-overlooked-part-in-software-development-writing-project-documentation)
- [MkDocs recipes](https://github.com/mkdocs/mkdocs/wiki/MkDocs-Recipes)


