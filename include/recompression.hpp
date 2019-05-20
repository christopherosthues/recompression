#pragma once

#include <memory>

#include "recompression/defs.hpp"
#include "recompression/recompression.hpp"
#include "recompression/fast_recompression.hpp"
#include "recompression/parallel_ls_recompression.hpp"
#include "recompression/parallel_gr_recompression.hpp"
#include "recompression/experimental/parallel_lock_recompression.hpp"
#include "recompression/experimental/parallel_gr2_recompression.hpp"
#include "recompression/hash_recompression.hpp"
#include "recompression/parallel_lp_recompression.hpp"
#include "recompression/parallel_recompression.hpp"
#include "recompression/parallel_rnd_recompression.hpp"
#include "recompression/parallel_rnddir_recompression.hpp"
#include "recompression/lce_query.hpp"
#include "recompression/radix_sort.hpp"
#include "recompression/rlslp.hpp"
#include "recompression/util.hpp"
#include "recompression/experimental/parallel_gr_alternate_recompression.hpp"
#include "recompression/experimental/parallel_order_great_recompression.hpp"
#include "recompression/experimental/parallel_order_less_recompression.hpp"
#include "recompression/experimental/parallel_ls3_recompression.hpp"
#include "recompression/experimental/parallel_ls5_recompression.hpp"
#include "recompression/experimental/parallel_ls_gain_recompression.hpp"
#include "recompression/experimental/parallel_parhip_recompression.hpp"
#include "recompression/io/bitistream.hpp"
#include "recompression/io/bitostream.hpp"
#include "recompression/coders/coder.hpp"
#include "recompression/coders/plain_rlslp_coder.hpp"
#include "recompression/coders/plain_fixed_rlslp_coder.hpp"
#include "recompression/coders/sorted_rlslp_coder.hpp"
#include "recompression/coders/sorted_rlslp_dr_coder.hpp"
#include "recompression/coders/rlslp_rule_sorter.hpp"

namespace recomp {

void sequential_variants(std::vector<std::string>& variants) {
    variants.emplace_back("fast_seq");
    variants.emplace_back("hash");
}

void parallel_variants(std::vector<std::string>& variants) {
    variants.emplace_back("parallel");
    variants.emplace_back("parallel_lp");
    variants.emplace_back("parallel_rnd");
    variants.emplace_back("parallel_ls");
    variants.emplace_back("parallel_gr");
    variants.emplace_back("parallel_rnddir");
    variants.emplace_back("parallel_lock");
}

void experimental_variants(std::vector<std::string>& variants) {
    variants.emplace_back("parallel_ls3");
    variants.emplace_back("parallel_ls5");
    variants.emplace_back("parallel_ls_gain");
    variants.emplace_back("parallel_gr2");
    variants.emplace_back("parallel_gr_alternate");
    variants.emplace_back("parallel_order_gr");
    variants.emplace_back("parallel_order_ls");
    variants.emplace_back("parallel_parhip");
}

template<typename variable_t = var_t>
std::unique_ptr<recompression<variable_t>> create_recompression(const std::string& name, std::string& dataset, std::string parhip, std::string dir) {

    size_t k = 1;
    if (name.find("parallel_rnd") == 0) {
        bool dir = false;
        if (name != "parallel_rnd") {
            auto number = name.substr(12);
            if (number.find("dir") == 0) {
                number = name.substr(15);
                dir = true;
            }
            if (!number.empty()) {
                k = util::str_to_int(number);
            } else {
                k = 1;
            }
            std::cout << "Using " << k << " iterations for " << name << std::endl;
        }
        if (dir) {
            return std::make_unique<parallel::parallel_rnddir_recompression<variable_t>>(dataset, k);
        } else {
            return std::make_unique<parallel::parallel_rnd_recompression<variable_t>>(dataset, k);
        }
    }
    if (name == "parallel") {
        return std::make_unique<parallel::parallel_recompression<variable_t>>(dataset);
    } else if (name == "parallel_parhip") {
        if (parhip.empty() || dir.empty()) {
            std::cerr << "Error. parhip and/or dir not specified" << std::endl;
            exit(-1);
        }
        return std::make_unique<parallel::parallel_parhip_recompression<variable_t>>(dataset, parhip, dir);
    } else if (name == "parallel_ls") {
        return std::make_unique<parallel::parallel_ls_recompression<variable_t>>(dataset);
    } else if (name == "parallel_ls3") {
        return std::make_unique<parallel::parallel_ls3_recompression<variable_t>>(dataset);
    } else if (name == "parallel_ls5") {
        return std::make_unique<parallel::parallel_ls5_recompression<variable_t>>(dataset);
    } else if (name == "parallel_ls_gain") {
        return std::make_unique<parallel::parallel_ls_gain_recompression<variable_t>>(dataset);
    } else if (name == "parallel_gr") {
        return std::make_unique<parallel::parallel_gr_recompression<variable_t>>(dataset);
    } else if (name == "parallel_gr2") {
        return std::make_unique<parallel::parallel_gr2_recompression<variable_t>>(dataset);
    } else if (name == "parallel_gr_alternate") {
        return std::make_unique<parallel::parallel_gr_alternate_recompression<variable_t>>(dataset);
    } else if (name == "parallel_lp") {
        return std::make_unique<parallel::parallel_lp_recompression<variable_t>>(dataset);
    } else if (name == "parallel_lock") {
        return std::make_unique<parallel::parallel_lock_recompression<variable_t>>(dataset);
    } else if (name == "parallel_order_ls") {
        return std::make_unique<parallel::recompression_order_ls<variable_t>>(dataset);
    } else if (name == "parallel_order_gr") {
        return std::make_unique<parallel::recompression_order_gr<variable_t>>(dataset);
    } else if (name == "fast_seq") {
        return std::make_unique<recompression_fast<variable_t>>(dataset);
    } else if (name == "hash") {
        return std::make_unique<hash_recompression<variable_t>>(dataset);
    } else {
        return std::unique_ptr<recompression<variable_t>>(nullptr);
    }
}

namespace coder {

template<typename variable_t = var_t>
void encode(const std::string& coder, const std::string& file_name, rlslp<variable_t>& rlslp) {
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
    }
}

template<typename variable_t = var_t>
rlslp<variable_t> decode(const std::string& coder, const std::string& file_name) {
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
        return rlslp<variable_t>{};
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
