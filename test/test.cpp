#include <random>
#include <chrono>
#include <algorithm>
#include <iostream>

namespace random
{ // 初始化
    static std::mt19937 &getGenerator()
    {
        static std::mt19937 generator([]()
                                      {
            std::random_device rd;
            auto seed = rd() ^ static_cast<unsigned int>(
                std::chrono::system_clock::now().time_since_epoch().count());
            std::mt19937 gen(seed);
            gen.discard(10); // 预热
            return gen; }());
        return generator;
    }
    // 获取随机数
    int getInt(int min, int max)
    {
        if (min > max)
            std::swap(min, max);
        std::uniform_int_distribution<int> dist(min, max);
        return dist(getGenerator());
    }

    double getDouble(double min, double max)
    {
        if (min > max)
            std::swap(min, max);
        std::uniform_real_distribution<double> dist(min, max);
        return dist(getGenerator());
    }

    double getDoubleOpen(double min, double max)
    {
        if (min > max)
            std::swap(min, max);
        double result;
        do
        {
            result = getDouble(min, max);
        } while (result == max);
        return result;
    }
}
int main()
{
    std::cout << random::getInt(1, 230);
}