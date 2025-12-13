# 主程序
./KeyBonk.exe: ./src/obj/main.o ./src/obj/global.o ./src/obj/main_window.o ./src/obj/setting.o ./src/obj/about.o ./src/obj/keyboard_hook.o ./src/obj/utils.o ./resource/resources.o
	g++ ./src/obj/main.o ./src/obj/global.o ./src/obj/main_window.o ./src/obj/setting.o ./src/obj/about.o ./src/obj/keyboard_hook.o ./src/obj/utils.o ./resource/resources.o \
	-std=c++17 -o ./KeyBonk.exe \
	-mwindows -municode  \
	-luser32 -lgdi32 -lole32 -lgdiplus -lwinmm

# main.o
./src/obj/main.o: ./src/main.cpp ./include/keybonk_global.hpp ./include/window_manager.hpp ./include/keyboard_hook.hpp ./resource/resources.hpp
	g++ -o ./src/obj/main.o ./src/main.cpp -c -std=c++17 -I./include

# global.o
./src/obj/global.o: ./src/global.cpp ./include/keybonk_global.hpp
	g++ -o ./src/obj/global.o ./src/global.cpp -c -std=c++17 -I./include

# main_window.o
./src/obj/main_window.o: ./src/main_window.cpp
	g++ -o ./src/obj/main_window.o ./src/main_window.cpp -c -std=c++17 -I./include

# setting.o
./src/obj/setting.o: ./src/setting.cpp
	g++ -o ./src/obj/setting.o ./src/setting.cpp -c -std=c++17 -I./include

# about.o
./src/obj/about.o: ./src/about.cpp
	g++ -o ./src/obj/about.o ./src/about.cpp -c -std=c++17 -I./include

# keyboard_hook.o
./src/obj/keyboard_hook.o: ./src/keyboard_hook.cpp ./include/keyboard_hook.hpp ./include/utils.hpp
	g++ -o ./src/obj/keyboard_hook.o ./src/keyboard_hook.cpp -c -std=c++17 -I./include

# utils.o
./src/obj/utils.o: ./src/utils.cpp ./include/utils.hpp
	g++ -o ./src/obj/utils.o ./src/utils.cpp -c -std=c++17 -I./include

# 资源文件
./resource/resources.o: ./resource/resources.rc ./resource/resources.hpp
	windres .\resource\resources.rc -F pe-x86-64 -o .\resource\resources.o