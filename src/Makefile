

GXX = @echo compiling $@ && g++
LINK = @echo lingking $@ && g++
CFLAGS = -g -Wall -W
#DEFINE = -DDEBUG
DEFINE = -DNDEBUG -O2
INCLUDE = -I./
LIBS = 

#如果需要其他头文件路径,就把下面的改一下
#INCLUDE += -I../

#如果需要其他库,就把下面的改一下
#LIBS += -L./path/ -lname

OBJECTS = main.o \
		  fetchimgfromfile.o \
		 #othersrc.o


TARGET = main

$(TARGET) : $(OBJECTS)
	$(LINK) $(CFLAGS) $(DEFINE) -o $@ $^ $(LIBS)

.cpp.o:
	$(GXX) -c $(CFLAGS) $(DEFINE) $(INCLUDE) -o $@ $<

clean:
	rm -rf $(TARGET) *.o *.swp
