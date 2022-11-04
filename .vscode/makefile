#source file search paths
VPATH = ./src 

	# compiler include search paths
INCS =  \
	-I../ravencode/pub/src \
	-I../raven-set \
	-I../boost/boost1_78 

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \
	-L../boost/boost1_78/lib \

	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj
RDIR = ../ravencode/pub/src

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_starter = \
	$(ODIR)/main.o $(ODIR)/soundsim.o \
	$(ODIR)/cGUI.o

OBJ_test = \
	$(ODIR)/test.o $(ODIR)/soundsim.o

### COMPILE

$(ODIR)/%.o: %.cpp soundsim.h cGUI.h
	g++ $< \
	-c -o $@  $(INCS) $(CCflags)

### LINK

starter: $(OBJ_starter)
	g++ $(OPT) \
	-o bin/soundsim.exe $^  \
	$(static_link_flags) $(LIBS)

test: $(OBJ_test)
	g++ -o bin/test.exe \
	$(OPT)  $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

