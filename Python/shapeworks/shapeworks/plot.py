import pyvista as pv
import numpy as np
import subprocess
from shapeworks import Image, Mesh
from .utils import num_subplots
from .conversion import sw2vtkImage, sw2vtkMesh
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

# helper function that addes a vtk image to a pyvista plotter
def add_volume_to_plotter(pvPlotter,      # pyvista plotter
                          vtkImg,         # vtk image to be added
                          rowIdx, colIdx, # subplot row and column index
                          title          = None,   # text to be added to subplot, use None to not show text
                          shade_volumes  = True,  # use shading when performing volume rendering
                          color_map      = "coolwarm", # color map for volume rendering, e.g., 'bone', 'coolwarm', 'cool', 'viridis', 'magma'
                          show_axes      = True,  # show a vtk axes widget for each rendering window
                          show_bounds    = False, # show volume bounding box
                          show_all_edges = True,  # add an unlabeled and unticked box at the boundaries of plot
                          font_size      = 10     # text font size for windows
                          ):

    # which subplot to add volume to
    pvPlotter.subplot(rowIdx, colIdx)

    # add volume
    pvPlotter.add_volume(vtkImg,
                         shade   = shade_volumes,
                         cmap    = color_map)

    if show_axes:
        pvPlotter.show_axes()

    if show_bounds:
        pvPlotter.show_bounds(all_edges = show_all_edges)

    # add text to this subplot to indicate which volume is being visualized
    if title is not None:
        pvPlotter.add_text(title, font_size = font_size)

# helper function that adds a mesh to a `pyvista` plotter
def add_mesh_to_plotter(pvPlotter,      # pyvista plotter
                        vtkMesh,        # vtk mesh to be added
                        rowIdx, colIdx, # subplot row and column index
                        title           = None,    # text to be added to subplot, use None to not show text
                        mesh_color      = "tan",  # string or 3 item list
                        mesh_style      = "surface", # visualization style of mesh. style='surface', style='wireframe', style='points'
                        show_mesh_edges = False, # show mesh edges
                        opacity         = 1,
                        show_axes       = True,  # show a vtk axes widget for each rendering window
                        show_bounds     = False, # show volume bounding box
                        show_all_edges  = True,  # add an unlabeled and unticked box at the boundaries of plot
                        font_size       = 10     # text font size for windows
                        ):

    # which subplot to add mesh to
    pvPlotter.subplot(rowIdx, colIdx)

    # add surface mesh
    pvPlotter.add_mesh(vtkMesh,
                       color      = mesh_color,
                       style      = mesh_style,
                       show_edges = show_mesh_edges,
                       opacity    = opacity)

    if show_axes:
        pvPlotter.show_axes()

    if show_bounds:
        pvPlotter.show_bounds(all_edges = show_all_edges)

    # add text to this subplot to indicate which volume is being visualized
    if title is not None:
        pvPlotter.add_text(title, font_size = font_size)

