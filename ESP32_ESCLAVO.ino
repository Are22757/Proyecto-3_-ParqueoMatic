#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

// Configuración de WiFi
const char* ssid = "POCO X4 Pro 5G";
const char* password = "12345678";

WebServer server(80);

// Configuración de NeoPixels
#define PIN_NEOPIXEL 5
#define NUMPIXELS 8
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Dirección I2C del esclavo
#define I2C_ADDRESS 0x04

// Estado de los estacionamientos (4 sensores)
bool parkingStatus[4] = {LOW, LOW, LOW, LOW};  // Estado de cada estacionamiento

// Configuración inicial
void setup() {
  Serial.begin(115200);
  Serial.println("Conectando a WiFi...");

  // Inicializar NeoPixels
  pixels.begin();
  pixels.show(); // Inicializa todos los píxeles a 'off'

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

 // Inicializar I2C
  Wire.begin(I2C_ADDRESS);
  Wire.onReceive(receiveEvent); // Configurar el evento de recepción



  // Rutas del servidor
  server.on("/", handle_OnConnect);
  server.on("/getStatus", handle_GetStatus);  // Ruta para obtener el estado de los estacionamientos
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

// Loop principal
void loop() {
  server.handleClient(); // Manejar las peticiones del servidor
}

// Evento de recepción I2C
void receiveEvent(int howMany) {
  while (Wire.available()) {
    char command = Wire.read(); // Leer el número de espacios disponibles
    switch (command) {
            case 'S': // Sensor 1 activo
                parkingStatus[0] = true;
                Serial.println("Sensor 1 activo");
                break;
            case 'T': // Sensor 1 inactivo
                parkingStatus[0] = false;
                Serial.println("Sensor 1 inactivo");
                break;
            case 'U': // Sensor 2 activo
                parkingStatus[1] = true;
                Serial.println("Sensor 2 activo");
                break;
            case 'V': // Sensor 2 inactivo
                parkingStatus[1] = false;
                Serial.println("Sensor 2 inactivo");
                break;
            case 'W': // Sensor 3 activo
                parkingStatus[2] = true;
                Serial.println("Sensor 3 activo");
                break;
            case 'X': // Sensor 3 inactivo
                parkingStatus[2] = false;
                Serial.println("Sensor 3 inactivo");
                break;
            case 'Y': // Sensor 4 activo
                parkingStatus[3] = true;
                Serial.println("Sensor 4 activo");
                break;
            case 'Z': // Sensor 4 inactivo
                parkingStatus[3] = false;
                Serial.println("Sensor 4 inactivo");
                break;
        }
    }

    // Actualizar los NeoPixels basados en el estado de los estacionamientos
    updateNeoPixels();
}
// Actualiza los NeoPixels según el estado del parqueo
void updateNeoPixels() {
  for (int i = 0; i < 4; i++) {
    if (parkingStatus[i]) {
      // Si el parqueo está ocupado, enciende el NeoPixel en rojo
      pixels.setPixelColor((3 - i) * 2, pixels.Color(255, 0, 0)); // NeoPixel 1 rojo
      pixels.setPixelColor((3 - i) * 2 + 1, pixels.Color(0, 0, 0)); // NeoPixel 2 apagado
    } else {
      // Si el parqueo está disponible, enciende el NeoPixel en verde
      pixels.setPixelColor((3 - i) * 2, pixels.Color(0, 255, 0)); // NeoPixel 1 verde
      pixels.setPixelColor((3 - i) * 2 + 1, pixels.Color(0, 0, 0)); // NeoPixel 2 apagado
    }
  }
  pixels.show(); // Actualiza los píxeles
}

// Manejador de la conexión principal
void handle_OnConnect() {
  server.send(200, "text/html", SendHTML());
}

// Función para enviar el estado de los botones en JSON
void handle_GetStatus() {
  DynamicJsonDocument json(200);
  
  for (int i = 0; i < 4; i++) {
    json["parking" + String(i + 1)] = parkingStatus[i];
  }
  
  String response;
  serializeJson(json, response);
  server.send(200, "application/json", response);
}

String SendHTML() {
  String ptr = "<!DOCTYPE html>\n";
  ptr += "<html lang=\"es\">\n";
  ptr += "<head>\n";
  ptr += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "    <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.5.0/css/bootstrap.min.css\"\n";
  ptr += "          integrity=\"sha384-9aIt2nRpC12Uk9gS9baDl411NQApFmC26EwAOH8WgZl5MYYxFfc+NcPb1dKGj7Sk\" crossorigin=\"anonymous\">\n";
  ptr += "    <link href=\"https://fonts.googleapis.com/css2?family=Oswald:wght@400&display=swap\" rel=\"stylesheet\">\n"; // Importar la fuente Oswald
  ptr += "    <title>ParqueoMatic</title>\n";
  ptr += "    <style>\n";
  ptr += "        html, body {\n";
  ptr += "            font-family: Serif;\n"; // Cambiar a fuente Serif
  ptr += "            display: inline-block;\n";
  ptr += "            margin: 0;\n";
  ptr += "            text-align: center;\n";
  ptr += "            background-color: black;\n"; // Fondo negro para toda la página
  ptr += "            color: white;\n"; // Texto blanco
  ptr += "            height: 100vh;\n"; // Altura completa de la ventana
  ptr += "        }\n";
  ptr += "        h1 {\n";
  ptr += "            color: white;\n";
  ptr += "            font-size: 4em;\n"; // Tamaño más grande del título
  ptr += "            font-family: 'Oswald', sans-serif;\n"; // Cambiar a fuente Oswald
  ptr += "            margin: 50px auto 10px;\n";
  ptr += "        }\n";
  ptr += "        .slogan {\n";
  ptr += "            font-size: 1.5em;\n"; // Tamaño del eslogan
  ptr += "            font-style: italic;\n"; // Cursiva
  ptr += "            margin-bottom: 50px;\n"; // Margen inferior para separar del contenido
  ptr += "        }\n";
  ptr += "        .estacionamientos-container {\n";
  ptr += "            display: flex;\n";
  ptr += "            justify-content: center;\n";
  ptr += "            margin-top: 100px;\n"; // Aumentar el margen superior para bajar los elementos
  ptr += "        }\n";
  ptr += "        .estacionamiento-group {\n";
  ptr += "            margin: 0 30px;\n"; // Espacio horizontal entre cada grupo
  ptr += "            text-align: center;\n"; // Centrar el texto dentro del grupo
  ptr += "        }\n";
  ptr += "        .estacionamiento {\n";
  ptr += "            width: 2cm;\n"; // Ancho del cuadro
  ptr += "            height: 3cm;\n"; // Altura del cuadro
  ptr += "            display: flex;\n";
  ptr += "            justify-content: center;\n";
  ptr += "            align-items: center;\n"; // Centrar el carro dentro del cuadro
  ptr += "            border: 2px solid #333;\n";
  ptr += "            background-color: #d3d3d3;\n"; // Fondo gris para los cuadros
  ptr += "            font-size: 3em;\n"; // Tamaño del emoji del carro
  ptr += "            margin: 20px 0 0;\n"; // Margen superior para bajar los cuadros
  ptr += "            position: relative;\n"; // Para posicionar el símbolo de disponible/no disponible
  ptr += "            border-radius: 15px;\n"; // Bordes redondeados
  ptr += "        }\n";
  ptr += "        .disponible, .no-disponible {\n";
  ptr += "            position: absolute;\n";
  ptr += "            top: -2.5cm;\n"; // Ajusta la posición vertical para que esté más arriba
  ptr += "            left: 5px;\n"; // Ajusta la posición horizontal
  ptr += "            font-size: 1em;\n"; // Tamaño del símbolo reducido
  ptr += "        }\n";
  ptr += "        .disponible {\n";
  ptr += "            color: green;\n"; // Color del símbolo de disponible
  ptr += "        }\n";
  ptr += "        .no-disponible {\n";
  ptr += "            color: red;\n"; // Color del símbolo de no disponible
  ptr += "        }\n";
  ptr += "        .estado {\n";
  ptr += "            width: 100px;\n"; // Ancho del cuadro de estado
  ptr += "            height: 30px;\n"; // Altura del cuadro de estado
  ptr += "            border-radius: 5px;\n"; // Esquinas redondeadas
  ptr += "            display: flex;\n";
  ptr += "            justify-content: center;\n";
  ptr += "            align-items: center;\n"; // Centrar el texto dentro del cuadro
  ptr += "            margin-top: 5px;\n"; // Espacio de 5px arriba del cuadro de estado
  ptr += "            font-size: 1em;\n"; // Tamaño del texto
  ptr += "            color: white;\n"; // Texto blanco
  ptr += "            position: relative;\n"; // Para ajustar la posición
  ptr += "            left: -10px;\n"; // Mover a la izquierda
  ptr += "            font-family: 'Arial Narrow', Arial, sans-serif;\n"; // Cambiar a fuente Arial Narrow
  ptr += "        }\n";
  ptr += "        .disponible-estado {\n";
  ptr += "            background-color: green;\n"; // Fondo verde para disponible
  ptr += "        }\n";
  ptr += "        .ocupado-estado {\n";
  ptr += "            background-color: red;\n"; // Fondo rojo para ocupado
  ptr += "        }\n";
  ptr += "    </style>\n"; 
  ptr += "    <script>\n";
  ptr += "        function updateStatus() {\n";
  ptr += "            fetch('/getStatus')\n";
  ptr += "                .then(response => response.json())\n";
  ptr += "                .then(data => {\n";
  ptr += "                    for (let i = 0; i < 4; i++) {\n";
  ptr += "                        const estacionamiento = document.querySelectorAll('.estacionamiento')[i];\n";
  ptr += "                        const estado = document.querySelectorAll('.estado')[i];\n";
  ptr += "                        if (data['parking' + (i + 1)]) {\n";
  ptr += "                            estacionamiento.innerHTML = '<span class=\"no-disponible\">&#9940;</span>';\n"; // Símbolo de no disponible
  ptr += "                            if (i == 0) {\n";
  ptr += "                                estacionamiento.innerHTML += '<div>&#128665;</div>'; // Carro azul\n"; // Carro azul para D23\n";
  ptr += "                            } else if (i == 1) {\n";
  ptr += "                                estacionamiento.innerHTML += '<div>&#128666;</div>'; // Carro naranja\n"; // Carro naranja para D13\n";
  ptr += "                            } else if (i == 2) {\n";
  ptr += "                                estacionamiento.innerHTML += '<div>&#128663;</div>'; // Carro rojo\n"; // Carro rojo para D12\n";
  ptr += "                            } else {\n";
  ptr += "                                estacionamiento.innerHTML += '<div>&#128659;</div>'; // Carro negro\n"; // Carro negro para D14\n";
  ptr += "                            }\n";
  ptr += "                            estado.className = 'estado ocupado-estado';\n";
  ptr += "                            estado.innerHTML = 'Ocupado';\n";
  ptr += "                        } else {\n";
  ptr += "                            estacionamiento.innerHTML = '<span class=\"disponible\">&#9989;</span><div></div>';\n"; // Cuadro vacío
  ptr += "                            estado.className = 'estado disponible-estado';\n";
  ptr += "                            estado.innerHTML = 'Disponible';\n";
  ptr += "                        }\n";
  ptr += "                    }\n";
  ptr += "                });\n";
  ptr += "        }\n";
  ptr += "        setInterval(updateStatus, 500); // Actualiza cada medio segundo\n"; // Intervalo de actualización
  ptr += "    </script>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "    <h1>ParqueoMatic</h1>\n";
  ptr += "    <div class=\"slogan\">Seguridad y comodidad, nuestro compromiso contigo</div>\n";
  ptr += "    <div class=\"estacionamientos-container\">\n";

  // Generar dinámicamente el estado de los estacionamientos
  for (int i = 0; i < 4; i++) {
    ptr += "        <div class=\"estacionamiento-group\">\n";
    ptr += "            <div class=\"estacionamiento\">\n";
    if (parkingStatus[i]) {
      ptr += "                <span class=\"no-disponible\">&#9940;</span>\n"; // Símbolo de no disponible
      if (i == 0) {
        ptr += "                <div>&#128665;</div> <!-- Carro azul -->\n"; // Carro azul para D23
      } else if (i == 1) {
        ptr += "                <div>&#128666;</div> <!-- Carro naranja -->\n"; // Carro naranja para D13
      } else if (i == 2) {
        ptr += "                <div>&#128663;</div> <!-- Carro rojo -->\n"; // Carro rojo para D12
      } else {
        ptr += "                <div>&#128659;</div> <!-- Carro negro -->\n"; // Carro negro para D14
      }
    } else {
      ptr += "                <span class=\"disponible\">&#9989;</span>\n"; // Símbolo de disponible
      ptr += "                <div></div> <!-- Cuadro vacío -->\n"; // Cuadro vacío
    }
    ptr += "            </div>\n";
    if (parkingStatus[i]) {
      ptr += "            <div class=\"estado ocupado-estado\">Ocupado ";
      ptr += "            </div>\n"; // Cuadro rojo
    } else {
      ptr += "            <div class=\"estado disponible-estado\">Disponible</div>\n"; // Cuadro verde
    }
    ptr += "        </div>\n";
  }

  ptr += "    </div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";

  return ptr;
}

// Manejador de rutas no encontradas
void handle_NotFound() {
  server.send(404, "text/plain", "Página no encontrada");
}
