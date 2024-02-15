#include "socket_unit.h"

const string brd_addr = "255.255.255.255";
const string loopback_addr = "127.0.0.1";

/*-----------[destionation device data]------------*/

DstUnit::DstUnit(string ip_addr, uint16_t port){
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    addr.sin_port = htons(port);
}

DstUnit::DstUnit(uint16_t local_port){
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(loopback_addr.c_str());
    addr.sin_port = htons(local_port);
}

/*-----------[sd unit function]------------*/

void SocketUnit::bind_socket(){
    bind(sd.sock, (const struct sockaddr *)&(sd.addr), sd.addr_len);
    //bind(global_sd.sock, (const struct sockaddr *)&(global_sd.addr), global_sd.addr_len);
}

SocketUnit::~SocketUnit(){
    close(sd.sock);
}

int SocketUnit::receive(void *buf, int size){
    unsigned int addr_len = sizeof(src_data);
    int size = recvfrom(sd.sock, buf, size, 0, (struct sockaddr *)&src_data, &addr_len);
    return size;
}

char* SocketUnit::get_src_addr(){
    return inet_ntoa(src_data.sin_addr);
}

int SocketUnit::get_src_port(){
    return ntohs(src_data.sin_port);
}

/*-----------[UDP unit function]-----------*/

void UDPUnit::set_socket(string ip_addr, uint16_t port){
    sd.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sd.addr.sin_family = AF_INET;
    sd.addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    sd.addr.sin_port = htons(port);
    sd.addr_len = sizeof(sd.addr);
}

UDPUnit::UDPUnit(string ip_addr, uint16_t port){
    set_socket(ip_addr, port);
    bind_socket();
}

UDPUnit::UDPUnit(uint16_t local_port){
    set_socket(loopback_addr, local_port);
    bind_socket();
}

int UDPUnit::send(void *buf, DstUnit *dst, int len){
    int size = sendto(sd.sock, buf, len, 0, (struct sockaddr *)&(dst->addr), sizeof(dst->addr));
    return size;
}

bool UDPUnit::enable_broadcast(int bc_port){
    if(permission_broadcast){
        cout << "Already enabled" << endl;
        return true;
    }

    permission_broadcast = true;
    bc_data.addr.sin_family = AF_INET;
    bc_data.addr.sin_addr.s_addr = inet_addr(brd_addr.c_str());
    bc_data.addr.sin_port = htons(bc_port);
    bc_data.addr_len = sizeof(bc_data.addr); 

    int result = setsockopt(sd.sock, SOL_SOCKET, SO_BROADCAST, (char *)&broadcast_flag, sizeof(broadcast_flag));
    if(result < 0){
        cout << "Couldn't enabled" << endl;
        return false;
    }
    else{
        cout << "Broadcast enabled" << endl;
        return true;
    }
}

int UDPUnit::send_all(void *buf){
    int size = sendto(sd.sock, buf, sizeof(*(uint8_t *)buf), 0, (struct sockaddr *)&(bc_data.addr), sizeof(bc_data.addr_len));
    return size;
}