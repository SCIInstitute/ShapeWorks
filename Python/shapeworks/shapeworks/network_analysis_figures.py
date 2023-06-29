#!/usr/bin/env python
# coding: utf-8

# In[1]:


import numpy as np
import pandas as pd

import trimesh
# import itertools
# import operator
import open3d as o3d
import scipy
# import spm1d
import vtk
from vtk.util.numpy_support import vtk_to_numpy
import os

#for images
import plotly.express as px
import matplotlib.pyplot as plt
import seaborn as sns
import vedo
import cv2

class NetworkAnalysisFigures:
    def __init__(self, network_analysis):
        self.network_analysis = network_analysis

    def run(self):
        # In[4]:

        analyze = self.network_analysis.analyze
        flag_model = self.network_analysis.flag_model
        group = self.network_analysis.group
        activities = self.network_analysis.activities
        project = self.network_analysis.project
        subjects = project.get_subjects()
        result_path = "results/"
        flag_analysis = self.network_analysis.flag_analysis
        particles = self.network_analysis.particles
        pts_index = self.network_analysis.pts_index
        timepoints = self.network_analysis.timepoints
        num_pts = analyze.get_num_particles()

        #variables for SPM
        n_iter = 10000
        poi = 0.05
        pthresh = [0.05]#,0.025,0.01,0.005,0.001

        #number of categories for anova analysis
        ng = len(np.unique(group))
        na = len(activities)
        ns = len(subjects)

        if os.path.exists(result_path) is not True:
            os.mkdir(result_path)

        if flag_analysis == 'anova': #anova analysis
            #variables for anova analysis

            subjrs = np.repeat(range(0,len(subjects)),len(activities))
            grouprs = np.repeat(group,len(activities))
            actrs = np.tile(range(len(activities)),len(subjects))


            ftests = ['A','B','AB'] #group, activity, group*activity
            fdesc = ['group','act','groupact']#correlating to the ftests variable

        elif flag_analysis == 'ttest':
            grouprs = np.copy(group)
            ftests = ['data'] #group, activity, group*activity
            fdesc = ['data']


        # In[7]:

        mesh_points, mesh_normals, mean_shape, surface = self.network_analysis.compute_mean_shape()

        mesh_normals


        # In[7]:


        #generate connectivity between particles
        pcd = o3d.geometry.PointCloud()
        pcd.points = o3d.utility.Vector3dVector(mean_shape[particles,:])
        pcd.normals = o3d.utility.Vector3dVector(mesh_normals[pts_index,:])

        distances = pcd.compute_nearest_neighbor_distance()
        avg_dist = np.mean(distances)
        radius = avg_dist*1.5

        mesh = o3d.geometry.TriangleMesh.create_from_point_cloud_ball_pivoting(pcd,
                                                                               o3d.utility.DoubleVector([radius,radius*2]))
        tris = np.asarray(mesh.triangles)


        # In[8]:


        with open(result_path+'tradZsig.npy', 'rb') as file:
            tradzsig = np.load(file)

        with open(result_path+'fvalues_size.npy', 'rb') as file:
            fvalues_size = np.load(file)

        poi = 0.05
        fr=87

        if flag_analysis == 'anova': #anova analysis
            traddf = pd.DataFrame({
                'Group': np.count_nonzero(tradzsig[:,:,0],axis=0),
                'Activity': np.count_nonzero(tradzsig[:,:,1],axis=0),
                'Group*Activity': np.count_nonzero(tradzsig[:,:,2],axis=0)})

        elif flag_analysis == 'ttest': #t-test
            traddf = pd.DataFrame({'Data': [np.count_nonzero(tradzsig)]})

        sns.set_style({'font.family':'sans-serif', 'font.serif':['Arial']})
        if flag_model == 'spatiotemporal': #anova analysis
            clustdf_size = pd.DataFrame({
                #     'Time': np.array(list(range(0,100))), 
                'Group': np.count_nonzero(fvalues_size[:,:,0,pthresh.index(poi)],axis=0),
                'Activity': np.count_nonzero(fvalues_size[:,:,1,pthresh.index(poi)],axis=0),
                'Group*Activity': np.count_nonzero(fvalues_size[:,:,2,pthresh.index(poi)],axis=0)})

            combidf = pd.concat([traddf,clustdf_size], axis=1)
            combidf.columns = ['1D Group', '1D Activity', '1D Group*Activity', 'ST Group', 'ST Activity', 'ST Group*Activity']
            #     combidf = combidf[['1D Group','ST Group','1D Activity','ST Activity','1D Group*Activity','ST Group*Activity']]
            #     combidf = combidf[['1D Group','1D Activity','1D Group*Activity']]
            for fr in range(timepoints):
                plt.figure(figsize = (5,2.4))
                p = sns.lineplot(data=combidf, palette="Set2", dashes=False, linewidth=3)
                p.tick_params(labelsize=8)
                # p.set_title('1D and Spatiotemporal SPM', fontsize=8)
                p.set_xlabel("Percent Gait", fontsize = 8)
                p.set_ylabel("Significant Particles", fontsize = 8)
                p.legend(fontsize=8)
                plt.ylim(0,32)
                plt.xlim(0,100)

                plt.axvline(fr, color='black')
                plt.rcParams["font.family"] = "sans-serif"
                plt.rcParams["font.serif"] = ["Arial"]
                plt.legend(bbox_to_anchor=(1.01, 0.85),
                           borderaxespad=0)
                plt.savefig(result_path+'Networkvs1D_'+str(num_pts)+'_'+str(fr)+'.png', dpi=300)

                img = cv2.imread(result_path+'Networkvs1D_'+str(num_pts)+'_'+str(fr)+'.png')
                if fr == 0:
                    height,width,layers=img.shape
                    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
                    video = cv2.VideoWriter(result_path+'plot_'+str(num_pts)+'.avi', fourcc, 5, (width, height))
                    video.write(img)

            cv2.destroyAllWindows()
            video.release()

        elif flag_model == 'fedata':
            clustdf_size = pd.DataFrame({
                'Group': np.count_nonzero(fvalues_size[:,:,0,pthresh.index(poi)],axis=0),
                'Activity': np.count_nonzero(fvalues_size[:,:,1,pthresh.index(poi)],axis=0),
                'Group*Activity': np.count_nonzero(fvalues_size[:,:,2,pthresh.index(poi)],axis=0)})
            print('Group - Traditional SPM: ',traddf['Group'][0],', Correspondence Network: ',clustdf_size['Group'][0])
            print('Activity - Traditional SPM: ',traddf['Activity'][0],', Correspondence Network: ',clustdf_size['Activity'][0])
            print('Group*Activity - Traditional SPM: ',traddf['Group*Activity'][0],', Correspondence Network: ',clustdf_size['Group*Activity'][0])
            print(len(particles), 'particles analyzed.')

        elif flag_model == 'cortthick': #t-test
            clustdf_size = pd.DataFrame({'Data': [np.count_nonzero(fvalues_size[:,:,:,pthresh.index(poi)])]})
            #     print(clustdf_size['Data'][0])
            print('Traditional SPM: ',traddf['Data'][0],', Correspondence Network: ',clustdf_size['Data'][0])



        # In[9]:


        if flag_model == 'cortthick':
            # generate lookup table for colormap
            vmin = 0
            vmax = 6  
        if flag_model == 'fedata':
            # generate lookup table for colormap
            vmin = 0
            vmax = 10
        if flag_model == 'spatiotemporal':
            # generate lookup table for colormap
            vmin = 0
            vmax = 20

        lut = vtk.vtkLookupTable()
        lut.SetTableRange(vmin, vmax)
        lut.SetHueRange(0,1)
        # lut.SetValueRange(vmin, vmax)
        n = 20
        lut.SetNumberOfTableValues(n)
        lut.Build()

        lut.SetTableValue(0, 0.9, 0.9, 0.9, 1)
        for i in range(1,n):
            c = vedo.colors.colorMap(i, 'PuRd', 1, n)
            lut.SetTableValue(i, c[0], c[1], c[2], 1)


        #read in mean shape and get bounds    
        # reader = vtk.vtkPolyDataReader()
        # reader.SetFileName(path+'mean_'+model_string+'.vtk')
        # reader.Update() 

        # surface = reader.GetOutput()

        bounds = np.array(surface.GetBounds())

        dims = np.array([101, 101, 101])
        box = vtk.vtkImageData()
        box.SetDimensions(dims)
        box.SetSpacing((bounds[1::2] - bounds[:-1:2])/(dims - 1))
        box.SetOrigin(bounds[::2])

        for analysis in ['1D', 'network']:#,'network']:#,'1D','network'
            for comparison in fdesc:   
                print('Processing',analysis+comparison,'data for visualization...')

                points_reader = vtk.vtkDelimitedTextReader()
                points_reader.SetFileName(result_path+'Points_'+analysis+comparison+'_'+str(num_pts)+'.txt')
                points_reader.DetectNumericColumnsOn()
                points_reader.SetFieldDelimiterCharacters('\t')
                points_reader.SetHaveHeaders(True)

                table_points = vtk.vtkTableToPolyData()
                table_points.SetInputConnection(points_reader.GetOutputPort())
                table_points.SetXColumn('x')
                table_points.SetYColumn('y')
                table_points.SetZColumn('z')
                table_points.Update()

                points = table_points.GetOutput()

                if flag_analysis == 'anova':
                    pts_reader = vtk.vtkDelimitedTextReader()
                    pts_reader.SetFileName(result_path+'Particles_'+analysis+comparison+'_'+str(num_pts)+'.txt')
                    pts_reader.DetectNumericColumnsOn()
                    pts_reader.SetFieldDelimiterCharacters('\t')
                    pts_reader.SetHaveHeaders(True)

                    table_pts = vtk.vtkTableToPolyData()
                    table_pts.SetInputConnection(pts_reader.GetOutputPort())
                    table_pts.SetXColumn('x')
                    table_pts.SetYColumn('y')
                    table_pts.SetZColumn('z')
                    table_pts.Update()

                    points_selection = table_pts.GetOutput()

                if flag_model == 'spatiotemporal':
                    #generate lut for progress bar visualization
                    lutpb = vtk.vtkLookupTable()
                    lutpb.SetTableRange(0, 99)
                    lutpb.SetHueRange(0,1)
                    lutpb.SetNumberOfTableValues(timepoints)
                    lutpb.Build()


                    for i in range(timepoints):#set all points to light grey
                        lutpb.SetTableValue(i, 0.8, 0.8, 0.8, 1)

                #start loop for each timepoint
                for fr in range(timepoints):
                    points.GetPointData().SetActiveScalars(str(fr))
                    if len(particles)<num_pts:
                        points_selection.GetPointData().SetActiveScalars(str(fr))
                        # Gaussian kernel
                    gaussian_kernel = vtk.vtkGaussianKernel()
                    gaussian_kernel.SetSharpness(2)
                    gaussian_kernel.SetRadius(avg_dist*1)

                    interpolator = vtk.vtkPointInterpolator()
                    interpolator.SetInputData(box)
                    interpolator.SetSourceData(points)
                    interpolator.SetKernel(gaussian_kernel)
                    interpolator.SetNullPointsStrategyToClosestPoint()
                    interpolator.Update()#??? does not hurt

                    resample = vtk.vtkResampleWithDataSet()
                    resample.SetInputData(surface)
                    resample.SetSourceConnection(interpolator.GetOutputPort())

                    mapper = vtk.vtkPolyDataMapper()#surface
                    mapper.SetInputConnection(resample.GetOutputPort())
                    mapper.SetScalarRange(vmin, vmax)
                    mapper.SetLookupTable(lut)

                    actor = vtk.vtkActor()
                    #             actor.GetProperty().LightingOff()
                    actor.SetMapper(mapper)

                    point_mapper = vtk.vtkPointGaussianMapper()#points
                    if len(particles)<num_pts:
                        point_mapper.SetInputData(points_selection)
                    else:
                        point_mapper.SetInputData(points)
                        point_mapper.SetScalarRange(vmin,vmax)
                        point_mapper.SetScaleFactor(0.8)
                        point_mapper.SetLookupTable(lut)
                        point_mapper.EmissiveOff();
                        point_mapper.SetSplatShaderCode(
                            "//VTK::Color::Impl\n"
                            "float dist = dot(offsetVCVSOutput.xy,offsetVCVSOutput.xy);\n"
                            "if (dist > 1.0) {\n"
                            "  discard;\n"
                            "} else {\n"
                            "  float scale = (1.0 - dist);\n"
                            "  ambientColor *= scale;\n"
                            "  diffuseColor *= scale;\n"
                            "}\n"
                        )

                    point_actor = vtk.vtkActor()
                    point_actor.SetMapper(point_mapper)

                    renderer = vtk.vtkRenderer()
                    renderer.SetBackground(1,1,1)
                    renWin = vtk.vtkRenderWindow()
                    renWin.AddRenderer(renderer)
                    iren = vtk.vtkRenderWindowInteractor()
                    iren.SetRenderWindow(renWin)

                    renderer.AddActor(actor)
                    renderer.AddActor(point_actor)

                    # iren.Initialize()

                    # renWin.Render()
                    # iren.Start()


                     #########################################################################
                     #             Define view
                     #########################################################################
                    if flag_model == 'cortthick':
                        ViewSize = [1400, 1200]
                        ScaleBarRatio = [0.7,0.1]
                        CenterOfRotation = [0,0,-5]
                        CameraPosition = [0, -170, -5]
                        CameraFocalPoint = [0,0,-5]
                        CameraViewUp = [0,0,1]
                        CameraParallelScale = 1
                    if flag_model == 'fedata':    
                        ViewSize = [800, 800]
                        ScaleBarRatio = [0.7,0.075]
                        CenterOfRotation = [-5,0,0]
                        CameraPosition = [75, -36, -120]
                        CameraFocalPoint = [-5,0,0]
                        CameraViewUp = [1,0,0]
                        CameraParallelScale = 1
                    if flag_model == 'spatiotemporal':    
                        ViewSize = [600, 800]
                        ScaleBarRatio = [0.7,0.075]
                        CenterOfRotation = [0,-10,-40]
                        CameraPosition = [115, -30, -130]
                        CameraFocalPoint = [0,-10,-40]
                        CameraViewUp = [1,0,0]
                        CameraParallelScale = 1

                    renWin.SetSize(ViewSize)
                    camera = renderer.GetActiveCamera()
                    camera.SetFocalPoint(CenterOfRotation)
                    camera.SetPosition(CameraPosition)
                    camera.SetFocalPoint(CameraFocalPoint)
                    camera.SetViewUp(CameraViewUp)
                    camera.SetParallelScale(CameraParallelScale)

                    #########################################################################
                    #             Add color bar
                    #########################################################################
                    if analysis == 'network':
                        scalarBar = vtk.vtkScalarBarActor()
                        scalarBar.SetLookupTable(lut)
                        if flag_analysis == 'anova':
                            scalarBar.SetTitle('F-value')
                        if flag_analysis == 'ttest':
                            scalarBar.SetTitle('T-value')
                            scalarBar.SetOrientationToHorizontal()
                            scalarBar.SetDisplayPosition(int(ViewSize[0]*(1-ScaleBarRatio[0])/2), 
                                                         int(ViewSize[1]*(ScaleBarRatio[1])/3))
                            scalarBar.SetWidth(ScaleBarRatio[0])
                            scalarBar.SetHeight(ScaleBarRatio[1])
                            scalarBar.SetLabelFormat("%.1f")
                            scalarBar.GetTitleTextProperty().ItalicOff()
                            scalarBar.GetTitleTextProperty().ShadowOff()
                            scalarBar.GetTitleTextProperty().SetColor(0,0,0)
                            renderer.AddActor2D(scalarBar)

                        textActor=vtk.vtkTextActor()
                        textActor.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
                        textActor.GetTextProperty().SetFontSize(24)
                        textActor.GetTextProperty().BoldOn()
                        textActor.SetDisplayPosition(int(ViewSize[0]*(1-ScaleBarRatio[0])), 
                                                     int(ViewSize[1]*(1-ScaleBarRatio[1]*.5)))
                        textActor.GetTextProperty().SetColor(1,1,1)
                        textActor.SetInput("Correspondence Network")
                        renderer.AddActor2D(textActor)

                    if analysis == '1D':
                        if flag_model == 'spatiotemporal':
                            lutpb.SetTableValue(fr, 0.5, 0.5, 0.5, 1)

                            progressBar = vtk.vtkScalarBarActor()
                            progressBar.SetLookupTable(lutpb)
                            progressBar.SetTitle('Percent Gait       ')
                            progressBar.SetNumberOfLabels(0)
                            progressBar.SetOrientationToHorizontal()
                            progressBar.SetDisplayPosition(int(ViewSize[0]*(1-ScaleBarRatio[0])/2), 
                                                           int(ViewSize[1]*(ScaleBarRatio[1])/3))
                            progressBar.SetWidth(ScaleBarRatio[0])
                            progressBar.SetHeight(ScaleBarRatio[1])
                            progressBar.GetTitleTextProperty().ItalicOff()
                            progressBar.GetTitleTextProperty().ShadowOff()
                            progressBar.GetTitleTextProperty().SetColor(0,0,0)
                            renderer.AddActor2D(progressBar)

                        if flag_model == 'cortthick' or flag_model == 'fedata':
                            scalarBar = vtk.vtkScalarBarActor()
                            scalarBar.SetLookupTable(lut)
                            if flag_model == 'fedata':
                                scalarBar.SetTitle('F-value')
                            if flag_model == 'cortthick':
                                scalarBar.SetTitle('T-value')
                                scalarBar.SetOrientationToHorizontal()
                                scalarBar.SetDisplayPosition(int(ViewSize[0]*(1-ScaleBarRatio[0])/2), 
                                                             int(ViewSize[1]*(ScaleBarRatio[1])/3))
                                scalarBar.SetWidth(ScaleBarRatio[0])
                                scalarBar.SetHeight(ScaleBarRatio[1])
                                scalarBar.SetLabelFormat("%.1f")
                                scalarBar.GetLabelTextProperty().ItalicOff()
                                scalarBar.GetLabelTextProperty().ShadowOff()
                                scalarBar.GetLabelTextProperty().SetColor(0,0,0)
                                renderer.AddActor2D(scalarBar)

                        textActor=vtk.vtkTextActor()
                        textActor.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
                        textActor.GetTextProperty().SetFontSize(24)
                        textActor.GetTextProperty().BoldOn()
                        if flag_model == 'spatiotemporal' or flag_model == 'fedata':
                            textActor.SetDisplayPosition(int(ViewSize[0]*(1-ScaleBarRatio[0]+0.075)), 
                                                         int(ViewSize[1]*(1-ScaleBarRatio[1]*.5)))
                        if flag_model == 'cortthick':
                            textActor.SetDisplayPosition(int(ViewSize[0]*(1-ScaleBarRatio[0]+0.15)),
                                                         int(ViewSize[1]*(1-ScaleBarRatio[1]*.5)))
                            textActor.GetTextProperty().SetColor(1,1,1)
                            textActor.SetInput("Traditional SPM")
                            #                 if comparison == 'group':
                            #                     textActor.SetInput("Group")
                            #                 elif comparison == 'act':
                            #                     textActor.SetInput("Activity")
                            #                 elif comparison == 'groupact':
                            #                     textActor.SetInput("Activity*Group")
                        renderer.AddActor2D(textActor)


                    l1 = vtk.vtkLight()
                    l1.SetPosition(CameraPosition)
                    l1.SetFocalPoint(CameraFocalPoint)
                    l1.SetColor(1.0, 1.0, 1.0)
                    l1.PositionalOn()
                    renderer.AddLight(l1)
                    l1.SwitchOn()


                    renderLarge = vtk.vtkWindowToImageFilter()
                    renderLarge.SetInput(renWin)

                    writer = vtk.vtkPNGWriter()
                    writer.SetInputConnection(renderLarge.GetOutputPort())
                    if flag_model == 'spatiotemporal':
                        writer.SetFileName(result_path+analysis+comparison+str(fr)+'.png')
                    if flag_model == 'cortthick' or flag_model == 'fedata':
                        writer.SetFileName(result_path+analysis+comparison+'.png')
                        renWin.SetOffScreenRendering(True)
                        renWin.Render()
                        writer.Write()


                    if flag_model == 'cortthick' or flag_model == 'fedata':
                        surfWriter = vtk.vtkPolyDataWriter()
                        surfWriter.SetFileName(result_path+analysis+comparison+'.vtk')
                        surfWriter.SetInputData(surface)
                        surfWriter.SetInputConnection(resample.GetOutputPort())
                        if flag_analysis == 'ttest':
                            surfWriter.SetScalarsName('T-value')
                        if flag_analysis == 'anova':
                            surfWriter.SetScalarsName('F-value')
                            surfWriter.Write()


        # In[10]:


        if flag_model == 'spatiotemporal':
            # generate lookup table for colormap
            vmin = 0
            vmax = 20
            fr = 87

            lut = vtk.vtkLookupTable()
            lut.SetTableRange(vmin, vmax)
            lut.SetHueRange(0,1)
            # lut.SetValueRange(vmin, vmax)
            n = 20
            lut.SetNumberOfTableValues(n)
            lut.Build()

            lut.SetTableValue(0, 0.9, 0.9, 0.9, 1)
            for i in range(1,n):
                c = vedo.colors.colorMap(i, 'PuRd', 1, n)
                lut.SetTableValue(i, c[0], c[1], c[2], 1)


            #read in mean shape and get bounds    
            # reader = vtk.vtkPolyDataReader()
            # reader.SetFileName(path+'mean_'+model_string+'.vtk')
            # reader.Update() 

            # surface = reader.GetOutput()

            bounds = np.array(surface.GetBounds())

            dims = np.array([101, 101, 101])
            box = vtk.vtkImageData()
            box.SetDimensions(dims)
            box.SetSpacing((bounds[1::2] - bounds[:-1:2])/(dims - 1))
            box.SetOrigin(bounds[::2])

            for analysis in ['1D','network']:#,'network']:#,'1D','ST'
                for comparison in fdesc:   
                    print('Processing',analysis+comparison,'data for visualization...')

                    points_reader = vtk.vtkDelimitedTextReader()
                    points_reader.SetFileName(result_path+'Points_'+analysis+comparison+'_'+str(num_pts)+'.txt')
                    points_reader.DetectNumericColumnsOn()
                    points_reader.SetFieldDelimiterCharacters('\t')
                    points_reader.SetHaveHeaders(True)

                    table_points = vtk.vtkTableToPolyData()
                    table_points.SetInputConnection(points_reader.GetOutputPort())
                    table_points.SetXColumn('x')
                    table_points.SetYColumn('y')
                    table_points.SetZColumn('z')
                    table_points.Update()

                    points = table_points.GetOutput()

                    pts_reader = vtk.vtkDelimitedTextReader()
                    pts_reader.SetFileName(result_path+'Particles_'+analysis+comparison+'_'+str(num_pts)+'.txt')
                    pts_reader.DetectNumericColumnsOn()
                    pts_reader.SetFieldDelimiterCharacters('\t')
                    pts_reader.SetHaveHeaders(True)

                    table_pts = vtk.vtkTableToPolyData()
                    table_pts.SetInputConnection(pts_reader.GetOutputPort())
                    table_pts.SetXColumn('x')
                    table_pts.SetYColumn('y')
                    table_pts.SetZColumn('z')
                    table_pts.Update()

                    points_selection = table_pts.GetOutput()


                    #generate lut for progress bar visualization
                    lutpb = vtk.vtkLookupTable()
                    lutpb.SetTableRange(0, 99)
                    lutpb.SetHueRange(0,1)
                    lutpb.SetNumberOfTableValues(timepoints)
                    lutpb.Build()


        #             for i in range(timepoints):#set all points to light grey
        #                 lutpb.SetTableValue(i, 0.8, 0.8, 0.8, 1)

                    #start loop for each timepoint
                    points.GetPointData().SetActiveScalars(str(fr))
                    points_selection.GetPointData().SetActiveScalars(str(fr))
                    # Gaussian kernel
                    gaussian_kernel = vtk.vtkGaussianKernel()
                    gaussian_kernel.SetSharpness(2)
                    gaussian_kernel.SetRadius(avg_dist*1)

                    interpolator = vtk.vtkPointInterpolator()
                    interpolator.SetInputData(box)
                    interpolator.SetSourceData(points)
                    interpolator.SetKernel(gaussian_kernel)
                    interpolator.SetNullPointsStrategyToClosestPoint()
                    interpolator.Update()#??? does not hurt

                    resample = vtk.vtkResampleWithDataSet()
                    resample.SetInputData(surface)
                    resample.SetSourceConnection(interpolator.GetOutputPort())

                    mapper = vtk.vtkPolyDataMapper()#surface
                    mapper.SetInputConnection(resample.GetOutputPort())
                    mapper.SetScalarRange(vmin, vmax)
                    mapper.SetLookupTable(lut)

                    actor = vtk.vtkActor()
                    #             actor.GetProperty().LightingOff()
                    actor.SetMapper(mapper)

                    point_mapper = vtk.vtkPointGaussianMapper()#points
                    point_mapper.SetInputData(points_selection)
                    point_mapper.SetScalarRange(vmin,vmax)
                    point_mapper.SetScaleFactor(0.8)
                    point_mapper.SetLookupTable(lut)
                    point_mapper.EmissiveOff();
                    point_mapper.SetSplatShaderCode(
                        "//VTK::Color::Impl\n"
                        "float dist = dot(offsetVCVSOutput.xy,offsetVCVSOutput.xy);\n"
                        "if (dist > 1.0) {\n"
                        "  discard;\n"
                        "} else {\n"
                        "  float scale = (1.0 - dist);\n"
                        "  ambientColor *= scale;\n"
                        "  diffuseColor *= scale;\n"
                        "}\n"
                    )

                    point_actor = vtk.vtkActor()
                    point_actor.SetMapper(point_mapper)

                    renderer = vtk.vtkRenderer()
                    renderer.SetBackground(1,1,1)
                    renWin = vtk.vtkRenderWindow()
                    renWin.AddRenderer(renderer)
                    iren = vtk.vtkRenderWindowInteractor()
                    iren.SetRenderWindow(renWin)

                    renderer.AddActor(actor)
                    renderer.AddActor(point_actor)

                     #########################################################################
                     #             Define view
                     #########################################################################
                    ViewSize = [900, 800]
                    ScaleBarRatio = [0.7,0.075]
                    CenterOfRotation = [0,-10,-40]
                    CameraPosition = [115, -30, -130]
                    CameraFocalPoint = [0,-10,-40]
                    CameraViewUp = [1,0,0]
                    CameraParallelScale = 1

                    renWin.SetSize(ViewSize)
                    camera = renderer.GetActiveCamera()
                    camera.SetFocalPoint(CenterOfRotation)
                    camera.SetPosition(CameraPosition)
                    camera.SetFocalPoint(CameraFocalPoint)
                    camera.SetViewUp(CameraViewUp)
                    camera.SetParallelScale(CameraParallelScale)

                    l1 = vtk.vtkLight()
                    l1.SetPosition(CameraPosition)
                    l1.SetFocalPoint(CameraFocalPoint)
                    l1.SetColor(1.0, 1.0, 1.0)
                    l1.PositionalOn()
                    renderer.AddLight(l1)
                    l1.SwitchOn()


                    renderLarge = vtk.vtkWindowToImageFilter()
                    renderLarge.SetInput(renWin)

                    writer = vtk.vtkPNGWriter()
                    writer.SetInputConnection(renderLarge.GetOutputPort())
                    writer.SetFileName(result_path+analysis+comparison+str(fr)+'_figure.png')
                    renWin.SetOffScreenRendering(True)
                    renWin.Render()
                    writer.Write()



        # In[11]:


        if flag_model == 'spatiotemporal':
            for comparison in fdesc:   
                #combine images into video 
                print('  Combining into video...')
                for j in range(timepoints):
                    imgst = cv2.imread(result_path+'network'+comparison+str(j)+'.png')
                    img1d = cv2.imread(result_path+'1D'+comparison+str(j)+'.png')
                    if j == 0:
                        height,width,layers=imgst.shape
                        fourcc = cv2.VideoWriter_fourcc(*'mp4v')
                        video = cv2.VideoWriter(result_path+comparison+'_'+str(num_pts)+'.avi', fourcc, 5, (width*2, height))
                        video.write(np.concatenate([imgst,img1d],axis=1))

                cv2.destroyAllWindows()
                video.release()


        # In[12]:


        if flag_model == 'spatiotemporal':
            #combine images into video 
            print('  Combining into video...')
            for j in range(timepoints):
                img1da = cv2.imread(result_path+'1Dact'+str(j)+'.png')
                img1dg = cv2.imread(result_path+'1Dgroup'+str(j)+'.png')
                img1dag = cv2.imread(result_path+'1Dgroupact'+str(j)+'.png')
                if j == 0:
                    height,width,layers=imgst.shape
                    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
                    video = cv2.VideoWriter(result_path+'1d_all_'+str(num_pts)+'.avi', fourcc, 5, (width*3, height))
                    video.write(np.concatenate([img1da,img1dg,img1dag],axis=1))

            cv2.destroyAllWindows()
            video.release()


        # In[13]:


        #plot means, diffs, and stdevs
        if flag_model == 'cortthick':
            plots = ['G0','G1','Diff','StDev']
        elif flag_model == 'fedata':
            plots = ['StDev_ah','StDev_dh','StDev_wh','StDev_wm','G0_ah','G0_dh','G0_wh','G0_wm','G1_ah','G1_dh','G1_wh','G1_wm','Diff_ah','Diff_dh','Diff_wh','Diff_wm']
        elif flag_model == 'spatiotemporal':
            plots = ['G0_incline','G1_incline','Diff_incline','StDev_incline','G0_walkSS','G1_walkSS','Diff_walkSS','StDev_walkSS']

        for plotdata in plots:
            print('Processing',plotdata,'data for visualization...')
            if flag_model == 'cortthick':
                if plotdata == 'G0' or plotdata == 'G1':
                    vmin = 0
                    vmax = 4  
                elif plotdata == 'Diff':
                    vmin = -1.2
                    vmax = 1.2
                elif plotdata == 'StDev':
                    vmin = 0
                    vmax = 1.2  
            if flag_model == 'fedata':
                if plotdata.startswith('G0') or plotdata.startswith('G1'):
                    vmin = 0
                    vmax = 4  
                elif plotdata.startswith('Diff'):
                    vmin = -2
                    vmax = 2 
                elif plotdata.startswith('StDev'):
                    vmin = 0
                    vmax = 3
            if flag_model == 'spatiotemporal':
                if plotdata.startswith('G0') or plotdata.startswith('G1'):
                    vmin = 0
                    vmax = 5  
                elif plotdata.startswith('Diff'):
                    vmin = -0.8
                    vmax = 0.8
                elif plotdata.startswith('StDev'):
                    vmin = 0
                    vmax = 1 

            lut = vtk.vtkLookupTable()
            lut.SetTableRange(vmin, vmax)
            lut.SetHueRange(0,1)
            # lut.SetValueRange(vmin, vmax)
            n = 20
            lut.SetNumberOfTableValues(n)
            lut.Build()

        #     lut.SetTableValue(0, 1, 1, 0.75, 1)
            for i in range(0,n):
                if plotdata.startswith('G0') or plotdata.startswith('G1'):
                    if flag_model == 'spatiotemporal':
                        c = vedo.colors.colorMap(n-i, 'Blues', 1, n)
                    else:
                        c = vedo.colors.colorMap(i, 'Blues', 1, n) 
                elif plotdata.startswith('Diff'):
                    c = vedo.colors.colorMap(i, 'PuOr', 1, n)
                elif plotdata.startswith('StDev'):
                    c = vedo.colors.colorMap(i, 'Greens', 1, n)
                    lut.SetTableValue(i, c[0], c[1], c[2], 1)

            if plotdata.startswith('G0'):
                #read in mean shape and get bounds    

                print("Alan: TODO: need group0 mesh")
                #reader = vtk.vtkPolyDataReader()
                #reader.SetFileName(path+group_names[0]+vtk_name+'.vtk')
                #reader.Update()
                #surface = reader.GetOutput()
            elif plotdata.startswith('G1'):
                #read in mean shape and get bounds    

                print("Alan: TODO: need group1 mesh")
                #reader = vtk.vtkPolyDataReader()
                #reader.SetFileName(path+group_names[1]+vtk_name+'.vtk')
                #reader.Update()
                #surface = reader.GetOutput()
            else:
                print("Alan: TODO: need mean mesh")
                #read in mean shape and get bounds    
                #reader = vtk.vtkPolyDataReader()
                #reader.SetFileName(path+'mean'+vtk_name+'.vtk')
                #reader.Update()
                #surface = reader.GetOutput()

            bounds = np.array(surface.GetBounds())

            dims = np.array([101, 101, 101])
            box = vtk.vtkImageData()
            box.SetDimensions(dims)
            box.SetSpacing((bounds[1::2] - bounds[:-1:2])/(dims - 1))
            box.SetOrigin(bounds[::2])

            points_reader = vtk.vtkDelimitedTextReader()
            points_reader.SetFileName(result_path+plotdata+'_'+str(num_pts)+'.txt')
            points_reader.DetectNumericColumnsOn()
            points_reader.SetFieldDelimiterCharacters('\t')
            points_reader.SetHaveHeaders(True)

            table_points = vtk.vtkTableToPolyData()
            table_points.SetInputConnection(points_reader.GetOutputPort())
            table_points.SetXColumn('x')
            table_points.SetYColumn('y')
            table_points.SetZColumn('z')
            table_points.Update()

            points = table_points.GetOutput()

            if flag_model == 'spatiotemporal':
                #generate lut for progress bar visualization
                lutpb = vtk.vtkLookupTable()
                lutpb.SetTableRange(0, 99)
                lutpb.SetHueRange(0,1)
                lutpb.SetNumberOfTableValues(timepoints)
                lutpb.Build()


                for i in range(timepoints):#set all points to light grey
                    lutpb.SetTableValue(i, 0.8, 0.8, 0.8, 1)

            #start loop for each timepoint
            for fr in range(timepoints):
                points.GetPointData().SetActiveScalars(str(fr))

                # Gaussian kernel
                gaussian_kernel = vtk.vtkGaussianKernel()
                gaussian_kernel.SetSharpness(2)
                gaussian_kernel.SetRadius(avg_dist*1)

                interpolator = vtk.vtkPointInterpolator()
                interpolator.SetInputData(box)
                interpolator.SetSourceData(points)
                interpolator.SetKernel(gaussian_kernel)
                interpolator.SetNullPointsStrategyToClosestPoint()
                interpolator.Update()#??? does not hurt

                resample = vtk.vtkResampleWithDataSet()
                resample.SetInputData(surface)
                resample.SetSourceConnection(interpolator.GetOutputPort())

                mapper = vtk.vtkPolyDataMapper()#surface
                mapper.SetInputConnection(resample.GetOutputPort())
                mapper.SetScalarRange(vmin, vmax)
                mapper.SetLookupTable(lut)

                actor = vtk.vtkActor()
                actor.SetMapper(mapper)

                renderer = vtk.vtkRenderer()
                renderer.SetBackground(1,1,1)
                renWin = vtk.vtkRenderWindow()
                renWin.AddRenderer(renderer)
                iren = vtk.vtkRenderWindowInteractor()
                iren.SetRenderWindow(renWin)

                renderer.AddActor(actor)


                 #########################################################################
                 #             Define view
                 #########################################################################
                if flag_model == 'cortthick':
                    ViewSize = [1400, 1200]
                    ScaleBarRatio = [0.7,0.1]
                    CenterOfRotation = [0,0,-5]
                    CameraPosition = [0, -170, -5]
                    CameraFocalPoint = [0,0,-5]
                    CameraViewUp = [0,0,1]
                    CameraParallelScale = 1
                if flag_model == 'fedata':    
                    ViewSize = [800, 800]
                    ScaleBarRatio = [0.7,0.075]
                    CenterOfRotation = [-5,0,0]
                    CameraPosition = [75, -36, -120]
                    CameraFocalPoint = [-5,0,0]
                    CameraViewUp = [1,0,0]
                    CameraParallelScale = 1
                if flag_model == 'spatiotemporal':    
                    ViewSize = [900, 800]
                    ScaleBarRatio = [0.7,0.075]
                    CenterOfRotation = [0,-10,-40]
                    CameraPosition = [115, -30, -130]
                    CameraFocalPoint = [0,-10,-40]
                    CameraViewUp = [1,0,0]
                    CameraParallelScale = 1

                renWin.SetSize(ViewSize)
                camera = renderer.GetActiveCamera()
                camera.SetFocalPoint(CenterOfRotation)
                camera.SetPosition(CameraPosition)
                camera.SetFocalPoint(CameraFocalPoint)
                camera.SetViewUp(CameraViewUp)
                camera.SetParallelScale(CameraParallelScale)

                #########################################################################
                #             Add color bar
                #########################################################################

                scalarBar = vtk.vtkScalarBarActor()
                scalarBar.SetLookupTable(lut)
                if flag_model == 'spatiotemporal':
                    scalarBar.SetTitle('Joint Space')
                if flag_model == 'fedata':
                    scalarBar.SetTitle('Contact Pressure')
                if flag_model == 'cortthick':
                    scalarBar.SetTitle('Cortical Thickness')
                    scalarBar.SetOrientationToHorizontal()
                    scalarBar.SetDisplayPosition(int(ViewSize[0]*(1-ScaleBarRatio[0])/2), 
                                                 int(ViewSize[1]*(ScaleBarRatio[1])/3))
                    scalarBar.SetWidth(ScaleBarRatio[0])
                    scalarBar.SetHeight(ScaleBarRatio[1])
                    scalarBar.SetLabelFormat("%.1f")
                    scalarBar.GetTitleTextProperty().ItalicOff()
                    scalarBar.GetTitleTextProperty().ShadowOff()
                    scalarBar.GetTitleTextProperty().SetColor(0,0,0)
                    renderer.AddActor2D(scalarBar)


                l1 = vtk.vtkLight()
                l1.SetPosition(CameraPosition)
                l1.SetFocalPoint(CameraFocalPoint)
                l1.SetColor(1,1,1)
                l1.PositionalOn()
                renderer.AddLight(l1)
                l1.SwitchOn()


                renderLarge = vtk.vtkWindowToImageFilter()
                renderLarge.SetInput(renWin)

                writer = vtk.vtkPNGWriter()
                writer.SetInputConnection(renderLarge.GetOutputPort())
                if flag_model == 'spatiotemporal':
                    writer.SetFileName(result_path+plotdata+str(fr)+'.png')
                if flag_model == 'cortthick' or flag_model == 'fedata':
                    writer.SetFileName(result_path+plotdata+'.png')
                    renWin.SetOffScreenRendering(True)
                    renWin.Render()
                    writer.Write()


                if flag_model == 'cortthick' or flag_model == 'fedata':
                    surfWriter = vtk.vtkPolyDataWriter()
                    surfWriter.SetFileName(result_path+plotdata+'.vtk')
                    surfWriter.SetInputData(surface)
                    surfWriter.SetInputConnection(resample.GetOutputPort())
                    if flag_model == 'cortthick':
                        surfWriter.SetScalarsName('Thickness')
                    if flag_model == 'fedata':
                        surfWriter.SetScalarsName('Stress')
                        surfWriter.Write()


        # In[14]:


        if flag_model == 'spatiotemporal':
            for act in activities:   
                #combine images into video 
                print('  Combining into video...')
                for j in range(timepoints):
                    imgdiff = cv2.imread(result_path+'Diff_'+act+str(j)+'.png')
                    imgsd = cv2.imread(result_path+'StDev_'+act+str(j)+'.png')
                    if j == 0:
                        height,width,layers=imgdiff.shape
                        fourcc = cv2.VideoWriter_fourcc(*'mp4v')
                        video = cv2.VideoWriter(result_path+'DiffSD_'+act+'_'+str(num_pts)+'.avi', fourcc, 5, (width*2, height))
                        video.write(np.concatenate([imgdiff,imgsd],axis=1))

                cv2.destroyAllWindows()
                video.release()


                #combine images into video 
                print('  Combining into video...')
                for j in range(timepoints):
                    imgg0 = cv2.imread(result_path+'G0_'+act+str(j)+'.png')
                    imgg1 = cv2.imread(result_path+'G1_'+act+str(j)+'.png')
                    if j == 0:
                        height,width,layers=imgg0.shape
                        fourcc = cv2.VideoWriter_fourcc(*'mp4v')
                        video = cv2.VideoWriter(result_path+'G0G1_'+act+'_'+str(num_pts)+'.avi', fourcc, 5, (width*2, height))
                        video.write(np.concatenate([imgg0,imgg1],axis=1))

                cv2.destroyAllWindows()
                video.release()


        # In[15]:


        #plot means, diffs, and stdevs
        fr = 87
        if flag_model == 'spatiotemporal':
            plots = ['G0_incline','G1_incline','Diff_incline','StDev_incline','G0_walkSS','G1_walkSS','Diff_walkSS','StDev_walkSS','ActivityDiff','incline','walkSS', 'StDev','G0_ActivityDiff','G0_StDev','G1_ActivityDiff','G1_StDev']
            for plotdata in plots:
                print('Processing',plotdata,'data for visualization...')  
                if 'Diff' in plotdata:
                    vmin = -0.8
                    vmax = 0.8
                elif 'StDev' in plotdata:
                    vmin = 0
                    vmax = 1.2 
                else:#f plotdata.startswith('G0') or plotdata.startswith('G1'):
                    vmin = 1.8
                    vmax = 4.8

                lut = vtk.vtkLookupTable()
                lut.SetTableRange(vmin, vmax)
                lut.SetHueRange(0,1)
                # lut.SetValueRange(vmin, vmax)
                n = 20
                lut.SetNumberOfTableValues(n)
                lut.Build()

            #     lut.SetTableValue(0, 1, 1, 0.75, 1)
                for i in range(0,n):
                    if 'Diff' in plotdata:
                        c = vedo.colors.colorMap(i, 'PuOr', 1, n)
                    elif 'StDev' in plotdata:
                        c = vedo.colors.colorMap(i, 'Greens', 1, n)
                    else:#if plotdata.startswith('G0') or plotdata.startswith('G1'):
                        if flag_model == 'spatiotemporal':
                            c = vedo.colors.colorMap(n-i, 'Blues', 1, n)
                        else:
                            c = vedo.colors.colorMap(i, 'Blues', 1, n) 
                            lut.SetTableValue(i, c[0], c[1], c[2], 1)

                if plotdata.startswith('G0'):
                    #read in mean shape and get bounds    
                    reader = vtk.vtkPolyDataReader()
                    reader.SetFileName(path+group_names[0]+vtk_name+'.vtk')
                    reader.Update() 

                    surface = reader.GetOutput()
                elif plotdata.startswith('G1'):
                    #read in mean shape and get bounds    
                    reader = vtk.vtkPolyDataReader()
                    reader.SetFileName(path+group_names[1]+vtk_name+'.vtk')
                    reader.Update() 

                    surface = reader.GetOutput()
                else:
                    #read in mean shape and get bounds    
                    reader = vtk.vtkPolyDataReader()
                    reader.SetFileName(path+'mean'+vtk_name+'.vtk')
                    reader.Update() 

                    surface = reader.GetOutput()

                bounds = np.array(surface.GetBounds())

                dims = np.array([101, 101, 101])
                box = vtk.vtkImageData()
                box.SetDimensions(dims)
                box.SetSpacing((bounds[1::2] - bounds[:-1:2])/(dims - 1))
                box.SetOrigin(bounds[::2])

                points_reader = vtk.vtkDelimitedTextReader()
                points_reader.SetFileName(result_path+plotdata+'_'+str(num_pts)+'.txt')
                points_reader.DetectNumericColumnsOn()
                points_reader.SetFieldDelimiterCharacters('\t')
                points_reader.SetHaveHeaders(True)

                table_points = vtk.vtkTableToPolyData()
                table_points.SetInputConnection(points_reader.GetOutputPort())
                table_points.SetXColumn('x')
                table_points.SetYColumn('y')
                table_points.SetZColumn('z')
                table_points.Update()

                points = table_points.GetOutput()

                #generate lut for progress bar visualization
                lutpb = vtk.vtkLookupTable()
                lutpb.SetTableRange(0, 99)
                lutpb.SetHueRange(0,1)
                lutpb.SetNumberOfTableValues(timepoints)
                lutpb.Build()

                #start loop for each timepoint
                points.GetPointData().SetActiveScalars(str(fr))

                # Gaussian kernel
                gaussian_kernel = vtk.vtkGaussianKernel()
                gaussian_kernel.SetSharpness(2)
                gaussian_kernel.SetRadius(avg_dist*1)

                interpolator = vtk.vtkPointInterpolator()
                interpolator.SetInputData(box)
                interpolator.SetSourceData(points)
                interpolator.SetKernel(gaussian_kernel)
                interpolator.SetNullPointsStrategyToClosestPoint()
                interpolator.Update()#??? does not hurt

                resample = vtk.vtkResampleWithDataSet()
                resample.SetInputData(surface)
                resample.SetSourceConnection(interpolator.GetOutputPort())

                mapper = vtk.vtkPolyDataMapper()#surface
                mapper.SetInputConnection(resample.GetOutputPort())
                mapper.SetScalarRange(vmin, vmax)
                mapper.SetLookupTable(lut)

                actor = vtk.vtkActor()
                actor.SetMapper(mapper)

                renderer = vtk.vtkRenderer()
                renderer.SetBackground(1,1,1)
                renWin = vtk.vtkRenderWindow()
                renWin.AddRenderer(renderer)
                iren = vtk.vtkRenderWindowInteractor()
                iren.SetRenderWindow(renWin)

                renderer.AddActor(actor)


                 #########################################################################
                 #             Define view
                 #########################################################################
                ViewSize = [900, 800]
                ScaleBarRatio = [0.7,0.075]
                CenterOfRotation = [0,-10,-40]
                CameraPosition = [115, -30, -130]
                CameraFocalPoint = [0,-10,-40]
                CameraViewUp = [1,0,0]
                CameraParallelScale = 1

                renWin.SetSize(ViewSize)
                camera = renderer.GetActiveCamera()
                camera.SetFocalPoint(CenterOfRotation)
                camera.SetPosition(CameraPosition)
                camera.SetFocalPoint(CameraFocalPoint)
                camera.SetViewUp(CameraViewUp)
                camera.SetParallelScale(CameraParallelScale)

                l1 = vtk.vtkLight()
                l1.SetPosition(CameraPosition)
                l1.SetFocalPoint(CameraFocalPoint)
                l1.SetColor(1,1,1)
                l1.PositionalOn()
                renderer.AddLight(l1)
                l1.SwitchOn()


                renderLarge = vtk.vtkWindowToImageFilter()
                renderLarge.SetInput(renWin)

                writer = vtk.vtkPNGWriter()
                writer.SetInputConnection(renderLarge.GetOutputPort())
                writer.SetFileName(result_path+plotdata+str(fr)+'_figure.png')
                renWin.SetOffScreenRendering(True)
                renWin.Render()
                writer.Write()


        print("Figures All Done!")