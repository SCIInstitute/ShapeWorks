import os
import shapeworks as sw
import json
from Constants import *

class Project_SSM:
    def __init__(self, project_name, opt_params=None) -> None:
        self.meshes_dir = f'{PROJECT_DIR}/old-meshes/'
        self.groomed_meshes_dir = f'{PROJECT_DIR}/old-meshes/'
        DIRS = [self.meshes_dir, self.groomed_meshes_dir]
        self.shape_models_dir = f'{PROJECT_DIR}/shape_models/'
        self.project_name = project_name
        if opt_params is None:
            self.opt_params = DEFAULT_OPT_PARAMS
        else:
            self.opt_params = opt_params
        make_dirs(DIRS)
        self.shapes_info_file = f'{self.meshes_dir}/description.json'
        self.shapes_info_dict = {}
        if not os.path.exists(self.shapes_info_file):
            self.shapes_info_dict = build_shapes_info_json(self.meshes_dir)
        else:
            with open(self.shapes_info_file) as json_file:
                self.shapes_info_dict = json.load(json_file)

    def create_sw_project_file(self):
        # All shapes are groomed and pre-aligned
        project_location = self.shape_models_dir
        project_subjects = []
        # print(self.shapes_info_dict["ALL_SUBJECTS"])
        # Add all Pre Time Points
        for idx, subj in enumerate(self.shapes_info_dict["ALL_SUBJECTS"]):
            subject = sw.Subject()
            subject.set_number_of_domains(NUM_TIME_POINTS)
            rel_mesh_files = []
            rel_groom_files = []
            for mesh_file in self.shapes_info_dict[subj]['PRE_ABLATION_TIME_POINTS']:
                rel_mesh_files += sw.utils.get_relative_paths([mesh_file], project_location)
                rel_groom_files += sw.utils.get_relative_paths([mesh_file], project_location)
            sub_group_dict = {"preAblation": "yes", "postAblation":"no"}
            subject.set_group_values(sub_group_dict)
            subject.set_groomed_filenames(rel_groom_files)
            subject.set_original_filenames(rel_mesh_files)
            project_subjects.append(subject)
        # Add all Post Time Points
        for idx, subj in enumerate(self.shapes_info_dict["ALL_SUBJECTS"]):
            subject = sw.Subject()
            subject.set_number_of_domains(NUM_TIME_POINTS)
            rel_mesh_files = []
            rel_groom_files = []
            for mesh_file in self.shapes_info_dict[subj]['POST_ABLATION_TIME_POINTS']:
                rel_mesh_files += sw.utils.get_relative_paths([mesh_file], project_location)
                rel_groom_files += sw.utils.get_relative_paths([mesh_file], project_location)
            sub_group_dict = {"preAblation": "no", "postAblation":"yes"}
            subject.set_group_values(sub_group_dict)
            subject.set_groomed_filenames(rel_groom_files)
            subject.set_original_filenames(rel_mesh_files)
            project_subjects.append(subject)
        # Set project
        shapeworks_project = sw.Project()
        shapeworks_project.set_subjects(project_subjects)
        parameters = sw.Parameters()
        for key in self.opt_params:
            parameters.set(key, sw.Variant([self.opt_params[key]]))
        parameters.set("domain_type", sw.Variant('mesh'))
        shapeworks_project.set_parameters("optimize", parameters)
        self.project_file_path = f"{self.shape_models_dir}/{self.project_name}.xlsx"
        shapeworks_project.save(self.project_file_path)
        print(f"----------- Base Model ShapeWorks Project saved at {self.project_file_path} ----------")
        # self.run_optimize(self.project_file_path)
