import pyvista as pv
import numpy as np
from ..shapeworks import * # works
# enable use_ipyvtk by default for interactive plots
pv.rcParams['use_ipyvtk'] = True 

# helper function to determine the best grid size (rows and columns) given the number of samples in a dataset.
def postive_factors(num_samples):
    factors = []

    for whole_number in range(1, num_samples + 1):
        if num_samples % whole_number == 0:
            factors.append(whole_number)

    return factors

def num_subplots(num_samples):
    factors = postive_factors(num_samples)
    cols    = min(int(np.ceil(np.sqrt(num_samples))),max(factors))
    rows    = int(np.ceil(num_samples/cols))

    return rows, cols

# helper function that addes a vtk image to a pyvista plotter
def add_volume_to_plotter(pvPlotter,      # pyvista plotter
                          vtkImg,         # vtk image to be added
                          rowIdx, colIdx, # subplot row and column index
                          title          = None,   # text to be added to the subplot, use None to not show text 
                          shade_volumes  = True,  # use shading when performing volume rendering
                          color_map      = "coolwarm", # color map for volume rendering, e.g., 'bone', 'coolwarm', 'cool', 'viridis', 'magma'
                          show_axes      = True,  # show a vtk axes widget for each rendering window
                          show_bounds    = False, # show volume bounding box
                          show_all_edges = True,  # add an unlabeled and unticked box at the boundaries of plot. 
                          font_size      = 10     # text font size for windows
                          ):
    
    # which subplot to add the volume to
    pvPlotter.subplot(rowIdx, colIdx)
    
    # add the volume
    pvPlotter.add_volume(vtkImg, 
                         shade   = shade_volumes,
                         cmap    = color_map)

    if show_axes:
        pvPlotter.show_axes()

    if show_bounds:
        pvPlotter.show_bounds(all_edges = show_all_edges)

    # add a text to this subplot to indicate which volume is being visualized
    if title is not None:
        pvPlotter.add_text(title, font_size = font_size)
        
# helper function that adds a mesh to a `pyvista` plotter.
def add_mesh_to_plotter(pvPlotter,      # pyvista plotter
                        vtkMesh,         # vtk mesh to be added
                        rowIdx, colIdx, # subplot row and column index
                        title           = None,    # text to be added to the subplot, use None to not show text 
                        mesh_color      = "tan",  # string or 3 item list
                        mesh_style      = "surface", # visualization style of the mesh. style='surface', style='wireframe', style='points'. 
                        show_mesh_edges = False, # show mesh edges
                        opacity         = 1,
                        show_axes       = True,  # show a vtk axes widget for each rendering window
                        show_bounds     = False, # show volume bounding box
                        show_all_edges  = True,  # add an unlabeled and unticked box at the boundaries of plot. 
                        font_size       = 10     # text font size for windows
                        ):
    
    # which subplot to add the mesh to
    pvPlotter.subplot(rowIdx, colIdx)

    # add the surface mesh
    pvPlotter.add_mesh(vtkMesh, 
                       color      = mesh_color, 
                       style      = mesh_style,
                       show_edges = show_mesh_edges,
                       opacity    = opacity)

    if show_axes:
        pvPlotter.show_axes()

    if show_bounds:
        pvPlotter.show_bounds(all_edges = show_all_edges)

    # add a text to this subplot to indicate which volume is being visualized
    if title is not None:
        pvPlotter.add_text(title, font_size = font_size)

