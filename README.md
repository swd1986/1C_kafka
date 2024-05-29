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


* Полезные команды
sudo dpkg -l | grep 1c-enterprise - список запущенных 1С


* Удаление
sudo dpkg --purge 1c-enterprise83-ws-nls
sudo dpkg --purge 1c-enterprise83-ws 
sudo dpkg --purge 1c-enterprise83-server-nls
sudo dpkg --purge 1c-enterprise83-server
sudo dpkg --purge 1c-enterprise83-common-nls
sudo dpkg --purge 1c-enterprise83-common 


