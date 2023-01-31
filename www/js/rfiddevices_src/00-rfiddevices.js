/*
 * Author: ikki Chung
 * Date: 2020.05.04
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var rfiddevices_PreLoadIndex = -1;
var rfiddevices_CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var rfiddevices_glCurrentPage = 1;

var gl_notOnLinelist_rfiddevices = [];
var gl_OnLinelist_rfiddevices = [];
var gl_editMode_count = 0;

rfiddevices_id_check_loading_status();

function rfiddevices_id_check_loading_status()
{
    //console.log("rfiddevices_id_check_loading_status() rfiddevices_PreLoadIndex:" + rfiddevices_PreLoadIndex +" rfiddevices_CurrentLoadIndex:" + rfiddevices_CurrentLoadIndex);
    switch(rfiddevices_CurrentLoadIndex)
    {
        case 0:
            if ( rfiddevices_PreLoadIndex != rfiddevices_CurrentLoadIndex )
            {
                // console.log("rfiddevices_id_check_loading_status() rfiddevices_PreLoadIndex:" + rfiddevices_PreLoadIndex +" != rfiddevices_CurrentLoadIndex:" + rfiddevices_CurrentLoadIndex);
                rfiddevices_PreLoadIndex = rfiddevices_CurrentLoadIndex;

                //load_rfiddevice_info();
                load_rfiddevice();
            }
        break;
        case 1:
            if ( rfiddevices_PreLoadIndex != rfiddevices_CurrentLoadIndex )
            {
                // console.log("rfiddevices_id_check_loading_status() rfiddevices_PreLoadIndex:" + rfiddevices_PreLoadIndex +" != rfiddevices_CurrentLoadIndex:" + rfiddevices_CurrentLoadIndex);
                rfiddevices_PreLoadIndex = rfiddevices_CurrentLoadIndex;

                draw_rfiddevice_table();

                rfiddevices_CurrentLoadIndex = rfiddevices_CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( rfiddevices_PreLoadIndex != rfiddevices_CurrentLoadIndex )
            {
                // console.log("rfiddevices_id_check_loading_status() rfiddevices_PreLoadIndex:" + rfiddevices_PreLoadIndex +" != rfiddevices_CurrentLoadIndex:" + rfiddevices_CurrentLoadIndex);
                rfiddevices_PreLoadIndex = rfiddevices_CurrentLoadIndex;

                rfiddevices_CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( rfiddevices_CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(rfiddevices_id_check_loading_status, 100);
    else
    {
        // console.log("rfiddevices_id_check_loading_status() Stop.");

        setTimeout(reload_device_info, 10000);
    }
}

function load_rfiddevice()
{

    if ( gl_activetab === 'activetab_uhfreaders' )
    {
    }
    else
    {
        setTimeout(load_rfiddevice, 1000);
        return;
    }

    var offset = (rfiddevices_glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var showonline = gl_showfilter_showonline ? 1:0;
    var showoffline = gl_showfilter_showoffline ? 1:0;

    gl_notOnLinelist_rfiddevices = [];
    gl_OnLinelist_rfiddevices = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loaddevice=1&s='+offset+'&count='+count+'&project_id='+project_id+'&showonline='+showonline+'&showoffline='+showoffline, function(data)
    {
        $.each(data, function(index, device)
        {
            if (device.alive === '0')
                gl_notOnLinelist_rfiddevices.push(device);
            else
                gl_OnLinelist_rfiddevices.push(device);
        });
    })
    .success(function() {
        rfiddevices_CurrentLoadIndex = rfiddevices_CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function create_empty_record_table_item(ui_index)
{
    var empty_record_table_item = document.getElementById('empty_record_table_item');
    if ( empty_record_table_item != null )
        return;

    var record_table = document.getElementById('RFID_DEVICE_TABLE');

    var row = document.createElement('tr');
    row.setAttribute("id", "empty_record_table_item");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // alive
    row.appendChild(document.createElement('td'));
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

    // macaddress
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_macaddress"));
    cellIndex++;

    // LoRAMacaddress
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_LoRAMacaddress"));
    cellIndex++;

    // gwid
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_gwid"));
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

function draw_rfiddevice_table()
{
    if( gl_editMode_count > 0)
    {
        console.log("draw_table() gl_notOnLinelist_rfiddevices.length:"+gl_notOnLinelist_rfiddevices.length +" gl_editMode_count:"+gl_editMode_count);
        console.log("draw_table() gl_OnLinelist_rfiddevices.length:"+gl_OnLinelist_rfiddevices.length +" gl_editMode_count:"+gl_editMode_count);
        return;
    }

    var record_table = document.getElementById('RFID_DEVICE_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

    if (gl_showfilter_showonline)
    {
        for (var i = 0; i < gl_OnLinelist_rfiddevices.length; ++i)
        {
            var device = gl_OnLinelist_rfiddevices[i];

            var row = rfiddevices_create_record_table_item(i, device);
            record_table.appendChild(row);
        }
    }
    if (gl_showfilter_showoffline)
    {
        for (var i = 0; i < gl_notOnLinelist_rfiddevices.length; ++i)
        {
            var device = gl_notOnLinelist_rfiddevices[i];

            var row = rfiddevices_create_record_table_item(i, device);
            record_table.appendChild(row);
        }
    }
}

function rfiddevices_create_record_table_item(ui_index, device)
{
    if (device == null)
        return null;

    // var row = document.createElement('tr');
    // if ((ui_index % 2) == 1)
    //     row.setAttribute("bgcolor","#ebebe0");

    var row = document.createElement('tr');
    row.setAttribute("id", device.macaddress+"_TABLE_ROW");

    if ( device.alive === '0' )
        row.setAttribute("bgcolor","#a3c2c2");
    else
        row.setAttribute("bgcolor","#FFFFFF");


    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+(ui_index + 1)));
    cellIndex++;

    // alive
    row.appendChild(document.createElement('td'));
    if ( device.alive != '0' )
    {
        var SpanAlive = document.createElement("span");
        SpanAlive.setAttribute("class","badge bg-fuchsia");
        SpanAlive.textContent = device.alive;
        row.cells[cellIndex].appendChild(SpanAlive);
    }
    cellIndex++;

    // 名稱
    var tdName = document.createElement('td');
    tdName.setAttribute("id","uiid_name_"+device.deviceid);
    row.appendChild(tdName);
    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = device.name;
    tdName.appendChild(SpanName);
    cellIndex++;

    // type
    var tdaccount = document.createElement('td');
    tdaccount.setAttribute("id","uiid_type_"+device.deviceid);
    row.appendChild(tdaccount);
    var SpanType  = document.createElement("span");
    if (device.type.length > 0)
    {
        SpanType.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    }
    SpanType.textContent = device.type;
    row.cells[cellIndex].appendChild(SpanType);
    cellIndex++;

    // IP
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","uiid_ip_"+device.deviceid);
    row.appendChild(tdtitle);
    var SpanIP  = document.createElement("span");
    if (device.ip.length > 0)
    {
        SpanIP.setAttribute("class","label label-info");
    }
    SpanIP.textContent = device.ip;
    row.cells[cellIndex].appendChild(SpanIP);
    cellIndex++;

    // verifykey
    var tdverifykey = document.createElement('td');
    row.appendChild(tdverifykey);
    var SpanVerifykey  = document.createElement("span");
    SpanVerifykey.textContent = device.verifykey;
    row.cells[cellIndex].appendChild(SpanVerifykey);
    cellIndex++;

    // macaddress
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","uiid_macaddress_"+device.deviceid);
    row.appendChild(tdtitle);
    var SpanMacaddress  = document.createElement("span");
    if (device.macaddress.length > 0)
    {
        SpanMacaddress.setAttribute("class","label label-primary");
        //SpanMacaddress.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    }
    SpanMacaddress.textContent = device.macaddress;
    row.cells[cellIndex].appendChild(SpanMacaddress);
    cellIndex++;

    // LoRAMacaddress
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","uiid_LoRAMacaddress_"+device.deviceid);
    row.appendChild(tdtitle);
    var SpanLoRAMacaddress  = document.createElement("span");
    if (device.LoRAMacaddress.length > 0)
    {
        SpanLoRAMacaddress.setAttribute("class","label label-warning");
    }
    SpanLoRAMacaddress.textContent = device.LoRAMacaddress;
    row.cells[cellIndex].appendChild(SpanLoRAMacaddress);
    cellIndex++;

    // gwid
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","uiid_gwid_"+device.deviceid);
    row.appendChild(tdtitle);
    var Spangwid  = document.createElement("span");
    if (device.gwid.length > 0)
    {
        Spangwid.setAttribute("class","label label-success");
    }
    Spangwid.textContent = device.gwid;
    row.cells[cellIndex].appendChild(Spangwid);
    cellIndex++;

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    // Edit button
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id","uiid_editbtn_"+device.deviceid);
    editBtn.setAttribute("title","Edit this User");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_device(\""+device.deviceid+"\");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","uiid_editimg_"+device.deviceid);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    // delete uesr button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("style","display:none");
    deleteBtn.setAttribute("id","uiid_deletebtn_"+device.deviceid);
    deleteBtn.setAttribute("title","Delete This Device");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","confirm_delete_device(\""+device.deviceid+"\", \""+ device.name +"\", \""+ device.type +"\");return false;");
    var imgDelete = document.createElement("i");
    imgDelete.setAttribute("class","fa fa-user-times");
    deleteBtn.appendChild(imgDelete);
    row.cells[cellIndex].appendChild(deleteBtn);
    cellIndex++;


    return row;
}

function edit_device(deviceid)
{
    console.log("edit_device() deviceid:"+deviceid);

    var device = getDevice(deviceid);
    if (device == null)
        return;

    gl_editMode_count += 1;

    create_edit_ui("uiid_name", deviceid, device.name);
    // create_edit_ui("uiid_type", deviceid, device.type);
    create_edit_ui("uiid_ip", deviceid, device.ip);
    create_edit_ui("uiid_macaddress", deviceid, device.macaddress);
    create_edit_ui("uiid_LoRAMacaddress", deviceid, device.LoRAMacaddress);
    create_edit_ui("uiid_gwid", deviceid, device.gwid);

    var editBtn = document.getElementById("uiid_editbtn_"+deviceid);
    editBtn.setAttribute("title","Save this Device");
    editBtn.setAttribute("onclick","save_device(\""+device.alive+"\",\""+deviceid+"\");return false;");
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

function save_device(alive, deviceid)
{
    var value_name = document.getElementById("uiid_name_"+deviceid+"_edit").value;
    //var value_type = document.getElementById("uiid_type_"+deviceid+"_edit").value;
    var value_ip = document.getElementById("uiid_ip_"+deviceid+"_edit").value;
    var value_macaddress = document.getElementById("uiid_macaddress_"+deviceid+"_edit").value;
    var value_LoRAMacaddress = document.getElementById("uiid_LoRAMacaddress_"+deviceid+"_edit").value;
    var value_gwid = document.getElementById("uiid_gwid_"+deviceid+"_edit").value;

    update_device(alive, deviceid, value_name, value_ip, value_macaddress, value_LoRAMacaddress, value_gwid);
}

function update_device(alive, deviceid, value_name, value_ip, value_macaddress, value_LoRAMacaddress, value_gwid)
{
    console.log("update_device() deviceid:"+deviceid);

    if ( value_name.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    gl_editMode_count = 0;

    // if ( value_ip.length == 0 )
    // {
    //     alert("請輸入IP位址");
    //     return;
    // }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_device'] = 1;
    jsondata['deviceid'] = deviceid;
    jsondata['alive'] = alive;
    jsondata['macaddress'] = value_macaddress;
    jsondata['name'] = value_name;
    //jsondata['type'] = value_type;
    jsondata['ip'] = value_ip;
    jsondata['gwid'] = value_gwid;
    jsondata['LoRAMacaddress'] = value_LoRAMacaddress;

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
    var input_macaddress = document.getElementById("input_macaddress").value;
    var input_LoRAMacaddress = document.getElementById("input_LoRAMacaddress").value;
    var input_gwid = document.getElementById("input_gwid").value;

    if ( input_name.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    // if ( input_ip.length == 0 )
    // {
    //     alert("請輸入IP位址");
    //     return;
    // }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_rfiddevice'] = 1;
    jsondata['macaddress'] = input_macaddress;
    jsondata['name'] = input_name;
    jsondata['type'] = input_type;
    jsondata['ip'] = input_ip;
    jsondata['gwid'] = input_gwid;
    jsondata['LoRAMacaddress'] = input_gwid;

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

    gl_editMode_count = 0;

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
    rfiddevices_PreLoadIndex = -1;
    rfiddevices_CurrentLoadIndex = 0;
    rfiddevices_id_check_loading_status();
}

function getDevice(deviceid)
{
    var retDevice = null;
    for (var i = 0; i < gl_OnLinelist_rfiddevices.length; ++i)
    {
        var device = gl_OnLinelist_rfiddevices[i];

        if ( device.deviceid === deviceid )
        {
            retDevice = device;
            return retDevice;
        }
    }
    for (var i = 0; i < gl_notOnLinelist_rfiddevices.length; ++i)
    {
        var device = gl_notOnLinelist_rfiddevices[i];

        if ( device.deviceid === deviceid )
        {
            retDevice = device;
            return retDevice;
        }
    }
    return retDevice;
}

function getDeviceName(deviceid)
{
    var retDeviceName = deviceid;
    for (var i = 0; i < gl_OnLinelist_rfiddevices.length; ++i)
    {
        var device = gl_OnLinelist_rfiddevices[i];

        if ( device.deviceid === deviceid )
        {
            retDeviceName = device.name;
            return retDeviceName;
        }
    }
    for (var i = 0; i < gl_notOnLinelist_rfiddevices.length; ++i)
    {
        var device = gl_notOnLinelist_rfiddevices[i];

        if ( device.deviceid === deviceid )
        {
            retDeviceName = device.name;
            return retDeviceName;
        }
    }
    return retDeviceName;
}

function rfiddevices_move_page_backward()
{
    var current_page_num_ui = document.getElementById('rfiddevices_id_current_page_num');

    if ( rfiddevices_glCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        rfiddevices_glCurrentPage--;
        current_page_num_ui.textContent = rfiddevices_glCurrentPage;

        reload_device_info();
    }
}

function rfiddevices_move_page_forward()
{
    var current_page_num_ui = document.getElementById('rfiddevices_id_current_page_num');

    rfiddevices_glCurrentPage++;
    current_page_num_ui.textContent = rfiddevices_glCurrentPage;

    reload_device_info();

}

function open_rfiddevices_page()
{
    var win = window.open('/index.php/vilsrfiddevices/'+project_id, '_blank');
    win.focus();
}
