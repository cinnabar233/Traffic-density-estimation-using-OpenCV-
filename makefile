all :
	g++ main.cpp -o exec -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
clean: 
	rm exec 
