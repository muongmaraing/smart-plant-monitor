# ម៉ូនីទ័រដើមឈើ — Smart Plant Monitor

កម្មវិធីវិភាគ **ទឹក · កម្តៅ · ពន្លឺ · ជាតិជី (EC)** សម្រាប់ដើមឈើ/ដំណាំ
ដំណើរការលើ **ទូរស័ព្ទ និងកុំព្យូទ័រ** ដោយមិនត្រូវការអ៊ីនធឺណិត។

## ១. ឯកសារនៅក្នុង folder នេះ

| ឯកសារ | មុខងារ |
|---|---|
| `index.html` | App ទាំងមូល (dashboard + វិភាគរោគសញ្ញាស្លឹក) — ១ ឯកសារតែមួយ |
| `manifest.webmanifest` | ធ្វើឲ្យ Install ជា App លើទូរស័ព្ទ (រូបតំណាង + ឈ្មោះ) |
| `sw.js` | Service Worker — ដំណើរការ offline |
| `icon-192.png` / `icon-512.png` | រូបតំណាង App |
| `firmware_esp32.ino` | Firmware ESP32 (អាន sensor + បញ្ចេញ JSON នៅ `/data`) |
| `_preview_phone.png` | រូបភាពមើលជាមុន |

## ២. ប្រើឥឡូវនេះ (គ្មានឧបករណ៍)

**លើកុំព្យូទ័រ:** ចុចពីរដង `index.html` → បើកក្នុង Chrome/Edge។
(បង្ហាញទិន្នន័យ Demo ២៤ ម៉ោង)

**លើទូរស័ព្ទ (WiFi ដូចកុំព្យូទ័រ):**
```bat
cd C:\Users\Maraing\OneDrive\Documents\SmartPlantMonitor
python -m http.server 8099
```
រួចបើកលើទូរស័ព្ទ: `http://<IP-កុំព្យូទ័រ>:8099`
→ Chrome menu → **Add to Home screen / ដំឡើង App** → មានរូបតំណាង App លើ home screen។

## ៣. ភ្ជាប់ sensor ពិត (ESP32)

1. ដំឡើង Arduino IDE → Library Manager: `DHT sensor library`, `Adafruit Unified Sensor`, `BH1750`
2. កែ `WIFI_SSID` / `WIFI_PASS` ក្នុង `firmware_esp32.ino` → Upload (Board: ESP32 Dev Module)
3. Serial Monitor នឹងបង្ហាញ IP → យក IP នោះដាក់ក្នុង App (ប្រអប់ "ការតភ្ជាប់")
4. ដោះធីក "ប្រើទិន្នន័យ Demo" → App ទាញទិន្នន័យពិតរាល់ ៥ វិនាទី

### ខ្សែភ្ជាប់

| ឧបករណ៍ | ESP32 | ចំណាំ |
|---|---|---|
| Capacitive soil moisture | GPIO 34 | analog, បញ្ចូល ៣.៣V |
| DHT22 | GPIO 4 | ១០kΩ pull-up |
| BH1750 (I2C) | SDA 21, SCL 22 | ៣.៣V |
| EC sensor (SEN0161) | GPIO 35 | analog |
| Relay + pump | GPIO 26 | ប្រើ relay module មាន opto |

### Calibration សំណើម
ដាក់ sensor ក្នុងខ្យល់ → អាន `analogRead` = `SOIL_DRY`
ដាក់ក្នុងទឹក → អាន = `SOIL_WET`

## ៤. តម្លៃឧបករណ៍ (ប្រហែល)

| ឧបករណ៍ | តម្លៃ USD |
|---|---|
| ESP32 DevKit | 5–8 |
| Capacitive soil moisture | 2–4 |
| DHT22 | 2–3 |
| BH1750 | 1.5–3 |
| EC sensor SEN0161 | 12–20 |
| Relay + mini pump | 6–10 |
| **សរុប (មូលដ្ឋាន)** | **≈ 20–30** |
| pH sensor (ស្រេចចិត្ត) | 15–25 |
| NPK RS485 sensor (ស្រេចចិត្ត) | 40–70 |

