/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cereais <cereais@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 15:03:54 by jperpct           #+#    #+#             */
/*   Updated: 2025/10/28 20:48:56 by cereais          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/Config.hpp"
#include <exception>
#include <http/HttpResponse.hpp>
#include "config/color.hpp"
#include "http/HttpParser.hpp"
#include "test.hpp"


void config_and_http_implemente( int argc ,char ** argv,char**env)
{
	(void)argc;
	(void)argv;
	(void)env;
	try
	{
	std::string conf = "_configtest/youpi.conf";
	Config config("test/youpi.conf",env);
	}
	catch(std::exception &e)
	{

		std::cout << RED <<"error:" << e.what() << RESET << std::endl;
	}	
	
}


void HTTP_test_request()
{

	std::string pasta = "./HTTP_request_files";  // caminho da pasta
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(pasta.c_str())) == NULL) {
        perror("opendir");
        return; 
    }

    while ((entry = readdir(dir)) != NULL) 
    {
        // Ignorar "." e ".."
        if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
            continue;

        std::string path = pasta + "/" + entry->d_name;

	 T_MSG( "Abrindo: " << path,BWHITE);

        std::ifstream file(path.c_str());
        if (!file.is_open()) {
            std::cerr << "Erro ao abrir: " << path << std::endl;
            continue;
        }

        std::string linha, conteudo;
        while (std::getline(file, linha))
            conteudo += linha + "\n";

        file.close();
	try{

		//HTTP_MSG("what" <<  HttpResponse::request_and_response(conteudo));
	}
	catch(std::exception &e)
	{
		std::cout << RED << "error: "<< e.what() << RESET << std::endl;
	}
    }
    closedir(dir);
}


void CGI_request_test(char **envp)
{
	std::string headers =
        "POST /main.py HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"; // separa cabeçalho do corpo

    // Chunks (tamanho + dados juntos)
    std::string chunk1 = "6\r\nHello \r\n";
    std::string chunk2 = "5\r\nthis \r\n";
    std::string chunk3 = "4\r\nis a\r\n";
    std::string chunk4 = "7\r\nchunked\r\n";
    std::string chunk5 = "8\r\nmessage.\r\n";

    // Chunk final indicando fim
    std::string final_chunk = "0\r\n\r\n";

	try {

	Config conf_info("./test/youpi.conf", envp);
	std::vector<ServerConfig>	configs = conf_info.getServerConfigVector();
	std::vector<Socket*>		sockets = conf_info.getSocketVector();
	HttpResponse::set_config(configs,envp);
	HTTP_MSG(  HttpResponse::request_and_response( headers, 8022));
	
	if(HttpResponse::get_chunks_status() == true)
	{
  		HttpResponse::request_and_response( chunk1 , 8022);
	//	HTTP_MSG(  HttpResponse::request_and_response( chunk1 , 8022));
	}
	if(HttpResponse::get_chunks_status() == true)
	{

  		HttpResponse::request_and_response( chunk2 , 8022);
	//	HTTP_MSG(  HttpResponse::request_and_response( chunk2 , 8022));
	}
	if(HttpResponse::get_chunks_status() == true)
	{

  		HttpResponse::request_and_response( chunk3 , 8022);
	//	HTTP_MSG(  HttpResponse::request_and_response( chunk3 , 8022));
	}
	if(HttpResponse::get_chunks_status() == true)
	{

		//HttpResponse::request_and_response( final_chunk , 8022);
		HTTP_MSG(  HttpResponse::request_and_response( final_chunk , 8022));
	}


	} 
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}



void CGI_request_test_not_chunked(char **envp)
{

std::string body = "Hello this is achunkedmessage.\r\n\r\n";

// Calcula o tamanho do corpo

std::string headers =
    "POST /main.py HTTP/1.1\r\n"
    "Host: www.example.com\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 1000 \r\n"
    "\r\n"; // separa cabeçalho do corpo

// Request completo
(void) envp;
    // Chunk final indicando fim

	try {

//	Config conf_info("./test/youpi.conf", envp);
//	std::vector<ServerConfig>	configs = conf_info.getServerConfigVector();
//	std::vector<Socket*>		sockets = conf_info.getSocketVector();
//	HttpResponse::set_config(configs,envp);
//	HTTP_MSG(  HttpResponse::request_and_response( headers, 8022));
	
	if(HttpResponse::get_chunks_status() == true)
	{
  //		HttpResponse::request_and_response(body , 8022);
		HTTP_MSG(  HttpResponse::request_and_response( body , 8022));
	}
	
	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

int main(int argc ,char ** argv,char**env) 
{
	//HTTP_test_parser();	
 //	test_httprespons();
	(void)argv;
	(void)argc;
	(void)env;
	//execute(env);
	//config_and_http_implemente(argc,argv,env);
	//HTTP_test_request();
	CGI_request_test(env);
	CGI_request_test_not_chunked(env);


        return 0;
}



