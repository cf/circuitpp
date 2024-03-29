#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <circuitpp/builder.h>
#include <circuitpp/cored.h>
#include <circuitpp/eval.h>
#include <circuitpp/example_generators.h>

using namespace circuitpp2;


std::string get_operator_str(ACOperationType op_type)
{
  switch (op_type)
  {
  case AC_OP_ADD:
    return "+";
  case AC_OP_SUB:
    return "-";
  case AC_OP_MUL:
    return "*";
  case AC_OP_DIV:
    return "/";
  case AC_OP_NEG:
    return "!";
  case AC_OP_EQ:
    return "==";
  default:
    return "";
  }
}
void print_op(ACOperation *op, ACCircuitBuilder<GoldilocksFelt> *builder)
{
  ACOperationType op_type = op->op_type();
  switch(op_type){
    case AC_OP_CONSTANT: {
      std::cout << ((ACConstantOp *)op)->get_constant();
      break;
    }
    case AC_OP_INPUT: { 
      std::cout << "x_" << ((ACInputOp *)op)->get_input_index();
      break;
    }
    case AC_OP_ADD:
    case AC_OP_SUB:
    case AC_OP_MUL:
    case AC_OP_DIV:
    case AC_OP_EQ: {
      // binary ops
      auto left = op->get_nth_input(0);
      auto right = op->get_nth_input(1);
      std::cout << "(";
      print_op(left, builder);
      std::cout << get_operator_str(op_type);
      print_op(right, builder);
      std::cout << ")";
      break;
    }
    case AC_OP_NEG: {
      // unary ops
      std::cout << "(";
      std::cout << get_operator_str(op_type);
      print_op(op->get_nth_input(0), builder);
      std::cout << ")";
      break;
    }
    case AC_OP_TARGET_AT: { 
      TargetAtOperation* target_at_op = (TargetAtOperation*)op;
      print_op(target_at_op->get_input(), builder);
      std::cout << "[" << ((TargetAtOperation *)op)->get_index() << "]";
      break;
    }
    case AC_OP_POSEIDON_HASH: {
      std::cout << "poseidon(";
      auto inputs = ((VectorInputFixedOutputLengthOperation<AC_OP_POSEIDON_HASH, 4> *)op)->get_inputs();
      for(auto input : inputs){
        print_op(input, builder);
        std::cout << ",";
      }
      std::cout << ")";
      break;
    }
    default: {
      std::cout << "unknown op";
      break;
    }
  }
}
