CPPFLAGS=-Ilibarchive/libarchive -DHAVE_ZLIB_H
LDFLAGS=-lz libarchive/.libs/libarchive.a

all: zip2tar tar2zip

zip2tar: zip2tar.o

tar2zip: tar2zip.o

clean:
	rm -f *.o zip2tar tar2zip