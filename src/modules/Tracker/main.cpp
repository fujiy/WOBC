// #define NDEBUG

#include <library/wobc.h>
#include <components/LiPoPower/lipo_power.h>
#include <components/LoRa/lora.h>
#include <components/Pressure/pressure.h>
#include <components/GPS/gps.h>
#include <components/Logger/logger.h>
#include <SPI.h>

#define SPI0_SCK_PIN 1
#define SPI0_MOSI_PIN 4
#define SPI0_MISO_PIN 3
#define SPI0_CS_PIN 2

#define SD_INSERTED_PIN 5
#define SDCARD_MOSI_PIN SPI0_MOSI_PIN
#define SDCARD_MISO_PIN SPI0_MISO_PIN
#define SDCARD_SS_PIN SPI0_CS_PIN
#define SDCARD_SCK_PIN SPI0_SCK_PIN

#define ST 6
#define PG 10
#define STAT1 43
#define STAT2 44
#define HEAT 9
#define CHARGELED 8
#define TEMP 7

#define LORA_CHANNEL 3
#define LORA_TX_PIN 38
#define LORA_RX_PIN 39
#define LORA_AUX_PIN 40
#define LORA_M0_PIN 12
#define LORA_M1_PIN 11

constexpr uint8_t module_id = 0x54;
constexpr uint8_t unit_id = 0x61;

HardwareSerial lora_serial(1);
core::SerialBus serial_bus(Serial);

component::LiPoPower power(Wire, ST, PG, STAT1, STAT2, HEAT, CHARGELED, TEMP, unit_id, 1);
component::LoRa lora(LORA_AUX_PIN, LORA_M0_PIN, LORA_M1_PIN, LORA_TX_PIN, LORA_RX_PIN, LORA_CHANNEL, 0);
component::Logger logger(SPI, SPI0_CS_PIN, SD_INSERTED_PIN);
component::Pressure pressure(Wire, unit_id);
component::GPS gps(47, 48, 115200, unit_id);


interface::WatchIndicator<unsigned> status_indicator(42, kernel::packetCount());
interface::WatchIndicator<unsigned> error_indicator(41, kernel::errorCount());

class Main : public process::Component {
public:
    Main() : process::Component("main", 0x00) {}
    kernel::Listener my_listener_;
    kernel::Listener heartbeat_;

    void setup() override {
        my_listener_.telemetry(); 
        listen(my_listener_, 8);
        heartbeat_.component(0x54);
        listen(heartbeat_,1);
    }

    void loop() override {
        while (my_listener_) {
            wcpp::Packet packet = my_listener_.pop();
                wcpp::Packet lorapacket = newPacket(64);
                auto im = packet.find("Im");
                if(!im){
                    lorapacket.command(lora.send_command_id, lora.component_id_base + 0);
                    lorapacket.append("Pa").setPacket(packet);
                    sendPacket(lorapacket);
                }
            }
        }
    }main_;

void setup() {
    Serial.begin(115200);
    kernel::setUnitId(unit_id);
    if (!kernel::begin(module_id, true)) return;

    Serial0.setPins(2, 1);
    Wire.begin(17, 16);
    serial_bus.begin();

    SPI.begin(SDCARD_SCK_PIN, SDCARD_MISO_PIN, SDCARD_MOSI_PIN, SDCARD_SS_PIN);

    delay(1000); 

    status_indicator.begin();
    status_indicator.blink_on_change();

    error_indicator.begin();
    error_indicator.set(true);

    power.begin();
    lora.begin();
    pressure.begin();
    gps.begin();
    logger.begin();
    main_.begin();

    error_indicator.set(false);
    error_indicator.blink_on_change(100);
}

void loop() {
    status_indicator.update();
    error_indicator.update();
}
