TARGET=1C_kafka.so

SOURCES=1C_kafka.cpp \
	dllmain.cpp \
	stdafx.cpp 

LIBS=pthread

OBJECTS=$(SOURCES:.cpp=.o)
INCLUDES=-I../../include
#debug
#CXXLAGS=$(CXXFLAGS) $(INCLUDES) -m32 -finput-charset=WINDOWS-1251 -fPIC
#release
CXXFLAGS=$(CXXFLAGS) $(INCLUDES) -m32 -finput-charset=WINDOWS-1251 -fPIC -O3 -Wall

all: $(TARGET)

-include $(OBJECTS:.o=.d)

%.o: %.cpp
	g++ -c  $(CXXLAGS) $*.cpp -o $*.o
	g++ -MM $(CXXLAGS) $*.cpp >  $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

	#debug
	#$(TARGET): $(OBJECTS) Makefile
	#g++ $(CXXLAGS) -shared $(OBJECTS) -o $(TARGET) $(addprefix -l, $(LIBS))

	#release
	$(TARGET): $(OBJECTS) Makefile
	g++ $(CXXFLAGS) -shared $(OBJECTS) -o $(TARGET) $(addprefix -l, $(LIBS))
	strip --strip-unneeded $(TARGET)

clean:
	-rm $(TARGET) *.o *.d
