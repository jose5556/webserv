/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cereais <cereais@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 16:04:44 by jperpct           #+#    #+#             */
/*   Updated: 2025/10/18 17:18:56 by cereais          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CGI_HPP
#define CGI_HPP

#include <http/HttpParser.hpp>
#include <config/debug.hpp>
#include <core/Server.hpp>
#include <string>


class Cgi
{
	private:
		std::string _request;
		std::string _file_name;
		std::string chek_program_pach(std::string program);
		std::vector<std::string> _path;
		int 	    save_chunk_fd(std::string);
		
	public:
		std::vector<char *> _envs;
		Cgi();
		Cgi(const Cgi &copy);
		~Cgi();
		Cgi &operator=(const Cgi &copy);		
		bool chek_program(std::string porgram);
		void create_env(char **env,std::vector<char *> env_request);
		std::string execute(std::string _request,std::string porgram);
};

#endif
