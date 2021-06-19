
# Getting Started with Markdown

ShapeWorks documentation is written using [Markdown](#markdown-basics), a text layout language that enables simple formatting for section headers, code samples, weblinks, and images, yet is still readable as plain text. 

!!! note 
    If you are viewing this document in GitHub, you can click on the pencil icon in the top-right corner to see its source. On GitHub, it can be used for issues and wiki documentation and edited inline.

## Grip

To edit your Markdown, it helps to have a convenient viewer. The `grip` instant preview application is beneficial, and is installed by the install_shapeworks script (see [How to Build ShapeWorks from Source?](build.md). From the ShapeWorks directory, just run `grip` (be sure to `conda activate shapeworks` beforehand), and then navigate to `http://localhost:6419` in your favorite browser. It will load and display markdown files just like GitHub, showing README.md by default. You can also specify relative paths to any markdown file below the directory from which it was run. Happy editing!

## Markdown Basics

ShapeWorks uses Markdown for much of its documentation.  
Here are the basics of using Markdown. The plain text is readable, and there are many editors available, such as [Dillinger](https://dillinger.io).  
One method is to use a plain text editor along with an automatic rendering tool such as [grip](https://github.com/joeyespo/grip/blob/master/README.md) to view the formatted results in a browser as the files are edited.

Use hash symbols to create section headers. Use more hashes for subsequent subsections.
```
# Main section
## Subsection
### Sub-subsection
#### And
##### so
###### on... 
```

Links are created by enclosing the text shown for the link in brackets and the link directly adjacent to parenthesis. Links to other '#'-indicated sections of the document are formed using a '#' followed by the lowercase text of the section name separated with dashes. For icons, add some additional brackets and a '!'.  
[external site](http://google.com)  
[link text](#local-section-name)  
[![thumbs up](https://66.media.tumblr.com/1f45d6ab69e02479f85ac1c9f1eb4301/tumblr_inline_pkaqpvkvHH1syktzs_540.png)](http://google.com)

```
[external site](http://google.com)
[link text](#local-section-name)
[![thumbs up](https://66.media.tumblr.com/1f45d6ab69e02479f85ac1c9f1eb4301/tumblr_inline_pkaqpvkvHH1syktzs_540.png)](http://google.com)
```

HTML comments can be utilized within a Markdown document if you don't want something shown in the rendered output:
```
<!--
  commented stuff
-->
```

Finally, code can be shown using triple back-ticks (the backward apostrophe: '\`' ), even highlighted for a particular language by following the first set of back-ticks with the language name.  

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
   

To quickly turn a URL or email address into a link, enclose it in angle brackets.

`<https://www.markdownguide.org>`  
`<fake@example.com>`

renders as

<https://www.markdownguide.org>  
<fake@example.com>



## See Also
[GitHub Markdown](https://help.github.com/en/articles/basic-writing-and-formatting-syntax)  
[Markdown Guide](https://www.markdownguide.org/basic-syntax/)  
[Markdown Editors](https://www.oberlo.com/blog/markdown-editors)
