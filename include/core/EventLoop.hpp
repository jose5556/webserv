/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joseoliv <joseoliv@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 18:44:16 by cereais           #+#    #+#             */
/*   Updated: 2025/11/11 11:48:00 by joseoliv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.hpp"
#include "Connection.hpp"
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

class Server;
class Connection;

class EventLoop
{

public:
	EventLoop();
	~EventLoop();

	void addListeningSocket(const Socket *socket);
	void run();

private:
	struct PollEntry
	{
		struct pollfd pfd;
		struct sockaddr_in socketAddr;
		Connection *conn;
		int port; // Added field to store the port of the server socket
	};

	std::vector<PollEntry> _pollEntries;

	void	closeConnection(PollEntry &entry);
	void	removeClosedEntries(std::vector<PollEntry> &entries);
};
