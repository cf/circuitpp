#pragma once

namespace dummy {
class Dummy {
  public:
    Dummy(int value){
      this->value = value;
    }
    int get_value() {
      return this->value;
    }
  protected:
    int value;
};
} // namespace dummy