/*
 * Author: ikki Chung
 * Date: 10 Aug 2021
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var glLiveMap;
var glFireFightingList = [];
var glCurrentFireFightingCaseIndex = 0;

var glNFCdevices = [];
var glAoAdevices = [];

var glNFCdeviceSelected = {};
// var glAoAdeviceSelected = {};
var glStandbydeviceSelected = {};
var glAreaUser = {};
var glUserAirUsing = {};
var glCaseUsers = {};
var glAllUsers = [];

var glUserBodyRecords = {};
var gl_bodyinfo_load_index = 0;

var glResetUserAreaTable = false;

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

                create_map();

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_user_info();

            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cases();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_NFCdevices();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_AoAdevices();
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                runLive_user_area();
                runLive_bodyinfo_records();
                runLive_field_user();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_websocket_firefighting_event();

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

function create_map()
{
    glLiveMap = new Indoor2DMap("mapid");

    glLiveMap.setMapCoeff(0, 0, 10240, 10240, 5, 100000, 121.51, 25.04, 0.0);
    glLiveMap.setMapImage("/images/empty.png", "Indoor 2D Map");
    glLiveMap.setMapLayerInfo(1);
    // glLiveMap.addOriginMarker(0, 0, 0);

    glLiveMap.showGoolgeMap();
    glLiveMap.showOriginLayer();

}

function load_cases()
{
    glFireFightingList = [];

    var targetURL = gl_target_server + "/php/firefighting.php";
    $.getJSON(targetURL, 'loadfirefightingcases=1&closed=0'+'&project_id='+project_id , function(data)
    {
        $.each(data, function(index, casedata)
        {
            glFireFightingList.push(casedata);
            glNFCdeviceSelected[casedata.casenumber] = [];
            // glAoAdeviceSelected[casedata.casenumber] = [];
            glStandbydeviceSelected[casedata.casenumber] = [];
            glAreaUser[casedata.casenumber] = [];

            updateNFCdeviceSelected(casedata.casenumber, casedata.NFCDeviceIds);
            // updateAoAdeviceSelected(casedata.casenumber, casedata.AoADeviceIds);
            updateStandbydeviceSelected(casedata.casenumber, casedata.StandbyDeviceIds);
        });

    })
    .complete(function() {
        show_addresses();
        load_cases_users();
    });
}

function load_cases_users()
{
    var cases = [];

    for	(var index = 0; index < glFireFightingList.length; index++)
    {
        var casedata = glFireFightingList[index];
        cases.push(casedata.casenumber);
    }

    var targetURL = gl_target_server + "/php/firefighting.php";
    $.getJSON(targetURL, 'loadfirefightingusers=1&casenumber='+ cases.toString() +'&project_id='+project_id , function(data)
    {
        $.each(data, function(index, userdata)
        {
            var caseusers = glAreaUser[userdata.casenumber];
            caseusers.push(userdata);
        });

    })
    .complete(function() {

        for	(var index = 0; index < glFireFightingList.length; index++)
        {
            var casedata = glFireFightingList[index];
            var caseusers = glAreaUser[casedata.casenumber];
            caseusers = caseusers.sort(function(a, b){return (b.enterTime < a.enterTime)});
            glAreaUser[casedata.casenumber] = caseusers;
        }

        show_user_area();
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    });
}

function getUserAirUsing(userid)
{
    var targetURL = gl_target_server + "/php/firefighting.php";
    $.getJSON(targetURL, 'loaduserairusing=1&userid='+ userid +'&project_id='+project_id , function(data)
    {
        glUserAirUsing[userid] = parseFloat(data.airusing);
    })
    .success(function() {
    });
}

function load_NFCdevices()
{
    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadnfclocator=1&s=0&count=100&project_id='+project_id+'&showonline=1&showoffline=1', function(data)
    {
        var nfclocator_size = 0;

        $.each(data, function(index, device)
        {
            glNFCdevices.push(device);

            nfclocator_size += 1;
        });
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
        show_nfcdevice_to_UI();
        show_nfcdeviceSelected_to_UI();
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    });
}

function load_AoAdevices()
{
    glAoAdevices = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadaoa=1&project_id='+project_id, function(data)
    {
        var aoa_num = parseInt(data['LOC_AOA_NUM']);

        for (var i=0; i<aoa_num; ++i)
        {
            var key = 'LOC_AOA_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            var node = data[key];
            var device = node.split(",");
            var nodeid = device[0];
            var name = device[1];
            var posX = device[5];
            var posY = device[6];
            var posZ = device[7];

            var aoaData = {};
            aoaData.id = nodeid;
            aoaData.name = name;
            aoaData.posX = posX;
            aoaData.posY = posY;
            aoaData.posZ = posZ;
            aoaData.type = "aoadevice";

            glAoAdevices.push(aoaData);
        }
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
        show_standbydevice_to_UI();
        show_standbydeviceSelected_to_UI();
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
            glAllUsers.push(user);
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

function runLive_user_area()
{
    if (glResetUserAreaTable == true)
    {
        show_user_area();
        glResetUserAreaTable = false;
    }

    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];
    var caseusers = glAreaUser[casedata.casenumber];

    for	(var index = 0; index < caseusers.length; index++)
    {
        var userdata = caseusers[index];

        var SpanEnterTime = document.getElementById("fieldentertime_" + userdata.userid);
        if (SpanEnterTime != undefined)
        {
            var difftime = getDiffTime(userdata.enterTime);
            SpanEnterTime.textContent = convertPrettyTime(difftime);

            var row = document.getElementById("fieldrow_" + userdata.userid);
            var SpanStatus = document.getElementById("status_" + userdata.userid);

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

        var airRemain = getAirRamin(userdata.userid, userdata.enterTime, userdata.airremain);
        var SpanAirRemain = document.getElementById("fieldairremain_" + userdata.userid);
        if (SpanAirRemain != undefined)
        {
            SpanAirRemain.textContent = airRemain;

            if (airRemain === '無')
            {
                var row = document.getElementById("fieldrow_" + userdata.userid);
                row.setAttribute("style","background-color:rgb(225, 20, 20);");

                var SpanStatus = document.getElementById("status_" + userdata.userid);
                SpanStatus.textContent = '建議增援/撤離';
            }
        }

        var Spanactivityfield = document.getElementById("fieldactivity_" + userdata.userid);
        if (Spanactivityfield != undefined)
        {
            Spanactivityfield.textContent = userdata.activity;
        }

        var SpanSPO2field = document.getElementById("fieldSPO2_" + userdata.userid);
        if (SpanSPO2field != undefined)
        {
            SpanSPO2field .textContent = userdata.SPO2;
        }

        var SpanHRfield = document.getElementById("fieldHR_" + userdata.userid);
        if (SpanHRfield != undefined)
        {
            SpanHRfield.textContent = userdata.HR;
        }

        var SpanUserAirUsingfield = document.getElementById("fielduserairusing_" + userdata.userid);
        if (SpanUserAirUsingfield != undefined)
        {
            var airusing = glUserAirUsing[userdata.userid];
            if (airusing != undefined)
            {
                SpanUserAirUsingfield.setAttribute("title","每分鐘消耗bar數:"+airusing);
                SpanUserAirUsingfield.textContent = " " + airusing;
            }
        }


        var SpanAirRemainstandby = document.getElementById("standbyairremain_" + userdata.userid);
        if (SpanAirRemainstandby != undefined)
        {
            SpanAirRemainstandby.textContent = userdata.airremain;
        }

        var Spanactivitystandby = document.getElementById("standbyactivity_" + userdata.userid);
        if (Spanactivitystandby != undefined)
        {
            Spanactivitystandby.textContent = userdata.activity;
        }

        var SpanSPO2standby = document.getElementById("standbySPO2_" + userdata.userid);
        if (SpanSPO2standby != undefined)
        {
            SpanSPO2standby.textContent = userdata.SPO2;
        }

        var SpanHRstandby = document.getElementById("standbyHR_" + userdata.userid);
        if (SpanHRstandby != undefined)
        {
            SpanHRstandby.textContent = userdata.HR;
        }

        var SpanUserAirUsingstandby = document.getElementById("standbyuserairusing_" + userdata.userid);
        if (SpanUserAirUsingstandby != undefined)
        {
            var airusing = glUserAirUsing[userdata.userid];
            if (airusing != undefined)
            {
                SpanUserAirUsingstandby.setAttribute("title","每分鐘消耗bar數:"+airusing);
                SpanUserAirUsingstandby.textContent = " " + airusing;
            }
        }

    }

    setTimeout(runLive_user_area, 1000);
}

function runLive_bodyinfo_records()
{
    var loStartDate = moment().subtract(1, 'days');
    var loEndDate = moment();

    var StartDate = loStartDate.format("YYYY-MM-DD HH:mm:ss");
    var EndDate = loEndDate.format("YYYY-MM-DD HH:mm:ss");

    if ( gl_bodyinfo_load_index >= glAllUsers.length )
    {
        gl_bodyinfo_load_index = 0;
        setTimeout(runLive_bodyinfo_records, 5000);
        return;
    }

    var userinfo = glAllUsers[gl_bodyinfo_load_index];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loaduserbodybaseinforecords=1'+'&accountid='+userinfo.userid+'&StartDate='+StartDate+'&EndDate='+EndDate, function(data)
    {
        var bodyinfoList = {};
        var userid = userinfo.userid;

        $.each(data, function(itemindex, bodyinfo)
        {
            bodyinfoList[itemindex] = bodyinfo;
        });
        glUserBodyRecords[userid] = bodyinfoList;
    })
    .success(function() {
    });

    gl_bodyinfo_load_index++;

    setTimeout(runLive_bodyinfo_records, 100);
}

function runLive_field_user()
{
    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];

    var caseUsers = {};
    glCaseUsers[casedata.casenumber] = [];

    // areaType == '0' 火場
    // areaType == '1' 待命熱區

    var targetURL = gl_target_server + "/php/firefighting.php";
    // $.getJSON(targetURL, 'loadfirefightingusers=1&casenumber='+ casedata.casenumber +'&areaType=0&project_id='+project_id , function(data)
    $.getJSON(targetURL, 'loadfirefightingevent=1&casenumber='+ casedata.casenumber +'&eventtype=FireFightingUser&project_id='+project_id , function(data)
    {
        $.each(data, function(index, eventdata)
        {
            var userdata = eventdata.json;
            if (caseUsers[userdata.userid] == undefined)
            {
                // console.log("runLive_field_user() caseUsers[userdata.userid] == undefined userid:" + userdata.userid);
                caseUsers[userdata.userid] = [];
            }
            caseUsers[userdata.userid].push(userdata);
        });
    })
    .complete(function() {
        glCaseUsers[casedata.casenumber] = caseUsers;
        update_field_user(caseUsers);
    });

    setTimeout(runLive_field_user, 2000);
}

function update_field_user(caseUsers)
{
    var userRecord = {};

    for (var key in caseUsers)
    {
        if (caseUsers.hasOwnProperty(key))
        {
            var userName = getUserName(key);
            userRecord[key] = {};

            // console.log(key + " -> " + userName);

            var enterTime = '';
            var leaveTime = '';
            var preEnterTime = '';
            var preAreaType = '';
            var totalStayTime = 0;

            var caseUser = caseUsers[key];
            for	(var index = 0; index < caseUser.length; index++)
            {
                var userdata = caseUser[index];

                if (userdata.areaType == '0')
                {
                    if (enterTime.length == 0)
                    {
                        enterTime = userdata.enterTime;
                    }
                    preEnterTime = userdata.enterTime;
                }
                else
                {
                    if ( preAreaType == '0')
                    {
                        leaveTime = userdata.enterTime;
                        // calculate time
                        var difftime = getEnterLeaveDiffTime(preEnterTime, leaveTime);
                        totalStayTime += difftime;
                        // console.log("difftime:" + difftime + " difftime:" + convertPrettyTime(difftime) + " totalStayTime:" + totalStayTime);
                    }
                }

                preAreaType = userdata.areaType;
            }
            const formatted = moment.utc(totalStayTime).format('HH:mm:ss');
            // console.log(key + " -> " + userName +" enterTime:" + enterTime +" leaveTime:" + leaveTime +" totalStayTime:" + convertPrettyTime(formatted));

            userRecord[key].userName = userName;
            userRecord[key].enterTime = enterTime;
            userRecord[key].leaveTime = leaveTime;
            userRecord[key].totalTime = convertPrettyTime(formatted);
        }
    }

    var field_users = document.getElementById("field_users");
    while (field_users.hasChildNodes())
    {
        field_users.removeChild(field_users.firstChild);
    }

    var table_ui = create_field_user_table();
    field_users.appendChild(table_ui);

    var record_table_field_user_body = document.getElementById("record_table_field_user_body");

    var ui_index = 1;
    for (var key in userRecord)
    {
        var record = userRecord[key];

        var row = create_field_user_row(ui_index, record);

        record_table_field_user_body.appendChild(row);

        ui_index += 1;
    }
}

function create_field_user_table()
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
    theadth_StayTime.textContent = '停留總時間';
    theadtrGroup.appendChild(theadth_StayTime);

    var tbodyGroup = document.createElement("tbody");
    tbodyGroup.setAttribute("id","record_table_field_user_body");
    tableGroup.appendChild(tbodyGroup);

    return divTabPane;
}

function create_field_user_row(ui_index, record)
{
    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // name
    row.appendChild(document.createElement('td'));
    var table_td_name_span = document.createElement('span');
    table_td_name_span.setAttribute("class",'badge bg-green');
    table_td_name_span.textContent = record.userName;
    row.cells[cellIndex].appendChild(table_td_name_span);
    cellIndex++;

    // Enter Time
    var table_td_entertime_td = document.createElement('td')
    row.appendChild(table_td_entertime_td);
    var table_td_entertime_span = document.createElement('span');
    table_td_entertime_span.setAttribute("class",'label label-warning');
    table_td_entertime_span.textContent = record.enterTime;
    table_td_entertime_td.appendChild(table_td_entertime_span);
    cellIndex++;

    // Leave Time
    var table_td_leaveltime_td = document.createElement('td')
    row.appendChild(table_td_leaveltime_td);
    var table_td_leaveltime_span = document.createElement('span');
    table_td_leaveltime_span.setAttribute("class",'label label-info');
    table_td_leaveltime_span.textContent = record.leaveTime;
    table_td_leaveltime_td.appendChild(table_td_leaveltime_span);
    cellIndex++;

    // stay Time
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(record.totalTime));
    cellIndex++;

    return row;
}

function update_device_info()
{
    var targetURL = gl_target_server + "/php/firefighting.php";

    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];


    var NFCdeviceSelected = glNFCdeviceSelected[casedata.casenumber];
    // var AoAdeviceSelected = glAoAdeviceSelected[casedata.casenumber];
    var StandbydeviceSelected = glStandbydeviceSelected[casedata.casenumber];

    var jsondata = {};
    jsondata['project_id'] = project_id;
    jsondata['updatedeviceinfo'] = 1;
    jsondata['casenumber'] = casedata.casenumber;
    jsondata['NFCDeviceIds'] = NFCdeviceSelected.toString();
    // jsondata['AoADeviceIds'] = AoAdeviceSelected.toString();
    jsondata['StandbyDeviceIds'] = StandbydeviceSelected.toString();

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

function update_user_airremain(casenumber, userid, newValue)
{
    var targetURL = gl_target_server + "/php/firefighting.php";

    var jsondata = {};
    jsondata['project_id'] = project_id;
    jsondata['updateuserairremain'] = 1;
    jsondata['casenumber'] = casenumber;
    jsondata['userid'] = userid;
    jsondata['airremain'] = newValue;

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

function update_user_area(casenumber, userid, toType)
{
    var targetURL = gl_target_server + "/php/firefighting.php";

    var jsondata = {};
    jsondata['project_id'] = project_id;
    jsondata['updateuserarea'] = 1;
    jsondata['casenumber'] = casenumber;
    jsondata['userid'] = userid;
    jsondata['areaType'] = toType;

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

function finish_case(calcAirUsing)
{
    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];

    var targetURL = gl_target_server + "/php/firefighting.php";

    var jsondata = {};
    jsondata['project_id'] = project_id;
    jsondata['finishcase'] = 1;
    jsondata['casenumber'] = casedata.casenumber;
    jsondata['calcairusing'] = calcAirUsing;

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
            PreLoadIndex = 1;
            CurrentLoadIndex = 2;
            setTimeout(check_loading_status, 100);
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

function reset_air_remain(userid, username)
{
    // console.log("reset_air_remain() userid:"+userid);
    var newValue = prompt('設定人員('+username+')氣瓶壓力(單位:bar)', '');

    if ( newValue != null)
    {
        console.log("reset_air_remain() userid:"+userid +' username:'+username +' newValue:'+newValue);
        var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];
        var caseusers = glAreaUser[casedata.casenumber];

        for	(var index = 0; index < caseusers.length; index++)
        {
            var userdata = caseusers[index];
            if ( userdata.userid == userid )
            {
                userdata.airremain = newValue;

                update_user_airremain(casedata.casenumber, userid, newValue);

                break;
            }
        }
    }
}

function confirm_change_user_area(userid, username, fromType, toType)
{
    var areaName= '待命熱區';
    if ( toType == '0' )
    {
        areaName= '管制區';
    }
    else
    if ( toType == '1' )
    {
        areaName= '待命熱區';
    }

    if (confirm('更改使用者('+username+')區域, 移至'+areaName+'?'))
    {
        change_user_area(userid, toType);
    }
    else
    {
        // Do nothing!
    }
}

function change_user_area(userid, toType)
{
    console.log("change_user_area() userid:"+userid +' toType:'+toType);
    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];
    var caseusers = glAreaUser[casedata.casenumber];

    var found = false;
    for	(var index = 0; index < caseusers.length; index++)
    {
        var userdata = caseusers[index];
        if ( userdata.userid == userid )
        {
            update_user_area(casedata.casenumber, userid, toType);
            found = true;
            break;
        }
    }

    if(found == false)
    {
        for	(var index = 0; index < glAllUsers.length; index++)
        {
            var userdata = glAllUsers[index];
            if ( userdata.userid == userid )
            {
                update_user_area(casedata.casenumber, userid, toType);
                found = true;
                break;
            }
        }
    }

    //
    // sort by enterTime
    //
    caseusers = caseusers.sort(function(a, b){return (b.enterTime < a.enterTime)});
    glAreaUser[casedata.casenumber] = caseusers;
}

function show_nfcdevice_to_UI()
{
    var nfcdevice_dropdown_menu = document.getElementById('nfcdevice_dropdown_menu');
    while (nfcdevice_dropdown_menu.hasChildNodes())
    {
        nfcdevice_dropdown_menu.removeChild(nfcdevice_dropdown_menu.firstChild);
    }

    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];
    var NFCdeviceSelected = glNFCdeviceSelected[casedata.casenumber];

    if (casedata.caseclosed == 1)
    {
        return;
    }

    // select all button
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_selectall(\"device_nfclocator\");return true;");
    item_li.appendChild(item_click);
    item_click.appendChild(document.createTextNode("全選"));
    nfcdevice_dropdown_menu.appendChild(item_li);

    for (var i = 0; i < glNFCdevices.length; ++i)
    {
        var nfcdevice = glNFCdevices[i];
        var existed = false;

        for (var j = 0; j < NFCdeviceSelected.length; ++j)
        {
            var selectedid = NFCdeviceSelected[j];
            if ( selectedid == nfcdevice.deviceid )
            {
                existed = true;
                break;
            }
        }

        var item = create_dropdown_device_item("nfclocator", nfcdevice.deviceid, parseInt(nfcdevice.macaddress), nfcdevice.name, existed);
        nfcdevice_dropdown_menu.appendChild(item);
    }
}

function show_standbydevice_to_UI()
{
    var standbydevice_dropdown_menu = document.getElementById('standbydevice_dropdown_menu');
    while (standbydevice_dropdown_menu.hasChildNodes())
    {
        standbydevice_dropdown_menu.removeChild(standbydevice_dropdown_menu.firstChild);
    }

    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];
    // var AoAdeviceSelected = glAoAdeviceSelected[casedata.casenumber];
    var StandbydeviceSelected = glStandbydeviceSelected[casedata.casenumber];

    if (casedata.caseclosed == 1)
    {
        return;
    }

    // select all button
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_selectall(\"device_standbydevice\");return true;");
    item_li.appendChild(item_click);
    item_click.appendChild(document.createTextNode("全選"));
    standbydevice_dropdown_menu.appendChild(item_li);

    for (var i = 0; i < glAoAdevices.length; ++i)
    {
        var aoadevice = glAoAdevices[i];
        var existed = false;

        for (var j = 0; j < StandbydeviceSelected.length; ++j)
        {
            var selectedid = StandbydeviceSelected[j];
            if ( selectedid === aoadevice.id )
            {
                existed = true;
                break;
            }
        }

        var item = create_dropdown_device_item("standbydevice", aoadevice.id, aoadevice.id, aoadevice.name, existed);
        standbydevice_dropdown_menu.appendChild(item);
    }

    for (var i = 0; i < glNFCdevices.length; ++i)
    {
        var nfcdevice = glNFCdevices[i];
        var existed = false;

        for (var j = 0; j < StandbydeviceSelected.length; ++j)
        {
            var selectedid = StandbydeviceSelected[j];
            if ( selectedid === nfcdevice.deviceid )
            {
                existed = true;
                break;
            }
        }

        var item = create_dropdown_device_item("standbydevice", nfcdevice.deviceid, parseInt(nfcdevice.macaddress), nfcdevice.name, existed);
        standbydevice_dropdown_menu.appendChild(item);
    }
}

function show_nfcdeviceSelected_to_UI()
{
    var nfclocator_selection = document.getElementById('nfclocator_selection');
    while (nfclocator_selection.hasChildNodes())
    {
        nfclocator_selection.removeChild(nfclocator_selection.firstChild);
    }

    // console.log("show_nfcdeviceSelected_to_UI() glNFCdeviceSelected.length:"+glNFCdeviceSelected.length);
    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];
    var NFCdeviceSelected = glNFCdeviceSelected[casedata.casenumber];

    for (var i = 0; i < NFCdeviceSelected.length; ++i)
    {
        var nfcdeviceid = NFCdeviceSelected[i];

        for (var j = 0; j < glNFCdevices.length; ++j)
        {
            var nfcdevice = glNFCdevices[j];
            if ( nfcdeviceid == nfcdevice.deviceid )
            {
                var span_NFCdevice = document.createElement("span");
                span_NFCdevice.textContent = nfcdevice.name;
                span_NFCdevice.setAttribute("class","badge bg-yellow");
                nfclocator_selection.appendChild(span_NFCdevice);

                // console.log("show_nfcdeviceSelected_to_UI() nfcdeviceName:"+nfcdeviceName);
                break;
            }
        }

    }
}

function show_standbydeviceSelected_to_UI()
{
    var standbydevice_selection = document.getElementById('standbydevice_selection');
    while (standbydevice_selection.hasChildNodes())
    {
        standbydevice_selection.removeChild(standbydevice_selection.firstChild);
    }

    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];
    var StandbydeviceSelected = glStandbydeviceSelected[casedata.casenumber];


    // console.log("show_standbydeviceSelected_to_UI() AoAdeviceSelected.length:"+AoAdeviceSelected.length);

    for (var i = 0; i < StandbydeviceSelected.length; ++i)
    {
        var selectedid = StandbydeviceSelected[i];

        // console.log("show_standbydeviceSelected_to_UI() selectedid:"+selectedid);

        var bFound = false;
        for (var j = 0; j < glAoAdevices.length; ++j)
        {
            var aoadevice = glAoAdevices[j];
            if ( selectedid == aoadevice.id )
            {
                var span_AoAdevice = document.createElement("span");
                span_AoAdevice.textContent = aoadevice.name;
                span_AoAdevice.setAttribute("class","badge bg-green");
                standbydevice_selection.appendChild(span_AoAdevice);

                bFound = true;
                // console.log("show_standbydeviceSelected_to_UI() aoadevice.name:"+aoadevice.name);
                break;
            }
        }

        if (bFound)
            continue;

        for (var j = 0; j < glNFCdevices.length; ++j)
        {
            var nfcdevice = glNFCdevices[j];
            if ( selectedid == nfcdevice.deviceid )
            {
                var span_NFCdevice = document.createElement("span");
                span_NFCdevice.textContent = nfcdevice.name;
                span_NFCdevice.setAttribute("class","badge bg-yellow");
                standbydevice_selection.appendChild(span_NFCdevice);

                // console.log("show_nfcdeviceSelected_to_UI() nfcdeviceName:"+nfcdeviceName);
                break;
            }
        }
    }
}

function dropdown_selectall(prefix)
{
    // console.log("dropdown_selectall() prefix:"+prefix);

    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];

    if( prefix === 'device_standbydevice')
    {
        for (var i = 0; i < glAoAdevices.length; ++i)
        {
            var aoadevice = glAoAdevices[i];

            var ele = document.getElementById("dropdown_checkbox_"+prefix+"_"+aoadevice.id);

            if ( ele == undefined )
            {
                // console.log("dropdown_selectall() i:"+i+" ele == undefined ");
                break;
            }
            ele.checked = true;
            putToStandbydeviceSelected(casedata.casenumber, aoadevice.id);
            show_standbydeviceSelected_to_UI();
        }

        for (var i = 0; i < glNFCdevices.length; ++i)
        {
            var nfcdevice = glNFCdevices[i];

            var ele = document.getElementById("dropdown_checkbox_"+prefix+"_"+nfcdevice.deviceid);

            if ( ele == undefined )
            {
                // console.log("dropdown_selectall() i:"+i+" ele == undefined ");
                break;
            }
            ele.checked = true;
            putToStandbydeviceSelected(casedata.casenumber, nfcdevice.deviceid);
            show_standbydeviceSelected_to_UI();
        }
    }
    else
    if( prefix === 'device_nfclocator')
    {
        for (var i = 0; i < glNFCdevices.length; ++i)
        {
            var nfcdevice = glNFCdevices[i];

            var ele = document.getElementById("dropdown_checkbox_"+prefix+"_"+nfcdevice.deviceid);

            if ( ele == undefined )
            {
                // console.log("dropdown_selectall() i:"+i+" ele == undefined ");
                break;
            }
            ele.checked = true;
            putToNFCdeviceSelected(casedata.casenumber, nfcdevice.deviceid);
            show_nfcdeviceSelected_to_UI();
        }
    }

    update_device_info();
}

function dropdown_checkbox_device_click(index)
{
    glDropdown_checkbox_device_click = true;
}

var glDropdown_checkbox_device_click = false;
function dropdown_device_click(prefix, deviceid)
{
    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];

    if (glDropdown_checkbox_device_click == true)
    {
        // console.log("dropdown_device_click() glDropdown_checkbox_device_click == true");
        if (document.getElementById("dropdown_checkbox_device_"+prefix+"_"+deviceid).checked == true)
        {
            putToDeviceSelected(casedata.casenumber, prefix, deviceid);
        }
        else
        {
            removeFromDeviceSelected(casedata.casenumber, prefix, deviceid);
        }
    }
    else
    {
        if (document.getElementById("dropdown_checkbox_device_"+prefix+"_"+deviceid).checked == true)
        {
            document.getElementById("dropdown_checkbox_device_"+prefix+"_"+deviceid).checked = false;
            removeFromDeviceSelected(casedata.casenumber, prefix, deviceid);
        }
        else
        {
            document.getElementById("dropdown_checkbox_device_"+prefix+"_"+deviceid).checked = true;
            putToDeviceSelected(casedata.casenumber, prefix, deviceid);
        }
    }
    show_DeviceSelected_to_UI(prefix);
    update_device_info();
    glDropdown_checkbox_device_click = false;
}

function create_dropdown_device_item(prefix, deviceid, datavalue, dataname, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", datavalue);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_device_click(\""+prefix+"\",\""+ deviceid +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_device_"+prefix+"_"+deviceid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_device_click();return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+dataname));
    item_li.appendChild(item_click);

    return item_li;
}

function show_DeviceSelected_to_UI(prefix)
{
    if (prefix === "nfclocator")
    {
        show_nfcdeviceSelected_to_UI();
    }
    else
    if (prefix === "standbydevice")
    {
        show_standbydeviceSelected_to_UI();
    }
}

function putToDeviceSelected(casenumber, prefix, deviceid)
{
    if (prefix === "nfclocator")
    {
        putToNFCdeviceSelected(casenumber, deviceid);
    }
    else
    if (prefix === "standbydevice")
    {
        putToStandbydeviceSelected(casenumber, deviceid);
    }
}

function removeFromDeviceSelected(casenumber, prefix, deviceid)
{
    if (prefix === "nfclocator")
    {
        removeFromNFCdeviceSelected(casenumber, deviceid);
    }
    else
    if (prefix === "standbydevice")
    {
        removeFromStandbydeviceSelected(casenumber, deviceid);
    }
}

function updateNFCdeviceSelected(casenumber, nfcDeviceIds)
{
    var NFCDeviceIds = nfcDeviceIds.split(",");

    // console.log("load_cases() casenumber:"+casedata.casenumber);
    // console.log("load_cases() casedata.NFCDeviceIds:"+casedata.NFCDeviceIds);
    // console.log("load_cases() casedata.AoADeviceIds:"+casedata.AoADeviceIds);
    // console.log("load_cases() NFCDeviceIds.length:"+NFCDeviceIds.length);
    // console.log("load_cases() AoADeviceIds.length:"+AoADeviceIds.length);

    for (var j = 0; j < NFCDeviceIds.length; ++j)
    {
        var nfcdeviceid = NFCDeviceIds[j];
        if ( nfcdeviceid.length > 0 )
        {
            // console.log("load_cases() casenumber:"+casedata.casenumber+" j:"+j+" nfcdeviceid:"+nfcdeviceid);
            putToNFCdeviceSelected(casenumber, parseInt(nfcdeviceid));
        }
    }
}

function updateStandbydeviceSelected(casenumber, standbyDeviceIds)
{
    var StandbyDeviceIds = standbyDeviceIds.split(",");

    for (var j = 0; j < StandbyDeviceIds.length; ++j)
    {
        var standbydeviceid = StandbyDeviceIds[j];
        if ( standbydeviceid.length > 0 )
        {
            // console.log("load_cases() casenumber:"+casedata.casenumber+" j:"+j+" aoadeviceid:"+aoadeviceid);
            putToStandbydeviceSelected(casenumber, standbydeviceid);
        }
    }
}

function putToNFCdeviceSelected(casenumber, deviceid)
{
    var NFCdeviceSelected = glNFCdeviceSelected[casenumber];

    var bFound = false;
    for (var i = 0; i < NFCdeviceSelected.length; ++i)
    {
        var nfcdeviceid = NFCdeviceSelected[i];
        if ( nfcdeviceid == deviceid )
        {
            bFound = true;
            break;
        }
    }
    if ( bFound == false )
    {
        // console.log("putToNFCdeviceSelected() deviceid:"+deviceid);
        NFCdeviceSelected.push(deviceid);
    }

    glNFCdeviceSelected[casenumber] = NFCdeviceSelected;
}

function removeFromNFCdeviceSelected(casenumber, deviceid)
{
    // console.log("removeFromNFCdeviceSelected() deviceid:"+deviceid);
    var NFCdeviceSelected = glNFCdeviceSelected[casenumber];

    for (var i = 0; i < NFCdeviceSelected.length; ++i)
    {
        var nfcdeviceid = NFCdeviceSelected[i];

        // console.log("removeFromNFCdeviceSelected() i:"+i+" nfcdeviceid:"+nfcdeviceid);

        if ( nfcdeviceid == deviceid )
        {
            NFCdeviceSelected.splice(i, 1);
            break;
        }
    }

    glNFCdeviceSelected[casenumber] = NFCdeviceSelected;
}

function putToStandbydeviceSelected(casenumber, deviceid)
{
    // var AoAdeviceSelected = glAoAdeviceSelected[casenumber];
    var StandbydeviceSelected = glStandbydeviceSelected[casenumber];

    var bFound = false;
    for (var i = 0; i < StandbydeviceSelected.length; ++i)
    {
        var standbydeviceid = StandbydeviceSelected[i];
        if ( standbydeviceid == deviceid )
        {
            bFound = true;
            break;
        }
    }
    if ( bFound == false )
    {
        StandbydeviceSelected.push(deviceid);
    }
    // glAoAdeviceSelected[casenumber] = AoAdeviceSelected;
    glStandbydeviceSelected[casenumber] = StandbydeviceSelected;
}

function removeFromStandbydeviceSelected(casenumber, deviceid)
{
    // console.log("removeFromNFCdeviceSelected() deviceid:"+deviceid);
    // var AoAdeviceSelected = glAoAdeviceSelected[casenumber];
    var StandbydeviceSelected = glStandbydeviceSelected[casenumber];

    for (var i = 0; i < StandbydeviceSelected.length; ++i)
    {
        var standbydeviceid = StandbydeviceSelected[i];

        // console.log("removeFromNFCdeviceSelected() i:"+i+" nfcdeviceid:"+nfcdeviceid);

        if ( standbydeviceid == deviceid )
        {
            StandbydeviceSelected.splice(i, 1);
            break;
        }
    }
    // glAoAdeviceSelected[casenumber] = AoAdeviceSelected;
    glStandbydeviceSelected[casenumber] = StandbydeviceSelected;
}

function show_addresses()
{
    document.getElementById("Address_longitude").innerHTML = "經度:";
    document.getElementById("Address_latitude").innerHTML = "緯度:";

    glCurrentFireFightingCaseIndex = 0;
    glLiveMap.clearAddressTarget();

    var dropdown_addresses_ul = document.getElementById("dropdown_addresses");
    while (dropdown_addresses_ul.hasChildNodes())
    {
        dropdown_addresses_ul.removeChild(dropdown_addresses_ul.firstChild);
    }

    for	(var index = 0; index < glFireFightingList.length; index++)
    {
        var casedata = glFireFightingList[index];

        var defaultchecked = false;
        if (index == 0 )
        {
            defaultchecked = true;
            document.getElementById("cur_address").innerHTML = casedata.address + '<span class="caret"></span>';
            document.getElementById("Address_longitude").innerHTML = "經度:" + casedata.lng;
            document.getElementById("Address_latitude").innerHTML = "緯度:" + casedata.lat;
            document.getElementById("record_starttime").innerHTML = '案號:' + casedata.casenumber + ' 記錄開始時間:' + casedata.starttime;

            var closecasebutton = document.getElementById("closecasebutton");
            if (casedata.caseclosed == 0)
            {
                closecasebutton.setAttribute("style","display:block;");
            }
            else
            {
                closecasebutton.setAttribute("style","display:none;");
            }

            glCurrentFireFightingCaseIndex = index;
            focus_map(casedata);

        }
        var item_li = create_dropdown_address_item(index, casedata.address, defaultchecked);
        dropdown_addresses_ul.appendChild(item_li);
    }
}

function create_dropdown_address_item(addressindex, address, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", addressindex);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_address_click(\""+ addressindex +"\");return true;");

    item_click.appendChild(document.createTextNode(" "+address));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_address_click(addressindex)
{
    var casedata = glFireFightingList[addressindex];
    glCurrentFireFightingCaseIndex = addressindex;

    focus_map(casedata);

    document.getElementById("cur_address").innerHTML = casedata.address + '<span class="caret"></span>';
    document.getElementById("Address_longitude").innerHTML = "經度:" + casedata.lng;
    document.getElementById("Address_latitude").innerHTML = "緯度:" + casedata.lat;
    document.getElementById("record_starttime").innerHTML = '案號:' + casedata.casenumber + ' 記錄開始時間:' + casedata.starttime;

    var closecasebutton = document.getElementById("closecasebutton");
    if (casedata.caseclosed == 0)
    {
        closecasebutton.setAttribute("style","display:block;");
    }
    else
    {
        closecasebutton.setAttribute("style","display:none;");
    }

    show_user_area();
    show_nfcdevice_to_UI();
    // show_aoadevice_to_UI();
    show_nfcdeviceSelected_to_UI();
    show_standbydeviceSelected_to_UI();
}

function show_user_area()
{
    var field_table = document.getElementById("FIRE_AREA_TABLE_BODY");
    while (field_table.hasChildNodes())
    {
        field_table.removeChild(field_table.firstChild);
    }
    var standby_table = document.getElementById("STANDBY_AREA_TABLE_BODY");
    while (standby_table.hasChildNodes())
    {
        standby_table.removeChild(standby_table.firstChild);
    }
    var standby2_table = document.getElementById("STANDBY2_AREA_TABLE_BODY");
    while (standby2_table.hasChildNodes())
    {
        standby2_table.removeChild(standby2_table.firstChild);
    }
    var standby3_table = document.getElementById("STANDBY3_AREA_TABLE_BODY");
    while (standby3_table.hasChildNodes())
    {
        standby3_table.removeChild(standby3_table.firstChild);
    }

    var casedata = glFireFightingList[glCurrentFireFightingCaseIndex];
    var caseusers = glAreaUser[casedata.casenumber];

    // console.log("show_user_area() casedata.casenumber:"+casedata.casenumber+" caseusers.length:"+caseusers.length);

    var field_count = 0;
    var standby_count = 0;
    var standby2_count = 0;

    for	(var index = 0; index < caseusers.length; index++)
    {
        var userdata = caseusers[index];

        // console.log("show_user_area() userdata.areaType:"+userdata.areaType +" userdata.userid:" + userdata.userid);

        var areaType = parseInt(userdata.areaType);

        if (areaType == 0)
        {
            // AREA_FIELD
            field_count++;
            var row = create_field_table_item(field_count, userdata);
            field_table.appendChild(row);
        }
        else
        if (areaType == 1)
        {
            // AREA_STANDBY1
            standby_count++;
            var row = create_standby_table_item(standby_count, userdata);
            standby_table.appendChild(row);
        }
        else
        if (areaType == 2)
        {
            // AREA_STANDBY2
            standby2_count++;
            var row = create_standby_table_item(standby2_count, userdata);
            standby2_table.appendChild(row);
        }
    }

    var standby3_count = 0;
    for	(var index = 0; index < glAllUsers.length; index++)
    {
        var userdata = glAllUsers[index];
        var userdata_userid = parseInt(userdata.userid);

        var found = false;
        for	(var j = 0; j < caseusers.length; j++)
        {
            var caseuserdata = caseusers[j];
            if (userdata_userid == parseInt(caseuserdata.userid) )
            {
                found = true;
                break;
            }
        }

        if (found == false)
        {
            // AREA_STANDBY3
            standby3_count++;
            var row = create_standby_table_item(standby3_count, userdata);
            standby3_table.appendChild(row);
        }
    }

    var infield_count = document.getElementById("infield_count");
    infield_count.textContent = field_count;

    var standby1_count_ui = document.getElementById("standby1_count");
    standby1_count_ui.textContent = standby_count;
    var standby2_count_ui = document.getElementById("standby2_count");
    standby2_count_ui.textContent = standby2_count;
    var standby3_count_ui = document.getElementById("standby3_count");
    standby3_count_ui.textContent = standby3_count;
}

function create_field_table_item(ui_index, userdata)
{
    var userName = getUserName(userdata.userid);

    var row = document.createElement('tr');
    row.setAttribute("id","fieldrow_" + userdata.userid);
    row.setAttribute("style","background-color:rgb(250, 250, 250);");
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
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

    var changearea_btn = document.createElement("div");
    row.cells[cellIndex].appendChild(changearea_btn);

    var nodata_btn = document.createElement("button");
    nodata_btn.setAttribute("type","button");
    nodata_btn.setAttribute("onclick","confirm_change_user_area('"+userdata.userid+"','"+userName+"','"+userdata.areaType+"','1');");
    nodata_btn.setAttribute("class","btn btn-sm");
    nodata_btn.textContent = '更改區域';
    changearea_btn.appendChild(nodata_btn);

    cellIndex++;

    // 心跳
    row.appendChild(document.createElement('td'));
    var SpanHR = document.createElement("span");
    SpanHR.setAttribute("id","fieldHR_" + userdata.userid);
    SpanHR.setAttribute("style","font-size:14px; font-weight:bold; color:#225533;");
    SpanHR.textContent = userdata.HR;
    row.cells[cellIndex].appendChild(SpanHR);
    cellIndex++;

    // 血氧
    row.appendChild(document.createElement('td'));
    var SpanSPO2 = document.createElement("span");
    SpanSPO2.setAttribute("id","fieldSPO2_" + userdata.userid);
    SpanSPO2.setAttribute("style","font-size:14px; font-weight:bold; color:#225533;");
    SpanSPO2.textContent = userdata.SPO2;
    row.cells[cellIndex].appendChild(SpanSPO2);
    cellIndex++;

    // 氣瓶壓力
    var tdAirRemain = document.createElement('td');
    tdAirRemain.setAttribute("style","height: 90px;");
    row.appendChild(tdAirRemain);
    var SpanAirRemain = document.createElement("span");
    SpanAirRemain.setAttribute("id","fieldairremain_" + userdata.userid);
    SpanAirRemain.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanAirRemain.textContent = getAirRamin(userdata.userid, userdata.enterTime, userdata.airremain);
    row.cells[cellIndex].appendChild(SpanAirRemain);

    var SpanUserAirUsing = document.createElement("span");
    SpanUserAirUsing.setAttribute("id","fielduserairusing_" + userdata.userid);
    SpanUserAirUsing.setAttribute("title","每分鐘消耗bar數:");
    SpanUserAirUsing.setAttribute("style","font-size:10px; font-weight:light; color:#337733;");
    // SpanUserAirUsing.textContent = 0;
    row.cells[cellIndex].appendChild(SpanUserAirUsing);

    var air_btn = document.createElement("div");
    row.cells[cellIndex].appendChild(air_btn);

    var nodata_btn = document.createElement("button");
    nodata_btn.setAttribute("type","button");
    nodata_btn.setAttribute("onclick","reset_air_remain('"+userdata.userid+"','"+userName+"');");
    nodata_btn.setAttribute("class","btn btn-sm");
    nodata_btn.textContent = '設定氣瓶量';
    air_btn.appendChild(nodata_btn);

    cellIndex++;

    // time
    var tdEnterTime = document.createElement('td');
    tdEnterTime.setAttribute("style","height: 90px;");
    row.appendChild(tdEnterTime);
    var SpanEnterTime = document.createElement("span");
    SpanEnterTime.setAttribute("id","fieldentertime_" + userdata.userid);
    SpanEnterTime.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    var difftime = getDiffTime(userdata.enterTime);
    SpanEnterTime.textContent = convertPrettyTime(difftime);
    row.cells[cellIndex].appendChild(SpanEnterTime);
    cellIndex++;

    // 姿態
    var tdPose = document.createElement('td');
    tdPose.setAttribute("style","height: 90px;");
    row.appendChild(tdPose);
    var SpanPose  = document.createElement("span");
    SpanPose.setAttribute("id","fieldactivity_" + userdata.userid);
    SpanPose.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanPose.textContent = userdata.activity;
    row.cells[cellIndex].appendChild(SpanPose);
    cellIndex++;

    // 狀態
    var tdStatus = document.createElement('td');
    tdStatus.setAttribute("style","height: 90px;");
    row.appendChild(tdStatus);
    var SpanStatus = document.createElement("span");
    SpanStatus.setAttribute("id","status_" + userdata.userid);
    SpanStatus.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanStatus.textContent = '正常';
    row.cells[cellIndex].appendChild(SpanStatus);
    cellIndex++;

    // if (checkExceedTime(difftime))
    // {
    //     SpanStatus.textContent = '建議增援/撤離';
    //     row.setAttribute("style","background-color:rgb(225, 20, 20);");
    // }
    // else
    // {
    //     SpanStatus.textContent = '正常';
    //     row.setAttribute("style","background-color:rgb(250, 250, 250);");
    // }

    return row;
}

function create_standby_table_item(ui_index, userdata)
{
    var userName = getUserName(userdata.userid);

    var row = document.createElement('tr');
    row.setAttribute("id","standbyrow_" + userdata.userid);
    row.setAttribute("style","background-color:rgb(250, 250, 250);");
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
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

    var changearea_btn = document.createElement("div");
    row.cells[cellIndex].appendChild(changearea_btn);

    var nodata_btn = document.createElement("button");
    nodata_btn.setAttribute("type","button");
    nodata_btn.setAttribute("onclick","confirm_change_user_area('"+userdata.userid+"','"+userName+"','"+userdata.areaType+"','0');");
    nodata_btn.setAttribute("class","btn btn-sm");
    nodata_btn.textContent = '更改區域';
    changearea_btn.appendChild(nodata_btn);

    cellIndex++;

    // 心跳
    row.appendChild(document.createElement('td'));
    var SpanHR = document.createElement("span");
    SpanHR.setAttribute("id","standbyHR_" + userdata.userid);
    SpanHR.setAttribute("style","font-size:14px; font-weight:bold; color:#225533;");
    SpanHR.textContent = userdata.HR;
    row.cells[cellIndex].appendChild(SpanHR);
    cellIndex++;

    // 血氧
    row.appendChild(document.createElement('td'));
    var SpanSPO2 = document.createElement("span");
    SpanSPO2.setAttribute("id","standbySPO2_" + userdata.userid);
    SpanSPO2.setAttribute("style","font-size:14px; font-weight:bold; color:#225533;");
    SpanSPO2.textContent = userdata.SPO2;
    row.cells[cellIndex].appendChild(SpanSPO2);
    cellIndex++;

    // 氣瓶壓力
    var tdAirRemain = document.createElement('td');
    tdAirRemain.setAttribute("style","height: 90px;");
    row.appendChild(tdAirRemain);
    var SpanAirRemain = document.createElement("span");
    SpanAirRemain.setAttribute("id","standbyairremain_" + userdata.userid);
    SpanAirRemain.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanAirRemain.textContent = userdata.airremain;
    row.cells[cellIndex].appendChild(SpanAirRemain);

    var SpanUserAirUsing = document.createElement("span");
    SpanUserAirUsing.setAttribute("id","standbyuserairusing_" + userdata.userid);
    SpanUserAirUsing.setAttribute("title","每分鐘消耗bar數:");
    SpanUserAirUsing.setAttribute("style","font-size:10px; font-weight:light; color:#337733;");
    // SpanUserAirUsing.textContent = 0;
    row.cells[cellIndex].appendChild(SpanUserAirUsing);

    var air_btn = document.createElement("div");
    row.cells[cellIndex].appendChild(air_btn);

    var nodata_btn = document.createElement("button");
    nodata_btn.setAttribute("type","button");
    nodata_btn.setAttribute("onclick","reset_air_remain('"+userdata.userid+"','"+userName+"');");
    nodata_btn.setAttribute("class","btn btn-sm");
    nodata_btn.textContent = '設定氣瓶量';
    air_btn.appendChild(nodata_btn);

    cellIndex++;

    // 姿態
    var tdPose = document.createElement('td');
    tdPose.setAttribute("style","height: 90px;");
    row.appendChild(tdPose);
    var SpanPose  = document.createElement("span");
    SpanPose.setAttribute("id","standbyactivity_" + userdata.userid);
    SpanPose.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanPose.textContent = userdata.activity;
    row.cells[cellIndex].appendChild(SpanPose);
    cellIndex++;

    // // time
    // var tdEnterTime = document.createElement('td');
    // tdEnterTime.setAttribute("style","height: 90px;");
    // row.appendChild(tdEnterTime);
    // var SpanEnterTime = document.createElement("span");
    // SpanEnterTime.setAttribute("id","entertime_" + ui_index);
    // SpanEnterTime.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    // var difftime = getDiffTime(userdata.enterTime);
    // SpanEnterTime.textContent = convertPrettyTime(difftime);
    // row.cells[cellIndex].appendChild(SpanEnterTime);
    // cellIndex++;

    return row;
}

function focus_map(casedata)
{
    glLiveMap.setBounds(parseFloat(casedata.lat) - 0.001, parseFloat(casedata.lng) - 0.001, parseFloat(casedata.lat) + 0.001, parseFloat(casedata.lng) + 0.001);

    glLiveMap.setTargetLocation(casedata.address, casedata.lat, casedata.lng);
}
//========================================================
// websocket
function load_websocket_firefighting_event()
{
    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsystemconfig=1'+'&project_id='+project_id, function(data)
    {
        glWebSocketPort = data.web_socket_port;
        load_FireFightingEvent_websocket();
    });
}

function load_FireFightingEvent_websocket()
{
    // var wsUri = gl_websocket_server +":"+glWebSocketPort+"/websocket/xystudy_"+project_id+"_2D1531973179180j8teuD5A4A_type|firefightingAlert";
    var wsUri = gl_websocket_server +":"+glWebSocketPort+"/websocket/maingroups_"+gl_maingroups+"_2D1531973179180j8teuD5A4A_type|firefightingevent";

    console.log("load_FireFightingEvent_websocket() wsUri:"+wsUri);

    websocket = new WebSocket(wsUri);
    websocket.onmessage = function(evt) { onMessageFireFightingEvent(evt) };
    websocket.onclose = function(evt) { onCloseFireFightingEvent(evt) };
    websocket.onopen = function(evt) { onOpenFireFightingEvent(evt) };
}

function onMessageFireFightingEvent(evt)
{
    var casedata = JSON.parse(evt.data);

    // console.log("onMessageFireFightingEvent() casedata:"+casedata);
    // console.log("onMessageFireFightingEvent() casedata.msgType:"+casedata.msgType);

    if (casedata.msgType === 'FireFightingInfo' )
    {
        // console.log("onMessageFireFightingEvent() casedata.id:"+casedata.id);
        // console.log("onMessageFireFightingEvent() casedata.maingroupid:"+casedata.maingroupid);
        // console.log("onMessageFireFightingEvent() casedata.caseclosed:"+casedata.caseclosed);
        console.log("onMessageFireFightingEvent() casedata.casenumber:"+casedata.casenumber);
        // console.log("onMessageFireFightingEvent() casedata.address:"+casedata.address);
        // console.log("onMessageFireFightingEvent() casedata.starttime:"+casedata.starttime);

        for (var i in glFireFightingList)
        {
            if (glFireFightingList[i].casenumber == casedata.casenumber)
            {
                glFireFightingList[i] = casedata;
            }
        }

        glNFCdeviceSelected[casedata.casenumber] = [];
        // glAoAdeviceSelected[casedata.casenumber] = [];
        glStandbydeviceSelected[casedata.casenumber] = [];

        updateNFCdeviceSelected(casedata.casenumber, casedata.NFCDeviceIds);
        // updateAoAdeviceSelected(casedata.casenumber, casedata.AoADeviceIds);
        updateStandbydeviceSelected(casedata.casenumber, casedata.StandbyDeviceIds);

        dropdown_address_click(glCurrentFireFightingCaseIndex);
    }
    else
    if (casedata.msgType === 'FireFightingUser' )
    {
        console.log("onMessageFireFightingEvent() casedata.userid:"+casedata.userid);
        console.log("onMessageFireFightingEvent() casedata.areaType:"+casedata.areaType);
        console.log("onMessageFireFightingEvent() casedata.casenumber:"+casedata.casenumber);
        console.log("onMessageFireFightingEvent() casedata.airremain:"+casedata.airremain);
        console.log("onMessageFireFightingEvent() casedata.enterTime:"+casedata.enterTime);
        console.log("onMessageFireFightingEvent() casedata.activity:"+casedata.activity);
        console.log("onMessageFireFightingEvent() casedata.HR:"+casedata.HR);
        console.log("onMessageFireFightingEvent() casedata.SPO2:"+casedata.SPO2);

        var curAreaUser = glAreaUser[casedata.casenumber];
        var found = false;
        var count = 0;
        for (var i in curAreaUser)
        {
            var user = curAreaUser[i];
            if (user.userid == casedata.userid)
            {

                if (curAreaUser[i].areaType != casedata.areaType)
                {
                    // reset table
                    glResetUserAreaTable = true;
                }

                curAreaUser[i] = casedata;
                found = true;

                console.log("onMessageFireFightingEvent() existed casedata.userid:"+casedata.userid);

                break;
            }
            count++;
        }
        if (found == false)
        {
            curAreaUser.push(casedata);
            // reset table
            glResetUserAreaTable = true;
            console.log("onMessageFireFightingEvent() new casedata.userid:"+casedata.userid);
        }

        //
        // sort by enterTime
        //
        var curAreaUserSorted = curAreaUser.sort(function(a, b){return (b.enterTime < a.enterTime)});

        glAreaUser[casedata.casenumber] = curAreaUserSorted;
    }
}

function onOpenFireFightingEvent(evt)
{
    console.log("onOpenFireFightingEvent() Connected to VILS Center!! ");

    var new_html = '<a><i class="fa fa-chain text-green"></i>';
    new_html += ' 系統已連線</a>';

    $('#connection_status').html(new_html);
}

function onCloseFireFightingEvent(evt)
{
    console.log("onCloseFireFightingEvent() reconnect to VILS Center!! ");

    var new_html = '<a><i class="fa fa-chain-broken text-red"></i>';
    new_html += ' 系統連線中...</a>';

    $('#connection_status').html(new_html);

    // reconnect
    setTimeout(load_FireFightingEvent_websocket, 2000);
}
//========================================================


function closeNewFireFightingbtn_click()
{
    $('#modal-askcloseaction').modal('show');
}

function continue_case_click()
{
    $('#modal-askcloseaction').modal('hide');
}

function end_case_and_save_airrecord_click()
{
    $('#modal-askcloseaction').modal('hide');
    finish_case(1);
}

function end_case_click()
{
    $('#modal-askcloseaction').modal('hide');
    finish_case(0);
}

function getUserName(userid)
{
    // console.log("getUserName() glAllUsers.length:"+glAllUsers.length);
    var int_userid = parseInt(userid);
    var userName = userid;
    for (var i = 0; i < glAllUsers.length; ++i)
    {
        var user = glAllUsers[i];

        // console.log("getUserName() userid:"+userid +" user.userid:"+user.userid);

        if( parseInt(user.userid) == userid )
        {
            return user.name;
        }
    }
    return userName;
}

function getAirRamin(userid, entertime, airremain)
{
    var diffseconds = moment().diff(entertime, 'seconds');
    // var diffmins = moment().diff(entertime, 'minutes');

    // console.log("getAirRamin() userid:"+userid +" entertime:"+entertime +" airremain:"+airremain+" diffseconds:"+diffseconds);

    var curUserAirUsingBar = 30;// L/min
    var curUserAirUsing = glUserAirUsing[userid];
    if ( curUserAirUsing == undefined)
    {
        setTimeout(getUserAirUsing, 10, userid);

        curUserAirUsingBar = 30;
    }
    else
    {
        curUserAirUsingBar = curUserAirUsing * 6.0;
    }

    var remaintime = "";

    // 1 bar == 6L
    var alertBar = 60;// bar
    var consumerBar = curUserAirUsingBar;// L/min
    var initialBar = parseInt(airremain);
    var userAvailableAir = ((initialBar - alertBar) * 6);// to L
    var userConsumerAir = diffseconds * (consumerBar/60); // L/sec
    var userRemainAir = userAvailableAir - userConsumerAir;
    var userRemainTime = userRemainAir / (consumerBar/60);

    // console.log("getAirRamin() userid:"+userid +" userAvailableAir:"+userAvailableAir+" userConsumerAir:"+userConsumerAir+" userRemainTime:"+userRemainTime+" diffseconds:"+diffseconds);
    if (userRemainTime <= 0)
        remaintime = '無';
    else
        remaintime = convertPrettyTime(convertFormatTime(userRemainTime));

    return remaintime;
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
    // console.log("getDiffTime() user_entertime:"+user_entertime);

    var entertime = moment(user_entertime, "YYYY-MM-DD HH:mm:ss");
    var now = moment().utcOffset("+08:00");

    // console.log("getDiffTime() now:"+now);

    // var now = moment();
    // var difftime = moment().diff(entertime);
    var difftime = moment(now - entertime).utc();

    // console.log("getDiffTime() user_entertime:"+user_entertime +" difftime:"+difftime);

    return difftime;
}

function getEnterLeaveDiffTime(user_entertime, user_leavetime)
{
    var entertime = moment(user_entertime, "YYYY-MM-DD HH:mm:ss");
    var leavetime = moment(user_leavetime, "YYYY-MM-DD HH:mm:ss");

    var difftime = moment(leavetime - entertime).utc();
    // console.log("getDiffTime() user_entertime:"+user_entertime +" difftime:"+difftime);
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

function convertFormatTime(seconds)
{
    const formatted = moment.utc(seconds*1000).format('HH:mm:ss');
    return formatted;
}

function convertPrettyTime(inDate)
{
    var cur_date = moment(inDate, "HH:mm:ss");
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    // console.log("convertPrettyTime() hour:"+hour +" minutes:"+minutes +" seconds:"+seconds);

    var perty_str = "";
    if (hour > 0)
        perty_str += hour + "小時";
    if (minutes > 0)
        perty_str += minutes + "分";
    if (seconds > 0)
        perty_str += seconds + "秒";

    return perty_str;
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

function getRandomInt(min, max)
{
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

//浮點數相加
function FloatAdd(arg1, arg2)
{
    var r1, r2, m;
    try { r1 = arg1.toString().split(".")[1].length; } catch (e) { r1 = 0; }
    try { r2 = arg2.toString().split(".")[1].length; } catch (e) { r2 = 0; }
    m = Math.pow(10, Math.max(r1, r2));
    return (FloatMul(arg1, m) + FloatMul(arg2, m)) / m;
}

//浮點數相減
function FloatSubtraction(arg1, arg2)
{
    var r1, r2, m, n;
    try { r1 = arg1.toString().split(".")[1].length } catch (e) { r1 = 0 }
    try { r2 = arg2.toString().split(".")[1].length } catch (e) { r2 = 0 }
    m = Math.pow(10, Math.max(r1, r2));
    n = (r1 >= r2) ? r1 : r2;
    return ((arg1 * m - arg2 * m) / m).toFixed(n);
}

//浮點數相乘
function FloatMul(arg1, arg2)
{
    var m = 0, s1 = arg1.toString(), s2 = arg2.toString();
    try { m += s1.split(".")[1].length; } catch (e) { }
    try { m += s2.split(".")[1].length; } catch (e) { }
    return Number(s1.replace(".", "")) * Number(s2.replace(".", "")) / Math.pow(10, m);
}

//浮點數相除
function FloatDiv(arg1, arg2)
{
    var t1 = 0, t2 = 0, r1, r2;
    try { t1 = arg1.toString().split(".")[1].length } catch (e) { }
    try { t2 = arg2.toString().split(".")[1].length } catch (e) { }
    r1 = Number(arg1.toString().replace(".", ""));
    r2 = Number(arg2.toString().replace(".", ""));
    return (r1 / r2) * Math.pow(10, t2 - t1);
}

function rotatePoint(latlngPoint, angleDeg, latlngCenter)
{
    var angleRad = angleDeg * Math.PI / 180;

    //console.log("rotatePoint() angleDeg:"+angleDeg +", angleRad:"+angleRad);
   //console.log("rotatePoint() Math.cos(angleRad):"+Math.cos(angleRad) +", Math.sin(angleRad):"+Math.sin(angleRad));

    var lngRotate = Math.cos(angleRad)*(latlngPoint.lng - latlngCenter.lng) - Math.sin(angleRad)*(latlngPoint.lat-latlngCenter.lat) + latlngCenter.lng;
    var latRotate = Math.sin(angleRad)*(latlngPoint.lng - latlngCenter.lng) + Math.cos(angleRad)*(latlngPoint.lat-latlngCenter.lat) + latlngCenter.lat;
    return (new L.latLng(latRotate, lngRotate));
}

function open_firefighting_record_page()
{
    var win = window.open('/index.php/vilsfirefightingrecord/'+project_id, '_blank');
    win.focus();
}
