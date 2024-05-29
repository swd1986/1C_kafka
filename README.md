*Полезные ссылки*

https://infostart.ru/1c/articles/193577/
https://infostart.ru/1c/tools/1658262/

https://infostart.ru/1c/articles/323372/ - Компилирование внешней компоненты AddInNative в ОС Linux

https://its.1c.ru/db/metod8dev/content/3221/hdoc

https://releases.1c.ru/version_files?nick=Platform83&ver=8.3.14.1694 - Ссылка на дистрибутивы

https://interface31.ru/tech_it/2016/01/vklyuchaem-tehnologicheskiy-zhurnal-dlya-1spredpriyatie.html - Включаем технологический журнал для 1С:Предприятие



* INSTALL 

sudo apt-get update -y
sudo apt install build-essential
sudo apt-get install -y gcc-multilib
sudo apt-get install gcc-multilib g++-multilib

* 1С  
sudo dpkg -i *.deb
apt-get -y install ttf-mscorefonts-installer
fc-cache –fv
apt-get install libfreetype6 libgsf-1-common unixodbc glib2.0
systemctl start srv1cv83

* Postgress
wget https://repo.postgrespro.ru/1c/1c-16/keys/pgpro-repo-add.sh
sh pgpro-repo-add.sh
apt-get install postgrespro-1c-16
/opt/pgpro/1c-16/bin/pg-setup initdb
/opt/pgpro/1c-16/bin/pg-setup service enable
/opt/pgpro/1c-16/bin/pg-setup service start
/opt/pgpro/1c-16/bin/pg-setup service status

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


