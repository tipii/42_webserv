/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:06:07 by sadarnau          #+#    #+#             */
/*   Updated: 2021/05/19 13:06:08 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

Webserv::Webserv( void )
{
	return ;
}

Webserv::Webserv( std::string listen, std::vector<Location > locationVector, std::vector<Location> locationExtVector ) : _listen(listen), _locationVector(locationVector),
																															_locationExtVector(locationExtVector)
{
	return ;
}

Webserv::Webserv( Webserv const & src )
{
	*this = src;

	return ;
}

Webserv::~Webserv( void )
{
	return ;
}

Webserv & Webserv::operator=( Webserv const & rhs)
{
	this->_IPaddr = rhs._IPaddr;
	this->_port = rhs._port;
	this->fd = rhs.fd;
	this->address = rhs.address;
	this->_listen = rhs._listen;
	this->_locationVector = rhs._locationVector;
	this->_locationExtVector = rhs._locationExtVector;
	this->_serverNb = rhs._serverNb;

	return ( *this );
}

int		Webserv::initialization( int i )
{
	this->_serverNb = i;

	if ((this->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		Logger::Write(Logger::ERROR, RED, "error : couldn't assign the socket...");
		return (1);
	}

	Logger::Write(Logger::DEBUG, GRN, "server[" + Utils::intToStr(this->_serverNb) + "] : socket created");
	
	this->fillAddress();
	
	// Fix binding error, it was due to TIME_WAIT who deosnt allow new connection to same socket before a certain time
	int reusePort = 1;
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort)) < 0)
		return (1);

	if ((bind(this->fd, (struct sockaddr *)&this->address, sizeof(this->address))) < 0)
	{
		Logger::Write(Logger::ERROR, RED, "error : couldn't bind the socket...");
		close(this->fd);
		return (1);
	}

	Logger::Write(Logger::DEBUG, GRN, "server[" + Utils::intToStr(this->_serverNb) + "] : socket binded");

	if ((listen(this->fd, 10000)) < 0) 			// 10000 = number of max connections
	{
		Logger::Write(Logger::ERROR, RED, "error : couldn't listen the socket");
		return (1);
	}

	Logger::Write(Logger::INFO, GRN, "server[" + Utils::intToStr(this->_serverNb) + "] : listening port " + this->getPort() + "...");

	return (0);
}

void	Webserv::fillAddress( void )
{
	this->_port = this->_listen.substr(this->_listen.find(":") + 1 , this->_listen.size());
	this->_IPaddr = this->_listen.substr(0, this->_listen.find(":"));

	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = inet_addr(this->_IPaddr.c_str());
	this->address.sin_port = htons(Utils::strToInt(this->_port));

	memset(this->address.sin_zero, 0, sizeof(this->address.sin_zero));

	return ;
}

int		Webserv::acceptConexion( void )
{
	struct sockaddr_in	address;
	unsigned int		addrlen = sizeof(address);
	long				socket = accept(this->fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	
	if (socket < 0)
	{
		Logger::Write(Logger::ERROR, RED, "error : could not accept new connection");
		return (-1);
	}
	
	fcntl(socket, F_SETFL, O_NONBLOCK);

	return (socket);
}

std::string	Webserv::getResponse( long socket, Client client )
{
	char		 		socketIP[16];
	struct sockaddr_in	addr;

	bzero(&addr, sizeof(addr));
	socklen_t len = sizeof(addr);
	getsockname(socket, (struct sockaddr *) &addr, &len);
	inet_ntop(AF_INET, &addr.sin_addr, socketIP, sizeof(socketIP));

	Request		request(&this->_locationVector, &this->_locationExtVector, socket, client.getBuffer(), socketIP);
	request.logRequest(this->_serverNb);

	Response	response(&request, socket);
	response.logResponse(this->_serverNb);

	return (response.getResponse());
}

int		Webserv::getFd( void )
{
	return (this->fd);
}

int		Webserv::getMaxFd( void )
{
	return (this->_maxFd);
}

fd_set					Webserv::getMasterSet( void )
{
	return (this->_master_fd);
}

struct sockaddr_in		&Webserv::getAddr( void )
{
	return (this->address);
}

std::string				Webserv::getIpAddress( void )
{
	return (this->_IPaddr);
}

std::string				Webserv::getPort( void )
{
	return (this->_port);
}

std::vector<Location>	Webserv::getLocationVector( void )
{
	return (this->_locationVector);
}

std::vector<Location>	Webserv::getLocationExtVector( void )
{
	return (this->_locationExtVector);
}

int						Webserv::getServerNb( void )
{
	return (this->_serverNb);
}

void					Webserv::logWebserv()
{
	std::ostringstream oss;

	oss << "server[" << this->getServerNb() << "] : " ;
	oss << "[fd: " << this->getFd() << "] ";
	oss << "[IP address: " << this->getIpAddress() << "] ";
	oss << "[port: " << this->getPort() << "] ";

	Logger::Write(Logger::DEBUG, WHT, oss.str());

	for (std::vector<Location>::iterator it2 = this->_locationVector.begin(); it2 != this->_locationVector.end(); ++it2)
		it2->logLocation();
	for (std::vector<Location>::iterator it2 = this->_locationExtVector.begin(); it2 != this->_locationExtVector.end(); ++it2)
		it2->logLocation();

	return ;
}