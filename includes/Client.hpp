/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:06:19 by sadarnau          #+#    #+#             */
/*   Updated: 2021/05/19 13:06:20 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <iostream>
# include <sys/socket.h>
# include "Logger.hpp"

class Client
{
private:

	long		_socket;
	long		_bytesSent;
	long		_bytesToSend;
	int			_servNb;
	std::string	_buff;
	std::string	_response;
	bool		_finishRead;
	bool		_finishWrite;

public:

	Client( void );									//default constructor
	Client( long socket, int servNb );				//constructor
	Client( Client const & src);  					//copy
	~Client( void );								//destructor
	Client & operator=( Client const & rhs );		//overload operator =

	int			myRecv( void );
	int			mySend( void );
	bool		checkWriteState( void );
	bool		checkReadState( void );
	void		deleteBuff( void );
	
	int			getSocket( void );
	int			getServerNb( void );
	bool		getFinishRead( void );
	bool		getFinishWrite( void );
	std::string	getBuffer( void );
	std::string	getResponse( void );
	
	void		setFinishWrite( bool finishWrite );
	void		setFinishRead( bool finishRead );
	void		setBytesToSend( long bytes );
	void		setResponse( std::string response );

};

std::ostream &	operator<<(std::ostream & o, Client & rhs);

#endif
