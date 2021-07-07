/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:06:26 by sadarnau          #+#    #+#             */
/*   Updated: 2021/05/19 13:06:27 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include "Client.hpp"
# include "Config.hpp"
# include "Webserv.hpp"
# include <signal.h>
# include <map>
# include <sys/select.h>

class Cluster
{
private:

	long					_maxFd;
	fd_set					_master_fd;
	std::vector<Webserv>	_serverList;
	std::vector<int>		_fdList;
	std::vector<Client>		_clients;
	std::vector<Client>		_readyClients;
	Config					_config;
	int						_nbServ;

public:

	Cluster( void );								//default constructor
	Cluster( Cluster const & src);  				//copy
	~Cluster( void );								//destructor
	Cluster & operator=( Cluster const & rhs );		//overload operator =

	int									initialization( std::string fileName );
	int									lanchServices( void );
	void								closeServices( void );

	void								processClient( Client client );
	void								setReadStatus( long socket );
	void								setWritingSet( fd_set *writefds );

	std::map<std::string, std::string>	getMap( void );
	fd_set								getMasterSet( void );
	std::vector<Webserv>				getServerList( void );
	int									getMaxFd( void );
	std::vector<int>					getFdList( void );

	void								logCluster();
};

#endif
