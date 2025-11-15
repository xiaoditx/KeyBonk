./KeyBonk.exe: ./src/obj/main.o
	g++ ./src/obj/main.o -std=c++17 -o ./KeyBonk.exe -mwindows -municode  -luser32 -lgdi32 -lole32

./src/obj/main.o: ./src/main.cpp
	g++ -o ./src/obj/main.o ./src/main.cpp -c -std=c++17