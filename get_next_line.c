/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasov <vtarasov@student.42warsaw.pl>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 14:14:29 by vtarasov          #+#    #+#             */
/*   Updated: 2026/07/23 19:49:14 by vtarasov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

static char	*save_post_newline(t_fdlist *head, int fd)
{
	char	*new;
	char	*post;
	size_t	idx;

	idx = 0;
	while (head)
	{
		if (head->fd == fd)
		{
			post = cstrchr(head->content, '\n');
			if (post)
				break ;
		}
		head = head->next;
	}
	if (!post)
		return ((char *)(0xFFFFFFFFFFFFFFFE));
	if (!*(post++))
		return ((char *)(0xFFFFFFFFFFFFFFFE));
	new = malloc(strlen_ct(post, 0) + 1);
	if (!new)
		return (0);
	while (*(post - 1))
		new[idx++] = *(post++);
	return (new);
}

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
			fdlist_clean_for_fd(list, fd);
			return (0);
		}
		localread = read(fd, last->content, BUFFER_SIZE);
		if (localread < 0)
		{
			fdlist_clean_for_fd(list, fd);
			return (0);
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

	out = malloc(1);
	*out = 0;
	while (head)
	{
		if (head->fd == fd)
		{
			buf = out;
			out = strjoin_rct(buf, head->content, '\n');
			free(buf);
			if (cstrchr(out, '\n'))
				break ;
		}
		head = head->next;
	}
	if (out[0] == 0)
	{
		free(out);
		return (0);
	}
	return (out);
}

static bool	put_save_into_stash(char *save, int fd, t_fdlist **head)
{
	size_t	idx;

	t_fdlist *const new = fdlist_addnew(head, fd);
	if (!new)
		return (false);
	idx = 0;
	while (*(save))
		new->content[idx++] = *(save++);
	new->content[idx] = 0;
	return (true);
}

char	*get_next_line(int fd)
{
	static t_fdlist	*head = 0;
	char			*line;
	char			*saveme;
	int				i;

	if (fd < 0 || BUFFER_SIZE <= 0 || read(fd, &line, 0) == -1)
		return (0);
	i = 0;
	if (!read_and_put(fd, &head, &i))
		return (0);
	line = extract_line(fd, head);
	if (!line)
		return (0);
	saveme = save_post_newline(head, fd);
	fdlist_clean_for_fd(&head, fd);
	if (saveme == 0)
		return (0);
	if (saveme != (char *)(0xFFFFFFFFFFFFFFFE))
		if (!put_save_into_stash(saveme, fd, &head))
			return (0);
	return (line);
}
