import sys
from pyqtgraph.Qt import QtCore, QtGui
import pyqtgraph as pg
import pyqtgraph.opengl as gl
import numpy as np
from PyQt5 import QtWidgets, QtCore
import OpenGL.GL as ogl


class Custom3DAxis(gl.GLAxisItem):
    """Class defined to extend 'gl.GLAxisItem'."""

    def __init__(self, parent, color=(0, 0, 0, .6)):
        gl.GLAxisItem.__init__(self)
        self.parent = parent
        self.c = color

    def add_labels(self):
        """Adds axes labels."""
        x, y, z = self.size()
        qfont = QtGui.QFont("Times", 16)
        qfont.setBold(True)
        #X label
        self.xLabel = gl.GLTextItem(pos=(x / 2, -y / 20, -z / 20 - 1.5),
                                    text="X",
                                    font=qfont)
        # self.xLabel.setGLViewWidget(self.parent)
        self.parent.addItem(self.xLabel)
        #Y label
        self.yLabel = gl.GLTextItem(pos=(-x / 20, y / 2, -z / 20 - 1.5),
                                    text="Y",
                                    font=qfont)
        # self.yLabel.setGLViewWidget(self.parent)
        self.parent.addItem(self.yLabel)
        #Z label
        self.zLabel = gl.GLTextItem(pos=(-x / 20, -y / 20 - 1.5, z / 2),
                                    text="Z",
                                    font=qfont)
        # self.zLabel.setGLViewWidget(self.parent)
        self.parent.addItem(self.zLabel)

    def add_tick_values(self, xticks=[], yticks=[], zticks=[]):
        """Adds ticks values."""
        x, y, z = self.size()
        xtpos = np.linspace(-x, x, len(xticks))
        ytpos = np.linspace(-y, y, len(yticks))
        ztpos = np.linspace(0, z, len(zticks))
        #X label
        for i, xt in enumerate(xticks):
            if (xt == 0):
                continue
            val = gl.GLTextItem(pos=(xtpos[i], -y / 20, -z / 20),
                                text=str(xt),
                                font=QtGui.QFont("Times", 10))
            # val.setGLViewWidget(self.parent)
            self.parent.addItem(val)
        #Y label
        for i, yt in enumerate(yticks):
            if (yt == 0):
                continue
            val = gl.GLTextItem(pos=(-x / 20, ytpos[i], -z / 20),
                                text=str(yt),
                                font=QtGui.QFont("Times", 10))
            # val.setGLViewWidget(self.parent)
            self.parent.addItem(val)
        #Z label
        for i, zt in enumerate(zticks):
            val = gl.GLTextItem(pos=(-x / 20, -y / 20, ztpos[i]),
                                text=str(zt),
                                font=QtGui.QFont("Times", 10))
            # val.setGLViewWidget(self.parent)
            self.parent.addItem(val)

    def add_text(self, text, pos=(0, 0, 0)):
        """Adds text."""
        qfont = QtGui.QFont("Times", 16)
        qfont.setBold(True)
        qfont.setItalic(True)
        self.textItem = gl.GLTextItem(pos=pos, text=text, font=qfont)
        # val.setGLViewWidget(self.parent)
        self.parent.addItem(self.textItem)

    def update_text(self, text):
        """Updates text."""
        self.textItem.setData(text=text)

    def paint(self):
        self.setupGLState()
        if self.antialias:
            ogl.glEnable(ogl.GL_LINE_SMOOTH)
            ogl.glHint(ogl.GL_LINE_SMOOTH_HINT, ogl.GL_NICEST)
        ogl.glBegin(ogl.GL_LINES)

        x, y, z = self.size()
        #Draw Z
        ogl.glColor4f(self.c[0], self.c[1], self.c[2], self.c[3])
        ogl.glVertex3f(0, 0, 0)
        ogl.glVertex3f(0, 0, z)
        #Draw Y
        ogl.glColor4f(self.c[0], self.c[1], self.c[2], self.c[3])
        ogl.glVertex3f(0.001, -y, 0)
        ogl.glVertex3f(0.001, y, 0)
        #Draw X
        ogl.glColor4f(self.c[0], self.c[1], self.c[2], self.c[3])
        ogl.glVertex3f(-x, 0.001, 0)
        ogl.glVertex3f(x, 0.001, 0)
        ogl.glEnd()


class drawing:

    def __init__(self):

        self.x = np.empty(10)
        self.y = np.empty(10)
        self.z = np.empty(10)
        self.colors = np.empty((10, 4))
        for i in range(10):
            self.colors[i] = [152 / 255.0, 245 / 255.0, 255 / 255.0, i / 10.0]

    def update_data(self, a):
        # a[0] = a[0] - 32768 - a[2] / 1000
        # a[1] = a[1] - 32768 - a[2] / 1000
        self.x = np.append(self.x, (a[0] - 32768) / 3000.0)
        self.y = np.append(self.y, (a[1] - 32768) / 3000.0)
        self.z = np.append(self.z, a[2] / 6000.0)
        self.x = self.x[1:]
        self.y = self.y[1:]
        self.z = self.z[1:]
        self.p1.setData(pos=np.vstack([self.x, self.y, self.z]).transpose())
        self.app.processEvents()

    def show_plot(self):
        self.app = pg.mkQApp()
        self.w = gl.GLViewWidget()
        background_color = QtGui.QColor(0, 0, 0, 255)
        self.w.setBackgroundColor(background_color)
        self.w.opts['distance'] = 30
        self.w.show()
        self.w.setWindowTitle('3D Touch Skin Demo')
        self.g = gl.GLGridItem()
        self.w.addItem(self.g)
        self.ax = Custom3DAxis(self.w, color=(0.9, 0.9, 0.9, .9))
        self.ax.setSize(10, 10, 10)
        self.ax.add_labels()
        self.ax.add_tick_values(
            xticks=[-2.5, -2, -1.5, -1, -0.5, 0, 0.5, 1, 1.5, 2, 2.5],
            yticks=[-2.5, -2, -1.5, -1, -0.5, 0, 0.5, 1, 1.5, 2, 2.5],
            zticks=[0, 2.5, 5, 7.5, 10])
        self.w.addItem(self.ax)
        self.p1 = gl.GLScatterPlotItem(pos=np.vstack([self.x, self.y,
                                                      self.z]).transpose(),
                                       color=self.colors,
                                       size=0.8,
                                       pxMode=False)

        self.w.addItem(self.p1)
        self.ax.add_text(" ", pos=(9, -10, 10))

    def show_text(self, text):
        self.ax.update_text(text)
        self.app.processEvents()


# draw = drawing()
# draw.show_plot()
# draw.show_text("Hello World")
# draw.update_data([60000, 60000, 10000])
# sys.exit(draw.app.exec())