# add and plot a list of volumes
def plot_volumes(volumeList,              # list of shapeworks images to be visualized
                 volumeNames     = None,  # list of strings of same size as shape list used to add text for each plot window, use None to not show text per window
                 use_same_window = False, # plot using multiple rendering windows if false
                 notebook        = False, # Plots inline if true
                 show_borders    = True,  # show borders for each rendering window
                 shade_volumes   = True,  # use shading when performing volume rendering
                 color_map       = "coolwarm", # color map for volume rendering, e.g., 'bone', 'coolwarm', 'cool', 'viridis', 'magma'
                 show_axes       = True,  # show a vtk axes widget for each rendering window
                 show_bounds     = True,  # show volume bounding box
                 show_all_edges  = True,  # add an unlabeled and unticked box at the boundaries of plot
                 font_size       = 10,    # text font size for windows
                 link_views      = True  # link all rendering windows so that they share same camera and axes boundaries,
                ):
    """
    Renders all segmentations in a dataset using a pyvista plotter to render
    multiple windows, each with a single segmentation, add segmentations to
    plotter, and start rendering.
    """
    if type(volumeList) is not list:
        volumeList = list([volumeList])

    num_samples = len(volumeList)

    if volumeNames is not None:
        if use_same_window and (len(volumeNames) > 1):
            print('A single title needed when all volumes are to be displayed on the same window')
            return
        elif (not use_same_window) and (len(volumeNames) != num_samples):
            print('volumeNames list is not consistent with number of samples')
            return

    if use_same_window:
        grid_rows, grid_cols = 1, 1
    else:
        # define grid size for given number of samples
        grid_rows, grid_cols = num_subplots(num_samples)

    # define plotter
    plotter = pv.Plotter(shape    = (grid_rows, grid_cols),
                         notebook = notebook,
                         border   = show_borders)

    # add given volume list (one at a time) to plotter
    for volumeIdx in range(num_samples):

        # which window to add current volume
        if use_same_window:
            rowIdx, colIdx = 0, 0
            titleIdx       = 0
        else:
            idUnraveled     = np.unravel_index(volumeIdx, (grid_rows, grid_cols))
            rowIdx, colIdx  = idUnraveled[0], idUnraveled[1]
            titleIdx        = volumeIdx

        # which title to use
        if volumeNames is not None:
            volumeName = volumeNames[titleIdx]
        else:
            volumeName = None

        # convert sw image to vtk image
        if type(volumeList[volumeIdx]) == Image:
            volume_vtk = sw2vtkImage(volumeList[volumeIdx], verbose = False)
        else:
            volume_vtk = volumeList[volumeIdx]

        # add current volume
        add_volume_to_plotter(plotter, volume_vtk,
                              rowIdx = rowIdx, colIdx = colIdx,
                              title          = volumeName,
                              shade_volumes  = shade_volumes,
                              color_map      = color_map,
                              show_axes      = show_axes,
                              show_bounds    = show_bounds,
                              show_all_edges = show_all_edges,
                              font_size      = font_size)
    # link views
    if link_views and (not use_same_window):
        plotter.link_views()
    plotter.show()

# add and plot a list of meshes
def plot_meshes(meshList,                # list of shapeworks meshes to be visualized
                meshNames       = None,  # list of strings of same size as shape list used to add text for each plot window, use None to not show text per window
                use_same_window = False, # plot using multiple rendering windows if false
                notebook        = False, # Plots inline if true
                show_borders    = True,  # show borders for each rendering window
                meshes_color    = 'tan', # color to be used for meshes (can be a list with same size as meshList if different colors are needed)
                mesh_style      = "surface", # visualization style of mesh. style='surface', style='wireframe', style='points'
                show_mesh_edges = False, # show mesh edges
                opacities       = 1,     # opacity to be used for meshes (can be a list with same size as meshList if different opacities are needed)
                show_axes       = True,  # show a vtk axes widget for each rendering window
                show_bounds     = True,  # show volume bounding box
                show_all_edges  = True,  # add an unlabeled and unticked box at the boundaries of plot
                font_size       = 10,    # text font size for windows
                link_views      = True   # link all rendering windows so that they share same camera and axes boundaries
               ):
    """
    assigns different colors for each mesh
    note this is very similar to plot_volumes but with meshes rather than volumes
    """
    if type(meshList) is not list:
        meshList = list([meshList])
        
    num_samples = len(meshList)

    if meshNames is not None:
        if use_same_window and (len(meshNames) > 1):
            print('A single title needed when all meshes are to be displayed on the same window')
            return
        elif (not use_same_window) and  (len(meshNames) != num_samples):
            print('meshNames list is not consistent with number of samples')
            return

    if type(meshes_color) is not list: # single color given
        meshes_color = [meshes_color] * num_samples

    if type(opacities) is not list: # single opacity given
        opacities = [opacities] * num_samples

    if use_same_window:
        grid_rows, grid_cols = 1, 1
    else:
        # define grid size for given number of samples
        grid_rows, grid_cols  = num_subplots(num_samples)

    # define plotter
    plotter = pv.Plotter(shape    = (grid_rows, grid_cols),
                         notebook = notebook,
                         border   = show_borders)

    # add given volume list (one at a time) to plotter
    for meshIdx in range(num_samples):

        # which window to add current mesh
        if use_same_window:
            rowIdx, colIdx = 0, 0
            titleIdx       = 0
        else:
            idUnraveled     = np.unravel_index(meshIdx, (grid_rows, grid_cols))
            rowIdx, colIdx  = idUnraveled[0], idUnraveled[1]
            titleIdx        = meshIdx

        # which title to use
        if meshNames is not None:
            meshName = meshNames[titleIdx]
        else:
            meshName = None

        # convert sw mesh to vtk mesh
        if type(meshList[meshIdx]) == Mesh:
            mesh_vtk = sw2vtkMesh(meshList[meshIdx], verbose = False)
        else:
            mesh_vtk = meshList[meshIdx]

        # add current mesh
        add_mesh_to_plotter(plotter, mesh_vtk,
                            rowIdx = rowIdx, colIdx = colIdx,
                            title           = meshName,
                            mesh_color      = meshes_color[meshIdx],
                            mesh_style      = mesh_style,
                            show_mesh_edges = show_mesh_edges,
                            opacity         = opacities[meshIdx],
                            show_axes       = show_axes,
                            show_bounds     = show_bounds,
                            show_all_edges  = show_all_edges,
                            font_size       = font_size)

    # link views
    if link_views and (not use_same_window):
        plotter.link_views()
    plotter.show()

