#include "logWriter.h"
#include <iostream>

LogWriter& LogWriter::instance() {
    static LogWriter writer;
    return writer;
}

void LogWriter::registerLogCallback(WriteLogCallback callback) {
    callback_ = callback;
}

void LogWriter::ToHex(std::string& message, const char* data, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        char buf[8];
        std::sprintf(buf, " 0x%x ", (unsigned char)data[i]);
        message.append(buf);
    }
}

void LogWriter::ToHex(std::string& message, std::string& data) {
    ToHex(message, data.c_str(), (unsigned)(data.size()));
}

void LogWriter::write(Level level, const std::string& data) {
    if ((level <= Error) && (level >= level_) && (level >= Debug)) {
        if (callback_) {
            callback_(level, data);
        } else {
            std::cout << getLevelName(level) << " :" << data << std::endl;
        }
    }
}

void LogWriter::write(Level level, const std::string&& data) {
    write(level, data);
}

void LogWriter::fatal(const std::string& data) {
    write(Level::Fatal, data);
}

void LogWriter::fatal(const std::string&& data) {
    write(Level::Fatal, data);
}

void LogWriter::warn(const std::string& data) {
    write(Level::Warn, data);
}

void LogWriter::warn(const std::string&& data) {
    write(Level::Warn, data);
}

void LogWriter::error(const std::string& data) {
    write(Level::Error, data);
}

void LogWriter::error(const std::string&& data) {
    write(Level::Error, data);
}

void LogWriter::info(const std::string& data) {
    write(Level::Info, data);
}

void LogWriter::info(const std::string&& data) {
    write(Level::Info, data);
}

void LogWriter::debug(const std::string& data) {
    write(Level::Debug, data);
}

void LogWriter::debug(const std::string&& data) {
    write(Level::Debug, data);
}

void LogWriter::setLevel(int level) {
    level_ = level;
}

int LogWriter::getLevel() {
    return level_;
}

const std::string& LogWriter::getLevelName(int level) {
    if (level >= 0 && level <=static_cast<int>(levelStr_.size())) {
        return levelStr_[level];
    }
    return nullLevel_;
}

LogWriter::LogWriter()
    :callback_(nullptr),
    level_(0)
{
    levelStr_.resize(Level::LevelSize);
    levelStr_[Level::Debug] = "Debug";
    levelStr_[Level::Info] = "Info";
    levelStr_[Level::Warn] = "Warn";
    levelStr_[Level::Error] = "Error";
    levelStr_[Level::Fatal] = "Fatal";

    nullLevel_ = "NullLevel";
}