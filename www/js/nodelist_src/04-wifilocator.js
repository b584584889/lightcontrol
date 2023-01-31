/*
 * Author: ikki Chung
 * Date: 2020.09.08
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var wifilocators_PreLoadIndex = -1;
var wifilocators_CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var wifilocators_glCurrentPage = 1;

var gl_notOnLinelist_wifilocators = [];
var gl_OnLinelist_wifilocators = [];
var gl_editMode_count = 0;

wifilocators_loading_status();

function wifilocators_loading_status()
{
    //console.log("wifilocators_loading_status() wifilocators_PreLoadIndex:" + wifilocators_PreLoadIndex +" wifilocators_CurrentLoadIndex:" + wifilocators_CurrentLoadIndex);
    switch(wifilocators_CurrentLoadIndex)
    {
        case 0:
            if ( wifilocators_PreLoadIndex != wifilocators_CurrentLoadIndex )
            {
                wifilocators_PreLoadIndex = wifilocators_CurrentLoadIndex;

                load_wifilocator();
            }
        break;
        case 1:
            if ( wifilocators_PreLoadIndex != wifilocators_CurrentLoadIndex )
            {
                wifilocators_PreLoadIndex = wifilocators_CurrentLoadIndex;

                draw_wifilocator_table();

                wifilocators_CurrentLoadIndex = wifilocators_CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( wifilocators_PreLoadIndex != wifilocators_CurrentLoadIndex )
            {
                wifilocators_PreLoadIndex = wifilocators_CurrentLoadIndex;

                wifilocators_CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( wifilocators_CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(wifilocators_loading_status, 100);
    else
    {
        // console.log("wifilocators_loading_status() Stop.");

        setTimeout(reload_wifidevice_device_info, 10000);
    }
}

function reload_wifidevice_device_info()
{
    wifilocators_PreLoadIndex = -1;
    wifilocators_CurrentLoadIndex = 0;
    wifilocators_loading_status();
}

function load_wifilocator()
{

    if ( gl_activetab === 'activetab_wifilocators' )
    {
    }
    else
    {
        setTimeout(load_wifilocator, 1000);
        return;
    }

    var offset = (wifilocators_glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var showonline = gl_showfilter_showonline ? 1:0;
    var showoffline = gl_showfilter_showoffline ? 1:0;

    gl_notOnLinelist_wifilocators = [];
    gl_OnLinelist_wifilocators = [];

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadwifilocator=1&s='+offset+'&count='+count+'&project_id='+project_id+'&showonline='+showonline+'&showoffline='+showoffline, function(data)
    {
        $.each(data, function(index, device)
        {
            if (device.alive === '0')
                gl_notOnLinelist_wifilocators.push(device);
            else
                gl_OnLinelist_wifilocators.push(device);
        });
    })
    .success(function() {
        wifilocators_CurrentLoadIndex = wifilocators_CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function draw_wifilocator_table()
{
    if( gl_editMode_count > 0)
    {
        console.log("draw_wifilocator_table() gl_notOnLinelist_wifilocators.length:"+gl_notOnLinelist_wifilocators.length +" gl_editMode_count:"+gl_editMode_count);
        console.log("draw_wifilocator_table() gl_OnLinelist_wifilocators.length:"+gl_OnLinelist_wifilocators.length +" gl_editMode_count:"+gl_editMode_count);
        return;
    }

    var record_table = document.getElementById('LOC_WIFI_LOCATOR_NODE_TABLE');

    while (record_table.rows.length >= 1)
        record_table.deleteRow(0);

    var uiindex = 0;
    if (gl_showfilter_showonline)
    {
        for (var i = 0; i < gl_OnLinelist_wifilocators.length; ++i)
        {
            var device = gl_OnLinelist_wifilocators[i];

            var deviceIndex = (wifilocators_glCurrentPage - 1) * LOAD_PAGE_NUM;
            var row = wifilocators_create_record_table_item(deviceIndex + i, device);
            record_table.appendChild(row);
        }
        uiindex = gl_OnLinelist_wifilocators.length;
    }
    if (gl_showfilter_showoffline)
    {
        for (var i = 0; i < gl_notOnLinelist_wifilocators.length; ++i)
        {
            var device = gl_notOnLinelist_wifilocators[i];

            var deviceIndex = (wifilocators_glCurrentPage - 1) * LOAD_PAGE_NUM + uiindex;
            var row = wifilocators_create_record_table_item(deviceIndex + i, device);
            record_table.appendChild(row);
        }
    }
}

function wifilocators_create_record_table_item(ui_index, device)
{
    if (device == null)
        return null;

    var row = document.createElement('tr');
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
    tdName.setAttribute("id","wifilocator_uiid_name_"+device.macaddress);
    row.appendChild(tdName);
    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = device.name;
    tdName.appendChild(SpanName);
    cellIndex++;

    // type
    var tdaccount = document.createElement('td');
    // tdaccount.setAttribute("id","wifilocator_uiid_type_"+device.macaddress);
    row.appendChild(tdaccount);
    var SpanType  = document.createElement("span");
    if (device.type.length > 0)
    {
        SpanType.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    }
    SpanType.textContent = device.type;
    row.cells[cellIndex].appendChild(SpanType);
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
    tdtitle.setAttribute("id","wifilocator_uiid_macaddress_"+device.macaddress);
    row.appendChild(tdtitle);
    var Spanmacaddress  = document.createElement("span");
    if (device.macaddress.length > 0)
    {
        Spanmacaddress.setAttribute("class","label label-primary");
    }
    Spanmacaddress.textContent = device.macaddress;
    row.cells[cellIndex].appendChild(Spanmacaddress);
    cellIndex++;

    // ap ssid
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","wifilocator_uiid_ssid_"+device.macaddress);
    row.appendChild(tdtitle);
    var Spanssid  = document.createElement("span");
    if (device.ssid.length > 0)
    {
        Spanssid.setAttribute("class","label label-warning");
    }
    Spanssid.textContent = device.ssid;
    row.cells[cellIndex].appendChild(Spanssid);
    cellIndex++;

    // wifimac
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","wifilocator_uiid_wifimac_"+device.macaddress);
    row.appendChild(tdtitle);
    var Spanwifimac  = document.createElement("span");
    if (device.wifimac.length > 0)
    {
        Spanwifimac.setAttribute("class","label label-success");
    }
    Spanwifimac.textContent = device.wifimac;
    row.cells[cellIndex].appendChild(Spanwifimac);
    cellIndex++;

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    // version
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","edgecontrol_uiid_version_"+device.macaddress);
    row.appendChild(tdtitle);
    var Spanversion  = document.createElement("span");
    if (device.version.length > 0)
    {
        Spanversion.setAttribute("class","label label-warning");
    }
    Spanversion.textContent = device.version;
    row.cells[cellIndex].appendChild(Spanversion);
    cellIndex++;

    // Edit button
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id","wifilocator_uiid_editbtn_"+device.macaddress);
    editBtn.setAttribute("title","Edit this device");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_wifilocator_device(\""+device.alive+"\", \""+device.macaddress+"\",\""+ device.name +"\", \""+ device.type +"\", \""+ device.ssid +"\", \""+ device.wifimac +"\");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","wifilocator_uiid_editimg_"+device.macaddress);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    // delete button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("style","display:none");
    deleteBtn.setAttribute("id","wifilocator_uiid_deletebtn_"+device.macaddress);
    deleteBtn.setAttribute("title","Delete This Device");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","confirm_delete_wifilocator_device(\""+device.macaddress+"\", \""+ device.name +"\", \""+ device.type +"\");return false;");
    var imgDelete = document.createElement("i");
    imgDelete.setAttribute("class","fa fa-user-times");
    deleteBtn.appendChild(imgDelete);
    row.cells[cellIndex].appendChild(deleteBtn);
    cellIndex++;


    return row;
}

function edit_wifilocator_device(alive, macaddress, name, type, ssid, wifimac)
{
    console.log("edit_wifilocator_device() macaddress:"+macaddress);

    gl_editMode_count += 1;

    //create_edit_ui("uiid_macaddress", macaddress, macaddress);
    create_edit_ui("wifilocator_uiid_name", macaddress, name);
    // create_edit_ui("uiid_type", macaddress, type);
    create_edit_ui("wifilocator_uiid_ssid", macaddress, ssid);
    create_edit_ui("wifilocator_uiid_wifimac", macaddress, wifimac);

    var editBtn = document.getElementById("wifilocator_uiid_editbtn_"+macaddress);
    editBtn.setAttribute("title","Save this Device");
    editBtn.setAttribute("onclick","save_wifilocator_device(\""+alive+"\",\""+macaddress+"\");return false;");
    var imgEdit = document.getElementById("wifilocator_uiid_editimg_"+macaddress);
    imgEdit.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("wifilocator_uiid_deletebtn_"+macaddress);
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

function save_wifilocator_device(alive, macaddress)
{
    //var value_macaddress = document.getElementById("uiid_macaddress_"+macaddress+"_edit").value;
    var value_name = document.getElementById("wifilocator_uiid_name_"+macaddress+"_edit").value;
    //var value_type = document.getElementById("uiid_type_"+macaddress+"_edit").value;
    var value_ssid = document.getElementById("wifilocator_uiid_ssid_"+macaddress+"_edit").value;
    var value_wifimac = document.getElementById("wifilocator_uiid_wifimac_"+macaddress+"_edit").value;

    update_wifilocator_device(alive, macaddress, value_name, value_ssid, value_wifimac);
}

function update_wifilocator_device(alive, macaddress, value_name, value_ssid, value_wifimac)
{
    console.log("update_wifilocator_device() macaddress:"+macaddress);
    console.log("update_wifilocator_device() value_name:"+value_name);
    //console.log("update_wifilocator_device() value_type:"+value_type);

    if ( value_name.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    if ( macaddress.length == 0 )
    {
        alert("請輸入硬體位址");
        return;
    }

    gl_editMode_count = 0;

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_wifilocator'] = 1;
    jsondata['alive'] = alive;
    jsondata['macaddress'] = macaddress;
    jsondata['name'] = value_name;
    //jsondata['type'] = value_type;
    jsondata['wifimac'] = value_wifimac;
    jsondata['ssid'] = value_ssid;

    var targetURL = gl_target_server + "/php/nfclocator.php";

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

            setTimeout(reload_wifidevice_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_wifilocator_device(macaddress, name, type)
{
    if (confirm('Are you sure you want to delete this device:'+name+' '+type+'   ?'))
    {
        // delete it!
        delete_wifilocator_device(macaddress);
    }
    else
    {
        // Do nothing!
    }
}

function delete_wifilocator_device(macaddress)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_wifilocator'] = 1;
    jsondata['macaddress'] = macaddress;

    gl_editMode_count = 0;

    var targetURL = gl_target_server + "/php/nfclocator.php";

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
            setTimeout(reload_wifidevice_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function wifilocators_move_page_backward()
{
    var current_page_num_ui = document.getElementById('wifilocators_id_current_page_num');

    if ( wifilocators_glCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        wifilocators_glCurrentPage--;
        current_page_num_ui.textContent = wifilocators_glCurrentPage;

        reload_wifidevice_device_info();
    }
}

function wifilocators_move_page_forward()
{
    var current_page_num_ui = document.getElementById('wifilocators_id_current_page_num');

    wifilocators_glCurrentPage++;
    current_page_num_ui.textContent = wifilocators_glCurrentPage;

    reload_wifidevice_device_info();

}
