/*
 * Author: ikki Chung
 * Date: 2020.12.10
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var sensors_PreLoadIndex = -1;
var sensors_CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var sensors_glCurrentPage = 1;

var gl_notOnLinelist_sensors = [];
var gl_OnLinelist_sensors = [];
var gl_editMode_count = 0;

sensors_loading_status();

function sensors_loading_status()
{
    //console.log("sensors_loading_status() sensors_PreLoadIndex:" + sensors_PreLoadIndex +" sensors_CurrentLoadIndex:" + sensors_CurrentLoadIndex);
    switch(sensors_CurrentLoadIndex)
    {
        case 0:
            if ( sensors_PreLoadIndex != sensors_CurrentLoadIndex )
            {
                sensors_PreLoadIndex = sensors_CurrentLoadIndex;

                load_sensor();
            }
        break;
        case 1:
            if ( sensors_PreLoadIndex != sensors_CurrentLoadIndex )
            {
                sensors_PreLoadIndex = sensors_CurrentLoadIndex;

                draw_sensor_table();

                sensors_CurrentLoadIndex = sensors_CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( sensors_PreLoadIndex != sensors_CurrentLoadIndex )
            {
                sensors_PreLoadIndex = sensors_CurrentLoadIndex;

                sensors_CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( sensors_CurrentLoadIndex != LOAD_STATE_END )
        setTimeout(sensors_loading_status, 100);
    else
    {
        // console.log("sensors_loading_status() Stop.");

        setTimeout(reload_sensor_device_info, 10000);
    }
}

function reload_sensor_device_info()
{
    sensors_PreLoadIndex = -1;
    sensors_CurrentLoadIndex = 0;
    sensors_loading_status();
}

function load_sensor()
{
    if ( gl_activetab === 'activetab_sensors' )
    {
    }
    else
    {
        setTimeout(load_sensor, 1000);
        return;
    }

    var offset = (sensors_glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = 5; //LOAD_PAGE_NUM;
    var showonline = gl_showfilter_showonline ? 1:0;
    var showoffline = gl_showfilter_showoffline ? 1:0;

    gl_notOnLinelist_sensors = [];
    gl_OnLinelist_sensors = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadsensordevice=1&s='+offset+'&count='+count+'&project_id='+project_id+'&showonline='+showonline+'&showoffline='+showoffline, function(data)
    {
        console.log("load_sensor() data:"+data);

        $.each(data, function(index, device)
        {
            console.log("load_sensor() index:"+index+" device:"+device);

            if (device.alive === '0')
                gl_notOnLinelist_sensors.push(device);
            else
                gl_OnLinelist_sensors.push(device);
        });
    })
    .success(function() {
        console.log('aaa');
    })
    .error(function(err) {
        var aaa = '';
        switch (aaa) {
            case '':

                break;
            default:

        }
        // console.log(err);
    })
    .complete(function() {
        sensors_CurrentLoadIndex = sensors_CurrentLoadIndex + 1;
    });
}

function draw_sensor_table()
{
    if( gl_editMode_count > 0)
    {
        console.log("draw_sensor_table() gl_notOnLinelist_sensors.length:"+gl_notOnLinelist_sensors.length +" gl_editMode_count:"+gl_editMode_count);
        console.log("draw_sensor_table() gl_OnLinelist_sensors.length:"+gl_OnLinelist_sensors.length +" gl_editMode_count:"+gl_editMode_count);
        return;
    }

    var record_table = document.getElementById('SENSOR_DEVICE_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

    var uiindex = 0;
    if (gl_showfilter_showonline)
    {
        for (var i = 0; i < gl_OnLinelist_sensors.length; ++i)
        {
            var device = gl_OnLinelist_sensors[i];

            var deviceIndex = (sensors_glCurrentPage - 1) * LOAD_PAGE_NUM;
            var row = sensors_create_record_table_item(deviceIndex + i, device);
            record_table.appendChild(row);
        }
        uiindex = gl_OnLinelist_sensors.length;
    }
    if (gl_showfilter_showoffline)
    {
        for (var i = 0; i < gl_notOnLinelist_sensors.length; ++i)
        {
            var device = gl_notOnLinelist_sensors[i];

            var deviceIndex = (sensors_glCurrentPage - 1) * LOAD_PAGE_NUM + uiindex;
            var row = sensors_create_record_table_item(deviceIndex + i, device);
            record_table.appendChild(row);
        }
    }
}

function sensors_create_record_table_item(ui_index, device)
{
    if (device == null)
    {
        console.log("sensors_create_record_table_item() device == null");
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

    // ID
    var tdID = document.createElement('td');
    tdID.setAttribute("id","sensor_uiid_ID_"+device.id);
    row.appendChild(tdID);
    var SpanID = document.createElement("span");
    SpanID.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanID.textContent = device.id;
    tdID.appendChild(SpanID);
    cellIndex++;

    // 名稱
    var tdName = document.createElement('td');
    tdName.setAttribute("id","sensor_uiid_name_"+device.id);
    row.appendChild(tdName);
    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = device.name;
    tdName.appendChild(SpanName);
    cellIndex++;

    // 類別
    var tdType = document.createElement('td');
    tdType.setAttribute("id","sensor_uiid_type_"+device.id);
    row.appendChild(tdType);
    var SpanType = document.createElement("span");
    SpanType.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanType.textContent = device.type;
    tdType.appendChild(SpanType);
    cellIndex++;

    // verifykey
    var tdverifykey = document.createElement('td');
    row.appendChild(tdverifykey);
    var SpanVerifykey  = document.createElement("span");
    SpanVerifykey.textContent = device.verifykey;
    row.cells[cellIndex].appendChild(SpanVerifykey);
    cellIndex++;

    // 經緯度
    var tdLngLat = document.createElement('td');
    tdLngLat.setAttribute("id","sensor_uiid_LngLat_"+device.id);
    row.appendChild(tdLngLat);
    var SpanLngLat  = document.createElement("span");
    SpanLngLat.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanLngLat.textContent = device.Longitude + ' ' + device.Latitude;
    row.cells[cellIndex].appendChild(SpanLngLat);

    var Latitude = parseInt(device.Latitude +'0');
    var Longitude = parseInt(device.Longitude +'0');
    if (Latitude != 0 && Longitude != 0)
    {
        var gmapLink = document.createElement("a");
        gmapLink.setAttribute("href","javascript:;");
        gmapLink.setAttribute("onclick","open_gmap("+ device.Latitude +","+ device.Longitude+");return false;");
        var imggmapLink = document.createElement("i");
        imggmapLink.setAttribute("class","fa fa-map-marker");
        gmapLink.appendChild(imggmapLink);
        row.cells[cellIndex].appendChild(gmapLink);
    }
    cellIndex++;

    // Battery
    var tdBattery = document.createElement('td');
    tdBattery.setAttribute("id","sensor_uiid_Battery_"+device.id);
    row.appendChild(tdBattery);
    var SpanBattery  = document.createElement("span");
    SpanBattery.setAttribute("class","label label-primary");
    SpanBattery.textContent = device.Battery;
    row.cells[cellIndex].appendChild(SpanBattery);
    cellIndex++;

    // info
    var tdinfo = document.createElement('td');
    tdinfo.setAttribute("id","sensor_uiid_info_"+device.id);
    row.appendChild(tdinfo);

    var label_style = check_heartbeat_and_oxy(device.jsonvalue);

    var Spaninfo  = document.createElement("span");
    Spaninfo.setAttribute("class", label_style);
    Spaninfo.textContent = device.jsonvalue;
    row.cells[cellIndex].appendChild(Spaninfo);
    cellIndex++;

    // Version
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","sensor_uiid_Version_"+device.id);
    row.appendChild(tdtitle);
    var SpanVersion  = document.createElement("span");
    if (device.Version.length > 0)
    {
        SpanVersion.setAttribute("class","label label-warning");
    }
    SpanVersion.textContent = device.Version;
    row.cells[cellIndex].appendChild(SpanVersion);
    cellIndex++;

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    // Edit button
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id","sensor_uiid_editbtn_"+device.id);
    editBtn.setAttribute("title","Edit this device");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_sensor_device(\""+device.alive+"\", \""+device.id+"\",\""+ device.name +"\",\""+ device.Longitude +"\",\""+ device.Latitude +"\");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","sensor_uiid_editimg_"+device.id);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    // delete button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("style","display:none");
    deleteBtn.setAttribute("id","sensor_uiid_deletebtn_"+device.id);
    deleteBtn.setAttribute("title","Delete This Device");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","confirm_delete_sensor_device(\""+device.id+"\", \""+ device.name +"\");return false;");
    var imgDelete = document.createElement("i");
    imgDelete.setAttribute("class","fa fa-user-times");
    deleteBtn.appendChild(imgDelete);
    row.cells[cellIndex].appendChild(deleteBtn);
    cellIndex++;

    return row;
}
// 123
function check_heartbeat_and_oxy(tdinfo)
{
    var label_style = "label label-success";
    var style = 'success';
    var result = JSON.parse(tdinfo);

    if (result.HR != null && result.HR.length > 0)
    {
        // console.log("check_heartbeat_and_oxy() HR:"+result.HR);
        if ( result.HR == '255' ||  result.HR == '0')
        {
            //nothing
        }
        else
        if ( result.HR >= '120' && result.HR <= '180' )
        {
            label_style = "label label-warning";
            style = 'warning';
        }
        else
        if ( result.HR > '180' && result.HR <= '254' )
        {
            label_style = "label label-danger";
            style = 'danger';
        }
    }
    if ( style == 'danger' )
    {
        return label_style;
    }

    if (result.SPO2 != null && result.SPO2.length > 0)
    {
        // console.log("check_heartbeat_and_oxy() SPO2:"+result.SPO2);
        if ( result.SPO2 == '255' ||  result.SPO2 == '0')
        {
            //nothing
        }
        else
        if ( result.SPO2 >= '90' && result.SPO2 <= '95' )
        {
            label_style = "label label-warning";
            style = 'warning';
        }
        else
        if ( result.SPO2 < '90' )
        {
            label_style = "label label-danger";
            style = 'danger';
        }
    }

    return label_style;
}

function edit_sensor_device(alive, id, name, Longitude, Latitude)
{
    // console.log("edit_sensor_device() id:"+id+" name:"+name+" Longitude:"+Longitude+" Latitude:"+Latitude);

    gl_editMode_count += 1;

    create_edit_sensor_ui("sensor_uiid_name", id, name);
    create_edit_sensor_ui("sensor_uiid_LngLat", id, Longitude+','+Latitude);

    var editBtn = document.getElementById("sensor_uiid_editbtn_"+id);
    editBtn.setAttribute("title","Save this Device");
    editBtn.setAttribute("onclick","save_sensor_device(\""+alive+"\",\""+id+"\");return false;");
    var imgEdit = document.getElementById("sensor_uiid_editimg_"+id);
    imgEdit.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("sensor_uiid_deletebtn_"+id);
    deleteBtn.setAttribute("style","display:block");
}

function create_edit_sensor_ui(uiid, deviceid, text)
{
    // console.log("create_edit_sensor_ui() uiid:"+uiid+" deviceid:"+deviceid);

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

function save_sensor_device(alive, id)
{
    var value_name = document.getElementById("sensor_uiid_name_"+id+"_edit").value;
    var value_LngLat = document.getElementById("sensor_uiid_LngLat_"+id+"_edit").value;

    update_sensor_device(alive, id, value_name, value_LngLat);
}

function update_sensor_device(alive, id, value_name, value_LngLat)
{
    console.log("update_sensor_device() id:"+id);
    console.log("update_sensor_device() value_name:"+value_name);
    console.log("update_sensor_device() value_LngLat:"+value_LngLat);

    if ( value_name.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    var Longitude = '';
    var Latitude = '';
    if ( value_LngLat.length > 0 )
    {
        var LngLat = value_LngLat.split(",");
        if (LngLat.length <= 1 || LngLat.length > 2)
        {
            alert("請輸入正確經緯度格式, 如121.0445673,24.774457");
            return;
        }

        Longitude = LngLat[0];
        Latitude = LngLat[1];

        if (Longitude.length < 1 || Latitude.length < 1)
        {
            alert("請輸入正確經緯度格式, 如121.0445673,24.774457");
            return;
        }
    }

    gl_editMode_count = 0;

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_sensor'] = 1;
    jsondata['alive'] = alive;
    jsondata['name'] = value_name;
    jsondata['id'] = id;
    jsondata['lat'] = Latitude;
    jsondata['lng'] = Longitude;

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

            setTimeout(reload_sensor_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_sensor_device(id, name)
{
    if (confirm('Are you sure you want to delete this device:'+name+' ?'))
    {
        // delete it!
        delete_sensor_device(id);
    }
    else
    {
        // Do nothing!
    }
}

function delete_sensor_device(id)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_sensor'] = 1;
    jsondata['id'] = id;

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
            setTimeout(reload_sensor_device_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function verify_new_sensor()
{
    var verifykey = document.getElementById("sensor_verifykey").value;

    if ( verifykey.length != 8 )
    {
        alert("請輸入正確機器驗證碼");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['verify_new_sensor'] = 1;
    jsondata['verifykey'] = verifykey;

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
            else
            {
                var result = JSON.parse(response);
                // console.log('verify_error:' + result.verify_error);
                // console.log('verify_pass:' + result.verify_pass);

                var ui_verify_error = document.getElementById("sensor_verify_error");
                var ui_verify_pass = document.getElementById("sensor_verify_pass");
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

                    reload_sensor_device_info();
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


function sensors_move_page_backward()
{
    var current_page_num_ui = document.getElementById('sensors_id_current_page_num');

    if ( sensors_glCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        sensors_glCurrentPage--;
        current_page_num_ui.textContent = sensors_glCurrentPage;

        reload_sensor_device_info()();
    }
}

function sensors_move_page_forward()
{
    var current_page_num_ui = document.getElementById('sensors_id_current_page_num');

    sensors_glCurrentPage++;
    current_page_num_ui.textContent = sensors_glCurrentPage;

    reload_sensor_device_info()();

}
