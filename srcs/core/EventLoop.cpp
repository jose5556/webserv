/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cereais <cereais@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 19:35:11 by joseoliv          #+#    #+#             */
/*   Updated: 2025/10/28 20:54:21 by cereais          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <core/EventLoop.hpp>
#include <core/Server.hpp>
#include <core/Connection.hpp>
#include <http/HttpResponse.hpp>

volatile sig_atomic_t stopServer = 0;

static void signalHandler(int signum) {
	if (signum == SIGINT) {
		std::cout << "\nShutting down the server gracefully...\n"
				  << std::endl;
		stopServer = 1;
	}
}

EventLoop::EventLoop() { _pollEntries.reserve(1024); }

EventLoop::~EventLoop() {

	for (size_t i = 0; i < _pollEntries.size(); ++i) {
		if (_pollEntries[i].conn)
			delete _pollEntries[i].conn;
		if (_pollEntries[i].pfd.fd >= 0)
			close(_pollEntries[i].pfd.fd);
		std::cout << "Connection closed: fd " << _pollEntries[i].pfd.fd << std::endl;
	}
}

void EventLoop::addListeningSocket(const Socket *socket) {
	struct pollfd pfd;
	pfd.fd = socket->getFd();
	pfd.events = POLLIN;
	pfd.revents = 0;

	PollEntry entry;
	entry.pfd = pfd;
	entry.conn = NULL;
	entry.socketAddr = socket->getAddr();
	entry.port = ntohs(socket->getAddr().sin_port);
	_pollEntries.push_back(entry);
}

void EventLoop::run() {

	while (true) {

		signal(SIGINT, signalHandler);
		if (stopServer) {
			while (!_pollEntries.empty()) {
				closeConnection(_pollEntries.back());
				_pollEntries.pop_back();
			}
			break;
		}
		// build contiguous temp pollfd array
		std::vector<struct pollfd> pfdArray;
		std::vector<size_t> indexMap;
		pfdArray.reserve(_pollEntries.size());
		for (size_t i = 0; i < _pollEntries.size(); ++i) {

			if (_pollEntries[i].pfd.fd >= 0)
			{
				pfdArray.push_back(_pollEntries[i].pfd);
				indexMap.push_back(i);
			}
		}

		if (!pfdArray.empty()) {
			int errorCode = poll(&pfdArray[0], pfdArray.size(), 5000);
			if (errorCode < 0) {
				if (errno == EINTR)
				{
					break;
				}
				perror("poll");
				break;
			}
			for (size_t j = 0; j < pfdArray.size(); ++j)
				_pollEntries[indexMap[j]].pfd.revents = pfdArray[j].revents;
		}

		std::vector<PollEntry> newClients;

		for (size_t i = 0; i < _pollEntries.size(); ++i) {
			PollEntry &entry = _pollEntries[i];

			if (entry.pfd.revents & (POLLHUP | POLLERR | POLLNVAL)) {
				std::cout << "Strange error, but client was brutally and tragically disconected :(" << std::endl;
				closeConnection(entry);
				entry.pfd.fd = -1;
				continue;
			}

			if (entry.pfd.revents & POLLIN) {
				if (entry.conn == NULL) {

					socklen_t addrLen = sizeof(entry.socketAddr);
					int clientFd = accept(entry.pfd.fd, (sockaddr *)&entry.socketAddr, &addrLen);
					if (clientFd < 0)
						std::cerr << "Failed to accept connection..." << std::endl;
					else {
						int flags = fcntl(clientFd, F_GETFL, 0);
						fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);

						Connection *conn = new Connection(clientFd);
						struct pollfd pfd;
						pfd.fd = clientFd;
						pfd.events = POLLIN;
						pfd.revents = 0;

						PollEntry clientEntry;
						clientEntry.pfd = pfd;
						clientEntry.conn = conn;
						clientEntry.socketAddr = entry.socketAddr;
						clientEntry.port = entry.port; // Copy the port from the listening socket's PollEntry

						newClients.push_back(clientEntry);
						std::cout << "New client accepted: fd " << clientFd << std::endl;
					}

				} else {

					if (!entry.conn->readRequest()) {
						closeConnection(entry);
						entry.pfd.fd = -1;
					}
					else if (entry.conn->isRequestComplete())
					{
						// std::cout << entry.conn->getReadBuffer() << std::endl;
						entry.conn->setWriteBuffer(HttpResponse::request_and_response(entry.conn->getReadBuffer(), entry.port));
						entry.conn->setReadBuffer(""); // clear buffer for the next read operation.
						entry.pfd.events = POLLOUT;
					}
				}
			}

			if (entry.pfd.revents & POLLOUT) {
				if (entry.conn && !entry.conn->writeResponse()) {

					closeConnection(entry);
					entry.pfd.fd = -1;
				}
				entry.pfd.events = POLLIN;
			}
			entry.pfd.revents = 0;
		}

		if (!newClients.empty())
			_pollEntries.insert(_pollEntries.end(), newClients.begin(), newClients.end());

		// remove entries with fd < 0 (closed)
		std::vector<PollEntry> remaining;
		remaining.reserve(_pollEntries.size());
		for (size_t k = 0; k < _pollEntries.size(); ++k)
		{
			if (_pollEntries[k].pfd.fd >= 0)
				remaining.push_back(_pollEntries[k]);
			else if (_pollEntries[k].conn)
				delete _pollEntries[k].conn;
		}
		_pollEntries.swap(remaining);
	}
}

void EventLoop::closeConnection(PollEntry &entry) {

	if (entry.pfd.fd >= 0)
		close(entry.pfd.fd);
	if (entry.conn)
		delete entry.conn;
	entry.conn = NULL;
	std::cout << "Connection closed: fd " << entry.pfd.fd << std::endl;
}
