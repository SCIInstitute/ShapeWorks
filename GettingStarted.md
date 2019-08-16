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

*TODO*

## Documentation

The documents contained in this repo are written using [Markdown](#markdown-basics), a text layout language that enables simple formatting for things like section headers, code samples, web links and images, yet is still readable as plain text. If you're viewing this document in github you can click on the pencil icon in the top-right corner to see its source. On github it can be used for issues and wiki documentation and edited inline.

[Markdown Basics](#markdown-basics)  
[GitHub Markdown](https://help.github.com/en/articles/basic-writing-and-formatting-syntax)  

## ShapeWorks Source

We use `git` for managing our source code so we can integrate changes from multiple people. Here are some of the basics so you can checkout the repository, make and share modifications, and keep track of all your changes along the way.
The ShapeWorks repositories are stored on [GitHub]. If you want to contribute, first _make sure you have an account_.  

[Source and branches](#source-and-branches)  
[Commits](#commits)  
[Logs and diffs](#logs-and-diffs)  
[Tags](#tags)  
[Advanced](#advanced)  

### Source and branches

To clone the ShapeWorks source, use one of the following commands:  
```
$ git clone https://github.com/SCIInstitute/ShapeWorks
```

In order to avoid having to type your password every time you refresh your source code you can copy your public ssh key (found in ~/.ssh/id_rsa.pub) and [add it to your github account](https://github.com/settings/keys). Now you can use this version to clone the code: 
```
$ git clone git@github.com:sciinstitute/ShapeWorks
```
If you've already cloned your code, simply change the `[remote "origin"]` in .git/config to `url = git@github.com:SCIInstitute/ShapeWorks`. All git commands remain the same.

_Now that you've cloned the source you can already [build ShapeWorks](BUILD.md#build) if you want._

View current state (branch and modifications, both staged and unstaged):  
```
$ git status
```

View all branches:  
```
$ git branch -a
```

Checkout a branch (or a tag or a specific commit):  
```
$ git checkout <branchname>
```

Fetch changes for a specific branch from GitHub:  
```
$ git fetch origin <branchname>
```

Fetch changes for all branches from GitHub:  
```
$ git fetch --all
```

Merge the fetched changes to your current branch:  
```
$ git merge origin/<branchname>
```

Shortcut to both fetch and merge upstream changes to the current branch:  
```
$ git pull
```

Push a branch to GitHub:  
```
$ git push origin <branchname>
```

Delete a branch (e.g., after a pull request has been completed and it has been merged ):  
```
$ git branch -d <branchname>
```

_Force_ delete a branch whether or not it's been merged:  
```
$ git branch -D <branchname>
```

Prune old branches that have been removed from GitHub (--dry-run will show you what will be pruned):  
```
$ git remote prune origin [--dry-run]
```

#### Commits

Check the current status of your changes scheduled to be committed using:  
```
$ git status
```

To commit your current changes to current *local* branch (first use `git diff` to ensure they are correct):  
```
$ git diff
$ git commit -m "description of changes"
```

This commits only the files specifically scheduled to be commited.  
If you want _all_ changes to be committed, use the `-a` switch:  
```
$ git commit -am "description of changes"
```
If you only want to commit scheduled files and a file has been changed after being scheduled, simply add it again for it to be updated:  
```
$ git add <filename>
```

To push your local changes for a given branch to GitHub, first use `git diff` to ensure they are correct, then push to origin:  
```
$ git diff origin/<branchname> <branchname>
$ git push origin <branchname>
```
_Never push directly to **master**_. After pushing, [submit a pull request](https://github.com/SCIInstitute/ShapeWorks/pull/new/master) (click the "New pull request" button on the main GitHub page) for your changes to be checked and merged to the master branch.

#### Logs and diffs

View the diff of the most recent commit:  
```
$ git diff HEAD~1
```

View the log of a particular file (including moves/renames):  
```
$ git log --follow -- <filename>
```

View the commit history for a file showing the detailed diffs for each commit:  
```
$ git log -p <filename>
```

View the commit history of a file/dir when its name is the same as a branch:  
```
$ git log -- <filename>
```

View the diff between one commit and its predecessor:  
```
$ git diff <commit_SHA>~1..<commit_SHA>
```

View only the files that changes for a given range of commits:  
```
$ git diff --name-only <SHA1> <SHA2>
```

#### Tags

Specific versions and releases can be identified using tags. 

List current tags:__
```
$ git tag --list
```

Show details for a specific tag (`--quiet` hides the diffs):__
```
$ git show <tagname> --quiet
```

By default, checking out a tag puts your repo into a 'detached head' state. Use the following command the first time you check out a tag or a specific commit to create a local branch with that name (this happens automatically when checking out branches):  
```
$ git checkout origin/<tag> -b <tagname>
```

Create a new tag using the following:  
```
$ git tag <tagname>
```

Tags must be explicitly pushed to github. Push your new tag using:  
```
$ git push origin <tagname>
```

Delete a tag using:  
```
$ git tag delete <tagname>
$ git push origin --delete <tagname>
```
The second command is only necessary if the tag has been pushed to Github.  

### Advanced
There are many other things that can be done, such as mashing together commits, temporarilyt "stashing" and retrieving stashed changes, etc.
Please add anything here that seems useful.

Stash your current changes (temporarily set them aside without commiting):
```
$ git stash -m "what is being stashed"
```

Retrieve the stashed changes:
```
$ git stash pop
```

Modify the description of the most recent commit:  
**_WARNING:_** only use this if the commit has not yet been pushed to GitHub.  
```
$ git commit --amend
```

## Building ShapeWorks

Please see **[BUILD.md](BUILD.md)** for the current build instructions.


## Markdown Basics

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
