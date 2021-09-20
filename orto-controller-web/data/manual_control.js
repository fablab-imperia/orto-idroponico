let manual_control = false;
let manual_interval = 0;
let manual_type = '';
let water_pump_active = false;
const PING_DELAY = 100;

function toggle_maual_control() {
  manual_control = !manual_control;
  if (manual_control) {
    websocket.send("T;");
  } else {
    websocket.send("F;");
  }

  if (manual_control) {
    document.getElementById('manual_automatic').innerText = 'Automatico';
    document.getElementById('manual_control_div').style.maxHeight = '100%';
    document.getElementById('manual_control_div').style.opacity = '1';
    document.getElementById('water_pump_action_text').innerText = 'Attiva';
    water_pump_active = false;
  } else {
    document.getElementById('manual_automatic').innerText = 'Manuale';
    document.getElementById('manual_control_div').style.maxHeight = '0';
    document.getElementById('manual_control_div').style.opacity = '0';
  }
}

function toggle_pump() {
  water_pump_active = !water_pump_active;

  if (water_pump_active) {
    document.getElementById('water_pump_action_text').innerText = 'Disattiva';
    websocket.send('P');
  } else {
    document.getElementById('water_pump_action_text').innerText = 'Attiva';
    websocket.send('p');
  }
}

function clicked(type) {
  manual_type = type;
  websocket.send(manual_type.toUpperCase());
  if (type != 'p') {
    manual_interval = setInterval(() => {
      websocket.send(manual_type.toUpperCase());
    }, PING_DELAY);
  }
}

function released(type) {
  clearInterval(manual_interval);
  websocket.send(manual_type);
}
