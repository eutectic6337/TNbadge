#ifndef BADGE_WIFI_H_
#define BADGE_WIFI_H_

const uint8_t WIFI_PASSWORD_LENGTH = 20;
char WiFi_password[WIFI_PASSWORD_LENGTH+1] = {0};

#ifdef ENABLE_WIFI
//284050
//732912
#include <WiFi.h>
const Time WIFI_STABILITY_ms = 100;
#define SSIDprefix "TNbadge"
char SSID[(sizeof SSIDprefix)+(sizeof MAC)*2] = SSIDprefix;
const char WiFi_password_alphabet[] =
  "abcdefghijklmnoppqrstuvwxyz[];,./"
  "ABCDEFGHIJKKLMNOPQRSTUVWXYZ{}:<>?"
  "`1234567890-="
  "~!@#$%^&*()_+"
  "\\|'\"";

// IDEA: probably no need to even connect to AP, let alone server 
#if 0
WiFiServer server(80);
#endif

/* IDEA:
   find out what friends are nearby through
   - broadcasting our own SSID,
   - scanning for SSIDs,
   - filtering by known SSID prefix,
   - sorting by RSSID
 */

/* IDEA:
   use hardware crypto engine

   #include "esp_secure_cert_crypto.h"
   #include "esp_secure_cert_read.h"
   #include "rom/aes.h"
   #include "rom/digital_signature.h"
   #include "rom/hmac.h"
   #include "rom/sha.h"

   (also in the SDK ...)
   #include "sodium.h" //https://doc.libsodium.org/
 */

/* Arduino Cryptography Library by Rhys Weatherley
   https://rweather.github.io/arduinolibs/
   #include <RNG.h>
   #include <AES.h>
   #include <SHA256.h>
   #include <SHA3.h>
   #include <Ed25519.h>
 */

 /* possibly useful magic numbers for API

 may want to tweak Tx power
 .setTxPower()
    WIFI_POWER_19_5dBm
    WIFI_POWER_19dBm
    WIFI_POWER_18_5dBm
    WIFI_POWER_17dBm
    WIFI_POWER_15dBm
    WIFI_POWER_13dBm
    WIFI_POWER_11dBm
    WIFI_POWER_8_5dBm
    WIFI_POWER_7dBm
    WIFI_POWER_5dBm
    WIFI_POWER_2dBm
    WIFI_POWER_MINUS_1dBm

sadly, doesn't look like we can use other than WPA2 or OPEN on soft-AP
.setMinSecurity()
    WIFI_AUTH_WPA3_PSK
    WIFI_AUTH_WPA2_WPA3_PSK
    WIFI_AUTH_WPA2_PSK

    WIFI_AUTH_OPEN
    WIFI_AUTH_WEP
    WIFI_AUTH_WPA_PSK
    WIFI_AUTH_WPA_WPA2_PSK
    WIFI_AUTH_ENTERPRISE
    WIFI_AUTH_WPA2_ENTERPRISE
    WIFI_AUTH_WAPI_PSK
    WIFI_AUTH_WPA3_ENT_192
    WIFI_AUTH_MAX

 */
void setup_WiFi()
{
  // ======== CUSTOMIZE WiFi HERE ========
  sprintf(SSID + (sizeof SSIDprefix) - 1, "%llX", MAC);
  LOG("SSID:");LOGln(SSID);
  // generate a completely random password, if not already pulled from prefs
  if (WiFi_password[0] == 0) {
    for (int i = 0; i < WIFI_PASSWORD_LENGTH; i++) {
      WiFi_password[i] = WiFi_password_alphabet[random((sizeof WiFi_password_alphabet)-1)];
    }
    WiFi_password[(sizeof WiFi_password)-1] =0;
  }
  LOG("WiFi password:");LOGln(WiFi_password);

  // Set to station+AP mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  delay(WIFI_STABILITY_ms);

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(SSID, WiFi_password)) {
    LOGln("Soft AP creation failed.");
    return;
  }
  IPAddress myIP = WiFi.softAPIP();
  LOG("AP IP address:");LOGln(myIP);
#if 0
  server.begin();
#endif
}
void loop_WiFi()
{
  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks(/*async=*/true);
  //(, bool show_hidden = false, bool passive = false, uint32_t max_ms_per_chan = 300, uint8_t channel = 0, const char * ssid=nullptr, const uint8_t * bssid=nullptr)
  if (n == WIFI_SCAN_RUNNING || n == WIFI_SCAN_FAILED) return;

  unsigned friends = 0;
  int32_t max_signal, min_signal;
  for (int i = 0; i < n; ++i) {
    // look at SSID and RSSI for each network found
    const char* s = WiFi.SSID(i).c_str();
    if (strstr(s, SSIDprefix) != s) continue;// not one of ours

    friends++;
    int32_t r = WiFi.RSSI(i);
    if (friends == 1 || r > max_signal) max_signal = r;
    if (friends == 1 || r < min_signal) min_signal = r;

    //FIXME: do something with encryptionType; maybe disown OPEN/WEP/WPA ?
    switch (WiFi.encryptionType(i))
    {
    case WIFI_AUTH_OPEN:
        Serial.print("open");
        break;
    case WIFI_AUTH_WEP:
        Serial.print("WEP");
        break;
    case WIFI_AUTH_WPA_PSK:
        Serial.print("WPA");
        break;
    case WIFI_AUTH_WPA2_PSK:
        Serial.print("WPA2");
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        Serial.print("WPA+WPA2");
        break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
        Serial.print("WPA2-EAP");
        break;
    case WIFI_AUTH_WPA3_PSK:
        Serial.print("WPA3");
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        Serial.print("WPA2+WPA3");
        break;
    case WIFI_AUTH_WAPI_PSK:
        Serial.print("WAPI");
        break;
    default:
        Serial.print("unknown");
    }
  }
  //FIXME: make result available to rest of program,
  //       c.f. pushbutton_debounced, lightdark_smoothed

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();
}
#else
#define setup_WiFi() ((void)0)
#define loop_WiFi() ((void)0)
#endif

#endif//BADGE_WIFI_H_
