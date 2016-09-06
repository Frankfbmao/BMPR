TOP	= ..
ALLSRC = $(wildcard *.cc)
ALLHD = $(wildcard *.h)
MAINSRC =
SOURCES = $(ALLSRC:$(MAINSRC)=)
OBJPATH = ./build/
OBJECTS = $(ALLSRC:%.cc=$(OBJPATH)%.o)
DEBUG_FLAGS = -g
LIBRARY =
LIBS    = $(LIBRARY) -lm
ifdef DBG
    TARGET  = btree
    CFLAGS  = $(DEBUG_FLAGS) $(INCFLAGS) $(DBG)
else
    TARGET  = btree
    CFLAGS  = $(INCFLAGS) -O2 
endif

CC      = /usr/bin/gcc
LL      = /usr/bin/g++
APP_INC  = .
INCFLAGS= -I$(APP_INC)
LFLAGS  = $(CFLAGS) $(LIBS)
MK_DIR  = if [ ! -d $(@D) ]; then mkdir $(@D); fi


debug: 
	$(MAKE) exe -f Makefile DBG="-g -DDEBUG"

exe: $(TARGET) 

$(TARGET) : clean $(OBJECTS) $(ALLHD)
	$(LL) $(LFLAGS) $(OBJECTS) -o $@

#$(LIBRARY) : $(OBJECTS)
#	$(MK_DIR)
#	@ar -rv $@ $?
#	@ctags $(SOURCES) $(APP_INC)/*.h
clean:
	rm -rf $(OBJPATH) $(TARGET) 

$(OBJPATH)%.o:%.cc
	@$(MK_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

