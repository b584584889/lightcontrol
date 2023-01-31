/*
 * Author: ikki Chung
 * Date: 26 July 2019
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/



var glTargetEndDate = getTargetDateTime(0);
var glTargetStartDate = getTargetDateTime(-1);

load_activity_user();
load_systeminfo();
load_sos_user();
load_area_user();
load_operation_mode();
load_inspection_event();
load_device_alive();
load_system_service();
load_nodemoveoffset();

update_query_time();

function update_query_time()
{
    glTargetEndDate = getTargetDateTime(0);
    glTargetStartDate = getTargetDateTime(-1);

    var data_range_datetime = document.getElementById('data_range_datetime');
    data_range_datetime.textContent = glTargetStartDate + " -> " + glTargetEndDate;

    setTimeout(update_query_time, 60000);
}

function load_activity_user()
{
    var targetURL = gl_target_server + "/php/dashboarddata.php";
    var p = $.getJSON(targetURL, 'loadactivity=1&nodetype=1'+'&project_id='+project_id, function(data)
    {
        //var users = data.users;
        //var records = data.records;

        //console.log("users.length:" + users.length + " records.length:" + records.length);

        //aliveString = "9,8,7,6,5,4,3,2,1,0,9,8,7,6,5,4,3,2,1,0,9,8,7,6,5,4,3,2,1,0"
        //aliveString = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0"
        // aliveString = "";
        // for(var index=0;index<records.length;index++)
        // {
        //     if (index != 0)
        //         aliveString += ",";
        //
        //     var record = records[index];
        //     aliveString += record.count;
        // }
        // aliveString += ",0";

        var main_activity_user_count = document.getElementById('main_activity_user_count');
        main_activity_user_count.textContent = data.devicecount;

        aliveString = data.records;
        aliveString += ",0";
        var main_activity_alive_count = document.getElementById('main_activity_alive_count');
        main_activity_alive_count.textContent = aliveString;

        var idname = "#main_activity_alive_count";
        $(idname).sparkline('html', {
          type    : 'bar',
          height  : '30',
          barColor: '#744',
          tooltipFormat: "{{offset:offset}} {{value}}",
          tooltipValueLookups:
          {
                'offset':
                {
                    0: '目前<br>活動用戶:',
                    1: '1分鐘前<br>活動用戶:',
                    2: '2分鐘前<br>活動用戶:',
                    3: '3分鐘前<br>活動用戶:',
                    4: '4分鐘前<br>活動用戶:',
                    5: '5分鐘前<br>活動用戶:',
                    6: '6分鐘前<br>活動用戶:',
                    7: '7分鐘前<br>活動用戶:',
                    8: '8分鐘前<br>活動用戶:',
                    9: '9分鐘前<br>活動用戶:',
                    10: '10分鐘前<br>活動用戶:',
                    11: '11分鐘前<br>活動用戶:',
                    12: '12分鐘前<br>活動用戶:',
                    13: '13分鐘前<br>活動用戶:',
                    14: '14分鐘前<br>活動用戶:',
                    15: '15分鐘前<br>活動用戶:',
                    16: '16分鐘前<br>活動用戶:',
                    17: '17分鐘前<br>活動用戶:',
                    18: '18分鐘前<br>活動用戶:',
                    19: '19分鐘前<br>活動用戶:',
                    20: '20分鐘前<br>活動用戶:',
                    21: '21分鐘前<br>活動用戶:',
                    22: '22分鐘前<br>活動用戶:',
                    23: '23分鐘前<br>活動用戶:',
                    24: '24分鐘前<br>活動用戶:',
                    25: '25分鐘前<br>活動用戶:',
                    26: '26分鐘前<br>活動用戶:',
                    27: '27分鐘前<br>活動用戶:',
                    28: '28分鐘前<br>活動用戶:',
                    29: '29分鐘前<br>活動用戶:',
                    30: ' ',
                }
            },
          // tooltipValueLookups: {
          //         levels: $.range_map({ ':-1': '分鐘前'})
          //     }
        });

    })
    .success(function() {
        setTimeout(load_activity_user, 30000);
    })
    .error(function() {
        setTimeout(load_activity_user, 30000);
    })
    .complete(function() {
    });
}


function load_systeminfo()
{
    var targetURL = gl_target_server + "/php/dashboarddata.php";
    var p = $.getJSON(targetURL, 'loadsysteminfo=1&project_id='+project_id, function(data)
    {
        var systeminfo_box_body = document.getElementById('systeminfo_box_body');

        // clear
        while (systeminfo_box_body.firstChild)
            systeminfo_box_body.removeChild(systeminfo_box_body.firstChild);

        var TotalDiskSize = parseFloat(data.TotalDiskSize)/1024.0;
        var TotalUsedSize = parseFloat(data.TotalUsedSize)/1024.0;
        var TotalFreeSize = parseFloat(data.TotalFreeSize)/1024.0;
        var DBUsedSize = parseFloat(data.DBUsedSize)/1024.0;

        systeminfo_box_body.appendChild(create_systeminfo_row("系統磁碟容量", TotalDiskSize.toFixed(2), "GB"));
        systeminfo_box_body.appendChild(create_systeminfo_row("已使用容量", TotalUsedSize.toFixed(2), "GB"));
        systeminfo_box_body.appendChild(create_systeminfo_row("可使用容量", TotalFreeSize.toFixed(2), "GB"));
        systeminfo_box_body.appendChild(create_systeminfo_row("資料庫使用容量", DBUsedSize.toFixed(2), "GB"));
        systeminfo_box_body.appendChild(create_systeminfo_row("記憶體使用", data.MemoryUsage, "MB"));
        systeminfo_box_body.appendChild(create_systeminfo_row("服務執行時間", data.SystemRunTime, ""));
        systeminfo_box_body.appendChild(create_systeminfo_row("CPU使用時間", data.CPURunTime, ""));


    })
    .success(function() {
        setTimeout(load_systeminfo, 120000);
    })
    .error(function() {
        setTimeout(load_systeminfo, 120000);
    })
    .complete(function() {
    });
}


function create_systeminfo_row(title, record, unit)
{
    var tablw_row = document.createElement("tr");

    var tablw_col_1 = document.createElement("td");
    tablw_col_1.textContent = title;
    tablw_row.appendChild(tablw_col_1);

    var tablw_col_2 = document.createElement("td");
    tablw_row.appendChild(tablw_col_2);

    var badge_alive = document.createElement("span");
    if (unit.length == 0)
        badge_alive.setAttribute("class","label label-warning");
    else
        badge_alive.setAttribute("class","label label-success");
    //badge_date.setAttribute("class","label label-info");
    //badge_date.setAttribute("class","label label-warning");
    //badge_date.setAttribute("class","label label-danger");
    badge_alive.textContent = record + ' ' + unit;
    tablw_col_2.appendChild(badge_alive);

    return tablw_row;
}


function load_sos_user()
{
    //console.log("load_sos_user() glTargetStartDate:" + glTargetStartDate+" glTargetEndDate:" + glTargetEndDate);

    var targetURL = gl_target_server + "/php/dashboarddata.php";
    var p = $.getJSON(targetURL, 'loadalert=1&startdate='+glTargetStartDate+'&enddate='+glTargetEndDate+'&project_id='+project_id, function(data)
    {
        var sos_box_body = document.getElementById('sos_box_body');

        // clear
        while (sos_box_body.firstChild)
            sos_box_body.removeChild(sos_box_body.firstChild);

        for (var index in data)
        {
            var record = data[index];
            var row  = create_sos_row(record);
            sos_box_body.appendChild(row);
        }

    })
    .success(function() {
        setTimeout(load_sos_user, 30000);
    })
    .error(function() {
        setTimeout(load_sos_user, 30000);
    })
    .complete(function() {
    });
}

function load_area_user()
{
    //console.log("load_area_user() glTargetStartDate:" + glTargetStartDate+" glTargetEndDate:" + glTargetEndDate);
    // var areauser_box_header = document.getElementById('areauser_box_header');
    // areauser_box_header.textContent = "區域內活動用戶數";

    var targetURL = gl_target_server + "/php/dashboarddata.php";
    var p = $.getJSON(targetURL, 'loadareauser=1&startdate='+glTargetStartDate+'&enddate='+glTargetEndDate, function(data)
    //var p = $.getJSON(targetURL, 'loadareauser=1'+'&project_id='+project_id, function(data)
    {
        var areauser_box_body = document.getElementById('areauser_box_body');
        // clear
        while (areauser_box_body.firstChild)
            areauser_box_body.removeChild(areauser_box_body.firstChild);

        for (var index in data)
        {
            var record = data[index];
            var row  = create_areauaer_row(record);
            areauser_box_body.appendChild(row);
        }

    })
    .success(function() {
        setTimeout(load_area_user, 30000);
    })
    .error(function() {
        setTimeout(load_area_user, 30000);
    })
    .complete(function() {
    });
}

function load_operation_mode()
{
    //console.log("load_operation_mode() glTargetStartDate:" + glTargetStartDate+" glTargetEndDate:" + glTargetEndDate);

    var targetURL = gl_target_server + "/php/dashboarddata.php";
    var p = $.getJSON(targetURL, 'loadoperationmode=1&startdate='+glTargetStartDate+'&enddate='+glTargetEndDate+'&project_id='+project_id, function(data)
    {
        var operationmode_box_body = document.getElementById('operationmode_box_body');
        // clear
        while (operationmode_box_body.firstChild)
            operationmode_box_body.removeChild(operationmode_box_body.firstChild);

        for (var index in data)
        {
            var record = data[index];
            if ( record == null )
                continue;
            if ( record.count > 0 )
            {
                var row  = create_operationmode_row(record);
                operationmode_box_body.appendChild(row);
            }
        }

    })
    .success(function() {
        setTimeout(load_operation_mode, 30000);
    })
    .error(function() {
        setTimeout(load_operation_mode, 30000);
    })
    .complete(function() {
    });
}

function load_inspection_event()
{
    //console.log("load_inspection_event() glTargetStartDate:" + glTargetStartDate+" glTargetEndDate:" + glTargetEndDate);

    var targetURL = gl_target_server + "/php/dashboarddata.php";
    var p = $.getJSON(targetURL, 'loadinspection=1&prettystring=1&startdate='+glTargetStartDate+'&enddate='+glTargetEndDate+'&project_id='+project_id, function(data)
    {
        var inspection_box_body = document.getElementById('inspection_box_body');
        // clear
        while (inspection_box_body.firstChild)
            inspection_box_body.removeChild(inspection_box_body.firstChild);

        for (var index in data)
        {
            var record = data[index];
            var row  = create_inspection_row(record);
            inspection_box_body.appendChild(row);
        }

    })
    .success(function() {
        setTimeout(load_inspection_event, 30000);
    })
    .error(function() {
        setTimeout(load_inspection_event, 30000);
    })
    .complete(function() {
    });
}

function load_device_alive()
{
    //console.log("load_device_alive()");

    var targetURL = gl_target_server + "/php/dashboarddata.php";
    var p = $.getJSON(targetURL, 'loaddevicealive=1'+'&project_id='+project_id, function(data)
    {
        var device_alive_box_body = document.getElementById('device_alive_box_body');
        // clear
        while (device_alive_box_body.firstChild)
            device_alive_box_body.removeChild(device_alive_box_body.firstChild);

        for (var index in data)
        {
            var record = data[index];

            //console.log("load_device_alive() type:" + record.type);

            var row  = create_devicealive_row(record);
            device_alive_box_body.appendChild(row);
        }

    })
    .success(function() {
        setTimeout(load_device_alive, 10000);
    })
    .error(function() {
        setTimeout(load_device_alive, 10000);
    })
    .complete(function() {
    });
}

function load_system_service()
{
    var targetURL = gl_target_server + "/php/dashboarddata.php";
    var p = $.getJSON(targetURL, 'loadsystemservice=1'+'&project_id='+project_id, function(data)
    {
        var system_service_box_body = document.getElementById('system_service_box_body');
        // clear
        while (system_service_box_body.firstChild)
            system_service_box_body.removeChild(system_service_box_body.firstChild);


        //console.log("load_system_service() total:" + data.total);

        var row  = create_systemservice_row(data);
        system_service_box_body.appendChild(row);

    })
    .success(function() {
        setTimeout(load_system_service, 10000);
    })
    .error(function() {
        setTimeout(load_system_service, 10000);
    })
    .complete(function() {
    });
}

function load_nodemoveoffset()
{
    var targetURL = gl_target_server + "/php/dashboarddata.php";
    var p = $.getJSON(targetURL, 'loadnodemoveoffset=1&startdate='+glTargetStartDate+'&enddate='+glTargetEndDate+'&project_id='+project_id, function(data)
    {
        var nodemoveoffset_box_body = document.getElementById('nodemoveoffset_box_body');
        // clear
        while (nodemoveoffset_box_body.firstChild)
            nodemoveoffset_box_body.removeChild(nodemoveoffset_box_body.firstChild);

        for (var index in data)
        {
            var record = data[index];

            var row  = create_nodemoveoffset_row(record);
            nodemoveoffset_box_body.appendChild(row);

            if (index >= 10)
                break;
        }
    })
    .success(function() {
        setTimeout(load_nodemoveoffset, 30000);
    })
    .error(function() {
        setTimeout(load_nodemoveoffset, 30000);
    })
    .complete(function() {
    });
}


function create_sos_row(record)
{
    var tablw_row = document.createElement("tr");

    var tablw_col_1 = document.createElement("td");
    //tablw_col_1.textContent = record.nodename;
    tablw_row.appendChild(tablw_col_1);

    var badge_nodename = document.createElement("span");
    badge_nodename.setAttribute("class","badge bg-green");
    badge_nodename.textContent = record.nodename;
    tablw_col_1.appendChild(badge_nodename);

    var tablw_col_2 = document.createElement("td");
    tablw_col_2.textContent = record.count;
    tablw_row.appendChild(tablw_col_2);

    var tablw_col_3 = document.createElement("td");
    tablw_row.appendChild(tablw_col_3);

    var badge_time = document.createElement("span");
    badge_time.setAttribute("class","label label-info");
    badge_time.textContent = convertPrettyCourseTime(record.max_time);
    tablw_col_3.appendChild(badge_time);


    return tablw_row;
}

function create_areauaer_row(record)
{
    var tablw_row = document.createElement("tr");

    var tablw_col_1 = document.createElement("td");
    tablw_row.appendChild(tablw_col_1);

    var badge_nodename = document.createElement("span");
    badge_nodename.setAttribute("class","label label-success");
    badge_nodename.textContent = record.areaname;
    tablw_col_1.appendChild(badge_nodename);

    var tablw_col_2 = document.createElement("td");
    tablw_col_2.textContent = record.count;
    tablw_row.appendChild(tablw_col_2);

    var tablw_col_3 = document.createElement("td");
    tablw_row.appendChild(tablw_col_3);

    var averageTime = parseInt(record.totaltime) / parseInt(record.count);

    var badge_time = document.createElement("span");
    //badge_time.setAttribute("class","label label-success");
    //badge_time.setAttribute("class","label label-info");
    badge_time.setAttribute("class","label label-warning");
    //badge_time.setAttribute("class","label label-danger");
    badge_time.textContent = convertPrettyTime(convertFormatTime(averageTime));
    tablw_col_3.appendChild(badge_time);


    return tablw_row;
}

function create_operationmode_row(record)
{
    var tablw_row = document.createElement("tr");

    var tablw_col_1 = document.createElement("td");
    tablw_row.appendChild(tablw_col_1);

    var badge_nodename = document.createElement("span");
    badge_nodename.setAttribute("class","label label-success");
    if ( record.mode == '0' )
        badge_nodename.textContent = ' ';
    else
    if ( record.mode == '1' )
        badge_nodename.textContent = ' UWB';
    else
    if ( record.mode == '2' )
        badge_nodename.textContent = ' LoRa_Ranging';
    else
    if ( record.mode == '3' )
        badge_nodename.textContent = ' GPS';

    tablw_col_1.appendChild(badge_nodename);

    var tablw_col_2 = document.createElement("td");
    tablw_col_2.textContent = record.count;
    tablw_row.appendChild(tablw_col_2);

    var tablw_col_3 = document.createElement("td");
    tablw_row.appendChild(tablw_col_3);

    var averageTime = parseInt(record.totaltime) / parseInt(record.count);

    var badge_time = document.createElement("span");
    //badge_time.setAttribute("class","label label-success");
    //badge_time.setAttribute("class","label label-info");
    badge_time.setAttribute("class","label label-warning");
    //badge_time.setAttribute("class","label label-danger");
    badge_time.textContent = convertPrettyTime(convertFormatTime(averageTime));
    tablw_col_3.appendChild(badge_time);


    return tablw_row;
}

function create_inspection_row(record)
{
    var eventjson = JSON.parse(record.eventjson);

    var tablw_row = document.createElement("tr");

    var tablw_col_1 = document.createElement("td");
    tablw_row.appendChild(tablw_col_1);

    var badge_nodename = document.createElement("span");
    badge_nodename.setAttribute("class","label label-success");
    badge_nodename.textContent = eventjson.GroupName;

    tablw_col_1.appendChild(badge_nodename);

    var tablw_col_2 = document.createElement("td");
    tablw_col_2.textContent = convertPrettyInspectionEvent(eventjson, record.prettyjson);
    tablw_row.appendChild(tablw_col_2);

    var tablw_col_3 = document.createElement("td");
    tablw_row.appendChild(tablw_col_3);

    var badge_date = document.createElement("span");
    //badge_time.setAttribute("class","label label-success");
    badge_date.setAttribute("class","label label-info");
    //badge_date.setAttribute("class","label label-warning");
    //badge_time.setAttribute("class","label label-danger");
    badge_date.textContent = convertPrettyDateOnly(record.datetime);
    tablw_col_3.appendChild(badge_date);

    var tablw_col_4 = document.createElement("td");
    tablw_row.appendChild(tablw_col_4);

    var badge_time = document.createElement("span");
    //badge_time.setAttribute("class","label label-success");
    //badge_time.setAttribute("class","label label-info");
    badge_time.setAttribute("class","label label-warning");
    //badge_time.setAttribute("class","label label-danger");
    badge_time.textContent = convertPrettyTimeOnly(record.datetime);
    tablw_col_4.appendChild(badge_time);


    return tablw_row;
}

function create_devicealive_row(record)
{
    var tablw_row = document.createElement("tr");

    var tablw_col_1 = document.createElement("td");
    tablw_row.appendChild(tablw_col_1);

    var badge_type = document.createElement("span");
    if ( record.type == 0 )
    {
        badge_type.setAttribute("class","badge bg-green-active");
        badge_type.textContent = '基站';
    }
    if ( record.type == 1 )
    {
        badge_type.setAttribute("class","badge bg-light-blue-active");
        badge_type.textContent = '標籤';
    }
    if ( record.type == 2 )
    {
        badge_type.setAttribute("class","badge bg-red-active");
        badge_type.textContent = '閘道';
    }
    if ( record.type == 3 )
    {
        badge_type.setAttribute("class","badge bg-yellow-active");
        badge_type.textContent = 'Locator';
    }
    if ( record.type == 4 )
    {
        badge_type.setAttribute("class","badge bg-purple-active");
        badge_type.textContent = 'LoRa Gateway';
    }
    tablw_col_1.appendChild(badge_type);

    var tablw_col_2 = document.createElement("td");
    tablw_col_2.textContent = record.total;
    tablw_row.appendChild(tablw_col_2);

    var tablw_col_3 = document.createElement("td");
    tablw_row.appendChild(tablw_col_3);

    var badge_alive = document.createElement("span");
    if (record.alive == 0)
        badge_alive.setAttribute("class","label label-danger");
    else
        badge_alive.setAttribute("class","label label-success");
    //badge_alive.setAttribute("class","label label-info");
    //badge_alive.setAttribute("class","label label-warning");
    //badge_alive.setAttribute("class","label label-danger");
    badge_alive.textContent = record.alive;
    tablw_col_3.appendChild(badge_alive);

    var tablw_col_4 = document.createElement("td");
    tablw_row.appendChild(tablw_col_4);

    var offline = record.total - record.alive;
    var badge_offline = document.createElement("span");
    badge_offline.textContent = offline;
    if ( offline != 0)
        badge_offline.setAttribute("class","label label-danger");
    //badge_offline.setAttribute("class","label label-info");
    //badge_offline.setAttribute("class","label label-warning");
    tablw_col_4.appendChild(badge_offline);

    return tablw_row;
}

function create_systemservice_row(record)
{
    var tablw_row = document.createElement("tr");

    var tablw_col_1 = document.createElement("td");
    tablw_col_1.textContent = record.total;
    tablw_row.appendChild(tablw_col_1);

    var tablw_col_2 = document.createElement("td");
    tablw_row.appendChild(tablw_col_2);

    var badge_alive = document.createElement("span");
    if (record.alive == 0)
        badge_alive.setAttribute("class","label label-danger");
    else
        badge_alive.setAttribute("class","label label-success");
    //badge_date.setAttribute("class","label label-info");
    //badge_date.setAttribute("class","label label-warning");
    //badge_date.setAttribute("class","label label-danger");
    badge_alive.textContent = record.alive;
    tablw_col_2.appendChild(badge_alive);

    var tablw_col_3 = document.createElement("td");
    tablw_row.appendChild(tablw_col_3);

    var offline = record.total - record.alive;

    var badge_offline = document.createElement("span");
    //badge_offline.setAttribute("class","label label-success");
    //badge_offline.setAttribute("class","label label-info");
    //badge_offline.setAttribute("class","label label-warning");
    if ( offline != 0)
        badge_offline.setAttribute("class","label label-danger");
    badge_offline.textContent = offline;
    tablw_col_3.appendChild(badge_offline);

    return tablw_row;
}

function create_nodemoveoffset_row(record)
{
    var tablw_row = document.createElement("tr");

    var tablw_col_1 = document.createElement("td");

    var badge_name = document.createElement("span");
    badge_name.setAttribute("class","badge bg-light-blue-active");
    badge_name.textContent = record.nodename;
    tablw_col_1.appendChild(badge_name);
    tablw_row.appendChild(tablw_col_1);

    var tablw_col_2 = document.createElement("td");
    tablw_row.appendChild(tablw_col_2);

    var badge_moveoffset = document.createElement("span");
    badge_moveoffset.setAttribute("class","label label-success");

    var moveoffset = parseInt(record.moveoffset) / 100;
    badge_moveoffset.textContent = parseInt(moveoffset);
    tablw_col_2.appendChild(badge_moveoffset);

    var tablw_col_3 = document.createElement("td");
    tablw_row.appendChild(tablw_col_3);

    var badge_time = document.createElement("span");
    badge_time.setAttribute("class","label label-warning");
    badge_time.textContent = convertPrettyTime(convertFormatTime(record.minutes * 60));
    tablw_col_3.appendChild(badge_time);

    return tablw_row;
}

function convertPrettyInspectionEvent(eventjson, prettyjson)
{
    // {"type":"checkpoint","projectid":"1","GroupID":"5","PathIndex":"4","TagNodeId":"Tag126","action":"enter","NodeId":"An012","issuetime":"2018-12-06 15:26:14"}
    // {"type":"checkpoint","projectid":"1","GroupID":"5","PathIndex":"3","TagNodeId":"Tag126","action":"leave","NodeId":"An057","issuetime":"2018-12-06 15:25:59"}
    // {"type":"checkpointpath","projectid":"1","GroupID":"5","PathIndex":"3","TagNodeId":"Tag126","action":"exceed","StartNodeId":"An057","EndNodeId":"An012","issuetime":"2018-12-06 15:26:23"}
    // {"type":"checkpointpath","projectid":"1","GroupID":"5","PathIndex":"3","TagNodeId":"Tag126","action":"finish","StartNodeId":"An057","EndNodeId":"An012","issuetime":"2018-12-06 15:26:26"}
    // {"type":"checkpointgroup","projectid":"1","GroupID":"85","GroupName":"巡檢路線6","action":"exceed","issuetime":"2019-06-14 16:30:02"}

    // {"type":"inspectionpoint","projectid":"1","GroupID":"4","PointIndex":"2","Inspector":"Tag0001","action":"leave","NodeId":"Locator00008","issuetime":"2020-03-13 09:07:25"}
    // {"type":"inspectionpointtravel","projectid":"1","GroupID":"4","PointIndex":"2","Inspector":"Tag0001","action":"finish","StartNodeId":"Locator00007","EndNodeId":"Locator00008","issuetime":"2020-03-13 09:07:25"}
    // {"type":"inspectionGroup","projectid":"1","GroupID":"4","GroupName":"巡檢路線2","action":"finish","issuetime":"2020-03-13 09:07:30"}


    if ( eventjson.type === 'inspectionpoint')
    {
        var eventMsg = "巡檢路線:" + eventjson.GroupID +" ";
        eventMsg = "巡檢員 " + prettyjson.tagname;
        //eventMsg = "巡檢員 " + eventjson.Inspector;

        if ( eventjson.action === 'enter' )
            eventMsg += ' 進入';
        else
        if ( eventjson.action === 'leave' )
            eventMsg += '離開';

        eventMsg += " 巡檢點:"+ prettyjson.inspectionpointname;
        //eventMsg += " 巡檢點:"+ eventjson.NodeId;

        return eventMsg;
    }
    else
    if ( eventjson.type === 'inspectionpointtravel')
    {
        var eventMsg = "巡檢路線:" + eventjson.GroupID +" ";
        //eventMsg = "巡檢員 " + prettyjson.tagname + " 巡檢路線:"+ prettyjson.startinspectionpointname + " -> " + prettyjson.endinspectionpointname;
        //eventMsg = "巡檢員 " + eventjson.Inspector + " 巡檢路線:"+ eventjson.StartNodeId + " -> " + eventjson.EndNodeId;

        eventMsg = "巡檢員 " + prettyjson.tagname + " 巡檢路線:";
        if ( prettyjson.startinspectionpointname != null && prettyjson.endinspectionpointname != null )
            eventMsg += prettyjson.startinspectionpointname + " -> " + prettyjson.endinspectionpointname;
        else
        if ( prettyjson.startinspectionpointname != null)
            eventMsg += prettyjson.startinspectionpointname ;
        else
        if ( prettyjson.endinspectionpointname != null )
            eventMsg += prettyjson.endinspectionpointname;

        if ( eventjson.action === 'exceed' )
        {
            eventMsg += ' 巡檢路線逾時';
        }
        else
        if ( eventjson.action === 'finish' )
        {
            eventMsg += '巡檢路線已完成';
        }

        return eventMsg;
    }
    else
    if ( eventjson.type === 'inspectionGroup')
    {
        if ( eventjson.action === 'exceed' )
        {
            return '巡檢已逾時';
        }
        else
        if ( eventjson.action === 'finish' )
        {
            return '巡檢已完成';
        }
    }

}

function getTargetDateTime(addDays)
{
    var targetDate = moment().add(addDays, 'd').format('YYYY-MM-DD HH:mm:ss');
    return targetDate;
}

function convertFormatTime(seconds)
{
    const formatted = moment.utc(seconds*1000).format('DD HH:mm:ss');
    return formatted;
}

function convertPrettyTime(inDate)
{
    // var cur_date = moment(inDate, "HH:mm:ss");
    var cur_date = moment(inDate, "DD HH:mm:ss");
    var day = cur_date.day() - 3;
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    // console.log("convertPrettyTime() inDate:"+inDate+" day:"+day);

    var perty_str = "";
    if (day > 0)
        perty_str += day + "天";
    if (hour > 0)
        perty_str += hour + "小時";
    if (minutes > 0)
        perty_str += minutes + "分";
    if (seconds > 0)
        perty_str += seconds + "秒";

    return perty_str;
}

function convertPrettyDateOnly(inTime)
{
    var cur_date = moment(inTime, "YYYY-MM-DD HH:mm:ss");
    var year = cur_date.format('YYYY');
    var month = cur_date.format('M');
    var day = cur_date.format('D');

    var perty_str = "";
    perty_str = year + "年" + month + "月" + day + "日 ";
    return perty_str;
}

function convertPrettyTimeOnly(inTime)
{
    var cur_date = moment(inTime, "YYYY-MM-DD HH:mm:ss");
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var perty_str = "";
    if (hour >= 0)
        perty_str += hour + "點";
    if (minutes >= 0)
        perty_str += minutes + "分";
    if (seconds >= 0)
        perty_str += seconds + "秒";

    return perty_str;
}

function convertPrettyCourseTime(inTime)
{
    var cur_date = moment(inTime, "YYYY-MM-DD HH:mm:ss");
    var year = cur_date.format('YYYY');
    var month = cur_date.format('M');
    var day = cur_date.format('D');
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var diffDays = moment().diff(cur_date, 'days');

    var inDate = cur_date.format('YYYY-MM-DD');
    var todayDate = moment().format('YYYY-MM-DD');

    //console.log("convertPrettyCourseTime() inTime:"+inTime+" inDate:"+inDate+" todayDate:"+todayDate);

    var perty_str = "";
    if ( !(inDate === todayDate) )
    {
        perty_str = year + "年" + month + "月" + day + "日 ";
    }

    if (hour > 0)
        perty_str += hour + "點";
    if (minutes > 0)
        perty_str += minutes + "分";
    if (seconds > 0)
        perty_str += seconds + "秒";

    return perty_str;
}

function open_dashboard_page()
{
    var win = window.open('/index.php/vilsdashboard/'+project_id, '_blank');
    win.focus();
}
