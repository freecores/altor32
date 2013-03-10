###############################################################################
## Makefile
###############################################################################

# Target
TARGET       = rom_gen

# Options
CFLAGS	    = 
LDFLAGS     = 

# Source Files
OBJ = rom_gen.o

###############################################################################
# Rules
###############################################################################
all: $(TARGET)
	
clean:
	-rm *.o $(TARGET)

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

%.o : %.cpp
	g++ -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJ) makefile
	g++ $(LDFLAGS) $(LIBS) $(OBJ) -o $@

