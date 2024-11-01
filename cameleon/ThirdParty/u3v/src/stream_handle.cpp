/* This code is part of a library or framework that handles streaming data from
  a device. It defines a struct called `StreamParams` with various parameters
  related to the stream, such as leader size, trailer size, payload size, and
  timeout. There are also methods for calculating maximum payload size and
  constructing `StreamParams` from a `DeviceControl`.

  The code begins by defining the `StreamParams` struct with its associated
  fields: `leader_size`, `trailer_size`, `payload_size`, `payload_count`,
  `payload_final1_size`, `payload_final2_size`, and `timeout`. It then
  implements a method called `new` for constructing a new instance of
  `StreamParams` with the provided values.

  Additionally, there is a method called `from_control` that takes a mutable
  reference to a type implementing the `DeviceControl` trait. This method
  retrieves various parameters from the device control and uses them to
  construct a new instance of `StreamParams`. It handles errors using the
  `Result` type and returns a `ControlResult<Self>`, which is likely an alias
  for another result type specific to the library or framework.

  The code also defines three helper functions: `read_leader`, `read_payload`,
  and `read_trailer`. These functions are responsible for reading data from the
  stream based on the provided parameters. They use an asynchronous pool
  (`async_pool`) to submit read requests and handle any errors that may occur
  during the process.

  Overall, this code is a part of a larger system that handles streaming data
  from a device. It provides functionality for configuring stream parameters,
  reading data from the stream in different sections (leader, payload, trailer),
  and handling asynchronous operations using an `AsyncPool`. The Rust code uses
  several traits that are not defined in the provided code snippet:
  `DeviceControl`, `Abrm`, `Sirm`, and `AsyncPool`. These traits likely define
  the behavior of device control, abstract base request model (ABRM), stream
  interface request model (SIRM), and asynchronous pool, respectively.

  To understand how these traits are implemented and used in this code, we would
  need to see their definitions or additional context. Without that information,
  it's difficult to provide a more detailed explanation of the dependencies and
  their roles in the code.
*/

#include <chrono>
#include <iostream>
#include <stdexcept>

#include "DeviceControl.h"

struct StreamParams {
  size_t leader_size;
  size_t trailer_size;
  size_t payload_size;
  size_t payload_count;
  size_t payload_final1_size;
  size_t payload_final2_size;
  std::chrono::duration<double> timeout;

  StreamParams(size_t leader, size_t trailer, size_t payload, size_t count,
               size_t final1, size_t final2, std::chrono::duration<double> t)
      : leader_size(leader), trailer_size(trailer), payload_size(payload),
        payload_count(count), payload_final1_size(final1),
        payload_final2_size(final2), timeout(t) {}

  size_t maximum_payload_size() const {
    return payload_size * payload_count + payload_final1_size +
           payload_final2_size;
  }

  static StreamParams from_control(DeviceControl &ctrl) {
    Abrm abrm{ctrl};
    Sirm sirm = abrm.sbrm(ctrl).sirm(ctrl);
    if (!sirm) {
      throw ControlError("the U3V device doesn't have `SIRM`");
    }

    StreamParams params;
    params.leader_size = sirm.maximum_leader_size(ctrl);
    params.trailer_size = sirm.maximum_trailer_size(ctrl);
    params.payload_size = sirm.payload_transfer_size(ctrl);
    params.payload_count = sirm.payload_transfer_count(ctrl);
    params.payload_final1_size = sirm.payload_final_transfer1_size(ctrl);
    params.payload_final2_size = sirm.payload_final_transfer2_size(ctrl);
    params.timeout = abrm.maximum_device_response_time(ctrl);

    return params;
  }
};

// Abrm class
class Abrm {
private:
  DeviceControl *ctrl;

public:
  explicit Abrm(DeviceControl *ctrl) : ctrl(ctrl) {}
  // Implement the necessary methods using ctrl, handling any errors or
  // exceptions as needed.
};

// Sirm class
class Sirm {
private:
  DeviceControl *ctrl;

public:
  explicit Sirm(DeviceControl *ctrl) : ctrl(ctrl) {}
  // Implement the necessary methods using ctrl, handling any errors or
  // exceptions as needed.
};

// AsyncPool class
class AsyncPool {
public:
  // Implement the necessary methods for asynchronous data transfer, handling
  // any errors or exceptions as needed.
};

// StreamParams struct
struct StreamParams {
  size_t leader_size;
  size_t trailer_size;
  size_t payload_size;
  size_t payload_count;
  size_t payload_final1_size;
  size_t payload_final2_size;
  std::chrono::duration<double> timeout;
};

void read_leader(AsyncPool &async_pool, const StreamParams &params,
                 std::vector<uint8_t> &buf) {
  size_t leader_size = params.leader_size;
  if (buf.size() < leader_size) {
    throw std::runtime_error("Buffer is too small to read the leader");
  }
  try {
    async_pool.submit(buf.data(), leader_size);
  } catch (const std::exception &e) {
    throw StreamError("Failed to read leader: " + std::string(e.what()));
  }
}

void read_payload(AsyncPool &async_pool, const StreamParams &params,
                  std::vector<uint8_t> &buf) {
  size_t payload_size = params.payload_size;
  size_t total_payload_size = payload_size * params.payload_count +
                              params.payload_final1_size +
                              params.payload_final2_size;
  if (buf.size() < total_payload_size) {
    throw std::runtime_error("Buffer is too small to read the payload");
  }
  size_t cursor = 0;
  try {
    for (size_t i = 0; i < params.payload_count; ++i) {
      async_pool.submit(buf.data() + cursor, payload_size);
      cursor += payload_size;
    }
    if (params.payload_final1_size != 0) {
      async_pool.submit(buf.data() + cursor, params.payload_final1_size);
      cursor += params.payload_final1_size;
    }
    if (params.payload_final2_size != 0) {
      async_pool.submit(buf.data() + cursor, params.payload_final2_size);
    }
  } catch (const std::exception &e) {
    throw StreamError("Failed to read payload: " + std::string(e.what()));
  }
}

void read_trailer(AsyncPool &async_pool, const StreamParams &params,
                  std::vector<uint8_t> &buf) {
  size_t trailer_size = params.trailer_size;
  if (buf.size() < trailer_size) {
    throw std::runtime_error("Buffer is too small to read the trailer");
  }
  try {
    async_pool.submit(buf.data(), trailer_size);
  } catch (const std::exception &e) {
    throw StreamError("Failed to read trailer: " + std::string(e.what()));
  }
}
