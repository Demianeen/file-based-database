TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

DB_NAME = ./mynewdb.db

run: clean build
	./$(TARGET) -nf $(DB_NAME)
	./$(TARGET) -f $(DB_NAME) -a "Timmy H.,123 Sheshire Ln., 120"
	./$(TARGET) -f $(DB_NAME) -a "John S.,24 Bakerly Ln., 240"
	./$(TARGET) -lf $(DB_NAME)


build: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm $(DB_NAME)

$(TARGET): $(OBJ)
	@mkdir -p bin
	gcc -o $@ $?

obj/%.o : src/%.c
	@mkdir -p obj
	gcc -c $< -o $@ -Iinclude
