/*
 * Author: ikki Chung
 * Date: 2020.11.26
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var glShowCount = 0;

var glSensorInfo_List = [];
var glBeaconInfo_List = [];

var gl_SensorData_load_index = 0;


// start loading
check_loading_status();

function check_loading_status()
{
    //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" CurrentLoadIndex:" + CurrentLoadIndex);
    switch(CurrentLoadIndex)
    {
        case 0:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_sensor();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_SensorData();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_SensorData();
                show_BeaconData();

                CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(check_loading_status, 100);
    else
    {
        if (glShowCount == 0)
        {
            setTimeout(reload_page, 3000);
        }
        else
        {
            setTimeout(reload_page, 10000);
        }
        glShowCount++;
        // console.log("check_loading_status() Stop.");
    }
}

function reload_page()
{
    PreLoadIndex = -1;
    CurrentLoadIndex = 0;

    check_loading_status();
}

function load_sensor()
{
    glSensorInfo_List = [];
    glBeaconInfo_List = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadsensordevice=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, device)
        {
            // console.log("load_sensor() index:"+index+" device.type:"+device.type);

            var sensorinfo = {};
            sensorinfo.id = device.id;
            sensorinfo.name = device.name;
            sensorinfo.value = [];
            sensorinfo.day = [];
            sensorinfo.jsonvalue = [];
            sensorinfo.Longitude = [];
            sensorinfo.Latitude = [];
            sensorinfo.datetime = [];
            glSensorInfo_List.push(sensorinfo);

            if ( device.type == 'beacon')
            {
                var beaconInfo = {};
                beaconInfo.id = device.id;
                beaconInfo.name = device.name;
                beaconInfo.value = "";
                beaconInfo.jsonvalue = device.jsonvalue;
                beaconInfo.datetime = moment(device.datetime, "YYYY-MM-DD HH:mm:ss");
                glBeaconInfo_List.push(beaconInfo);
            }

        });
    })
    .success(function() {

        gl_SensorData_load_index = 0;

        // console.log("load_sensor() glBeaconInfo_List.length:"+glBeaconInfo_List.length);

        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_SensorData()
{
    if (gl_SensorData_load_index >= glSensorInfo_List.length)
    {
        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }

    var FETCH_NUM = 3;
    var loadCount = 0;
    var sensornames = '';
    for (var i = gl_SensorData_load_index; i < glSensorInfo_List.length && loadCount < FETCH_NUM; i++, loadCount++)
    {
        var sensorinfo = glSensorInfo_List[i];
        if (loadCount > 0)
            sensornames = sensornames + ',';
        sensornames = sensornames + sensorinfo.id;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadsensordata=1&project_id='+project_id+'&names='+sensornames, function(data)
    {
        var sensorinfo = null;

        $.each(data, function(index, sensordata)
        {
            if (sensorinfo == null || !(sensorinfo.id === sensordata.name) )
            {
                sensorinfo = get_SensorData(sensordata.name);
            }
            if (sensorinfo != null)
            {
                sensorinfo.value.push(sensordata.value);
                sensorinfo.day.push(sensordata.day);
                sensorinfo.jsonvalue.push(sensordata.jsonvalue);
                sensorinfo.Longitude.push(sensordata.Longitude);
                sensorinfo.Latitude.push(sensordata.Latitude);
                sensorinfo.datetime.push(sensordata.datetime);

                // sensorinfo.value.unshift(sensordata.value);
                // sensorinfo.day.unshift(sensordata.day);
                // sensorinfo.jsonvalue.unshift(sensordata.jsonvalue);
                // sensorinfo.Longitude.unshift(sensordata.Longitude);
                // sensorinfo.Latitude.unshift(sensordata.Latitude);
                // sensorinfo.datetime.unshift(sensordata.datetime);
            }

            if (sensorinfo.id === sensordata.name)
            {
                var beaconInfo = get_BeaconData(sensordata.name);
                if (beaconInfo != null)
                {
                    var beaconInfo_datetime = moment(sensordata.datetime, "YYYY-MM-DD HH:mm:ss");
                    if ( beaconInfo_datetime > beaconInfo.datetime)
                    {
                        beaconInfo.value = sensordata.value;
                        beaconInfo.datetime = beaconInfo_datetime;
                    }
                }
            }
        });
    })
    .success(function() {

        gl_SensorData_load_index += FETCH_NUM;

        if (gl_SensorData_load_index >= glSensorInfo_List.length)
        {
            CurrentLoadIndex = CurrentLoadIndex + 1;
        }
        else
        {
            setTimeout(load_SensorData, 1);
        }
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function show_SensorData()
{
    var loc_tab_sensors = document.getElementById('SENSOR_NODE_ACTIVITY_TABLE_BODY');

    for (var i = 0; i < glSensorInfo_List.length; i++)
    {
        var sensorinfo = glSensorInfo_List[i];

        // console.log("show_SensorData() sensorinfo.name:"+sensorinfo.name );

        var row = create_SensorData_item(i, sensorinfo);
        if (row != null)
        {
            loc_tab_sensors.appendChild(row);

            // console.log("show_SensorData() sensorinfo.value.length:"+sensorinfo.value.length);
        }
    }
}

function create_SensorData_item(ui_index, sensorinfo)
{
    if (sensorinfo == null)
        return null;

    var devicealivetime = sensorinfo.datetime[0];

    var UI_ROW_ID = "row_SensorData_"+ui_index;
    var UI_ROW_UPDATETIME_ID = "row_updatetime_SensorData_"+ui_index;
    var UI_ROW_ACTIVITY_ID = "row_activity_SensorData_"+ui_index;
    var row = document.getElementById(UI_ROW_ID);
    if ( row != null )
    {
        // update time
        var SpanUpdatetime = document.getElementById(UI_ROW_UPDATETIME_ID);
        SpanUpdatetime.textContent = devicealivetime;

        var idname = "#"+UI_ROW_ACTIVITY_ID;
        $(idname).sparkline(sensorinfo.value, {width: sensorinfo.value.length * 4, tooltipSuffix: ""});

        return null;
    }

    var row = document.createElement('tr');
    row.setAttribute("id", UI_ROW_ID);
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Name
    row.appendChild(document.createElement('td'));
    var SpanName = document.createElement("span");
    SpanName.setAttribute("class","badge bg-green");
    // Span2.setAttribute("class","badge bg-blue");
    // Span2.setAttribute("class","badge bg-red");
    // Span2.setAttribute("class","badge bg-yellow");
    SpanName.textContent = sensorinfo.name;
    row.cells[cellIndex].appendChild(SpanName);
    cellIndex++;

    // time
    //	Update Time
    row.appendChild(document.createElement('td'));
    var SpanTime = document.createElement("span");
    SpanTime.setAttribute("id", UI_ROW_UPDATETIME_ID);
    SpanTime.setAttribute("class","badge bg-light-yellow");
    SpanTime.textContent = devicealivetime;
    row.cells[cellIndex].appendChild(SpanTime);
    cellIndex++;

    //activity
    row.appendChild(document.createElement('td'));
    var SpanActivity = document.createElement("span");
    SpanActivity.setAttribute("id", UI_ROW_ACTIVITY_ID);
    //SpanActivity.setAttribute("class","sparkline-1");
    //SpanActivity.setAttribute("id","activity"+node_macaddress);
    //SpanActivity.innerHTML = mpoints.toString();
    //SpanActivity.innerHTML = "8,4,0,0,0,0,1,4,4,10,10,10,10,0,0,0,4,6,5,9,10";
    row.cells[cellIndex].appendChild(SpanActivity);
    cellIndex++;

    return row;
}

function show_BeaconData()
{
    var loc_tab_beacon = document.getElementById('BEACON_NODE_ACTIVITY_TABLE_BODY');
    while (loc_tab_beacon.firstChild)
    {
        loc_tab_beacon.removeChild(loc_tab_beacon.firstChild);
    }

    // Sort the numbers in descending order:
    glBeaconInfo_List = glBeaconInfo_List.sort(function(a, b){return (b.datetime - a.datetime)});

    // console.log("show_BeaconData() glBeaconInfo_List.length:" +  glBeaconInfo_List.length);

    for (var i = 0; i < glBeaconInfo_List.length; i++)
    {
        var beaconinfo = glBeaconInfo_List[i];

        var row = create_BeaconData_item(i, beaconinfo);
        if (row != null)
        {
            loc_tab_beacon.appendChild(row);
        }
    }
}

function create_BeaconData_item(ui_index, beaconinfo)
{
    if (beaconinfo == null || beaconinfo.datetime == undefined)
        return null;

    var devicealivetime = beaconinfo.datetime.format('YYYY-MM-DD HH:mm:ss');

    var difftime = moment().diff(beaconinfo.datetime); //  in milliseconds,

    var row = document.createElement('tr');

    if ( difftime >= 15000 )
    {
        row.setAttribute("style","background-color:rgb(125, 120, 120);");
    }

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Name
    row.appendChild(document.createElement('td'));
    var SpanName = document.createElement("span");
    SpanName.setAttribute("class","badge bg-green");
    // Span2.setAttribute("class","badge bg-blue");
    // Span2.setAttribute("class","badge bg-red");
    // Span2.setAttribute("class","badge bg-yellow");
    SpanName.textContent = beaconinfo.name;
    row.cells[cellIndex].appendChild(SpanName);
    cellIndex++;

    // time
    //	Update Time
    row.appendChild(document.createElement('td'));
    var SpanTime = document.createElement("span");
    // SpanTime.setAttribute("id", UI_ROW_UPDATETIME_ID);
    SpanTime.setAttribute("class","badge bg-light-yellow");
    SpanTime.textContent = devicealivetime;
    row.cells[cellIndex].appendChild(SpanTime);
    cellIndex++;

    //activity
    row.appendChild(document.createElement('td'));
    var SpanActivity = document.createElement("span");
    SpanActivity.textContent = beaconinfo.value +'  '+beaconinfo.jsonvalue;
    // SpanActivity.setAttribute("id", UI_ROW_ACTIVITY_ID);
    //SpanActivity.setAttribute("class","sparkline-1");
    //SpanActivity.setAttribute("id","activity"+node_macaddress);
    //SpanActivity.innerHTML = mpoints.toString();
    //SpanActivity.innerHTML = "8,4,0,0,0,0,1,4,4,10,10,10,10,0,0,0,4,6,5,9,10";
    row.cells[cellIndex].appendChild(SpanActivity);
    cellIndex++;

    return row;
}

function get_SensorData(sensorname)
{
    for (var i = 0; i < glSensorInfo_List.length; i++)
    {
        var sensorinfo = glSensorInfo_List[i];
        if ( sensorinfo.id === sensorname)
        {
            return sensorinfo;
        }
    }
    return null;
}

function get_BeaconData(sensorname)
{
    for (var i = 0; i < glBeaconInfo_List.length; i++)
    {
        var beaconinfo = glBeaconInfo_List[i];
        if ( beaconinfo.id === sensorname)
        {
            return beaconinfo;
        }
    }
    return null;
}
