#include <cstring>
#include <stdexcept>
#include <circuitpp/core_base.h>
namespace circuitpp2
{

ACOperationStackType get_stack_type_for_op_type(ACOperationType op_type)
{
    if(op_type == AC_OP_POSEIDON_HASH){
        return AC_STACK_TYPE_TARGET_VEC;
    }else{
        return AC_STACK_TYPE_TARGET;
    }
}

int ACOperation::num_constant_inputs()
{
    return 0;
}

uint64_t ACOperation::get_nth_constant_input(int n)
{
    throw std::runtime_error("Operation does not have constant inputs");
}

uint64_t *ACOperation::get_hash_u64() const
{
    return reinterpret_cast<uint64_t *>(const_cast<uint8_t *>(hash));
}

uint8_t *ACOperation::get_hash_u8() const
{
    return const_cast<uint8_t *>(hash);
}

bool ACOperation::operator==(const ACOperation &o) const
{
    return (memcmp(this->hash, o.hash, AC_HASH_OUTPUT_SIZE) == 0);
}

OPIndex ACOperation::get_target_id()
{
    return target_id;
}

void ACOperation::set_target_id(OPIndex id)
{
    if(target_id == UNKNOWN_TARGET_ID){
        target_id = id;
    }else{
        throw std::runtime_error("Target ID already set");
    }
}

OPIndex ACOperation::get_global_target_id()
{
    return global_target_id;
}

void ACOperation::set_global_target_id(OPIndex id)
{
    if(global_target_id == UNKNOWN_TARGET_ID){
        global_target_id = id;
    }else{
        throw std::runtime_error("Target ID already set");
    }
}

} // namespace circuitpp

namespace std
{
std::size_t hash<circuitpp2::ACOperation>::operator()(circuitpp2::ACOperation const &p) const
{
    std::size_t seed = 0;
    auto h = p.get_hash_u64();
    spp::hash_combine(seed, h[0]);
    spp::hash_combine(seed, h[1]);
    spp::hash_combine(seed, h[2]);
    spp::hash_combine(seed, h[3]);
    return seed;
}
}