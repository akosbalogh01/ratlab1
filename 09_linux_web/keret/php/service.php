<?php
date_default_timezone_set("UTC"); 
require_once "lib/nusoap.php";
  
$server = new soap_server();

// Initialize WSDL support
$server->configureWSDL('laborservice', 'urn:laborservice');

// Register the method to expose
$server->register('Hello',                // method name
    array('name' => 'xsd:string'),        // input parameters
    array('return' => 'xsd:string'),      // output parameters
    'urn:laborservice',                   // namespace
    'urn:laborservice#hello',             // soapaction
    'rpc',                                // style
    'encoded',                            // use
    'Says hello to the caller'            // documentation
);

// TODO: regisztrálja az alábbi függvényeket: (void = üres)
// xsd:unsignedByte getBtn(xsd:unsignedByte ix);
$server->register('getBtn',                // method name
    array('ix' => 'xsd:unsignedByte'),     // input parameters
    array('return' => 'xsd:unsignedByte'), // output parameters
    'urn:laborservice',                    // namespace
    'urn:laborservice#getBtn',             // soapaction
    'rpc',                                 // style
    'encoded',                             // use
    'Get button value'                     // documentation
);

// xsd:unsignedByte getPot(void);
$server->register('getPot',                // method name
    array(),                               // input parameters
    array('return' => 'xsd:unsignedByte'), // output parameters
    'urn:laborservice',                    // namespace
    'urn:laborservice#getPot',             // soapaction
    'rpc',                                 // style
    'encoded',                             // use
    'Get potmeter value'                   // documentation
);

// xsd:unsignedShort getAdc(void);
$server->register('getAdc',                // method name
    array(),                               // input parameters
    array('return' => 'xsd:unsignedShort'),// output parameters
    'urn:laborservice',                    // namespace
    'urn:laborservice#getAdc',             // soapaction
    'rpc',                                 // style
    'encoded',                             // use
    'Get ADC value'                        // documentation
);

// xsd:string getTemp(void);
$server->register('getTemp',               // method name
    array(),                               // input parameters
    array('return' => 'xsd:string'),       // output parameters
    'urn:laborservice',                    // namespace
    'urn:laborservice#getTemp',            // soapaction
    'rpc',                                 // style
    'encoded',                             // use
    'Get temperature value'                // documentation
);

// void setLed(xsd:unsignedByte ix, xsd:boolean value);
$server->register('setLed',                // method name
    array('ix' => 'xsd:unsignedByte'),     // input parameters
    array('value' => 'xsd:boolean'),       // input parameters
    array(),                               // output parameters
    'urn:laborservice',                    // namespace
    'urn:laborservice#setLed',             // soapaction
    'rpc',                                 // style
    'encoded',                             // use
    'Set LED value'                        // documentation
);

// void setPot(xsd:unsignedByte value);
$server->register('setPot',                // method name
    array('value' => 'xsd:unsignedByte'),  // input parameters
    array(),                               // output parameters
    'urn:laborservice',                    // namespace
    'urn:laborservice#setPot',             // soapaction
    'rpc',                                 // style
    'encoded',                             // use
    'Set potmeter value'                   // documentation
);

// void setLcd(xsd:unsignedByte ix, xsd:string value);
$server->register('setLcd',                // method name
    array('ix' => 'xsd:unsignedByte'),     // input parameters
    array('value' => 'xsd:string'),        // input parameters
    array('return' => ''),                 // output parameters
    'urn:laborservice',                    // namespace
    'urn:laborservice#setLcd',             // soapaction
    'rpc',                                 // style
    'encoded',                             // use
    'Set LCD value'                        // documentation
);

// A nevter (namespace) legyen: urn:laborservice
// Soapaction: nevter#metodusAzonosito pl. 'urn:laborservice#getBtn'
// Style: rpc
// Use: encoded
// Documentation: a szolgaltatas leirasa

function Hello($name) {
        return 'Hello, ' . $name;
}

// TODO: implementalja a metodusokat (ld. status.php)
function readDataFromFile($filename) {
    //TODO: get handle (fopen)
    $handle=fopen($filename, "r");
    //TODO: get contents (fgets)
    $content=fgets($handle);
    //TODO: close handle (fclose)
    fclose($filename);
    //TODO: return contents
    return $content;
}

function writeDataToFile($filename,$data) {
    // HACK: LCD sortörlés (üres adatra nem hívódik meg az írás)
    if(strlen($data) == 0) $data = " ";
    //TODO: get handle (fopen)
    $handle=fopen($filename, "w");
    //TODO: write contents (fwrite)
    fwrite($handle, $data);
    //TODO: close handle (fclose)
    fclose($handle);
}

function getBtn($ix) {
  // TODO: attribútum lekérdezés és visszaadás
    return readDataFromFile("/sys/platform/devices/rpi2exp_btns/btn".$ix);
}

function getPot() {
  // TODO: attribútum lekérdezés és visszaadás
    return readDataFromFile("/sys/bus/i2c/devices/1-002e/wiper");
}

function getAdc() {
  // TODO: attribútum lekérdezés és visszaadás
    return readDataFromFile("/sys/bus/i2c/devices/1-004b/in");
}

function getTemp() {
  // TODO: attribútum lekérdezés és visszaadás
    return readDataFromFile("/sys/bus/i2c/devices/1-0048/temp")."C";
}

function setLed($ix, $value) {
  // TODO: attribútum beállítás, érték 0/1
    writeDataToFile("/sys/platform/devices/rpi2exp_leds/led".$ix, $value);
}

function setPot($value) {
  // TODO: attribútum beállítás
    writeDataToFile("/sys/bus/i2c/devices/1-002e/wiper", $value);
}

function setLcd($ix, $value) {
  // TODO: attribútum beállítás
    writeDataToFile("/sys/bus/spi/devices/spi0.0/line".$ix, $value);
}


$server->service(file_get_contents("php://input"));

?>
