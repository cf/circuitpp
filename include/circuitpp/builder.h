
#pragma once
#include "cored.h"
#include "finalized.h"

namespace circuitpp2
{
  typedef ACOperation* ACTarget;
  typedef std::array<ACTarget, 4> ACHashTarget;

  struct CircuitAssertion
  {
    ACOperation *left;
    ACOperation *right;
    std::string message;
  };
  class CircuitFinalizer : public FCircuit {
    public:
      CircuitFinalizer(int input_target_count, int generator_target_count){
        this->generator_target_count = generator_target_count;
        this->input_target_count = input_target_count;
        this->op_count = input_target_count+generator_target_count;
        this->total_op_count = input_target_count+generator_target_count;
      }
    void visit_generator(ACGeneratorDefinition * generator, OPIndex total_inputs){
      FGenerator gen;
      gen.target_inputs.resize(generator->inputs.size());
      gen.constant_inputs.resize(generator->constant_inputs.size());

      gen.op_dependency_id = this->total_op_count - this->input_target_count - this->generator_target_count;
      gen.generator_id = generator->generator_id;



      for(int i = 0; i < generator->inputs.size(); i++){
        auto input = generator->inputs[i];
        gen.target_inputs[i] = visit_op(input, total_inputs);
      }

      for(int i = 0; i < generator->inputs.size(); i++){
        gen.constant_inputs[i] = generator->constant_inputs[i];
      }
      
      this->generators.push_back(gen);
    }
    OPIndex visit_op(ACOperation *op, OPIndex total_inputs)
    {
      if(op->get_target_id() != UNKNOWN_TARGET_ID){
        return op->get_target_id();
      }
      auto op_type = op->op_type();
      if(op_type == AC_OP_INPUT){
        return ((ACInputOp*)op)->get_input_index();
      }else if(op_type == AC_OP_GENERATOR_INPUT){
        return( ((ACGeneratorInputOp*)op)->get_global_generator_input_index())+ total_inputs;
      }else if(op_type == AC_OP_CONSTANT){
        OPIndex id = op_count++;
        op->set_target_id(id);
        op->set_global_target_id(total_op_count++);
        operations.push_back(FOperation{op_type, {(uint64_t)((ACConstantOp*)op)->get_constant()}, {}});
        return id;
      }
      auto num_inputs = op->num_inputs();
      auto num_constants = op->num_constant_inputs();

      FOperation fop;
      fop.op_type = op_type;
      fop.target_inputs.resize(num_inputs);
      fop.constant_inputs.resize(num_constants);

      // walk inputs
      for(int i = 0; i < num_inputs; i++){
        auto input = op->get_nth_input(i);
        if(input == nullptr){
          throw std::runtime_error("missing input/null input");
        }else{
          fop.target_inputs[i] = visit_op(input, total_inputs);
        }
      }

      // walk constants
      for(int i=0; i < num_constants; i++){
        fop.constant_inputs[i] = op->get_nth_constant_input(i);
      }

      // set target id
      auto stack_type = op->stack_type();
      if(stack_type == AC_STACK_TYPE_TARGET){
        OPIndex id = op_count++;
        op->set_target_id(id);
        op->set_global_target_id(total_op_count++);
        operations.push_back(fop);
        return id;
      }else if(stack_type == AC_STACK_TYPE_TARGET_VEC){
        OPIndex id = vector_operation_count++;
        op->set_target_id(id);
        op->set_global_target_id(total_op_count++);
        operations.push_back(fop);
        return id;
      }else{
        throw std::runtime_error("unknown stack type");
      }

    }
    protected:
      OPIndex op_count = 0;
      OPIndex total_op_count = 0;
  };
template <typename Felt> 
  class ACCircuitBuilder
  {
  public:
    ACCircuitBuilder() {}
    ~ACCircuitBuilder() {
      operations.clear();
      output_ops.clear();
      assertions.clear();
    }
    uint64_t get_input_target_count()
    {
      return input_target_count;
    }
    uint64_t get_output_target_count()
    {
      return output_target_count;
    }
    ACOperation *add_input()
    {
      ACInputOp *op = new ACInputOp(input_target_count++);
      operations.insert(op);
      return op;
    }
    ACOperation *add_generator_input(uint64_t generator_id) {

      ACGeneratorInputOp *op = new ACGeneratorInputOp(generator_id, generator_target_count++);
      operations.insert(op);
      return op;

    }
    std::vector<ACOperation*> new_generator(uint64_t generator_id, std::vector<ACOperation *> inputs, std::vector<uint64_t> constants)
    {
      auto gen_inst = this->registry.get_generator_by_id(generator_id);
      ACGeneratorDefinition def;
      def.num_outputs = gen_inst->num_outputs(constants);
      def.constant_inputs = constants;
      def.inputs = inputs;
      def.generator_id = generator_id;
      def.start_generator_output_index = this->generator_target_count;

      this->generators.push_back(def);

      return gen_inst->constrain(this, &def);
      

    }
    ACOperation *constant(uint64_t c)
    {
      return injest_op(new ACConstantOp(c));
    }
    ACOperation *negate(ACOperation *input)
    {
      return injest_op(new UnaryOperation<AC_OP_NEG>(input));
    }
    ACOperation *add(ACOperation *input1, ACOperation *input2)
    {
      return injest_op(new BinaryOperation<AC_OP_ADD>(input1, input2));
    }
    ACOperation *sub(ACOperation *input1, ACOperation *input2)
    {
      return injest_op(new BinaryOperation<AC_OP_SUB>(input1, input2));
    }
    ACOperation *mul(ACOperation *input1, ACOperation *input2)
    {
      return injest_op(new BinaryOperation<AC_OP_MUL>(input1, input2));
    }
    ACOperation *is_equal(ACOperation *input1, ACOperation *input2)
    {
      return injest_op(new BinaryOperation<AC_OP_EQ>(input1, input2));
    }
    ACOperation *is_leq(ACOperation *input1, ACOperation *input2)
    {
      return injest_op(new BinaryOperation<AC_OP_LEQ>(input1, input2));
    }
    ACOperation *target_at(ACOperation *input, uint64_t index)
    {
      return injest_op(new TargetAtOperation(input, index));
    }
    ACHashTarget hash_no_pad(std::vector<ACOperation *> inputs)
    {
      auto base = injest_op(new VectorInputFixedOutputLengthOperation<AC_OP_POSEIDON_HASH, 4>(inputs));
      auto x0 = target_at(base, 0);
      auto x1 = target_at(base, 1);
      auto x2 = target_at(base, 2);
      auto x3 = target_at(base, 3);
      return {x0, x1, x2, x3};
    }
    ACHashTarget two_to_one(ACHashTarget left, ACHashTarget right)
    {
      std::vector<ACOperation *> inputs;
      inputs.insert(inputs.end(), left.begin(), left.end());
      inputs.insert(inputs.end(), right.begin(), right.end());
      return hash_no_pad(inputs);
    }

