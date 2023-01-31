/*
 * Author: ikki Chung
 * Created on: 2020/03/11
 * Description:
 *      SMIMS VILS RTLS Solutions event trigger
 **/


var glTags = [];
var glSensors = [];
// var glAnchors = [];

var gl_sensor_num = 0;
// var gl_anchor_num = 0;
// var gl_locator_num = 0;

var gl_sensor_load_index = 0;
// var gl_anchor_load_index = 0;
// var gl_locator_load_index = 0;

var LOAD_DEVICE_NUM = 20;

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;

var glInspectionGroups = [];
var glInspectionGroupDailyRecord = [];
var gl_inspectiongroup_num = 0;
var gl_inspectiongroup_load_index = 0;

var glInspectionPoint = [[]];
var gl_inspectionpointdaily_load_index = 0;
//var gl_TagBehavior_group_load_index = 0;
//var gl_TagBehavior_load_index = []; // for each group has different index

var gl_active_page_index = 1;// 1:daily 2:weekly 3:monthly
var glTargetDate = getTodayDate();
//
// var glWeeklyStartDate = getTargetDate(-6);
// var glWeeklyEndDate = getTargetDate(0);

var glMonthlyDate = getTargetMonth(0);
var glMonthlyStartDate = "";
var glMonthlyEndDate = "";

// var glDailyTableHeader = [];
// var glDailyTableData = [];
// var glWeeklyTableHeader = [];
// var glWeeklyTableData = [];
var glMonthlyTableHeader = [];
var glMonthlyTableData = [];

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
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_devices();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_monthly_table();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;


                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;


                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;


                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                document.getElementById("INSPECTION_REPORT_TABLE_MSG").innerText = "";
                PreLoadIndex = CurrentLoadIndex;
            }
        break;
    }

    if ( CurrentLoadIndex < 7 )
        setTimeout(check_loading_status, 100);
    else
    {
        console.log("check_loading_status() Stop.");
    }
}

