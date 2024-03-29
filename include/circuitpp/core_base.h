#pragma once
#include <stdint.h>
#include "blake3/blake3.h"
#include <array>
#include <map>
#include "sparsepp/spp.h"
#include <iostream>
#include <functional>
#include <string>
#include <xxhash_cx.h>

using std::string;

namespace circuitpp2
{
template <typename Felt> 
class ACCircuitBuilder;
template <typename Felt> 
class ACGeneratorBase;

#define GET_OPERATION_ID_STACK_TYPE (x)((x >> 48) & 0xF)
#define GET_OPERATION_ID_STACK_INDEX (x & 0x0000FFFFFFFFFFFFULL)
#define GET_COMBINED_OPERATION_ID(stack_type, stack_index) ((((uint64_t)stack_type) << 48) | stack_index)
#define UNKNOWN_TARGET_ID 0x1fffffffffffffULL
#define AC_HASH_OUTPUT_SIZE 32

typedef uint64_t OPIndex;
typedef uint32_t OPVecLength;
enum ACOperationStackType : unsigned char
{
    AC_STACK_TYPE_TARGET = 0,
    AC_STACK_TYPE_TARGET_VEC = 1,
};
enum ACOperationType : unsigned short
{
    AC_OP_CONSTANT = 0,
    AC_OP_INPUT = 1,
    AC_OP_GENERATOR_INPUT = 2,
    AC_OP_NEG = 3,
    AC_OP_ADD = 4,
    AC_OP_SUB = 5,
    AC_OP_MUL = 6,
    AC_OP_DIV = 7,
    AC_OP_EQ = 8,
    AC_OP_LEQ = 9,
    // 10-47 reserved
    AC_OP_TARGET_AT = 48,
    AC_OP_SELECT = 49,
    // 50-63 reserved

    AC_OP_POSEIDON_HASH = 64,
    //AC_OP_CUSTOM_GENERATOR = 65,

    // 0x1000 - 0x9000 for user defined operations
};
ACOperationStackType get_stack_type_for_op_type(ACOperationType op_type);

class ACOperation
{
public:
    // pure virtual function providing interface framework.
    virtual ACOperationType op_type() const = 0;
    virtual int num_inputs() = 0;
    virtual int num_constant_inputs();
    virtual int num_outputs() = 0;
    virtual ACOperationStackType stack_type() const = 0;

    virtual ACOperation *get_nth_input(int n) = 0;
    virtual uint64_t get_nth_constant_input(int n);
    uint64_t *get_hash_u64() const;
    uint8_t *get_hash_u8() const;

    bool operator==(const ACOperation &o) const;
    OPIndex get_target_id();
    void set_target_id(OPIndex id);
    OPIndex get_global_target_id();
    void set_global_target_id(OPIndex id);

protected:
    uint8_t hash[AC_HASH_OUTPUT_SIZE];
    OPIndex target_id = UNKNOWN_TARGET_ID;
    OPIndex global_target_id = UNKNOWN_TARGET_ID;
};

using namespace xxhash::literals;
#define	STR_XXH64__(s)	s ## _xxh64
#define	STR_XXH64(s)	STR_XXH64__(s)
#define AC_NAMED_GENERATOR(name) static constexpr std::string Name() {return name;} static const uint64_t ID = STR_XXH64(name);   constexpr std::string getName() const noexcept override { return Name(); }   constexpr uint64_t getId() const noexcept override { return ID; }

struct ACGeneratorDefinition {
    uint64_t generator_id;
    std::vector<ACOperation *> inputs;
    std::vector<uint64_t> constant_inputs;
    uint32_t num_outputs;
    uint32_t start_generator_output_index;
};

template <typename Felt> 
class ACGeneratorBase {
public:
    constexpr virtual std::string getName() const noexcept = 0;
    constexpr virtual uint64_t getId() const noexcept = 0;
    virtual int num_outputs(const std::vector<uint64_t>& constant_inputs) = 0;
    virtual void evaluate(const std::vector<uint64_t>& constant_inputs, std::vector<Felt>& inputs, std::vector<Felt>& outputs) = 0;
    virtual std::vector<ACOperation *> constrain(ACCircuitBuilder<Felt> * builder, ACGeneratorDefinition * definition) = 0;
    virtual ~ACGeneratorBase() = default;
};

} // namespace circuitpp

namespace std
{
// inject specialization of std::hash for ACOperation into namespace std
// ----------------------------------------------------------------
template <>
struct hash<circuitpp2::ACOperation>
{
    std::size_t operator()(circuitpp2::ACOperation const &p) const;
};
}