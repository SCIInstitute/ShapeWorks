import csv
import re
import numpy as np
from bokeh.document import Document
from bokeh.embed import file_html
from bokeh.layouts import gridplot
from bokeh.models import (BasicTicker, Circle, ColumnDataSource, DataRange1d,
                          Grid, LinearAxis, PanTool, Plot, WheelZoomTool,)
from bokeh.resources import INLINE
from bokeh.util.browser import view

def splom(data_csv):
    # read csv
    colors = [] # 0 for original, 1 for aug
    scores = []
    with open(data_csv, newline='') as csvfile:
        datareader = csv.reader(csvfile)
        index = 0
        for row in datareader:
            if "Generated" in row[0]:
                colors.append('red')
            else:
                colors.append('blue')
            scores.append(row[2:])
    scores = np.array(scores)

    # make splom
    plots = []
    for y in range(len(scores[0])):
        row = []
        for x in range(len(scores[0])):
            xax = (y == len(scores[0])-1)
            yax = (x == 0)
            xscores = [float(item) for item in list(scores[:,x])]
            yscores = [float(item) for item in list(scores[:,y])]
            source = ColumnDataSource(dict(x=xscores, y=yscores, colors=colors))
            plot = make_plot(source, x, y, xax, yax)
            row.append(plot)
        plots.append(row)

    grid = gridplot(plots)

    doc = Document()
    doc.add_root(grid)

    doc.validate()
    filename = "augmentation_splom.html"
    with open(filename, "w") as f:
        f.write(file_html(doc, INLINE, "Data SPLOM"))
    print("Wrote %s" % filename)
    view(filename)


def make_plot(source, xindex, yindex, xax=False, yax=False):
    xdr = DataRange1d(bounds=None)
    ydr = DataRange1d(bounds=None)
    mbl = 40 if yax else 0
    mbb = 40 if xax else 0
    plot = Plot(
        x_range=xdr, y_range=ydr, background_fill_color="#efe8e2",
        border_fill_color='white', plot_width=200 + mbl, plot_height=200 + mbb,
        min_border_left=2+mbl, min_border_right=2, min_border_top=2, min_border_bottom=2+mbb)

    circle = Circle(x='x', y='y', fill_color="colors", fill_alpha=0.2, size=4, line_color="colors")
    r = plot.add_glyph(source, circle)

    xdr.renderers.append(r)
    ydr.renderers.append(r)

    xticker = BasicTicker()
    if xax:
        xaxis = LinearAxis()
        xaxis.axis_label = str(xindex)
        plot.add_layout(xaxis, 'below')
        xticker = xaxis.ticker
    plot.add_layout(Grid(dimension=0, ticker=xticker))

    yticker = BasicTicker()
    if yax:
        yaxis = LinearAxis()
        yaxis.axis_label = str(yindex)
        yaxis.major_label_orientation = 'vertical'
        plot.add_layout(yaxis, 'left')
        yticker = yaxis.ticker
    plot.add_layout(Grid(dimension=1, ticker=yticker))

    plot.add_tools(PanTool(), WheelZoomTool())
    return plot