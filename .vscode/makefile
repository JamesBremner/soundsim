#source file search paths
VPATH = ./src 

	# compiler include search paths
INCS =  \
	-I../ravencode/pub/src \
	-I../raven-set \

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \

	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj
RDIR = ../ravencode/pub/src

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_starter = \
	$(ODIR)/main.o $(ODIR)/soundsim.o

### COMPILE

$(ODIR)/%.o: %.cpp soundsim.h cGUI.h
	g++  $(CCflags) \
	-c -o $@ $< $(INCS)

### LINK

starter: $(OBJ_starter)
	g++ $(OPT) \
	-o bin/soundsim.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

