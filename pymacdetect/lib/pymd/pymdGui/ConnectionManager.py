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


class ConnectionManager:
    def __init__(self, parent):
        self.parent = parent
        
        self.prepareUI()
    
    def prepareUI(self):
        self.prepareWindow()
        self.prepareServerList()
        
        self.winRef.add(self.withScrolledWindow(self.vwServerList))
        
        accel = Gtk.AccelGroup()
        
        accel.connect(Gdk.keyval_from_name('Escape'), 0, 0, lambda *args: self.on_accel_pressed("close", ""))
        
        self.winRef.add_accel_group(accel)
    
    def on_accel_pressed(self, param_1, param_2):
        if param_1 == "close":
            self.triggerClose(None, None)
    
    def prepareWindow(self):
        self.winRef = Gtk.Window()
        self.winRef.connect("delete-event", self.triggerClose)
        self.winRef.set_default_size(500, 400)
        self.winRef.set_border_width(10)
        self.winRef.set_position(Gtk.WindowPosition.CENTER)
        
        hdrTitle = Gtk.HeaderBar(title="Connection Manager")
        hdrTitle.props.show_close_button = True
        
        self.btnConnect = Gtk.Button("Connect")
        self.btnConnect.connect("clicked", self.clickConnect)
        hdrTitle.add(self.btnConnect)
        
        self.winRef.set_titlebar(hdrTitle)
    
    def clickConnect(self, wdgWidget):
        model, selected = self.vwServerList.get_selection().get_selected()
        
        if selected:
            self.what = "connect"
            self.where = model[selected][1]
        else:
            self.what = "cancel"
            self.where = None
        
        self.winRef.hide()
        self.winRef.set_modal(False)
        
        self.parent.processConnectionManager()
    
    def activateItem(self, wdg, itm, btn):
        self.clickConnect(wdg)
    
    def prepareServerList(self):
        self.lsServerList = Gtk.ListStore(str, str, str)
        
        self.vwServerList = Gtk.TreeView(self.lsServerList)
        self.vwServerList.connect("row-activated", self.activateItem)
        
        rdNickname = Gtk.CellRendererText()
        colNickname = Gtk.TreeViewColumn("Nickname", rdNickname, text=0)
        
        rdIPHostname = Gtk.CellRendererText()
        colIPHostname = Gtk.TreeViewColumn("IP / Hostname", rdIPHostname, text=1)
        
        rdMode = Gtk.CellRendererText()
        colMode = Gtk.TreeViewColumn("Mode", rdIPHostname, text=2)
        
        self.vwServerList.append_column(colNickname)
        self.vwServerList.append_column(colIPHostname)
        self.vwServerList.append_column(colMode)
    
    def triggerClose(self, evEvent, dtData):
        self.what = "cancel"
        self.where = None
        
        self.winRef.hide()
        self.winRef.set_modal(False)
        
        self.parent.processConnectionManager()
        
        return True
    
    def show(self):
        self.winRef.show_all()
    
    def withScrolledWindow(self, wdgWidget):
        scwScrolled = Gtk.ScrolledWindow()
        scwScrolled.add(wdgWidget)
        
        return scwScrolled
    
    def ipInList(self, strIP):
        for iter in self.lsServerList:
            if iter[1] == strIP:
                return True
        
        return False
    
    def addServer(self, siServerInfo):
        if not self.ipInList(siServerInfo["ip"]):
            self.lsServerList.append([siServerInfo["name"], siServerInfo["ip"], "automatic"])
