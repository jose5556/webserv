#pragma once

#ifndef LOCATIONCONFIG_H
#define LOCATIONCONFIG_H

// TODO Considerar meter estas libraries num unico ficheiro
#include <iostream>
#include <sstream>
#include <fstream>
#include <locale>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <climits>

#include <vector>
#include <map>
#include <algorithm>
#include <typeinfo>

#include <sys/socket.h> // TODO Ver se ha melhor
#include <netinet/in.h> // TODO Ver se ha melhor

enum t_methods
{
	GET,	// Retrieve a resource
	HEAD,	// Retrieve a header from a resource
	POST,	// Send data to server
	PUT,	// Upload a resource to a specific URL
	DELETE,	// Remove a resource
	OPTIONS,// Inquire server about available methods
	PATCH,	// Partially modify a resource
	TRACE,	// Echo a request
	CONNECT // Establish a proxy tunnel
};

class LocationConfig
{
public:
	LocationConfig();
	LocationConfig(std::istream& location_file, std::string line);
	LocationConfig(const LocationConfig &orig);
	LocationConfig &operator = (const LocationConfig &orig);
	virtual ~LocationConfig();

	std::string const		&getName(void) const;
	std::string const		&getRoot(void) const;
	std::string const		&getIndex(void) const;
	std::string const		&getPass(void) const;
	unsigned long			getClientBuffSize(void) const;
	//bool const				&getAlias(void) const;
	bool					checkSubLocation(void) const;
	LocationConfig			&getSubLocation(unsigned int num);
	LocationConfig const	&getSubLocation(unsigned int num) const;
	bool					hasMethod(t_methods method) const;

	std::vector<t_methods> const					&getMethods() const;
	std::map<std::string, LocationConfig>			&getSubLocationMap(void);
	std::map<std::string, LocationConfig> const		&getSubLocationMap(void) const;

	void	setSubLocation(LocationConfig* loc);
	void	setMethods(std::string const str);
	void	copyMethods(std::vector<t_methods> const &orig);
	void	setOneMethod(std::string word);
	void	setIndex(std::string index);
	void	setRoot(std::string root);
	void	setName(std::string name);
	void	setPass(std::string pass);
	void	setClientBuffSize(std::string buff_size);
	//void	setAlias(bool alias);

	LocationConfig*	clone(void) const;

	void	parse_location(std::istream& location_file, std::string line);

	std::string		_cgi_pass;	// ex: ./test_linux/ubuntu_cgi_tester

private:
	std::string		_name;		// ex: /directory
	std::string		_root;		// ex: ./joao-rib/
	std::string		_index;		// ex: joao.bad_extension
	//std::string		_cgi_pass;	// ex: ./test_linux/ubuntu_cgi_tester // artifact
	unsigned long	_client_body_buffer_size; // ex: 100
	//bool			_alias;		// Should be a string, if implemented // artifact

	std::vector<t_methods>		_methods;	// ex: GET

	std::map<std::string, LocationConfig>	_sub_locations;

	// maybe TODO ifs (cookies, bonus)
	// if ($cookie_<name> [operator] <value>) { <action> }
};

#endif

/*

Quando um request é recebido e parseado:
O servidor olha para a URI do request
Procura dentro das locations uma LocationConfig cujo path combine ou seja prefixo da URI.
Aplica as regras dessa location, por exemplo:
Sobrescreve o root global para aquela rota.
Decide se é CGI ou ficheiro estático.
Define o index se for diretório.

*/

/*
//Example 1
if ($cookie_user) {
    return 200 "Welcome back, $cookie_user!";
}

//Example 2
if ($cookie_sessionid = "") {
    return 403;
}

//Example 3
if ($cookie_auth = "") {
    return 302 /login;
}

//Example 4
if ($cookie_sessionid = "") {
    add_header Set-Cookie "sessionid=$request_id; Path=/;";
}

//Example 5
if ($cookie_nocache) {
    set $no_cache 1;
}

//Example 6
if ($cookie_role = "admin") {
    return 302 /admin_dashboard;
}

*/
