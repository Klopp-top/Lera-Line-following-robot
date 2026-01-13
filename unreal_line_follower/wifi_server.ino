#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "config.h"

WebServer server(80);
Preferences prefs;

// Глобальные переменные параметров
float KP = 20.0;
float KI = 0.0;
float KD = 8.0;
int BASE_SPEED = 100;
int MAX_SPEED = 200;

// Массив для хранения значений датчиков
int sensorData[8] = {0};

// Состояние робота
extern bool isRunning;

// Ручное управление
int manualLeft = 0;
int manualRight = 0;

void updateSensorData() {
  for(int i = 0; i < NUM_SENSORS; i++) {
    sensorData[i] = digitalRead(SENSOR_PINS[i]);
  }
}

void initWiFi() {
  loadParameters();
  
  // Отключаем энергосбережение WiFi чтобы не отключался
  WiFi.setSleep(false);
  
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  IPAddress IP = WiFi.softAPIP();
  
  Serial.println("========================");
  Serial.print("WiFi AP: ");
  Serial.println(WIFI_SSID);
  Serial.print("Password: ");
  Serial.println(WIFI_PASSWORD);
  Serial.print("IP Address: ");
  Serial.println(IP);
  Serial.println("========================");
  
  // Главная страница
  server.on("/", HTTP_GET, []() {
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'>";
    html += "<title>PID Settings</title><style>";
    html += "body{font-family:Arial;margin:0;padding:20px;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh}";
    html += ".container{max-width:500px;margin:0 auto;background:white;padding:20px;border-radius:15px;box-shadow:0 10px 40px rgba(0,0,0,0.3)}";
    html += "h1{color:#333;text-align:center;margin:0 0 20px 0;font-size:24px}";
    html += ".controls{display:flex;gap:10px;margin:15px 0}";
    html += ".ctrl-btn{flex:1;padding:15px;border:none;border-radius:8px;font-size:16px;font-weight:bold;cursor:pointer}";
    html += ".start-btn{background:linear-gradient(135deg,#28a745 0%,#20c997 100%);color:white}";
    html += ".stop-btn{background:linear-gradient(135deg,#dc3545 0%,#fd7e14 100%);color:white}";
    html += ".status-display{padding:15px;margin:15px 0;background:#f0f0f0;border-radius:8px;text-align:center;font-weight:bold;font-size:18px}";
    html += ".running{background:#d4edda;color:#155724}";
    html += ".stopped{background:#f8d7da;color:#721c24}";
    html += ".sensors{display:flex;gap:8px;margin:15px 0;padding:15px;background:#f0f0f0;border-radius:8px}";
    html += ".sensor{flex:1;text-align:center;padding:10px;border-radius:5px;font-weight:bold;transition:all 0.3s}";
    html += ".sensor.off{background:#ddd;color:#666}";
    html += ".sensor.on{background:#28a745;color:white;transform:scale(1.1)}";
    html += ".param{margin:10px 0;padding:12px;background:#f9f9f9;border-radius:8px;border-left:4px solid #667eea}";
    html += "label{display:block;margin-bottom:5px;font-weight:bold;color:#555;font-size:13px}";
    html += ".value{margin-left:10px;color:#667eea;font-weight:bold;font-size:16px}";
    html += "input{width:100%;padding:10px;border:2px solid #ddd;border-radius:6px;box-sizing:border-box;font-size:15px}";
    html += "input:focus{outline:none;border-color:#667eea}";
    html += "button{width:100%;padding:12px;margin:8px 0;border:none;border-radius:8px;cursor:pointer;font-size:15px;font-weight:bold}";
    html += ".save{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:white}";
    html += ".reset{background:linear-gradient(135deg,#6c757d 0%,#495057 100%);color:white}";
    html += ".msg{padding:12px;margin:10px 0;border-radius:8px;text-align:center;display:none}";
    html += ".success{background:#d4edda;color:#155724;border:2px solid #c3e6cb}";
    html += "</style></head><body><div class='container'>";
    html += "<h1>⚙️ Robot Control</h1>";
    
    // Статус и кнопки управления
    html += "<div class='status-display " + String(isRunning ? "running" : "stopped") + "' id='status'>";
    html += isRunning ? "🟢 LINE FOLLOW" : "🔴 MANUAL MODE";
    html += "</div>";
    
    html += "<div class='controls'>";
    html += "<button class='ctrl-btn start-btn' onclick='startRobot()'>🤖 LINE FOLLOW</button>";
    html += "<button class='ctrl-btn stop-btn' onclick='stopRobot()'>🎮 MANUAL</button>";
    html += "</div>";
    
    // Ручное управление (показывается только когда isRunning = false)
    html += "<div id='manual' style='display:" + String(isRunning ? "none" : "block") + "'>";
    html += "<div style='margin:20px 0;padding:20px;background:#f0f0f0;border-radius:10px'>";
    html += "<h3 style='text-align:center;margin:0 0 15px 0'>🎮 Manual Control</h3>";
    html += "<div style='display:grid;grid-template-columns:1fr 1fr 1fr;gap:10px;max-width:300px;margin:0 auto'>";
    html += "<div></div>";
    html += "<button class='ctrl-btn' style='background:#007bff;color:white;padding:30px' ontouchstart='move(150,150)' ontouchend='move(0,0)' onmousedown='move(150,150)' onmouseup='move(0,0)'>⬆️</button>";
    html += "<div></div>";
    html += "<button class='ctrl-btn' style='background:#007bff;color:white;padding:30px' ontouchstart='move(-100,100)' ontouchend='move(0,0)' onmousedown='move(-100,100)' onmouseup='move(0,0)'>⬅️</button>";
    html += "<button class='ctrl-btn' style='background:#dc3545;color:white;padding:30px' onclick='move(0,0)'>⏹️</button>";
    html += "<button class='ctrl-btn' style='background:#007bff;color:white;padding:30px' ontouchstart='move(100,-100)' ontouchend='move(0,0)' onmousedown='move(100,-100)' onmouseup='move(0,0)'>➡️</button>";
    html += "<div></div>";
    html += "<button class='ctrl-btn' style='background:#007bff;color:white;padding:30px' ontouchstart='move(-150,-150)' ontouchend='move(0,0)' onmousedown='move(-150,-150)' onmouseup='move(0,0)'>⬇️</button>";
    html += "<div></div>";
    html += "</div></div></div>";
    
    // Датчики
    html += "<div class='sensors' id='sensors'>";
    for(int i = 0; i < 8; i++) {
      html += "<div class='sensor " + String(sensorData[i] ? "on" : "off") + "' id='s" + String(i) + "'>" + String(i) + "</div>";
    }
    html += "</div>";
    
    // Параметры
    html += "<div class='param'><label>KP: <span class='value'>" + String(KP, 1) + "</span></label>";
    html += "<input type='number' id='kp' step='1' value='" + String(KP, 1) + "'></div>";
    
    html += "<div class='param'><label>KI: <span class='value'>" + String(KI, 1) + "</span></label>";
    html += "<input type='number' id='ki' step='0.1' value='" + String(KI, 1) + "'></div>";
    
    html += "<div class='param'><label>KD: <span class='value'>" + String(KD, 1) + "</span></label>";
    html += "<input type='number' id='kd' step='1' value='" + String(KD, 1) + "'></div>";
    
    html += "<div class='param'><label>BASE: <span class='value'>" + String(BASE_SPEED) + "</span></label>";
    html += "<input type='number' id='base' min='0' max='255' value='" + String(BASE_SPEED) + "'></div>";
    
    html += "<div class='param'><label>MAX: <span class='value'>" + String(MAX_SPEED) + "</span></label>";
    html += "<input type='number' id='max' min='0' max='255' value='" + String(MAX_SPEED) + "'></div>";
    
    html += "<div id='msg' class='msg'></div>";
    html += "<button class='save' onclick='saveParams()'>💾 Save Settings</button>";
    html += "<button class='reset' onclick='resetParams()'>🔄 Reset to Default</button>";
    
    html += "<script>";
    // Обновление статуса и датчиков
    html += "setInterval(()=>{fetch('/sensors').then(r=>r.json()).then(d=>{";
    html += "for(let i=0;i<8;i++){const s=document.getElementById('s'+i);";
    html += "s.className=d.sensors[i]?'sensor on':'sensor off';}";
    html += "const st=document.getElementById('status');";
    html += "st.className='status-display '+(d.running?'running':'stopped');";
    html += "st.textContent=d.running?'🟢 LINE FOLLOW':'🔴 MANUAL MODE';";
    html += "document.getElementById('manual').style.display=d.running?'none':'block';";
    html += "})},200);";
    
    // Ручное управление
    html += "function move(l,r){fetch('/move?l='+l+'&r='+r);}";
    
    // Старт робота
    html += "function startRobot(){fetch('/start').then(r=>r.text()).then(d=>{";
    html += "showMsg(d);});}";
    
    // Стоп робота
    html += "function stopRobot(){fetch('/stop').then(r=>r.text()).then(d=>{";
    html += "showMsg(d);});}";
    
    // Сохранение параметров
    html += "function saveParams(){";
    html += "const data={kp:document.getElementById('kp').value,ki:document.getElementById('ki').value,";
    html += "kd:document.getElementById('kd').value,base:document.getElementById('base').value,";
    html += "max:document.getElementById('max').value};";
    html += "fetch('/save?'+new URLSearchParams(data)).then(r=>r.text()).then(d=>{";
    html += "showMsg(d);setTimeout(()=>location.reload(),1500);});}";
    
    // Сброс
    html += "function resetParams(){if(confirm('Reset?')){";
    html += "fetch('/reset').then(r=>r.text()).then(d=>{";
    html += "showMsg(d);setTimeout(()=>location.reload(),1500);});}}";
    
    // Показ сообщений
    html += "function showMsg(text){const m=document.getElementById('msg');";
    html += "m.className='msg success';m.textContent='✓ '+text;m.style.display='block';}";
    html += "</script></div></body></html>";
    
    server.send(200, "text/html", html);
  });
  
  // API для получения данных датчиков и статуса
  server.on("/sensors", HTTP_GET, []() {
    updateSensorData();
    String json = "{\"sensors\":[";
    for(int i = 0; i < 8; i++) {
      json += String(sensorData[i]);
      if(i < 7) json += ",";
    }
    json += "],\"running\":";
    json += isRunning ? "true" : "false";
    json += "}";
    server.send(200, "application/json", json);
  });
  
  // Старт робота
  server.on("/start", HTTP_GET, []() {
    isRunning = true;
    resetPID();
    Serial.println(">>> STARTED via WEB <<<");
    server.send(200, "text/plain", "Robot STARTED!");
  });
  
  // Стоп робота
  server.on("/stop", HTTP_GET, []() {
    isRunning = false;
    manualLeft = 0;
    manualRight = 0;
    extern void stopMotors();
    stopMotors();
    Serial.println(">>> STOPPED via WEB <<<");
    server.send(200, "text/plain", "Robot STOPPED!");
  });
  
  // Ручное управление
  server.on("/move", HTTP_GET, []() {
    if(!isRunning) {
      if(server.hasArg("l")) manualLeft = server.arg("l").toInt();
      if(server.hasArg("r")) manualRight = server.arg("r").toInt();
      
      extern void setMotorLeft(int);
      extern void setMotorRight(int);
      setMotorLeft(manualLeft);
      setMotorRight(manualRight);
      
      Serial.print("Manual: L=");
      Serial.print(manualLeft);
      Serial.print(" R=");
      Serial.println(manualRight);
    }
    server.send(200, "text/plain", "OK");
  });
  
  // Сохранение параметров
  server.on("/save", HTTP_GET, []() {
    if(server.hasArg("kp")) KP = server.arg("kp").toFloat();
    if(server.hasArg("ki")) KI = server.arg("ki").toFloat();
    if(server.hasArg("kd")) KD = server.arg("kd").toFloat();
    if(server.hasArg("base")) BASE_SPEED = server.arg("base").toInt();
    if(server.hasArg("max")) MAX_SPEED = server.arg("max").toInt();
    
    saveParameters();
    resetPID();
    
    server.send(200, "text/plain", "Settings saved!");
  });
  
  // Сброс
  server.on("/reset", HTTP_GET, []() {
    KP = 20.0;
    KI = 0.0;
    KD = 8.0;
    BASE_SPEED = 100;
    MAX_SPEED = 200;
    
    saveParameters();
    resetPID();
    
    server.send(200, "text/plain", "Reset to defaults!");
  });
  
  server.begin();
  Serial.println("Server started");
}

void handleWebServer() {
  server.handleClient();
}

void loadParameters() {
  prefs.begin("robot", false);
  
  KP = prefs.getFloat("kp", 20.0);
  KI = prefs.getFloat("ki", 0.0);
  KD = prefs.getFloat("kd", 8.0);
  BASE_SPEED = prefs.getInt("base", 100);
  MAX_SPEED = prefs.getInt("max", 200);
  
  Serial.println("Parameters loaded:");
  Serial.print("KP="); Serial.print(KP);
  Serial.print(" KI="); Serial.print(KI);
  Serial.print(" KD="); Serial.print(KD);
  Serial.print(" BASE="); Serial.print(BASE_SPEED);
  Serial.print(" MAX="); Serial.println(MAX_SPEED);
}

void saveParameters() {
  prefs.putFloat("kp", KP);
  prefs.putFloat("ki", KI);
  prefs.putFloat("kd", KD);
  prefs.putInt("base", BASE_SPEED);
  prefs.putInt("max", MAX_SPEED);
  
  Serial.println("Parameters saved to NVS!");
}