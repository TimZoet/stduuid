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

#ifdef UUID_TIME_GENERATOR
#include <iphlpapi.h> 
#pragma comment(lib, "IPHLPAPI.lib")
#endif

#endif

#include "uuid.h"

namespace uuids
{
#ifdef UUID_TIME_GENERATOR
   // !!! DO NOT USE THIS IN PRODUCTION
   // this implementation is unreliable for good uuids
   class uuid_time_generator
   {
      using mac_address = std::array<unsigned char, 6>;

      std::optional<mac_address> device_address;

      bool get_mac_address()
      {         
         if (device_address.has_value())
         {
            return true;
         }
         
#ifdef _WIN32
         DWORD len = 0;
         auto ret = GetAdaptersInfo(nullptr, &len);
         if (ret != ERROR_BUFFER_OVERFLOW) return false;
         std::vector<unsigned char> buf(len);
         auto pips = reinterpret_cast<PIP_ADAPTER_INFO>(&buf.front());
         ret = GetAdaptersInfo(pips, &len);
         if (ret != ERROR_SUCCESS) return false;
         mac_address addr;
         std::copy(pips->Address, pips->Address + 6, std::begin(addr));
         device_address = addr;
#endif

         return device_address.has_value();
      }

      long long get_time_intervals()
      {
         auto start = std::chrono::system_clock::from_time_t(time_t(-12219292800));
         auto diff = std::chrono::system_clock::now() - start;
         auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count();
         return ns / 100;
      }

      static unsigned short get_clock_sequence()
      {
         static std::mt19937 clock_gen(std::random_device{}());
         static std::uniform_int_distribution<unsigned short> clock_dis;
         static std::atomic_ushort clock_sequence = clock_dis(clock_gen);
         return clock_sequence++;
      }

   public:
      uuid operator()()
      {
         if (get_mac_address())
         {
            std::array<uuids::uuid::value_type, 16> data;

            auto tm = get_time_intervals();

            auto clock_seq = get_clock_sequence();

            auto ptm = reinterpret_cast<uuids::uuid::value_type*>(&tm);

            memcpy(&data[0], ptm + 4, 4);
            memcpy(&data[4], ptm + 2, 2);
            memcpy(&data[6], ptm, 2);

            memcpy(&data[8], &clock_seq, 2);

            // variant must be 0b10xxxxxx
            data[8] &= 0xBF;
            data[8] |= 0x80;

            // version must be 0b0001xxxx
            data[6] &= 0x1F;
            data[6] |= 0x10;

            memcpy(&data[10], &device_address.value()[0], 6);

            return uuids::uuid{std::cbegin(data), std::cend(data)};
         }

         return {};
      }
   };
#endif
}
