
#pragma once
#include "core_base.h"
#include <stdint.h>
#include "blake3/blake3.h"
#include <array>
#include <map>
#include "sparsepp/spp.h"
#include <iostream>
#include <functional>
#include <string>
using std::string;
namespace circuitpp2
{

  static void get_dummy_hash_u64_x1(uint8_t hash[AC_HASH_OUTPUT_SIZE], ACOperationType op_type, uint64_t input1)
  {
    memset(hash, 0, AC_HASH_OUTPUT_SIZE);
    uint16_t *ptr_u16 = ((uint16_t *)(hash));
    ptr_u16[0] = op_type;
    ptr_u16[15] = op_type;
    uint64_t *ptr_u64 = ((uint64_t *)(hash + 2));
    ptr_u64[0] = input1;
    ptr_u64[1] = input1;
    ptr_u64[2] = input1;
  }

  static void get_dummy_hash_u64_x2(uint8_t hash[AC_HASH_OUTPUT_SIZE], ACOperationType op_type, uint64_t input1, uint64_t input2)
  {
    memset(hash, 0, AC_HASH_OUTPUT_SIZE);
    uint16_t *ptr_u16 = ((uint16_t *)(hash));
    ptr_u16[0] = op_type;
    ptr_u16[15] = op_type;
    uint64_t *ptr_u64 = ((uint64_t *)(hash + 2));
    ptr_u64[0] = input1;
    ptr_u64[1] = input2;
    ptr_u64[2] = input1 ^ input2;
  }

  static void get_dummy_hash_u64_x3(uint8_t hash[AC_HASH_OUTPUT_SIZE], ACOperationType op_type, uint64_t input1, uint64_t input2, uint64_t input3)
  {
    memset(hash, 0, AC_HASH_OUTPUT_SIZE);
    uint16_t *ptr_u16 = ((uint16_t *)(hash));
    ptr_u16[0] = op_type;
    ptr_u16[15] = op_type;
    uint64_t *ptr_u64 = ((uint64_t *)(hash + 2));
    ptr_u64[0] = input1;
    ptr_u64[1] = input2;
    ptr_u64[2] = input3;
  }

  static void get_hash_unary_op(uint8_t hash[AC_HASH_OUTPUT_SIZE], ACOperationType op_type, uint8_t input_hash[AC_HASH_OUTPUT_SIZE])
  {
    blake3::blake3_hasher hasher;
    blake3::blake3_hasher_init(&hasher);
    blake3::blake3_hasher_update(&hasher, &op_type, sizeof(ACOperationType));
    blake3::blake3_hasher_update(&hasher, input_hash, AC_HASH_OUTPUT_SIZE);
    blake3::blake3_hasher_finalize(&hasher, hash, AC_HASH_OUTPUT_SIZE);
    //*((uint16_t *)hash) = op_type;
  }
  static void get_hash_unary_op_with_int(uint8_t hash[AC_HASH_OUTPUT_SIZE], ACOperationType op_type, uint8_t input_hash[AC_HASH_OUTPUT_SIZE], int input2)
  {
    blake3::blake3_hasher hasher;
    blake3::blake3_hasher_init(&hasher);
    blake3::blake3_hasher_update(&hasher, &op_type, sizeof(ACOperationType));
    blake3::blake3_hasher_update(&hasher, input_hash, AC_HASH_OUTPUT_SIZE);
    blake3::blake3_hasher_update(&hasher, &input2, sizeof(int));
    blake3::blake3_hasher_finalize(&hasher, hash, AC_HASH_OUTPUT_SIZE);
    //*((uint16_t *)hash) = op_type;
  }
  static void get_hash_binary_op(uint8_t hash[AC_HASH_OUTPUT_SIZE], ACOperationType op_type, uint8_t input1_hash[AC_HASH_OUTPUT_SIZE], uint8_t input2_hash[AC_HASH_OUTPUT_SIZE])
  {
    blake3::blake3_hasher hasher;
    blake3::blake3_hasher_init(&hasher);
    blake3::blake3_hasher_update(&hasher, &op_type, sizeof(ACOperationType));
    blake3::blake3_hasher_update(&hasher, input1_hash, AC_HASH_OUTPUT_SIZE);
    blake3::blake3_hasher_update(&hasher, input2_hash, AC_HASH_OUTPUT_SIZE);
    blake3::blake3_hasher_finalize(&hasher, hash, AC_HASH_OUTPUT_SIZE);
    //*((uint16_t *)hash) = op_type;
  }
  static void get_hash_ternary_op(uint8_t hash[AC_HASH_OUTPUT_SIZE], ACOperationType op_type, uint8_t input1_hash[AC_HASH_OUTPUT_SIZE], uint8_t input2_hash[AC_HASH_OUTPUT_SIZE], uint8_t input3_hash[AC_HASH_OUTPUT_SIZE])
  {
    blake3::blake3_hasher hasher;
    blake3::blake3_hasher_init(&hasher);
    blake3::blake3_hasher_update(&hasher, &op_type, sizeof(ACOperationType));
    blake3::blake3_hasher_update(&hasher, input1_hash, AC_HASH_OUTPUT_SIZE);
    blake3::blake3_hasher_update(&hasher, input2_hash, AC_HASH_OUTPUT_SIZE);
    blake3::blake3_hasher_update(&hasher, input3_hash, AC_HASH_OUTPUT_SIZE);
    blake3::blake3_hasher_finalize(&hasher, hash, AC_HASH_OUTPUT_SIZE);
    //*((uint16_t *)hash) = op_type;
  }