# add and plot a list of volumes
def plot_volumes(volumeList,           # list of shapeworks images to be visualized
                 volumeNames     = None,  # list of strings of same size as shape list used to add text for each plot window, use None to not show text per window 
                 use_same_window = False, # plot using multiple rendering windows if false
                 is_interactive  = True,  # to enable interactive plots
                 show_borders    = True,  # show borders for each rendering window
                 shade_volumes   = True,  # use shading when performing volume rendering
                 color_map       = "coolwarm", # color map for volume rendering, e.g., 'bone', 'coolwarm', 'cool', 'viridis', 'magma'
                 show_axes       = True,  # show a vtk axes widget for each rendering window
                 show_bounds     = True,  # show volume bounding box
                 show_all_edges  = True,  # add an unlabeled and unticked box at the boundaries of plot. 
                 font_size       = 10,    # text font size for windows
                 link_views      = True   # link all rendering windows so that they share same camera and axes boundaries
                ):
    """
    Renders all segmentations in a dataset using a pyvista plotter to render
    multiple windows, each with a single segmentation, add segmentations to the
    plotter, and start rendering.
    """
    
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
        # define grid size for the given number of samples
        grid_rows, grid_cols  = num_subplots(num_samples)

    # define the plotter
    plotter = pv.Plotter(shape    = (grid_rows, grid_cols),
                         notebook = is_interactive, 
                         border   = show_borders) 
    
    # add the given volume list (one at a time) to the plotter
    for volumeIdx in range(num_samples):
        
        # which window to add the current volume
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
        # else:
            volume_vtk = volumeList[volumeIdx]

        # add the current volume
        add_volume_to_plotter( plotter, volume_vtk,   
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

    # now, time to render our volumes
    plotter.show(use_ipyvtk = is_interactive)

# add and plot a list of meshes
def plot_meshes(meshList,           # list of shapeworks meshes to be visualized
                meshNames       = None,  # list of strings of same size as shape list used to add text for each plot window, use None to not show text per window 
                use_same_window = False, # plot using multiple rendering windows if false
                is_interactive  = True,  # to enable interactive plots
                show_borders    = True,  # show borders for each rendering window
                meshes_color    = 'tan', # color to be used for meshes (can be a list with the same size as meshList if different colors are needed)
                mesh_style      = "surface", # visualization style of the mesh. style='surface', style='wireframe', style='points'. 
                show_mesh_edges = False, # show mesh edges
                opacities       = 1,     # opacity to be used for meshes (can be a list with the same size as meshList if different opacities are needed) 
                show_axes       = True,  # show a vtk axes widget for each rendering window
                show_bounds     = True,  # show volume bounding box
                show_all_edges  = True,  # add an unlabeled and unticked box at the boundaries of plot. 
                font_size       = 10,    # text font size for windows
                link_views      = True   # link all rendering windows so that they share same camera and axes boundaries
               ):
    """
    assigns different colors for each mesh
    note this is very similar to the plot_volumes but with meshes rather than volumes
    """
    
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
        # define grid size for the given number of samples
        grid_rows, grid_cols  = utils.num_subplots(num_samples)

    # define the plotter
    plotter = pv.Plotter(shape    = (grid_rows, grid_cols),
                         notebook = is_interactive, 
                         border   = show_borders) 
    
    # add the given volume list (one at a time) to the plotter
    for meshIdx in range(num_samples):
        
        # which window to add the current mesh
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
        if type(meshList[meshIdx]) == sw.Mesh:
            mesh_vtk = utils.sw2vtkMesh(meshList[meshIdx],
                                  verbose = False)
        else:
            mesh_vtk = meshList[meshIdx]

        # add the current mesh
        add_mesh_to_plotter( plotter, mesh_vtk,   
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

    # now, time to render our meshes
    plotter.show(use_ipyvtk = is_interactive)

# helper function to add and plot a list of meshes/volumes mix
def plot_meshes_volumes_mix(objectList,    # list of shapeworks meshes to be visualized
                            objectsType, # list of 'vol', 'mesh' of same size as objectList
                            objectNames     = None,  # list of strings of same size as shape list used to add text for each plot window, use None to not show text per window 
                            use_same_window = False, # plot using multiple rendering windows if false
                            is_interactive  = True,  # to enable interactive plots
                            show_borders    = True,  # show borders for each rendering window
                            meshes_color    = 'tan', # color to be used for meshes (can be a list with the same size as meshList if different colors are needed)
                            mesh_style      = "surface", # visualization style of the mesh. style='surface', style='wireframe', style='points'. 
                            shade_volumes   = True,  # use shading when performing volume rendering
                            color_map       = "coolwarm", # color map for volume rendering, e.g., 'bone', 'coolwarm', 'cool', 'viridis', 'magma' 
                            show_mesh_edges = False, # show mesh edges
                            opacities       = 1,     # opacity to be used for meshes (can be a list with the same size as meshList if different opacities are needed) 
                            show_axes       = True,  # show a vtk axes widget for each rendering window
                            show_bounds     = True,  # show volume bounding box
                            show_all_edges  = True,  # add an unlabeled and unticked box at the boundaries of plot. 
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
        # define grid size for the given number of samples
        grid_rows, grid_cols  = utils.num_subplots(num_samples)

    # define the plotter
    plotter = pv.Plotter(shape    = (grid_rows, grid_cols),
                         notebook = is_interactive, 
                         border   = show_borders) 
    
    # add the given volume list (one at a time) to the plotter
    for objectIdx in range(num_samples):
        
        # which window to add the current mesh
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
            if type(objectList[objectIdx]) == sw.Image:
                object_vtk = utils.sw2vtkImage(objectList[objectIdx],
                                         verbose = False)
            else:
                object_vtk = objectList[objectIdx]
            
            # add the current volume
            add_volume_to_plotter( plotter, object_vtk,   
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
            if type(objectList[objectIdx]) == sw.Mesh:
                object_vtk = utils.sw2vtkMesh(objectList[objectIdx],
                                        verbose = False)
            else:
                object_vtk = objectList[objectIdx]

            # add the current mesh
            add_mesh_to_plotter( plotter, object_vtk,   
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

    # now, time to render our mesh/volume mix
    plotter.show(use_ipyvtk = is_interactive)
