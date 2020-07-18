export CXX=g++
export CXXFLAGS=-std=c++17 -pedantic-errors -Wall -Wextra -Wsign-conversion -Werror
export LIBS=-lSDL2 -lGL -lm -ldl -lpthread
export DBG?=-g
export OPTIMIZATION?=-O3 -march=native -mfpmath=sse -flto

LIBS=-lSDL2 -lGL -lm -ldl -lpthread
LIBSW64=-lSDL2 -lopengl32 -lm -lpthread


DuEngineDIR=DuEngine
_INLCUDE_F=dependencies/include dependencies/imgui .
INCLUDE_F=$(patsubst %, -I$(DuEngineDIR)/%, $(_INLCUDE_F))

LIBS_OBJ=libglad.so libimgui.so libDuEngine.so
LIBS_OBJW64=libglad.dll libimgui.dll
Tetris.o: tetris.cpp $(LIBS_OBJ)
	$(CXX) -o $@ $< $(CXXFLAGS) $(DBG) $(INCLUDE_F) -L$(DuEngineDIR) -lglad -limgui -lDuEngine $(LIBS) -Wl,-rpath=$(DuEngineDIR) $(OPTIMIZATION)

Tetris.exe: tetris.cpp $(LIBS_OBJW64)
	$(CXX) -o $@ $< $(CXXFLAGS) $(DBG) $(INCLUDE_F) -L$(DuEngineDIR) -llibglad -llibDuEngine $(LIBSW64) -Wl,-rpath=$(DuEngineDIR) $(OPTIMIZATION) $(LIBSW64)

.PHONY: clear Prepare clearAll DuEngineBuild DuEngineBuildw64
clear:
	rm -f Tetris.o

clearAll: clear
	$(MAKE) -C DuEngine clear
clearLib:
	$(MAKE) -C DuEngine clearAll
Prepare:
	$(MAKE) -C DuEngine Prepare

$(LIBS_OBJ): DuEngineBuild
DuEngineBuild:
	$(MAKE) -C DuEngine build

$(LIBS_OBJW64): DuEngineBuildw64
DuEngineBuildw64:
	$(MAKE) -C DuEngine buildwin
