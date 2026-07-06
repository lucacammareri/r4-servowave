# R4 ServoWave 🕹️

Control de un servo posicional vía WiFi usando un **Arduino UNO R4 WiFi**, con una interfaz web tipo joystick.

## 📦 Contenido del repo

```
.
├── ServoWiFiJoystick/
│   └── ServoWiFiJoystick.ino   # Sketch principal: servidor web + joystick + control del servo
├── DiagnosticoWiFi/
│   └── DiagnosticoWiFi.ino     # Sketch de diagnóstico para probar la conexión WiFi
└── README.md
```

## 🔧 Hardware necesario

- Arduino UNO R4 WiFi
- Servo posicional (0°–180°)
- Fuente externa de 5V para el servo (recomendado, sobre todo si es un servo grande)

### Conexión del servo

| Cable del servo       | Va a                          |
|------------------------|-------------------------------|
| Señal (naranja/amarillo) | Pin **D9** del Arduino       |
| VCC (rojo)             | Alimentación externa **5V**   |
| GND (marrón/negro)     | GND común con el Arduino      |

> ⚠️ No alimentes servos grandes desde el pin 5V del Arduino: puede resetear la placa.

## 🚀 Cómo usarlo

1. Abrí `ServoWiFiJoystick/ServoWiFiJoystick.ino` en el Arduino IDE.
2. Completá tu red WiFi (debe ser **2.4GHz**, el módulo del R4 no soporta 5GHz):

   ```cpp
   const char* ssid     = "TU_WIFI";
   const char* password = "TU_PASSWORD";
   ```

3. Subí el sketch a la placa.
4. Abrí el Monitor Serie (115200 baudios) y esperá el mensaje:

   ```
   Conectado! IP: 192.168.x.x
   ```

5. Desde un celular o PC conectado **a la misma red WiFi**, abrí esa IP en el navegador.
6. Usá el joystick para mover el servo. El ángulo actual se muestra en pantalla.

## 🩺 Si no conecta o no carga la página

Usá `DiagnosticoWiFi/DiagnosticoWiFi.ino`: te informa la versión de firmware del módulo WiFi, el estado de la conexión y la IP asignada, para descartar problemas de credenciales, banda de red o firmware desactualizado.

Problemas comunes:
- Red de **5GHz** en vez de 2.4GHz (no soportada).
- SSID/password mal escritos.
- Firmware del módulo WiFi desactualizado (actualizar desde *Herramientas > Firmware Updater* en el IDE).
- **Aislamiento de clientes (client isolation)** activado en el router (común en redes de instituciones): bloquea que dispositivos de la misma red se vean entre sí.
- La IP asignada por DHCP cambió desde la última vez.

## 🛣️ Próximos pasos / ideas

- [ ] Modo Access Point como alternativa (sin depender de un router externo)
- [ ] Botón para centrar el servo en 90°
- [ ] Guardar la última posición en memoria (EEPROM)
- [ ] WebSocket para respuesta más fluida en tiempo real

## 📄 Licencia

Uso educativo / libre para modificar y compartir.
