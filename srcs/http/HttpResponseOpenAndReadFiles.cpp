#include <fcntl.h>
#include <http/HttpResponse.hpp>
#include <http/Http_throw.hpp>
#include <http/HttpParser.hpp>
#include <dirent.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <iostream>

std::string HttpResponse::open_static_path(std::string file)
{
	std::string 		request = "HTTP/1.1 200 OK\r\n";
	size_t 			size_  = file.rfind('.');
	std::string 		type_file = "";
	static DIR 		*fd ;
	std::string 		body = "";
	static std::string 	temp;
	struct dirent *entry;

	T_MSG("start_open_path = " << file, YELLOW);

	if(size_ != std::string::npos)
	 		type_file = file.substr(size_,file.size());		 
	_request_status = false;	

	 if ((fd = opendir(file.c_str())) == NULL) 
			throw Not_found_404();
	body =  "</head><body><h1>Index of /uploads/</h1><table><tr>j<th>Nome do arquivo</th></tr>";
    	while ((entry = readdir(fd)) != NULL) 
	{
		std::string path = _parser->_pach_info + "/" + entry->d_name;
		std::string file  = entry->d_name;

		body +=	"<tr><td><a href="+ path + ">"+ file+ "</a></td></tr>";

	}
	  closedir(fd);
	body += "  </tr></tbody></table></body></html>";

		std::stringstream ss;
		ss << body.size();
		request += "Content-Length: " + ss.str() + "\r\n";

		

		request += "Content-Type: " + _types[".html"] +"\n";
	request += "Connection: keep-alive\r\n\n";

		request += body+ "\r\n\r\n";
	return (request);
}


std::string HttpResponse::open_static_file(std::string file)
{
	std::string 		request = "HTTP/1.1 200 OK\r\n";
	size_t 			size_  = file.rfind('.');
	std::string 		type_file = "";
	char 			buffer[1024];
	int 			read_bits;
	int 			size = 100000;
	static int 		fd = -1;
	std::string 		body = "";
	static std::string 	temp;
	static bool 		loop = true;

	T_MSG("start_open_file = " << file, YELLOW);

	if(size_ != std::string::npos)
	 		type_file = file.substr(size_,file.size());		 
	_request_status = false;	

	if(HttpResponse::_new_response == false)
	{
		if((fd = open(file.c_str(),O_RDWR , 0644) )== -1)
			throw Not_found_404();
	}

	body = temp;

	while ((read_bits = read(fd,buffer,1024)) > 0)
	{
		body.append(buffer,read_bits);
		if((int)body.size() > size)
			break;
	}
	if(read_bits != 0 && loop == true)
	{
		_request_status = true;	
		loop = false;
		HttpResponse::_new_response = true;
		request += "Transfer-Encoding: chunked\r\n";
		temp = body;
		body = "";
	}
	else if(read_bits == 0 && loop == true )
	{
		std::stringstream ss;
		ss << body.size();
		request += "Content-Length: " + ss.str() + "\r\n";
	}
	else 
	{
		request = "0/r/n/r/n";
		if(body.size() == 0)
		{
			_request_status = false;
			close(fd);
			fd = -1;
			HttpResponse::_new_response = false;
			return (request);
		}
		_request_status = true;	
		HttpResponse::_new_response = true;
		request = body.substr(0,size);
		if((int)body.size() > size)
			temp = body.substr(size,body.size());
		else
			temp = "";
		std::stringstream ss;
		ss << request.size();
		request = ss.str() + "\r\n" +request + "\r\n";
		return (request);
	}
		
	if(!_types[type_file].empty())

		request += "Content-Type: " + _types[type_file] +"\n";
	else
	{
		//TODO this vereficasion no finic
		request += "Content-Disposition: attachment; filename= " +   file+ '\n' ;
		request += "Content-Type: application/" + file.substr(file.size() - 4, file.size()) + ";\r\n";
	}	

	request += "Access-Control-Allow-Origin: *\r\n";

	request += "Access-Control-Allow-Methods: POST, OPTIONS\r\n";

	request += "Access-Control-Allow-Headers: *\r\n";

	request += "Connection: close\r\n\n";

	if(read_bits == 0)
	{
		request += body+ "\r\n\r\n";
		close(fd);
		fd = -1;
	}
	return (request);
}

std::string get_string_chunks(std::string chunk)
{
	std::string nub;
	std::string body_chunk;
	std::stringstream nb;
	int size;
	int len_the_chunk;

	size = chunk.find("/r/n");
	if(size == (int)std::string::npos)
		throw Not_found_404();
	nub = chunk.substr(0,size);
	nb  << nub;
	nb >> len_the_chunk;
	body_chunk = chunk.substr(size,len_the_chunk);
	return body_chunk;
}

std::string HttpResponse::save_file_post(std::string file, std::string request)
{
	static int value = 1;
	int fd = -1;
	std::string chunk;
	
	HTTP_MSG("create file in"  << file);
	fd = open(file.c_str(), O_RDONLY );
//	if(fd != -1 &&  _new_request == false)
//	{
	//	//TODO change the error 	
	//	_new_request = false;
	//	 throw Not_found_404();
	//}else 
	//	close(fd);

	 fd = open(file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
	if(fd == -1)
	{	
		_new_request = false;
		 throw Not_found_404();
	}
	if(_parser->_is_chunk == HTTP_CHUNKS)
	{
		if(value == 1)
			value = 0;
		_new_request = true;
		if(request == "0/r/n/r/n")
		{

			_new_request = false;
			value = 1;
			throw Director_Open_200();
		}else
		{
		 write(fd,chunk.c_str(),chunk.size());
		}
		return ("");
	}
	else 
	{
		 write(fd,request.c_str(), request.size());
	}
		
	throw Director_Open_200();

}


std::string HttpResponse::gener_erro_page(int error, std::string status)
{
	std::string response, mens;
	std::stringstream ok;
	std::stringstream size;
	ok << error;
	_new_request = false;
	response = "";
	response = "HTTP/1.1 " + ok.str() + " " + status + "\r\n";
	if(error == 405)
	{
		std::string method;
		for(int i =0 ; i < (int)_parser->_methods_allow.size();i++)
		{
			if(_parser->_methods_allow[i] == GET)
				method = " GET";
			if(_parser->_methods_allow[i] == POST)
				method += " POST";
			if(_parser->_methods_allow[i] == DELETE)
				method += " DELETE";

		}
		response += "Allow:" + method + "\n"; 
	}
	if(error == 200)
	{
	 response += "Content-Type: text/html\r\n";
	 response += "Connection: close\r\n";
	 response += "Content-Length: 0 \r\n\r\n";
	  return (response);
	}
	response += "Content-Type: text/html; charset=UTF-8 \n";
	// response = TODO add host the server
	mens =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head><title>" +
		ok.str() + status + "</title></head>\n"
							"<body>\n"
							"<h1>" +
		status + "</h1>\n"
				 "<hr>\n"
				 "<p> " + ok.str() +"</p>"
				 "</body>\n"
				 "</html>\n";
	size << mens.size();
	response += "Content-Length: " + size.str() + "\n\n";
	response += mens;
	return response;
}
