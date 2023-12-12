#ifndef SOCKET_UNIT_H
#define SOCKET_UNIT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>

using namespace std;

#define USE_BROADCST 1
#define BUF_SIZE 4

extern const string brd_addr;
const static int broadcast_flag = 1;

typedef struct{
    int sock;
    struct sockaddr_in addr;
    uint16_t addr_len;
}socket_data;

class DstData{
public:
    struct sockaddr_in addr;
    DstData(string ip_addr, int port);
};

class SocketUnit{
protected:
    socket_data global_socket;
    struct sockaddr_in src_data;
    uint8_t RxBuf[BUF_SIZE];
    void bind_socket();
public:
    ~SocketUnit();

    int receive();
    void get_RxData(void *buf);
    char* get_src_addr();
    int get_src_port();
};

class UDPUnit: public SocketUnit{
protected:
    socket_data bc_data;
    bool permission_broadcast = false;
public:
    UDPUnit(string ip_addr, int port);
    int send(void *buf, DstData *dst);

    bool enable_broadcast(int bc_port);
    int send_all(void *buf);
};
#endif