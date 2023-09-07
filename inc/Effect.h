#pragma once
#include <vector>
class Effect {
  private:
    /* data */
  public:
    Effect(/* args */){};
    ~Effect(){};
    virtual void Trigger(){};
    virtual void Release(){};
    virtual void Process(std::vector<float>& samples){};
};
