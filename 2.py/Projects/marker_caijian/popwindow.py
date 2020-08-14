# -*- coding: utf-8 -*-
"""
Created on Tue Feb 27 16:03:26 2018

@author: qinchanghai
"""
import sys
import numpy as np
import pandas as pd

import time
import hashlib
import os
import traceback
import struct
from datetime import datetime

from PyQt5.QtWidgets import (QApplication, QWidget, QToolTip,
     QPushButton, QMainWindow, QAction, QDockWidget, QMessageBox,
     QFileDialog, QGridLayout, QLabel, QHBoxLayout,QVBoxLayout,QDialogButtonBox,
     QLineEdit, QTableView, QInputDialog, QRadioButton,QProgressBar,
     QTabWidget, QSizePolicy, QCheckBox,QMenu,QComboBox)

from PyQt5.QtCore import pyqtSignal,QObject,Qt,QDateTime

from PyQt5.QtGui import (QStandardItemModel,QStandardItem,QScreen,\
                         QBrush,  QColor ,  QFont 
    )

from guidata.qt.QtGui import QSplitter, QListWidget,QCursor,QToolBar,QDialog,\
        QIcon,QDoubleValidator,QListWidgetItem
from guidata.qt.QtCore import QSize, QT_VERSION_STR, PYQT_VERSION_STR, \
        Qt,Signal, pyqtSignal,QObject

from guiqwt.colormap import get_colormap_list, get_cmap, build_icon_from_cmap

from guiqwt.tools import (RectangleTool, EllipseTool, HRangeTool, PlaceAxesTool,
                          MultiLineTool, FreeFormTool, SegmentTool, CircleTool,
                          AnnotatedRectangleTool, AnnotatedEllipseTool,
                          AnnotatedSegmentTool, AnnotatedCircleTool, LabelTool,
                          AnnotatedPointTool,ColormapTool,
                          VCursorTool, HCursorTool, XCursorTool,
                          ObliqueRectangleTool, AnnotatedObliqueRectangleTool)


import guiqwt
from guiqwt.baseplot import BasePlot
from guiqwt.transitional import QwtLinearColorMap, QwtInterval,\
        QwtPlotCurve,QwtPlotItem

from guiqwt.plot import ImageWidget,CurveDialog,PlotManager
from guiqwt.curve import CurvePlot,  ErrorBarCurveItem,\
        CurveItem#PlotItemList,

from guiqwt.builder import make

# QComboBox,QVBoxLayout, 
from PyQt5.QtGui import (QStandardItemModel,QStandardItem,QScreen\
                         
    )


#sys.path.append('D:\\py\\xuexiyige\\ui\\net\\')
#sys.path.append('D:\\py\\xuexiyige\\statistics\\')


import xml.etree.ElementTree as ET

from guiqwt.interfaces import (IBasePlotItem, IDecoratorItemType,
                               ISerializableType, ICurveItemType,
                               ITrackableItemType, IPanel)
from guidata.configtools import get_icon, get_image_layout
from guidata.qthelpers import create_action, add_actions
from guiqwt.panels import PanelWidget, ID_ITEMLIST
from guiqwt.shapes import Marker
from guiqwt.config import CONF, _
from guiqwt.styles import GridParam
from guiqwt.curve import GridItem





class tableview(QTableView):
    click_sig = pyqtSignal(int)
    def __init__(self,parent=None):
        super(tableview,self).__init__(parent)
        self.setContextMenuPolicy(Qt.CustomContextMenu)
        self.customContextMenuRequested.connect(self.showMenu)
        self.contextMenu = QMenu(self)
        self.rownum = -1
        
        
    def showMenu(self,pos):
        self.contextMenu.exec_(QCursor.pos())
        
#    def mouseClickEvent(self,event):
    def mousePressEvent(self,event):
        if event.buttons() == Qt.RightButton:
            pos = event.pos()
            item = self.indexAt(pos)
            self.rownum = item.row()
            self.click_sig.emit(self.rownum)
            print('at %d %d'%(item.row(),item.column()))
        QTableView.mousePressEvent(self,event)
    
#    def RightClickEvent(self,event):
    def mouseDoubleClickEvent(self,event):
#        global rownum
        pos = event.pos()
        item = self.indexAt(pos)
        self.rownum = item.row()
        self.click_sig.emit(self.rownum)
        if item:
            print('at %d %d'%(item.row(),item.column()))
            
            
class colormapTool(guiqwt.tools.CommandTool):
    def __init__(self, manager):
        super(colormapTool, self).__init__(manager,_("Colormap"),
                                           tip=_("Select colormap for active "
                                                 "image"),
                                           toolbar_id=None)

        self.action.setEnabled(False)
        self.action.setIconText("")
        self.default_icon = build_icon_from_cmap(get_cmap("jet"),
                                                 width=16, height=16)
        self.action.setIcon(self.default_icon)

    def create_action_menu(self, manager):
        """Create and return menu for the tool's action"""
        menu = QMenu()
        for cmap_name in get_colormap_list():
            cmap = get_cmap(cmap_name)
            icon = build_icon_from_cmap(cmap)
            action = menu.addAction(icon, cmap_name)
            action.setEnabled(True)
        return menu



class ZoomRectHandler(guiqwt.events.RectangularSelectionHandler):
    def stop_moving_action(self, filter, event):
        filter.plot.do_choose_rect_view(self.start, event.pos())
        
class SelectPointTool(guiqwt.tools.InteractiveTool):
    TITLE = _("Point selection")
    ICON = "point_selection.png"
    MARKER_STYLE_SECT = "plot"
    MARKER_STYLE_KEY = "marker/curve"
    CURSOR = Qt.PointingHandCursor
    
    def __init__(self, manager, mode="reuse", on_active_item=False,
                 title=None, icon=None, tip=None, end_callback=None,
                 toolbar_id=guiqwt.tools.DefaultToolbarID, marker_style=None,
                 switch_to_default_tool=None):
        super(SelectPointTool, self).__init__(manager, toolbar_id,
                              title=title, icon=icon, tip=tip,
                              switch_to_default_tool=switch_to_default_tool)
        assert mode in ("reuse", "create")
        self.mode = mode
        self.end_callback = end_callback
        self.marker = None
        self.last_pos = None
        self.on_active_item = on_active_item
        if marker_style is not None:
            self.marker_style_sect = marker_style[0]
            self.marker_style_key = marker_style[1]
        else:
            self.marker_style_sect = self.MARKER_STYLE_SECT
            self.marker_style_key = self.MARKER_STYLE_KEY
    
    def set_marker_style(self, marker):
        marker.set_style(self.marker_style_sect, self.marker_style_key)
    
    def setup_filter(self, baseplot):
        filter = baseplot.filter
        # Initialisation du filtre
        start_state = filter.new_state()
        # Bouton gauche :
        handler = guiqwt.events.QtDragHandler(filter, Qt.LeftButton, start_state=start_state)
        handler.SIG_START_TRACKING.connect(self.start)
        handler.SIG_MOVE.connect(self.move)
        handler.SIG_STOP_NOT_MOVING.connect(self.stop)
        handler.SIG_STOP_MOVING.connect(self.stop)
        return guiqwt.tools.setup_standard_tool_filter(filter, start_state)

    def start(self, filter, event):
        if self.marker is None:
            title = ""
            if self.TITLE:
                title = "<b>%s</b><br>" % self.TITLE
            if self.on_active_item:
                constraint_cb = filter.plot.on_active_curve
                label_cb = lambda x, y: title + \
                           filter.plot.get_coordinates_str(x, y)
            else:
                constraint_cb = None
                label_cb = lambda x, y: \
                           "%sx = %g<br>y = %g" % (title, x, y)
            self.marker = Marker(label_cb=label_cb,
                                 constraint_cb=constraint_cb)
            self.set_marker_style(self.marker)
        self.marker.attach(filter.plot)
        self.marker.setZ(filter.plot.get_max_z()+1)
        self.marker.setVisible(True)

    def stop(self, filter, event):
        self.move( filter, event )
        if self.mode != "reuse":
            self.marker.detach()
            self.marker = None
        if self.end_callback:
            self.end_callback(self)

    def move(self, filter, event):
        if self.marker is None:
            return # something is wrong ...
        self.marker.move_local_point_to( 0, event.pos() )
        filter.plot.replot()
        self.last_pos = self.marker.xValue(), self.marker.yValue()

    def get_coordinates(self):
        return self.last_pos
        
