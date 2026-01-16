// Copyright (C) 2022 Xilinx, Inc
// SPDX-License-Identifier: BSD-3-Clause

#include <chrono>
#include <experimental/xrt_ip.h>

#ifdef U55N
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#include <json/json.h>
#include <limits.h>
#include <map>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vnx/cmac.hpp>
#include <vnx/networklayer.hpp>
#include <xrt/xrt_device.h>

/***************
 *    CONFIG   *
 ***************/

typedef struct
{
    int32_t cmac_id;
    int32_t net_id;
} kernel_instance_t;

typedef struct
{
    const char *  hostname;
    uint32_t      board_id;
    const char *  ip_address[2];
    kernel_instance_t   krnl[2];
} ip_config_table_t;

typedef struct {
    const char *  src_ip;
    const char *  des_ip;
    uint16_t src_port;
    uint16_t des_port;
    uint8_t  index;
} socket_config_t;


#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) sizeof(arr)/sizeof((arr)[0])
#endif


#include "ip_table.h"
#include "socket_table.h"




int32_t get_net_id(std::string ip_address)
{
    for (uint32_t  i = 0; i < ARRAY_SIZE(ip_lut); i++) {
        for (uint32_t j = 0; j < 2; j ++) {
            //printf("%s q:%s\n", ip_lut[i].ip_address[j], ip_address.c_str());
            if (std::string(ip_lut[i].ip_address[j]) ==  ip_address) {
                return ip_lut[i].krnl[j].net_id;
            }
        }
    }
    return -1;
}

ip_config_table_t * get_ip_config(std::string hostname, uint32_t board_id)
{
    ip_config_table_t * default_ip_config = &ip_lut[0]; // will set default to the first matched hostname.
    for (uint32_t  i = 0; i < ARRAY_SIZE(ip_lut); i++) {
        if (std::string(ip_lut[i].hostname) ==  hostname) {
            if (ip_lut[i].board_id == board_id) {
                printf("Found the ip configure for host %s (%d)\n",  ip_lut[i].hostname, board_id);
                return &ip_lut[i];
            }
        }
    }
    printf("Host %s with board %d does not exist in configure\n",  hostname.c_str(), board_id);
    printf("Use default config: %s, if0 ip: %s, if1 ip: %s\n",
           default_ip_config->hostname,
           default_ip_config->ip_address[0],
           default_ip_config->ip_address[1] );

    return default_ip_config;
}




const std::vector<std::pair<const char *, const char *>>  kernel = {{
        {"cmac_0", "networklayer_0"},
        {"cmac_1", "networklayer_1"}
    }
};



Json::Value parse_json(const std::string &string) {
    Json::Reader reader;
    Json::Value json;
    reader.parse(string, json);
    return json;
}

std::vector< vnx::CMAC > cmac_list;
std::vector< vnx::Networklayer > net_list;





