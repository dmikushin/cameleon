/* This is a library for controlling USB devices using the libusb-1.0 library.
   It defines a `ControlHandle` struct that provides methods for opening/closing
   the device, reading/writing data, enabling/disabling streaming, and managing
   connection configuration.

   Here's an overview of the main components and functionalities:

   1. **ControlHandle Struct**: This struct represents a handle to control a USB
   device. It contains a `libusb_device` field for the libusb device, a
   `libusb_context` field for the libusb context, and a `ConnectionConfig` field
   for managing connection configuration. The struct also has an associated
   `DeviceInfo` type that holds information about the device.

   2. **ControlResult Enum**: This enum is used to represent the result of
   control operations on the USB device. It can be either `Ok(T)`, where `T` is
   the success value, or `Err(ControlError)`, where `ControlError` is an error
   type that wraps a libusb error code and provides additional context
   information.

   3. **DeviceInfo Struct**: This struct holds information about a USB device,
   including its vendor ID, product ID, bus number, address, serial number, and
   manufacturer name. It implements the `Debug`, `Clone`, and `PartialEq` traits
   for convenient usage.

   4. **ConnectionConfig Struct**: This struct manages connection configuration
   for a USB device, including timeout duration, retry count, maximum command
   length, and maximum acknowledge length. It provides default values for these
   parameters.

   5. **DeviceControl Trait**: This trait defines the methods that can be used
   to control a USB device. It includes methods for opening/closing the device,
   reading/writing data, enabling/disabling streaming, managing connection
   configuration, and retrieving device information. The `ControlHandle` struct
   implements this trait.

   6. **SharedControlHandle Struct**: This struct provides thread-safe access to
   a `ControlHandle`. It wraps an `Arc<Mutex<ControlHandle>>`, which allows
   multiple threads to share the same handle while ensuring that only one thread
   can modify it at a time. The struct implements the `From` trait for
   converting a `ControlHandle` into a `SharedControlHandle`, and it also
   implements the `DeviceControl` trait by delegating to the wrapped
   `ControlHandle`.

   7. **Helper Functions**: The code includes several helper functions, such as
   `get_device_info`, `open_device`, `close_device`, `read_data`, `write_data`,
   `enable_streaming`, and `disable_streaming`, which are used to implement the
   methods of the `ControlHandle` struct.

   Overall, this code provides a convenient and safe way to control USB devices
   in C++ using the libusb-1.0 library. It includes support for managing
   connection configuration, handling errors, and providing thread-safe access
   to device handles.
*/

#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "ControlHandle.h"
#include "DeviceControl.h"

namespace u3v {

// SharedControlHandle: This is a wrapper around `ControlHandle` that provides
// thread-safe access to its methods. It uses an `Arc<Mutex<T>>` to ensure that
// only one thread can modify the underlying `ControlHandle` at a time. The
// `impl_shared_control_handle!` macro is used to automatically generate
// thread-safe versions of the methods in `ControlHandle`.
class SharedControlHandle : public DeviceControl {
public:
  explicit SharedControlHandle(std::shared_ptr<ControlHandle> handle)
      : m_handle(handle), m_mutex() {}

  // Thread safe version of ControlHandle::buffer_capacity.
  virtual size_t buffer_capacity() const override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->buffer_capacity();
  }

  // Thread safe version of ControlHandle::resize_buffer.
  virtual void resize_buffer(size_t size) override {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_handle->resize_buffer(size);
  }

  // Thread safe version of ControlHandle::timeout_duration.
  virtual std::chrono::milliseconds timeout_duration() const override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->timeout_duration();
  }

  // Thread safe version of ControlHandle::set_timeout_duration.
  virtual void set_timeout_duration(std::chrono::milliseconds duration) override {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_handle->set_timeout_duration(duration);
  }

  // Thread safe version of ControlHandle::retry_count.
  virtual unsigned int retry_count() const override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->retry_count();
  }

  // Thread safe version of ControlHandle::set_retry_count.
  virtual void set_retry_count(unsigned int count) override {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_handle->set_retry_count(count);
  }

  // Returns the device info of the handle.
  virtual const u3v::DeviceInfo& device_info() const override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->device_info();
  }

  // Thread safe version of DeviceControl::is_opened.
  virtual bool is_opened() const override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->is_opened();
  }

  // Thread safe version of DeviceControl::open.
  virtual bool open() override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->open();
  }

  // Thread safe version of DeviceControl::close.
  virtual bool close() override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->close();
  }

  // Thread safe version of DeviceControl::read.
  virtual bool read(uint64_t address, std::vector<uint8_t> &buf) override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->read(address, buf);
  }

  // Thread safe version of DeviceControl::write.
  virtual bool write(uint64_t address, const std::vector<uint8_t> &data) override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->write(address, data);
  }

  // Thread safe version of DeviceControl::genapi.
  virtual std::string genapi() override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->genapi();
  }

  // Thread safe version of DeviceControl::enable_streaming.
  virtual bool enable_streaming() override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->enable_streaming();
  }

  // Thread safe version of DeviceControl::disable_streaming.
  virtual bool disable_streaming() override {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_handle->disable_streaming();
  }

private:
  std::shared_ptr<ControlHandle> m_handle;
  mutable std::mutex m_mutex;
};

// ConnectionConfig: This struct holds configuration parameters for the
// connection to the device, such as the timeout duration, retry count, and
// maximum command and acknowledge lengths. It implements the `Default` trait to
// provide default values for these parameters.
class ConnectionConfig {
public:
  // Timeout duration of each transaction between device.
  std::chrono::duration<double> timeout_duration;

  // The value determines how many times to retry when pending acknowledge is
  // returned from the device.
  uint16_t retry_count;

  // Maximum length of a command sent to device from host. Unit is byte.
  uint32_t maximum_cmd_length;

  // Maximum length of an acknowledge sent to host from device. Unit is byte.
  uint32_t maximum_ack_length;

  ConnectionConfig()
      : timeout_duration(INITIAL_TIMEOUT_DURATION), retry_count(3),
        maximum_cmd_length(INITIAL_MAXIMUM_CMD_LENGTH),
        maximum_ack_length(INITIAL_MAXIMUM_ACK_LENGTH) {}
};

// This allows for polymorphism
std::unique_ptr<u3v::DeviceControl> make_device_control(ControlHandle &ctrl) {
  return std::unique_ptr<u3v::DeviceControl>(dynamic_cast<u3v::DeviceControl*>(&ctrl));
}

// This allows for polymorphism
std::unique_ptr<u3v::DeviceControl> make_device_control(SharedControlHandle &ctrl) {
  return std::unique_ptr<u3v::DeviceControl>(dynamic_cast<u3v::DeviceControl*>(&ctrl));
}

} // namespace u3v

