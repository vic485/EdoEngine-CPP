// =============================================================================
// EdoIncludes.h
// Base includes required throughout the engine and external apps
//
// Created by Victor on 2019/07/09.
// =============================================================================

#ifndef EDOCORE_EDOINCLUDES_H
#define EDOCORE_EDOINCLUDES_H

// C system headers
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// Stl headers
#include <string>

using std::string; // ascii string
using std::wstring; // wide string

#include <vector>

using std::vector;

#include <list>

using std::list;

#include <map>

using std::map;

#include <set>
//using std::set;

#include <ios>
#include <iostream>
#include <sstream>

using std::cout; // ascii std streams
using std::cin;
using std::cerr;
using std::wcout; // wide std streams
using std::wcin;
using std::wcerr;

using std::endl;

using std::istream; // ascii streams
using std::ostream;
using std::ostringstream;

using std::wistream; // wide streams
using std::wostream;
using std::wostringstream;

#include <fstream>

using std::fstream; // ascii file streams
using std::ifstream;
using std::ofstream;

using std::wfstream; // wide file streams
using std::wifstream;
using std::wofstream;

#include <stack>

using std::stack;

#include <algorithm>

using std::min;
using std::max;
using std::transform;

#include <cctype>

using std::tolower;
using std::toupper;

#include <utility>

using std::pair;

#include <exception>
#include <iomanip>
#include <iterator>

#include <float.h>
#include <limits>

using std::numeric_limits;

#include <valarray>

/*!
 * \defgroup CompileTimeAssertions Compile-time assertions
 *
 * Compile-time assertions are useful to check if a specific condition is satisfied during compile (not run-!)time. This can be
 * handy in some cases.
 *
 * \example_snippet_start
 * struct Assertion_Example
 * {
 *     struct Base
 *     {};
 *
 *     struct Derived : Base
 *     {};
 *
 *     struct NotDerived
 *     {};
 *
 * 	   void test()
 * 	   {
 * 		   EdoCompileTimeAssert(2 > 1, Huh)								// should not be triggered
 * 		   EdoCompileTimeAssert_IsKindOf(Base, Base);					// should not be triggered
 * 		   EdoCompileTimeAssert_IsKindOf(Derived, Base);				// should not be triggered
 * 		   EdoCompileTimeAssert_IsKindOf(NotDerived, Base);				// should give error
 * 		   EdoCompileTimeAssert(1 > 2, OneIsNotSmallerThanTwo);			// should give error
 * 	   }
 * };
 * @example_snippet_end
 */

/*!
 * \brief
 * Compile-time assertions base template
 * \tparam B
 * \ingroup CompileTimeAssertions
 */
template<bool B>
struct EdoCompileTimeAssert {
};

/*!
 * \brief
 * Compile-time assertions template specialization for succeeded check
 * \ingroup CompileTimeAssertions
 */
template<>
struct EdoCompileTimeAssert<true> {
    static void DoAssert() {}
};

/*!
 * \brief
 * Compile-time assertion main macro
 * Use this one to check for a specific condition
 * \note
 * Currently works only if not used at global level or inside a class. We could get rid of that restriction if we
 * removed the {} around the EdoCompileTimeAssertMacro, but then we would actually introduce the typedef into the scope.
 * We don't want that. Maybe we should find a better way to do compile-time assertions.
 * \ingroup CompileTimeAssertions
 * \param x
 * The condition which should evaluate to some bool
 * \param errormsg
 * The error message given when the assertion fails
 */
#define EdoCompileTimeAssert(x, errormsg) { const bool condition = (x); EdoCompileTimeAssert<condition>::DoAssert(); }

/*!
 * \brief
 * Compile-time assertion which tests whether targ (a type) derives from baseclass (or is of type baseclass)
 * Use this one to check for a specific condition
 * \param targ
 * The class to check (derived class)
 * \param baseclass
 * The class to check agains (base class)
 * \ingroup CompileTimeAssertions
 */
#define EdoCompileTimeAssert_IsKindOf(targ, baseclass) (void)static_cast<baseclass*>((targ*)0);

/*!
 * \brief
 * A little std helper
 * Predicate to delete an object, useful to plug-in to some of the stl functions
 * \example_snippet_start
 *      for_each(vec.begin(), vec.end(), DeleteObject());
 * \example_snippet_end
 */
struct DeleteObject {
    template<typename T>
    void operator()(T *ptr) { delete ptr; }
};