  static void get_hash_vector_inputs_op(uint8_t hash[AC_HASH_OUTPUT_SIZE], ACOperationType op_type, ACOperation *const *inputs, int num_inputs)
  {
    blake3::blake3_hasher hasher;
    blake3::blake3_hasher_init(&hasher);
    blake3::blake3_hasher_update(&hasher, &op_type, sizeof(ACOperationType));
    for (int i = 0; i < num_inputs; i++)
    {
      blake3::blake3_hasher_update(&hasher, inputs[i]->get_hash_u8(), AC_HASH_OUTPUT_SIZE);
    }
    blake3::blake3_hasher_finalize(&hasher, hash, AC_HASH_OUTPUT_SIZE);
    //*((uint16_t *)hash) = op_type;
  }
  class ACConstantOp : public ACOperation
  {
  public:
    ACConstantOp(uint64_t constant)
    {
      this->constant = constant;
      get_dummy_hash_u64_x1(hash, AC_OP_CONSTANT, constant);
    }
    ACOperationType op_type() const
    {
      return AC_OP_CONSTANT;
    }
    int num_constant_inputs(){
      return 1;
    }
    uint64_t get_nth_constant_input(int n){
      if(n == 0){
        return constant;
      }else{
        throw std::runtime_error("Invalid constant input index");
      }
    }
    int num_inputs()
    {
      return 0;
    }
    int num_outputs()
    {
      return 1;
    }
    ACOperationStackType stack_type() const
    {
      return AC_STACK_TYPE_TARGET;
    }
    ACOperation *get_nth_input(int n)
    {
      return nullptr;
    }
    uint64_t get_constant()
    {
      return constant;
    }

  protected:
    uint64_t constant;
  };
  class ACInputOp : public ACOperation
  {
  public:
    ACInputOp(uint64_t input_index)
    {
      this->input_index = input_index;
      get_dummy_hash_u64_x1(hash, AC_OP_INPUT, input_index);
    }
    ACOperationType op_type() const
    {
      return AC_OP_INPUT;
    }
    ACOperation *get_nth_input(int n)
    {
      return nullptr;
    }

