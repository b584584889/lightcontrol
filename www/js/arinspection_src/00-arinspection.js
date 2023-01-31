/*
 * Author: ikki Chung
 * Date: 2022 05 18
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var glCurrentPage = 1;
var LOAD_PAGE_NUM = 20;

var gl_arinspection_List = [];

// var pre_input_airLineCode_value = '';
// var pre_input_airLineNum_value = '';
// var pre_input_date_value = '';

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

                load_arinspectioninfo();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_arinspection_records();
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

function load_arinspectioninfo()
{
    gl_arinspection_List = [];

    var loadType = "";
    if (document.getElementById('ImageCheckBtn').checked)
    {
        loadType = "0";
    }
    if (document.getElementById('AudioCheckBtn').checked)
    {
        loadType = "1";
    }

    // var input_airLineCode_value = document.getElementById("search_airLineCode_text").value;
    // var input_airLineNum_value = document.getElementById("search_airLineNum_text").value;
    // var input_date_value = document.getElementById("search_date_text").value;
    //
    // pre_input_airLineCode_value = input_airLineCode_value;
    // pre_input_airLineNum_value = input_airLineNum_value;
    // pre_input_date_value = input_date_value;

    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    var targetURL = gl_target_server + "/api/arinspection.php";
    var querytext = 'loadrecord=1&key=sotA6cY54U&type='+loadType+"&offset="+offset+"&count="+count;

    // if(input_airLineCode_value.length > 0)
    // {
    //     querytext = querytext + '&airLineCode=' + input_airLineCode_value;
    // }
    // if(input_airLineNum_value.length > 0)
    // {
    //     querytext = querytext + '&airLineNum=' + input_airLineNum_value;
    // }
    // if(input_date_value.length > 0)
    // {
    //     querytext = querytext + '&day=' + input_date_value;
    // }

    var p = $.getJSON(targetURL, querytext, function(data)
    {
        $.each(data, function(index, arinspection)
        {
            gl_arinspection_List.push(arinspection);
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

function show_arinspection_records()
{
    var table_body_ui = document.getElementById('table_content');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    for(var index=0;index<gl_arinspection_List.length;index++)
    {
        arinspectioninfo(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, gl_arinspection_List[index]);
    }
}

function arinspectioninfo(table_body_ui, ui_index, record)
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

    // created
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.created);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // datetime
    row.appendChild(document.createElement('td'));
    var colDivDATE = createColDiv(record.datetime);
    row.cells[cellIndex].appendChild(colDivDATE);
    cellIndex++;

    // android_id
    row.appendChild(document.createElement('td'));
    var colDivST = createColDiv(record.android_id);
    row.cells[cellIndex].appendChild(colDivST);
    cellIndex++;

    // Longitude
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.Longitude);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // Latitude
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.Latitude);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // type
    row.appendChild(document.createElement('td'));

    var filetype = "照片";
    if (record.filetype == 1)
        filetype = "錄音";

    var colDiv = createColDiv(filetype);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // image
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv("file");
    if (record.filetype == 0)
        colDiv = createImageColDiv(record.file);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;
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

function createColDiv(textValue)
{
    var colDiv = document.createElement('div');
    colDiv.appendChild(document.createTextNode(textValue));
    return colDiv;
}

function createImageColDiv(textValue)
{
    var colDiv = document.createElement('div');

    var img_src= "data:image/png;base64, " + textValue;

    var imageTag = document.createElement('img');
    imageTag.setAttribute("src",img_src);
    imageTag.setAttribute("width",200);
    imageTag.setAttribute("height",112.5);

    colDiv.appendChild(imageTag);
    return colDiv;
}


var gl_search_airLineCode_start = 0;
function search_airLineCode_text_change()
{
    var input_value = document.getElementById("search_airLineCode_text").value;

    if ( input_value === pre_input_airLineCode_value )
    {
        console.log("search_airLineCode_text_change() input_value === pre_input_airLineCode_value, input_value:"+input_value);
        return;
    }

    gl_search_airLineCode_start = 0;

    glCurrentPage = 1;
    reload_page();

    return false;
}

function check_search_airLineCode_text(e)
{
    if ( gl_search_airLineCode_start == 0 )
    {
        gl_search_airLineCode_start = new Date();
    }

    var cur_time = new Date();
    var seconds = (cur_time.getTime() - gl_search_airLineCode_start.getTime()) / 1000;

    // console.log("check_search_text_change() seconds:"+seconds);

    if ( CurrentLoadIndex != LOAD_STATE_END )
    {
        console.log("check_search_airLineCode_text() CurrentLoadIndex != LOAD_STATE_END");

        setTimeout(search_airLineCode_text_change, 500);
        return;
    }

    if ( seconds >= 1 )
    {
        gl_search_airLineCode_start = 0;
        search_airLineCode_text_change();
    }
    else
    {
        setTimeout(search_airLineCode_text_change, 500);
    }
}

var gl_search_airLineNum_start = 0;
function search_airLineNum_text_change()
{
    var input_value = document.getElementById("search_airLineNum_text").value;

    if ( input_value === pre_input_airLineNum_value )
    {
        console.log("search_airLineNum_text_change() input_value === pre_input_airLineNum_value, input_value:"+input_value);
        return;
    }

    gl_search_airLineNum_start = 0;

    glCurrentPage = 1;
    reload_page();

    return false;
}

function check_search_airLineNum_text(e)
{
    if ( gl_search_airLineNum_start == 0 )
    {
        gl_search_airLineNum_start = new Date();
    }

    var cur_time = new Date();
    var seconds = (cur_time.getTime() - gl_search_airLineNum_start.getTime()) / 1000;

    if ( CurrentLoadIndex != LOAD_STATE_END )
    {
        console.log("check_search_airLineNum_text() CurrentLoadIndex != LOAD_STATE_END");

        setTimeout(search_airLineNum_text_change, 500);
        return;
    }

    if ( seconds >= 1 )
    {
        gl_search_airLineNum_start = 0;
        search_airLineNum_text_change();
    }
    else
    {
        setTimeout(search_airLineNum_text_change, 500);
    }
}

var gl_search_date_start = 0;
function search_date_text_change()
{
    var input_value = document.getElementById("search_date_text").value;

    if ( input_value === pre_input_date_value )
    {
        console.log("search_date_text_change() input_value === pre_input_date_value, input_value:"+input_value);
        return;
    }

    gl_search_date_start = 0;

    glCurrentPage = 1;
    reload_page();

    return false;
}

function check_search_date_text(e)
{
    if ( gl_search_date_start == 0 )
    {
        gl_search_date_start = new Date();
    }

    var cur_time = new Date();
    var seconds = (cur_time.getTime() - gl_search_date_start.getTime()) / 1000;

    // console.log("check_search_text_change() seconds:"+seconds);

    if ( CurrentLoadIndex != LOAD_STATE_END )
    {
        console.log("check_search_date_text() CurrentLoadIndex != LOAD_STATE_END");

        setTimeout(search_date_text_change, 500);
        return;
    }

    if ( seconds >= 1 )
    {
        gl_search_date_start = 0;
        search_date_text_change();
    }
    else
    {
        setTimeout(search_date_text_change, 500);
    }
}

// const $source_airLineCode = document.querySelector('#search_airLineCode_text');
// $source_airLineCode.addEventListener('input', check_search_airLineCode_text);
//
// const $source_airLineNum = document.querySelector('#search_airLineNum_text');
// $source_airLineNum.addEventListener('input', check_search_airLineNum_text);
//
// const $source_date = document.querySelector('#search_date_text');
// $source_date.addEventListener('input', check_search_date_text);

function toggleImageCheckbox(element)
{
    // element.checked = !element.checked;
    document.getElementById('AudioCheckBtn').checked = false;
    reload_page();
}

function toggleAudioCheckbox(element)
{
    // element.checked = !element.checked;
    document.getElementById('ImageCheckBtn').checked = false;
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

function open_arinspectioninfo_page()
{
    var win = window.open('/index.php/vilsarinspectioninfo/'+project_id, '_blank');
    win.focus();
}
