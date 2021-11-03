#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "../libft/libft.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

void	here_doc(int ac, char **av, char **envp);
void	error_message_bo(char *message, int *end, int num, t_list *cmds);
void	free_path(char **paths);
char	*get_path(char *cmd, char **envp);
void	replace(int ac, t_list *cmds, int end[], int j, char **envp, t_list *cmds_re);
void	print_out(t_list *cmds);
void	normal_free(t_list *cmds);