# ShapeWorks Documentation

This is a repository for sharing information on the use and development of the ShapeWorks toolkit.

[The ShapeWorks Team](#the-team)  
[User Documentation](#user-documentation)  
[Developer Documentation](#documentation)  
[ShapeWorks Source](#shapeworks-source)  
[Building ShapeWorks](#building-shapeworks)  


## The Team

The ShapeWorks team has several methods of communication, including mailing lists, a [Gitter community] forum, and [Trello boards] for planning.

### Mailing Lists
We have a mailing list for all users of ShapeWorks: `shapeworks-users@sci.utah.edu`
And we have a mailing list for ShapeWorks developers: `shapeworks-dev@sci.utah.edu`

### Community Forums
We use Gitter for our community discussion forums, which allows separation of different topics into specific "rooms" as well as both public and private forums, so our community of users can also easily reach out to us. You can access the ShapeWorks Gitter using the [Gitter community] link above or download the [Gitter app](https://gitter.im/apps) for any platform or your phone. It provides easy integration with [GitHub] and has no limitation on the number of messages as is the case with other services.

[![Gitter](https://badges.gitter.im/ShapeWorks/ShapeWorks.svg)](https://gitter.im/ShapeWorks/ShapeWorks?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

### Trello Boards
We are using these [Trello boards] for planning. Though similar to a Gitter discussion, these boards can include figures and grow with each ShapeWorks sub-project.

### Source Code
[GitHub] is the place for all our source code, as well as the appropriate location to file bugs and features requests. See [ShapeWorks source](#shapeworks-source) for details of checkout out and committing newly modified code.

Next that currently there are separate repositories for [ShapeWorks Studio](https://github.com/SCIInstitute/ShapeWorksStudio) and the [ShapeWorks documentation](https://github.com/SCIInstitute/shapeworks.pages). The GUI and documentation _are not up to date with the latest ShapeWorks tools,_ but once these are updated they will both be incorporated with the main ShapeWorks codebase (see the github isses for these tasks: [update studio](https://github.com/SCIInstitute/ShapeWorks/issues/45), [update docs](https://github.com/SCIInstitute/ShapeWorks/issues/46)).

## User documentation

See [Documentation/GettingStarted.md](Documentation/GettingStarted.md) for links to ShapeWorks documentation and examples.

## Markdown

The documents contained in this repo are written using [Markdown](#markdown-basics), a text layout language that enables simple formatting for things like section headers, code samples, web links and images, yet is still readable as plain text. If you're viewing this document in github you can click on the pencil icon in the top-right corner to see its source. On github it can be used for issues and wiki documentation and edited inline.

[Markdown Basics](#markdown-basics)  
[GitHub Markdown](https://help.github.com/en/articles/basic-writing-and-formatting-syntax)  

## ShapeWorks Source

**[Using `git`](GIT.md)**

## Building ShapeWorks

Please see **[BUILD.md](BUILD.md)** for the current build instructions.


## Markdown Basics

ShapeWorks uses Markdown for much of its documentation.  
Here are the basics of using Markdown. The plain text is readable and there are many editors available, such as [Dillinger](https://dillinger.io).  
One method is to use a plaion text editor along with an automatic rendering tool such as [grip](https://github.com/joeyespo/grip/blob/master/README.md) to view the formatted results in a browser as the files are edited.

Use hash symbols to create section headers. Use more hashes for successive subsections.
```
# Main section
## Subsection
### Sub-subsection
#### And
##### so
###### on... 
```

Links are created by enclosing the text shown for the link in brackets and the link directly adjacent in paranthesis. Links to other '#'-indicated sections of the document are formed by using a '#' followed by the lowercase text of the section name separated with dashes. For icons, add some additional brackets and a '!'.  
[external site](http://google.com)  
[link text](#local-section-name)  
[![thumbs up](https://66.media.tumblr.com/1f45d6ab69e02479f85ac1c9f1eb4301/tumblr_inline_pkaqpvkvHH1syktzs_540.png)](http://google.com)

```
[external site](http://google.com)
[link text](#local-section-name)
[![thumbs up](https://66.media.tumblr.com/1f45d6ab69e02479f85ac1c9f1eb4301/tumblr_inline_pkaqpvkvHH1syktzs_540.png)](http://google.com)
```

HTML comments can be utilized within a Markdown document if you don't want something to be shown in the rendered output:
```
<!--
  commented stuff
-->
```

Finally, code can be shown using triple back-ticks (the backwards apostrophe: '\`' ), even highlighted for a particular language by following the first set of back-ticks with the language name.  

````
```python  
print("Hello Markdown!")
```  
````  

```python
print("Hello Markdown!")
```  

Use just a single tick to keep monospaced text `inline with the rest of the text`.

You can also add tables, quoted text like you'd see in an email, bulleted items, images and more. Here are [several](https://www.markdownguide.org/cheat-sheet) [Markdown](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) [quick](https://commonmark.org/help/) [references](https://agea.github.io/tutorial.md/).


<!-- shortcut links used in this document -->

   [github]: <https://github.com/>
   [Gitter community]: <https://gitter.im/ShapeWorks>
   [Trello boards]: <https://trello.com/shapeworksteam>
   
## PATH environment variable

**Be careful** doing this! You are responsible for messing up your own PATH.  

### Adding to the PATH on OSX/Linux
`$ export PATH=path/to/add:$PATH`  

Verify the results with the command: `$ echo $PATH`  

### Adding to the PATH on Windows
`$ set PATH=path/to/add;%PATH%`  
This only modifies the path for the current command prompt.  
To permanently add to the path:  
- Go to *Settings/Edit the system environment variables/Environment Variables*  
- Choose the *Path* variable and press *Edit...*  
- Add your path entry to the list  

Verify the results with the command: `$ echo %PATH%`  
