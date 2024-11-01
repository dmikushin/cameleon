#ifndef CAMERA_H
#define CAMERA_H

#include <string>

namespace u3v {

class Camera {
public:
    // Constructor
    Camera(const std::string& vendorName, const std::string& modelName, const std::string& serialNumber);

    // Getters and setters for the member variables
    const std::string& getVendorName() const;
    void setVendorName(const std::string& vendorName);

    const std::string& getModelName() const;
    void setModelName(const std::string& modelName);

    const std::string& getSerialNumber() const;
    void setSerialNumber(const std::string& serialNumber);

private:
    // Member variables
    std::string m_vendorName;
    std::string m_modelName;
    std::string m_serialNumber;
};

} // namespace u3v

#endif // CAMERA_H
