From now onwards fim library is hosted in a separate repository at https://prafulag@bitbucket.org/prafulprojects/fim_v4
It is added as a submodule

1. Cloning a fresh hipworks repository:
    - git clone git@bitbucket.org:prafulprojects/hipworks.git
    - vi .gitmodules
        - edit url = https://prafulag@bitbucket.org/prafulprojects/fim_v4 ---> url = https://<your_bitbucket_username>@bitbucket.org/prafulprojects/fim_v4
    - git submodule init
    - git submodule update
    - cd fim_v4
    - git checkout master

2. First time updating your repository after addition of submodules:
    - git pull
    - vi .gitmodules
        - edit url = https://prafulag@bitbucket.org/prafulprojects/fim_v4 ---> url = https://<your_bitbucket_username>@bitbucket.org/prafulprojects/fim_v4
    - git submodule init
    - git submodule update
    - you may need to enter your bitbucket password at this point
    - cd fim_v4
    - git checkout master
        
3. Pushing changes made in submodule code:
    - Go to the submodule directory
    - git status
    - if status shows headless repository then perform git checkout master
    - Now add/commit/push as a regular repository.

4. Updating changes from remote host to submodule:
    - Go to the submodule directory
    - git pull <same as a regular repository>
