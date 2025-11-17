/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cereais <cereais@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/26 14:03:03 by jperpct           #+#    #+#             */
/*   Updated: 2025/10/18 17:17:08 by cereais          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/color.hpp"
#include "http/HttpParser.hpp"
#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <http/Http_throw.hpp>
#include "http/HttpParser.hpp"
#include "http/HttpResponse.hpp"
#include <config/color.hpp>
#include <config/LocationConfig.hpp>
#include <core/Server.hpp>
#include <http/HttpParser.hpp>
#include <cgi/cgi.hpp>
#include <ostream>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>



Cgi::Cgi(){}
Cgi::~Cgi(){}


std::string Cgi::chek_program_pach(std::string porgram)
{
	T_MSG("error" << _path.size(),BWHITE )	;

	if(!(_path.size() >= 1))
	{
		int i = -1;
		while(++i < (int)_envs.size())
		{
			if(std::strncmp("PATH=",_envs[i],5) == 0)
			{
				break;	 // ver error a for 
			}
		}
		std::stringstream ss(_envs[i]);
		std::string path;
		
		while (std::getline(ss,path,':'))
		{
			_path.push_back(path);	
		}	
	}
	struct dirent *name_dir;
	std::string name_program_dir;
	for(int i=0;(int)_path.size() > i; i++)
	{
		if(i == 0)
		{
			_path[i] = _path[i].substr(5,_path[i].size());
		}
 		DIR* dir = opendir(_path[i].c_str());
		if(dir == NULL)
		{
			continue;
		}
		name_dir = readdir(dir);
		while ( name_dir != NULL) 
		{
			name_program_dir = name_dir->d_name;
			if(name_program_dir == porgram)
			{

				 if (access((_path[i] +"/"+name_program_dir).c_str(), X_OK) == 0)
				 {
					closedir(dir);
					return (_path[i]);
				 }
				 else
				 {
					 // tenho que chekar que error por aqui 
					 throw Not_found_404();
				 }
			}
			name_dir = readdir(dir);
		}

		closedir(dir);
	}
	T_MSG("error",BWHITE )	;
	return NULL;
}

void Cgi::create_env( char **env,std::vector<char *> env_request)
{
	int i = 0;
	_envs = env_request;

	while (env[i] != NULL)
	{
		_envs.push_back(env[i]);

		i++;
	} 
	for (int e = 0; e < (int)env_request.size();e++) 
	{
		//HTTP_MSG(env_request[e]);
	}
}

int Cgi::save_chunk_fd(std::string str)
{
	static int fd=  -1;
	static int body = 0;


	
	std::stringstream port;
	port << HttpParser::_port;
	_file_name =  "/tmp/saida_"+ port.str() + ".txt";
	if( HttpParser::_is_chunk == HTTP_CONTENT)
	{
		if(body == 1)
		{
			body = 0;
			fd = open(_file_name.c_str(),O_RDWR | O_CREAT , 0644);
			write(fd,str.c_str(),str.size()-4);
			close(fd);
			return (open(_file_name.c_str(),O_RDWR | O_CREAT , 0644));
		}
		if(body == 0)
		{ 
			body = 1;
			return(-1);
		}
	}

	if(HttpParser::_is_chunk == HTTP_CHUNKS)
	{
		if(str.empty())
		{
			return (-1);
		}
		if(str == "0\r\n\r\n")
			return (open(_file_name.c_str(),O_RDWR | O_CREAT , 0644));
		fd = open(_file_name.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
		if(fd == -1)
			throw Not_found_404(); //TODO nao e este o error  

		int size =  str.find('\r');
		if(size == (int)std::string::npos)
			throw Not_found_404(); //TODO nao e este o error  
		int bits;
		std::string nb = str.substr(0,size);
		std::string al = str.substr(size+2,str.rfind('\r'));
		std::stringstream ss(str);
		ss >> bits;
		if(ss.fail())
			throw Not_found_404(); //TODO nao e este o error  
		write(fd,al.c_str(),bits);
		close(fd);
	}
	return -1;
}

std::string Cgi::execute(std::string _request, std::string porgram)
{
	int pid ;
	int fd_in;
	int fd_out[2];
	int status,read_bits;
	char buffer[1024];
	std::string  response = "";
	char **end = NULL;
	
	int fd =-1;
	
	if((fd = Cgi::save_chunk_fd(_request)) == -1)
	{
		return "";
	}
	fd_in = fd;	
	HttpResponse::_new_request = false;
	if(pipe(fd_out) == -1)
		exit(1);	
	_envs.push_back(NULL);	
 	std::cout.flush();
	pid = fork();
	if(pid == -1)
		exit(1);	

	if(pid == 0)
	{	

		dup2(fd_in,0);
		dup2(fd_out[1],1);	
		close(fd_in);
		int i  = execve( porgram.c_str(),end,_envs.data());
		HTTP_MSG("merda = " << i)
		perror("execve");
		exit(33);
	}
	else
	{
		close(fd_out[1]);
		close(fd_in);
		while ((read_bits = read(fd_out[0],buffer,1024)) > 0)
		{
			response.append(buffer,read_bits);
		}
		response.append("\r\n\r\n");
		close(fd_out[0]);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
   		 int exit_code = WEXITSTATUS(status);
    		std::cout << "CGI exited with code: " << exit_code << response << std::endl;
		std::remove(_file_name.c_str());
		if(exit_code == 33) // TODO change this value for 0 
			throw Not_found_404();
		} 
	}
	return response;
}