# helper function to add and plot a list of meshes/volumes mix
def plot_meshes_volumes_mix(objectList,              # list of shapeworks meshes to be visualized
                            objectsType,             # list of 'vol', 'mesh' of same size as objectList
                            objectNames     = None,  # list of strings of same size as shape list used to add text for each plot window, use None to not show text per window
                            use_same_window = False, # plot using multiple rendering windows if false
                            notebook        = False, # Plots inline if true
                            show_borders    = True,  # show borders for each rendering window
                            meshes_color    = 'tan', # color to be used for meshes (can be a list with same size as meshList if different colors are needed)
                            mesh_style      = "surface", # visualization style of mesh. style='surface', style='wireframe', style='points'
                            shade_volumes   = True,  # use shading when performing volume rendering
                            color_map       = "coolwarm", # color map for volume rendering, e.g., 'bone', 'coolwarm', 'cool', 'viridis', 'magma'
                            show_mesh_edges = False, # show mesh edges
                            opacities       = 1,     # opacity to be used for meshes (can be a list with same size as meshList if different opacities are needed)
                            show_axes       = True,  # show a vtk axes widget for each rendering window
                            show_bounds     = True,  # show volume bounding box
                            show_all_edges  = True,  # add an unlabeled and unticked box at the boundaries of plot
                            font_size       = 10,    # text font size for windows
                            link_views      = True   # link all rendering windows so that they share same camera and axes boundaries
                            ):

    num_samples = len(objectList)

    if objectNames is not None:
        if use_same_window and (len(objectNames) > 1):
            print('A single title needed when all objects are to be displayed on the same window')
            return
        elif (not use_same_window) and  (len(objectNames) != num_samples):
            print('objectNames list is not consistent with number of samples')
            return

    if type(meshes_color) is not list: # single color given
        meshes_color = [meshes_color] * num_samples

    if type(opacities) is not list: # single opacity given
        opacities = [opacities] * num_samples

    if use_same_window:
        grid_rows, grid_cols = 1, 1
    else:
        # define grid size for given number of samples
        grid_rows, grid_cols  = num_subplots(num_samples)

    # define plotter
    plotter = pv.Plotter(shape    = (grid_rows, grid_cols),
                         notebook = notebook,
                         border   = show_borders)

    # add given volume list (one at a time) to plotter
    for objectIdx in range(num_samples):

        # which window to add current mesh
        if use_same_window:
            rowIdx, colIdx = 0, 0
            titleIdx       = 0
        else:
            idUnraveled     = np.unravel_index(objectIdx, (grid_rows, grid_cols))
            rowIdx, colIdx  = idUnraveled[0], idUnraveled[1]
            titleIdx        = objectIdx

        # which title to use
        if objectNames is not None:
            objectName = objectNames[titleIdx]
        else:
            objectName = None

        if objectsType[objectIdx] == 'vol':

            # convert sw image to vtk image
            if type(objectList[objectIdx]) == Image:
                object_vtk = sw2vtkImage(objectList[objectIdx], verbose = False)
            else:
                object_vtk = objectList[objectIdx]

            # add current volume
            add_volume_to_plotter(plotter, object_vtk,
                                  rowIdx = rowIdx, colIdx = colIdx,
                                  title          = objectName,
                                  shade_volumes  = shade_volumes,
                                  color_map      = color_map,
                                  show_axes      = show_axes,
                                  show_bounds    = show_bounds,
                                  show_all_edges = show_all_edges,
                                  font_size      = font_size)

        else: # 'mesh'
            # convert sw mesh to vtk image
            if type(objectList[objectIdx]) == Mesh:
                object_vtk = sw2vtkMesh(objectList[objectIdx], verbose = False)
            else:
                object_vtk = objectList[objectIdx]

            # add current mesh
            add_mesh_to_plotter(plotter, object_vtk,
                                rowIdx = rowIdx, colIdx = colIdx,
                                title           = objectName,
                                mesh_color      = meshes_color[objectIdx],
                                mesh_style      = mesh_style,
                                show_mesh_edges = show_mesh_edges,
                                opacity         = opacities[objectIdx],
                                show_axes       = show_axes,
                                show_bounds     = show_bounds,
                                show_all_edges  = show_all_edges,
                                font_size       = font_size)

    # link views
    if link_views and (not use_same_window):
        plotter.link_views()
    plotter.show()

