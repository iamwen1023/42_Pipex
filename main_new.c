#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "libft/libft.h"
#include <errno.h>

# define BUILTINS_PATH "/srcs/builtins"

char    *ms_get_path(void)
{
    return (ft_strjoin(ft_strjoin(ft_strjoin(
                getcwd(NULL, 0), BUILTINS_PATH), ":"), getenv("PATH")));
}

char    *ms_getbin_path(char *bin)
{
    char    *path;
    char    **dir;
    char    *bin_path;

    if (!bin)
        return (0);
    path = ms_get_path();
    if (!path)
        return (0);
    dir = ft_split(path, ':');
    if (!dir)
        return (0);
    while (*dir)
    {
        bin_path = ft_strjoin(ft_strjoin(*dir, "/"), bin);
        if (!bin_path)
            return (0);
        if (access(bin_path, F_OK|X_OK) == 0)
            return (bin_path);
		else 
		{
			perror("access: ");
			exit(1);
		}
        ++dir;
    }
    return (0);
}

int	child_process(int end[], char **av, char **ev)
{
	int	file1;
	char **cmd;
	char *path;
	char *envarr[] = { NULL };
	//for "< file1"
	file1 = open(av[1], O_RDONLY);
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
	path = ms_getbin_path(cmd[0]);
	if (execve(path, cmd, envarr) ==  -1)
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
	char *envarr[] = { NULL };
	//for "> file2"
	file2 = open(av[4], O_WRONLY | O_CREAT);
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
	path = ms_getbin_path(cmd[0]);
	if (execve(path, cmd, envarr) ==  -1)
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

    pipe(end);
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
		printf("arg != 5");
		return 1;
	}
	pipex(av, env);
	//leaks from not free?
	return (0);
}