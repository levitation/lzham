// File: platform.cpp
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
#include "lzham_core.h"
#include "lzham_timer.h"

#ifdef LZHAM_PLATFORM_X360
#include <xbdm.h>
#endif

//#define _WIN32_WINNT 0x500
//#include "windows.h"

bool lzham_is_debugger_present(void)
{
#ifdef LZHAM_PLATFORM_X360
   return DmIsDebuggerPresent() != 0;
#else
   return IsDebuggerPresent() != 0;
#endif
}

void lzham_debug_break(void)
{
   DebugBreak();
}

void lzham_output_debug_string(const char* p)
{
   OutputDebugStringA(p);
}

#ifdef __GNUC__
int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...)
{
   if (!sizeOfBuffer)
      return 0;

   va_list args;
   va_start(args, format);
   int c = vsnprintf(buffer, sizeOfBuffer, format, args);
   va_end(args);

   buffer[sizeOfBuffer - 1] = '\0';

   if (c < 0)
      return sizeOfBuffer - 1;

   return LZHAM_MIN(c, (int)sizeOfBuffer - 1);
}
int vsprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, va_list args)
{
   if (!sizeOfBuffer)
      return 0;
   
   int c = vsnprintf(buffer, sizeOfBuffer, format, args);

   buffer[sizeOfBuffer - 1] = '\0';

   if (c < 0)
      return sizeOfBuffer - 1;

   return LZHAM_MIN(c, (int)sizeOfBuffer - 1);
}
#endif

#if LZHAM_BUFFERED_PRINTF

namespace lzham
{
   struct buffered_str
   {
      enum { cBufSize = 256 };
      char m_buf[cBufSize];
   };

   static lzham::vector<buffered_str> g_buffered_strings;
   static volatile long g_buffered_string_locked;
   
   static void lock_buffered_strings()
   {
      while (InterlockedExchange(&g_buffered_string_locked, 1) == 1)
      {
         lzham_yield_processor();
         lzham_yield_processor();
         lzham_yield_processor();
         lzham_yield_processor();
      }

      LZHAM_MEMORY_IMPORT_BARRIER
   }
   
   static void unlock_buffered_strings()
   {
      LZHAM_MEMORY_EXPORT_BARRIER

      InterlockedExchange(&g_buffered_string_locked, 0);
   }

} // namespace lzham

void lzham_buffered_printf(const char *format, ...)
{
   format;
   
   char buf[lzham::buffered_str::cBufSize];
   
   va_list args;
   va_start(args, format);
   vsnprintf_s(buf, sizeof(buf), sizeof(buf), format, args);
   va_end(args);   

   buf[sizeof(buf) - 1] = '\0';
   
   lzham::lock_buffered_strings();
   
   if (!lzham::g_buffered_strings.capacity())
   {
      lzham::g_buffered_strings.try_reserve(2048);
   }

   if (lzham::g_buffered_strings.try_resize(lzham::g_buffered_strings.size() + 1))
   {
      memcpy(lzham::g_buffered_strings.back().m_buf, buf, sizeof(buf));
   }

   lzham::unlock_buffered_strings();
}

void lzham_flush_buffered_printf()
{
   lzham::lock_buffered_strings();

   for (lzham::uint i = 0; i < lzham::g_buffered_strings.size(); i++)
   {
      printf("%s", lzham::g_buffered_strings[i].m_buf);
   }

   lzham::g_buffered_strings.try_resize(0);

   lzham::unlock_buffered_strings();
}
#endif   