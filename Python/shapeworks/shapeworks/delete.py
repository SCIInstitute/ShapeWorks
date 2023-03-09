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


username = input('Enter username: ')
password = getpass.getpass('Enter password: ')
with swcc_session()  as session:
    token = session.login(username, password)
    session = swcc_session(token=token).__enter__()

    # username = input('Enter username: ')
    # password = getpass.getpass('Enter password: ')
    print('Authenticated with running server.')
    

    print('Uploading demo dataset and project.')
    for dataset_name in ["lumps"]:
        dataset = Dataset.from_name(dataset_name)
        if dataset:
                print('Deleting previous version of %s' % dataset_name)
                dataset.delete()

        print('Done.')
