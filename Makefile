NAME = webserv
COMPILER = c++
INCLUDE = -Iinclude -Isrc -Isrc/Exceptions -Isrc/Exceptions/HttpCodeException -Isrc/Class/Parser -Isrc/Class/Socket -Isrc/Class/Webserv -Isrc/Class/DirectoryHandle
FLAGS = -Wall -Wextra -Werror -std=c++98 -MMD $(INCLUDE)
OBJ = $(addprefix obj/, $(SRCS:.cpp=.o))
DEPENDENCIES = $(OBJ:.o=.d)

SRCS = 	main.cpp \
		src/Class/Socket/Socket.cpp \
		src/Class/Parser/Parser.cpp \
		src/Class/Webserv/Webserv.cpp \
		src/Class/DirectoryHandle/DirectoryHandle.cpp \
		src/file_management.cpp \
		src/Exceptions/HttpCodeException/HttpCodeException.cpp

all: $(NAME)

$(NAME): $(OBJ)
	@$(COMPILER) $(FLAGS) $(OBJ) -o $(NAME)
	@echo "✅ $(NAME) has been compiled!"

obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(COMPILER) $(FLAGS) -c $< -o $@

clean:
	@rm -rf obj
	@echo "🧹 Object files deleted"

fclean: clean
	@rm -f $(NAME)
	@echo "🧹 Executable deleted"

re: fclean all

# COLOR CODES
COLORS = \033[0;33m
RESET = \033[0m

help:
	@echo "$(COLORS)all:$(RESET) Compiles the program."
	@echo "$(COLORS)clean:$(RESET) Deletes object files."
	@echo "$(COLORS)fclean:$(RESET) Deletes object files and executable."
	@echo "$(COLORS)re:$(RESET) Recompiles the program from scratch."

-include $(DEPENDENCIES)

.PHONY: all clean fclean re