#pragma once
#include <vector>
class IEffect {
  private:
    /* data */
  public:
    virtual void Trigger() = 0;
    virtual void Release() = 0;
    virtual void Process(std::vector<float>& samples) = 0;
};
