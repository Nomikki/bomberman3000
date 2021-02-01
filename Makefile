WORKDIR = %cd%

CC = gcc.exe
CXX = g++.exe
AR = ar.exe
LD = g++.exe
WINDRES = windres.exe

INC = -I..\\..\\sfml\\SFML-2.3\\include -Iinclude
CFLAGS = -Wall -fexceptions
RESINC = 
LIBDIR = -L..\\..\\sfml_64\\SFML-2.5.1\\lib
LIB = ..\\enet-1.3.13\\bin\\Release\\libenet.a
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS) -lsfml-system-d -lsfml-window-d -lsfml-graphics-d -lsfml-network-d -lsfml-audio-d
OBJDIR_DEBUG = obj\\Debug
DEP_DEBUG = 
OUT_DEBUG = standalone\\bomberman-klooni-d.exe

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s -lsfml-system -lsfml-window -lsfml-graphics -lsfml-network -lsfml-audio
OBJDIR_RELEASE = obj\\Release
DEP_RELEASE = 
OUT_RELEASE = standalone\\bomberman-klooni.exe

OBJ_DEBUG = $(OBJDIR_DEBUG)\\src\\unit.o $(OBJDIR_DEBUG)\\src\\sounds.o $(OBJDIR_DEBUG)\\src\\settings.o $(OBJDIR_DEBUG)\\src\\resources.o $(OBJDIR_DEBUG)\\src\\network.o $(OBJDIR_DEBUG)\\src\\main.o $(OBJDIR_DEBUG)\\src\\level.o $(OBJDIR_DEBUG)\\src\\common.o $(OBJDIR_DEBUG)\\src\\chat.o

OBJ_RELEASE = $(OBJDIR_RELEASE)\\src\\unit.o $(OBJDIR_RELEASE)\\src\\sounds.o $(OBJDIR_RELEASE)\\src\\settings.o $(OBJDIR_RELEASE)\\src\\resources.o $(OBJDIR_RELEASE)\\src\\network.o $(OBJDIR_RELEASE)\\src\\main.o $(OBJDIR_RELEASE)\\src\\level.o $(OBJDIR_RELEASE)\\src\\common.o $(OBJDIR_RELEASE)\\src\\chat.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	cmd /c if not exist standalone md standalone
	cmd /c if not exist $(OBJDIR_DEBUG)\\src md $(OBJDIR_DEBUG)\\src

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)\\src\\unit.o: src\\unit.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src\\unit.cpp -o $(OBJDIR_DEBUG)\\src\\unit.o

$(OBJDIR_DEBUG)\\src\\sounds.o: src\\sounds.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src\\sounds.cpp -o $(OBJDIR_DEBUG)\\src\\sounds.o

$(OBJDIR_DEBUG)\\src\\settings.o: src\\settings.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src\\settings.cpp -o $(OBJDIR_DEBUG)\\src\\settings.o

$(OBJDIR_DEBUG)\\src\\resources.o: src\\resources.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src\\resources.cpp -o $(OBJDIR_DEBUG)\\src\\resources.o

$(OBJDIR_DEBUG)\\src\\network.o: src\\network.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src\\network.cpp -o $(OBJDIR_DEBUG)\\src\\network.o

$(OBJDIR_DEBUG)\\src\\main.o: src\\main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src\\main.cpp -o $(OBJDIR_DEBUG)\\src\\main.o

$(OBJDIR_DEBUG)\\src\\level.o: src\\level.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src\\level.cpp -o $(OBJDIR_DEBUG)\\src\\level.o

$(OBJDIR_DEBUG)\\src\\common.o: src\\common.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src\\common.cpp -o $(OBJDIR_DEBUG)\\src\\common.o

$(OBJDIR_DEBUG)\\src\\chat.o: src\\chat.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src\\chat.cpp -o $(OBJDIR_DEBUG)\\src\\chat.o

clean_debug: 
	cmd /c del /f $(OBJ_DEBUG) $(OUT_DEBUG)
	cmd /c rd standalone
	cmd /c rd $(OBJDIR_DEBUG)\\src

before_release: 
	cmd /c if not exist standalone md standalone
	cmd /c if not exist $(OBJDIR_RELEASE)\\src md $(OBJDIR_RELEASE)\\src

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)\\src\\unit.o: src\\unit.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src\\unit.cpp -o $(OBJDIR_RELEASE)\\src\\unit.o

$(OBJDIR_RELEASE)\\src\\sounds.o: src\\sounds.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src\\sounds.cpp -o $(OBJDIR_RELEASE)\\src\\sounds.o

$(OBJDIR_RELEASE)\\src\\settings.o: src\\settings.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src\\settings.cpp -o $(OBJDIR_RELEASE)\\src\\settings.o

$(OBJDIR_RELEASE)\\src\\resources.o: src\\resources.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src\\resources.cpp -o $(OBJDIR_RELEASE)\\src\\resources.o

$(OBJDIR_RELEASE)\\src\\network.o: src\\network.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src\\network.cpp -o $(OBJDIR_RELEASE)\\src\\network.o

$(OBJDIR_RELEASE)\\src\\main.o: src\\main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src\\main.cpp -o $(OBJDIR_RELEASE)\\src\\main.o

$(OBJDIR_RELEASE)\\src\\level.o: src\\level.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src\\level.cpp -o $(OBJDIR_RELEASE)\\src\\level.o

$(OBJDIR_RELEASE)\\src\\common.o: src\\common.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src\\common.cpp -o $(OBJDIR_RELEASE)\\src\\common.o

$(OBJDIR_RELEASE)\\src\\chat.o: src\\chat.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src\\chat.cpp -o $(OBJDIR_RELEASE)\\src\\chat.o

clean_release: 
	cmd /c del /f $(OBJ_RELEASE) $(OUT_RELEASE)
	cmd /c rd standalone
	cmd /c rd $(OBJDIR_RELEASE)\\src

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

