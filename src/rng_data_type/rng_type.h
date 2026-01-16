#ifndef __RNG_TYPE_H__
#define __RNG_TYPE_H__

#include "stream_type.h"

#include "rng_data_struct.h"



typedef ap_uint<RNG_WIDTH>      rng32_t;


GEN_STRUCT_INTERFACE(rng_state)

GEN_STRUCT_INTERFACE(rng_output)



#ifndef __SYNTHESIS__
    #define DEBUG_PRINT(f_, ...) printf((f_), __VA_ARGS__);
#else
    #define DEBUG_PRINT(f_, ...) ;
#endif


unsigned int minRand(unsigned int seed, int load) {
    static ap_uint<32> lfsr;

    if (load == 1) lfsr = seed;
    bool b_32 = lfsr.get_bit(32 - 32);
    bool b_22 = lfsr.get_bit(32 - 22);
    bool b_2 = lfsr.get_bit(32 - 2);
    bool b_1 = lfsr.get_bit(32 - 1);
    bool new_bit = b_32 ^ b_22 ^ b_2 ^ b_1;
    lfsr = lfsr >> 1;
    lfsr.set_bit(31, new_bit);

    return lfsr.to_uint();
}



#endif /* __RNG_TYPE_H__ */
