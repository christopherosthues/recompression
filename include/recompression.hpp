#pragma once

#include <memory>

#include "recompression/defs.hpp"
#include "recompression/recompression.hpp"
#include "recompression/fast_recompression.hpp"
#include "recompression/parallel_ls_recompression.hpp"
#include "recompression/parallel_gr_recompression.hpp"
#include "recompression/parallel_lock_recompression.hpp"
#include "recompression/hash_recompression.hpp"
#include "recompression/parallel_lp_recompression.hpp"
#include "recompression/parallel_order_great_recompression.hpp"
#include "recompression/parallel_order_less_recompression.hpp"
#include "recompression/parallel_recompression.hpp"
#include "recompression/parallel_rnd_recompression.hpp"
#include "recompression/parallel_wrnd_recompression.hpp"
#include "recompression/lce_query.hpp"
#include "recompression/radix_sort.hpp"
#include "recompression/rlslp.hpp"
#include "recompression/util.hpp"
#include "recompression/io/bitistream.hpp"
#include "recompression/io/bitostream.hpp"
#include "recompression/coders/coder.hpp"
#include "recompression/coders/plain_rlslp_coder.hpp"
#include "recompression/coders/plain_fixed_rlslp_coder.hpp"
#include "recompression/coders/sorted_rlslp_coder.hpp"
#include "recompression/coders/sorted_rlslp_dr_coder.hpp"
#include "recompression/coders/rlslp_rule_sorter.hpp"

namespace recomp {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
std::unique_ptr<recompression<variable_t, terminal_count_t>> create_recompression(const std::string& name, std::string& dataset) {
    if (name == "parallel") {
        return std::make_unique<parallel::parallel_recompression<variable_t, terminal_count_t>>(dataset);
    } else if (name == "parallel_ls") {
        return std::make_unique<parallel::parallel_ls_recompression<variable_t, terminal_count_t>>(dataset);
    } else if (name == "parallel_gr") {
        return std::make_unique<parallel::parallel_gr_recompression<variable_t, terminal_count_t>>(dataset);
    } else if (name == "parallel_lp") {
        return std::make_unique<parallel::parallel_lp_recompression<variable_t, terminal_count_t>>(dataset);
    } else if (name == "parallel_rnd") {
        return std::make_unique<parallel::parallel_rnd_recompression<variable_t, terminal_count_t>>(dataset);
    } else if (name == "parallel_wrnd") {
        return std::make_unique<parallel::parallel_wrnd_recompression<variable_t, terminal_count_t>>(dataset);
    } else if (name == "parallel_lock") {
        return std::make_unique<parallel::parallel_lock_recompression<variable_t, terminal_count_t>>(dataset);
    } else if (name == "parallel_order_ls") {
        return std::make_unique<parallel::recompression_order_ls<variable_t, terminal_count_t>>(dataset);
    } else if (name == "parallel_order_gr") {
        return std::make_unique<parallel::recompression_order_gr<variable_t, terminal_count_t>>(dataset);
    } else if (name == "fast") {
        return std::make_unique<recompression_fast<variable_t, terminal_count_t>>(dataset);
    } else if (name == "hash") {
        return std::make_unique<hash_recompression<variable_t, terminal_count_t>>(dataset);
    } else {
        return std::unique_ptr<recompression<variable_t, terminal_count_t>>(nullptr);
    }
}

namespace coder {

template<typename variable_t = var_t, typename terminal_count_t = term_t>
void encode(const std::string& coder, const std::string& file_name, rlslp<variable_t, terminal_count_t>& rlslp) {
    if (coder == "plain") {
        PlainRLSLPCoder::Encoder enc{file_name};
        enc.encode(rlslp);
    } else if (coder == "fixed") {
        PlainFixedRLSLPCoder::Encoder enc{file_name};
        enc.encode(rlslp);
    } else if (coder == "sorted") {
        SortedRLSLPCoder::Encoder enc{file_name};
        enc.encode(rlslp);
    } else if (coder == "sorted_dr") {
        SortedRLSLPDRCoder::Encoder enc{file_name};
        enc.encode(rlslp);
//    } else {
//        std::cout << "Unknown coder '" << coder << "'.\n";
    }
}

template<typename variable_t = var_t, typename terminal_count_t = term_t>
rlslp<variable_t, terminal_count_t> decode(const std::string& coder, const std::string& file_name) {
    if (coder == "plain") {
        PlainRLSLPCoder::Decoder dec{file_name};
        return dec.decode();
    } else if (coder == "fixed") {
        PlainFixedRLSLPCoder::Decoder dec{file_name};
        return dec.decode();
    } else if (coder == "sorted") {
        SortedRLSLPCoder::Decoder dec{file_name};
        return dec.decode();
    } else if (coder == "sorted_dr") {
        SortedRLSLPDRCoder::Decoder dec{file_name};
        return dec.decode();
    } else {
        return rlslp<variable_t, terminal_count_t>{};
    }
}

std::string get_coder_extension(const std::string& name) {
    if (name == "plain") {
        return coder::PlainRLSLPCoder::k_extension;
    } else if (name == "fixed") {
        return coder::PlainFixedRLSLPCoder::k_extension;
    } else if (name == "sorted") {
        return coder::SortedRLSLPCoder::k_extension;
    } else if (name == "sorted_dr") {
        return coder::SortedRLSLPDRCoder::k_extension;
    } else {
        return "unkown";
    }
}

}  // namespace coder

}  // namespace recomp
