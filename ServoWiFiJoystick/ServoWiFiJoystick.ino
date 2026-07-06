/*
  ============================================================
   R4 SERVO CONTROL - Joystick WiFi
  ============================================================
   Arduino UNO R4 WiFi + Servo posicional controlado desde
   una pagina web con joystick tactil.

   CONEXION DEL SERVO:
     - Señal (naranja/amarillo) -> pin D9
     - VCC (rojo)               -> alimentacion externa 5V
     - GND (marron/negro)       -> GND comun con el Arduino

   Antes de subir el codigo, completa tu SSID y PASSWORD
   de tu red WiFi de casa mas abajo.
  ============================================================
*/

#include <WiFiS3.h>
#include <Servo.h>

// ======== CONFIGURA TU WIFI ACA ========
const char* ssid     = "TU_WIFI";
const char* password = "TU_PASSWORD";
// ========================================

WiFiServer server(80);
Servo miServo;
int angulo = 90;

const int SERVO_PIN = 9;

void setup() {
  Serial.begin(115200);

  miServo.attach(SERVO_PIN);
  miServo.write(angulo);

  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 30) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Conectado! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("No se pudo conectar. Revisa SSID/PASSWORD.");
  }

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String reqLine = "";
  while (client.connected() && client.available()) {
    char c = client.read();
    reqLine += c;
    if (c == '\n') break;
  }

  // Descartar el resto de los headers rapido
  while (client.available()) client.read();

  if (reqLine.indexOf("GET /set?ang=") != -1) {
    int idx = reqLine.indexOf("ang=") + 4;
    int fin = reqLine.indexOf(' ', idx);
    int nuevoAngulo = reqLine.substring(idx, fin).toInt();
    nuevoAngulo = constrain(nuevoAngulo, 0, 180);
    angulo = nuevoAngulo;
    miServo.write(angulo);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Access-Control-Allow-Origin: *");
    client.println("Connection: close");
    client.println();
    client.print(angulo);
  } else {
    // Servir la pagina principal
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.print(paginaHTML());
  }

  delay(1);
  client.stop();
}

String paginaHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>R4 Servo Control</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    min-height: 100vh;
    display: flex;
    align-items: center;
    justify-content: center;
    font-family: 'Segoe UI', system-ui, sans-serif;
    background: radial-gradient(circle at 20% 20%, #1e0b3f, #05010f 70%);
    color: #fff;
    overflow: hidden;
    user-select: none;
  }
  body::before, body::after{
    content:"";
    position:fixed;
    width:400px; height:400px;
    border-radius:50%;
    filter: blur(90px);
    opacity:0.35;
    z-index:0;
  }
  body::before{ background:#8a2be2; top:-100px; left:-100px; }
  body::after{ background:#00e5ff; bottom:-120px; right:-100px; }

  .card {
    position: relative;
    z-index: 1;
    width: 340px;
    padding: 28px 26px 34px;
    border-radius: 24px;
    background: rgba(255,255,255,0.06);
    backdrop-filter: blur(18px);
    border: 1px solid rgba(255,255,255,0.12);
    box-shadow: 0 20px 50px rgba(0,0,0,0.5);
    text-align: center;
  }

  .logo {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 10px;
    margin-bottom: 4px;
  }
  .logo svg { width: 34px; height: 34px; }
  .logo-text {
    font-size: 22px;
    font-weight: 700;
    letter-spacing: 0.5px;
    background: linear-gradient(90deg, #b26bff, #4dd8ff);
    -webkit-background-clip: text;
    background-clip: text;
    color: transparent;
  }
  .subtitle {
    font-size: 12px;
    color: #a9a3c2;
    margin-bottom: 22px;
    letter-spacing: 1px;
    text-transform: uppercase;
  }

  .angle-display {
    font-size: 46px;
    font-weight: 800;
    margin-bottom: 4px;
    text-shadow: 0 0 18px rgba(138,43,226,0.7);
  }
  .angle-label {
    font-size: 11px;
    color: #a9a3c2;
    margin-bottom: 20px;
    letter-spacing: 2px;
  }

  .joystick-wrap {
    display: flex;
    justify-content: center;
    margin-bottom: 10px;
  }
  .joystick-base {
    position: relative;
    width: 220px;
    height: 220px;
    border-radius: 50%;
    background: radial-gradient(circle at 35% 30%, rgba(255,255,255,0.08), rgba(255,255,255,0.02));
    border: 1px solid rgba(255,255,255,0.15);
    box-shadow: inset 0 6px 18px rgba(0,0,0,0.5), 0 0 40px rgba(138,43,226,0.15);
    touch-action: none;
  }
  .joystick-track {
    position:absolute;
    top:50%; left:10px; right:10px;
    height:2px;
    background: rgba(255,255,255,0.12);
  }
  .joystick-knob {
    position: absolute;
    width: 64px;
    height: 64px;
    border-radius: 50%;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    background: linear-gradient(145deg, #b26bff, #6f2fd6);
    box-shadow: 0 8px 20px rgba(138,43,226,0.6), inset 0 2px 6px rgba(255,255,255,0.4);
    cursor: grab;
    transition: box-shadow 0.15s;
  }
  .joystick-knob:active { cursor: grabbing; box-shadow: 0 4px 30px rgba(0,229,255,0.7); }

  .status {
    margin-top: 18px;
    font-size: 11px;
    color: #6fffb0;
    display: flex;
    align-items: center;
    justify-content: center;
    gap: 6px;
  }
  .dot {
    width: 8px; height: 8px;
    border-radius: 50%;
    background: #6fffb0;
    box-shadow: 0 0 8px #6fffb0;
    animation: pulse 1.4s infinite;
  }
  @keyframes pulse {
    0%,100% { opacity: 1; }
    50% { opacity: 0.3; }
  }
</style>
</head>
<body>
  <div class="card">
    <div class="logo">
      <svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg">
        <circle cx="32" cy="32" r="30" stroke="url(#g1)" stroke-width="3"/>
        <path d="M32 14 L36 24 L28 24 Z" fill="url(#g1)"/>
        <circle cx="32" cy="32" r="8" fill="url(#g1)"/>
        <defs>
          <linearGradient id="g1" x1="0" y1="0" x2="64" y2="64" gradientUnits="userSpaceOnUse">
            <stop stop-color="#b26bff"/>
            <stop offset="1" stop-color="#4dd8ff"/>
          </linearGradient>
        </defs>
      </svg>
      <div class="logo-text">R4 ServoWave</div>
    </div>
    <div class="subtitle">Control WiFi</div>

    <div class="angle-display" id="angleDisplay">90&deg;</div>
    <div class="angle-label">ANGULO ACTUAL</div>

    <div class="joystick-wrap">
      <div class="joystick-base" id="base">
        <div class="joystick-track"></div>
        <div class="joystick-knob" id="knob"></div>
      </div>
    </div>

    <div class="status"><span class="dot"></span> Conectado</div>
  </div>

<script>
  const base = document.getElementById('base');
  const knob = document.getElementById('knob');
  const angleDisplay = document.getElementById('angleDisplay');

  const radius = 78; // recorrido maximo del knob desde el centro
  let dragging = false;
  let lastSent = -1;
  let sendTimer = null;

  function setKnob(x) {
    knob.style.transform = `translate(calc(-50% + ${x}px), -50%)`;
  }

  function xToAngle(x) {
    // x va de -radius a +radius -> angulo de 0 a 180
    const t = (x + radius) / (radius * 2);
    return Math.round(t * 180);
  }

  function enviarAngulo(ang) {
    if (ang === lastSent) return;
    lastSent = ang;
    fetch('/set?ang=' + ang).catch(() => {});
  }

  function handleMove(clientX) {
    const rect = base.getBoundingClientRect();
    let x = clientX - (rect.left + rect.width / 2);
    x = Math.max(-radius, Math.min(radius, x));
    setKnob(x);
    const ang = xToAngle(x);
    angleDisplay.textContent = ang + '\u00b0';

    clearTimeout(sendTimer);
    sendTimer = setTimeout(() => enviarAngulo(ang), 40);
  }

  function releaseKnob() {
    dragging = false;
  }

  // Mouse
  knob.addEventListener('mousedown', () => dragging = true);
  window.addEventListener('mousemove', (e) => { if (dragging) handleMove(e.clientX); });
  window.addEventListener('mouseup', releaseKnob);

  // Touch
  knob.addEventListener('touchstart', (e) => { dragging = true; e.preventDefault(); }, {passive:false});
  window.addEventListener('touchmove', (e) => {
    if (dragging) { handleMove(e.touches[0].clientX); e.preventDefault(); }
  }, {passive:false});
  window.addEventListener('touchend', releaseKnob);

  // Posicion inicial centrada (90 grados)
  setKnob(0);
</script>
</body>
</html>
)rawliteral";
  return html;
}
