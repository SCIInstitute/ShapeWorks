import pandas as pd
from shutil import rmtree
from pathlib import Path
import glob 
from swcc.api import swcc_session
from swcc.models import Dataset
from swcc.models import (Dataset, GroomedSegmentation, OptimizedParticles,OptimizedShapeModel, Project, Segmentation, Subject)
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






# # For development, it is possible to generate a session outside of a context manager.
# ctx = swcc_session(token=token)
# session = ctx.__enter__()


# print("Printing the datasets")

# for d in Dataset.list(): 
#     print(d.name,d.id,d.description)

 
# old = Dataset.from_name('ellipsoid_with_mesh')
# # Either get rid of an existing dataset with the same name, or create a new name for testing

# if old:
#   print('Deleting previous version of %s' % 'lefT_atrium_test_4')
#   old.delete()


# print(get_license("left_atrium-v0/License.txt"))
def upload_dataset(dataset_dir,dataset_name,shape_dir,shape_ext,license_filename,description,image_dir=None,image_ext=None,force_create=False):

    license  = get_license(license_filename)

    
    shape_dir = dataset_dir + shape_dir
    shape_file_list = get_file_column(shape_dir,".nrrd")
    if image_dir is not None:
        image_dir = dataset_dir + image_dir
        image_file_list = get_file_column(image_dir, ".nrrd")
    else:
        image_file_list = None

    create_data_file(dataset_name+".xlsx",shape_file_list,image_file_list)




    dataset = Dataset(
            name=dataset_name,
            license=license,
            description=description,
            acknowledgement='NIH')
    if(force_create==False):
        dataset.create()
    else:
        dataset.force_create()
    data = dataset.load_data_spreadsheet(Path(dataset_name+".xlsx"))
#api as a context manager
with swcc_session()  as session:
    token = session.login('username', 'password')
    session = swcc_session(token=token).__enter__()
    print([(d.id, d.name) for d in Dataset.list()])

    dataset_dir = "left_atrium-v0/"
    dataset_name = "left_atrium"
    description = "left atrium dataset"

    upload_dataset(dataset_dir,dataset_name,"segmentations/",".nrrd",dataset_dir+"/License.txt",description,"images/",".nrrd",True)