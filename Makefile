include common.mk
BIN=bin

.PHONY: all clean
all:
	(cd src; make)
	(cd scripts; make)
clean:
	(cd src; make clean)
	(cd scripts; make clean)

# -- TEST --
cgi:
	(cd src; make)
	$(BIN)/eva.e 8081 config.ini cgi
so:
	(cd src; make)
	$(BIN)/eva.e 8080 config.ini so
