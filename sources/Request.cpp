/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpalhol <tpalhol@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/19 13:07:06 by tpalhol           #+#    #+#             */
/*   Updated: 2021/05/19 13:07:08 by tpalhol          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

////////////////////
// COPLIEN'S
////////////////////
Request::Request(vlocation *locationVector, vlocation *locationExtVector, int inSock, std::string buff, std::string ip) // commentaire a enlever ?
{
	this->_ip = ip;
	this->_inSocket = inSock;
	this->_locationVector = locationVector;
	this->_locationExtVector = locationExtVector;
	this->_buff = buff;
	this->_contentLength = 0;
	this->_badRequest = false;

	this->_parseRequest(this->_buff);

	this->_selectLocation();
	if(this->_selectLocationExt())
		this->_mergeLocation();
		
	this->_selectedLocation.logLocation();
	this->_parseUrl();
	this->_createPath();

	return ;
}

Request::Request(Request const & src)
{
	*this = src;

	return ;
}

Request::~Request(void)
{
	return ;
}

Request & Request::operator=(Request const & rhs)
{
    this->_buff = rhs._buff;
	this->_inSocket = rhs._inSocket;

	return (*this);
}


////////////////////
// PARSE
////////////////////
void	Request::_parseRequest(std::string req)
{
	std::string			header;
	std::string			body;

	if (req.find("\r\n\r\n") != std::string::npos)
	{
		header = req.substr(0, req.find("\r\n\r\n"));
		body = req.substr(req.find("\r\n\r\n") + 4, req.length());
	}
	else
	{
		header = req;
		body = "";
	}

	// parse header
	std::string					line;
    std::string					key;
    std::string					value;
    std::string					httpVersion;
	std::vector<std::string>	tmpVector;

	unsigned long it;
	Utils::cutLine(&it, &line, &header, "\r\n");
	tmpVector = Utils::concatToVector(line, " ");
	if (tmpVector.size() != 3)
	{
		this->_badRequest = true;
		Logger::Write(Logger::ERROR, RED, "request : bad request : request line : " + line);
	}

	this->_method = tmpVector[0];
	this->_target = tmpVector[1];
	this->_httpVersion = tmpVector[2];

	// handle request header
	while (Utils::cutLine(&it, &line, &header, "\r\n"))
    {
		if (line.find(":") == std::string::npos)
		{
			this->_badRequest = true;
			Logger::Write(Logger::ERROR, RED, "request : bad request : header line : " + line);
			break ;
		}
		key = line.substr(0, line.find(":"));
		value = line.substr(line.find(":") + 2, line.size());
		if (this->_headers.count(key))
		{
			this->_badRequest = true;
			break ;
		}
		this->_headers[key] = value;

		if (it == std::string::npos)
			break ;
	}

	if (this->_headers["Transfer-Encoding"] == "chunked")
		this->_body = this->_unchunkBody(body);
	else
		this->_body = body;

	unsigned long  i;
	for (i = 0; i < this->_body.size(); i++);

	this->_contentLength = i;
	return ;
}

////////////////////
// UTILS
////////////////////
std::string		Request::_unchunkBody(std::string body)
{
	int			chunkLength;
	char		*cbody = (char *)body.c_str();
	char		*bodyptr = cbody;
	char		*result;
	if(!(result = (char *)malloc(sizeof(char) * body.size())))
		throw (std::exception());
	char		*saveresult = result;
	char		hexStr[100];
	char		*itptr;
	ptrdiff_t	hexLen;

	Libft::memset(result, 0, sizeof(char) * body.size() - 1);
	Libft::memset(hexStr, 0, 99);

	itptr = Libft::strstr(bodyptr, "\r\n");
	hexLen = itptr - bodyptr;
	Libft::memcpy(hexStr, bodyptr, hexLen);
	chunkLength = Utils::hexStrtoInt(hexStr);

	while (chunkLength)
	{
		bodyptr += hexLen + 2;
		Libft::memcpy(result, bodyptr, chunkLength);

		result = result + chunkLength;
		bodyptr += chunkLength + 2;

		itptr = Libft::strstr(bodyptr, "\r\n");
		hexLen = itptr - bodyptr;
		Libft::memset(hexStr, 0, 99);
		Libft::memcpy(hexStr, bodyptr, hexLen);

		chunkLength = Utils::hexStrtoInt(hexStr);
	}

	std::string tmpBody = saveresult;
	free(saveresult);
	return (tmpBody);
}

void	Request::_selectLocation(void)
{
	// iter through locations
	for (vlocation::iterator it = this->_locationVector->begin() ; it != this->_locationVector->end(); ++it)
    {
		Location tmpLoc(*it);
				
		// compare location["path"] to target on lacation["path"] length
		if (std::strncmp(tmpLoc.getPath().c_str(), this->_target.c_str(), tmpLoc.getPath().size()) == 0) 
		{
			// check if its a valid route (for now, if its a correct folder), "/" is always valid
			// relevant if this->target = "/weeeee" and loc["path"] = "/we" for example (/we is not a good loc)
			if (tmpLoc.getPath() == "/" || this->_target[tmpLoc.getPath().length()] == '\0' || this->_target[tmpLoc.getPath().length()] == '/')
			{
				//if no location, add anyone, if theres something, check if the new match on more char than the previous one
				if(!this->_selectedLocation.isSet())
					this->_selectedLocation = tmpLoc;
				else
					if(this->_selectedLocation.getPath().size() < tmpLoc.getPath().size())
						this->_selectedLocation = tmpLoc;
			}
		}
	}
	this->_urlTargetPath = this->_target;

	// delete location in target (ex: if location is /salut and target /salut/index.html, target become /index.html) - not in case of root loc /
	if (this->_selectedLocation.getPath() != "/")
		this->_target = this->_target.substr(this->_selectedLocation.getPath().size(), this->_target.size());

	// format target
	if(this->_target[0] != '/')
		this->_target.insert(0, "/");

	return ;
}

bool	Request::_selectLocationExt()
{
	std::string targetExt = Utils::getExtension(this->_target);

	for (vlocation::iterator it = this->_locationExtVector->begin(); it != this->_locationExtVector->end(); ++it)
	{
		if (it->getPath() == targetExt)
		{
			this->_selectedLocationExt = *it;
			return (true);
		}
	}

	return (false);
}

void	Request::_mergeLocation(void)
{
	std::map<std::string, std::string>	locationSetting = this->_selectedLocation.getSettingMap();
	std::map<std::string, std::string>	locationExtSetting = this->_selectedLocationExt.getSettingMap();

	for (std::map<std::string, std::string>::const_iterator it = locationExtSetting.begin(); it != locationExtSetting.end(); ++it)
	{
		if (it->first != "path" && it->second != "")
		{
			if (!it->first.compare("accepted_method"))
			{
				if (locationSetting["accepted_method"].size())
				{
					if (it->second.size())
					{
						locationSetting["accepted_method"] += "/";
						locationSetting["accepted_method"] += it->second;
					}
					else 
						locationSetting["accepted_method"] = "";
				}
			}
			else
				locationSetting[it->first] = it->second;
		}
	}
	
	this->_selectedLocation = Location(locationSetting);

	return ;
}

void	Request::_parseUrl(void)
{
	int i;

	//Search for query
	if((i = this->_target.find("?")) != -1)
	{
		//query separator is found
		this->_queryString = this->_target.substr(i + 1, this->_target.size() - 2); // i + 1 to skip &, so size - (1 + '&')
		this->_target = this->_target.substr(0, i);
		this->_urlTargetPath = this->_urlTargetPath.substr(0, this->_urlTargetPath.find("?"));
	}

	return ;
}

void			Request::updateTarget(std::string target)
{
	this->_target = target;
	this->_createPath();

	return ;
}

void	Request::_createPath(void)
{
	//Create absolute path
	if (this->_selectedLocation.getRoot()[0] == '/')
		this->_absoluteTargetPath = Utils::safeUrlJoin(this->_selectedLocation.getRoot(), this->_target);
	else
	{
		char cwd[1000];
		if(getcwd(cwd, sizeof(cwd)) == NULL)
			Logger::Write(Logger::ERROR, RED, "error : getcwd");
		std::string currentdir = cwd;
		this->_absoluteTargetPath = Utils::safeUrlJoin(currentdir, this->_selectedLocation.getRoot());
		this->_absoluteTargetPath = Utils::safeUrlJoin(this->_absoluteTargetPath, this->_target);
	}

	return ;
}

////////////////////
// GETTERS / SETTERS
////////////////////
int		Request::getInSock(void)
{
	return (this->_inSocket);
}

std::string		Request::getMethod(void)
{
	return (this->_method);
}

std::string		Request::getBody(void)
{
	return (this->_body);
}

std::string		Request::getTarget(void)
{
	return (this->_target);
}

std::string		Request::getUrlTargetPath(void)
{
	return (this->_urlTargetPath);
}

std::string		Request::getAbsoluteTargetPath(void)
{
	return (this->_absoluteTargetPath);
}

std::string		Request::getQueryString(void)
{
	return (this->_queryString);
}

Location		Request::getSelectedLocation(void)
{
	return (this->_selectedLocation);
}

std::map<std::string, std::string>	Request::getHeaders(void)
{
	return(this->_headers);
}

size_t			Request::getContentLength(void)
{
	return (this->_contentLength);
}

bool			Request::getBadRequest(void)
{
	return(this->_badRequest);
}

std::string			Request::getHttpVersion(void)
{
	return(this->_httpVersion);
}

std::string			Request::getIP(void)
{
	return (this->_ip);
}

////////////////////
// LOG
////////////////////
void	Request::logRequest(int serverNbr)
{
	Logger::Write(Logger::INFO, BLU, "server[" + Utils::intToStr(serverNbr) +
		"] : request received [method: " + this->getMethod() + "] [location: " +
		this->getSelectedLocation().getPath() + "] [target: " + this->getTarget() + "]");
	
	std::ostringstream oss;
	
	oss << "request :\n\n" ;
	oss << std::setw(30) << "request->method" << " : " << this->_method << std::endl;
	oss << std::setw(30) << "request->target" << " : " << this->_target << std::endl;
	oss << std::setw(30) << "request->query" << " : " << this->_queryString << std::endl;
	oss << std::setw(30) << "request->urlTargetPath" << " : " << this->_urlTargetPath << std::endl;
	oss << std::setw(30) << "request->absoluteTargetPath" << " : " << this->_absoluteTargetPath << std::endl;
	oss << std::setw(30) << "selectedLocation.getPath()" << " : " << this->_selectedLocation.getPath() << std::endl << std::endl;
	oss << "Content of request->headers :" << std::endl << std::endl; 
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	{
		oss << std::setw(20) << it->first << " : " << it->second  << std::endl;
	}
	Logger::Write(Logger::DEBUG, WHT, oss.str());

	std::ostringstream oss2;

	// RAW REQUEST
	oss2 << "Raw request (MAX_LOG " + Utils::intToStr(MAX_LOG) + "):" << std::endl << std::endl;
	oss2 << this->_buff.substr(0, MAX_LOG);
	oss2 << "\n";
	oss2 << RESET;
	Logger::Write(Logger::MORE, BLU, oss2.str());

	return ;
}