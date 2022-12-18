set PATH=%PATH%;C:/Qt/5.15.2/mingw81_64/bin
set QMAKESPEC=C:/Qt/5.15.2/mingw81_64/mkspecs/win32-g++
qmake -makefile -o Makefile  CONFIG+=debug_and_release GestureHandRecognition.pro