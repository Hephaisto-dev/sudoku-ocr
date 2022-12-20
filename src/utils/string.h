#pragma once

#include "common.h"



/**
 * @file string.h
 * @brief This file provide stuff to manipulate string in unicode encoding and with a
 * more flexible and efficient way than the standard C \b char*. \n
 * Every functions that end with '_cp' create a new string
 * and make a copy of data. \n
 * Every functions that end with '_nc' don't copy any data and use
 * the string given as parameter as destination. \n
 */



/**
 * @brief A structure that represent a string (in unicode encoding)
 *
 */
typedef struct string_s {
    wchar_t* data; // A pointer to the string
    size_t len; // String length
    size_t allocated; // Number of allocated wchar_t
} *string_t;




/**
 * @brief Create a new empty string (that must be free at some time!)
 *
 * @return string_t A new empty string
 */
string_t str_new();


/**
 * @brief Create a string with an already allocated size
 *
 * @param len The len in characters to allocate
 * @return string_t A new empty string but with already allocated space
 */
string_t str_alloc(size_t len);


/**
 * @brief Free an allocated string from memory
 *
 * @param s The string to free
 */
void str_free(string_t s);


/**
 * @brief Get the length in characters of \p s
 *
 * @param s The string to get the length from
 * @return size_t The length in characters of \p s
 */
static inline size_t str_len(string_t s) {
    return s->len;
}


/**
 * @brief Clear a string (set length to 0)
 *
 * @param s The string to clear
 */
void str_clear(string_t s);


/**
 * @brief Extract a sub string from a string
 *
 * @param s The string to extract from
 * @param begin The first index of the sub string to extract
 * @param len The length of the string to extract
 * @param out Where to output the extracted sub string ( can be NULL or \p s )
 * @return string_t A new string with the extracted part if \p out is \b NULL or \p out otherwise
 */
string_t str_sub(string_t s, size_t begin, size_t len, string_t out);


/**
 * @brief Replace all instance of \p rep by \p by in th input string
 *
 * @param s The string where to replace characters
 * @param rep The character to replace
 * @param by The replacement character
 * @param out Where to output the processed string ( can be NULL or \p s )
 * @return string_t A new string with replaced characters if \p out is \b NULL or \p out otherwise
 */
string_t str_rep_char(string_t s, wchar_t rep, wchar_t by, string_t out);


/**
 * @brief Replace all instance of \p rep by \p by in th input string
 *
 * @param s The string where to replace sub strings
 * @param rep The string to replace
 * @param by The replacement string
 * @param out Where to output the processed string ( can be NULL or \p s )
 * @return string_t A new string with replaced sub strings if \p out is \b NULL or \p out otherwise
 */
string_t str_rep_str(string_t s, string_t rep, string_t by, string_t out);


/**
 * @brief Concatenate \p s with \p a
 *
 * @param s The first string to concatenate
 * @param a The second string to concatenate
 * @param out Where to output the result of the concatenation
 * @return string_t A new string as the result of the concatenation if \p out is \b NULL or \p out otherwise
 */
string_t str_merge(string_t s, string_t a, string_t out);


/**
 * @brief Get character at index \p i
 *
 * @param i The index of the character to get
 * @return wchar_t The requested character
 */
static inline wchar_t str_get(string_t s, size_t i) {
    return s->data[i];
}


/**
 * @brief Set the character at index \p i
 *
 * @param i The index of the character to get
 * @param c The character to set
 * @return void
 */
static inline void str_set(string_t s, size_t i, wchar_t c) {
    s->data[i] = c;
}



/*
// Split a string in substring with char as separator
size_t str_split_char(string_t s, wchar_t sep, string_t* out, size_t max_out_len);

// Split a string in substring with string as separator
size_t str_split_str(string_t s, string_t sep, string_t* out, size_t max_out_len);
*/



string_t str_from_wchars_cp(const wchar_t* s);
string_t str_from_wchars_nc(wchar_t* s);

wchar_t* str_to_wchars_cp(string_t s);
wchar_t* str_to_wchars_nc(string_t s);



string_t str_sub_cp(string_t s, size_t begin, size_t len);
void     str_sub_nc(string_t s, size_t begin, size_t len);


string_t str_rep_char_cp(string_t s, wchar_t rep, wchar_t by);
void     str_rep_char_nc(string_t s, wchar_t rep, wchar_t by);


string_t str_rep_str_cp(string_t s, string_t rep, string_t by);
void     str_rep_str_nc(string_t s, string_t rep, string_t by);


string_t str_merge_cp(string_t s, string_t a);
void     str_merge_nc(string_t s, string_t a);
