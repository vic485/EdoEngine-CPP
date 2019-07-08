// =============================================================================
// EdoTextLog.cpp
// Implements methods of the text logging system
//
// Created by Victor on 2019/07/08.
// =============================================================================

#include "EdoTextLog.h"
#include <fstream>

// TODO: move this
#define TEXT(x) __TEXT(x)
#define __TEXT(x) L##x

using namespace Edo::Types;
using namespace Edo::Utils;

EdoTextLog::EdoTextLog() {
    Init(TEXT("log.txt"), false, true, 16384);
}

EdoTextLog::EdoTextLog(const Edo::Types::EdoString &strFile, const bool append, const bool timestamp,
                       const long truncate) {
    Init(strFile, append, timestamp, truncate);
}

EdoTextLog::~EdoTextLog() {
    // Write .txt file footer
    std::fstream ss(m_logFile.c_str(), std::ios::out | std::ios::app);
    if (ss.is_open()) {
        ss << TEXT("===============   Logging stopped on ") << GetDateTimeString() << TEXT("   ===============")
           << std::endl;
    }
    ss.close();
}

void EdoTextLog::Init(const Edo::Types::EdoString &strFile, const bool append, const bool timestamp,
                      const long truncate) {
    m_truncate = truncate;
    m_logFile = strFile;
    m_append = append;
    m_timestamp = timestamp;

    // Write .txt file header
    std::fstream ss(m_logFile.c_str(), std::ios::out | (append ? std::ios::app : std::ios::trunc));
    if (ss.is_open()) {
        ss << EdoByteOrderMark::Utf8();
        ss << TEXT("===============   Logging started on ") << GetDateTimeString() << TEXT("   ===============")
           << std::endl;
    }
    ss.close();
}

void EdoTextLog::SetTimestamp(const bool enable) {
    m_timestamp = enable;
}

bool EdoTextLog::GetTimestamp() const {
    return m_timestamp;
}

void EdoTextLog::SetAppend(const bool enable) {
    m_append = enable;
}

bool EdoTextLog::GetAppend() const {
    return m_append;
}

void EdoTextLog::SetFilename(const Edo::Types::EdoString &strFile) {
    Init(strFile, m_append, m_timestamp, m_truncate);
}

EdoString EdoTextLog::GetFilename() const {
    return m_logFile;
}

void EdoTextLog::SetTruncLength(const long len) {
    m_truncate = len;
}

const long EdoTextLog::GetTruncLength() const {
    return m_truncate;
}

void EdoTextLog::Write(const Edo::Types::EdoString &logString, const Edo::Types::EdoString &type,
                       const Edo::Types::EdoString &file, int line) {
    // Write the formatted log string to log
    EdoString logMessage(logString);
    if (logMessage.Size() > m_truncate) {
        logMessage = logMessage.Substr(0, m_truncate) + TEXT(" ... (the logger data omitted the rest of the data here) ...");
    }

    std::fstream ss(m_logFile.c_str(), std::ios::out | std::ios::app);
    EdoString msg;

    if (m_timestamp) {
        msg += GetDateTimeString() + TEXT(" ");
    }

    msg += TEXT("[") + type + TEXT("] ") + logMessage + TEXT(" >> ") + file + TEXT(":") + line;

    if (ss.is_open()) {
        ss << msg << std::endl; // Write to .txt file
        std::cout << msg << std::endl; // Write to stdout
    }
    ss.close();
}

EdoString EdoTextLog::GetDateTimeString() {
    // TODO: format current date and time year/month/day - hour:minute
    return EdoString("");
}
