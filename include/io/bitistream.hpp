
#pragma once

#include <climits>
#include <fstream>
#include <ios>

class BitIStream {
    std::ifstream stream;

    std::uint8_t current = 0;
    std::uint8_t buffer = 0;

    bool end = false;
    std::uint8_t end_bits = 0;

    std::uint8_t cursor = 0;

    inline void read_buffer() {
        current = buffer;
        current = 7;

        char c;
        if (stream.get(c)) {
            buffer = static_cast<std::uint8_t>(c);

            if (stream.get(c)) {
                stream.unget();
            } else {
                end_bits = static_cast<std::uint8_t>(c);
                end_bits &= 0x7;
                if (end_bits >= 6) {
                    end = true;
                    buffer = 0;
                }
            }
        } else {
            end = true;
            end_bits = current & static_cast<std::uint8_t>(0x7);

            buffer = 0;
        }
    }

 public:

    inline BitIStream(const std::string& file_name, std::ios_base::openmode mode = std::ios_base::out) {
        stream = std::ifstream(file_name, mode);
    }

    inline BitIStream(std::ifstream&& stream) : stream(std::move(stream)) {
        char c;
        if (this->stream.get(c)) {
            end = false;
            buffer = static_cast<std::uint8_t>(c);

            read_buffer();
        } else {
            end = true;
            end_bits = 0;
        }
    }

    ~BitIStream() {}

    /**
     * @brief Resets the internal state.
     */
    inline void reset() {
//        dirty = false;
//        buffer = 0;
//        cursor = 7;
    }

    inline auto tellg() -> decltype(stream.tellg()) {
        return stream.tellg();
    }

    inline void close() {
        stream.close();
    }

    /**
     * @brief Reads one bit from the input stream.
     *
     * @param bit The bit to read
     */
    inline std::uint8_t read_bit() {
        if (!eof()) {
            std::uint8_t bit = (current >> cursor) & std::uint8_t(1);
            if (cursor) {
                cursor--;
            } else {
                read_buffer();
            }
            return bit;
        } else {
            return 0;
        }
    }

    /**
     * @brief Reads the unary representation of the value to the output stream.
     *
     * @tparam value_t The type of integer
     */
    template<typename value_t>
    inline value_t read_unary() {
        value_t value = 0;
        while (read_bit()) {
            value++;
        }
        return value;
    }

    /**
     * @brief Writes the bit representation of the integer to the output stream.
     *
     * The integer value gets written in MSB first order to the output stream.
     *
     * @tparam value_t The type of integer
     * @param bits The number of bits to write of the integer from position @code{bit} down to the lowest bit
     */
    template<typename value_t>
    inline value_t read_int(size_t bits = sizeof(value_t) * CHAR_BIT) {
        value_t value = 0;
        for (size_t i = 0; i < bits; ++i) {
            value = (value << value_t(1)) | read_bit();
//            value <<= 1;
//            value |= read_bit();
        }
        return value;
    }

    inline bool eof() const {
        return end && cursor <= (7 - end_bits);
    }

//    /**
//     * @brief Writes one bit to the output stream.
//     *
//     * @param bit The bit to write
//     */
//    inline std::uint8_t operator<<() {
//        return read_bit();
//    }
//
//    /**
//     * @brief Writes the bit representation of the integer to the output stream.
//     *
//     * The integer value gets written in MSB first order to the output stream.
//     *
//     * @tparam value_t The type of integer
//     * @param value The value to write
//     */
//    template<typename value_t>
//    inline void operator<<(value_t value) {
//        write_int(value);
//        // write_int(value, bits_for(value));
//    }
};
