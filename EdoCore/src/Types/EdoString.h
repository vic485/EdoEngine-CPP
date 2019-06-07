// =============================================================================
// EdoString.h
// Defines a cross-platform and regional string class
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
#ifndef EDOCORE_EDOSTRING_H
#define EDOCORE_EDOSTRING_H

#include "EdoBase.h"
#include <cstring>
#include <stdexcept>
#include <cstddef>
#include <algorithm>

namespace Edo {
    namespace Types {
        /***************************************
         * Basic Types
         ***************************************/
        typedef unsigned char utf8;
        typedef unsigned short utf16; // Not implemented in CEGUI, Custom UTF16 implementation later
        typedef unsigned int utf32;

#define STR_QUICKBUFF_SIZE 32

        /*!
          \brief
            Custom string class with Unicode support. This is for the most part,
            a drop-in replacement for std::string
         */
        class EDO_API EdoString {
        public:
            /***************************************
             * Integral types
             ***************************************/
            typedef utf32 value_type; //!< Basic 'code point' type used for EdoString (utf32)
            typedef size_t size_type; //!< Unsigned type used for size values and indices
            typedef std::ptrdiff_t difference_type; //!< Signed type used for differences
            typedef utf32 &reference; //!< Type used for utf32 code point references
            typedef const utf32 &const_reference; //!< Type used for constant utf32 code point references
            typedef utf32 *pointer; //!< Type used for utf32 code point pointers
            typedef const utf32 *const_pointer; //!< Type used for constant utf32 code point pointers

            static const size_type npos; //!< Value used to represent 'not found' conditions and 'all code points' etc.

        private:
            /***************************************
             * Implementation data
             ***************************************/
            size_type d_cpLength; //!< Holds length of string in code point (not including null termination)
            size_type d_reserve; //!< Code point reserve size (currently allocated buffer size in code points)

            mutable utf8 *d_encodedBuff; //!< Holds string data encoded as utf8 (generated only by calls to c_str() and data())
            mutable size_type d_encodedDatLen; //!< Holds length of encoded data (in case it's smaller than buffer)
            mutable size_type d_encodedBuffLen; //!< Length of above buffer (since buffer can be bigger than the data it holds to save re-allocation)

            utf32 d_quickBuff[STR_QUICKBUFF_SIZE]; //!< This is a integrated 'quick' buffer to save allocations for smallish strings
            utf32 *d_buffer; //!< Pointer to the main buffer memory. This is only valid when quick-buffer is not being used

        public:
            /***************************************
             * Iterator classes
             ***************************************/
            //! regular iterator for EdoString
            class iterator : public std::iterator<std::random_access_iterator_tag, utf32> {
            public:
                utf32 *d_ptr;

                iterator() : d_ptr(nullptr) {}

                explicit iterator(utf32 *const ptr) : d_ptr(ptr) {}

                utf32 &operator*() const { return *d_ptr; }

                utf32 *operator->() const { return &**this; }

                EdoString::iterator &operator++() {
                    ++d_ptr;
                    return *this;
                }

                // TODO: Should this and --(int) have 'const' at the start?
                EdoString::iterator operator++(int) {
                    EdoString::iterator temp = *this;
                    ++*this;
                    return temp;
                }

                EdoString::iterator &operator--() {
                    --d_ptr;
                    return *this;
                }

                EdoString::iterator operator--(int) {
                    EdoString::iterator temp = *this;
                    --*this;
                    return temp;
                }

                EdoString::iterator &operator+=(difference_type offset) {
                    d_ptr += offset;
                    return *this;
                }

                EdoString::iterator operator+(difference_type offset) const {
                    EdoString::iterator temp = *this;
                    return temp += offset;
                }

                EdoString::iterator &operator-=(difference_type offset) { return *this += -offset; }

                EdoString::iterator operator-(difference_type offset) const {
                    EdoString::iterator temp = *this;
                    return temp -= offset;
                }

                utf32 &operator[](difference_type offset) const { return *(*this + offset); }

                friend difference_type
                operator-(const EdoString::iterator &lhs, const EdoString::iterator &rhs) {
                    return lhs.d_ptr - rhs.d_ptr;
                }

                friend iterator operator+(difference_type offset, const EdoString::iterator &iter) {
                    return iter + offset;
                }

                friend bool operator==(const EdoString::iterator &lhs, const EdoString::iterator &rhs) {
                    return lhs.d_ptr == rhs.d_ptr;
                }

                friend bool operator!=(const EdoString::iterator &lhs, const EdoString::iterator &rhs) {
                    return lhs.d_ptr != rhs.d_ptr;
                }

                friend bool operator<(const EdoString::iterator &lhs, const EdoString::iterator &rhs) {
                    return lhs.d_ptr < rhs.d_ptr;
                }

                friend bool operator>(const EdoString::iterator &lhs, const EdoString::iterator &rhs) {
                    return lhs.d_ptr > rhs.d_ptr;
                }

                friend bool operator<=(const EdoString::iterator &lhs, const EdoString::iterator &rhs) {
                    return lhs.d_ptr <= rhs.d_ptr;
                }

                friend bool operator>=(const EdoString::iterator &lhs, const EdoString::iterator &rhs) {
                    return lhs.d_ptr >= rhs.d_ptr;
                }
            };

            //! const iterator for EdoString
            class const_iterator : public std::iterator<std::random_access_iterator_tag, const utf32> {
            public:
                const utf32 *d_ptr;

                const_iterator() : d_ptr(nullptr) {}

                explicit const_iterator(const utf32 *const ptr) : d_ptr(ptr) {}

                const_iterator(const EdoString::iterator &iter) : d_ptr(iter.d_ptr) {}

                const utf32 &operator*() const { return *d_ptr; }

                const utf32 *operator->() const { return &**this; }

                const_iterator &operator++() {
                    ++d_ptr;
                    return *this;
                }

                const_iterator operator++(int) {
                    const_iterator temp = *this;
                    ++*this;
                    return temp;
                }

                const_iterator &operator--() {
                    --d_ptr;
                    return *this;
                }

                const_iterator operator--(int) {
                    const_iterator temp = *this;
                    --*this;
                    return temp;
                }

                const_iterator &operator+=(difference_type offset) {
                    d_ptr += offset;
                    return *this;
                }

                const_iterator operator+(difference_type offset) const {
                    const_iterator temp = *this;
                    return temp += offset;
                }

                const_iterator &operator-=(difference_type offset) { return *this += -offset; }

                const_iterator operator-(difference_type offset) const {
                    const_iterator temp = *this;
                    return temp -= offset;
                }

                const utf32 &operator[](difference_type offset) const { return *(*this + offset); }

                const_iterator &operator=(const EdoString::iterator &iter) {
                    d_ptr = iter.d_ptr;
                    return *this;
                }

                friend const_iterator operator+(difference_type offset, const const_iterator &iter) {
                    return iter + offset;
                }

                friend difference_type operator-(const const_iterator &lhs, const const_iterator &rhs) {
                    return lhs.d_ptr - rhs.d_ptr;
                }

                friend bool operator==(const const_iterator &lhs, const const_iterator &rhs) {
                    return lhs.d_ptr == rhs.d_ptr;
                }

                friend bool operator!=(const const_iterator &lhs, const const_iterator &rhs) {
                    return lhs.d_ptr != rhs.d_ptr;
                }

                friend bool operator<(const const_iterator &lhs, const const_iterator &rhs) {
                    return lhs.d_ptr < rhs.d_ptr;
                }

                friend bool operator>(const const_iterator &lhs, const const_iterator &rhs) {
                    return lhs.d_ptr > rhs.d_ptr;
                }

                friend bool operator<=(const const_iterator &lhs, const const_iterator &rhs) {
                    return lhs.d_ptr <= rhs.d_ptr;
                }

                friend bool operator>=(const const_iterator &lhs, const const_iterator &rhs) {
                    return lhs.d_ptr >= rhs.d_ptr;
                }
            };

            /*!
             \brief
                Constant reverse iterator class for EdoString objects
             */
            typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

            /*!
             \brief
                Reverse iterator class for EdoString objects
             */
            typedef std::reverse_iterator<EdoString::iterator> reverse_iterator;

        public:
            ///////////////////////////////////////////////
            // Default constructor and destructor
            //////////////////////////////////////////////
            /*!
             \brief
                Constructs and empty string
             */
            EdoString() { Init(); }

            /*!
             \brief
                Destructor for EdoString objects
             */
            ~EdoString();

            //////////////////////////////////////////////
            // Construction via Edo::Types::EdoString
            //////////////////////////////////////////////
            /*!
             * \brief
             * Copy constructor - Creates a new string with the same value as \a str
             * @param str
             * EdoString object used to initialize the newly created string
             */
            EdoString(const EdoString &str) {
                Init();
                Assign(str);
            }

            /*!
             * \brief
             * Constructs a new string initialized with code points from another EdoString object
             * @param str
             * String object used to initialize newly created string
             * @param str_idx
             * Starting code-point of \a str to be used when initializing the new EdoString
             * @param str_num
             * Maximum number of code points from \a str that are to be assigned to the new EdoString
             */
            EdoString(const EdoString &str, size_type str_idx, size_type str_num = npos) {
                Init();
                Assign(str, str_idx, str_num);
            }

            //////////////////////////////////////////////
            // Construction via std::string
            //////////////////////////////////////////////
            /*!
             * \brief
             * Constructs a new string and initializes it using the std::string std_str
             * @param std_str
             * The std::string object that is to be used to initialize the new EdoString object
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * \exception std::length_error
             * Thrown if resulting EdoString object would be too big
             */
            EdoString(const std::string &std_str) {
                Init();
                Assign(std_str);
            }

            /*!
             * \brief
             * Constructs a new string initialized with characters from the given std::string object
             * @param std_str
             * std::string object used to initialize the newly created string
             * @param str_idx
             * Starting character of \a std_str to be used when initializing the new EdoString
             * @param str_num
             * Maximum number of character from \a std_str that are to be assigned to the new EdoString
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * \exception std::length_error
             * Thrown if resulting EdoString object would be too big
             */
            EdoString(const std::string &std_str, size_type str_idx, size_type str_num = npos) {
                Init();
                Assign(std_str, str_idx, str_num);
            }

            //////////////////////////////////////////////
            // Construction via UTF-8 stream (for straight ASCII use. only codes 0x00 - 0x7F are valid)
            //////////////////////////////////////////////
            /*!
             * \brief
             * Constructs a new EdoString object and initializes it using the provided utf8 encoded string buffer
             * @param utf8_str
             * Pointer to a buffer containing a null-terminated Unicode string encoded as utf8 data
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \exception std::length_error
             * Thrown if resulting EdoString object would be too big
             */
            EdoString(const utf8 *utf8_str) {
                Init();
                Assign(utf8_str);
            }

            /*!
             * \brief
             * Constructs a new EdoString object and initializes it using the provided utf8 encoded string buffer
             * @param utf8_str
             * Pointer to a buffer containing a null-terminated Unicode string encoded as utf8 data
             * @param chars_len
             * Length of the provided utf8 string in code units (not code-points)
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \exception std::length_error
             * Thrown if resulting EdoString object would be too big
             */
            EdoString(const utf8 *utf8_str, size_type chars_len) {
                Init();
                Assign(utf8_str, chars_len);
            }

            //////////////////////////////////////////////
            // Construction via code-point (using a UTF-32 code unit)
            //////////////////////////////////////////////
            /*!
             * \brief
             * Constructs a new EdoString that is initialized with the specified code point
             * @param num
             * The number of times \a code_point is to be put into new EdoString object
             * @param code_point
             * The Unicode code point to be used when initializing the EdoString object
             * \exception std::length_error
             * Thrown if resulting EdoString object would be too big
             */
            EdoString(size_type num, utf32 code_point) {
                Init();
                Assign(num, code_point);
            }

            //////////////////////////////////////////////
            // Construction via iterator
            //////////////////////////////////////////////
            // Create string with characters in the range [beg, end)
            /*!
             * Construct a new string object and initialize it with code-points from the range [beg, end)
             * @param iter_beg
             * Iterator describing the start of the data to be used when initializing the EdoString object
             * @param iter_end
             * Iterator describing the (exclusive) end of the data to be used when initializing the EdoString object
             */
            EdoString(const_iterator iter_beg, const_iterator iter_end) {
                Init();
                Append(iter_beg, iter_end);
            }

            //////////////////////////////////////////////
            // Construction via c-string
            //////////////////////////////////////////////
            /*!
             * \brief
             * Constructs a new EdoString object and initializes it using the provided c-string
             * @param cstr
             * Pointer to a c-string
             * \exception std::length_error
             * Thrown if resulting EdoString object would be too big
             */
            EdoString(const char *cstr) {
                Init();
                Assign(cstr);
            }

            /*!
             * \brief
             * Constructs a new EdoString object and initializes it using characters from the provided array
             * @param chars
             * Char array
             * @param chars_len
             * Number of chars from the array to be used
             * \exception std::length_error
             * Thrown if resulting EdoString object would be too big
             */
            EdoString(const char *chars, size_type chars_len) {
                Init();
                Assign(chars, chars_len);
            }

            //////////////////////////////////////////////
            // Size operations
            /////////////////////////////////////////////
            /*!
             * \brief
             * Returns the size of the EdoString in code points
             * @return
             * Number of code points currently in the EdoString
             */
            size_type Size() const { return d_cpLength; }

            /*!
             * \brief
             * Returns the size of the EdoString in code points
             * @return
             * Number of code points currently in the EdoString
             */
            size_type Length() const { return d_cpLength; }

            /*!
             * \brief
             * Returns true if the EdoString is empty
             * @return
             * true if the EdoString is empty, else false
             */
            bool Empty() const { return d_cpLength == 0; }

            /*!
             * \brief
             * Returns the maximum size of an EdoString
             *
             * Any operation that would result in an EdoString that is larger than this will throw the std::length_error exception
             * @return
             * The maximum number of code points that a string can contain
             */
            size_type MaxSize() const { return ((size_type) -1) / sizeof(utf32); }

            //////////////////////////////////////////////
            // Capacity operations
            //////////////////////////////////////////////
            // return the number of code points the string could hold without re-allocation
            // (due to internal encoding this will always report the figure for worst-case encoding, and could even be < size()!)
            /*!
             * \brief
             * Return the number of code points that the EdoString could hold before a re-allocation would be required.
             * @return
             * Size of the current reserve buffer. This is the maximum number of code points the EdoString could hold before
             * a buffer re-allocation would be required
             */
            size_type Capacity() const { return d_reserve - 1; }

            // Reserve internal memory for at-least 'num' code-points (characters). If num is 0, request is shrink-to-fit
            /*!
             * Specifies the amount of reserve capacity to allocate.
             * @param num
             * The number of code points to allocate space for. If \a num is larger than the current reserve, then a re-allocation
             * will occur. If \a num is smaller than the current reserve (but not 0) the buffer may be shrunk to the larger of
             * the specified numbers, or the current EdoString size (not implemented yet). If \a num is 0, then the buffer is
             * re-allocated to fit the current EdoString size
             * \exception std::length_error
             * Thrown if resulting EdoString object would be too big
             */
            void Reserve(size_type num = 0) {
                if (num == 0)
                    Trim();
                else
                    Grow(num);
            }

