OPENNI_INCLUDE_PATH=/usr/local/Cellar/openni/1.5.7.10/include/ni #You may need to adapt me

BOOST_SUFFIX=-mt        # Change this to empty if you are not using multithreaded boost
BOOST_LIBS=boost_system boost_signals boost_thread
BOOST_LDFLAGS=$(BOOST_LIBS:%=-l%$(BOOST_SUFFIX))

CXXFLAGS+=$(shell pkg-config opencv --cflags) -I$(OPENNI_INCLUDE_PATH) -Wno-attributes
LDFLAGS+=$(shell pkg-config opencv --libs) -lOpenNI $(BOOST_LDFLAGS)
debug: CXXFLAGS += -g

SRC_FILES=$(shell find . -iname "*.cpp")
HDR_FILES=$(shell find . -iname "*.h")
OBJ_FILES=$(SRC_FILES:%.cpp=%.o)
DEP_FILES=$(SRC_FILES:%.cpp=%.d)

EXENAME=assignment5

all: $(EXENAME)

debug: all

%.d: %.cpp
	$(CXX) -MM $(CXXFLAGS) $< > $@

$(EXENAME): $(OBJ_FILES) $(HDR_FILES)
	$(CXX) -o $@ $(OBJ_FILES) $(LDFLAGS)

clean:
	$(RM) $(OBJ_FILES) $(DEP_FILES)
	$(RM) $(EXENAME)

run: $(EXENAME)
	./$(EXENAME)

.PHONY: all clean run

-include $(DEP_FILES)