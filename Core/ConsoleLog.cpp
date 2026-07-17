#include "Core/ConsoleLog.h"

namespace Engine
{

namespace
{
std::vector<std::string> Messages;
}

void ConsoleLog::Info(const std::string& message)
{
    Messages.push_back("[Info] " + message);
}

void ConsoleLog::Warning(const std::string& message)
{
    Messages.push_back("[Warning] " + message);
}

void ConsoleLog::Error(const std::string& message)
{
    Messages.push_back("[Error] " + message);
}

void ConsoleLog::Clear()
{
    Messages.clear();
}

const std::vector<std::string>& ConsoleLog::GetMessages()
{
    return Messages;
}

}