    int num_constant_inputs(){
      return 1;
    }
    uint64_t get_nth_constant_input(int n){
      if(n == 0){
        return input_index;
      }else{
        throw std::runtime_error("Invalid constant input index");
      }
    }
    int num_inputs()
    {
      return 0;
    }
    int num_outputs()
    {
      return 1;
    }
    ACOperationStackType stack_type() const
    {
      return AC_STACK_TYPE_TARGET;
    }
    void set_input_index(uint64_t index)
    {
      this->input_index = index;
    }
    uint64_t get_input_index()
    {
      return input_index;
    }
    void get_hash_for_op_map(std::array<uint8_t, AC_HASH_OUTPUT_SIZE> &hash)
    {
      hash.fill(0);
      ACOperationType ot = op_type();
      uint16_t *ptr_u16 = ((uint16_t *)(hash.data()));
      ptr_u16[0] = op_type();
      uint64_t *ptr_u64 = ((uint64_t *)(hash.data() + 2));
      ptr_u64[0] = input_index;
    }
    OPIndex get_target_id()
    {
      return input_index;
    }
    OPIndex get_global_target_id()
    {
      return input_index;
    }

  protected:
    uint64_t input_index;
  };

  class ACGeneratorInputOp : public ACOperation
  {
  public:
    ACGeneratorInputOp(uint64_t generator_id, uint64_t global_generator_input_index)
    {
      this->global_generator_input_index = global_generator_input_index;
      get_dummy_hash_u64_x2(hash, AC_OP_GENERATOR_INPUT, global_generator_input_index, generator_id);
    }
    ACOperationType op_type() const
    {
      return AC_OP_GENERATOR_INPUT;
    }
    ACOperation *get_nth_input(int n)
    {
      return nullptr;
    }

    int num_constant_inputs(){
      return 2;
    }
    uint64_t get_nth_constant_input(int n){
      if(n == 0){
        return generator_id;
      }else if(n == 1){
        return global_generator_input_index;
      }else{
        throw std::runtime_error("Invalid constant input index");
      }
    }
    int num_inputs()
    {
      return 0;
    }
    int num_outputs()
    {
      return 1;
    }
    ACOperationStackType stack_type() const
    {
      return AC_STACK_TYPE_TARGET;
    }
    uint64_t get_global_generator_input_index()
    {
      return global_generator_input_index;
    }
    void get_hash_for_op_map(std::array<uint8_t, AC_HASH_OUTPUT_SIZE> &hash)
    {
      hash.fill(0);
      ACOperationType ot = op_type();
      uint16_t *ptr_u16 = ((uint16_t *)(hash.data()));
      ptr_u16[0] = op_type();
      uint64_t *ptr_u64 = ((uint64_t *)(hash.data() + 2));
      ptr_u64[0] = global_generator_input_index;
    }
    uint64_t get_generator_id()
    {
      return generator_id;
    }

  protected:
    uint64_t generator_id = 0;
    uint64_t global_generator_input_index = 0;
  };

  class TargetAtOperation : public ACOperation
  {
  public:
    TargetAtOperation(ACOperation *input, int index)
    {
      if(input->stack_type() != AC_STACK_TYPE_TARGET_VEC){
        throw std::runtime_error("TargetAtOperation input must be a vector operation.");
      }
      this->input = input;
      this->index = index;
      get_hash_unary_op_with_int(hash, AC_OP_TARGET_AT, input->get_hash_u8(), index);
    }

    int num_constant_inputs(){
      return 1;
    }
    uint64_t get_nth_constant_input(int n){
      if(n == 0){
        return index;
      }else{
        throw std::runtime_error("Invalid constant input index");
      }
    }
    ACOperation *get_nth_input(int n)
    {
      if (n == 0)
      {
        return input;
      }
      return nullptr;
    }
    ACOperationType op_type() const
    {
      return AC_OP_TARGET_AT;
    }
    int num_inputs()
    {
      return 1;
    }
    int num_outputs()
    {
      return 1;
    }

