
#pragma once
#include "cored.h"

namespace circuitpp2
{

  struct FGeneratorSerializedHeader {
    uint64_t generator_id;
    OPIndex op_dependency_id;
    OPVecLength target_input_count;
    OPVecLength constant_input_count;
  };

  struct FGenerator {
    public:
    uint64_t generator_id;
    OPIndex op_dependency_id;
    std::vector<uint64_t> constant_inputs;
    std::vector<OPIndex> target_inputs;
    size_t size_bytes() const {
      return sizeof(FGeneratorSerializedHeader) + constant_inputs.size()*sizeof(OPIndex) + target_inputs.size()*sizeof(uint64_t);
    }
    size_t write(uint8_t * buffer) const {

      FGeneratorSerializedHeader * header = (FGeneratorSerializedHeader*)buffer;
      header->generator_id = generator_id;
      header->op_dependency_id = op_dependency_id;
      header->target_input_count = target_inputs.size();
      header->constant_input_count = constant_inputs.size();

      uint8_t * ptr = buffer + sizeof(FGeneratorSerializedHeader);
      for (auto target : target_inputs) {
        *((OPIndex*)ptr) = target;
        ptr += sizeof(OPIndex);
      }
      for (auto constant : constant_inputs) {
        *((uint64_t*)ptr) = constant;
        ptr += sizeof(uint64_t);
      }
      return size_bytes();
    }
    size_t read(const uint8_t * buffer) {
      const uint8_t * ptr = buffer;
      const FGeneratorSerializedHeader * header = (FGeneratorSerializedHeader*)buffer;
      generator_id = header->generator_id;
      op_dependency_id = header->op_dependency_id;
      target_inputs.resize(header->target_input_count);
      constant_inputs.resize(header->constant_input_count);
      ptr += sizeof(FGeneratorSerializedHeader);
      for (size_t i = 0; i < target_inputs.size(); i++) {
        target_inputs[i] = *((OPIndex*)ptr);
        ptr += sizeof(OPIndex);
      }
      for (size_t i = 0; i < constant_inputs.size(); i++) {
        constant_inputs[i] = *((uint64_t*)ptr);
        ptr += sizeof(uint64_t);
      }
      return size_bytes();
    }
  };

  struct FOperationSerializedHeader {
    ACOperationType op_type;
    OPVecLength target_input_count;
    OPVecLength constant_input_count;
  };

  struct FOperation {
    ACOperationType op_type;
    std::vector<uint64_t> constant_inputs;
    std::vector<OPIndex> target_inputs;
    size_t size_bytes() const {
      return sizeof(FOperationSerializedHeader) + constant_inputs.size()*sizeof(OPIndex) + target_inputs.size()*sizeof(uint64_t);
    }
    size_t write(uint8_t * buffer) const {

      FOperationSerializedHeader * header = (FOperationSerializedHeader*)buffer;
      header->op_type = op_type;
      header->target_input_count = target_inputs.size();
      header->constant_input_count = constant_inputs.size();

      uint8_t * ptr = buffer + sizeof(FOperationSerializedHeader);
      for (auto target : target_inputs) {
        *((OPIndex*)ptr) = target;
        ptr += sizeof(OPIndex);
      }
      for (auto constant : constant_inputs) {
        *((uint64_t*)ptr) = constant;
        ptr += sizeof(uint64_t);
      }
      return size_bytes();
    }
    size_t read(const uint8_t * buffer) {
      const uint8_t * ptr = buffer;
      const FOperationSerializedHeader * header = (FOperationSerializedHeader*)buffer;
      op_type = header->op_type;
      target_inputs.resize(header->target_input_count);
      constant_inputs.resize(header->constant_input_count);
      ptr += sizeof(FOperationSerializedHeader);
      for (size_t i = 0; i < target_inputs.size(); i++) {
        target_inputs[i] = *((OPIndex*)ptr);
        ptr += sizeof(OPIndex);
      }
      for (size_t i = 0; i < constant_inputs.size(); i++) {
        constant_inputs[i] = *((uint64_t*)ptr);
        ptr += sizeof(uint64_t);
      }
      return size_bytes();
    }
  };
  struct FCircuitAssertionSerializedHeader {
    OPIndex left;
    OPIndex right;
    OPVecLength message_length;
  };
  struct FCircuitAssertion {
    OPIndex left;
    OPIndex right;
    std::string message;
    size_t size_bytes() const {
      return sizeof(FCircuitAssertionSerializedHeader) + message.size();
    }
    size_t write(uint8_t * buffer) const {
      FCircuitAssertionSerializedHeader * header = (FCircuitAssertionSerializedHeader*)buffer;
      header->left = left;
      header->right = right;
      header->message_length = message.size();
      memcpy(buffer + sizeof(FCircuitAssertionSerializedHeader), message.c_str(), message.size());
      return size_bytes();
    }
    size_t read(const uint8_t * buffer) {
      const FCircuitAssertionSerializedHeader * header = (FCircuitAssertionSerializedHeader*)buffer;
      left = header->left;
      right = header->right;
      message = std::string((const char*)(buffer + sizeof(FCircuitAssertionSerializedHeader)), header->message_length);
      return size_bytes();
    }
  };
  struct FCircuitSerializedHeader {
    OPIndex input_target_count;
    OPIndex generator_target_count;
    OPVecLength operation_count;
    OPVecLength vector_operation_count;
    OPVecLength assertion_count;
    OPVecLength generator_count;
    OPVecLength output_count;
  };
  class FCircuit {
  public:
    OPIndex input_target_count = 0;
    OPIndex generator_target_count = 0;
    OPVecLength vector_operation_count = 0;
    std::vector<FOperation> operations;
    std::vector<FGenerator> generators;

