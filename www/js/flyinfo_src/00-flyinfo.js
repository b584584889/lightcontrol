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

var gl_flyinfo_List = [];

var pre_input_airLineCode_value = '';
var pre_input_airLineNum_value = '';
var pre_input_date_value = '';

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

                load_flyinfo();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_flyinfo_records();
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

function load_flyinfo()
{
    gl_flyinfo_List = [];

    var loadType = "";
    if (document.getElementById('ArrivalCheckBtn').checked)
    {
        loadType = "Arrival";
    }
    if (document.getElementById('DepartureCheckBtn').checked)
    {
        loadType = "Departure";
    }

    var input_airLineCode_value = document.getElementById("search_airLineCode_text").value;
    var input_airLineNum_value = document.getElementById("search_airLineNum_text").value;
    var input_date_value = document.getElementById("search_date_text").value;

    pre_input_airLineCode_value = input_airLineCode_value;
    pre_input_airLineNum_value = input_airLineNum_value;
    pre_input_date_value = input_date_value;

    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    var targetURL = gl_target_server + "/api/sensorinfo.php";
    var querytext = 'loadflyinfo=1&key=sotA6cY54U&type='+loadType+"&offset="+offset+"&count="+count;

    if(input_airLineCode_value.length > 0)
    {
        querytext = querytext + '&airLineCode=' + input_airLineCode_value;
    }
    if(input_airLineNum_value.length > 0)
    {
        querytext = querytext + '&airLineNum=' + input_airLineNum_value;
    }
    if(input_date_value.length > 0)
    {
        querytext = querytext + '&day=' + input_date_value;
    }

    var p = $.getJSON(targetURL, querytext, function(data)
    {
        $.each(data, function(index, flyinfo)
        {
            gl_flyinfo_List.push(flyinfo);
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

function show_flyinfo_records()
{
    var table_body_ui = document.getElementById('table_content');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    for(var index=0;index<gl_flyinfo_List.length;index++)
    {
        flyinfo(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, gl_flyinfo_List[index]);
    }
}

function flyinfo(table_body_ui, ui_index, record)
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

    // airBoardingGate
    row.appendChild(document.createElement('td'));
    var colDiv = createColDiv(record.airBoardingGate);
    colDiv.setAttribute("id","airBoardingGate_"+record.FDATE+"_"+record.airLineNum);
    row.cells[cellIndex].appendChild(colDiv);
    cellIndex++;

    // triggerTime
    row.appendChild(document.createElement('td'));
    var colDivtriggerTime = document.createElement("span");
    // colDivtriggerTime.setAttribute("class","label label-info");
    colDivtriggerTime.setAttribute("id","triggerTime_"+record.FDATE+"_"+record.airLineNum);
    colDivtriggerTime.textContent = record.triggerTime;
    row.cells[cellIndex].appendChild(colDivtriggerTime);
    cellIndex++;

    // Edit button
    row.appendChild(document.createElement('td'));
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id","uiid_editbtn_"+record.FDATE+"_"+record.airLineNum);
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_record(\""+record.FDATE+"\",\""+record.airLineNum+"\");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","uiid_editimg_"+record.FDATE+"_"+record.airLineNum);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);
    cellIndex++;
}

function edit_record(FDATE, airLineNum)
{
    var flyinfo = getFlyInfo(FDATE, airLineNum);
    if (flyinfo == null)
        return;

    create_edit_ui("airBoardingGate", FDATE, airLineNum, flyinfo.airBoardingGate);
    create_edit_ui("triggerTime", FDATE, airLineNum, flyinfo.triggerTime);

    var editBtn = document.getElementById("uiid_editbtn_"+FDATE+"_"+airLineNum);
    editBtn.setAttribute("onclick","save_record(\""+FDATE+"\",\""+airLineNum+"\");return false;");
    var imgEdit = document.getElementById("uiid_editimg_"+FDATE+"_"+airLineNum);
    imgEdit.setAttribute("class","fa fa-save");
}

function create_edit_ui(uiid, FDATE, airLineNum, text)
{
    var uiid_name = clearChildView(uiid+"_"+FDATE+"_"+airLineNum);

    var ui_new_input = document.createElement("div");
    var new_input = create_input_text(uiid+"_"+FDATE+"_"+airLineNum+"_edit", text);
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

function getFlyInfo(FDATE, airLineNum)
{
    var retflyinfo = null;
    for(var i=0;i<gl_flyinfo_List.length;i++)
    {
        var flyinfo = gl_flyinfo_List[i];

        if ( flyinfo.FDATE === FDATE && flyinfo.airLineNum === airLineNum )
        {
            retflyinfo = flyinfo;
            break;
        }

    }
    return retflyinfo;
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

function confirm_delete_record(title, recordid)
{
    if (confirm('確定刪除公告事項:'+title+'   ?'))
    {
        // delete it!
        delete_record(recordid);
    }
    else
    {
        // Do nothing!
    }
}

function save_record(FDATE, airLineNum)
{
    var ui_airBoardingGate = document.getElementById("airBoardingGate_"+FDATE+"_"+airLineNum+"_edit");
    var ui_triggerTime = document.getElementById("triggerTime_"+FDATE+"_"+airLineNum+"_edit");

    var input_airBoardingGate = ui_airBoardingGate.value;
    var input_triggerTime = ui_triggerTime.value;

    console.log("save_record() input_airBoardingGate:"+input_airBoardingGate+" input_triggerTime:"+input_triggerTime);

    var SendData = "updateFlyInfo=1";
    SendData += "&FDATE="+FDATE;
    SendData += "&airLineNum="+airLineNum;
    SendData += "&airBoardingGate="+input_airBoardingGate;
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

const $source_airLineCode = document.querySelector('#search_airLineCode_text');
$source_airLineCode.addEventListener('input', check_search_airLineCode_text);

const $source_airLineNum = document.querySelector('#search_airLineNum_text');
$source_airLineNum.addEventListener('input', check_search_airLineNum_text);

const $source_date = document.querySelector('#search_date_text');
$source_date.addEventListener('input', check_search_date_text);

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

function open_flyinfo_page()
{
    var win = window.open('/index.php/vilsflyinfo/'+project_id, '_blank');
    win.focus();
}
