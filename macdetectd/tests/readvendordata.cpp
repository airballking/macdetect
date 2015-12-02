#include <cstdlib>

#include <macdetectd/Data.h>


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  macdetect::Data dtData;
  if(dtData.readVendors()) {
    macdetect::Data::Vendor vdApple = dtData.vendorForMAC("00:CD:FE:00:00:00");
    
    if(vdApple.bValid && vdApple.strVendor == "Apple, Inc.") {
      nReturnvalue = EXIT_SUCCESS;
    }
  }
  
  return nReturnvalue;
}
