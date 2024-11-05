#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define BUF_SIZE 1024

void send_tcp_message() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[BUF_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha na inicializacao do Winsock.\n");
        return;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Erro ao criar socket TCP\n");
        WSACleanup();
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Conexao TCP falhou\n");
        closesocket(sock);
        WSACleanup();
        return;
    }

    printf("Digite a mensagem (TCP): ");
    fgets(buffer, BUF_SIZE, stdin);
    send(sock, buffer, strlen(buffer), 0);
    closesocket(sock);
    WSACleanup();
}

void send_udp_message() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[BUF_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha na inicializacao do Winsock.\n");
        return;
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Erro ao criar socket UDP\n");
        WSACleanup();
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Digite a mensagem (UDP): ");
    fgets(buffer, BUF_SIZE, stdin);
    sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    closesocket(sock);
    WSACleanup();
}

int main() {
    int choice;
    while (1) {
        printf("Escolha o modo de envio:\n1. TCP\n2. UDP\n3. Sair\n");
        scanf("%d", &choice);
        getchar();  // Limpa o newline do buffer
        if (choice == 1) {
            send_tcp_message();
        } else if (choice == 2) {
            send_udp_message();
        } else if (choice == 3) {
            break;
        } else {
            printf("Escolha invalida!\n");
        }
    }
    return 0;
}