function load_devices()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_sensor_num = parseInt(data['LOC_SENSOR_NUM']);

        gl_sensor_load_index = 0;
    })
    .success(function() {
        setTimeout(load_sensors, 10);
    })
    .error(function() {
        setTimeout(load_devices, 1000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_sensors()
{
    if (gl_sensor_load_index >= gl_sensor_num)
    {
        gl_sensor_load_index = 0;
        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }
    if (gl_sensor_load_index == 0)
    {
        glSensors = [];
    }
    var count = LOAD_DEVICE_NUM;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadsensordevice=1&s='+gl_sensor_load_index+'&count='+count+'&project_id='+project_id+'&showonline='+1+'&showoffline='+1, function(data)
    {
        // console.log("load_sensor() data:"+data);
        var sensor_num = 0;

        $.each(data, function(index, device)
        {
            // console.log("load_sensor() index:"+index+" device:"+device);
            sensor_num += 1;

            if ( device.type === 'iCover' )
            {
                glSensors.push(device);
            }
        });

        gl_sensor_load_index += sensor_num;
    })
    .success(function() {
        if (gl_sensor_load_index >= gl_sensor_num)
        {
            gl_sensor_load_index = 0;
            CurrentLoadIndex = CurrentLoadIndex + 1;
        }
        else
        {
            setTimeout(load_sensors, 10);
        }
    })
    .error(function() {
        gl_sensor_load_index = 0;
        setTimeout(load_sensors, 1000);
    })
    .complete(function() {
    });
}

function create_table_header_column(text_content, ui_width, text_color, border_color, bg_color)
{
    var theadth = document.createElement("th");
    theadth.setAttribute("bgcolor", bg_color);
    // bodycol.setAttribute("class","tabletd");
    if (ui_width == 0)
        theadth.setAttribute("style","text-align: center;border-color:"+border_color+";color:"+text_color);
    else
        theadth.setAttribute("style","text-align: center;border-color:"+border_color+";color:"+text_color+";width:"+ui_width+"px");
    theadth.textContent = text_content;
    return theadth;
}

function create_table_body_column(text_content, text_color, border_color, bg_color)
{
    var bodycol = document.createElement("td");
    bodycol.setAttribute("bgcolor", bg_color);
    bodycol.setAttribute("class","tabletd");
    bodycol.setAttribute("style","border-color:"+border_color+";color:"+text_color);
    bodycol.textContent = text_content;
    return bodycol;
}

function show_monthly_table()
{
    var status_date = document.getElementById("monthly_targetdate");
    if (status_date != undefined)
    {
        status_date.textContent = convertToShowMonth(glMonthlyDate);
    }

    var table_header = document.getElementById('MONTHLY_REPORT_TABLE_HEADER');
    while (table_header.firstChild)
    {
        table_header.removeChild(table_header.firstChild);
    }

    glMonthlyTableHeader = [];

    glMonthlyTableHeader.push("感測器");
    var th = create_table_header_column("感測器", 200, "#223344", "#223344", "#cce6ff");
    table_header.appendChild(th);

    glMonthlyTableHeader.push("定時回報數");
    var th = create_table_header_column("定時回報數", 200, "#223344", "#223344", "#ff9933");
    table_header.appendChild(th);

    glMonthlyTableHeader.push("警⽰事件");
    var th = create_table_header_column("警⽰事件", 0, "#223344", "#223344", "#ff9933");
    table_header.appendChild(th);

    var table_body = document.getElementById('MONTHLY_REPORT_TABLE');
    while (table_body.firstChild)
    {
        table_body.removeChild(table_body.firstChild);
    }

    glMonthlyTableData = [];

    for (var i = 0; i < glSensors.length; i++)
    {
        var sensorData = glSensors[i];

        var bodytr = document.createElement("tr");
        bodytr.setAttribute("class","tabletr");
        bodytr.setAttribute("id","monthly_table_row_"+ sensorData.id);
        table_body.appendChild(bodytr);

        var col = create_table_body_column(sensorData.name, "#223344", "#223344", "#fefefe");
        bodytr.appendChild(col);

        var col = create_table_body_column("--", "#00aa00", "#223344", "#fefefe");
        col.setAttribute("id","monthly_table_row_"+ sensorData.id+"_count");
        // col.setAttribute("id","monthly_table_row_Tag9E88_count");
        bodytr.appendChild(col);

        var col = create_table_body_column("--", "#00aa00", "#223344", "#fefefe");
        col.setAttribute("id","monthly_table_row_"+ sensorData.id+"_events");
        // col.setAttribute("id","monthly_table_row_Tag9E88_events");
        bodytr.appendChild(col);

        var target_month = moment(glMonthlyDate, "YYYYMM");
        var target_MonthStart = moment(target_month).startOf('month');
        var target_MonthEnd = moment(target_month).endOf('month');

        load_firedevent_count('777776', sensorData.id, target_MonthStart.format('YYYYMMDD'), target_MonthEnd.format('YYYYMMDD'));// OpenDoor
        load_firedevent('777777', sensorData.id, target_MonthStart.format('YYYYMMDD'), target_MonthEnd.format('YYYYMMDD'));// CloseDoor

        // load_firedevent_count('119999', 'Tag9E88', target_MonthStart.format('YYYYMMDD'), target_MonthEnd.format('YYYYMMDD'));
        // load_firedevent('119999', 'Tag9E88', target_MonthStart.format('YYYYMMDD'), target_MonthEnd.format('YYYYMMDD'));
    }
}

function create_event_content(index, event_table, tabletr, eventdata)
{
    if (index % 5 == 0)
    {
        tabletr = document.createElement("tr");
        event_table.appendChild(tabletr);
    }

    var tabletd = document.createElement("td");
    tabletd.setAttribute("class","tabletd");
    tabletd.setAttribute("style","font-weight: bold;border-color:#00aa00;color:#223344;padding-left:10px;width:30px;text-align: center;");
    tabletd.textContent = index + 1;
    tabletr.appendChild(tabletd);

    var tabletd = document.createElement("td");
    tabletd.setAttribute("class","tabletd");
    tabletd.setAttribute("style","border-color:#00aa00;color:#223344;width:180px;text-align: center;");
    tabletd.textContent = eventdata.datetime;
    tabletr.appendChild(tabletd);

    return tabletr;
}

function load_firedevent_count(eventid, sensorid, startDate, endDate)
{
    gl_firedevent_List = [];

	var targetURL = gl_target_server + "/php/eventfire.php";
    var querytext = 'project_id=1';
    querytext = querytext + '&loadcount=1';
    querytext = querytext + '&nodeid=' + sensorid;
    querytext = querytext + '&startdate='+startDate+'&enddate='+endDate;

    if(eventid.length > 0)
    {
        querytext = querytext + '&eventid=' + eventid;
    }

    var p = $.getJSON(targetURL, querytext, function(data)
    {
        var count_uiid = "monthly_table_row_"+ sensorid+"_count";
        var count_ui = document.getElementById(count_uiid);
        count_ui.textContent = data.count;

    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_firedevent(eventid, sensorid, startDate, endDate)
{
    gl_firedevent_List = [];

	var targetURL = gl_target_server + "/php/eventfire.php";
    var querytext = 'project_id=1';
    querytext = querytext + '&loadevent=1';
    querytext = querytext + '&nodeid=' + sensorid;
    querytext = querytext + '&startdate='+startDate+'&enddate='+endDate;

    if(eventid.length > 0)
    {
        querytext = querytext + '&eventid=' + eventid;
    }

    var p = $.getJSON(targetURL, querytext, function(data)
    {
        var events_uiid = "monthly_table_row_"+ sensorid+"_events";
        var events_table_parent = document.getElementById(events_uiid);
        events_table_parent.textContent = '';

        var event_table = document.createElement("table");
        events_table_parent.appendChild(event_table);

        var tabletr;
        $.each(data, function(index, eventdata)
        {
            tabletr = create_event_content(index, event_table, tabletr, eventdata);
        });

    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function tab_monthly_onclick()
{
    console.log("tab_monthly_onclick()");

    gl_active_page_index = 3;
    show_monthly_table();
}

function move_monthly_forward()
{
    var month_date = moment(glMonthlyDate, "YYYYMM");
    var target_month_date = month_date.add(1, 'month').format('YYYYMM');

    glMonthlyDate = target_month_date;

    show_monthly_table();
}

function move_monthly_backward()
{
    var month_date = moment(glMonthlyDate, "YYYYMM");
    var target_month_date = month_date.subtract(1, 'month').format('YYYYMM');

    glMonthlyDate = target_month_date;

    show_monthly_table();
}

function get_pdf_report(type, TableHeader, TableData, StartDate, EndDate)
{
    html2canvas($('#MONTHLY_REPORT_TABLE_PDF')[0], {
        width: 1600,
        height: 2000
    }).then(function (canvas) {

        var data = canvas.toDataURL();
        var docDefinition = {
            pageSize: 'A4',
            // by default we use portrait, you can change it to landscape if you wish
            pageOrientation: 'portrait',
            // [left, top, right, bottom] or [horizontal, vertical] or just a number for equal margins
            pageMargins: [20, 10, 20, 0],
            content: [{
                image: data,
                width: 640
            }]
        };

        pdfMake.createPdf(docDefinition).download("月報表.pdf");

    });
    return false;
}

function get_monthly_pdf_report()
{
    get_pdf_report("monthly", glMonthlyTableHeader, glMonthlyTableData, glMonthlyStartDate, glMonthlyEndDate);
}

function getNodeName(NodeId)
{
    var NodeName = NodeId;

    for (var i = 0; i < glAnchors.length; i++)
    {
        if ( glAnchors[i].itemid == NodeId)
        {
            NodeName = glAnchors[i].itemname;
            break;
        }
    }

    return NodeName;
}

function getInspectorName(InspectorId)
{
    var NodeName = InspectorId;

    for (var i = 0; i < glTags.length; i++)
    {
        var nodeData = glTags[i];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];

        if ( nodeid === InspectorId)
        {
            NodeName = nodename;
            break;
        }
    }

    return NodeName;
}

function isANumber(str)
{
    return !/\D/.test(str);
}

// get today date
function getTodayDate()
{
    var Today = moment(new Date());
    //var today = ""+ Today.getFullYear() + (Today.getMonth()+1) + Today.getDate();
    var today = Today.format('YYYYMMDD');

    //today = "20181218";
    return today;
}

function getTargetDate(addDays)
{
    var targetDate = moment().add(addDays, 'd').format('YYYYMMDD');
    return targetDate;
}

function getTargetMonth(addMonths)
{
    var targetDate = moment().add(addMonths, 'm').format('YYYYMM');
    return targetDate;
}

function convertToShowDate(inDate)
{
    var cur_date = moment(inDate, "YYYYMMDD");
    var outDate = cur_date.format('YYYY/MM/DD');

    return outDate;
}

function convertToShowMonth(inMonth)
{
    var cur_date = moment(inMonth, "YYYYMM");
    var outDate = cur_date.format('YYYY/MM');

    return outDate;
}

function convertRecordTime(inDate)
{
    var cur_date = moment(inDate, "YYYY-MM-DD HH:mm:ss");
    var outDate = cur_date.format('HH:mm:ss');

    return outDate;
}

function open_inspection_report_page()
{
    var win = window.open('/index.php/vilsinspectionreport/'+project_id, '_blank');
    win.focus();
}
