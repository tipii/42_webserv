/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: memartin <memartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:24:47 by memartin          #+#    #+#             */
/*   Updated: 2021/05/19 13:24:49 by memartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include "Utils.hpp"

class Location
{
	private:
		bool								_isSet;
		std::string							_listen;
		std::string							_path;
		std::string							_serverName;
		std::string							_clientMaxBodySizeStr;
		size_t								_clientMaxBodySize;
		std::string							_root;
		std::string							_cgiPath;
		std::string							_cgiExt;
		std::string							_autoindex;
		std::string							_authentication;
		std::vector<std::string>			_index;
		std::vector<std::string>			_acceptedMethod;
		std::map<std::string, std::string>	_errorPage;
		std::map<std::string, std::string>	_settingMap;

	public:
		Location(void);
		Location(std::map<std::string, std::string> locationMap);
		Location(const Location &rhs);
		~Location(void);
		Location &operator=(const Location &rhs);

		std::string							getListen(void);
		std::string							getPath(void);
		std::string							getServerName(void);
		size_t								getClientMaxBodySize(void);
		std::string							getClientMaxBodySizeStr(void);
		std::string							getRoot(void);
		std::string							getCgiPath(void);
		std::string							getCgiExt(void);
		std::string							getAutoindex(void);
		std::string							getAuthentication(void);
		std::vector<std::string>			getIndex(void);
		std::vector<std::string>			getAcceptedMethod(void);
		std::map<std::string, std::string>	getErrorPage(void);
		std::map<std::string, std::string>	getSettingMap(void);
		bool								isSet(void);

		void								logLocation(void);
};

#endif