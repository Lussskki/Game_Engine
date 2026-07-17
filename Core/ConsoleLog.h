#pragma once

#include <string>
#include <vector>

namespace Engine
{

class ConsoleLog
{
public:
    static void Info(const std::string& message);
    static void Warning(const std::string& message);
    static void Error(const std::string& message);
    static void Clear();
    static const std::vector<std::string>& GetMessages();
};

}
