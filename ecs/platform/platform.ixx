module;
#include <bit>
#include <cstdint>
#include <string_view>
export module nx.platform;

namespace nx
{

  export
   using PlatformMagic = int64_t;

   export
   consteval PlatformMagic make_platform(std::string_view  name)
   {
      if (name.size() > sizeof(PlatformMagic))
      {
         throw "platform name to long";
      }

      alignas(PlatformMagic) unsigned char buffer[sizeof(PlatformMagic)] = {0};

      for (std::size_t i = 0; i < name.size(); ++i) {
         buffer[i] = static_cast<unsigned char>(name[i]);
      }

      return std::bit_cast<PlatformMagic>(buffer);
   }

   export
   constexpr PlatformMagic UnknownPlatform = make_platform("Unknown");

   export
  template<PlatformMagic  T>
  struct Platform;

   export
   template<>
   struct Platform<UnknownPlatform>
   {
      static constexpr bool  is_platform_type (){ return false;};

      [[nodiscard]] constexpr static const char* name()
      {
         return "Unknown";
      }

      [[nodiscard]] static constexpr PlatformMagic magic()
      {
         return UnknownPlatform;
      }

   };

   template<PlatformMagic T>
   bool is_platform_type()
   {
      return Platform<T>::is_platform_type;
   }

   template<PlatformMagic T>
   const char* platform_name()
   {
      return Platform<T>::name();
   }


   export struct IPlatformComponent
   {
      virtual ~IPlatformComponent()  = default;


      virtual constexpr PlatformMagic platform_type() const = 0;
   };
}