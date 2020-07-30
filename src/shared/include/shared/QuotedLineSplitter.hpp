//
// Created by Nicholas Robison on 7/30/20.
//

#ifndef MOBILITY_CPP_QUOTEDLINESPLITTER_HPP
#define MOBILITY_CPP_QUOTEDLINESPLITTER_HPP

#include <vector>
#include <string>
#include <string_view>

namespace shared {

    std::vector<std::string> QuotedStringSplitter(const std::string_view v, const char split_char = ',') {
        std::vector<std::string> results;
        const char *temp_start = v.data();

        bool in_quote = false;
        for (const char *p = temp_start; *p; p++) {
            if (*p == '"') {
                in_quote = !in_quote;
            } else if (*p == split_char && !in_quote) {
                results.emplace_back(temp_start, p - temp_start);
                temp_start = p + 1;
            }
        }
        results.emplace_back(temp_start);

        return results;
    }
}


#endif //MOBILITY_CPP_QUOTEDLINESPLITTER_HPP
