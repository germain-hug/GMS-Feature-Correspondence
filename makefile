CC := gcc -g

CFLAGS= -std=c++14 -c

# Folders
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin
INCLUDEDIR := include

# Targets
EXECUTABLE := GMS
TARGET := $(TARGETDIR)/$(EXECUTABLE)

# Final Paths
INSTALLBINDIR := /usr/local/bin

# Code Lists
SRCEXT := cpp
CUDAEXT := cu
SOURCES_CPP := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT)) 
OBJECTS_CPP := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES_CPP:.$(SRCEXT)=.o))

SOURCES_CUDA := $(shell find $(SRCDIR) -type f -name *.$(CUDAEXT))
OBJECTS_CUDA := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES_CUDA:.$(CUDAEXT)=.o))

OBJECTS := $(OBJECTS_CPP) $(OBJECTS_CUDA)

INC :=	-I/usr/local/cuda-8.0/targets/x86_64-linux/include/ \
		-I ~/eigen/

LIB :=	-L/usr/local/cuda/lib64/ \
		-L/usr/local/lib/ \
		-lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lopencv_video \
        -lopencv_features2d \
        -lopencv_calib3d \
        -lopencv_gpu \
        -lopencv_flann \
        -lglog \
		-lcudart \
        -lcublas \
		-lcudart \
		-lcurand \
		-lstdc++ \
		-ldl \
		-lm

$(TARGET): $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@echo "Linking..."
	@echo "  Linking $(TARGET)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling $<..."; $(CC) $(CFLAGS) $(INC) -I$(INCLUDEDIR) -c -o $@ $<

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(CUDAEXT)
	@echo "Compiling $<..."; $(CC) $(CFLAGS) $(INC) -I$(INCLUDEDIR) -c -o $@ $<


clean:
	@echo "Cleaning build/main.o..."; $(RM) build/main.o 
	@echo "Cleaning $(TARGET)..."; $(RM) $(TARGET)

.PHONY: clean