/*!
 * \brief
 * Tries to get a key from a std::map and returns true and a <key, value> pair on success.
 * Very similar to std::map::find, but much handier to use
 * \tparam KeyType
 * \tparam DataType
 * \tparam Map
 * \param m
 * The map to be searched
 * \param searchFor
 * The key to be searched
 * \param result
 * Pair of key and the value which belongs to the key
 * \return
 * True if the key was found (also sets result), false if the key was not found (does not set result)
 * \example_snippet_start
 * 		std::map<EdoString, EdoString> userToFruit;
 * 		userToFruit["john doe"] = "apple";
 * 		pair<EdoString, EdoString> userAndHisFavouriteFruit("unknown user", "unknown fruit");
 *
 * 		if ( Find(userToFruit, "john doe", userAndHisFavouriteFruit) ) { ... do something with userAndHisFavouriteFruit ... }
 * 		else { ... user not found ... )
 * \example_snippet_end
 */
template <typename KeyType, typename DataType, typename Map>
bool Find(const Map &m, const KeyType &searchFor, typename std::pair<KeyType, DataType> &result) {
    typename Map::const_iterator it;
    if ((it = m.Find(searchFor)) != m.End()) {
        result = *it;
        return true;
    }
    return false;
}

/*!
 * \brief
 * Tries to get a key from a std::map and returns true and the element on success.
 * Very similar to std::map::find, but much handier to use
 * \tparam KeyType
 * \tparam DataType
 * \tparam Map
 * \param m
 * The map to be searched
 * \param searchFor
 * The key to be searched
 * \param result
 * The value which belongs to the key
 * \return
 * Tue if the key was found (also sets result), false if the key was not found (does not set result)
 * \example_snippet_start
 * 		std::map<EdoString, EdoString> userToFruit;
 * 		userToFruit["john doe"] = "apple";
 * 		EdoString usersFruit("unknown");
 *
 * 		if ( Find(userToFruit, "john doe", usersFruit) ) { ... do something with usersFruit ... }
 * 		else { ... user not found ... )
 * \example_snippet_end
 */
template <typename KeyType, typename DataType, typename Map>
bool Find(const Map &m, const KeyType &searchFor, DataType &result) {
    typename Map::const_iterator it;
    if ((it = m.Find(searchFor)) != m.End()) {
        result = it->second;
        return true;
    }
    return false;
}

/*!
 * \brief
 * Tries to get a key from a std::map. Returns the element if found, else a default value that's specified
 * Very similar to std::map::find, but much handier to use
 * \tparam KeyType
 * \tparam ValueType
 * \tparam Map
 * \param m
 * The map to be searched
 * \param searchFor
 * The key to be searched
 * \param returnElse
 * The default value that should be returned when the key was not found
 * \return
 * The key if found, else the value specified by returnElse
 * \example_snippet_start
 * 		std::map<EdoString, EdoString> userToFruit;
 * 		userToFruit["john doe"] = "apple";
 *
 * 		EdoString fruit = find_else(userToFruit, "john doe", "no fruit found (user does not exist)") );
 * \example_snippet_end
 */
template <typename KeyType, typename ValueType, typename Map>
const ValueType &Find_Else(const Map &m, const KeyType &searchFor, const ValueType &returnElse) {
    typename Map::const_iterator it;
    if ((it = m.Find(searchFor)) != m.End()) {
        return it->second;
    }
    return returnElse;
}

/*!
 * \brief
 * Joins a vector of values into a single string with specifiable delimiter.
 * The values are converted the same way as toString does.
 * \tparam DataType
 * \param vec
 * The vector which holds the individual values
 * \param delimiter
 * THe delimiter string to put between items
 * \return
 * The joined string
 * \example_snippet_start
 * 		std::vector<float> pos3d;
 *      pos3d.push_back(0); pos3d.push_back(1); pos3d.push_back(2);
 *      EdoString result = join(pos3d, ", ");
 * \example_snippet_end{ "0\, 1\, 2" }
 * \see ToString
 */
template <typename DataType>
string Join(const vector<DataType> &vec, const string &delimiter) {
    ostringstream result;

    typename vector<DataType>::const_iterator it = vec.begin();
    for (; it != vec.end(); ++it) {
        result << *it;
        if ((it + 1) != vec.end())
            result << delimiter;
    }

    return result.str();
}

#endif // EDOCORE_EDOINCLUDES_H
