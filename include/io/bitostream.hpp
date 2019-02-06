
#pragma once

#include <climits>
#include <fstream>
#include <ios>
#include <vector>

#include "util.hpp"

/**
 *
 */
class BitOStream {
    std::ofstream stream;

    bool dirty = false;
    std::uint8_t buffer = 0;
    std::uint8_t cursor = 7;

    /**
     * @brief Writes the current buffer to the output stream.
     */
    inline void write_buffer() {
        if (dirty) {
            stream.put(static_cast<char>(buffer));
            reset();
        }
    }

 public:
    inline BitOStream(const std::string& file_name, std::ios_base::openmode mode = std::ios_base::out) {
        stream = std::ofstream(file_name, mode);
    }

    inline BitOStream(BitOStream& ostream) {
        stream = std::move(ostream.stream);
        dirty = ostream.dirty;
        buffer = ostream.buffer;
        cursor = ostream.cursor;
    }

    inline BitOStream(std::ofstream&& stream) : stream(std::move(stream)) {}

    ~BitOStream() {
        char last = static_cast<char>(7 - cursor);
        if (cursor >= 2) {
            buffer |= last;
        } else {
            write_buffer();
            buffer = static_cast<std::uint8_t>(last);
        }

        dirty = true;
        write_buffer();
    }

    /**
     * @brief Resets the internal state.
     */
    inline void reset() {
        dirty = false;
        buffer = 0;
        cursor = 7;
    }

    inline auto tellp() -> decltype(stream.tellp()) {
        return stream.tellp();
    }

    inline void close() {
        stream.close();
    }

    /**
     * @brief Write one bit to the ouput stream.
     *
     * @param bit The bit to write
     */
    inline void write_bit(bool bit) {
        if (bit) {
            buffer |= (1 << cursor);
        }

        dirty = true;
        cursor--;
        if (cursor < 0) {
            write_buffer();
        }
    }

    /**
     * @brief Writes the unary representation of the value to the output stream.
     *
     * @tparam value_t The type of integer
     * @param value The value to write
     */
    template<typename value_t>
    inline void write_unary(value_t value) {
        while (value--) {
            write_bit(true);
        }
        write_bit(false);
    }

    /**
     * @brief Writes the bit representation of the integer to the output stream.
     *
     * The integer value gets written in MSB first order to the output stream.
     *
     * @tparam value_t The type of integer
     * @param value The value to write
     * @param bits The number of bits to write of the integer from position @code{bit} down to the lowest bit
     */
    template<typename value_t>
    inline void write_int(value_t value, size_t bits = sizeof(value_t) * CHAR_BIT) {
        for (size_t i = bits - 1; i >= 0; --i) {
            write_bit((value & value_t(value_t(1) << i)) != value_t(0));
        }
    }

    inline void write_bitvector_compressed(std::vector<bool>& bv) {
        write_int<size_t>(bv.size());
        if (!bv.empty()) {
            auto val = bv[0];
            std::uint32_t len = 0;
            for (size_t i = 0; i < bv.size(); ++i) {
                while (i < bv.size() && bv[i] == val) {
                    len++;
                    i++;
                }
                std::uint32_t value = 0;
                if (len > 31) {
                    value |= (std::uint32_t(1) << 31);
                    if (val) {
                        value |= (std::uint32_t(1) << 30);
                    }
                    std::uint32_t check_len = len | (std::uint32_t(3) << 30);
                    if (check_len) {
                        // TODO(Chris): verkleinern
                    } else {
                        value |= len;
                    }
                } else {
                    for (size_t j = 0; j < 31 && i < bv.size(); ++j, ++i) {
                        if (bv[i - len]) {
                            value |= (std::uint32_t(1) << (30 - j));
                        }
                    }
                }
                write_int<std::uint32_t>(value);
            }
        }
    }

    /**
     * @brief Writes one bit to the output stream.
     *
     * @param bit The bit to write
     */
    inline void operator<<(bool bit) {
        write_bit(bit);
    }

    /**
     * @brief Writes the bit representation of the integer to the output stream.
     *
     * The integer value gets written in MSB first order to the output stream.
     *
     * @tparam value_t The type of integer
     * @param value The value to write
     */
    template<typename value_t>
    inline void operator<<(value_t value) {
        write_int(value);
        // write_int(value, bits_for(value));
    }
};
