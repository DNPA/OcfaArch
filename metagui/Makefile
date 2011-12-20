

all:
	make -C lucene  all
	make -C showtree all
	make -C db all

install:
	make -C lucene  install
	make -C showtree install	
	make -C db install

clean:
	make -C lucene clean
	cd showtree;make clean

dpkg:
	echo No make dpkg defined for metagui.
	exit 1
