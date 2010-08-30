// File: lzham_helpers.h
//
// Copyright (c) 2009-2010 Richard Geldreich, Jr. <richgel99@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#pragma once

#define LZHAM_NO_COPY_OR_ASSIGNMENT_OP(c) c(const c&); c& operator= (const c&);

namespace lzham
{
   namespace helpers
   {
      template<typename T> struct rel_ops 
      {
         friend bool operator!=(const T& x, const T& y) { return (!(x == y)); }
         friend bool operator> (const T& x, const T& y) { return (y < x); }
         friend bool operator<=(const T& x, const T& y) { return (!(y < x)); }
         friend bool operator>=(const T& x, const T& y) { return (!(x < y)); }
      };
      
      template <typename T> 
      inline T* construct(T* p) 
      { 
         return new (static_cast<void*>(p)) T; 
      }

      template <typename T, typename U> 
      inline T* construct(T* p, const U& init) 
      { 
         return new (static_cast<void*>(p)) T(init); 
      }

      template <typename T> 
      void construct_array(T* p, uint n) 
      { 
         T* q = p + n;
         for ( ; p != q; ++p)
            new (static_cast<void*>(p)) T; 
      }
      
      template <typename T, typename U> 
      void construct_array(T* p, uint n, const U& init) 
      { 
         T* q = p + n;
         for ( ; p != q; ++p)
            new (static_cast<void*>(p)) T(init); 
      }

      template <typename T> 
      inline void destruct(T* p) 
      {	   
         p;
         p->~T(); 
      } 
      
      template <typename T> inline void destruct_array(T* p, uint n) 
      {	
         if ( LZHAM_HAS_DESTRUCTOR(T) )
         {
            T* q = p + n;
            for ( ; p != q; ++p)
               p->~T();
         }               
      } 
                  
   }  // namespace helpers    
   
}  // namespace lzham
