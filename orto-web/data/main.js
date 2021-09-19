const temp_gauge = {x:200, y:300, r:160, width:40, angle_ref:Math.PI/6, background_color:'#ccc', min:0, max:40, fixed:1};
const ph_gauge = {x:700, y:300, r:160, width:40, angle_ref:Math.PI/6, background_color:'#ccc', min:0, max:14, fixed:1};
const cond_gauge = {x:1200, y:300, r:160, width:40, angle_ref:Math.PI/6, background_color:'#ccc', min:0, max:3000, fixed:0};

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);
function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen    = onOpen;
  websocket.onclose   = onClose;
  websocket.onmessage = onMessage; // <-- add this line
}
function onOpen(event) {
  console.log('Connection opened');
}
function onClose(event) {
  console.log('Connection closed');
  setTimeout(initWebSocket, 2000);
}
function onMessage(event) {
  document.getElementById('debug').innerText = event.data;
  let data = JSON.parse(event.data);
  console.log(data);

	let canvas = document.getElementById("canvas_id");
	let ctx = canvas.getContext('2d');
	ctx.clearRect(0, 0, canvas.width, canvas.height);
  draw_gauge(ctx, temp_gauge, data['temp'], ' C');
  draw_gauge(ctx, ph_gauge, data['ph'], ' pH');
  draw_gauge(ctx, cond_gauge, data['cond'], '');
}
function onLoad(event) {
  initWebSocket();
}
function toggle(){
  websocket.send('toggle');
}












function draw_gauge(ctx, gauge, value, um){


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
  ctx.fillText(value.toFixed(gauge.fixed) + um, gauge.x, gauge.y);


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
