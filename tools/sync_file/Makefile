.PHONY: clean server client all

cc = g++
RM = rm
SEVER_EXE = server
CLIENT_EXE = client

server:
	$(cc) -o ./$(SEVER_EXE)/$(SEVER_EXE) ./$(SEVER_EXE)/main.cpp epoll_socket/*.cpp file_operation/*.cpp -I./

client:
	$(cc) -o ./$(CLIENT_EXE)/$(CLIENT_EXE) ./$(CLIENT_EXE)/main.cpp epoll_socket/*.cpp file_operation/*.cpp -I./

all: $(SEVER_EXE) $(CLIENT_EXE)

clean:
	$(RM) ./$(SEVER_EXE)/$(SEVER_EXE) ./$(CLIENT_EXE)/$(CLIENT_EXE)