class ChooseTool(guiqwt.tools.InteractiveTool):
    TITLE = _("Rectangle choose")
    ICON = os.path.dirname(os.path.realpath(sys.argv[0]))+"\\tick.png"
    
    def setup_filter(self, baseplot):

        filter = baseplot.filter
        start_state = filter.new_state()
        handler = ZoomRectHandler(filter, Qt.LeftButton,
                                  start_state=start_state)
        shape, h0, h1 = self.get_shape()
        handler.set_shape(shape, h0, h1)
        return guiqwt.events.setup_standard_tool_filter(filter, start_state)
    
    def get_shape(self):
        shape = guiqwt.shapes.RectangleShape(0, 0, 1, 1)
        shape.set_style("plot", "shape/rectzoom")
        return shape, 0, 2
       
class ItemListWidget(QListWidget):
    """
    PlotItemList
    List of items attached to plot
    """
    def __init__(self, parent):
        super(ItemListWidget, self).__init__(parent)
        
        self.manager = None
        self.plot = None # the default plot...
        self.plots = None
        self.items = []
        
        self.currentRowChanged.connect(self.current_row_changed)
        self.itemChanged.connect(self.item_changed)
        self.itemSelectionChanged.connect(self.refresh_actions)
        self.itemSelectionChanged.connect(self.selection_changed)
        
        self.setWordWrap(True)
        self.setMinimumWidth(140)
        self.setSelectionMode(QListWidget.ExtendedSelection)
        
        # Setup context menu
        self.menu = QMenu(self)
        self.menu_actions = self.setup_actions()
        self.refresh_actions()
        add_actions(self.menu, self.menu_actions)

    def register_panel(self, manager):
        self.manager = manager

        for plot in self.manager.get_plots():
            plot.SIG_ITEMS_CHANGED.connect(self.items_changed)
            plot.SIG_ACTIVE_ITEM_CHANGED.connect(self.items_changed)
        self.plot = self.manager.get_plot()

    def contextMenuEvent(self, event):
        """Override Qt method"""
        self.refresh_actions()
        self.menu.popup(event.globalPos())
                     
    def setup_actions(self):
        self.movedown_ac = create_action(self, _("Move to back"),
                                     icon=get_icon('arrow_down.png'),
                                     triggered=lambda: self.move_item("down"))
        self.moveup_ac = create_action(self, _("Move to front"),
                                       icon=get_icon('arrow_up.png'),
                                       triggered=lambda: self.move_item("up"))
        settings_ac = create_action(self, _("Parameters..."),
                    icon=get_icon('settings.png'),
                    triggered=self.edit_plot_parameters )
        self.remove_ac = create_action(self, _("Remove"),
                                       icon=get_icon('trash.png'),
                                       triggered=self.remove_item)
        return [self.moveup_ac, self.movedown_ac, None,
                settings_ac, self.remove_ac]

    def edit_plot_parameters(self):
        self.plot.edit_plot_parameters("item")
    
    def __is_selection_contiguous(self):
        indexes = sorted([self.row(lw_item) for lw_item
                          in self.selectedItems()])
        return len(indexes) <= 1 or list(range(indexes[0], indexes[-1]+1)) == indexes
        
    def get_selected_items(self):
        """Return selected QwtPlot items
        
        .. warning::

            This is not the same as 
            :py:data:`guiqwt.baseplot.BasePlot.get_selected_items`.
            Some items could appear in itemlist without being registered in 
            plot widget items (in particular, some items could be selected in 
            itemlist without being selected in plot widget)
        """
        length = len(self.plot.items)
        return [length-1-self.row(lw_item)
                for lw_item in self.selectedItems()]
        
    def refresh_actions(self):
        is_selection = len(self.selectedItems()) > 0
        for action in self.menu_actions:
            if action is not None:
                action.setEnabled(is_selection)
        if is_selection:
            remove_state = True
            for item in [self.items[idx] for idx in self.get_selected_items()]:
                remove_state = remove_state and not item.is_readonly()
            self.remove_ac.setEnabled(remove_state)
            for action in [self.moveup_ac, self.movedown_ac]:
                action.setEnabled(self.__is_selection_contiguous())            
        
    def __get_item_icon(self, item):
        from guiqwt.label import LegendBoxItem, LabelItem
        from guiqwt.annotations import (AnnotatedShape, AnnotatedRectangle,
                                        AnnotatedCircle, AnnotatedEllipse,
                                        AnnotatedPoint, AnnotatedSegment)
        from guiqwt.shapes import (SegmentShape, RectangleShape, EllipseShape,
                                   PointShape, PolygonShape, Axes,
                                   XRangeSelection)
        from guiqwt.image import (BaseImageItem, Histogram2DItem,
                                  ImageFilterItem)
        from guiqwt.histogram import HistogramItem

        icon_name = 'item.png'
        for klass, icon in ((HistogramItem, 'histogram.png'),
                            (ErrorBarCurveItem, 'errorbar.png'),
                            (CurveItem, 'curve.png'),
                            (GridItem, 'grid.png'),
                            (LegendBoxItem, 'legend.png'),
                            (LabelItem, 'label.png'),
                            (AnnotatedSegment, 'segment.png'),
                            (AnnotatedPoint, 'point_shape.png'),
                            (AnnotatedCircle, 'circle.png'),
                            (AnnotatedEllipse, 'ellipse_shape.png'),
                            (AnnotatedRectangle, 'rectangle.png'),
                            (AnnotatedShape, 'annotation.png'),
                            (SegmentShape, 'segment.png'),
                            (RectangleShape, 'rectangle.png'),
                            (PointShape, 'point_shape.png'),
                            (EllipseShape, 'ellipse_shape.png'),
                            (Axes, 'gtaxes.png'),
                            (Marker, 'marker.png'),
                            (XRangeSelection, 'xrange.png'),
                            (PolygonShape, 'freeform.png'),
                            (Histogram2DItem, 'histogram2d.png'),
                            (ImageFilterItem, 'funct.png'),
                            (BaseImageItem, 'image.png'),):
            if isinstance(item, klass):
                icon_name = icon
                break
        return get_icon(icon_name)
        
    def items_changed(self, plot):
        """Plot items have changed"""
        active_plot = self.manager.get_active_plot()
        if active_plot is not plot:
            return
        self.plot = plot
        _block = self.blockSignals(True)
        active = plot.get_active_item()
        self.items = plot.get_public_items(z_sorted=True)
        self.clear()
        for item in self.items:
            title = item.title().text()
