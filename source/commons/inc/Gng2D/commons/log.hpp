#pragma once
#include <iostream>
#include <sstream>
#include <string_view>

namespace Gng2D {
struct LOG
{
    template <typename... T>
    static void TRACE(T... msg)
    {
        out << TRACE_TXT;
        ((out << " " << msg), ...);
        out << "\n";
    }

    template <typename... T>
    static void DEBUG(T... msg)
    {
        out << DEBUG_TXT;
        ((out << " " << msg), ...);
        out << "\n";
    }

    template <typename... T>
    static void OK(T... msg)
    {
        out << OK_TXT;
        ((out << " " << msg), ...);
        out << "\n";
    }

    template <typename... T>
    static void INFO(T... msg)
    {
        out << INFO_TXT;
        ((out << " " << msg), ...);
        out << "\n";
    }

    template <typename... T>
    static void WARN(T... msg)
    {
        out << WARN_TXT;
        ((out << " " << msg), ...);
        out << "\n";
    }

    template <typename... T>
    static void ERROR(T... msg)
    {
        out << ERROR_TXT;
        ((out << " " << msg), ...);
        out << "\n";
    }

    template <typename... T>
    static void FATAL(T... msg)
    {
        std::stringstream out;
        out << FATAL_TXT;
        ((out << " " << msg), ...);
        out << std::endl;
        throw std::runtime_error(out.str());
    }

  private:
    inline static auto& out = std::cout;

    static constexpr std::string_view TRACE_TXT = " [ TRACE ] ";
    static constexpr std::string_view DEBUG_TXT = " [ DEBUG ] ";
    static constexpr std::string_view OK_TXT    = " [ OK    ] ";
    static constexpr std::string_view INFO_TXT  = " [ INFO  ] ";
    static constexpr std::string_view WARN_TXT  = " [ WARN  ] ";
    static constexpr std::string_view ERROR_TXT = " [ ERROR ] ";
    static constexpr std::string_view FATAL_TXT = " [ FATAL ] ";
};
} // namespace Gng2D
