# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: zwong <zwong@student.42kl.edu.my>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/12 11:48:29 by wxuerui           #+#    #+#              #
#    Updated: 2024/01/19 16:44:35 by zwong            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# variables
NAME		=	webserv
CXX			=	c++
CXXFLAGS	=	-Wall -Werror -Wextra -std=c++98 $(HEADER)
HEADER		=	-I$(INC_DIR) -I$(INC_DIR)/classes
ASAN		=	-fsanitize=address -g3
INC_DIR		=	include
SRC_DIR		=	src/
OBJ_DIR		=	obj/

ifeq ($(DB), 1)
	CXXFLAGS += -fsanitize=address -g3
endif


# sources
vpath %.cpp	$(SRC_DIR)
vpath %.h	$(INC_DIR)
vpath %.o	$(OBJ_DIR)

SRC_FILES	=	main \
				Parser \
				Path \
				ConnectionHandler \
				Request	\
				Response \
				utils

SRC			=	$(addprefix $(SRC_DIR), $(addsuffix .cpp, $(SRC_FILES)))
OBJ			=	$(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC_FILES)))


# colors
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


all:
	@make obj_dir_create
	@make $(NAME)
	@make ascii_art

ifeq ($(DB), 1)
	@echo "$(YELLOW)ASAN MODE$(RESET)"
endif


obj_dir_create:
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o:	$(SRC_DIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME):	$(OBJ)
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