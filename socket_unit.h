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

/*----------[socket data struct]-----------*/

typedef struct{
    int sock;
    struct sockaddr_in addr;
    uint16_t addr_len;
}socket_data;

class DstUnit{
public:
    struct sockaddr_in addr;
    DstUnit(string ip_addr, uint16_t port);
    DstUnit(uint16_t local_port);
};

/*----------[socket_unit: base class]----------*/

class SocketUnit{
protected:
    socket_data sd;

    struct sockaddr_in src_data;
    uint8_t RxBuf[BUF_SIZE];

    virtual void set_socket(string ip_addr, uint16_t port) = 0;
    void bind_socket();
public:
    ~SocketUnit();

    int receive();
    void get_RxData(void *buf);
    char* get_src_addr();
    int get_src_port();
};

/*----------[UDP_socket_unit]-----------*/

class UDPUnit: public SocketUnit{
protected:
    socket_data bc_data;
    bool permission_broadcast = false;

    void set_socket(string ip_addr, uint16_t port);         
public:
    UDPUnit(string ip_addr, uint16_t port);
    UDPUnit(uint16_t local_port);

    int send(void *buf, DstUnit *dst, int len);
    bool enable_broadcast(int bc_port);
    int send_all(void *buf);
};

#endif