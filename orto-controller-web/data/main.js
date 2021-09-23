/*
This file is part of Orto idroponico Fablab Imperia

Orto idroponico Fablab Imperia is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Nome-Programma is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
*/

const temp_gauge = {x:300, y:300, r:190, width:60, angle_ref:Math.PI/6, background_color:'#ccc', min:0, max:35, fixed:1, um:'°C'};
const ph_gauge = {x:850, y:300, r:190, width:60, angle_ref:Math.PI/6, background_color:'#ccc', min:0, max:14, fixed:1, um:'pH'};
const cond_gauge = {x:1400, y:300, r:190, width:60, angle_ref:Math.PI/6, background_color:'#ccc', min:0, max:3000, fixed:0, um:'uS/cm'};


let canvas = document.getElementById("gauge_canvas");
let ctx = canvas.getContext('2d');

let gateway = `ws://${window.location.hostname}/ws`;
let websocket;

let connection_animation = true;
let animation_step = 0;


window.addEventListener('load', onLoad);

function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  try {
    websocket = new WebSocket(gateway);
  } catch (e) {
    console.log(e);
  }

  websocket.onopen    = onOpen;
  websocket.onclose   = onClose;
  websocket.onmessage = onMessage; // <-- add this line


}

function onOpen(event) {
  console.log('Connection opened');
  connection_animation = false;
}

function onClose(event) {
  console.log('Connection closed');
  setTimeout(initWebSocket, 2000);

  if (!connection_animation) {
    connection_animation = true;
    connection_animation_step();
  }
}

function onMessage(event) {
  document.getElementById('log').innerText = new Date().toLocaleString() + '  -  ' + event.data + '\n' + document.getElementById('log').innerText.slice(0, 200000);  // Keep only about 1000 lines
  let data = JSON.parse(event.data);
  //console.log(data);

  manual_control = data['manual_control'];
  water_pump_active = data['pump_on'];

  set_maual_control(toggle = false, send_command = false);
  set_pump(toggle = false, send_command = false);

	ctx.clearRect(0, 0, canvas.width, canvas.height);
  draw_gauge(ctx, temp_gauge, data['temp'], undefined);
  draw_gauge(ctx, ph_gauge, data['ph'], config_values.pH_threshold);
  draw_gauge(ctx, cond_gauge, data['cond'], config_values.cond_threshold);

  update_config_values(data);
}

function onLoad(event) {
  initWebSocket();
  connection_animation_step();
}








function connection_animation_step () {
  ctx.save();
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  ctx.translate(canvas.width / 2, canvas.height / 2);

  ctx.font = "Bold 95px Arial";
  ctx.textAlign="center";
  ctx.textBaseline = "middle";
  ctx.fillStyle = "#000";
  ctx.fillText(" ".repeat(Math.floor(animation_step/50) % 4) + "Connecting" + ".".repeat(Math.floor(animation_step/50) % 4), 0, -200);

  ctx.lineCap = "round";
  ctx.beginPath();
  ctx.lineWidth = 14;
  ctx.strokeStyle = 'gray';
  ctx.arc(0, 0, 60, -animation_step/30, -animation_step/30 + Math.PI * 3 / 2, true);
  ctx.stroke();
  ctx.beginPath();
  ctx.arc(0, 0, 80, animation_step/20, animation_step/20 + Math.PI * 3 / 2, true);

  ctx.stroke();

  ctx.restore();
  animation_step += 1;

  if (connection_animation) {
    requestAnimationFrame(connection_animation_step);
  }
}



