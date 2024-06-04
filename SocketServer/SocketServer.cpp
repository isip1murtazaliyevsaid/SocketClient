#define WIN32_LEAN_AND_MEAN // уменьшаем объем заголовков Windows для ускорения компиляции

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
    WSADATA wsaData; // структура для хранения информации о инициализации Winsock
    ADDRINFO  hints; // структура для предоставления подсказок функции getaddrinfo()
    ADDRINFO* addrResult; // указатель на список адресов, возвращенных getaddrinfo()
    SOCKET ConnectSocket = INVALID_SOCKET; // сокет для соединения с клиентом
    SOCKET ListenSocket = INVALID_SOCKET; // сокет для прослушивания входящих соединений
    char recvBuffer[512]; // буфер для приема данных от клиента

    const char* sendBuffer = "\nHello from server"; // буфер для отправки данных клиенту

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // инициализируем библиотеку Winsock
    if (result != 0) {
        cout << "WSAStartup failed with error: " << result << endl; // выводим сообщение об ошибке и выходим из программы
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); // очищаем структуру hints
    hints.ai_family = AF_INET; // указываем семейство адресов IPv4
    hints.ai_socktype = SOCK_STREAM; // указываем тип сокета - потоковый сокет
    hints.ai_protocol = IPPROTO_TCP; // указываем протокол TCP/IP
    hints.ai_flags = AI_PASSIVE; // указываем, что сокет будет использоваться для принятия входящих соединений

    result = getaddrinfo(NULL, "666", &hints, &addrResult); // получаем информацию о сетевом адресе и порте
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl; // выводим сообщение об ошибке и выходим из программы
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // создаем сокет для прослушивания
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl; // выводим сообщение об ошибке и выходим из программы
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); // привязываем сокет к определенному адресу и порту
    if (result == SOCKET_ERROR) {
        cout << "bind failed with error: " << WSAGetLastError() << endl; // выводим сообщение об ошибке и выходим из программы
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = listen(ListenSocket, SOMAXCONN); // устанавливаем сокет в режим прослушивания
    if (result == SOCKET_ERROR) {
        cout << "listen failed with error: " << WSAGetLastError() << endl; // выводим сообщение об ошибке и выходим из программы
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ConnectSocket = accept(ListenSocket, NULL, NULL); // принимаем входящее соединение
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "accept failed with error: " << WSAGetLastError() << endl; // выводим сообщение об ошибке и выходим из программы
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    do {
        ZeroMemory(recvBuffer, 512); // очищаем буфер приема
        result = recv(ConnectSocket, recvBuffer, 512, 0); // принимаем данные от клиента

        if (result > 0) {
            cout << "Received: " << result << " bytes" << endl; // выводим количество принятых байт
            cout << "Received data: " << recvBuffer << endl; // выводим принятые данные

            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0); // отправляем данные клиенту

            if (result == SOCKET_ERROR) {
                cout << "send failed with error: " << WSAGetLastError() << endl; // выводим сообщение об ошибке и выходим из программы
                closesocket(ListenSocket);
                freeaddrinfo(addrResult);
                WSACleanup();
                return 1;
            }
        }
        else if (result == 0) {
            cout << "\nConnection closing" << endl; // выводим сообщение о закрытии соединения
        }
        else {
            cout << "recv failed with error: " << WSAGetLastError() << endl; // выводим сообщение об ошибке и выходим из программы
            closesocket(ConnectSocket);
        }

    } while (result > 0); // продолжаем цикл, пока принимаем данные от клиента

    result = shutdown(ConnectSocket, SD_SEND); // закрываем сокет для отправки данных

    if (result == SOCKET_ERROR) {
        cout << "shutdown failed with error: " << WSAGetLastError() << endl; // выводим сообщение об ошибке и выходим из программы
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addrResult); // освобождаем память, выделенную для структуры addrResult
    WSACleanup(); // завершаем использование Winsock
    return 0;
}