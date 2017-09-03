//
// Created by lg on 17-5-14.
//
#pragma once

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "DebugPrint.h"
#include <cstring>

class InetAddress final {
public:
    InetAddress() = default;

    explicit InetAddress(const std::string &ip, in_port_t port, sa_family_t family = AF_INET) {
        std::memset(&_addr6, 0, size6());

        _addr.sin_family = family;
        _addr.sin_port = ::htons(port);

        if (::inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr) <= 0) {
            dout << "inet_pton 失败";
        }
    }

    explicit InetAddress(bool loopback, in_port_t port, sa_family_t family = AF_INET) {
        std::memset(&_addr6, 0, size6());

        _addr.sin_family = family;
        _addr.sin_port = ::htons(port);

        _addr.sin_addr.s_addr = loopback ? ::htonl(INADDR_LOOPBACK) : ::htonl(INADDR_ANY);
    }

    const sockaddr *getSockAddr() const { return reinterpret_cast<const sockaddr *>(&_addr); }

    sockaddr *getSockAddr() { return reinterpret_cast<sockaddr *>(&_addr); }

    sa_family_t getfamily() const { return _addr.sin_family; }

    in_port_t portNetEndian() const { return _addr.sin_port; }
    int portHostEndianconst(){return ntohs(_addr.sin_port); }

    static constexpr socklen_t size() { return static_cast<socklen_t>(sizeof(sockaddr_in)); }

    static constexpr socklen_t size6() { return static_cast<socklen_t>(sizeof(sockaddr_in6)); }

private:
    union {
        sockaddr_in _addr;
        sockaddr_in6 _addr6;
    };
};
