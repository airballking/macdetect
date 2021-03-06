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
import pymacdetect_ext
import ConnectionManager
import sqlite3
import os
import cairo
import time
import random
import math


(ID_COLUMN_TEXT, ID_COLUMN_PIXBUF) = range(2)


class ConnectionState:
    DISCONNECTED = 0
    CONNECTING = 1
    CONNECTED = 2

class EventType:
    MACAdded = "mac_added"
    MACRemoved = "mac_removed"
    MACEvidenceUpdated = "mac_evidence_updated"


class MainWindow:
    def __init__(self, bDemonstration):
        self.startTime = self.currentTime()
        
        self.cliClient = PyMACDetect.Client()
        self.bDemonstration = bDemonstration
        
        self.prepareUI()
        self.setConnectionState(ConnectionState.DISCONNECTED, "")
        
        self.prepareEnvironment()
        
        self.log("Startup")
        
        if self.bDemonstration:
            self.log("PyMACDetect was started in demo mode. Identities and MACs will be shortened.")
        
        self.check_server_info = GObject.timeout_add(100, self.checkServerInfo)
        
        self.arrEvents = {}
        self.arrColors = [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0], [1.0, 0.0, 1.0], [1.0, 1.0, 0.0], [0.0, 1.0, 1.0], [1.0, 0.5, 0.5], [0.5, 1.0, 0.5], [0.5, 0.5, 1.0]]
    
    def color(self, index):
        return self.arrColors[index % len(self.arrColors)]
    
    def currentTime(self):
        return time.mktime(time.localtime())
    
    def addEvent(self, event_type, mac, evidence_field="", evidence_value="", event_time=None):
        if not mac in self.arrEvents:
            self.arrEvents[mac] = []
        
        if not event_time:
            event_time = self.currentTime()
        
        self.arrEvents[mac].append({"time": event_time,
                                    "event_type": event_type,
                                    "evidence_field": evidence_field,
                                    "evidence_value": evidence_value})
        
        if event_type == EventType.MACAdded:
            height = len(self.arrEvents) * 25 + 50
            
            self.setTimelineHeight(height)
        
        self.daTimeline.queue_draw()
    
    def macEvents(self, mac):
        if mac in self.arrEvents:
            return self.arrEvents[mac]
        else:
            return []
    
    def sortedEventMACs(self):
        return sorted(self.arrEvents.keys())
    
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
            self.sql_c.execute("CREATE TABLE macs_history (mac text)")
            
            self.sql_conn.commit()
        
        self.loadIdentities()
        self.loadMACHistory()
    
    def loadIdentities(self):
        self.sql_c.execute("SELECT * FROM identities")
        identities = self.sql_c.fetchall()
        pixbuf = Gtk.IconTheme.get_default().load_icon("face-smile", 32, 0)
        
        for identity in identities:
            self.lsIdentities.append([identity[1] if self.bDemonstration == False else self.demoIdentityStr(identity[1]), pixbuf, identity[0]])
    
    def checkServerInfo(self):
        if self.cliClient:
            server_infos = self.cliClient.detectedServerInfo()
            
            if server_infos:
                for siServerInfo in server_infos:
                    self.cmgrConnectionManager.addServer(siServerInfo)
        
        return True
    
    def checkPyMACDetect(self):
        if self.cliClient and self.cliClient.connected():
            packet = None
            
            try:
                packet = self.cliClient.receive()
            except pymacdetect_ext.DisconnectedError:
                self.tsMACList.clear()
                self.lsDeviceList.clear()
                self.arrEvents = {}
                self.daTimeline.queue_draw()
                self.log("Connection closed by server.")
                self.setConnectionState(ConnectionState.Disconnected, "")
                
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
                    elif what == "mac-evidence-changed":
                        key = subs["field"]["content"]
                        content = subs["value"]["content"]
                        mac_address = subs["mac-address"]["content"]
                        
                        self.updateMACEvidence(mac_address, {key: content})
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
                                
                                self.cliClient.send({"request": {"content": "mac-evidence",
                                                       "subs": {"mac-address": {"content": mac}}}})
                    elif what == "mac-evidence":
                        if "subs" in subs["mac-evidence"]:
                            evidence_dict = {}
                            mac_address = ""
                            
                            for sub in subs["mac-evidence"]["subs"]:
                                key = sub
                                content = subs["mac-evidence"]["subs"][sub]["content"]
                                
                                if key == "mac-address":
                                    mac_address = content
                                else:
                                    evidence_dict[key] = content
                            
                            if mac_address != "":
                                self.updateMACEvidence(mac_address, evidence_dict)
            
            return True
        else:
            return False
    
    def updateMACEvidence(self, mac_address, evidence_dict):
        for key in evidence_dict:
            content = evidence_dict[key]
            
            for i in range(len(self.tsMACList)):
                treeiter = self.tsMACList.get_iter(Gtk.TreePath(i))
                row = self.tsMACList[treeiter]
                
                if row[0] == mac_address:
                    if self.tsMACList.iter_has_child(treeiter):
                        childiter = self.tsMACList.iter_children(treeiter)
                        
                        bFound = False
                        while childiter:
                            childrow = self.tsMACList[childiter]
                            
                            if childrow[0] == key:
                                childrow[5] = content
                                bFound = True
                                break
                            else:
                                childiter = self.tsMACList.iter_next(childiter)
                        
                        if not bFound:
                            self.addEvent(EventType.MACEvidenceUpdated, mac_address, key, content)
                            self.tsMACList.append(treeiter, [key, "", "", 0, "", content, False, key, ""])
                    else:
                        self.addEvent(EventType.MACEvidenceUpdated, mac_address, key, content)
                        self.tsMACList.append(treeiter, [key, "", "", 0, "", content, False, key, ""])
                    
                    break
    
    def prepareUI(self):
        self.cmgrConnectionManager = ConnectionManager.ConnectionManager(self)
        
        self.prepareWindow()
        self.prepareLog()
        self.prepareDeviceList()
        self.prepareStack()
        self.prepareDeviceView()
        self.prepareIdentityView()
        self.prepareTimelineView()
        
        accel = Gtk.AccelGroup()
        
        accel.connect(Gdk.keyval_from_name('1'), Gdk.ModifierType.MOD1_MASK, 0, lambda *args: self.on_accel_pressed("switch", "devices"))
        accel.connect(Gdk.keyval_from_name('2'), Gdk.ModifierType.MOD1_MASK, 0, lambda *args: self.on_accel_pressed("switch", "identities"))
        accel.connect(Gdk.keyval_from_name('3'), Gdk.ModifierType.MOD1_MASK, 0, lambda *args: self.on_accel_pressed("switch", "timeline"))
        
        accel.connect(Gdk.keyval_from_name('c'), Gdk.ModifierType.CONTROL_MASK, 0, lambda *args: self.on_accel_pressed("connect", ""))
        
        self.winRef.add_accel_group(accel)
    
    def on_accel_pressed(self, param_1, param_2):
        if param_1 == "switch":
            self.stkStack.set_visible_child_name(param_2)
        elif param_1 == "connect":
            self.clickConnectionManager(None)
    
    def prepareWindow(self):
        self.winRef = Gtk.Window()
        self.winRef.connect("delete-event", self.triggerQuit)
        self.winRef.set_default_size(600, 500)
        self.winRef.set_border_width(10)
        self.winRef.set_position(Gtk.WindowPosition.CENTER)
        
        hdrTitle = Gtk.HeaderBar(title="PyMACDetect Desktop" if self.bDemonstration == False else "PyMACDetect Desktop (Demo Mode)")
        hdrTitle.props.show_close_button = True
        
        self.btnConnection = Gtk.Button("Connect")
        self.btnConnection.connect("clicked", self.clickConnectionManager)
        
        self.lblServer = Gtk.Label("")
        
        self.btnSettings = Gtk.Button()
        self.btnSettings.connect("clicked", self.clickSettings)
        img = Gtk.Image()
        img.set_from_pixbuf(Gtk.IconTheme.get_default().load_icon("preferences-system", 16, 0))
        self.btnSettings.set_image(img)
        self.btnSettings.show_all()
        
        self.btnAbout = Gtk.Button()
        self.btnAbout.connect("clicked", self.clickAbout)
        img = Gtk.Image()
        img.set_from_pixbuf(Gtk.IconTheme.get_default().load_icon("help-about", 16, 0))
        self.btnAbout.set_image(img)
        self.btnAbout.show_all()
        
        hdrTitle.add(self.btnConnection)
        hdrTitle.add(self.lblServer)
        #hdrTitle.pack_end(self.btnSettings)
        hdrTitle.pack_end(self.btnAbout)
        
        self.winRef.set_titlebar(hdrTitle)
    
    def clickSettings(self, wdg):
        prefs = Gtk.Dialog("Settings", self.winRef, Gtk.DialogFlags.MODAL,
                           (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
                            Gtk.STOCK_OK, Gtk.ResponseType.OK))
        
        # TODO: Populate settings dialog here.
        
        if prefs.run() == Gtk.ResponseType.OK:
            # TODO: Process settings here.
            pass
    
    def clickAbout(self, wdg):
        about = Gtk.MessageDialog(self.winRef, 0, Gtk.MessageType.INFO,
                                  Gtk.ButtonsType.OK, "About PyMACDetect Desktop Client")
        about.format_secondary_text('''
PyMACDetect is developed by Jan Winkler <jan.winkler.84@gmail.com>.
Copyright 2015. Licensed under GPL 2.0.

Get the most up to date version here:
https://github.com/fairlight1337/macdetect

For more details, see the LICENSE file in the base macdetect folder.''')
        
        about.run()
        about.destroy()
    
    def setConnectionState(self, state, where):
        self.connectionState = state
        
        self.lblServer.set_text(where)
        
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
            self.setConnectionState(ConnectionState.CONNECTING, where)
            
            if self.cliClient.connect(where):
                self.setConnectionState(ConnectionState.CONNECTED, where)
                self.log("Connected to " + where + ".")
                
                self.check_timeout_id = GObject.timeout_add(10, self.checkPyMACDetect)
                self.cliClient.send({"request": {"content": "devices-list"}})
                self.requestKnownMACAddresses()
            else:
                self.setConnectionState(ConnectionState.DISCONNECTED, where)
                self.log("Failed to connect to " + where + ".")
        else:
            # Everything else
            pass
    
    def log(self, message):
        current_time = time.strftime("%H:%M:%S", time.gmtime())
        self.lsLog.append([current_time, message])
    
    def clickConnectionManager(self, wdgWidget):
        self.cliClient.detectServers()
        
        if self.connectionState == ConnectionState.DISCONNECTED:
            self.cmgrConnectionManager.show()
            self.cmgrConnectionManager.winRef.set_modal(True)
        elif self.connectionState == ConnectionState.CONNECTING:
            # Cannot cancel at the moment
            pass
        elif self.connectionState == ConnectionState.CONNECTED:
            self.cliClient.disconnect()
            self.setConnectionState(ConnectionState.DISCONNECTED, "")
            self.log("Connection closed.")
            
            self.tsMACList.clear()
            self.lsDeviceList.clear()
            self.arrEvents = {}
            self.daTimeline.queue_draw()
    
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
    
    def filterIdentities(self, model, iter, data):
        subj = self.entSearchMACs.get_text().lower()
        
        if subj == "" or subj in model[iter][0].lower():
            return True
    
    def filterMACs(self, model, iter, data):
        subj = self.entSearchMACs.get_text().lower()
        row = model[iter]
        
        if subj == "" or subj in row[0].lower() or subj in row[1].lower() or subj in row[2].lower() or subj in row[4].lower() or subj in row[5].lower():
            return True
        else:
            if self.tsMACList.iter_has_child(iter):
                childiter = self.tsMACList.iter_children(iter)
                childrow = self.tsMACList[childiter]
                
                if subj in childrow[0].lower() or subj in childrow[5].lower():
                    return True
        
        return False
    
    def searchMACsEdited(self, wdg):
        selected_stack = self.sswSwitcher.get_stack().get_visible_child_name()
        
        if selected_stack == "devices":
            self.fltMACFilter.refilter()
        elif selected_stack == "identities":
            self.fltIdentities.refilter()
    
    def switchStack(self, wdg, btn):
        self.entSearchMACs.set_text("")
    
    def prepareStack(self):
        self.stkStack = Gtk.Stack()
        self.stkStack.set_transition_type(Gtk.StackTransitionType.SLIDE_LEFT_RIGHT)
        self.stkStack.set_transition_duration(300)
        
        self.sswSwitcher = Gtk.StackSwitcher()
        self.sswSwitcher.set_stack(self.stkStack)
        self.sswSwitcher.connect("set-focus-child", self.switchStack)
        
        self.entSearchMACs = Gtk.Entry()
        self.entSearchMACs.set_placeholder_text("Search")
        self.entSearchMACs.set_icon_from_icon_name(Gtk.EntryIconPosition.SECONDARY, "system-search")
        self.entSearchMACs.connect("changed", self.searchMACsEdited)
        
        hbxSwitcher = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=6)
        hbxSwitcher.pack_start(self.sswSwitcher, True, False, 0)
        
        hbxSwitcher.pack_end(self.entSearchMACs, False, False, 0)
        
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
        self.vbxDevicesAndControls = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
        
        self.vbxDevicesAndControls.pack_start(self.withScrolledWindow(self.vwMACList), True, True, 0)
        
        self.scwMACHistoryList = self.withScrolledWindow(self.vwMACHistoryList)
        
        hbxDeviceControls = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=6)
        
        btnExpandAll = Gtk.Button("Expand all")
        btnExpandAll.connect("clicked", self.clickExpandAllDevices)
        
        btnCollapseAll = Gtk.Button("Collapse all")
        btnCollapseAll.connect("clicked", self.clickCollapseAllDevices)
        
        hbxDeviceControls.pack_start(btnExpandAll, False, False, 0)
        hbxDeviceControls.pack_start(btnCollapseAll, False, False, 0)
        
        btnMACHistory = Gtk.ToggleButton("MAC History")
        hbxDeviceControls.pack_end(btnMACHistory, False, False, 0)
        btnMACHistory.connect("clicked", self.toggleMACHistory)
        
        self.vbxDevicesAndControls.pack_start(hbxDeviceControls, False, False, 0)
        
        hbxDeviceView.pack_start(self.vbxDevicesAndControls, True, True, 0)
        
        self.stkStack.add_titled(hbxDeviceView, "devices", "MACs")
    
    def toggleMACHistory(self, wdg):
        if wdg.get_state() == Gtk.StateType.ACTIVE:
            self.vbxDevicesAndControls.pack_start(self.scwMACHistoryList, True, True, 0)
            self.scwMACHistoryList.show_all()
        else:
            self.vbxDevicesAndControls.remove(self.scwMACHistoryList)
    
    def clickExpandAllDevices(self, wdg):
        self.vwMACList.expand_all()
    
    def clickCollapseAllDevices(self, wdg):
        self.vwMACList.collapse_all()
    
    def addDevice(self, device, devtype):
        is_present = False
        
        for treeiter in self.lsDeviceList:
            if treeiter[1] == device:
                is_present = True
                break
        
        if not is_present:
            pixbuf = None
            
            if devtype == "wired":
                pixbuf = Gtk.IconTheme.get_default().load_icon("network-wired", 16, 0)
            elif devtype == "wireless":
                pixbuf = Gtk.IconTheme.get_default().load_icon("network-wireless", 16, 0)
            
            self.lsDeviceList.append([False, device, pixbuf, devtype])
    
    def removeDevice(self, device):
        for i in range(len(self.lsDeviceList)):
            treeiter = self.lsDeviceList.get_iter(Gtk.TreePath(i))
            row = self.lsDeviceList[treeiter]
            
            if row[1] == device:
                self.lsDeviceList.remove(treeiter)
                break
        
        # Remove all MAC addresses related to this interface
        changed = True
        while changed:
            changed = False
            
            for i in range(len(self.tsMACList)):
                treeiter = self.tsMACList.get_iter(Gtk.TreePath(i))
                row = self.tsMACList[treeiter]
                
                if row[2] == device:
                    self.tsMACList.remove(treeiter)
                    changed = True
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
        self.requestKnownMACAddresses()
    
    def disableStream(self, device):
        self.cliClient.send({"request": {"content": "disable-stream",
                                         "subs": {"device-name": {"content": device}}}})
    
    def requestKnownMACAddresses(self):
        self.cliClient.send({"request": {"content": "known-mac-addresses"}})
    
    def prepareDeviceList(self):
        self.lsDeviceList = Gtk.ListStore(bool, str, GdkPixbuf.Pixbuf, str)
        self.vwDeviceList = Gtk.TreeView(self.lsDeviceList)
        
        rdActive = Gtk.CellRendererToggle()
        rdActive.connect("toggled", self.deviceListToggled)
        colActive = Gtk.TreeViewColumn("Active", rdActive, active=0)
        colActive.set_sort_column_id(0)
        
        rdText = Gtk.CellRendererText()
        colDeviceName = Gtk.TreeViewColumn("Device", rdText, text=1)
        colDeviceName.set_sort_column_id(1)
        
        rdType = Gtk.CellRendererPixbuf()
        colType = Gtk.TreeViewColumn("Type", rdType, pixbuf=2)
        colType.set_sort_column_id(3)
        
        self.vwDeviceList.append_column(colActive)
        self.vwDeviceList.append_column(colDeviceName)
        self.vwDeviceList.append_column(colType)
    
    def addMAC(self, mac, vendor, device):
        is_present = False
        
        for treeiter in self.tsMACList:
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
            
            demo_mac = self.demoMAC(mac)
            demo_identity_str = self.demoIdentityStr(identity_str)
            self.tsMACList.append(None, [mac, vendor, device, identity, identity_str, nickname, True, demo_mac, demo_identity_str])
            self.sql_c.execute("INSERT INTO macs_data VALUES(?, ?)", (mac, "",))
            self.sql_conn.commit()
            
            self.addEvent(EventType.MACAdded, mac, "", "")
        
        self.addMACToHistory(mac)
    
    def loadMACHistory(self):
        self.sql_c.execute("SELECT * FROM macs_history")
        historical_macs = self.sql_c.fetchall()
        
        for row in historical_macs:
            self.tsMACHistoryList.append(None, [row[0], self.demoMAC(row[0])])
            
    def addMACToHistory(self, mac):
        self.sql_c.execute("SELECT mac FROM macs_history WHERE mac=?", (mac,))
        result = self.sql_c.fetchone()
        
        is_new = result == None
        
        if is_new:
            self.sql_c.execute("INSERT INTO macs_history VALUES(?)", (mac,))
            self.sql_conn.commit()
            
            self.tsMACHistoryList.append([mac, self.demoMAC(mac)])
    
    def demoMAC(self, mac):
        # Replace the last two words with `XX`
        return mac[:-5] + "XX:XX"
    
    def demoIdentityStr(self, identity_str):
        # Return everything except for the last token
        return " ".join(identity_str.split(" ")[:-1])
    
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
        
        pixbuf = Gtk.IconTheme.get_default().load_icon("face-smile", 32, 0)
        self.lsIdentities.append([self.nameForIdentity(identity_id), pixbuf, identity_id])
        
        return identity_id
    
    def assignMACToIdentity(self, mac, identity_id):
        self.sql_c.execute("DELETE FROM macs WHERE mac=?", (mac,))
        
        self.sql_c.execute("INSERT INTO macs VALUES (?, ?)", (mac, identity_id,))
        self.sql_conn.commit()
        
        for treeiter in self.tsMACList:
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
        for i in range(len(self.lsDeviceList)):
            treeiter = self.lsDeviceList.get_iter(Gtk.TreePath(i))
            row = self.lsDeviceList[treeiter]
            
            if row[0] == mac and row[2] == device:
                self.lstMACs.remove(treeiter)
                break
        
        self.addEvent(EventType.MACRemoved, mac, "", "")
    
    def nicknameEdited(self, wdgWidget, ptPath, txtText):
        self.tsMACList[ptPath][5] = txtText
        
        mac = self.tsMACList[ptPath][0]
        self.setNicknameForMAC(mac, txtText)
    
    def setNicknameForMAC(self, mac, nickname):
        self.sql_c.execute("UPDATE macs_data SET nickname=? WHERE mac=?", (nickname, mac,))
        self.sql_conn.commit()
        
        for i in range(len(self.tsMACList)):
            treeiter = self.tsMACList.get_iter(Gtk.TreePath(i))
            row = self.tsMACList[treeiter]
            
            if row[0] == mac:
                row[5] = nickname
    
    def prepareMACList(self):
        self.tsMACList = Gtk.TreeStore(str, str, str, int, str, str, bool, str, str)
        
        self.fltMACFilter = self.tsMACList.filter_new()
        self.fltMACFilter.set_visible_func(self.filterMACs, None)
        
        self.tmsMACList = Gtk.TreeModelSort(self.fltMACFilter)
        
        self.vwMACList = Gtk.TreeView(self.tmsMACList)
        
        rdAddress = Gtk.CellRendererText()
        colAddress = Gtk.TreeViewColumn("MAC Address", rdAddress, text=0 if not self.bDemonstration else 7)
        colAddress.set_sort_column_id(0)
        
        rdNickname = Gtk.CellRendererText()
        rdNickname.set_property("editable", True)
        rdNickname.connect("edited", self.nicknameEdited)
        colNickname = Gtk.TreeViewColumn("Nickname", rdNickname, text=5)
        colNickname.set_sort_column_id(5)
        colNickname.add_attribute(rdNickname, "editable", 6)
        
        rdIdentity = Gtk.CellRendererText()
        colIdentity = Gtk.TreeViewColumn("Identity", rdIdentity, text=4 if not self.bDemonstration else 8)
        colIdentity.set_sort_column_id(4)
        
        rdDevice = Gtk.CellRendererText()
        colDevice = Gtk.TreeViewColumn("Device", rdDevice, text=2)
        colDevice.set_sort_column_id(2)
        
        rdVendor = Gtk.CellRendererText()
        colVendor = Gtk.TreeViewColumn("Vendor", rdVendor, text=1)
        colVendor.set_sort_column_id(1)
        
        self.vwMACList.append_column(colAddress)
        self.vwMACList.append_column(colNickname)
        self.vwMACList.append_column(colIdentity)
        self.vwMACList.append_column(colDevice)
        self.vwMACList.append_column(colVendor)
        
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
        
        # MAC History list
        self.tsMACHistoryList = Gtk.TreeStore(str, str)
        self.vwMACHistoryList = Gtk.TreeView(self.tsMACHistoryList)
        
        rdAddress = Gtk.CellRendererText()
        colAddress = Gtk.TreeViewColumn("MAC Address", rdAddress, text=0 if not self.bDemonstration else 1)
        colAddress.set_sort_column_id(0)
        self.vwMACHistoryList.append_column(colAddress)
    
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
        
        dlg.set_resizable(False)
        
        content_box = dlg.get_content_area()
        
        listbox = Gtk.ListBox()
        scwList = Gtk.ScrolledWindow()
        scwList.add(listbox)
        scwList.set_min_content_height(200)
        
        vboxContent = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
        vboxContent.pack_start(scwList, True, True, 0)
        content_box.add(vboxContent)
        
        select_row = None
        id_mac = self.identityForMAC(mac)
        self.assignExistingIdentity = id_mac
        
        for identity in self.lsIdentities:
            row = Gtk.ListBoxRow()
            row.value = identity[2]
            
            highlight = False
            if identity[2] == id_mac:
                select_row = row
                highlight = True
            
            label = Gtk.Label()
            label.set_justify(Gtk.Justification.LEFT)
            
            if highlight:
                label.set_markup("<b>" + identity[0] + "</b>")
            else:
                label.set_markup(identity[0])
            
            hbx = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=6)
            hbx.pack_start(label, True, True, 0)
            
            row.add(hbx)
            listbox.add(row)
        
        if select_row:
            listbox.select_row(select_row)
        
        if len(self.lsIdentities) > 0 and not self.assignExistingIdentity:
            listbox.select_row(listbox.get_row_at_index(0))
        
        listbox.connect("row-activated", self.selectedIdentityAssign)
        
        current_identity_id = self.identityForMAC(mac)
        self.assignExistingIdentity = current_identity_id
        
        hbox = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=20)
        labelnn = Gtk.Label("Device Nickname", xalign=0)
        entrynn = Gtk.Entry()
        entrynn.set_text(nickname)
        hbox.pack_start(labelnn, False, False, 3)
        hbox.pack_end(entrynn, True, True, 3)
        
        vboxContent.pack_start(hbox, False, False, 0)
        
        dlg.show_all()
        result = dlg.run()
        
        if result == Gtk.ResponseType.OK:
            identity_id = self.assignExistingIdentity
            identity_name = self.nameForIdentity(identity_id)
            
            self.assignMACToIdentity(mac, identity_id)
            self.setNicknameForMAC(mac, entrynn.get_text())
        elif result == Gtk.ResponseType.CANCEL:
            # Do nothing.
            pass
        
        dlg.destroy()
    
    def selectedIdentityAssign(self, wdgWidget, selected):
        self.assignExistingIdentity = selected.value
    
    def clickMACList(self, wdgWidget, evEvent):
        if evEvent.type == Gdk.EventType.BUTTON_PRESS and evEvent.button == 3:
            if len(self.tsMACList) > 0:
                path, column, a, b = wdgWidget.get_path_at_pos(evEvent.x, evEvent.y)
                
                if path:
                    wdgWidget.row_activated(path, column)
                    wdgWidget.set_cursor(path)
                    
                    model, treeiter = self.vwMACList.get_selection().get_selected()
                    if treeiter != None:
                        self.mnuMACs.popup(None, None, None, None, evEvent.button, evEvent.time)
                        
                        return True
    
    def onDrawTimeline(self, wdg, ctx):
        width = wdg.get_allocation().width
        height = wdg.get_allocation().height
        
        ctx.set_antialias(cairo.ANTIALIAS_NONE)
        
        ctx.set_source_rgb(1, 1, 1)
        ctx.rectangle(0, 0, width, height)
        ctx.fill()
        
        ticks_margin_top = -7
        
        height_per_mac = 25
        margin_top = 50
        margin_side = 25
        bar_thickness = 10
        
        macs = self.sortedEventMACs()
        widest_mac = 0
        mac_height = 0
        
        for mac in macs:
            xbearing, ybearing, txt_width, txt_height, xadvance, yadvance = ctx.text_extents(mac)
            if txt_width > widest_mac:
                widest_mac = txt_width
            
            mac_height = txt_height
        
        bars_begin_x = margin_side + widest_mac + 10
        bars_begin_y = margin_top - bar_thickness / 2 - mac_height / 2
        bars_width = width - (margin_side * 2) - (widest_mac + 10)
        bars_height = height_per_mac * len(macs)
        
        # This is the unix timestamp of zero seconds into the current
        # day
        loc_time = time.gmtime(self.startTime)
        strtime = ("0" if loc_time.tm_mday < 10 else "") + str(loc_time.tm_mday) + " " + ("0" if loc_time.tm_mday < 10 else "") + str(loc_time.tm_mon) + " " + str(loc_time.tm_year)
        lt = time.mktime(time.strptime(strtime, "%d %m %Y"))
        
        # Prepare the MAC attendance segments
        arrSegments = {}
        for mac in macs:
            arrSegments[mac] = []
            on = False
            on_time = -1
            events = self.macEvents(mac)
            
            for event in events:
                if on == False:
                    if event["event_type"] == EventType.MACAdded:
                        on = True
                        on_time = event["time"]
                else:
                    if event["event_type"] == EventType.MACRemoved:
                        on = False
                        arrSegments[mac].append([on_time - lt, event["time"] - lt])
                        on_time = -1
            
            if on  == True:
                # This is the currently running segment
                on = False
                arrSegments[mac].append([on_time - lt, self.currentTime() - lt])
                on_time = -1
        
        # Draw the actual MAC attendances
        index = 0
        for mac in macs:
            ctx.set_source_rgb(0, 0, 0)
            ctx.move_to(margin_side, margin_top + index * height_per_mac)
            ctx.show_text(mac)
            
            color = self.color(index)
            ctx.set_source_rgb(color[0], color[1], color[2])
            
            y = bars_begin_y + index * height_per_mac
            h = bar_thickness
            day_length = 86400
            
            for segment in arrSegments[mac]:
                time_begin = segment[0]
                time_end = segment[1]
                
                x = bars_begin_x + bars_width * (time_begin / day_length)
                w = bars_width * (time_end - time_begin) / day_length
                
                ctx.rectangle(x, y, w, h)
            
            ctx.fill()
            
            index = index + 1
        
        alt = self.startTime# Time.mktime(time.localtime())
        
        tdiff = alt - lt
        day_secs = 86400
        
        unknown_width = bars_width * (tdiff / day_secs)
        diagonal_stroke_distance = 10
        angle = math.radians(225.0)
        
        strokes_horizontal = int(bars_width / diagonal_stroke_distance)
        strokes_vertical = int(bars_height / diagonal_stroke_distance)
        
        for i in range(len(macs)):
            x = bars_begin_x
            y = bars_begin_y + ticks_margin_top + i * height_per_mac + height_per_mac / 2 - bar_thickness / 2
            w = unknown_width
            h = bar_thickness
            
            ctx.rectangle(x, y, w, h)
        
        ctx.clip()
        
        ctx.set_source_rgb(1, 1, 1)
        ctx.rectangle(bars_begin_x, bars_begin_y + ticks_margin_top, bars_width, bars_height)
        ctx.fill()
        
        ctx.set_source_rgb(0.8, 0.8, 0.8)
        ctx.set_line_width(1.0)
        
        for i in range(strokes_horizontal + strokes_vertical):
            w = i * diagonal_stroke_distance
            
            ctx.move_to(bars_begin_x + w, bars_begin_y + ticks_margin_top)
            ctx.line_to(bars_begin_x + w - bars_height * (math.cos(angle) / math.sin(angle)), bars_begin_y + bars_height + ticks_margin_top)
            
            ctx.stroke()
        
        for i in range(len(macs)):
            x = bars_begin_x
            y = bars_begin_y + ticks_margin_top + i * height_per_mac + height_per_mac / 2 - bar_thickness / 2 + 1
            w = unknown_width
            h = bar_thickness - 1
            
            ctx.rectangle(x, y, w, h)
            
            ctx.stroke()
        
        ctx.reset_clip()
        
        ctx.set_source_rgb(0, 0, 0)
        ctx.set_line_width(1.0)
        
        ctx.move_to(bars_begin_x + unknown_width, bars_begin_y + ticks_margin_top)
        ctx.line_to(bars_begin_x + unknown_width, bars_begin_y + bars_height + ticks_margin_top)
        ctx.stroke()
        
        ctx.move_to(bars_begin_x, bars_begin_y + ticks_margin_top)
        ctx.line_to(bars_begin_x, bars_begin_y + bars_height + ticks_margin_top)
        ctx.stroke()
        
        tick_fraction = bars_width / 24.0
        
        for i in range(25):
            if i * tick_fraction >= unknown_width:
                ctx.set_source_rgb(0.0, 0.0, 0.0)
                
                if i == 0 or i == 24:
                    ctx.move_to(i * tick_fraction + bars_begin_x, bars_begin_y + ticks_margin_top)
                    ctx.line_to(i * tick_fraction + bars_begin_x, bars_begin_y + bars_height + ticks_margin_top)
                else:
                    for j in range(len(macs)):
                        base_x = i * tick_fraction + bars_begin_x
                        base_y = bars_begin_y + j * height_per_mac
                        
                        ctx.move_to(base_x, base_y - 3)
                        ctx.line_to(base_x, base_y + bar_thickness + 3)
                        
                        sub_ticks = 5
                        
                        if tick_fraction < 20:
                            sub_ticks = 1
                        elif tick_fraction < 30:
                            sub_ticks = 3
                        
                        for k in range(sub_ticks):
                            tick_x_offset = float((float(k + 1.0) / (float(sub_ticks) + 1.0)))
                            
                            ctx.move_to(base_x + tick_fraction * tick_x_offset,
                                        base_y - 0)
                            ctx.line_to(base_x + tick_fraction * tick_x_offset,
                                        base_y + bar_thickness + 0)
            else:
                ctx.set_source_rgb(0.9, 0.9, 0.9)
            
            tick_text = str(i)
            xbearing, ybearing, txt_width, txt_height, xadvance, yadvance = ctx.text_extents(tick_text)
            ctx.move_to(i * tick_fraction + bars_begin_x - txt_width / 2, bars_begin_y + ticks_margin_top - txt_height)
            
            ctx.show_text(tick_text)
            
            ctx.stroke()
    
    def redrawTimeline(self):
        self.daTimeline.queue_draw()
    
    def prepareTimelineView(self):
        vbxTimeline = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
        
        self.daTimeline = Gtk.DrawingArea()
        self.daTimeline.connect("draw", self.onDrawTimeline)
        
        self.sclTimelineSlider = Gtk.HScale()
        self.sclTimelineSlider.connect("value-changed", self.timelineScaleValueChanged)
        self.setTimelineMax(100)
        
        GObject.timeout_add_seconds(1, self.redrawTimeline)
        
        hbxTimeline = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=6)
        
        vbxTimeline.pack_start(self.withScrolledWindow(self.daTimeline), True, True, 0)
        # TODO(winkler): Flip, no fill levels, no digits, increment step discrete
        #vbxTimeline.pack_start(self.sclTimelineSlider, False, False, 0)
        
        # NOTE(winkler): The scale will be used to set the resolution
        # of the timeline. This feature has not been implemented yet,
        # and so the scale is not added to the UI at the moment.
        
        self.stkStack.add_titled(vbxTimeline, "timeline", "Timeline")
        
        self.timelinePosition = 0.0
        
    def setTimelineHeight(self, height):
        self.daTimeline.set_size_request(-1, height)
    
    def setTimelineMax(self, value):
        self.sclTimelineSlider.set_range(0, value)
    
    def timelineScaleValueChanged(self, wdg):
        self.timelinePosition = self.sclTimelineSlider.get_value()
        self.daTimeline.queue_draw()
    
    def prepareIdentityView(self):
        self.prepareIdentityFlow()
        
        hbxIdentities = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=6)
        hbxIdentities.pack_start(self.scwFlow, True, True, 0)
        
        vbxControls = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
        
        self.idImage = Gtk.Image()
        vbxControls.pack_start(self.idImage, False, True, 0)
        self.idImage.set_size_request(100, 100)
        
        # NOTE(winkler): Commented out until this feature gets actually used.
        #hbxIdentities.pack_start(vbxControls, False, True, 0)
        
        self.stkStack.add_titled(hbxIdentities, "identities", "Identities")
    
    def prepareIdentityFlow(self):
        self.scwFlow = Gtk.ScrolledWindow()
        self.scwFlow.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)
        
        self.ivIdentities = Gtk.IconView()
        self.ivIdentities.set_text_column(ID_COLUMN_TEXT)
        self.ivIdentities.set_pixbuf_column(ID_COLUMN_PIXBUF)
        
        self.lsIdentities = Gtk.ListStore(str, GdkPixbuf.Pixbuf, int)
        
        self.fltIdentities = self.lsIdentities.filter_new()
        self.fltIdentities.set_visible_func(self.filterIdentities, None)
        
        self.ivIdentities.set_model(self.fltIdentities)
        
        self.mnuIdentityEdit = Gtk.Menu()
        mniRename = Gtk.MenuItem("Rename")
        mniRename.connect("activate", self.renameIdentityInList)
        self.mnuIdentityEdit.add(mniRename)
        mniDelete = Gtk.MenuItem("Delete")
        mniDelete.connect("activate", self.deleteIdentityFromList)
        self.mnuIdentityEdit.add(mniDelete)
        
        self.mnuIdentityEdit.show_all()
        
        self.ivIdentities.connect("button-press-event", self.clickIdentityIV)
        
        self.scwFlow.add(self.ivIdentities)
        
        self.selected_identity = None
    
    def renameIdentityInList(self, wdg):
        for i in range(len(self.lsIdentities)):
            treeiter = self.lsIdentities.get_iter(Gtk.TreePath(i))
            row = self.lsIdentities[treeiter]
            
            if row[2] == self.selected_identity:
                rename = Gtk.Dialog("Rename Identity", self.winRef, Gtk.DialogFlags.MODAL,
                                    (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
                                     Gtk.STOCK_OK, Gtk.ResponseType.OK))
                
                name = self.nameForIdentity(self.selected_identity)
                entry = Gtk.Entry()
                entry.set_text(name)
                
                box = rename.get_content_area()
                box.add(entry)
                rename.show_all()
                
                if rename.run() == Gtk.ResponseType.OK:
                    new_name = entry.get_text()
                    row[0] = new_name
                    
                    for i in range(len(self.tsMACList)):
                        treeiter = self.tsMACList.get_iter(Gtk.TreePath(i))
                        row = self.tsMACList[treeiter]
                        
                        if row[3] == self.selected_identity:
                            row[4] = new_name
                    
                    self.sql_c.execute("UPDATE identities SET name=? WHERE id=?", (new_name, self.selected_identity,))
                    self.sql_conn.commit()
                
                rename.destroy()
    
    def deleteIdentityFromList(self, wdg):
        if self.selected_identity != None:
            confirm = Gtk.Dialog("Delete Identity", self.winRef, Gtk.DialogFlags.MODAL,
                                 (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
                                  Gtk.STOCK_OK, Gtk.ResponseType.OK))
            
            name = self.nameForIdentity(self.selected_identity)
            label = Gtk.Label("Delete Identity '%s'?" % name)
            
            box = confirm.get_content_area()
            box.add(label)
            confirm.show_all()
            
            if confirm.run() == Gtk.ResponseType.OK:
                self.deleteIdentity(self.selected_identity)
                self.selected_identity = None
            
            confirm.destroy()
    
    def deleteIdentity(self, identity_id):
        self.sql_c.execute("DELETE FROM identities WHERE id=?", (identity_id,))
        self.sql_c.execute("DELETE FROM macs WHERE identity_id=?", (identity_id,))
        self.sql_conn.commit()
        
        for i in range(len(self.lsIdentities)):
            treeiter = self.lsIdentities.get_iter(Gtk.TreePath(i))
            row = self.lsIdentities[treeiter]
            
            if row[2] == identity_id:
                self.lsIdentities.remove(treeiter)
                break
        
        for i in range(len(self.tsMACList)):
            treeiter = self.tsMACList.get_iter(Gtk.TreePath(i))
            row = self.tsMACList[treeiter]
            
            if row[3] == identity_id:
                row[3] = -1
                row[4] = ""
    
    def clickIdentityIV(self, wdg, evt):
        if evt.type == Gdk.EventType.BUTTON_PRESS and evt.button == 3:
            if len(self.lsIdentities) > 0:
                path = self.ivIdentities.get_path_at_pos(evt.x, evt.y)
                
                if path:
                    self.ivIdentities.select_path(path)
                    self.ivIdentities.set_cursor(path, None, False)
                    
                    sel_itms = self.ivIdentities.get_selected_items()
                    if len(sel_itms) > 0:
                        selected = self.lsIdentities[sel_itms[0]]
                        identity_id = selected[2]
                        self.selected_identity = identity_id
                        
                        self.mnuIdentityEdit.popup(None, None, None, None, evt.button, evt.time)
    
    def triggerQuit(self, evEvent, dtData):
        Gtk.main_quit()
    
    def show(self):
        self.winRef.show_all()
    
    def withScrolledWindow(self, wdgWidget):
        scwScrolled = Gtk.ScrolledWindow()
        scwScrolled.add(wdgWidget)
        
        return scwScrolled
