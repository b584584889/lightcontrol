/*
 * Author: ikki Chung
 * Date: 10 Aug 2021
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/
var glNFCdevices = [];
var gl_nfcdevcice_num = 0;

var glAllAreas = [];// put area info
var glAllUsers = [];
var gl0DSection = [];
var glCurrentAreaUser = [];
var glCurrentArea_Records = [];
var glCurrentAreaUserAirRemain = [];

var glSensor_List = [];
var gl_sensor_num = 0;
var gl_sensor_load_index = 0;

var glUserBodyRecords = {};
var gl_bodyinfo_load_index = 0;

var glCurrentNFCdevice = "";
var glCurrentArea = "";

var glManualChanged = false;
var glAutoRunStarted = false;
var glFirstRun = true;

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

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

                load_NFCdevices();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_nfcdevice_to_UI();
                load_user_info();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_maparea();
                load_0dsection();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_area_user();
                runLive_sensor();
                runLive_userActiviry();
                runLive_bodyinfo_records();

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                upate_area_user_time();

                CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(check_loading_status, 100);
    else
    {
        console.log("check_loading_status() Stop.");
    }
}

function load_NFCdevices()
{
    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadnfclocator=1&s=0&count=100&project_id='+project_id+'&showonline=1&showoffline=1', function(data)
    {
        var nfclocator_size = 0;

        $.each(data, function(index, device)
        {
            glNFCdevices[nfclocator_size] = {};
            glNFCdevices[nfclocator_size].itemid = device.macaddress;
            glNFCdevices[nfclocator_size].itemname = device.name;
            glNFCdevices[nfclocator_size].posX = 0;
            glNFCdevices[nfclocator_size].posY = 0;
            glNFCdevices[nfclocator_size].posZ = 0;
            glNFCdevices[nfclocator_size].type = "nfcdevice";
            nfclocator_size += 1;
        });

        gl_nfcdevcice_num += nfclocator_size;

    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    });
}

function load_user_info()
{
    glAllUsers = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var queryParameter = 'loaduserbaseinfo=1&project_id='+project_id;

    var p = $.getJSON(targetURL, queryParameter, function(data)
    {
        $.each(data, function(index, user)
        {
            glAllUsers[index] = [];
            glAllUsers[index] = user;
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function show_nfcdevice_to_UI()
{
    var nfclocator_selection_btn_group = document.getElementById('nfclocator_selection');
    load_Items_to_newevent_UI("nfclocator", "btn-info", nfclocator_selection_btn_group, glNFCdevices);
}

function load_Items_to_newevent_UI(itemtype, uitype, tag_selection_btn_group, items)
{
    //var tags = [];
    //var index = 0;
    var itemid = "";
    var itemname = "";

    console.log("load_Items_to_newevent_UI() items.length:"+items.length +" itemtype:"+itemtype);

    if ( items.length > 0 )
    {
        itemid = items[0].itemid;
        itemname = items[0].itemname;
    }

    var btn_group1_button1 = document.createElement('button');
    tag_selection_btn_group.appendChild(btn_group1_button1);
    btn_group1_button1.setAttribute("class","btn "+uitype);
    btn_group1_button1.setAttribute("data-id", itemid);
    btn_group1_button1.id = "section_"+itemtype+"_0_name";
    var btn_group1_button1_text = document.createTextNode(itemname);
    btn_group1_button1.appendChild(btn_group1_button1_text);

    // dropdown button
    var btn_group1_button2 = document.createElement('button');
    tag_selection_btn_group.appendChild(btn_group1_button2);
    btn_group1_button2.setAttribute("class","btn "+uitype+" dropdown-toggle");
    btn_group1_button2.setAttribute("data-toggle", "dropdown");
    btn_group1_button2.id = "section_"+itemtype+"_0";

    var btn_group1_button2_span1 = document.createElement('span');
    btn_group1_button2.appendChild(btn_group1_button2_span1);
    btn_group1_button2_span1.setAttribute("class","caret");

    var btn_group1_button2_span2 = document.createElement('span');
    btn_group1_button2.appendChild(btn_group1_button2_span2);
    btn_group1_button2_span2.setAttribute("class","sr-only");
    //var btn_group1_button2_span2_text = document.createTextNode("Toggle Dropdown");
    //btn_group1_button2_span2.appendChild(btn_group1_button2_span2_text);

    var dropdown_ul = document.createElement('ul');
    tag_selection_btn_group.appendChild(dropdown_ul);
    dropdown_ul.setAttribute("class", "dropdown-menu");
    dropdown_ul.setAttribute("role", "menu");
    //dropdown_ul.id = "dropdown_menu";

    for (var i = 0; i < items.length; i++)
    {
        var dropdown_li_0 = document.createElement('li');
        dropdown_ul.appendChild(dropdown_li_0);

        var dropdown_li_0_href = document.createElement('a');
        dropdown_li_0.appendChild(dropdown_li_0_href);
        dropdown_li_0_href.setAttribute("href", "javascript:;");
        dropdown_li_0_href.setAttribute("onclick", "section_"+itemtype+"_select(0,"+ i +");return false;");
        dropdown_li_0_href.setAttribute("data-id", items[i].itemid);
        dropdown_li_0_href.id = "section_"+itemtype+"_0_item_"+i;

        if (itemtype == "nfclocator" && items[i].type == "nfcdevice")
        {
            var span_anchor = document.createElement("span");
            span_anchor.textContent = items[i].itemname + " (NFC)";
            span_anchor.setAttribute("class","badge bg-yellow");
            dropdown_li_0_href.appendChild(span_anchor);

            if (i == 0)
            {
                section_nfclocator_select(0,0);
                glCurrentNFCdevice = items[i].itemid;
            }
        }
    }
}

function section_nfclocator_select(eventidx, itemidx)
{
    var select_value = $("#section_nfclocator_"+eventidx+"_item_"+itemidx).html();
    var select_id = $("#section_nfclocator_"+eventidx+"_item_"+itemidx).attr("data-id");

    glCurrentNFCdevice = select_id;

    $("#section_nfclocator_"+eventidx+"_name").html(select_value);
    $("#section_nfclocator_"+eventidx+"_name").attr("data-id", select_id);

    show_area_user();
    if (glFirstRun)
    {
        glFirstRun = false;
    }
    else
    if (glAutoRunStarted)
    {
        glManualChanged = true;
    }
}

function load_maparea()
{
    //
    // Get Area first
    //
    var targetURL = gl_target_server + "/php/map.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loadLocationArea=1', function(data)
    {
        var index = 0;
        $.each(data, function(itemindex, area)
        {
            var area_json = JSON.parse(area);
            glAllAreas[index] = {};
            glAllAreas[index].itemid = area_json.properties.areaid;
            glAllAreas[index].itemname = area_json.properties.areaname;
            glAllAreas[index].itemtype = area_json.properties.areatype;
            index++;
        });

        // add default empty area
        glAllAreas[index] = {};
        glAllAreas[index].itemid = "Unknown";
        glAllAreas[index].itemname = "Unknown";
        glAllAreas[index].itemtype = 0;
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {

    });
}

function load_0dsection()
{
    gl0DSection = [];

    var targetURL = gl_target_server + "/php/sections.php";
    $.getJSON(targetURL, 'loadsections=1&project_id='+project_id, function(data)
    {
        var index = 0;
        $.each(data, function(sectionkey, value)
        {
            if ( value.SectionType == "0" )
            {
                gl0DSection[index] = {};
                gl0DSection[index].sectionkey = sectionkey;
                gl0DSection[index].nodeIDList = value.nodeIDList.split(',');
                gl0DSection[index].AreaID = value.AreaID;
                index++;
            }
        });
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    });
}

function show_area_user()
{
    // console.log("show_area_user() glManualChanged:"+glManualChanged+" glAutoRunStarted:"+glAutoRunStarted);

    if (glManualChanged && glAutoRunStarted)
    {
        glManualChanged = false;
        return;
    }
    var bFound = false;
    for (var i = 0; i < gl0DSection.length; i++)
    {
        for (var j = 0; j < gl0DSection[i].nodeIDList.length; j++)
        {
            if ( glCurrentNFCdevice === gl0DSection[i].nodeIDList[j] )
            {
                glCurrentArea = gl0DSection[i].AreaID;
                bFound = true;
                break;
            }
        }
        if (bFound)
            break;
    }
    // console.log("show_area_user() glCurrentNFCdevice:"+glCurrentNFCdevice+" glCurrentArea:"+glCurrentArea);

    if (bFound)
    {
        load_area_user_info();
        load_area_user_airremain();
        load_recent_record_express(glCurrentArea);
    }
    else
    {
        // clear all 123
        glAutoRunStarted = false;
        var loc_table = document.getElementById('NODE_LOCATION_TABLE_BODY');
        if (loc_table != undefined)
        {
            while (loc_table.rows.length >= 1)
                 loc_table.deleteRow(0);
        }

         var table_body = document.getElementById("record_table_rollcall_body");
         if (table_body != undefined)
         {
             while (table_body.rows.length >= 1)
              table_body.deleteRow(0);
          }
    }
}

function load_area_user_info()
{
    var loCurrentAreaUser = [];

    var targetURL = gl_target_server + "/php/fpnodes.php";

    var getQuery = $.getJSON(targetURL, 'loadenterarea=1&areaid='+glCurrentArea+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, value)
        {
            var newuser = [];
            newuser.NodeId = value.NodeId;
            newuser.entertime = value.entertime;
            var newuserentertime = moment(newuser.entertime, "YYYY-MM-DD HH:mm:ss");

            var bFound = false;
            for (var i = 0; i < loCurrentAreaUser.length; ++i)
            {
                var user = loCurrentAreaUser[i];

                // console.log("load_area_user_info() user.entertime:"+user.entertime +" newuser.entertime:"+newuser.entertime);
                var userentertime = moment(user.entertime, "YYYY-MM-DD HH:mm:ss");

                if ( userentertime.diff(newuserentertime) > 0 )
                {
                    // console.log("load_area_user_info() splice i:"+i);

                    loCurrentAreaUser.splice(i, 0, newuser);
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                loCurrentAreaUser.push(newuser);
            }
            // glAutoRunStarted = false;
            // console.log("load_area_user_info() glCurrentAreaUser.length:"+glCurrentAreaUser.length);


            // localCurrentAreaUser[index] = {};
            // localCurrentAreaUser[index].NodeId = value.NodeId;
            // localCurrentAreaUser[index].entertime = value.entertime;
            // index++;
        });
    })
    .success(function() {
        // console.log("load_area_user_info() success glCurrentArea:"+glCurrentArea);
    })
    .error(function() {
        // console.log("load_area_user_info() error glCurrentArea:"+glCurrentArea);
    })
    .complete(function() {
        // console.log("load_area_user_info() complete glCurrentArea:"+glCurrentArea);

        glCurrentAreaUser = loCurrentAreaUser;

        show_area_user_info();
        setTimeout(show_area_user, 1000);
        glAutoRunStarted = true;
    });
}

function load_area_user_airremain()
{
    var loCurrentAreaUserAirRemain = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";

    var getQuery = $.getJSON(targetURL, 'get_air_remain=1&areaid='+glCurrentArea+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, value)
        {
            var newRemain = [];
            newRemain.userid = value.userid;
            newRemain.airremain = value.airremain;

            loCurrentAreaUserAirRemain.push(newRemain);
        });
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
        glCurrentAreaUserAirRemain = loCurrentAreaUserAirRemain;
    });
}

function show_area_user_info()
{
    var loc_table = document.getElementById('NODE_LOCATION_TABLE_BODY');

    var areaName = getAreaName(glCurrentArea);

    while (loc_table.rows.length >= 1)
         loc_table.deleteRow(0);

     // console.log("show_area_user_info() glCurrentArea:"+glCurrentArea);

    for (var i = 0; i < glCurrentAreaUser.length; ++i)
    {
        var user = glCurrentAreaUser[i];

        var userName = getUserName(user.NodeId);
        var row = create_location_table_item(i+1, user.NodeId, areaName, userName, user.entertime);
        loc_table.appendChild(row);
    }
}

function create_location_table_item(ui_index, userid, areaName, userName, entertime)
{
    var row = document.createElement('tr');
    row.setAttribute("id","row_" + ui_index);
    row.setAttribute("style","background-color:rgb(250, 250, 250);");
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Area name
    var tdAreaName = document.createElement('td');
    tdAreaName.setAttribute("style","height: 90px;");
    row.appendChild(tdAreaName);
    var SpanAreaName = document.createElement("span");
    // SpanAreaName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanAreaName.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(54, 127, 169);");
    SpanAreaName.textContent = areaName;
    row.cells[cellIndex].appendChild(SpanAreaName);
    cellIndex++;

    // User name
    var tdUserName = document.createElement('td');
    tdUserName.setAttribute("style","height: 90px;");
    row.appendChild(tdUserName);
    var SpanUserName = document.createElement("span");
    // SpanUserName.setAttribute("class","badge bg-green-active");
    // SpanUserName.setAttribute("class","label label-success");
    SpanUserName.setAttribute("style","font-size:24px; font-weight:bold; color:#330033;");
    SpanUserName.textContent = userName;
    row.cells[cellIndex].appendChild(SpanUserName);
    cellIndex++;

    // time
    var tdEnterTime = document.createElement('td');
    tdEnterTime.setAttribute("style","height: 90px;");
    row.appendChild(tdEnterTime);
    var SpanEnterTime = document.createElement("span");
    SpanEnterTime.setAttribute("id","entertime_" + ui_index);
    SpanEnterTime.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    var difftime = getDiffTime(entertime);
    SpanEnterTime.textContent = convertPrettyTime(difftime);
    row.cells[cellIndex].appendChild(SpanEnterTime);
    cellIndex++;

    // air
    var tdAirRemain = document.createElement('td');
    tdAirRemain.setAttribute("style","height: 90px;");
    row.appendChild(tdAirRemain);
    var SpanAirRemain = document.createElement("span");
    SpanAirRemain.setAttribute("id","entertime_" + ui_index);
    SpanAirRemain.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanAirRemain.textContent = getAirRamin(userid, entertime);
    row.cells[cellIndex].appendChild(SpanAirRemain);

    var air_btn = document.createElement("div");
    row.cells[cellIndex].appendChild(air_btn);

    var nodata_btn = document.createElement("button");
    nodata_btn.setAttribute("type","button");
    nodata_btn.setAttribute("onclick","reset_air_remain('"+userid+"','"+userName+"');");
    nodata_btn.setAttribute("class","btn btn-sm");
    nodata_btn.textContent = '設定氣瓶量';
    air_btn.appendChild(nodata_btn);

    cellIndex++;

    // status
    var tdStatus = document.createElement('td');
    tdStatus.setAttribute("style","height: 90px;");
    row.appendChild(tdStatus);
    var SpanStatus = document.createElement("span");
    SpanStatus.setAttribute("id","status_" + ui_index);
    SpanStatus.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanStatus.textContent = '正常';
    row.cells[cellIndex].appendChild(SpanStatus);
    cellIndex++;

    if (checkExceedTime(difftime))
    {
        SpanStatus.textContent = '建議增援/撤離';
        row.setAttribute("style","background-color:rgb(225, 20, 20);");
    }
    else
    {
        SpanStatus.textContent = '正常';
        row.setAttribute("style","background-color:rgb(250, 250, 250);");
    }

    return row;
}

function upate_area_user_time()
{
    for (var i = 0; i < glCurrentAreaUser.length; ++i)
    {
        var user = glCurrentAreaUser[i];
        var ui_index = i + 1;
        var SpanEnterTime = document.getElementById("entertime_" + ui_index);
        var difftime = getDiffTime(user.entertime);
        SpanEnterTime.textContent = convertPrettyTime(difftime);

        var row = document.getElementById("row_" + ui_index);
        var SpanStatus = document.getElementById("status_" + ui_index);

        if (checkExceedTime(difftime))
        {
            SpanStatus.textContent = '建議增援/撤離';
            row.setAttribute("style","background-color:rgb(225, 20, 20);");
        }
        else
        {
            SpanStatus.textContent = '正常';
            row.setAttribute("style","background-color:rgb(250, 250, 250);");
        }
    }

    // setTimeout(upate_area_user_time, 1000);
}

function checkExceedTime(difftime)
{
    var cur_date = moment(difftime, "HH:mm:ss");
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var danger = false;
    if (hour > 0)
        danger = true;
    else
    if (minutes >= 20)
        danger = true;

    return danger;
}

function getDiffTime(user_entertime)
{
    var entertime = moment(user_entertime, "YYYY-MM-DD HH:mm:ss");
    // var now = moment().utcOffset("+08:00");
    var now = moment();
    // var difftime = moment().diff(entertime);
    var difftime = moment(now - entertime).utc();
    return difftime;
}

function convertToStayTime(user_entertime)
{
    var entertime = moment(user_entertime, "YYYY-MM-DD HH:mm:ss");
    // var now = moment().utcOffset("+08:00");
    var now = moment();
    // var difftime = moment().diff(entertime);
    var difftime = moment(now - entertime).utc();

    var staytime = convertPrettyTime(difftime);
    return staytime;
}

function convertPrettyTime(inDate)
{
    var cur_date = moment(inDate, "HH:mm:ss");
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var perty_str = "";
    if (hour > 0)
        perty_str += hour + "小時";
    if (minutes > 0)
        perty_str += minutes + "分";
    if (seconds > 0)
        perty_str += seconds + "秒";

    return perty_str;
}

function load_recent_record_express(areaId)
{
    var recentdays = 1;
    var targetStartDate = getAreaTargetDate(0);
    var targetEndDate = getAreaTargetDate(0);
    glCurrentArea_Records = [];

    // console.log("load_recent_record_express() areaId:"+areaId+" targetStartDate:" + targetStartDate);

    var targetURL = gl_target_server + "/php/dashboarddata.php";
    $.getJSON(targetURL, 'loadrecentrollcallrecord=1&areaid='+areaId+'&recentdays='+recentdays+'&startdate='+targetStartDate+'&project_id='+project_id, function(data)
    {
        // console.log("load_recent_record_express() data.length:"+data.length +", data[0].day:"+data[0].day);

        var EndDateNum = convertTimelineDateToNumber(targetEndDate);

        for (var index in data)
        {
            var dateData = data[index];
            // var issueday = dateData.issueday;

            // var record = {};
            // record.issueday = issueday;
            // record.data = dateData.data;
            //
            // var d1 = convertTimelineDateToNumber(issueday);
            //
            // if (d1 < EndDateNum)
            // {
            //     EndDateNum = d1;
            // }

            // console.log("create_course_rollcall_row() issueday:"+issueday+", record.data[0].nodename:"+record.data[0].nodename);

            glCurrentArea_Records.push(dateData.data);
        }

        // console.log("load_recent_record_express() length:"+glCurrentArea_Records.length+", targetStartDate:"+targetStartDate +", targetEndDate:"+targetEndDate);

        put_record_to_table(areaId, glCurrentArea_Records)
    });
}

function put_record_to_table(areaid, records)
{
    var pre_record = null;

    var footer_timeline = document.getElementById("timeline_footer");

    var table_ui = create_record_table_rollcall();
    footer_timeline.appendChild(table_ui);

    var rows = records.length;
    var table_height = (rows + 1) * 50 + 50;
    if (rows > 5)
        table_height = 300;

    // table_ui.setAttribute("style", "height:"+table_height+"px;overflow:scroll");

    var table_body_id = "record_table_rollcall_body";
    var table_body = document.getElementById(table_body_id);
    while (table_body.rows.length >= 1)
         table_body.deleteRow(0);


    var ui_index = 1;
    for(var index in records)
    {
        var recorddata = records[index];
        for(var dataindex in recorddata)
        {
            var record = recorddata[dataindex];
            var row = create_course_rollcall_row(ui_index, record);
            table_body.appendChild(row);

            ui_index++;
        }
    }
}

function create_course_rollcall_row(ui_index, record)
{
    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","table_row_"+ui_index);

    // console.log("create_course_rollcall_row() ui_index:"+ui_index+", nodename:"+record.nodename);


    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // name
    row.appendChild(document.createElement('td'));
    //row.cells[cellIndex].appendChild(document.createTextNode(record.nodename));
    var table_td_name_span = document.createElement('span');
    table_td_name_span.setAttribute("class",'badge bg-green');
    table_td_name_span.textContent = getUserName(record.nodename);
    row.cells[cellIndex].appendChild(table_td_name_span);
    cellIndex++;

    // Enter Time
    var table_td_entertime_td = document.createElement('td')
    row.appendChild(table_td_entertime_td);
    var table_td_entertime_span = document.createElement('span');
    //table_td_entertime_span.setAttribute("class",'badge bg-light-blue');
    table_td_entertime_span.setAttribute("class",'label label-warning');
    table_td_entertime_span.textContent = convertPrettyRollCallTime(record.min_time);
    table_td_entertime_td.appendChild(table_td_entertime_span);
    cellIndex++;

    // Leave Time
    var table_td_leaveltime_td = document.createElement('td')
    row.appendChild(table_td_leaveltime_td);
    var table_td_leaveltime_span = document.createElement('span');
    //table_td_leaveltime_span.setAttribute("class",'badge bg-purple');
    table_td_leaveltime_span.setAttribute("class",'label label-info');
    table_td_leaveltime_span.textContent = convertPrettyRollCallTime(record.max_time);
    table_td_leaveltime_td.appendChild(table_td_leaveltime_span);
    cellIndex++;

    //var stayTime = moment.utc(moment(record.DataTime,"YYYY-MM-DD HH:mm:ss").diff(moment(record.DataTime,"YYYY-MM-DD HH:mm:ss"))).format("HH:mm:ss")

    // stay Time
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(convertPrettyTime(convertFormatTime(record.totaltime))));
    cellIndex++;

    return row;
}

function create_record_table_rollcall()
{
    var divTabPane = document.getElementById("record_box_table_area");
    if (divTabPane != undefined)
        return divTabPane;

    var divTabPane = document.createElement('div');
    divTabPane.setAttribute("id","record_box_table_area");

    //
    // Box
    //
    var boxGroup = document.createElement('div');
    boxGroup.setAttribute("class","box");
    divTabPane.appendChild(boxGroup);

    //
    // box-body
    //
    var boxBody = document.createElement("div");
    boxBody.setAttribute("class","box-body no-padding");
    boxGroup.appendChild(boxBody);

    // table
    var tableGroup = document.createElement('table');
    tableGroup.setAttribute("class","table table-bordered");
    boxBody.appendChild(tableGroup);

    var theadGroup = document.createElement("thead");
    tableGroup.appendChild(theadGroup);

    var theadtrGroup = document.createElement("tr");
    theadtrGroup.setAttribute("bgcolor","#cce6ff");
    theadtrGroup.setAttribute("id","record_table_head");
    theadGroup.appendChild(theadtrGroup);

    var theadth = document.createElement("th");
    theadth.setAttribute("style","width: 10px");
    theadth.textContent = '#';
    theadtrGroup.appendChild(theadth);

    var theadthName = document.createElement("th");
    // theadthName.setAttribute("style","width: 150px");
    theadthName.textContent = '人員名稱';
    theadtrGroup.appendChild(theadthName);

    var theadth_EnterTime = document.createElement("th");
    theadth_EnterTime.setAttribute("style","text-align:center");
    theadth_EnterTime.textContent = '最早進入時間';
    theadtrGroup.appendChild(theadth_EnterTime);

    var theadth_LeaveTime = document.createElement("th");
    theadth_LeaveTime.setAttribute("style","text-align:center");
    theadth_LeaveTime.textContent = '最晚離開時間';
    theadtrGroup.appendChild(theadth_LeaveTime);

    var theadth_StayTime = document.createElement("th");
    //theadth_StayTime.setAttribute("style","width: 50px; text-align:center");
    theadth_StayTime.textContent = '停留總時間';
    theadtrGroup.appendChild(theadth_StayTime);

    var tbodyGroup = document.createElement("tbody");
    tbodyGroup.setAttribute("id","record_table_rollcall_body");
    tableGroup.appendChild(tbodyGroup);

    return divTabPane;
}

function reset_air_remain(userid, username)
{
    // console.log("reset_air_remain() userid:"+userid);
    var newValue = prompt('設定人員('+username+')氣瓶壓力(單位:bar)', '');

    if ( newValue != null)
    {
        // console.log("reset_air_remain() userid:"+userid +' username:'+username +' newValue:'+newValue);

        var jsondata = {};
        jsondata['project_id'] = ''+project_id;
        jsondata['reset_air_remain'] = 1;
        jsondata['userid'] = userid;
        jsondata['areaid'] = glCurrentArea;
        jsondata['remain'] = newValue;
        var targetURL = gl_target_server + "/php/vilsnodes.php";

        jQuery.ajax({
          url: targetURL,
          type: 'POST',
          data: jsondata,
          success:
            function(response)
            {
            },
          error:
            function(xhr, status, error)
            {
                alert("error = " + error);
                alert("xhr.responseText = " + xhr.responseText);
            }
        });
        return false;
    }
}

function getAirRamin(userid, entertime)
{
    var diffseconds = moment().diff(entertime, 'seconds');
    // var diffmins = moment().diff(entertime, 'minutes');

    // console.log("getAirRamin() userid:"+userid +" entertime:"+entertime+" diffseconds:"+diffseconds+" diffmins:"+diffmins);


    var remaintime = "";
    for (var i = 0; i < glCurrentAreaUserAirRemain.length; ++i)
    {
        var airremain = glCurrentAreaUserAirRemain[i];
        if ( airremain.userid === userid )
        {
            // 1 bar == 6L
            var alertBar = 60;// bar
            var consumerBar = 30;// L/min
            var initialBar = parseInt(airremain.airremain);
            var userAvailableAir = ((initialBar - alertBar) * 6);// to L
            var userConsumerAir = diffseconds * (consumerBar/60); // L/sec
            var userRemainAir = userAvailableAir - userConsumerAir;
            var userRemainTime = userRemainAir / (consumerBar/60);

            // console.log("getAirRamin() userid:"+userid +" userAvailableAir:"+userAvailableAir+" userConsumerAir:"+userConsumerAir+" userRemainTime:"+userRemainTime+" diffseconds:"+diffseconds);
            if (userRemainTime <= 0)
                remaintime = '無';
            else
                remaintime = convertPrettyTime(convertFormatTime(userRemainTime));
            break;
        }
    }
    return remaintime;
}

function show_maparea_info(areaid, areaname, count)
{

    // var showmsg = "區域:"+gl_rollcall_areaname+ "<br>應到:"+gl_all_tag.length+"<br>實到:"+gl_area_tag.length+"<br>未到:"+gl_none_tag.length;
    var showmsg = "區域:"+areaname+ "<br>實到:"+count;

    var popupID = "maparea_popup_"+areaid;
    var popupUI = document.getElementById(popupID);
    if ( popupUI == null )
    {
        console.log(popupID + " is null");
    }
    else
    {
        popupUI.innerHTML = showmsg;
    }

    // console.log(showmsg);
}
function runLive_sensor()
{
    if (gl_sensor_load_index == 0)
    {
        glSensor_List = [];
    }

    var sensor_num = 0;
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadsensordevice=1&s='+gl_sensor_load_index+'&count=50&project_id='+project_id, function(data)
    {
        $.each(data, function(index, device)
        {
            gl_sensor_load_index += 1;
            sensor_num += 1;
            glSensor_List.push(device);
        });
    })
    .success(function() {

        if (sensor_num < 50)
        {
            gl_sensor_load_index = 0;
            setTimeout(runLive_sensor, 5000);
        }
        else
        {
            setTimeout(runLive_sensor, 50);
        }
    })
    .error(function() {
        gl_sensor_load_index = 0;
        setTimeout(runLive_sensor, 300);
    })
    .complete(function() {
    });
}

function runLive_userActiviry()
{
    if (glUserActivity_List.length == 0)
    {
        collect_user_sensor();
    }

    if (glUserActivity_List.length == 0)
    {
        setTimeout(runLive_userActiviry, 5000);
        return;
    }

    update_user_area();

    setTimeout(runLive_userActiviry, 1000);
}

function collect_user_sensor()
{
    for	(var index = 0; index < glUser_List.length; index++)
    {
        var userData = glUser_List[index];
        if (userData.sensors != undefined)
        {
            // console.log("runLive_userActiviry() name:" + userData.name +" sensors:" + userData.sensors);

            var sensors = userData.sensors.split(",");
            for	(var j = 0; j < sensors.length; j++)
            {
                var sensorid = sensors[j];
                for	(var k = 0; k < glSensor_List.length; k++)
                {
                    var sensorData = glSensor_List[k];
                    if( sensorData.id === sensorid)
                    {
                        if( sensorData.type == 'ProMOS Watch')
                        {
                            console.log("collect_user_sensor() name:" + userData.name +" sensorid:" + sensorid);

                            var useractivity = {};
                            useractivity.userid = userData.userid;
                            useractivity.account = userData.account;
                            useractivity.username = userData.name;
                            useractivity.sensorid = sensorData.id;
                            if (userData.uwbcards != undefined)
                                useractivity.uwbcards = userData.uwbcards.split(",");
                            else
                                useractivity.uwbcards = [];
                            useractivity.activity = 'WALKING';

                            glUserActivity_List.push(useractivity);
                        }
                        break;
                    }
                }
            }
        }
    }
}

function update_user_area()
{
    var UserActivityDashBoard = document.getElementById("UserActivityDashBoard");
    var divRow = undefined;

    var showCount = 0;
    for	(var index = 0; index < glUserActivity_List.length; index++)
    {
        var userActivity = glUserActivity_List[index];
        for	(var j = 0; j < userActivity.uwbcards.length; j++)
        {
            var uwbcard = userActivity.uwbcards[j];
            for (var i = 0; i < glTag_List.length; ++i)
            {
                var nodeData = glTag_List[i];
                var macaddress = nodeData[6];
                var areaId = nodeData[19];
                var areaName = nodeData[20];
                var activity = nodeData[40];
                if( uwbcard == macaddress)
                {
                    // console.log("update_user_area() username:" + userActivity.username +" macaddress:" + macaddress +" areaName:" + areaName);
                    userActivity.areaId = areaId;
                    userActivity.areaName = areaName;
                    userActivity.activity = activity;
                    break;
                }
            }
        }
        if ( showCount == 0 )
        {
            var rowId = 'ActivityDashBoardRow_'+index;
            divRow = document.getElementById(rowId);
            if (divRow == undefined)
            {
                divRow = document.createElement("div");
                divRow.setAttribute("id", rowId);
                divRow.setAttribute("class","row");
                divRow.setAttribute("style","padding:10px;");
                UserActivityDashBoard.appendChild(divRow);
            }
        }

        //var colId = 'ActivityDashBoardCol_'+index+'_'+showCount;
        var colId = 'ActivityDashBoardCol_'+userActivity.userid;
        updateUserActivity(divRow, colId, userActivity);

        showCount++;
        if (showCount >= 2)
        {
            showCount = 0;
        }
    }
}

function updateUserActivity(divRow, colId, userActivity)
{
    var divCol = document.getElementById(colId);
    if (divCol == undefined)
    {
        divCol = document.createElement("div");
        divCol.setAttribute("id", colId);
        divCol.setAttribute("class","col-lg-6 col-xs-6");
        divRow.appendChild(divCol);
    }

    var boxId = colId+'_box';
    var box = document.getElementById(boxId);
    if (box == undefined)
    {
        box = document.createElement("div");
        box.setAttribute("id", boxId);
        divCol.appendChild(box);
    }
    //box.setAttribute("class","small-box bg-red");
    box.setAttribute("class","small-box bg-green");
    //box.setAttribute("class","small-box bg-yellow");

    // inner
    var innerId = colId+'_inner';
    var inner = document.getElementById(innerId);
    if (inner == undefined)
    {
        inner = document.createElement("div");
        inner.setAttribute("id", innerId);
        inner.setAttribute("class","inner");
        box.appendChild(inner);
    }

    var usernameId = colId+'_username';
    var username = document.getElementById(usernameId);
    if (username == undefined)
    {
        username = document.createElement("h4");
        username.setAttribute("id", usernameId);
        inner.appendChild(username);
    }
    username.textContent = userActivity.username;

    var bodyinfoId = colId+'_bodyinfo';
    var bodyinfo = document.getElementById(bodyinfoId);
    if (bodyinfo == undefined)
    {
        bodyinfo = document.createElement("p");
        bodyinfo.setAttribute("id", bodyinfoId);
        inner.appendChild(bodyinfo);
        bodyinfo.textContent = '心率: 血氧:';
    }

    var locationId = colId+'_location';
    var location = document.getElementById(locationId);
    if (location == undefined)
    {
        location = document.createElement("p");
        location.setAttribute("id", locationId);
        inner.appendChild(location);
    }
    location.textContent = '位置:' + userActivity.areaName;

    // icon
    var iconId = colId+'_icon';
    var icon = document.getElementById(iconId);
    if (icon == undefined)
    {
        icon = document.createElement("div");
        icon.setAttribute("id", iconId);
        icon.setAttribute("class","icon");
        box.appendChild(icon);
    }

    var imgId = colId+'_img';
    var img = document.getElementById(imgId);
    if (img == undefined)
    {
        img = document.createElement("img");
        img.setAttribute("id", imgId);
        img.setAttribute("style","height:50px;width:50px;opacity:0.5;");
        icon.appendChild(img);
    }

    var alert_type = 0;
    if (userActivity.activity == 'LAYING')
    {
        alert_type = 2;
        img.setAttribute("src","/images/man-fall.png");
        box.setAttribute("class","small-box bg-red");
    }
    else
    if (userActivity.activity.indexOf('WALKING') >= 0)
    {
        img.setAttribute("src","/images/man-walking.png");
    }
    else
    {
        img.setAttribute("src","/images/man-stand.png");
    }

    // footer
    var footerId = colId+'_footer';
    var footer = document.getElementById(footerId);
    if (footer == undefined)
    {
        footer = document.createElement("a");
        footer.setAttribute("id", footerId);
        footer.setAttribute("href","#");
        footer.setAttribute("class","small-box-footer");
        box.appendChild(footer);
    }

    var chartID = colId + '_chart';
    var chart = document.getElementById(chartID);
    if (chart == undefined)
    {
        chart = document.createElement("div");
        chart.setAttribute("id", chartID);
        chart.setAttribute("style","height: 180px; background-color:#445566;");
        footer.appendChild(chart);
    }

    var bodyinfoList = glUserBodyRecords[userActivity.account];
    if (bodyinfoList != undefined)
    {
        var dataitemlist = [];
        var heartbeatinfoList = bodyinfoList.heartbeat;
        var bloodoxygeninfoList = bodyinfoList.bloodoxygen;

        var heartbeat_new = 90;
        var bloodoxygen_new = 99;

        for(var index = heartbeatinfoList.length - 1; index>=0; index--)
        {
            var bloodoxygeninfo = bloodoxygeninfoList[index];
            var heartbeatinfo = heartbeatinfoList[index];
            var bloodoxygen_value = parseFloat(bloodoxygeninfo.data);
            var heartbeat_value = parseFloat(heartbeatinfo.data);

            var info = [];
            info['y'] = bloodoxygeninfo.datetime;
            info['bloodoxygen'] = bloodoxygen_value.toFixed(0);
            info['heartbeat'] = heartbeat_value.toFixed(0);
            dataitemlist.push(info);

            if (index == heartbeatinfoList.length - 1)
            {
                heartbeat_new = info['heartbeat'];
                bloodoxygen_new = info['bloodoxygen'];
                bodyinfo.textContent = '心率:'+ info['heartbeat'] +' 血氧:'+ info['bloodoxygen'];
            }
        }

        if (alert_type != 2)
            alert_type = check_user_alert_type(heartbeat_new, bloodoxygen_new);
        if (alert_type == 2)
        {
            box.setAttribute("class","small-box bg-red");
        }
        else
        if (alert_type == 1)
        {
            box.setAttribute("class","small-box bg-yellow");
        }

        if (dataitemlist.length > 0)
        {
            var area = glActivityCharts[chartID];
            if (area == undefined)
            {
                // console.log("updateUserActivity() userActivity.account:"+userActivity.account+" area == undefined");

                area = new Morris.Area({
                  element: chartID,
                  resize: true,
                  redraw: true,
                  // data: dataitemlist,
                  data: [
                    {y: '', bloodoxygen: 80, heartbeat: 80}],
                  // data: [
                  //   {y: '2021-05-23 12:43:20', bloodoxygen: 99, heartbeat: 130},
                  //   {y: '2021-05-23 12:43:21', bloodoxygen: 99, heartbeat: 120},
                  //   {y: '2021-05-23 12:43:22', bloodoxygen: 98, heartbeat: 100},
                  //   {y: '2021-05-23 12:43:23', bloodoxygen: 97, heartbeat: 90},
                  //   {y: '2021-05-23 12:43:24', bloodoxygen: 95, heartbeat: 100},
                  //   {y: '2021-05-23 12:43:25', bloodoxygen: 90, heartbeat: 110},
                  //   {y: '2021-05-23 12:43:26', bloodoxygen: 88, heartbeat: 130},
                  //   {y: '2021-05-23 12:43:27', bloodoxygen: 85, heartbeat: 140},
                  //   {y: '2021-05-23 12:43:28', bloodoxygen: 80, heartbeat: 150},
                  //   {y: '2021-05-23 12:43:29', bloodoxygen: 85, heartbeat: 160}
                  // ],
                  xkey: 'y',
                  ykeys: ['heartbeat', 'bloodoxygen'],
                  ymin: 80,
                  ymax: 200,
                  behaveLikeLine: true,
                  fillOpacity: 0.6,
                  labels: ['心率', '血氧'],
                  pointFillColors:['#ffffff'],
                  pointStrokeColors: ['#558dbc'],
                  lineColors: ['#3c8dbc', '#aa3344'],
                  hideHover: 'auto'
                });
                glActivityCharts[chartID] = area;
            }

            // console.log("updateUserActivity() userActivity.account:"+userActivity.account+" chartID:" + chartID +" dataitemlist.length:" + dataitemlist.length);
            // $("#"+chartID).empty();
            area.setData(dataitemlist);
            // area.redraw();

        }
    }

    return divCol;
}

function runLive_bodyinfo_records()
{
    var loStartDate = moment().subtract(1, 'days');
    var loEndDate = moment();

    var StartDate = loStartDate.format("YYYY-MM-DD HH:mm:ss");
    var EndDate = loEndDate.format("YYYY-MM-DD HH:mm:ss");

    if ( gl_bodyinfo_load_index >= glUserActivity_List.length )
    {
        gl_bodyinfo_load_index = 0;
        setTimeout(runLive_bodyinfo_records, 5000);
        return;
    }

    var userActivity = glUserActivity_List[gl_bodyinfo_load_index];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loaduserbodybaseinforecords=1'+'&accountid='+userActivity.account+'&StartDate='+StartDate+'&EndDate='+EndDate, function(data)
    {
        var bodyinfoList = {};
        var account = userActivity.account;

        $.each(data, function(itemindex, bodyinfo)
        {
            bodyinfoList[itemindex] = bodyinfo;
        });
        glUserBodyRecords[account] = bodyinfoList;
    })
    .success(function() {
    });


    gl_bodyinfo_load_index++;

    setTimeout(runLive_bodyinfo_records, 100);
}

function check_user_alert_type(heartbeat, bloodoxygen)
{
    var alert_type = 0;// 0: normal, 1: warning 2: danger
    var SPO2_WARNING_MIN = 90;
    var SPO2_WARNING_MAX = 95;
    var SPO2_DANGER_MIN = 1;
    var SPO2_DANGER_MAX = 89;
    var HR_WARNING_MIN = 120;
    var HR_WARNING_MAX = 180;
    var HR_DANGER_MIN = 181;
    var HR_DANGER_MAX = 254;

    if ( heartbeat >= HR_WARNING_MIN && heartbeat <= HR_WARNING_MAX )
    {
        alert_type = 1;
    }
    else
    if ( heartbeat >= HR_DANGER_MIN && heartbeat <= HR_DANGER_MAX )
    {
        alert_type = 2;
    }

    if ( alert_type >= 2 )
        return alert_type;

    if ( bloodoxygen >= SPO2_WARNING_MIN && bloodoxygen <= SPO2_WARNING_MAX )
    {
        if (alert_type <= 1)
        {
            alert_type = 1;
        }
    }
    else
    if ( bloodoxygen >= SPO2_DANGER_MIN && bloodoxygen <= SPO2_DANGER_MAX )
    {
        alert_type = 2;
    }

    return alert_type;
}

function convertTimelineDateToNumber(inDate)
{
    var cur_date = moment(inDate, "YYYY-MM-DD");
    var outDate = cur_date.format('YYYYMMDD');

    return Number(outDate);
}

function convertPrettyRollCallTime(inTime)
{
    var cur_date = moment(inTime, "HH:mm");
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var perty_str = "";
    if (hour >= 0)
        perty_str += hour + "點";
    if (minutes >= 0)
        perty_str += minutes + "分";

    return perty_str;
}

function convertFormatTime(seconds)
{
    const formatted = moment.utc(seconds*1000).format('HH:mm:ss');
    return formatted;
}

function getAreaTargetDate(addDays)
{
    var targetDate = moment().add(addDays, 'd').format('YYYY-MM-DD');
    return targetDate;
}

function getAreaName(areaid)
{
    var areaName = areaid;
    for (var i = 0; i < glAllAreas.length; ++i)
    {
        var area = glAllAreas[i];
        if( area.itemid === areaid )
        {
            return area.itemname;
        }
    }
    return areaName;
}

function getUserName(userid)
{
    var userName = userid;
    for (var i = 0; i < glAllUsers.length; ++i)
    {
        var user = glAllUsers[i];
        if( user.userid === userid )
        {
            return user.name;
        }
    }
    return userName;
}

function open_fieldcontrol_page()
{
    var win = window.open('/index.php/vilsfieldcontrol/'+project_id, '_blank');
    win.focus();
}
