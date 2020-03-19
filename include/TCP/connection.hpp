//
// Created by mateusz on 23.01.2020.
//

#ifndef PROTOCOLCONVERTER_CONNECTION_HPP
#define PROTOCOLCONVERTER_CONNECTION_HPP

#include <memory>
#include <type_traits>

#include <sys/socket.h>
#include <netinet/in.h>
#include <libnet.h>
#include <poll.h>
#include <cerrno>

#include "modbusRequest.hpp"
#include "modbusResponse.hpp"
#include "modbusException.hpp"

namespace MB::TCP
{
class Connection
{
public:
    static const unsigned int DefaultTCPTimeout = 500;

private:
    int _sockfd = -1;
    uint16_t _messageID = 0;
    int _timeout = DefaultTCPTimeout;

public:
    explicit Connection() noexcept : _sockfd(-1), _messageID(0) {};
    explicit Connection(int sockfd) noexcept;
    Connection(const Connection & copy) = delete;
    Connection(Connection && moved) noexcept;
    Connection& operator=(Connection &&other) noexcept
    {
        if (this == &other)
            return *this;

        if (_sockfd != -1 && _sockfd != other._sockfd)
            ::close(_sockfd);

        _sockfd = other._sockfd;
        _messageID = other._messageID;
        other._sockfd = -1;

        return *this;
    }

    [[nodiscard]] int getSockfd() const
    {
        return _sockfd;
    }

    static Connection with(std::string addr, int port);

    ~Connection();

    void sendRequest(const MB::ModbusRequest& req);
    void sendResponse(const MB::ModbusResponse& res);
    void sendException(const MB::ModbusException& ex);

    MB::ModbusRequest awaitRequest();
    MB::ModbusResponse awaitResponse();

    [[nodiscard]] uint16_t getMessageId() const
    {
        return _messageID;
    }

    void setMessageId(uint16_t messageId)
    {
        _messageID = messageId;
    }
};
}


#endif //PROTOCOLCONVERTER_CONNECTION_HPP
