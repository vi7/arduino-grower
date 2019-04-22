const ip = 'http://192.168.1.30:80';

function refreshTemperature() {
    $.ajax({
        type: "GET",
        url: ip + "/v1/dht/temperature",
        dataType: "json",
        success: function (result) {
            $("#temperature")[0].innerHTML = result.temperature + "°";
        }
    });
}

function refreshHumidity() {
    $.ajax({
        type: "GET",
        url: ip + "/v1/dht/humidity",
        dataType: "json",
        success: function (result) {
            $("#humidity")[0].innerHTML = result.humidity + "%";
        }
    });
}

function refreshPowerStatus() {
    $.ajax({
        type: "GET",
        url: ip + "/v1/relay/power",
        dataType: "json",
        success: function (result) {
            let power = $("#power-status")[0];
            power.innerHTML = result.power.toUpperCase();
            if (result.power) {
                power.style.color = "#28a745";
            } else {
                power.style.color = "#dc3545"
            }
        }
    });
}

$(document).ready(function () {
   refreshTemperature();
   refreshHumidity();
   refreshPowerStatus();
});


//todo implement on server side: send one response with all sensors values
//todo implement possibility to set server ip in input field