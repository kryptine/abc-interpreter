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

RUN	install_clean_nightly.sh base lib-argenv lib-directory lib-dynamics lib-graphcopy lib-platform lib-stdlib
COPY etc/ByteCode.env ByteCode.env
RUN tail -n +3 ByteCode.env >> /opt/clean/etc/IDEEnvs

RUN git clone https://gitlab.science.ru.nl/cstaps/clean-tools /tmp/clean-tools &&\
	make -C /tmp/clean-tools/clm -f Makefile.linux64 &&\
	mv /tmp/clean-tools/clm/clm /opt/clean/bin &&\
	cd /tmp && rm -r clean-tools

RUN git clone https://gitlab.science.ru.nl/cstaps/clean-ide /tmp/clean-ide &&\
	cd /tmp/clean-ide/cpm &&\
	clm -h 256m -nr -nt\
		-I Posix -I ../BatchBuild -I ../Pm -I ../Unix -I ../Util -I ../Interfaces/LinkerInterface\
		-IL ArgEnv -IL Directory -IL StdLib\
		Cpm -o cpm &&\
	mv cpm /opt/clean/bin &&\
	cd /tmp && rm -r clean-ide
