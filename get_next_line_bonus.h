/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasov <vtarasov@student.42warsaw.pl>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 14:14:27 by vtarasov          #+#    #+#             */
/*   Updated: 2026/07/29 22:04:16 by vtarasov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H
# include <stddef.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4096
# endif

typedef struct s_fdlist
{
	struct s_fdlist	*next;
	int				fd;
	char			content[BUFFER_SIZE + 1];
}					t_fdlist;

t_fdlist			*fdlist_addnew(t_fdlist **list, int fd);
void				fdlist_clean_for_fd_until_nl(t_fdlist **list, int fd);
char				*strjoin_rct(const char *left, const char *right,
						char s2_terminator);
char				*cstrchr(const char *s, int c);
size_t				strlen_ct(char const *s, char term);

char				*get_next_line(int fd);

#endif