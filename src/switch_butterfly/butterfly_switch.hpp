template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void switch_2x2_frp(
    frp_stream_t   &in1,
    frp_stream_t   &in2,
    frp_stream_t   &out1,
    frp_stream_t   &out2,
    const route_id_t id1,
    const route_id_t id2,
    const route_id_t mask

)
{

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

    frp_stream_t     inner_connection_g1[2];
#pragma HLS stream variable=inner_connection_g1  depth=inner_fifo_size

    frp_stream_t     inner_connection_g2[2];
#pragma HLS stream variable=inner_connection_g2  depth=inner_fifo_size

    distribute_package< FRP_TEMPLATE_CONSTANT() > (
        in1,
        inner_connection_g1[0],
        inner_connection_g1[1],
        id1, id2, mask
    );

    distribute_package< FRP_TEMPLATE_CONSTANT() > (
        in2,
        inner_connection_g2[0],
        inner_connection_g2[1],
        id1, id2, mask
    );

    merge_package< FRP_TEMPLATE_CONSTANT() >(
        inner_connection_g1[0],
        inner_connection_g2[0],
        out1
    );
    merge_package< FRP_TEMPLATE_CONSTANT() >(
        inner_connection_g1[1],
        inner_connection_g2[1],
        out2
    );

}


template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void switch_4x4_frp(
    frp_stream_t   &in1,
    frp_stream_t   &in2,
    frp_stream_t   &in3,
    frp_stream_t   &in4,
    frp_stream_t   &out1,
    frp_stream_t   &out2,
    frp_stream_t   &out3,
    frp_stream_t   &out4
)
{
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

    frp_stream_t     inner_connection_g1[4];
#pragma HLS stream variable=inner_connection_g1  depth=inner_fifo_size

    // level 1:
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        in1, in2,
        inner_connection_g1[0], inner_connection_g1[1],
        0, 1,  0x1);

    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        in3, in4,
        inner_connection_g1[2], inner_connection_g1[3],
        0, 1,  0x1);

    // level 2:
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[0],  inner_connection_g1[2],
        out1, out3,
        0, 2,  0x2);

    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[1],  inner_connection_g1[3],
        out2, out4,
        0, 2,  0x2);
}



template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void switch_8x8_frp(
    frp_stream_t   &in1,
    frp_stream_t   &in2,
    frp_stream_t   &in3,
    frp_stream_t   &in4,
    frp_stream_t   &in5,
    frp_stream_t   &in6,
    frp_stream_t   &in7,
    frp_stream_t   &in8,
    frp_stream_t   &out1,
    frp_stream_t   &out2,
    frp_stream_t   &out3,
    frp_stream_t   &out4,
    frp_stream_t   &out5,
    frp_stream_t   &out6,
    frp_stream_t   &out7,
    frp_stream_t   &out8
)
{
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

    frp_stream_t     inner_connection_g1[8];
#pragma HLS stream variable=inner_connection_g1  depth=inner_fifo_size

    switch_4x4_frp< FRP_TEMPLATE_CONSTANT() > (
        in1, in2, in3, in4,
        inner_connection_g1[0], inner_connection_g1[1], inner_connection_g1[2], inner_connection_g1[3]);

    switch_4x4_frp< FRP_TEMPLATE_CONSTANT() > (
        in5, in6, in7, in8,
        inner_connection_g1[4], inner_connection_g1[5], inner_connection_g1[6], inner_connection_g1[7]);

    // level 3:
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[0], inner_connection_g1[4],
        out1, out5,
        0, 4,  0x4);
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[1], inner_connection_g1[5],
        out2, out6,
        0, 4,  0x4);
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[2], inner_connection_g1[6],
        out3, out7,
        0, 4,  0x4);
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[3], inner_connection_g1[7],
        out4, out8,
        0, 4,  0x4);
}


template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void switch_16x16_frp(
    frp_stream_t   &in01,
    frp_stream_t   &in02,
    frp_stream_t   &in03,
    frp_stream_t   &in04,
    frp_stream_t   &in05,
    frp_stream_t   &in06,
    frp_stream_t   &in07,
    frp_stream_t   &in08,
    frp_stream_t   &in11,
    frp_stream_t   &in12,
    frp_stream_t   &in13,
    frp_stream_t   &in14,
    frp_stream_t   &in15,
    frp_stream_t   &in16,
    frp_stream_t   &in17,
    frp_stream_t   &in18,
    frp_stream_t   &out01,
    frp_stream_t   &out02,
    frp_stream_t   &out03,
    frp_stream_t   &out04,
    frp_stream_t   &out05,
    frp_stream_t   &out06,
    frp_stream_t   &out07,
    frp_stream_t   &out08,
    frp_stream_t   &out11,
    frp_stream_t   &out12,
    frp_stream_t   &out13,
    frp_stream_t   &out14,
    frp_stream_t   &out15,
    frp_stream_t   &out16,
    frp_stream_t   &out17,
    frp_stream_t   &out18
)
{
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

    frp_stream_t     inner_connection_g1[8];
#pragma HLS stream variable=inner_connection_g1  depth=inner_fifo_size

    frp_stream_t     inner_connection_g2[8];
#pragma HLS stream variable=inner_connection_g2  depth=inner_fifo_size

    switch_8x8_frp< FRP_TEMPLATE_CONSTANT() > (
        in01, in02, in03, in04, in05, in06, in07, in08,
        inner_connection_g1[0], inner_connection_g1[1], inner_connection_g1[2], inner_connection_g1[3],
        inner_connection_g1[4], inner_connection_g1[5], inner_connection_g1[6], inner_connection_g1[7]);

    switch_8x8_frp< FRP_TEMPLATE_CONSTANT() > (
        in11, in12, in13, in14, in15, in16, in17, in18,
        inner_connection_g2[0], inner_connection_g2[1], inner_connection_g2[2], inner_connection_g2[3],
        inner_connection_g2[4], inner_connection_g2[5], inner_connection_g2[6], inner_connection_g2[7]);


    // level 3:
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[0], inner_connection_g2[0],
        out01, out11,
        0, 8,  0x8);
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[1], inner_connection_g2[1],
        out02, out12,
        0, 8,  0x8);
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[2], inner_connection_g2[2],
        out03, out13,
        0, 8,  0x8);
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[3], inner_connection_g2[3],
        out04, out14,
        0, 8,  0x8);


    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[4], inner_connection_g2[4],
        out05, out15,
        0, 8,  0x8);
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[5], inner_connection_g2[5],
        out06, out16,
        0, 8,  0x8);
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[6], inner_connection_g2[6],
        out07, out17,
        0, 8,  0x8);
    switch_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[7], inner_connection_g2[7],
        out08, out18,
        0, 8,  0x8);
}