    ACOperationStackType stack_type() const
    {
      return AC_STACK_TYPE_TARGET;
    }
    ACOperation *get_input()
    {
      return input;
    }
    int get_index()
    {
      return index;
    }

  protected:
    ACOperation *input;
    int index;
  };

  template <ACOperationType T_OPERATION_TYPE>
  class UnaryOperation : public ACOperation
  {
  public:
    UnaryOperation(ACOperation *input)
    {
      this->input = input;
      get_hash_unary_op(hash, T_OPERATION_TYPE, input->get_hash_u8());
    }
    ACOperation *get_nth_input(int n)
    {
      if (n == 0)
      {
        return input;
      }
      return nullptr;
    }
    ACOperationType op_type() const
    {
      return T_OPERATION_TYPE;
    }
    int num_inputs()
    {
      return 1;
    }
    int num_outputs()
    {
      return 1;
    }

    ACOperationStackType stack_type() const
    {
      return AC_STACK_TYPE_TARGET;
    }
    ACOperation *get_input()
    {
      return input;
    }

  protected:
    ACOperation *input;
  };

  template <ACOperationType T_OPERATION_TYPE>
  class BinaryOperation : public ACOperation
  {
  public:
    BinaryOperation(ACOperation *input1, ACOperation *input2)
    {
      this->input1 = input1;
      this->input2 = input2;
      get_hash_binary_op(hash, T_OPERATION_TYPE, input1->get_hash_u8(), input2->get_hash_u8());
    }
    ACOperation *get_nth_input(int n)
    {
      if (n == 0)
      {
        return input1;
      }
      else if (n == 1)
      {
        return input2;
      }
      return nullptr;
    }
    ACOperationType op_type() const
    {
      return T_OPERATION_TYPE;
    }
    int num_inputs()
    {
      return 2;
    }
    int num_outputs()
    {
      return 1;
    }
    ACOperationStackType stack_type() const
    {
      return AC_STACK_TYPE_TARGET;
    }

  protected:
    ACOperation *input1;
    ACOperation *input2;
  };

  template <ACOperationType T_OPERATION_TYPE>
  class TernaryOperation : public ACOperation
  {
  public:
    TernaryOperation(ACOperation *input1, ACOperation *input2, ACOperation *input3)
    {
      this->input1 = input1;
      this->input2 = input2;
      this->input3 = input3;
      get_hash_ternary_op(hash, T_OPERATION_TYPE, input1->get_hash_u8(), input2->get_hash_u8());
    }
    ACOperation *get_nth_input(int n)
    {
      if (n == 0)
      {
        return input1;
      }
      else if (n == 1)
      {
        return input2;
      }
      else if (n == 2)
      {
        return input2;
      }
      return nullptr;
    }
    ACOperationType op_type() const
    {
      return T_OPERATION_TYPE;
    }
    int num_inputs()
    {
      return 3;
    }
    int num_outputs()
    {
      return 1;
    }
    ACOperationStackType stack_type() const
    {
      return AC_STACK_TYPE_TARGET;
    }

  protected:
    ACOperation *input1;
    ACOperation *input2;
    ACOperation *input3;
  };


  template <ACOperationType T_OPERATION_TYPE, int NUM_OUTPUTS>
  class VectorInputFixedOutputLengthOperation : public ACOperation
  {
  public:
    VectorInputFixedOutputLengthOperation(std::vector<ACOperation *> inputs)
    {
      this->inputs = inputs;
      get_hash_vector_inputs_op(hash, T_OPERATION_TYPE, inputs.data(), inputs.size());
    }
    ACOperation *get_nth_input(int n)
    {
      if (n < inputs.size())
      {
        return inputs[n];
      }
      return nullptr;
    }
    ACOperationType op_type() const
    {
      return T_OPERATION_TYPE;
    }
    int num_inputs()
    {
      return inputs.size();
    }
    int num_outputs()
    {
      return NUM_OUTPUTS;
    }

