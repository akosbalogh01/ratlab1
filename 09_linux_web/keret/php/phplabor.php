<html>
<title>HDU8 Ratlab</title>
<body>

<?php
	if(isset($_REQUEST["potmeter"])) {
		$value=$_REQUEST["potmeter"];
		
		if($value>=0 && $value<=127){
			$potmeter=fopen("/sys/bus/i2c/devices/1-002e/wiper", "w");
			fwrite($potmeter, $value);
		}
		else {
			echo "<p style=\"color: red\"> Please select a value between 0 and 127!</p>";
		}
	}
?>

<h1>Buttons</h1>
<?php
	for ($i=1; $i<=4; $i++) {
		$btn=fopen("/sys/bus/platform/devices/rpi2exp_btns/btn".$i, "r");
		echo "<p>Button".$i" status: ".fgets($btn)."</p>";
	}
?>

<h1>ADC</h1>
<?php
	$wiper=fopen("/sys/bus/i2c/devices/1-002e/wiper", "r");
	$adc=fopen("/sys/bus/i2c/devices/1-004b/in", "r");
	
	echo "<p>Wiper: ".fgets($wiper).", Voltage: ".fgets($adc)."mV. </p>";
?>

<h1>Temperature</h1>
<?php
	$temp=fopen("/sys/bus/i2c/devices/1-0048/temp", "r");
	
	echo "<p>".gets($temp)."C</p>";
?>

<h1>LEDs</h1>
<form method="post" action=""/>
	<?php
		for ($i=1; $i<=4; $i++) {
			echo "<label for=led".$i.">Led ".$i."</label> <input type=checkbox name=led".$i." value=led".$i;
			if (isset($_REQUEST["led".$i])) echo " checked /></br>";
			else echo " /></br>";
		}
	?>
	<input type=submit value="Set LEDs"/>
</form>

<h1>Potmeter</h1>
<form method="post" action=""/>
	<label for=potmeter>Set value for potmeter:</label><input type=number name=potmeter <?php if(isset($_REQUEST["potmeter"])) echo "value=".$_REQUEST["potmeter"];?> /></br>
	<input type=submit value="Set Value"/>
</form>

<h1>LCD screen</h1>
<form method="post" action=""/>
	<label for=lcd1>First row</label><input type=text name=lcd0 maxlength=16 value="<?php echo (isset($_REQUEST["lcd0"])) ? $_REQUEST["lcd0"] : ""; ?>" /></br>
	<label for=lcd2>Second row</label><input type=text name=lcd1 maxlength=16 value="<?php echo (isset($_REQUEST["lcd1"])) ? $_REQUEST["lcd1"] : ""; ?>" /></br>
	<input type=submit value="Write on LCD" />
</form>

<?php
	for ($i=1; $i<=4; $i++) {
		$led=fopen("/sys/bus/platform/devices/rpi2exp_leds/led".$i, "w");
		fwrite($led, (isset($_REQUEST["led".$i])) ? "1" : "0");
	}
	for ($i=0; $i<=1; $i++) {
		$lcd=fopen("/sys/bus/spi/devices/spi0.0/line".$i, "w");
		if($_REQUEST["lcd".$i]=="") fwrite($lcd, "\0");
		else fwrite($lcd, $_REQUEST["lcd".$i]);
	}
?>

</body>
</html>