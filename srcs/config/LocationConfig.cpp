#include "../../include/config/ServerConfig.hpp"
#include "../../include/config/LocationConfig.hpp"
#include "../../include/config/Config.hpp"

// |----------------------
// | HELPER FUNCTIONS
// |----------------------

// |----------------------
// | MEMBER FUNCTIONS
// |----------------------

void	LocationConfig::parse_location(std::istream& location_file, std::string line)
{
	// Set name
	unsigned int range = line.size();
	if (line.at(range - 1) == '{')
	{
		range--;
	}
	this->setName(formatFakePath(trim_whitespace(line.substr(8, range - 8))));

	if (this->getName() == "")
	{
		throw Config::BadConfigException("Empty field (location name): ", line);
	}
	else if (this->getName().find("//") != std::string::npos)
	{
		throw Config::BadConfigException("Bad syntax (location name): ", line);
	}
	else if (this->getName().find(".(") != std::string::npos && this->getName().find("|") != std::string::npos)
	{
		throw Config::BadConfigException("Syntax not supported by this project: ", line);
	}

	// Printing loop, once per line in Configuration file
	while (getline(location_file, line))
	{
		line = trim_whitespace(line);
		// End function if Location segment ends
		if (line == "}")
		{
			break ;
		}

		if (line.compare(0, 8, "location") == 0)
		{
			this->setSubLocation(new LocationConfig(location_file, line));
			if (!this->checkSubLocation())
			{
				throw Config::BadConfigException("Input error (sub-location): ", line);
			}
		}
		else if (line.compare(0, 4, "root") == 0)
		{
			this->setRoot(trim_whitespace(line.substr(4)));
			if (this->getRoot() == "")
			{
				throw Config::BadConfigException("Empty field (root): ", line);
			}
			else if (this->getRoot().find("//") != std::string::npos)
			{
				throw Config::BadConfigException("Bad syntax (root): ", line);
			}
		}
		else if (line.compare(0, 5, "index") == 0)
		{
			this->setIndex(trim_whitespace(line.substr(5)));
			if (this->getIndex() == "")
			{
				throw Config::BadConfigException("Empty field (index): ", line);
			}
		}
		else if (line.compare(0, 8, "cgi_pass") == 0)
		{
			this->setPass(trim_whitespace(line.substr(9)));
			if (this->getPass() == "")
			{
				throw Config::BadConfigException("Empty field (cgi_pass): ", line);
			}
			else if (this->getPass().find("//") != std::string::npos)
			{
				throw Config::BadConfigException("Bad syntax (cgi_pass): ", line);
			}
		}
		else if (line.compare(0, 13, "allow_methods") == 0)
		{
			this->setMethods(trim_whitespace(line.substr(13)));
			if (this->_methods.empty())
			{
				throw Config::BadConfigException("Empty field (allow_methods): ", line);
			}
		}
		else if (line.compare(0, 23, "client_body_buffer_size") == 0)
		{
			this->setClientBuffSize(trim_whitespace(line.substr(23)));
		}
		else if (line.compare(0, 5, "alias") == 0)
		{
			std::cout << "Warning: Alias is not supported by this project." << std::endl;
			//throw Config::BadConfigException("Alias is not supported by this project", "");
			//this->setAlias(true);
		}
	}

	// If sub-location does not contain methods, inherit from this main location
	if (!this->getMethods().empty())
	{
		for (unsigned int i = 0; i < this->getSubLocationMap().size(); i++)
		{
			if (this->getSubLocation(i).getMethods().empty())
			{
				this->getSubLocation(i).copyMethods(this->getMethods());
			}
		}
	}
}

// |----------------------
// | GETTERS & SETTERS
// |----------------------

bool	LocationConfig::hasMethod(t_methods method) const
{
	for (unsigned int i = 0; i < this->_methods.size(); i++)
	{
		if (this->_methods[i] == method)
		{
			return (true);
		}
	}
	return (false);
}

void	LocationConfig::copyMethods(std::vector<t_methods> const &orig)
{
	for (std::vector<t_methods>::const_iterator it = orig.begin();
			it != orig.end(); it++)
	{
		this->_methods.push_back(*it);
	}
}

void	LocationConfig::setMethods(std::string const str)
{
	for(unsigned int i = 0; i < str.size() ; i++)
	{
		if (!isDelim(str[i]))
		{
			size_t k = 0;
			while (i + k < str.size() && !isDelim(str[i + k]))
				k++;
			this->setOneMethod(str.substr(i, k));
			i += k - 1;
		}
	}
}

void	LocationConfig::setOneMethod(std::string word)
{
	const std::string method_name[] = 
	{
		"GET",
		"HEAD",
		"POST",
		"PUT",
		"DELETE",
		"OPTIONS",
		"PATCH",
		"TRACE",
		"CONNECT"
	};

	unsigned int method_num = sizeof(method_name) / sizeof(method_name[0]);
	for (unsigned int i = 0; i < method_num ; i++)
	{
		if (capitalize(word) == method_name[i] && !this->hasMethod(static_cast<t_methods>(i)))
		{
			this->_methods.push_back(static_cast<t_methods>(i));
			return ;
		}
	}
	throw Config::BadConfigException("Invalid method: ", capitalize(word));
}

void	LocationConfig::setIndex(std::string index)
{
	this->_index = index;
}

