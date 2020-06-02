
# Use Cases

- [Downloading Datasets](#Downloading-Datasets)
- [Running Use Cases](#Running-the-Use-Cases)
  - [Ellipsoid](UseCases/Ellipsoid.md)
  - [Fixed Domains Ellipsoid](UseCases/FixedDomainEllipsoid.md)
  - [Left Atrium](UseCases/LeftAtrium.md)
  - [Femur](UseCases/Femur.md)
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

![ShapeWorks Portal Account Registration Screenshot](images/ShapeWorksPortalAccountRegistration.png)

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

To see the full list of optional tags, run:
          
            python RunUseCase.py --help
            
### Running a Subset

To run on a subset of the data in the use case, add the --use_subsample tag followed by the number of samples you wish to use. For example:

            python RunUseCase.py --use_case [insert name of use case here] --use_subsample 4

This will select a representative sample of the specified size to run through the pipeline so that it runs faster and uses less memory. The sample is selected by running k-mean clustering, then picking one sample from each cluster so that the resulting sample is representative of all the data.

## Dataset Guidelines
Check out [Datasets.md](Datasets.md) for dataset upload instructions and guidelines. 