def plot_mesh_contour(Image):
    p = pv.Plotter()
    contours=Image.contour()
    p.add_mesh(Image, opacity=0.5)
    p.add_mesh(contours, color="yellow", line_width=5)
    p.show_bounds(grid=True)
    p.show()


# plot the PCA metrics similar to Studio visualization in Analyze pane
def plot_pca_metrics(cumulative_variance,explained_variance,shape_models_dir):
    N = len(cumulative_variance) 
    X = np.array(list(range(N))) + 1
    plt.bar(X, explained_variance)
    plt.plot(X,cumulative_variance,linewidth=4.0,c='black')
    fig = plt.gcf()
    fig.set_size_inches(20, 10)
    plt.title('Variance Plot',fontsize=16)
    plt.xlabel('Mode',fontsize=16)
    plt.ylabel('Explained Variance',fontsize=16)
    plt.xticks(X)
    plt.legend(["Cumulative Explained Variance","Explained Variance"],fontsize=16)
    plt.savefig(shape_models_dir+"variance_plot.png",dpi=700)
    plt.show(block=False)
    plt.close(fig)

    print("\nPCA metrics plot saved in directory - " + shape_models_dir)
    print()



#violin plot for loadings
def pca_loadings_violinplot(loadings,cumulative_variance,shape_models_dir):

    
    
    min_dims = np.where(cumulative_variance <=99)[0]
    #if the first mode is the most dominant, min_dims will be empty
    if(min_dims.size==0):
        min_dims = 1
    else:
        min_dims = min_dims[-1]+1

    print("\nNumber of modes covering 99% variance - ", min_dims)
    min_dims = 10 if min_dims>15 else min_dims
    loadings = loadings[:,:min_dims]
    dims = []
    for i in range(len(loadings)):
        for j in range(np.shape(loadings)[1]):
            dims.append(j+1)
            

    loadings = loadings.flatten()
    data = {'PCA Mode':dims, "PCA Score":loadings}
    df = pd.DataFrame(data) 
    plt.figure(figsize=(10,6),dpi=300)  
    ax = sns.violinplot(x='PCA Mode', y='PCA Score',\
                        data=df, palette="cool_r", split=True, scale="area")
    fig = plt.gcf()
    plt.title("Violin Plots Showing the Distribution of the PCA Loadings",fontsize=16)
    plt.savefig(shape_models_dir+"pca_loadings_violin_plot.png",dpi=700)
    plt.show(block=False)
    plt.close(fig)

    print("\nLoadings plot saved in directory - " + shape_models_dir)
    print()




#Function to generate the line plot for shape evaluation statistics
def plot_mode_line(save_dir,filename,title,ylabel):
    # Load scree plot data
    Y = np.loadtxt(save_dir+ filename)
    N = len(Y)
    X = np.arange(1, N+1)

    # Plot data
    plt.plot(X, Y, linewidth=4.0)
    fig = plt.gcf()
    fig.set_size_inches(10, 10)
    plt.title(title,fontsize=16)
    plt.xlabel('Mode',fontsize=16)
    plt.ylabel(ylabel,fontsize=16)
    plt.xticks(X)
    plt.ylim(bottom=0, top=max(Y)*1.5)
    plt.xlim(left=1, right=N)
    plt.grid()
    plt.savefig(save_dir+title.lower()+"_plot.png")
    plt.show(block=False)
    plt.pause(3)
    plt.close(fig)

    print("Figure saved in directory -" + save_dir)
    print()



#Call to ShapeWorksStudio to visualize the best and the worst reconstruction



def visualize_reconstruction(save_dir):
    print("*************************")
    print("* Best reconstruction")
    print("*************************")
    subprocess.check_call(['ShapeWorksStudio', save_dir+'/0perc.xml'])

    print("*************************")
    print("* Worst reconstruction")
    print("*************************")
    subprocess.check_call(['ShapeWorksStudio', save_dir+'/100perc.xml'])

    print()