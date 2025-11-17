/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser_throw.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cereais <cereais@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 15:24:36 by jperpct           #+#    #+#             */
/*   Updated: 2025/10/18 17:17:39 by cereais          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <http/HttpParser.hpp>
#include <http/Http_throw.hpp>

const char *Badd_Request_400::what(void) const throw()
{
	HttpParser::_pach_info = "/400.html";
	HttpParser::_http_page_error = 400;
	return "Bad Request";
};

const char *Forbidden_403::what(void) const throw()
{
	HttpParser::_pach_info = "/403.html";
	HttpParser::_http_page_error = 403;
	return "Forbidden";
};

const char *Not_Implemented_501::what(void) const throw()
{
	HttpParser::_pach_info = "/501.html";
	HttpParser::_http_page_error = 501;
	return "Not Implemented";
};

const char *Version_Not_Supported_505::what(void) const throw()
{
	HttpParser::_pach_info = "/505.html";
	HttpParser::_http_page_error = 505;
	return "HTTP Version Not Supported";
};
const char *Not_found_404::what(void) const throw()
{
	HttpParser::_pach_info = "/404.html";
	HttpParser::_http_page_error = 404;
	return "Not Found";
}
