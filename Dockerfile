FROM camilstaps/clean:nightly

RUN \
	dpkg --add-architecture i386 &&\
	apt-get update -qq &&\
	apt-get install -y -qq --no-install-recommends build-essential gcc-multilib time bc libncurses5-dev libncurses5-dev:i386 git

RUN	install_clean_nightly.sh base lib-platform lib-dynamics lib-graphcopy

RUN git clone https://gitlab.science.ru.nl/cstaps/clean-tools /tmp/clean-tools &&\
	make -C /tmp/clean-tools/clm -f Makefile.linux64 &&\
	mv /tmp/clean-tools/clm/clm /opt/clean/bin
