
#include "parallel_recompression.hpp"

#include <omp.h>

#include <chrono>

#ifdef DEBUG

#include <sstream>

#endif

#include <glog/logging.h>


/* ======================= bcomp ======================= */
void recomp::bcomp(recomp::text_t& text, recomp::rlslp& rlslp) {
    DLOG(INFO) << "BComp input - text size: " << text.size();
    const auto startTime = std::chrono::system_clock::now();

    // TODO(Chris): Implement parallel BComp

    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    DLOG(INFO) << "Time for bcomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
               << "[ms]";
}


/* ======================= pcomp ======================= */
void recomp::pcomp(recomp::text_t& text, recomp::rlslp& rlslp) {
    DLOG(INFO) << "PComp input - text size: " << text.size();
    const auto startTime = std::chrono::system_clock::now();

    // TODO(Chris): Implement parallel BComp

    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    DLOG(INFO) << "Time for pcomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
               << "[ms]";
}

inline void recomp::compute_multiset(const recomp::text_t& text, recomp::multiset_t& multiset) {
    const auto startTime = std::chrono::system_clock::now();

    // TODO(Chris): Implement parallel multiset

    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    DLOG(INFO) << "Time for computing multiset: "
               << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
}

inline void recomp::compute_partition(const recomp::multiset_t& multiset,
                                      const recomp::alphabet_t& alphabet,
                                      recomp::partition_t& partition) {
    const auto startTime = std::chrono::system_clock::now();

    // TODO(Chris): have to wait for those symbols which are not yet assigned to a partition -> if first letter
    // is assigned to a partition all entries in the multiset that depend on this symbol can compute a partial
    // result and must wait for the next symbol that is not assigned to a partition -> locks only on symbols that
    // are not fully processed

    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    DLOG(INFO) << "Time for computing partition: "
               << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]";
}

void recomp::recomp(recomp::text_t& text, recomp::rlslp& rlslp) {
    recomp(text, rlslp, recomp::CHAR_ALPHABET);
}

void recomp::recomp(recomp::text_t& text, recomp::rlslp& rlslp, const recomp::terminal_count_t& alphabet_size) {
    DLOG(INFO) << "recomp input - text size: " << text.size() << " - alphabet size: " << std::to_string(alphabet_size);
    const auto startTime = std::chrono::system_clock::now();

    bool not_finished = text.size() > 1;
    rlslp.terminals = alphabet_size;

    while (not_finished) {
        bcomp(text, rlslp);
        not_finished = text.size() > 1;

        if (not_finished) {
            pcomp(text, rlslp);
            not_finished = text.size() > 1;
        }
    }

    rlslp.root = static_cast<variable_t>(rlslp.non_terminals.size() - 1);

    const auto endTime = std::chrono::system_clock::now();
    const auto timeSpan = endTime - startTime;
    DLOG(INFO) << "Time for recomp: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count()
               << "[ms]";
}
