.PHONY: all download clean

all: download Python-3.12.4 gcc-15.1.0 glibc-2.41 go1.24.4

Python-3.12.4 gcc-15.1.0 glibc-2.41 go1.24.4:
	cd libs/$@ && cloc --by-file --sql=../$@.sql .

download: libs/glibc-2.41 libs/gcc-15.1.0 libs/go1.24.4 libs/Python-3.12.4

libs:
	mkdir -p libs

libs/glibc-2.41: libs
	cd libs && wget https://ftp.gnu.org/gnu/glibc/glibc-2.41.tar.xz
	cd libs && aunpack glibc-2.41.tar.xz
	cd libs && rm -f glibc-2.41.tar.xz

libs/gcc-15.1.0: libs
	cd libs && wget https://mirror.koddos.net/gcc/releases/gcc-15.1.0/gcc-15.1.0.tar.xz
	cd libs && aunpack gcc-15.1.0.tar.xz
	cd libs && rm -f gcc-15.1.0.tar.xz

libs/go1.24.4: libs
	cd libs && wget https://go.dev/dl/go1.24.4.linux-amd64.tar.gz
	cd libs && aunpack go1.24.4.linux-amd64.tar.gz
	cd libs && rm -f go1.24.4.linux-amd64.tar.gz
	cd libs && mv go go1.24.4

libs/Python-3.12.4: libs
	cd libs && wget https://www.python.org/ftp/python/3.12.4/Python-3.12.4.tgz
	cd libs && aunpack Python-3.12.4.tgz
	cd libs && rm -f Python-3.12.4.tgz

clean:
	rm -r -f libs/
