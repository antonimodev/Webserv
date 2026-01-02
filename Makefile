NAME = webserv
COMPILER = c++

INCLUDE = 	-Iinclude \
			-Isrc \
			-Isrc/Exceptions \
			-Isrc/Exceptions/HttpCodeException \
			-Isrc/Exceptions/ParseException \
			-Isrc/Class/Parser \
			-Isrc/Class/Pipe \
			-Isrc/Class/Socket \
			-Isrc/Class/Webserv \
			-Isrc/Class/DirectoryHandler \
			-Isrc/Class/ConfParser \
			-Isrc/Class/CgiHandler \
			-Isrc/Class/ExecveBuilder

FLAGS = -Wall -Wextra -Werror -std=c++98 -MMD $(INCLUDE)
OBJ = $(addprefix obj/, $(SRCS:.cpp=.o))
DEPENDENCIES = $(OBJ:.o=.d)

SRCS = 	main.cpp \
		src/Class/Socket/Socket.cpp \
		src/Class/Parser/Parser.cpp \
		src/Class/Webserv/Webserv.cpp \
		src/Class/DirectoryHandler/DirectoryHandler.cpp \
		src/Class/CgiHandler/CgiHandler.cpp \
		src/Class/ExecveBuilder/ExecveBuilder.cpp \
		src/Class/Pipe/Pipe.cpp \
		src/file_management.cpp \
		src/Exceptions/HttpCodeException/HttpCodeException.cpp \
		src/Class/ConfParser/ConfParser.cpp \
		src/Class/ConfParser/handler_functions.cpp \

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

t: re
	@clear
	@./webserv ./conf/test.conf || 2>/dev/null

t2: re
	@clear
	@valgrind ./webserv ./conf/test.conf || 2>/dev/null

.PHONY: all clean fclean re