#ifndef HallCurrentVector_H
#define HallCurrentVector_H
#include <vector>

class HallCurrentVector
{
public:
    HallCurrentVector();
    void push_back(float current);
    float getavg();
    void clear();

private:
    std::vector<float> hallCurrents;
};

#endif // HallCurrentVector_H
