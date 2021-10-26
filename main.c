#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "libft/libft.h"
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
        ++dir;
    }
    return (0);
}
  

int main(int ac, char **av, char **env)
{
    //try cm1 | cm2
    int fd1;
    int fd2;
    //char *argarr[] = { "ls","-l", NULL };
    char **argarr1;
    char *envarr1[] = { NULL };
    char *envarr2[] = { NULL };
    char *path1;
    char **argarr2;
    char *path2;
    int fdpipe[2];

    if (pipe(fdpipe) == -1)
        return 1;
    int pid1 = fork();
    if (pid1 < 0)
        return 1;
    if (pid1 == 0)
    {
        //child process 1
        argarr1 = ft_split(av[1], ' ');
        int i = -1;
        path1 = ms_getbin_path(argarr1[0]);
        //close stdout, redict stdout to fdpipe[1]
        dup2(fdpipe[1], STDOUT_FILENO);
        close(fdpipe[0]);
        close(fdpipe[1]);
        if (execve(path1, argarr1, envarr1) ==  -1)
        {
            printf("Erorr happended while execve\n");
            return (1);
        }
    }
    int pid2 = fork();
    if (pid2 < 0)
        return 1;
    if  (pid2 == 0)
    {
        //child process 2
        argarr2 = ft_split(av[2], ' ');
        int i = -1;
        path2 = ms_getbin_path(argarr2[0]);
        dup2(fdpipe[0], STDIN_FILENO);
        close(fdpipe[0]);
        close(fdpipe[1]);
        if (execve(path2, argarr2, envarr2) ==  -1)
        {
            printf("Erorr happended while execve2\n");
            return (1);
        }
    }
    close(fdpipe[0]);
    close(fdpipe[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);    
    //leaks from malloc
    return (0);
}
