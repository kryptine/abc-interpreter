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
	curl "http://ftp.mozilla.org/pub/firefox/nightly/latest-mozilla-central/jsshell-linux-x86_64.zip" -o jsshell.zip &&\
	unzip "jsshell.zip" &&\
	cp js /usr/bin &&\
	cd - &&\
	rm -r /tmp/jsshell

RUN	install_clean_nightly.sh base lib-argenv lib-directory lib-dynamics lib-graphcopy lib-platform lib-stdlib
