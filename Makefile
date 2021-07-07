######################
###    VARIABLES
######################

# COLORS
_RED		=	\e[1;31m
_GREEN		=	\e[1;32m
_YELLOW		=	\e[1;33m
_BLUE		=	\e[1;34m
_END		=	\e[1;0m

NAME =			webserv

# SOURCES
SRC_PATH =		sources/
SRC =			main.cpp Request.cpp Response.cpp Webserv.cpp Config.cpp Logger.cpp Utils.cpp Cluster.cpp Location.cpp Cgi.cpp Client.cpp

# HEADER
HEAD_PATH =		includes/

# All .o (object file)
OBJS_PATH =		objs/
OBJS =			$(SRC:%.cpp=$(OBJS_PATH)%.o)

# Compilation
CLANG =			clang++

# Compilation flags
CLANGFLAGS =	-Werror -Wall -Wextra -std=c++98

# LOGS
LOG_PATH =		log/log.txt

######################
###    RULES
######################

all:			$(OBJS)
				@$(CLANG) $(CLANGFLAGS) $(OBJS) -o $(NAME)
				@printf "$(_GREEN)All done!$(_END)\n"

fsanitize:		$(OBJS)
				@$(CLANG) $(CLANGFLAGS) -g3 -fsanitize=address $(OBJS) -o $(NAME)
				@printf "$(_GREEN)All done!$(_END)\n"

$(NAME):		all

# Getting .o from .cpp rule
$(OBJS_PATH)%.o: $(SRC_PATH)%.cpp
				@printf "$(_YELLOW)Compiling $< $(_END)⌛\n"
				@$(CLANG) $(CLANGFLAGS) -I $(HEAD_PATH) -c $< -o $@

$(OBJS):		| $(OBJS_PATH)

$(OBJS_PATH):
				@printf "$(_GREEN)Creating obj/ dir$(_END)\n"
				@mkdir $(OBJS_PATH)

fclean:
				@rm -rf $(OBJS_PATH)
				@rm -rf $(NAME)
				@rm -rf $(LOG_PATH)
				@printf "$(_RED)Deleting our webserv </3\n$(_END)"

clean:
				@rm -rf $(OBJS_PATH)
				@printf "$(_RED)Deleting all object files </3\n$(_END)"

re:				fclean all

.PHONY:			all clean fclean re