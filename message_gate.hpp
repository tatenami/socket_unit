#ifndef MESSAGE_GATE_H
#define MESSAGE_GATE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <string>
#include <iostream>

namespace msglib{

    /* --- [ const values ] --- */

    extern const std::string loopback_address;
    extern const int non_blocking_flag;
    extern const int blocking_flag;

        

    /* --- [ Basic Datas ] ---*/

    class DstInfo{
    public:
        sockaddr_in addr;
        int         addr_len;

    public:
        DstInfo(std::string ip_address, int port_number);
    };

    struct socket_data{
        int         sock_fd;
        sockaddr_in addr;
        socklen_t   addr_len;
    };

    /* --- [ SocketBase Class ] --- */

    class SocketBase{
    protected:
        socket_data sd_;        // sender: destination socket data.  receiver: own socket data.
        std::string ip_addr;
        uint16_t    port;

    protected:
        virtual void set_socket(std::string ip_address, int port_number) = 0;
        void print_socket_info();

    public:
        SocketBase(std::string ip_address, int port_number);
        virtual ~SocketBase();
    };

    /* --- [ UDP Classes ] ---*/

    class UDPSender: public SocketBase{
    protected:
        void set_socket(std::string ip_address, int port_number);

    public:
        UDPSender(std::string ip_address, int port_number);
        void set_dst_info(DstInfo new_dst);
        void set_dst_info(std::string ip_address, int port_number);
        int send(void* msg, int size) const;
        int send(void* msg, int size, DstInfo dst) const;
        int send(void* msg, int size, std::string ip_address, int port_number) const;
    };
    
    class UDPReceiver: public SocketBase{
    protected:
        sockaddr_in sender_addr_;

    protected:
        void set_socket(std::string ip_address, int port_number);

    public:
        UDPReceiver(std::string ip_address, int port_number);
        void change_to_non_blocking();
        void change_to_blocking();
        int receive(void *buf, int size);
        char* get_sender_addr();
        int get_sender_port();
    };

    /* --- [ UDPUnit Class ] ---- */

    class UDPUnit{
    public:
        UDPSender   sender;
        UDPReceiver receiver;

    public:
        UDPUnit(std::string ip_address, int port_number);
    };
};

#endif // MESSAGE_GATE_H