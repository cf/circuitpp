
#ifndef poseidon_goldilocks
#define poseidon_goldilocks
#include <stdint.h>
#include <vector>
#include "goldilocks_field.h"

#define RATE 8
#define CAPACITY 4
#define HASH_SIZE 4
#define SPONGE_WIDTH (RATE + CAPACITY)
#define HALF_N_FULL_ROUNDS 4
#define N_FULL_ROUNDS_TOTAL (2 * HALF_N_FULL_ROUNDS)
#define N_PARTIAL_ROUNDS 22
#define N_ROUNDS (N_FULL_ROUNDS_TOTAL + N_PARTIAL_ROUNDS)

namespace Goldilocks
{

class PoseidonGoldilocks
{

private:
    inline void static pow7(Felt &x);
    inline void static pow7_(Felt *x);
    inline void static add_(Felt *x, const Felt C[SPONGE_WIDTH]);
    inline void static pow7add_(Felt *x, const Felt C[SPONGE_WIDTH]);
    inline void static mvp_(Felt *state, const Felt mat[SPONGE_WIDTH][SPONGE_WIDTH]);
    inline Felt static dot_(Felt *x, const Felt C[SPONGE_WIDTH]);
    inline void static prod_(Felt *x, const Felt alpha, const Felt C[SPONGE_WIDTH]);

public:
    void static hash_full_result_seq(Felt *, const Felt *);    
    inline void static hash_full_result(Felt *state, const Felt *input){hash_full_result_seq(state, input);};

    void static hash_seq(Felt (&state)[CAPACITY], const Felt (&input)[SPONGE_WIDTH]);
    void static hash(Felt (&state)[CAPACITY], const Felt (&input)[SPONGE_WIDTH]);
    void static linear_hash_seq(Felt *output, Felt *input, uint64_t size);
    void static linear_hash(Felt *output, Felt *input, uint64_t size);
    void static hash_n_to_m_no_pad(std::vector<Felt> &output, const Felt *input, uint64_t size, size_t output_size);
};

inline void PoseidonGoldilocks::pow7(Felt &x)
{
    Felt x2 = x * x;
    Felt x3 = x * x2;
    Felt x4 = x2 * x2;
    x = x3 * x4;
};

inline void PoseidonGoldilocks::pow7_(Felt *x)
{
    Felt x2[SPONGE_WIDTH], x3[SPONGE_WIDTH], x4[SPONGE_WIDTH];
    for (int i = 0; i < SPONGE_WIDTH; ++i)
    {
        x2[i] = x[i] * x[i];
        x3[i] = x[i] * x2[i];
        x4[i] = x2[i] * x2[i];
        x[i] = x3[i] * x4[i];
    }
};

inline void PoseidonGoldilocks::add_(Felt *x, const Felt C[SPONGE_WIDTH])
{
    for (int i = 0; i < SPONGE_WIDTH; ++i)
    {
        x[i] = x[i] + C[i];
    }
}
inline void PoseidonGoldilocks::prod_(Felt *x, const Felt alpha, const Felt C[SPONGE_WIDTH])
{
    for (int i = 0; i < SPONGE_WIDTH; ++i)
    {
        x[i] = alpha * C[i];
    }
}

inline void PoseidonGoldilocks::pow7add_(Felt *x, const Felt C[SPONGE_WIDTH])
{
    Felt x2[SPONGE_WIDTH], x3[SPONGE_WIDTH], x4[SPONGE_WIDTH];
    for (int i = 0; i < SPONGE_WIDTH; ++i)
    {
        x2[i] = x[i] * x[i];
        x3[i] = x[i] * x2[i];
        x4[i] = x2[i] * x2[i];
        x[i] = x3[i] * x4[i];
        x[i] = x[i] + C[i];
    }
};

inline Felt PoseidonGoldilocks::dot_(Felt *x, const Felt C[SPONGE_WIDTH])
{
    Felt s0 = x[0] * C[0];
    for (int i = 1; i < SPONGE_WIDTH; i++)
    {
        s0 = s0 + x[i] * C[i];
    }
    return s0;
};

// rick: check transpose access to matrix
inline void PoseidonGoldilocks::mvp_(Felt *state, const Felt mat[SPONGE_WIDTH][SPONGE_WIDTH])
{
    Felt old_state[SPONGE_WIDTH];
    std::memcpy(old_state, state, sizeof(Felt) * SPONGE_WIDTH);

    for (int i = 0; i < SPONGE_WIDTH; i++)
    {

        state[i] = mat[0][i] * old_state[0];
        for (int j = 1; j < SPONGE_WIDTH; j++)
        {
            state[i] = state[i] + (mat[j][i] * old_state[j]);
        }
    }
};
} // namespace Goldilocks

#endif /* poseidon_goldilocks */