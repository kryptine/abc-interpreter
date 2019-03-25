FROM camilstaps/clean:nightly

RUN \
	dpkg --add-architecture i386 &&\
	apt-get update -qq &&\
	apt-get install -y -qq --no-install-recommends\
		ca-certificates\
		git\
		build-essential\
		clang\
		gcc-multilib\
		gcc-mingw-w64-x86-64\
		libncurses5-dev\
		libncurses5-dev:i386\
		time\
		bc

# JavaScript shell, for WebAssembly tests
RUN apt-get install -y -qq --no-install-recommends curl unzip libnspr4 &&\
	mkdir /tmp/jsshell &&\
	cd /tmp/jsshell &&\
	curl "http://ftp.mozilla.org/pub/firefox/nightly/latest-oak/jsshell-linux-x86_64.zip" -o jsshell.zip &&\
	unzip "jsshell.zip" &&\
	cp js /usr/bin &&\
	cd - &&\
	rm -r /tmp/jsshell

# Wabt, to convert WebAssembly text to binary
# TODO: when SpiderMonkey's function for this is updated this dependency can be removed
RUN apt-get install -y -qq --no-install-recommends cmake python file &&\
	git clone --recurse-submodules https://github.com/webassembly/wabt /tmp/wabt &&\
	mkdir /tmp/wabt/build &&\
	cd /tmp/wabt/build &&\
	cmake .. &&\
	make wat2wasm &&\
	cp wat2wasm /usr/bin &&\
	rm -rf /tmp/wabt

RUN	install_clean_nightly.sh base lib-argenv lib-directory lib-dynamics lib-graphcopy lib-platform lib-stdlib

RUN git clone https://gitlab.science.ru.nl/cstaps/clean-tools /tmp/clean-tools &&\
	make -C /tmp/clean-tools/clm -f Makefile.linux64 &&\
	mv /tmp/clean-tools/clm/clm /opt/clean/bin &&\
	cd /tmp && rm -r clean-tools
