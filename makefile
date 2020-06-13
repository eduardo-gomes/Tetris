CXX=g++
CXXFLAGS=-std=c++17 -pedantic-errors -Wall -Wextra -Wsign-conversion -Werror
LIBS=-lSDL2 -lGL -lm -lvorbis -lvorbisfile -ldl -lpthread
DBG?=-g
OPTIMIZATION?=-O3 -march=native -mfpmath=sse

LIBS=-lSDL2 -lGL -lm -lvorbis -lvorbisfile -ldl -lpthread


DuEngineDIR=DuEngine
_INLCUDE_F=dependencies/include dependencies/imgui .
INCLUDE_F=$(patsubst %, -I$(DuEngineDIR)/%, $(_INLCUDE_F))

LIBS_OBJ=libglad.so libimgui.so libDuEngine.so
Tetris.o: tetris.cpp $(LIBS_OBJ)
	$(CXX) -o $@ $< $(CXXFLAGS) $(DBG) $(INCLUDE_F) -L$(DuEngineDIR) -lglad -limgui -lDuEngine $(LIBS) -Wl,-rpath=$(DuEngineDIR)

.PHONY: clear Prepare clearAll DuEngineBuild
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

IMGUIDIR=dependencies/imgui
_IMGUISRC=imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_impl_opengl3.cpp imgui_impl_sdl.cpp imgui_widgets.cpp
IMGUISRC=$(patsubst %,$(IMGUIDIR)/%,$(_IMGUISRC))
