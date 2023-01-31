/*
 * Author: ikki Chung
 * Date: 2020.05.06
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var glCurrentPage = 1;

var glReloadCount = 0;

var gl_users = [];
var gl_rfiddevices = [];
var gl_nfclocators = [];
var gl_cardrollcall = {};
var gl_cardrollcallevent = {};
var gl_cardrollcallrecord = [];

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

                load_user();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_rfiddevice();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_nfclocator();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cardrollcall(rollcallid);
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cardrollcallevent(eventid);
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cardrollcallrecord(rollcallid, eventid);
            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                draw_cardrollcallevent_table();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 7:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                draw_cardrollcallrecord_table();

                CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(check_loading_status, 100);
    else
    {
        if (glReloadCount > 0)
        {
            glReloadCount = glReloadCount - 1;
            setTimeout(reload_device_info, 5000);
        }

        console.log("check_loading_status() Stop.");
    }
}

function reload_device_info()
{
    PreLoadIndex = 3;
    CurrentLoadIndex = 4;
    check_loading_status();
}

function load_user()
{
    gl_users = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduser=1&&project_id='+project_id, function(data)
    {
        var user_num = 0;
        $.each(data, function(index, user)
        {
            gl_users.push(user);
            user_num++;
        });

    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_rfiddevice()
{
    gl_rfiddevices = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loaddevice=1&project_id='+project_id, function(data)
    {
        var rfiddevice_num = 0;
        $.each(data, function(index, device)
        {
            gl_rfiddevices.push(device);
            rfiddevice_num++;
        });

    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_nfclocator()
{
    gl_nfclocators = [];

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadnfclocator=1&project_id='+project_id, function(data)
    {
        var rfiddevice_num = 0;
        $.each(data, function(index, device)
        {
            gl_nfclocators.push(device);
            rfiddevice_num++;
        });

    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_cardrollcall(rollcallid)
{
    gl_cardrollcall = {};

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcardrollcall=1&project_id='+project_id+'&rollcallid='+rollcallid, function(data)
    {
        gl_cardrollcall = data;

    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_cardrollcallevent(eventid)
{
    gl_cardrollcallevent = {};

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcardrollcallevent=1&project_id='+project_id+'&eventid='+eventid, function(data)
    {
        gl_cardrollcallevent = data;
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_cardrollcallrecord(rollcallid, eventid)
{
    gl_cardrollcallrecord = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcardrollcallrecord=1&&project_id='+project_id+'&rollcallid='+rollcallid+'&eventid='+eventid, function(data)
    {
        $.each(data, function(index, record)
        {
            gl_cardrollcallrecord.push(record);
        });

    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function draw_cardrollcallevent_table()
{
    var record_table = document.getElementById('RFID_ROLLCALLRESULT_TABLE');

    while (record_table.rows.length >= 1)
        record_table.deleteRow(0);

    console.log("draw_table() gl_cardrollcallevent.rollcallname:"+gl_cardrollcallevent.rollcallname);

    var row = create_event_table_item(0, gl_cardrollcallevent);
    if (row != null)
    {
        record_table.appendChild(row);
    }
}

function create_event_table_item(ui_index, record)
{
    if (record == null)
        return null;

    var row = document.createElement('tr');
    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+(ui_index + 1)));
    cellIndex++;

    // 名稱
    var tdName = document.createElement('td');
    tdName.setAttribute("id","uiid_name_"+record.eventid);
    row.appendChild(tdName);
    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = record.rollcallname;
    tdName.appendChild(SpanName);
    cellIndex++;

    // startdatetime
    var tdcardid = document.createElement('td');
    tdcardid.setAttribute("id","uiid_cardid_"+record.eventid);
    row.appendChild(tdcardid);
    // var SpanStartDateTime  = document.createElement("span");
    // SpanStartDateTime.setAttribute("class","label label-primary");
    // SpanStartDateTime.textContent = record.StartDateTime;
    // row.cells[cellIndex].appendChild(SpanStartDateTime);
    var badge_StartDate = document.createElement("span");
    badge_StartDate.setAttribute("class","label label-info");
    badge_StartDate.textContent = convertPrettyDateOnly(record.StartDateTime);
    row.cells[cellIndex].appendChild(badge_StartDate);

    var badge_StartTime = document.createElement("span");
    badge_StartTime.setAttribute("class","label label-warning");
    badge_StartTime.textContent = convertPrettyTimeOnly(record.StartDateTime);
    row.cells[cellIndex].appendChild(badge_StartTime);
    cellIndex++;

    // EndDateTime
    var tduserid = document.createElement('td');
    tduserid.setAttribute("id","uiid_userid_"+record.recordid);
    row.appendChild(tduserid);
    // var SpanEndDateTime  = document.createElement("span");
    // SpanEndDateTime.setAttribute("class","label label-primary");
    // SpanEndDateTime.textContent = record.EndDateTime;
    // row.cells[cellIndex].appendChild(SpanEndDateTime);
    var badge_EndDate = document.createElement("span");
    badge_EndDate.setAttribute("class","label label-info");
    badge_EndDate.textContent = convertPrettyDateOnly(record.EndDateTime);
    row.cells[cellIndex].appendChild(badge_EndDate);

    var badge_EndTime = document.createElement("span");
    badge_EndTime.setAttribute("class","label label-warning");
    badge_EndTime.textContent = convertPrettyTimeOnly(record.EndDateTime);
    row.cells[cellIndex].appendChild(badge_EndTime);
    cellIndex++;

    // totalcount
    var tdnodeid = document.createElement('td');
    tdnodeid.setAttribute("id","uiid_nodeid_"+record.recordid);
    row.appendChild(tdnodeid);
    var Spantotalcount  = document.createElement("span");
    Spantotalcount.setAttribute("class","label label-primary");
    Spantotalcount.textContent = record.totalcount;
    row.cells[cellIndex].appendChild(Spantotalcount);
    cellIndex++;

    // presentcount
    var tdpresent = document.createElement('td');
    tdpresent.setAttribute("id","uiid_present_"+record.recordid);
    row.appendChild(tdpresent);
    var Spanppresentcount  = document.createElement("span");
    if (record.presentcount == 0)
    {
        Spanppresentcount.setAttribute("class","label label-danger");
        //Spanpresent.textContent = record.present;
        Spanppresentcount.textContent = record.presentcount;

    }
    else
    {
        Spanppresentcount.setAttribute("class","label label-primary");
        //Spanpresent.textContent = record.present;
        Spanppresentcount.textContent = record.presentcount;
    }
    row.cells[cellIndex].appendChild(Spanppresentcount);
    cellIndex++;

    var absentCount = record.totalcount - record.presentcount;
    var tdabsent = document.createElement('td');
    tdabsent.setAttribute("id","uiid_present_"+record.recordid);
    row.appendChild(tdabsent);
    var Spanppresentcount  = document.createElement("span");
    if (absentCount == 0)
    {
        Spanppresentcount.setAttribute("class","label label-primary");
        //Spanpresent.textContent = record.present;
        Spanppresentcount.textContent = absentCount;
    }
    else
    {
        Spanppresentcount.setAttribute("class","label label-danger");
        //Spanpresent.textContent = record.present;
        Spanppresentcount.textContent = absentCount;
    }
    row.cells[cellIndex].appendChild(Spanppresentcount);
    cellIndex++;

    return row;
}

function draw_cardrollcallrecord_table()
{
    var record_table = document.getElementById('RFID_ROLLCALLRECORD_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

     console.log("draw_table() gl_cardrollcallrecord.length:"+gl_cardrollcallrecord.length);

    for (var i = 0; i < gl_cardrollcallrecord.length; ++i)
    {
        var record = gl_cardrollcallrecord[i];

        var row = create_record_table_item(i, record);
        record_table.appendChild(row);
    }
}

function create_record_table_item(ui_index, record)
{
    if (record == null)
        return null;

    var row = document.createElement('tr');
    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    var UseName = '';
    var cellIndex = 0;

    var rollcall = gl_cardrollcall;

    if (record.userid > 0)
    {
        UseName = getUseName(parseInt(record.userid));
    }

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+(ui_index + 1)));
    cellIndex++;

    // card id
    var tdcardid = document.createElement('td');
    row.appendChild(tdcardid);
    var Spancardid  = document.createElement("span");
    if (record.cardid.length > 0)
    {
        Spancardid.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    }
    Spancardid.textContent = record.cardid;
    row.cells[cellIndex].appendChild(Spancardid);
    cellIndex++;

    // 點名裝置
    row.appendChild(document.createElement('td'));
    var ui_rollcalldevices = createRollCallDeviceList(record);
    row.cells[cellIndex].appendChild(ui_rollcalldevices);

    // if ( record.rfiddeviceid.length > 0 )
    // {
    //     var rfiddevice_div = document.createElement("div");
    //     var label = create_rfiddevice_label(getRFIDDeviceName(record.rfiddeviceid));
    //     rfiddevice_div.appendChild(label);
    //     row.cells[cellIndex].appendChild(rfiddevice_div);
    // }
    // else
    // if ( record.nfclocator.length > 0 )
    // {
    //     var nfclocator_div = document.createElement("div");
    //     var label = create_nfclocator_label(getNFCLocatorDeviceName(record.nfclocator));
    //     nfclocator_div.appendChild(label);
    //     row.cells[cellIndex].appendChild(nfclocator_div);
    // }
    cellIndex++;

    // userid
    var tduserid = document.createElement('td');
    row.appendChild(tduserid);
    var Spanuserid  = document.createElement("span");
    if (record.userid > 0)
    {
        Spanuserid.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
        Spanuserid.textContent = UseName;
    }
    row.cells[cellIndex].appendChild(Spanuserid);
    cellIndex++;

    // present
    var tdpresent = document.createElement('td');
    row.appendChild(tdpresent);
    var Spanpresent  = document.createElement("span");
    if (record.present == 0)
    {
        Spanpresent.setAttribute("class","label label-danger");
        //Spanpresent.textContent = record.present;
        if (rollcall != null)
            Spanpresent.textContent = rollcall.absentcomment;

    }
    else
    {
        Spanpresent.setAttribute("class","label label-primary");
        //Spanpresent.textContent = record.present;
        if (rollcall != null)
            Spanpresent.textContent = rollcall.presentcomment;
    }
    row.cells[cellIndex].appendChild(Spanpresent);
    cellIndex++;

    // time
    var tddatetime = document.createElement('td');
    row.appendChild(tddatetime);

    var badge_EndDate = document.createElement("span");
    badge_EndDate.setAttribute("class","label label-info");
    if (record.present != 0)
    {
        badge_EndDate.textContent = convertPrettyDateOnly(record.updatetime);
    }
    row.cells[cellIndex].appendChild(badge_EndDate);

    var badge_EndTime = document.createElement("span");
    badge_EndTime.setAttribute("class","label label-warning");
    if (record.present != 0)
    {
        badge_EndTime.textContent = convertPrettyTimeOnly(record.updatetime);
    }
    row.cells[cellIndex].appendChild(badge_EndTime);
    cellIndex++;

    // 操作
    row.appendChild(document.createElement('td'));
    if (record.present == 0)
    {
        var intime = check_rollcall_intime();
        if (intime)
        {
            var checkBtn = document.createElement("button");
            checkBtn.setAttribute("title","設定點名已到");
            checkBtn.setAttribute("href","javascript:;");
            checkBtn.setAttribute("onclick","confirm_set_present(\""+record.userid+ "\",\""+record.cardid+"\", \""+UseName+"\");return false;");
            var imgCheck = document.createElement("i");
            imgCheck.setAttribute("class","fa fa-user-plus");
            checkBtn.appendChild(imgCheck);
            row.cells[cellIndex].appendChild(checkBtn);
        }
    }
    cellIndex++;

    return row;
}

function createRollCallDeviceList(rollcall)
{
    var span_devicelist = document.createElement("span");
    span_devicelist.setAttribute("id","uiid_devicelist_"+rollcall.rollcallid);

    if ( rollcall.rfiddeviceids.length > 0 )
    {
        var span_rfid_devicelist = create_rfid_devicelist_ui(rollcall.rollcallid, rollcall.rfiddeviceids);
        span_devicelist.appendChild(span_rfid_devicelist);
    }

    if ( rollcall.nfclocators.length > 0 )
    {
        var span_nfc_devicelist = create_nfc_devicelist_ui(rollcall.rollcallid, rollcall.nfclocators);
        span_devicelist.appendChild(span_nfc_devicelist);
    }
    return span_devicelist;
}

function create_rfid_devicelist_ui(rollcallid, rfiddeviceids)
{
    var rfiddeviceidList = rfiddeviceids.split(",");
    var rfiddevice_span = document.createElement("span");
    rfiddevice_span.setAttribute("id","uiid_uhfreader_"+rollcallid);

    for(var index=0;index<rfiddeviceidList.length;index++)
    {
        var rfiddevice_div = document.createElement("div");
        var label = create_rfiddevice_label(getRFIDDeviceName(rfiddeviceidList[index]));
        rfiddevice_div.appendChild(label);
        rfiddevice_span.appendChild(rfiddevice_div);
    }

    return rfiddevice_span;
}

function create_nfc_devicelist_ui(rollcallid, nfclocators)
{
    var nfclocatorList = nfclocators.split(",");
    var nfclocator_span = document.createElement("span");
    nfclocator_span.setAttribute("id","uiid_nfclocator_"+rollcallid);

    for(var index=0;index<nfclocatorList.length;index++)
    {
        var nfclocator_div = document.createElement("div");
        var label = create_nfclocator_label(getNFCLocatorDeviceName(nfclocatorList[index]));
        nfclocator_div.appendChild(label);
        nfclocator_span.appendChild(nfclocator_div);
    }

    return nfclocator_span;
}

function create_rfiddevice_label(context)
{
    var rfiddevice_span = document.createElement("span");
    //rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a7d0;");
    rfiddevice_span.textContent = context;
    return rfiddevice_span;
}

function create_nfclocator_label(context)
{
    var rfiddevice_span = document.createElement("span");
    //rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#f39c12;");
    rfiddevice_span.textContent = context;
    return rfiddevice_span;
}

function clearChildView(uiid)
{
    var ui_item = document.getElementById(uiid);
    if ( ui_item != null )
    {
        while (ui_item.firstChild)
        {
            ui_item.removeChild(ui_item.firstChild);
        }
    }
    return ui_item;
}

function check_rollcall_intime()
{
    var intime = false;
    var StartDateTime = moment(gl_cardrollcallevent.StartDateTime, "YYYY-MM-DD HH:mm:ss");
    var EndDateTime = moment(gl_cardrollcallevent.EndDateTime, "YYYY-MM-DD HH:mm:ss");
    var CurDateTime = moment();

    // console.log("check_rollcall_intime() gl_cardrollcallevent.StartDateTime:"+gl_cardrollcallevent.StartDateTime);
    // console.log("check_rollcall_intime() gl_cardrollcallevent.EndDateTime:"+gl_cardrollcallevent.EndDateTime);
    // console.log("check_rollcall_intime() CurDateTime:"+CurDateTime);

    if ( (CurDateTime >= StartDateTime) && (CurDateTime <= EndDateTime) )
    {
        intime = true;
    }

    return intime;
}

function update_device(deviceid, value_name, value_type, value_ip)
{
    console.log("update_device() deviceid:"+deviceid);

    if ( value_name.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    if ( value_ip.length == 0 )
    {
        alert("請輸入IP位址");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_device'] = 1;
    jsondata['deviceid'] = deviceid;
    jsondata['name'] = value_name;
    jsondata['type'] = value_type;
    jsondata['ip'] = value_ip;

    var targetURL = gl_target_server + "/php/rfiddevice.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
            if ( response.status == 400 )
            {
                alert("update Error! response = " + JSON.stringify(response));
            }

            setTimeout(reload_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function set_present(userid, cardid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_rollcall_record'] = 1;
    jsondata['rollcallid'] = rollcallid;
    jsondata['eventid'] = eventid;
    jsondata['cardid'] = cardid;
    jsondata['userid'] = userid;

    var targetURL = gl_target_server + "/php/rfiddevice.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
            if ( response.status == 400 )
            {
                alert("update Error! response = " + JSON.stringify(response));
            }

            glReloadCount = 5;
            setTimeout(reload_device_info, 5000);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_set_present(userid, cardid, UseName)
{
    if (confirm('是否要設定'+UseName+'點名已到?'))
    {
        set_present(userid, cardid);
    }
    else
    {
        // Do nothing!
    }
}

function getUseName(userid)
{
    var username = userid;
    for (var i = 0; i < gl_users.length; ++i)
    {
        var user = gl_users[i];

        if ( parseInt(user.userid) == userid )
        {
            username = user.name;
            break;
        }

    }
    return username;
}

function getRFIDDeviceName(deviceid)
{
    var RFIDDeviceName = deviceid;
    for (var i = 0; i < gl_rfiddevices.length; ++i)
    {
        var rfiddevice = gl_rfiddevices[i];

        if ( rfiddevice.deviceid === deviceid )
        {
            RFIDDeviceName = rfiddevice.name;
            break;
        }

    }
    return RFIDDeviceName;
}

function getNFCLocatorDeviceName(macaddress)
{
    var NFCLocatorName = macaddress;
    for (var i = 0; i < gl_nfclocators.length; ++i)
    {
        var nfclocator = gl_nfclocators[i];

        if ( nfclocator.macaddress === macaddress )
        {
            NFCLocatorName = nfclocator.name;
            break;
        }

    }
    return NFCLocatorName;
}

function convertPrettyDateOnly(inTime)
{
    //var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset("+08:00");
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
    //var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset("+08:00");
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

function open_cardrollcallevent_page()
{
    var win = window.open('/index.php/vilscardrollcallevent/'+project_id, '_blank');
    win.focus();
}
