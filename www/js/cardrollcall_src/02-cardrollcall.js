/*
 * Author: ikki Chung
 * Date: 2020.05.05
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var glCurrentPage = 1;

var gl_usersubgroups = [];
var gl_userdatasubgroups = [];

var gl_rfiddevices = [];
var gl_nfclocators = [];
var gl_cardrollcall = [];
var gl_NewRollCall = {};


// var glSetWeekDay = false;

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

                load_usersubgroup();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_rfiddevice();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_nfclocator();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cardrollcall();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                draw_cardrollcall_table();

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

function load_usersubgroup()
{
    gl_usersubgroups = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadusersubgroup=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, usersubgroup)
        {
            usersubgroup.subgroupid = parseInt(usersubgroup.subgroupid);
            gl_usersubgroups.push(usersubgroup);
        });
    })
    .success(function() {
        load_userdatasubgroup();
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_userdatasubgroup()
{
    gl_userdatasubgroups = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaddatasubgroup=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, group)
        {
            gl_userdatasubgroups.push(group);
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_rfiddevice()
{
    gl_rfiddevices = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loaddevice=1&project_id='+project_id, function(data)
    {
        var rfiddevice_num = 0;
        $.each(data, function(index, device)
        {
            gl_rfiddevices.push(device);
            rfiddevice_num++;
        });

    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_nfclocator()
{
    gl_nfclocators = [];

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadnfclocator=1&project_id='+project_id, function(data)
    {
        var rfiddevice_num = 0;
        $.each(data, function(index, device)
        {
            gl_nfclocators.push(device);
            rfiddevice_num++;
        });

    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_cardrollcall()
{
    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    gl_cardrollcall = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcardrollcall=1&s='+offset+'&count='+count+'&project_id='+project_id, function(data)
    {
        var cardrollcall_num = 0;
        $.each(data, function(index, device)
        {
            gl_cardrollcall.push(device);
            cardrollcall_num++;
        });

    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function create_empty_record_table_item(ui_index)
{
    var empty_record_table_item = document.getElementById('empty_record_table_item');
    if ( empty_record_table_item != null )
        return;

    gl_NewRollCall.rollcallid = 88888;
    gl_NewRollCall.weekday = ['1','2','3','4','5','6','0'];
    gl_NewRollCall.validdate = [];
    gl_NewRollCall.invaliddate = [];

    var record_table = document.getElementById('RFID_ROLLCALL_TABLE');
    var row = record_table.insertRow(0);

    //var row = document.createElement('tr');
    row.setAttribute("id", "empty_record_table_item");
    row.setAttribute("bgcolor","#ebebe0");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // name
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_inputarea_text("input_name"));
    cellIndex++;

    // StartTime
    row.appendChild(document.createElement('td'));

    // time Picker
    var div_StartTime_TimePicker = create_time_picker("rollcall_StartTime", "07:00 AM");
    row.cells[cellIndex].appendChild(div_StartTime_TimePicker);
    cellIndex++;

    // EndTime
    row.appendChild(document.createElement('td'));
    // time Picker
    var div_EndTime_TimePicker = create_time_picker("rollcall_EndTime", "08:00 AM");
    row.cells[cellIndex].appendChild(div_EndTime_TimePicker);
    cellIndex++;

    // Schedule date
    var td_Date = document.createElement('td')
    var weekdaydiv = document.createElement("div");
    weekdaydiv.setAttribute("id","rollcall_weekday");
    td_Date.appendChild(weekdaydiv);
    var weekdayui = create_weekday_ui(88888, gl_NewRollCall.weekday, true);
    weekdaydiv.appendChild(weekdayui);
    var div_Date = document.createElement("div");
    div_Date.setAttribute("id","div_calendar_88888");
    td_Date.appendChild(div_Date);
    var div_comment = document.createElement("div");
    td_Date.appendChild(div_comment);
    var span_disableddate = document.createElement("span");
    span_disableddate.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-image: linear-gradient(to bottom, #cc4400, #994411);");
    span_disableddate.textContent = '日期';
    div_comment.appendChild(span_disableddate);
    var span_disabledtext = document.createElement("span");
    span_disabledtext.textContent = ':代表不點名日期';
    div_comment.appendChild(span_disabledtext);
    row.appendChild(td_Date);
    cellIndex++;

    // RFID device list
    row.appendChild(document.createElement('td'));
    var div_devicelist = create_edit_devicelist_ui("rollcall_devicelist", "new", "", "");
    row.cells[cellIndex].appendChild(div_devicelist);
    cellIndex++;

    // Subgroup list
    row.appendChild(document.createElement('td'));
    var div_subgrouplist = create_edit_subgrouplist_ui("rollcall_subgrouplist", "new", "");
    row.cells[cellIndex].appendChild(div_subgrouplist);
    cellIndex++;

    // 排程推播設定
    row.appendChild(document.createElement('td'));

    // presentnotify
    var div_presentnotify = document.createElement("div");
    row.cells[cellIndex].appendChild(div_presentnotify);

    var item_presentnotify_checkbox = document.createElement("input");
    item_presentnotify_checkbox.setAttribute("id","checkbox_presentnotify");
    item_presentnotify_checkbox.setAttribute("type","checkbox");
    item_presentnotify_checkbox.checked = false;
    var item_presentnotify_checkbox_label = document.createElement("label");
    item_presentnotify_checkbox_label.setAttribute("for","checkbox_presentnotify");
    item_presentnotify_checkbox_label.textContent = "點名已到時, 發送推播. ";
    div_presentnotify.appendChild(item_presentnotify_checkbox);
    div_presentnotify.appendChild(item_presentnotify_checkbox_label);

    // presentcomment
    div_presentnotify.appendChild(create_input_text("input_presentcomment", "點名已到"));

    // absentnotify
    var div_absentnotify = document.createElement("div");
    row.cells[cellIndex].appendChild(div_absentnotify);

    var item_absentnotify_checkbox = document.createElement("input");
    item_absentnotify_checkbox.setAttribute("id","checkbox_absentnotify");
    item_absentnotify_checkbox.setAttribute("type","checkbox");
    item_absentnotify_checkbox.checked = false;
    var item_absentnotify_checkbox_label = document.createElement("label");
    item_absentnotify_checkbox_label.setAttribute("for","checkbox_absentnotify");
    item_absentnotify_checkbox_label.textContent = "點完名未到時, 發送推播";
    div_absentnotify.appendChild(item_absentnotify_checkbox);
    div_absentnotify.appendChild(item_absentnotify_checkbox_label);

    // absentcomment
    div_absentnotify.appendChild(create_input_text("input_absentcomment", "點名未到"));

    // 可重複推播
    var div_resend = document.createElement("div");
    row.cells[cellIndex].appendChild(div_resend);

    var item_resend_checkbox = document.createElement("input");
    item_resend_checkbox.setAttribute("id","checkbox_resend");
    item_resend_checkbox.setAttribute("type","checkbox");
    item_resend_checkbox.checked = false;
    var item_absentnotify_checkbox_label = document.createElement("label");
    item_absentnotify_checkbox_label.setAttribute("for","checkbox_resend");
    item_absentnotify_checkbox_label.textContent = "可重複推播";
    div_resend.appendChild(item_resend_checkbox);
    div_resend.appendChild(item_absentnotify_checkbox_label);

    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_rollcall();");
    Button_save.textContent = "儲存";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;

    // record_table.appendChild(row);
}

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("input");
    input_text.setAttribute("type","text");
    input_text.setAttribute("id",ui_id);
    input_text.setAttribute("value",text);

    return input_text;
}

function create_inputarea_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.textContent = text;

    return input_text;
}

function create_time_picker(ui_id, timetext)
{
    // time Picker
    var div_TimePicker = document.createElement("div");
    div_TimePicker.setAttribute("class","bootstrap-timepicker");
    div_TimePicker.setAttribute("style","width:200px;");

    var div_inputgroup = document.createElement("div");
    div_inputgroup.setAttribute("class","input-group");
    div_TimePicker.appendChild(div_inputgroup);
    // input
    var input_Time = document.createElement("input");
    input_Time.setAttribute("type","text");
    input_Time.setAttribute("id",ui_id);
    input_Time.setAttribute("readonly","readonly");
    input_Time.setAttribute("class","form-control timepicker");
    input_Time.setAttribute("value",timetext);
    div_inputgroup.appendChild(input_Time);

    // clock icon
    var div_clock = document.createElement("div");
    div_clock.setAttribute("class","input-group-addon");
    div_inputgroup.appendChild(div_clock);
    // icon
    var div_clock_icon = document.createElement("i");
    div_clock_icon.setAttribute("class","fa fa-clock-o");
    div_clock.appendChild(div_clock_icon);

    return div_TimePicker;
}

function create_edit_devicelist_ui(uiid, rollcallid, rfiddeviceids, nfclocators)
{
    var div_devicelist = document.createElement("div");

    if ( gl_groupid == 5 )
    {
        // no rfid device
    }
    else
    {
        var div_rfid_devicelist = create_edit_rfid_devicelist_ui(uiid, rollcallid, rfiddeviceids);
        div_devicelist.appendChild(div_rfid_devicelist);
    }

    var div_nfc_devicelist = create_edit_nfc_devicelist_ui(uiid, rollcallid, nfclocators);
    div_devicelist.appendChild(div_nfc_devicelist);

    return div_devicelist;
}

function create_edit_rfid_devicelist_ui(uiid, rollcallid, idlist)
{
    var div_devicelist = document.createElement("div");
    div_devicelist.setAttribute("class","btn-group");

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "UHF感應器";
    div_devicelist.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_devicelist.appendChild(div_ul);

    if ( gl_groupid == 1 || gl_groupid == 2 )
    {
        // select all button
        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","dropdown_selectall(\"device_uhfreader\",\""+rollcallid+"\");return true;");
        item_li.appendChild(item_click);
        item_click.appendChild(document.createTextNode("全選"));
        div_ul.appendChild(item_li);
    }

    var idList = idlist.split(",");

    for (var i = 0; i < gl_rfiddevices.length; ++i)
    {
        var rfiddevice = gl_rfiddevices[i];
        var existed = check_numer_in_list(parseInt(rfiddevice.deviceid), idList);

        // console.log("create_edit_rfid_devicelist_ui() rfiddevice.deviceid:"+rfiddevice.deviceid+" idList:"+idList +" existed:"+existed);

        div_ul.appendChild(create_dropdown_device_item("uhfreader", rollcallid, i, parseInt(rfiddevice.deviceid), rfiddevice.name, existed));
    }

    return div_devicelist;
}

function create_edit_nfc_devicelist_ui(uiid, rollcallid, idlist)
{
    var div_devicelist = document.createElement("div");
    div_devicelist.setAttribute("class","btn-group");

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "NFC Locator";
    div_devicelist.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_devicelist.appendChild(div_ul);

    if ( gl_groupid == 1 || gl_groupid == 2 )
    {
        // select all button
        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","dropdown_selectall(\"device_nfclocator\",\""+rollcallid+"\");return true;");
        item_li.appendChild(item_click);
        item_click.appendChild(document.createTextNode("全選"));
        div_ul.appendChild(item_li);
    }

    var idList = idlist.split(",");

    for (var i = 0; i < gl_nfclocators.length; ++i)
    {
        var nfcdevice = gl_nfclocators[i];
        var ret = idList.indexOf(nfcdevice.macaddress);
        var existed = false;
        if (ret >= 0)
            existed = true;
        div_ul.appendChild(create_dropdown_device_item("nfclocator", rollcallid, i, parseInt(nfcdevice.macaddress), nfcdevice.name, existed));
    }

    return div_devicelist;
}

function create_dropdown_device_item(prefix, rollcallid, index, datavalue, dataname, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", datavalue);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_device_click(\""+prefix+"\",\""+rollcallid+"\","+ index +");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_device_"+prefix+"_"+rollcallid+"_"+index);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_device_click();return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+dataname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_selectall(prefix, rollcallid)
{
    console.log("dropdown_selectall() prefix:"+prefix+" rollcallid:"+rollcallid);

    for(var i=0;i<100;i++)
    {
        var ele = document.getElementById("dropdown_checkbox_"+prefix+"_"+rollcallid+"_"+i);
        if ( ele == undefined )
        {
            console.log("dropdown_selectall() rollcallid:"+rollcallid+" i:"+i+" ele == undefined ");
            break;
        }
        ele.checked = true;
    }
}

function dropdown_checkbox_device_click(rollcallid, index)
{
    glDropdown_checkbox_device_click = true;
}

var glDropdown_checkbox_device_click = false;
function dropdown_device_click(prefix, rollcallid, index)
{
    if (glDropdown_checkbox_device_click == true)
    {
        glDropdown_checkbox_device_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_device_"+prefix+"_"+rollcallid+"_"+index).checked == true)
        document.getElementById("dropdown_checkbox_device_"+prefix+"_"+rollcallid+"_"+index).checked = false;
    else
        document.getElementById("dropdown_checkbox_device_"+prefix+"_"+rollcallid+"_"+index).checked = true;

    glDropdown_checkbox_device_click = false;
}


function create_edit_subgrouplist_ui(uiid, rollcallid, idlist)
{
    var div_subgrouplist = document.createElement("div");
    div_subgrouplist.setAttribute("class","btn-group");

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "子集合";
    div_subgrouplist.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_subgrouplist.appendChild(div_ul);

    if ( gl_groupid == 1 || gl_groupid == 2 )
    {
        // select all button
        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","dropdown_selectall(\"subgroup\",\""+rollcallid+"\");return true;");
        item_li.appendChild(item_click);
        item_click.appendChild(document.createTextNode("全選"));
        div_ul.appendChild(item_li);
    }

    var idList = idlist.split(",");

    for (var i = 0; i < gl_userdatasubgroups.length; ++i)
    {
        var usersubgroup = gl_userdatasubgroups[i];
        var existed = check_numer_in_list(parseInt(usersubgroup.subgroupid), idList);
        div_ul.appendChild(create_dropdown_subgroup_item(rollcallid, i, parseInt(usersubgroup.subgroupid), usersubgroup.subgroupname, existed));
    }

    return div_subgrouplist;
}

function create_dropdown_subgroup_item(rollcallid, index, datavalue, dataname, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", datavalue);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_subgroup_click(\""+rollcallid+"\","+ index +");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_subgroup_"+rollcallid+"_"+index);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_subgroup_click(\""+rollcallid+"\","+ index +");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+dataname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_checkbox_subgroup_click(rollcallid, index)
{
    glDropdown_checkbox_subgroup_click = true;
}

var glDropdown_checkbox_subgroup_click = false;
function dropdown_subgroup_click(rollcallid, index)
{
    if (glDropdown_checkbox_subgroup_click == true)
    {
        glDropdown_checkbox_subgroup_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_subgroup_"+rollcallid+"_"+index).checked == true)
        document.getElementById("dropdown_checkbox_subgroup_"+rollcallid+"_"+index).checked = false;
    else
        document.getElementById("dropdown_checkbox_subgroup_"+rollcallid+"_"+index).checked = true;

    glDropdown_checkbox_subgroup_click = false;
}

function getSelected_uhfreader_Device(rollcallid)
{
    var deviceids = [];
    for (var index = 0; index < gl_rfiddevices.length; ++index)
    {
        var rfiddevice = gl_rfiddevices[index];

        if (document.getElementById("dropdown_checkbox_device_uhfreader_"+rollcallid+"_"+index).checked)
        {
            deviceid = parseInt(rfiddevice.deviceid);
            deviceids.push(deviceid);
        }
    }
    return deviceids.toString();
}

function getSelected_nfclocator_Device(rollcallid)
{
    var deviceids = [];
    for (var index = 0; index < gl_nfclocators.length; ++index)
    {
        var nfclocator = gl_nfclocators[index];

        if (document.getElementById("dropdown_checkbox_device_nfclocator_"+rollcallid+"_"+index).checked)
        {
            deviceids.push(nfclocator.macaddress);
        }
    }
    return deviceids.toString();
}

function getSelectedSubgroup(rollcallid)
{
    var subgroups = [];
    for (var index = 0; index < gl_userdatasubgroups.length; ++index)
    {
        var usersubgroup = gl_userdatasubgroups[index];

        if (document.getElementById("dropdown_checkbox_subgroup_"+rollcallid+"_"+index).checked)
        {
            var subgroupid = parseInt(usersubgroup.subgroupid);
            subgroups.push(subgroupid);
        }
    }
    return subgroups.toString();
}

function draw_cardrollcall_table()
{
    var record_table = document.getElementById('RFID_ROLLCALL_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

     //console.log("draw_cardrollcall_table() gl_cardrollcall.length:"+gl_cardrollcall.length);

     var ui_index = 0;
    //for (var i = 0; i < gl_cardrollcall.length; ++i)
    for (var i = gl_cardrollcall.length-1; i>=0; i--)
    {
        var rollcall = gl_cardrollcall[i];

        var row = create_record_table_item(ui_index, rollcall);
        record_table.appendChild(row);
        ui_index += 1;
    }

    //for (var i = 0; i < gl_cardrollcall.length; ++i)
    for (var i = gl_cardrollcall.length-1; i>=0; i--)
    {
        var rollcall = gl_cardrollcall[i];
        var calendar_id = "div_calendar_"+rollcall.rollcallid;
        var isEditing = false;

        // console.log("draw_cardrollcall_table() calendar_id:"+calendar_id);
        $('#'+calendar_id).datepicker(
        {
            rollcallid: rollcall.rollcallid,
            language: 'zh-TW',
            weekStart: 1,
            beforeShowDay: function (date, rollcallid)
            {
                var rollcall = getRollCall(rollcallid);
                //console.log("beforeShowDay() id:"+id);
                // console.log("beforeShowDay() this.picker.id:"+$(this).picker.id);
                return checkValidDate(date, rollcall, isEditing, false);
            }
        });
    }
}

function checkValidDate(date, rollcall, isEditing, isToggleSelect)
{
    //var allDates = dt.getFullYear() + '-' + (dt.getMonth() + 1) + '-' + dt.getDate() + ' week:' + dt.getDay();
    var targetDate = date.getFullYear() + "-" + ( '0' + (date.getMonth()+1) ).slice( -2 ) + "-" + ( '0' + date.getDate() ).slice( -2 );
    // console.log("checkValidDate():"+targetDate+" rollcallid:"+rollcallid +" isEditing:"+isEditing +" isToggleSelect:"+isToggleSelect);

    var day = date.getDay();
    var existed_index = rollcall.weekday.indexOf(''+day);
    //console.log("checkValidDate() $(this).id:"+$(this).id);
    // console.log("checkValidDate():"+allDates+" rollcallid:"+rollcallid +" isEditing:"+isEditing);
    // console.log("checkValidDate() day:"+day+" rollcall.weekday:"+rollcall.weekday+" existed_index:"+existed_index);

    // console.log("checkValidDate():"+allDates+" day:"+day+" rollcallid:"+rollcallid +" isEditing:"+isEditing +" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index);

    if ( existed_index >= 0 )
    {
        //console.log("checkValidDate(): 1 isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index);

        if (isToggleSelect)
        {
            var isActive = true;
            // 要點名週間
            // 檢查是否在 rollcall.invaliddate
            var indexOfinvaliddate = rollcall.invaliddate.indexOf(targetDate);
            if (indexOfinvaliddate >= 0)
            {
                // 從 rollcall.invaliddate 移除
                //console.log("checkValidDate(): 1 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 移除 不要點名");
                rollcall.invaliddate.splice(indexOfinvaliddate, 1);
                isActive = false;
            }
            else
            {
                // 加入 不要點名
                //console.log("checkValidDate(): 1 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 加入 不要點名");
                rollcall.invaliddate.push(targetDate);
            }

            if (isActive == false)
            {
                // 檢查是否在 rollcall.validdate
                var indexOfvaliddate = rollcall.validdate.indexOf(targetDate);
                if (indexOfvaliddate < 0)
                {
                    // 加入 要點名
                    //console.log("checkValidDate(): 1 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 加入 不要點名");
                    rollcall.validdate.push(targetDate);
                }
            }
            else
            {
                // 從 rollcall.validdate 移除
                var indexOfvaliddate = rollcall.validdate.indexOf(targetDate);
                if (indexOfvaliddate >= 0)
                {
                    //console.log("checkValidDate(): 1 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 移除 要點名");
                    rollcall.validdate.splice(indexOfvaliddate, 1);
                }
            }

            if (isActive)
            {
                //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 設定不點名");
                // 設定不點名
                return {enabled:isEditing, classes:'active'};
            }
            else
            {
                //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 設定要點名");
                // 設定要點名
                return {enabled:isEditing, classes:'noactive'};
            }
        }
        else
        {
            //console.log("checkValidDate(): 1 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" noactive");

            // 要點名週間
            // 檢查是否在 rollcall.invaliddate
            var indexOfinvaliddate = rollcall.invaliddate.indexOf(targetDate);
            if (indexOfinvaliddate >= 0)
            {
                // 設定不點名
                return {enabled:isEditing, classes:'active'};
            }
            else
            {
                // 設定要點名
                return {enabled:isEditing, classes:'noactive'};
            }
        }
    }
    else
    {
        //console.log("checkValidDate(): 2 isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index);

        if (isToggleSelect)
        {
            var isActive = false;
            // 不點名週間
            // 檢查是否在 rollcall.validdate
            var indexOfvaliddate = rollcall.validdate.indexOf(targetDate);
            if (indexOfvaliddate >= 0)
            {
                // 移除 要點名
                // 從 rollcall.validdate 移除
                //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 移除 要點名");
                rollcall.validdate.splice(indexOfvaliddate, 1);
                isActive = true;
            }
            else
            {
                // 加入 要點名
                //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 加入 要點名");
                rollcall.validdate.push(targetDate);
            }

            if (isActive)
            {
                // 檢查是否在 rollcall.invaliddate
                var indexOfinvaliddate = rollcall.invaliddate.indexOf(targetDate);
                if (indexOfinvaliddate < 0)
                {
                    // 加入 不要點名
                    //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 加入 不要點名");
                    rollcall.invaliddate.push(targetDate);
                }
            }
            else
            {
                // 從 rollcall.invaliddate 移除
                var indexOfinvaliddate = rollcall.invaliddate.indexOf(targetDate);
                if (indexOfinvaliddate >= 0)
                {
                    //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 移除 不要點名");
                    rollcall.invaliddate.splice(indexOfinvaliddate, 1);
                }
            }

            if (isActive)
            {
                //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 設定不點名");
                // 設定不點名
                return {enabled:isEditing, classes:'active'};
            }
            else
            {
                //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 設定要點名");
                // 設定要點名
                return {enabled:isEditing, classes:'noactive'};
            }

            //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" noactive");
        }
        else
        {
            // 不點名週間
            // 檢查是否在 rollcall.validdate
            var indexOfvaliddate = rollcall.validdate.indexOf(targetDate);
            if (indexOfvaliddate >= 0)
            {
                //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 顯示要點名");
                // 顯示要點名
                return {enabled:isEditing, classes:'noactive'};
            }
            else
            {
                //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 顯示不點名");
                // 顯示不點名
                return {enabled:isEditing, classes:'active'};
            }
            //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" active");
            //return {enabled:isEditing, classes:'highlighted'};
            //return {enabled:isEditing};
        }
    }

    return {enabled:isEditing};
}

function create_record_table_item(ui_index, rollcall)
{
    if (rollcall == null)
        return null;

    var row = document.createElement('tr');
    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+(ui_index + 1)));
    cellIndex++;

    // 名稱
    var tdName = document.createElement('td');
    tdName.setAttribute("id","uiid_name_"+rollcall.rollcallid);
    row.appendChild(tdName);
    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = rollcall.rollcallname;
    tdName.appendChild(SpanName);
    cellIndex++;

    // StartTime
    var td_StartTime = document.createElement('td')
    td_StartTime.setAttribute("id","rollcall_StartTime_"+rollcall.rollcallid);
    row.appendChild(td_StartTime);
    if (rollcall.StartDate.length > 0)
    {
        var span_StartDate = document.createElement("span");
        span_StartDate.setAttribute("class","badge bg-green-active");
        span_StartDate.textContent = rollcall.StartDate;
        row.cells[cellIndex].appendChild(span_StartDate);
    }
    var span_StartTime = document.createElement("span");
    span_StartTime.setAttribute("class","badge bg-yellow-active");
    span_StartTime.textContent = rollcall.StartTime;
    row.cells[cellIndex].appendChild(span_StartTime);
    cellIndex++;

    // EndTime
    var td_EndTime = document.createElement('td')
    td_EndTime.setAttribute("id","rollcall_EndTime_"+rollcall.rollcallid);
    row.appendChild(td_EndTime);
    row.appendChild(document.createElement('td'));
    if (rollcall.EndDate.length > 0)
    {
        var span_EndDate = document.createElement("span");
        span_EndDate.setAttribute("class","badge bg-green-active");
        span_EndDate.textContent = rollcall.EndDate;
        row.cells[cellIndex].appendChild(span_EndDate);
    }
    var span_EndTime = document.createElement("span");
    span_EndTime.setAttribute("class","badge bg-yellow-active");
    span_EndTime.textContent = rollcall.EndTime;
    row.cells[cellIndex].appendChild(span_EndTime);
    cellIndex++;

    // Schedule date
    var td_Date = document.createElement('td')
    //td_Date.setAttribute("class","bg-green-gradient");
    var weekdaydiv = document.createElement("div");
    weekdaydiv.setAttribute("id","div_weekday_"+rollcall.rollcallid);
    td_Date.appendChild(weekdaydiv);
    var weekdayui = create_weekday_ui(rollcall.rollcallid, rollcall.weekday, false);
    weekdaydiv.appendChild(weekdayui);
    var div_Date = document.createElement("div");
    div_Date.setAttribute("id","div_calendar_"+rollcall.rollcallid);
    td_Date.appendChild(div_Date);
    var div_comment = document.createElement("div");
    td_Date.appendChild(div_comment);
    var span_disableddate = document.createElement("span");
    span_disableddate.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-image: linear-gradient(to bottom, #cc4400, #994411);");
    span_disableddate.textContent = '日期';
    div_comment.appendChild(span_disableddate);
    var span_disabledtext = document.createElement("span");
    span_disabledtext.textContent = ':代表不點名日期';
    div_comment.appendChild(span_disabledtext);

    row.cells[cellIndex].appendChild(td_Date);
    cellIndex++;

    // 點名裝置
    row.appendChild(document.createElement('td'));
    //var ui_rfiddeviceids = createRFIDDeviceList(rollcall.rollcallid, rollcall.rfiddeviceids);
    var ui_rfiddeviceids = createRollCallDeviceList(rollcall);
    row.cells[cellIndex].appendChild(ui_rfiddeviceids);
    cellIndex++;

    // subgroup
    row.appendChild(document.createElement('td'));
    var ui_subgroups = createSubgroupList(rollcall.rollcallid, rollcall.subgroups);
    row.cells[cellIndex].appendChild(ui_subgroups);
    cellIndex++;

    // presentnotify
    row.appendChild(document.createElement('td'));
    var div_presentnotify = document.createElement("div");
    div_presentnotify.setAttribute("id","div_presentnotify_"+rollcall.rollcallid);
    row.cells[cellIndex].appendChild(div_presentnotify);

    var item_presentnotify_checkbox = document.createElement("input");
    item_presentnotify_checkbox.setAttribute("id","checkbox_presentnotify_"+rollcall.rollcallid);
    item_presentnotify_checkbox.setAttribute("type","checkbox");
    item_presentnotify_checkbox.setAttribute("disabled",true);
    var item_presentnotify_checkbox_label = document.createElement("label");
    item_presentnotify_checkbox_label.setAttribute("id","checkbox_presentnotify_label_"+rollcall.rollcallid);
    item_presentnotify_checkbox_label.setAttribute("for","checkbox_presentnotify_"+rollcall.rollcallid);
    item_presentnotify_checkbox_label.textContent = "點名已到時, 發送推播. 推播內容:";
    div_presentnotify.appendChild(item_presentnotify_checkbox);
    div_presentnotify.appendChild(item_presentnotify_checkbox_label);
    if ( rollcall.presentnotify === '1' )
    {
        item_presentnotify_checkbox.checked = true;
        var item_presentcomment = create_comment_label(rollcall.presentcomment);
        item_presentcomment.setAttribute("id","label_presentcomment_"+rollcall.rollcallid);
        div_presentnotify.appendChild(item_presentcomment);
    }
    else
    {
        item_presentnotify_checkbox_label.setAttribute("style","color: gray;");
        var item_presentcomment = create_disable_comment_label(rollcall.presentcomment);
        item_presentcomment.setAttribute("id","label_presentcomment_"+rollcall.rollcallid);
        div_presentnotify.appendChild(item_presentcomment);
    }

    // absentnotify
    var div_absentnotify = document.createElement("div");
    div_absentnotify.setAttribute("id","div_absentnotify_"+rollcall.rollcallid);
    row.cells[cellIndex].appendChild(div_absentnotify);

    var item_absentnotify_checkbox = document.createElement("input");
    item_absentnotify_checkbox.setAttribute("id","checkbox_absentnotify_"+rollcall.rollcallid);
    item_absentnotify_checkbox.setAttribute("type","checkbox");
    item_absentnotify_checkbox.setAttribute("disabled",true);
    var item_absentnotify_checkbox_label = document.createElement("label");
    item_absentnotify_checkbox_label.setAttribute("id","checkbox_absentnotify_label_"+rollcall.rollcallid);
    item_absentnotify_checkbox_label.setAttribute("for","checkbox_absentnotify_"+rollcall.rollcallid);
    item_absentnotify_checkbox_label.textContent = "點名未到時, 發送推播. 推播內容:";
    div_absentnotify.appendChild(item_absentnotify_checkbox);
    div_absentnotify.appendChild(item_absentnotify_checkbox_label);
    if ( rollcall.absentnotify === '1' )
    {
        item_absentnotify_checkbox.checked = true;
        var item_absentcomment = create_comment_label(rollcall.absentcomment);
        item_absentcomment.setAttribute("id","label_absentcomment_"+rollcall.rollcallid);
        div_absentnotify.appendChild(item_absentcomment);
    }
    else
    {
        item_absentnotify_checkbox_label.setAttribute("style","color: gray;");
        var item_absentcomment = create_disable_comment_label(rollcall.absentcomment);
        item_absentcomment.setAttribute("id","label_absentcomment_"+rollcall.rollcallid);
        div_absentnotify.appendChild(item_absentcomment);
    }

    // 可重複推播
    var div_resend = document.createElement("div");
    div_resend.setAttribute("id","div_resend_"+rollcall.rollcallid);
    row.cells[cellIndex].appendChild(div_resend);

    var div_resend_checkbox = document.createElement("input");
    div_resend_checkbox.setAttribute("id","checkbox_resend_"+rollcall.rollcallid);
    div_resend_checkbox.setAttribute("type","checkbox");
    div_resend_checkbox.setAttribute("disabled",true);
    var div_resend_checkbox_label = document.createElement("label");
    div_resend_checkbox_label.setAttribute("id","checkbox_resend_label_"+rollcall.rollcallid);
    div_resend_checkbox_label.setAttribute("for","checkbox_resend_"+rollcall.rollcallid);
    div_resend_checkbox_label.textContent = "可重複推播";
    div_resend.appendChild(div_resend_checkbox);
    div_resend.appendChild(div_resend_checkbox_label);
    if ( rollcall.resend === '1' )
    {
        div_resend_checkbox.checked = true;
    }
    else
    {
        div_resend_checkbox_label.setAttribute("style","color: gray;");
    }
    cellIndex++;

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    // Edit button
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id","uiid_editbtn_"+rollcall.rollcallid);
    editBtn.setAttribute("title","Edit this rollcall");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_rollcall(\""+rollcall.rollcallid+"\");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","uiid_editimg_"+rollcall.rollcallid);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    // delete uesr button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("style","display:none");
    deleteBtn.setAttribute("id","uiid_deletebtn_"+rollcall.rollcallid);
    deleteBtn.setAttribute("title","Delete This RollCall");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","confirm_delete_rollcall(\""+rollcall.rollcallid+"\", \""+rollcall.rollcallname+"\");return false;");
    var imgDelete = document.createElement("i");
    imgDelete.setAttribute("class","fa fa-user-times");
    deleteBtn.appendChild(imgDelete);
    row.cells[cellIndex].appendChild(deleteBtn);
    cellIndex++;


    return row;
}

function create_weekday_ui(rollcallid, validdays, isEditing)
{
    var weekdaybox = document.createElement("div");
    weekdaybox.setAttribute("class","box");
    weekdaybox.setAttribute("style","background-image: linear-gradient(to bottom, #a9dbde, #e4eded);");

    var weekdayboxheader = document.createElement("div");
    weekdayboxheader.setAttribute("class","box-header with-border");
    weekdaybox.appendChild(weekdayboxheader);

    var weekdayboxheadertitle = document.createElement("div");
    weekdayboxheadertitle.appendChild(document.createTextNode("週間點名"));
    weekdayboxheadertitle.setAttribute("style","text-align:center;");
    weekdayboxheader.appendChild(weekdayboxheadertitle);

    var weekdayboxbody = document.createElement("div");
    weekdayboxbody.setAttribute("class","box-body no-padding");
    weekdaybox.appendChild(weekdayboxbody);

    var boxtable = document.createElement("table");
    boxtable.setAttribute("class","table table-bordered");
    boxtable.setAttribute("style","background-image: linear-gradient(to bottom, #e4eded, #e4eded);");
    weekdayboxbody.appendChild(boxtable);

    var boxtablethead = document.createElement("thead");
    boxtable.appendChild(boxtablethead);

    var boxtabletheadtr = document.createElement("tr");
    boxtable.appendChild(boxtabletheadtr);

    var boxtabletheadth1 = document.createElement("th");
    boxtabletheadth1.textContent = "一";
    boxtabletheadtr.appendChild(boxtabletheadth1);

    var boxtabletheadth2 = document.createElement("th");
    boxtabletheadth2.textContent = "二";
    boxtabletheadtr.appendChild(boxtabletheadth2);

    var boxtabletheadth3 = document.createElement("th");
    boxtabletheadth3.textContent = "三";
    boxtabletheadtr.appendChild(boxtabletheadth3);

    var boxtabletheadth4 = document.createElement("th");
    boxtabletheadth4.textContent = "四";
    boxtabletheadtr.appendChild(boxtabletheadth4);

    var boxtabletheadth5 = document.createElement("th");
    boxtabletheadth5.textContent = "五";
    boxtabletheadtr.appendChild(boxtabletheadth5);

    var boxtabletheadth6 = document.createElement("th");
    boxtabletheadth6.textContent = "六";
    boxtabletheadtr.appendChild(boxtabletheadth6);

    var boxtabletheadth7 = document.createElement("th");
    boxtabletheadth7.textContent = "日";
    boxtabletheadtr.appendChild(boxtabletheadth7);

    var boxtabletbody = document.createElement("tbody");
    boxtable.appendChild(boxtabletbody);

    var boxtabletbodytr = document.createElement("tr");
    boxtabletbody.appendChild(boxtabletbodytr);


    for(var i=1;i<=7;i++)
    {
        var day = ''+i;
        if ( i==7 )
            day = '0';
        var checked = false;
        if (validdays.indexOf(day) >= 0)
        {
            checked = true;
        }

        var boxtabletbodytd = document.createElement("td");
        var checkboxui = create_checkbox(rollcallid, day, checked, isEditing);
        boxtabletbodytd.appendChild(checkboxui);
        boxtabletbodytr.appendChild(boxtabletbodytd);
    }

    return weekdaybox;
}

function create_checkbox(rollcallid, day, checked, isEditing)
{
    var inputui = document.createElement("input");
    inputui.setAttribute("id","uiid_checkbox_"+day+"_"+rollcallid);
    inputui.setAttribute("type","checkbox");
    inputui.setAttribute("onclick","weekday_checkbox_click(\""+rollcallid+"\",\""+day+"\")");
    if (checked)
    {
        inputui.setAttribute("checked",checked);
    }
    if (!isEditing)
    {
        inputui.setAttribute("disabled",true);
    }
    return inputui;
}

function weekday_checkbox_click(rollcallid, day)
{
    var checkd = document.getElementById("uiid_checkbox_"+day+"_"+rollcallid).checked;

    var rollcall = getRollCall(rollcallid);

    var existed_index = rollcall.weekday.indexOf(day);
    if (checkd)
    {
        if (existed_index < 0)
        {
            rollcall.weekday.push(day);
            // 重新檢查 validdate  與  invaliddate
            check_invaliddate(rollcall, day);
        }
    }
    else
    {
        if (existed_index >= 0)
        {
            rollcall.weekday.splice(existed_index, 1);
            // 重新檢查 validdate  與  invaliddate
            check_validdate(rollcall, day);
        }
    }

    //glSetWeekDay = true;
    console.log("weekday_checkbox_click() day:"+day +" rollcallid:"+rollcallid +" checkd:"+checkd +" weekday:"+rollcall.weekday);

    var calendar_id = "div_calendar_"+rollcall.rollcallid;
    $('#'+calendar_id).datepicker('setDate', null);

}

// 重新檢查 validdate  與  invaliddate
function check_validdate(rollcall, day)
{
    // 檢查是否有 日期 在不要點名 週間
    // for (var index in rollcall.validdate)
    var index = rollcall.validdate.length;
    while (index--)
    {
        var existed_date = rollcall.validdate[index];
        const date = moment(existed_date);
        const dow = date.day();

        // console.log("check_validdate() existed_date:"+existed_date +" dow:"+dow +" day:"+day);

        if ( dow == day )
        {
            // 移除這個日期
            console.log("check_validdate() 移除這個日期 existed_date:"+existed_date);
            rollcall.validdate.splice(index, 1);
        }
        //var existed_index = rollcall.weekday.indexOf(''+dow);

        // console.log("check_validdate() existed_date:"+existed_date +" dow:"+dow +" existed_index:"+existed_index);

        // if (existed_index < 0)
        // {
        //     // 移除這個日期
        //     console.log("check_validdate() 移除這個日期 existed_date:"+existed_date);
        //
        // }
    }

}

function check_invaliddate(rollcall, day)
{
    // 檢查是否有 日期 在要點名 週間
    var index = rollcall.invaliddate.length;
    while (index--)
    {
        var existed_date = rollcall.invaliddate[index];
        const date = moment(existed_date);
        const dow = date.day();

        // console.log("check_invaliddate() existed_date:"+existed_date +" dow:"+dow +" day:"+day);

        if ( dow == day )
        {
            // 移除這個日期
            console.log("check_invaliddate() 移除這個日期 existed_date:"+existed_date);
            rollcall.invaliddate.splice(index, 1);
        }
    }


    // for (day in rollcall.weekday)
    // {
    //     console.log("check_invaliddate() day:"+day +" rollcallid:"+rollcall.rollcallid);
    //
    // }
}

function createRollCallDeviceList(rollcall)
{
    var span_devicelist = document.createElement("span");
    span_devicelist.setAttribute("id","uiid_devicelist_"+rollcall.rollcallid);

    if ( rollcall.rfiddeviceids.length > 0 )
    {
        var span_rfid_devicelist = create_rfid_devicelist_ui(rollcall.rollcallid, rollcall.rfiddeviceids);
        span_devicelist.appendChild(span_rfid_devicelist);
    }

    if ( rollcall.nfclocators.length > 0 )
    {
        var span_nfc_devicelist = create_nfc_devicelist_ui(rollcall.rollcallid, rollcall.nfclocators);
        span_devicelist.appendChild(span_nfc_devicelist);
    }
    return span_devicelist;
}

function create_rfid_devicelist_ui(rollcallid, rfiddeviceids)
{
    var rfiddeviceidList = rfiddeviceids.split(",");
    var rfiddevice_span = document.createElement("span");
    rfiddevice_span.setAttribute("id","uiid_uhfreader_"+rollcallid);

    for(var index=0;index<rfiddeviceidList.length;index++)
    {
        var rfiddevice_div = document.createElement("div");
        var label = create_rfiddevice_label(getRFIDDeviceName(rfiddeviceidList[index]));
        rfiddevice_div.appendChild(label);
        rfiddevice_span.appendChild(rfiddevice_div);
    }

    return rfiddevice_span;
}

function create_nfc_devicelist_ui(rollcallid, nfclocators)
{
    var nfclocatorList = nfclocators.split(",");
    var nfclocator_span = document.createElement("span");
    nfclocator_span.setAttribute("id","uiid_nfclocator_"+rollcallid);

    for(var index=0;index<nfclocatorList.length;index++)
    {
        var nfclocator_div = document.createElement("div");
        var label = create_nfclocator_label(getNFCLocatorDeviceName(nfclocatorList[index]));
        nfclocator_div.appendChild(label);
        nfclocator_span.appendChild(nfclocator_div);
    }

    return nfclocator_span;
}

// function createRFIDDeviceList(rollcallid, rfiddeviceids)
// {
//     var rfiddeviceidList = rfiddeviceids.split(",");
//     var rfiddevice_span = document.createElement("span");
//     rfiddevice_span.setAttribute("id","uiid_rfiddevice_"+rollcallid);
//
//     for(var index=0;index<rfiddeviceidList.length;index++)
//     {
//         var rfiddevice_div = document.createElement("div");
//         var label = create_rfiddevice_label(getRFIDDeviceName(rfiddeviceidList[index]));
//         rfiddevice_div.appendChild(label);
//         rfiddevice_span.appendChild(rfiddevice_div);
//     }
//
//     return rfiddevice_span;
// }

function createSubgroupList(rollcallid, subgroups)
{
    var subgroupList = subgroups.split(",");
    var subgroup_span = document.createElement("span");
    subgroup_span.setAttribute("id","uiid_subgroup_"+rollcallid);

    for(var index=0;index<subgroupList.length;index++)
    {
        var subgroup_div = document.createElement("div");
        var subgroupName = getSubgroupName(subgroupList[index]);
        if (subgroupName.length > 0)
        {
            var label = create_subgroup_label(subgroupName);
            subgroup_div.appendChild(label);
            subgroup_span.appendChild(subgroup_div);
        }
    }

    return subgroup_span;
}

function create_comment_label(context)
{
    var comment_span = document.createElement("span");
    comment_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(54, 127, 169);");
    comment_span.textContent = context;
    return comment_span;
}

function create_disable_comment_label(context)
{
    var comment_span = document.createElement("span");
    comment_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(214, 214, 214);");
    comment_span.textContent = context;
    return comment_span;
}

function create_rfiddevice_label(context)
{
    var rfiddevice_span = document.createElement("span");
    //rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a7d0;");
    rfiddevice_span.textContent = context;
    return rfiddevice_span;
}

function create_nfclocator_label(context)
{
    var rfiddevice_span = document.createElement("span");
    //rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#f39c12;");
    rfiddevice_span.textContent = context;
    return rfiddevice_span;
}

function create_subgroup_label(context)
{
    var subgroup_span = document.createElement("span");
    subgroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    subgroup_span.textContent = context;
    return subgroup_span;
}

function edit_rollcall(rollcallid)
{
    console.log("edit_rollcall() rollcallid:"+rollcallid);

    var rollcall = getRollCall(rollcallid);
    if (rollcall == null)
        return;

    create_edit_ui("uiid_name", rollcallid, rollcall.rollcallname);

    clearChildView("rollcall_StartTime_"+rollcall.rollcallid);
    clearChildView("rollcall_EndTime_"+rollcall.rollcallid);

    var td_StartTime = document.getElementById("rollcall_StartTime_"+rollcall.rollcallid);
    var td_EndTime = document.getElementById("rollcall_EndTime_"+rollcall.rollcallid);

    var div_StartTime_TimePicker = create_time_picker("rollcall_StartTime_"+rollcall.rollcallid+"_edit", rollcall.StartTime);
    td_StartTime.appendChild(div_StartTime_TimePicker);
    var div_EndTime_TimePicker = create_time_picker("rollcall_EndTime_"+rollcall.rollcallid+"_edit", rollcall.EndTime);
    td_EndTime.appendChild(div_EndTime_TimePicker);

    var weekday_id = "div_weekday_"+rollcall.rollcallid;
    clearChildView(weekday_id);
    var weekdaydiv = document.getElementById(weekday_id);
    var weekdayui = create_weekday_ui(rollcall.rollcallid, rollcall.weekday, true);
    weekdaydiv.appendChild(weekdayui);

    var calendar_id = "div_calendar_"+rollcall.rollcallid;
    $('#'+calendar_id).datepicker('destroy');
    var isEditing = true;
    $('#'+calendar_id).datepicker(
    {
        rollcallid: rollcall.rollcallid,
        language: 'zh-TW',
        weekStart: 1,
        beforeShowDay: function (date, rollcallid)
        {
            //console.log("edit_rollcall():beforeShowDay id:"+id);
            //console.log("edit_rollcall():beforeShowDay this.picker.id:"+$(this).picker.id);

            var rollcall = getRollCall(rollcallid);
            return checkValidDate(date, rollcall, isEditing, false);
        }
    }).on('changeDate', function (ev)
    {
        // console.log("edit_rollcall() changeDate() ev.rollcallid:"+ev.rollcallid);
        // console.log("edit_rollcall() changeDate() ev.date:"+ev.date);
        if (ev.rollcallid != undefined && ev.date != undefined)
        {
            var rollcall = getRollCall(ev.rollcallid);
            checkValidDate(ev.date, rollcall, isEditing, true);

            $('#'+calendar_id).datepicker("update", ev.date);
        }
    });
    $('#'+calendar_id).datepicker('setDate', null);

    clearChildView("uiid_devicelist_"+rollcall.rollcallid);
    var devicelist_span = document.getElementById("uiid_devicelist_"+rollcall.rollcallid);

    var div_devicelist = create_edit_devicelist_ui("rollcall_devicelist", rollcall.rollcallid, rollcall.rfiddeviceids, rollcall.nfclocators);
    devicelist_span.appendChild(div_devicelist);

    // clearChildView("uiid_rfiddevice_"+rollcall.rollcallid);
    // var rfiddevice_span = document.getElementById("uiid_rfiddevice_"+rollcall.rollcallid);
    // var div_devicelist = create_edit_devicelist_ui("rollcall_devicelist", rollcall.rollcallid, rollcall.rfiddeviceids, rollcall.nfclocators);
    // rfiddevice_span.appendChild(div_devicelist);

    clearChildView("uiid_subgroup_"+rollcall.rollcallid);
    var subgroup_span = document.getElementById("uiid_subgroup_"+rollcall.rollcallid);
    var div_subgroups = create_edit_subgrouplist_ui("rollcall_subgrouplist", rollcall.rollcallid, rollcall.subgroups);
    subgroup_span.appendChild(div_subgroups);

    var item_presentnotify_checkbox = document.getElementById("checkbox_presentnotify_"+rollcall.rollcallid);
    item_presentnotify_checkbox.disabled = false;

    var item_presentnotify_checkbox_label = document.getElementById("checkbox_presentnotify_label_"+rollcall.rollcallid);
    item_presentnotify_checkbox_label.setAttribute("style","color: black;");

    var item_absentnotify_checkbox = document.getElementById("checkbox_absentnotify_"+rollcall.rollcallid);
    item_absentnotify_checkbox.disabled = false;

    var item_absentnotify_checkbox_label = document.getElementById("checkbox_absentnotify_label_"+rollcall.rollcallid);
    item_absentnotify_checkbox_label.setAttribute("style","color: black;");

    var label_presentcomment = document.getElementById("label_presentcomment_"+rollcall.rollcallid);
    var label_absentcomment = document.getElementById("label_absentcomment_"+rollcall.rollcallid);

    label_presentcomment.parentNode.removeChild(label_presentcomment);
    label_absentcomment.parentNode.removeChild(label_absentcomment);

    var div_presentnotify = document.getElementById("div_presentnotify_"+rollcall.rollcallid);
    var div_absentnotify = document.getElementById("div_absentnotify_"+rollcall.rollcallid);

    var new_presentcomment_input = create_input_text("input_presentcomment_"+rollcall.rollcallid, rollcall.presentcomment);
    div_presentnotify.appendChild(new_presentcomment_input);

    var new_absentcomment_input = create_input_text("input_absentcomment_"+rollcall.rollcallid, rollcall.absentcomment);
    div_absentnotify.appendChild(new_absentcomment_input);

    var item_resend_checkbox = document.getElementById("checkbox_resend_"+rollcall.rollcallid);
    item_resend_checkbox.disabled = false;

    var item_resend_checkbox_label = document.getElementById("checkbox_resend_label_"+rollcall.rollcallid);
    item_resend_checkbox_label.setAttribute("style","color: black;");

    var editBtn = document.getElementById("uiid_editbtn_"+rollcallid);
    editBtn.setAttribute("title","Save this RollCall");
    editBtn.setAttribute("onclick","save_rollcall(\""+rollcallid+"\");return false;");
    var imgEdit = document.getElementById("uiid_editimg_"+rollcallid);
    imgEdit.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("uiid_deletebtn_"+rollcallid);
    deleteBtn.setAttribute("style","display:block");

    $('.timepicker').timepicker({
      showInputs: false
    })
}

function create_edit_ui(uiid, rollcallid, text)
{
    var uiid_name = clearChildView(uiid+"_"+rollcallid);
    var new_input = create_input_text(uiid+"_"+rollcallid+"_edit", text);
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

function save_rollcall(rollcallid)
{
    var rollcallname = document.getElementById("uiid_name_"+rollcallid+"_edit").value;

    var StartTime = document.getElementById("rollcall_StartTime_"+rollcallid+"_edit").value;
    var EndTime = document.getElementById("rollcall_EndTime_"+rollcallid+"_edit").value;

    var weekdays = [];
    if ( document.getElementById("uiid_checkbox_1_"+rollcallid).checked )
        weekdays.push('1');
    if ( document.getElementById("uiid_checkbox_2_"+rollcallid).checked )
        weekdays.push('2');
    if ( document.getElementById("uiid_checkbox_3_"+rollcallid).checked )
        weekdays.push('3');
    if ( document.getElementById("uiid_checkbox_4_"+rollcallid).checked )
        weekdays.push('4');
    if ( document.getElementById("uiid_checkbox_5_"+rollcallid).checked )
        weekdays.push('5');
    if ( document.getElementById("uiid_checkbox_6_"+rollcallid).checked )
        weekdays.push('6');
    if ( document.getElementById("uiid_checkbox_0_"+rollcallid).checked )
        weekdays.push('0');


    var rollcallrecord = getRollCall(rollcallid);
    var validdate = rollcallrecord.validdate;
    var invaliddate = rollcallrecord.invaliddate;

    var rfiddeviceids = getSelected_uhfreader_Device(rollcallid);
    var nfclocators = getSelected_nfclocator_Device(rollcallid);
    var subgroups = getSelectedSubgroup(rollcallid);

    var presentnotify = 0;
    if ( document.getElementById("checkbox_presentnotify_"+rollcallid).checked )
        presentnotify = 1;
    var presentcomment = document.getElementById("input_presentcomment_"+rollcallid).value;
    var absentnotify = 0;
    if ( document.getElementById("checkbox_absentnotify_"+rollcallid).checked )
        absentnotify = 1;
    var absentcomment = document.getElementById("input_absentcomment_"+rollcallid).value;

    var resend = 0;
    if ( document.getElementById("checkbox_resend_"+rollcallid).checked )
        resend = 1;

    // console.log("save_rollcall() rollcallname:"+rollcallname);
    // console.log("save_rollcall() StartTime:"+StartTime+" EndTime:"+EndTime);
    // console.log("save_rollcall() weekdays:"+weekdays);
    // console.log("save_rollcall() presentnotify:"+presentnotify+" absentnotify:"+absentnotify);
    // console.log("save_rollcall() presentcomment:"+presentcomment+" absentcomment:"+absentcomment);

    if ( rollcallname.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    if ( StartTime === EndTime )
    {
        alert("請選擇不同時段");
        return;
    }

    update_rollcall(rollcallid, rollcallname, StartTime, EndTime, rfiddeviceids, nfclocators, subgroups, weekdays.toString(), validdate.toString(), invaliddate.toString(), presentnotify, presentcomment, absentnotify, absentcomment, resend);
}

function update_rollcall(rollcallid, rollcallname, StartTime, EndTime, rfiddeviceids, nfclocators, subgroups, weekdays, validdate, invaliddate, presentnotify, presentcomment, absentnotify, absentcomment, resend)
{
    // console.log("update_rollcall() rollcallid:"+rollcallid);
    // console.log("update_rollcall() rfiddeviceids:"+rfiddeviceids);
    // console.log("update_rollcall() nfclocators:"+nfclocators);
    console.log("update_rollcall() validdate:"+validdate);
    console.log("update_rollcall() invaliddate:"+invaliddate);

    if ( rollcallname.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    if ( (rfiddeviceids.length == 0) && (nfclocators.length == 0) )
    {
        alert("請選擇使用點名裝置");
        return;
    }

    if ( StartTime === EndTime )
    {
        alert("請選擇不同時段");
        return;
    }

    var rollcall = getRollCall(rollcallid);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_cardrollcall'] = 1;
    jsondata['rollcallid'] = rollcallid;
    jsondata['rollcallname'] = rollcallname;
    jsondata['StartTime'] = StartTime;
    jsondata['EndTime'] = EndTime;
    jsondata['StartDate'] = rollcall.StartDate;
    jsondata['EndDate'] = rollcall.EndDate;
    jsondata['rfiddeviceids'] = rfiddeviceids;
    jsondata['nfclocators'] = nfclocators;
    jsondata['subgroups'] = subgroups;
    jsondata['weekday'] = weekdays;
    jsondata['validdate'] = validdate;
    jsondata['invaliddate'] = invaliddate;
    jsondata['presentnotify'] = presentnotify;
    jsondata['presentcomment'] = presentcomment;
    jsondata['absentnotify'] = absentnotify;
    jsondata['absentcomment'] = absentcomment;
    jsondata['maingroups'] = rollcall.maingroups;
    jsondata['type'] = rollcall.type;
    jsondata['curriculumrecordid'] = rollcall.curriculumrecordid;

    var targetURL = gl_target_server + "/php/rfiddevice.php";

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

            setTimeout(reload_rollcall_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function save_new_rollcall()
{
    var rollcallname = document.getElementById("input_name").value;
    var StartTime = document.getElementById("rollcall_StartTime").value;
    var EndTime = document.getElementById("rollcall_EndTime").value;

    var weekday = gl_NewRollCall.weekday.toString();
    var validdate = gl_NewRollCall.validdate.toString();
    var invaliddate = gl_NewRollCall.invaliddate.toString();

    var rfiddeviceids = getSelected_uhfreader_Device("new");
    var nfclocators = getSelected_nfclocator_Device("new");
    var subgroups = getSelectedSubgroup("new");

    // console.log("save_new_rollcall() rfiddeviceids:"+rfiddeviceids);
    // console.log("save_new_rollcall() nfclocators:"+nfclocators);
    // console.log("save_new_rollcall() subgroups:"+subgroups);

    var presentnotify = 0;
    if ( document.getElementById("checkbox_presentnotify").checked )
        presentnotify = 1;
    var presentcomment = document.getElementById("input_presentcomment").value;
    var absentnotify = 0;
    if ( document.getElementById("checkbox_absentnotify").checked )
        absentnotify = 1;
    var absentcomment = document.getElementById("input_absentcomment").value;

    var resend = 0;
    if ( document.getElementById("checkbox_resend").checked )
        resend = 1;

    if ( rollcallname.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    if ( (rfiddeviceids.length == 0) && (nfclocators.length == 0) )
    {
        alert("請選擇使用點名裝置");
        return;
    }

    if ( StartTime === EndTime )
    {
        alert("請選擇不同時段");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_cardrollcall'] = 1;
    jsondata['rollcallname'] = rollcallname;
    jsondata['StartTime'] = StartTime;
    jsondata['EndTime'] = EndTime;
    jsondata['StartDate'] = '';
    jsondata['EndDate'] = '';
    jsondata['rfiddeviceids'] = rfiddeviceids;
    jsondata['nfclocators'] = nfclocators;
    jsondata['subgroups'] = subgroups;
    jsondata['weekday'] = weekday;
    jsondata['validdate'] = validdate;
    jsondata['invaliddate'] = invaliddate;
    jsondata['presentnotify'] = presentnotify;
    jsondata['presentcomment'] = presentcomment;
    jsondata['absentnotify'] = absentnotify;
    jsondata['absentcomment'] = absentcomment;
    jsondata['resend'] = resend;
    jsondata['maingroups'] = '';
    jsondata['type'] = '0';
    jsondata['curriculumrecordid'] = '0';

    var targetURL = gl_target_server + "/php/rfiddevice.php";

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
            setTimeout(reload_rollcall_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_rollcall(rollcallid, rollcallname)
{
    if (confirm('Are you sure you want to delete this rollcall:'+rollcallname+' ?'))
    {
        // delete it!
        delete_rollcall(rollcallid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_rollcall(rollcallid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_cardrollcall'] = 1;
    jsondata['rollcallid'] = rollcallid;

    var targetURL = gl_target_server + "/php/rfiddevice.php";

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
            setTimeout(reload_rollcall_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function create_new_cardrollcall()
{
    create_empty_record_table_item(gl_cardrollcall.length + 1);

    $('.timepicker').timepicker({
      showInputs: false
    })

    var calendar_id = "div_calendar_88888";

    var isEditing = true;
    $('#'+calendar_id).datepicker(
    {
        rollcallid: 88888,
        language: 'zh-TW',
        weekStart: 1,
        beforeShowDay: function (date, rollcallid)
        {
            var rollcall = getRollCall(rollcallid);
            return checkValidDate(date, rollcall, isEditing, false);
        }
    }).on('changeDate', function (ev)
    {
        if (ev.rollcallid != undefined && ev.date != undefined)
        {
            var rollcall = getRollCall(ev.rollcallid);
            checkValidDate(ev.date, rollcall, isEditing, true);

            $('#'+calendar_id).datepicker("update", ev.date);
        }
    });

    return false;
}

// $("#createnewcardrollcallbutton").click(function()
// {
//     create_empty_record_table_item(gl_cardrollcall.length + 1);
//
//     $('.timepicker').timepicker({
//       showInputs: false
//     })
//
//     var calendar_id = "div_calendar_88888";
//
//     var isEditing = true;
//     $('#'+calendar_id).datepicker(
//     {
//         rollcallid: 88888,
//         language: 'zh-TW',
//         weekStart: 1,
//         beforeShowDay: function (date, rollcallid)
//         {
//             var rollcall = getRollCall(rollcallid);
//             return checkValidDate(date, rollcall, isEditing, false);
//         }
//     }).on('changeDate', function (ev)
//     {
//         if (ev.rollcallid != undefined && ev.date != undefined)
//         {
//             var rollcall = getRollCall(ev.rollcallid);
//             checkValidDate(ev.date, rollcall, isEditing, true);
//
//             $('#'+calendar_id).datepicker("update", ev.date);
//         }
//     });
//
//     return false;
// });

function reload_rollcall_info()
{
    PreLoadIndex = -1;
    CurrentLoadIndex = 0;
    check_loading_status();
}

function getRollCall(rollcallid)
{
    var retRollCall = null;
    for (var i = 0; i < gl_cardrollcall.length; ++i)
    {
        var rollcall = gl_cardrollcall[i];

        if ( rollcall.rollcallid === rollcallid )
        {
            retRollCall = rollcall;
            break;
        }

    }

    if ( rollcallid == 88888 )
    {
        return gl_NewRollCall;
    }

    return retRollCall;
}

function getRFIDDeviceName(deviceid)
{
    var RFIDDeviceName = deviceid;
    for (var i = 0; i < gl_rfiddevices.length; ++i)
    {
        var rfiddevice = gl_rfiddevices[i];

        if ( rfiddevice.deviceid === deviceid )
        {
            RFIDDeviceName = rfiddevice.name;
            break;
        }

    }
    return RFIDDeviceName;
}

function getNFCLocatorDeviceName(macaddress)
{
    var NFCLocatorName = macaddress;
    for (var i = 0; i < gl_nfclocators.length; ++i)
    {
        var nfclocator = gl_nfclocators[i];

        if ( nfclocator.macaddress === macaddress )
        {
            NFCLocatorName = nfclocator.name;
            break;
        }

    }
    return NFCLocatorName;
}

function getSubgroupName(subgroupid)
{
    var subgroupname = '';

    // console.log("getSubgroupName() gl_usersubgroups.length:"+gl_usersubgroups.length);

    for (var i = 0; i < gl_usersubgroups.length; ++i)
    {
        var subgroup = gl_usersubgroups[i];

        // console.log("getSubgroupName() subgroup.subgroupid:"+subgroup.subgroupid + " subgroupid:"+subgroupid);

        if ( subgroup.subgroupid == subgroupid )
        {
            subgroupname = subgroup.subgroupname;
            break;
        }

    }

    // console.log("getSubgroupName() subgroupname:"+subgroupname + " subgroupid:"+subgroupid);

    return subgroupname;
}

function check_numer_in_list(targetNumber, dataList)
{
    var existed = false;
    for (var i = 0; i < dataList.length; ++i)
    {
        var data = dataList[i];

        if ( parseInt(data) == targetNumber )
        {
            existed = true;
            break;
        }
    }

    return existed;
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

        reload_rollcall_info();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    glCurrentPage++;
    current_page_num_ui.textContent = glCurrentPage;

    reload_rollcall_info();
}

function open_cardrollcall_page()
{
    var win = window.open('/index.php/vilscardrollcall/'+project_id, '_blank');
    win.focus();
}
