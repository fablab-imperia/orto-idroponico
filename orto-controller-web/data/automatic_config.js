let editing = false;

function editing_config(element) {
  editing = true;
  document.getElementById('save_config').disabled = false;
  element.style.backgroundColor = '#4CAF50';
}

function save_automatic_config () {
  let options = ['pH_threshold', 'cond_threshold', 'peristaltic_time', 'control_cycle_time', 'water_pump_cycle_time', 'water_pump_active_time'];
  let i = 0;
  for (option of options) {
    let value = Number(document.getElementById(option).value);
    let um_coefficient = Number(document.getElementById(option+'_um').options[document.getElementById(option+'_um').selectedIndex].value);
    websocket.send(i + (value*um_coefficient).toString() + ';');
    i++;
  }
}

function update_config_values(data) {
  let config_values = {
    pH_threshold : data['ph_th'],
    cond_threshold : data['cond_th'],
    peristaltic_time : data['t_peristaltic'],
    control_cycle_time : data['t_reads'],
    water_pump_cycle_time : data['t_pump_cycle'],
    water_pump_active_time : data['t_pump_active']
  }

  let parameters = ['pH_threshold', 'cond_threshold', 'peristaltic_time', 'control_cycle_time', 'water_pump_cycle_time', 'water_pump_active_time'];

  // Set values and unit of measure
  let parameters_changed = false;
  for (parameter of parameters) {
    let current_val = config_values[parameter];
    let current_um = document.getElementById(parameter + '_um').children[0];
    for (child of document.getElementById(parameter + '_um').children) {
      if (config_values[parameter]/Number(child.value) >= 1) {
        current_val = config_values[parameter]/Number(child.value);
        current_um = child;
      }
    }
    if (!editing) {
      document.getElementById(parameter).value = current_val;
      current_um.selected = true;
    } else {
      if (document.getElementById(parameter).value == current_val) {
        document.getElementById(parameter).style.backgroundColor = '';
      } else {
        parameters_changed = true;
        document.getElementById(parameter).style.backgroundColor = '#4CAF50';
      }
      if (current_um.selected) {
        document.getElementById(parameter + '_um').style.backgroundColor = '';
      } else {
        parameters_changed = true;
        document.getElementById(parameter + '_um').style.backgroundColor = '#4CAF50';
      }
    }
  }
  if (parameters_changed) {
    document.getElementById('save_config').disabled = false;
  } else {
    document.getElementById('save_config').disabled = true;
    editing = false;
  }

/*
  document.getElementById('pH_threshold').value = config_values.pH_threshold;
  document.getElementById('cond_threshold').value = config_values.cond_threshold;
  document.getElementById('peristaltic_time').value = config_values.peristaltic_time;
  document.getElementById('control_cycle_time').value = config_values.control_cycle_time;
  document.getElementById('water_pump_cycle_time').value = config_values.water_pump_cycle_time;
  document.getElementById('water_pump_active_time').value = config_values.water_pump_active_time;
*/
}
