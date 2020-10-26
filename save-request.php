<? php

/*
  HUE CAT 2020
  www.huecat.es
  Conectar ESP32/ESP8266 con una Base de Datos MySQL
*/

$srvName = "localhost"; //Nombre del servidor

// NOMBRE DE LA BASE DE DATOS
$dbName = "mqtt_Arduin0";

// USUARIO DE LA BASE DE DATOS
$usrName = "huecat_placaESP8266";

// CONTRASEÑA DE LA BASE DE DATOS
$password = "huecat2020"; //ñ

//NOMBRE DE LA API QUE IDENTIFICARÁ AL DISPOSITIVO
$apiKey = "INGRESA_TU_API";
$idSensor = "1";
$nameSensor = "PIR";
$loc = "Madrid";


//////////////////// MODO PRUEBA: CAMBIAR GET POR POST ANTES DE USAR ARDUINO !! !!! !!!! !!! !! !

if ($_SERVER["REQUEST_METHOD"] == "GET") { //Si ha "entrado" algo

  $apiKeyRcv = test_input($_GET["api_key"]);
  if ($apiKeyRcv == $apiKey) { //Si la API está bien

    $id_sensor = test_input($_GET["id_sensor"]);
    $lugar = test_input($_GET["loc"]);

    // Crear cadena con parámetros de conexión
    $conexion = new mysqli($srvName, $usrName, $password, $dbName);

    // Check connection
    if ($conexion->connect_error) {
      die("Connection failed: " . $conexion->connect_error);
    }

    $sql = "INSERT INTO lectura_sensor (id_sensor, nombre_sensor,    lugar)
           VALUES ( '". $idSensor . "','". $nameSensor . "','". $loc . "')";

    if ($conexion->query($sql) == = TRUE) {
      echo "New record created successfully";
    }
    else {
      echo "Error: " . $sql . "<br>" . $conexion->error;
    }

    $conexion->close();
  }
  else {
    echo "API incorrecta";
  }

} //Se ejecutó el php pero sin recibir datos
else {
  echo "No se han recibido datos";
}


function test_input($dato) {
  echo "-> ".$dato."<br>";
  $dato = trim($dato);
  echo "-> ".$dato."<br>";
  $dato = stripslashes($dato);
  echo "-> ".$dato."<br>";
  $dato = htmlspecialchars($dato);
  echo "-> ".$dato."<br>";

  return $dato;
}

