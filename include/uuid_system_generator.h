#pragma once

#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include <array>
#include <string_view>
#include <iterator>
#include <random>
#include <memory>
#include <functional>
#include <type_traits>
#include <optional>
#include <chrono>
#include <numeric>
#include <atomic>
#include <span>

#ifdef _WIN32

#include <objbase.h>

#elif defined(__linux__) || defined(__unix__)

#include <uuid/uuid.h>

#elif defined(__APPLE__)

#include <CoreFoundation/CFUUID.h>

#endif

#include "uuid.h"

namespace uuids
{
   class uuid_system_generator
   {
   public:
      using result_type = uuid;

      uuid operator()()
      {
#ifdef _WIN32

         GUID newId;
         HRESULT hr = ::CoCreateGuid(&newId);

         if (FAILED(hr))
         {
            throw std::system_error(hr, std::system_category(), "CoCreateGuid failed");
         }

         std::array<uint8_t, 16> bytes =
         { {
               static_cast<unsigned char>((newId.Data1 >> 24) & 0xFF),
               static_cast<unsigned char>((newId.Data1 >> 16) & 0xFF),
               static_cast<unsigned char>((newId.Data1 >> 8) & 0xFF),
               static_cast<unsigned char>((newId.Data1) & 0xFF),

               (unsigned char)((newId.Data2 >> 8) & 0xFF),
               (unsigned char)((newId.Data2) & 0xFF),

               (unsigned char)((newId.Data3 >> 8) & 0xFF),
               (unsigned char)((newId.Data3) & 0xFF),

               newId.Data4[0],
               newId.Data4[1],
               newId.Data4[2],
               newId.Data4[3],
               newId.Data4[4],
               newId.Data4[5],
               newId.Data4[6],
               newId.Data4[7]
            } };

         return uuid{ std::begin(bytes), std::end(bytes) };

#elif defined(__linux__) || defined(__unix__)

         uuid_t id;
         uuid_generate(id);

         std::array<uint8_t, 16> bytes =
         { {
               id[0],
               id[1],
               id[2],
               id[3],
               id[4],
               id[5],
               id[6],
               id[7],
               id[8],
               id[9],
               id[10],
               id[11],
               id[12],
               id[13],
               id[14],
               id[15]
            } };

         return uuid{ std::begin(bytes), std::end(bytes) };

#elif defined(__APPLE__)
         auto newId = CFUUIDCreate(NULL);
         auto bytes = CFUUIDGetUUIDBytes(newId);
         CFRelease(newId);

         std::array<uint8_t, 16> arrbytes =
         { {
               bytes.byte0,
               bytes.byte1,
               bytes.byte2,
               bytes.byte3,
               bytes.byte4,
               bytes.byte5,
               bytes.byte6,
               bytes.byte7,
               bytes.byte8,
               bytes.byte9,
               bytes.byte10,
               bytes.byte11,
               bytes.byte12,
               bytes.byte13,
               bytes.byte14,
               bytes.byte15
            } };
         return uuid{ std::begin(arrbytes), std::end(arrbytes) };
#else
         return uuid{};
#endif
      }
   };

}
