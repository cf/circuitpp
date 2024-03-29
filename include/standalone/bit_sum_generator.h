
#pragma once
#include <string>
#include <xxhash_cx.h>
#include <circuitpp/core_base.h>
#include <circuitpp/builder.h>

namespace circuitpp2
{

  template <typename Felt> class BitSumGenerator : public ACGeneratorBase<Felt> {
    public:
      AC_NAMED_GENERATOR("BitSum")


      int num_outputs(const std::vector<uint64_t>& constant_inputs) override {
        return (int)constant_inputs[0];
      }
      void evaluate(const std::vector<uint64_t>& constant_inputs, std::vector<Felt>& inputs, std::vector<Felt>& outputs) override{
        uint64_t num_bits = constant_inputs[0];

        uint64_t x = (uint64_t)inputs[0];
        for(uint64_t i=0;i<num_bits;i++){
          // get the i-th bit
          outputs[i] = Felt( (x >> i) & 1ULL );
        }
      }
      std::vector<ACOperation *> constrain(ACCircuitBuilder<Felt> * builder, ACGeneratorDefinition * definition) override{
        uint64_t num_bits = definition->constant_inputs[0];

        std::vector<ACOperation *> outputs;
        outputs.resize(num_bits);


        auto one = builder->constant(1);
        auto zero = builder->constant(0);

        // add an input for each bit and ensure it is either 0 or 1
        for(uint64_t i=0;i<num_bits;i++){
          outputs[i] = builder->add_generator_input(this->getId());
          //ensure it is either 0 or 1 using: x*(1-x) == 0
          builder->connect(builder->mul(outputs[i], builder->sub(one, outputs[i])), zero);
        }

        auto sum = outputs[0]; // start the sum at output 0
          // sum up all the bits
        for(uint64_t i=1;i<num_bits;i++){
          // shifted = b_i * 2^i
          auto shifted = builder->mul(outputs[i], builder->constant(1ULL << i));
          // sum += b_i * 2^i
          sum = builder->add(sum, shifted);
        }


        // ensure input == sum, meaning that our bits are correct
        builder->connect(definition->inputs[0], sum);
        return outputs;
      }
      BitSumGenerator(){}
  };
}