//
// Created by nikita on 28.12.2020.
//

#ifndef LAB6_8_SERVER_HPP
#define LAB6_8_SERVER_HPP
#include<zmqpp/zmqpp.hpp>
#include<unistd.h>
#include<iostream>
#include<string>
#define check(VAL, BADVAL, MSG) if (VAL == BADVAL) { perror(MSG); exit(1); }

const std::string host = "tcp://127.0.0.1:";

int get_port() {
    static unsigned int port(3000);
    static zmqpp::context context;
    static zmqpp::socket socket(context, zmqpp::socket_type::pull);
    while (true) {
        std::cout << "trying " << port;
        try {
            socket.bind(host + std::to_string(port));
        } catch (zmqpp::zmq_internal_exception& ex) {
            ++port;
            std::cout << std::endl;
            continue;
        }
        socket.unbind(host + std::to_string(port));
        std::cout << " ok" << std::endl;
        return port++;
    }
}

void create_node(int& id, int& port, int& ping_port) {
    char* arg_node = strdup("./node");
    char* arg_id = strdup((std::to_string(id)).c_str());
    char* arg_port = strdup((std::to_string(port)).c_str());
    char* arg_ping_port = strdup((std::to_string(ping_port)).c_str());
    char* args[] = {arg_node, arg_id, arg_port, arg_ping_port, nullptr};
    execv("./node", args);
}

bool send_msg(zmqpp::socket& socket, const std::string& msg) {
    zmqpp::message message;
    message << msg;
    try {
        socket.send(message);
        return true;
    } catch(...) {
        return false;
    }
}

std::string recieve_msg(zmqpp::socket& socket) {
    zmqpp::message message;
    try {
        socket.receive(message);
    } catch(zmqpp::zmq_internal_exception& e) {
        message = false;
    }
    std::string answer;
    message >> answer;
    return answer;
}

int bind_socket(zmqpp::socket& socket) {
    int port = 3000;
    while (true) {
        try {
            socket.bind(host+std::to_string(port));
            break;
        } catch(zmqpp::zmq_internal_exception& e) {
            ++port;
        }
    }
    return port;
}
#endif //LAB6_8_SERVER_HPP
