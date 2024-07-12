
# Компонента для работы 1С с KAFKA 

[Инструкция по установке](https://confluence.bi.group/pages/viewpage.action?pageId=100479064)


# Compatibles
Ubuntu 22.04
Sentos 8 /9
Oracle Linux 9

# Полезные ссылки

Документация confluent
https://developer.confluent.io/get-started/c/#build-producer
https://docs.confluent.io/kafka-clients/librdkafka/current/overview.html#cplus-installation

https://confluence.bi.group/pages/viewpage.action?pageId=100473262 - планирование

https://infostart.ru/1c/articles/323372/ - Компилирование внешней компоненты AddInNative в ОС Linux
https://its.1c.ru/db/metod8dev/content/3221/hdoc - документация

https://releases.1c.ru/version_files?nick=Platform83&ver=8.3.14.1694 - Ссылка на дистрибутивы

https://interface31.ru/tech_it/2016/01/vklyuchaem-tehnologicheskiy-zhurnal-dlya-1spredpriyatie.html - Включаем технологический журнал для 1С:Предприятие

* INSTALL 

sudo apt-get update -y
sudo apt install build-essential
sudo apt-get install -y gcc-multilib
sudo apt-get install gcc-multilib g++-multilib
sudo apt install librdkafka-dev

* Слетела лицуха
sudo find / -type f -name "1cv8conn.pfl" -delete

* RUN 
g++ producer.cpp -o producer -lrdkafka++ -lrdkafka -lz -lpthread -lrt

* 1С  
sudo dpkg -i *.deb
apt-get -y install ttf-mscorefonts-installer
fc-cache –fv
apt-get install libfreetype6 libgsf-1-common unixodbc glib2.0
systemctl start srv1cv83

* debug mode
/etc/init.d/srv1cv83


* HASP  ???
https://ftp.etersoft.ru/pub/Etersoft/HASP/last/Ubuntu/22.04/


* Postgress
wget https://repo.postgrespro.ru/1c/1c-16/keys/pgpro-repo-add.sh
sh pgpro-repo-add.sh
apt-get install postgrespro-1c-16
/opt/pgpro/1c-16/bin/pg-setup initdb
/opt/pgpro/1c-16/bin/pg-setup service enable
/opt/pgpro/1c-16/bin/pg-setup service start
/opt/pgpro/1c-16/bin/pg-setup service status

sudo -u postgres psql
ALTER USER postgres WITH PASSWORD 'postgres';

* Полезные команды
sudo dpkg -l | grep 1c-enterprise - список запущенных 1С

* Удаление
sudo dpkg --purge 1c-enterprise83-ws-nls
sudo dpkg --purge 1c-enterprise83-ws 
sudo dpkg --purge 1c-enterprise83-server-nls
sudo dpkg --purge 1c-enterprise83-server
sudo dpkg --purge 1c-enterprise83-common-nls
sudo dpkg --purge 1c-enterprise83-common 

* Права на запуск билдера
chmod +x build.sh
chmod +x producer.exe
chmod +x producer.cpp
