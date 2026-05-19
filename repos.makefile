.PHONY: all configure install test clean vars

all:
	cd repos/stz-c   && $(MAKE) all
	cd repos/stz-cpp && $(MAKE) all

configure:
	cd repos/stz-c   && $(MAKE) configure
	cd repos/stz-cpp && $(MAKE) configure

install:
	cd repos/stz-c   && $(MAKE) install
	cd repos/stz-cpp && $(MAKE) install

test:
	cd repos/stz-c   && $(MAKE) test
	cd repos/stz-cpp && $(MAKE) test

clean:
	cd repos/stz-c   && $(MAKE) clean
	cd repos/stz-cpp && $(MAKE) clean

vars:
	cd repos/stz-c   && $(MAKE) vars
	cd repos/stz-cpp && $(MAKE) vars