## ៥. អ្វីដែល sensor ដឹង និងមិនដឹង

**ដឹងបានច្បាស់ (ភាពជឿជាក់ខ្ពស់):**
- ខ្វះទឹក / សើមពេក → soil moisture %
- កម្តៅខ្ពស់ ខ្យល់ស្ងួត → heat stress
- ពន្លឺមិនគ្រប់ → etiolation
- ជាតិជីសរុបតិច/ច្រើន (EC) → ត្រូវបំប៉ន ឬត្រូវលាងអំបិល

**មិនដឹងច្បាស់ (សង្ស័យបានតែប៉ុណ្ណោះ):**
- ខ្វះជីវជាតិជាក់លាក់ (N, P, K, Fe, Mg…) — EC ប្រាប់តែជាតិអំបិលសរុប
- ជំងឺផ្សិត / មេរោគ

**ដំណោះស្រាយសម្រាប់ជីវជាតិ:** ថតរូបស្លឹក → វិភាគដោយ AI (រូប + រោគសញ្ញា)
ឬកាន់តែជាក់លាក់: យកដីទៅពិនិត្យ (soil test) នៅមន្ទីរពិសោធន៍កសិកម្ម។

## ៦. ដាក់លើ GitHub Pages (ដំណើរការហើយ)

App ឡើងផ្ទាល់ហើយ — ទូរស័ព្ទបើកបានគ្រប់ទីកន្លែងដែលមាន internet ដោយមិនត្រូវការ PC ឬ LAN:

- URL: **https://muongmaraing.github.io/smart-plant-monitor/**
- Repo: https://github.com/muongmaraing/smart-plant-monitor (public)
- កែកូដនៅ PC → `git add -A && git commit -m "..."` → `git push` → ទូរស័ព្ទឃើញកំណែថ្មីក្នុង ~១ នាទី (sw.js ជា network-first ហើយ)

⚠️ ដែនកំណត់សំខាន់: ទំព័រ `https://` **មិនអាច**ហៅ `http://<ESP32-IP>/data` បានទេ (mixed content — browser បិទ)។
ដូច្នេះ URL ខាងលើប្រើបានពេញលេញសម្រាប់ **Demo mode** និងសម្រាប់ដំឡើងជា App;
ឯទិន្នន័យ sensor ពិត ត្រូវជ្រើស ១ ក្នុង ៣៖
1. ប្រើ LAN `http://` (ផ្នែក ២) — ទូរស័ព្ទ និង ESP32 នៅ WiFi តែមួយ
2. បើកឯកសារ `index.html` ក្នុងទូរស័ព្ទផ្ទាល់ (offline, file://) — ហៅ http បាន
3. ESP32 បញ្ជូនទៅ cloud (ThingSpeak/Blynk/MQTT) → app អានពី cloud (https ទាំងសងខាង) — ប្រើបានគ្រប់ទីកន្លែង

## ៧. ធ្វើជា APK ពិត (ពេលត្រូវការ)

PWA ដំឡើងបានលើទូរស័ព្ទដោយមិនត្រូវការ APK។ បើចង់បាន `.apk` ពិត:
1. ដាក់ folder នេះលើ **GitHub Pages** (https) → ទទួល URL
2. ចូល `pwabuilder.com` → ដាក់ URL → Download **Android APK**
3. ឬប្រើ **Android Studio / Capacitor** បើចង់បន្ថែម Bluetooth, notification

⚠️ ចំណាំ: បើ App ដំណើរការលើ `https://` នោះ browser នឹងហាមឃាត់ការហៅ `http://<ESP32-IP>/data`
(mixed content)។ ដូច្នេះ សម្រាប់ការប្រើក្នុងផ្ទះ/ការិយាល័យ គួរប្រើ LAN `http://` ដូចក្នុងផ្នែក ២។
សម្រាប់ផលិតផលពិត ត្រូវប្រើ MQTT/Cloud broker (ឧ. EMQX, Blynk, Firebase) ជំនួស។
