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

var gl_firedevent_List = [];

var pre_input_eventid_value = '';

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

                load_firedevent();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_firedevent_records();
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

function load_firedevent()
{
    gl_firedevent_List = [];

    var input_eventid_value = document.getElementById("search_eventid_text").value;

    pre_input_eventid_value = input_eventid_value;

    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;
	var targetURL = gl_target_server + "/php/eventfire.php";
    var querytext = 'project_id=1&prettystring=0&markasread=0&offset='+offset+"&count="+count;

    if(input_eventid_value.length > 0)
    {
        querytext = querytext + '&eventid=' + input_eventid_value;
    }

    var p = $.getJSON(targetURL, querytext, function(data)
    {
        $.each(data, function(index, firedevent)
        {
            gl_firedevent_List.push(firedevent);
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

function show_firedevent_records()
{
    var table_body_ui = document.getElementById('table_content');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    for(var index=0;index<gl_firedevent_List.length;index++)
    {
        firedevent(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, gl_firedevent_List[index]);
    }
}

function firedevent(table_body_ui, ui_index, record)
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

    // firedeventid
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.firedeventid);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // eventid
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.eventid);
    colDiv.setAttribute("id","sensorid_"+record.firedeventid);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // empid
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.empid);
    colDiv.setAttribute("id","empid_"+record.firedeventid);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // comment
    row.appendChild(document.createElement('td'));
    var colDivDATE = createColDiv(record.comment);
    colDivDATE.setAttribute("id","comment_"+record.firedeventid);
    row.cells[cellIndex].appendChild(colDivDATE);
    cellIndex++;

    // eventjson
    row.appendChild(document.createElement('td'));
    var colDivST = createColDiv(JSON.stringify(record.eventjson));
    colDivST.setAttribute("id","eventjson_"+record.firedeventid);
    row.cells[cellIndex].appendChild(colDivST);
    cellIndex++;

    // datetime
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.datetime);
    colDiv.setAttribute("id","datetime_"+record.firedeventid);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // Edit button
    row.appendChild(document.createElement('td'));
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id","uiid_editbtn_"+record.firedeventid);
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_record(\""+record.firedeventid+"\");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","uiid_editimg_"+record.firedeventid);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);
    cellIndex++;
}

function edit_record(firedeventid)
{
    var firedevent = getfiredevent(firedeventid);
    if (firedevent == null)
        return;

    create_edit_ui("empid", firedeventid, firedevent.empid);
    create_edit_ui("comment", firedeventid, firedevent.comment);
    create_edit_ui("eventjson", firedeventid, JSON.stringify(firedevent.eventjson));
    create_edit_ui("datetime", firedeventid, firedevent.datetime);

    var editBtn = document.getElementById("uiid_editbtn_"+firedeventid);
    editBtn.setAttribute("onclick","save_record(\""+firedeventid+"\");return false;");
    var imgEdit = document.getElementById("uiid_editimg_"+firedeventid);
    imgEdit.setAttribute("class","fa fa-save");
}

function create_edit_ui(uiid, firedeventid, text)
{
    var uiid_name = clearChildView(uiid+"_"+firedeventid);

    var ui_new_input = document.createElement("div");
    var new_input = create_input_text(uiid+"_"+firedeventid+"_edit", text);
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

function getfiredevent(firedeventid)
{
    var retfiredevent = null;
    for(var i=0;i<gl_firedevent_List.length;i++)
    {
        var firedevent = gl_firedevent_List[i];

        if (firedevent.firedeventid === firedeventid)
        {
            retfiredevent = firedevent;
            break;
        }
    }
    return retfiredevent;
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

var gl_search_eventid_start = 0;
function search_eventid_text_change()
{
    var input_value = document.getElementById("search_eventid_text").value;

    if ( input_value === pre_input_eventid_value )
    {
        console.log("search_eventid_text_change() input_value === pre_input_eventid_value, input_value:"+input_value);
        return;
    }

    gl_search_eventid_start = 0;

    glCurrentPage = 1;
    reload_page();

    return false;
}

function check_search_eventid_text(e)
{
    if ( gl_search_eventid_start == 0 )
    {
        gl_search_eventid_start = new Date();
    }

    var cur_time = new Date();
    var seconds = (cur_time.getTime() - gl_search_eventid_start.getTime()) / 1000;

    // console.log("check_search_text_change() seconds:"+seconds);

    if ( CurrentLoadIndex != LOAD_STATE_END )
    {
        console.log("check_search_eventid_text() CurrentLoadIndex != LOAD_STATE_END");

        setTimeout(search_eventid_text_change, 500);
        return;
    }

    if ( seconds >= 1 )
    {
        gl_search_eventid_start = 0;
        search_eventid_text_change();
    }
    else
    {
        setTimeout(search_eventid_text_change, 500);
    }
}

function save_record(firedeventid)
{
    var ui_empid = document.getElementById("empid_"+firedeventid+"_edit");
    var ui_comment = document.getElementById("comment_"+firedeventid+"_edit");
    var ui_eventjson = document.getElementById("eventjson_"+firedeventid+"_edit");
    var ui_datetime = document.getElementById("datetime_"+firedeventid+"_edit");

    var input_empid = ui_empid.value;
    var input_comment = ui_comment.value;
    var input_eventjson = ui_eventjson.value;
    var input_datetime = ui_datetime.value;

    console.log("save_record() input_eventjson:"+input_eventjson+" input_datetime:"+input_datetime);

    var SendData = "updatefiredevent=1";
    SendData += "&firedeventid="+firedeventid;
    SendData += "&empid="+input_empid;
    SendData += "&comment="+input_comment;
    SendData += "&eventjson="+input_eventjson;
    SendData += "&datetime="+input_datetime;

    jQuery.ajax({
     url: '/php/eventfire.php',
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

const $source_airLineCode = document.querySelector('#search_eventid_text');
$source_airLineCode.addEventListener('input', check_search_eventid_text);

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

function open_firedevent_page()
{
    var win = window.open('/index.php/vilsfiredevent/'+project_id, '_blank');
    win.focus();
}
