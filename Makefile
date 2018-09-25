CXX = g++
CXXFLAGS = -O3
LDFLAGS = -lm

OBJ = paste.o board.o sha256.o

all: genpaste pastein newbored pasteout recompress heatmap

genpaste: $(OBJ) genpaste.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

pastein: $(OBJ) pastein.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

pasteout: $(OBJ) pasteout.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

newbored: $(OBJ) newbored.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

recompress: $(OBJ) recompress.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

heatmap: $(OBJ) heatmap.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

%.o: %.c
	$(CXX) -o $@ $(CXXFLAGS) -c $^

%: %.gz
	gzip -dc $^ > $@

clean:
	rm -f $(OBJ) genpaste.o pasteout.o newbored.o pastein.o recompress.o heatmap.o
	rm -f genpaste pasteout newbored pastein recompress heatmap
