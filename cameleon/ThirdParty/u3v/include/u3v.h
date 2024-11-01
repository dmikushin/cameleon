#ifndef U3V_H
#define U3V_H

#include "ControlHandle.h"
#include "StreamHandle.h"
#include "Camera.h"

#include <vector>

namespace u3v {

// Enumerate all U3V compatible cameras connected to the host.
std::vector<Camera<ControlHandle, StreamHandle>> enumerateCameras();

// Convert u3v::Error to ControlError
ControlError convertToControlError(const u3v::Error& err);

// Convert u3v::Error to StreamError
StreamError convertToStreamError(const u3v::Error& err);

} // namespace u3v

#endif // U3V_H

