/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: memartin <memartin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:24:18 by memartin          #+#    #+#             */
/*   Updated: 2021/05/19 13:24:18 by memartin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

Logger Logger::Log;

Logger::Logger(void)
{
}

Logger::~Logger(void)
{
	Log.Stop();
}

const std::string Logger::_priorityNames[] = { "MORE ", "DEBUG", "INFO ", "ERROR", "NONE" };

void Logger::Start(Priority minPriority)
{
	Log._minPriority = minPriority;
	Log._fileStream.open("./log/log.txt", std::ofstream::out);
	Log._fileStream << "--------LOG----------\n";

	if (Log._fileStream.is_open())
        Log._fileStream.close();

	return ;
}

void Logger::Write(Priority priority, std::string color, const std::string &message)
{
	if (priority >= Log._minPriority) {
		Log._fileStream.open("./log/log.txt", std::ofstream::app);

		std::string col(color);
		std::ostringstream oss;
		
		if (priority == DEBUG)
			oss << BLU;
		else if (priority == INFO)
			oss << YEL;
		else if (priority == ERROR)
			oss << RED;
		oss << _priorityNames[priority] << "[" << Utils::getTimeHMS() << "]: " << col << message << RESET ;
		oss << std::endl;

		std::ostream& stream1 = Log._fileStream;
		stream1 << oss.str();
		std::cout << oss.str();
		if (Log._fileStream.is_open())
			Log._fileStream.close();
	}

	return ;
}

void Logger::Error(const std::string &message)
{
	if (DISPLAY_ERRNO && errno != 0)
		Logger::Write(Logger::ERROR, RED, message + "Errno -> (" + std::string(strerror(errno)));
	else
		Logger::Write(Logger::ERROR, RED, message);

	return ;
}

void Logger::Stop(void)
{
    if (Log._fileStream.is_open())
        Log._fileStream.close();

	return ;
}