import pandas as pd
from shutil import rmtree
from pathlib import Path
import glob 
from swcc.api import swcc_session
from swcc.models import Dataset
import shapeworks as sw
from swcc.models import (Dataset, GroomedSegmentation, OptimizedParticles, Project, Segmentation, Subject)
import getpass
from pathlib import Path
from swcc.api import swcc_session
from swcc.models import Dataset, Project
def get_license(filename):
    license = Path(filename).read_text()
    return license

def get_file_column(folder,extension):
    filelist = glob.glob(folder+"/*"+extension)

    return filelist

def create_data_file(filename,shape_files,image_files=None):
    df = pd.DataFrame()
    if shape_files is not None:
        df["shape_file"] = shape_files
        if image_files is not None:
            print(len(shape_files),len(image_files))
            # assert len(shape_files)!=len(image_files),"number of shape files don't match the number of image files"
            df["image_file"] = image_files
    writer = pd.ExcelWriter(filename, engine ="xlsxwriter")
    df.to_excel(writer,sheet_name="data",index=False)
    writer.save()

def upload_dataset(dataset_name,license_filename,description,project_file):
    # print("in upload dataset")
    # username,password = sw.portal.login()

    username = input('Enter username: ')
    password = getpass.getpass('Enter password: ')
    with swcc_session(base_url="http://localhost:8000/api/v1")  as session:
        token = session.login(username, password)
        session = swcc_session(token=token).__enter__()

        # username = input('Enter username: ')
        # password = getpass.getpass('Enter password: ')
        print('Authenticated with running server.')
        

        print('Uploading demo dataset and project.')
        dataset = Dataset.from_name(dataset_name)
        # if dataset:
        #     print('Deleting previous version of %s' % dataset_name)
            # dataset.delete()
        # import pdb;pdb.set_trace()
        if not dataset:
            dataset = Dataset(
                name=dataset_name,
                description=description,
                license=get_license(license_filename),
                acknowledgement='No acknowledgement',
            ).force_create()
        project_file = Path(project_file)
        project = Project(
            file=project_file,
            description=description,
            dataset=dataset,
            # last_cached_analysis="project_demo_analysis.json",
        ).create()

        print('Done.')

if __name__=="__main__":
    # dataset_names = ["ellipsoid_multiscale","ellipsoid_fd","ellipsoid_fd_tiny_test","ellipsoid_mesh","ellipsoid_mesh_tiny_test","ellipsoid_multiple_domain_singlescale","ellipsoid_multiple_domain_singlescale_tiny_test","ellipsoid_mesh_multiple_domain_singlescale","ellipsoid_mesh_multiple_domain_singlescale_tiny_test"]
    # project_files = ["/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/ellipsoid/shape_models/ellipsoid_multiscale.swproj",
    #                 "/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/ellipsoid_fd/shape_models/ellipsoid_fd_multiscale.swproj",
    #                 "/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/ellipsoid_fd/shape_models/ellipsoid_fd_tiny_test_multiscale.swproj",
    #                 "/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/ellipsoid_mesh/shape_models/ellipsoid_mesh_multiscale.swproj",
    #                 "/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/ellipsoid_mesh/shape_models/ellipsoid_mesh_tiny_test_multiscale.swproj",
    #                 "/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/ellipsoid_multiple_domain/shape_models/ellipsoid_multiple_domain_singlescale.swproj",
    #                 "/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/ellipsoid_multiple_domain/shape_models/ellipsoid_multiple_domain_tiny_test_singlescale.swproj",
    #                 "/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/ellipsoid_multiple_domain_mesh/shape_models/ellipsoid_multiple_domain_mesh_singlescale.swproj",
    #                 "/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/ellipsoid_multiple_domain_mesh/shape_models/ellipsoid_multiple_domain_mesh_tiny_test_singlescale.swproj"]
    # descriptions = ["Uploading ellipsoid multiscale dataset",
    #                 "Uploading ellipsoid fd dataset",
    #                 "Uploading ellipsoid fd tiny test dataset",
    #                 "Uploading ellipsoid mesh dataset",
    #                 "Uploading ellipsoid mesh tiny test dataset",]
    #                 "Uploading ellipsoid multiple domain singlescale dataset",
    #                 "Uploading ellipsoid multiple domain singlescale tiny test dataset",
    #                 "Uploading ellipsoid mesh multiple domain singlescale dataset",
    #                 "Uploading ellipsoid mesh multiple domain singlescale tiny test dataset",
    # dataset_names = ["lumps","lumps_tiny_test"]
    # # left_atrium_tiny_test - "/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/left_atrium/shape_models/left_atrium_tiny_test_multiscale_particles",
    # project_files = [
    # "/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/lumps/shape_models/lumps_multiscale.swproj",
    # "/home/sci/mkaranam/Desktop/ShapeWorks/Examples/Python/Output/lumps/shape_models/lumps_tiny_test_multiscale.swproj"]
    # descriptions = ["Uploading lumps dataset",
    #                 "Uploading lumps tiny test dataset"]

    upload_dataset(
        "lumps",
        "/home/sci/mkaranam/Desktop/ShapeWorks/LICENSE.txt",
        descriptions[i],
        project_files[i]
        )