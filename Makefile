CC = gcc

FLEX_FILE = flex_rules.l
LEX_YY_C_FILE = lex.yy.c
LEXER_EXE_NAME = lexer

LEXER_TEST_FILE = input.txt

.PHONY: build
build: clean flex
	$(CC) ./$(LEX_YY_C_FILE) -o $(LEXER_EXE_NAME)

.PHONY: flex
flex: $(FLEX_FILE)
	flex ./$(FLEX_FILE)
	mv ./$(LEX_YY_C_FILE) ./

.PHONY: clean
clean:
	rm -rf $(wildcard *.exe) $(wildcard *.o) ./$(LEX_YY_C_FILE)

.PHONY: test
test:
	cat ./input_data/$(LEXER_TEST_FILE) | ./$(LEXER_EXE_NAME)
