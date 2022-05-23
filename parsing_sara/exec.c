/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iouardi <iouardi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 18:47:11 by iouardi           #+#    #+#             */
/*   Updated: 2022/05/23 03:36:24 by iouardi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../parsing_sara/minishell.h"

void    echo_command(char **arr) // char **arr
{
	int		i;

	if (!ft_strncmp(arr[1], "-n", ft_strlen(arr[1])))
	{
		i = 2;
		while (arr[i])
		{
			write (1, arr[i], ft_strlen(arr[i]));
			i++;
		}
	}
	else
	{
		i = 1;
		while (arr[i])
		{
			write (1, arr[i], ft_strlen(arr[i]));
			i++;
		}
		write (1, "\n", 1);
	}
}

char	*check_path_home(t_env *env)
{
	t_env	*tmp;

	tmp = env;
	while (tmp)
	{
		if (ft_strnstr(tmp->name, "HOME", ft_strlen(tmp->name)))
			return(tmp->value);
		tmp= tmp->next;
	}
	return (NULL);
}

void	cd_command(char	**arr, t_env *env)
{
	int		i;
	t_env	*tmp_new;
	t_env	*tmp;
	t_env	*tmp1;
	char	*path;

	i = 1;
	tmp = env;
	if (!arr[1])
	{
		path = check_path_home(env);
		chdir(path);
		return ;
	}
	tmp = env;
	if (!chdir(arr[1]))
	{
		while (tmp)
		{
			if (!ft_strcmp(tmp->name, "TERM_PROGRAM_VERSION"))
			{
				tmp_new = new_node("OLDPWD", arr[1]);
				tmp1 = tmp->next->next;
				tmp->next = tmp_new;
				tmp_new->next = tmp1;
				return ;
			}
			tmp = tmp->next;
		}
	}
	else
		printf("bash: cd: %s: No such file or directory\n", arr[1]);
}

void	pwd_command()
{
	char	*buffer;

	if ((buffer = getcwd(NULL, 0)))
		printf("%s\n", buffer);//don't forget to check in case of an error
}

void	print_export_env(t_env *env)
{
	t_env	*tmp;

	tmp = env;
	while (tmp)
	{
		if (!tmp->value)
			printf("declare -x %s\n", tmp->name);
		else			
			printf("declare -x %s=\"%s\"\n", tmp->name, tmp->value);
		tmp = tmp->next;
	}
}

char	**split_name_value(char	*arg)
{
	char	**arr;

	arr = ft_split(arg, '=');
	return (arr);
}

int	parse_args(char *var)
{
	int		i;

	if (!ft_isalpha(var[0]) && var[0] != '_')
	{
		printf("bash: export: `%s': not a valid identifier\n", var);
		return (0);
	}
	i = 1;
	while (var[i])
	{
		if (!ft_isalnum(var[i]) && var[i] != '_')
		{
			printf("bash: export: `%s': not a valid identifier\n", var);
			return (0);
		}
		i++;
	}
	return (1);
}

int	already_exists(char **arg, t_env *env)
{
	t_env	*tmp;
	
	tmp = env;
	while (tmp)
	{
		if (!ft_strcmp(tmp->name, arg[0]))
		{
			if (!arg[1] && !tmp->value)
			{
				printf("----------------env->value%s\n", tmp->value);
				return (1);
			}
			else if (!ft_strcmp(tmp->value, arg[1]))
					return (1);
		}
		tmp = tmp->next;
	}
	return (0);
}

int	already_exists_modified_value(char **arg, t_env *env)
{
	t_env	*tmp;

	tmp = env;
	while (tmp)
	{
		if (!ft_strcmp(tmp->name, arg[0]) && ft_strcmp(tmp->value, arg[1]))
			return (1);
		tmp = tmp->next;
	}
	return (0);
}

void	export_command(char **arr, t_env *env)
{
	int		i;
	char	**arg_splited;

	i = 1;
	while (arr[i])
	{
		arg_splited = split_name_value(arr[i]);
		if (!parse_args(arg_splited[0]))
			return ;
		if (!already_exists(arg_splited, env))
			add_back(&env, new_node(arg_splited[0], arg_splited[1]));
		else if (!already_exists_modified_value(arg_splited, env))
				env_add_change1(&env, arg_splited[0], arg_splited[1]);
		else
			i++;
	}
	if (!arr[1])
		print_export_env(env);
}

void	execute_commands(t_data	*data)
{
	if (!ft_strncmp(data->f_list->arr[0], "echo", 5))
		echo_command(data->f_list->arr);
	if (!ft_strncmp(data->f_list->arr[0], "cd", 3))
		cd_command(data->f_list->arr, data->env);
	if (!ft_strncmp(data->f_list->arr[0], "pwd", 4))
		pwd_command();
	if (!ft_strncmp(data->f_list->arr[0], "export", 7))
		export_command(data->f_list->arr, data->env);
}