            //////////////////////////////////////////////
            // Comparisons
            //////////////////////////////////////////////
            /*!
             * \brief
             * Compares this EdoString with the EdoString 'str'
             * \note
             * This does currently not properly consider Unicode and / or the system locale.
             * @param str
             * The EdoString object that is to be compared with this String.
             * @return
             * - 0 if the EdoString objects are equal
             * - <0 if this EdoString is lexicographically smaller than \a str
             * - >0 if this EdoString is lexicographically greater than \a str
             */
            int Compare(const EdoString &str) const { return Compare(0, d_cpLength, str); }

            /*!
             * \brief
             * Compares code points from this EdoString with code points from the EdoString 'str'
             * \note
             * This does currently not properly consider Unicode and / or the system locale.
             * @param idx
             * Index of the first code point from this EdoString to consider
             * @param len
             * Maximum number of code points from this EdoString to consider
             * @param str
             * The EdoString object that is compared to this EdoString
             * @param str_idx
             * Index of the first code point from EdoString \a str to consider
             * @param str_len
             * Maximum number of code points from EdoString \a str to consider
             * @return
             * - 0 if the EdoString objects are equal
             * - <0 if this EdoString is lexicographically smaller than \a str
             * - >0 if this EdoString is lexicographically greater than \a str
             * \exception
             * std::out_of_range Thrown if either \a idx or \a str_idx are invalid
             */
            int Compare(size_type idx, size_type len, const EdoString &str, size_type str_idx = 0,
                        size_type str_len = npos) const {
                if (d_cpLength < idx || str.d_cpLength < str_idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (len == npos || idx + len > d_cpLength)
                    len = d_cpLength - idx;

                if (str_len == npos || str_idx + str_len > str.d_cpLength)
                    str_len = str.d_cpLength - str_idx;

                int val = (len == 0) ? 0 : Utf32CompUtf32(&ptr()[idx], &str.ptr()[str_idx],
                                                          (len < str_len) ? len : str_len);

                return (val != 0) ? ((val < 0) ? -1 : 1) : (len < str_len) ? -1 : (len == str_len) ? 0 : 1;
            }

            /*!
             * \brief
             * Compares this EdoString with the std::string 'std_str'
             * \note
             * This does currently not properly consider Unicode and / or the system locale.
             * @param std_str
             * The std::string object that is compared with this EdoString.
             * \note
             * Characters from \a std_str are considered to represent Unicode code points in the range 0x00..0xFF. No
             * translation of the encountered data is performed.
             * @return
             * - 0 if the EdoString objects are equal
             * - <0 if this EdoString is lexicographically smaller than \a str
             * - >0 if this EdoString is lexicographically greater than \a str
             */
            int Compare(const std::string &std_str) const { return Compare(0, d_cpLength, std_str); }

            /*!
             * \brief
             * Compares code points from this EdoString with code points from the std::string 'std_str'
             * \note
             * This does currently not properly consider Unicode and / or the system locale.
             * @param idx
             * Index of the first code point from this EdoString to consider.
             * @param len
             * Maximum number of code points from this EdoString to consider.
             * @param std_str
             * The std::string object that is to be compared with this EdoString.
             * \note
             * Characters from \a std_str are considered to represent Unicode code points in the range 0x00..0xFF. No
             * translation of the encountered data is performed.
             * @param str_idx
             * Index of the first character from std::string \a std_str to consider.
             * @param str_len
             * Maximum number of characters from std::string \a std_str to consider.
             * @return
             * - 0 if the EdoString objects are equal
             * - <0 if this EdoString is lexicographically smaller than \a str
             * - >0 if this EdoString is lexicographically greater than \a str
             * \exception
             * std::out_of_range Thrown if either \a idx or \a str_idx are invalid
             */
            int Compare(size_type idx, size_type len, const std::string &std_str, size_type str_idx = 0,
                        size_type str_len = npos) const {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (std_str.size() < str_idx)
                    throw std::out_of_range("Index is out of range for std::string");

                if (len == npos || idx + len > d_cpLength)
                    len = d_cpLength - idx;

                if (str_len == npos || str_idx + str_len > std_str.size())
                    str_len = (size_type) std_str.size() - str_idx;

                int val = (len == 0) ? 0 : Utf32CompChar(&ptr()[idx], &std_str.c_str()[str_idx],
                                                         (len < str_len) ? len : str_len);

                return (val != 0) ? ((val < 0) ? -1 : 1) : (len < str_len) ? -1 : (len == str_len) ? 0 : 1;
            }

            /*!
             * \brief
             * Compares this EdoString with the null-terminated utf8 encoded 'utf8_str'
             * \note
             * This does currently not properly consider Unicode and / or the system locale.
             * @param utf8_str
             * The buffer containing valid Unicode data encoded as utf8 that is compared with this EdoString.
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @return
             * - 0 if the EdoString objects are equal
             * - <0 if this EdoString is lexicographically smaller than \a str
             * - >0 if this EdoString is lexicographically greater than \a str
             */
            int Compare(const utf8 *utf8_str) const { return Compare(0, d_cpLength, utf8_str, EncodedSize(utf8_str)); }

            /*!
             * \brief
             * Compares code points from this EdoString with the null-terminated utf8 encoded 'utf8_str'
             * \note
             * This does currently not properly consider Unicode and / or the system locale.
             * @param idx
             * Index of the first code point from this EdoString to consider
             * @param len
             * Maximum number of code points from this EdoString to consider
             * @param utf8_str
             * The buffer containing valid Unicode data encoded as utf8 that is to be compared with this EdoString
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @return
             * - 0 if the EdoString objects are equal
             * - <0 if this EdoString is lexicographically smaller than \a str
             * - >0 if this EdoString is lexicographically greater than \a str
             * \exception
             * std::out_of_range Thrown if \a idx is invalid
             */
            int Compare(size_type idx, size_type len, const utf8 *utf8_str) const {
                return Compare(idx, len, utf8_str, EncodedSize(utf8_str));
            }

            /*!
             * \brief
             * Compares code points from this EdoString with the utf8 encoded data in buffer 'utf8_str'
             * \note
             * This does currently not properly consider Unicode and / or the system locale.
             * @param idx
             * Index of the first code point from this EdoString to consider
             * @param len
             * Maximum number of code points from this EdoString to consider
             * @param utf8_str
             * The buffer containing valid Unicode data encoded as utf8 that is compared to this EdoString
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @param str_cpLen
             * The number of encoded code points in the buffer \a utf8_str (this is not the same as the number of code units)
             * @return
             * - 0 if the EdoString objects are equal
             * - <0 if this EdoString is lexicographically smaller than \a str
             * - >0 if this EdoString is lexicographically greater than \a str
             * \exception
             * std::out_of_range Thrown if \a idx is invalid.
             * \exception
             * std::length_error Thrown if \a str_cpLen is set to npos
             */
            int Compare(size_type idx, size_type len, const utf8 *utf8_str, size_type str_cpLen) const {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (str_cpLen == npos)
                    throw std::length_error("Length for utf8 encoded string can not be 'npos'");

                if (len == npos || idx + len > d_cpLength)
                    len = d_cpLength - idx;

                int val = (len == 0) ? 0 : Utf32CompUtf8(&ptr()[idx], utf8_str, (len < str_cpLen) ? len : str_cpLen);

                return (val != 0) ? ((val < 0) ? -1 : 1) : (len < str_cpLen) ? -1 : (len == str_cpLen) ? 0 : 1;
            }

            /*!
             * \brief
             * Compares this EdoString with the given c-string
             * \note
             * This does currently not properly consider Unicode and / or the system locale.
             * @param cstr
             * The c-string that is to be compared with this EdoString
             * @return
             * - 0 if the EdoString objects are equal
             * - <0 if this EdoString is lexicographically smaller than \a str
             * - >0 if this EdoString is lexicographically greater than \a str
             */
            int Compare(const char *cstr) const { return Compare(0, d_cpLength, cstr, strlen(cstr)); }

            /*!
             * \brief
             * Compares code points from this EdoString with the given c-string
             * \note
             * This does currently not properly consider Unicode and / or the system locale.
             * @param idx
             * Index of the first code point from this EdoString to consider
             * @param len
             * Maximum number of code points from this EdoString to consider
             * @param cstr
             * The c-string that is to be compared with this EdoString
             * @return
             * - 0 if the EdoString objects are equal
             * - <0 if this EdoString is lexicographically smaller than \a str
             * - >0 if this EdoString is lexicographically greater than \a str
             */
            int Compare(size_type idx, size_type len, const char *cstr) const {
                return Compare(idx, len, cstr, strlen(cstr));
            }

            /*!
             * \brief
             * Compares code points from this EdoString with chars in the given char array
             * \note
             * This does currently not properly consider Unicode and / or the system locale.
             * @param idx
             * Index of the first code point from this EdoString to consider
             * @param len
             * Maximum number of code points from this EdoString to consider
             * @param chars
             * The array containing the chars that are to be compared with this EdoString
             * @param chars_len
             * The number of chars in the array
             * @return
             * - 0 if the EdoString objects are equal
             * - <0 if this EdoString is lexicographically smaller than \a str
             * - >0 if this EdoString is lexicographically greater than \a str
             * \exception
             * std::out_of_range Thrown if \a idx is invalid.
             * \exception
             * std::length_error Thrown if \a str_cpLen is set to npos
             */
            int Compare(size_type idx, size_type len, const char *chars, size_type chars_len) const {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (chars_len == npos)
                    throw std::length_error("Length for char array can not be 'npos'");

                if (len == npos || idx + len > d_cpLength)
                    len = d_cpLength - idx;

                int val = (len == 0) ? 0 : Utf32CompChar(&ptr()[idx], chars, (len < chars_len) ? len : chars_len);

                return (val != 0) ? ((val < 0) ? -1 : 1) : (len < chars_len) ? -1 : (len == chars_len) ? 0 : 1;
            }

            //////////////////////////////////////////////
            // Character access
            //////////////////////////////////////////////
            /*!
             * \brief
             * Returns the code point at the given index
             * @param idx
             * Zero based index of the code point to be returned
             * \note
             * - For constant strings length()/size() provide a valid index and will access the default utf32 value.
             * - For non-constant strings length()/size() is an invalid index, and accessing (especially writing) at this index could cause string corruption
             * @return
             * The utf32 code point at the given index within the EdoString
             */
            reference operator[](size_type idx) { return ptr()[idx]; }

            /*!
             * \brief
             * Returns the code point at the given index
             * @param idx
             * Zero based index of the code point to be returned
             * \note
             * - For constant strings length()/size() provide a valid index and will access the default utf32 value.
             * - For non-constant strings length()/size() is an invalid index, and accessing (especially writing) at this index could cause string corruption
             * @return
             * The utf32 code point at the given index within the EdoString
             */
            value_type operator[](size_type idx) const { return ptr()[idx]; }

            /*!
             * \brief
             * Returns the code point at the given index
             * @param idx
             * Zero based index of the code point to be returned
             * @return
             * The utf32 code point at the given index within the EdoString
             * \exception
             * std::out_of_range Thrown if \a idx is >= length()
             */
            reference At(size_type idx) {
                if (d_cpLength <= idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                return ptr()[idx];
            }

            /*!
             * \brief
             * Returns the code point at the given index
             * @param idx
             * Zero based index of the code point to be returned
             * @return
             * The utf32 code point at the given index within the EdoString
             * \exception
             * std::out_of_range Thrown if \a idx is >= length()
             */
            const_reference At(size_type idx) const {
                if (d_cpLength <= idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                return ptr()[idx];
            }

            //////////////////////////////////////////////
            // C-Strings and arrays
            //////////////////////////////////////////////
            /*!
             * \brief
             * Returns contents of the EdoString as a null terminated string of utf8 encoded data
             * @return
             * Pointer to a char buffer containing the contents of the EdoString encoded as null-terminated utf8 data
             * \note
             * The buffer returned from this function is owned by the EdoString object.
             * \note
             * Any function that modifies the EdoString data will invalidate the buffer returned by this call.
             */
            const char *c_str() const {
                return (const char *) BuildUtf8Buff();
            }

            /*!
             * \brief
             * Returns contents of the EdoString as utf8 encoded data.
             * @return
             * Pointer to a buffer containing the contents of the EdoString encoded utf8 data.
             * \note
             * The buffer returned from this function is owned by the EdoString object.
             * \note
             * Any function that modifies the EdoString data will invalidate the buffer returned by this call.
             */
            const utf8 *Data() const {
                return BuildUtf8Buff();
            }

            /*!
             * \brief
             * Returns a pointer to the buffer in use.
             */
            utf32 *ptr() {
                return (d_reserve > STR_QUICKBUFF_SIZE) ? d_buffer : d_quickBuff;
            }

            /*!
             * \brief
             * Returns a const pointer to the buffer in use
             */
            const utf32 *ptr() const {
                return (d_reserve > STR_QUICKBUFF_SIZE) ? d_buffer : d_quickBuff;
            }

            // copy, at most, 'len' code-points of the EdoString, beginning with code-point 'idx', into the array'buf'
            // as valid utf8 encoded data. Return number of utf8 code units placed into the buffer
            /*!
             * \brief
             * Copies an area of the EdoString into the provided buffer as encoded utf8 data.
             * @param buf
             * Pointer to a buffer that is to receive the encoded data (this must be big enough to hold the encoded data)
             * @param len
             * Maximum number of code points from the EdoString that should be encoded into the buffer
             * @param idx
             * Index of the first code point to be encoded into the buffer
             * @return
             * The number of utf8 encoded code units transferred to the buffer
             * \note
             * A code unit does not equal a code point. A utf32 code point, when encoded as utf8, can occupy between 1 and 4 code units.
             * \exception
             * std::out_of_range Thrown if \a idx was invalid for this EdoString
             */
            size_type Copy(utf8 *buf, size_type len = npos, size_type idx = 0) const {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (len == npos)
                    len = d_cpLength;

                return Encode(&ptr()[idx], buf, npos, len);
            }

            //////////////////////////////////////////////
            // UTF8 Encoding Length information
            //////////////////////////////////////////////
            // return the number of bytes required to hold 'num' code-points, starting at code-point 'idx', of the string
            // when encoded as utf8 data.
            /*!
             * \brief
             * Return the number of utf8 code units required to hold an area of the EdoString when encoded as utf8 data
             * @param num
             * Maximum number of code points to consider when calculating utf8 encoded size.
             * @param idx
             * Index of the first code point to consider when calculating utf8 encoded size.
             * @return
             * The number of utf8 code units (bytes) required to hold the specified sub-string when encoded as utf8 data.
             * \exception
             * std::out_of_range Thrown if \a idx was invalid for this EdoString
             */
            size_type Utf8StreamLen(size_type num = npos, size_type idx = 0) const {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                size_type maxLen = d_cpLength - idx;

                return EncodedSize(&ptr()[idx], std::min(num, maxLen));
            }

            //////////////////////////////////////////////
            // Assignment Functions
            //////////////////////////////////////////////
            /*!
             * \brief
             * Assign the value of EdoString \a str to this EdoString
             * @param str
             * EdoString object containing the string value to be assigned.
             * @return
             * This EdoString after the assignment has happened
             */
            EdoString &operator=(const EdoString &str) {
                return Assign(str);
            }

            /*!
             * Assign a sub-string of EdoString \a str to this EdoString
             * @param str
             * String object containing the string data to be assigned.
             * @param str_idx
             * Index of the first code point in \a str that is to be assigned
             * @param str_num
             * Maximum number of code points from \a str that are to be assigned
             * @return
             * This EdoString after the assignment has happened
             * \exception
             * std::out_of_range Thrown if \a str_idx is invalid for \a str
             */
            EdoString &Assign(const EdoString &str, size_type str_idx = 0, size_type str_num = npos) {
                if (str.d_cpLength < str_idx)
                    throw std::out_of_range("Index was out of range for EdoString");

                if (str_num == npos || str_num > str.d_cpLength - str_idx)
                    str_num = str.d_cpLength - str_idx;

                Grow(str_num);
                SetLen(str_num);
                memcpy(ptr(), &str.ptr()[str_idx], str_num * sizeof(utf32));

                return *this;
            }

            /*!
             * \brief
             * Assign the value of std::string \a std_str to this EdoString
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * @param std_str
             * std::string object containing the string value to be assigned
             * @return
             * This EdoString after the assignment has happened
             * \exception
             * std::length_error Thrown if the resulting EdoString would have been too large.
             */
            EdoString &operator=(const std::string &std_str) {
                return Assign(std_str);
            }

            /*!
             * \brief
             * Assign a sub-string of std::string \a std_str to this EdoString
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * @param std_str
             * std::string object containing the string value to be assigned
             * @param str_idx
             * Index of the first character of \a std_str to be assigned
             * @param str_num
             * Maximum number of characters from \a std_str to be assigned
             * @return
             * This EdoString after the assignment has happened
             * \exception
             * std::out_of_range Thrown if \a str_idx is invalid for \a std_str
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &Assign(const std::string &std_str, size_type str_idx = 0, size_type str_num = npos) {
                if (std_str.size() < str_idx)
                    throw std::out_of_range("Index was out of range for std::string object");

                if (str_num == npos || str_num > (size_type) std_str.size() - str_idx)
                    str_num = (size_type) std_str.size() - str_idx;

                Grow(str_num);
                SetLen(str_num);

                while (str_num--) {
                    (*this)[str_num] = static_cast<utf32>(static_cast<unsigned char>(std_str[str_num + str_idx]));
                }

                return *this;
            }

            /*!
             * \brief
             * Assign to this EdoString the string value represented by the given null-terminated utf8 encoded data
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @param utf8_str
             * Buffer containing valid null-terminated utf8 encoded data
             * @return
             * This EdoString after the assignment has happened
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &operator=(const utf8 *utf8_str) {
                return Assign(utf8_str, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Assign to this EdoString the string value represented by the given null-terminated utf8 encoded data
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @param utf8_str
             * Buffer containing valid null-terminated utf8 encoded data
             * @param str_num
             * Number of code units (not code points) in the buffer pointed to by \a utf8_str
             * @return
             * This EdoString after the assignment has happened
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &Assign(const utf8 *utf8_str, size_type str_num) {
                if (str_num == npos)
                    throw std::length_error("Length for utf8 encoded string can not be 'npos'");

                size_type encSize = EncodedSize(utf8_str, str_num);

                Grow(encSize);
                Encode(utf8_str, ptr(), d_reserve, str_num);
                SetLen(encSize);
                return *this;
            }

            /*!
             * \brief
             * Assigns the specifiec utf32 code point to this EdoString. Result is always a EdoString 1 code point in length.
             * @param code_point
             * Valid utf32 Unicode code point to be assigned to the EdoString
             * @return
             * This EdoString after assignment
             */
            EdoString &operator=(utf32 code_point) {
                return Assign(1, code_point);
            }

            /*!
             * \brief
             * Assigns the specifiec code point repeatedly to the EdoString
             * @param num
             * The number of times to assign the code point
             * @param code_point
             * Valid utf32 Unicode code point to be assigned to the string
             * @return
             * This EdoString after assignment.
             * \exception
             * std::length_error Throw if \a num was 'npos'
             */
            EdoString &Assign(size_type num, utf32 code_point) {
                if (num == npos)
                    throw std::length_error("Code point count can not be 'npos'");

                Grow(num);
                SetLen(num);
                utf32 *p = ptr();

                while (num--)
                    *p++ = code_point;

                return *this;
            }

            /*!
             * \brief
             * Assign to this EdoString the given C-string
             * @param cstr
             * Pointer to a valid C style string
             * @return
             * This EdoString after the assignment has happened
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &operator=(const char *cstr) {
                return Assign(cstr, strlen(cstr));
            }

            /*!
             * \brief
             * Assign to this EdoString the given C-string
             * @param cstr
             * Pointer to a valid C style string
             * @return
             * This EdoString after the assignment has happened
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &Assign(const char *cstr) {
                return Assign(cstr, strlen(cstr));
            }

            /*!
             * \brief
             * Assign to this EdoString the given C-string
             * @param cstr
             * Pointer to a valid C style string
             * @param chars_len
             * Number of chars to be assigned
             * @return
             * This EdoString after the assignment has happened
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &Assign(const char *chars, size_type chars_len) {
                Grow(chars_len);
                utf32 *pt = ptr();

                for (size_type i = 0; i < chars_len; ++i) {
                    *pt++ = static_cast<utf32>(static_cast<unsigned char>(*chars++));
                }

                SetLen(chars_len);
                return *this;
            }

            void Swap(EdoString &str) {
                size_type tempLen = d_cpLength;
                d_cpLength = str.d_cpLength;
                str.d_cpLength = tempLen;

                size_type tempRes = d_reserve;
                d_reserve = str.d_reserve;
                str.d_reserve = tempRes;

                utf32 *tempBuf = d_buffer;
                d_buffer = str.d_buffer;
                str.d_buffer = tempBuf;

                // TODO: See if we need to swap 'quick buffer' data
                if (tempRes <= STR_QUICKBUFF_SIZE) {
                    utf32 tempQbf[STR_QUICKBUFF_SIZE];

                    memcpy(tempQbf, d_quickBuff, STR_QUICKBUFF_SIZE * sizeof(utf32));
                    memcpy(d_quickBuff, str.d_quickBuff, STR_QUICKBUFF_SIZE * sizeof(utf32));
                    memcpy(str.d_quickBuff, tempQbf, STR_QUICKBUFF_SIZE * sizeof(utf32));
                }
            }

            //////////////////////////////////////////////
            // Appending Functions
            //////////////////////////////////////////////
            /*!
             * \brief
             * Appends the EdoString \a str
             * @param str
             * EdoString object that is to be appended
             * @return
             * This EdoString after the append operation
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &operator+=(const EdoString &str) {
                return Append(str);
            }

            /*!
             * \brief
             * Appends a sub-string of the EdoString \a str
             * @param str
             * EdoString object containing data to be appended
             * @param str_idx
             * Index of the first code point to be appended
             * @param str_num
             * Maximum number of code points to be appended
             * @return
             * This EdoString after the append operation
             * \exception
             * std::out_of_range Thrown if \a str_idx is invalid for \a str
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Append(const EdoString &str, size_type str_idx = 0, size_type str_num = npos) {
                if (str.d_cpLength < str_idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (str_num == npos || str_num > str.d_cpLength - str_idx)
                    str_num = str.d_cpLength - str_idx;

                Grow(d_cpLength + str_num);
                memcpy(&ptr()[d_cpLength], &str.ptr()[str_idx], str_num * sizeof(utf32));
                SetLen(d_cpLength + str_num);
                return *this;
            }

            /*!
             * \brief
             * Appends the std::string \a std_str
             * @param std_str
             * std::string object that is to be appended
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * @return
             * This EdoString after the append operation
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &operator+=(const std::string &std_str) {
                return Append(std_str);
            }

            /*!
             * \brief
             * Appends a sub-string of the std::string \a std_str
             * @param std_str
             * std::string object containing data to be appended
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * @param str_idx
             * Index of the first character to be appended
             * @param str_num
             * Maximum number of characters to be appended
             * @return
             * This EdoString after the append operation
             * \exception
             * std::out_of_range Thrown if \a str_idx is invalid for \a std_str
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Append(const std::string &std_str, size_type str_idx = 0, size_type str_num = npos) {
                if (std_str.size() < str_idx)
                    throw std::out_of_range("Index is out of range for std::string");

                if (str_num == npos || str_num > (size_type) std_str.size() - str_idx)
                    str_num = (size_type) std_str.size() - str_idx;

                size_type newSize = d_cpLength + str_num;

                Grow(newSize);
                utf32 *pt = &ptr()[newSize - 1];

                while (str_num--)
                    *pt-- = static_cast<utf32>(static_cast<unsigned char>(std_str[str_num]));

                SetLen(newSize);
                return *this;
            }

            /*!
             * \brief
             * Appends to the EdoString the null-terminated utf8 encoded data in the buffer utf8_str
             * @param utf8_str
             * Buffer holding the null-terminated utf8 encoded data that is to be appended
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @return
             * This EdoString after the append operation
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &operator+=(const utf8 *utf8_str) {
                return Append(utf8_str, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Appends to the EdoString the null-terminated utf8 encoded data in the buffer utf8_str.
             * @param utf8_str
             * Buffer holding the null-terminated utf8 encoded data that is to be appended
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @return
             * This EdoString after the append operation
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Append(const utf8 *utf8_str) {
                return Append(utf8_str, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Appends to the EdoString the utf8 encoded data in the buffer utf8_str
             * @param utf8_str
             * Buffer holding the utf8 encoded data that is to be appended
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @param len
             * Number of code units (not code points) in the buffer to append
             * @return
             * This EdoString after the append operation
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large, or if \a len was 'npos'
             */
            EdoString &Append(const utf8 *utf8_str, size_type len) {
                if (len == npos)
                    throw std::length_error("Length for utf8 encoded string can not be 'npos'");

                size_type encSize = EncodedSize(utf8_str, len);
                size_type newSize = d_cpLength + encSize;

                Grow(newSize);
                Encode(utf8_str, &ptr()[d_cpLength], encSize, len);
                SetLen(newSize);

                return *this;
            }

            /*!
             * \brief
             * Appends a single code point to the string
             * @param code_point
             * utf32 Unicode code point that is to be appended
             * @return
             * This EdoString after the append operation
             * \exception
             * std::length_error Thrown if resulting EdoString would be too long
             */
            EdoString &operator+=(utf32 code_point) {
                return Append(1, code_point);
            }

            /*!
             * \brief
             * Appends a single code point multiple times to the string
             * @param num
             * Number of copies of the code point to be appended
             * @param code_point
             * utf32 Unicode code point that is to be appended
             * @return
             * This EdoString after the append operation
             * \exception
             * std::length_error Thrown if resulting EdoString would be too long, or if \a num was 'npos'
             */
            EdoString &Append(size_type num, utf32 code_point) {
                if (num == npos)
                    throw std::length_error("Code point cound can not be 'npos'");

                size_type newSize = d_cpLength + num;
                Grow(newSize);

                utf32 *p = &ptr()[d_cpLength];

                while (num--)
                    *p++ = code_point;

                SetLen(newSize);
                return *this;
            }

            /*!
             * \brief
             * Appends a single code point to the string
             * @param code_point
             * utf32 Unicode code point that is to be appended
             * \exception
             * std::length_error Thrown if resulting EdoString would be too long
             */
            void PushBack(utf32 code_point) {
                Append(1, code_point);
            }

            /*!
             * \brief
             * Appends the code points in the range [beg, end)
             * @param iter_beg
             * Iterator describing the start of the range to be appended
             * @param iter_end
             * Iterator describing the (exclusive) end of the range to be appended
             * @return
             * This EdoString after the append operation
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &Append(const_iterator iter_beg, const_iterator iter_end) {
                return Replace(End(), End(), iter_beg, iter_end);
            }

            /*!
             * \brief
             * Appends to the EdoString the given c-string
             * @param cstr
             * c-string that is to be appended
             * @return
             * This EdoString after the append operation
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &operator+=(const char *cstr) {
                return Append(cstr, strlen(cstr));
            }

            /*!
             * \brief
             * Appends to the EdoString chars from the given char array
             * @param chars
             * char array holding the chars that are to be appended
             * @param chars_len
             * Number of chars to be appended
             * @return
             * This EdoString after the append operation
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large, or if \a chars_len was 'npos'
             */
            EdoString &Append(const char *chars, size_type chars_len) {
                if (chars_len == npos)
                    throw std::length_error("Length for char array can not be 'npos'");

                size_type newSize = d_cpLength + chars_len;
                Grow(newSize);
                utf32 *pt = &ptr()[newSize - 1];

                while (chars_len--)
                    *pt-- = static_cast<utf32>(static_cast<unsigned char>(chars[chars_len]));

                SetLen(newSize);
                return *this;
            }

            //////////////////////////////////////////////
            // Insertion Functions
            //////////////////////////////////////////////
            /*!
             * \brief
             * Inserts the given EdoString object at the specified position
             * @param idx
             * Index where the string is to be inserted
             * @param str
             * EdoString object that is to be inserted
             * @return
             * This EdoString object after the insert
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Insert(size_type idx, const EdoString &str) {
                return Insert(idx, str, 0, npos);
            }

            /*!
             * \brief
             * Inserts a sub-string of the given EdoString object at the specified position
             * @param idx
             * Index where the string is to be inserted
             * @param str
             * EdoString object containing data to be inserted
             * @param str_idx
             * Index of the first code point from \a str to be inserted
             * @param str_num
             * Maximum number of code points from \a str to be inserted
             * @return
             * This EdoString after the insert
             * \exception
             * std::out_of_range Thrown if \a idx or \a str_idx are out of range
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Insert(size_type idx, const EdoString &str, size_type str_idx, size_type str_num) {
                if (d_cpLength < idx || str.d_cpLength < str_idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (str_num == npos || str_num > str.d_cpLength - str_idx)
                    str_num = str.d_cpLength - str_idx;

                size_type newSize = d_cpLength + str_num;
                Grow(newSize);
                memmove(&ptr()[idx + str_num], &ptr()[idx], (d_cpLength - idx) * sizeof(utf32));
                memcpy(&ptr()[idx], &str.ptr()[str_idx], str_num * sizeof(utf32));
                SetLen(newSize);

                return *this;
            }

            /*!
             * \brief
             * Inserts the given std::string object at the specified position
             * @param idx
             * Index where the std::string is to be inserted
             * @param std_str
             * std::string object that is to be inserted
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * @return
             * This EdoString after the insert
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Insert(size_type idx, const std::string &std_str) {
                return Insert(idx, std_str, 0, npos);
            }

            /*!
             * \brief
             * Inserts a sub-string of the given std::string object at the specified position
             * @param idx
             * Index where the string is to be inserted
             * @param std_str
             * std::string object containing data to be inserted
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * @param str_idx
             * Index of the first character from \a std_str to be inserted
             * @param str_num
             * Maximum number of characters from \a std_str to be inserted
             * @return
             * This EdoString after the insert
             * \exception
             * std::out_of_range Thrown if \a idx or \a str_idx are out of range
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Insert(size_type idx, const std::string &std_str, size_type str_idx, size_type str_num) {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (std_str.size() < str_idx)
                    throw std::out_of_range("Index is out of range for std::string");

                if (str_num == npos || str_num > (size_type) std_str.size() - str_idx)
                    str_num = (size_type) std_str.size() - str_idx;

                size_type newSize = d_cpLength + str_num;
                Grow(newSize);

                memmove(&ptr()[idx + str_num], &ptr()[idx], (d_cpLength - idx) * sizeof(utf32));
                utf32 *pt = &ptr()[idx + str_num - 1];

                while (str_num--)
                    *pt-- = static_cast<utf32>(static_cast<unsigned char>(std_str[str_idx + str_num]));

                SetLen(newSize);
                return *this;
            }

            /*!
             * \brief
             * Inserts the given null-terminated utf8 encoded data at the specified position
             * @param idx
             * Index where the data is to be inserted
             * @param utf8_str
             * Buffer containing the null-terminated utf8 encoded data that is to be inserted
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @return
             * This EdoString after the insert
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Insert(size_type idx, const utf8 *utf8_str) {
                return Insert(idx, utf8_str, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Inserts the given utf8 encoded data at the specified position
             * @param idx
             * Index where the data is to be inserted
             * @param utf8_str
             * Buffer containing the utf8 encoded data that is to be inserted
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @param len
             * Length of the data to be inserted in utf8 code units (not code points)
             * @return
             * This EdoString after the insert
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large, or if \a len is 'npos'
             */
            EdoString &Insert(size_type idx, const utf8 *utf8_str, size_type len) {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (len == npos)
                    throw std::length_error("Length of utf8 encoded string can not be 'npos'");

                size_type encSize = EncodedSize(utf8_str, len);
                size_type newSize = d_cpLength + encSize;

                Grow(newSize);
                memmove(&ptr()[idx + encSize], &ptr()[idx], (d_cpLength - idx) * sizeof(utf32));
                Encode(utf8_str, &ptr()[idx], encSize, len);
                SetLen(newSize);

                return *this;
            }

            /*!
             * \brief
             * Inserts a code point multiple times into the EdoString
             * @param idx
             * Index where the code point(s) are to be inserted
             * @param num
             * The number of times to insert the code point
             * @param code_point
             * The utf32 code point that is to be inserted
             * @return
             * This EdoString after the insertion
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large, or if \a num is 'npos'
             */
            EdoString &Insert(size_type idx, size_type num, utf32 code_point) {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (num == npos)
                    throw std::length_error("Code point count can not be 'npos'");

                size_type newSize = d_cpLength + num;
                Grow(newSize);

                memmove(&ptr()[idx + num], &ptr()[idx], (d_cpLength - idx) * sizeof(utf32));
                utf32 *pt = &ptr()[idx + num - 1];

                while (num--)
                    *pt-- = code_point;

                SetLen(newSize);
                return *this;
            }

            /*!
             * \brief
             * Inserts a code point multiple times into the EdoString
             * @param pos
             * Iterator describing the position where the code point(s) are to be inserted
             * @param num
             * The number of times to insert the code point
             * @param code_point
             * The utf32 code point that is to be inserted
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large, or if \a num is 'npos'
             */
            void Insert(iterator pos, size_type num, utf32 code_point) {
                Insert(safe_iter_dif(pos, Begin()), num, code_point);
            }

            /*!
             * \brief
             * Inserts a single code point into the EdoString
             * @param pos
             * Iterator describing the position where the code point is to be inserted
             * @param code_point
             * The utf32 code point that is to be inserted
             * @return
             * This EdoString after the insertion
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            iterator Insert(iterator pos, utf32 code_point) {
                Insert(pos, 1, code_point);
                return pos;
            }

            /*!
             * \brief
             * Inserts code points specified by the range [beg, end)
             * @param iter_pos
             * Iterator describing the position where the data is to be inserted
             * @param iter_beg
             * Iterator describing the beginning of the range to be inserted
             * @param iter_end
             * Iterator describing the (exclusive) end of the range to be inserted
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            void Insert(iterator iter_pos, const_iterator iter_beg, const_iterator iter_end) {
                Replace(iter_pos, iter_pos, iter_beg, iter_end);
            }

            /*!
             * \brief
             * Inserts the given c-string at the specified position
             * @param idx
             * Index where the c-string is to be inserted
             * @param cstr
             * c-string that is to be inserted
             * @return
             * This EdoString after the insert
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Insert(size_type idx, const char *cstr) {
                return Insert(idx, cstr, strlen(cstr));
            }

            /*!
             * \brief
             * Inserts chars from the given char array at the specified position
             * @param idx
             * Index where the data is to be inserted
             * @param chars
             * char array containing the chars that are to be inserted
             * @param chars_len
             * Length of the char array to be inserted
             * @return
             * This EdoString after the insert
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large, or if \a chars_len is 'npos'
             */
            EdoString &Insert(size_type idx, const char *chars, size_type chars_len) {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (chars_len == npos)
                    throw std::length_error("Length of char array can not be 'npos'");

                size_type newSize = d_cpLength + chars_len;
                Grow(newSize);
                memmove(&ptr()[idx + chars_len], &ptr()[idx], (d_cpLength - idx) * sizeof(utf32));
                utf32 *pt = &ptr()[idx + chars_len - 1];

                while (chars_len--)
                    *pt-- = static_cast<utf32>(static_cast<unsigned char>(chars[chars_len]));

                SetLen(newSize);
                return *this;
            }

            //////////////////////////////////////////////
            // Erasing characters
            //////////////////////////////////////////////
            /*!
             * \brief
             * Removes all data from the EdoString
             */
            void Clear() {
                SetLen(0);
                Trim();
            }

            /*!
             * \brief
             * Removes all data from the EdoString
             * @return
             * The empty EdoString (*this)
             */
            EdoString &Erase() {
                Clear();
                return *this;
            }

            /*!
             * \brief
             * Erase a single code point from the string
             * @param idx
             * The index of the code point to be removed
             * @return
             * This EdoString after the erase operation
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            EdoString &Erase(size_type idx) {
                return Erase(idx, 1);
            }

            /*!
             * \brief
             * Erase a range of code points
             * @param idx
             * Index of the first code point to be removed
             * @param len
             * Maximum number of code points to be removed
             * @return
             * This EdoString after the erase operation
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            EdoString &Erase(size_type idx, size_type len) {
                // Cover the no-op case
                if (len == 0)
                    return *this;

                if (d_cpLength <= idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (len == npos)
                    len = d_cpLength - idx;

                size_type newSize = d_cpLength - len;
                memmove(&ptr()[idx], &ptr()[idx + len], (d_cpLength - idx - len) * sizeof(utf32));
                SetLen(newSize);

                return *this;
            }

            /*!
             * \brief
             * Erase the code point described by the given iterator
             * @param pos
             * Iterator describing the code point to be erased
             * @return
             * This EdoString after the erase operation
             */
            EdoString &Erase(iterator pos) {
                return Erase(safe_iter_dif(pos, Begin()), 1);
            }

            /*!
             * \brief
             * Erase a range of code points described by the iterators [beg, end)
             * @param iter_beg
             * Iterator describing the position of the beginning of the range to erase
             * @param iter_end
             * Iterator describing the (exclusive) end of the rage to erase
             * @return
             * This EdoString after the erase operation
             */
            EdoString &Erase(iterator iter_beg, iterator iter_end) {
                return Erase(safe_iter_dif(iter_beg, Begin()), safe_iter_dif(iter_end, iter_beg));
            }

            //////////////////////////////////////////////
            // Resizing
            //////////////////////////////////////////////
            /*!
             * \brief
             * Resizes the EdoString either by inserting default utf32 code points to make it larger, or by truncating
             * to make it smaller
             * @param num
             * The length, in code points, that the EdoString is to be made
             * \exception
             * std::length_error Thrown if the EdoString would be too large
             */
            void Resize(size_type num) {
                Resize(num, utf32());
            }

            /*!
             * \brief
             * Resizes the EdoString either by inserting the given utf32 code point to make it larger, or by truncating
             * to make it smaller
             * @param num
             * The length, in code points, that the EdoString is to be made
             * @param code_point
             * The utf32 code point that should be used when making the String larger
             * \exception
             * std::length_error Thrown if the EdoString would be too large
             */
            void Resize(size_type num, utf32 code_point) {
                if (num < d_cpLength)
                    SetLen(num);
                else
                    Append(num - d_cpLength, code_point);
            }

            //////////////////////////////////////////////
            // Replacing Characters
            //////////////////////////////////////////////
            /*!
             * \brief
             * Replace code points in the EdoString with the specified EdoString object
             * @param idx
             * Index of the first code point to be replaced
             * @param len
             * Maximum number of code points to be replaced (if this is 0, operation is an insert at position \a idx)
             * @param str
             * The EdoString object that is to replace the specified code points
             * @return
             * This EdoString after the replace operation
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Replace(size_type idx, size_type len, const EdoString &str) {
                return Replace(idx, len, str, 0, npos);
            }

            /*!
             * \brief
             * Replace the code points in the range [beg, end) with the specified EdoString object
             * \note
             * if \a iter_beg == \a iter_end, the operation is an insert at iterator position \a iter_beg
             * @param iter_beg
             * Iterator describing the start of the range to be replaced
             * @param iter_end
             * Iterator describing the (exclusive) end of the range to be replaced
             * @param str
             * The EdoString object that is to replace the specified range of code points
             * @return
             */
            EdoString &Replace(iterator iter_beg, iterator iter_end, const EdoString &str) {
                return Replace(safe_iter_dif(iter_beg, Begin()), safe_iter_dif(iter_end, iter_beg), str, 0, npos);
            }

            /*!
             * \brief
             * Replace code points in the EdoString with a specified sub-string of a given EdoString object
             * @param idx
             * Index of the first code point to be replaced
             * @param len
             * Maximum number of code points to be replaced. If this is 0, the operation is an insert at position \a idx
             * @param str
             * EdoString object containing the data that will replace the specified range of code points
             * @param str_idx
             * Index of the first code point of \a str that is to replace the specified code point range
             * @param str_num
             * Maximum number of code points of \a str that are to replace the specified code point range
             * @return
             * This EdoString after the replace operation
             * \exception
             * std::out_of_range Thrown if \a idx or \a str_idx are invalid
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &
            Replace(size_type idx, size_type len, const EdoString &str, size_type str_idx, size_type str_num) {
                if (d_cpLength < idx || str.d_cpLength < str_idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if ((str_idx + str_num) > str.d_cpLength || str_num == npos)
                    str_num = str.d_cpLength - str_idx;

                if ((len + idx) > d_cpLength || len == npos)
                    len = d_cpLength - idx;

                size_type newSize = d_cpLength + str_num - len;
                Grow(newSize);

                if (idx + len < d_cpLength)
                    memmove(&ptr()[idx + str_num], &ptr()[len + idx], (d_cpLength - idx - len) * sizeof(utf32));

                memcpy(&ptr()[idx], &str.ptr()[str_idx], str_num * sizeof(utf32));
                SetLen(newSize);

                return *this;
            }

            /*!
             * \brief
             * Replace code points in the EdoString with the specified std::string object
             * @param idx
             * Index of the first code point to be replaced
             * @param len
             * Maximum number of code points to be replaced (if this is 0, operation is an insert at position \a idx)
             * @param std_str
             * The std::string object that is to replace the specified code points
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * @return
             * This EdoString after the replace operation
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Replace(size_type idx, size_type len, const std::string &std_str) {
                return Replace(idx, len, std_str, 0, npos);
            }

            /*!
             * \brief
             * Replace the code points in the range [beg, end) with the specified std::string object
             * \note
             * If \a iter_beg == \a iter_end, the operation is an insert at iterator position \a iter_beg
             * @param iter_beg
             * Iterator describing the start of the range to be replaced
             * @param iter_end
             * Iterator describing the (exclusive) end of the range to be replaced
             * @param std_str
             * The std::string object that is to replace the specified range of code points
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * @return
             * This EdoString after the replace operation
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &Replace(iterator iter_beg, iterator iter_end, const std::string &std_str) {
                return Replace(safe_iter_dif(iter_beg, Begin()), safe_iter_dif(iter_end, iter_beg), std_str, 0, npos);
            }

            /*!
             * \brief
             * Replace code points in the EdoString with a specified sub-string of a given std::string object
             * @param idx
             * Index of the first code point to be replaced
             * @param len
             * Maximum number of code points to be replaced. If this is 0, the operation is an insert at position \a idx
             * @param std_str
             * std::string object containing the data that will replace the specified range of code points
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * @param str_idx
             * Index of the first code point of \a std_str that is to replace the specified code point range
             * @param str_num
             * Maximum number of code points of \a std_str that are to replace the specified code point range
             * @return
             * This EdoString after the replace operation
             * \exception
             * std::out_of_range Thrown if \a idx or \a str_idx are invalid
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &
            Replace(size_type idx, size_type len, const std::string &std_str, size_type str_idx, size_type str_num) {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (std_str.size() < str_idx)
                    throw std::out_of_range("Index is out of range for std::string");

                if (str_idx + str_num > std_str.size() || str_num == npos)
                    str_num = (size_type) std_str.size() - str_idx;

                if (len + idx > d_cpLength || len == npos)
                    len = d_cpLength - idx;

                size_type newSize = d_cpLength + str_num - len;
                Grow(newSize);

                if (idx + len < d_cpLength)
                    memmove(&ptr()[idx + str_num], &ptr()[len + idx], (d_cpLength - idx - len) * sizeof(utf32));

                utf32 *pt = &ptr()[idx + str_num - 1];

                while (str_num--)
                    *pt-- = static_cast<utf32>(static_cast<unsigned char>(std_str[str_idx + str_num]));

                SetLen(newSize);
                return *this;
            }

            /*!
             * \brief
             * Replace code points in the EdoString with the specified null-terminated utf8 encoded data
             * @param idx
             * Index of the first code point to be replaced
             * @param len
             * Maximum number of code points to be replaced (if this is 0, operation is an insert at position \a idx)
             * @param utf8_str
             * Buffer containing the null-terminated utf8 encoded data that is to replace the specified code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @return
             * This EdoString after the replace operation
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Replace(size_type idx, size_type len, const utf8 *utf8_str) {
                return Replace(idx, len, utf8_str, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Replace the code points in the range [beg, end) with the specified null-terminated utf8 encoded data
             * \note
             * If \a iter_beg == \a iter_end, the operation is an insert at iterator position \a iter_beg
             * @param iter_beg
             * Iterator describing the start of the range to be replaced
             * @param iter_end
             * Iterator describing the (exclusive) end of the range to be replaced
             * @param utf8_str
             * Buffer containing the null-terminated utf8 encoded data that is to replace the specified range of code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @return
             * This EdoString after the replace operation
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &Replace(iterator iter_beg, iterator iter_end, const utf8 *utf8_str) {
                return Replace(iter_beg, iter_end, utf8_str, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Replace code points in the EdoString with the specified utf8 encoded data
             * @param idx
             * Index of the first code point to be replaced
             * @param len
             * Maximum number of code points to be replaced (if this is 0, the operation is an insert at position \a idx)
             * @param utf8_str
             * Buffer containing the null terminated utf8 encoded data that is to replace the specified code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @param str_len
             * Length of the utf8 encoded data in utf8 code units (not code points)
             * @return
             * This EdoString after the replace operation
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large, or if \a str_len was 'npos'
             */
            EdoString &Replace(size_type idx, size_type len, const utf8 *utf8_str, size_type str_len) {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (str_len == npos)
                    throw std::length_error("Length for utf8 encoded string can not be 'npos'");

                if (len + idx > d_cpLength || len == npos)
                    len = d_cpLength - idx;

                size_type encSize = EncodedSize(utf8_str, str_len);
                size_type newSize = d_cpLength + encSize - len;
                Grow(newSize);

                if (idx + len < d_cpLength)
                    memmove(&ptr()[idx + encSize], &ptr()[len + idx], (d_cpLength - idx - len) * sizeof(utf32));

                Encode(utf8_str, &ptr()[idx], encSize, str_len);
                SetLen(newSize);
                return *this;
            }

            /*!
             * \brief
             * Replace the code points in the range [beg, end) with the specified null-terminated utf8 encoded data
             * \note
             * If \a iter_beg == \a iter_end, the operation is an insert at iterator position \a iter_beg
             * @param iter_beg
             * Iterator describing the start of the range to be replaced
             * @param iter_end
             * Iterator describing the (exclusive) end of the range to be replaced
             * @param utf8_str
             * Buffer containing the null-terminated utf8 encoded data that is to replace the specified range of code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * @param str_len
             * Length of the utf8 encoded data in utf8 code units (not code points)
             * @return
             * This EdoString after the replace operation
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large, or if \a str_len was 'npos'
             */
            EdoString &Replace(iterator iter_beg, iterator iter_end, const utf8 *utf8_str, size_type str_len) {
                return Replace(safe_iter_dif(iter_beg, Begin()), safe_iter_dif(iter_end, iter_beg), utf8_str, str_len);
            }

            /*!
             * \brief
             * Replaces a specified range of code points with occurrences of a given code point
             * @param idx
             * Index of the first code point to be replaced
             * @param len
             * Maximum number of code points to replace. If this is 0 the operation is an insert
             * @param num
             * Number of occurrences of \a code_point that are to replaced the specified range of code points
             * @param code_point
             * Code point that is to be used when replacing the specified range of code points
             * @return
             * This EdoString after the replace operation
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too long, or if \a num was 'npos'
             */
            EdoString &Replace(size_type idx, size_type len, size_type num, utf32 code_point) {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (num == npos)
                    throw std::length_error("Code point count can not be 'npos'");

                if (len + idx > d_cpLength || len == npos)
                    len = d_cpLength - idx;

                size_type newSize = d_cpLength + num - len;
                Grow(newSize);

                if (idx + len < d_cpLength)
                    memmove(&ptr()[idx + num], &ptr()[len + idx], (d_cpLength - idx - len) * sizeof(utf32));

                utf32 *pt = &ptr()[idx + num - 1];

                while (num--)
                    *pt-- = code_point;

                SetLen(newSize);
                return *this;
            }

            /*!
             * \brief
             * Replace the code points in the range [beg, end) with occurrences of a given code point
             * \note
             * If \a iter_beg == \a iter_end, the operation is an insert at iterator position \a iter_beg
             * @param iter_beg
             * Iterator describing the start of the range to be replaced
             * @param iter_end
             * Iterator describing the (exclusive) end of the range to be replaced
             * @param num
             * Number of occurrences of \a code_point that are to replace the specified range of code points
             * @param code_point
             * Code point that is to be used when replacing the specified range of code points
             * @return
             * This EdoString after the replace operation
             * \exception
             * std::length_error Thrown if resulting EdoString would have been too long, or if \a num was 'npos'
             */
            EdoString &Replace(iterator iter_beg, iterator iter_end, size_type num, utf32 code_point) {
                return Replace(safe_iter_dif(iter_beg, Begin()), safe_iter_dif(iter_end, iter_beg), num, code_point);
            }

            /*!
             * \brief
             * Replace the code points in the range [beg, end) with code points from the range [newBeg, newEnd)
             * \note
             * If \a iter_beg == \a iter_end, the operation is an insert at iterator position \a iter_beg
             * @param iter_beg
             * Iterator describing the start of the range to be replaced
             * @param iter_end
             * Iterator describing the (exclusive) end of the range to be replaced
             * @param iter_newBeg
             * Iterator describing the beginning of the range to insert
             * @param iter_newEnd
             * Iterator describing the (exclusive) end of the range to insert
             * @return
             * This EdoString after the insert operation
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too long
             */
            EdoString &
            Replace(iterator iter_beg, iterator iter_end, const_iterator iter_newBeg, const_iterator iter_newEnd) {
                if (iter_newBeg == iter_newEnd)
                    Erase(safe_iter_dif(iter_beg, Begin()), safe_iter_dif(iter_end, iter_beg));
                else {
                    size_type str_len = safe_iter_dif(iter_newEnd, iter_newBeg);
                    size_type idx = safe_iter_dif(iter_beg, Begin());
                    size_type len = safe_iter_dif(iter_end, iter_beg);

                    if (len + idx > d_cpLength)
                        len = d_cpLength - idx;

                    size_type newSize = d_cpLength + str_len - len;
                    Grow(newSize);

                    if (idx + len < d_cpLength)
                        memmove(&ptr()[idx + str_len], &ptr()[len + idx], (d_cpLength - idx - len) * sizeof(utf32));

                    memcpy(&ptr()[idx], iter_newBeg.d_ptr, str_len * sizeof(utf32));
                    SetLen(newSize);
                }

                return *this;
            }

            /*!
             * \brief
             * Replace code points in the EdoString with the specified c-string
             * \param idx
             * Index of the first code point to be replaced
             * \param len
             * Maximum number of code points to be replaced (if this is 0, operation is an insert at position \a idx)
             * \param cstr
             * C-string that is to replace the specified code points
             * \return
             * This EdoString after the replace operation
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large
             */
            EdoString &Replace(size_type idx, size_type len, const char *cstr) {
                return Replace(idx, len, cstr, strlen(cstr));
            }

            /*!
             * \brief
             * Replace the code points in the range [beg, end) with the specified c-string
             * \note
             * If \a iter_beg == \a iter_end, the operation is an insert at iterator position \a iter_beg
             * \param iter_beg
             * Iterator describing the start of the range to be replaced
             * \param iter_end
             * Iterator describing the (exclusive) end of the range to be replaced
             * \param cstr
             * C-string that is to replace the specified range of code points
             * \return
             * This EdoString aftre the replace operation
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString &Replace(iterator iter_beg, iterator iter_end, const char *cstr) {
                return Replace(iter_beg, iter_end, cstr, strlen(cstr));
            }

            /*!
             * \brief
             * Replace code points in the EdoString with chars from the given char array
             * \param idx
             * Index of the first code point to be replaced
             * \param len
             * Maximum number of code points to be replaced (if this is 0, operation is an insert at position \a idx)
             * \param chars
             * Char array containing the chars that are to replace the specified code points
             * \param chars_len
             * Number of chars in the given array
             * \return
             * This EdoString after the replace operation
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             * \exception
             * std::length_error Thrown if resulting EdoString would be too large, or if \a chars_len was 'npos'
             */
            EdoString &Replace(size_type idx, size_type len, const char *chars, size_type chars_len) {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for EdoString");

                if (chars_len == npos)
                    throw std::length_error("Length for the char array can not be 'npos'");

                if (len + idx > d_cpLength || len == npos)
                    len = d_cpLength - idx;

                size_type newSize = d_cpLength + chars_len - len;
                Grow(newSize);

                if (idx + len < d_cpLength)
                    memmove(&ptr()[idx + chars_len], &ptr()[len + idx], (d_cpLength - idx - len) * sizeof(utf32));

                utf32 *pt = &ptr()[idx + chars_len - 1];

                while (chars_len--)
                    *pt-- = static_cast<utf32>(static_cast<unsigned char>(chars[chars_len]));

                SetLen(newSize);
                return *this;
            }

            /*!
             * \brief
             * Replace the code points in the range [beg, end) with chars from the given char array
             * \note
             * If \a iter_beg == \a iter_end, the operatoin is an insert at iterator position \a iter_beg
             * \param iter_beg
             * Iterator describing the start of the range to be replaced
             * \param iter_end
             * Iterator describing the (exclusive) end of the range to be replaced
             * \param chars
             * Char array containing the chars that are to replace the specified range of code points
             * \param chars_len
             * Number of chars in the char array
             * \return
             * This EdoString after the replace operatoin
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large, or if \a chars_len was 'npos'
             */
            EdoString &Replace(iterator iter_beg, iterator iter_end, const char *chars, size_type chars_len) {
                return Replace(safe_iter_dif(iter_beg, Begin()), safe_iter_dif(iter_end, iter_beg), chars, chars_len);
            }

            //////////////////////////////////////////////
            // Find a code point
            //////////////////////////////////////////////
            /*!
             * \brief
             * Search forwards for a given code point
             * \param code_point
             * The utf32 code point to search for
             * \param idx
             * Index of the code point where the search is to start.
             * \return
             * - Index of the first occurrence of \a code_point travelling forwards from \a idx.
             * - npos if the code point could not be found
             */
            size_type Find(utf32 code_point, size_type idx = 0) const {
                if (idx < d_cpLength) {
                    const utf32 *pt = &ptr()[idx];

                    while (idx < d_cpLength) {
                        if (*pt++ == code_point)
                            return idx;

                        ++idx;
                    }
                }

                return npos;
            }

            /*!
             * \brief
             * Search backwards for a given code point
             * \param code_point
             * The utf32 code point to search for
             * \param idx
             * Index of code point where the search is to start
             * \return
             * - Index of the first occurrence of \a code_point travelling backwards from \a idx
             * - npos if the code point could not be found
             */
            size_type RFind(utf32 code_point, size_type idx = npos) const {
                if (idx >= d_cpLength)
                    idx = d_cpLength - 1;

                if (d_cpLength > 0) {
                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (*pt-- == code_point)
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            //////////////////////////////////////////////
            // Find a substring
            //////////////////////////////////////////////
            /*!
             * \brief
             * Search forwards for a sub-string
             * \param str
             * EdoString object describing the sub-string to search for
             * \param idx
             * Index of the code point where the search is to start
             * \return
             * - Index of the first occurrence of sub-string \a str travelling forwards from \a idx.
             * - npos if the sub-string could not be found
             */
            size_type Find(const EdoString &str, size_type idx = 0) const {
                if (str.d_cpLength == 0 && idx < d_cpLength)
                    return idx;

                if (idx < d_cpLength) {
                    // Loop while search string could fit in the search area
                    while (d_cpLength - idx >= str.d_cpLength) {
                        if (0 == Compare(idx, str.d_cpLength, str))
                            return idx;

                        ++idx;
                    }
                }

                return npos;
            }

            /*!
             * \brief
             * Search backwards for a sub-string
             * \param str
             * EdoString object describing the sub-string to search for
             * \param idx
             * Index of the code point where the search is to start
             * \return
             * - Index of the first occurrence of sub-string \a str travelling backwards from \a idx
             * - npos if the sub-string could not be found
             */
            size_type RFind(const EdoString &str, size_type idx = npos) const {
                if (str.d_cpLength == 0)
                    return (idx < d_cpLength) ? idx : d_cpLength;

                if (str.d_cpLength <= d_cpLength) {
                    if (idx > d_cpLength - str.d_cpLength)
                        idx = d_cpLength - str.d_cpLength;

                    do {
                        if (0 == Compare(idx, str.d_cpLength, str))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            /*!
             * \brief
             * Search forwards for a sub-string
             * \param std_str
             * std::string object describing the sub-string to search for
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * \param idx
             * Index of the code point where the search is to start
             * \return
             * - Index of the first occurrence of sub-string \a std_str travelling forwards from \a idx
             * - npos if the sub-string could not be found
             */
            size_type Find(const std::string &std_str, size_type idx = 0) const {
                std::string::size_type sze = std_str.size();

                if (sze == 0 && idx < d_cpLength)
                    return idx;

                if (idx < d_cpLength) {
                    // Loop while search string could fit in to the search area
                    while (d_cpLength - idx >= sze) {
                        if (0 == Compare(idx, (size_type) sze, std_str))
                            return idx;

                        ++idx;
                    }
                }

                return npos;
            }

            /*!
             * \brief
             * Search backwards for a sub-string
             * \param std_str
             * std::string object describing the sub-string to search for
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * \param idx
             * Index of the code point where the search is to start
             * \return
             */
            size_type RFind(const std::string &std_str, size_type idx = npos) const {
                std::string::size_type sze = std_str.size();

                if (sze == 0)
                    return (idx < d_cpLength) ? idx : d_cpLength;

                if (sze <= d_cpLength) {
                    if (idx > d_cpLength - sze)
                        idx = d_cpLength - sze;

                    do {
                        if (0 == Compare(idx, (size_type) sze, std_str))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            /*!
             * \brief
             * Search forwards for a sub-string
             * \param utf8_str
             * Buffer containing null-terminated utf8 encoded data describin the sub-string to search for
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the code point where the search is to start
             * \return
             * - Index of the first occurrence of sub-string \a utf_str travelling forwards from \a idx.
             * - npos if the sub-string could not be found
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type Find(const utf8 *utf8_str, size_type idx = 0) const {
                return Find(utf8_str, idx, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Search backwards for a sub-string
             * \param utf8_str
             * Buffer containing null-terminated utf8 encoded data describing the sub-string to search for
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the code point where the search is to start
             * \return
             * - Index of the first occurrence of sub-string \a utf8_str travelling backwards from \a idx
             * - npos if the sub-string could not be found
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type RFind(const utf8 *utf8_str, size_type idx = npos) const {
                return RFind(utf8_str, idx, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Search forwards for a sub-string
             * \param utf8_str
             * Buffer containing utf8 encoded data describing the sub-string to search for
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the code point where the search is to start
             * \param str_len
             * Length of the utf8 encoded sub-string in utf8 code units (not code points)
             * \return
             * - Index of the first occurrence of sub-string \a utf8_str travelling forwards from \a idx
             * - npos if the sub-string could not be found
             * \exception
             * std::length_error Thrown if \a str_len is 'npos'
             */
            size_type Find(const utf8 *utf8_str, size_type idx, size_type str_len) const {
                if (str_len == npos)
                    throw std::length_error("Length of utf8 encoded string can not be 'npos'");

                size_type sze = EncodedSize(utf8_str, str_len);

                if (sze == 0 && idx < d_cpLength)
                    return idx;

                if (idx < d_cpLength) {
                    // Loop while search string could fit in to search area
                    while (d_cpLength - idx >= sze) {
                        if (0 == Compare(idx, sze, utf8_str, sze))
                            return idx;

                        ++idx;
                    }
                }

                return npos;
            }

            /*!
             * \brief
             * Search backwards for a sub-string
             * \param utf8_str
             * Buffer containing utf8 encoded data describing the sub-string to search for
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the code point where the search is to start
             * \param str_len
             * Length of the utf8 encoded sub-string in utf8 code units (not code points)
             * \return
             * - Index of the first occurrence of sub-string \a utf8_str travelling backwards from \a idx
             * - 'npos' if the sub-string could not be found
             * \exception
             * std::length_error Thrown if \a str_len is 'npos'
             */
            size_type RFind(const utf8 *utf8_str, size_type idx, size_type str_len) const {
                if (str_len == npos)
                    throw std::length_error("Length for utf8 encoded string can not be 'npos'");

                size_type sze = EncodedSize(utf8_str, str_len);

                if (sze == 0)
                    return (idx < d_cpLength) ? idx : d_cpLength;

                if (sze <= d_cpLength) {
                    if (idx > d_cpLength - sze)
                        idx = d_cpLength - sze;

                    do {
                        if (0 == Compare(idx, sze, utf8_str, sze))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            /*!
             * \brief
             * Search forwards for a sub-string
             * \param cstr
             * C-string describing the sub-string to search for
             * \param idx
             * Index of the code point where the search is to start
             * \return
             * - Index of the first occurrence of sub-string \a cstr travelling forwards from \a idx
             * - npos if the sub-string could not be found
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type Find(const char *cstr, size_type idx = 0) const {
                return Find(cstr, idx, strlen(cstr));
            }

            /*!
             * \brief
             * Search backwards for a sub-string
             * \param cstr
             * C-string describing the sub-string to search for
             * \param idx
             * Index of the code point where the search is to start
             * \return
             * - Index of the first occurrence of sub-string \a cstr travelling backwards from \a idx
             * - npos if the sub-string could not be found
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type RFind(const char *cstr, size_type idx = npos) const {
                return RFind(cstr, idx, strlen(cstr));
            }

            /*!
             * \brief
             * Search forwards for a sub-string
             * \param chars
             * Char array describing the sub-string to search for
             * \param idx
             * Index of the code point where the search is to start
             * \param chars_len
             * Number of chars in the char array
             * \return
             * - Index of the first occurrence of sub-string \a chars travelling forwards from \a idx
             * - npos if the sub-string could not be found
             * \exception
             * std::length_error Thrown if \a chars_len is 'npos'
             */
            size_type Find(const char *chars, size_type idx, size_type chars_len) const {
                if (chars_len == npos)
                    throw std::length_error("Length for char array can not be 'npos'");

                if (chars_len == 0 && idx < d_cpLength)
                    return idx;

                if (idx < d_cpLength) {
                    // Loop while search string could fit in to search area
                    while (d_cpLength - idx >= chars_len) {
                        if (0 == Compare(idx, chars_len, chars, chars_len))
                            return idx;

                        ++idx;
                    }
                }

                return npos;
            }

            /*!
             * \brief
             * Search backwards for a sub-string
             * \param chars
             * Char array describing the sub-string to search for
             * \param idx
             * Index of the code point where the search is to start
             * \param chars_len
             * Number of chars in the char array.
             * \return
             * - Index of the first occurrence of sub-string \a chars travelling backwards from \a idx
             * - npos if the sub-string could not be found
             */
            size_type RFind(const char *chars, size_type idx, size_type chars_len) const {
                if (chars_len == npos)
                    throw std::length_error("Length for char array can not be 'npos'");

                if (chars_len == 0)
                    return (idx < d_cpLength) ? idx : d_cpLength;

                if (chars_len <= d_cpLength) {
                    if (idx > d_cpLength - chars_len)
                        idx = d_cpLength - chars_len;

                    do {
                        if (0 == Compare(idx, chars_len, chars, chars_len))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            //////////////////////////////////////////////
            // Find first of different code-points
            //////////////////////////////////////////////
            /*!
             * \brief
             * Find the first occurrence of one of a set of code points.
             * \param str
             * EdoString object describing the set of code points
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the first occurrence of any one of the code points in \a str starting from \a idx
             * - npos if none of the code points in \a str were found
             */
            size_type FindFirstOf(const EdoString &str, size_type idx = 0) const {
                if (idx < d_cpLength) {
                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos != str.Find(*pt++))
                            return idx;
                    } while (++idx != d_cpLength);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the first code point that is not one of a set of code points
             * \param str
             * EdoString object describing the set of code points
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the first code point that does not match any one of the code points in \a str starting from \a idx
             * - npos if all code points matched one of the code points in \a str
             */
            size_type FindFirstNotOf(const EdoString &str, size_type idx = 0) const {
                if (idx < d_cpLength) {
                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos == str.Find(*pt++))
                            return idx;
                    } while (++idx != d_cpLength);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the first occurrence of one of a set of code points
             * \param std_str
             * std::string object describing the set of code points.
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the first occurrence of any one of the code points in \a std_str starting from \a idx
             * - npos if none of the code points in \a std_str were found
             */
            size_type FindFirstOf(const std::string &std_str, size_type idx = 0) const {
                if (idx < d_cpLength) {
                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos != FindCodePoint(std_str, *pt++))
                            return idx;
                    } while (++idx != d_cpLength);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the first code point that is not one of a set of code points
             * \param std_str
             * std::string object describing the set of code points
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the first code point that does not match any one of the code points in \a std_str starting from \a idx
             * - npos if all code points matched one of the code points in \a std_str
             */
            size_type FindFirstNotOf(const std::string &std_str, size_type idx = 0) const {
                if (idx < d_cpLength) {
                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos == FindCodePoint(std_str, *pt++))
                            return idx;
                    } while (++idx != d_cpLength);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the first occurrence of one of a set of code points
             * \param utf8_str
             * Buffer containing null-terminated utf8 encoded data describing the set of code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the first occurrence of any one of the code points in \a utf8_str starting from \a idx
             * - npos if none of the code points in \a utf8_str were found
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type FindFirstOf(const utf8 *utf8_str, size_type idx = 0) const {
                return FindFirstOf(utf8_str, idx, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Find the first code point that is not one of a set of code points
             * \param utf8_str
             * Buffer containing null-terminated utf8 encoded data describing the set of code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the first code point that does not match any one of the code points in \a utf8_str starting from \a idx
             * - npos if all code points matched one of the code points in \a utf8_str
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type FindFirstNotOf(const utf8 *utf8_str, size_type idx = 0) const {
                return FindFirstNotOf(utf8_str, idx, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Find the first occurrence of one of a set of code points
             * \param utf8_str
             * Buffer containing utf8 encoded data describing the set of code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the start point for the search
             * \param str_len
             * Length of the utf8 encoded data in utf8 code units (not code points)
             * \return
             * - Index of the first occurrence of any one of the code points in \a utf8_str starting from \a idx
             * - npos if none of the code points in \a utf8_str were found
             * \exception
             * std::length_error Thrown if \a str_len was 'npos'
             */
            size_type FindFirstOf(const utf8 *utf8_str, size_type idx, size_type str_len) const {
                if (str_len == npos)
                    throw std::length_error("Length for utf8 encoded string can not be 'npos'");

                if (idx < d_cpLength) {
                    size_type encSize = EncodedSize(utf8_str, str_len);

                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos != FindCodePoint(utf8_str, encSize, *pt++))
                            return idx;
                    } while (++idx != d_cpLength);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the first code point that is not one of a set of code points
             * \param utf8_str
             * Buffer containing utf8 encoded data describing the set of code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the start point for the search
             * \param str_len
             * Length of the utf8 encoded data in utf8 code units (not code points)
             * \return
             * - Index of the first code point that does not match any one of the code points in \a utf8_str starting from \a idx
             * - npos if all code points matched one of the code points in \a utf8_str
             * \exception
             * std::length_error Thrown if \a str_len was 'npos'
             */
            size_type FindFirstNotOf(const utf8 *utf8_str, size_type idx, size_type str_len) const {
                if (str_len == npos)
                    throw std::length_error("Length for utf8 encoded string can not be 'npos'");

                if (idx < d_cpLength) {
                    size_type encSize = EncodedSize(utf8_str, str_len);

                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos == FindCodePoint(utf8_str, encSize, *pt++))
                            return idx;
                    } while (++idx != d_cpLength);
                }

                return npos;
            }

            /*!
             * \brief
             * Search forwards for a given code point
             * \param code_point
             * The utf32 code point to search for
             * \param idx
             * Index of the code point where the search is to start
             * \return
             * - Index of the first occurrence of \a code_point starting from \a idx
             * - npos if the code point could not be found
             */
            size_type FindFirstOf(utf32 code_point, size_type idx = 0) const {
                return Find(code_point, idx);
            }

            /*!
             * \brief
             * Search forwards for the first code point that does not match a given code point
             * \param code_point
             * The utf32 code point to search for
             * \param idx
             * Index of the code point where the search is to start
             * \return
             * - Index of the first code point that does not match \a code_point starting from \a idx
             * - npos if all code points matched \a code_point
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type FindFirstNotOf(utf32 code_point, size_type idx = 0) const {
                if (idx < d_cpLength) {
                    do {
                        if ((*this)[idx] != code_point)
                            return idx;
                    } while (idx++ < d_cpLength);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the first occurrence of one of a set of chars
             * \param cstr
             * C-string describing the set of chars
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the first occurrence of any one of the code points in \a cstr starting from \a idx
             * - npos if none of the code points in \a cstr were found
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type FindFirstOf(const char *cstr, size_type idx = 0) const {
                return FindFirstOf(cstr, idx, strlen(cstr));
            }

            /*!
             * \brief
             * Find the first code point that is not one of a set of chars
             * \param cstr
             * C-string describing the set of chars
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the first code point that does not match any one of the code points in \a cstr starting from \a idx
             * - npos if all code points matched one of the code points in \a cstr
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type FindFirstNotOf(const char *cstr, size_type idx = 0) const {
                return FindFirstNotOf(cstr, idx, strlen(cstr));
            }

            /*!
             * \brief
             * Find the first occurrence of one of a set of chars
             * \param chars
             * Char array containing the set of chars
             * \param idx
             * Index of the start point for the search
             * \param chars_len
             * Number of chars in the array
             * \return
             * - Index of the first occurrence of any one of the code points in \a chars starting from \a idx
             * - npos if none of the code points in \a chars were found
             * \exception
             * std::length_error Thrown if \a chars_len was 'npos'
             */
            size_type FindFirstOf(const char *chars, size_type idx, size_type chars_len) const {
                if (chars_len == npos)
                    throw std::length_error("Length for char array can not be 'npos'");

                if (idx < d_cpLength) {
                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos != FindCodePoint(chars, chars_len, *pt++))
                            return idx;
                    } while (++idx != d_cpLength);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the first code point that is not one of a set of chars
             * \param chars
             * Char array containing the set of chars
             * \param idx
             * Index of the start point for the search
             * \param chars_len
             * Number of chars in the char array
             * \return
             * - Index of the first code point that does not match any one of the code points in \a chars starting from \a idx
             * - npos if all code points matched one of the code points in \a chars
             * \exception
             * std::length_error Thrown if \a chars_len was 'npos'
             */
            size_type FindFirstNotOf(const char *chars, size_type idx, size_type chars_len) const {
                if (chars_len == npos)
                    throw std::length_error("Length for char array can not be 'npos'");

                if (idx < d_cpLength) {
                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos == FindCodePoint(chars, chars_len, *pt++))
                            return idx;
                    } while (++idx != d_cpLength);
                }

                return npos;
            }

            //////////////////////////////////////////////
            // Find last of different code-points
            //////////////////////////////////////////////
            /*!
             * \brief
             * Find the last occurrence of one of a set of code points
             * \param str
             * EdoString object describing the set of code points
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the last occurrence of any one of the code points in \a str starting from \a idx
             * - npos if none of the code points in \a str were found
             */
            size_type FindLastOf(const EdoString &str, size_type idx = npos) const {
                if (d_cpLength > 0) {
                    if (idx >= d_cpLength)
                        idx = d_cpLength - 1;

                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos != str.Find(*pt--))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the last code point that is not one of a set of code points
             * \param str
             * EdoString object describing the set of code points
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the last code point that does not match any one of the code points in \a str starting from \a idx
             * - npos if all code points matched one of the code points in \a str
             */
            size_type FindLastNotOf(const EdoString &str, size_type idx = npos) const {
                if (d_cpLength < 0) {
                    if (idx >= d_cpLength)
                        idx = d_cpLength - 1;

                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos == str.Find(*pt--))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the last occurrence of one of a set of code points
             * \param std_str
             * std::string object describing the set of code points
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the last occurrence of any one of the code points in \a std_str starting from \a idx
             * - npos if none of the code points in \a std_str were found
             */
            size_type FindLastOf(const std::string &std_str, size_type idx = npos) const {
                if (d_cpLength > 0) {
                    if (idx >= d_cpLength)
                        idx = d_cpLength - 1;

                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos != FindCodePoint(std_str, *pt--))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the last code point that is not one of a set of code points
             * \param std_str
             * std::string object describing the set of code points
             * \note
             * The characters of \a std_str are taken to be unencoded data which represent Unicode code points 0x00..0xFF.
             * No translation of the provided data will occur.
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the last code point that does not match any one of the code points in \a std_str starting from \a idx
             * - npos if all code points matched one of the code points in \a std_str
             */
            size_type FindLastNotOf(const std::string &std_str, size_type idx = npos) const {
                if (d_cpLength > 0) {
                    if (idx >= d_cpLength)
                        idx = d_cpLength - 1;

                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos == FindCodePoint(std_str, *pt--))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the last occurrence of one of a set of code points
             * \param utf8_str
             * Buffer containing null-terminated utf8 encoded data describing the set of code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the last occurrence of any one of the code points in \a utf8_str starting from \a idx
             * - npos if none of the code points in \a utf8_str were found
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type FindLastOf(const utf8 *utf8_str, size_type idx = npos) const {
                return FindLastOf(utf8_str, idx, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Find the last code point that is not one of a set of code points
             * \param utf8_str
             * Buffer containing null-terminated utf8 encoded data describing the set of code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the last code point that does not match any one of the code points in \a utf8_str starting from \a idx
             * - npos if all code points matched one of the code points in \a utf8_str
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type FindLastNotOf(const utf8 *utf8_str, size_type idx = npos) const {
                return FindLastNotOf(utf8_str, idx, UtfLength(utf8_str));
            }

            /*!
             * \brief
             * Find the last occurrence of one of a set of code points
             * \param utf8_str
             * Buffer containing utf8 encoded data describing the set of code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the start point for the search
             * \param str_len
             * Length of the utf8 encoded data in utf8 code units (not code points)
             * \return
             * - Index of the last occurrence of any one of the code points in \a utf8_str starting from \a idx
             * - npos if none of the code points in \a utf8_str were found
             * \exception
             * std::length_error Thrown if \a str_len was 'npos'
             */
            size_type FindLastOf(const utf8 *utf8_str, size_type idx, size_type str_len) const {
                if (str_len == npos)
                    throw std::length_error("Length for utf8 encoded string can not be 'npos'");

                if (d_cpLength > 0) {
                    if (idx >= d_cpLength)
                        idx = d_cpLength - 1;

                    size_type encSize = EncodedSize(utf8_str, str_len);

                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos != FindCodePoint(utf8_str, encSize, *pt--))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the last code point that is not one of a set of code points
             * \param utf8_str
             * Buffer containing utf8 encoded data describing the set of code points
             * \note
             * A basic string literal (cast to utf8*) can be passed to this function, provided that the string is
             * comprised only of code points 0x00 - 0x7F. The use of extended ASCII characters (with values >0x7F)
             * would result in incorrect behaviour as the EdoString will attempt to 'decode' the data, with
             * unpredictable results.
             * \param idx
             * Index of the start point for the search
             * \param str_len
             * Length of the utf8 encoded data in utf8 code units (not code points)
             * \return
             * - Index of the last code point that does not match any one of the code points in \a utf8_str starting from \a idx
             * - npos if all code points matched one of the code points in \a utf8_str
             * \exception
             * std::length_error Thrown if \a str_len was 'npos'
             */
            size_type FindLastNotOf(const utf8 *utf8_str, size_type idx, size_type str_len) const {
                if (str_len == npos)
                    throw std::length_error("Length for utf8 encoded string can not be 'npos'");

                if (d_cpLength > 0) {
                    if (idx >= d_cpLength)
                        idx = d_cpLength - 1;

                    size_type encSize = EncodedSize(utf8_str, str_len);

                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos == FindCodePoint(utf8_str, encSize, *pt--))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            /*!
             * \brief
             * Search for last occurrence of a given code point
             * \param code_point
             * The utf32 code point to search for
             * \param idx
             * Index of the code point where the search is to start
             * \return
             * - Index of the last occurrence of \a code_point starting from \a idx
             * - npos if the code point could not be found
             */
            size_type FindLastOf(utf32 code_point, size_type idx = npos) const {
                return RFind(code_point, idx);
            }

            /*!
             * \brief
             * Search for the last code point that does not match a given code point
             * \param code_point
             * The utf32 code point to search for
             * \param idx
             * Index of the code point where the search is to start
             * \return
             * - Index of the last code point that does not match \a code_point starting from \a idx
             * - npos if all code points matched \a code_point
             */
            size_type FindLastNotOf(utf32 code_point, size_type idx = npos) const {
                if (d_cpLength > 0) {
                    if (idx >= d_cpLength)
                        idx = d_cpLength - 1;

                    do {
                        if ((*this)[idx] != code_point)
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the last occurrence of one of a set of chars
             * \param cstr
             * C-string describing the set of chars
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the last occurrence of any one of the code points in \a cstr starting from \a idx
             * - npos if none of the code points in \a cstr were found
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            size_type FindLastOf(const char *cstr, size_type idx = npos) const {
                return FindLastOf(cstr, idx, strlen(cstr));
            }

            /*!
             * \brief
             * Find the last code point that is not one of a set of chars
             * \param cstr
             * C-string describing the set of chars
             * \param idx
             * Index of the start point for the search
             * \return
             * - Index of the last code point that does not match any one of the code points in \a cstr starting from \a idx
             * - npos if all code points matched one of the code points in \a cstr
             */
            size_type FindLastNotOf(const char *cstr, size_type idx = npos) const {
                return FindLastNotOf(cstr, idx, strlen(cstr));
            }

            /*!
             * \brief
             * Find the last occurrence of one of a set of chars
             * \param chars
             * Char array containing the set of chars
             * \param idx
             * Index of the start point for the search
             * \param chars_len
             * Number of chars in the char array
             * \return
             * - Index of the last occurrence of any one of the code points in \a chars starting from \a idx
             * - npos if none of the code points in \a chars were found
             * \exception
             * std::length_error Thrown if \a chars_len was 'npos'
             */
            size_type FindLastOf(const char *chars, size_type idx, size_type chars_len) const {
                if (chars_len == npos)
                    throw std::length_error("Length for char array can not be 'npos'");

                if (d_cpLength > 0) {
                    if (idx >= d_cpLength)
                        idx = d_cpLength - 1;

                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos != FindCodePoint(chars, chars_len, *pt--))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            /*!
             * \brief
             * Find the last code point that is not one of a set of chars
             * \param chars
             * char array containing the set of chars
             * \param idx
             * Index of the start point for the search
             * \param chars_len
             * Number of chars in the char array
             * \return
             * - Index of the last code point that does not match any one of the code points in \a chars starting from \a idx
             * - npos if all code points matched one of the code points in \a chars
             * \exception
             * std::length_error Thrown if \a chars_len was 'npos'
             */
            size_type FindLastNotOf(const char *chars, size_type idx, size_type chars_len) const {
                if (chars_len == npos)
                    throw std::length_error("Length for char array can not be 'npos'");

                if (d_cpLength > 0) {
                    if (idx >= d_cpLength)
                        idx = d_cpLength - 1;

                    const utf32 *pt = &ptr()[idx];

                    do {
                        if (npos == FindCodePoint(chars, chars_len, *pt--))
                            return idx;
                    } while (idx-- != 0);
                }

                return npos;
            }

            //////////////////////////////////////////////
            // Substring
            //////////////////////////////////////////////
            /*!
             * \brief
             * Returns a substring of this EdoString
             * \param idx
             * Index of the first code point for the sub-string
             * \param len
             * Maximum number of code points to use for the sub-string
             * \return
             * An EdoString object containing the specified sub-string
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for this EdoString
             */
            EdoString Substr(size_type idx = 0, size_type len = npos) const {
                if (d_cpLength < idx)
                    throw std::out_of_range("Index is out of range for this EdoString");

                return EdoString(*this, idx, len);
            }

            //////////////////////////////////////////////
            // Iterator creation
            //////////////////////////////////////////////
            /*!
             * \brief
             * Return a forward iterator that describes the beginning of the EdoString
             * \return
             * Iterator object that describes the beginning of the EdoString
             */
            iterator Begin() {
                return iterator(ptr());
            }

            /*!
             * \brief
             * Return a constant forward iterator that describes the beginning of the EdoString
             * \return
             * const_iterator object that describes the beginning of the EdoString
             */
            const_iterator Begin() const {
                return const_iterator(ptr());
            }

            /*!
             * \brief
             * Return a forward iterator that describes the end of the EdoString
             * \return
             * iterator object that describes the end of the EdoString
             */
            iterator End() {
                return iterator(&ptr()[d_cpLength]);
            }

            /*!
             * \brief
             * Return a constant forward iterator that describes the end of the EdoString
             * \return
             * const_iterator object that describes the end of the EdoString
             */
            const_iterator End() const {
                return const_iterator(&ptr()[d_cpLength]);
            }

            /*!
             * \brief
             * Return a reverse iterator that describes the beginning of the EdoString
             * \return
             * reverse_iterator object that describes the beginning of the EdoString (actually at the end)
             */
            reverse_iterator RBegin() {
                return reverse_iterator(End());
            }

            /*!
             * \brief
             * Return a constant reverse iterator that describes the beginning of the EdoString
             * \return
             * const_reverse_iterator object that describes the beginning of the EdoString (actually at the end)
             */
            const_reverse_iterator RBegin() const {
                return const_reverse_iterator(End());
            }

            /*!
             * \brief
             * Return a reverse iterator that describes the end of the EdoString
             * \return
             * reverse_iterator object that describes the end of the EdoString (actually at the beginning)
             */
            reverse_iterator REnd() {
                return reverse_iterator(Begin());
            }

            /*!
             * \brief
             * Return a constant reverse iterator that describes the end of the EdoString
             * \return
             * const_reverse_iterator object that describes the end of the EdoString (actually at the beginning)
             */
            const_reverse_iterator REnd() const {
                return const_reverse_iterator(Begin());
            }

            // CUSTOM UTF16 CODE (not present in CEGUI::String)
            // TODO: There are some missing operators and functions probably
            //////////////////////////////////////////////
            // Construction via std::wstring
            //////////////////////////////////////////////
            /*!
             * \brief
             * Constructs a new EdoString and initializes it using the std::wstring \a w_str
             * \param w_str
             * The std::wstring object that is to be used to initialize the new EdoString object
             * \note
             * The characters of \a w_str are taken to be unencoded data which represent Unicode code points 0x0000..0xFFFF.
             * No translation of the provided data will occur
             * \exception
             * std::length_error Thrown if the resulting EdoString would be too large
             */
            EdoString(const std::wstring &w_str) {
                Init();
                Assign(w_str);
            }

            EdoString(const wchar_t *w_chars) {
                Init();
                Assign(w_chars);
            }

            /*!
             * \brief
             * Assign a substring of std::wstring \a w_str to this EdoString
             * \note
             * The characters of \a w_str are taken to be unencoded data which represent Unicode code points 0x0000..0xFFFF.
             * No translation of the provided data will occur
             * \param w_str
             * std::wstring object containing the string value to be assigned
             * \param idx
             * Index of the first character of \a w_str to be assigned
             * \param num
             * Maximum number of characters from \a std_str to be assigned
             * \return
             * This EdoString after the assignment has happened
             * \exception
             * std::out_of_range Thrown if \a idx is invalid for \a w_str
             * \exception
             * std::length_error Thrown if the resulting EdoString would have been too large
             */
            EdoString &Assign(const std::wstring &w_str, size_type idx = 0, size_type num = npos) {
                if (w_str.size() < idx)
                    throw std::out_of_range("Index was out of range for std::wstring object");

                if (num == npos || num > (size_type) w_str.size() - idx)
                    num = (size_type) w_str.size() - idx;

                Grow(num);
                SetLen(num);

                while (num--)
                    (*this)[num] = static_cast<utf32>(w_str[num + idx]);

                return *this;
            }

            /*!
             * \brief
             * Assign to this EdoString the given wide c-string
             * \param chars
             * Pointer to a valid C style wide string
             * \return
             * This EdoString after the assignment has happened
             * \exception
             * std::length_error Thrown if the resulting EdoString would have been too large
             */
            EdoString &Assign(const wchar_t *chars) {
                size_t num = wcslen(chars);

                Grow(num);
                SetLen(num);

                while (num--)
                    (*this)[num] = static_cast<utf32>(chars[num]);

                return *this;
            }

            /*!
             * \brief
             * Convert this EdoString to a UTF-16 encoded std::wstring
             * \param strictConversion
             * If true, illegal characters will throw an error, otherwise they are replaced
             * \return
             * UTF-16 representation of this EdoString
             * \exception
             * std::invalid_argument Thrown if a character in the conversion is invalid
             */
            std::wstring ToUtf16(bool strictConversion = true) const {
                std::wstring target;

                const utf32 *sourceStart = ptr();
                const utf32 *sourceEnd = ptr() + Size();
                const utf32 *source = sourceStart;

                while (source < sourceEnd) {
                    utf32 ch = *source++;

                    // Target is a character <= 0xFFF
                    if (ch <= 0x0000FFFF) {
                        // UTF-16 surrogate values are illegal in UTF32. 0xFFFF or 0xFFFE are both reserved values
                        if (ch >= 0xD800 && ch <= 0xDFFF) {
                            if (strictConversion) {
                                --source; // Return to illegal value itself
                                throw std::invalid_argument("Illegal conversion between UTF32 and UTF16");
                            } else {
                                target.append(1, 0x0000FFFD); // Used as a replacement char
                            }
                        } else {
                            target.append(1, (utf16) ch); // Normal case
                        }
                    } else if (ch > 0x0010FFFF) { // Max legal UTF-32 character
                        if (strictConversion) {
                            throw std::invalid_argument("Illegal conversion between UTF32 and UTF16");
                        } else {
                            target.append(1, 0x0000FFFD);
                        }
                    } else {
                        // Target is a character in range 0xFFFF - 0x10FFFF
                        ch -= 0x0010000UL; // Half base
                        target.append(1, (utf16) ((ch >> 10) + 0xD800));
                        target.append(1, (utf16) ((ch >> 0x3FFUL) + 0xDC00));
                    }
                }

                return target;
            }

        private:
            //////////////////////////////////////////////
            // Implementation Functions
            //////////////////////////////////////////////
            // String management

            // Change size of allocated buffer so it is at least 'new_size'
            // May or may not cause re-allocation and copy of buffer if size is larger
            // will never re-allocate to make size smaller. (see Trim())
            bool Grow(size_type new_size);

            // Perform re-allocation to remove wasted space.
            void Trim();

            // Set the length of the string, and terminate it, according to the given value (will not re-allocate, use Grow() first).
            void SetLen(size_type len) {
                d_cpLength = len;
                ptr()[len] = (utf32) 0;
            }

            // Initialize EdoString object
            void Init() {
                d_reserve = STR_QUICKBUFF_SIZE;
                d_encodedBuff = nullptr;
                d_encodedBuffLen = 0;
                d_encodedDatLen = 0;
                d_buffer = nullptr;
                SetLen(0);
            }

            // Return true if the given pointer is inside the string data
            bool Inside(utf32 *in_ptr) {
                if (in_ptr < ptr() || ptr() + d_cpLength <= in_ptr)
                    return false;
                else
                    return true;
            }

            // Compute distance between two iterators, returning a 'safe' value
            size_type safe_iter_dif(const const_iterator &iter1, const const_iterator &iter2) const {
                return (iter1.d_ptr == 0) ? 0 : (iter1 - iter2);
            }

            // Encoding functions
            // for all:
            //  src_len is in code units, or 0 for null terminating string.
            //  dst_len is in code units
            //  returns number of code units put into dest buffer
            size_type Encode(const utf32 *src, utf8 *dest, size_type dest_len, size_type src_len = 0) const {
                // Count length for null terminated source...
                if (src_len == 0)
                    src_len = UtfLength(src);

                size_type destCapacity = dest_len;

                // While there is data in the source buffer,
                for (unsigned int idx = 0; idx < src_len; ++idx) {
                    utf32 cp = src[idx];

                    // Check if there is enough destination buffer to receive this encoded unit (exit loop & return if there is not)
                    if (destCapacity < EncodedSize(cp))
                        break;

                    if (cp < 0x80) {
                        *dest++ = (utf8) cp;
                        --destCapacity;
                    } else if (cp < 0x0800) {
                        *dest++ = (utf8) ((cp >> 6) | 0xC0);
                        *dest++ = (utf8) ((cp & 0x3F) | 0x80);
                        destCapacity -= 2;
                    } else if (cp < 0x10000) {
                        *dest++ = (utf8) ((cp >> 12) | 0xE0);
                        *dest++ = (utf8) (((cp >> 6) & 0x3F) | 0x80);
                        *dest++ = (utf8) ((cp & 0x3F) | 0x80);
                        destCapacity -= 3;
                    } else {
                        *dest++ = (utf8) ((cp >> 18) | 0xF0);
                        *dest++ = (utf8) (((cp >> 12) & 0x3F) | 0x80);
                        *dest++ = (utf8) (((cp >> 6) & 0x3F) | 0x80);
                        *dest++ = (utf8) ((cp & 0x3F) | 0x80);
                        destCapacity -= 4;
                    }
                }

                return dest_len - destCapacity;
            }

            size_type Encode(const utf8 *src, utf32 *dest, size_type dest_len, size_type src_len = 0) const {
                // Count length for null terminated source
                if (src_len == 0)
                    src_len = UtfLength(src);

                size_type destCapacity = dest_len;

                // While there is data in the source buffer
                for (unsigned int idx = 0; (idx < src_len && destCapacity > 0);) {
                    utf32 cp;
                    utf8 cu = src[idx++];

                    if (cu < 0x80)
                        cp = (utf32) cu;
                    else if (cu < 0xE0) {
                        cp = ((cu & 0x1F) << 6);
                        cp |= (src[idx++] & 0x3F);
                    } else if (cu < 0xF0) {
                        cp = ((cu & 0x0F) << 12);
                        cp |= ((src[idx++] & 0x3F) << 6);
                        cp |= (src[idx++] & 0x3F);
                    } else {
                        cp = ((cu & 0x07) << 18);
                        cp |= ((src[idx++] & 0x3F) << 12);
                        cp |= ((src[idx++] & 0x3F) << 6);
                        cp |= (src[idx++] & 0x3F);
                    }

                    *dest++ = cp;
                    --destCapacity;
                }

                return dest_len - destCapacity;
            }

            // Return the number of utf8 code units required to encode the given utf32 code point
            size_type EncodedSize(utf32 code_point) const {
                if (code_point < 0x80)
                    return 1;
                else if (code_point < 0x0800)
                    return 2;
                else if (code_point < 0x10000)
                    return 3;
                else
                    return 4;
            }

            // Return the number of code units required to re-encode given utf32 data as utf8.
            // Return does not include terminating null
            size_type EncodedSize(const utf32 *buf) const {
                return EncodedSize(buf, UtfLength(buf));
            }

            // Return number of code units required to re-encode given utf32 data as utf8.
            // Len is number of code units in 'buf'
            size_type EncodedSize(const utf32 *buf, size_type len) const {
                size_type count = 0;

                while (len--)
                    count += EncodedSize(*buf++);

                return count;
            }

            // Return number of utf32 code units required to re-encode given utf8 data as utf32.
            // Return does not include terminating null
            size_type EncodedSize(const utf8 *buf) const {
                return EncodedSize(buf, UtfLength(buf));
            }

            // Return number of utf32 code units required to re-encode given utf8 data as utf32.
            // Len is number of code units in 'buf'
            size_type EncodedSize(const utf8 *buf, size_type len) const {
                utf8 tcp;
                size_type count = 0;

                while (len--) {
                    tcp = *buf++;
                    ++count;
                    size_type size = 0;

                    if (tcp < 0x80) {}
                    else if (tcp < 0xE0) {
                        size = 1;
                        ++buf;
                    } else if (tcp < 0xF0) {
                        size = 2;
                        buf += 2;
                    } else {
                        size = 3;
                        buf += 3;
                    }

                    if (len >= size)
                        len -= size;
                    else
                        break;
                }

                return count;
            }

            // Return number of code units in a null terminated string
            size_type UtfLength(const utf8 *utf8_str) const {
                size_type cnt = 0;

                while (*utf8_str++)
                    cnt++;

                return cnt;
            }

            // Return number of code units in a null terminated string
            size_type UtfLength(const utf32 *utf32_str) const {
                size_type cnt = 0;

                while (*utf32_str++)
                    cnt++;

                return cnt;
            }

            // Build an internal buffer with the string encoded as utf8 (remains valid until string is modified)
            utf8 *BuildUtf8Buff() const;

            // Compare two utf32 buffers
            int Utf32CompUtf32(const utf32 *buf1, const utf32 *buf2, size_type cp_count) const {
                if (!cp_count)
                    return 0;

                while (--cp_count && *buf1 == *buf2)
                    buf1++, buf2++;

                return *buf1 - *buf2;
            }

            // Compare utf32 buffer with char buffer (chars are taken to be code-points in the range 0x00 - 0xFF)
            int Utf32CompChar(const utf32 *buf1, const char *buf2, size_type cp_count) const {
                if (!cp_count)
                    return 0;

                while (--cp_count && *buf1 == static_cast<utf32>(static_cast<unsigned char>(*buf2)))
                    *buf1++, *buf2++;

                return *buf1 - static_cast<utf32>(static_cast<unsigned char>(*buf2));
            }

            // Compare utf32 buffer with encoded utf8 data
            int Utf32CompUtf8(const utf32 *buf1, const utf8 *buf2, size_type cp_count) const {
                if (!cp_count)
                    return 0;

                utf32 cp;
                utf8 cu;

                do {
                    cu = *buf2++;

                    if (cu < 0x80) {
                        cp = (utf32) cu;
                    } else if (cu < 0xE0) {
                        cp = (cu & 0x1F) << 6;
                        cp |= *buf2++ & 0x3F;
                    } else if (cu < 0xF0) {
                        cp = (cu & 0x0F) << 12;
                        cp |= (*buf2++ & 0x3F) << 6;
                        cp |= *buf2++ & 0x3F;
                    } else {
                        cp = (cu & 0x07) << 18;
                        cp |= (*buf2++ & 0x3F) << 12;
                        cp |= (*buf2++ & 0x3F) << 6;
                        cp |= *buf2++ & 0x3F;
                    }
                } while (*buf1++ == cp && --cp_count);

                return *--buf1 - cp;
            }

            // Return index of first occurrence of 'code_point' in std::string 'str', or npos if none
            size_type FindCodePoint(const std::string &str, utf32 code_point) const {
                size_type idx = 0, sze = (size_type) str.size();

                while (idx != sze) {
                    if (code_point == static_cast<utf32>(static_cast<unsigned char>(str[idx])))
                        return idx;

                    ++idx;
                }

                return npos;
            }

            // Return index of first occurrence of 'code_point' in utf8 encoded string 'str', or npos if none.
            // Len is in code points
            size_type FindCodePoint(const utf8 *str, size_type len, utf32 code_point) const {
                size_type idx = 0;

                utf32 cp;
                utf8 cu;

                while (idx != len) {
                    cu = *str++;

                    if (cu < 0x80) {
                        cp = (utf32) cu;
                    } else if (cu < 0xE0) {
                        cp = (cu & 0x1F) << 6;
                        cp |= *str++ & 0x3F;
                    } else if (cu < 0xF0) {
                        cp = (cu & 0x0F) << 12;
                        cp |= (*str++ & 0x3F) << 6;
                        cp |= *str++ & 0x3F;
                    } else {
                        cp = (cu & 0x07) << 18;
                        cp |= (*str++ & 0x3F) << 12;
                        cp |= (*str++ & 0x3F) << 6;
                        cp |= *str++ & 0x3F;
                    }

                    if (code_point == cp)
                        return idx;

                    ++idx;
                }

                return npos;
            }

            // Return index of first occurrence of 'code_point' in char array 'chars', or npos if none
            size_type FindCodePoint(const char *chars, size_type chars_len, utf32 code_point) const {
                for (size_type idx = 0; idx != chars_len; ++idx) {
                    if (code_point == static_cast<utf32>(static_cast<unsigned char>(chars[idx])))
                        return idx;
                }

                return npos;
            }
        };

        //////////////////////////////////////////////
        // Comparison operators
        //////////////////////////////////////////////
        /*!
         * \brief
         * Return true if EdoString \a str1 is equal to \a EdoString str2
         */
        bool EDO_API operator==(const EdoString &str1, const EdoString &str2);

        /*!
         * \brief
         * Return true if EdoString \a str is equal to std::string \a std_str
         */
        bool EDO_API operator==(const EdoString &str, const std::string &std_str);

        /*!
         * \brief
         * Return true if EdoString \a str is equal to std::string \a std_str
         */
        bool EDO_API operator==(const std::string &std_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is equal to null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator==(const EdoString &str, const utf8 *utf8_str);

        /*!
         * \brief
         * Return true if EdoString \a str is equal to null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator==(const utf8 *utf8_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str1 is not equal to \a EdoString str2
         */
        bool EDO_API operator!=(const EdoString &str1, const EdoString &str2);

        /*!
         * \brief
         * Return true if EdoString \a str is not equal to std::string \a std_str
         */
        bool EDO_API operator!=(const EdoString &str, const std::string &std_str);

        /*!
         * \brief
         * Return true if EdoString \a str is not equal to std::string \a std_str
         */
        bool EDO_API operator!=(const std::string &std_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is not equal to null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator!=(const EdoString &str, const utf8 *utf8_str);

        /*!
         * \brief
         * Return true if EdoString \a str is not equal to null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator!=(const utf8 *utf8_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str1 is lexicographically less than \a EdoString str2
         */
        bool EDO_API operator<(const EdoString &str1, const EdoString &str2);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than std::string \a std_str
         */
        bool EDO_API operator<(const EdoString &str, const std::string &std_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than std::string \a std_str
         */
        bool EDO_API operator<(const std::string &std_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator<(const EdoString &str, const utf8 *utf8_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator<(const utf8 *utf8_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str1 is lexicographically greater than \a EdoString str2
         */
        bool EDO_API operator>(const EdoString &str1, const EdoString &str2);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than std::string \a std_str
         */
        bool EDO_API operator>(const EdoString &str, const std::string &std_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than std::string \a std_str
         */
        bool EDO_API operator>(const std::string &std_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator>(const EdoString &str, const utf8 *utf8_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator>(const utf8 *utf8_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str1 is lexicographically less than or equal to \a EdoString str2
         */
        bool EDO_API operator<=(const EdoString &str1, const EdoString &str2);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than or equal to std::string \a std_str
         */
        bool EDO_API operator<=(const EdoString &str, const std::string &std_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than or equal to std::string \a std_str
         */
        bool EDO_API operator<=(const std::string &std_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than or equal to null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator<=(const EdoString &str, const utf8 *utf8_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than or equal to null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator<=(const utf8 *utf8_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str1 is lexicographically greater than or equal to \a EdoString str2
         */
        bool EDO_API operator>=(const EdoString &str1, const EdoString &str2);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than or equal to std::string \a std_str
         */
        bool EDO_API operator>=(const EdoString &str, const std::string &std_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than or equal to std::string \a std_str
         */
        bool EDO_API operator>=(const std::string &std_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than or equal to null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator>=(const EdoString &str, const utf8 *utf8_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than or equal to null-terminated utf8 data \a utf8_str
         */
        bool EDO_API operator>=(const utf8 *utf8_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is equal to c-string \a c_str
         */
        bool EDO_API operator==(const EdoString &str, const char *c_str);

        /*!
         * \brief
         * Return true if EdoString \a str is equal to c-string \a c_str
         */
        bool EDO_API operator==(const char *c_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is not equal to c-string \a c_str
         */
        bool EDO_API operator!=(const EdoString &str, const char *c_str);

        /*!
         * \brief
         * Return true if EdoString \a str is not equal to c-string \a c_str
         */
        bool EDO_API operator!=(const char *c_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than c-string \a c_str
         */
        bool EDO_API operator<(const EdoString &str, const char *c_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than c-string \a c_str
         */
        bool EDO_API operator<(const char *c_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than c-string \a c_str
         */
        bool EDO_API operator>(const EdoString &str, const char *c_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than c-string \a c_str
         */
        bool EDO_API operator>(const char *c_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than or equal to c-string \a c_str
         */
        bool EDO_API operator<=(const EdoString &str, const char *c_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically less than or equal to c-string \a c_str
         */
        bool EDO_API operator<=(const char *c_str, const EdoString &str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than or equal to c-string \a c_str
         */
        bool EDO_API operator>=(const EdoString &str, const char *c_str);

        /*!
         * \brief
         * Return true if EdoString \a str is lexicographically greater than or equal to c-string \a c_str
         */
        bool EDO_API operator>=(const char *c_str, const EdoString &str);

        //////////////////////////////////////////////
        // Concatenation operator functions
        //////////////////////////////////////////////
        /*!
         * \brief
         * Return EdoString object that is the concatenation of the given inputs
         * \param str1
         * EdoString object describing first part of the new string
         * \param str2
         * EdoString object describing the second part of the new string
         * \return
         * An EdoString object that is the concatenation of \a str1 and \a str2
         * \exception
         * std::length_error Thrown if the resulting EdoString would be too large
         */
        EdoString EDO_API operator+(const EdoString &str1, const EdoString &str2);

        /*!
         * \brief
         * Return EdoString object that is the concatenation of the given inputs
         * \param str
         * EdoString object describing first part of the new string
         * \param std_str
         * std::string object describing the second part of the new string
         * \return
         * An EdoString object that is the concatenation of \a str and \a std_str
         * \exception
         * std::length_error Thrown if the resulting EdoString would be too large
         */
        EdoString EDO_API operator+(const EdoString &str, const std::string &std_str);

        /*!
         * \brief
         * Return EdoString object that is the concatenation of the given inputs
         * \param std_str
         * std::string object describing first part of the new string
         * \param str
         * EdoString object describing the second part of the new string
         * \return
         * An EdoString object that is the concatenation of \a std_str and \a str
         * \exception
         * std::length_error Thrown if the resulting EdoString would be too large
         */
        EdoString EDO_API operator+(const std::string &std_str, const EdoString &str);

        /*!
         * \brief
         * Return EdoString object that is the concatenation of the given inputs
         * \param str
         * EdoString object describing first part of the new string
         * \param utf8_str
         * Buffer containing null-terminated utf8 encoded data describing the second part of the new string
         * \return
         * An EdoString object that is the concatenation of \a str and \a utf8_str
         * \exception
         * std::length_error Thrown if the resulting EdoString would be too large
         */
        EdoString EDO_API operator+(const EdoString &str, const utf8 *utf8_str);

        /*!
         * \brief
         * Return EdoString object that is the concatenation of the given inputs
         * \param utf8_str
         * Buffer containing null-terminated utf8 encoded data describing first part of the new string
         * \param str
         * EdoString object describing the second part of the new string
         * \return
         * An EdoString object that is the concatenation of \a utf8_str and \a str
         * \exception
         * std::length_error Thrown if the resulting EdoString would be too large
         */
        EdoString EDO_API operator+(const utf8 *utf8_str, const EdoString &str);

        /*!
         * \brief
         * Return EdoString object that is the concatenation of the given inputs
         * \param str
         * EdoString object describing first part of the new string
         * \param code_point
         * Utf32 code point describing the second part of the new string
         * \return
         * An EdoString object that is the concatenation of \a str and \a code_point
         * \exception
         * std::length_error Thrown if the resulting EdoString would be too large
         */
        EdoString EDO_API operator+(const EdoString &str, const utf32 code_point);

        /*!
         * \brief
         * Return EdoString object that is the concatenation of the given inputs
         * \param code_point
         * Utf32 code point describing first part of the new string
         * \param str
         * EdoString object describing the second part of the new string
         * \return
         * An EdoString object that is the concatenation of \a code_point and \a str
         * \exception
         * std::length_error Thrown if the resulting EdoString would be too large
         */
        EdoString EDO_API operator+(const utf32 code_point, const EdoString &str);

        /*!
         * \brief
         * Return EdoString object that is the concatenation of the given inputs
         * \param str
         * EdoString object describing first part of the new string
         * \param c_str
         * C-string describing the second part of the new string
         * \return
         * An EdoString object that is the concatenation of \a str and \a c_str
         * \exception
         * std::length_error Thrown if the resulting EdoString would be too large
         */
        EdoString EDO_API operator+(const EdoString &str, const char *c_str);

        /*!
         * \brief
         * Return EdoString object that is the concatenation of the given inputs
         * \param c_str
         * C-string describing first part of the new string
         * \param str
         * EdoString object describing the second part of the new string
         * \return
         * An EdoString object that is the concatenation of \a c_str and \a str
         * \exception
         * std::length_error Thrown if the resulting EdoString would be too large
         */
        EdoString EDO_API operator+(const char *c_str, const EdoString &str);

        //////////////////////////////////////////////
        // Output (stream) functions
        //////////////////////////////////////////////
        EDO_API std::ostream &operator<<(std::ostream &s, const EdoString &str);

        //////////////////////////////////////////////
        // Modifying operations
        //////////////////////////////////////////////
        /*!
         * \brief
         * Swap the contents of two EdoString objects
         * \param str1
         * EdoString object who's contents are to be swapped with \a str2
         * \param str2
         * EdoString object who's contents are to be swapped with \a str1
         */
        void EDO_API Swap(EdoString &str1, EdoString &str2);

        /*!
         * \brief
         * Functor that can be used as comparator in std::map with EdoString keys. It's faster than using the default,
         * but the map will no longer be sorted alphabetically
         */
        struct EdoStringFastLessCompare {
            bool operator()(const EdoString &a, const EdoString &b) const {
                const size_t la = a.Length();
                const size_t lb = b.Length();
                if (la == lb)
                    return memcmp(a.ptr(), b.ptr(), la * sizeof(utf32)) < 0;

                return (la < lb);
            }
        };
    } // Namespace Types
} // Namespace Edo

#endif //EDOCORE_EDOSTRING_H
