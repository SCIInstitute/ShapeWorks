import vedo
import numpy as np
from random import random

# this is one instance of the class Plotter with 1 rows and 5 columns
vp1 = vedo.Plotter(shape=(5,6), axes=0, title='ContourDomain Visualizer')

# set a different background color for a specific subwindow (the last one)
# vp1.renderers[24].SetBackground(0.8, 0.9, 0.9)  # use vtk method SetBackground()

is_hidden = False
meshes = []
x_filter = 1e8
n_shapes = 30

def x_filter_slider(widget, event):
    global x_filter
    value = widget.GetRepresentation().GetValue()
    x_filter = value

def key_press(evt):
    global is_hidden, meshes, vp1, x_filter, x_filter_slider
    if evt.keyPressed == 'h':
        for i in range(n_shapes):
            if is_hidden:
                vp1.renderers[i].AddActor(meshes[i][0])
                vp1.renderers[i].AddActor(meshes[i][1])
            else:
                vp1.renderers[i].RemoveActor(meshes[i][0])
                vp1.renderers[i].RemoveActor(meshes[i][1])
        is_hidden = not is_hidden
    elif evt.keyPressed == '0':
        x_filter += 0.05
        x_filter_slider.GetRepresentation().SetValue(x_filter)
        print(f'x_filter: {x_filter}')
    elif evt.keyPressed == '9':
        x_filter -= 0.05
        x_filter = max(x_filter, 0)
        x_filter_slider.GetRepresentation().SetValue(x_filter)
        print(f'x_filter: {x_filter}')

for i in range(n_shapes):
    fname = f'contours/{i}.vtp'

    # line = vedo.shapes.Line(pts, lw=3)

    line = vp1.load(fname)
    line.wireframe()
    line.lw(4)
    line.lc('#000000')
    meshes.append(line)

    vp1.show(fname, at=i)
    vp1.show(meshes[i], at=i)

vp1.addCallback('KeyPress', key_press, 50000)
x_filter_slider = vp1.addSlider2D(x_filter_slider, xmin=0.0, xmax=2.5, value=0.1, pos=([0.05,0.05],[0.15,0.05]), title="test")


list_colors = [
    [255, 0, 0],
    [0, 0, 255],
    [0, 255, 255],
    [255, 255, 0],
    [255, 0, 255],
    [128, 128, 128],
    [40,240, 40],
    [240, 40, 40],
    [40, 40, 240],
    [0, 255, 0],
]
class ParticleSystem:
    def __init__(self, at):
        pts = np.zeros((256, 3))
        colors = np.zeros((256, 3), dtype=np.uint8)
        for i in range(256):
            c_idx = i % len(list_colors)
            c = list_colors[c_idx]
            colors[i] = c
        self.msph = vedo.Points(pts, c=colors, r=28)
        self.at = at
        vp1.add(self.msph, at=at)
    
    def update(self, all_pts):
        global x_filter

        colors = np.zeros(all_pts.shape, dtype='uint8')
        for i in range(colors.shape[0]):
            colors[i,:] = list_colors[i%len(list_colors)]

        if all_pts.shape[0] != 256:
            colors = np.vstack([
                colors,
                np.full((256-all_pts.shape[0], 3), 0).astype('uint8')
            ])
            all_pts = np.vstack([
                all_pts,
                np.full((256-all_pts.shape[0], 3), 50000)
            ])
        
        alpha = np.full((256, ), 0.8)

        self.msph.points(all_pts)
        # print(colors)
        # self.msph.pointColors(colors, alpha=alpha)

systems = []

for d in range(n_shapes):
    systems.append(ParticleSystem(at=d))

while True:
    all_pts = np.loadtxt('ramdisk/mega_local.particles')
    rpd = all_pts.shape[0] // n_shapes
    for d,ps in enumerate(systems):
        pts = all_pts[d*rpd:(d+1)*rpd]
        ps.update(pts)
    vp1.allowInteraction()
    vp1.render()
    # vp1.show()
