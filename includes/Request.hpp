/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpalhol <tpalhol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:07:54 by tpalhol           #+#    #+#             */
/*   Updated: 2021/05/19 13:07:55 by tpalhol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <unistd.h>
# include <stddef.h>
# include <string>
# include <iostream>
# include <map>
# include <vector>
# include <sstream>
# include <colors.hpp>
# include <iomanip>
# include "Location.hpp"
# include "Logger.hpp"
# include "Utils.hpp"

class Location ;

typedef std::vector< Location > vlocation;

class Request
{
private:
	vlocation							*_locationVector;
	vlocation							*_locationExtVector;
	int									_inSocket;
	std::string							_buff;
	std::string							_ip;

	long								_contentLength;
	std::string							_method;
	std::string							_target;
	std::string							_httpVersion;
	std::string							_body;
	std::string							_urlTargetPath;
	std::string							_absoluteTargetPath;
	std::map<std::string, std::string>	_headers;
	std::string							_queryString;
	Location							_selectedLocation;
	Location							_selectedLocationExt;
	bool								_badRequest;

	
	void								_parseRequest(std::string req);
	std::string							_unchunkBody(std::string body);
	void								_parseUrl(void);
	void								_selectLocation(void);
	bool								_selectLocationExt(void);
	void								_mergeLocation(void);
	void								_createPath(void);

public:
	Request(vlocation *_locationVector, vlocation *_locationExtVector, int inSock, std::string buff, std::string ip);
	Request(Request const & src);
	~Request(void);
	Request & operator=(Request const & rhs);

	void								updateTarget(std::string target);
	int									getInSock(void);
	std::string							getMethod(void);
	std::string							getBody(void);
	std::string							getTarget(void);
	std::string							getUrlTargetPath(void);
	std::string							getAbsoluteTargetPath(void);
	std::string							getQueryString(void);
	std::string							getAuthorization(void);
	std::map<std::string, std::string>	getHeaders(void);
	Location							getSelectedLocation(void);
	size_t								getContentLength(void);
	bool								getBadRequest(void);
	std::string							getHttpVersion(void);
	std::string							getIP(void);

	void								logRequest(int serverNb);
};

#endif
