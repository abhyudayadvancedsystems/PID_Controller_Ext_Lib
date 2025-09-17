#pragma once
#include <driver/twai.h>

class CanRPM {
public:
    CanRPM(uint32_t canId, uint32_t timeoutMs=500)
      : _canId(canId), _timeoutMs(timeoutMs), _lastRpm(0), _lastMsgTime(0) {}

    bool update() {
      twai_message_t msg;
      if (twai_receive(&msg, pdMS_TO_TICKS(10)) == ESP_OK &&
          !msg.extd && msg.identifier == _canId && msg.data_length_code >= 2) {
        uint8_t A = msg.data[0];
        uint8_t B = msg.data[1];
        _lastRpm = ((uint16_t(A) << 8) | uint16_t(B)) / 4;
        _lastMsgTime = millis();
        return true;
      }
      return false;
    }

    bool commOk() const { return (millis() - _lastMsgTime) <= _timeoutMs; }

    uint16_t rpm() const { return _lastRpm; }

private:
    uint32_t _canId, _timeoutMs;
    uint16_t _lastRpm;
    unsigned long _lastMsgTime;
};
