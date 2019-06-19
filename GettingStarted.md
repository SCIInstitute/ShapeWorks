# ShapeWorks Documentation

This is a repository for sharing information on the use and development of the ShapeWorks toolkit.

## The Team

The ShapeWorks team has several methods of communication, including mailing lists, a [Gitter community] forum, and [Trello boards] for planning.

### Mailing Lists
We have a mailing list for all users of ShapeWorks: shapeworks-users@sci.utah.edu.
And we have a mailing list for ShapeWorks developers: shapeworks-dev@sci.utah.edu.

### Community Forums
We use Gitter for our community discussion forums, which allows separation of different topics into specific "rooms" as well as both public and private forums, so our community of users can also easily reach out to us. You can access the ShapeWorks Gitter using the [Gitter community] link above or download the [Gitter app](https://gitter.im/apps) for any platform or your phone. It provides easy integration with [GitHub] and has no limitation on the number of messages as is the case with other services.

[![Gitter](https://badges.gitter.im/ShapeWorks/ShapeWorks.svg)](https://gitter.im/ShapeWorks/ShapeWorks?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

### Trello Boards
We are using these [Trello boards] for planning. Though similar to a Gitter discussion, these boards can include figures and grow with each ShapeWorks sub-project.

### Source Code
[GitHub] is the place for all our source code, as well as the appropriate location to file bugs and features requests. There are six ShapeWorks repositories, four of which actually constitute a single cohesive repo consisting of the [ShapeWorks parent](https://github.com/SCIInstitute/shapeworks) and three sub-modules specifically for the tools needed to [prep](https://github.com/SCIInstitute/ShapeWorks-Prep), [run](https://github.com/SCIInstitute/ShapeWorks-Run), and [post](https://github.com/SCIInstitute/ShapeWorks-Post) data analyses using ShapeWorks.

Next, there is a separate repository for the [ShapeWorks GUI](https://github.com/SCIInstitute/ShapeWorksStudio). And finally, we have a repo for [documentation](https://github.com/SCIInstitute/shapeworks.pages). Please note that currently the GUI and documentation _are not up to date with the latest ShapeWorks tools._


## Document formatting

The documents contained in this repo are written using [Markdown](#markdown-basics), a simple text layout language that enables basic formatting for things like section headerd and code samples as well as easy addition of web links and images. If you're viewing this document in github you can click on the pencil icon in the top-right corner to see its source. 

[Markdown Basics](#markdown-basics)

## Markdown Basics

Here are the basics of using Markdown. The plain text is readable and there are many editors available, such as [Dillinger](https://dillinger.io). 

Use hash symbols to create section headers. Use more hashes for successive subsections.
# Main section
## First subsection
### Second subsection
#### Third subsection
##### Fourth subsection
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

Finally, code can be shown using triple back-ticks (the backwards apostrophe: '`'), even highlighted for a particular language by following the first set of back-ticks with the language name. 
````
```sh
$ echo "Hello Markdown!"
```
````
```sh
$ echo "Hello Markdown!"
```
Use just a single tick to keep monospaced text `inline with the rest of the text`.

You can also add tables, quoted text like you'd see in an email, bulleted items, images and more. Here are [several](https://www.markdownguide.org/cheat-sheet) [Markdown](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet) [quick](https://commonmark.org/help/) [references](https://agea.github.io/tutorial.md/).


## Source Control

We use `git` for keeping track of sources revisions and integrating changes from multiple individuals. Here are some of the basics so you can checkout a repository, make some modifications, and share them others, keeping track of all your changes along the way.

The ShapeWorks repositories are stored on [GitHub], so first _make sure you have an account_.

Our repositories use some external libraries such as [fim_v4](https://bitbucket.org/sheryjoe/fim_v4) as submodules, which can be tricky, but unless you are modifying code in one of these, they are automatically synced. See [git submodules](#git-submodules) for more info on using them.

### Basics

Clone the source:
```
$ git clone git@github.com:SCIInstitute/ShapeWorks-[Prep|Tools|...(todo)]
```

View current state (branch and modifications, both staged and unstaged):
```
$ git status
```

View all branches:
```
$ git branch -a
```

Checkout a branch:
```
$ git checkout <branchname>
```

Create a branch based on the current branch:
```
$ git branch -b <branchname>
```

Checkout a branch:
```
$ git checkout <branchname>
```

Delete a branch that's been merged with its parent:
```
$ git branch -d <branchname>
```

_Force_ delete a branch whether or not it's been merged:
```
$ git branch -D <branchname>
```

Push a branch to GitHub:
```
$ git push origin <branchname>
```

Prune old branches that have been removed from GitHub (--dry-run will show you what will be pruned):
```
$ git remote prune origin [--dry-run]
```

### Useful

View the log of a particular file (including moves/renames):
```
$ git log --follow -- <filename>
```

View the diff of the most recent commit:
```
$ git diff HEAD~1
```

View the diff between one commit and its predecessor:
```
$ git diff <commit_SHA>~1..<commit_SHA>
```

View only the files that changes for a given range of commits:
```
$ git diff --name-only <SHA1> <SHA2>
```

Stash your changes temporarily:
```
$ git stash save "desc of why these changes are being stashed"
```

Bring back stashed changes:  
```
$ git stash pop
```

Modify the description of the most recent commit:  
**_WARNING:_** only use this if the commit has not yet been pushed to GitHub.
```
git commit --amend
```  

### Advanced
There are many other things that can be done, such as mashing together commits, getting different stashed changes than the most recent, ...

Please add anything here that seems useful.

#### Commit history

View the commit history including moves and renames:
```
$ git log --follow <filename>
```

View the commit history including diffs:
```
$ git log -p <filename>
```

View the commit history of a file/dir when name is the same as a branch:
```
$ git log -- <filename>
```

#### Checkout a specific commit


### Git Submodules

Submodules are used in order to incorporate external libraries that also need to be modified to work with our code base, and it is desired for those modifications to be pushed back to the external library. Furthermore, the library is used by other projects at the same time. These three reasons must all be true to warrant the additional complication and susceptibility to errors that is brought on by the use of submodules.

If the library isn't used by any other projects, it is much simpler to just clone it separately and deploy it like any other external library. If the required changes will not be pushed back to the submodule, either copy its code directly into your project or clone externally, build and deploy as in 1. 


**TODO**

* submodules can be automatically pulled using CMake, but if a submodule is updated, can we ensure that CMake knows current version is up-to-date and update it if not? 
* What if the user is modifying their version of the submodule (using their own branch of course, _never master_)? Will CMake know this and avoid updating? Or should it update and a merge conflicts be noted? 
* How are submodule commits handled? (just please provide the necessary documentation)

## <next topic>...


[//]: # (After these reference links are used they get stripped out by the markdown processor.

   [github]: <https://github.com/>
   [Gitter community]: <https://gitter.im/ShapeWorks>
   [Trello boards]: <https://trello.com/shapeworksteam>
