/* The provided Rust code is a module that contains several classes for parsing
   and dumping bytes for various data types. These classes are used in the
   context of USB device communication.

   - `ParseBytes`: This class defines a method `parse_bytes` which takes a byte
   slice as input and returns a value of the implementing type or an error if
   parsing fails. The class is implemented for several numeric data types (u8,
   u16, u32, u64, i8, i16, i32, i64) to parse bytes into their respective values
   using little-endian byte order. It's also implemented for
   `DeviceConfiguration` and `BusSpeed`, which are custom types used in the
   context of USB device communication.

   - `DumpBytes`: This class defines a method `dump_bytes` which takes a mutable
   byte slice as input and writes the bytes representation of the implementing
   value into it. The class is implemented for several numeric data types (u8,
   u16, u32, u64, i8, i16, i32, i64) to write their little-endian byte
   representation into a buffer. It's also implemented for `&str` and
   `DeviceConfiguration`, which are used to write string data and device
   configuration bytes into a buffer, respectively.

   - The code uses macros (`impl_parse_bytes_for_numeric!` and
   `impl_dump_bytes_for_numeric!`) to generate implementations for the
   `ParseBytes` and `DumpBytes` classes for numeric data types, reducing
   repetitive code.

   Overall, this module provides a convenient way to parse bytes into various
   data types and dump data types into byte buffers, which is essential for USB
   device communication where data needs to be exchanged in a specific binary
   format.
*/

#include <cstdint>
#include <stdexcept>
#include <type_classes>

// This function takes a pointer to an array of bytes and returns the parsed
// value. It uses bitwise operations to combine the bytes into a single value,
// assuming little-endian byte order. The `static_assert` statement ensures that
// the template type is an integral type.
template <typename T> T ParseBytes(const uint8_t *bytes) {
  static_assert(std::is_integral<T>::value, "T must be an integral type");
  T result = 0;
  for (size_t i = 0; i < sizeof(T); ++i) {
    result |= static_cast<T>(bytes[i]) << (8 * i);
  }
  return result;
}

namespace u3v {
enum class BusSpeed : uint8_t {
  LowSpeed = 0b1,
  FullSpeed = 0b10,
  HighSpeed = 0b100,
  SuperSpeed = 0b1000,
  SuperSpeedPlus = 0b10000
};
}

class ControlError : public std::runtime_error {
public:
  using runtime_error::runtime_error;
};

template <typename T> class ParseBytes {
public:
  static T parse_bytes(const uint8_t *bytes) {
    // Implementation depends on the actual type T and the byte order.
    // This is a placeholder implementation for u32.
    return static_cast<T>(bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) |
                          (bytes[3] << 24));
  }
};

template <> class ParseBytes<u3v::BusSpeed> {
public:
  static u3v::BusSpeed parse_bytes(const uint8_t *bytes) {
    uint32_t raw = ParseBytes<uint32_t>::parse_bytes(bytes);
    switch (raw) {
    case 0b1:
      return u3v::BusSpeed::LowSpeed;
    case 0b10:
      return u3v::BusSpeed::FullSpeed;
    case 0b100:
      return u3v::BusSpeed::HighSpeed;
    case 0b1000:
      return u3v::BusSpeed::SuperSpeed;
    case 0b10000:
      return u3v::BusSpeed::SuperSpeedPlus;
    default:
      throw ControlError("invalid bus speed defined");
    }
  }
};

// This function will dump the bytes of a numeric value into a buffer. It uses
// `std::is_integral` to ensure that the type is an integer, and it uses
// `static_assert` to check that the buffer is large enough to hold the value.
// The loop iterates over each byte in the value, shifting the bits to the right
// and casting them to `uint8_t`.
//
// You can use this function like this:
// uint32_t value = 0x12345678;
// uint8_t buf[sizeof(value)];
// dumpBytes(value, buf);
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
dumpBytes(const T &value, uint8_t *buf) {
  static_assert(sizeof(T) <= sizeof(buf), "Buffer is too small");
  for (size_t i = 0; i < sizeof(T); ++i) {
    buf[i] = static_cast<uint8_t>(value >> (i * 8));
  }
}

// Assuming that DeviceConfiguration is a class with a method to get its data as
// a byte array.
class DeviceConfiguration {
public:
  const std::vector<uint8_t> &getData() const { return data; }

private:
  std::vector<uint8_t> data;
};

// DumpBytes class for string type
// DumpBytes function throws a `std::runtime_error` if the string or device
// configuration is too large for the buffer.
void dumpBytes(const std::string &str, uint8_t *buf, size_t bufSize) {
  if (str.size() > bufSize) {
    throw std::runtime_error("too large string");
  }

  std::memcpy(buf, str.c_str(), str.size());
  // Zero terminate if data is shorter than buffer length.
  if (str.size() < bufSize) {
    buf[str.size()] = 0;
  }
}

// DumpBytes class for DeviceConfiguration type
// DumpBytes function throws a `std::runtime_error` if the string or device
// configuration is too large for the buffer.
void dumpBytes(const DeviceConfiguration &config, uint8_t *buf,
               size_t bufSize) {
  const auto &data = config.getData();
  if (data.size() > bufSize) {
    throw std::runtime_error("too large device configuration");
  }

  std::memcpy(buf, data.data(), data.size());
}
