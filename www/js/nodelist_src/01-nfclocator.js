/*
 * Author: ikki Chung
 * Date: 2020.05.13
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var nfclocators_PreLoadIndex = -1;
var nfclocators_CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var nfclocators_glCurrentPage = 1;

var gl_notOnLinelist_nfclocators = [];
var gl_OnLinelist_nfclocators = [];
var gl_editMode_count = 0;

nfclocators_loading_status();

function nfclocators_loading_status()
{
    // console.log("nfclocators_loading_status() nfclocators_PreLoadIndex:" + nfclocators_PreLoadIndex +" nfclocators_CurrentLoadIndex:" + nfclocators_CurrentLoadIndex);
    switch(nfclocators_CurrentLoadIndex)
    {
        case 0:
            // console.log("nfclocators_loading_status() nfclocators_PreLoadIndex:" + nfclocators_PreLoadIndex +" nfclocators_CurrentLoadIndex:" + nfclocators_CurrentLoadIndex);
            if ( nfclocators_PreLoadIndex != nfclocators_CurrentLoadIndex )
            {
                nfclocators_PreLoadIndex = nfclocators_CurrentLoadIndex;

                load_nfclocator();
            }
        break;
        case 1:
            // console.log("nfclocators_loading_status() nfclocators_PreLoadIndex:" + nfclocators_PreLoadIndex +" nfclocators_CurrentLoadIndex:" + nfclocators_CurrentLoadIndex);
            if ( nfclocators_PreLoadIndex != nfclocators_CurrentLoadIndex )
            {
                nfclocators_PreLoadIndex = nfclocators_CurrentLoadIndex;

                draw_nfclocator_table();

                nfclocators_CurrentLoadIndex = nfclocators_CurrentLoadIndex + 1;
            }
        break;
        case 2:
            // console.log("nfclocators_loading_status() nfclocators_PreLoadIndex:" + nfclocators_PreLoadIndex +" nfclocators_CurrentLoadIndex:" + nfclocators_CurrentLoadIndex);
            if ( nfclocators_PreLoadIndex != nfclocators_CurrentLoadIndex )
            {
                nfclocators_PreLoadIndex = nfclocators_CurrentLoadIndex;

                nfclocators_CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( nfclocators_CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(nfclocators_loading_status, 100);
    else
    {
        // console.log("nfclocators_loading_status() Stop.");

        setTimeout(reload_nfcdevice_device_info, 10000);
    }
}

function reload_nfcdevice_device_info()
{
    nfclocators_PreLoadIndex = -1;
    nfclocators_CurrentLoadIndex = 0;
    nfclocators_loading_status();
}

function load_nfclocator()
{
    // console.log("load_nfclocator() gl_activetab:"+gl_activetab);

    if ( gl_activetab === 'activetab_nfclocators' )
    {
    }
    else
    {
        setTimeout(load_nfclocator, 1000);
        return;
    }

    var offset = (nfclocators_glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
    var showonline = gl_showfilter_showonline ? 1:0;
    var showoffline = gl_showfilter_showoffline ? 1:0;

    gl_notOnLinelist_nfclocators = [];
    gl_OnLinelist_nfclocators = [];

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadnfclocator=1&s='+offset+'&count='+count+'&project_id='+project_id+'&showonline='+showonline+'&showoffline='+showoffline, function(data)
    {
        $.each(data, function(index, device)
        {
            if (device.alive === '0')
                gl_notOnLinelist_nfclocators.push(device);
            else
                gl_OnLinelist_nfclocators.push(device);
        });

    })
    .success(function() {
        nfclocators_CurrentLoadIndex = nfclocators_CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function draw_nfclocator_table()
{
    if( gl_editMode_count > 0)
    {
        console.log("draw_nfclocator_table() gl_OnLinelist_nfclocators.length:"+gl_OnLinelist_nfclocators.length +" gl_editMode_count:"+gl_editMode_count);
        console.log("draw_nfclocator_table() gl_notOnLinelist_nfclocators.length:"+gl_notOnLinelist_nfclocators.length +" gl_editMode_count:"+gl_editMode_count);
        return;
    }

    var record_table = document.getElementById('LOC_NFCLOCATOR_TABLE');

    while (record_table.rows.length >= 1)
        record_table.deleteRow(0);

    var uiindex = 0;
    if (gl_showfilter_showonline)
    {
         for (var i = 0; i < gl_OnLinelist_nfclocators.length; ++i)
         {
             var device = gl_OnLinelist_nfclocators[i];

             var nfclocatorIndex = (nfclocators_glCurrentPage - 1) * LOAD_PAGE_NUM;
             var row = nfclocators_create_record_table_item(nfclocatorIndex + i, device);
             record_table.appendChild(row);
         }
         uiindex = gl_OnLinelist_nfclocators.length;
     }
     if (gl_showfilter_showoffline)
     {
         for (var i = 0; i < gl_notOnLinelist_nfclocators.length; ++i)
         {
             var device = gl_notOnLinelist_nfclocators[i];

             var deviceIndex = (nfclocators_glCurrentPage - 1) * LOAD_PAGE_NUM + uiindex;
             var row = nfclocators_create_record_table_item(deviceIndex + i, device);
             record_table.appendChild(row);
         }
     }
}

function nfclocators_create_record_table_item(ui_index, device)
{
    if (device == null)
        return null;

    var row = document.createElement('tr');
    row.setAttribute("id", device.macaddress+"_TABLE_ROW");

    if ( device.alive === '0' )
        row.setAttribute("bgcolor","#a3c2c2");
    else
        row.setAttribute("bgcolor","#FFFFFF");

    // if ((ui_index % 2) == 1)
    //     row.setAttribute("bgcolor","#ebebe0");

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
    tdName.setAttribute("id","uiid_name_"+device.macaddress);
    row.appendChild(tdName);
    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = device.name;
    tdName.appendChild(SpanName);
    cellIndex++;

    // type
    var tdaccount = document.createElement('td');
    tdaccount.setAttribute("id","uiid_type_"+device.macaddress);
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
    tdtitle.setAttribute("id","uiid_macaddress_"+device.macaddress);
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
    tdtitle.setAttribute("id","uiid_LoRAMacaddress_"+device.macaddress);
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
    tdtitle.setAttribute("id","uiid_gwid_"+device.macaddress);
    row.appendChild(tdtitle);
    var Spangwid  = document.createElement("span");
    if (device.gwid.length > 0)
    {
        Spangwid.setAttribute("class","label label-success");
    }
    Spangwid.textContent = device.gwid;
    row.cells[cellIndex].appendChild(Spangwid);
    cellIndex++;

    // version
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","uiid_version_"+device.macaddress);
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
    editBtn.setAttribute("id","uiid_editbtn_"+device.macaddress);
    editBtn.setAttribute("title","Edit this device");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_nfclocator_device(\""+device.alive+"\", \""+device.macaddress+"\",\""+ device.name +"\", \""+ device.type +"\", \""+ device.LoRAMacaddress +"\", \""+ device.gwid +"\");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","uiid_editimg_"+device.macaddress);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    // delete button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("style","display:none");
    deleteBtn.setAttribute("id","uiid_deletebtn_"+device.macaddress);
    deleteBtn.setAttribute("title","Delete This Device");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","confirm_delete_nfclocator_device(\""+device.macaddress+"\", \""+ device.name +"\", \""+ device.type +"\");return false;");
    var imgDelete = document.createElement("i");
    imgDelete.setAttribute("class","fa fa-user-times");
    deleteBtn.appendChild(imgDelete);
    row.cells[cellIndex].appendChild(deleteBtn);
    cellIndex++;


    return row;
}

function edit_nfclocator_device(alive, macaddress, name, type, LoRAMacaddress, gwid)
{
    console.log("edit_nfclocator_device() macaddress:"+macaddress);

    gl_editMode_count += 1;

    //create_edit_ui("uiid_macaddress", macaddress, macaddress);
    create_edit_ui("uiid_name", macaddress, name);
    // create_edit_ui("uiid_type", macaddress, type);
    if (groupid != 5)
    {
        create_edit_ui("uiid_LoRAMacaddress", macaddress, LoRAMacaddress);
        create_edit_ui("uiid_gwid", macaddress, gwid);
    }

    var editBtn = document.getElementById("uiid_editbtn_"+macaddress);
    editBtn.setAttribute("title","Save this Device");
    editBtn.setAttribute("onclick","save_nfclocator_device(\""+alive+"\",\""+macaddress+"\",\""+LoRAMacaddress+"\",\""+gwid+"\");return false;");
    var imgEdit = document.getElementById("uiid_editimg_"+macaddress);
    imgEdit.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("uiid_deletebtn_"+macaddress);
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

function save_nfclocator_device(alive, macaddress, LoRAMacaddress, gwid)
{
    //var value_macaddress = document.getElementById("uiid_macaddress_"+macaddress+"_edit").value;
    var value_name = document.getElementById("uiid_name_"+macaddress+"_edit").value;
    //var value_type = document.getElementById("uiid_type_"+macaddress+"_edit").value;

    if (groupid != 5)
    {
        var value_LoRAMacaddress = document.getElementById("uiid_LoRAMacaddress_"+macaddress+"_edit").value;
        var value_gwid = document.getElementById("uiid_gwid_"+macaddress+"_edit").value;
        update_nfclocator_device(alive, macaddress, value_name, value_LoRAMacaddress, value_gwid);
    }
    else
    {
        update_nfclocator_device(alive, macaddress, value_name, LoRAMacaddress, gwid);
    }
}

function update_nfclocator_device(alive, macaddress, value_name, value_LoRAMacaddress, value_gwid)
{
    console.log("update_nfclocator_device() macaddress:"+macaddress);
    console.log("update_nfclocator_device() value_name:"+value_name);
    //console.log("update_nfclocator_device() value_type:"+value_type);

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
    jsondata['update_nfclocator'] = 1;
    jsondata['alive'] = alive;
    jsondata['macaddress'] = macaddress;
    jsondata['name'] = value_name;
    //jsondata['type'] = value_type;
    jsondata['gwid'] = value_gwid;
    jsondata['LoRAMacaddress'] = value_LoRAMacaddress;

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

            setTimeout(reload_nfcdevice_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_nfclocator_device(macaddress, name, type)
{
    if (confirm('Are you sure you want to delete this device:'+name+' '+type+'   ?'))
    {
        // delete it!
        delete_nfclocator_device(macaddress);
    }
    else
    {
        // Do nothing!
    }
}

function delete_nfclocator_device(macaddress)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_nfclocator'] = 1;
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
            setTimeout(reload_nfcdevice_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function create_empty_nfclocator_record_table_item(ui_index)
{
    var empty_record_table_item = document.getElementById('empty_nfclocator_record_table_item');
    if ( empty_record_table_item != null )
        return;

    var record_table = document.getElementById('LOC_NFCLOCATOR_TABLE');

    var row = document.createElement('tr');
    row.setAttribute("id", "empty_nfclocator_record_table_item");

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

    // verifykey
    row.appendChild(document.createElement('td'));
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
    Button_save.setAttribute("onclick","save_new_nfclocator_device();");
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

function save_new_nfclocator_device()
{
    var input_name = document.getElementById("input_name").value;
    var input_type = document.getElementById("input_type").value;
    var input_macaddress = document.getElementById("input_macaddress").value;
    var input_LoRAMacaddress = document.getElementById("input_LoRAMacaddress").value;
    var input_gwid = document.getElementById("input_gwid").value;

    if ( input_name.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    if ( input_macaddress.length == 0 )
    {
        alert("請輸入硬體位址");
        return;
    }

    gl_editMode_count = 0;

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_nfclocator'] = 1;
    jsondata['macaddress'] = input_macaddress;
    jsondata['name'] = input_name;
    jsondata['type'] = input_type;
    jsondata['gwid'] = input_gwid;
    jsondata['LoRAMacaddress'] = input_LoRAMacaddress;

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
            setTimeout(reload_nfcdevice_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function verify_new_nfclocator()
{
    var verifykey = document.getElementById("nfclocator_verifykey").value;

    if ( verifykey.length != 8 )
    {
        alert("請輸入正確機器驗證碼");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['verify_new_nfclocator'] = 1;
    jsondata['verifykey'] = verifykey;

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
            else
            {
                var result = JSON.parse(response);
                // console.log('verify_error:' + result.verify_error);
                // console.log('verify_pass:' + result.verify_pass);

                var ui_verify_error = document.getElementById("nfclocator_verify_error");
                var ui_verify_pass = document.getElementById("nfclocator_verify_pass");
                if (result.verify_error.length > 0)
                {
                    ui_verify_error.textContent = result.verify_error;

                    ui_verify_pass.setAttribute("style","display:none;");
                    ui_verify_error.setAttribute("style","display:block;");
                }
                else
                if (result.verify_pass.length > 0)
                {
                    ui_verify_pass.textContent = result.verify_pass;

                    ui_verify_pass.setAttribute("style","display:block;");
                    ui_verify_error.setAttribute("style","display:none;");

                    reload_nfcdevice_device_info();
                }
            }
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function getNFCDeviceName(deviceid)
{
    var retDeviceName = deviceid;
    for (var i = 0; i < gl_OnLinelist_nfclocators.length; ++i)
    {
        var device = gl_OnLinelist_nfclocators[i];

        if ( device.deviceid === deviceid )
        {
            retDeviceName = device.name;
            return retDeviceName;
        }
    }

    for (var i = 0; i < gl_notOnLinelist_nfclocators.length; ++i)
    {
        var device = gl_notOnLinelist_nfclocators[i];

        if ( device.deviceid === deviceid )
        {
            retDeviceName = device.name;
            return retDeviceName;
        }
    }
    return retDeviceName;
}

$("#createnewnfclocatorbutton").click(function()
{
    var total_count = gl_OnLinelist_nfclocators.length + gl_notOnLinelist_nfclocators.length;
    create_empty_nfclocator_record_table_item(total_count + 1);

    return false;
});


function nfclocators_move_page_backward()
{
    var current_page_num_ui = document.getElementById('nfclocators_id_current_page_num');

    if ( nfclocators_glCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        nfclocators_glCurrentPage--;
        current_page_num_ui.textContent = nfclocators_glCurrentPage;

        reload_nfcdevice_device_info();
    }
}

function nfclocators_move_page_forward()
{
    var current_page_num_ui = document.getElementById('nfclocators_id_current_page_num');

    nfclocators_glCurrentPage++;
    current_page_num_ui.textContent = nfclocators_glCurrentPage;

    reload_nfcdevice_device_info();

}
