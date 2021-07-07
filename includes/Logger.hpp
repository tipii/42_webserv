/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: memartin <memartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:24:51 by memartin          #+#    #+#             */
/*   Updated: 2021/05/19 13:24:52 by memartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <string>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <cerrno>
# include <cstring>
# include "Utils.hpp"
# include "colors.hpp"

# define DISPLAY_ERRNO 0
# define MAX_LOG 1000

class Logger 
{
public:
	enum Priority
	{
		MORE,
		DEBUG,
	    INFO,
	    ERROR,
		NONE
	};

private:
	Logger();
	Logger(const Logger &rhs);
	Logger &operator=(const Logger &rhs);

	std::ofstream _fileStream;
	Priority _minPriority;
	static const std::string _priorityNames[];
	static Logger Log;

	public:
	static void Start(Priority minPriority);
	static void Stop(void);
	static void Write(Priority priority, std::string color, const std::string &message);
	static void Error(const std::string &message);
	~Logger();
};

#endif