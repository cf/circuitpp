#pragma once
#include <circuitpp/builder.h>
#include <circuitpp/cored.h>
#include <circuitpp/eval.h>

using namespace circuitpp2;

void test_multiplication_circuit(){
  ACCircuitBuilder<GoldilocksFelt> builder;

  auto a = builder.add_input();
  auto b = builder.add_input();


  // c = a*b
  auto c = builder.mul(a,b);

  builder.register_output(c);
  FCircuit circuit = builder.build();
  
  // a = 1337, b = 222
  std::vector<GoldilocksFelt> inputs = {1337,222};
  auto x = CircuitEvaluatorGoldilocks(builder.get_registry(), &circuit, inputs);


  // c should be 1337*222 = 296814
  std::cout << "test_bit_mul" << std::endl;
  auto results = x.eval_circuit();
  for(auto result : results){
    std::cout << result.to_string() << std::endl;
  }

}
