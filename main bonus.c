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

void	pipex(int ac, char **av, char **envp)
{
	int		end[2];
	pid_t	child1;
	pid_t	child2;
	int		status;

	if (pipe(end) < 0)
		error_message("Pipe ", 0, 0);
	child1 = fork();
	if (child1 < 0)
		error_message("Fork ", end, 0);
	if (!child1)
		child1_process(end, av, envp);
	child2 = fork();
	if (child2 < 0)
		error_message("Fork ", end, 0);
	if (!child2)
		child2_process(end, av, envp);
	close(end[0]);
	close(end[1]);
	waitpid(child1, &status, 0);
	waitpid(child2, &status, 0);
}

int	main(int ac, char **av, char **env)
{
	if (ac < 5)
	{
		perror("arg < 5");
		return (1);
	}
	pipex(ac, av, env);
	// leaks from not free?
	// check env?
	// fork double var..
	// comment not found
	return (0);
}