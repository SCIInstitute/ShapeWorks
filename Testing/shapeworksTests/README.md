shapeworks executable sample scripts
=====

These scripts serve both as examples of using the shapeworks command and unit tests.

***Creating the scripts***

The following example script shows what the tests look like and includes comments about why:

```bash
#!/bin/bash

# IMPORTANT: must have `if [[ $? ]]; then exit -1; fi` between each line or an earlier test can fail unnoticed

shapeworks readimage --name $DATA/1x2x2.nrrd translate -x 10.0 -y 10.0 -z 10.0 compare --name $DATA/translate1.nrrd
if [[ $? ]]; then exit -1; fi   
shapeworks readimage --name $DATA/1x2x2.nrrd translate -x -10.0 -y -10.0 -z -10.0 compare --name $DATA/translate2.nrrd
if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/la-bin.nrrd translate --centerofmass 1 compare --name $DATA/translate_com.nrrd

# the last test's result will be the return from the script, so if it gets this far all the prev tests succeeded
```
Use `chmod u+x <scriptname>` to ensure it can be executed directly.

To generate data against which to compare, temporarily add a writeimage to the script and comment out the exit status checks:
```bash
shapeworks readimage --name $DATA/1x2x2.nrrd scale -x 1.0 -y 2.0 -z 2.0 writeimage --name ~/data/sw/tmp/scale1-test.nrrd compare --name $DATA/scale1.nrrd
# if [[ $? ]]; then exit -1; fi
shapeworks readimage --name $DATA/la-bin-centered.nrrd scale -x 1.0 -y -2.0 -z -1.0 writeimage --name ~/data/sw/tmp/scale2-test.nrrd compare --name $DATA/scale2.nrrd
```
Note the `writeimage --name <path>` before the compare.

*_Be sure to verify the outputs are correct before committing the script and data!_*


***Running the scripts***

To run the scripts independently, ensure the `shapeworks` executable is in your path and export `DATA` to point to `../data/shapeworks`, like this:

```bash
$ export PATH=/Applications/ShapeWorks/bin:$PATH
$ export DATA=.`pwd`/data/shapeworks
```

Now you should be able to execute these examples directly.
Alternatively, they can all be executed using the `shapeworksTests` command, which even accepts parameters to limit which tests get run (see googletests[../README.md#googletest] for details).
