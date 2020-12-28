#include<zmqpp/zmqpp.hpp>
#include <signal.h>

#include<iostream>
#include<sstream>
#include<set>
#include<string>
#include<vector>

#include"server.hpp"
#include"BinTree.hpp"



void print_menu(){
    std::cout << "----------------------------------" << std::endl
              << "create [id]" << std::endl
              << "remove [id]" << std::endl
              << "exec [id] [cmd - start/stop/time]" << std::endl
              << "heartbeat [time (ms)]" << std::endl
              << "menu" << std::endl
              << "exit" << std::endl
              << "----------------------------------" << std::endl;
}

struct Token{
    BinTree *ptrTree;
    int time;
    std::unique_ptr<zmqpp::socket> ping_ptr;
};

void* count(void* arg){
    Token* tok = (Token*)arg;
    BinTree* tree = tok->ptrTree;
    std::vector<int> unavailable;
    int time = tok->time;
    while(tok->ping_ptr){
        for(int i = 0; i < time; ++i){
            usleep(1000);
            tree->add_time();
        }
        unavailable = tree->unable_nodes(time);
        if(!unavailable.empty()){
            for(auto i : unavailable){
                std::cout << "Heartbit: node " << i << " is unavailable now\n";
            }
        }
        unavailable.clear();
    }
    return nullptr;
}
void* ping(void* arg){
    Token* tok = (Token*) arg;
    BinTree* tree = tok->ptrTree;
    int id;
    size_t length = sizeof(int);
    while(tok->ping_ptr){
        tok->ping_ptr->receive_raw((char*)&id, length);
        tree->reset(id);
    }
    return nullptr;
}
int main() {
    BinTree tree;

    int child_pid = 0;
    int child_id = 0;

    Token tok;
    pthread_t thread;
    pthread_t thread2;

    zmqpp::context context;
    zmqpp::socket main_socket(context, zmqpp::socket_type::req);
    zmqpp::socket ping_socket(context, zmqpp::socket_type::pull);


    int port = bind_socket(main_socket);
    int ping_port = bind_socket(ping_socket);

    int input_id;
    std::string result;
    std::string msg;
    std::string cmd;
    std::string subcmd;


    print_menu();
    while (true) {
        std::cin >> cmd;
        if (cmd == "create") {
            std::cin >> input_id;
            if (child_pid == 0) {
                child_pid = fork();
                if (child_pid == -1) {
                    std::cout << "Unable to create first worker node" << std::endl;
                    child_pid = 0;
                    exit(1);
                } else if (child_pid == 0) {
                    create_node(input_id, port, ping_port);
                } else {
                    child_id = input_id;
                    msg = "pid";
                    send_msg(main_socket, msg);
                    result = recieve_msg(main_socket);
                }
            }
            else {
                std::ostringstream msg_stream;
                msg_stream << "create " << input_id;
                send_msg(main_socket, msg_stream.str());
                result = recieve_msg(main_socket);
            }

            if (result.substr(0,2) == "OK") {
                tree.insert(input_id);
            }
            std::cout << result << std::endl;

        }
        else if (cmd == "remove") {
            if (child_pid == 0) {
                std::cout << "Error: Not found" << std::endl;
                continue;
            }

            std::cin >> input_id;
            if (input_id == child_id) {
                kill(child_pid, SIGTERM);
                kill(child_pid, SIGKILL);
                child_id = 0;
                child_pid = 0;
                std::cout << "OK" << std::endl;
                tree.erase(input_id);
                continue;
            }
            msg = "remove " + std::to_string(input_id);
            send_msg(main_socket, msg);
            result = recieve_msg(main_socket);
            if (result.substr(0, 2) == "OK") {
                tree.erase(input_id);
            }
            std::cout << result << std::endl;

        }
        else if (cmd == "exec") {
            std::cin >> input_id >> subcmd;
            std::vector<int> path = tree.get_path_to(input_id);
            if(path.empty()) {
                std::cout << "Error: Not found" << std::endl;
                continue;
            }
            path.erase(path.begin());
            msg = "exec " + subcmd + " " + std::to_string(path.size());
            for (int i : path) {
                msg += " " + std::to_string(i);
            }
            send_msg(main_socket, msg);
            result = recieve_msg(main_socket);
            std::cout << result << std::endl;

        }
        else if (cmd == "heartbeat") {
            int timing;
            std::cin >> timing;
            if (child_pid == 0) {
                std::cout << "Error: Not found" << std::endl;
                continue;
            }
            msg = "heartbeat "+ std::to_string(timing);
            send_msg(main_socket, msg);
            result = recieve_msg(main_socket);
            std::cout << result << std::endl;
            tok.ping_ptr.reset(&ping_socket);
            tok.ptrTree = &tree;
            tok.time = timing;
            pthread_create(&thread, nullptr, ping, &tok);
            pthread_create(&thread2, nullptr, count, &tok);
        }
        else if (cmd == "menu") {
            print_menu();
        }
        else if (cmd == "exit") {
            if (child_pid > 0) {
                send_msg(main_socket, "kill_child");
                result = recieve_msg(main_socket);
                if (result.substr(0, 2) == "OK") {
                    kill(child_pid, SIGTERM);
                    kill(child_pid, SIGKILL);
                    child_id = 0;
                    child_pid = 0;
                    std::cout << "OK" << std::endl;
                    tree.erase(input_id);
                }
            }
            break;
        }
    }
    return 0;
}