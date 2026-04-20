#pragma once

#include <string_view>

namespace plugini::strings {

/**
 * Minimal wildcard matcher supporting '*' (any sequence) and '?' (single char).
 * Implemented iteratively to avoid pulling in any external string-utility dependency.
 */
inline bool matchesWildCard(std::string_view text, std::string_view pattern)
{
    size_t t = 0;
    size_t p = 0;
    size_t starIdx = std::string_view::npos;
    size_t matchIdx = 0;

    while (t < text.size())
    {
        if (p < pattern.size() && (pattern[p] == '?' || pattern[p] == text[t]))
        {
            ++t;
            ++p;
        }
        else if (p < pattern.size() && pattern[p] == '*')
        {
            starIdx = p++;
            matchIdx = t;
        }
        else if (starIdx != std::string_view::npos)
        {
            p = starIdx + 1;
            t = ++matchIdx;
        }
        else
        {
            return false;
        }
    }

    while (p < pattern.size() && pattern[p] == '*') { ++p; }
    return p == pattern.size();
}

} // namespace plugini::strings
