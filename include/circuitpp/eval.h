
#pragma once
#include "core_base.h"
#include "cored.h"
#include "finalized.h"
#include "goldilocks_field.h"
#include "poseidon_goldilocks.h"
namespace circuitpp2 {
  template <typename Felt> class ACEvalContext {
  public:
    ACEvalContext() {
      this->circuit = nullptr;
      this->targets = {};
    }
    ACEvalContext(FCircuit *circuit, ExampleGeneratorRegistry<Felt> *registry,
                  const std::vector<Felt> inputs) {
      this->circuit = circuit;
      this->registry = registry;
      // copy inputs to targets
      this->targets = inputs;
      this->targets.resize(circuit->operations.size() + circuit->input_target_count + circuit->generator_target_count
                           - circuit->vector_operation_count);
      this->vec_targets.resize(circuit->vector_operation_count);
      
    }

    OPIndex get_generator_input_start() const { return this->circuit->input_target_count; }

    FGenerator *get_generator_at(OPIndex id) { return &this->circuit->generators[id]; }
    Felt set_target_at(OPIndex id, Felt value) {
      this->targets[id] = value;
      return value;
    }
    void set_vec_target_at(OPIndex id, std::vector<Felt> value) {
      this->vec_targets[id] = value;
    }
    Felt resolve_target(OPIndex id) const { return targets[id]; }
    void resolve_targets(const std::vector<OPIndex> &target_inputs,
                         std::vector<Felt> &resolved_targets) const {
      resolved_targets.resize(target_inputs.size());
      for (int i = 0; i < target_inputs.size(); i++) {
        resolved_targets[i] = Felt((uint64_t)targets[target_inputs[i]]);
      }
    }
    const std::vector<Felt> resolve_vec_target(OPIndex id) const { return vec_targets[id]; }
    Felt get_target_at(OPIndex id, int index) const { return vec_targets[id][index]; }

    std::vector<Felt> targets;
    std::vector<std::vector<Felt>> vec_targets;
    FCircuit *circuit;
    ExampleGeneratorRegistry<Felt> *registry;
  };

  template <typename Felt> class ACEvaluatorBase {
  public:
    virtual Felt eval_target_op(const ACEvalContext<Felt> &context, FOperation *op) const = 0;
    virtual std::vector<Felt> eval_vector_op(const ACEvalContext<Felt> &context,
                                             FOperation *op) const
        = 0;
  };

  template <typename Felt> class ACEvaluatorCore : public ACEvaluatorBase<Felt> {
  public:
    ACEvaluatorCore(ACEvaluatorBase<Felt> *alt_evaluator = nullptr) {
      this->alt_evaluator = alt_evaluator;
    }
    Felt eval_target_op(const ACEvalContext<Felt> &context, FOperation *op) const {
      auto op_type = op->op_type;
      switch (op_type) {
        case AC_OP_CONSTANT: {
          return Felt(op->constant_inputs[0]);
        }
        case AC_OP_INPUT: {
          return context.resolve_target(op->target_inputs[0]);
        }
        case AC_OP_GENERATOR_INPUT: {
          return context.resolve_target(context.get_generator_input_start()
                                        + op->constant_inputs[1]);
        }
        case AC_OP_ADD:
        case AC_OP_SUB:
        case AC_OP_MUL:
        case AC_OP_DIV:
        case AC_OP_EQ:
        case AC_OP_LEQ: {
          auto left = context.resolve_target(op->target_inputs[0]);
          auto right = context.resolve_target(op->target_inputs[1]);
          switch (op_type) {
            case AC_OP_ADD:
              return left + right;
            case AC_OP_SUB:
              return left - right;
            case AC_OP_MUL:
              return left * right;
            case AC_OP_DIV:
              throw std::runtime_error("division not supported");
            case AC_OP_EQ:
              return left == right ? 1 : 0;
            case AC_OP_LEQ:
              return left <= right ? 1 : 0;
            default:
              throw std::runtime_error("unknown binary op type");
          }
        }
        case AC_OP_NEG: {
          return context.resolve_target(op->target_inputs[0]) == Felt(0) ? 1 : 0;
        }
        case AC_OP_TARGET_AT: {
          return context.get_target_at(op->target_inputs[0], op->constant_inputs[0]);
        }
        default: {
          if (alt_evaluator != nullptr) {
            return alt_evaluator->eval_target_op(context, op);
          } else {
            throw std::runtime_error("unknown op type");
          }
        }
      }
    }

  protected:
    ACEvaluatorBase<Felt> *alt_evaluator;
  };

