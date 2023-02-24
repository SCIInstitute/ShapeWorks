import pandas as pd
from shutil import rmtree
from pathlib import Path
import glob 
from swcc.api import swcc_session
from swcc.models import Dataset
from swcc.models import (Dataset, Project)
import getpass
from pathlib import Path
from swcc.api import swcc_session
from swcc.models import Dataset, Project

def get_text(filename):
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
            df["image_file"] = image_files
    writer = pd.ExcelWriter(filename, engine ="xlsxwriter")
    df.to_excel(writer,sheet_name="data",index=False)
    writer.save()

def upload_dataset(dataset_name,license_filename,ack_filename,description,project_file,overwrite=True):
    username = input("Username: ")
    password = getpass.getpass("Password: ")
    
    upload_dir = 'uploads'
    with swcc_session() as session:
        token = session.login(username, password)
        session = swcc_session(token=token).__enter__()

        print(f'Uploading {dataset_name} dataset and project (overwrite=True)')
        dataset = Dataset.from_name(dataset_name)
        if overwrite:
            if dataset:
                print('Deleting previous version of %s' % dataset_name)
                dataset.delete()
        if not dataset:
            dataset = Dataset(
                name=dataset_name,
                # private=private,
                description=description,
                license=get_text(license_filename),
                acknowledgement=get_text(ack_filename),
            ).force_create()
        project_file = Path(upload_dir, project_file)
        project = Project(
            file=project_file,
            description='Project created via SWCC',
            dataset=dataset,
        ).create()
        print(project)

        print('Done. \n')
