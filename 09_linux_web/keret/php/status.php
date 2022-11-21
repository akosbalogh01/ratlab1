<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Cache-Control" content="no-cache">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Lang" content="en">
<meta name="description" content="">
<meta name="keywords" content="">
<title>RPi2EXP interface</title>
<link rel="stylesheet" type="text/css" href="my.css">
</head>
<body>
<?php

// TODO: implementálja a metódusokat

function readDataFromFile($filename) {
    //TODO: get handle (fopen)
    //TODO: get contents (fgets)
    //TODO: close handle (fclose)
    //TODO: return contents
} 
 
function writeDataToFile($filename,$data) {
    // HACK: LCD sortörlés (üres adatra nem hívódik meg az írás)
    if(strlen($data) == 0) $data = " ";
    //TODO: get handle (fopen)
    //TODO: write contents (fwrite)
    //TODO: close handle (fclose)
} 
        
function getBtn($ix) {
  // TODO: attribútum lekérdezés és visszaadás
}

function getPot() {
  // TODO: attribútum lekérdezés és visszaadás
}

function getAdc() {
  // TODO: attribútum lekérdezés és visszaadás
}

function getTemp() {
  // TODO: attribútum lekérdezés és visszaadás
}

function setLed($ix, $value) {
  // TODO: attribútum beállítás, érték 0/1
}

function setPot($value) {
  // TODO: attribútum beállítás
}

function setLcd($ix, $value) {
  // TODO: attribútum beállítás 
}

// TODO: beérkező adatok alapján a LED-ek állítása

// TODO: a potméter értékének validálása és beállítása

// TODO: az LCD sorainak beálltása

?>  
<h2>Current button values are: <?php echo getBtn(1);  ?> <?php echo getBtn(2);  ?> <?php echo getBtn(3);  ?> <?php echo getBtn(4);  ?></h2>
<?php 
// TODO: értékek kiírása 
?>
<br/>
<form action="">
  <h3>Set LED value to:</h3>  
<?php 
// TODO: utolsó LED beállítás megjelenítése és az új beállítás fogadása
?>
  <h3>Set Pot:</h3>
<?php 
// TODO: hibás értékre figyelmeztető szöveg
// TODO: utolsó potméter beállítás megjelenítése és az új beállítás fogadása
?>
  <h3>Set LCD:</h3>
<?php 
// TODO: utolsó LCD beállítás megjelenítése és az új beállítás fogadása
?>
  <input type="submit" value="Set"> 
</form>

</body>
</html>
