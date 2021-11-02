#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "../libft/libft.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

void	here_doc(int ac, char **av, char **envp);
void	error_message(char *message, int end[], char ***cmd);
void	free_path(char **paths);
char	*get_path(char *cmd, char **envp);
void	replace(int ac, t_list *cmds, int end[], int j, char **envp);
void	print_out(t_list *cmds);