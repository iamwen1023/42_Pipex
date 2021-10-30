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

void	error_message(char *message, int end[], char ***cmd)
{
	int	i;

	i = 0;
	perror(message);
	if (end)
	{
		close(end[0]);
		close(end[1]);
	}
	if (*cmd)
	{
		while ((*cmd)[i])
		{
			free((*cmd)[i]);
			i++;
		}
		free(*cmd);
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
	//if not last cmd
    if (cmds->next)
	{
		fprintf(stderr,"cmds1:%s, %d, %d\n", (char *)cmds->content, j, 2 * j + 1);
        if (dup2(end[2 * j + 1], STDOUT_FILENO) < 0)
			error_message("Dup2 1", 0, 0);
    }
	//if not first cmd
	if (j != 0)
	{
		fprintf(stderr,"cmds2:%s, %d, %d\n", (char *)cmds->content, j, 2 * (j-1));
        if (dup2(end[2 * (j-1)], STDIN_FILENO) < 0)
			error_message("Dup2 2", 0, 0);
    }
	i = -1;
    while(++i < (ac - 2) * 2)	
		close(end[i]);
	cmd = ft_split((char *)cmds->content, ' ');
	path = get_path(cmd[0], envp);
    if (!path)
	{
		perror(cmd[0]);
		fprintf(stderr,"HERE???\n");
		exit(1);
		//error_message(cmd[0], 0, 0);
	}
	if (execve(path, cmd, envp) == -1)
		error_message("execve ", end, &cmd);
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

int	parse_map(int fd, char **g_map)
{
	char	buffer[2];
	int		ret;

	ret = 1;
	while (ret > 0)
	{
		ret = read(fd, buffer, 1);
		buffer[1] = '\0';
		if (ret < 0 )
			break ;
		else if (ret == 0)
			return (0);
		if ((*g_map) == 0)
		{
			(*g_map) = malloc(sizeof(char) * 2);
			if (!(*g_map))
				return (-1);
			(*g_map)[0] = buffer[0];
			(*g_map)[1] = '\0';
		}
		else
			(*g_map) = ft_strjoin_map(&(*g_map), buffer);
	}
	return (1);
}

void	here_doc(int ac, char **av, char **envp)
{
	char	buffer[2];
	int		ret;
	char	*content;

	ret = 1;
	while (ret > 0)
	{
		ret = read(STDIN_FILENO, buffer, 1);
		buffer[1] = '\0';
		if (ret < 0 )
			break ; // protect??
		else if (ret == 0)
			return (0);
		if ((*g_map) == 0)
		{
			(*g_map) = malloc(sizeof(char) * 2);
			if (!(*g_map))
				return (-1);
			(*g_map)[0] = buffer[0];
			(*g_map)[1] = '\0';
		}
		else
			(*g_map) = ft_strjoin_map(&(*g_map), buffer);
	}


}

void	pipex(int ac, char **av, char **envp)
{
	int		end[(ac - 2) * 2];
	pid_t	child1;
	t_list	*cmds;
	int		file1;
	int		file2;
	int		j;
	int		i;

	cmds = NULL;
	cmds = init_cmd(ac, av);
	print_out(cmds);
	i = -1;
	while(++i < ac - 2)
	{
		if (pipe(&end[2 * i]) < 0)
		{
			perror("pipe");
			exit(1);
		}
	}
	j = 0;
	while(cmds != NULL )
	{
		child1 = fork();
		if (child1 < 0)
			error_message("Fork ", end, 0);
        if (child1 == 0)
		{
			if (j == 0)
			{
				file1 = open(av[1], O_RDONLY);
				if (file1 < 0)
					error_message("Open ", 0, 0);
				if (dup2(file1, STDIN_FILENO) < 0)
					error_message("Dup2 ", 0, 0);
				close(file1);
			}
			else if (j == ac - 4)
			{
				file2 = open(av[ac - 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
				if (file2 < 0)
					error_message("Open ", end, 0);
				if (dup2(file2, STDOUT_FILENO) < 0)
					error_message("Dup2 ", end, 0);
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
	while (errno != ECHILD)
		wait(NULL);
}

int	main(int ac, char **av, char **env)
{
	// if (ac < 5)
	// {
	// 	perror("arg < 5");
	// 	return (1);
	// }
	if (!ft_strncmp(av[1], "here_doc" , 5))
	{
			printf("here_doc\n");
			return ;
	}
	pipex(ac, av, env);
	// leaks from not free?
	// check env?
	// fork double var..
	// comment not found
	return (0);
}