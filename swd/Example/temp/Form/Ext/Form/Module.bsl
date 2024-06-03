﻿&НаСервере
Процедура ПриСозданииНаСервере(Отказ, СтандартнаяОбработка)
	

	ОбработкаОбъект = РеквизитФормыВЗначение("Объект");
	МакетКомпоненты = ОбработкаОбъект.ПолучитьМакет("NativeAddIn");
	МетоположениеКомпоненты = ПоместитьВоВременноеХранилище(МакетКомпоненты, УникальныйИдентификатор);
	
	ОбработкаОбъект = РеквизитФормыВЗначение("Объект");
	ФайлОбработки = Новый Файл(ОбработкаОбъект.ИспользуемоеИмяФайла);
	ТекущийКаталог = ФайлОбработки.Путь;
	
	БольшоеЧисло = 3147483647;
	
КонецПроцедуры

&НаКлиенте
Процедура ТестКомпоненты(Команда)

	УстановитьВнешнююКомпоненту(МетоположениеКомпоненты);
	ПодключитьВнешнююКомпоненту(МетоположениеКомпоненты, "TestAddin", ТипВнешнейКомпоненты.Native);
	ВнешняяКомпонента = Новый("AddIn.TestAddin.SimpleAlias");
	
	Сообщить("Версия: " + ВнешняяКомпонента.Версия);
	
	ВнешняяКомпонента.TEXT = "Свойство: ";
	Сообщить(ВнешняяКомпонента.ТексТ);
	
	ВнешняяКомпонента.УстановитьТЕКСТ("Функция: ");
	Сообщить(ВнешняяКомпонента.ПолучитьТЕКСТ());
	
	ВнешняяКомпонента.УстановитьТЕКСТ();
	Сообщить(ВнешняяКомпонента.ПолучитьТЕКСТ());
	
	ВнешняяКомпонента.Число = БольшоеЧисло;
	ТипЗначения = ВнешняяКомпонента.Число;
	
КонецПроцедуры

&НаКлиенте
Процедура Автотест(Команда)
	
	Массив = СтрРазделить(ИмяФормы, ".");
	Массив[Массив.Количество() - 1] = "Test";
	НовоеИмя = СтрСоединить(Массив, ".");
	ОткрытьФорму(НовоеИмя);

КонецПроцедуры
