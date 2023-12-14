ser:
	${make}g++ -o serv/server.out serv/main.cpp
	${make}./serv/server.out

cli:
	${make}g++ -o client/client.out client/main.cpp
	${make}./client/client.out