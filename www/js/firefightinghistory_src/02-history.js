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

var glNFCdevices = [];
var glAoAdevices = [];
var glAllUsers = [];
var glCaseUsers = [];

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
    $.getJSON(targetURL, 'loadfirefightingcases=1&closed=1'+'&project_id='+project_id , function(data)
    {
        $.each(data, function(index, casedata)
        {
            glFireFightingList.push(casedata);
        });

    })
    .complete(function() {
        // load_cases_users();
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    });
}

// function load_cases_users()
// {
//     var cases = [];
//
//     for	(var index = 0; index < glFireFightingList.length; index++)
//     {
//         var casedata = glFireFightingList[index];
//         cases.push(casedata.casenumber);
//     }
//
//     var targetURL = gl_target_server + "/php/firefighting.php";
//     $.getJSON(targetURL, 'loadfirefightingusers=1&casenumber='+ cases.toString() +'&project_id='+project_id , function(data)
//     {
//         $.each(data, function(index, userdata)
//         {
//             var caseusers = glAreaUser[userdata.casenumber];
//             caseusers.push(userdata);
//         });
//
//     })
//     .complete(function() {
//         CurrentLoadIndex = CurrentLoadIndex + 1; // next state
//     });
// }

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
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    });
}

function manual_search_text_onfocus(id)
{
    var input_value = document.getElementById("manual_search_text").value;

    add_search_result_to_table(input_value);
}

function manual_search_text_change(e)
{
    var input_value = document.getElementById("manual_search_text").value;

    add_search_result_to_table(input_value);
}

const $source = document.querySelector('#manual_search_text');
$source.addEventListener('input', manual_search_text_change);// register for oninput

function add_search_result_to_table(searchtext)
{
    var dropdown_searchresult = document.getElementById('dropdown_searchresult');
    while (dropdown_searchresult.hasChildNodes())
    {
        dropdown_searchresult.removeChild(dropdown_searchresult.firstChild);
    }

    var searchtext_lc = searchtext.toLowerCase();
    // console.log("add_search_result_to_table() searchtext_lc:"+searchtext_lc);

    var count = 0;
    for	(var index = 0; index < glFireFightingList.length; index++)
    {
        var casedata = glFireFightingList[index];
        var casenumber = ''+casedata.casenumber;

        var ret = casenumber.indexOf(searchtext_lc);

        if ( ret < 0 )
            continue;

        // console.log("add_search_result_to_table() Sensor name:"+sensorData.name);

        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("tabIndex","-1");
        item_click.setAttribute("data-value", casedata.casenumber);
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","searchresult_click(\""+ casedata.casenumber +"\");return true;");

        item_click.appendChild(document.createTextNode(" "+casedata.casenumber));
        item_li.appendChild(item_click);

        // console.log("add_search_result_to_table() casenumber:"+casedata.casenumber +' ret:'+ret +' count:'+count);

        dropdown_searchresult.appendChild(item_li);

        count += 1;
    }

    // Toggle dropdown if not already visible:
    if (count > 0)
    {
        if ($('#dropdown_searchresult').is(":hidden"))
        {
            $('#dropdown_searchresult').toggle();
        }
    }

}

function searchresult_click(casenumber)
{
    console.log("searchresult_click() id:"+casenumber);

    //$('#dropdown_searchresult').dropdown('toggle');
    $('#dropdown_searchresult').toggle();

    update_FireFighting_info(casenumber);
    update_cases_users(casenumber);
    update_cases_events(casenumber);
}

function update_FireFighting_info(casenumber)
{
    var record_address = document.getElementById("record_address");
    var record_starttime = document.getElementById("record_starttime");
    var record_closetime = document.getElementById("record_closetime");
    var record_StandbyDeviceIds = document.getElementById("record_StandbyDeviceIds");
    var record_NFCDeviceIds = document.getElementById("record_NFCDeviceIds");

    while (record_StandbyDeviceIds.hasChildNodes())
    {
        record_StandbyDeviceIds.removeChild(record_StandbyDeviceIds.firstChild);
    }
    while (record_NFCDeviceIds.hasChildNodes())
    {
        record_NFCDeviceIds.removeChild(record_NFCDeviceIds.firstChild);
    }

    for	(var index = 0; index < glFireFightingList.length; index++)
    {
        var casedata = glFireFightingList[index];
        if (casedata.casenumber === casenumber)
        {
            record_address.innerHTML = casedata.address;
            record_starttime.innerHTML = casedata.starttime;
            record_closetime.innerHTML = casedata.closetime;
            record_StandbyDeviceIds.appendChild(create_StandbyDeviceList(casedata.StandbyDeviceIds));
            record_NFCDeviceIds.appendChild(create_NFCList(casedata.NFCDeviceIds));
            break;
        }
    }
}

