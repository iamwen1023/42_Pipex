INCS		= includes
LIB			= libft
LIB.A		= libft.a
SRC			= main.c
SRC_B		= main_bonus.c here_doc.c
OBJ			= $(SRC:.c=.o)
OBJ_B		= $(SRC_B:.c=.o)
NAME		= pipex
NAME_B		= pipex_bonus
FLAGS		= -Wall -Wextra -Werror -I${INCS}  -fsanitize=address -g3 -O0
	
all: $(NAME)

bonus : $(NAME_B)
$(LIB.A):
		make -C $(LIB)
		@mv $(LIB)/$(LIB.A) .

$(NAME): $(LIB.A) $(OBJ)
		gcc -o $@ $(OBJ) $(LIB.A) $(FLAGS)

$(NAME_B): $(LIB.A) $(OBJ_B)
		gcc -o $@ $(OBJ_B) $(LIB.A) $(FLAGS)
		
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