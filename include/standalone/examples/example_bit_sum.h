#pragma once
#include <circuitpp/builder.h>
#include <circuitpp/cored.h>
#include <circuitpp/eval.h>
#include <standalone/bit_sum_generator.h>


void test_bit_sum_circuit(){
  circuitpp2::ACCircuitBuilder<GoldilocksFelt> builder;

  // start: add generators
  auto bsg = new BitSumGenerator<GoldilocksFelt>();
  builder.add_generator(bsg);
  // end: add generators

  auto a = builder.add_input();
  auto b = builder.add_input();

  // 32-bit decomposition
  auto bits = builder.new_generator(bsg->getId(), {a}, {32});

  auto non_zero_bit_count = bits[0];
  for(int i=1;i<bits.size();i++){
    non_zero_bit_count = builder.add(non_zero_bit_count, bits[i]);
  }

  builder.connect(non_zero_bit_count, builder.constant(6), "must have 5 non zero bits");

  
  auto e = ACHashTarget{a,a,a,a};
  auto f = ACHashTarget{b,b,b,b};

  auto h = builder.two_to_one(e,f);

  // hash
  builder.register_output(h[0]);
  builder.register_output(h[1]);
  builder.register_output(h[2]);
  builder.register_output(h[3]);

  builder.register_output(bits[0]);
  FCircuit circuit = builder.build();
  // 1337 = 0b10100111001 -> 6 zero bits
  // 223 = 0b11011111 -> 7 non zero bits
  auto x = CircuitEvaluatorGoldilocks(builder.get_registry(), &circuit, {1337,223});

  std::cout << "test_bit_sum_circuit" << std::endl;
  auto results = x.eval_circuit();
  for(auto result : results){
    std::cout << result.to_string() << std::endl;
  }



}
