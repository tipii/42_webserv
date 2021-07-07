/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpalhol <tpalhol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:07:14 by tpalhol           #+#    #+#             */
/*   Updated: 2021/05/19 13:07:16 by tpalhol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

Cgi::Cgi(Request *req, Response *res)
{
	this->_res = res;
	this->_req = req;
	this->_initEnv();
	this->_envC = this->_envToCArray();
	this->_initArgC();
	this->logCgi();
}

Cgi::Cgi(Cgi const &src)
{
	*this = src;

	return ;
}

Cgi::~Cgi(void)
{
	if (this->_envC)
	{
		for (int i = 0; this->_envC[i]; i++)
			delete[] this->_envC[i];
		delete this->_envC;
	}

	if (this->_argC)
	{
		for (int i = 0; this->_argC[i]; i++)
			delete[] this->_argC[i];
		delete this->_argC;
	}
}

Cgi & Cgi::operator=(Cgi const &rhs)
{
	(void)rhs;
	return ( *this );
}

bool		Cgi::processCgi(void)
{
	Logger::Write(Logger::DEBUG, YEL, "call to cgi " + this->_req->getSelectedLocation().getCgiPath());

	std::string result;
	pid_t		pid;
	int			status;
	int			stdIn = dup(STDIN_FILENO);
	int			stdOut = dup(STDOUT_FILENO);
	FILE		*fIn = tmpfile();
	FILE		*fOut = tmpfile();
	int			fdIn = fileno(fIn);
	int			fdOut = fileno(fOut);

	// writing content of body in fdIn
	write(fdIn, this->_req->getBody().c_str(), this->_req->getBody().size());
	lseek(fdIn, 0, SEEK_SET);

	if ((pid = fork()) == -1)
	{
			Logger::Write(Logger::ERROR, RED, "cgi : fork failed");
			this->_closeFd(fIn, fOut, fdIn, fdOut);
			return false;
	}
	else if(pid == 0)
	{
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);

		if(execve(this->_req->getSelectedLocation().getCgiPath().c_str(), this->_argC, this->_envC) == -1)
		{		
			dup2(stdIn, STDIN_FILENO);
			dup2(stdOut, STDOUT_FILENO);

			this->_closeFd(fIn, fOut, fdIn, fdOut);
			Logger::Write(Logger::ERROR, RED, "cgi : in child : execve failed : " + std::string(strerror(errno)));
		}
	}
	else
	{
		char	buffer[1024];
		int		ret;

		wait(&status);
		lseek(fdOut, 0, SEEK_SET);

		// read on fdOut to get output
		while ((ret = read(fdOut, buffer, 1023)) != 0)
		{
        	buffer[ret] = 0;
			this->_result += buffer;
		}
	}

	dup2(stdIn, STDIN_FILENO);
	dup2(stdOut, STDOUT_FILENO);

	this->_closeFd(fIn, fOut, fdIn, fdOut);
	this->parseResponse();
	return true;
}



////////////////////
// INIT
////////////////////
void    	Cgi::_initEnv(void)
{
	std::string listen = this->_req->getSelectedLocation().getListen();
	std::string port = listen.substr(listen.find(":") + 1 , listen.size());
	std::string ip = listen.substr(0, listen.find(":"));

	std::map<std::string, std::string>	reqHeaders = this->_req->getHeaders();
	this->_env["AUTH_TYPE"] = reqHeaders["WWW-Authenticate"];
	this->_env["CONTENT_LENGTH"] = reqHeaders["Content-Length"];
	this->_env["CONTENT_TYPE"] = reqHeaders["Content-Type"];
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["PATH_INFO"] =  this->_req->getUrlTargetPath();
	this->_env["PATH_TRANSLATED"] = this->_req->getAbsoluteTargetPath();
	this->_env["QUERY_STRING"] = this->_req->getQueryString();
	this->_env["REMOTE_ADDR"] = this->_req->getIP();

	if (this->_res->getIsAuthenticationSucessfull())
	{
		this->_env["REMOTE_IDENT"] = reqHeaders["Authorization"];
		this->_env["REMOTE_USER"] = this->_res->getAuthorization();
	}
	this->_env["REQUEST_METHOD"] = this->_req->getMethod();
	if (this->_req->getQueryString() != "")
		this->_env["REQUEST_URI"] = this->_req->getUrlTargetPath() + "?" + this->_req->getQueryString();
	else
		this->_env["REQUEST_URI"] = this->_req->getUrlTargetPath();

	this->_env["SCRIPT_NAME"] = this->_req->getUrlTargetPath();
	this->_env["SERVER_NAME"] = ip;
	this->_env["SERVER_PORT"] = port;
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "webserv";
	this->_env["REDIRECT_STATUS"] = "200";

	for (std::map<std::string, std::string>::iterator it = reqHeaders.begin(); it != reqHeaders.end(); ++it)
	{
		if (it->second != "")
			this->_env["HTTP_" + Utils::formatToCgiEnv(it->first)] = it->second;
	}
}

void		Cgi::_initArgC(void)
{
	this->_argC = new char*[3];

	this->_argC[0] = new char[this->_req->getSelectedLocation().getCgiPath().size() + 1];
	this->_argC[0] = std::strcpy(this->_argC[0], this->_req->getSelectedLocation().getCgiPath().c_str());

	this->_argC[1] = new char[this->_req->getAbsoluteTargetPath().size() + 1];
	this->_argC[1] = std::strcpy(this->_argC[1], this->_req->getAbsoluteTargetPath().c_str());

	this->_argC[2] = 0;
}

////////////////////
// UTILS
////////////////////
void		Cgi::parseResponse(void)
{
	std::string				header;
	std::string				body;
	std::string				res = this->_result;

	if (res.find("\r\n\r\n") != std::string::npos)
	{
		header = res.substr(0, res.find("\r\n\r\n"));
		this->_result = res.substr(res.find("\r\n\r\n") + 4, res.length());
	}
	else
		return ;

	unsigned long			it;
	std::string				line;
 	std::string				key;
    std::string				value;
	while (Utils::cutLine(&it, &line, &header, "\r\n"))
    {
		if(line.find(":") == std::string::npos)
			continue ;
			
		key = line.substr(0, line.find(":"));
		value = line.substr(line.find(":") + 2, line.size());
		this->_headers[key] = value;

		if (it == std::string::npos)
			break ;	
	}
}

void		Cgi::_closeFd(FILE *fIn, FILE *fOut, int fdIn, int fdOut)
{
	close(fdIn);
	close(fdOut);
	fclose(fIn);
	fclose(fOut);
}

char		**Cgi::_envToCArray(void)
{
	char    **res = new char*[this->_env.size() + 1];
	int     i = 0;

	for (std::map<std::string, std::string>::const_iterator it = this->_env.begin(); it != this->_env.end(); ++it)
	{
		std::string     elem = it->first + "=" + it->second;
		res[i] = new char[elem.size() + 1];
		res[i] = std::strcpy(res[i], elem.c_str());
		i++;
	}
	res[i] = 0;

	return (res);
}

void		Cgi::logCgi(void)
{
	Utils::printMap(this->_env, "cgi::_env\n");
}

////////////////////
// GETTERS / SETTERS
////////////////////
std::string	Cgi::getResult(void)
{
	return (this->_result);
}

std::map<std::string, std::string>	Cgi::getCgiHeaders(void)
{
	return (this->_headers);
}
