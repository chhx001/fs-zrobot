all:tcp_client.o tcp_server.o zrtcp.o
	cc -o tcp_client tcp_client.o zrtcp.o
	cc -o tcp_server tcp_server.o zrtcp.o
clean:
	rm ./*.o tcp_server tcp_client
