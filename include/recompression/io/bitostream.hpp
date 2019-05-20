
#pragma once

#include <climits>
#include <fstream>
#include <ios>
#include <vector>

#include "recompression/util.hpp"

/**
 *
 */
class BitOStream {
    std::ofstream stream;

    bool dirty = false;
    std::uint8_t buffer = 0;
    std::int8_t cursor = 7;

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
    inline BitOStream(const std::string& file_name, std::ios_base::openmode mode = std::ios_base::out) : file_name(file_name) {
        stream = std::ofstream(file_name, mode);
        if (!stream) {
            std::cerr << "Failed to read file " << file_name << std::endl;
            exit(1);
        }
        reset();
    }

    ~BitOStream() {
        if (stream.is_open()) {
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
    }

    const std::string& get_file_name() const {
        return file_name;
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
        char last = static_cast<char>(7 - cursor);
        if (cursor >= 2) {
            buffer |= last;
        } else {
            write_buffer();
            buffer = static_cast<std::uint8_t>(last);
        }

        dirty = true;
        write_buffer();
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

    template<typename value_t>
    inline void write_elias_gamma_code(value_t value) {
        std::uint8_t bits = recomp::util::bits_for(value);
        write_unary(bits);
        write_int(value, bits);
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
        for (int i = bits - 1; i >= 0; --i) {
            write_bit((value & (value_t(value_t(1) << i))) != value_t(0));
        }
    }

    inline void write_bitvector_compressed(std::vector<bool>& bv) {
        write_int<size_t>(bv.size());
        if (!bv.empty()) {
            bool val = bv[0];
            std::uint32_t len = 0;
            for (size_t i = 0; i < bv.size();) {
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
                    std::uint32_t check_len = len & (std::uint32_t(3) << 30);
                    if (check_len) {
                        i -= len;
                        if (len & (std::uint32_t(1) << 31)) {
                            len = len >> 2;
                        } else {
                            len = len >> 1;
                        }
                        i += len;
                        value |= len;
                        // TODO(Chris): we already know the total length, split it here and write all but the last out,
                        //  the last is handled by the loop itself
                    } else {
                        value |= len;
                    }
                } else {
                    i -= len;
                    for (size_t j = 0; j < 31 && i < bv.size(); ++j, ++i) {
                        if (bv[i]) {
                            value |= (std::uint32_t(1) << (30 - j));
                        }
                    }
                }
                if (i < bv.size()) {
                    val = bv[i];
                }
                len = 0;
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
    }

 private:
    std::string file_name;
};
