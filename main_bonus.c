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

void	child1_process(int end[], char **av, char **ev)
{
	int		file1;
	char	**cmd;
	char	*path;

	file1 = open(av[1], O_RDONLY);
	if (file1 < 0)
		error_message("Open ", end, 0);
	if (dup2(file1, STDIN_FILENO) < 0)
		error_message("Dup2 ", end, 0);
	if (dup2(end[1], STDOUT_FILENO) < 0)
		error_message("Dup2 ", end, 0);
	close(end[0]);
	close(file1);
	cmd = ft_split(av[2], ' ');
	path = get_path(cmd[0], ev);
	if (!path)
	{
		perror(cmd[0]);
		perror(cmd[1]);
		perror(cmd[2]);
		error_message(cmd[0], end, &cmd);
	}
	if (execve(path, cmd, ev) == -1)
		error_message("execve ", end, &cmd);
}

void	child2_process(int end[], char **av, char **ev)
{
	int		file2;
	char	**cmd;
	char	*path;

	file2 = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (file2 < 0)
		error_message("Open ", end, 0);
	if (dup2(file2, STDOUT_FILENO) < 0)
		error_message("Dup2 ", end, 0);
	if (dup2(end[0], STDIN_FILENO) < 0)
		error_message("Dup2 ", end, 0);
	close(end[1]);
	close(file2);
	cmd = ft_split(av[3], ' ');
	path = get_path(cmd[0], ev);
	if (!path)
	{
		perror(cmd[0]);
		error_message("command not found", end, &cmd);
	}
	if (execve(path, cmd, ev) == -1)
		error_message("execve ", end, &cmd);
}
t_list *init_cmd(int ac, char **av)
{
	int		i;
	t_list *cmd;
	t_list *cmds;

	
	i = 0;
	cmds = NULL;
	while(++i <  ac)
	{
		cmd = ft_lstnew(av[i]);
		ft_lstadd_back(&cmds, cmd);
	}
	return (cmds);
}
void	replace(t_list *cmds, int end[], int j)
{
	//if not last cmd
    if (cmds->next)
	{
		printf("cmds1:%s, %d, %d\n", cmds->content, j, 2 * j + 1);
        if (dup2(end[2 * j + 1], STDOUT_FILENO) < 0)
			error_message("Dup2 1", 0, 0);
    }
	//if not first cmd
	if (j != 0)
	{
		printf("cmds2:%s, %d, %d\n", cmds->content, j, 2 * (j-1));
        if (dup2(end[2 * (j-1)], STDIN_FILENO) < 0)
			error_message("Dup2 2", 0, 0);
    }
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
	int		end[(ac - 2) * 2];

	pid_t	child1;
	t_list	*cmds;
	char	**cmd;
	char	*path;
	int		j;
	int		i;


	cmds = NULL;
	if (!ft_strncmp(av[1], "here_doc" , 5))
	{
			printf("here_doc\n");
			return ;
	}
	cmds = init_cmd(ac, av);
	print_out(cmds);
	i = -1;
	while(++i < ac - 3)
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
			replace(cmds, end, j);
			// //if not last cmd
            // if (cmds->next)
			// {
            //     if (dup2(end[2 * j + 1], STDOUT_FILENO < 0))
			// 		error_message("Dup2 ", end, 0);
            // }
			// //if not first cmd
			// if (j != 0)
			// {
            //     if (dup2(end[2 * j], STDIN_FILENO < 0))
			// 		error_message("Dup2 ", end, 0);
            // }
			i = -1;
			//printf("fine2:%d\n", j);
            while(++i < (ac - 2) * 2)	
				close(end[i]);
			cmd = ft_split((char *)cmds->content, ' ');
			//printf("cmd sper: %s\n", cmd[0]);
			path = get_path(cmd[0], envp);
			//printf("path: %s\n", path);
            if (!path)
			{
				perror(cmd[0]);
				error_message(cmd[0], 0, 0);
			}
			//printf("fine:%d\n", j);
			if (execve(path, cmd, envp) == -1)
			{
				error_message("execve ", end, &cmd);
			}
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
	pipex(ac, av, env);
	// leaks from not free?
	// check env?
	// fork double var..
	// comment not found
	return (0);
}