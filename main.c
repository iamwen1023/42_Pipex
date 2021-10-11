#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "libft/libft.h"


int main(int ac, char **av, char **env)
{
    int fd1;
    int fd2;
    //char *argarr[] = { "ls","-l", NULL };
    char **argarr;
    char *envarr[] = { NULL };
    char *path;


    if (ac != 5)
    {
        printf("Wrong paramaters\n");
        return (1);   
    }
    fd1 = open(av[1], O_RDONLY);
    if (!fd1)
    {
        printf("Erorr happended while opening fd1\n");
        return (1);
    }
    printf("excute\n");
    //usr/bin/ls?
    argarr = ft_split(av[2], ' ');
    int i = -1;
    printf("argrr:\n");
    while(argarr[++i])
    {
        printf("%d:%s\n", i , argarr[i]);
    }
    path = ft_strjoin("/bin/", argarr[0]);
    printf("path:%s\n", path);
    if (execve(path, argarr, envarr) ==  -1)
    {
        printf("Erorr happended while execve\n");
        return (1);
    }

    return (0);
