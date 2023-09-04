#pragma once
#include <vector>
class Effect {
  private:
    /* data */
  public:
    Effect(/* args */){};
    ~Effect(){};
    virtual void OnSetNote(){};
    virtual void OnUnsetNote(){};
    //virtual void RetriggerState(){};
    virtual void Process(std::vector<float>& samples){};
};
