#ifndef __SINK_INSTANCE_HPP__
#define __SINK_INSTANCE_HPP__

template < typename stream_t, typename pkg_t >
void sink_instance(uint32_t num_last, uint32_t nolast, uint32_t max_size,  stream_t  &in,
                   debug_msg_inner_stream_t &out)
{

    uint32_t stop_counter = 0;
    uint64_t write_counter = 0;
    uint32_t timeout_counter = 0;
    uint64_t sum_value = 0;

    while (1)
    {
        pkg_t  pkg;
        if (in.read_nb(pkg)) {

            sum_value += pkg.data.range(63, 0);

            write_counter ++;
            timeout_counter = 0;
        }
        else {
            timeout_counter ++;
        }

        if (timeout_counter > (max_size))
        {
            break;
        }
    }

    debug_msg_item_t np_msg =  write_counter;
    out.write(np_msg);
    debug_msg_item_t sum_msg = sum_value;
    out.write(sum_msg);

    return;
}

template < int N >
void debug_dump(  debug_msg_inner_stream_t (&in)[N], ap_uint<64>   *mem )
{
    debug_msg_item_t dump_array[N * 2];
#pragma HLS ARRAY_PARTITION variable=dump_array   dim=0


    for (int i = 0; i < N; i++)
    {
        dump_array[i] = in[i].read();
    }

    for (int i = 0; i < N; i++)
    {
        dump_array[i + N] = in[i].read();
    }

    for (int i = 0; i < N * 2; i ++)
    {
        mem[i] = dump_array[i];
    }


}

#endif /* __SINK_INSTANCE_HPP__ */