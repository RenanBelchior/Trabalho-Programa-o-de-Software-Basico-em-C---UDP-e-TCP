#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define BUF_SIZE 1024

void handle_tcp(SOCKET tcp_sock) {
    char buffer[BUF_SIZE];
    SOCKET new_socket;
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);

    printf("Esperando conexões TCP...\n");
    new_socket = accept(tcp_sock, (struct sockaddr*)&client_addr, &addrlen);
    if (new_socket == INVALID_SOCKET) {
        perror("Falha na conexão TCP");
        closesocket(tcp_sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    while (recv(new_socket, buffer, BUF_SIZE, 0) > 0) {
        buffer[strlen(buffer)] = '\0';
        printf("TCP: %s\n", buffer);
    }
    closesocket(new_socket);
}

void handle_udp(SOCKET udp_sock) {
    char buffer[BUF_SIZE];
    struct sockaddr_in client_addr;
    int addr_len = sizeof(client_addr);

    printf("Esperando mensagens UDP...\n");
    while (1) {
        int len = recvfrom(udp_sock, buffer, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len);
        if (len == SOCKET_ERROR) {
            printf("Erro ao receber mensagem UDP\n");
            continue;
        }
        buffer[len] = '\0';
        printf("UDP: %s\n", buffer);
    }
}

int main() {
    WSADATA wsaData;
    SOCKET tcp_sock, udp_sock;
    struct sockaddr_in server_addr;

    // Inicializar o Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha na inicialização do Winsock.\n");
        return 1;
    }

    // Configuração TCP
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock == INVALID_SOCKET) {
        printf("Erro ao criar socket TCP\n");
        WSACleanup();
        return 1;
    }

    // Configuração UDP
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock == INVALID_SOCKET) {
        printf("Erro ao criar socket UDP\n");
        closesocket(tcp_sock);
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind para TCP
    if (bind(tcp_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Erro ao fazer bind TCP\n");
        closesocket(tcp_sock);
        closesocket(udp_sock);
        WSACleanup();
        return 1;
    }
    listen(tcp_sock, 3);

    // Bind para UDP
    if (bind(udp_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Erro ao fazer bind UDP\n");
        closesocket(tcp_sock);
        closesocket(udp_sock);
        WSACleanup();
        return 1;
    }

    // Processamento das conexões TCP e UDP
    HANDLE tcp_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)handle_tcp, (LPVOID)tcp_sock, 0, NULL);
    handle_udp(udp_sock);

    // Fechar conexões
    WaitForSingleObject(tcp_thread, INFINITE);
    CloseHandle(tcp_thread);
    closesocket(tcp_sock);
    closesocket(udp_sock);
    WSACleanup();

    return 0;
}