    ACOperationStackType stack_type() const
    {
      return AC_STACK_TYPE_TARGET_VEC;
    }
    std::vector<ACOperation *> get_inputs()
    {
      return inputs;
    }

  protected:
    std::vector<ACOperation *> inputs;
  };

  template <ACOperationType T_OPERATION_TYPE>
  class VectorInputSingleOutputOperation : public ACOperation
  {
  public:
    VectorInputSingleOutputOperation(std::vector<ACOperation *> inputs)
    {
      this->inputs = inputs;
      get_hash_vector_inputs_op(hash, T_OPERATION_TYPE, inputs.data(), inputs.size());
    }
    ACOperation *get_nth_input(int n)
    {
      if (n < inputs.size())
      {
        return inputs[n];
      }
      return nullptr;
    }
    ACOperationType op_type() const
    {
      return T_OPERATION_TYPE;
    }
    int num_inputs()
    {
      return inputs.size();
    }
    int num_outputs()
    {
      return 1;
    }

    ACOperationStackType stack_type() const
    {
      return AC_STACK_TYPE_TARGET;
    }
    std::vector<ACOperation *> get_inputs()
    {
      return inputs;
    }

  protected:
    std::vector<ACOperation *> inputs;
  };

  template <ACOperationType T_OPERATION_TYPE>
  class SingleInputVectorOutputOperation : public ACOperation
  {
  public:
    SingleInputVectorOutputOperation(ACOperation *input, int output_length)
    {
      this->input = input;
      this->output_length = output_length;
      get_hash_unary_op(hash, T_OPERATION_TYPE, input->get_hash_u8());
    }
    ACOperation *get_nth_input(int n)
    {
      if (n == 0)
      {
        return input;
      }
      return nullptr;
    }
    ACOperationType op_type() const
    {
      return T_OPERATION_TYPE;
    }
    int num_inputs()
    {
      return 1;
    }
    int num_outputs()
    {
      return output_length;
    }

    ACOperationStackType stack_type() const
    {
      return AC_STACK_TYPE_TARGET_VEC;
    }
    ACOperation *get_input()
    {
      return input;
    }

  protected:
    ACOperation *input;
    int output_length;
  };

template <typename Felt> 
  class ExampleGeneratorRegistry {
  public:
    ExampleGeneratorRegistry() {}
    void add_generator(ACGeneratorBase<Felt> * generator){
      this->generator_map[generator->getId()] = generator;
    }
    ACGeneratorBase<Felt> *get_generator_by_id(uint64_t id) {
      if (this->generator_map.contains(id)) {
        return this->generator_map[id];
      }

      throw std::runtime_error("missing generator");
    }

  private:
    std::map<uint64_t, ACGeneratorBase<Felt> *> generator_map;
  };
/*
  template <ACOperationType T_OPERATION_TYPE>
  class CustomGeneratorOperation : public ACOperation
  {
  public:
    CustomGeneratorOperation(std::vector<ACOperation *> inputs, int output_count)
    {
      this->inputs = inputs;
      this->output_count = output_count;
      get_hash_vector_inputs_op(hash, T_OPERATION_TYPE, inputs.data(), inputs.size());
    }
    ACOperation *get_nth_input(int n)
    {
      if (n < inputs.size())
      {
        return inputs[n];
      }
      return nullptr;
    }
    ACOperationType op_type() const
    {
      return T_OPERATION_TYPE;
    }
    int num_inputs()
    {
      return inputs.size();
    }
    int num_outputs()
    {
      return output_count;
    }
    void set_num_outputs(int count)
    {
      output_count = count;
    }

    ACOperationStackType stack_type() const
    {
      return AC_STACK_TYPE_TARGET_VEC;
    }
    std::vector<ACOperation *> get_inputs()
    {
      return inputs;
    }

  protected:
    std::vector<ACOperation *> inputs;
    int output_count = 0;
  };*/
} // namespace circuitpp
