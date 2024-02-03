# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: wxuerui <wangxuerui2003@gmail.com>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/12 11:48:29 by wxuerui           #+#    #+#              #
#    Updated: 2024/02/03 14:17:28 by wxuerui          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME		=	webserv
CXX			=	c++
CXXFLAGS	=	-Wall -Werror -Wextra -std=c++98 $(INCLUDES)
ASAN		=	-fsanitize=address -g3

ifeq ($(DB), 1)
	CXXFLAGS += -fsanitize=address -g3
endif

############################
# FOLDERS
############################

SRC_DIR		=	src/
OBJ_DIR		=	obj/

CGI_DIR		=	cgi/
EVENT_DIR	=	event/
HTTP_DIR	=	http/
CONFIG_DIR	=	config/
UTILS_DIR	=	utils/
PATH_DIR	=	path/


############################
# INCLUDES
############################

INC_DIR		=	include/

INCLUDES	=	-I$(INC_DIR) \
				-I$(SRC_DIR)$(CGI_DIR) \
				-I$(SRC_DIR)$(EVENT_DIR) \
				-I$(SRC_DIR)$(HTTP_DIR) \
				-I$(SRC_DIR)$(CONFIG_DIR) \
				-I$(SRC_DIR)$(PATH_DIR) \
				-I$(SRC_DIR)$(UTILS_DIR)


############################
# FILES
############################

CGI_FILES	=	CgiHandler
EVENT_FILES	=	AConnectionHandler Select
HTTP_FILES	=	Request Response
CONFIG_FILES=	Parser Config
PATH_FILES	=	Path
UTILS_FILES	=	utils
MAIN		=	main


############################
# SRC
############################

SRC			=	$(addsuffix .cpp, $(MAIN)) \
				$(addprefix $(CGI_DIR), $(addsuffix .cpp, $(CGI_FILES))) \
				$(addprefix $(EVENT_DIR), $(addsuffix .cpp, $(EVENT_FILES))) \
				$(addprefix $(HTTP_DIR), $(addsuffix .cpp, $(HTTP_FILES))) \
				$(addprefix $(CONFIG_DIR), $(addsuffix .cpp, $(CONFIG_FILES))) \
				$(addprefix $(PATH_DIR), $(addsuffix .cpp, $(PATH_FILES))) \
				$(addprefix $(UTILS_DIR), $(addsuffix .cpp, $(UTILS_FILES)))

OBJ_SUBDIR	=	$(addprefix $(OBJ_DIR), $(CGI_DIR)) \
				$(addprefix $(OBJ_DIR), $(EVENT_DIR)) \
				$(addprefix $(OBJ_DIR), $(HTTP_DIR)) \
				$(addprefix $(OBJ_DIR), $(CONFIG_DIR)) \
				$(addprefix $(OBJ_DIR), $(PATH_DIR)) \
				$(addprefix $(OBJ_DIR), $(UTILS_DIR))
			
OBJ = $(addprefix $(OBJ_DIR), $(SRC:cpp=o))


############################
# COLORS
############################

DEF_COLOR	=	\033[0;39m
GRAY		=	\033[0;90m
RED			=	\033[0;91m
GREEN		=	\033[0;92m
YELLOW		=	\033[0;93m
BLUE		=	\033[0;94m
MAGENTA		=	\033[0;95m
CYAN		=	\033[0;96m
WHITE		=	\033[0;97m
RESET		=	\033[0m


##############################################


all:	$(NAME)
	@make ascii_art

ifeq ($(DB), 1)
	@echo "$(YELLOW)ASAN MODE$(RESET)"
endif


$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_SUBDIR)

$(OBJ_DIR)%.o:	$(SRC_DIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJ_DIR) $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean:	clean
	rm -f $(NAME)

re:	fclean all

ascii_art:
	@clear
	@echo ''
	@echo ' __      __      ___.                              '
	@echo '/  \    /  \ ____\_ |__   ______ ______________  __'
	@echo '\   \/\/   // __ \| __ \ /  ___// __ \_  __ \  \/ /'
	@echo ' \        /\  ___/| \_\ \\___ \\  ___/|  | \/\   / '
	@echo '  \__/\  /  \___  >___  /____  >\___  >__|    \_/  '
	@echo '       \/       \/    \/     \/     \/             '
	@echo ''

.PHONY: all clean fclean re obj_dir_create