function update_cases_users(casenumber)
{
    glCaseUsers = [];

    var targetURL = gl_target_server + "/php/firefighting.php";
    $.getJSON(targetURL, 'loadfirefightingusers=1&casenumber='+ casenumber +'&project_id='+project_id , function(data)
    {
        $.each(data, function(index, userdata)
        {
            glCaseUsers.push(userdata);
        });
    })
    .complete(function() {
        update_cases_users_info(glCaseUsers);
    });
}

function update_cases_events(casenumber)
{
    var glCaseEvents = [];

    var targetURL = gl_target_server + "/php/firefighting.php";
    $.getJSON(targetURL, 'loadfirefightingevent=1&casenumber='+ casenumber +'&project_id='+project_id , function(data)
    {
        $.each(data, function(index, eventdata)
        {
            glCaseEvents.push(eventdata);
        });
    })
    .complete(function() {
        update_cases_events_info(glCaseEvents);
    });
}

function create_StandbyDeviceList(StandbyDeviceIds)
{
    var standbydevice_selection = document.createElement("div");
    if (StandbyDeviceIds == undefined)
    {
        return standbydevice_selection;
    }

    var standbyDeviceIds = StandbyDeviceIds.split(",");

    for (var j = 0; j < standbyDeviceIds.length; ++j)
    {
        var standbydeviceid = standbyDeviceIds[j];

        for (var i = 0; i < glAoAdevices.length; ++i)
        {
            var aoadevice = glAoAdevices[i];
            if ( aoadevice.id === standbydeviceid)
            {
                var span_AoAdevice = document.createElement("span");
                span_AoAdevice.textContent = aoadevice.name;
                span_AoAdevice.setAttribute("class","badge bg-green");
                standbydevice_selection.appendChild(span_AoAdevice);

                break;
            }
        }

        for (var i = 0; i < glNFCdevices.length; ++i)
        {
            var nfcdevice = glNFCdevices[i];
            if ( ''+nfcdevice.deviceid === standbydeviceid)
            {
                var span_NFCdevice = document.createElement("span");
                span_NFCdevice.textContent = nfcdevice.name;
                span_NFCdevice.setAttribute("class","badge bg-yellow");
                standbydevice_selection.appendChild(span_NFCdevice);

                break;
            }
        }

    }
    return standbydevice_selection;
}

function create_NFCList(NFCDeviceIds)
{
    var nfclocator_selection = document.createElement("div");

    var nfcDeviceIds = NFCDeviceIds.split(",");

    for (var j = 0; j < nfcDeviceIds.length; ++j)
    {
        var nfcdeviceid = nfcDeviceIds[j];

        for (var i = 0; i < glNFCdevices.length; ++i)
        {
            var nfcdevice = glNFCdevices[i];
            if ( nfcdevice.deviceid === nfcdeviceid)
            {
                var span_NFCdevice = document.createElement("span");
                span_NFCdevice.textContent = nfcdevice.name;
                span_NFCdevice.setAttribute("class","badge bg-yellow");
                nfclocator_selection.appendChild(span_NFCdevice);

                break;
            }
        }
    }
    return nfclocator_selection;
}

function update_cases_users_info(glCaseUsers)
{
    var record_Users = document.getElementById("record_Users");
    while (record_Users.hasChildNodes())
    {
        record_Users.removeChild(record_Users.firstChild);
    }

    for	(var index = 0; index < glCaseUsers.length; index++)
    {
        var userdata = glCaseUsers[index];
        var userName = getUserName(userdata.userid);

        var SpanUserName = document.createElement("span");
        SpanUserName.setAttribute("class","badge bg-blue-active");
        // SpanUserName.setAttribute("class","label label-success");
        SpanUserName.setAttribute("style","font-size:18px; font-weight:bold; color:#330033; margin-top:2px; margin-right:2px;");
        SpanUserName.textContent = userName;
        record_Users.appendChild(SpanUserName);
    }
}

function update_cases_events_info(glCaseEvents)
{
    var record_events = document.getElementById("record_events");
    while (record_events.hasChildNodes())
    {
        record_events.removeChild(record_events.firstChild);
    }

    for	(var index = 0; index < glCaseEvents.length; index++)
    {
        var eventdata = glCaseEvents[index];

        var row = document.createElement("tr");

        var td1 = document.createElement("td");
        row.appendChild(td1);
        var timeDiv = document.createElement("div");
        timeDiv.textContent = eventdata.datetime;
        td1.appendChild(timeDiv);

        var td2 = document.createElement("td");
        row.appendChild(td2);
        var typeDiv = document.createElement("div");
        typeDiv.textContent = eventdata.eventtype;
        td2.appendChild(typeDiv);

        var td3 = document.createElement("td");
        row.appendChild(td3);
        td3.appendChild(convertEventData(eventdata.eventtype, eventdata.json));

        record_events.appendChild(row);
    }
}

