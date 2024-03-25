#include "Serial/Connection.h"

#ifndef _WIN32

namespace MB::Serial {

Connection::Connection(const std::string &path)
{
    _fd = open(path.c_str(), O_RDWR | O_SYNC);

    if (_fd < 0) {
        throw std::runtime_error("Cannot open serial port " + path);
    }

    if (tcgetattr(_fd, &_termios) != 0) {
        throw std::runtime_error("Error at tcgetattr - " + std::to_string(errno));
    }

    cfmakeraw(&_termios);

    _termios.c_iflag &= ~(PARMRK | INPCK);
    _termios.c_iflag |= IGNPAR;
}

void Connection::connect()
{
    tcflush(_fd, TCIFLUSH);
    if (tcsetattr(_fd, TCSAFLUSH, &_termios) != 0) {
        throw std::runtime_error("Error {" + std::to_string(_fd) + "} at tcsetattr - " + std::to_string(errno));
    }
}

Connection::~Connection()
{
    if (_fd >= 0)
        ::close(_fd);
    _fd = -1;
}

std::vector<uint8_t> Connection::send_request(const MB::ModbusRequest &request)
{
    return send(request.toRaw());
}

std::vector<uint8_t> Connection::sendResponse(const MB::ModbusResponse &response)
{
    return send(response.toRaw());
}

std::vector<uint8_t> Connection::sendException(const MB::ModbusException &exception)
{
    return send(exception.toRaw());
}

std::vector<uint8_t> Connection::await_raw_message()
{
    std::vector<uint8_t> data(1024);

    pollfd waitingFD = {.fd = _fd, .events = POLLIN, .revents = POLLIN};

    if (::poll(&waitingFD, 1, _timeout) <= 0) {
        throw MB::ModbusException(MB::utils::Timeout);
    }

    auto size = ::read(_fd, data.begin().base(), 1024);

    if (size < 0) {
        throw MB::ModbusException(MB::utils::SlaveDeviceFailure);
    }

    data.resize(size);
    data.shrink_to_fit();

    return data;
}

// TODO: Figure out how to return raw data when exception is being thrown
std::tuple<MB::ModbusResponse, std::vector<uint8_t>> Connection::await_response()
{
    std::vector<uint8_t> data;
    data.reserve(8);

    MB::ModbusResponse response(0, MB::utils::ReadAnalogInputRegisters);

    while (true) {
        try {
            auto tmpResponse = await_raw_message();
            data.insert(data.end(), tmpResponse.begin(), tmpResponse.end());

            if (MB::ModbusException::exist(data))
                throw MB::ModbusException(data);

            response = MB::ModbusResponse::fromRawCRC(data);
            break;
        } catch (const MB::ModbusException &ex) {
            if (MB::utils::isStandardErrorCode(ex.getErrorCode()) || ex.getErrorCode() == MB::utils::Timeout ||
                ex.getErrorCode() == MB::utils::SlaveDeviceFailure)
                throw ex;
            continue;
        }
    }

    return std::tie(response, data);
}

std::tuple<MB::ModbusRequest, std::vector<uint8_t>> Connection::await_request()
{
    std::vector<uint8_t> data;
    data.reserve(8);

    MB::ModbusRequest request(0, MB::utils::ReadAnalogInputRegisters);

    while (true) {
        try {
            auto tmpResponse = await_raw_message();
            data.insert(data.end(), tmpResponse.begin(), tmpResponse.end());

            request = MB::ModbusRequest::fromRawCRC(data);
            break;
        } catch (const MB::ModbusException &ex) {
            if (ex.getErrorCode() == MB::utils::Timeout || ex.getErrorCode() == MB::utils::SlaveDeviceFailure)
                throw ex;
            continue;
        }
    }

    return std::tie(request, data);
}

std::vector<uint8_t> Connection::send(std::vector<uint8_t> data)
{
    data.reserve(data.size() + 2);
    const auto crc = utils::calculateCRC(data.begin().base(), data.size());

    data.push_back(reinterpret_cast<const uint8_t *>(&crc)[0]);
    data.push_back(reinterpret_cast<const uint8_t *>(&crc)[1]);

    // Ensure that nothing will intervene in our communication
    // WARNING: It may conflict with something (although it may also help in
    // most cases)
    tcflush(_fd, TCOFLUSH);
    // Write
    write(_fd, data.begin().base(), data.size());
    // It may be a good idea to use tcdrain, although it has tendency to not
    // work as expected tcdrain(_fd);

    return data;
}

Connection::Connection(Connection &&moved) noexcept
{
    _fd = moved._fd;
    _termios = moved._termios;
    moved._fd = -1;
}

Connection &Connection::operator=(Connection &&moved)
{
    if (this == &moved)
        return *this;

    _fd = moved._fd;
    memcpy(&_termios, &(moved._termios), sizeof(moved._termios));
    moved._fd = -1;
    return *this;
}

}  // namespace MB::Serial

#endif
