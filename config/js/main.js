function getConfigData() {
    var backgroundColorPicker = document.getElementById('background_color_picker');
    var labelColorPicker = document.getElementById('label_color_picker');
    var hourMarkColorPicker = document.getElementById('hour_mark_color_picker');
    var minuteMarkColorPicker = document.getElementById('minute_mark_color_picker');
    var hourHandColorPicker = document.getElementById('hour_hand_color_picker');
    var minuteHandColorPicker = document.getElementById('minute_hand_color_picker');
    var secondHandColorPicker = document.getElementById('second_hand_color_picker');
    var numbersCheckbox = document.getElementById('show_numbers_checkbox');
    var secondHandCheckbox = document.getElementById('show_second_hand_checkbox');
    var dateCheckbox = document.getElementById('show_date_checkbox');
    var stepsCheckbox = document.getElementById('show_steps_checkbox');
    var temperatureCheckbox = document.getElementById('show_temperature_checkbox');
    var imperialUnits = document.getElementById('imperial_radio');
    var metricUnits = document.getElementById('metric_radio');

    var units = 'imperial';
    if (metricUnits.checked) {
      units = 'metric';
    }

    var options = {
      'background_color': backgroundColorPicker.value,
      'label_color': labelColorPicker.value,
      'hour_mark_color': hourMarkColorPicker.value,
      'minute_mark_color': minuteMarkColorPicker.value,
      'hour_hand_color': hourHandColorPicker.value,
      'minute_hand_color': minuteHandColorPicker.value,
      'second_hand_color': secondHandColorPicker.value,
      'show_numbers': numbersCheckbox.checked,
      'show_second_hand': secondHandCheckbox.checked,
      'show_date': dateCheckbox.checked,
      'show_steps': stepsCheckbox.checked,
      'show_temperature': temperatureCheckbox.checked,
      'units': units
    };

    // Save for next launch
    localStorage.background_color = options.background_color;
    localStorage.label_color = options.label_color;
    localStorage.hour_mark_color = options.hour_mark_color;
    localStorage.minute_mark_color = options.minute_mark_color;
    localStorage.hour_hand_color = options.hour_hand_color;
    localStorage.minute_hand_color = options.minute_hand_color;
    localStorage.second_hand_color = options.second_hand_color;
    localStorage.show_numbers = options.show_numbers;
    localStorage.show_second_hand = options.show_second_hand;
    localStorage.show_date = options.show_date;
    localStorage.show_steps = options.show_steps;
    localStorage.show_temperature = options.show_temperature;
    localStorage.units = options.units;

    console.log('Got options: ' + JSON.stringify(options));
    return options;
  }

  function getQueryParam(variable, defaultValue) {
    var query = location.search.substring(1);
    var vars = query.split('&');
    for (var i = 0; i < vars.length; i++) {
      var pair = vars[i].split('=');
      if (pair[0] === variable) {
        return decodeURIComponent(pair[1]);
      }
    }
    return defaultValue || false;
  }

  var submitButton = document.getElementById('submit_button');
  submitButton.addEventListener('click', function() {
    console.log('Submit');

    // Set the return URL depending on the runtime environment
    var return_to = getQueryParam('return_to', 'pebblejs://close#');
    document.location = return_to + encodeURIComponent(JSON.stringify(getConfigData()));
  });

  (function() {
      var backgroundColorPicker = document.getElementById('background_color_picker');
      var labelColorPicker = document.getElementById('label_color_picker');
      var hourMarkColorPicker = document.getElementById('hour_mark_color_picker');
      var minuteMarkColorPicker = document.getElementById('minute_mark_color_picker');
      var hourHandColorPicker = document.getElementById('hour_hand_color_picker');
      var minuteHandColorPicker = document.getElementById('minute_hand_color_picker');
      var secondHandColorPicker = document.getElementById('second_hand_color_picker');
      var numbersCheckbox = document.getElementById('show_numbers_checkbox');
      var secondHandCheckbox = document.getElementById('show_second_hand_checkbox');
      var dateCheckbox = document.getElementById('show_date_checkbox');
      var stepsCheckbox = document.getElementById('show_steps_checkbox');
      var temperatureCheckbox = document.getElementById('show_temperature_checkbox');
      var imperialUnits = document.getElementById('imperial_radio');
      var metricUnits = document.getElementById('metric_radio');

    // Load any previously saved configuration, if available
    if (localStorage['background_color']) backgroundColorPicker.value = localStorage['background_color'];
    if (localStorage['label_color']) labelColorPicker.value = localStorage['label_color'];
    if (localStorage['hour_mark_color']) hourMarkColorPicker.value = localStorage['hour_mark_color'];
    if (localStorage['minute_mark_color']) minuteMarkColorPicker.value = localStorage['minute_mark_color'];
    if (localStorage['hour_hand_color']) hourHandColorPicker.value = localStorage['hour_hand_color'];
    if (localStorage['minute_hand_color']) minuteHandColorPicker.value = localStorage['minute_hand_color'];
    if (localStorage['second_hand_color']) secondHandColorPicker.value = localStorage['second_hand_color'];
    if (localStorage['show_numbers']) numbersCheckbox.checked = JSON.parse(localStorage['show_numbers']);
    if (localStorage['show_second_hand']) secondHandCheckbox.checked = JSON.parse(localStorage['show_second_hand']);
    if (localStorage['show_date']) dateCheckbox.checked = JSON.parse(localStorage['show_date']);
    if (localStorage['show_steps']) stepsCheckbox.checked = JSON.parse(localStorage['show_steps']);
    if (localStorage['show_temperature']) temperatureCheckbox.checked = JSON.parse(localStorage['show_temperature']);
    if (localStorage['units']) {
      if (localStorage['units'] == 'metric') {
        metricUnits.checked = true;
        imperialUnits.checked = false;
      }
    }
  })();