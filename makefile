
.DEFAULT:
	@ echo "Type ""make all"" to create the executable and type ""make run args=<filename>"" to run"


all :
	g++ main.cpp -o exec -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
	g++ main2.cpp -o  -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
clean: 
	rm -f exec 
	rm -f "Cropped traffic.jpg"
	rm -f "Cropped empty.jpg"
	rm -f "Transformed traffic.jpg"
	rm -f "Transformed empty.jpg"
	rm -f exec2
	rm -f out.txt
	rm -f plot.jpg
run :
	./exec2 traffic.mp4
main2 : 
	g++ main2.cpp -o exec2 -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
main :
	g++ main.cpp -o exec -pthread -std=c++11  `pkg-config --cflags --libs opencv4`

