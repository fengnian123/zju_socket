srv:
	${make}g++ -o a.out serv/main.cpp
	${make}./a.out

cli:
	${make}g++ -o a.out client/main.cpp
	${make}./a.out