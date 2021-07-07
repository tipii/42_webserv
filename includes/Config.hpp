/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: memartin <memartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:24:44 by memartin          #+#    #+#             */
/*   Updated: 2021/05/19 13:24:45 by memartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <fstream>
# include <sstream>
# include <map>
# include <iostream>
# include <string>
# include "Logger.hpp"
# include "Location.hpp"
# include "Webserv.hpp"

class Request ;
class Webserv ;

class Config
{
private:

	std::ifstream						f;
	std::string							_listen;
	std::vector<std::string>			_allListen;
	std::map<std::string, std::string>	_configMap;
	std::vector<Location> 				_locationVector;
	std::vector<Location> 				_locationExtVector;
	std::vector<Webserv>				_serverVector;
	int									_lineIt;

	void								initConfigMap(void);
	void								initLocationMap(std::map<std::string, std::string> & newLoc, std::string path);
	void								addConfigToLocation(std::map<std::string, std::string> newLoc, bool isExtension);
	void								checkFile(std::string fileName);
	size_t								checkSemiColon(std::string str);
	bool								checkMultipleListen(std::string listen);
	void								createServerMap(void);
	void								newLocationConfig(std::string path);
	void								throwConfigError(std::string str);
	bool								checkBadLineConf(std::string str);

public:

	Config(void);
	Config(Config const & src);
	~Config(void);
	Config & operator=(Config const & rhs);

	void								parseFile(std::string fileName);
	std::map<std::string, std::string>	getConfigMap(void);
	std::vector<Location>				getLocationVector(void);
	std::vector<Webserv> 				getServerVector(void);
};

#endif
