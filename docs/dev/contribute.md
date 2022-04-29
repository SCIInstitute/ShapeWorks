# How to Contribute to ShapeWorks?

<!--## Source Management using Git
-->

ShapeWorks uses `git` for managing source code so changes can be integrated from multiple people. Here are some of the basics to check out the repository, make and share modifications, and keep track of all your changes along the way.
The ShapeWorks repositories are stored on [GitHub](https://github.com/SCIInstitute/ShapeWorks). 

!!! imporant
    If you want to contribute, first _make sure you have a developer account_.  
  

## Source and Branches

To clone the ShapeWorks source, use one of the following commands:  
```
$ git clone https://github.com/SCIInstitute/ShapeWorks
```

To avoid having to type your password every time you refresh your source code, you can copy your public ssh key (found in ~/.ssh/id_rsa.pub) and [add it to your GitHub account](https://github.com/settings/keys). Now you can use this version to clone the code: 
```
$ git clone git@github.com:sciinstitute/ShapeWorks
```
If you've already cloned your code, simply change the `[remote "origin"]` in .git/config to `url = git@github.com:SCIInstitute/ShapeWorks`. All git commands remain the same.

_Now that you've cloned the source you can [build ShapeWorks](build.md) from source if you want._

View current state (branch and modifications, both staged and unstaged):  
```
$ git status
```

View all branches:  
```
$ git branch -a
```

Checkout an exising branch (or a tag or a specific commit):  
```
$ git checkout <branchname>
```

Create a new branch
```
$ git checkout -b <branchname>
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
For example, use this command to merge the latest master to your current branch. After that, you can test as necessary, then push your branch and make a pull request when it's ready:
`git merge origin/master`

Shortcut to both fetch and merge upstream changes to the current branch:  
```
$ git pull
```

Push a branch to GitHub:  
```
$ git push origin <branchname>
```

Delete a branch (e.g., after a pull request has been completed and merged ):  
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

## Commits

Check the current status of your changes scheduled to be committed using:  
```
$ git status
```

To commit your current changes to current *local* branch (first, use `git diff` to ensure they are correct):  
```
$ git diff
$ git commit -m "description of changes"
```

This commits only the files specifically scheduled to be committed.  
If you want _all_ changes to be committed, use the `-a` switch:  
```
$ git commit -am "description of changes"
```
If you only want to commit scheduled files and a file has been changed after being scheduled, add it again to be updated:  
```
$ git add <filename>
```

To push your local changes for a given branch to GitHub, first use `git diff` to ensure they are correct, then push to origin:  
```
$ git diff origin/<branchname> <branchname>
$ git push origin <branchname>
```

!!! danger "Never push directly to **master** "
    After pushing, [submit a pull request](https://github.com/SCIInstitute/ShapeWorks/pull/new/master) (click the "New pull request" button on the main GitHub page) for your changes to be checked and merged to the master branch.

## Logs and diffs

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

## Tags

Specific versions and releases can be identified using tags. 

List current tags:
```
$ git tag --list
```

Show details for a specific tag (`--quiet` hides the diffs):
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

Tags must be explicitly pushed to GitHub. Push your new tag using:  
```
$ git push origin <tagname>
```

Delete a tag using:  
```
$ git tag delete <tagname>
$ git push origin --delete <tagname>
```
The second command is only necessary if the tag has been pushed to Github.  

## Advanced
There are many other things that can be done, such as mashing together commits, temporarily "stashing" and retrieving stashed changes, etc.
Please add anything here that seems useful.

Stash your current changes (temporarily set them aside without committing):
```
$ git stash -m "what is being stashed"
```

Retrieve the stashed changes:
```
$ git stash pop
```

Modify the description of the most recent commit:  

!!! imporant 
    Only use this if the commit has not yet been pushed to GitHub. 
     
```
$ git commit --amend
```

For further study please find various Git tutorials online, for example:

[githowto](https://githowto.com)

