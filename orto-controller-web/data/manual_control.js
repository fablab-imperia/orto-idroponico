let manual_control = false;
let manual_interval = 0;
let manual_type = '';
let water_pump_active = false;
const PING_DELAY = 100;

function set_maual_control(toggle = true, send_command = true) {
  if (toggle) {
    manual_control = !manual_control;
    if (manual_control) {
      water_pump_active = false;
    }
  }
  if (send_command) {
    if (manual_control) {
      websocket.send("M");    // Manual control
    } else {
      websocket.send("U");    // aUtomatic control
    }
  }

  if (manual_control) {
    document.getElementById('manual_automatic').innerText = 'Automatico';
    document.getElementById('manual_control_div').style.maxHeight = '100%';
    document.getElementById('manual_control_div').style.opacity = '1';
    document.getElementById('water_pump_action_text').innerText = 'Attiva';
  } else {
    document.getElementById('manual_automatic').innerText = 'Manuale';
    document.getElementById('manual_control_div').style.maxHeight = '0';
    document.getElementById('manual_control_div').style.opacity = '0';
  }
}

function set_pump(toggle = true, send_command = true) {
  if (toggle) {
    water_pump_active = !water_pump_active;
  }

  if (send_command) {
    if (water_pump_active) {
      websocket.send('P');
    } else {
      websocket.send('p');
    }
  }

  if (water_pump_active) {
    document.getElementById('water_pump_action_text').innerText = 'Disattiva';
  } else {
    document.getElementById('water_pump_action_text').innerText = 'Attiva';
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
