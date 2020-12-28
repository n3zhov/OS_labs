//
// Created by nikita on 28.12.2020.
//
#include<zmqpp/zmqpp.hpp>
#include<csignal>

#include<iostream>
#include <sstream>
#include<string>
#include<vector>
#include<unistd.h>
#include<pthread.h>

#include"server.hpp"

zmqpp::context context;
struct Token{
    int time;
    int port;
    int id;
};

void* send_ping(void* arg){
    Token* tok = (Token*)arg;
    int microseconds = tok->time*1000;
    int id = tok->id;
    int port = tok->port;
    zmqpp::socket ping_socket(context, zmqpp::socket_type::push);
    size_t length = sizeof(int);
    try{
        ping_socket.connect(host+std::to_string(port));
    }
    catch(zmqpp::zmq_internal_exception& ex){
        std::cout << "Bind error!\n";
        return nullptr;
    }
    while(ping_socket){
        ping_socket.send_raw((char*)&id, length, zmqpp::socket::dont_wait);
        usleep(microseconds);
    }
    return nullptr;
}

int main(int argc, char** argv) {
    if(argc != 4) {
        std::cerr << "Not enough parameters" << std::endl;
        exit(-1);
    }
    pthread_t thread;
    Token tok;
    int time;
    int id = std::stoi(argv[1]);
    int port_parent = std::stoi(argv[2]);
    int port_ping = std::stoi(argv[3]);
    tok.port = port_ping;
    tok.id = id;
    zmqpp::socket parent_socket(context, zmqpp::socket_type::rep);
    parent_socket.connect(host+std::to_string(port_parent));

    int left_pid = 0;
    int right_pid = 0;
    int left_id = 0;
    int right_id = 0;

    zmqpp::socket_t left_socket(context, zmqpp::socket_type::req);
    zmqpp::socket_t right_socket(context, zmqpp::socket_type::req);


    int left_port = bind_socket(left_socket);
    int right_port = bind_socket(right_socket);

    std::string request;
    std::string msg;
    std::string cmd;
    std::string subcmd;

    int value;

    std::string left_res;
    std::string right_res;

    int input_id;

    auto start_clock = std::chrono::high_resolution_clock::now();
    auto stop_clock = std::chrono::high_resolution_clock::now();
    auto time_clock = 0;
    bool flag_clock = false;


    while (true) {
        request = recieve_msg(parent_socket);
        std::istringstream cmd_stream(request);
        cmd_stream >> cmd;
        msg.clear();
        std::ostringstream res(msg);
        if (cmd == "id") {
            msg = "OK: " + std::to_string(id);
            send_msg(parent_socket, msg);
        } else if (cmd == "pid") {
            msg = "OK: " + std::to_string(getpid());
            send_msg(parent_socket, msg);
        } else if (cmd == "create") {
            cmd_stream >> input_id;
            if (input_id == id) {
                msg = "Error: Already exists";
                send_msg(parent_socket, msg);
            } else if (input_id < id) {
                if (left_pid == 0) {
                    left_pid = fork();
                    if (left_pid == -1) {
                        msg = "Error: Cannot fork";
                        send_msg(parent_socket, msg);
                        left_pid = 0;
                    } else if (left_pid == 0) {
                        create_node(input_id, left_port, port_ping);
                    } else {
                        left_id = input_id;
                        msg = "pid";
                        send_msg(left_socket, msg);
                        send_msg(parent_socket, recieve_msg(left_socket));
                    }
                } else {
                    send_msg(left_socket, request);
                    send_msg(parent_socket, recieve_msg(left_socket));
                }
            } else {
                if (right_pid == 0) {
                    right_pid = fork();
                    if (right_pid == -1) {
                        msg = "Error: Cannot fork";
                        send_msg(parent_socket, msg);
                        right_pid = 0;
                    } else if (right_pid == 0) {
                        create_node(input_id, right_port, port_ping);
                    } else {
                        right_id = input_id;
                        msg = "pid";
                        send_msg(right_socket, msg);
                        send_msg(parent_socket, recieve_msg(right_socket));
                    }
                } else {
                    send_msg(right_socket, request);
                    send_msg(parent_socket, recieve_msg(right_socket));
                }
            }
        } else if (cmd == "remove") {
            cmd_stream >> input_id;
            if (input_id < id) {
                if (left_id == 0) {
                    msg = "Error: Not found";
                    send_msg(parent_socket, msg);
                } else if (left_id == input_id) {
                    msg = "kill_child";
                    send_msg(left_socket, msg);
                    msg = recieve_msg(left_socket);
                    kill(left_pid, SIGTERM);
                    kill(left_pid, SIGKILL);
                    left_id = 0;
                    left_pid = 0;
                    send_msg(parent_socket, msg);
                } else {
                    send_msg(left_socket, request);
                    send_msg(parent_socket, recieve_msg(left_socket));
                }
            } else {
                if (right_id == 0) {
                    msg = "Error: Not found";
                    send_msg(parent_socket, msg);
                } else if (right_id == input_id) {
                    msg = "kill_child";
                    send_msg(right_socket, msg);
                    msg = recieve_msg(right_socket);
                    kill(right_pid, SIGTERM);
                    kill(right_pid, SIGKILL);
                    right_id = 0;
                    right_pid = 0;
                    send_msg(parent_socket, msg);
                } else {
                    send_msg(right_socket, request);
                    send_msg(parent_socket, recieve_msg(right_socket));
                }
            }

        } else if (cmd == "exec") {
            cmd_stream >> subcmd >> value;
            std::vector<int> path(value);
            for(int i = 0; i < value; ++i){
                cmd_stream >> path[i];
            }
            if(path.empty()) {
                msg = "OK: " + std::to_string(id) + " " + subcmd;
                if(subcmd == "start") {
                    start_clock = std::chrono::high_resolution_clock::now();
                    flag_clock = true;
                }
                else if(subcmd == "stop") {
                    if(flag_clock) {
                        stop_clock = std::chrono::high_resolution_clock::now();
                        time_clock += std::chrono::duration_cast<std::chrono::milliseconds>(stop_clock - start_clock).count();
                        flag_clock = false;
                    }
                } else if(subcmd == "time") {
                    msg += ": " + std::to_string(time_clock);
                }
                send_msg(parent_socket, msg);
            } else {
                input_id = path.front();
                path.erase(path.begin());
                res << "exec " << subcmd << " " << path.size();
                for(int i: path){
                    res << " " << i;
                }
                if (input_id < id) {
                    send_msg(left_socket, res.str());
                    msg = recieve_msg(left_socket);
                    send_msg(parent_socket, msg);
                } else {
                    send_msg(right_socket, res.str());
                    msg = recieve_msg(right_socket);
                    send_msg(parent_socket, msg);
                }
            }

        }
        else if (cmd == "heartbeat") {
            cmd_stream >> time;
            tok.time = time;
            pthread_create(&thread, nullptr, send_ping, &tok);
            if (left_pid != 0) {
                send_msg(left_socket, request);
                left_res = recieve_msg(left_socket);
            }
            if (right_pid != 0) {
                send_msg(right_socket, request);
                right_res = recieve_msg(right_socket);
            }
            if (!left_res.empty() && left_res.substr(0, 5) != "Error") {
                msg += left_res + '\n';
            }
            if (!right_res.empty() && right_res.substr(0, 5) != "Error") {
                msg += right_res + '\n';
            }
            msg += "OK: " + std::to_string(id);
            send_msg(parent_socket, msg);

        } else if (cmd == "kill_child") {
            if (left_pid == 0 && right_pid == 0) {
                msg = "OK";
                send_msg(parent_socket, msg);
            } else {
                if (left_pid != 0) {
                    msg = "kill_child";
                    send_msg(left_socket, msg);
                    recieve_msg(left_socket);
                    kill(left_pid, SIGTERM);
                    kill(left_pid, SIGKILL);
                }
                if (right_pid != 0) {
                    msg = "kill_child";
                    send_msg(right_socket, msg);
                    recieve_msg(right_socket);
                    kill(right_pid, SIGTERM);
                    kill(right_pid, SIGKILL);
                }
                msg = "OK";
                send_msg(parent_socket, msg);
            }
        }
        if (port_parent == 0) {
            break;
        }
    }
    return 0;
}
