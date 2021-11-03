#include "includes/pipex.h"

void	free_path(char **paths)
{
	int	i;

	i = 0;
	while (paths[i])
	{
		free(paths[i]);
		i++;
	}
}

void	error_message_bo(char *message, int *end, int num, t_list *cmds)
{
	int		i;
	t_list	*current;

	i = -1;
	perror(message);
	while(++i < num)
		close(end[i]);
	free(end);
	while (cmds)
	{
		current = cmds;
		cmds = cmds->next;
		free(current);
		current = NULL;
	}
	exit(1);
}

char	*get_path(char *cmd, char **envp)
{
	char	**paths;
	char	*path;
	int		i;
	char	*part_path;

	i = 0;
	while (ft_strnstr(envp[i], "PATH", 4) == 0)
		i++;
	paths = ft_split(envp[i] + 5, ':');
	i = 0;
	while (paths[i])
	{
		part_path = ft_strjoin(paths[i], "/");
		path = ft_strjoin(part_path, cmd);
		free(part_path);
		if (access(path, F_OK) == 0)
		{
			free_path(paths);
			return (path);
		}
		i++;
	}
	return (0);
}

t_list *init_cmd(int ac, char **av)
{
	int		i;
	t_list *cmd;
	t_list *cmds;

	
	i = 1;
	cmds = NULL;
	while(++i <  ac - 1)
	{
		cmd = ft_lstnew(av[i]);
		ft_lstadd_back(&cmds, cmd);
	}
	return (cmds);
}

void	replace(int ac, t_list *cmds, int end[], int j, char **envp)
{
	int i;
	char	**cmd;
	char	*path;
	//if not last cmds
    if (cmds->next)
	{
        if (dup2(end[2 * j + 1], STDOUT_FILENO) < 0)
			error_message_bo("Dup2 1", end, (ac - 2) * 2, cmds);
    }
	//if not first cmd
	if (j != 0)
	{
        if (dup2(end[2 * (j-1)], STDIN_FILENO) < 0)
			error_message_bo("Dup2 2", end, (ac - 2) * 2, cmds);
    }
	i = -1;
    while(++i < (ac - 2) * 2)	
		close(end[i]);
	cmd = ft_split((char *)cmds->content, ' ');
	path = get_path(cmd[0], envp);
    if (!path)
		error_message_bo(cmd[0], end, (ac - 2) * 2, cmds);
	if (execve(path, cmd, envp) == -1)
		error_message_bo("execve ", end, (ac - 2) * 2, cmds);
}

void print_out(t_list *cmds)
{
	while(cmds->next)
	{
		printf("cmd:%s\n", (char *)cmds->content);
		cmds= cmds->next;
	}
	printf("cmd:%s\n", (char *)cmds->content);

}
void	pipex(int ac, char **av, char **envp)
{
	//int		end[(ac - 2) * 2];
	int		*end;
	pid_t	child1;
	t_list	*cmds;
	int		file1;
	int		file2;
	int		j;
	int		i;

	cmds = NULL;
	cmds = init_cmd(ac, av);
	end = malloc((ac - 2) * 2 * sizeof(int));
	if (!end)
		error_message_bo("malloc", 0, 0, cmds);
	i = -1;
	while(++i < ac - 2)
	{
		if (pipe(end) < 0)
			error_message_bo("pipe", 0, 0, cmds);
		end = end + (2 * i);
	}
	j = 0;
	while(cmds != NULL )
	{
		child1 = fork();
		if (child1 < 0)
			error_message_bo("Fork", end, (ac - 2) * 2, cmds);
        if (child1 == 0)
		{
			if (j == 0)
			{
				file1 = open(av[1], O_RDONLY);
				if (file1 < 0)
					error_message_bo("Open ", end, (ac - 2) * 2, cmds);
				if (dup2(file1, STDIN_FILENO) < 0)
					error_message_bo("Dup2 ", end, (ac - 2) * 2, cmds);
				close(file1);
			}
			else if (j == ac - 4)
			{
				file2 = open(av[ac - 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
				if (file2 < 0)
					error_message_bo("Open ", end, (ac - 2) * 2, cmds);
				if (dup2(file2, STDOUT_FILENO) < 0)
					error_message_bo("Dup2 ", end, (ac - 2) * 2, cmds);
				close(file2);
			}
			replace(ac, cmds, end, j, envp);
        }
		cmds = cmds->next;
		j++;
    }
	//parent process
	i = -1;
	while(++i < (ac - 2) * 2)	
		close(end[i]);
	free(end);
	while (errno != ECHILD)
		wait(NULL);
}


int	main(int ac, char **av, char **env)
{
	if (ac < 5)
	{
		perror("arg < 5");
		return (1);
	}
	if (!ft_strncmp(av[1], "here_doc" , 9))
	{
			here_doc(ac, av, env);
			return (0);
	}
	pipex(ac, av, env);
	// leaks from not free?
	// check env?
	// fork double var..
	// comment not found
	return (0);
}