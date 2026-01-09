# ================= VARIABLES =================

NAME        = 	webserv
CXX         = 	c++
CXXFLAGS    = 	-Wall -Wextra -Werror -std=c++98 -MMD
RM          =	rm -rf


INCLUDE     =	-Iinclude \
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


CXXFLAGS    +=	$(INCLUDE)


SRCS        =	main.cpp \
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
				src/Class/ConfParser/handler_functions.cpp


# Objects & dependencies
OBJ_DIR     =	obj
OBJ         =	$(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
DEPS        =	$(OBJ:.o=.d)


# Colors
GREEN       =	\033[0;32m
YELLOW      =	\033[0;33m
RESET       =	\033[0m


# ================= RULES =================

all: $(NAME)


$(NAME): $(OBJ)
	@echo -e "$(YELLOW)🔗 Linking objects...$(RESET)"
	@$(CXX) $(OBJ) -o $(NAME)
	@echo -e "$(GREEN)✅ $(NAME) compiled successfully!$(RESET)"


$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	@rm -rf obj
	@echo "🧹 Object files deleted"


fclean: clean
	@rm -f $(NAME)
	@echo "🧹 Executable deleted"


re: fclean
	@$(MAKE) --no-print-directory all


t: re
	@clear
	@./$(NAME) ./conf/test.conf 2>/dev/null || true


help:
	@echo -e "$(GREEN)Available targets:$(RESET)"
	@echo -e "$(YELLOW)all$(RESET):    Compiles the program."
	@echo -e "$(YELLOW)clean$(RESET):  Deletes object files."
	@echo -e "$(YELLOW)fclean$(RESET): Deletes object files and executable."
	@echo -e "$(YELLOW)re$(RESET):     Recompiles the program from scratch."
	@echo -e "$(YELLOW)t$(RESET):      Recompiles and runs with test config."


# ================= DEPENDENCIES =================

-include $(DEPS)

.PHONY: all clean fclean re t help