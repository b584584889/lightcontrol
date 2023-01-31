/*
 * Author: ikki Chung
 * Date: 2022 02 20
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var glCurrentPage = 1;
var LOAD_PAGE_NUM = 20;

var gl_flyevent_List = [];

var pre_input_sensorid_value = '';

// start loading
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

                load_flyevent();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_flyevent_records();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;


                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

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

function load_flyevent()
{
    gl_flyevent_List = [];

    var loadType = "";
    if (document.getElementById('ArrivalCheckBtn').checked)
    {
        loadType = "Arrival";
    }
    if (document.getElementById('DepartureCheckBtn').checked)
    {
        loadType = "Departure";
    }

    var input_sensorid_value = document.getElementById("search_sensorid_text").value;

    pre_input_sensorid_value = input_sensorid_value;

    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    var targetURL = gl_target_server + "/api/sensorinfo.php";
    var querytext = 'loadevent=1&key=sotA6cY54U&type='+loadType+"&offset="+offset+"&count="+count;

    if(input_sensorid_value.length > 0)
    {
        querytext = querytext + '&sensorid=' + input_sensorid_value;
    }

    var p = $.getJSON(targetURL, querytext, function(data)
    {
        $.each(data, function(index, flyevent)
        {
            gl_flyevent_List.push(flyevent);
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function show_flyevent_records()
{
    var table_body_ui = document.getElementById('table_content');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    for(var index=0;index<gl_flyevent_List.length;index++)
    {
        flyevent(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, gl_flyevent_List[index]);
    }
}

function flyevent(table_body_ui, ui_index, record)
{
    var cellIndex = 0;

    var row = document.createElement('tr');
    if ((ui_index % 2) == 0)
        row.setAttribute("bgcolor","#ebebe0");
    table_body_ui.appendChild(row);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // eventid
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.eventid);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // sensorid
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.sensorid);
    colDiv.setAttribute("id","sensorid_"+record.eventid);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // type
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.type);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // DATE
    row.appendChild(document.createElement('td'));
    var colDivDATE = createColDiv(record.FDATE);
    row.cells[cellIndex].appendChild(colDivDATE);
    cellIndex++;

    // STA, STD
    row.appendChild(document.createElement('td'));
    var colDivST = createColDiv(getFlyInfoValue(record.type, record.STA, record.STD));
    row.cells[cellIndex].appendChild(colDivST);
    cellIndex++;

    // amhsETA, amhsETD
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(getFlyInfoValue(record.type, record.amhsETA, record.amhsETD));
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // ritETA, ritETD
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(getFlyInfoValue(record.type, record.ritETA, record.ritETD));
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // ATA, ATD
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(getFlyInfoValue(record.type, record.ATA, record.ATD));
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // airLineCode
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.airLineCode);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // airLineNum
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.airLineNum);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // DepartureAirport, ArrivalAirport
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(getFlyInfoValue(record.type, record.DepartureAirport, record.ArrivalAirport));
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // airPlaneType
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.airPlaneType);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // triggerTime
    row.appendChild(document.createElement('td'));
    var colDivtriggerTime = document.createElement("span");
    // colDivtriggerTime.setAttribute("class","label label-info");
    colDivtriggerTime.setAttribute("id","triggerTime_"+record.eventid);
    colDivtriggerTime.textContent = record.triggerTime;
    row.cells[cellIndex].appendChild(colDivtriggerTime);
    cellIndex++;

    // datetime
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.datetime);
    colDiv.setAttribute("id","datetime_"+record.eventid);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // Edit button
    row.appendChild(document.createElement('td'));
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id","uiid_editbtn_"+record.eventid);
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_record(\""+record.eventid+"\");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","uiid_editimg_"+record.eventid);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);
    cellIndex++;
}

function edit_record(eventid)
{
    var flyevent = getFlyEvent(eventid);
    if (flyevent == null)
        return;

    create_edit_ui("sensorid", eventid, flyevent.sensorid);
    create_edit_ui("datetime", eventid, flyevent.datetime);
    create_edit_ui("triggerTime", eventid, flyevent.triggerTime);

    var editBtn = document.getElementById("uiid_editbtn_"+eventid);
    editBtn.setAttribute("onclick","save_record(\""+eventid+"\");return false;");
    var imgEdit = document.getElementById("uiid_editimg_"+eventid);
    imgEdit.setAttribute("class","fa fa-save");
}

function create_edit_ui(uiid, eventid, text)
{
    var uiid_name = clearChildView(uiid+"_"+eventid);

    var ui_new_input = document.createElement("div");
    var new_input = create_input_text(uiid+"_"+eventid+"_edit", text);
    ui_new_input.appendChild(new_input);

    uiid_name.appendChild(ui_new_input);

    return uiid_name;
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

function getFlyEvent(eventid)
{
    var retflyevent = null;
    for(var i=0;i<gl_flyevent_List.length;i++)
    {
        var flyevent = gl_flyevent_List[i];

        if (flyevent.eventid === eventid)
        {
            retflyevent = flyevent;
            break;
        }
    }
    return retflyevent;
}

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("input");
    input_text.setAttribute("type","text");
    input_text.setAttribute("id",ui_id);
    input_text.value = text;

    return input_text;
}

function createColDiv(textValue)
{
    var colDiv = document.createElement('div');
    colDiv.appendChild(document.createTextNode(textValue));
    return colDiv;
}

function getFlyInfoValue(type, A, D)
{
    if (type === "Arrival")
        return A;
    else
        return D;
}

var gl_search_sensorid_start = 0;
function search_sensorid_text_change()
{
    var input_value = document.getElementById("search_sensorid_text").value;

    if ( input_value === pre_input_sensorid_value )
    {
        console.log("search_sensorid_text_change() input_value === pre_input_sensorid_value, input_value:"+input_value);
        return;
    }

    gl_search_sensorid_start = 0;

    glCurrentPage = 1;
    reload_page();

    return false;
}

function check_search_sensorid_text(e)
{
    if ( gl_search_sensorid_start == 0 )
    {
        gl_search_sensorid_start = new Date();
    }

    var cur_time = new Date();
    var seconds = (cur_time.getTime() - gl_search_sensorid_start.getTime()) / 1000;

    // console.log("check_search_text_change() seconds:"+seconds);

    if ( CurrentLoadIndex != LOAD_STATE_END )
    {
        console.log("check_search_sensorid_text() CurrentLoadIndex != LOAD_STATE_END");

        setTimeout(search_sensorid_text_change, 500);
        return;
    }

    if ( seconds >= 1 )
    {
        gl_search_sensorid_start = 0;
        search_sensorid_text_change();
    }
    else
    {
        setTimeout(search_sensorid_text_change, 500);
    }
}

function save_record(eventid)
{
    var ui_sensorid = document.getElementById("sensorid_"+eventid+"_edit");
    var ui_datetime = document.getElementById("datetime_"+eventid+"_edit");
    var ui_triggerTime = document.getElementById("triggerTime_"+eventid+"_edit");

    var input_sensorid = ui_sensorid.value;
    var input_datetime = ui_datetime.value;
    var input_triggerTime = ui_triggerTime.value;

    console.log("save_record() input_sensorid:"+input_sensorid+" input_triggerTime:"+input_triggerTime);

    var SendData = "updateFlyEvent=1";
    SendData += "&eventid="+eventid;
    SendData += "&sensorid="+input_sensorid;
    SendData += "&datetime="+input_datetime;
    SendData += "&triggerTime="+input_triggerTime;

    jQuery.ajax({
     url: '/api/sensorinfo.php',
     type: 'POST',
     data: SendData,
     dataType: 'text',
      success:
        function(response)
        {
            setTimeout(reload_page, 300);
        },
      error:
        function(xhr, status, error)
        {
            setTimeout(reload_page, 300);
        }
    });
}

const $source_airLineCode = document.querySelector('#search_sensorid_text');
$source_airLineCode.addEventListener('input', check_search_sensorid_text);

function toggleArrivalCheckbox(element)
{
    // element.checked = !element.checked;
    document.getElementById('DepartureCheckBtn').checked = false;
    reload_page();
}

function toggleDepartureCheckbox(element)
{
    // element.checked = !element.checked;
    document.getElementById('ArrivalCheckBtn').checked = false;
    reload_page();
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

function reload_page()
{
    PreLoadIndex = -1;
    CurrentLoadIndex = 0;
    check_loading_status();
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
        reload_page();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    glCurrentPage++;
    current_page_num_ui.textContent = glCurrentPage;
    reload_page();
}

function convertPrettyDateOnly(inTime, offset)
{
    var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset(offset);
    var year = cur_date.format('YYYY');
    var month = cur_date.format('M');
    var day = cur_date.format('D');

    var perty_str = "";
    perty_str = year + "年" + month + "月" + day + "日 ";
    return perty_str;
}

function convertPrettyTimeOnly(inTime, offset)
{
    var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset(offset);
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

function open_flyevent_page()
{
    var win = window.open('/index.php/vilsflyevent/'+project_id, '_blank');
    win.focus();
}
