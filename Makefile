SERV_FILES = $(wildcard serveur/src/*.c)
CLI_FILES = $(wildcard client/src/*.c)

.PHONY : serv
serv:
	@gcc -o servjeu $(SERV_FILES)
	@./servjeu


.PHONY : client
client:
	@gcc -o clientjeu $(CLI_FILES)
	@./clientjeu
