/*
 * Copyright (c) 2014-2022 Kartik Kumar (me@kartikkumar.com)
 * Distributed under the MIT License.
 * See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
 */

#include <cassert>

#include <circuitpp/poseidon_goldilocks.h>
#include "circuitpp/poseidon_goldilocks_constants.h"
#include <math.h> /* floor */

namespace Goldilocks
{

void PoseidonGoldilocks::hash_seq(Felt (&state)[CAPACITY], Felt const (&input)[SPONGE_WIDTH])
{
    Felt aux[SPONGE_WIDTH];
    hash_full_result_seq(aux, input);
    std::memcpy(state, aux, CAPACITY * sizeof(Felt));
}
void PoseidonGoldilocks::hash(Felt (&state)[CAPACITY], Felt const (&input)[SPONGE_WIDTH])
{
    Felt aux[SPONGE_WIDTH];
    hash_full_result_seq(aux, input);
    std::memcpy(state, aux, CAPACITY * sizeof(Felt));
}
void PoseidonGoldilocks::hash_full_result_seq(Felt *state, const Felt *input)
{
    const int length = SPONGE_WIDTH * sizeof(Felt);
    std::memcpy(state, input, length);

    add_(state, &(PoseidonGoldilocksConstants::C[0]));
    for (int r = 0; r < HALF_N_FULL_ROUNDS - 1; r++)
    {
        pow7add_(state, &(PoseidonGoldilocksConstants::C[(r + 1) * SPONGE_WIDTH]));
        mvp_(state, PoseidonGoldilocksConstants::M);
    }
    pow7add_(state, &(PoseidonGoldilocksConstants::C[(HALF_N_FULL_ROUNDS * SPONGE_WIDTH)]));
    mvp_(state, PoseidonGoldilocksConstants::P);

    for (int r = 0; r < N_PARTIAL_ROUNDS; r++)
    {
        pow7(state[0]);
        state[0] = state[0] + PoseidonGoldilocksConstants::C[(HALF_N_FULL_ROUNDS + 1) * SPONGE_WIDTH + r];
        Felt s0 = dot_(state, &(PoseidonGoldilocksConstants::S[(SPONGE_WIDTH * 2 - 1) * r]));
        Felt W_[SPONGE_WIDTH];
        prod_(W_, state[0], &(PoseidonGoldilocksConstants::S[(SPONGE_WIDTH * 2 - 1) * r + SPONGE_WIDTH - 1]));
        add_(state, W_);
        state[0] = s0;
    }

    for (int r = 0; r < HALF_N_FULL_ROUNDS - 1; r++)
    {
        pow7add_(state, &(PoseidonGoldilocksConstants::C[(HALF_N_FULL_ROUNDS + 1) * SPONGE_WIDTH + N_PARTIAL_ROUNDS + r * SPONGE_WIDTH]));
        mvp_(state, PoseidonGoldilocksConstants::M);
    }
    pow7_(&(state[0]));
    mvp_(state, PoseidonGoldilocksConstants::M);
}

void PoseidonGoldilocks::hash_n_to_m_no_pad(std::vector<Felt> &output, const Felt *input, uint64_t size, size_t output_size)
{
    uint64_t remaining = size;
    Felt state[SPONGE_WIDTH];

    while (remaining)
    {
        if (remaining == size)
        {
            memset(state + RATE, 0, CAPACITY * sizeof(Felt));
        }
        else
        {
            std::memcpy(state + RATE, state, CAPACITY * sizeof(Felt));
        }

        uint64_t n = (remaining < RATE) ? remaining : RATE;

        memset(&state[n], 0, (RATE - n) * sizeof(Felt));

        std::memcpy(state, input + (size - remaining), n * sizeof(Felt));

        hash_full_result_seq(state, state);

        remaining -= n;
    }
    output.resize(output_size);
    size_t real_output_size = std::min(output_size, (size_t)SPONGE_WIDTH);
    for(size_t i=0;i<real_output_size;i++)
    {
        output[i] = state[i];
    }

    //std::memcpy(output.data(), state, real_output_size * sizeof(Felt));
}
void PoseidonGoldilocks::linear_hash_seq(Felt *output, Felt *input, uint64_t size)
{
    uint64_t remaining = size;
    Felt state[SPONGE_WIDTH];

    if (size <= CAPACITY)
    {
        std::memcpy(output, input, size * sizeof(Felt));
        return; // no need to hash
    }
    while (remaining)
    {
        if (remaining == size)
        {
            memset(state + RATE, 0, CAPACITY * sizeof(Felt));
        }
        else
        {
            std::memcpy(state + RATE, state, CAPACITY * sizeof(Felt));
        }

        uint64_t n = (remaining < RATE) ? remaining : RATE;

        memset(&state[n], 0, (RATE - n) * sizeof(Felt));

        std::memcpy(state, input + (size - remaining), n * sizeof(Felt));

        hash_full_result_seq(state, state);

        remaining -= n;
    }
    if (size > 0)
    {
        std::memcpy(output, state, CAPACITY * sizeof(Felt));
    }
    else
    {
        memset(output, 0, CAPACITY * sizeof(Felt));
    }
}
void PoseidonGoldilocks::linear_hash(Felt *output, Felt *input, uint64_t size)
{
    uint64_t remaining = size;
    Felt state[SPONGE_WIDTH];

    if (size <= CAPACITY)
    {
        std::memcpy(output, input, size * sizeof(Felt));
        std::memset(&output[size], 0, (CAPACITY - size) * sizeof(Felt));
        return; // no need to hash
    }
    while (remaining)
    {
        if (remaining == size)
        {
            memset(state + RATE, 0, CAPACITY * sizeof(Felt));
        }
        else
        {
            std::memcpy(state + RATE, state, CAPACITY * sizeof(Felt));
        }

        uint64_t n = (remaining < RATE) ? remaining : RATE;

        memset(&state[n], 0, (RATE - n) * sizeof(Felt));

        std::memcpy(state, input + (size - remaining), n * sizeof(Felt));

        hash_full_result_seq(state, state);

        remaining -= n;
    }
    if (size > 0)
    {
        std::memcpy(output, state, CAPACITY * sizeof(Felt));
    }
    else
    {
        memset(output, 0, CAPACITY * sizeof(Felt));
    }
}

} // namespace Goldilocks
