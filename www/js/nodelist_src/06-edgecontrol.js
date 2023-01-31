/*
 * Author: ikki Chung
 * Date: 2020.09.09
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var edgecontrols_PreLoadIndex = -1;
var edgecontrols_CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var edgecontrols_glCurrentPage = 1;

var gl_notOnLinelist_edgecontrols = [];
var gl_OnLinelist_edgecontrols = [];
var gl_editMode_count = 0;

edgecontrols_loading_status();

function edgecontrols_loading_status()
{
    //console.log("edgecontrols_loading_status() edgecontrols_PreLoadIndex:" + edgecontrols_PreLoadIndex +" edgecontrols_CurrentLoadIndex:" + edgecontrols_CurrentLoadIndex);
    switch(edgecontrols_CurrentLoadIndex)
    {
        case 0:
            if ( edgecontrols_PreLoadIndex != edgecontrols_CurrentLoadIndex )
            {
                edgecontrols_PreLoadIndex = edgecontrols_CurrentLoadIndex;

                load_edgecontrol();
            }
        break;
        case 1:
            if ( edgecontrols_PreLoadIndex != edgecontrols_CurrentLoadIndex )
            {
                edgecontrols_PreLoadIndex = edgecontrols_CurrentLoadIndex;

                draw_edgecontrol_table();

                edgecontrols_CurrentLoadIndex = edgecontrols_CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( edgecontrols_PreLoadIndex != edgecontrols_CurrentLoadIndex )
            {
                edgecontrols_PreLoadIndex = edgecontrols_CurrentLoadIndex;

                edgecontrols_CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( edgecontrols_CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(edgecontrols_loading_status, 100);
    else
    {
        // console.log("edgecontrols_loading_status() Stop.");

        setTimeout(reload_edgecontrol_device_info, 10000);
    }
}

function reload_edgecontrol_device_info()
{
    edgecontrols_PreLoadIndex = -1;
    edgecontrols_CurrentLoadIndex = 0;
    edgecontrols_loading_status();
}

function load_edgecontrol()
{
    if ( gl_activetab === 'activetab_edgecontrols' )
    {
    }
    else
    {
        setTimeout(load_edgecontrol, 1000);
        return;
    }

    var offset = (edgecontrols_glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var showonline = gl_showfilter_showonline ? 1:0;
    var showoffline = gl_showfilter_showoffline ? 1:0;

    gl_notOnLinelist_edgecontrols = [];
    gl_OnLinelist_edgecontrols = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadedgecontrol=1&s='+offset+'&count='+count+'&project_id='+project_id+'&showonline='+showonline+'&showoffline='+showoffline, function(data)
    {
        $.each(data, function(index, device)
        {
            // console.log("load_edgecontrol() index:"+index+" device:"+device);

            if (device.alive === '0')
                gl_notOnLinelist_edgecontrols.push(device);
            else
                gl_OnLinelist_edgecontrols.push(device);
        });
    })
    .success(function() {
        edgecontrols_CurrentLoadIndex = edgecontrols_CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function draw_edgecontrol_table()
{
    if( gl_editMode_count > 0)
    {
        console.log("draw_edgecontrol_table() gl_notOnLinelist_edgecontrols.length:"+gl_notOnLinelist_edgecontrols.length +" gl_editMode_count:"+gl_editMode_count);
        console.log("draw_edgecontrol_table() gl_OnLinelist_edgecontrols.length:"+gl_OnLinelist_edgecontrols.length +" gl_editMode_count:"+gl_editMode_count);
        return;
    }

    var record_table = document.getElementById('LOC_EDGE_CONTROL_NODE_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

    var uiindex = 0;
    if (gl_showfilter_showonline)
    {
        for (var i = 0; i < gl_OnLinelist_edgecontrols.length; ++i)
        {
            var device = gl_OnLinelist_edgecontrols[i];

            var deviceIndex = (edgecontrols_glCurrentPage - 1) * LOAD_PAGE_NUM;
            var row = edgecontrols_create_record_table_item(deviceIndex + i, device);
            record_table.appendChild(row);
        }
        uiindex = gl_OnLinelist_edgecontrols.length;
    }
    if (gl_showfilter_showoffline)
    {
        for (var i = 0; i < gl_notOnLinelist_edgecontrols.length; ++i)
        {
            var device = gl_notOnLinelist_edgecontrols[i];

            var deviceIndex = (edgecontrols_glCurrentPage - 1) * LOAD_PAGE_NUM + uiindex;
            var row = edgecontrols_create_record_table_item(deviceIndex + i, device);
            record_table.appendChild(row);
        }
    }
}

function edgecontrols_create_record_table_item(ui_index, device)
{
    if (device == null)
    {
        console.log("edgecontrols_create_record_table_item() device == null");
        return null;
    }

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
    tdName.setAttribute("id","edgecontrol_uiid_name_"+device.macaddress);
    row.appendChild(tdName);
    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = device.name;
    tdName.appendChild(SpanName);
    cellIndex++;

    // type
    var tdaccount = document.createElement('td');
    tdaccount.setAttribute("id","edgecontrol_uiid_type_"+device.macaddress);
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
    tdtitle.setAttribute("id","edgecontrol_uiid_macaddress_"+device.macaddress);
    row.appendChild(tdtitle);
    var Spanmacaddress  = document.createElement("span");
    if (device.macaddress.length > 0)
    {
        Spanmacaddress.setAttribute("class","label label-primary");
    }
    Spanmacaddress.textContent = device.macaddress;
    row.cells[cellIndex].appendChild(Spanmacaddress);
    cellIndex++;

    // LoRAMacaddress
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","edgecontrol_uiid_LoRAMacaddress_"+device.macaddress);
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
    tdtitle.setAttribute("id","edgecontrol_uiid_gwid_"+device.macaddress);
    row.appendChild(tdtitle);
    var Spangwid  = document.createElement("span");
    if (device.gwid.length > 0)
    {
        Spangwid.setAttribute("class","label label-success");
    }
    Spangwid.textContent = device.gwid;
    row.cells[cellIndex].appendChild(Spangwid);
    cellIndex++;

    // ip
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","edgecontrol_uiid_ip_"+device.macaddress);
    row.appendChild(tdtitle);
    var Spanip  = document.createElement("span");
    if (device.ip.length > 0)
    {
        Spanip.setAttribute("class","label label-warning");
    }
    Spanip.textContent = device.ip;
    row.cells[cellIndex].appendChild(Spanip);
    cellIndex++;

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

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    // Edit button
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id","edgecontrol_uiid_editbtn_"+device.macaddress);
    editBtn.setAttribute("title","Edit this device");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_edgecontrol_device(\""+device.alive+"\", \""+device.macaddress+"\",\""+ device.name +"\");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","edgecontrol_uiid_editimg_"+device.macaddress);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    // delete button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("style","display:none");
    deleteBtn.setAttribute("id","edgecontrol_uiid_deletebtn_"+device.macaddress);
    deleteBtn.setAttribute("title","Delete This Device");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","confirm_delete_edgecontrol_device(\""+device.macaddress+"\", \""+ device.name +"\");return false;");
    var imgDelete = document.createElement("i");
    imgDelete.setAttribute("class","fa fa-user-times");
    deleteBtn.appendChild(imgDelete);
    row.cells[cellIndex].appendChild(deleteBtn);
    cellIndex++;

    return row;
}

function edit_edgecontrol_device(alive, macaddress, name)
{
    console.log("edit_edgecontrol_device() macaddress:"+macaddress);

    gl_editMode_count += 1;

    create_edit_ui("edgecontrol_uiid_name", macaddress, name);

    var editBtn = document.getElementById("edgecontrol_uiid_editbtn_"+macaddress);
    editBtn.setAttribute("title","Save this Device");
    editBtn.setAttribute("onclick","save_edgecontrol_device(\""+alive+"\",\""+macaddress+"\");return false;");
    var imgEdit = document.getElementById("edgecontrol_uiid_editimg_"+macaddress);
    imgEdit.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("edgecontrol_uiid_deletebtn_"+macaddress);
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

function save_edgecontrol_device(alive, macaddress)
{
    var value_name = document.getElementById("edgecontrol_uiid_name_"+macaddress+"_edit").value;

    update_edgecontrol_device(alive, macaddress, value_name);
}

function update_edgecontrol_device(alive, macaddress, value_name)
{
    console.log("update_edgecontrol_device() macaddress:"+macaddress);
    console.log("update_edgecontrol_device() value_name:"+value_name);
    //console.log("update_edgecontrol_device() value_type:"+value_type);

    if ( value_name.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    gl_editMode_count = 0;

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_edgecontrol'] = 1;
    jsondata['alive'] = alive;
    jsondata['name'] = value_name;
    jsondata['macaddress'] = macaddress;

    var targetURL = gl_target_server + "/php/vilsnodes.php";

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

            setTimeout(reload_edgecontrol_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_edgecontrol_device(macaddress, name)
{
    if (confirm('Are you sure you want to delete this device:'+name+' ?'))
    {
        // delete it!
        delete_edgecontrol_device(macaddress);
    }
    else
    {
        // Do nothing!
    }
}

function delete_edgecontrol_device(macaddress)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_edgecontrol'] = 1;
    jsondata['macaddress'] = macaddress;

    gl_editMode_count = 0;

    var targetURL = gl_target_server + "/php/vilsnodes.php";

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
            setTimeout(reload_edgecontrol_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function edgecontrols_move_page_backward()
{
    var current_page_num_ui = document.getElementById('edgecontrols_id_current_page_num');

    if ( edgecontrols_glCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        edgecontrols_glCurrentPage--;
        current_page_num_ui.textContent = edgecontrols_glCurrentPage;

        reload_edgecontrol_device_info();
    }
}

function edgecontrols_move_page_forward()
{
    var current_page_num_ui = document.getElementById('edgecontrols_id_current_page_num');

    edgecontrols_glCurrentPage++;
    current_page_num_ui.textContent = edgecontrols_glCurrentPage;

    reload_edgecontrol_device_info();

}
