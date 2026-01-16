


template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void work_stealing_2x2_frp(
    frp_stream_t   &in1,
    frp_stream_t   &in2,
    frp_stream_t   &out1,
    frp_stream_t   &out2
)
{

#pragma HLS interface ap_ctrl_none port=return
#pragma HLS dataflow

    frp_stream_t     inner_connection_g1[2];
#pragma HLS stream variable=inner_connection_g1  depth=inner_fifo_size

    frp_stream_t     inner_connection_g2[2];
#pragma HLS stream variable=inner_connection_g2  depth=inner_fifo_size

    ws_distribute_package< FRP_TEMPLATE_CONSTANT() > (
        in1,
        inner_connection_g1[0],
        inner_connection_g1[1]
    );

    ws_distribute_package< FRP_TEMPLATE_CONSTANT() > (
        in2,
        inner_connection_g2[0],
        inner_connection_g2[1]
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
void work_stealing_4x4_frp(
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
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        in1, in2,
        inner_connection_g1[0], inner_connection_g1[1]);

    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        in3, in4,
        inner_connection_g1[2], inner_connection_g1[3]);

    // level 2:
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[0],  inner_connection_g1[2],
        out1, out3);

    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[1],  inner_connection_g1[3],
        out2, out4);
}

template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void work_stealing_8x8_frp(
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

    work_stealing_4x4_frp< FRP_TEMPLATE_CONSTANT() > (
        in1, in2, in3, in4,
        inner_connection_g1[0], inner_connection_g1[1], inner_connection_g1[2], inner_connection_g1[3]);

    work_stealing_4x4_frp< FRP_TEMPLATE_CONSTANT() > (
        in5, in6, in7, in8,
        inner_connection_g1[4], inner_connection_g1[5], inner_connection_g1[6], inner_connection_g1[7]);

    // level 3:
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[0], inner_connection_g1[4],
        out1, out5);
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[1], inner_connection_g1[5],
        out2, out6);
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[2], inner_connection_g1[6],
        out3, out7);
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[3], inner_connection_g1[7],
        out4, out8);
}


template <typename stream_t, typename frp_stream_t, typename pkg_t, typename frp_t, typename struct_t>
void work_stealing_16x16_frp(
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

    work_stealing_8x8_frp< FRP_TEMPLATE_CONSTANT() > (
        in01, in02, in03, in04, in05, in06, in07, in08,
        inner_connection_g1[0], inner_connection_g1[1], inner_connection_g1[2], inner_connection_g1[3],
        inner_connection_g1[4], inner_connection_g1[5], inner_connection_g1[6], inner_connection_g1[7]);

    work_stealing_8x8_frp< FRP_TEMPLATE_CONSTANT() > (
        in11, in12, in13, in14, in15, in16, in17, in18,
        inner_connection_g2[0], inner_connection_g2[1], inner_connection_g2[2], inner_connection_g2[3],
        inner_connection_g2[4], inner_connection_g2[5], inner_connection_g2[6], inner_connection_g2[7]);


    // level 3:
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[0], inner_connection_g2[0],
        out01, out11);
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[1], inner_connection_g2[1],
        out02, out12);
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[2], inner_connection_g2[2],
        out03, out13);
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[3], inner_connection_g2[3],
        out04, out14);


    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[4], inner_connection_g2[4],
        out05, out15);
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[5], inner_connection_g2[5],
        out06, out16);
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[6], inner_connection_g2[6],
        out07, out17);
    work_stealing_2x2_frp< FRP_TEMPLATE_CONSTANT() > (
        inner_connection_g1[7], inner_connection_g2[7],
        out08, out18);
}