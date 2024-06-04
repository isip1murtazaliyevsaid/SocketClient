#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main()
{
    WSADATA wsaData; // структура для хранения информации о реализации Windows Sockets
    ADDRINFO hints; // структура для хранения параметров сокета
    ADDRINFO* addrResult; // указатель на список адресов, полученных после вызова getaddrinfo
    SOCKET ConnectSocket = INVALID_SOCKET; // инициализация сокета для подключения
    char recvBuffer[512]; // буфер для приема данных

    const char* sendBuffer1 = "\nHello from client aaa"; // буфер для отправки данных 1
    const char* sendBuffer2 = "\nHello from client bbb"; // буфер для отправки данных 2

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // инициализация библиотеки Windows Sockets
    if (result != 0) { // проверка успешности инициализации
        cout << "WSAStartup failed result" << endl; // вывод сообщения об ошибке
        return 1; // выход из программы с ошибкой
    }

    ZeroMemory(&hints, sizeof(hints)); // обнуление структуры hints
    hints.ai_family = AF_INET; // установка семейства адресов (IPv4)
    hints.ai_socktype = SOCK_STREAM; // установка типа сокета (потоковый)
    hints.ai_protocol = IPPROTO_TCP; // установка протокола (TCP)

    getaddrinfo("localhost", "666", &hints, &addrResult); // получение информации об адресе сервера
    if (result != 0) { // проверка успешности получения информации
        cout << "getaddrinfo failed with error" << endl; // вывод сообщения об ошибке
        freeaddrinfo(addrResult); // освобождение памяти, выделенной для addrResult
        WSACleanup(); // очистка ресурсов Winsock
        return 1; // выход из программы с ошибкой
    }
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype,
        addrResult->ai_protocol); // создание сокета

    if (ConnectSocket == INVALID_SOCKET) { // проверка успешности создания сокета
        cout << "Socket creation with" << endl; // вывод сообщения об ошибке
        freeaddrinfo(addrResult); // освобождение памяти, выделенной для addrResult
        WSACleanup(); // очистка ресурсов Winsock
        return 1; // выход из программы с ошибкой
    }
    result = connect(ConnectSocket, addrResult->ai_addr,
        (int)addrResult->ai_addrlen); // установка соединения с сервером
    if (result == SOCKET_ERROR) { // проверка успешности установки соединения
        cout << "Unable connect to server" << endl; // вывод сообщения об ошибке
        closesocket(ConnectSocket); // закрытие сокета
        ConnectSocket = INVALID_SOCKET; // обнуление сокета
        freeaddrinfo(addrResult); // освобождение памяти, выделенной для addrResult
        WSACleanup(); // очистка ресурсов Winsock
        return 1; // выход из программы с ошибкой
    }
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0); // отправка данных

    if (result == SOCKET_ERROR) { // проверка успешности отправки данных
        cout << "Send failed, error" << result << endl; // вывод сообщения об ошибке
        freeaddrinfo(addrResult); // освобождение памяти, выделенной для addrResult
        WSACleanup(); // очистка ресурсов Winsock
        return 1; // выход из программы с ошибкой
    }

    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0); // отправка данных

    if (result == SOCKET_ERROR) { // проверка успешности отправки данных
        cout << "Send failed, error" << result << endl; // вывод сообщения об ошибке
        freeaddrinfo(addrResult); // освобождение памяти, выделенной для addrResult
        WSACleanup(); // очистка ресурсов Winsock
        return 1; // выход из программы с ошибкой
    }
    cout << "Send:" << result << "bytes:" << endl; // вывод информации о количестве отправленных байт

    result = shutdown(ConnectSocket, SD_SEND); // закрытие отправки данных

    if (result == SOCKET_ERROR) { // проверка успешности закрытия отправки данных
        cout << "shutdown failed, error" << result << endl; // вывод сообщения об ошибке
        freeaddrinfo(addrResult); // освобождение памяти, выделенной для addrResult
        WSACleanup(); // очистка ресурсов Winsock
        return 1; // выход из программы с ошибкой
    }

    do {
        ZeroMemory(recvBuffer, 512); // обнуление буфера приема данных
        result = recv(ConnectSocket, recvBuffer, 512, 0); // прием данных

        if (result > 0) { // проверка успешности приема данных
            cout << "Received: " << result << " bytes" << endl; // вывод информации о количестве принятых байт
            cout << "Received data: " << recvBuffer << endl; // вывод принятых данных

        }
        else if (result == 0) // проверка на закрытие соединения
            cout << "\nConnection closed" << endl; // вывод сообщения об закрытии соединения

    } while (result > 0); // повторение цикла при успешном приеме данных
    freeaddrinfo(addrResult); // освобождение памяти, выделенной для addrResult
    WSACleanup(); // очистка ресурсов Winsock
    return 0; // успешное завершение программы
}