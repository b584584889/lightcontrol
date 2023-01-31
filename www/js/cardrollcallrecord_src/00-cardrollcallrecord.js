/*
 * Author: ikki Chung
 * Date: 2020.05.05
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var glCurrentPage = 1;

var gl_user_info = [];
var gl_rfiddevices = [];
var gl_nfclocators = [];
var gl_cardrollcall = [];
var gl_cardrollcallrecord = [];

var LOAD_USER_NUM = 500;
var gl_user_load_index = 0;

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

                load_rfiddevice();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_nfclocator();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cardrollcall();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cardrollcallrecord();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_user_info();
            }
        break;
        case 5:
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
        console.log("check_loading_status() Stop.");
    }
}

// function load_user_info()
// {
//     var user_num = 0;
//
//     var targetURL = gl_target_server + "/php/vilsnodes.php";
//     var p = $.getJSON(targetURL, 'loaduser=1&s='+gl_user_load_index+'&count='+LOAD_USER_NUM+'&project_id='+project_id, function(data)
//     {
//         $.each(data, function(index, user)
//         {
//             user.userid = parseInt(user.userid);
//             gl_user_info.push(user);
//
//             user_num += 1;
//         });
//
//         gl_user_load_index += user_num;
//     })
//     .success(function() {
//         if (user_num < LOAD_USER_NUM)
//         {
//             CurrentLoadIndex = CurrentLoadIndex + 1;
//         }
//         else
//         {
//             setTimeout(load_user_info, 20);
//         }
//     })
//     .error(function() {
//         CurrentLoadIndex = CurrentLoadIndex + 1;
//     })
//     .complete(function() {
//     });
// }

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

function load_cardrollcallrecord()
{
    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    gl_cardrollcallrecord = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcardrollcallrecord=1&s='+offset+'&count='+count+'&project_id='+project_id, function(data)
    {
        var cardrollcall_num = 0;
        $.each(data, function(index, record)
        {
            gl_cardrollcallrecord.push(record);
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

function load_user_info()
{
    var queryuser = '';

    var queryCount = 0;
    for (var i = 0; i < gl_cardrollcallrecord.length; ++i)
    {
        var record = gl_cardrollcallrecord[i];

        //
        // check exist
        //
        var bFound = false;
        for (var j = 0; j < gl_user_info.length; ++j)
        {
            var existed_user = gl_user_info[j];
            if ( existed_user.userid == parseInt(record.userid) )
            {
                bFound = true;
                break;
            }
        }

        if ( bFound == false)
        {
            if ( queryCount > 0 )
                queryuser += ',';
            queryuser += record.userid;
            queryCount += 1;
        }
    }

    if (queryuser.length == 0)
    {
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'queryusers=1&userid='+queryuser+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, user)
        {
            user.userid = parseInt(user.userid);
            gl_user_info.push(user);
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .complete(function() {
    });
}

function create_empty_record_table_item(ui_index)
{
    var empty_record_table_item = document.getElementById('empty_record_table_item');
    if ( empty_record_table_item != null )
        return;

    var record_table = document.getElementById('RFID_ROLLCALLRECORD_TABLE');

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

    var cellIndex = 0;

    var rollcall = getRollCall(parseInt(record.rollcallid));

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+(ui_index + 1)));
    cellIndex++;

    // 名稱
    var tdName = document.createElement('td');
    tdName.setAttribute("id","uiid_name_"+record.recordid);
    row.appendChild(tdName);
    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    if (rollcall != null)
        SpanName.textContent = rollcall.rollcallname;
    tdName.appendChild(SpanName);
    cellIndex++;

    // card id
    var tdcardid = document.createElement('td');
    tdcardid.setAttribute("id","uiid_cardid_"+record.recordid);
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

    // if ( (record.rfiddeviceid.length > 0) && (record.rfiddeviceid != '0') )
    // {
    //     var rfiddevice_div = document.createElement("div");
    //     var label = create_rfiddevice_label(getRFIDDeviceName(record.rfiddeviceid));
    //     rfiddevice_div.appendChild(label);
    //     row.cells[cellIndex].appendChild(rfiddevice_div);
    // }
    // else
    // if ( (record.nfclocator.length > 0) && (record.rfiddeviceid != '0') )
    // {
    //     var nfclocator_div = document.createElement("div");
    //     var label = create_nfclocator_label(getNFCLocatorDeviceName( record.nfclocator));
    //     nfclocator_div.appendChild(label);
    //     row.cells[cellIndex].appendChild(nfclocator_div);
    // }
    cellIndex++;

    // userid
    var tduserid = document.createElement('td');
    tduserid.setAttribute("id","uiid_userid_"+record.recordid);
    row.appendChild(tduserid);
    var Spanuserid  = document.createElement("span");
    if (record.userid > 0)
    {
        Spanuserid.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
        Spanuserid.textContent = getUseName(parseInt(record.userid));
    }
    row.cells[cellIndex].appendChild(Spanuserid);
    cellIndex++;

    // nodeid
    // var tdnodeid = document.createElement('td');
    // tdnodeid.setAttribute("id","uiid_nodeid_"+record.recordid);
    // row.appendChild(tdnodeid);
    // var Spannodeid  = document.createElement("span");
    // if (record.nodeid.length > 0)
    // {
    //     Spannodeid.setAttribute("class","label label-primary");
    // }
    // Spannodeid.textContent = record.nodeid;
    // row.cells[cellIndex].appendChild(Spannodeid);
    // cellIndex++;

    // present
    var tdpresent = document.createElement('td');
    tdpresent.setAttribute("id","uiid_present_"+record.recordid);
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
    tddatetime.setAttribute("id","uiid_present_"+record.recordid);
    row.appendChild(tddatetime);
    var Spandatetime  = document.createElement("span");
    Spandatetime.setAttribute("class","label label-primary");
    if (record.present != 0)
    {
        Spandatetime.textContent = record.updatetime;
    }
    row.cells[cellIndex].appendChild(Spandatetime);
    cellIndex++;

    // var tdoperation = document.createElement('td');
    // row.appendChild(tdoperation);

    // // Edit button
    // var editBtn = document.createElement("button");
    // editBtn.setAttribute("id","uiid_editbtn_"+device.deviceid);
    // editBtn.setAttribute("title","Edit this User");
    // editBtn.setAttribute("href","javascript:;");
    // editBtn.setAttribute("onclick","edit_device(\""+device.deviceid+"\");return false;");
    // var imgEdit = document.createElement("i");
    // imgEdit.setAttribute("id","uiid_editimg_"+device.deviceid);
    // imgEdit.setAttribute("class","fa fa-edit");
    // editBtn.appendChild(imgEdit);
    // row.cells[cellIndex].appendChild(editBtn);
    //
    // // delete uesr button
    // var deleteBtn = document.createElement("button");
    // deleteBtn.setAttribute("style","display:none");
    // deleteBtn.setAttribute("id","uiid_deletebtn_"+device.deviceid);
    // deleteBtn.setAttribute("title","Delete This Device");
    // deleteBtn.setAttribute("href","javascript:;");
    // deleteBtn.setAttribute("onclick","confirm_delete_device(\""+device.deviceid+"\", \""+ device.name +"\", \""+ device.type +"\");return false;");
    // var imgDelete = document.createElement("i");
    // imgDelete.setAttribute("class","fa fa-user-times");
    // deleteBtn.appendChild(imgDelete);
    // row.cells[cellIndex].appendChild(deleteBtn);
    // cellIndex++;


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
    //console.log("getRollCall() rollcallid:"+rollcallid +" gl_cardrollcall.length:"+gl_cardrollcall.length);

    var retRollCall = null;
    for (var i = 0; i < gl_cardrollcall.length; ++i)
    {
        var rollcall = gl_cardrollcall[i];
        //console.log("getRollCall() rollcall.rollcallid:"+rollcall.rollcallid);

        if ( parseInt(rollcall.rollcallid) == rollcallid )
        {
            retRollCall = rollcall;
            break;
        }

    }
    return retRollCall;
}

function getUseName(userid)
{
    var username = userid;
    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];

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

function open_cardrollcallrecord_page()
{
    var win = window.open('/index.php/vilscardrollcallrecord/'+project_id, '_blank');
    win.focus();
}
