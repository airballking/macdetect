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


from gi.repository import Gtk


class MainWindow:
    def __init__(self):
        self.prepareUI()
    
    def prepareUI(self):
        self.prepareWindow()
        self.prepareStack()
        self.prepareDeviceView()
        self.prepareIdentityView()
    
    def prepareWindow(self):
        self.winRef = Gtk.Window()
        self.winRef.connect("delete-event", self.triggerQuit)
        self.winRef.set_default_size(400, 500)
        self.winRef.set_border_width(10)
        
        hdrTitle = Gtk.HeaderBar(title="PyMACDetect Desktop")
        hdrTitle.props.show_close_button = True
        
        self.winRef.set_titlebar(hdrTitle)
    
    def prepareStack(self):
        self.stkStack = Gtk.Stack()
        self.stkStack.set_transition_type(Gtk.StackTransitionType.SLIDE_LEFT_RIGHT)
        self.stkStack.set_transition_duration(300)
        
        sswSwitcher = Gtk.StackSwitcher()
        sswSwitcher.set_stack(self.stkStack)
        
        vbxStack = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
        vbxStack.pack_start(sswSwitcher, False, False, 0)
        vbxStack.pack_start(self.stkStack, True, True, 0)
        
        self.winRef.add(vbxStack)
    
    def prepareDeviceView(self):
        self.prepareDeviceList()
        self.prepareMACList()
        
        hbxDeviceView = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=6)
        hbxDeviceView.pack_start(self.vwDeviceList, False, True, 0)
        hbxDeviceView.pack_start(self.vwMACList, True, True, 0)
        
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
        
        # self.fbFlowBox = Gtk.FlowBox()
        # self.fbFlowBox.set_valign(Gtk.Align.START)
        # self.fbFlowBox.set_max_children_per_line(30)
        # self.fbFlowBox.set_selection_mode(Gtk.SelectionMode.NONE)
        
        # TODO(winkler): Fill flowbox with formerly saved data here.
        
        # self.scwFlow.add(self.fbFlowBox)
    
    def triggerQuit(self, evEvent, dtData):
        Gtk.main_quit()
    
    def show(self):
        self.winRef.show_all()
