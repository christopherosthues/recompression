#pragma once

#include <cstdint>
#include <string>
#include <thread>

#include "defs.hpp"
#include "rlslp.hpp"

namespace recomp {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
class recompression {
 public:
    typedef std::vector<variable_t> text_t;
    typedef std::vector<variable_t> alphabet_t;

    std::string dataset = "data";
    size_t level = 0;

    inline recompression() = default;

    inline recompression(std::string& dataset) : dataset(dataset) {}

    virtual void recomp(text_t& text,
                        rlslp<variable_t, terminal_count_t>& rlslp,
                        const terminal_count_t& alphabet_size,
                        const size_t cores = std::thread::hardware_concurrency()) {};

    virtual void recomp(text_t& text,
                        rlslp <variable_t, terminal_count_t>& rlslp,
                        const size_t cores = std::thread::hardware_concurrency()) {
        this->recomp(text, rlslp, recomp::CHAR_ALPHABET, cores);
    }
};

}  // namespace recomp
