if not exist build mkdir build_debug
cd build_debug
cmake -S ../ -B . -G "MinGW Makefiles" -DDEBUG=1
mingw32-make.exe
move debug__stalcraft_auction_monitor.exe ../
cd ..
pause