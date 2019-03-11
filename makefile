cc=g++
LDFLAGS=-std=c++11 -lpthread 
INCLUDE=-I/jsoncpp/include /lib/libjson.a
#找库的路径
client=chatclient
server=chatserver

.PHONY:all
all:$(server) $(client)
$(server):chatserver.cc
	$(cc) -o  $@ $^  $(INCLUDE) $(LDFLAGS)

$(client):chatclient.cc
	$(cc) -o  $@ $^  $(INCLUDE) -lncurses  $(LDFLAGS)
.PHONY:clean
clean:
	rm -f $(server) $(client)
