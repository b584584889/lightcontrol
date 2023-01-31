
//////// implementation log.cpp ///////////
#include <ctime>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "log.h"

#include "../common.h"

namespace vilslog
{
    void flush() { out.flush() ; }

    namespace // detail
    {
        std::string time_stamp()
        {
            const auto now = time(nullptr) ;
            char cstr[256] {};
            return std::strftime( cstr, sizeof(cstr), "%Y%m%d_%H%M%S", std::localtime(&now) ) ? cstr : "" ;
        }

        std::string path_to_session_log_file()
        {
            //static const std::string log_dir = "/Library/WebServer/Documents/log/vilscenter_" ;
            static const std::string log_dir = LOG_DIRECTORY ;
            static const std::string log_file_name = "log.txt" ;
            return log_dir +"/vilscenter_"+ time_stamp() + '_' + log_file_name ;
        }
    }

   const std::string path = path_to_session_log_file() ;
   std::ofstream out = std::ofstream( path );
   int MODE = MODE_FILE;

   int printf(const char* format, ...)
   {
       int i=0;
       char str_tmp[65536];
       va_list vArgList;
       va_start (vArgList, format);
       i = vsnprintf(str_tmp, 65536, format, vArgList);
       va_end(vArgList);

       if (MODE == MODE_FILE)
       {

           vilslog::out <<currentDateTimeForSQL()<<" "<<str_tmp;
           vilslog::flush() ;
       }
       else
       {
           fprintf(stdout, "%s %s", currentDateTimeForSQL().c_str(), str_tmp);
       }
       flush();
       return i;
   }

  int printRaw(const char* format, ...)
  {
      int i=0;
      char str_tmp[65536];
      va_list vArgList;
      va_start (vArgList, format);
      i = vsnprintf(str_tmp, 65536, format, vArgList);
      va_end(vArgList);

      if (MODE == MODE_FILE)
      {
          vilslog::out <<str_tmp;
          vilslog::flush() ;
      }
      else
      {
          fprintf(stdout, "%s", str_tmp);
      }

      flush();
      return i;
  }

   void setMode(int mode)
   {
       MODE = mode;
   }

}
