
#include "rlslp.hpp"

/* ======================= non_terminal ======================= */
recomp::rlslp::non_terminal::non_terminal() = default;

// inline rlslp::non_terminal::non_terminal(variable_t terminal) : production() {
//     this->production[0] = terminal;
//     this->len = 1;
// }

recomp::rlslp::non_terminal::non_terminal(recomp::variable_t first, recomp::variable_t second, size_t len)
        : production() {
    this->production[0] = first;
    this->production[1] = second;
    this->len = len;
}

bool recomp::rlslp::non_terminal::operator==(const recomp::rlslp::non_terminal& nt) const {
    return this->production == nt.production && this->len == nt.len;
}

recomp::variable_t& recomp::rlslp::non_terminal::first() {
    return this->production[0];
}

recomp::variable_t& recomp::rlslp::non_terminal::second() {
    return this->production[1];
}

const recomp::variable_t& recomp::rlslp::non_terminal::first() const {
    return this->production[0];
}

const recomp::variable_t& recomp::rlslp::non_terminal::second() const {
    return this->production[1];
}


/* ======================= rlslp ======================= */
void recomp::rlslp::extract(std::stringstream& sstream, size_t i, size_t len, recomp::variable_t nt) const {
    if (len > 0) {
        if (nt < static_cast<variable_t>(terminals)) {
            sstream << static_cast<char>(nt);
        } else {
            auto first = non_terminals[nt - terminals].first();
            auto second = non_terminals[nt - terminals].second();
            size_t first_len = 1;
            if (first >= static_cast<variable_t>(terminals)) {
                first_len = non_terminals[first - terminals].len;
            }

            // Block
            if (second < 0) {
                auto b_len = first_len;
                auto idx = 0;

                while (i >= first_len) {
                    first_len += b_len;
                    idx++;
                }
                auto r_len = rest_len(i, len, first_len);

                extract(sstream, i - idx * b_len, len - r_len, first);

                while (r_len > 0) {
                    if (r_len > b_len) {
                        r_len = b_len;
                    }
                    extract(sstream, 0, r_len, first);
                    first_len += b_len;
                    r_len = rest_len(i, len, first_len);
                }
            } else {  // Pair
                auto r_len = rest_len(i, len, first_len);

                if (i < first_len) {
                    extract(sstream, i, len - r_len, first);

                    if (r_len > 0) {
                        extract(sstream, 0, r_len, second);
                    }
                } else {
                    extract(sstream, i - first_len, len, second);
                }
            }
        }
    }
}

std::string recomp::rlslp::extract(size_t i, size_t len) const {
    std::stringstream sstream;
    if (!empty() && i < non_terminals[root].len && len > 0) {
        extract(sstream, i, len, root);
    }
    return sstream.str();
}

recomp::rlslp::non_terminal& recomp::rlslp::operator[](size_t i) {
    return this->non_terminals[i];
}

const recomp::rlslp::non_terminal& recomp::rlslp::operator[](size_t i) const {
    return this->non_terminals[i];
}

size_t recomp::rlslp::size() const {
    return non_terminals.size();
}

bool recomp::rlslp::empty() const {
    return size() == 0;
}

void recomp::rlslp::reserve(size_t size) {
    non_terminals.reserve(size);
}

void recomp::rlslp::resize(size_t size) {
    non_terminals.resize(size);
}

void recomp::rlslp::derive(std::stringstream& sstream, recomp::variable_t& nt) {
    if (nt < static_cast<variable_t>(terminals)) {
        sstream << static_cast<char>(nt);
    } else {
        derive(sstream, non_terminals[nt - terminals].first());
        auto second = non_terminals[nt - terminals].second();
        // pair
        if (second >= 0) {
            derive(sstream, second);
        } else {  // block
            variable_t b_len = -second - 1;
            while (b_len--) {
                derive(sstream, non_terminals[nt - terminals].first());
            }
        }
    }
}

std::string recomp::rlslp::derive_text() {
    std::stringstream sstream;
    derive(sstream, root);
    return sstream.str();
}


/* ======================= to_string ======================= */
std::string std::to_string(const recomp::rlslp::non_terminal& nt) {
    std::stringstream sstream("production: (");
    sstream << std::to_string(nt.production[0]) << "," << std::to_string(nt.production[1])
            << ") len: " << std::to_string(nt.len);

    return sstream.str();
}

std::string std::to_string(const recomp::rlslp& rlslp) {
    std::stringstream sstream("number of terminals: ");
    sstream << rlslp.terminals << std::endl;
    sstream << "non-terminals: " << std::endl;
    size_t i = 0;
    for (const auto& nt : rlslp.non_terminals) {
        sstream << i++ << ": " << to_string(nt) << std::endl;
    }

    return sstream.str();
}
