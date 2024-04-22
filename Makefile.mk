# Имя целевого исполняемого файла
TARGET = TaskManager

# Компилятор C
CC = gcc

# Флаги компилятора
CFLAGS = -Wall -Wextra -g

# Файлы исходного кода
SRCS = main.c

# Объектные файлы
OBJS = $(SRCS:.c=.o)

# Правило для сборки целевого исполняемого файла
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Правило для компиляции объектных файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Правило для очистки
clean:
	rm -f $(OBJS) $(TARGET)

# Правило для запуска программы
run: $(TARGET)
	./$(TARGET)
