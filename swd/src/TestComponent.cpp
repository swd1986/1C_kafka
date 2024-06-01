#include "TestComponent.h"

std::vector<std::u16string> TestComponent::names = {
	AddComponent(u"AddInNative", []() { return new TestComponent; }),
	AddComponent(u"SimpleAlias", []() { return new TestComponent; }),
};

TestComponent::TestComponent()
{
	AddProperty(
		u"Version", u"Версия",
		[&](VH var) { var = this->version(); });

	AddProperty(
		u"Text", u"Текст",
		[&](VH var) { var = this->getTestString(); },
		[&](VH var) { this->setTestString(var); });

	AddProperty(
		u"Number", u"Число",
		[&](VH var) { var = this->value; },
		[&](VH var) { this->value = var; });

	AddFunction(u"GetText", u"ПолучитьТекст", [&]() { this->result = this->getTestString(); });
	AddFunction(u"GetVersion", u"ПолучитьВерсию", [&]() { this->result = this->getVersion(); });
	AddFunction(u"test_send", u"test_send", [&](VH par) { this->result = this->test_send(par); });

	AddProcedure(u"SetText", u"УстановитьТекст", [&](VH par) { this->setTestString(par); }, {{0, u"default: "}});
}

#include <iostream>
#include <ctime>

std::u16string TestComponent::getTestString()
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[255];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	return text + MB2WCHAR(buffer);
}

std::u16string TestComponent::getVersion()
{
	return text + MB2WCHAR("MDM -> Linux Kafka Native версия 0.1");
}

std::u16string TestComponent::test_send(const std::u16string &p_text)
{
	return p_text;
}

void TestComponent::setTestString(const std::u16string &text)
{
	this->text = text;
}
