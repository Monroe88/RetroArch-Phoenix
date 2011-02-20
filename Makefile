TARGET = ssnes-phoenix

CXXFILES = $(wildcard *.cpp)
CFILES = $(wildcard *.c)
OBJ = phoenix/phoenix.o $(CXXFILES:.cpp=.o) $(CFILES:.c=.o)
HEADERS = $(wildcard *.h) $(wildcard *.hpp)

QT_CFLAGS = $(shell pkg-config --cflags QtCore QtGui)
QT_LIBS = $(shell pkg-config --libs QtCore QtGui)

INCLUDES = -Iphoenix

all: $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -std=gnu++0x -c -o $@ $<

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -std=gnu99 -c -o $@ $<

phoenix/phoenix.o: phoenix/phoenix.cpp phoenix/qt/qt.moc
	$(CXX) $(CXXFLAGS) $(INCLUDES) -std=gnu++0x -DPHOENIX_QT $(QT_CFLAGS) -c -o $@ $<

%.moc: %.moc.hpp
	moc -i -o $@ $<


$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(QT_LIBS)

clean:
	rm -f *.o
	rm -f $(TARGET)
	rm -f phoenix/*.o

.PHONY: clean