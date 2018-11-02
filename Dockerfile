FROM camilstaps/clean:nightly

RUN \
	dpkg --add-architecture i386 &&\
	apt-get update -qq &&\
	apt-get install -y -qq --no-install-recommends\
		git\
		build-essential\
		clang\
		gcc-multilib\
		gcc-mingw-w64-x86-64\
		libncurses5-dev\
		libncurses5-dev:i386\
		time\
		bc

RUN	install_clean_nightly.sh base lib-platform lib-dynamics lib-graphcopy

RUN git clone https://gitlab.science.ru.nl/cstaps/clean-tools /tmp/clean-tools &&\
	make -C /tmp/clean-tools/clm -f Makefile.linux64 &&\
	mv /tmp/clean-tools/clm/clm /opt/clean/bin
