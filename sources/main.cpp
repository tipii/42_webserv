/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:05:57 by sadarnau          #+#    #+#             */
/*   Updated: 2021/05/19 13:05:58 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Cluster.hpp"
#include <sys/select.h>
#include <list>
#include <csignal>

Cluster cluster;

void							handle_sig(int sig_num)
{
	(void)sig_num;
	Logger::Write(Logger::INFO, RED, "stoping webserv");

	cluster.closeServices();

	exit(1);
}

void							set_sig(void)
{
	signal(SIGINT, handle_sig);
	signal(SIGQUIT, handle_sig);
}

bool	setLoggerMode(std::string mode)
{
	if (mode == "more")
		Logger::Start(Logger::MORE);
	else if (mode == "debug")
		Logger::Start(Logger::DEBUG);
	else if (mode == "info")
		Logger::Start(Logger::INFO);
	else if (mode == "error")
		Logger::Start(Logger::ERROR);
	else if (mode == "none")
		Logger::Start(Logger::NONE);
	else
	{
		std::cerr << mode << " is not a valid mode for --log.\nTry -l [none, error, [info], debug, more]" << std::endl;
		return (false);
	}
	Logger::Write(Logger::INFO, GRN, "Logger in < " + mode + " > mode");

	return (true);
}

void	showUsage(std::string name)
{
    std::cerr << "Usage: " << name << " <option> PATH_TO_CONF\n"
              << "Options:\n"
              << "\t-h,--help\tShow this help message\n"
              << "\t-l,--log MODE\tSpecify the logger mode [none, error, [info], debug, more]"
              << std::endl;

	return ;
}

int main(int argc, char *argv[])
{
    if (argc > 4)
	{
        showUsage(argv[0]);
        return (1);
    }

    std::string conf = "files/default.conf";
    std::string mode = "info";

    for (int i = 1; i < argc; ++i)
	{
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help"))
		{
            showUsage(argv[0]);
            return (0);
        }
		else if ((arg == "-l") || (arg == "--log"))
		{
            if (i + 1 < argc)
                mode = argv[++i];
			else
			{
    		    std::cerr << "--logger option requires one argument." << std::endl << std::endl;
				showUsage(argv[0]);
                return (1);
            }  
        }
		else
            conf = argv[i];
    }

	if(!setLoggerMode(mode))
		return (1);

	Logger::Write(Logger::INFO, YEL, "configuration file : " + conf);

	try
	{
		if (cluster.initialization(conf))
			return (1);

		set_sig();		// handle SIQUIT and SIGINT

		if (cluster.lanchServices())
			return (1);
	}
	catch(const std::exception& e)
	{
		cluster.closeServices();
		return (1);
	}

	return (0);
}
