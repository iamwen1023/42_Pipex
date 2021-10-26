INCS		= includes
LIB			= libft
LIB.A		= libft.a
SRC			= main_new.c
OBJ			= $(SRC:.c=.o)
NAME		= pipex
FLAGS		= -Wall -Wextra -Werror -I${INCS}  -fsanitize=address -g3 -O0
	
all: $(NAME)

$(LIB.A):
		make -C $(LIB)
		@mv $(LIB)/$(LIB.A) .

$(NAME): $(LIB.A) $(OBJ)
		gcc -o $@ $(OBJ) $(LIB.A) $(FLAGS) 

%.o: %.c
		cc $(FLAGS) -c $< -o $(<:.c=.o)

clean:
		$(RM) $(OBJ) $(OBJ_B) $(LIB.A)
		make clean -C $(LIB)

fclean:		clean
		$(RM)  $(NAME) $(NAME_B) $(LIB.A)
		make fclean -C $(LIB)

re:		fclean all

.PHONY: 	all clean fclean re