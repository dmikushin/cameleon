#include <iostream>
#include <vector>
#include <string>

#include "u3v.h"

using namespace std;
using namespace u3v;

int main() {
    // Enumerates cameras connected to the host.
    vector<Camera> cameras = enumerateCameras();

    // If no camera is found, return.
    if (cameras.empty()) {
        return 0;
    }

    Camera& camera = cameras.back();
    // Opens the camera.
    camera.open();

    Control* ctrl = &camera.ctrl;
    // Get Abrm.
    Abrm abrm = ctrl->abrm();

    // Read serial number from ABRM.
    string serial_number = abrm.serial_number(ctrl);
    cout << serial_number << endl;

    // Check user defined name feature is supported.
    // If it is suppoted, read from and write to the register.
    DeviceCapability device_capability = abrm.device_capability();
    if (device_capability.is_user_defined_name_supported()) {
        // Read from user defined name register.
        string user_defined_name = abrm.user_defined_name(ctrl);
        cout << user_defined_name << endl;

        // Write new name to the register.
        abrm.set_user_defined_name(ctrl, "cameleon");
    }

    return 0;
}

