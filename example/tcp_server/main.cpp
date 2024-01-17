#include <iostream>
#include <thread>
#ifdef _WIN32
#    include <winsock2.h>
#    pragma comment(lib, "ws2_32.lib")
#else
#    include <arpa/inet.h>
#    include <fcntl.h>
#    include <netinet/in.h>
#    include <sys/select.h>
#    include <sys/socket.h>
#    include <unistd.h>
#endif

#define PORT 1150

void handle_client(int client_sock)
{
    char buffer[4096];
    size_t bytes_received;

    while ((bytes_received = recv(client_sock, buffer, sizeof(buffer), 0)) > 0) {
        std::cout << buffer << '\n';
        send(client_sock, buffer, bytes_received, 0);
    }

#ifdef _WIN32
    closesocket(client_sock);
#else
    close(client_sock);
#endif
}

int main()
{
#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return 1;
    }
#endif

    auto sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1) {
        std::cerr << "Error creating socket\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error binding socket\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (listen(sockfd, SOMAXCONN) == -1) {
        std::cerr << "Error listening on socket\n";
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    while (true) {
        sockaddr_in client_address;

#ifdef _WIN32
        int client_len = sizeof(client_address);
#else
        socklen_t client_len = sizeof(client_address);
#endif

        int client_sock = accept(sockfd, (struct sockaddr *)&client_address, &client_len);
        if (client_sock == -1) {
            std::cerr << "Error accepting connection\n";
            continue;
        }

        std::cout << "Accepted connection from: " << inet_ntoa(client_address.sin_addr) << '\n';

        std::thread client_thread(handle_client, client_sock);
        client_thread.detach();
    }

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
