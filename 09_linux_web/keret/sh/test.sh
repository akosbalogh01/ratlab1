#!/bin/sh

cat << EOF
Content-Type: text/html

<html>
<head>
<title>cgi shell scripting example</title>
</head>
<body>
<h1>Status</h1>
EOF
echo Date: $(date) "<br />"
echo Uptime: $(uptime) "<br />"

echo BTN1 status:
cat /sys/bus/platform/devices/rpi2exp_btns/btn1
echo "<br />"
echo BTN2 status:
cat /sys/bus/platform/devices/rpi2exp_btns/btn2
echo "<br />"
echo BTN3 status:
cat /sys/bus/platform/devices/rpi2exp_btns/btn3
echo "<br />"
echo BTN4 status:
cat /sys/bus/platform/devices/rpi2exp_btns/btn4
echo "<br />"

echo Wiper status:
cat /sys/bus/i2c/devices/1-002e/wiper
echo "<br />"

echo ADC status:
cat /sys/bus/i2c/devices/1-004b/in
echo "<br />"

echo Temperature status:
cat /sys/bus/i2c/devices/1-0048/temp
echo C "<br />"

cat << EOF
</body>
</html>
EOF
