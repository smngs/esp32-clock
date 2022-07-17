# esp32-clock

## Depends

* platformio
  * Adafruit SSD1306
  * Adafruit BME680 Library
  * Adafruit Unified Sensor

## Usage

### Compile

```sh
$ cat << EOF > src/secret.h
    const char* SSID = "<ssid>";
    const char* WIFI_PASSWORD = "<password>";
EOF
  
$ pio run
```

### Upload

```sh
$ pio run -t upload
```

### Upload (via ota)

```sh
$ pio run -t upload --upload-port '<ip-address>'
```