    std::vector<FCircuitAssertion> assertions;
    std::vector<OPIndex> output_ops;

    size_t size_bytes() const {
      size_t size = sizeof(FCircuitSerializedHeader);
      for (auto & op : operations) {
        size += op.size_bytes();
      }
      for (auto & generator : generators) {
        size += generator.size_bytes();
      }
      for (auto & assertion : assertions) {
        size += assertion.size_bytes();
      }
      size += output_ops.size()*sizeof(OPIndex);
      return size;
    }
    size_t write(uint8_t * buffer) const {
      uint8_t * start_pointer = buffer;
      FCircuitSerializedHeader * header = (FCircuitSerializedHeader*)buffer;
      header->input_target_count = input_target_count;
      header->generator_target_count = generator_target_count;
      header->operation_count = operations.size();
      header->vector_operation_count = vector_operation_count;
      header->assertion_count = assertions.size();
      header->generator_count = generators.size();
      header->output_count = output_ops.size();
      uint8_t * ptr = buffer + sizeof(FCircuitSerializedHeader);
      for (auto & op : operations) {
        ptr += op.write(ptr);
      }
      for (auto & assertion : assertions) {
        ptr += assertion.write(ptr);
      }
      for (auto & generator : generators) {
        ptr += generator.write(ptr);
      }
      for (auto output : output_ops) {
        *((OPIndex*)ptr) = output;
        ptr += sizeof(OPIndex);
      }
      return ptr - start_pointer;
    }
    size_t read(const uint8_t * buffer) {
      const uint8_t * ptr = buffer;
      const FCircuitSerializedHeader * header = (FCircuitSerializedHeader*)buffer;
      input_target_count = header->input_target_count;
      generator_target_count = header->generator_target_count;
      operations.resize(header->operation_count);
      assertions.resize(header->assertion_count);
      generators.resize(header->generator_count);
      output_ops.resize(header->output_count);
      ptr += sizeof(FCircuitSerializedHeader);
      for (size_t i = 0; i < operations.size(); i++) {
        ptr += operations[i].read(ptr);
      }
      for (size_t i = 0; i < assertions.size(); i++) {
        ptr += assertions[i].read(ptr);
      }
      for (size_t i = 0; i < generators.size(); i++) {
        ptr += generators[i].read(ptr);
      }
      for (size_t i = 0; i < output_ops.size(); i++) {
        output_ops[i] = *((OPIndex*)ptr);
        ptr += sizeof(OPIndex);
      }
      return ptr - buffer;
    }
  };
}