function convertEventData(eventtype, json)
{
    if (eventtype === 'FireFightingInfo')
    {
        return convertEventData_FireFightingInfo(json);
    }
    else
    if (eventtype === 'FireFightingUser')
    {
        return convertEventData_FireFightingUser(json);
    }
    else
    if (eventtype === 'FireFightingAirUsing')
    {
        return convertEventData_FireFightingAirUsing(json);
    }
    else
    if (eventtype === 'UserFireFightingAirUsing')
    {
        return convertEventData_UserFireFightingAirUsing(json);
    }
}

function convertEventData_FireFightingInfo(json)
{
    var contentDiv = document.createElement("div");

    contentDiv.appendChild(create_two_col_block("案件狀態", document.createTextNode(getCaseStatus(json.caseclosed))));
    contentDiv.appendChild(create_two_col_block("火場記錄開始時間", document.createTextNode(json.starttime)));
    contentDiv.appendChild(create_two_col_block("火場記錄結束時間", document.createTextNode(json.closetime)));
    contentDiv.appendChild(create_two_col_block("待命區定位器", create_StandbyDeviceList(json.StandbyDeviceIds)));
    contentDiv.appendChild(create_two_col_block("進入管制器", create_NFCList(json.NFCDeviceIds)));

    return contentDiv;
}

function convertEventData_FireFightingUser(json)
{
    var contentDiv = document.createElement("div");

    contentDiv.appendChild(create_two_col_block("人員", document.createTextNode(getUserName(json.userid))));
    contentDiv.appendChild(create_two_col_block("區域型態", document.createTextNode(getAreaName(json.areaType))));
    contentDiv.appendChild(create_two_col_block("氣瓶剩餘量", document.createTextNode(json.airremain)));
    contentDiv.appendChild(create_two_col_block("進入時間", document.createTextNode(json.enterTime)));
    contentDiv.appendChild(create_two_col_block("活動狀態", document.createTextNode(json.activity)));
    contentDiv.appendChild(create_two_col_block("心率", document.createTextNode(json.HR)));
    contentDiv.appendChild(create_two_col_block("血氧", document.createTextNode(json.SPO2)));
    contentDiv.appendChild(create_two_col_block("電量", document.createTextNode(json.battery)));

    return contentDiv;
}

function convertEventData_FireFightingAirUsing(json)
{
    var contentDiv = document.createElement("div");

    contentDiv.appendChild(create_two_col_block("人員", document.createTextNode(getUserName(json.userid))));
    contentDiv.appendChild(create_two_col_block("開始氣瓶剩餘量", document.createTextNode(json.startairremain)));
    contentDiv.appendChild(create_two_col_block("開始時間", document.createTextNode(json.airremain)));
    contentDiv.appendChild(create_two_col_block("結束氣瓶剩餘量", document.createTextNode(json.endairremain)));
    contentDiv.appendChild(create_two_col_block("結束時間", document.createTextNode(json.starttime)));
    contentDiv.appendChild(create_two_col_block("氣瓶消耗", document.createTextNode(json.airusing)));
    contentDiv.appendChild(create_two_col_block("使用時間", document.createTextNode(json.totaltime)));

    return contentDiv;
}

function convertEventData_UserFireFightingAirUsing(json)
{
    var contentDiv = document.createElement("div");

    contentDiv.appendChild(create_two_col_block("人員", document.createTextNode(getUserName(json.userid))));
    contentDiv.appendChild(create_two_col_block("氣瓶剩餘量", document.createTextNode(json.airremain)));

    return contentDiv;
}

function create_two_col_block(label_name, data)
{
    var blockDiv = document.createElement("div");
    blockDiv.setAttribute("class","input-group");

    var Span1 = document.createElement("span");
    Span1.setAttribute("class","input-group-addon");
    Span1.setAttribute("style","font-size:12px; font-weight:bold; color:#330033; width:150px;");
    Span1.textContent = label_name;
    blockDiv.appendChild(Span1);

    var Span2 = document.createElement("span");
    Span2.setAttribute("class","input-group-addon");
    Span2.setAttribute("style","font-size:12px; font-weight:bold; color:#003333; background-color:#eeeeee; width:150px;");
    Span2.appendChild(data);
    blockDiv.appendChild(Span2);

    return blockDiv;
}

function getAreaName(areaType)
{
    if (areaType == '0')
        return "火場";
    else
    if (areaType == '1')
        return "待命熱區";
    else
        return "一般待命區";
}

function getCaseStatus(caseclosed)
{
    if (caseclosed == '1')
        return "結束";
    else
        return "尚未結束";
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

    var perty_str = "";
    if (hour > 0)
        perty_str += hour + "小時";
    if (minutes > 0)
        perty_str += minutes + "分";
    if (seconds > 0)
        perty_str += seconds + "秒";

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

function open_firefighting_history_page()
{
    var win = window.open('/index.php/vilsfirefightinghistory/'+project_id, '_blank');
    win.focus();
}