#            print('title:',title)
            lw_item = QListWidgetItem(self.__get_item_icon(item), title, self)
            lw_item.setCheckState(Qt.Checked if item.isVisible()
                                  else Qt.Unchecked)
            lw_item.setSelected(item.selected)
            font = lw_item.font()
            if item is active:
                font.setItalic(True)
            else:
                font.setItalic(False)
            lw_item.setFont(font)
            self.addItem(lw_item)
#            self.sortItems(Qt.AscendingOrder)
        self.refresh_actions()
        self.blockSignals(_block)
            
    def current_row_changed(self, index):
        """QListWidget current row has changed"""
        if index == -1:
            return
        item = self.items[index]
        if not item.can_select():
            item = None
        if item is None:
            self.plot.replot()
                
    def selection_changed(self):
        item_idxs = self.get_selected_items()
        if self.plots is None:return
        for plot in self.plots:
#            print(plot.items)
            
            plot.select_some_items([plot.items[idx] for idx in item_idxs])
            plot.replot()
        
    def item_changed(self, listwidgetitem):
        """QListWidget item has changed"""
        item = self.items[self.row(listwidgetitem)]
        idx = self.row(listwidgetitem)
        visible = listwidgetitem.checkState() == Qt.Checked
#        if visible != item.isVisible():
#            self.plot.set_item_visible(item, visible)
        for plot in self.plots:
            length = len(plot.items)
            item = plot.items[length-1-idx]
#            print(item.title().text())
            if visible != item.isVisible():
                plot.set_item_visible(item,visible)
    
    def move_item(self, direction):
        """Move item to the background/foreground
        Works only for contiguous selection
        -> 'refresh_actions' method should guarantee that"""
        items = [self.items[idx] for idx in self.get_selected_items()]
        if direction == 'up':
            self.plot.move_up(items)
        else:
            self.plot.move_down(items)
        # Re-select items which can't be selected in plot widget but can be 
        # selected in ItemListWidget:
        for item in items:
            lw_item = self.item(self.items.index(item))
            if not lw_item.isSelected():
                lw_item.setSelected(True)
        self.plot.replot()
        
    def remove_item(self):
        if len(self.selectedItems()) == 1:
            message = _("Do you really want to remove this item?")
        else:
            message = _("Do you really want to remove selected items?")
        answer = QMessageBox.warning(self, _("Remove"), message,
                                     QMessageBox.Yes | QMessageBox.No)
        if answer == QMessageBox.Yes:
            items = [self.items[idx] for idx in self.get_selected_items()]
            self.plot.del_items(items)
            self.plot.replot()
        

class PlotItemList(PanelWidget):
    """Construct the `plot item list panel`"""
    __implements__ = (IPanel,)
    PANEL_ID = ID_ITEMLIST
    PANEL_TITLE = _("Item list")
    PANEL_ICON = "item_list.png"
    
    def __init__(self, parent):
        super(PlotItemList, self).__init__(parent)
        self.manager = None
        
        vlayout = QVBoxLayout()
        self.setLayout(vlayout)
        
        style = "<span style=\'color: #444444\'><b>%s</b></span>"
        layout, _label = get_image_layout(self.PANEL_ICON,
                                          style % self.PANEL_TITLE,
                                          alignment=Qt.AlignCenter)
        vlayout.addLayout(layout)
        self.listwidget = ItemListWidget(self)
        vlayout.addWidget(self.listwidget)
        
        toolbar = QToolBar(self)
        vlayout.addWidget(toolbar)
        add_actions(toolbar, self.listwidget.menu_actions)

    def register_panel(self, manager):
        """Register panel to plot manager"""
        self.manager = manager
        self.listwidget.register_panel(manager)
                         
    def configure_panel(self):
        """Configure panel"""
        pass
  
import guidata
class HomeTool(guiqwt.tools.CommandTool):
#    TITLE = _("Home")
#    ICON = "home.png"
    def __init__(self, manager, toolbar_id=guiqwt.tools.DefaultToolbarID):
        super(HomeTool, self).__init__(manager, _("Home"),
             guidata.configtools.get_icon(
                os.path.dirname(os.path.realpath(sys.argv[0]))+"\home.png"),
                                       toolbar_id=toolbar_id)                                  
    def activate_command(self, plot, checked):
        plot.do_home()
        
class MyCurvePlot(CurvePlot):
    SIG_RECTCHOOSE_CHANGED = pyqtSignal(list)
    def __init__(self, parent=None, title=None, xlabel=None, ylabel=None,
                 xunit=None, yunit=None, gridparam=None,
                 section="plot", axes_synchronised=False):
        super(CurvePlot, self).__init__(parent, section)

        self.axes_reverse = [False]*4
        
        self.set_titles(title=title, xlabel=xlabel, ylabel=ylabel,
                        xunit=xunit, yunit=yunit)
                
        self.antialiased = False

        self.set_antialiasing(CONF.get(section, "antialiasing"))
        
        self.axes_synchronised = axes_synchronised
        
        # Installing our own event filter:
        # (qwt's event filter does not fit our needs)
        self.canvas().installEventFilter(self.filter)
        self.canvas().setMouseTracking(True)
    
        self.cross_marker = Marker()
        self.curve_marker = Marker(label_cb=self.get_coordinates_str,
                                   constraint_cb=self.on_active_curve)
        self.cross_marker.set_style(section, "marker/cross")
        self.curve_marker.set_style(section, "marker/curve")
        self.cross_marker.setVisible(False)
        self.curve_marker.setVisible(False)
        self.cross_marker.attach(self)
        self.curve_marker.attach(self)

        # Background color
        self.setCanvasBackground(Qt.white)        
        
        self.curve_pointer = False
        self.canvas_pointer = False
        
        # Setting up grid
        if gridparam is None:
            gridparam = GridParam(title=_("Grid"), icon="grid.png")
            gridparam.read_config(CONF, section, "grid")
        self.grid = GridItem(gridparam)
        self.add_item(self.grid, z=-1)
        self.all_data = {}#[np.arange(100),np.random.rand(100)]
        self.chosen_item = None

    
    def get_all_data(self):
        return self.all_data
    def set_all_data(self,x,y,key):
        '''x:xdata  array
        y:ydata    array
        key:index  '''
        self.all_data[key] = [x,y]

    def add_item(self, item, z=None):
        """
        Add a *plot item* instance to this *plot widget*
        
            * item: :py:data:`qwt.QwtPlotItem` object implementing
              the :py:data:`guiqwt.interfaces.IBasePlotItem` interface
            * z: item's z order (None -> z = max(self.get_items())+1)
        """
        if isinstance(item, QwtPlotCurve):
            item.setRenderHint(QwtPlotItem.RenderAntialiased, self.antialiased)
            idx = len(self.items)
            self.set_all_data(item.get_data()[0],item.get_data()[1],idx)
        BasePlot.add_item(self, item, z)
        
    def __clean_item_references(self, item):
        """Remove all reference to this item (active,
        last_selected"""
        if item is self.active_item:
            self.active_item = None
            self._active_xaxis = self.DEFAULT_ACTIVE_XAXIS
            self._active_yaxis = self.DEFAULT_ACTIVE_YAXIS
        for key, it in list(self.last_selected.items()):
            if item is it:
                del self.last_selected[key]
    
    def del_items(self, items):
        """Remove item from widget"""
        items = items[:] # copy the list to avoid side effects when we empty it
        active_item = self.get_active_item()
        while items:
            item = items.pop()
            key = self.items.index(item) 
            item.detach()
            # raises ValueError if item not in list
            self.items.remove(item)
            try:
                self.all_data.pop(key)
            except (KeyError,UnboundLocalError):
                pass
            self.__clean_item_references(item)
            self.SIG_ITEM_REMOVED.emit(item)
        self.SIG_ITEMS_CHANGED.emit(self)
        if active_item is not self.get_active_item():
            self.SIG_ACTIVE_ITEM_CHANGED.emit(self)
        
    def del_item(self, item):
        """
        Remove item from widget
        Convenience function (see 'del_items')
        """
        try:
            key = self.items.index(item)
            self.del_items([item])
            self.all_data.pop(key)
        except ValueError:
            raise ValueError("item not in plot")
        except KeyError:
            pass
        
    def del_all_items(self, except_grid=True):
        """Del all items, eventually (default) except grid"""
        items = [item for item in self.items
                 if not except_grid or item is not self.grid]
        self.del_items(items)
        self.all_data = {}
        
    def do_autoscale(self, replot=True, axis_id=None):
        """Do autoscale on all axes"""
        auto = self.autoReplot()
        self.setAutoReplot(False)
        # XXX implement the case when axes are synchronised
        for axis_id in self.AXIS_IDS if axis_id is None else [axis_id]:
            vmin, vmax = None, None
            if not self.axisEnabled(axis_id):
                continue
            for item in self.get_items():
                if isinstance(item, self.AUTOSCALE_TYPES) \
                   and not item.is_empty() and item.isVisible():
                    bounds = item.boundingRect()
                    if axis_id == item.xAxis():
                        xmin, xmax = bounds.left(), bounds.right()
                        if vmin is None or xmin < vmin:
                            vmin = xmin
                        if vmax is None or xmax > vmax:
                            vmax = xmax
                    elif axis_id == item.yAxis():
                        ymin, ymax = bounds.top(), bounds.bottom()
                        if vmin is None or ymin < vmin:
                            vmin = ymin
                        if vmax is None or ymax > vmax:
                            vmax = ymax
            if vmin is None or vmax is None:
                continue
            if vmin == vmax: # same behavior as MATLAB
                vmin -= 1
                vmax += 1
            elif self.get_axis_scale(axis_id) == 'lin':
                dv = vmax-vmin
                vmin -= .002*dv
                vmax += .002*dv
            elif vmin > 0 and vmax > 0: # log scale
                dv = np.log10(vmax)-np.log10(vmin)
                vmin = 10**(np.log10(vmin)-.002*dv)
                vmax = 10**(np.log10(vmax)+.002*dv)
            self.set_axis_limits(axis_id, vmin, vmax)
            
        self.setAutoReplot(auto)
        if replot:
            try:
                xmin,xmax = self.get_axis_limits(2)
                ymin,ymax = self.get_axis_limits(0)
                all_data = self.get_all_data()
                for i in all_data.keys():
                    x,y = all_data[i][0],all_data[i][1]
                    idx1 = np.where( (x<=xmax) & (x>=xmin) )[0]
                    idx2 = np.where( (y<=ymax) & (y>=ymin) )[0]
                    idx = idx1#np.intersect1d(idx1,idx2)
