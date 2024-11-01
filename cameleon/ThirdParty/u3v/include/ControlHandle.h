#ifndef CONTROL_HANDLE_H
#define CONTROL_HANDLE_H

#include "DeviceControl.h"

#include <chrono>
#include <mutex>
#include <stdint.h>
#include <string>

namespace u3v {

// Initial timeout duration for transaction between device and host.
// This value is temporarily used until the device's bootstrap register value is
// read.
const std::chrono::milliseconds INITIAL_TIMEOUT_DURATION(500);

// Initial maximum command packet length for transaction between device and
// host. This value is temporarily used until the device's bootstrap register
// value is read.
const uint32_t INITIAL_MAXIMUM_CMD_LENGTH = 128;

// Initial maximum acknowledge packet length for transaction between device and
// host. This value is temporarily used until the device's bootstrap register
// value is read.
const uint32_t INITIAL_MAXIMUM_ACK_LENGTH = 128;

const uint32_t PAYLOAD_TRANSFER_SIZE = 1024 * 64;

class ControlHandleImpl;
class DeviceInfo;

// ControlHandle: This is a struct that represents a handle to control a device.
// It contains a `device_info` field, which holds information about the device,
// and a `connection` field, which manages the connection to the device. The
// `Connection` struct is not shown in the provided code snippet.
class ControlHandle : public DeviceControl {
public:
  // Constructor and destructor for RAII (Resource Acquisition Is
  // Initialization)
  ControlHandle();
  ~ControlHandle();

  // Methods translated from Rust's ControlHandle struct
  virtual size_t buffer_capacity() const override;
  virtual void resize_buffer(size_t size) override;

  virtual std::chrono::milliseconds timeout_duration() const override;
  virtual void set_timeout_duration(std::chrono::milliseconds duration) override;

  virtual unsigned int retry_count() const override;
  virtual void set_retry_count(unsigned int count) override;

  virtual const u3v::DeviceInfo& device_info() const override;
  virtual bool is_opened() const override;

  // Add error handling mechanism for ControlResult in C++
  virtual bool open() override;
  virtual bool close() override;

  virtual bool read(uint64_t address, std::vector<uint8_t> &buf) override;
  virtual bool write(uint64_t address, const std::vector<uint8_t> &data) override;

  virtual std::string genapi() override;

  virtual bool enable_streaming() override;
  virtual bool disable_streaming() override;

private:
  // Private member to hold the actual ControlHandle instance from Rust
  u3v::ControlHandleImpl* impl_;
  std::mutex mutex_;
};

// This allows for polymorphism
class ControlHandle;
std::unique_ptr<u3v::DeviceControl> make_device_control(ControlHandle &ctrl);

// This allows for polymorphism
class SharedControlHandle;
std::unique_ptr<u3v::DeviceControl> make_device_control(SharedControlHandle &ctrl);

} // namespace u3v

#endif // CONTROL_HANDLE_H

