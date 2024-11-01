#include "u3v.h"

namespace u3v {

// Enumerate all U3V compatible cameras connected to the host.
std::vector<Camera<ControlHandle, StreamHandle>> enumerateCameras() {
    std::vector<DeviceInfo> devices = u3v::enumerateDevices();
    std::vector<Camera<ControlHandle, StreamHandle>> cameras;

    for (const auto& dev : devices) {
        ControlHandle ctrl(dev);
        if (!ctrl.isValid()) {
            continue;
        }

        StreamHandle strm(dev);
        if (!strm.isValid()) {
            continue;
        }

        CameraInfo camera_info{ dev.vendorName, dev.modelName, dev.serialNumber };
        cameras.emplace_back(ctrl, strm, nullptr, camera_info);
    }

    return cameras;
}

// Convert u3v::Error to ControlError
ControlError convertToControlError(const u3v::Error& err) {
    switch (err.type()) {
        case u3v::ErrorType::LibUsb: {
            const auto libusb_error = std::get<u3v::LibUsbError>(err);
            switch (libusb_error) {
                // Handle different LibUsb errors and convert to ControlError
                // ...
            }
        }
        case u3v::ErrorType::BufferIo:
        case u3v::ErrorType::InvalidPacket:
            return ControlError(ControlErrorType::Io, err.what());
        case u3v::ErrorType::InvalidDevice:
            return ControlError(ControlErrorType::InvalidDevice, "invalid device");
    }
}

// Convert u3v::Error to StreamError
StreamError convertToStreamError(const u3v::Error& err) {
    switch (err.type()) {
        case u3v::ErrorType::LibUsb: {
            const auto libusb_error = std::get<u3v::LibUsbError>(err);
            switch (libusb_error) {
                // Handle different LibUsb errors and convert to StreamError
                // ...
            }
        }
        default:
            return StreamError(StreamErrorType::Io, err.what());
    }
}

} // namespace u3v

