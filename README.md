## video

[Ver video de la UI en funcionamiento](media/previw_ui.mp4)



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

## ⚙️ Cómo funciona `ServoWiFiJoystick.ino`

El sketch combina tres partes: **conexión WiFi**, un **mini servidor web** corriendo en el Arduino, y una **página HTML con joystick** que ese servidor le manda al navegador.

### 1. Conexión a la red (modo Station)

En el `setup()`, el Arduino se conecta a tu router con `WiFi.begin(ssid, password)`, igual que lo haría un celular. Una vez conectado, `WiFi.localIP()` te da la IP que le asignó el router por DHCP — esa es la dirección que después escribís en el navegador.

### 2. El servidor web (`WiFiServer`)

El Arduino levanta un servidor HTTP muy simple en el puerto 80 con `WiFiServer server(80)`. En el `loop()`, hace básicamente esto:

1. Espera a que un cliente (tu navegador) se conecte: `server.available()`.
2. Lee la primera línea del pedido HTTP, algo como:
   ```
   GET /set?ang=120 HTTP/1.1
   ```
3. Según lo que dice esa línea, decide qué responder:
   - Si el pedido es `GET /set?ang=NNN` → es el joystick pidiendo mover el servo a `NNN` grados. El Arduino extrae ese número del texto, lo limita entre 0 y 180 con `constrain()`, mueve el servo con `miServo.write(angulo)`, y responde solo con el número (texto plano) para confirmar.
   - Si el pedido es cualquier otra cosa (por ejemplo, cuando entrás por primera vez a `http://IP`) → responde con la página HTML completa (la del joystick), generada por la función `paginaHTML()`.

No usa ninguna librería de servidor "pesada": todo el HTTP se maneja a mano, leyendo texto crudo del socket. Por eso el código de parseo es tan simple (busca el texto `"ang="` dentro del pedido).

### 3. La página HTML/CSS/JS (lo que ves en el navegador)

Todo el frontend vive como un string gigante (`R"rawliteral(...)"`) dentro de `paginaHTML()`. Ese string se manda completo al navegador la primera vez que entrás a la IP. Una vez cargado, **el navegador ya no depende del Arduino para dibujar nada** — solo le manda pedidos cortos cada vez que movés el joystick.

Dentro de esa página:

- **HTML**: define la tarjeta, el logo, el número grande del ángulo y el círculo del joystick (`#base` y `#knob`).
- **CSS**: le da el estilo "glassmorphism" (blur, transparencias, gradientes) y las animaciones (el punto verde pulsante, las sombras con brillo).
- **JavaScript** es la parte más importante funcionalmente:
  - Escucha eventos de mouse (`mousedown`, `mousemove`, `mouseup`) y de touch (`touchstart`, `touchmove`, `touchend`) sobre el knob del joystick.
  - Cuando arrastrás, calcula cuántos píxeles se movió el knob desde el centro (`handleMove`), lo limita a un radio máximo (`radius = 78`), y con `xToAngle()` convierte esa posición en un ángulo de 0 a 180 grados.
  - Actualiza el número en pantalla al instante (sin esperar respuesta del Arduino, para que se sienta fluido).
  - Con un pequeño **debounce de 40ms** (`setTimeout`), manda el pedido `fetch('/set?ang=' + ang)` al Arduino — así, aunque arrastres rápido, no lo bombardea con cientos de pedidos por segundo.
  - Guarda el último ángulo enviado (`lastSent`) para no repetir pedidos si el ángulo no cambió.

### Resumen del recorrido de un movimiento

```
Arrastrás el knob (dedo/mouse)
        ↓
JavaScript calcula el ángulo (0–180°)
        ↓
fetch('/set?ang=120')  →  el navegador manda un HTTP GET
        ↓
Arduino lee "ang=120" del pedido
        ↓
miServo.write(120)  →  el servo gira
        ↓
Arduino responde "120" como confirmación
```

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
