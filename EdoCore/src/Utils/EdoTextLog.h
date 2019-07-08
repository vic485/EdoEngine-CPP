// =============================================================================
// EdoTextLog.h
// Defines a system for logging messages to text files
//
// Created by Victor on 2019/07/08.
// =============================================================================

#ifndef EDOCORE_EDOTEXTLOG_H
#define EDOCORE_EDOTEXTLOG_H

#include "../Types/EdoString.h"
#include <iostream>

namespace Edo {
    namespace Utils {
        using namespace Edo::Types;

        /*!
         * \brief
         * A simple logger class which saves all messages it receives to a text file
         * The entries can contain timestamps as well as file and line information.
         */
        class EdoTextLog {
        public:
            EdoTextLog();

            /*!
             * \brief
             * Constructor
             * \param strFile
             * The device path and filename where the log file is stored, e.g. "C:\mygame\log.txt"
             * \param append
             * If true, the file will not be overwritten when logging starts, but instead it will be appended to. If false
             * the file will be truncated first
             * \param timestamp
             * If true, insert timestamps into the log file. If false, omit them
             * \param truncate
             * Maximum length of single log entry
             */
            EdoTextLog(const EdoString &strFile, const bool append = false, const bool timestamp = true,
                       const long truncate = 16384);

            virtual ~EdoTextLog();

            /*!
             * \brief
             * Initializes the log file
             * \param strFile
             * The device path and filename where the log file is stored, e.g. "C:\mygame\log.txt"
             * \param append
             * If true, the file will not be overwritten when logging starts, but instead it will be appended to. If false
             * the file will be truncated first
             * \param timestamp
             * If true, insert timestamps into the log file. If false, omit them
             * \param truncate
             * Maximum length of single log entry
             */
            void Init(const EdoString &strFile, const bool append = false, const bool timestamp = true,
                      const long truncate = 16384);

            /*!
             * \brief
             * Enables/Disables timestamps
             * \param enable
             * True to enable or false to disable timestamps
             */
            void SetTimestamp(const bool enable);

            /*!
             * \brief
             * Returns whether timestamps in enabled
             * \return
             * Whether timestamps are enabled
             */
            bool GetTimestamp() const;

            /*!
             * \brief
             * Sets the append/truncate mode
             * \param enable
             * True to enable appending or false to enable truncating
             */
            void SetAppend(const bool enable);

            /*!
             * \brief
             * Returns whether the log was opened in append or truncate mode
             * \return
             * True for append mode, false for truncate mode
             */
            bool GetAppend() const;

            /*!
             * \brief
             * Sets the log file's device filename
             * \param strFile
             * The device path and filename where the log file is stored, e.g. "C:\mygame\log.txt
             */
            void SetFilename(const EdoString &strFile);

            /*!
             * \brief
             * Returns the device filename of the logfile
             * \return
             * Device filename of the log file
             */
            EdoString GetFilename() const;

            /*!
             * \brief
             * Sets the truncation length
             * \param len
             * The maximum length of a single log message
             */
            void SetTruncLength(const long len);

            /*!
             * \brief
             * Returns the truncation length
             * \return
             * The maximum length of a single log message
             */
            const long GetTruncLength() const;

            /*!
             * \brief
             * Writes an entry to the log
             * \param logString
             * The message to log
             * \param type
             * Message type e.g. "Warn", "Information", ...
             * \param file
             * The file the log call is coming from
             * \param line
             * The line number the log call is coming from
             */
            void Write(const EdoString &logString, const EdoString &type, const EdoString &file, int line);

        private:
            /*!
             * \brief
             * Returns the formatted date and time string
             * \return
             * Current date and time string in format: YYYY/MM/dd - HH:mm (24 hour)
             */
            EdoString GetDateTimeString();

            EdoString m_logFile; //!< Device filename of the logfile
            bool m_append; //!< True if append mode, else truncate
            bool m_timestamp; //!< Timestamps enabled?
            unsigned long m_truncate; //!< Length of the circular log
        };
    }
}

#endif //EDOCORE_EDOTEXTLOG_H
