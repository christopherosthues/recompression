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
#include <iostream>

class BitIStream {
 private:
    std::ifstream stream;

 public:
    explicit inline BitIStream(const std::string& file_name, std::ios_base::openmode mode = std::ios_base::out)
            : file_name(file_name) {
        stream = std::ifstream(file_name, mode);
        if (!stream) {
            std::cerr << "Failed to read file " << file_name << std::endl;
            exit(1);
        }
        char c;
        end = !stream.get(c);
        if (!end) {
            buffer = static_cast<std::uint8_t>(c);
            read_buffer();
        }
    }

    ~BitIStream() = default;

    const std::string& get_file_name() const {
        return file_name;
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
            std::uint8_t mask = current >> cursor;
            if (cursor) {
                cursor--;
            } else {
                read_buffer();
            }
            return mask & _one_set;
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

    template<typename value_t>
    inline value_t read_elias_gamma_code() {
        auto bits = read_unary<uint8_t>();
        return read_int<value_t>(bits);
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
            auto val = read_bit();
            value = (value << value_t(1)) | val;
        }
        return value;
    }

    inline std::vector<bool> read_bitvector_compressed() {
        auto size = read_int<size_t>();
        std::vector<bool> bv(size);
        if (size > 0) {
            for (size_t i = 0; i < bv.size();) {
                auto value = read_int<std::uint32_t>();
                auto type = (value >> 31) & std::uint32_t(1);
                if (type) {
                    bool v = (bool) ((value >> 30) & std::uint32_t(1));
                    std::uint32_t len = ((value << 2) >> 2);
                    for (size_t j = 0; j < len && i < bv.size(); ++j, ++i) {
                        bv[i] = v;
                    }
                } else {
                    for (size_t j = 0; j < 31 && i < bv.size(); ++j, ++i) {
                        bool v = (bool) ((value >> (30 - j)) & std::uint32_t(1));
                        bv[i] = v;
                    }
                }
            }
        }
        return bv;
    }

    inline bool eof() const {
        return end && cursor <= _all_set - end_bits;
    }

 private:
    std::string file_name;

    static constexpr std::uint8_t _all_set = std::uint8_t(7);
    static constexpr std::uint8_t _one_set = std::uint8_t(1);

    std::uint8_t current = 0;
    std::uint8_t buffer = 0;

    bool end = false;
    std::uint8_t end_bits = 0;

    std::uint8_t cursor = 0;

    inline void read_buffer() {
        current = buffer;
        cursor = _all_set;

        char c;
        if (stream.get(c)) {
            buffer = static_cast<std::uint8_t>(c);

            if (stream.get(c)) {
                stream.unget();
            } else {
                if ((end_bits = buffer & _all_set) >= 6) {
                    buffer = 0;
                    end = true;
                }
            }
        } else {
            end_bits = _all_set & current;
            buffer = 0;
            end = true;
        }
    }
};
