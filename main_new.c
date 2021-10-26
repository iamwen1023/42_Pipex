#include "includes/pipex.h"
void	free_path(char **paths)
{
	int i;

	i = 0;
	while(paths[i])
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

int	child_process(int end[], char **av, char **ev)
{
	int	file1;
	char **cmd;
	char *path;

	//for "< file1"
	file1 = open(av[1], O_RDONLY, 0777);
	if (file1 < 0)
	{
		perror("Open: ");
		return (1);
	}
	if (dup2(file1, STDIN_FILENO) < 0)
	{
		perror("Dup2: ");
		return (1);
	}
	if (dup2(end[1], STDOUT_FILENO) < 0)
	{
		perror("Dup2: ");
		return (1);
	}
	close(end[0]);
	close(file1);
	cmd = ft_split(av[2], ' ');
	//path = ms_getbin_path(cmd[0]);
	path = get_path(cmd[0], ev);
	if (!path)
	{
		perror("can not fine path!");
		exit(1);
	}
	if (execve(path, cmd, ev) ==  -1)
    {
        perror("execve: ");
        return (1);
    }
	return (0);
}

int	parent_process(int end[], char **av, char **ev)
{
	int	file2;
	char **cmd;
	char *path;

	//for "> file2"
	file2 = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (file2 < 0)
	{
		perror("Open: ");
		return (1);
	}
	if (dup2(file2, STDOUT_FILENO) < 0)
	{
		perror("Dup2: ");
		return (1);
	}
	if (dup2(end[0], STDIN_FILENO) < 0)
	{
		perror("Dup2: ");
		return (1);
	}
	close(end[1]);
	close(file2);
	cmd = ft_split(av[3], ' ');
	//path = ms_getbin_path(cmd[0]);
	path = get_path(cmd[0], ev);
	if (!path)
	{
		perror("can not fine path!");
		exit(1);
	}
	if (execve(path, cmd, ev) ==  -1)
    {
        perror("execve: ");
        return (1);
    }
	return (0);
} 
void    pipex(char **av, char **envp)
{
    int   end[2];
    pid_t child1;
	pid_t child2;
	int   status;

    if (pipe(end) < 0)
		return (perror("Pipe: "));
    child1 = fork();
    if (child1 < 0)
        return (perror("Fork: "));
    if (!child1) 
        child_process(end, av, envp);
	child2 = fork();
	if (child2 < 0)
        return (perror("Fork: "));
    if (!child2)
        parent_process(end, av, envp);
	close(end[0]);
	close(end[1]);
	waitpid(child1, &status, 0);
	waitpid(child2, &status, 0);
}

int main(int ac, char **av, char **env)
{
	if (ac != 5)
	{
		perror("arg != 5");
		return 1;
	}
	pipex(av, env);
	//leaks from not free?
	return (0);
}