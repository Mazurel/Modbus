#include <cstdint>
#include <optional>
#include <vector>

//! This namespace contains functions used for CRC calculation
namespace MB::CRC {
//! Calculates CRC based on the input buffer - C style
uint16_t calculateCRC(const uint8_t *buff, std::size_t len);

//! Calculate CRC based on the input vector of bytes
inline uint16_t calculateCRC(const std::vector<uint8_t> &buffer, std::optional<std::size_t> len = std::nullopt) {
    std::size_t bufferLength = buffer.size();
    if (len.has_value() && bufferLength >= *len) {
        bufferLength = *len;
    }

    return calculateCRC(static_cast<const uint8_t *>(&(*buffer.begin())), bufferLength);
}
}; // namespace MB::CRC
