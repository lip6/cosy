/* Copyright 2017 Hakan Metin - All rights reserved */

#ifndef INCLUDE_DSB_PRINTER_H_
#define INCLUDE_DSB_PRINTER_H_

#include <iostream>
#include <functional>
#include <iomanip>
#include <string>
#include <vector>
#include <cassert>
#include <cstring>

#define LEFT 40

namespace cosy {
class Printer {
 private:
        static const int sz = 80;

 public:
        static void printSection(const char * string) {
            std::string comment("c ");
            int len = sz - strlen(string) - comment.size();
            std::cout << comment;
            std::cout <<  std::setfill('=') << std::setw(len/2) << "=";
            std::cout << string;
            std::cout <<  std::setfill('=') << std::setw((len/2 + len%2)-1)
                      << "=";
            std::cout << std::endl;
        }

        static void print(const char * string) {
            std::string comment("c ");
            std::cout << comment << string << std::endl;
        }

        template<typename T>
        static void print(T value) {
            std::string comment("c ");
            std::cout << comment << value << std::endl;
        }

        template<typename T>
        static void printStat(const char * string, T value,
                              const char *end = "") {
            std::string comment("c ");

            int len = sz - strlen(string) - comment.size() - LEFT;
            std::cout << comment << string;
            std::cout <<  std::setfill(' ') << std::setw(len) << " ";
            std::cout << value << " " << end;
            std::cout << std::endl;
        }

        template<typename T>
        static void printStat(const char * string, T value, T total,
                              const char *end = "") {
            static int space = 7;
            std::string comment("c ");

            int len = sz - strlen(string) - comment.size() - LEFT;
            std::cout << comment << string;
            std::cout <<  std::setfill(' ') << std::setw(len) << " ";
            std::cout << std::left << std::setw(space) << value
                      << " / " << total << " == " <<
                (value/static_cast<double>(total))*100 << "%" << end;
            std::cout << std::endl;
        }

        template<typename T>
        static void print(const char * string, T value,
                          const char *end = "") {
            std::string comment("c ");
            std::cout << comment << string << " ";
            std::cout << value << " " << end << std::endl;
        }
        template<class T>
        static void print(const char * s, const std::vector<T>& vec) {
            std::cout << "c " << s << ": ";
            for (const T e : vec)
                std::cout << e << " ";
            std::cout << std::endl;
        }
};
}  // namespace cosy

#endif  // INCLUDE_DSB_PRINTER_H_

/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
