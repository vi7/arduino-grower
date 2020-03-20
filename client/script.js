const pinkponyIp = 'http://192.168.1.30';

function getIP() {
    let ip = document.getElementById("ip-input").value;
    if ('' !== ip) {
        return ip;
    } else return pinkponyIp;
}

function refreshTemperature() {
    $.ajax({
        type: "GET",
        url: getIP() + "/v2/dht/temperature",
        dataType: "json",
        success: function (result) {
            $("#temperature")[0].innerHTML = result.temperature + "°";
        }
    });
}

function refreshHumidity() {
    $.ajax({
        type: "GET",
        url: getIP() + "/v2/dht/humidity",
        dataType: "json",
        success: function (result) {
            $("#humidity")[0].innerHTML = result.humidity + "%";
        }
    });
}


function refreshPowerStatus(device) {
    $.ajax({
        type: "GET",
        url: getIP() + "/v2/relay/" + device + "/power/status",
        dataType: "json",
        success: function (result) {
            showPowerStat(device, result);
        }
    });
}

function showPowerStat(device, result) {
    let powerControl = document.getElementById(device + "-power-status");
    if (result.power === "1") {
        powerControl.innerHTML = "ON";
        powerControl.style.color = "green";
    } else {
        powerControl.innerHTML = "OFF";
        powerControl.style.color = "red"
    }
}

function powerOn(device) {
    $.ajax({
        type: "GET",
        url: getIP() + "/v2/relay/" + device + "/power/on",
        dataType: "json",
        success: function (result) {
            showPowerStat(device, result);
        }
    });
}

function powerOff(device) {
    $.ajax({
        type: "GET",
        url: getIP() + "/v2/relay/" + device + "/power/off",
        dataType: "json",
        success: function (result) {
            showPowerStat(device, result);
        }
    });
}

function refreshAll() {
    refreshTemperature();
    refreshHumidity();
    refreshPowerStatus("lamp");
    refreshPowerStatus("fan");
    refreshPowerStatus("hum");
}

$(document).ready(function () {
    $("#ip-input")[0].placeholder = pinkponyIp;
    refreshAll();
});
