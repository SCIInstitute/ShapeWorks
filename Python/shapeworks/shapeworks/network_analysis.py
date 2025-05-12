
import numpy as np
import pandas as pd

import trimesh
import itertools
import open3d as o3d
import scipy
import spm1d
import vtk
from vtk.util.numpy_support import vtk_to_numpy

from shapeworks.utils import sw_message
from shapeworks.utils import sw_progress
from shapeworks.utils import sw_check_abort

import os, sys

import shapeworks as sw
np.random.seed(0)


class NetworkAnalysis:
    def __init__(self, project):
        self.project = project
        self.analyze = sw.Analyze(self.project)
        self.timepoints = 1
        self.activites = []
        self.flag_analysis = 'ttest'
        self.flag_model = 'cortthick'
        self.flag_run_permutations = True
        self.flag_run_cluster = True
        self.flag_run_1dspm = True
        self.pvalue_of_interest = 0.05
        self.pvalue_threshold = 0.05
        self.num_iterations = 10000

    def set_run_1dspm(self, run_1dspm):
        self.flag_run_1dspm = run_1dspm

    def set_run_permutations(self, run_permutations):
        self.flag_run_permutations = run_permutations

    def set_run_cluster(self, run_cluster):
        self.flag_run_cluster = run_cluster

    def set_target_feature(self, target_feature):
        self.activities = [ target_feature ]
        self.target_feature = target_feature

    def set_target_group(self, target_group):
        self.target_group = target_group

    def set_model_type(self, model_type):
        self.flag_model = model_type

    def set_pvalue_of_interest(self, pvalue_of_interest):
        self.pvalue_of_interest = pvalue_of_interest

    def set_pvalue_threshold(self, pvalue_threshold):
        self.pvalue_threshold = pvalue_threshold

    def set_num_iterations(self, num_iterations):
        self.num_iterations = num_iterations

    def compute_mean_shape(self):
        sw_message("Computing mean shape")
        mesh = self.analyze.get_mean_shape()
        mesh.write("mesh.vtk")

        #read in vtk mean surface and get vertices and normals
        reader = vtk.vtkPolyDataReader()
        reader.SetFileName("mesh.vtk")
        reader.Update()
        surface = reader.GetOutput()
        norms = vtk.vtkPolyDataNormals()
        norms.SetInputData(surface)
        norms.ComputePointNormalsOn()
        norms.Update()
        mesh_normals = vtk_to_numpy(surface.GetPointData().GetNormals())
        mesh_points = vtk_to_numpy(surface.GetPoints().GetData())
        mean_shape_points = self.analyze.get_mean_shape_points()
        # reshape to 3 columns
        mean_shape = mean_shape_points.reshape(-1, 3)
        return mesh_points, mesh_normals, mean_shape, surface

    def run(self):
        project = self.project
        analyze = self.analyze
        num_pts = analyze.get_num_particles()
        activities = self.activities
        flag_model = self.flag_model
        flag_analysis = self.flag_analysis
        activities = self.activities
        num_subjects = analyze.get_num_subjects()
        timepoints = self.timepoints
        mesh_points, mesh_normals, mean_shape, surface = self.compute_mean_shape()

        # #### scalar thickness dataset - each subject has particle values for a single timepoint and no activities ####
        if flag_model == 'cortthick':

            target_group = self.target_group
            print(f"Groups available in this project = {project.get_group_names()}")
            group_values = project.get_group_values(target_group)
            print(f"Group values: {group_values})")
            group = map(lambda x: x.get_group_value(target_group), project.get_subjects())
            group = np.array(list(group))
            for i in range(len(group_values)):
                group[group == group_values[i]] = i
            group = group.astype(int)
            group.mean()


            all_data = np.ndarray(
                (num_pts, timepoints, num_subjects, len(activities)));  # particles,time,subjects,activity

            activity = 0
            for s in range(num_subjects):
                features = analyze.get_subject_features(s, self.target_feature)
                # reshape into a matrix
                features = features.reshape(-1, 1)
                all_data[:, :, s, activity] = features

            # find all particles that are always non-zero
            particles = np.where(np.all((np.all((~np.any((all_data == 0), axis=1)), axis=1)), axis=1))[0]

        # #### fe model dataset - each subject has particle values for four activites ####
        elif flag_model == 'fedata':
            flag_analysis = 'anova'
            timepoints = 1
            num_pts = 768

            model_string = 'labrum_groups_withIGPL_wm_test_' + str(num_pts)
            path = basepath + 'FEModelData/'
            result_path = path + 'results_' + str(num_pts) + '/'
            project_file = pd.read_excel(path + model_string + '.xlsx')

            activities = ['ah', 'dh', 'wh', 'wm']  # within subjects factor
            filesuffix = '_labrum_wm_surface_pressure_scaled1.csv'

            subjects = []
            for file in project_file.shape_labrum:
                subjects.append((file.split('meshes/')[1]).split('_labrum')[0])

            all_data = np.ndarray(
                (num_pts, timepoints, len(subjects), len(activities)));  # particles,time,subjects,activity
            for s in range(0, len(subjects)):
                #         for a in range(0,len(activities)):
                all_data[:, 0, s, :] = np.genfromtxt(
                    path + 'particleValues_' + str(num_pts) + '/scalars_' + subjects[s] + filesuffix, delimiter=',')[
                                       1::, 4::].reshape(num_pts, 4)

            # read in mean particle files
            mean_shape = np.loadtxt(path + 'mean_' + model_string + '.particles')
            vtk_name = '_' + model_string

            # read in vtk mean surface and get vertices and normals
            reader = vtk.vtkPolyDataReader()
            reader.SetFileName(path + 'mean' + vtk_name + '.vtk')
            reader.Update()
            surface = reader.GetOutput()
            norms = vtk.vtkPolyDataNormals()
            norms.SetInputData(surface)
            norms.ComputePointNormalsOn()
            norms.Update()
            mesh_normals = vtk_to_numpy(surface.GetPointData().GetNormals())
            mesh_points = vtk_to_numpy(surface.GetPoints().GetData())

            # find all particles that are always non-zero
            particles = np.loadtxt(path + 'mean_' + model_string + '_articulating.txt').astype(int)

            # dictionary for storing data
            datadict = dict()

            # assign groups (between subjects factor)
            group = np.array(project_file.group_type)
            group_names = np.unique(group)
            for i in range(len(group_names)):
                group[group == group_names[i]] = i
            group = group.astype(int)



        # #### spatiotemporal dataset - each subject has particle values at each timepoint for two activity files ####
        elif flag_model == 'spatiotemporal':
            flag_analysis = 'anova'
            timepoints = 100
            num_pts = 2048

            model_string = str(num_pts) + '_pelvis'
            path = basepath + 'B2BDistance/'
            result_path = path + 'results_' + str(num_pts) + '/'
            project_file = pd.read_excel(path + model_string + '.xlsx')

            activities = ['incline', 'walkSS']  # within subjects factor
            filesuffix = '_particleValues_' + str(num_pts) + '.csv'

            subjects = []
            for file in project_file.shape_pelvis:
                subjects.append((file.split('meshes/')[1]).split('_trans')[0])

            all_data = np.ndarray(
                (num_pts, timepoints, len(subjects), len(activities)));  # particles,time,subjects,activity
            for s in range(0, len(subjects)):
                for a in range(0, len(activities)):
                    all_data[:, :, s, a] = np.genfromtxt(
                        path + 'particleValues/' + subjects[s] + '_' + activities[a] + filesuffix, delimiter=',')

            # read in mean particle files
            mean_shape = np.loadtxt(path + 'mean_' + model_string + '.particles')
            vtk_name = '_' + model_string

            # read in vtk mean surface and get vertices and normals
            reader = vtk.vtkPolyDataReader()
            reader.SetFileName(path + 'mean_' + model_string + '.vtk')
            reader.Update()
            surface = reader.GetOutput()
            norms = vtk.vtkPolyDataNormals()
            norms.SetInputData(surface)
            norms.ComputePointNormalsOn()
            norms.Update()
            mesh_normals = vtk_to_numpy(surface.GetPointData().GetNormals())
            mesh_points = vtk_to_numpy(surface.GetPoints().GetData())

            # find all particles that are always non-zero
            #     particles = np.where(np.all((np.all((~np.any((all_data==0),axis=1)), axis=1)), axis=1))[0]
            particles = np.loadtxt(path + 'mean_' + model_string + '_articulating.txt').astype(int)

            # dictionary for storing data
            datadict = dict()

            # assign groups (between subjects factor)
            group = np.array(project_file.group_type)
            group_names = np.unique(group)
            for i in range(len(group_names)):
                group[group == group_names[i]] = i
            group = group.astype(int)



        # In[4]:

        print(str(len(group[group == 0])), 'in', group_values[0], 'group; ', str(len(group[group == 1])), 'in',
              group_values[1], 'group')
        print(str(len(particles)), 'of', str(num_pts), 'particles analyzed.')

        # In[5]:

        # identify the closest mesh vertex for each particle
        kdtree = scipy.spatial.KDTree(mesh_points)
        dist, pts_index = kdtree.query(mean_shape[particles, :])


        # In[6]:

        pts_index.mean()

        # In[7]:

        result_path = "results/"
        # variables for SPM
        ### Alan : this was 10,000
        #    n_iter = 10000
        n_iter = 200
        poi = 0.05  # p-value of interest
        pthresh = [0.05]  # ,0.025,0.01,0.005,0.001


        n_iter = self.num_iterations
        poi = self.pvalue_of_interest
        pthresh = [self.pvalue_threshold]  # ,0.025,0.01,0.005,0.001


        subjects = project.get_subjects()
        # number of categories for analysis
        ng = len(np.unique(group))
        na = len(activities)
        ns = len(subjects)

        if os.path.exists(result_path) is not True:
            os.mkdir(result_path)

        # In[8]:

        # elif flag_analysis == 'ttest':
        grouprs = np.copy(group)
        ftests = ['data']
        fdesc = ['data']

        # In[9]:

        # if flag_run1dspm:
        # flag_analysis == 'ttest': #t-test analysis
        np.random.seed(0)

        # 1D SPM (nonparametric) mapped back onto the mean cam shape
        snpm = spm1d.stats.nonparam.ttest2(
            np.transpose(all_data[:, :, np.where(grouprs == 0)[0], :].reshape(num_pts, len(np.where(grouprs == 0)[0]))),
            np.transpose(all_data[:, :, np.where(grouprs == 1)[0], :].reshape(num_pts, len(np.where(grouprs == 1)[0]))))
        
        alpha = 0.05
        # Specified alpha must be greater than or equal to (1/nPermTotal)=0.33333
        if (alpha < 1/n_iter):
            alpha = 1/n_iter
        snpmi = snpm.inference(alpha, two_tailed=True, iterations=n_iter, force_iterations=True)  # assume one-sided

        Z = snpmi.z  # flattened test statistic  (i.e., t value) over only non-zero-variance nodes
        tradzstar = snpmi.zstar  # critical test statistic  (i.e., critical t value)

        tradzsig = Z.copy()
        tradzsig[np.abs(Z) < tradzstar] = 0

        tradzsig = tradzsig.reshape(len(particles), timepoints, len(ftests))

        # In[10]:

        # save data to file for faster access
        with open(result_path + 'tradZstar.npy', 'wb') as f:
            np.save(f, tradzstar)
        with open(result_path + 'tradZsig.npy', 'wb') as f:
            np.save(f, tradzsig)

        # In[11]:

        tradzstar

        # In[12]:

        tradzsig.mean()

        # In[13]:

        # elif flag_analysis == 'ttest': #t-test

        traddf = pd.DataFrame({'Data': [np.count_nonzero(tradzsig)]})

        # In[14]:

        traddf

        # In[15]:

        # generate connectivity between particles
        pcd = o3d.geometry.PointCloud()
        pcd.points = o3d.utility.Vector3dVector(mean_shape[particles, :])
        pcd.normals = o3d.utility.Vector3dVector(mesh_normals[pts_index, :])

        distances = pcd.compute_nearest_neighbor_distance()
        avg_dist = np.mean(distances)
        radius = avg_dist * 1.5

        mesh = o3d.geometry.TriangleMesh.create_from_point_cloud_ball_pivoting(pcd,
                                                                               o3d.utility.DoubleVector(
                                                                                   [radius, radius * 2]))
        tris = np.asarray(mesh.triangles)

        # In[16]:


        # run analysis for all permutations of data for cluster network analysis
        if self.flag_run_permutations:
            perms = np.zeros((n_iter, ns * na), dtype=int)
            F_perms = np.zeros((len(particles), timepoints, len(ftests), n_iter))

            if flag_analysis == 'anova':  # anova analysis
                if timepoints > 1:
                    # for each particle across time
                    snpmdata = np.transpose(all_data[particles, :, :, :].reshape(len(particles), timepoints, ns * na))
                    for it in range(n_iter):
                        if it == 0:
                            perms[it, :] = range(ns * na)
                        else:
                            perms[it, :] = np.random.permutation(ns * na)

                        for pt in range(len(particles)):
                            snpm_perms = spm1d.stats.nonparam.anova2onerm(snpmdata[perms[it, :], :, pt], grouprs, actrs,
                                                                          subjrs)

                            F_perms[pt, :, :, it] = np.transpose(
                                [snpm_perms['A'].z, snpm_perms['B'].z, snpm_perms['AB'].z])
                else:  # 1 timepoint
                    # all timepoints and particles
                    snpmdata = np.transpose(all_data[particles, :, :, :].reshape(len(particles) * timepoints, ns * na))
                    for it in range(n_iter):
                        if it == 0:
                            perms[it, :] = range(ns * na)
                        else:
                            perms[it, :] = np.random.permutation(ns * na)

                        # compare between activities for each particle across time
                        snpm_perms = spm1d.stats.nonparam.anova2onerm(snpmdata[perms[it, :], :], grouprs, actrs, subjrs)

                        F_perms[:, :, :, it] = np.transpose(
                            [snpm_perms['A'].z, snpm_perms['B'].z, snpm_perms['AB'].z]).reshape(
                            (len(particles), timepoints, len(ftests)))

            elif flag_analysis == 'ttest':  # t-test
                for it in range(n_iter):
                    if it == 0:
                        perms[it, :] = range(ns)
                    else:
                        perms[it, :] = np.random.permutation(ns)

                    snpm_perms = spm1d.stats.nonparam.ttest2(np.transpose(
                        all_data[:, :, perms[it, np.where(grouprs == 0)[0]], :].reshape(num_pts, len(
                            np.where(grouprs == 0)[0]))),
                                                             np.transpose(
                                                                 all_data[:, :, perms[it, np.where(grouprs == 1)[0]],
                                                                 :].reshape(num_pts, len(np.where(grouprs == 1)[0]))))

                    F_perms[:, :, :, it] = snpm_perms.z.reshape(len(particles), timepoints, len(ftests))

            with open(result_path + 'F_perms.npy', 'wb') as file:
                np.save(file, F_perms)
        else:
            with open(result_path + 'F_perms.npy', 'rb') as file:
                F_perms = np.load(file)

        # In[17]:

        # find connected test statistics, as defined as spatial connectivity via an edge or direct temporal connectivity
        if self.flag_run_cluster:
            # initialize variables
            fclust_size = np.zeros((len(ftests), len(pthresh)))
            lc_it = np.zeros((len(ftests), len(pthresh), n_iter))

            for poi in pthresh:
                if flag_analysis == 'anova':  # anova analysis
                    Fcrit = [scipy.stats.f.ppf(1 - poi, ng - 1, ns - ng),
                             scipy.stats.f.ppf(1 - poi, na - 1, (ns - 1) * (na - 1) - (na - 1) * (ng - 1)),
                             scipy.stats.f.ppf(1 - poi, (ng - 1) * (na - 1), (ns - 1) * (na - 1) - (na - 1) * (ng - 1))]
                elif flag_analysis == 'ttest':  # t-test analysis (assume one-sided)
                    Fcrit = [scipy.stats.t.ppf(1 - poi, ns - 1)]

                # for each analysis
                for f in range(len(ftests)):
                    print('Ftest: ', f, ',P value:', poi)
                    for it in range(n_iter):
                        if it % 1000 == 0:
                            print(it)
                        cnum = 1
                        all_nodes = np.zeros([len(particles), timepoints])
                        comp_size = []

                        components = [None] * timepoints
                        # for each timepoint
                        for t in range(timepoints):
                            nodes = np.where(abs(F_perms[:, t, f, it]) > Fcrit[f])[
                                0]  # find particles with signficant test statistic
                            nn = list()
                            # for each particle find respective triangle
                            for nd in nodes:
                                nodeinx = np.where(tris == nd)[0]
                                nn.extend(nodeinx)  # find tris containing significant nodes
                            nodetris = np.unique(nn)
                            # create graph (each edge of mesh)
                            if nodetris.size > 0:
                                edges = []
                                for tri in tris[nodetris, :]:
                                    edges.extend(list(itertools.combinations(tri, 2)))

                                # compute connected components and add to components list
                                components[t] = list(trimesh.graph.connected_components(edges, min_len=1,
                                                                                        nodes=nodes))  # less than three will not have a triangle
                                for component in components[t]:
                                    all_nodes[component, t] = cnum
                                    comp_size.append(len(component))
                                    cnum = cnum + 1

                        # find connected component across time by particle
                        nzind = np.nonzero(all_nodes)
                        checkcomp = []
                        duplicates = list(set([i for i in list(nzind[0]) if list(nzind[0]).count(
                            i) > 1]))  # particles that are duplicated in non-zero
                        ccdupind = []
                        for dp in duplicates:
                            nxind = np.nonzero(all_nodes[dp, :].ravel())[0]
                            sequences = np.split(nxind, np.array(np.where(np.diff(nxind) > 1)[0]) + 1)
                            for seq in sequences:
                                if len(seq) > 1:  # find components that span more than one timepoint and add to list
                                    dataadded = 0;
                                    if len(checkcomp) == 0:
                                        checkcomp.append(np.array(all_nodes[dp, seq]))
                                    else:
                                        for check in range(len(checkcomp)):
                                            if len(set.intersection(set(checkcomp[check]),
                                                                    set(all_nodes[dp, seq]))) > 0:
                                                checkcomp[check] = np.array(
                                                    np.unique(np.append(checkcomp[check], all_nodes[dp, seq]).ravel()))
                                                if dataadded == 1:
                                                    # remove duplicate entry
                                                    ccdupind.append(check)
                                                dataadded = 1
                                        if dataadded == 0:
                                            checkcomp.append(np.array(all_nodes[dp, seq]))

                        # remove duplicate components by index
                        for ccdi in reversed(np.unique(np.array(ccdupind))):
                            del checkcomp[ccdi]

                        # final check for components that overlap
                        removecc = []
                        for ind0 in range(len(checkcomp)):
                            for ind1 in range(ind0 + 1, len(checkcomp)):
                                if len(set.intersection(set(checkcomp[ind0]), set(checkcomp[ind1]))) > 0:
                                    checkcomp[ind0] = np.unique(np.append(checkcomp[ind0], checkcomp[ind1]))
                                    removecc = np.append(removecc, ind1)
                        for rcc in reversed(np.unique(removecc)):
                            del checkcomp[rcc.astype(int)]

                        # for all larger components - add the size and sum of all components
                        comp_size = np.array(comp_size)
                        connectedcomps = {}
                        evaluatedcomps = []
                        sizecount = 0
                        for cc in checkcomp:
                            sizecount = sum(comp_size[cc.astype(int) - 1]) + sizecount
                            evaluatedcomps = np.append(evaluatedcomps, cc.astype(int) - 1)
                            lc_it[f, pthresh.index(poi), it] = max(lc_it[f, pthresh.index(poi), it],
                                                                   sum(comp_size[cc.astype(int) - 1]))
                        for x in range(0, len(comp_size)):
                            if x not in evaluatedcomps:
                                sizecount = comp_size[x] + sizecount
                                lc_it[f, pthresh.index(poi), it] = max(lc_it[f, pthresh.index(poi), it], comp_size[x])
                        if sizecount != sum(comp_size):
                            print('  Total summed size', sizecount, 'compared to', sum(comp_size),
                                  len(np.nonzero(all_nodes)[0]), '!!!!!!!!!')
                            print(evaluatedcomps)
                            print(checkcomp, duplicates)

                    fclust_size[f, pthresh.index(poi)] = int(
                        np.ceil(np.quantile(lc_it[f, pthresh.index(poi), :], 1 - poi)));
                    print('    Cluster size of', fclust_size[f, pthresh.index(poi)])

            with open(result_path + 'fclust_size_iter.npy', 'wb') as file:
                np.save(file, lc_it)

            with open(result_path + 'tris.npy', 'wb') as file:
                np.save(file, tris)

            with open(result_path + 'fclust_size.npy', 'wb') as file:
                np.save(file, fclust_size)

        else:
            with open(result_path + 'fclust_size_iter.npy', 'rb') as file:
                lc_it = np.load(file)

            with open(result_path + 'fclust_size.npy', 'rb') as file:
                fclust_size = np.load(file)

            with open(result_path + 'tris.npy', 'rb') as file:
                tris = np.load(file)

        # In[18]:

        # find f-values for all timepoints
        fvalues = np.zeros([len(particles), timepoints, len(ftests)])
        fvalues_size = np.zeros([len(particles), timepoints, len(ftests), len(pthresh)])

        if flag_analysis == 'anova':  # anova analysis
            snpm = spm1d.stats.nonparam.anova2onerm(
                np.transpose(all_data[particles, :, :, :].reshape(len(particles) * timepoints, ns * na)),
                grouprs, actrs, subjrs)  # compare between activities for each particle across time
            fvalues = np.transpose([snpm['A'].z, snpm['B'].z, snpm['AB'].z]).reshape(
                (len(particles), timepoints, len(ftests)))

        elif flag_analysis == 'ttest':  # t-test analysis
            snpm = spm1d.stats.nonparam.ttest2(np.transpose(
                all_data[:, :, np.where(grouprs == 0)[0], :].reshape(num_pts, len(np.where(grouprs == 0)[0]))),
                                               np.transpose(
                                                   all_data[:, :, np.where(grouprs == 1)[0], :].reshape(num_pts, len(
                                                       np.where(grouprs == 1)[0]))))
            fvalues = snpm.z.reshape(len(particles), timepoints, len(ftests))

        # start network-based analysis
        components = [None] * timepoints
        for poi in pthresh:
            if flag_analysis == 'anova':  # anova analysis
                Fcrit = [scipy.stats.f.ppf(1 - poi, ng - 1, ns - ng),
                         scipy.stats.f.ppf(1 - poi, na - 1, (ns - 1) * (na - 1) - (na - 1) * (ng - 1)),
                         scipy.stats.f.ppf(1 - poi, (ng - 1) * (na - 1), (ns - 1) * (na - 1) - (na - 1) * (ng - 1))]
            elif flag_analysis == 'ttest':  # t-test analysis (assume one-sided)
                Fcrit = [scipy.stats.t.ppf(1 - poi, ns - 1)]

            # for each analysis
            for f in range(len(ftests)):
                print('Ftest: ', f, ',P value:', poi)
                cnum = 1
                all_nodes = np.zeros([len(particles), timepoints])
                comp_size = []

                # for each timepoint
                for t in range(timepoints):
                    nodes = np.where(abs(fvalues[:, t, f]) > Fcrit[f])[
                        0]  # find particles with signficant test statistic
                    nn = list()
                    # for each particle find respective triangle
                    for nd in nodes:
                        nodeinx = np.where(tris == nd)[0]
                        nn.extend(nodeinx)  # tris containing significant particles
                    nodetris = np.unique(nn)
                    # create graph (each edge of mesh)
                    if nodetris.size > 0:
                        edges = []
                        for tri in tris[nodetris, :]:
                            edges.extend(list(itertools.combinations(tri, 2)))

                        # compute connected components and add to components list
                        components[t] = list(trimesh.graph.connected_components(edges, min_len=1,
                                                                                nodes=nodes))  # less than three will not have a triangle
                        for component in components[t]:
                            all_nodes[component, t] = cnum
                            comp_size.append(len(component))
                            cnum = cnum + 1

                # find connected component across time by particle
                nzind = np.nonzero(all_nodes)
                uniquepairs = dict()
                checkcomp = []
                ccdupind = []
                duplicates = list(set([i for i in list(nzind[0]) if
                                       list(nzind[0]).count(i) > 1]))  # particles that are duplicated in non-zero
                for dp in duplicates:
                    nxind = np.nonzero(all_nodes[dp, :].ravel())[0]
                    sequences = np.split(nxind, np.array(np.where(np.diff(nxind) > 1)[0]) + 1)
                    for seq in sequences:
                        if len(seq) > 1:
                            dataadded = 0;
                            # add array of components to a list for reviewing (more than two adjacent times)
                            if len(checkcomp) == 0:
                                checkcomp.append(np.array(all_nodes[dp, seq]))
                            else:
                                for check in range(len(checkcomp)):
                                    if len(set.intersection(set(checkcomp[check]), set(all_nodes[dp, seq]))) > 0:
                                        checkcomp[check] = np.array(
                                            np.unique(np.append(checkcomp[check], all_nodes[dp, seq]).ravel()))
                                        if dataadded == 1:
                                            # remove duplicate entry
                                            ccdupind.append(check)
                                        dataadded = 1
                                if dataadded == 0:
                                    checkcomp.append(np.array(all_nodes[dp, seq]))

                # remove duplicate components by index
                for ccdi in reversed(np.unique(np.array(ccdupind))):
                    del checkcomp[ccdi]

                # final check for components that overlap
                removecc = []
                for ind0 in range(len(checkcomp)):
                    for ind1 in range(ind0 + 1, len(checkcomp)):
                        if len(set.intersection(set(checkcomp[ind0]), set(checkcomp[ind1]))) > 0:
                            checkcomp[ind0] = np.unique(np.append(checkcomp[ind0], checkcomp[ind1]))
                            removecc = np.append(removecc, ind1)
                for rcc in reversed(np.unique(removecc)):
                    del checkcomp[rcc.astype(int)]

                # for all larger components - add the size and sum of all components
                comp_size = np.array(comp_size)
                sig_nodes = np.zeros([len(particles), timepoints])
                connectedcomps = {}
                evaluatedcomps = []
                cccount = 0
                sizecount = 0
                for cc in checkcomp:
                    connectedcomps[cccount] = {}
                    connectedcomps[cccount]['components'] = cc

                    ccpts = []
                    starttime = 100
                    endtime = 0
                    for component in connectedcomps[cccount]['components']:
                        [pts, ti] = np.where(all_nodes == component)
                        starttime = min(starttime, min(ti))
                        endtime = max(endtime, max(ti))
                        ccpts = np.unique(np.append(pts, ccpts))
                    connectedcomps[cccount]['particles'] = ccpts
                    connectedcomps[cccount]['size'] = sum(comp_size[cc.astype(int) - 1])
                    connectedcomps[cccount]['starttime'] = starttime
                    connectedcomps[cccount]['endtime'] = endtime

                    # accumulators for checking
                    evaluatedcomps = np.append(evaluatedcomps, connectedcomps[cccount]['components'])
                    sizecount = sizecount + connectedcomps[cccount]['size']
                    cccount = cccount + 1

                for x in range(1, len(comp_size) + 1):
                    if x not in evaluatedcomps:
                        sizecount = comp_size[x - 1] + sizecount
                        connectedcomps[cccount] = {}
                        connectedcomps[cccount]['components'] = x
                        [pts, ti] = np.where(all_nodes == x)
                        connectedcomps[cccount]['particles'] = pts
                        connectedcomps[cccount]['size'] = comp_size[x - 1]
                        connectedcomps[cccount]['starttime'] = min(ti)
                        connectedcomps[cccount]['endtime'] = max(ti)
                        cccount = cccount + 1

                if sizecount != sum(comp_size):
                    print(evaluatedcomps, cccount, duplicates)
                    print('  Total summed size', sizecount, 'compared to', sum(comp_size),
                          len(np.nonzero(all_nodes)[0]), '!!!!!!!!!')

                for component in connectedcomps.keys():
                    complen = connectedcomps[component]['size']
                    if complen >= int(fclust_size[f, pthresh.index(poi)]):
                        print('    Cluster of', complen, 'nodes from time', connectedcomps[component]['starttime'],
                              'to', connectedcomps[component]['endtime'], ', pval =',
                              1 - scipy.stats.percentileofscore(lc_it[f, pthresh.index(poi), :], complen) / 100)
                        try:
                            for comp in connectedcomps[component]['components']:
                                ccloc = np.where(all_nodes == comp)
                                if complen >= int(fclust_size[f, pthresh.index(poi)]):
                                    fvalues_size[ccloc[0], ccloc[1], f, pthresh.index(poi)] = fvalues[
                                        ccloc[0], ccloc[1], f]
                        except:
                            comp = connectedcomps[component]['components']
                            ccloc = np.where(all_nodes == comp)
                            print('  ', comp, len(ccloc[0]))
                            if complen >= int(fclust_size[f, pthresh.index(poi)]):
                                fvalues_size[ccloc[0], ccloc[1], f, pthresh.index(poi)] = fvalues[ccloc[0], ccloc[1], f]
                    else:
                        print('      Insignificant cluster of', complen, 'nodes from time',
                              connectedcomps[component]['starttime'], 'to', connectedcomps[component]['endtime'],
                              ', pval =',
                              1 - scipy.stats.percentileofscore(lc_it[f, pthresh.index(poi), :], complen) / 100)

        with open(result_path + 'fvalues.npy', 'wb') as file:
            np.save(file, fvalues)

        with open(result_path + 'fvalues_size.npy', 'wb') as file:
            np.save(file, fvalues_size)

        # In[19]:

        poi = 0.05  # p-value of interest
        # save data to data frame
        if flag_model == 'spatiotemporal':  # temporal anova analysis
            clustdf_size = pd.DataFrame({
                'Group': np.count_nonzero(fvalues_size[:, :, 0, pthresh.index(poi)], axis=0),
                'Activity': np.count_nonzero(fvalues_size[:, :, 1, pthresh.index(poi)], axis=0),
                'Group*Activity': np.count_nonzero(fvalues_size[:, :, 2, pthresh.index(poi)], axis=0)})

            combidf = pd.concat([traddf, clustdf_size], axis=1)
            combidf.columns = ['1D Group', '1D Activity', '1D Group*Activity', 'ST Group', 'ST Activity',
                               'ST Group*Activity']

        elif flag_model == 'fedata':  # anova
            clustdf_size = pd.DataFrame({
                'Group': np.count_nonzero(fvalues_size[:, :, 0, pthresh.index(poi)], axis=0),
                'Activity': np.count_nonzero(fvalues_size[:, :, 1, pthresh.index(poi)], axis=0),
                'Group*Activity': np.count_nonzero(fvalues_size[:, :, 2, pthresh.index(poi)], axis=0)})
            print('Group - Traditional SPM: ', traddf['Group'][0], ', Correspondence Network: ',
                  clustdf_size['Group'][0])
            print('Activity - Traditional SPM: ', traddf['Activity'][0], ', Correspondence Network: ',
                  clustdf_size['Activity'][0])
            print('Group*Activity - Traditional SPM: ', traddf['Group*Activity'][0], ', Correspondence Network: ',
                  clustdf_size['Group*Activity'][0])
            print(len(particles), 'particles analyzed.')

        elif flag_model == 'cortthick':  # t-test
            clustdf_size = pd.DataFrame({'Data': [np.count_nonzero(fvalues_size[:, :, :, pthresh.index(poi)])]})
            print('Traditional SPM: ', traddf['Data'][0], ', Correspondence Network: ', clustdf_size['Data'][0])

        # In[20]:

        # write new data to text file for figure generation
        particle_fvalues_size = np.zeros([num_pts, timepoints, len(fdesc)])
        particle_fvalues_1d = np.zeros([num_pts, timepoints, len(fdesc)])
        if flag_model == 'spatiotemporal':
            particle_data = np.ones(all_data.shape) * 10
            particle_data[particles, :, :, :] = all_data[particles, :, :, :]
        elif flag_model == 'fedata':
            particle_data = np.zeros(all_data.shape)
            particle_data[particles, :, :, :] = all_data[particles, :, :, :]

        headers = ['x', 'y', 'z']
        for i in range(timepoints):
            headers.append(str(i))

        if flag_analysis == 'anova':  # anova analysis
            particle_fvalues_size[particles, :, :] = fvalues_size[:, :, :, pthresh.index(poi)]
            particle_fvalues_1d[particles, :, :] = tradzsig
            for analysis in ['1D', 'network']:
                for comparison in fdesc:
                    if analysis == '1D':
                        pointsdata = np.concatenate([mean_shape,
                                                     particle_fvalues_1d[:, :, fdesc.index(comparison)].reshape(num_pts,
                                                                                                                timepoints)],
                                                    axis=1)
                        ptsdata = np.concatenate([mean_shape[particles, :], tradzsig[:, :, fdesc.index(comparison)]],
                                                 axis=1)
                    elif analysis == 'network':
                        pointsdata = np.concatenate([mean_shape,
                                                     particle_fvalues_size[:, :, fdesc.index(comparison)].reshape(
                                                         num_pts, timepoints)], axis=1)
                        ptsdata = np.concatenate([mean_shape[particles, :],
                                                  fvalues_size[:, :, fdesc.index(comparison)].reshape(len(particles),
                                                                                                      timepoints)],
                                                 axis=1)
                    np.savetxt(result_path + 'Points_' + analysis + comparison + '_' + str(num_pts) + '.txt',
                               pointsdata, delimiter='\t', header='\t'.join(headers), comments='')
                    np.savetxt(result_path + 'Particles_' + analysis + comparison + '_' + str(num_pts) + '.txt',
                               ptsdata, delimiter='\t', header='\t'.join(headers), comments='')

            for a in range(len(activities)):
                g0data = np.concatenate([mean_shape, np.mean(particle_data[:, :, group == 0, a], axis=2)], axis=1)
                np.savetxt(result_path + 'G0_' + activities[a] + '_' + str(num_pts) + '.txt', g0data, delimiter='\t',
                           header='\t'.join(headers), comments='')

                g1data = np.concatenate([mean_shape, np.mean(particle_data[:, :, group == 1, a], axis=2)], axis=1)
                np.savetxt(result_path + 'G1_' + activities[a] + '_' + str(num_pts) + '.txt', g1data, delimiter='\t',
                           header='\t'.join(headers), comments='')

                actdata = np.concatenate([mean_shape, np.mean(particle_data[:, :, :, a], axis=2)], axis=1)
                np.savetxt(result_path + activities[a] + '_' + str(num_pts) + '.txt', g1data, delimiter='\t',
                           header='\t'.join(headers), comments='')
                if flag_model == 'fedata':  # control = 0, dysplasia = 1
                    diffdata = np.concatenate([mean_shape, np.mean(particle_data[:, :, group == 1, a], axis=2) -
                                               np.mean(particle_data[:, :, group == 0, a], axis=2)], axis=1)
                elif flag_model == 'spatiotemporal':
                    diffdata = np.concatenate([mean_shape, np.mean(particle_data[:, :, group == 0, a], axis=2) -
                                               np.mean(particle_data[:, :, group == 1, a], axis=2)], axis=1)
                np.savetxt(result_path + 'Diff_' + activities[a] + '_' + str(num_pts) + '.txt', diffdata,
                           delimiter='\t', header='\t'.join(headers), comments='')

                stddata = np.concatenate([mean_shape, np.std(particle_data[:, :, :, a], axis=2)], axis=1)
                np.savetxt(result_path + 'StDev_' + activities[a] + '_' + str(num_pts) + '.txt', stddata,
                           delimiter='\t', header='\t'.join(headers), comments='')

            if flag_model == 'spatiotemporal':
                diffdata = np.concatenate([mean_shape, np.mean(particle_data[:, :, :, 0], axis=2) -
                                           np.mean(particle_data[:, :, :, 1], axis=2)], axis=1)
                np.savetxt(result_path + 'ActivityDiff_' + str(num_pts) + '.txt', diffdata, delimiter='\t',
                           header='\t'.join(headers), comments='')

                stddata = np.concatenate(
                    [mean_shape, np.std(particle_data.reshape(num_pts, timepoints, ns * na), axis=2)], axis=1)
                np.savetxt(result_path + 'StDev_' + str(num_pts) + '.txt', stddata, delimiter='\t',
                           header='\t'.join(headers), comments='')

                for g in range(ng):
                    diffdata = np.concatenate([mean_shape, np.mean(particle_data[:, :, group == g, 0], axis=2) -
                                               np.mean(particle_data[:, :, group == g, 1], axis=2)], axis=1)
                    np.savetxt(result_path + 'G' + str(g) + '_ActivityDiff_' + str(num_pts) + '.txt', diffdata,
                               delimiter='\t', header='\t'.join(headers), comments='')

                    stddata = np.concatenate([mean_shape, np.std(
                        particle_data[:, :, group == g, :].reshape(num_pts, timepoints, len(group[group == g]) * na),
                        axis=2)], axis=1)
                    np.savetxt(result_path + 'G' + str(g) + '_StDev_' + str(num_pts) + '.txt', stddata, delimiter='\t',
                               header='\t'.join(headers), comments='')



        elif flag_analysis == 'ttest':
            particle_fvalues_size[particles.astype(int), :, :] = fvalues_size[:, :, :, pthresh.index(poi)]
            particle_fvalues_1d[particles.astype(int), :, :] = tradzsig.reshape(len(particles), timepoints, len(fdesc))
            self.particle_network_values = particle_fvalues_size[:,0,0]
            self.particle_spm_values = particle_fvalues_1d[:,0,0]
            for analysis in ['1D', 'network']:
                for comparison in fdesc:
                    if analysis == '1D':
                        pointsdata = np.concatenate(
                            [mean_shape, particle_fvalues_1d[:, :, :].reshape(len(particles), 1)], axis=1)
                    elif analysis == 'network':
                        pointsdata = np.concatenate(
                            [mean_shape, particle_fvalues_size[:, :, :].reshape(len(particles), 1)], axis=1)
                    np.savetxt(result_path + 'Points_' + analysis + comparison + '_' + str(num_pts) + '.txt',
                               pointsdata, delimiter='\t', header='\t'.join(headers), comments='')


            g0data = np.concatenate(
                [mean_shape, np.mean(all_data[:, :, group == 0, :], axis=2).reshape(len(particles), 1)], axis=1)
            np.savetxt(result_path + 'G0_' + str(num_pts) + '.txt', g0data, delimiter='\t', header='\t'.join(headers),
                       comments='')

            g1data = np.concatenate(
                [mean_shape, np.mean(all_data[:, :, group == 1, :], axis=2).reshape(len(particles), 1)], axis=1)
            np.savetxt(result_path + 'G1_' + str(num_pts) + '.txt', g1data, delimiter='\t', header='\t'.join(headers),
                       comments='')

            diffdata = np.concatenate(
                [mean_shape, np.mean(all_data[:, :, group == 0, :], axis=2).reshape(len(particles), 1) -
                 np.mean(all_data[:, :, group == 1, :], axis=2).reshape(len(particles), 1)], axis=1)
            np.savetxt(result_path + 'Diff_' + str(num_pts) + '.txt', diffdata, delimiter='\t',
                       header='\t'.join(headers), comments='')

            stddata = np.concatenate([mean_shape, np.std(all_data, axis=2).reshape(len(particles), 1)], axis=1)
            np.savetxt(result_path + 'StDev_' + str(num_pts) + '.txt', stddata, delimiter='\t',
                       header='\t'.join(headers), comments='')

        # In[21]:

        if flag_model == 'cortthick':
            print('Traditional SPM: ', traddf['Data'][0], ', Correspondence Network: ', clustdf_size['Data'][0],
                  'with min size:',
                  fclust_size[f, pthresh.index(poi)].astype(int))

        # In[21]:

        self.particles = particles
        self.group = group
        self.pts_index = pts_index
