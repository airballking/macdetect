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


from gi.repository import Gtk, Gdk, GdkPixbuf, GObject
from pymdLib import PyMACDetect
import ConnectionManager
from time import gmtime, strftime, sleep


(ID_COLUMN_TEXT, ID_COLUMN_PIXBUF) = range(2)


class MainWindow:
    def __init__(self):
        self.cliClient = PyMACDetect.Client()
        self.prepareUI()
        
        self.log("Startup")
    
    def checkPyMACDetect(self):
        if self.cliClient and self.cliClient.connected():
            packet = None
            
            try:
                packet = self.cliClient.receive()
            except pymacdetect.DisconnectedError:
                return False
            
            if packet:
                print packet
                if "info" in packet:
                    what = packet["info"]["content"]
                    subs = None
                    
                    if "subs" in packet["info"]:
                        subs = packet["info"]["subs"]
                    
                    if what == "device-added":
                        device = subs["device-name"]["content"]
                        devtype = subs["hardware-type"]["content"]
                        
                        self.addDevice(device, devtype)
                    elif what == "device-removed":
                        device = subs["device-name"]["content"]
                        
                        self.removeDevice(device)
                    elif what == "mac-address-discovered":
                        mac = subs["mac"]["content"]
                        vendor = subs["vendor"]["content"]
                        device = subs["device-name"]["content"]
                        
                        self.addMAC(mac, vendor, device)
                    elif what == "mac-address-disappeared":
                        mac = subs["mac"]["content"]
                        device = subs["device-name"]["content"]
                        
                        self.removeMAC(mac, device)
                    elif what == "device-evidence-changed":
                        # TODO(winkler): Handle this case
                        pass
                    elif what == "device-state-changed":
                        # TODO(winkler): Handle this case
                        pass
                elif "response" in packet:
                    what = packet["response"]["content"]
                    subs = None
                    
                    if "subs" in packet["response"]:
                        subs = packet["response"]["subs"]
                    
                    if what == "devices-list":
                        if "subs" in subs["devices-list"]:
                            for sub in subs["devices-list"]["subs"]:
                                device = sub
                                devtype = subs["devices-list"]["subs"][sub]["subs"]["hardware-type"]["content"]
                                
                                self.addDevice(device, devtype)
                    elif what == "known-mac-addresses":
                        if "subs" in subs["known-mac-addresses"]:
                            for sub in subs["known-mac-addresses"]["subs"]:
                                mac = sub
                                properties = subs["known-mac-addresses"]["subs"][sub]["subs"]
                                
                                vendor = properties["vendor"]["content"]
                                device = properties["device-name"]["content"]
                                
                                self.addMAC(mac, vendor, device)
            
            return True
        else:
            return False
    
    def prepareUI(self):
        self.cmgrConnectionManager = ConnectionManager.ConnectionManager(self)
        
        self.prepareWindow()
        self.prepareLog()
        self.prepareDeviceList()
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
    
    def processConnectionManager(self):
        what = self.cmgrConnectionManager.what
        where = self.cmgrConnectionManager.where
        
        if what == "connect":
            self.log("Connecting to " + where)
            if self.cliClient.connect(where):
                self.log("Connected to " + where)
                
                self.check_timeout_id = GObject.timeout_add(10, self.checkPyMACDetect)
                self.cliClient.send({"request": {"content": "devices-list"}})
                self.requestKnownMACAddresses()
            else:
                self.log("Failed to connect to " + where)
        else:
            # Everything else
            pass
    
    def log(self, message):
        time = strftime("%H:%M:%S", gmtime())
        self.lsLog.append([time, message])
    
    def clickConnectionManager(self, wdgWidget):
        self.cmgrConnectionManager.show()
        self.cmgrConnectionManager.winRef.set_modal(True)
    
    def logChanged(self, wdgWidget, evEvent, dtData=None):
        adjAdjustment = wdgWidget.get_vadjustment()
        adjAdjustment.set_value(adjAdjustment.get_upper() - adjAdjustment.get_page_size())
    
    def prepareLog(self):
        self.lsLog = Gtk.ListStore(str, str)
        self.vwLog = Gtk.TreeView(self.lsLog)
        
        self.vwLog.connect('size-allocate', self.logChanged)
        
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
        
        hbxMain = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=6)
        
        scwDeviceList = self.withScrolledWindow(self.vwDeviceList)
        scwDeviceList.set_min_content_width(170)
        scwDeviceList.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
        
        hbxMain.pack_start(scwDeviceList, False, True, 0)
        hbxMain.pack_start(vbxStack, True, True, 0)
        
        vbxMain = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
        vbxMain.pack_start(hbxMain, True, True, 0)
        vbxMain.pack_start(scwLog, False, False, 0)
        
        self.winRef.add(vbxMain)
    
    def prepareDeviceView(self):
        self.prepareMACList()
        
        hbxDeviceView = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=6)
        
        hbxDeviceView.pack_start(self.withScrolledWindow(self.vwMACList), True, True, 0)
        
        self.stkStack.add_titled(hbxDeviceView, "devices", "Device View")
    
    def addDevice(self, device, devtype):
        is_present = False
        
        for treeiter in self.lsDeviceList:
            if treeiter[1] == device:
                is_present = True
                break
        
        if not is_present:
            self.lsDeviceList.append([False, device, devtype])
    
    def removeDevice(self, device):
        for treeiter in self.lsDeviceList:
            if treeiter[1] == device:
                self.lsDeviceList.remove(treeiter)
                break
    
    def deviceListToggled(self, wdgWidget, ptPath):
        treeiter = self.lsDeviceList[ptPath]
        state = not treeiter[0]
        treeiter[0] = state
        
        device = treeiter[1]
        
        if state:
            self.enableStream(device)
            self.requestKnownMACAddresses()
        else:
            self.disableStream(device)
    
    def enableStream(self, device):
        self.cliClient.send({"request": {"content": "enable-stream",
                                         "subs": {"device-name": {"content": device}}}})
    
    def disableStream(self, device):
        self.cliClient.send({"request": {"content": "disable-stream",
                                         "subs": {"device-name": {"content": device}}}})
    
    def requestKnownMACAddresses(self):
        self.cliClient.send({"request": {"content": "known-mac-addresses"}})
    
    def prepareDeviceList(self):
        self.lsDeviceList = Gtk.ListStore(bool, str, str)
        self.vwDeviceList = Gtk.TreeView(self.lsDeviceList)
        
        rdActive = Gtk.CellRendererToggle()
        rdActive.connect("toggled", self.deviceListToggled)
        colActive = Gtk.TreeViewColumn("Active", rdActive, active=0)
        
        rdType = Gtk.CellRendererText()
        colType = Gtk.TreeViewColumn("Type", rdType, text=2)
        
        rdText = Gtk.CellRendererText()
        colDeviceName = Gtk.TreeViewColumn("Device", rdText, text=1)
        
        self.vwDeviceList.append_column(colActive)
        self.vwDeviceList.append_column(colDeviceName)
        self.vwDeviceList.append_column(colType)
    
    def addMAC(self, mac, vendor, device):
        is_present = False
        
        for treeiter in self.lsMACList:
            if treeiter[0] == mac and treeiter[2] == device:
                is_present = True
                break
        
        if not is_present:
            self.lsMACList.append([mac, vendor, device])
    
    def removeMAC(self, mac, device):
        for treeiter in self.lsMACList:
            if treeiter[0] == mac and treeiter[2] == device:
                self.lsMACList.remove(treeiter)
                break
    
    def prepareMACList(self):
        self.lsMACList = Gtk.ListStore(str, str, str)
        self.vwMACList = Gtk.TreeView(self.lsMACList)
        
        rdAddress = Gtk.CellRendererText()
        colAddress = Gtk.TreeViewColumn("MAC Address", rdAddress, text=0)
        
        rdVendor = Gtk.CellRendererText()
        colVendor = Gtk.TreeViewColumn("Vendor", rdVendor, text=1)
        
        rdDevice = Gtk.CellRendererText()
        colDevice = Gtk.TreeViewColumn("Device", rdDevice, text=2)
        
        self.vwMACList.append_column(colAddress)
        self.vwMACList.append_column(colVendor)
        self.vwMACList.append_column(colDevice)
    
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