#                    print('item NO:',i,isinstance(self.items[i],QwtPlotItem))
                    x,y = all_data[i][0][idx],all_data[i][1][idx]
                    self.items[i].set_data( x , y )
            except:
                print('######auto_scale error')
                traceback.print_exc()
                pass
            self.replot()
        self.SIG_PLOT_AXIS_CHANGED.emit(self)
        
    def do_choose_rect_view(self, start, end):
        x1, y1 = start.x(), start.y()
        x2, y2 = end.x(), end.y()
        xaxis, yaxis = self.get_active_axes()
        active_axes = [ (x1, x2, xaxis),
                        (y1, y2, yaxis) ]
        xmin,xmax,ymin,ymax = x1,x2,y1,y2
        if self.chosen_item in self.items:
            self.del_item(self.chosen_item)
        for h1, h2, k in active_axes:
            o1 = self.invTransform(k, h1)
            o2 = self.invTransform(k, h2)
            if o1 > o2:
                o1, o2 = o2, o1
            if o1 == o2:
                continue
            if self.get_axis_direction(k):
                o1, o2 = o2, o1
            if k == 0:
                ymin,ymax = o1,o2
            elif k == 2:
                xmin,xmax = o1,o2  
#            print(h1,h2,k,o1,o2)
            #k=0:yLeft  k=2:xBottom
        
        all_data = self.get_all_data()
        idx_all = []
        arr_x, arr_y = np.array([]), np.array([])
        for i in all_data.keys():
            x,y = all_data[i][0],all_data[i][1]
            idx1 = np.where( (y <= ymax) & (y >= ymin) )[0]
            idx2 = np.where( (x <= xmax) & (x >= xmin) )[0]
            idx = np.intersect1d(idx1,idx2)
            idx_all.append(i)
            idx_all.append(idx)
            x,y = all_data[i][0][idx],all_data[i][1][idx]
            arr_x,arr_y = np.hstack((arr_x,x)),np.hstack((arr_y,y))
        try:
            x
        except:
            return
#        print(all_data.keys(),type(x))
        try:
            style = self.items[0].style()
        except:
            style = self.items[1].style()
        # style()返回整数值  choice 为列表
        self.chosen_item = make.curve(arr_x,arr_y,color='#66ccff',linewidth=4,\
        curvestyle = guiqwt.styles.CURVESTYLE_CHOICES[style][0])
        self.add_item_with_z_offset(self.chosen_item,self.get_max_z()+1)
#        print(len(x),xmax,xmin,ymax,ymin)
#            self.items[i].set_data(x,y)
        self.replot()
        self.SIG_RECTCHOOSE_CHANGED.emit(idx_all)

    def do_zoom_rect_view(self, start, end):
        # XXX implement the case when axes are synchronised
        x1, y1 = start.x(), start.y()
        x2, y2 = end.x(), end.y()
        xaxis, yaxis = self.get_active_axes()
        active_axes = [ (x1, x2, xaxis),
                        (y1, y2, yaxis) ]
        xmin,xmax,ymin,ymax = x1,x2,y1,y2
        for h1, h2, k in active_axes:
            o1 = self.invTransform(k, h1)
            o2 = self.invTransform(k, h2)
            if o1 > o2:
                o1, o2 = o2, o1
            if o1 == o2:
                continue
            if self.get_axis_direction(k):
                o1, o2 = o2, o1
            #k=0:yLeft  k=2:xBottom
            self.setAxisScale(k, o1, o2)
        
        all_data = self.get_all_data()
        for i in all_data.keys():
            x,y = all_data[i][0],all_data[i][1]
            idx1 = np.where( (y<=ymax) & (y>=ymin) )[0]
            idx2 = np.where( (x<=xmax) & (x>=xmin) )[0]
            idx = idx2#np.intersect1d(idx1,idx2)
            x,y = all_data[i][0][idx],all_data[i][1][idx]
            self.items[i].set_data(x,y)
        self.do_autoscale()
        self.replot()
        self.SIG_PLOT_AXIS_CHANGED.emit(self)    
        
    def do_home(self):
        all_data = self.get_all_data()
        for i in all_data.keys():
            x,y = all_data[i][0],all_data[i][1]
            self.items[i].set_data(x,y)
        self.do_autoscale()
        self.replot()
        self.SIG_PLOT_AXIS_CHANGED.emit(self)    
        

     
        
DoubleValidator = QDoubleValidator()
DoubleValidator.setRange(380,40000)
DoubleValidator.setNotation(QDoubleValidator.StandardNotation)
DoubleValidator.setDecimals(2)
        
