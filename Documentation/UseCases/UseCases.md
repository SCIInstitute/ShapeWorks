
# Use Cases

- [Downloading Datasets](#Downloading-Datasets)
- [Running Use Cases](#Running-the-Use-Cases)
  - [Ellipsoid](Ellipsoid.md)
  - [Fixed Domains Ellipsoid](FixedDomainEllipsoid.md)
  - [Left Atrium](LeftAtrium.md)
  - [Femur](Femur.md)
- [Dataset Guidelines](#Dataset-Guidelines)


## Downloading Datasets from [ShapeWorks Data Portal](http://cibc1.sci.utah.edu:8080/)

Use Case datasets such as the ellipsoid and left atrium datasets will be downloaded
automatically from the *[ShapeWorks Data Portal](http://cibc1.sci.utah.edu:8080/)*.
When running one of the use case example scripts, you will see something like this:
```
Can't find ellipsoid.zip in the current directory.
.___________________________.
|                           |
|     ShapeWorks Portal     |
|___________________________|

Downloading the ellipsoid dataset from the ShapeWorks Portal
Login info not found in current directory.
New ShapeWorks Portal users: Register an account at http://cibc1.sci.utah.edu:8080/#?dialog=register
Returning ShapeWorks Portal users: Enter your username and password

Username: 
```

![ShapeWorks Portal Account Registration Screenshot](../images/ShapeWorksPortalAccountRegistration.png)

**Do not use the same password as for your bank account or email!**  
After registering a free account, you can log in from within the script

```
Username: joeshmoe
Password:
[1/1 MB]
joeshmoe downloaded the ellipsoid dataset from the ShapeWorks Portal.
```

## Running the Use Cases

The use cases are located at: [/Examples/Python](https://github.com/SCIInstitute/ShapeWorks/tree/master/Examples/Python). When a use case is run the data is automatically downloaded but you must make an account as described above first.

To run a use case, run the following command from the [/Examples/Python](https://github.com/SCIInstitute/ShapeWorks/tree/master/Examples/Python) directory:
            
            python RunUseCase.py --use_case [insert name of use case here]
            
        
The following use case names are currently supported:
* ellipsoid
* ellipsoid_fd
* left_atrium
* femur
            
### Optional Tags
There are optional tags to run with thie use cases.
To see the full list of optional tags, run:
          
            python RunUseCase.py --help

#### --use_subsample
To run on a subset of the data in the use case, add the --use_subsample tag followed by the number of samples you wish to use. For example:

            python RunUseCase.py --use_case [insert name of use case here] --use_subsample 4

This will select a representative sample of the specified size to run through the pipeline so that it runs faster and uses less memory. The sample is selected by running clustering, then picking one sample from each cluster so that the resulting sample is representative of all the data.

#### --interactive
When the interactive tag is used, the user must press enter after each step. This allows the user to inspect the output between steps of the pipeline.

            python RunUseCase.py --use_case [insert name of use case here] --interactive

#### --start_with_prepped_data
When this tag is used, the grooming steps are skipped. Instead of generating the distance transforms from segmentations via grooming, the distance transforms from the data .zip folder are used in optimization. If a user wishes to start with the optimization step, add --start_with_prepped_data tag.

            python RunUseCase.py --use_case [insert name of use case here] --start_with_prepped_data
           
#### --start_with_image_and_segmentation_data
Some use cases can be run on just segmentations or on segmentations plus the corresponding full image. To carry the image through the grooming proccess with the segmentation, the --start_with_image_and_segmentation_data tag must be used.

            python RunUseCase.py --use_case [insert name of use case here] --start_with_image_and_segmentation_data
            
#### --use_single_scale
Use cases can be run with multi-scale or single-scale optimization. The default mode is mutli-scale, to run single-scale use the --use_single_scale tag.

            python RunUseCase.py --use_case [insert name of use case here] --use_single_scale
           
#### --tiny_test
Users can run a fast verision of the use case by using the --tiny_test tag. This runs on a subset of the data for fewer optimization iterations to verify ShapeWorks has been properly installed. It is meant to test that use cases can run, not to create a good correspondence model.

           python RunUseCase.py --use_case [insert name of use case here] --tiny_test
           
#### shapeworks_path
This can be used to automatically add the ShapeWorks executable to the users PATH varibale if it has not already been set. To use add the Shapeworks_path tag followed by the path to the bin folder that contains ShapeWorks executables.

           python RunUseCase.py --use_case [insert name of use case here] shapeworks_path [path to shapeworks bin folder here]

### Running subsequent analysis

To run ShapeWorksStudio again without running the full pipeline, you must first navigate to the Python/Examples directory, and then run ShapeWorksStudio on the appropriate analysis xml file.  For example:

            cd /path/to/shapeworks/Examples/Python
            ShapeWorksStudio TestEllipsoids/PointFiles/analyze.xml
	    

## Dataset Guidelines
Check out [Datasets.md](Datasets.md) for dataset upload instructions and guidelines. 

