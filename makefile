OPENNI_INCLUDE_PATH=/usr/local/Cellar/openni/1.5.7.10/include/ni #You may need to adapt me
CXXFLAGS=$(shell pkg-config opencv --cflags) -I$(OPENNI_INCLUDE_PATH) -Wall -Wextra
LDFLAGS=$(shell pkg-config opencv --libs) -lOpenNI
 
SRC_FILES=$(shell find . -iname "*.cpp")
HDR_FILES=$(shell find . -iname "*.h")
OBJ_FILES=$(SRC_FILES:%.cpp=%.o)
 
EXENAME=assignment2
 
all: $(EXENAME)
 
$(EXENAME): $(OBJ_FILES) $(HDR_FILES)
	$(CXX) -o $@ $(OBJ_FILES) $(LDFLAGS)
 
clean:
	$(RM) *.o
	$(RM) $(EXENAME)
 
run:
	./$(EXENAME)
 
.PHONY: all clean run