// Dom7
var $$ = Dom7;

// Framework7 App main instance
var app  = new Framework7({
  root: '#app', // App root element
  id: 'io.framework7.testapp', // App bundle ID
  name: 'Framework7', // App name
  theme: 'ios', // Automatic theme detection
});

// Init/Create main view
var mainView = app.views.create('.view-main', {
  url: '/'
});

app.request.get('check_relay_one_status', function (data) {
  $$('#relay-one').html(JSON.parse(data).relayone_status == "true" ? "&nbsp&nbspOn&nbsp&nbsp" : "&nbsp&nbspOff&nbsp&nbsp")
  .toggleClass(JSON.parse(data).relayone_status == "true" ? "color-green" : "color-red")
  .addClass(JSON.parse(data).relayone_status == "true" ? "color-green" : "color-red");
});

app.request.get('check_relay_two_status', function (data) {
  $$('#relay-two').html(JSON.parse(data).relaytwo_status == "true" ? "&nbsp&nbspOn&nbsp&nbsp" : "&nbsp&nbspOff&nbsp&nbsp")
  .toggleClass(JSON.parse(data).relaytwo_status == "true" ? "color-green" : "color-red")
  .addClass(JSON.parse(data).relaytwo_status == "true" ? "color-green" : "color-red");
});

var temp_gauge = app.gauge.get('.temp-gauge');
var humid_gauge = app.gauge.get('.humid-gauge');


// setTimeout Example
(function loopingFunction() {
    app.request.get('get_dh11_data', function (data) {
      temp_gauge.update({
        value: JSON.parse(data).temperature / 100,
        valueText: JSON.parse(data).temperature + " °C"
      });
      
      humid_gauge.update({
        value: JSON.parse(data).humidity / 100,
        valueText: JSON.parse(data).humidity + " %"
      });
    });
    setTimeout(loopingFunction, 10000);
})();

//- Two groups
var ac1 = app.actions.create({
  buttons: [
    // First group
    [
      {
        text: '<span><i class="icon f7-icons">home</i> Tableau de bord</span>',
        label: true,
        bold: true
      },
      {
        text: '<span><i class="childroom icon f7-icons">bulb</i> Chambre d\'enfant</span>',
        onClick: function () {
          app.request.get('relay_one', function (data) {
            $$('#relay-one').html(JSON.parse(data).relayone_status == "true" ? "&nbsp&nbspOn&nbsp&nbsp" : "&nbsp&nbspOff&nbsp&nbsp")
            .toggleClass(JSON.parse(data).relayone_status == "true" ? "color-red" : "color-green")
            .addClass(JSON.parse(data).relayone_status == "true" ? "color-green" : "color-red");
          });
        }
      },
      {
        text: '<span><i class="icon f7-icons">bulb</i> Chambre des parents</span>',
        onClick: function () {
          app.request.get('relay_two', function (data) {
            $$('#relay-two').html(JSON.parse(data).relaytwo_status == "true" ? "&nbsp&nbspOn&nbsp&nbsp" : "&nbsp&nbspOff&nbsp&nbsp")
            .toggleClass(JSON.parse(data).relaytwo_status == "true" ? "color-red" : "color-green")
            .addClass(JSON.parse(data).relaytwo_status == "true" ? "color-green" : "color-red");
          });
        }
      }
    ],
    // Second group
    [
      {
        text: 'Annuler',
        color: 'red'
      }
    ]
  ]
});

$$('.ac-1').on('click', function () {
    ac1.open();
});