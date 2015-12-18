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
import pymacdetect
import ConnectionManager
from time import gmtime, strftime, sleep
import sqlite3
import os


(ID_COLUMN_TEXT, ID_COLUMN_PIXBUF) = range(2)


class ConnectionState:
    DISCONNECTED = 0
    CONNECTING = 1
    CONNECTED = 2


class MainWindow:
    def __init__(self):
        self.cliClient = PyMACDetect.Client()
        
        self.prepareUI()
        self.setConnectionState(ConnectionState.DISCONNECTED)
        
        self.prepareEnvironment()
        
        self.log("Startup")
    
    def prepareEnvironment(self):
        datadir = os.path.expanduser("~") + "/.pymacdetect"
        dbfile = "identities.db"
        
        if os.path.isdir(datadir) == False:
            os.makedirs(datadir)
        
        init_db = not (os.path.exists(datadir + "/" + dbfile) and os.path.isdir(datadir + "/" + dbfile) == False)
        self.sql_conn = sqlite3.connect(datadir + "/" + dbfile)
        self.sql_c = self.sql_conn.cursor()
        
        if init_db:
            self.sql_c.execute("CREATE TABLE identities (id integer, name text)")
            self.sql_c.execute("CREATE TABLE macs (mac text, identity_id integer)")
            self.sql_c.execute("CREATE TABLE macs_data (mac text, nickname text)")
            self.sql_conn.commit()
        
        self.loadIdentities()
    
    def loadIdentities(self):
        self.sql_c.execute("SELECT * FROM identities")
        identities = self.sql_c.fetchall()
        pixbuf = Gtk.IconTheme.get_default().load_icon("face-plain", 16, 0)
        
        for identity in identities:
            self.lsIdentities.append([identity[1], pixbuf, identity[0]])
    
    def checkPyMACDetect(self):
        if self.cliClient and self.cliClient.connected():
            packet = None
            
            try:
                packet = self.cliClient.receive()
            except pymacdetect.DisconnectedError:
                self.lsMACList.clear()
                self.lsDeviceList.clear()
                self.log("Connection closed by server.")
                self.setConnectionState(ConnectionState.Disconnected)
                
                return False
            
            if packet:
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
        
        self.btnConnection = Gtk.Button("Connect");
        self.btnConnection.connect("clicked", self.clickConnectionManager)
        
        hdrTitle.add(self.btnConnection)
        
        self.winRef.set_titlebar(hdrTitle)
    
    def setConnectionState(self, state):
        self.connectionState = state
        
        if state == ConnectionState.DISCONNECTED:
            self.btnConnection.set_label("Connect")
        elif state == ConnectionState.CONNECTING:
            self.btnConnection.set_label("Cancel")
        elif state == ConnectionState.CONNECTED:
            self.btnConnection.set_label("Disconnect")
    
    def processConnectionManager(self):
        what = self.cmgrConnectionManager.what
        where = self.cmgrConnectionManager.where
        
        if what == "connect":
            self.log("Connecting to " + where + "...")
            self.setConnectionState(ConnectionState.CONNECTING)
            
            if self.cliClient.connect(where):
                self.setConnectionState(ConnectionState.CONNECTED)
                self.log("Connected to " + where + ".")
                
                self.check_timeout_id = GObject.timeout_add(10, self.checkPyMACDetect)
                self.cliClient.send({"request": {"content": "devices-list"}})
                self.requestKnownMACAddresses()
            else:
                self.setConnectionState(ConnectionState.DISCONNECTED)
                self.log("Failed to connect to " + where + ".")
        else:
            # Everything else
            pass
    
    def log(self, message):
        time = strftime("%H:%M:%S", gmtime())
        self.lsLog.append([time, message])
    
    def clickConnectionManager(self, wdgWidget):
        if self.connectionState == ConnectionState.DISCONNECTED:
            self.cmgrConnectionManager.show()
            self.cmgrConnectionManager.winRef.set_modal(True)
        elif self.connectionState == ConnectionState.CONNECTING:
            # Cannot cancel at the moment
            pass
        elif self.connectionState == ConnectionState.CONNECTED:
            self.cliClient.disconnect()
            self.setConnectionState(ConnectionState.DISCONNECTED)
            self.log("Connection closed.")
            
            self.lsMACList.clear()
            self.lsDeviceList.clear()
    
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
        for i in range(len(self.lsDeviceList)):
            treeiter = self.lsDeviceList.get_iter(Gtk.TreePath(i))
            row = self.lsDeviceList[treeiter]
            
            if row[1] == device:
                self.lsDeviceList.remove(treeiter)
                break
        
        # Remove all MAC addresses related to this interface
        for i in range(len(self.lsMACList)):
            treeiter = self.lsMACList.get_iter(Gtk.TreePath(i))
            row = self.lsMACList[treeiter]
            
            if row[2] == device:
                self.lsMACList.remove(treeiter)
    
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
            identity = self.identityForMAC(mac)
            
            if identity > -1:
                identity_str = self.nameForIdentity(identity)
            else:
                identity_str = ""
            
            nickname = self.nicknameForMAC(mac)
            
            self.lsMACList.append([mac, vendor, device, identity, identity_str, nickname])
            self.sql_c.execute("INSERT INTO macs_data VALUES(?, ?)", (mac, "",))
            self.sql_conn.commit()
    
    def nicknameForMAC(self, mac):
        self.sql_c.execute("SELECT nickname FROM macs_data WHERE mac=?", (mac,))
        result = self.sql_c.fetchone()
        
        if result != None:
            return result[0]
        else:
            return ""
    
    def getNewIdentityID(self, name):
        identity_id = 0
        
        while self.identityIDExists(identity_id):
            identity_id = identity_id + 1
        
        self.sql_c.execute("INSERT INTO identities VALUES (?, ?)", (str(identity_id), name,))
        self.sql_conn.commit()
        
        pixbuf = Gtk.IconTheme.get_default().load_icon("face-plain", 16, 0)
        self.lsIdentities.append([self.nameForIdentity(identity_id), pixbuf, identity_id])
        
        return identity_id
    
    def assignMACToIdentity(self, mac, identity_id):
        self.sql_c.execute("DELETE FROM macs WHERE mac=?", (mac,))
        
        self.sql_c.execute("INSERT INTO macs VALUES (?, ?)", (mac, identity_id,))
        self.sql_conn.commit()
        
        for treeiter in self.lsMACList:
            if treeiter[0] == mac:
                treeiter[3] = identity_id
                treeiter[4] = self.nameForIdentity(identity_id)
    
    def identityForMAC(self, mac):
        self.sql_c.execute("SELECT identity_id FROM macs WHERE mac=?", (mac,))
        result = self.sql_c.fetchone()
        
        if result != None:
            return result[0]
        else:
            return -1
    
    def identityIDExists(self, identity_id):
        self.sql_c.execute("SELECT * FROM identities WHERE id=?", (str(identity_id),))
        
        if self.sql_c.fetchone() == None:
            return False
        else:
            return True
    
    def nameForIdentity(self, identity_id):
        self.sql_c.execute("SELECT * FROM identities WHERE id=?", (str(identity_id),))
        
        result = self.sql_c.fetchone()
        
        if result != None:
            return result[1]
        else:
            return None
    
    def removeMAC(self, mac, device):
        for treeiter in self.lsMACList:
            if treeiter[0] == mac and treeiter[2] == device:
                self.lsMACList.remove(treeiter)
                break
    
    def nicknameEdited(self, wdgWidget, ptPath, txtText):
        self.lsMACList[ptPath][5] = txtText
        
        mac = self.lsMACList[ptPath][0]
        self.setNicknameForMAC(mac, txtText)
    
    def setNicknameForMAC(self, mac, nickname):
        self.sql_c.execute("UPDATE macs_data SET nickname=? WHERE mac=?", (nickname, mac,))
        self.sql_conn.commit()
    
    def prepareMACList(self):
        self.lsMACList = Gtk.ListStore(str, str, str, int, str, str)
        self.vwMACList = Gtk.TreeView(self.lsMACList)
        
        rdAddress = Gtk.CellRendererText()
        colAddress = Gtk.TreeViewColumn("MAC Address", rdAddress, text=0)
        
        rdVendor = Gtk.CellRendererText()
        colVendor = Gtk.TreeViewColumn("Vendor", rdVendor, text=1)
        
        rdDevice = Gtk.CellRendererText()
        colDevice = Gtk.TreeViewColumn("Device", rdDevice, text=2)
        
        rdIdentity = Gtk.CellRendererText()
        colIdentity = Gtk.TreeViewColumn("Identity", rdIdentity, text=4)
        
        rdNickname = Gtk.CellRendererText()
        rdNickname.set_property("editable", True)
        rdNickname.connect("edited", self.nicknameEdited)
        colNickname = Gtk.TreeViewColumn("Nickname", rdNickname, text=5)
        
        self.vwMACList.append_column(colAddress)
        self.vwMACList.append_column(colVendor)
        self.vwMACList.append_column(colDevice)
        self.vwMACList.append_column(colIdentity)
        self.vwMACList.append_column(colNickname)
        
        self.mnuMACs = Gtk.Menu()
        mniAssign = Gtk.MenuItem("Assign to Identity")
        
        mniAssignMenu = Gtk.Menu()
        mniAssign.set_submenu(mniAssignMenu)
        mniAssignNew = Gtk.MenuItem("New Identity")
        mniAssignMenu.append(mniAssignNew)
        mniAssignExisting = Gtk.MenuItem("Existing Identity")
        mniAssignMenu.append(mniAssignExisting)
        
        mniAssignNew.connect("activate", self.clickAssignNew)
        mniAssignExisting.connect("activate", self.clickAssignExisting)
        
        self.mnuMACs.append(mniAssign)
        self.mnuMACs.show_all()
        
        self.vwMACList.connect("button-press-event", self.clickMACList)
    
    def clickAssignNew(self, wdgWidget):
        model, treeiter = self.vwMACList.get_selection().get_selected()
        row = model[treeiter]
        mac = row[0]
        nickname = self.nicknameForMAC(mac)
        
        dlg = Gtk.Dialog("Create new Identity", self.winRef, Gtk.DialogFlags.MODAL,
                         (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
                          Gtk.STOCK_OK, Gtk.ResponseType.OK))
        
        content_box = dlg.get_content_area()
        
        vbxRows = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
        content_box.add(vbxRows)
        
        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=30)
        label1 = Gtk.Label("Identity Name", xalign=0)
        entry1 = Gtk.Entry()
        hbox.pack_start(label1, False, False, 3)
        hbox.pack_end(entry1, True, True, 3)
        vbxRows.pack_start(hbox, False, False, 0)
        
        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=30)
        label2 = Gtk.Label("Device MAC", xalign=0)
        label22 = Gtk.Label(mac)
        hbox.pack_start(label2, False, False, 3)
        hbox.pack_end(label22, True, True, 3)
        vbxRows.pack_start(hbox, False, False, 0)
        
        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=30)
        label3 = Gtk.Label("Device Nickname", xalign=0)
        entry3 = Gtk.Entry()
        entry3.set_text(nickname)
        hbox.pack_start(label3, False, False, 3)
        hbox.pack_end(entry3, True, True, 3)
        vbxRows.pack_start(hbox, False, False, 0)
        
        dlg.show_all()
        
        result = dlg.run()
        
        if result == Gtk.ResponseType.OK:
            identity_name = entry1.get_text()
            nickname = entry3.get_text()
            
            self.setNicknameForMAC(mac, nickname)
            
            identity_id = self.getNewIdentityID(identity_name)
            self.assignMACToIdentity(mac, identity_id)
        elif result == Gtk.ResponseType.CANCEL:
            # Do nothing.
            pass
        
        dlg.destroy()
    
    def clickAssignExisting(self, wdgWidget):
        model, treeiter = self.vwMACList.get_selection().get_selected()
        row = model[treeiter]
        mac = row[0]
        
        nickname = self.nicknameForMAC(mac)
        
        dlg = Gtk.Dialog("Assign to existing Identity", self.winRef, Gtk.DialogFlags.MODAL,
                         (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
                          Gtk.STOCK_OK, Gtk.ResponseType.OK))
        
        content_box = dlg.get_content_area()
        
        # TODO(winkler): Fix selection mode (is not selectable, should
        # be selectable)
        listbox = Gtk.ListBox()
        scwList = Gtk.ScrolledWindow()
        scwList.add(listbox)
        
        content_box.add(scwList)
        
        # TODO(winkler): Populate the listbox here.
        for treeiter in self.lsIdentities:
            row = Gtk.ListBoxRow()
            label = Gtk.Label(treeiter[0])
            label.set_justify(Gtk.Justification.LEFT)
            row.add(label)
            # TODO: hbox, left align, pack_start
            listbox.add(row)
        
        dlg.show_all()
        
        result = dlg.run()
        
        if result == Gtk.ResponseType.OK:
            # TODO(winkler): Process the dialog's data here.
            pass
        elif result == Gtk.ResponseType.CANCEL:
            # Do nothing.
            pass
        
        dlg.destroy()
    
    def clickMACList(self, wdgWidget, evEvent):
        if evEvent.type == Gdk.EventType.BUTTON_PRESS and evEvent.button == 3:
            if len(self.lsMACList) > 0:
                path, column, a, b = wdgWidget.get_path_at_pos(evEvent.x, evEvent.y)
                
                if path:
                    wdgWidget.row_activated(path, column)
                    wdgWidget.set_cursor(path)
                    
                    model, treeiter = self.vwMACList.get_selection().get_selected()
                    if treeiter != None:
                        self.mnuMACs.popup(None, None, None, None, evEvent.button, evEvent.time)
                        
                        return True
    
    def prepareIdentityView(self):
        self.prepareIdentityFlow()
        
        hbxIdentities = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=6)
        hbxIdentities.pack_start(self.scwFlow, True, True, 0)
        
        vbxControls = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
        
        self.idImage = Gtk.Image()
        vbxControls.pack_start(self.idImage, False, True, 0)
        self.idImage.set_size_request(100, 100)
        
        hbxIdentities.pack_start(vbxControls, False, True, 0)
        
        self.stkStack.add_titled(hbxIdentities, "identities", "Identity View")
    
    def prepareIdentityFlow(self):
        self.scwFlow = Gtk.ScrolledWindow()
        self.scwFlow.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
        
        self.ivIdentities = Gtk.IconView()
        self.ivIdentities.set_text_column(ID_COLUMN_TEXT)
        self.ivIdentities.set_pixbuf_column(ID_COLUMN_PIXBUF)
        
        self.lsIdentities = Gtk.ListStore(str, GdkPixbuf.Pixbuf, int)
        self.ivIdentities.set_model(self.lsIdentities)
        
        self.scwFlow.add(self.ivIdentities)
    
    def triggerQuit(self, evEvent, dtData):
        Gtk.main_quit()
    
    def show(self):
        self.winRef.show_all()
    
    def withScrolledWindow(self, wdgWidget):
        scwScrolled = Gtk.ScrolledWindow()
        scwScrolled.add(wdgWidget)
        
        return scwScrolled
