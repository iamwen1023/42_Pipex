#include "includes/pipex.h"

t_list	*init_cmd_doc(int ac, char **av)
{
	int		i;
	t_list	*cmd;
	t_list	*cmds;

	i = 2;
	cmds = NULL;
	while (++i < ac - 1)
	{
		cmd = ft_lstnew(av[i]);
		ft_lstadd_back(&cmds, cmd);
	}
	return (cmds);
}
int		*pipex_creat_here(int ac, t_list *cmds)
{
	int		*end;
	int		i;

	end = malloc((ac - 5) * 2 * sizeof(int));
	if (!end)
		error_message_bo("malloc", 0, 0, cmds);
	i = -1;
	while(++i < ac - 5)
	{
		if (pipe(&end[2 * i]) < 0)
			error_message_bo("pipe", 0, 0, cmds);
	}
	return end;
}

void	pipex_heredoc(int ac, char **av, char **envp)
{
	//int		end[(ac - 5) * 2];
	int		*end;
	pid_t	child1;
	t_list	*cmds;
	t_list	*cmds_re;
	int		file1;
	int		file2;
	int		j;
	int		i;
	//Variable length array forbidden

	cmds = NULL;
	cmds = init_cmd_doc(ac, av);
	cmds_re = cmds;
	end = pipex_creat_here(ac , cmds);
	j = 0;
	while (cmds != NULL )
	{
		child1 = fork();
		if (child1 < 0)
			error_message_bo("Fork ", end, (ac - 5) * 2, cmds_re);
		if (child1 == 0)
		{
			if (j == 0)
			{
				file1 = open("temp", O_RDONLY);
				if (file1 < 0)
					error_message_bo("Open ", end, (ac - 5) * 2, cmds_re);
				if (dup2(file1, STDIN_FILENO) < 0)
					error_message_bo("Dup2 ", end, (ac - 5) * 2, cmds_re);
				close(file1);
			}
			else if (j == ac - 5)
			{
				file2 = open(av[ac - 1], O_WRONLY | O_CREAT | O_APPEND, 0777);
				if (file2 < 0)
					error_message_bo("Open ", end, (ac - 5) * 2, cmds_re);
				if (dup2(file2, STDOUT_FILENO) < 0)
					error_message_bo("Dup2 ", end, (ac - 5) * 2, cmds_re);
				close(file2);
			}
			replace(ac - 3, cmds, end, j, envp, cmds_re);
			normal_free(cmds_re);
		}
		cmds = cmds->next;
		j++;
    }
	//parent process
	i = -1;
	while (++i < (ac - 5) * 2)	
		close(end[i]);
	free(end);
	normal_free(cmds_re);
	while (errno != ECHILD)
		wait(NULL);
}

int	check_get_next_line(char *content, char *keyword)
{
	int	len;

	len = 0;
	while (content[len] && content[len] != '\n')
		len++;
	if (ft_strncmp(content, keyword, (int)ft_strlen(keyword)))
	{
		content = content + len + 1;
		if (*content)
		{
			if (check_get_next_line(content, keyword) == 1)
				return 1;
			return (0);
		}
		else
			return (0);
	}
	else
		return (1);
}

char	*ft_strjoin_gnl(char **s1, char *s2)
{
	char	*arr;
	size_t	i;
	size_t	j;

	if (!(*s1) || !s2)
		return (0);
	arr = (char *)malloc((ft_strlen(*s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (!arr)
		return (0);
	i = 0;
	while ((*s1)[i])
	{
		arr[i] = (*s1)[i];
		i++;
	}
	j = 0;
	while (s2[j])
	{
		arr[i + j] = s2[j];
		j++;
	}
	arr[i + j] = '\0';
	free(*s1);
	(*s1) = NULL;
	return (arr);
}

int	get_next_line(char **content, char *keyword)
{
	char	buffer[2];
	int		ret;
	char	*newkeyword;

	ret = 1;
	if (!keyword)
	{
		printf("no KEYWORD\n");
		exit(1);
	}
	newkeyword = ft_strjoin(keyword, "\n");
	while (ret > 0)
	{
		ret = read(0, buffer, 1);
		buffer[1] = '\0';
		if (ret < 0 )
			break ;
		else if (ret == 0)
			return (0);
		if ((*content) == 0)
		{
			(*content) = malloc(sizeof(char) * 2);
			if (!(*content))
				return (-1);
			(*content)[0] = buffer[0];
			(*content)[1] = '\0';
		}
		else
			(*content) = ft_strjoin_gnl(content, buffer);
		if (buffer[0] == '\n')
		{
			if (check_get_next_line(*content, newkeyword) == 1)
			{
				free(newkeyword);
				return (0);			
			}
		}
	}
	return (-1);
}

void	here_doc(int ac, char **av, char **envp)
{
	static char	*content = NULL;
	int			fd;

	fd = open("temp", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd < 0)
		error_message_bo("Open ", 0, 0 , 0);
	if (get_next_line(&content, av[2]) < 0)
	{
		close(fd);
		if (*content)
			free(content);
		perror("gnl");
		exit(1);
	}
	write(0, content, ft_strlen(content) - ft_strlen(av[2]) - 1);
	write(fd, content, ft_strlen(content) - ft_strlen(av[2]) - 1);
	close(fd);
	if (*content)
		free(content);
	pipex_heredoc(ac, av, envp);
	if (!access("temp", F_OK))
        unlink("temp");
}