    void register_output(ACOperation *output)
    {
      output_target_count++;
      output_ops.push_back(output);
    }
    void register_outputs(std::vector<ACOperation *> outputs)
    {
      for (auto output : outputs)
      {
        register_output(output);
      }
    }
    const std::vector<ACOperation *> *get_output_ops()
    {
      return &(this->output_ops);
    }
    ACOperation *injest_op(ACOperation *op)
    {
      auto it = operations.find(op);
      if (it != operations.end())
      {
        return *it;
      }

      operations.insert(op);
      return op;
    }
    void connect(ACOperation *left, ACOperation *right, const std::string &message = "")
    {
      assertions.push_back({left, right, message});
    }

    CircuitFinalizer build()
    {
      CircuitFinalizer finalizer(input_target_count, generator_target_count);
      finalize(finalizer);
      return finalizer;
    }

    void finalize(CircuitFinalizer &finalizer)
    {
      for(auto generator : generators){
        finalizer.visit_generator(&generator, input_target_count);
      }
      for (auto assertion : assertions)
      {
        finalizer.assertions.push_back({finalizer.visit_op(assertion.left, input_target_count), finalizer.visit_op(assertion.right, input_target_count), assertion.message});
      }
      for (auto output : output_ops)
      {
        finalizer.output_ops.push_back(finalizer.visit_op(output, input_target_count));
      }

    }
    uint64_t add_generator(ACGeneratorBase<Felt> * generator){
      this->registry.add_generator(generator);
      return generator->getId();
    }

    ExampleGeneratorRegistry<Felt> * get_registry() {
      return &registry;
    }



  protected:
    int input_target_count = 0;
    int generator_target_count = 0;
    int output_target_count = 0;
    int total_target_count = 0;
    int total_target_vec_count = 0;
    spp::sparse_hash_set<ACOperation *> operations;
    std::vector<ACOperation *> output_ops;
    std::vector<CircuitAssertion> assertions;
    std::vector<ACGeneratorDefinition> generators;
    ExampleGeneratorRegistry<Felt> registry;
  };

} // namespace circuitpp
