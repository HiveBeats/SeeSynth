#pragma once

#include <vector>
#include "Oscillator.h"

class Adder
{
private:
    /* data */
public:
    Adder(/* args */);
    ~Adder();
    std::vector<float> & SumOscillators(const std::vector<Oscillator*> & oscillators, float duration);
};

Adder::Adder(/* args */)
{
}

Adder::~Adder()
{
}
