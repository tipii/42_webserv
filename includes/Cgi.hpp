/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpalhol <tpalhol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:07:41 by tpalhol           #+#    #+#             */
/*   Updated: 2021/05/19 13:07:43 by tpalhol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <map>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include "Request.hpp"
# include "Response.hpp"

class Request ;
class Response ;

class Cgi
{
	private:
		Request								*_req;
		Response							*_res;
		std::map<std::string, std::string>	_env;
		char								**_envC;
		char								**_argC;
		std::string							_result;
		std::map<std::string, std::string>	_headers;
		
		void								_initEnv(void);
		void								_initArgC(void);
		char								**_envToCArray(void);
		void								_closeFd(FILE *fIn, FILE *fOut, int fdIn, int fdOut);


	public:
		Cgi(Request *req, Response *res);
		Cgi(Cgi const & src);
		~Cgi(void);
		Cgi & operator=(Cgi const & rhs);	

		bool								processCgi(void);
		std::string							getResult(void);
		void								logCgi(void);
		void								parseResponse(void);
		std::map<std::string, std::string>	getCgiHeaders(void);
};

#endif