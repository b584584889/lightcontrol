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

var gl_cardrollcall = [];
var gl_cardrollcallevent = [];
var gl_usersubgroups = [];
var gl_cardrollcallsubgroupevent = {};


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

                load_cardrollcall();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cardrollcallevent();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_usersubgroup();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cardrollcallsubgroupevent();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                draw_cardrollcallevent_table();

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

function load_cardrollcall()
{
    gl_cardrollcall = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcardrollcall=1&project_id='+project_id, function(data)
    {
        var cardrollcall_num = 0;
        $.each(data, function(index, rollcall)
        {
            gl_cardrollcall.push(rollcall);
            cardrollcall_num++;
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

function load_cardrollcallevent()
{
    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    gl_cardrollcallevent = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcardrollcallevent=1&s='+offset+'&count='+count+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, record)
        {
            gl_cardrollcallevent.push(record);
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

function load_usersubgroup()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadusersubgroup=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, usersubgroup)
        {
            usersubgroup.subgroupid = parseInt(usersubgroup.subgroupid);
            gl_usersubgroups.push(usersubgroup);
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

function load_cardrollcallsubgroupevent()
{
    gl_cardrollcallsubgroupevent = {};

    var queryeventids = '';
    var queryCount = 0;
    for (var i = 0; i < gl_cardrollcallevent.length; ++i)
    {
        var event = gl_cardrollcallevent[i];

        if ( queryCount > 0 )
            queryeventids += ',';
        queryeventids += event.eventid;
        queryCount += 1;
    }

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcardrollcallsubgroupevent=1&project_id='+project_id+'&eventids='+queryeventids, function(data)
    {
        $.each(data, function(eventid, records)
        {
            gl_cardrollcallsubgroupevent[eventid] = [];
            gl_cardrollcallsubgroupevent[eventid].push(records);

            //
            // update subggroup event
            //
            for (var i = 0; i < gl_cardrollcallevent.length; ++i)
            {
                var event = gl_cardrollcallevent[i];
                if ( event.eventid === eventid )
                {
                    var totalcount = 0;
                    var presentcount = 0;

                    //
                    // find valid subgroup
                    //
                    for (var j = 0; j < records.length; ++j)
                    {
                        var record = records[j];
                        var valid = check_valid_subgroup(parseInt(record.subgroupid));
                        if ( valid )
                        {
                            totalcount += parseInt(record.totalcount);
                            presentcount += parseInt(record.presentcount);
                        }
                    }

                    // console.log("load_cardrollcallsubgroupevent() eventid:"+eventid+" event.totalcount:"+event.totalcount+" totalcount:"+totalcount);
                    // console.log("load_cardrollcallsubgroupevent() eventid:"+eventid+" event.presentcount:"+event.presentcount+" presentcount:"+presentcount);

                    event.totalcount = totalcount;
                    event.presentcount = presentcount;

                    gl_cardrollcallevent[i] = event;
                    break;
                }
            }

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

function check_valid_subgroup(subgroupid)
{
    for (var j = 0; j < gl_usersubgroups.length; j++)
    {
        var usersubgroup = gl_usersubgroups[j];
        // var usersubgroupid = usersubgroup.usersubgroupid;
        //var usersubgroupid = parseInt(usersubgroup);

        // console.log("check_valid_subgroup() usersubgroup.subgroupid:"+usersubgroup.subgroupid+" subgroupid:"+subgroupid);

        if ( usersubgroup.subgroupid == subgroupid )
        {
            return true;
        }
    }

    return false;
}

function create_empty_record_table_item(ui_index)
{
    var empty_record_table_item = document.getElementById('empty_record_table_item');
    if ( empty_record_table_item != null )
        return;

    var record_table = document.getElementById('RFID_ROLLCALLEVENT_TABLE');

    var row = document.createElement('tr');
    row.setAttribute("id", "empty_record_table_item");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // name
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_name"));
    cellIndex++;

    // type
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_type"));
    cellIndex++;

    // IP
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_ip"));
    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_device();");
    Button_save.textContent = "儲存";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;

    record_table.appendChild(row);
}

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.textContent = text;

    return input_text;
}

function draw_cardrollcallevent_table()
{
    var record_table = document.getElementById('RFID_ROLLCALLEVENT_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

     // console.log("draw_table() gl_cardrollcallevent.length:"+gl_cardrollcallevent.length);

     var ui_index = 0;
    for (var i = 0; i < gl_cardrollcallevent.length; ++i)
    {
        var record = gl_cardrollcallevent[i];

        var row = create_record_table_item(ui_index, record);
        if (row != null)
        {
            record_table.appendChild(row);
            ui_index++;
        }
    }
}

function create_record_table_item(ui_index, record)
{
    if (record == null)
        return null;

    var row = document.createElement('tr');
    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    var cellIndex = 0;

    var rollcall = getRollCall(record.rollcallid);
    if (rollcall == null)
        return null;

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
    SpanName.textContent = rollcall.rollcallname;
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
    tduserid.setAttribute("id","uiid_userid_"+record.eventid);
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
    tdnodeid.setAttribute("id","uiid_nodeid_"+record.eventid);
    row.appendChild(tdnodeid);
    var Spantotalcount  = document.createElement("span");
    Spantotalcount.setAttribute("class","label label-primary");
    Spantotalcount.textContent = record.totalcount;
    row.cells[cellIndex].appendChild(Spantotalcount);
    cellIndex++;

    // presentcount
    var tdpresent = document.createElement('td');
    tdpresent.setAttribute("id","uiid_present_"+record.eventid);
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
    tdabsent.setAttribute("id","uiid_present_"+record.eventid);
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

    // 操作
    row.appendChild(document.createElement('td'));

    var checkBtn = document.createElement("button");
    checkBtn.setAttribute("title","查看詳細資訊");
    checkBtn.setAttribute("href","javascript:;");
    checkBtn.setAttribute("onclick","show_rollcallresult(\""+rollcall.rollcallid+"\",\""+record.eventid+"\");return false;");
    var imgCheck = document.createElement("i");
    imgCheck.setAttribute("class","fa fa-calendar-check-o ");
    checkBtn.appendChild(imgCheck);
    row.cells[cellIndex].appendChild(checkBtn);

    return row;
}

function edit_device(deviceid)
{
    console.log("edit_device() deviceid:"+deviceid);

    var device = getDevice(deviceid);
    if (device == null)
        return;

    create_edit_ui("uiid_name", deviceid, device.name);
    create_edit_ui("uiid_type", deviceid, device.type);
    create_edit_ui("uiid_ip", deviceid, device.ip);

    var editBtn = document.getElementById("uiid_editbtn_"+deviceid);
    editBtn.setAttribute("title","Save this Device");
    editBtn.setAttribute("onclick","save_device(\""+deviceid+"\");return false;");
    var imgEdit = document.getElementById("uiid_editimg_"+deviceid);
    imgEdit.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("uiid_deletebtn_"+deviceid);
    deleteBtn.setAttribute("style","display:block");
}

function create_edit_ui(uiid, deviceid, text)
{
    var uiid_name = clearChildView(uiid+"_"+deviceid);
    var new_input = create_input_text(uiid+"_"+deviceid+"_edit", text);
    uiid_name.appendChild(new_input);
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

function save_device(deviceid)
{
    var value_name = document.getElementById("uiid_name_"+deviceid+"_edit").value;
    var value_type = document.getElementById("uiid_type_"+deviceid+"_edit").value;
    var value_ip = document.getElementById("uiid_ip_"+deviceid+"_edit").value;

    update_device(deviceid, value_name, value_type, value_ip);
}

function show_rollcallresult(rollcallid, eventid)
{
    var win = window.open('/index.php/vilsuserrollcallresult/'+project_id+'?rollcallid='+rollcallid+'&eventid='+eventid, '_blank');
    win.focus();
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

function save_new_device()
{
    var input_name = document.getElementById("input_name").value;
    var input_type = document.getElementById("input_type").value;
    var input_ip = document.getElementById("input_ip").value;

    if ( input_name.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    if ( input_ip.length == 0 )
    {
        alert("請輸入IP位址");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_rfiddevice'] = 1;
    jsondata['name'] = input_name;
    jsondata['type'] = input_type;
    jsondata['ip'] = input_ip;

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

function confirm_delete_device(deviceid, name, type)
{
    if (confirm('Are you sure you want to delete this device:'+name+' '+type+'   ?'))
    {
        // delete it!
        delete_device(deviceid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_device(deviceid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_device'] = 1;
    jsondata['deviceid'] = deviceid;

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

$("#createnewrfiddevicebutton").click(function()
{
    create_empty_record_table_item(gl_rfiddevices.length + 1);

    return false;
});

function reload_device_info()
{
    PreLoadIndex = -1;
    CurrentLoadIndex = 0;
    check_loading_status();
}

function getDevice(deviceid)
{
    var retDevice = null;
    for (var i = 0; i < gl_rfiddevices.length; ++i)
    {
        var device = gl_rfiddevices[i];

        if ( device.deviceid === deviceid )
        {
            retDevice = device;
            break;
        }

    }
    return retDevice;
}

function getRollCall(rollcallid)
{
    // console.log("getRollCall() rollcallid:"+rollcallid +" gl_cardrollcall.length:"+gl_cardrollcall.length);

    var rollCall = null;
    for (var i = 0; i < gl_cardrollcall.length; ++i)
    {
        var rollcall = gl_cardrollcall[i];

        if ( rollcall.rollcallid === rollcallid )
        {
            rollCall = rollcall;
            break;
        }

    }
    return rollCall;
}

function move_page_backward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    if ( glCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        glCurrentPage--;
        current_page_num_ui.textContent = glCurrentPage;

        reload_device_info();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    glCurrentPage++;
    current_page_num_ui.textContent = glCurrentPage;

    reload_device_info();

}

function convertPrettyDateOnly(inTime)
{
    //var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset("+08:00");
    var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss");
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
    var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss");
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