class LineEdit(QLineEdit):
    def __init__(self,parent=None):
        super(QLineEdit,self).__init__(parent)
        self.setAcceptDrops(1)
        self.setClearButtonEnabled(1)
        
    def dragEnterEvent(self,event):
        event.accept()
        
    def dropEvent(self,event):
        st = str(event.mimeData().urls())
        st = st.replace("[PyQt5.QtCore.QUrl('file:///",'')
        st = st.replace('),','')
        st = st.replace("[PyQt5.QtCore.QUrl(u'file:///",'')
        st = st.replace("')]",'')
        self.setText(st)

class popwnd(QWidget):
    def __init__(self,parent=None):
        super().__init__(parent)
        self.init()
        self.postprocess()
    def init(self):
#        wndlayout = QVBoxLayout()
#        self.setLayout(wndlayout)
        pass

    def handle_click(self):
        if not self.isVisible():
            self.show()
        self.activateWindow()

    def handle_close(self):
        self.close()
    def postprocess(self):
        pass


class Common_Filter(popwnd):
    
    def init(self):
        self.setWindowTitle('滤波设置')
        self.checkall = QCheckBox()
        
        self.cb = []
        self.vlayout = QGridLayout()
        self.vlayout.addWidget(QLabel('滤波设置'),1,0)
        self.vlayout.addWidget(self.checkall,1,4)
        grid = QGridLayout()
        self.vlayout.addLayout(grid,2,0)
        self.filter_in_result = QCheckBox('在结果中滤波')
        self.vlayout.addWidget(self.filter_in_result,3,0)
        self.flt_btn = QPushButton('开始滤波')
        self.vlayout.addWidget(self.flt_btn,4,1,1,2)
        self.setLayout(self.vlayout)
        self.checkall.clicked.connect(self.checkallitem)

    def checkallitem(self):
        try:
            self.check
        except:
            return
        if self.checkall.isChecked():
            for i in range(len(self.check)):
                self.check[i].setChecked(1)
        else:
            for i in range(len(self.check)):
                self.check[i].setChecked(0)
    def reset_filter(self):
        grid = QGridLayout()
        self.le_min, self.le_max, self.check = [],[],[]
        for i,name in enumerate(self.cb):
            grid.addWidget(QLabel(str(self.cb[i])+'下限'),i,1)
            self.le_min.append(QLineEdit())
            grid.addWidget(self.le_min[i],i,2)
            grid.addWidget(QLabel(str(self.cb[i])+'上限'),i,3)
            self.le_max.append(QLineEdit())
            grid.addWidget(self.le_max[i],i,4)
            self.check.append(QCheckBox())
            grid.addWidget(self.check[i],i,5)
            
        self.vlayout.addLayout(grid,2,0)
    
class Pdw_Filter(Common_Filter):
    
    def init(self):
        self.setWindowTitle('滤波设置')
        self.checkall = QCheckBox()
        
        self.cb = []
        self.vlayout = QGridLayout()
        self.vlayout.addWidget(QLabel('滤波设置'),1,0)
        self.vlayout.addWidget(self.checkall,1,4)
        grid = QGridLayout()
        self.vlayout.addLayout(grid,2,0)
        self.reset_file_btn = QPushButton('重置文件指针')
        self.vlayout.addWidget(self.reset_file_btn,3,0)
        self.flt_btn = QPushButton('开始传送')
        self.vlayout.addWidget(self.flt_btn,4,3,1,2)
        self.setLayout(self.vlayout)
        self.checkall.clicked.connect(self.checkallitem)

    def reset_filter(self):
        grid = QGridLayout()
        self.le_min, self.le_max, self.check = [],[],[]
        for i,name in enumerate(self.cb):
            grid.addWidget(QLabel(str(self.cb[i])+'下限'),i,1)
            self.le_min.append(QLineEdit())
            grid.addWidget(self.le_min[i],i,2)
            grid.addWidget(QLabel(str(self.cb[i])+'上限'),i,3)
            self.le_max.append(QLineEdit())
            grid.addWidget(self.le_max[i],i,4)
            self.check.append(QCheckBox())
            grid.addWidget(self.check[i],i,5)
        grid.addWidget(QLabel('最大脉冲数'),i+1,1)
        self.pls_num_le = QLineEdit()
        grid.addWidget(self.pls_num_le,i+1,2)
        
        grid.addWidget(QLabel('最大持续时间'),i+1,3)
        self.pls_time_le = QLineEdit()
        grid.addWidget(self.pls_time_le,i+1,4)
        
        self.vlayout.addLayout(grid,2,0)

class filterbox(QWidget):

    def __init__(self,parent=None):
        super(filterbox,self).__init__(parent)
        self.setWindowTitle('filter')
        filter_box = QVBoxLayout()
        self.setLayout(filter_box)
        filter_pdwbox = QGridLayout()

        filter_box.addLayout(filter_pdwbox,1)

        filter_pdwbox.addWidget(QLabel('滤波设置'),0,1)
        self.flt_all_cb = QCheckBox()
        filter_pdwbox.addWidget(self.flt_all_cb,0,5)

        filter_pdwbox.addWidget(QLabel('载频下限/MHz'),1,1)
        self.rf_least_le = QLineEdit('380')
        self.rf_least_le.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)
        filter_pdwbox.addWidget(self.rf_least_le,1,2)
        filter_pdwbox.addWidget(QLabel('载频上限/MHz'),1,3)
        self.rf_most_le = QLineEdit('40000')
        self.rf_most_le.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)
        filter_pdwbox.addWidget(self.rf_most_le,1,4)
        self.flt_rf_cb = QCheckBox()
        filter_pdwbox.addWidget(self.flt_rf_cb,1,5)
        filter_pdwbox.addWidget(QLabel(''),1,6)

        filter_pdwbox.addWidget(QLabel('脉宽下限/us'),2,1)
        self.pw_least_le = QLineEdit('0')
        self.pw_least_le.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)
        filter_pdwbox.addWidget(self.pw_least_le,2,2)
        filter_pdwbox.addWidget(QLabel('脉宽上限/us'),2,3)
        self.pw_most_le = QLineEdit('1024')
        self.pw_most_le.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)
        filter_pdwbox.addWidget(self.pw_most_le,2,4)
        self.flt_pw_cb = QCheckBox()
        filter_pdwbox.addWidget(self.flt_pw_cb,2,5)

        filter_pdwbox.addWidget(QLabel('方位下限/°'),3,1)
        self.doa_least_le = QLineEdit('0')
        self.doa_least_le.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)
        filter_pdwbox.addWidget(self.doa_least_le,3,2)
        filter_pdwbox.addWidget(QLabel('方位上限/°'),3,3)
        self.doa_most_le = QLineEdit('360')
        self.doa_most_le.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)
        filter_pdwbox.addWidget(self.doa_most_le,3,4)
        self.flt_doa_cb = QCheckBox()
        filter_pdwbox.addWidget(self.flt_doa_cb,3,5)

        filter_pdwbox.addWidget(QLabel('幅度下限/dBm'),4,1)
        self.pa_least_le = QLineEdit('0')
        self.pa_least_le.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)
        filter_pdwbox.addWidget(self.pa_least_le,4,2)
        filter_pdwbox.addWidget(QLabel('幅度上限/dBm'),4,3)
        self.pa_most_le = QLineEdit('255')
        self.pa_most_le.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)
        filter_pdwbox.addWidget(self.pa_most_le,4,4)
        self.flt_pa_cb = QCheckBox()
        filter_pdwbox.addWidget(self.flt_pa_cb,4,5)

        filter_pdwbox.addWidget(QLabel('反射滤波设置'),5,1)
        filter_pdwbox.addWidget(QLabel('滤波时间/us'),6,1)
        self.flt_time_le = QLineEdit('0')
        self.flt_time_le.setSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred)
        filter_pdwbox.addWidget(self.flt_time_le,6,2)
        filter_pdwbox.addWidget(QLabel('滤波模式'),6,3)
        self.flt_mode_comb = QComboBox()
        self.flt_mode_comb.addItem('滤除所有方位')
        self.flt_mode_comb.addItem('滤除不同方位')
        filter_pdwbox.addWidget(self.flt_mode_comb,6,4)
        
        filter_pdwbox.addWidget(QLabel('在结果中滤波'),7,1)
        self.filinre = QCheckBox()
        filter_pdwbox.addWidget(self.filinre,7,2)
        self.filterbtn = QPushButton('开始滤波')
        filter_pdwbox.addWidget(self.filterbtn,8,2,1,2)
        self.filterbtn.clicked.connect(self.setFilter)
        self.flt_all_cb.clicked.connect(self.check_all)


        
    def handle_click(self):
        if not self.isVisible():
            self.show()
        self.activateWindow()
        
    def check_all(self):
        if not self.flt_all_cb.isChecked():
            self.flt_pa_cb.setChecked(0)
            self.flt_pw_cb.setChecked(0)
            self.flt_rf_cb.setChecked(0)
            self.flt_doa_cb.setChecked(0)
        else:
            self.flt_pa_cb.setChecked(1)
            self.flt_pw_cb.setChecked(1)
            self.flt_rf_cb.setChecked(1)
            self.flt_doa_cb.setChecked(1)
            

    def setFilter(self):
        self.doa_limit = (float(self.doa_least_le.text()),float(self.doa_most_le.text()))
        self.rf_limit = (float(self.rf_least_le.text()),float(self.rf_most_le.text()))
        self.pa_limit = (float(self.pa_least_le.text()),float(self.pa_most_le.text()))
        self.pw_limit = (float(self.pw_least_le.text()),float(self.pw_most_le.text()))

    def handle_close(self):
        self.close()
        
