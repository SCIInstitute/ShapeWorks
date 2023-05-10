# ShapeWorks Cloud Client (SWCC)

**Note**: This documentation version could be outdated. [Click here](http://sciinstitute.github.io/ShapeWorks/dev/cloud/swcc.html) to get the most up-to-date version.

The ShapeWorks Cloud Client (SWCC) is a Python module that acts as an interface for sending requests to an active ShapeWorks Cloud server. The average use cases for this module include downloading data for custom analysis, uploading new datasets, and creating new projects.

Currently, this python module is the only way to upload datasets to ShapeWorks Cloud, since this action should only be performed by trusted and informed users.

## Authentication & Sessions

This library includes a session class called `SwccSession` which handles authentication to a live ShapeWorks Cloud server. This class is context-managed, so you can open a session in a python `with` block, and it will be closed for you after the block is complete. Within the live context, you can provide your credentials to the `login` method and receive a token. You do not need to do anything with the token, but it is available to you. After the login statement, you may begin sending requests to the server with various other methods (see following sections).

Below is an example:

```python
from swcc.api import swcc_session

with swcc_session() as session:
    token = session.login(username, password)
    # begin sending requests

# automatically logged out
```

## Datasets and Projects

The primary use cases for the python client include uploading and downloading Datasets and Projects. It is not likely that you will need to manipulate other classes directly, since Datasets and Projects manage subsequent classes for you.

### Datasets

The `Dataset` class has the following attributes, for which you can provide values either in the constructor or by setting them directly: `name, file, license, description, acknowledgement, keywords, contributors, publications`. It also has properties which may be read from the object: `subjects, projects, segmentations, meshes, contours, images, landmarks, constraints`.

The `Dataset` class has the following methods:

1. `download(output_path)`: Download all files related to subsequent objects including Segmentations, Meshes, Images, etc.
2. `create()`: Save the state of the python object as a database object on the server
3. `force_create()`: Save the object to the database, overwriting it if it already exists
4. `from_name(name)`: Returns an iterator of all Dataset objects in the server database that match the provided name string
5. `add_project(file, keywords, description)`: A shortcut for creating a Project object associated with this Dataset, without using the Project constructor directly
6. `add_subject(subject_name)`: A shortcut for creating a Subject object associated with this Dataset, without using the Subject constructor directly

Below is an example.

```python
my_dataset = Dataset(name=”My Dataset”, description=”This is my first dataset object”)
my_dataset.license = license_file.read()
my_dataset.acknowledgement = acknowledgement_file.read()
my_dataset.add_subject(“Subject One”)
my_dataset.add_project(project_file, “Keywords”, “First Project for My Dataset”)
my_dataset.create()
my_dataset.force_create()  # has no effect
print(my_dataset.id) # after saving, the object has an id
print(my_dataset.meshes)

other_dataset = Dataset.from_name(“Other”)
other_dataset.download(“/tmp”)
```

### Projects

The `Project` class has the following attributes, for which you can provide values either in the constructor or by setting them directly: `file, keywords, description, dataset, last_cached_analysis`. It also has properties which may be read from the object: `groomed_segmentations, groomed_meshes, particles`.

The `Project` class has the following methods:

1. `download(output_path)`: Download all files related to subsequent objects including Subjects, Segmentations, Meshes, Images, Groomed Segmentations, Groomed Meshes, Particles, etc.
2. `create()`: Save the state of the python object as a database object on the server and interpret the contents of the file saved to the “file” attribute as subsequent objects

#### The Project File

The file saved to a Project encodes the project’s relationship to all subsequent objects, and it should be formatted properly so that the `create` method can correctly interpret the contents. The Project file is a JSON-type file with the extension “.swproj”. The contents of the file should reference the paths of relevant files via absolute or relative paths. Be sure that these files are accessible to the script you run.

## Other Classes and Methods

As mentioned above, the most common use cases of the SWCC API involve using the Dataset and Project classes, which will handle their subsequent objects. In a rarer use case, one might need to manipulate one of the subsequent objects directly.

The Project and Dataset classes inherit from a class called `APIModel`. The following other classes exist and also inherit from this model:

1. Segmentation
2. Mesh
3. Contour
4. Image
5. GroomedSegmentation
6. GroomedMesh
7. OptimizedParticles
8. Landmarks
9. Constraints
10. CachedAnalysisGroup
11. CachedAnalysisModePCA
12. CachedAnalysisMode
13. CachedAnalysis

Each of these classes has the following methods:

1. `from_id(id)`: Class method. Fetch an existing object of this type from the server that matches the provided id
2. `list()`: Class method. Return a list of all existing objects of this type from the server
3. `delete()`: Delete an object from the database
4. `create()`: Save an object to the database

## Examples

```python
def upload_dataset(dataset_name, description, project_file, private=True):
   username = input("Username: ")
   password = getpass("Password: ")


   with swcc_session(base_url="[shapeworks server url]/api/v1") as session:
       token = session.login(username, password)
       session = swcc_session(token=token).__enter__()


       print(f'Uploading {dataset_name} dataset and project (overwrite=True)')
       dataset = Dataset(
           name=dataset_name,
           private=private,
           description=description,
           license='No license',
           acknowledgement='No acknowledgement',
       ).force_create()
       project_file = Path(upload_dir, project_file)
       project = Project(
           file=project_file,
           description='Project created via SWCC',
           dataset=dataset,
           # last_cached_analysis="project_demo_analysis.json",
       ).create()
       print(project)

       print('Done. \n')

def download_project(dataset_name, folder_name):
   username = input("Username: ")
   password = getpass("Password: ")


   with swcc_session(base_url="[shapeworks server url]/api/v1") as session:
       token = session.login(username, password)
       session = swcc_session(token=token).__enter__()


       print(f'Downloading {dataset_name} dataset and project.')
       dataset = Dataset.from_name(dataset_name)
       for project in dataset.projects:
           project.download(Path(download_dir, folder_name))

       print('Done. \n')
```
