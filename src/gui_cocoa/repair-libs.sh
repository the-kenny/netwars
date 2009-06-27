# This script is used to repair the search path for libraries of Netwars.app
# It makes it possible to deliver dynamic libraries (.dylib) in the appbundle
# so that no other installation is required 

mkdir Netwars.app/Contents/Frameworks/

cp /usr/local/lib/libboost_filesystem-mt.dylib Netwars.app/Contents/Frameworks/
cp /usr/local/lib/libboost_signals-mt.dylib Netwars.app/Contents/Frameworks/
cp /usr/local/lib/libboost_system-mt.dylib Netwars.app/Contents/Frameworks/

# boost.filesystem
install_name_tool -id \
 @executable_path/../Frameworks/libboost_filesystem-mt.dylib \
 Netwars.app/Contents/Frameworks/libboost_filesystem-mt.dylib 

install_name_tool -change libboost_filesystem-mt.dylib \
 @executable_path/../Frameworks/libboost_filesystem-mt.dylib \
 Netwars.app/Contents/MacOS/Netwars 

# boost.signals
install_name_tool -id \
 @executable_path/../Frameworks/libboost_signals-mt.dylib \
 Netwars.app/Contents/Frameworks/libboost_signals-mt.dylib 

install_name_tool -change libboost_signals-mt.dylib \
 @executable_path/../Frameworks/libboost_signals-mt.dylib \
 Netwars.app/Contents/MacOS/Netwars 

# boost.system
install_name_tool -id \
 @executable_path/../Frameworks/libboost_system-mt.dylib \
 Netwars.app/Contents/Frameworks/libboost_system-mt.dylib 

install_name_tool -change libboost_system-mt.dylib \
 @executable_path/../Frameworks/libboost_system-mt.dylib \
 Netwars.app/Contents/MacOS/Netwars 

# changing boost.filesystem to find boost.system 
install_name_tool -change libboost_system-mt.dylib \
 @executable_path/../Frameworks/libboost_system-mt.dylib \
 Netwars.app/Contents/Frameworks/libboost_filesystem-mt.dylib