#include <string>
#include <iostream>
#include <set>
#include <cstdlib>
#include <random>

#ifndef Helper_hpp
#define Helper_hpp

using namespace std;

namespace BotManager
{
    std::string capitalizeString(std::string s);
    void printf_debug(std::string s);
    std::string escape(const char *src, const std::set<char> escapee, const char marker);

    class Randomer
    {
        // random seed by default
        std::mt19937 gen_;
        std::uniform_int_distribution<size_t> dist_;

    public:
        /*  ... some convenient ctors ... */

        Randomer(size_t min, size_t max, unsigned int seed = std::random_device{}())
            : gen_{seed}, dist_{min, max}
        {
        }

        // if you want predictable numbers
        void SetSeed(unsigned int seed)
        {
            gen_.seed(seed);
        }

        size_t operator()()
        {
            return dist_(gen_);
        }
    };

} // namespace BotManager

#endif