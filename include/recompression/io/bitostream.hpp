/*
 * This file integrates customized parts of tudocomp into recompression.
 * tudocomp is licensed under the Apache License Version 2.0, which follows.
 *
 * TuDoComp - TU Dortmund lossless compression framework
 * Copyright (C) 2016 Patrick Dinklage, Dominik Köppl, Marvin Löbel, Johannes Fischer
 * Contact found at: https://ls11-www.cs.tu-dortmund.de/staff/koeppl
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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
 private:
    std::ofstream stream;

 public:
    explicit inline BitOStream(const std::string& file_name, std::ios_base::openmode mode = std::ios_base::out)
            : file_name(file_name) {
        stream = std::ofstream(file_name, mode);
        if (!stream) {
            std::cerr << "Failed to read file " << file_name << std::endl;
            exit(1);
        }
    }

    ~BitOStream() {
        if (stream.is_open()) {
            write_end();
        }
    }

    const std::string& get_file_name() const {
        return file_name;
    }

    inline void close() {
        write_end();
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
            write_bit(((value_t(value_t(1) << i)) & value) != value_t(0));
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
                    value |= (_bit32_one << 31);
                    if (val) {
                        value |= (_bit32_one << 30);
                    }
                    std::uint32_t check_len = len & (std::uint32_t(3) << 30);
                    if (check_len) {
                        i -= len;
                        if (len & (_bit32_one << 31)) {
                            len = len >> 2;
                        } else {
                            len = len >> 1;
                        }
                        i += len;
                        value |= len;
                    } else {
                        value |= len;
                    }
                } else {
                    i -= len;
                    for (size_t j = 0; j < 31 && i < bv.size(); ++j, ++i) {
                        if (bv[i]) {
                            value |= (_bit32_one << (30 - j));
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

    static constexpr std::uint32_t _bit32_one = std::uint32_t(1);
    static constexpr std::uint8_t _all_set = std::uint8_t(7);

    bool dirty = false;
    std::uint8_t buffer = 0;
    std::int8_t cursor = _all_set;

    inline void write_end() {
        if (cursor < 2) {
            write_buffer();
            buffer = _all_set - cursor;
        } else {
            buffer |= (_all_set - cursor);
        }

        dirty = true;
        write_buffer();
    }

    /**
     * @brief Writes the current buffer to the output stream.
     */
    inline void write_buffer() {
        if (dirty) {
            stream.put(static_cast<char>(buffer));
            dirty = false;
            buffer = 0;
            cursor = _all_set;
        }
    }
};
