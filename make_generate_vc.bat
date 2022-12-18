set PATH=%PATH%;C:\Qt\5.15.2\msvc2019_64\bin;
set PATH=%PATH%;"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.29.30133\bin\HostX64\x64"
set QMAKESPEC=C:/Qt/5.15.2/msvc2019_64/mkspecs/win32-g++


qmake "CONFIG+=debug_and_release" GestureHandRecognition.pro -spec win32-msvc
qmake "CONFIG+=debug_and_release" GestureHandRecognition.pro -spec win32-msvc -tp vc