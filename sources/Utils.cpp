/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: memartin <memartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:24:25 by memartin          #+#    #+#             */
/*   Updated: 2021/05/19 13:24:25 by memartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"


namespace Utils {
	std::string 	getTimeHMS(void)
	{
		struct timeval 	tv;
		struct tm 		time;
		struct timezone tz;
		char 			buffer[1000];
		std::string 	date;

		if (gettimeofday(&tv, &tz) < 0)
			return (NULL);	// maybe throw error ?
		std::ostringstream ss;
		ss << tv.tv_sec;
		std::string res = ss.str();
		strptime(res.c_str(), "%s", &time);
		strftime(buffer, sizeof(buffer), "%H:%M:%S", &time);
		date = buffer;

		return (date);
	}

	bool			isPathAFile(std::string path)
	{
		struct stat s;

		if (stat(path.c_str(), &s) == 0 )
		{
			if (s.st_mode & S_IFREG)
				return (true);
		}

		return (false);
	}

	std::string 	getDate(void)
	{
		struct timeval 	tv;
		struct tm 		time;
		struct timezone tz;
		char 			buffer[1000];
		std::string 	date;

		if (gettimeofday(&tv, &tz) < 0)
			return (NULL);	// maybe throw error ?
		strptime(longToStr(tv.tv_sec).c_str(), "%s", &time);
		strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S CEST", &time);
		date = buffer;

		return (date);
	}

	void 			splitStringToVector(std::string line, std::vector<std::string> &split)
	{
		std::string	res;
		size_t		pos1 = 0;
		size_t		pos2 = 0;
		char		*str = new char [line.length() + 1];

		std::strcpy(str, line.c_str());
		str[line.length()] = 0;

		while (str[pos1] != 0 && str[pos2] != 0)
		{
			while ((str[pos1] == ' ' || str[pos1] == '\t') && str[pos1] != 0)
				++pos1;
			if (str[pos1] == 0)
				break ;
			pos2 = pos1 + 1;
			while ((str[pos2] != ' ' && str[pos2] != '\t') && str[pos2] != 0)
				++pos2;
			res = line.substr(pos1, pos2 - pos1);
			split.push_back(res);
			pos1 = pos2;
		}

		delete[] str;

		return ;
	}

	std::string decode64(std::string &str)
	{
		int val = 0;
		int valb = -8;
		unsigned char c;
		std::string res;
		std::vector<int> tab(256, -1);
		std::string::iterator it2 = str.end();

		for (int i = 0; i < 64; i++)
			tab["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i; 
		for (std::string::iterator it = str.begin(); it != it2; it++)
		{
			c = *it;
			if (tab[c] == -1) break;
			val = (val << 6) + tab[c];
			valb += 6;
			if (valb >= 0)
			{
				res.push_back(char((val >> valb) & 0xFF));
				valb -= 8;
			}
		}

		return (res);
	}

	void			printMap(std::map<std::string, std::string> m, std::string type)
	{
		std::ostringstream oss;

		oss << type;
		for (std::map<std::string, std::string>::const_iterator it = m.begin(); it != m.end(); ++it)
		{
			oss << std::setw(20) << std::left << it->first << ":" << it->second << "\r\n";
		}
		oss << '\n' << "----------\n\n";
		Logger::Write(Logger::DEBUG, WHT, oss.str());

		return ;
	}

	std::string	safeUrlJoin(std::string url1, std::string url2)
	{
		if(url1[url1.size() - 1] == '/' )
			url1 = url1.substr(0, url1.size() - 1);

		if(url2[0] == '/')
			url2 = url2.substr(1, url2.size());

		return (std::string(url1 + "/" + url2));
	}

	std::vector<std::string>	concatToVector(std::string toParse, std::string sep)
	{
		std::vector<std::string>	result;
		std::string					token;
		size_t						pos;

		while ((pos = toParse.find(sep)) != std::string::npos)
		{
			token = toParse.substr(0, pos);
			toParse = toParse.substr(pos + sep.size(), toParse.length());
			result.push_back(token);
		}

		result.push_back(toParse);

		return (result);
	}

	bool						isStrNumber(std::string s)
	{
		std::string::const_iterator it = s.begin();

		while (it != s.end() && std::isdigit(*it)) 
			++it;

		return (!s.empty() && it == s.end());
	}

	size_t						convertClientSizeFromStr(std::string str)
	{
		size_t				res;
		size_t				weight = 1;
		std::stringstream 	ss;

		if (str.find("K") != std::string::npos)
		{
			ss << str.substr(0, str.find("K"));
			weight = 1000;
		}
		else if (str.find("M") != std::string::npos)
		{
			ss << str.substr(0, str.find("M"));
			weight = 1000000;
		}
		else if (str.find("G") != std::string::npos)
		{
			ss << str.substr(0, str.find("G"));
			weight = 1000000000;
		}
		else if (!isStrNumber(str))
			return (0);
		else
			ss << str;

		ss >> res;
		res *= weight;

		return (res);
	}



	std::string					getExtension(std::string target)
	{
		if(target.find(".") == std::string::npos)
			return ("");
			
		std::string ext = target.substr(target.find("."), target.length());

		return (ext);
	}

	/**
	* Return a line from source separted by sep. Should be use in a while, it always return true.
	* Check it == std::string::npos to break after last line is returned 
	* 
	* Be careful, src WILL be CHANGE after each call, so be sure to use a copy of src
	*/
	int							cutLine(unsigned long *it, std::string *line, std::string *src, std::string sep)
	{
		*it = src->find(sep);
		*line = src->substr(0, *it);
		*src = src->substr(*it + sep.size(), src->size());
		return(1);
	}

	std::string					formatToCgiEnv(std::string toFormat)
	{
		std::transform(toFormat.begin(), toFormat.end(), toFormat.begin(), ::toupper);
		std::replace(toFormat.begin(), toFormat.end(), '-', '_');
		return (toFormat);
	}

    std::string                 getFileContent(std::string pathToFile)
	{
		std::ifstream 	f(pathToFile.c_str());
		std::stringstream buff;

		if (f.is_open())
		{
			buff << f.rdbuf();
			f.close();
		}
		return (buff.str());
	}

	long						strToLong(std::string str)
	{
		std::stringstream ss; 
		long res;

		ss << str;  
		ss >> res;

		return (res);  
	}

	int						strToInt(std::string str)
	{
		std::stringstream ss; 
		int res;

		ss << str;  
		ss >> res;

		return (res);  
	}

	std::string					longToStr(long i)
	{
		std::stringstream convert;
		convert << i;

		return (convert.str());
	}

	std::string					intToStr(int i)
	{
		std::stringstream convert;
		convert << i;

		return (convert.str());
	}

	size_t						hexStrtoInt(std::string hex)
	{
		size_t 				length;   
		std::stringstream 	ss;

		ss << std::hex << hex;
		ss >> length;

		return (length);
	}

	int							checkLastChar(std::string string, std::string needle)
	{
		unsigned long	i = string.size();
		int				j = needle.size();

		while (j > 0)
		{
			i--;
			j--;
			if (i < 0 || string[i] != needle[j])
				return (1);
		}
		return (0);
	}
}

namespace	Libft
{
	void		*memset(void *b, int c, size_t len)
	{
		unsigned char *dest;

		dest = (unsigned char *)b;
		while (len > 0)
		{
			*dest = c;
			dest++;
			len--;
		}
		return (b);
	}

	int compare(const char *a, const char *b)
	{
		while (*a && *b)
		{
			if (*a != *b) {
				return 0;
			}
	
			a++;
			b++;
		}
	
		return (*b == '\0');
	}
 
	char* strstr(const char* haystack, const char* needle)
	{
		while (*haystack != '\0')
		{
			if ((*haystack == *needle) && compare(haystack, needle)) {
				return (char *)haystack;
			}
			haystack++;
		}
	
		return NULL;
	}

	size_t		strlen(const char *str)
	{
		size_t i;

		i = 0;
		while (str[i])
			i++;
		return (i);
	}

	void	*memcpy(void *dst, const void *src, size_t n)
	{
		char *csrc = (char *)src;
		char *cdest = (char *)dst;
	
		for (size_t i = 0; i < n; i++)
			cdest[i] = csrc[i];
		
		return (dst);
	}
}