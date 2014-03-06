CC=gcc
OPTFLAGS= -Wall -g

NM=genMaterials

INCLUDE_PATH=  -I/usr/include -I/usr/include/opencv -I/home/drn2369/usr/include
LIB_PATH=  -L/usr/lib

CPPFLAGS=$(OPTFLAGS) $(INCLUDE_PATH)


NM_OBJS=  main.o  ortho.o extractor.o points.o IO.o utils.o patch.o cameraInfo.o
NM_LIBS= -lgdal `pkg-config --libs opencv`


all: $(NM) 
%.o : %.cpp
	$(CXX) -c  $(CPPFLAGS) $<

$(NM): $(NM_OBJS)
	$(CXX) -o $@ $(CPPFLAGS) $(LIB_PATH) $(NM_OBJS) $(NM_LIBS)


clean:
	rm -f *.o *~ $(NM)