class SaveMoreWnd(QWidget):
    def __init__(self,parent=None):
        super().__init__(parent)
        self.init()
    def init(self):
        self.setWindowTitle('保存高级设置')
        self.wndlayout = QGridLayout()
        self.setLayout(self.wndlayout)
        self.lot= QGridLayout()
        self.wndlayout.addWidget(QLabel('属性名'),1,1)
        self.check_all_cb =QCheckBox('全选') 
        self.wndlayout.addWidget(self.check_all_cb,1,2)
        self.wndlayout.addLayout(self.lot,2,1,1,2)
        self.saveBtn = QPushButton('保存')
        self.wndlayout.addWidget(self.saveBtn,3,1)
        self.checkbox = []
        self.InitConnect()

    def InitConnect(self):
        self.check_all_cb.clicked.connect(self.setCheck)
#        
#        
    def setCheck(self):
        if self.check_all_cb.isChecked():
            for i in self.checkbox:
                i.setChecked(1)
            print('all_chosen')
        else:
            for i in self.checkbox:
                i.setChecked(0)
        
class DrawMoreWnd(QWidget):
    def __init__(self,parent=None):
        super().__init__(parent)
        self.init()
    def init(self):
        self.setWindowTitle('绘图高级设置')
        wndlayout = QGridLayout()
        self.setLayout(wndlayout)
        
        wndlayout.addWidget(QLabel('全景图设置'),3,1)
        wndlayout.addWidget(QLabel('颜色区分标志值'),3,2)
        self.flag_draw_cb = QCheckBox('启用')
        self.flag_draw_cb.setChecked(0)
        wndlayout.addWidget(self.flag_draw_cb,3,3)
        
        wndlayout.addWidget(QLabel('标志种类选择'),4,2)
        self.flag_cb = QComboBox()
        self.flag_cb.setEnabled(0)
        FLAGS = ['DOAflag','TOAflag','CWflag','Watchflag','Jamflag','PWflag']
        for i in FLAGS:
            self.flag_cb.addItem(i)
        wndlayout.addWidget(self.flag_cb,4,3)
        
        wndlayout.addWidget(QLabel('横轴选择'),5,2)
        self.xlabel_cb = QComboBox()
        self.xlabel_cb.setEnabled(1)
        xlabels = ['TOA_all/us','TOA/us','NUM']
        for i in xlabels:
            self.xlabel_cb.addItem(i)
        wndlayout.addWidget(self.xlabel_cb,5,3)
        
        wndlayout.addWidget(QLabel('全图绘制'),6,2)
        self.onelayout_cb = QCheckBox('启用')
        self.onelayout_cb.setChecked(1)
        wndlayout.addWidget(self.onelayout_cb,6,3)
        
        wndlayout.addWidget(QLabel('直方图设置'),7,1)
        wndlayout.addWidget(QLabel('直方图绘制'),7,2)
        self.hist_cb = QCheckBox('启用')
        wndlayout.addWidget(self.hist_cb,7,3)
        
        wndlayout.addWidget(QLabel('自定义设置'),8,1)
        wndlayout.addWidget(QLabel('横轴选择'),8,2)
        self.sdefine_x_cb = QComboBox()
        wndlayout.addWidget(self.sdefine_x_cb,8,3)
        wndlayout.addWidget(QLabel('纵轴选择'),9,2)
        self.sdefine_y_cb = QComboBox()
        wndlayout.addWidget(self.sdefine_y_cb,9,3)
        self.okBtn = QPushButton('自定义绘制')
        wndlayout.addWidget(self.okBtn,11,3)
        self.InitConnect()
        
    def InitConnect(self):
        self.flag_draw_cb.clicked.connect(self.setCheck)
        
        
    def setCheck(self):
        if self.flag_draw_cb.isChecked():
            self.flag_cb.setEnabled(1)
        else:
            self.flag_cb.setEnabled(0)


        
class Pdw_RF_Range_Set_Wnd(Common_Filter):
    
    def init(self):
        self.setWindowTitle('频段选择')
        self.layout = QGridLayout()
        self.layout.addWidget(QLabel('频段选择'),1,1)
        self.layout.addWidget(QLabel('频率范围'),2,1)
        rf_min_le = QLineEdit('8000')
        rf_max_le = QLineEdit('18000')
        self.layout.addWidget(rf_min_le,2,2)
        self.layout.addWidget('~',2,3)
        self.layout.addWidget(rf_max_le,2,4)
        self.layout.addWidget(QLabel('MHz'),2,5)
        self.set_rf_range_btn = QPushButton('设置')
        self.layout.addWidget(self.set_rf_range_btn,3,5)
        
        


