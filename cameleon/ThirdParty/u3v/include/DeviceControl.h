#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <chrono>
#include <memory>
#include <string>

namespace u3v {

class DeviceInfo;

// DeviceControl: This is a trait that defines methods for controlling a device.
// It is a pure abstract base class that defines the interface for controlling a
// device. It includes methods for opening and closing the connection, reading
// and writing data to the device, generating API documentation, and enabling
// and disabling streaming. The `impl_shared_control_handle!` macro is used to
// automatically implement these methods for `SharedControlHandle`.
class DeviceControl {
public:
  // The class uses RAII (Resource Acquisition Is Initialization) idiom by
  // providing a virtual destructor.
  virtual ~DeviceControl() = default;

  // Getters and setters for timeout_duration, retry_count, buffer_capacity
  virtual std::chrono::milliseconds timeout_duration() const = 0;
  virtual void set_timeout_duration(std::chrono::milliseconds duration) = 0;

  virtual unsigned int retry_count() const = 0;
  virtual void set_retry_count(unsigned int count) = 0;

  virtual size_t buffer_capacity() const = 0;
  virtual void resize_buffer(size_t size) = 0;

  // Other methods
  virtual const u3v::DeviceInfo& device_info() const = 0;
  virtual bool is_opened() const = 0;

  virtual bool open() = 0;
  virtual bool close() = 0;

  virtual bool read(uint64_t address, std::vector<uint8_t> &buf) = 0;
  virtual bool write(uint64_t address, const std::vector<uint8_t> &data) = 0;

  virtual std::string genapi() = 0;

  virtual bool enable_streaming() = 0;
  virtual bool disable_streaming() = 0;
};

} // namespace u3v

#endif // DEVICE_CONTROL_H
