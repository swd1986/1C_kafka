*Полезные ссылки*

https://github.com/mfontanini/cppkafka
https://github.com/perkss/c-kafka-examples/tree/main - здесь есть что то интересное?
https://github.com/morganstanley/modern-cpp-kafka
https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.cpp#L395
https://developer.confluent.io/get-started/c/#build-producer

https://confluence.bi.group/pages/viewpage.action?pageId=100473262 - планирование

https://infostart.ru/1c/articles/193577/
https://infostart.ru/1c/tools/1658262/

https://infostart.ru/1c/articles/323372/ - Компилирование внешней компоненты AddInNative в ОС Linux

https://its.1c.ru/db/metod8dev/content/3221/hdoc

https://releases.1c.ru/version_files?nick=Platform83&ver=8.3.14.1694 - Ссылка на дистрибутивы

https://interface31.ru/tech_it/2016/01/vklyuchaem-tehnologicheskiy-zhurnal-dlya-1spredpriyatie.html - Включаем технологический журнал для 1С:Предприятие

https://github.com/lintest/AddinTemplate.git - Шаблон, который я начал проект
https://infostart.ru/1c/articles/2044928/ - Внешняя компонента 1С и С++. Продолжаем разговор.

https://docs.confluent.io/kafka-clients/librdkafka/current/overview.html#cplus-installation

DEB(у меня) RPM (цель)


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


* Открытие портов
sudo ufw allow 1540/tcp
sudo ufw allow 1541/tcp
sudo ufw allow 5432/tcp


* Полезные команды
sudo dpkg -l | grep 1c-enterprise - список запущенных 1С


* Удаление
sudo dpkg --purge 1c-enterprise83-ws-nls
sudo dpkg --purge 1c-enterprise83-ws 
sudo dpkg --purge 1c-enterprise83-server-nls
sudo dpkg --purge 1c-enterprise83-server
sudo dpkg --purge 1c-enterprise83-common-nls
sudo dpkg --purge 1c-enterprise83-common 

* Общие папки
sudo adduser shustov vboxsf

* GIT
git config --global user.name "swd1986"
git config --global user.email swd1986@yandex.ru
git remote set-url origin https://<TOKEN>@github.com/swd1986/1C_kafka.git

* SH
chmod +x build.sh
chmod +x producer.exe
chmod +x producer.cpp


* MS Code
sudo code --user-data-dir="~/.vscode-root"

* LINUX Oracle
sudo yum update
sudo yum install gcc gcc-c++ make automake autoconf kernel-devel
sudo yum install gcc-multilib


cd librdkafka && ./configure && make && make install && ldconfig
