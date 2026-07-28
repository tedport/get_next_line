/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasov <vtarasov@student.42warsaw.pl>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 14:14:30 by vtarasov          #+#    #+#             */
/*   Updated: 2026/07/28 22:39:22 by vtarasov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdlib.h>

t_fdlist	*fdlist_addnew(t_fdlist **list, int fd)
{
	t_fdlist		*cur;
	size_t			idx;
	t_fdlist *const	new = malloc(sizeof(t_fdlist));

	idx = 0;
	if (!new)
		return (0);
	while (idx < BUFFER_SIZE + 1)
		new->content[idx++] = 0;
	new->fd = fd;
	new->next = 0;
	if (!*list)
		*list = new;
	else
	{
		cur = *list;
		while (cur->next)
			cur = cur->next;
		cur->next = new;
	}
	return (new);
}

void	fdlist_clean_for_fd(t_fdlist **list, int fd)
{
	t_fdlist	*node_to_free;
	t_fdlist	**current;

	if (!list || !*list)
		return ;
	current = list;
	while (*current)
	{
		if ((*current)->fd == fd)
		{
			node_to_free = *current;
			*current = (*current)->next;
			free(node_to_free);
		}
		else
			current = &(*current)->next;
	}
}

char	*cstrchr(const char *s, int c)
{
	while (1)
	{
		if (*s == c)
			return ((char *)s);
		if (*s == 0)
			break ;
		s++;
	}
	return (0);
}

size_t	strlen_ct(char const *s, char term)
{
	int	i;

	i = 0;
	while (s[i] && s[i] != term)
		i++;
	return (i);
}

char	*strjoin_rct(const char *s1, const char *s2, char s2t)
{
	const size_t	desired_size = (strlen_ct(s1, 0) + strlen_ct(s2, s2t) + 1);
	size_t			i;
	size_t			j;
	char *const		out = malloc(desired_size + (s2[strlen_ct(s2, s2t)] != 0));

	if (!out)
		return (0);
	out[desired_size + (s2[strlen_ct(s2, s2t)] != 0) - 1] = 0;
	i = 0;
	j = 0;
	while (s1[i])
	{
		out[i] = s1[i];
		i++;
	}
	while (s2[j])
	{
		out[i + j] = s2[j];
		j++;
		if (s2[j - 1] == s2t)
			break ;
	}
	return (out);
}
