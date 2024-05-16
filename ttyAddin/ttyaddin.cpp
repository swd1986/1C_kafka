#include "ttyaddin.h"

std::string TtyAddin::extensionName() { // наименование нашего класса которое будет передано в 1с
    return "TTY";
}
TtyAddin::TtyAddin()
{
    // Universal property. Could store any supported by native api type.    //присутствует в шаблоне
    sample_property = std::make_shared<variant_t>();
    AddProperty(L"SampleProperty", L"ОбразецСвойства", sample_property);

    // Full featured property registration example      //присутствует в шаблоне
    AddProperty(L"Version", L"ВерсияКомпоненты", [&]() {
        auto s = std::string(Version);
        return std::make_shared<variant_t>(std::move(s));
    });
    //Регистрация методов
    //Указываются названия методов которые мы будем использовать в 1С и связанные с ними методы класса
    AddMethod(L"Сonnect", L"ПодключитьПорт", this, &TtyAddin::ConnectPort);
    AddMethod(L"Disconnect", L"ОтключитьПорт", this, &TtyAddin::DisconnectPort);
    AddMethod(L"Read", L"ЧитатьПорт", this, &TtyAddin::ReadPort);
    AddMethod(L"Send", L"ОтправитьВПорт", this, &TtyAddin::SendToPort);
}
// определение метода инициализации порта и подключение к нему
// принимает строку пути к файлу порта и скорость порта (число)
// в случае успеха возвращает истину
variant_t TtyAddin::ConnectPort (const variant_t & serialPortName, const variant_t & Baud)
{   variant_t res = false;
    //проверка операндов на соответствие типов
    if (std::holds_alternative<std::string>(serialPortName) && (std::holds_alternative<int32_t>(Baud)))
    {  res = true;
       PortNameStr = std::get<std::string>(serialPortName); //помещаем путь к порту в строку пример "/dev/ttyACM0"
       const char *PortName=PortNameStr.c_str();    // преобразуем в строку в стиле Си (массив char) и сохраняем указатель на нее
       serial_port = open(PortName, O_RDWR); //открываем указанный порт для чтения и записи  (только для чтения O_RDONLY)
       if (serial_port < 0) {
           throw std::runtime_error(u8"указанный порт отсутствует в системе");  //если открыть порт не удалось
       }
       switch (static_cast<int>(std::get<int32_t>(Baud)))   // в соответствии с указанной скоростью устанавливаем скорость порта
        {
            case 1200:cfsetspeed(&tty, B1200);
                      break;
            case 2400:cfsetspeed(&tty, B2400);
                      break;
            case 4800:cfsetspeed(&tty, B4800);
                      break;
            case 9600:cfsetspeed(&tty, B9600);
                      break;
            case 19200:cfsetspeed(&tty, B19200);
                      break;
            default: res = false;
                      throw std::runtime_error(u8"значение скорости недопустимо");
                      break;
        }
       // вводим основные настройки
       tty.c_cflag &= ~PARENB; // без паритета
       tty.c_cflag &= ~CSTOPB; // 1 стоп бит
       tty.c_cflag |= CS8;     // 8 бит
       tty.c_cflag &= ~CRTSCTS; // без RTS/CTS аппаратного управления потоком
       //сохраняем настройки
       if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
       {  throw std::runtime_error(u8"невозможно сохранить настройки порта");
       }
       memset(&read_buf, '\0', sizeof(read_buf));//инициализируем буфер
       TtyAddin::SetEventBufferDepth(10);  //устанавливаем размер очереди событий в 1с  функция описана в 1С:ИТС
    }
    else{  res = false;
           throw std::runtime_error(u8"метод serialSetting - неподдерживаемые типы данных");} //если имя порта не строка, а скорость не число
    return res;
}
//определение метода отключения от порта
void TtyAddin::DisconnectPort(void) //Отключаем порт
{   close(serial_port); }
//определение метода чтения буфера порта
void TtyAddin::ReadPort ()
{   tcflush(serial_port,TCIOFLUSH); // чистим буфер порта от мусора перед чтением
    sleep(1);                       // ждём (секунды)
    int num_bytes = read(serial_port, &read_buf[0], sizeof(read_buf)); //читаем буфер порта
    if (num_bytes <= 0) //если -1 ошибка 0 буфер пуст
    {
        throw std::runtime_error(u8"данные в порт не поступают");
    }
    else {   ExternalEvent(extensionName(), PortNameStr, static_cast<std::string>(read_buf));} // вывод в 1с через внешнее событие
}
//определение метода отправки строки в порт
void TtyAddin::SendToPort (const variant_t & data)
{   if (std::holds_alternative<std::string>(data))  //проверяем соответствие типа введённых данных
    {   std::string dataString = std::get<std::string>(data); //преобразуем в строку
        const char * msg = dataString.c_str();      //и  переводим ее в строку в стиле си
          write(serial_port, msg, sizeof(msg));     // отправляем
    }
    else {   throw std::runtime_error(u8"метод serialSetting - неподдерживаемые типы данных");}
}
//переопределяем проунаследованные методы
bool TtyAddin::ExternalEvent(const std::string &src, const std::string &msg, const std::string &data)
{   return Component::ExternalEvent( src, msg, data);}
bool TtyAddin::SetEventBufferDepth(long depth)
{   return Component::SetEventBufferDepth(depth);}
long TtyAddin::GetEventBufferDepth()
{   return Component::GetEventBufferDepth();}
