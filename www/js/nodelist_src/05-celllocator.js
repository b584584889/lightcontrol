/*
 * Author: ikki Chung
 * Date: 2020.09.08
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var celllocators_PreLoadIndex = -1;
var celllocators_CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var celllocators_glCurrentPage = 1;

var gl_notOnLinelist_celllocators = [];
var gl_OnLinelist_celllocators = [];
var gl_editMode_count = 0;

celllocators_loading_status();

function celllocators_loading_status()
{
    //console.log("celllocators_loading_status() celllocators_PreLoadIndex:" + celllocators_PreLoadIndex +" celllocators_CurrentLoadIndex:" + celllocators_CurrentLoadIndex);
    switch(celllocators_CurrentLoadIndex)
    {
        case 0:
            if ( celllocators_PreLoadIndex != celllocators_CurrentLoadIndex )
            {
                celllocators_PreLoadIndex = celllocators_CurrentLoadIndex;

                load_celllocator();
            }
        break;
        case 1:
            if ( celllocators_PreLoadIndex != celllocators_CurrentLoadIndex )
            {
                celllocators_PreLoadIndex = celllocators_CurrentLoadIndex;

                draw_celllocator_table();

                celllocators_CurrentLoadIndex = celllocators_CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( celllocators_PreLoadIndex != celllocators_CurrentLoadIndex )
            {
                celllocators_PreLoadIndex = celllocators_CurrentLoadIndex;

                celllocators_CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( celllocators_CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(celllocators_loading_status, 100);
    else
    {
        // console.log("celllocators_loading_status() Stop.");

        setTimeout(reload_celllocator_device_info, 10000);
    }
}

function reload_celllocator_device_info()
{
    celllocators_PreLoadIndex = -1;
    celllocators_CurrentLoadIndex = 0;
    celllocators_loading_status();
}

function load_celllocator()
{
    if ( gl_activetab === 'activetab_celllocators' )
    {
    }
    else
    {
        setTimeout(load_celllocator, 1000);
        return;
    }

    var offset = (celllocators_glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var showonline = gl_showfilter_showonline ? 1:0;
    var showoffline = gl_showfilter_showoffline ? 1:0;

    gl_notOnLinelist_celllocators = [];
    gl_OnLinelist_celllocators = [];

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadcelllocator=1&s='+offset+'&count='+count+'&project_id='+project_id+'&showonline='+showonline+'&showoffline='+showoffline, function(data)
    {
        $.each(data, function(index, device)
        {
            // console.log("load_celllocator() index:"+index+" device:"+device);
            if (device.alive === '0')
                gl_notOnLinelist_celllocators.push(device);
            else
                gl_OnLinelist_celllocators.push(device);
        });
    })
    .success(function() {
        celllocators_CurrentLoadIndex = celllocators_CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function draw_celllocator_table()
{
    if( gl_editMode_count > 0)
    {
        console.log("draw_celllocator_table() gl_notOnLinelist_celllocators.length:"+gl_notOnLinelist_celllocators.length +" gl_editMode_count:"+gl_editMode_count);
        console.log("draw_celllocator_table() gl_OnLinelist_celllocators.length:"+gl_OnLinelist_celllocators.length +" gl_editMode_count:"+gl_editMode_count);
        return;
    }

    var record_table = document.getElementById('LOC_CELL_LOCATOR_NODE_TABLE');

    while (record_table.rows.length >= 1)
        record_table.deleteRow(0);

    var uiindex = 0;
    if (gl_showfilter_showonline)
    {
        for (var i = 0; i < gl_OnLinelist_celllocators.length; ++i)
        {
            var device = gl_OnLinelist_celllocators[i];

            var deviceIndex = (celllocators_glCurrentPage - 1) * LOAD_PAGE_NUM;
            var row = celllocators_create_record_table_item(deviceIndex + i, device);
            record_table.appendChild(row);
        }
        uiindex = gl_OnLinelist_celllocators.length;
    }
    if (gl_showfilter_showoffline)
    {
        for (var i = 0; i < gl_notOnLinelist_celllocators.length; ++i)
        {
            var device = gl_notOnLinelist_celllocators[i];

            var deviceIndex = (celllocators_glCurrentPage - 1) * LOAD_PAGE_NUM + uiindex;
            var row = celllocators_create_record_table_item(deviceIndex + i, device);
            record_table.appendChild(row);
        }
    }
}

function celllocators_create_record_table_item(ui_index, device)
{
    if (device == null)
    {
        console.log("celllocators_create_record_table_item() device == null");
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
    tdName.setAttribute("id","celllocator_uiid_name_"+device.macaddress);
    row.appendChild(tdName);
    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = device.name;
    tdName.appendChild(SpanName);
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
    tdtitle.setAttribute("id","celllocator_uiid_macaddress_"+device.macaddress);
    row.appendChild(tdtitle);
    var Spanmacaddress  = document.createElement("span");
    if (device.macaddress.length > 0)
    {
        Spanmacaddress.setAttribute("class","label label-primary");
    }
    Spanmacaddress.textContent = device.macaddress;
    row.cells[cellIndex].appendChild(Spanmacaddress);
    cellIndex++;

    // version
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","celllocator_uiid_version_"+device.macaddress);
    row.appendChild(tdtitle);
    var Spanversion  = document.createElement("span");
    if (device.version.length > 0)
    {
        Spanversion.setAttribute("class","label label-warning");
    }
    Spanversion.textContent = device.version;
    row.cells[cellIndex].appendChild(Spanversion);
    cellIndex++;

    row.appendChild(document.createElement('td'));
    // Info
    var info = '';

    if(device.version.length > 0)
        info += ' V'+device.version;

    if (device.panid.length > 0) // locator
        info += ' panid:' + device.panid;

    if (device.mode.length > 0) // Opertaion_Mode
        info += ' mode:' + device.mode;

    if ( device.gwid.length > 0)
        info += ' gwid:' + device.gwid;

    info += ' ';

    if ( device.Longitude.length > 1 && device.Latitude.length > 1)
    {
        info += ' 經緯度('+device.Longitude+','+device.Latitude+')';
    }

    row.cells[cellIndex].appendChild(document.createTextNode(info));

    var LatitudeValue = parseInt(device.Latitude);
    var LongitudeValue = parseInt(device.Longitude);
    if (LatitudeValue != 0 && LongitudeValue != 0)
    {
        var btnQrcode = document.createElement("button");
        var gmapLink = document.createElement("a");
        gmapLink.setAttribute("href","javascript:;");
        gmapLink.setAttribute("onclick","open_gmap("+ device.Latitude +","+ device.Longitude+");return false;");
        var imggmapLink = document.createElement("i");
        imggmapLink.setAttribute("class","fa fa-map-marker");
        gmapLink.appendChild(imggmapLink);
        btnQrcode.appendChild(gmapLink);
        row.cells[cellIndex].appendChild(btnQrcode);
    }

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    // Edit button
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id","celllocator_uiid_editbtn_"+device.macaddress);
    editBtn.setAttribute("title","Edit this device");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_celllocator_device(\""+device.alive+"\", \""+device.macaddress+"\",\""+ device.name +"\");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","celllocator_uiid_editimg_"+device.macaddress);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    // delete button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("style","display:none");
    deleteBtn.setAttribute("id","celllocator_uiid_deletebtn_"+device.macaddress);
    deleteBtn.setAttribute("title","Delete This Device");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","confirm_delete_celllocator_device(\""+device.macaddress+"\", \""+ device.name +"\");return false;");
    var imgDelete = document.createElement("i");
    imgDelete.setAttribute("class","fa fa-user-times");
    deleteBtn.appendChild(imgDelete);
    row.cells[cellIndex].appendChild(deleteBtn);
    cellIndex++;


    return row;
}

function edit_celllocator_device(alive, macaddress, name)
{
    console.log("edit_celllocator_device() macaddress:"+macaddress);

    gl_editMode_count += 1;

    create_edit_ui("celllocator_uiid_name", macaddress, name);

    var editBtn = document.getElementById("celllocator_uiid_editbtn_"+macaddress);
    editBtn.setAttribute("title","Save this Device");
    editBtn.setAttribute("onclick","save_celllocator_device(\""+alive+"\",\""+macaddress+"\");return false;");
    var imgEdit = document.getElementById("celllocator_uiid_editimg_"+macaddress);
    imgEdit.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("celllocator_uiid_deletebtn_"+macaddress);
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

function save_celllocator_device(alive, macaddress)
{
    var value_name = document.getElementById("celllocator_uiid_name_"+macaddress+"_edit").value;

    update_celllocator_device(alive, macaddress, value_name);
}

function update_celllocator_device(alive, macaddress, value_name)
{
    console.log("update_celllocator_device() macaddress:"+macaddress);
    console.log("update_celllocator_device() value_name:"+value_name);
    //console.log("update_celllocator_device() value_type:"+value_type);

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
    jsondata['update_celllocator'] = 1;
    jsondata['alive'] = alive;
    jsondata['macaddress'] = macaddress;
    jsondata['name'] = value_name;

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

            setTimeout(reload_celllocator_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_celllocator_device(macaddress, name)
{
    if (confirm('Are you sure you want to delete this device:'+name+' ?'))
    {
        // delete it!
        delete_celllocator_device(macaddress);
    }
    else
    {
        // Do nothing!
    }
}

function delete_celllocator_device(macaddress)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_celllocator'] = 1;
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
            setTimeout(reload_celllocator_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}


function celllocators_move_page_backward()
{
    var current_page_num_ui = document.getElementById('celllocators_id_current_page_num');

    if ( celllocators_glCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        celllocators_glCurrentPage--;
        current_page_num_ui.textContent = celllocators_glCurrentPage;

        reload_celllocator_device_info();
    }
}

function celllocators_move_page_forward()
{
    var current_page_num_ui = document.getElementById('celllocators_id_current_page_num');

    celllocators_glCurrentPage++;
    current_page_num_ui.textContent = celllocators_glCurrentPage;

    reload_celllocator_device_info();

}
