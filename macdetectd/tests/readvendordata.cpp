// macdetect, a daemon and clients for detecting MAC addresses
// Copyright (C) 2015 Jan Winkler <jan.winkler.84@gmail.com>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

/** \author Jan Winkler */


// System
#include <cstdlib>

// MAC Detect
#include <macdetectd/Data.h>


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  macdetect::Data dtData;
  if(dtData.readVendors()) {
    macdetect::Data::Vendor vdApple = dtData.vendorForMAC("00:CD:FE:00:00:00");
    
    if(vdApple.bValid) {
      if(vdApple.strVendor == "Apple, Inc.") {
	macdetect::Data::Vendor vdInvalid = dtData.vendorForMAC("FF:FF:FF:00:00:00");
	
	nReturnvalue = EXIT_SUCCESS;
      } else {
	std::cerr << "Vendor name not as expected; got: '" << vdApple.strVendor << "'" << std::endl;
      }
    } else {
      std::cerr << "Retrieved vendor invalid" << std::endl;
    }
  } else {
    std::cerr << "Couldn't read vendor data" << std::endl;
  }
  
  return nReturnvalue;
}
