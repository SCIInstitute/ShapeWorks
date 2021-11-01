# How to Add New Use Cases?

Adding a new use case entails the following.

- Python-scripted workflow
- Use case dataset
- Use case documentation 


## Python-scripted workflow

To add a new use case to the codebase: 

- Add `<use-case-name>.py` python script to `Examples/Python/`, where the `<use-case-name>` is a descriptive name for the use case, typically associated with the class of shapes to be modeled (e.g., anatomy name)
- Update `Examples/Python/RunUseCase.py` to add the new use case to the list of cases. Look for the line `parser.add_argument("--use_case" ...)`. Make sure to use the `<use-case-name>` to the use case list.
- Each use case needs to include the option to run a tiny test, which typically runs on only the first three shapes. 
- Each use case needs to have the option to run on a representative subsample of the dataset of any size. This subsample is selected via clustering on the shapes and selecting one shape from each cluster. 


## Use case dataset

To add the dataset associated with the new use case:

- Organize the dataset to adhere to the [Dataset Requirements](../dev/datasets.md#dataset-requirements).
- Upload the datset to the [ShapeWorks Data Portal](https://girder.shapeworks-cloud.org/). Visit [Uploading a Dataset](../dev/datasets.md#uploading-a-dataset).

## Use case documentation 
 
To add the documentation of the new dataset, add a markdown file `<use-case-name>.md` in `docs/use-cases` with the following structure. Please use the [Naming and Organization](../dev/docs.md#naming-and-organization) conventions currently deployed for ShapeWorks documentation. Then, add a corresponding entry in `mkdocs.yml`.

!!! important
    If you use the documentation of an existing use case as a starting point, PLEASE make sure to adapt the documentation to your new use case. 

!!! important 
    PLEASE, use a spell and grammar checker (e.g., [grammarly](https://app.grammarly.com)).

- **What is the Use Case?**: The goal of this use case, i.e., what does this use case demonstrate about ShapeWorks? Details about the dataset, e.g., number of samples, class of shapes, shape representation (meshes, segmentation, or contours), dataset source ... etc.
- **Grooming Steps**: Details about the data preprocessing pipeline for this use case that highlights specific considerations for this use case. Snapshots of intermediate and final groom outputs for illustration.
- **Supported Tags**: List of tags that can be used with the use case.
- **Optimization Parameters**: Default optimization parameters used for this use case, including a discussion of intuitions/insights on how these parameters were chosen.
- **Analyzing Shape Model**: Videos and snapshots of the optimized shape model(s), including the mean shape, scree plot, modes of variations, and individual samples. When applicable, also include videos and snapshots of results that are specific to the use case (e.g., group differences, linear discriminant analysis ... etc.).
