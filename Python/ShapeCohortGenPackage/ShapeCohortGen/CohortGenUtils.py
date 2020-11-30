import os

'''
Make folder
'''
def make_dir(dirPath):
    if not os.path.exists(dirPath):
        os.makedirs(dirPath)

'''
Get list of full paths for files in dir
'''
def get_files(folder):
	file_list = []
	for file in os.listdir(folder):
		file_path = folder + file
		file_path = file_path.replace(" ","")
		file_list.append(file_path)
	file_list = sorted(file_list)
	return file_list