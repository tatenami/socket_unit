#include "message_gate.hpp"

/* --- [ const values ] --- */

const std::string msglib::loopback_address{"127.0.0.1"};
const int msglib::non_blocking_flag{1};
const int msglib::blocking_flag{0};

/* --- [ SocketBase Class ] ---*/

void msglib::SocketBase::print_socket_info(){
    std::cout << "[ IP:" << this->ip_addr << " PORT: " << this->port << " ]" << "\n";
}

msglib::SocketBase::SocketBase(std::string ip_address, int port_number){
    this->ip_addr = ip_address;
    this->port = port_number;
}

msglib::SocketBase::~SocketBase(){
    close(sd_.sock_fd);
    std::cout << "close socket" << std::endl;
}

/* --- [ DstInfo class ] ---*/

msglib::DstInfo::DstInfo(std::string ip_address, int port_number){
    this->addr.sin_family = AF_INET;
    this->addr.sin_addr.s_addr = inet_addr(ip_address.c_str());
    this->addr.sin_port = htons(port_number);
    this->addr_len = sizeof(this->addr);
}

/* --- [ UDPSender Class ] ---*/

void msglib::UDPSender::set_socket(std::string ip_address, int port_number){
    sd_.sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sd_.addr.sin_family = AF_INET;
    sd_.addr.sin_addr.s_addr = inet_addr(ip_address.c_str());
    sd_.addr.sin_port = htons(port_number);
    sd_.addr_len = static_cast<socklen_t>(sizeof(sd_.addr));
}

msglib::UDPSender::UDPSender(std::string ip_address, int port_number): 
    SocketBase(ip_address, port_number)
{
    set_socket(ip_address, port_number);
    std::cout << "Create UDPSender: ";
    print_socket_info();
}

void msglib::UDPSender::set_dst_info(DstInfo new_dst){
    set_socket(std::string(inet_ntoa(new_dst.addr.sin_addr)), static_cast<int>(ntohs(new_dst.addr.sin_port)));
}

void msglib::UDPSender::set_dst_info(std::string ip_address, int port_number){
    sd_.addr.sin_family = AF_INET;
    sd_.addr.sin_addr.s_addr = inet_addr(ip_address.c_str());
    sd_.addr.sin_port = htons(port_number);
    sd_.addr_len = static_cast<socklen_t>(sizeof(sd_.addr));
}

int msglib::UDPSender::send(void* msg, int size) const{
    int result_size = sendto(sd_.sock_fd, msg, size, 0, (struct sockaddr *)&(sd_.addr), sd_.addr_len);
    if(result_size < 0){
        std::cout << "fail to send data" << std::endl;
    }
    return size;
}

int msglib::UDPSender::send(void* msg, int size, DstInfo dst) const{
    return sendto(sd_.sock_fd, msg, size, 0, (struct sockaddr *)&(dst.addr), dst.addr_len);
}

int msglib::UDPSender::send(void* msg, int size, std::string ip_address, int port_number) const{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_address.c_str());
    addr.sin_port = htons(port_number);
    return sendto(sd_.sock_fd, msg, size, 0, (struct sockaddr *)&(sd_.addr), sd_.addr_len);
}

/* --- [ UDPReceiver Class ] --- */

void msglib::UDPReceiver::set_socket(std::string ip_address, int port_number){
    sd_.sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sd_.addr.sin_family = AF_INET;
    sd_.addr.sin_addr.s_addr = inet_addr(ip_address.c_str());
    sd_.addr.sin_port = htons(port_number);
    sd_.addr_len = sizeof(sd_.addr);
    bind(sd_.sock_fd, (const struct sockaddr *)&(sd_.addr), sd_.addr_len);
}

msglib::UDPReceiver::UDPReceiver(std::string ip_address, int port_number):
    SocketBase(ip_address, port_number)
{
    set_socket(ip_address, port_number);
    std::cout << "Create UDPReceiver: ";
    print_socket_info();
}

int msglib::UDPReceiver::receive(void* buf, int size){
    socklen_t addr_len = sizeof(sender_addr_);
    int result_size = recvfrom(sd_.sock_fd, buf, size, 0, (struct sockaddr *)&sender_addr_, &addr_len);
    return size;
}

void msglib::UDPReceiver::change_to_non_blocking(){
    ioctl(sd_.sock_fd, FIONBIO, &non_blocking_flag);
}

void msglib::UDPReceiver::change_to_blocking(){
    ioctl(sd_.sock_fd, FIONBIO, &blocking_flag);
}

char* msglib::UDPReceiver::get_sender_addr(){
    return inet_ntoa(sender_addr_.sin_addr);
}

int msglib::UDPReceiver::get_sender_port(){
    return static_cast<int>(ntohs(sender_addr_.sin_port));
}

/* --- [ UDPUnit Class ] --- */

msglib::UDPUnit::UDPUnit(std::string ip_address, int port_number):
    sender(ip_address, port_number),
    receiver(ip_address, port_number)
{
    
}