const pinkponyIp = 'http://192.168.1.30:80';

function getIP() {
    let ip = document.getElementById("ip-input").value;
    if ('' !== ip) {
        return ip;
    }
    return pinkponyIp;
}

function refreshTemperature() {
    $.ajax({
        type: "GET",
        url: getIP() + "/v1/dht/temperature",
        dataType: "json",
        success: function (result) {
            $("#temperature")[0].innerHTML = result.temperature + "°";
        }
    });
}

function refreshHumidity() {
    $.ajax({
        type: "GET",
        url: getIP() + "/v1/dht/humidity",
        dataType: "json",
        success: function (result) {
            $("#humidity")[0].innerHTML = result.humidity + "%";
        }
    });
}

function refreshPowerStatus() {
    $.ajax({
        type: "GET",
        url: getIP() + "/v1/relay/power",
        dataType: "json",
        success: function (result) {
            let powerControl = $("#power-status")[0];
            if (result.power === "1") {
                powerControl.innerHTML = "ON";
                powerControl.style.color = "green";
            } else {
                powerControl.innerHTML = "OFF";
                powerControl.style.color = "red"
            }
        }
    });
}

function refreshAll() {
    refreshTemperature();
    refreshHumidity();
    refreshPowerStatus();
}

$(document).ready(function () {
    refreshAll();
});
