#ifndef IMU_HANDLER_H
#define IMU_HANDLER_H

#include <M5Unified.h>

struct IMUData {
    float gyro_x;
    float gyro_y;
    float gyro_z;
};

class IMUHandler {
public:
    IMUHandler();
    bool initialize();
    bool update();
    IMUData getData() const;

private:
    IMUData imuData;
};

#endif
