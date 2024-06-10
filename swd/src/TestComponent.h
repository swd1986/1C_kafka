#ifndef __TESTCOMPONENT_H__
#define __TESTCOMPONENT_H__

#include "AddInNative.h"

class TestComponent : public AddInNative
{
private:
    static std::vector<std::u16string> names;
    TestComponent();
    int64_t value;

private:
    std::u16string text;
    std::u16string getTestString();
    std::u16string getVersion();
    std::u16string SendProducer(
        const std::u16string &p_brokers,
        const std::u16string &p_topic,
        const std::u16string &p_username,
        const std::u16string &p_password,
        const std::u16string &p_key,
        const std::u16string &p_message);
    void setTestString(const std::u16string &text);
};
#endif //__TESTCOMPONENT_H__