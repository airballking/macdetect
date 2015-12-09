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


import pymacdetect


class Client:
    def __init__(self):
        self.mdcClient = pymacdetect.createClient()
    
    def __exit__(self):
        pymacdetect.destroyClient(self.mdcClient)
    
    def connect(self, strIP):
        return pymacdetect.connectClient(self.mdcClient, strIP)
    
    def disconnect(self):
        return pymacdetect.disconnectClient(self.mdcClient)
    
    def enableStream(self, strDeviceName):
        return self.send({"request": {"content": "enable-stream",
                                      "subs": {"device-name": {"content": strDeviceName}}}});
    
    def disableStream(self, strDeviceName):
        return self.send({"request": {"content": "disable-stream",
                                      "subs": {"device-name": {"content": strDeviceName}}}});
    
    def receive(self):
        return pymacdetect.receive(self.mdcClient)
    
    def send(self, dicMessage):
        return pymacdetect.send(self.mdcClient, dicMessage)
