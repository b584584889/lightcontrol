/*
 * Author: ikki Chung
 * Date: 2020.03.19
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


//
// globa value
//
var glMaintenanceRecords = [];
var gl_record_load_index = 0;
var LOAD_DEVICE_NUM = 20;

//
// start load
//
load_maintenancerecords();

function load_maintenancerecords()
{
    glMaintenanceRecords = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loadmaintenancerecord=1&s='+gl_record_load_index+'&count='+LOAD_DEVICE_NUM, function(data)
    {
        var index = 0;
        $.each(data, function(itemindex, record)
        {
            glMaintenanceRecords.push(record);
            index++;
        });

        draw_table();
    });
}

function draw_table()
{
    //console.log("draw_table() glTagLocation.length:"+glTagLocation.length);

    var record_table = document.getElementById('MAINTENANCE_TABLE_BODY');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

     console.log("draw_table() glMaintenanceRecords.length:"+glMaintenanceRecords.length);

    for (var i = 0; i < glMaintenanceRecords.length; ++i)
    {
        var record = glMaintenanceRecords[i];

        var row = create_record_table_item(i, record);
        record_table.appendChild(row);
    }
}

function create_record_table_item(ui_index, record)
{
    if (record == null)
        return null;

    var row = document.createElement('tr');
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+(ui_index + 1)));
    cellIndex++;

    // 儀器名稱
    var tdName = document.createElement('td');
    tdName.setAttribute("style","height: 90px;");
    row.appendChild(tdName);
    var Spanmac = document.createElement("span");
    //Spanmac.setAttribute("class","badge bg-green-active");
    Spanmac.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    Spanmac.textContent = record.name;
    row.cells[cellIndex].appendChild(Spanmac);
    cellIndex++;

    // 廠牌型別
    var tdType = document.createElement('td');
    tdName.setAttribute("style","height: 90px;");
    row.appendChild(tdType);
    var SpanType = document.createElement("span");
    //Spanmac.setAttribute("class","badge bg-green-active");
    SpanType.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    // if (areatype == '0')
    //     SpanType.setAttribute("class","badge bg-yellow-active");
    // if (areatype == '1')
    //     SpanType.setAttribute("class","badge bg-fuchsia");
    // if (areatype == '2')
    //     SpanType.setAttribute("class","badge bg-green-active");
    // if (areatype == '3')
    //     SpanType.setAttribute("class","badge bg-purple");
    // if (areatype == '4')
    //     SpanType.setAttribute("class","badge bg-teal-active");
    // if (areatype == '5')
    //     SpanType.setAttribute("class","badge bg-green-active");
    // if (areatype == '6')
    //     SpanType.setAttribute("class","badge bg-purple");
    SpanType.textContent = record.type;
    row.cells[cellIndex].appendChild(SpanType);
    cellIndex++;

    // 保管單位
    row.appendChild(document.createElement('td'));
    var Spanunit = document.createElement("span");
    Spanunit.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    //Spanunit.setAttribute("class","badge bg-blue");
    Spanunit.textContent = record.unit;
    row.cells[cellIndex].appendChild(Spanunit);
    cellIndex++;

    // 管理人員
    row.appendChild(document.createElement('td'));
    var Spanmanager = document.createElement("span");
    Spanmanager.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    //Spanmanager.setAttribute("class","badge bg-blue");
    Spanmanager.textContent = record.manager;
    row.cells[cellIndex].appendChild(Spanmanager);
    cellIndex++;

    // 資產年限
    row.appendChild(document.createElement('td'));
    var Spanassetlife = document.createElement("span");
    Spanassetlife.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    //Spanassetlife.setAttribute("class","badge bg-blue");
    Spanassetlife.textContent = record.assetlife;
    row.cells[cellIndex].appendChild(Spanassetlife);
    cellIndex++;

    // 保養校正(日期通知)
    row.appendChild(document.createElement('td'));
    var Spanmaintenancedate = document.createElement("span");
    Spanmaintenancedate.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    //Spanmaintenancedate.setAttribute("class","badge bg-blue");
    Spanmaintenancedate.textContent = record.maintenancedate;
    row.cells[cellIndex].appendChild(Spanmaintenancedate);
    cellIndex++;

    // 保養校正(日期通知)
    row.appendChild(document.createElement('td'));
    var Spanstatus = document.createElement("span");
    Spanstatus.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    //Spanstatus.setAttribute("class","badge bg-blue");
    Spanstatus.textContent = record.status;
    row.cells[cellIndex].appendChild(Spanstatus);
    cellIndex++;

    // datetime
    row.appendChild(document.createElement('td'));
    var Spandatetime = document.createElement("span");
    Spandatetime.setAttribute("class","badge bg-blue");
    Spandatetime.textContent = record.datetime;
    row.cells[cellIndex].appendChild(Spandatetime);
    cellIndex++;

    return row;
}

function create_empty_record_table_item(ui_index)
{
    var record_table = document.getElementById('MAINTENANCE_TABLE_BODY');

    var row = document.createElement('tr');
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // 儀器名稱
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_name"));
    cellIndex++;

    // 廠牌型別
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_type"));
    cellIndex++;

    // 保管單位
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_unit"));
    cellIndex++;

    // 管理人員
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_manager"));
    cellIndex++;

    // 資產年限
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_assetlife"));
    cellIndex++;

    // 保養校正(日期通知)
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_maintenancedate"));
    cellIndex++;

    // 儀器設備狀況(良好、維修校正、故障)
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_status"));
    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_record();");
    Button_save.textContent = "儲存";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;

    record_table.appendChild(row);
}

function create_input_text(ui_id, text)
{
    // var input_text = document.createElement("input");
    // input_text.setAttribute("type","text");
    // input_text.setAttribute("id",ui_id);
    // input_text.textContent = text;

    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.textContent = text;

    return input_text;
}

$("#createnewmaintenancebutton").click(function()
{
    create_empty_record_table_item(glMaintenanceRecords.length + 1);

    return false;
});

function save_new_record()
{
    var input_name = document.getElementById("input_name").value;
    var input_type = document.getElementById("input_type").value;
    var input_unit = document.getElementById("input_unit").value;
    var input_manager = document.getElementById("input_manager").value;
    var input_assetlife = document.getElementById("input_assetlife").value;
    var input_maintenancedate = document.getElementById("input_maintenancedate").value;
    var input_status = document.getElementById("input_status").value;

    console.log("save_new_record() input_name:"+input_name);

    if ( input_name.length == 0 )
    {
        alert("請輸入儀器名稱");
        return;
    }

    if ( input_maintenancedate.length == 0 )
    {
        alert("請輸入保養校正(日期通知)");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_maintenance'] = 1;
    jsondata['name'] = input_name;
    jsondata['type'] = input_type;
    jsondata['unit'] = input_unit;
    jsondata['manager'] = input_manager;
    jsondata['assetlife'] = input_assetlife;
    jsondata['maintenancedate'] = input_maintenancedate;
    jsondata['status'] = input_status;

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

            setTimeout(load_maintenancerecords, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

}

function open_maintenance_page()
{
    var win = window.open('/index.php/vilsmaintenance/'+project_id, '_blank');
    win.focus();
}
