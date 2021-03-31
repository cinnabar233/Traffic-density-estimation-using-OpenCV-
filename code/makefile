all:  method-1 method-2 method-3 method-4 sparseOpt baseline capture dense-opt-flow
	
clean: 
	rm -f *.out
	rm -f *.txt

method-1:
	g++ method-1.cpp -o method-1.out -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
	@ echo " Run the executable using the command ./method-1.out <filename> <number of frames to be skipped>"

method-2:
	g++ method-2.cpp -o method-2.out -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
	@ echo " Run the executable using the command ./method-2.out <filename> <resolution reduction factor>"
	
method-3:
	g++ method-3.cpp -o method-3.out -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
	@ echo " Run the executable using the command ./method-3.out <filename> <number of threads>"

method-4:
	g++ method-4.cpp -o method-4.out -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
	@ echo " Run the executable using the command ./method-4.out <filename> <number of threads>"

sparseOpt:
	g++ sparse-optical-flow.cpp -o sparse-optical-flow.out -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
	@ echo " Run the executable using the command ./sparse-optical-flow.out <filename> "

baseline:
	g++ baseline-queue-density.cpp -o baseline-queue-density.out -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
	@ echo " Run the executable using the command ./baseline-queue-density.out <filename> "
	g++ baseline-dynamic-density.cpp -o baseline-dynamic-density.out -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
	@ echo " Run the executable using the command ./baseline-dynamic-density.out <filename> "

capture:
	g++ vid_cap_time.cpp -o vidcap.out -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
	@ echo " Run the executable using the command ./vidcap.out <filename> "

dense-opt-flow:
	g++ denseOF-temporal-pthreads.cpp -o denseOF-temporal.out -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
	@ echo " Run the executable using the command ./denseOF-temporal.out <filename> <number of threads>"
	g++ denseOF-spatial-pthreads.cpp -o denseOF-spatial.out -pthread -std=c++11  `pkg-config --cflags --libs opencv4`
	@ echo " Run the executable using the command ./denseOF-spatial.out <filename> <number of threads>"
	

.DEFAULT:
	@ echo "Type ""make all"" to create all executables