/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: memartin <memartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:25:00 by memartin          #+#    #+#             */
/*   Updated: 2021/05/19 13:25:01 by memartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <time.h>
# include <sys/time.h>
# include <string>
# include <algorithm>
# include <map>
# include <vector>
# include <iterator>
# include <sstream>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include "Logger.hpp"

class Webserv ;

namespace   Utils
{
	std::string 	            getTimeHMS(void);
	std::string 	            getDate(void);
	void 			            splitStringToVector(std::string line, std::vector<std::string> &split);
	void			            printMap(std::map<std::string, std::string> m, std::string type);
	std::string                 safeUrlJoin(std::string url1, std::string url2);
	std::vector<std::string>	concatToVector(std::string toParse, std::string sep);
	bool						isPathAFile(std::string path);
	size_t						convertClientSizeFromStr(std::string str);
	std::string					getExtension(std::string target);
	std::string 				decode64(std::string &str);
	int							cutLine(unsigned long *it, std::string *line, std::string *src, std::string sep);
	std::string					formatToCgiEnv(std::string toFormat);
    std::string                 getFileContent(std::string pathToFile);
 
	std::string					intToStr(int i);
	std::string					longToStr(long i);
	long						strToLong(std::string str);
	int						    strToInt(std::string str);
	size_t						hexStrtoInt(std::string hex);
	int							checkLastChar(std::string string, std::string needle);
}

namespace	Libft
{
	void						*memset(void *b, int c, size_t len);
	int							compare(const char *X, const char *Y);
	char						*strstr(const char *haystack, const char *needle);
	size_t						strlen(const char *str);
	void						*memcpy(void *dst, const void *src, size_t n);
}

#endif