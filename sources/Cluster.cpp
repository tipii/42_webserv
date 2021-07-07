/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:05:45 by sadarnau          #+#    #+#             */
/*   Updated: 2021/05/19 13:05:49 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

Cluster::Cluster( void )
{
	return ;
}

Cluster::Cluster( Cluster const & src )
{
	*this = src;

	return ;
}

Cluster::~Cluster( void )
{
	return ;
}

Cluster & Cluster::operator=( Cluster const & rhs)
{
    this->_clients = rhs._clients;
    this->_config = rhs._config;
    this->_fdList = rhs._fdList;
    this->_master_fd = rhs._master_fd;
    this->_maxFd = rhs._maxFd;
    this->_nbServ = rhs._nbServ;
    this->_readyClients = rhs._readyClients;
    this->_serverList = rhs._serverList;

	return ( *this );
}

int								Cluster::initialization( std::string fileName )
{
	this->_maxFd = 0;
	this->_config.parseFile(fileName);

	this->_serverList = this->_config.getServerVector();
	this->_nbServ = this->_serverList.size();

	FD_ZERO(&this->_master_fd);										// create a master file descriptor set and initialize it to zero

	for (int i = 0; i < this->_nbServ; i++)
	{
		Logger::Write(Logger::INFO, GRN, "server[" + Utils::intToStr(i) + "] : creation");
		if (this->_serverList[i].initialization(i))
			return (1);
		FD_SET(this->_serverList[i].getFd(), &this->_master_fd);	// adding our first fd socket, the server one.
		if(this->_serverList[i].getFd() > this->_maxFd)
			this->_maxFd = this->_serverList[i].getFd();
	}

	this->logCluster();

	return (0);
}

int								Cluster::lanchServices( void )
{
	fd_set	copyMasterSet;
	fd_set	writingSet;

	while(1)
	{
		// We have to make a copy of the master fd set because select() will change bits
		// of moving fds  (( man 2 select ))
		copyMasterSet = this->_master_fd;
		setWritingSet(&writingSet);

		int	ret = select(this->_maxFd + 1, &copyMasterSet, &writingSet, 0, 0);

		if (ret < 0)
		{
			Logger::Write(Logger::ERROR, RED, "Error : select have deconné");
			throw (std::exception());
		}
		
		for (std::vector<Client>::iterator it = this->_readyClients.begin(); it != this->_readyClients.end(); it++)
		{
			if (FD_ISSET(it->getSocket(), &writingSet))
			{
				Logger::Write(Logger::DEBUG, GRN, "still have " + Utils::intToStr(it->getResponse().size()) + " bytes to send");
				if (!it->mySend())
				{
					if (close(it->getSocket()) < 0)
						return (1);
					FD_CLR(it->getSocket(), &this->_master_fd);
					setReadStatus(it->getSocket());
					this->_readyClients.erase(it);
				}
				else if (it->getFinishWrite())
				{
					Logger::Write(Logger::DEBUG, GRN, "send is done\n");
					setReadStatus(it->getSocket());
					this->_readyClients.erase(it);
				}
			}
			else
			{
				Logger::Write(Logger::ERROR, RED, "server[" + Utils::intToStr(it->getServerNb()) + "] : error with send, fd[" + Utils::intToStr(it->getSocket()) + "]");
				if (close(it->getSocket()) < 0)
					return (1);
				FD_CLR(it->getSocket(), &this->_master_fd);
				setReadStatus(it->getSocket());
				this->_readyClients.erase(it);
			}
			break;
		}

		for(int i = 0; i < this->_nbServ; i++)							// We go throught every server fds to see if we have a new connection
			if (FD_ISSET(this->_serverList[i].getFd(), &copyMasterSet))	// if serv fd changed -> new connection
			{
				long	sock;
				Logger::Write(Logger::INFO, GRN, "server[" + Utils::intToStr(i) + "] : new connection");
				if ((sock = this->_serverList[i].acceptConexion()) < 0)
					return (1);

				FD_SET(sock, &this->_master_fd);						// add the new fd in the master fd set
				if (sock > this->_maxFd)								// check until where we have to select
					this->_maxFd = sock;

				this->_clients.push_back(Client(sock, i));
				break ;													// no need to check any more serv
			}

		for (std::vector<Client>::iterator it = this->_clients.begin() ; it != this->_clients.end() ; it++)
		{
			if (FD_ISSET(it->getSocket(), &copyMasterSet))
			{
				if ( !it->myRecv() )
				{
					if (close(it->getSocket()) < 0)
						return (1);
					FD_CLR(it->getSocket(), &this->_master_fd);
					this->_clients.erase(it);
				}
				else
					if (it->getFinishRead())
					{
						Logger::Write(Logger::DEBUG, GRN, "read is done");
						processClient(*it);
						it->deleteBuff();
					}
				break;
				}
		}
	}

	return (0);
}

void							Cluster::closeServices( void )
{
	if(!this->_readyClients.empty())
		Logger::Write(Logger::INFO, RED, "closing active clients...");
	for (std::vector<Client>::iterator it = this->_readyClients.begin() ; it != this->_readyClients.end() ; it++)
		close(it->getSocket());
	
	if(!this->_clients.empty())
		Logger::Write(Logger::INFO, RED, "closing all clients...");
	for (std::vector<Client>::iterator it = this->_clients.begin() ; it != this->_clients.end() ; it++)
		close(it->getSocket());

	for(unsigned long i = 0; i < this->_serverList.size(); i++)
	{
		Logger::Write(Logger::INFO, RED, "closing server[" + Utils::intToStr(i) + "]...");
		close(this->_serverList[i].getFd());
	}

	return ;
}

void							Cluster::processClient( Client client )
{
	std::string response = this->_serverList[client.getServerNb()].getResponse( client.getSocket(), client );
	Client readyClient = client;
	readyClient.setBytesToSend(response.length());
	readyClient.setResponse(response);

	this->_readyClients.push_back(readyClient);

	return ;
}

void							Cluster::setReadStatus( long socket )
{
	for (std::vector<Client>::iterator it = this->_clients.begin() ; it != this->_clients.end() ; it++)
		if (it->getSocket() == socket)
		{
			it->setFinishRead(false);
			break;
		}

	return ;
}

void							Cluster::setWritingSet( fd_set *writefds )
{
	FD_ZERO(writefds);
	std::vector<Client> list = this->_readyClients;
	for (std::vector<Client>::iterator it = list.begin() ; it != list.end() ; it++)
		FD_SET(it->getSocket(), writefds);

	return ;
}

std::map<std::string, std::string>	Cluster::getMap( void )
{
	return (this->_config.getConfigMap());
}

fd_set								Cluster::getMasterSet( void )
{
	return (this->_master_fd);
}

std::vector<Webserv>				Cluster::getServerList( void )
{
	return (this->_serverList);
}

int									Cluster::getMaxFd( void )
{
	return (this->_maxFd);
}

std::vector<int>					Cluster::getFdList( void )
{
	return (this->_fdList);
}

void								Cluster::logCluster(void)
{
	for (std::vector<Webserv>::iterator it = this->_serverList.begin(); it != this->_serverList.end(); ++it)
		it->logWebserv();
}
