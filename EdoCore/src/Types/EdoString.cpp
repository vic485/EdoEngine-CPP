// =============================================================================
// EdoString.cpp
// Implements certain parts of the cross-platform and regional string class
//
// Created by Victor on 2019/05/27.
// =============================================================================
/******************COPYRIGHT FROM ORIGINAL IMPLEMENTATION*********************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "EdoString.h"
#include <iostream>

namespace Edo {
    namespace Types {
        // Definition of 'no position' value
        const EdoString::size_type EdoString::npos = (EdoString::size_type) (-1);

        ///////////////////////////////////////////////
        // Destructor
        ///////////////////////////////////////////////
        EdoString::~EdoString() {
            if (d_reserve > STR_QUICKBUFF_SIZE)
                delete[] d_buffer;

            if (d_encodedBuffLen > 0)
                delete[] d_encodedBuff;
        }

        bool EdoString::Grow(EdoString::size_type new_size) {
            // Check if too big
            if (MaxSize() <= new_size)
                throw std::length_error("Resulting EdoString would be too large");

            // Increase, as we always null-terminate the buffer
            ++new_size;

            if (new_size > d_reserve) {
                utf32 *temp = new utf32[new_size]; // TODO: Is this okay from CEGUI_NEW_ARRAY_PT()?

                if (d_reserve > STR_QUICKBUFF_SIZE) {
                    memcpy(temp, d_buffer, (d_cpLength + 1) * sizeof(utf32));
                    delete[] d_buffer;
                } else {
                    memcpy(temp, d_quickBuff, (d_cpLength + 1) * sizeof(utf32));
                }

                d_buffer = temp;
                d_reserve = new_size;

                return true;
            }

            return false;
        }

        // Perform re-allocation to remove wasted space
        void EdoString::Trim() {
            size_type min_size = d_cpLength + 1;

            // Only re-allocate when not using quick-buffer, and when size can be trimmed
            if (d_reserve > STR_QUICKBUFF_SIZE && d_reserve > min_size) {
                // See if we can trim to quick-buffer
                if (min_size <= STR_QUICKBUFF_SIZE) {
                    memcpy(d_quickBuff, d_buffer, min_size * sizeof(utf32));
                    delete[] d_buffer;
                    d_reserve = STR_QUICKBUFF_SIZE;
                }
                    // Re-allocate buffer
                else {
                    utf32 *temp = new utf32[min_size];
                    memcpy(temp, d_buffer, min_size * sizeof(utf32));
                    delete[] d_buffer;
                    d_buffer = temp;
                    d_reserve = min_size;
                }
            }
        }

        // Build an internal buffer with the string encoded as utf8 (remains valid until string is modified).
        utf8 *EdoString::BuildUtf8Buff() const {
            size_type buffSize = EncodedSize(ptr(), d_cpLength) + 1;

            if (buffSize > d_encodedBuffLen) {
                if (d_encodedBuffLen > 0)
                    delete[] d_encodedBuff;

                d_encodedBuff = new utf8[buffSize];
                d_encodedBuffLen = buffSize;
            }

            Encode(ptr(), d_encodedBuff, buffSize, d_cpLength);

            // Always add a null at the end
            d_encodedBuff[buffSize - 1] = (utf8) 0;
            d_encodedDatLen = buffSize;

            return d_encodedBuff;
        }

        ///////////////////////////////////////////////
        // Comparison operators
        ///////////////////////////////////////////////
        bool operator==(const EdoString &str1, const EdoString &str2) {
            return str1.Compare(str2) == 0;
        }

        bool operator==(const EdoString &str, const std::string &std_str) {
            return str.Compare(std_str) == 0;
        }

        bool operator==(const std::string &std_str, const EdoString &str) {
            return str.Compare(std_str) == 0;
        }

        bool operator==(const EdoString &str, const utf8 *utf8_str) {
            return str.Compare(utf8_str) == 0;
        }

        bool operator==(const utf8 *utf8_str, const EdoString &str) {
            return str.Compare(utf8_str) == 0;
        }

        bool operator!=(const EdoString &str1, const EdoString &str2) {
            return str1.Compare(str2) != 0;
        }

        bool operator!=(const EdoString &str, const std::string &std_str) {
            return str.Compare(std_str) != 0;
        }

        bool operator!=(const std::string &std_str, const EdoString &str) {
            return str.Compare(std_str) != 0;
        }

        bool operator!=(const EdoString &str, const utf8 *utf8_str) {
            return str.Compare(utf8_str) != 0;
        }

        bool operator!=(const utf8 *utf8_str, const EdoString &str) {
            return str.Compare(utf8_str) != 0;
        }

        bool operator<(const EdoString &str1, const EdoString &str2) {
            return str1.Compare(str2) < 0;
        }

        bool operator<(const EdoString &str, const std::string &std_str) {
            return str.Compare(std_str) < 0;
        }

        bool operator<(const std::string &std_str, const EdoString &str) {
            return str.Compare(std_str) < 0;
        }

        bool operator<(const EdoString &str, const utf8 *utf8_str) {
            return str.Compare(utf8_str) < 0;
        }

        bool operator<(const utf8 *utf8_str, const EdoString &str) {
            return str.Compare(utf8_str) < 0;
        }

        bool operator>(const EdoString &str1, const EdoString &str2) {
            return str1.Compare(str2) > 0;
        }

        bool operator>(const EdoString &str, const std::string &std_str) {
            return str.Compare(std_str) > 0;
        }

        bool operator>(const std::string &std_str, const EdoString &str) {
            return str.Compare(std_str) > 0;
        }

        bool operator>(const EdoString &str, const utf8 *utf8_str) {
            return str.Compare(utf8_str) > 0;
        }

        bool operator>(const utf8 *utf8_str, const EdoString &str) {
            return str.Compare(utf8_str) > 0;
        }

        bool operator<=(const EdoString &str1, const EdoString &str2) {
            return str1.Compare(str2) <= 0;
        }

        bool operator<=(const EdoString &str, const std::string &std_str) {
            return str.Compare(std_str) <= 0;
        }

        bool operator<=(const std::string &std_str, const EdoString &str) {
            return str.Compare(std_str) <= 0;
        }

        bool operator<=(const EdoString &str, const utf8 *utf8_str) {
            return str.Compare(utf8_str) <= 0;
        }

        bool operator<=(const utf8 *utf8_str, const EdoString &str) {
            return str.Compare(utf8_str) <= 0;
        }

        bool operator>=(const EdoString &str1, const EdoString &str2) {
            return str1.Compare(str2) >= 0;
        }

        bool operator>=(const EdoString &str, const std::string &std_str) {
            return str.Compare(std_str) >= 0;
        }

        bool operator>=(const std::string &std_str, const EdoString &str) {
            return str.Compare(std_str) >= 0;
        }

        bool operator>=(const EdoString &str, const utf8 *utf8_str) {
            return str.Compare(utf8_str) >= 0;
        }

        bool operator>=(const utf8 *utf8_str, const EdoString &str) {
            return str.Compare(utf8_str) >= 0;
        }

        ///////////////////////////////////////////////
        // C-string operators
        ///////////////////////////////////////////////
        bool operator==(const EdoString &str, const char *c_str) {
            return str.Compare(c_str) == 0;
        }

        bool operator==(const char *c_str, const EdoString &str) {
            return str.Compare(c_str) == 0;
        }

        bool operator!=(const EdoString &str, const char *c_str) {
            return str.Compare(c_str) != 0;
        }

        bool operator!=(const char *c_str, const EdoString &str) {
            return str.Compare(c_str) != 0;
        }

        bool operator<(const EdoString &str, const char *c_str) {
            return str.Compare(c_str) < 0;
        }

        bool operator<(const char *c_str, const EdoString &str) {
            return str.Compare(c_str) < 0;
        }

        bool operator>(const EdoString &str, const char *c_str) {
            return str.Compare(c_str) > 0;
        }

        bool operator>(const char *c_str, const EdoString &str) {
            return str.Compare(c_str) > 0;
        }

        bool operator<=(const EdoString &str, const char *c_str) {
            return str.Compare(c_str) <= 0;
        }

        bool operator<=(const char *c_str, const EdoString &str) {
            return str.Compare(c_str) <= 0;
        }

        bool operator>=(const EdoString &str, const char *c_str) {
            return str.Compare(c_str) >= 0;
        }

        bool operator>=(const char *c_str, const EdoString &str) {
            return str.Compare(c_str) >= 0;
        }

        ///////////////////////////////////////////////
        // Concatenation operator functions
        ///////////////////////////////////////////////
        EdoString operator+(const EdoString &str1, const EdoString &str2) {
            EdoString temp(str1);
            temp.Append(str2);
            return temp;
        }

        EdoString operator+(const EdoString &str, const std::string &std_str) {
            EdoString temp(str);
            temp.Append(std_str);
            return temp;
        }

        EdoString operator+(const std::string &std_str, const EdoString &str) {
            EdoString temp(std_str);
            temp.Append(str);
            return temp;
        }

        EdoString operator+(const EdoString &str, const utf8 *utf8_str) {
            EdoString temp(str);
            temp.Append(utf8_str);
            return temp;
        }

        EdoString operator+(const utf8 *utf8_str, const EdoString &str) {
            EdoString temp(utf8_str);
            temp.Append(str);
            return temp;
        }

        EdoString operator+(const EdoString &str, utf32 code_point) {
            EdoString temp(str);
            temp.Append(1, code_point);
            return temp;
        }

        EdoString operator+(utf32 code_point, const EdoString &str) {
            EdoString temp(1, code_point);
            temp.Append(str);
            return temp;
        }

        EdoString operator+(const EdoString &str, const char *c_str) {
            EdoString temp(str);
            temp.Append(EdoString(c_str)); // TODO: Ambiguous call when we only pass c_str?
            return temp;
        }

        EdoString operator+(const char *c_str, const EdoString &str) {
            EdoString temp(c_str);
            temp.Append(str);
            return temp;
        }

        ///////////////////////////////////////////////
        // Output (stream) functions
        ///////////////////////////////////////////////
        std::ostream &operator<<(std::ostream &s, const EdoString &str) {
            return s << str.c_str();
        }

        ///////////////////////////////////////////////
        // Modifying operations
        ///////////////////////////////////////////////
        void Swap(EdoString &str1, EdoString &str2) {
            str1.Swap(str2);
        }
    } // Namespace Types
} // Namespace Edo
