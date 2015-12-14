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


from PyMACDetect import Client

cliClient = Client()

ip = "127.0.0.1"
#ip = "192.168.178.26"
#ip = "192.168.178.25"

if cliClient.connect(ip):
    cliClient.enableStream("wlan0")
    
    while True:
        packet = cliClient.receive()
        
        if packet:
            print packet
        else:
            print "Nope"
            break
else:
    print "Unable to connect to '" + ip + "'"
