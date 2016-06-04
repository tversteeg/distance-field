GEN_NAME=dfield_generator
REN_NAME=dfield_renderer

RM=rm -rf
CFLAGS=-g -Wall -pedantic -O3 -DUSE_GLEW -fopenmp
LDLIBS=-fopenmp -lccore -lGL -lGLU -lGLEW -lm -lX11 -lXrandr -lpthread

GSRCS=src/dfield_generator.c src/lodepng.c
GOBJS=$(subst .c,.o,$(GSRCS))

RSRCS=src/dfield_renderer.c
ROBJS=$(subst .c,.o,$(RSRCS))

all: $(GEN_NAME) $(REN_NAME)

$(GEN_NAME): $(GOBJS)
	$(CC) $(LDFLAGS) -o $(GEN_NAME) $(GOBJS) $(LDLIBS)

$(REN_NAME): $(ROBJS)
	$(CC) $(LDFLAGS) -o $(REN_NAME) $(ROBJS) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(GOBJS) $(GEN_NAME) $(ROBJS) $(REN_NAME)
