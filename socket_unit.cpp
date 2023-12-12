#include "socket_unit.h"

const string brd_addr = "255.255.255.255";

DstData::DstData(string ip_addr, int port){
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    addr.sin_port = htons(port);
}

void SocketUnit::bind_socket(){
    bind(global_socket.sock, (const struct sockaddr *)&(global_socket.addr), global_socket.addr_len);
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


UDPUnit::UDPUnit(string ip_addr, int port){
    global_socket.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    global_socket.addr.sin_family = AF_INET;
    global_socket.addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    global_socket.addr.sin_port = htons(port);
    global_socket.addr_len = sizeof(global_socket.addr);
    bind_socket();
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