class Draw_Wgt_C(QWidget):
    def __init__(self,size_x = 3,size_y = 1,connect_num = 0\
                 ,itemlist = 0,parent=None):
        super().__init__(parent)
        self.shape = (size_x,size_y)
        self.connect_num = connect_num
        if self.connect_num> size_x*size_y:
            self.connect_num=size_x*size_y
        self.init(size_x,size_y,itemlist)
        
    def init(self,size_x,size_y,itemlist):
        self.setWindowTitle('')
        self.plt_mgr = PlotManager(self)
        self.cp = []
        self.toolbar = QToolBar(_("Tools"))
        self.curves = []
        self.layout = QGridLayout()
        self.layout.addWidget(self.toolbar,0,1)
        for i in range(size_x*size_y):
            self.cp.append(MyCurvePlot())#guiqwt.curve.CurvePlot()
        func = []
        import functools
        for i in range(self.connect_num):
            func.append(functools.partial(self.set_range1,i))
        for i in range(self.connect_num):
            self.cp[i].axisWidget(self.cp[i].xBottom).scaleDivChanged.connect(\
                    func[i])
        for i in range(size_x):
            for j in range(size_y):
                            
                self.layout.addWidget(self.cp[i*size_y+j],i+1,j+1)
                self.plt_mgr.add_plot(self.cp[i*size_y+j])
    #            self.cp[i].setCanvasBackground(Qt.black)
                self.cp[i*size_y+j].SIG_RECTCHOOSE_CHANGED.connect(self.rect_choose)
#                self.cp[i*size_y+j].del_item(self.cp[i*size_y+j].grid)
                self.cp[i*size_y+j].SIG_ITEMS_CHANGED.connect(self.plt_mgr.update_tools_status)
                self.cp[i*size_y+j].SIG_ACTIVE_ITEM_CHANGED.connect(self.plt_mgr.update_tools_status)
                self.cp[i*size_y+j].SIG_PLOT_AXIS_CHANGED.connect(self.plt_mgr.plot_axis_changed)
        self.setLayout(self.layout)
        if itemlist:
            self.itemlist = PlotItemList(self)#self.cp[0])
#            self.itemlist.register_panel(self.plt_mgr)
            self.itemlist.setVisible(0)
            self.itemlist.listwidget.plots = self.cp
            self.layout.addWidget(self.itemlist,1,size_y+1,size_x,1)
            self.plt_mgr.add_panel(self.itemlist)
            self.plt_mgr.get_itemlist_panel()#.show()
        
#        self.plt_mgr.get_itemlist_panel().show()
        self.add_toolbar()
        self.plt_mgr.add_separator_tool(self.toolbar)
        self.plt_mgr.add_tool(HomeTool)
        self.plt_mgr.add_tool(ChooseTool)
        self.plt_mgr.add_tool(guiqwt.tools.SelectPointTool)
        self.register_tool()
        self.resize(QSize(1366,768))
        
    def rect_choose(self,idx):
        
        for i in range(self.connect_num):
            if self.cp[i].chosen_item in self.cp[i].items:
                self.cp[i].del_item(self.cp[i].chosen_item)
            all_data = self.cp[i].get_all_data()
            arr_x, arr_y = np.array([]), np.array([])
            no = 0
            for j in all_data.keys():
                if not self.cp[i].items[j].isVisible():
                    no += 1
                    continue
                x,y = all_data[j][0],all_data[j][1]
#                print('len:',len(x),len(y))
#                idx1 = np.where( (y <= ymax) & (y >= ymin) )[0]
#                idx2 = np.where( (x <= xmax) & (x >= xmin) )[0]
#                idx = np.intersect1d(idx1,idx2)
                x,y = all_data[j][0][idx[2*no+1]],all_data[j][1][idx[2*no+1]]
                no += 1
                arr_x,arr_y = np.hstack((arr_x,x)),np.hstack((arr_y,y))
                
#            print(all_data.keys(),type(x))
            
            try:
                style = self.cp[i].items[0].style()
            except:
                style = self.cp[i].items[1].style()
            # style()返回整数值  choice 为列表
            self.cp[i].chosen_item = make.curve(arr_x,arr_y,color='#66ccff',linewidth=4,\
            curvestyle = guiqwt.styles.CURVESTYLE_CHOICES[style][0])
            self.cp[i].add_item_with_z_offset(self.cp[i].chosen_item,self.cp[i].get_max_z()+1)
    #            self.items[i].set_data(x,y)
            self.cp[i].replot()
        
    def add_toolbar(self):
        self.plt_mgr.add_toolbar(self.toolbar)
        
    def _axis_change(self,i):
        self.AXIS_CHANGED.emit(i)
        
    def set_range1(self,i):
        xmin = self.cp[i].axisScaleDiv(self.cp[i].xBottom).lowerBound()
        xmax = self.cp[i].axisScaleDiv(self.cp[i].xBottom).upperBound()
        for j in range(self.shape[0]*self.shape[1]):
            if j == i: continue
            self.cp[j].setAxisScale(self.cp[j].xBottom,xmin,xmax)


        
    def register_tool(self):
        self.plt_mgr.register_all_curve_tools()
        
class PdwDrawWgt(Draw_Wgt_C):
    def __init__(self,size_x = 4,size_y = 1,connect_num = 4\
                 ,itemlist = 0,parent=None):
        super().__init__(size_x ,size_y ,connect_num \
                 ,itemlist = 0,parent=None)
        self.setName()
        
    def setName(self):
        if self.shape[0] != 5 and self.shape[0] != 4:return
        self.cp[0].set_titles(title ='TOA-RF',xlabel='TOA',ylabel='RF')
        self.cp[1].set_titles(title ='TOA-PA',xlabel='TOA',ylabel='PA')
        self.cp[2].set_titles(title ='TOA-DOA',xlabel='TOA',ylabel='DOA')
        self.cp[3].set_titles(title ='TOA-PW',xlabel='TOA',ylabel='PW')
        if self.shape[0] == 5:
            self.cp[4].set_titles(title ='TOA-PRI',xlabel='TOA',ylabel='PRI')

        
class OriginDataParseWnd(QWidget):
    def __init__(self,parent=None):
        super().__init__(parent)
        self.init()
    def init(self):
        self.setWindowTitle('全脉冲数据解析')
        self.Origin_Data_wnd = QWidget()
        lot = QGridLayout()
        self.ParseBtn = QPushButton('解析')
        self.FilterBtn = QPushButton('滤波')
        self.SaveBtn = QPushButton('保存')
        self.save_flt_cb = QCheckBox('滤波数据保存')
        self.SaveMoreBtn = QPushButton('保存高级设置...')
        self.ChooseSrcCb = QComboBox()
        self.ChooseSrcCb.addItem('xyg-c')
        self.ChooseSrcCb.addItem('xyg-z')
        self.ChooseSrcCb.addItem('xyg_z_tpedw')
        self.ChooseSrcCb.addItem('struct')
        self.ChooseSrcCb.addItem('x55')
        self.ChooseSrcCb.setCurrentIndex(1)
        self.DrawBtn = QPushButton('绘图')
        self.draw_flt_cb = QCheckBox('滤波数据绘制')
        self.DrawMoreBtn = QPushButton('绘图高级设置...')
        self.le_filename = LineEdit()
        lot.addWidget(QLabel('文件名'),1,1)
        lot.addWidget(self.le_filename,1,2,1,3)
        lot.addWidget(self.ChooseSrcCb,2,1)
        lot.addWidget(self.ParseBtn,2,2)
        lot.addWidget(self.FilterBtn,2,3)
        lot.addWidget(self.SaveBtn,2,4)
        lot.addWidget(self.save_flt_cb,2,5)
        lot.addWidget(self.SaveMoreBtn,2,6)
        lot.addWidget(self.DrawBtn,2,7)
        lot.addWidget(self.draw_flt_cb,2,8)
        lot.addWidget(self.DrawMoreBtn,2,9)
        self.setLayout(lot)
        self.filterbox = filterbox()
        self.FilterBtn.clicked.connect(self.filterbox.show)
        self.SaveMoreWnd = SaveMoreWnd()
        self.SaveMoreBtn.clicked.connect(self.SaveMoreWnd.show)
        self.DrawMoreWnd = DrawMoreWnd()
        self.DrawMoreBtn.clicked.connect(self.DrawMoreWnd.show)
        self.tableView = tableview()
        lot.addWidget(self.tableView,3,1,3,9)
