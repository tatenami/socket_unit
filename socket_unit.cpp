#include "socket_unit.h"

const string brd_addr = "255.255.255.255";
const string loopback_addr = "127.0.0.1";

/*-----------[destionation device data]------------*/

DstData::DstData(string ip_addr, int port){
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    addr.sin_port = htons(port);
}

/*-----------[socket unit function]------------*/

void SocketUnit::bind_socket(socket_data& sd){
    bind(sd.sock, (const struct sockaddr *)&(sd.addr), sd.addr_len);
    //bind(global_socket.sock, (const struct sockaddr *)&(global_socket.addr), global_socket.addr_len);
}

SocketUnit::~SocketUnit(){
    close(global_socket.sock);
}

int SocketUnit::receive(){
    unsigned int addr_len = sizeof(src_data);
    int size = recvfrom(global_socket.sock, RxBuf, BUF_SIZE, 0, (struct sockaddr *)&src_data, &addr_len);
    return size;
}

void SocketUnit::get_RxData(void *buf){
    memcpy(buf, RxBuf, BUF_SIZE);
}

char* SocketUnit::get_src_addr(){
    return inet_ntoa(src_data.sin_addr);
}

int SocketUnit::get_src_port(){
    return ntohs(src_data.sin_port);
}

/*-----------[UDP unit function]-----------*/

void UDPUnit::set_socket(socket_data& sd, string ip_addr, uint16_t port){
    sd.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sd.addr.sin_family = AF_INET;
    sd.addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    sd.addr.sin_port = htons(port);
    sd.addr_len = sizeof(sd.addr);
}

UDPUnit::UDPUnit(string ip_addr, int port){
    // global_socket.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // global_socket.addr.sin_family = AF_INET;
    // global_socket.addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    // global_socket.addr.sin_port = htons(port);
    // global_socket.addr_len = sizeof(global_socket.addr);
    set_socket(global_socket, ip_addr, port);
    bind_socket(global_socket);
}

int UDPUnit::set_local_socket(uint16_t local_port){
    // local_socket.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // local_socket.addr.sin_family = AF_INET;
    // local_socket.addr.sin_addr.s_addr = inet_addr(loopback_addr.c_str());
    // local_socket.addr.sin_port = htons(local_port);
    // local_socket.addr_len = sizeof(local_socket.addr);
    set_socket(local_socket, loopback_addr, local_port);
    bind_socket(local_socket);
    return 0;
}

int UDPUnit::send(void *buf, DstData *dst){
    int size = sendto(global_socket.sock, buf, sizeof(*(uint8_t *)buf), 0, (struct sockaddr *)&(dst->addr), sizeof(dst->addr));
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

    int result = setsockopt(global_socket.sock, SOL_SOCKET, SO_BROADCAST, (char *)&broadcast_flag, sizeof(broadcast_flag));
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
    int size = sendto(global_socket.sock, buf, sizeof(*(uint8_t *)buf), 0, (struct sockaddr *)&(bc_data.addr), sizeof(bc_data.addr_len));
    return size;
}