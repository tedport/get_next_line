/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasov <vtarasov@student.42warsaw.pl>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 14:14:29 by vtarasov          #+#    #+#             */
/*   Updated: 2026/08/03 19:29:15 by vtarasov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

static t_fdlist	*read_and_put(int fd, t_fdlist **list, int *bytes_read)
{
	t_fdlist	*last;
	int			localread;

	localread = 1;
	while (localread > 0)
	{
		last = fdlist_addnew(list, fd);
		if (!last)
		{
			fdlist_clean_for_fd(list, fd, false);
			return (NULL);
		}
		localread = read(fd, last->content, BUFFER_SIZE);
		if (localread < 0)
		{
			fdlist_clean_for_fd(list, fd, false);
			return (NULL);
		}
		last->content[localread] = 0;
		*bytes_read += localread;
		if (cstrchr(last->content, '\n'))
			break ;
	}
	return (last);
}

static char	*extract_line(int fd, t_fdlist *head)
{
    char	*buf;
    char	*out;

    out = strjoin_rct("", "", 0);
    while (out && head)
    {
        if (head->fd == fd)
        {
            buf = out;
            out = strjoin_rct(buf, head->content, '\n');
            free(buf);
            if (out && cstrchr(out, '\n'))
                break ;
        }
        head = head->next;
    }
    if (out && *out)
        return (out);
    free(out);
    return (NULL);
}

static void	strip_pre_newline(t_fdlist *head, int fd)
{
	char	*newline;
	char	*src;
	int		i;

	while (head)
	{
		if (head->fd == fd)
		{
			newline = cstrchr(head->content, '\n');
			if (newline)
			{
				src = newline + 1;
				i = 0;
				while (src[i])
				{
					head->content[i] = src[i];
					i++;
				}
				head->content[i] = '\0';
			}
			return ;
		}
		head = head->next;
	}
}

static bool	stash_has_newline(t_fdlist *head, int fd)
{
	while (head)
	{
		if (head->fd == fd && cstrchr(head->content, '\n'))
			return (true);
		head = head->next;
	}
	return (false);
}

char	*get_next_line(int fd)
{
	static t_fdlist	*head = 0;
	char			*line;
	int				i;

	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, &line, 0) == -1)
	{
		fdlist_clean_for_fd(&head, fd, false);
		return (NULL);
	}
	i = 0;
	if (!stash_has_newline(head, fd))
	{
		if (!read_and_put(fd, &head, &i))
			return (NULL);
	}
	line = extract_line(fd, head);
	if (!line)
	{
		fdlist_clean_for_fd(&head, fd, false);
		return (NULL);
	}
	fdlist_clean_for_fd(&head, fd, true);
	strip_pre_newline(head, fd);
	return (line);
}