void	LocationConfig::setRoot(std::string root)
{
	validatePath(root, false);
	this->_root = root;
}

void	LocationConfig::setName(std::string name)
{
	this->_name = name;
}

void	LocationConfig::setPass(std::string pass)
{
	validatePath(pass, true);
	this->_cgi_pass = pass;
}

void	LocationConfig::setClientBuffSize(std::string buff_size)
{
	char *safeguard;
	this->_client_body_buffer_size = std::strtoul(buff_size.c_str(), &safeguard, 10);
	if (*safeguard != '\0')
	{
		throw Config::BadConfigException("Invalid client_body_buffer_size", "");
	}
}

/*void	LocationConfig::setAlias(bool alias)
{
	this->_alias = alias;
}*/

void	LocationConfig::setSubLocation(LocationConfig* loc) 
{
	if (loc)
	{
		// TODO Esclarecer se este troço é necessário
		/*if (this->getName() == "/" && loc->getName()[0] == '/')
			;
		else if (this->getName() == "/" && loc->getName()[0] != '/')
			loc->setName(this->getName() + loc->getName());
		else if*/
		if (loc->getName()[0] == '/')
			loc->setName(this->getName() + loc->getName());
		else
			loc->setName(this->getName() + "/" + loc->getName());
		this->_sub_locations[loc->getName()] = *loc;
		delete (loc);
	}
}

std::string const	&LocationConfig::getName(void) const
{
	return(this->_name);
}

std::string const	&LocationConfig::getRoot(void) const
{
	return(this->_root);
}

std::string const	&LocationConfig::getIndex(void) const
{
	return(this->_index);
}

std::string const	&LocationConfig::getPass(void) const
{
	return(this->_cgi_pass);
}

unsigned long	LocationConfig::getClientBuffSize(void) const
{
	return(this->_client_body_buffer_size);
}

/*bool const	&LocationConfig::getAlias(void) const
{
	return(this->_alias);
}*/

std::vector<t_methods> const	&LocationConfig::getMethods() const
{
	return (this->_methods);
}

bool	LocationConfig::checkSubLocation(void) const
{
	if (this->_sub_locations.empty())
	{
		return(false);
	}
	return(true);
}

std::map<std::string, LocationConfig>	&LocationConfig::getSubLocationMap(void)
{
	return(this->_sub_locations);
}

std::map<std::string, LocationConfig> const	&LocationConfig::getSubLocationMap(void) const
{
	return(this->_sub_locations);
}

LocationConfig	&LocationConfig::getSubLocation(unsigned int num)
{
	if (num >= this->_sub_locations.size())
	{
		std::ostringstream oss;
		oss << (_sub_locations.size() - 1);
		throw Config::BadConfigException("Out of bounds: Sub-Locations only go to ", oss.str());
	}

	std::map<std::string, LocationConfig>::iterator it = this->_sub_locations.begin();
	unsigned int i = 0;
	while (i < num)
	{
		i++;
		it++;
	}

	return(it->second);
}

LocationConfig const	&LocationConfig::getSubLocation(unsigned int num) const
{
	if (num >= this->_sub_locations.size())
	{
		std::ostringstream oss;
		oss << (_sub_locations.size() - 1);
		throw Config::BadConfigException("Out of bounds: Sub-Locations only go to ", oss.str());
	}

	std::map<std::string, LocationConfig>::const_iterator it = this->_sub_locations.begin();
	unsigned int i = 0;
	while (i < num)
	{
		i++;
		it++;
	}

	return(it->second);
}

LocationConfig*	LocationConfig::clone(void) const
{
	//std::cout << "LocationConfig was cloned" << std::endl;
	LocationConfig *copy = new LocationConfig(*this);
	return (copy);
}

// |----------------------
// | CONSTRUCTORS & DESTRUCTORS
// |----------------------

LocationConfig &LocationConfig::operator = (const LocationConfig &orig) 
{
	if (this != &orig)
	{
		this->_sub_locations = orig._sub_locations;
		this->_name = orig._name;
		this->_root = orig._root;
		this->_index = orig._index;
		this->_cgi_pass = orig._cgi_pass;
		this->_methods = orig._methods;
		this->_client_body_buffer_size = orig._client_body_buffer_size;
		//this->_alias = orig._alias;
	}
	//std::cout << "LocationConfig assignment copy-constructed." << std::endl;
	return (*this);
}

LocationConfig::LocationConfig(const LocationConfig &orig)
{
	*this = orig;
	//std::cout << "LocationConfig copy-constructed." << std::endl;
}

LocationConfig::LocationConfig(std::istream& location_file, std::string line)
{
	this->_client_body_buffer_size = 100000;
	//this->setAlias(false);
	this->parse_location(location_file, line);
	//std::cout << "LocationConfig constructed." << std::endl;
}

LocationConfig::LocationConfig(void)
{
	this->setMethods("");
	this->setName("");
	this->setRoot("");
	this->setIndex("");
	this->setPass("");
	this->_client_body_buffer_size = 100000;
	//this->setAlias(false);
	//std::cout << "LocationConfig constructed." << std::endl;
}

LocationConfig::~LocationConfig(void)
{
	//std::cout << "LocationConfig destructed." << std::endl;
}

// |----------------------
// | OVERLOAD OPERATIONS
// |----------------------

// |----------------------
// | EXCEPTIONS
// |----------------------

