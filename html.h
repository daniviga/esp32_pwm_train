/* -*- coding: utf-8 -*-
* vim: tabstop=2 shiftwidth=2 softtabstop=2
*
* ESP32_PWM_TRAIN - A basic PWM controller for DC trains
* Copyright (C) 2022 Daniele Viganò <daniele@vigano.me>
*
* ESP32_PWM_TRAIN is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ESP32_PWM_TRAIN is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
* Inspired by https://randomnerdtutorials.com/esp32-web-server-slider-pwm/
*/

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Train Control Web Server</title>
  <style>
    html {font-family: Sans; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 2.0rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #4000ff;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background:#01070a; cursor: pointer;}
    .slider::-moz-range-thumb {width:35px;height:35px;background:#01070a;cursor:pointer;} 
    #dirSlider {width: 100px;}
    #emergency {width: 150px; height: 35px; background-color: #f00; font-weight: bold;}
  </style>
</head>
<body>
  <h2>Train Control Web Server</h2>
  <p><small>Frequency: %FREQUENCY% Hz</small></p>
  <p><span id="textslider_value">%PWMVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM()" id="dirSlider" min="-1" max="1" value="%DIRVALUE%" step="1" class="slider">
  <input type="range" onchange="updateSliderPWM()" id="pwmSlider" min="0" max="255" value="%PWMVALUE%" step="1" class="slider">
  <input type="button" onclick="emergency()" id="emergency" value="Emergency stop"></p>
<script>
function updateSliderPWM() {
  var dir_value = document.getElementById("dirSlider").value;
  var pwm_value = document.getElementById("pwmSlider").value;
  document.getElementById("textslider_value").innerHTML = pwm_value;
  console.log("Direction: "+dir_value+" Speed: "+pwm_value);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/command?pwm="+pwm_value+"&direction="+dir_value, true);
  xhr.send();
}
function emergency() {
  console.log("Emergency stop!");
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/command?pwm=0&direction=0", true);
  xhr.send();
  document.getElementById("dirSlider").value = 0;
  document.getElementById("pwmSlider").value = 0;
  document.getElementById("textslider_value").innerHTML = 0;
}
</script>
</body>
</html>
)rawliteral";
