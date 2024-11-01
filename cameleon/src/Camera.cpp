#include "Camera.h"

namespace u3v {

Camera::Camera(const std::string& vendorName, const std::string& modelName, const std::string& serialNumber) :
        m_vendorName(vendorName), m_modelName(modelName), m_serialNumber(serialNumber) {}

const std::string& Camera::getVendorName() const { return m_vendorName; }
void Camera::setVendorName(const std::string& vendorName) { m_vendorName = vendorName; }

const std::string& Camera::getModelName() const { return m_modelName; }
void Camera::setModelName(const std::string& modelName) { m_modelName = modelName; }

const std::string& Camera::getSerialNumber() const { return m_serialNumber; }
void Camera::setSerialNumber(const std::string& serialNumber) { m_serialNumber = serialNumber; }

} // namespace u3v