  typedef poseidon_goldilocks::Goldilocks::Felt GoldilocksFelt;

  class ACEvaluatorBaseGoldilocks : public ACEvaluatorCore<GoldilocksFelt> {
    std::vector<GoldilocksFelt> eval_vector_op(const ACEvalContext<GoldilocksFelt> &context,
                                               FOperation *op) const {
      auto op_type = op->op_type;
      switch (op_type) {
        case AC_OP_POSEIDON_HASH: {
          std::vector<GoldilocksFelt> outputs;
          outputs.resize(4);
          
          std::vector<GoldilocksFelt> inputs;
          inputs.resize(op->target_inputs.size());
          context.resolve_targets(op->target_inputs, inputs);
          poseidon_goldilocks::Goldilocks::PoseidonGoldilocks::hash_n_to_m_no_pad(
              outputs, inputs.data(), op->target_inputs.size(), 4);
          return outputs;
          break;
        }
        default: {
          throw std::runtime_error("unknown vector op type");
        }
      }
    }
  };

  template <typename Felt> class CircuitEvaluator {
  public:
    CircuitEvaluator(ACEvaluatorBase<Felt> *evaluator, ExampleGeneratorRegistry<Felt> *registry,
                     FCircuit *circuit, const std::vector<Felt> inputs) {
      this->context = ACEvalContext<Felt>(circuit, registry, inputs);
      this->evaluator = evaluator;
    }
    OPIndex eval_generator(OPIndex g_index, OPIndex gen_target_index){

          auto gen = this->context.circuit->generators[g_index];
          auto gen_inst = context.registry->get_generator_by_id(gen.generator_id);
          std::vector<Felt> inputs;
          inputs.resize(gen.target_inputs.size());
          auto num_outputs = gen_inst->num_outputs(gen.constant_inputs);
          std::vector<Felt> outputs;
          outputs.resize(num_outputs);
          auto inp = 0;
          for (auto input : gen.target_inputs) {
            auto v = context.resolve_target(input);
            inputs[inp] = v;
            inp++;
          }
          gen_inst->evaluate(gen.constant_inputs, inputs, outputs);
          for (int i = 0; i < num_outputs; i++) {
            context.set_target_at(gen_target_index++, outputs[i]);
          }
          return gen_target_index;
        
    }
    std::vector<Felt> eval_circuit() {

      context.vec_targets.resize(context.circuit->vector_operation_count);
      OPIndex target_index
          = context.circuit->input_target_count + context.circuit->generator_target_count;
      OPIndex gen_target_index = context.circuit->input_target_count;
      OPIndex vec_index = 0;
      auto circuit = context.circuit;
      auto total_generators = circuit->generators.size();
      auto current_generator = 0;
      auto total_ops = circuit->operations.size();
      OPIndex min_id = current_generator < total_generators
                           ? circuit->generators[current_generator].op_dependency_id
                           : total_ops;

      OPIndex op_index = 0;

      for (auto &op : circuit->operations) {
        if (op_index >= min_id) {
          gen_target_index = eval_generator(current_generator, gen_target_index);

          current_generator++;
          min_id = current_generator < total_generators
                       ? circuit->generators[current_generator].op_dependency_id
                       : total_ops;

        }
        if (get_stack_type_for_op_type(op.op_type) == AC_STACK_TYPE_TARGET_VEC) {
          context.set_vec_target_at(vec_index++, evaluator->eval_vector_op(context, &op));
        } else {
          context.set_target_at(target_index++, evaluator->eval_target_op(context, &op));
        }
        op_index++;
      }
      std::vector<Felt> outputs;

      context.resolve_targets(circuit->output_ops, outputs);
      for (auto assertion : circuit->assertions) {
        auto left = context.resolve_target(assertion.left);
        auto right = context.resolve_target(assertion.right);
        if (left != right) {
          throw std::runtime_error("assertion failed: " + assertion.message + "(left: "
                                   + left.to_string() + " != right: " + right.to_string() + ")");
        }
      }
      return outputs;
    }

  protected:
    ACEvaluatorBase<Felt> *evaluator;
    ACEvalContext<Felt> context;
  };

  class CircuitEvaluatorGoldilocks : public CircuitEvaluator<GoldilocksFelt> {
  public:
    CircuitEvaluatorGoldilocks(ExampleGeneratorRegistry<GoldilocksFelt> * registry, FCircuit *circuit, const std::vector<GoldilocksFelt> inputs)
        : CircuitEvaluator<GoldilocksFelt>(new ACEvaluatorBaseGoldilocks(),
                                           registry, circuit, inputs) {}
  };
}  // namespace circuitpp2