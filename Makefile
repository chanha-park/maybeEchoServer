# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: chanhpar <chanhpar@student.42seoul.kr>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/01/24 18:21:13 by chanhpar          #+#    #+#              #
#    Updated: 2023/01/24 18:44:26 by chanhpar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

.DEFAULT_GOAL	:= all

CXX				:= c++
RM				:= rm -f

CXXFLAGS		:= -Wall -Wextra -Werror -std=c++98 -pedantic

SRCS_SERVER		:= server.cpp
SRCS_CLIENT		:= client.cpp

OBJS_SERVER		:= $(SRCS_SERVER:.cpp=.o)
OBJS_CLIENT		:= $(SRCS_CLIENT:.cpp=.o)

DEPS_SERVER		:= $(SRCS_SERVER:.cpp=.d)
DEPS_CLIENT		:= $(SRCS_CLIENT:.cpp=.d)
-include $(DEPS_SERVER) $(DEPS_CLIENT)

INCS			:=

INCS_DIR		:= ./include/

CPPFLAGS		:= -I$(INCS_DIR)

SHELL			:= /bin/bash

NAME_SERVER		:= server.out
NAME_CLIENT		:= client.out
NAME			:= $(NAME_SERVER) $(NAME_CLIENT)

.PHONY: all
all: $(SRCS_SERVER) $(SRCS_CLIENT) $(addprefix $(INCS_DIR), $(INCS))
	@$(MAKE) server
	@$(MAKE) client

.PHONY: server
server: $(NAME_SERVER)

$(NAME_SERVER): $(OBJS_SERVER)
	$(CXX) $(CXXFLAGS) $^ -o $(NAME_SERVER)

.PHONY: client
client: $(NAME_CLIENT)

$(NAME_CLIENT): $(OBJS_CLIENT)
	$(CXX) $(CXXFLAGS) $^ -o $(NAME_CLIENT)

.PHONY: clean
clean:
	$(RM) $(OBJS_SERVER) $(OBJS_CLIENT) $(DEPS_SERVER) $(DEPS_CLIENT)
	@echo "clean done!"

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)
	@echo "fclean done!"

.PHONY: re
re: fclean
	$(MAKE) all
