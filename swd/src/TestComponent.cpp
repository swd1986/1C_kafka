#include "TestComponent.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <string>
#include <librdkafka/rdkafka.h>
static volatile sig_atomic_t run = 1;

using std::string;

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
	AddFunction(u"SendProducer", u"SendProducer", [&](VH par1, VH par2) { this->result = this->SendProducer(par1, par2); });

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
	return text + MB2WCHAR("MDM -> Linux Kafka Native версия: " + this->version());
}

std::u16string TestComponent::SendProducer(const std::u16string &p_text1, const std::u16string &p_text2)
{
	return p_text1 + p_text2;
}

void TestComponent::setTestString(const std::u16string &text)
{
	this->text = text;
}
