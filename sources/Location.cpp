/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: memartin <memartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:24:34 by memartin          #+#    #+#             */
/*   Updated: 2021/05/19 13:24:34 by memartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(void)
{
	this->_isSet = false;
}

Location::Location(std::map<std::string, std::string> locationMap) : _listen(""), _path(""), _serverName(""), _clientMaxBodySizeStr("1M"), _clientMaxBodySize(1000000), _root(""), _cgiPath(""),  _cgiExt(""), _autoindex(""), _authentication(""), _settingMap(locationMap)
{
	this->_isSet = true;
	this->_index.clear();
	this->_acceptedMethod.clear();
	this->_errorPage.clear();
	for (std::map<std::string, std::string>::const_iterator it = locationMap.begin(); it != locationMap.end(); ++it)
	{
		if (!it->first.compare("listen"))
			this->_listen = it->second;
		else if (!it->first.compare("path"))
			this->_path = it->second;
		else if (!it->first.compare("server_name"))
			this->_serverName = it->second;
		else if (!it->first.compare("client_max_body_size"))
		{
			if (it->second.size())
				this->_clientMaxBodySizeStr = it->second;
			this->_clientMaxBodySize = Utils::convertClientSizeFromStr(this->_clientMaxBodySizeStr);
		}
		else if (!it->first.compare("root"))
			this->_root = it->second;
		else if (!it->first.compare("autoindex"))
			this->_autoindex = it->second;
		else if (!it->first.compare("authentication"))
			this->_authentication = it->second;
		else if (!it->first.compare("cgi_path"))
			this->_cgiPath = it->second;
		else if (!it->first.compare("cgi_ext"))
			this->_cgiExt = it->second;
		else if (!it->first.compare("index"))
			this->_index = Utils::concatToVector(it->second, "/");
		else if (!it->first.compare("accepted_method"))
		{
			if (it->second.size())
				this->_acceptedMethod = Utils::concatToVector(it->second, "/");
		}
		else
			this->_errorPage[it->first] = it->second;
	}

	return ;
}

Location::Location(const Location &rhs)
{
	*this = rhs;

	return ;
}

Location::~Location(void)
{
	return ;
}

Location &Location::operator=(const Location &rhs)
{
	this->_listen = rhs._listen;
	this->_path = rhs._path;
	this->_serverName = rhs._serverName;
	this->_clientMaxBodySize = rhs._clientMaxBodySize;
	this->_clientMaxBodySizeStr = rhs._clientMaxBodySizeStr;
	this->_root = rhs._root;
	this->_cgiPath = rhs._cgiPath;
	this->_cgiExt = rhs._cgiExt;
	this->_autoindex = rhs._autoindex;
	this->_authentication = rhs._authentication;
	this->_index = rhs._index;
	this->_acceptedMethod = rhs._acceptedMethod;
	this->_errorPage = rhs._errorPage;
	this->_settingMap = rhs._settingMap;

	return (*this);
}

std::string		Location::getListen(void)
{
	return (this->_listen);
}

std::string		Location::getPath(void)
{
	return (this->_path);
}

std::string		Location::getServerName(void)
{
	return (this->_serverName);
}

size_t			Location::getClientMaxBodySize(void)
{
	return (this->_clientMaxBodySize);
}

std::string		Location::getClientMaxBodySizeStr(void)
{
	return (this->_clientMaxBodySizeStr);
}

std::string		Location::getRoot(void)
{
	return (this->_root);
}

std::string		Location::getCgiPath(void)
{
	return (this->_cgiPath);
}

std::string		Location::getCgiExt(void)
{
	return (this->_cgiExt);
}

std::string		Location::getAutoindex(void)
{
	return (this->_autoindex);
}

std::string		Location::getAuthentication(void)
{
	return (this->_authentication);
}

std::vector<std::string>			Location::getIndex(void)
{
	return (this->_index);
}

std::vector<std::string>			Location::getAcceptedMethod(void)
{
	return (this->_acceptedMethod);
}

std::map<std::string, std::string>	Location::getErrorPage(void)
{
	return (this->_errorPage);
}

std::map<std::string, std::string>	Location::getSettingMap(void)
{
	return (this->_settingMap);
}


bool								Location::isSet(void)
{
	return (this->_isSet);
}

void								Location::logLocation(void)
{
	std::ostringstream 					oss;
	std::vector<std::string>			vec;
	std::map<std::string, std::string>	mapstr;

	oss << "location ";
	oss << "[path: " << this->_path << "] : ";
	oss << "[listen: " << this->_listen << "]";
	oss << "[server_name: " << this->_serverName << "]";
	oss << "[client_max_body_size: " << this->_clientMaxBodySizeStr << "]";
	oss << "[root: " << this->_root << "]";
	oss << "[cgi_path: " << this->_cgiPath << "]";
	oss << "[cgi_ext: " << this->_cgiExt << "]";
	oss << "[autoindex: " << this->_autoindex << "]";
	oss << "[authentication: " << this->_authentication << "]";
	oss << "[index: ";

	vec = this->getIndex();
	if (!vec.empty())
	{
		for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it)
			oss << *it << " ";
	}

	oss << "]";
	oss << "[accepted_method: ";
	vec = this->getAcceptedMethod();

	if (!vec.empty())
		for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it)
			oss << *it << " ";
	oss << "]";

	mapstr = this->getErrorPage();

	oss << "[error_pages :";
	if (!mapstr.empty())
		for (std::map<std::string, std::string>::const_iterator it = mapstr.begin(); it != mapstr.end(); ++it)
			oss << it->first << " " << it->second << " ";
	oss << "]";

	Logger::Write(Logger::DEBUG, WHT, oss.str());

	return ;
}