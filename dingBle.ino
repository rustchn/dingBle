
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "esp_sleep.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t temprature_sens_read();
//uint8_t g_phyFuns;

#ifdef __cplusplus
}
#endif

BLEAdvertising *pAdvertising;

//uint8_t bleMac[6] = {0x30, 0xAE, 0xA4, 0x17, 0x28, 0x25}; 
//uint8_t bleRaw[31] = {0x02, 0x01, 0x1A, 0x17, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x3C, 0xFE};
//大厅
//uint8_t bleRaw[31] = {0x02, 0x01, 0x06, 0x17, 0xFF, 0x00, 0x01, 0xB5, 0x00, 0x02, 0x7C, 0x12, 0xC0, 0x20, 0x00, 0x00, 0x00, 0x09, 0xD8, 0x82, 0x07, 0x09, 0x01, 0x10, 0x00, 0x00, 0x00, 0x03, 0x03, 0x3C, 0xFE};



const char * hex_str;
//大厅

//uint8_t bleMac[6] = {0x30, 0xAE, 0xA4, 0x17, 0x28, 0x25};
uint8_t bleMac[6] = {0x30, 0xAE, 0xA4, 0x16, 0xF1, 0x05};
uint8_t bleRaw[31] = {0};


void setup() {
//  bleMac = {0x30, 0xAE, 0xA4, 0x17, 0x28, 0x25};
  hex_str = "02010617FF0001B500024D8A74FA00000009D0574DE2011000000003033CFE";
  
  Serial.begin(115200);
  //数据转化
  Serial.println(hex_str);
  int m = HexStrToByte(hex_str,strlen(hex_str),bleRaw);
  Serial.println(m);

  // esp32没有提供设置蓝牙mac地址的api 通过查看esp32的源代码
  // 此操作将根据蓝牙mac算出base mac
  if (UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR) {
    bleMac[5] -= 2;
  } else if (UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR) {
    bleMac[5] -= 1;
  }
  esp_base_mac_addr_set(bleMac);

  // 初始化
  BLEDevice::init("");

  // Create the BLE Server
  // BLEServer *pServer = BLEDevice::createServer(); // <-- no longer required to instantiate BLEServer, less flash and ram usage

  pAdvertising = BLEDevice::getAdvertising();

  // 设备信息设置成空白的
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  pAdvertising->setScanResponseData(oScanResponseData);

  // 里面有个 m_customScanResponseData = true; 和 m_customScanResponseData = true; 所以只能先随便设置一下
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  pAdvertising->setAdvertisementData(oAdvertisementData);

  // 简单粗暴直接底层api重新设置一下抓到的raw
  esp_err_t errRc = ::esp_ble_gap_config_adv_data_raw(bleRaw, 31);
  if (errRc != ESP_OK) {
    Serial.printf("esp_ble_gap_config_adv_data_raw: %d\n", errRc);
  }

  pAdvertising->start();
}

//source 被转换的16进制字符串
//sourceLen 被转换的16进制字符串长度
//dest 转换后的16进制数组
//返回值 转换后的16进制数组长度
int HexStrToByte(const char* source, int sourceLen, uint8_t* dest)
{
  short i;
  uint8_t highByte, lowByte;
  int len = 0;
  for (i = 0; i < sourceLen; i += 2)
  {
    highByte = toupper(source[i + 0]);
    lowByte  = toupper(source[i + 1]);

    if (highByte > 0x39)
      highByte -= 0x37;
    else
      highByte -= 0x30;

    if (lowByte > 0x39)
      lowByte -= 0x37;
    else
      lowByte -= 0x30;

    dest[i / 2] = (highByte << 4) | lowByte;
    len++;
  }
  return len;
}


uint8_t convert( char * src) {
  uint8_t ret = 0, d;
  while (*src) {
    if ( *src >= '0' && *src <= '9') d = *src - '0';
    else if ( *src >= 'a' && *src <= 'f') d = *src - 'a' + 10;
    else if ( *src >= 'A' && *src <= 'Z') d = *src - 'A' + 10;
    else {
      Serial.printf("字符串有不可识别字符。");
      return 0;
    };
    ret = ret * 16 + d;
    src ++;
  } return ret;
}

void loop() {
  // 闪灯灯
  Serial.println("Sparkle");
  delay(1000);
  // 20分钟去待机避免忘了关
  if (millis() > 1200000) {
    esp_deep_sleep_start();
  }
}
