#include "IMUHandler.h"

IMUHandler::IMUHandler() : imuData{0.0f, 0.0f, 0.0f, 0.0f} {}

bool IMUHandler::initialize() {
    auto imu_type = M5.Imu.getType();
    if (imu_type == m5::imu_none) {
        return false;
    }
    M5.Imu.init();
    return true;
}

bool IMUHandler::update() {
    if (M5.Imu.update()) {
        auto data = M5.Imu.getImuData();
        imuData.gyro_x = data.gyro.x;
        imuData.gyro_y = data.gyro.y;
        imuData.gyro_z = data.gyro.z;
        imuData.accel_x = data.accel.x;
        return true;
    }
return false;
}

IMUData IMUHandler::getData() const {
    return imuData;
}