function draw_gauge(ctx, gauge, value, target){


  /**************************** Gauge ***************************/
  // Background
  ctx.lineWidth = gauge.width;
  ctx.beginPath();
  ctx.arc(gauge.x, gauge.y, gauge.r, Math.PI - gauge.angle_ref, gauge.angle_ref);
  ctx.strokeStyle = gauge.background_color;
  ctx.stroke();

  // Background right extreme
  ctx.lineWidth = 1;
  ctx.beginPath()
  ctx.arc(gauge.x + gauge.r*Math.cos(gauge.angle_ref), gauge.y + gauge.r*Math.sin(gauge.angle_ref), gauge.width/2, gauge.angle_ref-0.05, Math.PI + gauge.angle_ref + 0.05);
  ctx.fillStyle = gauge.background_color;
  ctx.fill();

  // Value text
  ctx.font = "Bold 75px Arial";
  ctx.textAlign="center";
  ctx.textBaseline = "middle";
  ctx.fillStyle = "#000";
  ctx.fillText(value.toFixed(gauge.fixed), gauge.x, gauge.y);
  ctx.fillText(gauge.um, gauge.x, gauge.y + 100);


  // Gauge fill

  let fill_angle = (Math.PI + 2 * gauge.angle_ref) / (gauge.max - gauge.min) * (value - gauge.min);
  let rgb_val = interpolateColors(value, [{val:gauge.min, color:[0, 255, 0]}, {val:(gauge.max+gauge.min)/2, color:[255, 255, 0]}, {val:gauge.max, color:[255, 0, 0]}])
  let color = rgb(rgb_val = rgb_val);
//  console.log(color);

  if (value > gauge.max) {
    fill_angle = Math.PI + 2 * gauge.angle_ref;
    color = '#f00';
    /*
    // Background right extreme
    ctx.lineWidth = 1;
    ctx.beginPath()
    ctx.arc(gauge.x + gauge.r*Math.cos(gauge.angle_ref), gauge.y + gauge.r*Math.sin(gauge.angle_ref), gauge.width/2, gauge.angle_ref-0.05, Math.PI + gauge.angle_ref + 0.05);
    ctx.fillStyle = color;
    ctx.fill();
    */
  } else if (value < gauge.min) {
    fill_angle = 0;
    color = '#0f0';
  }

  ctx.strokeStyle = color;
  ctx.fillStyle = color;

  // Left extreme fill color
  ctx.beginPath()
  ctx.arc(gauge.x - gauge.r*Math.cos(gauge.angle_ref), gauge.y + gauge.r*Math.sin(gauge.angle_ref), gauge.width/2, -gauge.angle_ref, Math.PI - gauge.angle_ref);
  ctx.fill();

  // Fill proportional to value
  ctx.lineWidth = gauge.width;
  ctx.beginPath();
  ctx.arc(gauge.x, gauge.y, gauge.r, Math.PI - gauge.angle_ref, Math.PI - gauge.angle_ref + fill_angle);
  ctx.stroke();

  // Right extreme fill color
  ctx.beginPath()
  ctx.arc(gauge.x + gauge.r*Math.cos(Math.PI - gauge.angle_ref + fill_angle), gauge.y + gauge.r*Math.sin(Math.PI - gauge.angle_ref + fill_angle), gauge.width/2, 0, 2 * Math.PI);
  ctx.fill();


  // Draw target value
  if (target != undefined) {
    ctx.fillStyle = '#000';
    ctx.beginPath();
    let angle_target = Math.PI - gauge.angle_ref +  (Math.PI + 2 * gauge.angle_ref) / (gauge.max - gauge.min) * (target - gauge.min);
    ctx.arc(gauge.x + gauge.r*Math.cos(angle_target), gauge.y + gauge.r*Math.sin(angle_target), gauge.width/4, 0, 2 * Math.PI);
    ctx.fill();
  }
}



function interpolateColors(value, points) {
  let x_min, x_max, y_min, y_max;
  for (let point of points) {
//    console.log(point)
    if (value == point.val) {
      return point.color;
    }
    if (value < point.val && x_min != undefined) {
      x_max = point.val;
      y_max = point.color;
      break;
    }
    if (value > point.val) {
      x_min = point.val;
      y_min = point.color;
    }
  }
//  console.log(x_min, x_max, y_min, y_max)
  let result = [0, 0, 0];

  if (x_min != undefined && x_max != undefined) {
    for (let i = 0; i < 3; i++) {
      result[i] = Math.round((y_max[i] - y_min[i])/(x_max - x_min) * (value - x_min) + y_min[i]);
//      console.log(value, result[i])
    }
  }

  return result;
}


function rgb(rgb_val) {
  let r = rgb_val[0];
  let g = rgb_val[1];
  let b = rgb_val[2];
  return "#"+(r).toString(16).padStart(2,'0')+(g).toString(16).padStart(2,'0')+(b).toString(16).padStart(2,'0');
}
