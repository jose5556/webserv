/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http_thow.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jperpct <jperpect@student.42porto.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:27:29 by jperpct           #+#    #+#             */
/*   Updated: 2025/09/24 14:48:03 by jperpct          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <exception>

	class   Badd_Request_400 : public std::exception 
	{
		public:
			virtual const char *what()const throw();
	};

	class   Not_Implemented_501 : public std::exception
	{
		public:
			virtual const char *what()const throw();
	};
	class   Version_Not_Supported_505 : public std::exception
	{
		public:
			virtual const char *what()const throw();
	};
	class   Not_found_404: public std::exception
	{
		public:
			virtual const char *what()const throw();
	};
	class   Forbidden_403: public std::exception
	{
		public:
			virtual const char *what()const throw();
	};
