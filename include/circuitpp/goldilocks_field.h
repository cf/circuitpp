
#ifndef goldilocks_field
#define goldilocks_field
#include <stdint.h>
#include <string>

namespace Goldilocks
{
    static const uint64_t GOLDILOCKS_PRIME = 18446744069414584321ULL;

    // helper for mul
    inline uint64_t reduce128(uint64_t x_hi, uint64_t x_lo){
        uint64_t x_hi_hi = x_hi>>32;
        uint64_t x_hi_lo = x_hi & 0xffffffff;
        uint64_t t0 = x_lo - x_hi_hi;
        if(t0 > x_lo){
            t0 -= 0xffffffff;
        }
        uint64_t t1 = x_hi_lo*0xffffffff;
        uint64_t t2 = t0 + t1;
        if(t2 < t1){
            t2 += 0xffffffff;
        }
        return t2;
    }

    // Prime field
    class Felt
    {
    public:
        Felt() = default;
        ~Felt() = default;
        constexpr Felt(uint64_t v) : inner(v) {}
        
        Felt operator+(const Felt rhs) const
        {
            return Felt(add(inner, rhs.inner));
        }
        Felt operator-(const Felt rhs) const
        {
            return Felt(sub(inner, rhs.inner));
        }
        Felt operator/(const Felt rhs) const
        {
            return Felt(mul(inner, rhs.inverse().inner));
        }
        Felt operator*(const Felt rhs) const
        {
            return Felt(mul(inner, rhs.inner));
        }
        
        bool operator==(const Felt rhs) const
        {
            return inner == rhs.inner;
        }
        bool operator!=(const Felt rhs) const
        {
            return inner != rhs.inner;
        }

        bool operator<(const Felt rhs) const
        {
            return inner < rhs.inner;
        }
        bool operator>(const Felt rhs) const
        {
            return inner > rhs.inner;
        }

        bool operator<=(const Felt rhs) const
        {
            return inner <= rhs.inner;
        }
        bool operator>=(const Felt rhs) const
        {
            return inner >= rhs.inner;
        }



        constexpr explicit operator uint64_t() const
        {
            return inner;
        }
        
        inline uint64_t u64() const
        {
            return inner;
        }
        inline std::string to_string() const
        {
            return std::to_string(inner);
        }
        

        // used for S-box in the Rescue Prime Optimized hash function
        Felt pow7() {
            uint64_t t2 = mul(inner, inner);
            return mul(mul(mul(t2, t2), t2), inner);
        }

        inline Felt inverse() const
        {
            uint64_t t2 = exp_acc<1>(inner, inner);
            uint64_t t3 = exp_acc<1>(t2, inner);
            uint64_t t6 = exp_acc<3>(t3, t3);
            uint64_t t12 = exp_acc<6>(t6, t6);
            uint64_t t24 = exp_acc<12>(t12, t12);
            uint64_t t30 = exp_acc<6>(t24, t6);
            uint64_t t31 = exp_acc<1>(t30, inner);
            uint64_t t63 = exp_acc<32>(t31, t31);
            uint64_t inv = exp_acc<1>(t63, inner);
            return Felt(inv);
        }


    private:
        uint64_t inner;

        // Field modulus `p = 2^64 - 2^32 + 1`
        constexpr static const uint64_t N = 18446744069414584321ULL;

        // Square of auxiliary modulus R for Montgomery reduction `R2 ≡ (2^64)^2 mod p`
        constexpr static const uint64_t R2 = 18446744065119617025ULL;

        template<unsigned N_ACC>
        inline uint64_t exp_acc(uint64_t element, const uint64_t tail) const {
#pragma unroll
            for (unsigned i = 0; i < N_ACC; i++) {
                element = mul(element, element);
            }
            return mul(element, tail);
        }

        template<unsigned N_ACC>
        inline uint64_t sqn(uint64_t element) const {
#pragma unroll
            for (unsigned i = 0; i < N_ACC; i++) {
                element = mul(element, element);
            }
            return element;
        }

        inline uint64_t add(const uint64_t a, const uint64_t b) const
        {
            uint64_t a_plus_b = a+b;

            if(a_plus_b < a){
                uint64_t c = a_plus_b + 0xffffffff;
                if(c<a_plus_b){
                    a_plus_b = c+0xffffffff;
                }else{
                  a_plus_b = c;
                }
            }

            // a_plus_b will be at most 36893488138829168640
            // so we can subtract instead of using modulo
            return a_plus_b < N ? a_plus_b : (a_plus_b - N);
        }

        inline uint64_t sub(const uint64_t a, const uint64_t b) const
        {
            if(a>=b){
                return a-b;
            }else{
                return a+N-b;
            }
        }

        inline uint64_t mul(const uint64_t lhs, const uint64_t rhs) const
        {
            // since lhs = lhs_hi * 2^32 + lhs_lo and rhs = rhs_hi * 2^32 + rhs_lo
            // we can compute lhs * rhs = 
            // lhs_hi * rhs_hi * 2^64 + (lhs_hi * rhs_lo + rhs_hi * lhs_lo)*2^32 + lhs_lo * rhs_lo
            uint64_t a = lhs >> 32;
            uint64_t b = lhs & 0xFFFFFFFF;
            uint64_t c = rhs >> 32;
            uint64_t d = rhs & 0xFFFFFFFF;

            uint64_t ad = a * d;
            uint64_t bd = b * d;

            uint64_t adbc = ad + (b * c);
            uint64_t adbc_carry = adbc < ad ? 1 : 0;

            uint64_t product_lo = bd + (adbc << 32);
            uint64_t product_lo_carry = product_lo < bd ? 1 : 0;
            uint64_t product_hi = (a * c) + (adbc >> 32) + (adbc_carry << 32) + product_lo_carry;
            return reduce128(product_hi, product_lo) % N;
        }
    };

}

#endif /* goldilocks_field */