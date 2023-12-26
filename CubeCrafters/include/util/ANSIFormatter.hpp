#ifndef ANSI_FORMATTER_HPP
#define ANSI_FORMATTER_HPP

#include <format>
#include <memory>
#include <string>

namespace ANSIFormatter
{
    std::string replaceAll(std::string str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;

        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }

        return str;
    }

    template<typename... Args>
    std::string format(std::string formatted, Args&&... args)
    {
        formatted = std::vformat(formatted, std::make_format_args(args...));

        formatted = replaceAll(formatted, "&f", "\033[1;37m");
        formatted = replaceAll(formatted, "&r", "\033[0m");
        formatted = replaceAll(formatted, "&0", "\033[1;30m");
        formatted = replaceAll(formatted, "&1", "\033[1;34m");
        formatted = replaceAll(formatted, "&2", "\033[1;32m");
        formatted = replaceAll(formatted, "&3", "\033[1;36m");
        formatted = replaceAll(formatted, "&4", "\033[1;31m");
        formatted = replaceAll(formatted, "&5", "\033[1;35m");
        formatted = replaceAll(formatted, "&6", "\033[1;33m");

        return formatted;
    }

    std::string deFormat(std::string formatted)
    {
        formatted = replaceAll(formatted, "\033[1;37m", "");
        formatted = replaceAll(formatted, "\033[0m", "");
        formatted = replaceAll(formatted, "\033[1;30m", "");
        formatted = replaceAll(formatted, "\033[1;34m", "");
        formatted = replaceAll(formatted, "\033[1;32m", "");
        formatted = replaceAll(formatted, "\033[1;36m", "");
        formatted = replaceAll(formatted, "\033[1;31m", "");
        formatted = replaceAll(formatted, "\033[1;35m", "");
        formatted = replaceAll(formatted, "\033[1;33m", "");

        return formatted;
    }
}

#endif // !ANSI_FORMATTER_HPP