#ifndef   LOG_WRITER_H
#define   LOG_WRITER_H

#include  <string>
#include  <functional>
#include  <vector>

#define   USED_STD_OUT     1

class LogWriter
{
public:
    using WriteLogCallback = std::function<void(int,const std::string&)>;

    enum Level{
        Debug = 0,
        Info,
        Warn,
        Error,
        Fatal,
        LevelSize
    };
    static LogWriter& instance();
    static void ToHex(std::string& message, const char* data, unsigned int size);
    static void ToHex(std::string& message, std::string& data);

public:
    void registerLogCallback(WriteLogCallback callback);

    void write(Level level, const std::string& data);
    void write(Level level, const std::string&& data);

    void fatal(const std::string& data);
    void fatal(const std::string&& data);
    void warn(const std::string& data);
    void warn(const std::string&& data);
    void error(const std::string& data);
    void error(const std::string&& data);
    void info(const std::string& data);
    void info(const std::string&& data);
    void debug(const std::string& data);
    void debug(const std::string&& data);

    void setLevel(int level);
    int getLevel();
    const std::string& getLevelName(int level);

private:
    LogWriter();

    WriteLogCallback callback_;
    int level_;
    std::vector<std::string> levelStr_;
    std::string nullLevel_;
};

#endif 

