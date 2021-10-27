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

int		exce_process(int end[], int index, char *allcmd, char **ev)
{
	//int		end[2];
	char	*path;
	char	**cmd;

	printf("exce process:%s\n", allcmd);
	// if (pipe(end) < 0)
	// {
	// 	(perror("Pipe: "));
	// 	return (1);
	// }
	if (index == 2)
	{
		printf("index 2\n");
		if (dup2(end[1], STDOUT_FILENO) < 0)
		{
			perror("Dup2: ");
			return (1);
		}
		close(end[0]);
	}
	else
	{
		printf("index other nub\n");
		if (dup2(end[1], STDOUT_FILENO) < 0)
		{
			perror("Dup2: ");
			return (1);
		}
		// if (dup2(end[0], STDIN_FILENO) < 0)
		// {
		// 	perror("Dup2: ");
		// 	return (1);
		// }
		 close(end[0]);
		// close(end[1]);
	}
	cmd = ft_split(allcmd, ' ');
	path = get_path(cmd[0], ev);
	if (!path)
	{
		perror("can not fine path!");
		exit(1);
	}
	printf("exce process down:%s\n", path);
	if (execve(path, cmd, ev) == -1)
	{
		perror("execve: ");
		exit(1);
	}
	return (0);
}

int		last_process(char *allcmd, char **ev)
{
	int		end[2];
	char	*path;
	char	**cmd;

	if (dup2(end[0], STDIN_FILENO) < 0)
	{
		perror("Dup2: ");
		return (1);
	}
	close(end[1]);
	cmd = ft_split(allcmd, ' ');
	path = get_path(cmd[0], ev);
	if (!path)
	{
		perror("can not fine path!");
		exit(1);
	}
	if (execve(path, cmd, ev) == -1)
	{
		perror("execve: ");
		return (1);
	}
	return (0);
}

void	pipex(int ac, char **av, char **envp)
{
	int		file1;
	int		file2;
	pid_t	child;
	int		status;
	int		i;
	int		end[2];  

	i = 1;
	//ac is even or odd?
	while(i < ac -1)
	{
		if (!ft_strncmp(av[1], "here_doc" , 5))
			printf("here_doc\n");
		else
		{
			file1 = open(av[1], O_RDONLY, 0777);
			printf("open file1\n");
			if (file1 < 0)
			{
				perror("Open: ");
				return;
			}
			if (dup2(file1, STDIN_FILENO) < 0)
			{
				perror("Dup2: ");
				return;
			}
			close(file1);
			while (++i < ac -2)
			{
				printf("here:%d\n", i);
				child = fork();
				if (pipe(end) < 0)
				{
					perror("Pipe: ");
					return ;
				}
				if (child < 0)
					return (perror("Fork: "));
				if (!child)
					exce_process(end, i,av[i], envp);
				printf("wait here:%d\n", i);
				if (dup2(end[0], STDIN_FILENO) < 0)
				{
					perror("Dup2: ");
				 	return ;
				}
				close(end[1]);
				waitpid(child, &status, 0);
			}
			printf("kepp?\n");
			file2 = open(av[ac -1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
			if (file2 < 0)
			{
				perror("Open: ");
				return;
			}
			if (dup2(file2, STDOUT_FILENO) < 0)
			{
				perror("Dup2: ");
				return;
			}
			close(file2);
			child = fork();
			if (child < 0)
				return (perror("Fork: "));
			if (!child)
				last_process(av[ac - 2], envp);
			waitpid(child, &status, 0);
		}
	}
}

int	main(int ac, char **av, char **env)
{
	if (ac < 5)
	{
		perror("arg < 5");
		return (1);
	}
	pipex(ac, av, env);
	//leaks from not free?
	//checl env?
	return (0);
}