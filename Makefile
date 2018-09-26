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

.PHONY: clean
clean:
	rm -f $(OBJ) genpaste.o pasteout.o newbored.o pastein.o recompress.o heatmap.o
	rm -f genpaste pasteout newbored pastein recompress heatmap

# for Ubuntu 16.04 LTS Google Cloud instance
.PHONY: setup
setup:
	apt-get -y update
	apt-get -y install apache2
	apt-get -y install make
	apt-get -y install g++
	apt-get -y install imagemagick

.PHONY: boreds
boreds:
	./newbored main main 1024 1024 86400
	./newbored news news 1024 2048 86400
	./newbored test test 512 512 3600