int config_network(int argc, char *argv[], bool reconfig) {
    for (long unsigned int item  = 0 ; item < ARRAY_SIZE(ip_lut); item ++) {
        for (std::size_t ip_index = 0; ip_index < 2; ip_index ++) {
            ip_lut[item].krnl[ip_index].cmac_id =  -1;
            ip_lut[item].krnl[ip_index].net_id = -1;
        }
    }
    // Retrieve host and device information
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    int device_id = 1;

    // Read xclbin files from commandline
    std::vector<const char *> args(argv + 1, argv + argc);


    if (args.size() >= 2) {
        device_id = std::stoi(args[1]);
        std::cerr << "Loading XRT device " << device_id << std::endl;
    }
    for (long unsigned int item  = 0 ; item < ARRAY_SIZE(ip_lut); item ++) {

        if (std::string(ip_lut[item].hostname) ==  hostname) {
            xrt::device device = xrt::device(ip_lut[item].board_id);
            const std::string platform_json =
                device.get_info<xrt::info::device::platform>();
            const Json::Value platform_dict = parse_json(platform_json);
            const std::string platform =
                platform_dict["platforms"][0]["static_region"]["vbnv"].asString();
            std::cout << "FPGA platform: " << platform << std::endl;

            std::string binaryFile;
            if (argc < 2) {
                std::string app_name = argv[0];
                app_name = app_name.substr(2, app_name.size() - 6);
                binaryFile = "build_dir_" + app_name + "/kernel.xclbin";
                printf("try to use default xclbin: %s\n", binaryFile.c_str());
            }
            else { binaryFile = argv[1]; }

            auto xclbin_uuid = device.load_xclbin(binaryFile);
            std::cout << "Loaded " << binaryFile << " onto FPGA on " << hostname << " device id " << ip_lut[item].board_id << std::endl;
            // Give time for xclbin to be loaded completely before attempting to read


            for (std::size_t ip_index = 0; ip_index < 2; ip_index ++) {

                auto cus = kernel[ip_index];
                std::this_thread::sleep_for(std::chrono::seconds(1));
                auto cmac = vnx::CMAC(xrt::ip(device, xclbin_uuid,
                                              std::string(cus.first) + ":{" +
                                              std::string(cus.first) + "}"));

                // Enable rsfec if necessary
                cmac.set_rs_fec(false);
                //cmac.set_rs_fec(true);

                auto networklayer = vnx::Networklayer(
                                        xrt::ip(device, xclbin_uuid,
                                                "networklayer:{" + std::string(cus.second) + "}"));
                cmac_list.push_back(cmac);
                net_list.push_back(networklayer);
                ip_lut[item].krnl[ip_index].cmac_id = cmac_list.size() - 1;
                ip_lut[item].krnl[ip_index].net_id = net_list.size() - 1;
            }
        }
    }
    if (reconfig)
    {
        for (long unsigned int item  = 0 ; item < ARRAY_SIZE(ip_lut); item ++) {

            if (std::string(ip_lut[item].hostname) ==  hostname) {

                for (std::size_t ip_index = 0; ip_index < 2; ip_index ++) {
                    int32_t kernel_id = ip_lut[item].krnl[ip_index].cmac_id;
                    auto cmac  = cmac_list[kernel_id];
                    auto networklayer  = net_list[kernel_id];
                    auto cus = kernel[ip_index];


                    bool link_status;

                    // Can take a few tries before link is ready.
                    //for (std::size_t i = 0; i < 5; ++i) {
                    uint32_t time_out = 0;
                    while (true) {
                        auto status = cmac.link_status();
                        link_status = status["rx_status"];
                        time_out++;
                        if (time_out > 10)
                        {
                            break;
                        }
                        if (link_status) {
                            break;
                        }
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }
                    std::cout << "Link interface " << cus.first << ": "
                              << (link_status ? "true" : "false") << std::endl;
                    std::cout << "RS-FEC enabled: " << (cmac.get_rs_fec() ? "true" : "false")
                              << std::endl;

                    // Continue to next xclbin if no link is found.
                    if (!link_status) {
                        continue;
                    }
                    std::string ip = get_ip_config(hostname, ip_lut[item].board_id)->ip_address[ip_index];
                    std::cout << "setting up IP " << ip << " to interface " << cus.first
                              << std::endl;
                    networklayer.update_ip_address(ip);
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }

        }
        for (long unsigned int item = 0; item < ARRAY_SIZE(socket_lut); item ++)
        {
            socket_config_t * p_socket = &socket_lut[item];

            std::string src_ip(p_socket->src_ip);
            int32_t net_id = get_net_id(src_ip);
            if (net_id >= 0) {
                net_list[net_id].configure_socket(p_socket->index,
                                                  std::string(p_socket->des_ip),
                                                  p_socket->des_port,
                                                  p_socket->src_port,
                                                  true );
                printf("set socket %s:%d to %s:%d\n",
                       p_socket->src_ip, p_socket->src_port,
                       p_socket->des_ip, p_socket->des_port);
            } else {
                //printf("get net error for %s\n", p_socket->src_ip);
            }
        }
        for (long unsigned int item = 0; item < net_list.size(); item++)
        {
            net_list[item].populate_socket_table();
            net_list[item].print_socket_table(1);
        }


        for (long unsigned int item  = 0 ; item < ARRAY_SIZE(ip_lut); item ++) {
            ip_config_table_t * p_ip = &ip_lut[item];
            if (std::string(p_ip->hostname) ==  hostname) {
                for (std::size_t ip_index = 0; ip_index < 2; ip_index ++) {

                    std::vector<std::string> target_ips;
                    target_ips.clear();

                    std::string  local_ip(p_ip->ip_address[ip_index]);

                    for (long unsigned int sk_i = 0; sk_i < ARRAY_SIZE(socket_lut); sk_i ++)
                    {
                        socket_config_t * p_socket = &socket_lut[sk_i];
                        std::string src_ip(p_socket->src_ip);
                        std::string des_ip(p_socket->des_ip);
                        if ((local_ip  == src_ip) && (des_ip != local_ip)) {
                            target_ips.push_back(des_ip);
                        }
                    }

                    while (true) {
                        int32_t kernel_id = p_ip->krnl[ip_index].cmac_id;
                        auto networklayer  = net_list[kernel_id];
                        networklayer.arp_discovery();

                        std::this_thread::sleep_for(std::chrono::seconds(5));
                        auto table = networklayer.read_arp_table(255);
                        int num_of_des = 0;
                        for ( auto const& x : table) {
                            auto id = x.first;
                            auto value = x.second;
                            for (auto ip :  target_ips) {
                                std::cout << "arp table: [" << id << "] = " << value.first << "  " << value.second << "; " << std::endl;

                                if ( value.second == ip ) {
                                    std::cout << "arp table: [" << id << "] = " << value.first << "  " << value.second << "; " << std::endl;
                                    num_of_des ++;
                                }
                            }
                        }
                          networklayer.get_icmp_in_pkts();
                            networklayer.get_icmp_out_pkts();

                        if (num_of_des == target_ips.size())
                        {
                            networklayer.get_icmp_in_pkts();
                            networklayer.get_icmp_out_pkts();
                            break;
                        }
                    }
                }
            }
        }


    }

    return 0;
}




void dump_full_dist(bool full_log)
{
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    for (long unsigned int item  = 0 ; item < ARRAY_SIZE(ip_lut); item ++) {
        ip_config_table_t *p_ip = &ip_lut[item];

        if (std::string(p_ip->hostname) ==  hostname) {

            for (std::size_t ip_index = 0; ip_index < 2; ip_index ++) {
                int32_t kernel_id = ip_lut[item].krnl[ip_index].cmac_id;
                auto cmac  = cmac_list[kernel_id];
                auto networklayer  = net_list[kernel_id];
                printf("##################################\n");
                printf("[ETH DUMP] cmac id %ld \n", kernel_id);
                printf("[ETH DUMP] hostname %s board id %d\n", p_ip->hostname, p_ip->board_id);
                printf("[ETH DUMP] ip address %s\n", p_ip->ip_address[ip_index]);

                networklayer.get_icmp_in_pkts();
                networklayer.get_icmp_out_pkts();

                networklayer.get_udp_app_in_pkts();
                networklayer.get_udp_app_out_pkts();
                if (full_log)
                {
                    networklayer.get_udp_in_pkts();
                    networklayer.get_udp_out_pkts();
                    for (const auto& elem : cmac.link_status())
                    {
                        std::cout << "[ETH DUMP]-[UDP] " << elem.first << " " << elem.second << " " << "\n";
                    }

                    vnx::stats_t cmac_stats =  cmac.statistics(true);
                    for (const auto& elem : cmac_stats.tx)
                    {
                        std::cout << "[ETH DUMP]-[TX] " << elem.first << " " << elem.second << " " << "\n";
                    }

                    for (const auto& elem : cmac_stats.rx)
                    {
                        std::cout << "[ETH DUMP]-[RX] " << elem.first << " " << elem.second << " " << "\n";
                    }
                }
            }
        }
    }
}

void dump_package(void)
{
    dump_full_dist(false);
}
