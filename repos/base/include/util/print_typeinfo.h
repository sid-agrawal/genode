/*
 * \brief  Helpers for printing typeinfo
 * \author Sidhartha Agrawal
 * \date   2023-06-24
 */


#ifndef _INCLUDE__UTIL__PRINT_TYPEINFO_H
#define _INCLUDE__UTIL__PRINT_TYPEINFO_H

namespace Genode {


	/** print the type info for template var **/
        template <typename T>
        void printTypeInfo(T)
        {
           Genode::log("Value of t: ", __PRETTY_FUNCTION__);
        }

        // /** print the type info for template var **/
        // template <typename T>
        // void kprintTypeInfo(T)
        // {
        //    Genode::raw("Value of t: ", __PRETTY_FUNCTION__);
        // }
}

#endif /* _INCLUDE__UTIL__PRINT_TYPEINFO_H */
