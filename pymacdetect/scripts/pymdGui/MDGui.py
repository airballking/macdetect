## macdetect, a daemon and clients for detecting MAC addresses
## Copyright (C) 2015 Jan Winkler <jan.winkler.84@gmail.com>
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

## \author Jan Winkler


from gi.repository import Gtk, Gdk, GdkPixbuf
from pymdLib import PyMACDetect
import ConnectionManager


(ID_COLUMN_TEXT, ID_COLUMN_PIXBUF) = range(2)


class MainWindow:
    def __init__(self):
        self.prepareUI()
    
    def prepareUI(self):
        self.cmgrConnectionManager = ConnectionManager.ConnectionManager()
        
        self.prepareWindow()
        self.prepareLog()
        self.prepareStack()
        self.prepareDeviceView()
        self.prepareIdentityView()
    
    def prepareWindow(self):
        self.winRef = Gtk.Window()
        self.winRef.connect("delete-event", self.triggerQuit)
        self.winRef.set_default_size(600, 500)
        self.winRef.set_border_width(10)
        self.winRef.set_position(Gtk.WindowPosition.CENTER)
        
        hdrTitle = Gtk.HeaderBar(title="PyMACDetect Desktop")
        hdrTitle.props.show_close_button = True
        
        self.btnConnection = Gtk.Button("Servers");
        self.btnConnection.connect("clicked", self.clickConnectionManager)
        
        hdrTitle.add(self.btnConnection)
        
        self.winRef.set_titlebar(hdrTitle)
    
    def clickConnectionManager(self, wdgWidget):
        self.cmgrConnectionManager.show()
        self.cmgrConnectionManager.winRef.set_modal(True)
    
    def prepareLog(self):
        self.lsLog = Gtk.ListStore(str, str)
        self.lsLog.append(["19:32", "Startup"]) # Test
        
        self.vwLog = Gtk.TreeView(self.lsLog)
        
        rdTime = Gtk.CellRendererText()
        colTime = Gtk.TreeViewColumn("Time", rdTime, text=0)
        
        rdDescription = Gtk.CellRendererText()
        colDescription = Gtk.TreeViewColumn("Description", rdDescription, text=1)
        
        self.vwLog.append_column(colTime)
        self.vwLog.append_column(colDescription)
    
    def prepareStack(self):
        self.stkStack = Gtk.Stack()
        self.stkStack.set_transition_type(Gtk.StackTransitionType.SLIDE_LEFT_RIGHT)
        self.stkStack.set_transition_duration(300)
        
        sswSwitcher = Gtk.StackSwitcher()
        sswSwitcher.set_stack(self.stkStack)
        
        hbxSwitcher = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=0)
        hbxSwitcher.pack_start(sswSwitcher, True, False, 0)
        
        vbxStack = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
        vbxStack.pack_start(hbxSwitcher, False, True, 0)
        vbxStack.pack_start(self.stkStack, True, True, 0)
        
        scwLog = self.withScrolledWindow(self.vwLog)
        scwLog.set_min_content_height(100)
        
        vbxStack.pack_start(scwLog, False, False, 0)
        
        self.winRef.add(vbxStack)
    
    def prepareDeviceView(self):
        self.prepareDeviceList()
        self.prepareMACList()
        
        hbxDeviceView = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=6)
        
        scwDeviceList = self.withScrolledWindow(self.vwDeviceList)
        scwDeviceList.set_min_content_width(170)
        scwDeviceList.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
        
        hbxDeviceView.pack_start(scwDeviceList, False, True, 0)
        hbxDeviceView.pack_start(self.withScrolledWindow(self.vwMACList), True, True, 0)
        
        self.stkStack.add_titled(hbxDeviceView, "devices", "Device View")
    
    def prepareDeviceList(self):
        self.lsDeviceList = Gtk.ListStore(bool, str, str)
        self.lsDeviceList.append([False, "eth0", "wired"]) # Test
        
        self.vwDeviceList = Gtk.TreeView(self.lsDeviceList)
        
        rdActive = Gtk.CellRendererToggle()
        colActive = Gtk.TreeViewColumn("Active", rdActive, active=0)
        
        rdType = Gtk.CellRendererText()
        colType = Gtk.TreeViewColumn("Type", rdType, text=2)
        
        rdText = Gtk.CellRendererText()
        colDeviceName = Gtk.TreeViewColumn("Device", rdText, text=1)
        
        self.vwDeviceList.append_column(colActive)
        self.vwDeviceList.append_column(colDeviceName)
        self.vwDeviceList.append_column(colType)
    
    def prepareMACList(self):
        self.lsMACList = Gtk.ListStore(str, str)
        self.lsMACList.append(["AA:BB:CC:DD:EE:FF", "Apple, Inc."]) # Test
        
        self.vwMACList = Gtk.TreeView(self.lsMACList)
        
        rdAddress = Gtk.CellRendererText()
        colAddress = Gtk.TreeViewColumn("MAC Address", rdAddress, text=0)
        
        rdVendor = Gtk.CellRendererText()
        colVendor = Gtk.TreeViewColumn("Vendor", rdVendor, text=1)
        
        self.vwMACList.append_column(colAddress)
        self.vwMACList.append_column(colVendor)
    
    def prepareIdentityView(self):
        self.prepareIdentityFlow()
        
        hbxIdentities = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=6)
        hbxIdentities.pack_start(self.scwFlow, True, True, 0)
        
        self.stkStack.add_titled(hbxIdentities, "identities", "Identity View")
    
    def prepareIdentityFlow(self):
        self.scwFlow = Gtk.ScrolledWindow()
        self.scwFlow.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
        
        self.ivIdentities = Gtk.IconView()
        self.ivIdentities.set_text_column(ID_COLUMN_TEXT)
        self.ivIdentities.set_pixbuf_column(ID_COLUMN_PIXBUF)
        
        self.lsIdentities = Gtk.ListStore(str, GdkPixbuf.Pixbuf)
        self.ivIdentities.set_model(self.lsIdentities)
        
        pixbuf = Gtk.IconTheme.get_default().load_icon("image-missing", 16, 0)
        self.lsIdentities.append(["Item 1", pixbuf])
        
        self.scwFlow.add(self.ivIdentities)
    
    def triggerQuit(self, evEvent, dtData):
        Gtk.main_quit()
    
    def show(self):
        self.winRef.show_all()
    
    def withScrolledWindow(self, wdgWidget):
        scwScrolled = Gtk.ScrolledWindow()
        scwScrolled.add(wdgWidget)
        
        return scwScrolled