#        lot.addWidget(self.filterbox,3,1,4,5)
        self.ProgBar = QProgressBar()
        lot.addWidget(self.ProgBar,7,1,1,9)
        self.ProgBar.setMaximum(100)
        
class MatrixSearchWnd(QWidget):
    def __init__(self,parent=None):
        super().__init__(parent)
        self.init()
    def init(self):
        self.setWindowTitle('目标查找')
        self.wndlayout = QGridLayout()
        self.setLayout(self.wndlayout)
        self.wndlayout.addWidget(QLabel('参数设置'),2,2)
        self.wndlayout.addWidget(QLabel('文件名'),3,2)
        
        self.name_le = LineEdit()
        self.wndlayout.addWidget(self.name_le,3,3,1,4)
#        rf
        self.wndlayout.addWidget(QLabel('rf_min'),4,2)
        self.rf_min_le = LineEdit()
        self.rf_min_le.setValidator(DoubleValidator)
        self.wndlayout.addWidget(self.rf_min_le,4,3)
        self.wndlayout.addWidget(QLabel('rf_max'),4,4)
        self.rf_max_le = LineEdit()
        self.rf_max_le.setValidator(DoubleValidator)
        self.wndlayout.addWidget(self.rf_max_le,4,5)
#        pri
        self.wndlayout.addWidget(QLabel('pri_min'),5,2)
        self.pri_min_le = LineEdit()
        self.pri_min_le.setValidator(DoubleValidator)
        self.wndlayout.addWidget(self.pri_min_le,5,3)
        self.wndlayout.addWidget(QLabel('pri_max'),5,4)
        self.pri_max_le = LineEdit()
        self.pri_max_le.setValidator(DoubleValidator)
        self.wndlayout.addWidget(self.pri_max_le,5,5)
#        doa
        self.wndlayout.addWidget(QLabel('doa_min'),6,2)
        self.doa_min_le = LineEdit('0')
        self.wndlayout.addWidget(self.doa_min_le,6,3)
        self.wndlayout.addWidget(QLabel('doa_max'),6,4)
        self.doa_max_le = LineEdit('1024')
        self.wndlayout.addWidget(self.doa_max_le,6,5)
#        pa
        self.wndlayout.addWidget(QLabel('pa_min'),7,2)
        self.pa_min_le = LineEdit('0')
        self.wndlayout.addWidget(self.pa_min_le,7,3)
        self.wndlayout.addWidget(QLabel('pa_max'),7,4)
        self.pa_max_le = LineEdit('3000')
        self.wndlayout.addWidget(self.pa_max_le,7,5)
#        pw
        self.wndlayout.addWidget(QLabel('pw_min'),8,2)
        self.pw_min_le = LineEdit('0')
        self.wndlayout.addWidget(self.pw_min_le,8,3)
        self.wndlayout.addWidget(QLabel('pw_max'),8,4)
        self.pw_max_le = LineEdit('3000')
        self.wndlayout.addWidget(self.pw_max_le,8,5)
        
        self.wndlayout.addWidget(QLabel('ddoaThr'),9,2)
        self.ddoaThr_le = LineEdit('10')
        self.wndlayout.addWidget(self.ddoaThr_le,9,3)
        self.wndlayout.addWidget(QLabel('drfThr'),9,4)
        self.drfThr_le = LineEdit('200')
        self.wndlayout.addWidget(self.drfThr_le,9,5)
        self.wndlayout.addWidget(QLabel('multi'),9,6)
        self.multi_le = LineEdit('2')
        self.wndlayout.addWidget(self.multi_le,9,7)
        
        self.wndlayout.addWidget(QLabel('脉冲数'),10,2)
        self.pls_num_le = LineEdit('10000')
        self.wndlayout.addWidget(self.pls_num_le,10,3)
        self.search_btn = QPushButton('搜索')
        self.wndlayout.addWidget(self.search_btn,10,4)
        self.save_btn = QPushButton('保存结果')
        self.wndlayout.addWidget(self.save_btn,10,5)
        self.wndlayout.addWidget(QLabel('脉冲列表'),11,2)
        self.pdw_table = QTableView()
        self.wndlayout.addWidget(self.pdw_table,12,2,4,6)
        
        self.Draw_wgt = Draw_Wgt_C(5,1,5)
        self.Draw_wgt.setWindowTitle('脉冲图')
        self.Draw_wgt.cp[0].set_titles(title ='RF',xlabel='TOA',ylabel='RF/MHz')
        self.Draw_wgt.cp[1].set_titles(title ='PA',xlabel='TOA',ylabel='PA/dBm')
        self.Draw_wgt.cp[2].set_titles(title ='DOA',xlabel='TOA',ylabel='DOA/deg')
        self.Draw_wgt.cp[3].set_titles(title ='PW',xlabel='TOA',ylabel='PW/us')
        self.Draw_wgt.cp[4].set_titles(title ='PRI',xlabel='TOA',ylabel='PRI/us')
        self.wndlayout.addWidget(self.Draw_wgt,3,8,12,8)
        
        
class SDFilter(QDialog):
    def __init__(self,parent=None):
        super().__init__(parent)
        self.init()
    def init(self):
        self.setWindowTitle('筛选')
        self.setWindowFlags(Qt.Widget)
        self.wndlayout = QGridLayout()
        self.setLayout(self.wndlayout)
        self.lot= QGridLayout()
        self.wndlayout.addWidget(QLabel('属性名'),1,1)
        self.comb_check_all =QCheckBox('全选') 
        self.wndlayout.addWidget(self.comb_check_all,1,4)
        self.wndlayout.addLayout(self.lot,2,1,1,4)
        
        self.buttonBox = QDialogButtonBox(self)
        self.buttonBox.setOrientation(Qt.Horizontal)
        self.buttonBox.setStandardButtons(QDialogButtonBox.Cancel|QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        
        self.buttonBox.accepted.connect(self.accept)
        self.buttonBox.rejected.connect(self.reject)
#        self.btn_filter = QPushButton('筛选')
        self.wndlayout.addWidget(self.buttonBox,3,2)
        self.checkbox = []
        self.lineedit_min = []
        self.lineedit_max = []
        self.lineedit_str = []
        self.InitConnect()

    def InitConnect(self):
        self.comb_check_all.clicked.connect(self.setCheck)
#        
#        
    def setCheck(self):
        if self.comb_check_all.isChecked():
            for i in self.checkbox:
                i.setChecked(1)
            print('all_chosen')
        else:
            for i in self.checkbox:
                i.setChecked(0)
 
#class HistDataSource(object):
#    """
#    An objects that provides an Histogram data source interface
#    to a simple numpy array of data
#    """
##    __implements__ = (IHistDataSource,)
#    def __init__(self, data):
#        self.data = data
#
#    def get_histogram(self, nbins, edges):
#        """Returns the histogram computed for nbins bins"""
#        return np.histogram(self.data, nbins, edges)
    
if __name__=='__main__':

    app = QApplication(sys.argv)

#    ex = Draw_Wgt()
#    ex = My_Draw_Wgt()
    ex = PdwDrawWgt()
    ex.show()

    sys.exit(app.exec_())


