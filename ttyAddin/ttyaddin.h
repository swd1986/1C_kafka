#ifndef TTYADDIN_H
#define TTYADDIN_H

#include "ttyAddin_global.h"
#include "Component.h"

// C library headers
#include <iostream>
#include <string>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

class TTYADDIN_EXPORT TtyAddin : public Component
{
public:
    const char *Version = u8"1.0.0";    //присутствует в шаблоне версия нашего класса
    explicit TtyAddin();
    variant_t ConnectPort (const variant_t &, const variant_t &);// объявление метода инициализации порта и подключение к нему
    void DisconnectPort(void);  //объявление метода отключения от порта
    void ReadPort ();           //объявление метода чтения буфера порта
    void SendToPort (const variant_t &);    //объявление метода отправки строки в порт
    std::string extensionName() override;   //наименование класса
private:
    //переопределяем проунаследованные методы
    bool ExternalEvent(const std::string &, const std::string &, const std::string &);
    bool SetEventBufferDepth(long);
    long GetEventBufferDepth();

    int serial_port;
    struct termios tty;  //структура данных настроек порта
    char read_buf [256]; //буфер временного хранения принятых данных
    //std::string str;
    std::string PortNameStr;//хранение пути к порту
    std::shared_ptr<variant_t> sample_property; //наименование нашего класса которое будет передано в 1с
};

#endif // TTYADDIN_H
