/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:06:34 by sadarnau          #+#    #+#             */
/*   Updated: 2021/05/19 13:06:34 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "Logger.hpp"
# include "Config.hpp"
# include "Location.hpp"
# include "Client.hpp"
# include <unistd.h>		//read function
# include <fcntl.h>
# include <arpa/inet.h>		//inet_addr function
# include <string>
# include <iostream>
# include <cwchar>
# include <cstring>
# include <netinet/in.h>	//sockaddr struct
# include <sys/select.h>	//FD_ZERO

class Request ;

class Webserv
{
private:

	int									fd;
	std::string							_buff;
	struct sockaddr_in					address;
	std::string							_listen;
	std::vector<Location>				_locationVector;
	std::vector<Location>				_locationExtVector;
	fd_set								_master_fd;
	long								_maxFd;
	std::string							_port;
	std::string							_IPaddr;
	int									_serverNb;
	std::map<int, struct sockaddr_in>	_clientMap;

public:

	Webserv( void );						 																		//default constructor
	Webserv( std::string listen, std::vector<Location> locationVector, std::vector<Location> locationExtVector );	//constructor
	Webserv( Webserv const & src);  																				//copy
	~Webserv( void );																								//destructor
	Webserv & operator=( Webserv const & rhs );																		//overload operator =

	int									initialization( int i );
	void								fillAddress( void );
	int									acceptConexion( void );
	int									handleRequest( long socket );
	std::string							getResponse( long socket, Client client );
	void								deleteSocket( long socket );

	int									getFd( void );
	int									getMaxFd( void );
	fd_set								getMasterSet( void );
	struct sockaddr_in					&getAddr( void );
	std::vector<int>					getFdList( void );
	std::string							getIpAddress( void );
	std::string							getPort( void );
	std::string							getListen( void );
	std::vector<Location>				getLocationVector( void );
	std::vector<Location>				getLocationExtVector( void );
	int									getServerNb( void );

	void								logWebserv();
};

#endif
