# Adding to PATH Environment Variable

:fa-exclamation-circle: **Be careful** doing this! You are responsible for messing up your own PATH.  

## Adding to the PATH on OSX/Linux
`$ export PATH=path/to/add:$PATH`  

Verify the results with the command: `$ echo $PATH`  

## Adding to the PATH on Windows
`$ set PATH=path/to/add;%PATH%`  
This only modifies the path for the current command prompt.  

To permanently add to the path:     

- Go to *Settings/Edit the system environment variables/Environment Variables*  
- Choose the *Path* variable and press *Edit...*  
- Add your path entry to the list  

Verify the results with the command: `$ echo %PATH%`  

