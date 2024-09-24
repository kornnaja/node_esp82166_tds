#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Replace these with your network credentials
const char* ssid = "Sixzac007";
const char* password = "12345678";

// Create an instance of the server on port 80
ESP8266WebServer server(80);

const int TdsSensorPin = A0;
float tdsValue = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  server.on("/", handleRoot);
  server.on("/tds", handleTDS);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>TDS Sensor</title>"
                "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.min.js\"></script>"
                "<script src=\"https://cdn.jsdelivr.net/npm/chartjs-plugin-doughnutlabel@1.0.0\"></script>"
                "<style>body { font-family: Arial, sans-serif; text-align: center; } canvas { width: 100%; max-width: 600px; }</style>"
                "</head><body>"
                "<h1>TDS Sensor Data</h1>"
                "<canvas id=\"gauge\"></canvas>"
                "<h2 id=\"quality\">Loading...</h2>"
                "<script>"
                "var ctx = document.getElementById('gauge').getContext('2d');"
                "var gauge = new Chart(ctx, {"
                "  type: 'doughnut',"
                "  data: {"
                "    datasets: [{"
                "      data: [0, 1200],"
                "      backgroundColor: ['#3cba54', '#ff0000'],"
                "      label: 'TDS'"
                "    }],"
                "    labels: ['TDS', '']"
                "  },"
                "  options: {"
                "    responsive: true,"
                "    legend: {"
                "      display: false"
                "    },"
                "    plugins: {"
                "      doughnutlabel: {"
                "        labels: ["
                "          {"
                "            text: '0',"
                "            font: {"
                "              size: 20,"
                "              weight: 'bold'"
                "            },"
                "            color: '#000'"
                "          },"
                "          {"
                "            text: 'PPM'"
                "          }"
                "        ]"
                "      }"
                "    }"
                "  }"
                "});"
                "function fetchTDS() {"
                "  fetch('/tds').then(response => response.json()).then(data => {"
                "    gauge.data.datasets[0].data[0] = data.tds;"
                "    gauge.update();"
                "    gauge.options.plugins.doughnutlabel.labels[0].text = data.tds;"
                "    var qualityText = '';"
                "    if (data.tds < 50) qualityText = 'Very Good';"
                "    else if (data.tds <= 150) qualityText = 'Good';"
                "    else if (data.tds <= 250) qualityText = 'Fair';"
                "    else if (data.tds <= 300) qualityText = 'Poor';"
                "    else if (data.tds <= 500) qualityText = 'Very Poor';"
                "    else qualityText = 'Unacceptable';"
                "    document.getElementById('quality').innerText = qualityText;"
                "  }).catch(error => console.error('Error fetching TDS data:', error));"
                "}"
                "setInterval(fetchTDS, 2000);"
                "</script></body></html>";
  server.send(200, "text/html", html);
}

void handleTDS() {
  int tdsAnalogValue = analogRead(TdsSensorPin);
  tdsValue = calculateTDS(tdsAnalogValue);
  String tdsJson = "{\"tds\":" + String(tdsValue) + "}";
  server.send(200, "application/json", tdsJson);
}

float calculateTDS(int analogValue) {
  float voltage = analogValue * (3.3 / 1024.0); // Convert analog value to voltage
  float tds = (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage) * 0.5; // Example calibration
  return tds;
}
