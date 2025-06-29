.PHONY: download libs

all: Python-3.12.4 gcc-15.1.0 glibc-2.41 go1.24.4

download: download-glibc download-gcc download-go download-python

download-glibc:
	mkdir -p libs
	cd libs && wget https://ftp.gnu.org/gnu/glibc/glibc-2.41.tar.xz
	cd libs && aunpack glibc-2.41.tar.xz
	cd libs && rm -f glibc-2.41.tar.xz

download-gcc:
	mkdir -p libs
	cd libs && wget https://mirror.koddos.net/gcc/releases/gcc-15.1.0/gcc-15.1.0.tar.xz
	cd libs && aunpack gcc-15.1.0.tar.xz
	cd libs && rm -f gcc-15.1.0.tar.xz

download-go:
	mkdir -p libs
	cd libs && wget https://go.dev/dl/go1.24.4.linux-amd64.tar.gz
	cd libs && aunpack go1.24.4.linux-amd64.tar.gz
	cd libs && rm -f go1.24.4.linux-amd64.tar.gz

download-python:
	mkdir -p libs
	cd libs && wget https://www.python.org/ftp/python/3.12.4/Python-3.12.4.tgz
	cd libs && aunpack Python-3.12.4.tgz
	cd libs && rm -f Python-3.12.4.tgz

Python-3.12.4 gcc-15.1.0 glibc-2.41 go1.24.4:
	cd libs/$@ && cloc --by-file --sql=../$@.sql .
