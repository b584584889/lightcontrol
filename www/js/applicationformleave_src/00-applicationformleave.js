/*
 * Author: ikki Chung
 * Date: 2020.07.23
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


//
// globa value
//
var LOAD_PAGE_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var glFirstTimeLoad = 1;
var glWaitWebSocket = 0;
var glCurrentPage = 1;

var gl_LeaveFormRecords = [];
var gl_signoffuser = [];
var gl_user_info = [];
var gl_rfiddevices = [];
var gl_nfclocators = [];

var gl_uploadfile_uid = '';
var gl_uploadimage_uid = '';
var gl_LoadSingleUser = '';

var gl_signoffrecords = [];

var LOAD_USER_NUM = 500;
var gl_user_load_index = 0;

check_loading_status();

function check_loading_status()
{
    //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" CurrentLoadIndex:" + CurrentLoadIndex);
    switch(CurrentLoadIndex)
    {
        case 0:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                check_ui_enable();

                load_leaveformrecords();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                gl_user_info = [];
                gl_user_load_index = 0;
                load_user_info();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_signoff_user_info();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                draw_table();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_leaveform_response();

                create_targetdatetime_dropdown_list();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_rfiddevice();

            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_nfclocator();

            }
        break;
        case 7:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
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

function reload_form_info()
{
    PreLoadIndex = -1;
    CurrentLoadIndex = 0;
    check_loading_status();
}

function check_ui_enable()
{
    if ( gl_groupid == 3)
    {
        var filter_none_signoff = document.getElementById('FILTER_NONE_SIGNOFF');
        filter_none_signoff.setAttribute("style","display:none;");
    }
    else
    {
        var create_new_form = document.getElementById('CREATE_NEW_LEAVEFORM');
        create_new_form.setAttribute("style","display:none;");
    }

    var targetdatetime_range_selector = document.getElementById('targetdatetime_range_selector');
    var rollcalldevice_dropdown_selector = document.getElementById('rollcalldevice_dropdown_selector');

    var current_targetdatetime = document.getElementById('current_targetdatetime');
    var type = current_targetdatetime.value;
    if (type == 5)
    {
        targetdatetime_range_selector.setAttribute("style","display:block;");
        rollcalldevice_dropdown_selector.setAttribute("style","display:none;");
    }
    else
    if (type == 6)
    {
        targetdatetime_range_selector.setAttribute("style","display:none;");
        rollcalldevice_dropdown_selector.setAttribute("style","display:block;");
    }
    else
    {
        targetdatetime_range_selector.setAttribute("style","display:none;");
        rollcalldevice_dropdown_selector.setAttribute("style","display:none;");
    }

    if(glFirstTimeLoad == 1)
    {
        //Date picker
        $('#targetdatetime_startdate').datepicker({
            autoclose: true,
            format: "yyyy/mm/dd"
        });
        $('#targetdatetime_startdate').datepicker("setDate", new Date());

        $('#targetdatetime_enddate').datepicker({
            autoclose: true,
            format: "yyyy/mm/dd"
        });
        $('#targetdatetime_enddate').datepicker("setDate", new Date());

        glFirstTimeLoad = 0;
    }
}

function load_leaveformrecords()
{
    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    gl_LeaveFormRecords = [];
    gl_signoffuser = [];

    var filter_none_signoff_checkbox = document.getElementById('FILTER_NONE_SIGNOFF_CHECKBOX');
    var load_none_signoff = filter_none_signoff_checkbox.checked;
    var status = -1;
    if (filter_none_signoff_checkbox.checked)
    {
        status = 0;
    }

    var startdate='';
    var enddate='';
    var LoadSingelUser='';

    var current_targetdatetime = document.getElementById('current_targetdatetime');
    var type = current_targetdatetime.value;

    console.log("load_leaveformrecords() type:"+type);
    if ( type == 0 )
    {
        // "依最近時間"
    }
    else
    if ( type == 1 )
    {
        // "本日"
        startdate = moment().format("YYYY/MM/DD");
        enddate = moment().format("YYYY/MM/DD");
    }
    else
    if ( type == 2 )
    {
        // "本週"
        startdate = moment().startOf('week').format("YYYY/MM/DD");
        enddate = moment().endOf('week').format("YYYY/MM/DD");
    }
    else
    if ( type == 3 )
    {
        // "本月"
        startdate = moment().startOf('month').format("YYYY/MM/DD");
        enddate = moment().endOf('month').format("YYYY/MM/DD");
    }
    else
    if ( type == 4 )
    {
        // "接下來7天"
        startdate = moment().format("YYYY/MM/DD");
        var today = moment();
        enddate = moment(today).add(7, 'days').format("YYYY/MM/DD");
    }
    else
    if ( type == 5 )
    {
        //"特定期間"
        startdate = document.getElementById("targetdatetime_startdate").value;
        enddate = document.getElementById("targetdatetime_enddate").value;
    }
    else
    if ( type == 6 )
    {
        LoadSingelUser = gl_LoadSingleUser;
        if ( LoadSingelUser.length == 0 )
        {
            CurrentLoadIndex = CurrentLoadIndex + 1;
            return;
        }
    }

    var targetURL = gl_target_server + "/php/applicationform.php";
    var queryString = 'project_id='+project_id+'&loadleaveformrecord=1&s='+offset+'&count='+count+'&status='+status+'&startdate='+startdate+'&enddate='+enddate;

    if ( LoadSingelUser.length > 0 )
    {
        queryString += '&singleuser='+LoadSingelUser;
    }

    $.getJSON(targetURL, queryString, function(data)
    {
        var index = 0;
        $.each(data, function(itemindex, record)
        {
            gl_LeaveFormRecords.push(record);

            var found = false;
            for (var i = 0; i < gl_signoffuser.length; ++i)
            {
                var signoffuser = gl_signoffuser[i];
                if ( signoffuser === record.signoffuserid )
                {
                    found = true;
                    break;
                }
            }
            if ( found == false && record.signoffuserid.length > 0)
            {
                gl_signoffuser.push(record.signoffuserid);
            }
            index++;
        });

        CurrentLoadIndex = CurrentLoadIndex + 1;
    });
}

function load_user_info()
{
    var user_num = 0;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduser=1&s='+gl_user_load_index+'&count='+LOAD_USER_NUM+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, user)
        {
            user.userid = parseInt(user.userid);
            gl_user_info.push(user);

            user_num += 1;
        });

        gl_user_load_index += user_num;
    })
    .success(function() {
        if (user_num < LOAD_USER_NUM)
        {
            CurrentLoadIndex = CurrentLoadIndex + 1;
        }
        else
        {
            setTimeout(load_user_info, 20);
        }
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .complete(function() {
    });
}

function load_signoff_user_info()
{
    if ( gl_signoffuser.length == 0 )
    {
        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }
    else
    {
        var queryUser = gl_signoffuser.toString();

        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var p = $.getJSON(targetURL, 'loadusername=1&project_id='+project_id+'&user='+queryUser, function(data)
        {
            $.each(data, function(index, user)
            {
                user.userid = parseInt(user.userid);
                gl_user_info.push(user);
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
}

function draw_table()
{
    var record_table = document.getElementById('APPFORM_TABLE_BODY');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

     // console.log("draw_table() gl_LeaveFormRecords.length:"+gl_LeaveFormRecords.length);

    for (var i = 0; i < gl_LeaveFormRecords.length; ++i)
    {
        var record = gl_LeaveFormRecords[i];

        var row = create_record_table_item(i, record);
        record_table.appendChild(row);
    }

    if ( gl_LeaveFormRecords.length == 0 )
    {
        var row = document.createElement('tr');
        var col1 = document.createElement('td');
        col1.appendChild(document.createTextNode('無資料'));
        row.appendChild(col1);
        record_table.appendChild(row);
    }

}

function create_record_table_item(ui_index, record)
{
    if (record == null)
        return null;

    var row = document.createElement('tr');
    row.setAttribute("id", record.formid);
    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    var cellIndex = 0;

    // 表單編號
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(record.formid));
    cellIndex++;

    // 申請時間
    var tdName = document.createElement('td');
    tdName.setAttribute("style","height: 90px;");
    row.appendChild(tdName);
    var Spandatetime = document.createElement("span");
    Spandatetime.setAttribute("class","badge bg-green-active");
    Spandatetime.textContent = record.applytime;
    row.cells[cellIndex].appendChild(Spandatetime);
    cellIndex++;

    // 請假人員
    var tdType = document.createElement('td');
    tdName.setAttribute("style","height: 90px;");
    row.appendChild(tdType);
    var SpanUserName = document.createElement("span");
    SpanUserName.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(54, 127, 169);");
    SpanUserName.textContent = getUserName(record.userid);
    row.cells[cellIndex].appendChild(SpanUserName);
    // var divaccount = document.createElement("div");
    // divaccount.textContent = '['+record.accountid+']';
    // row.cells[cellIndex].appendChild(divaccount);

    cellIndex++;

    // 假勤項目
    row.appendChild(document.createElement('td'));
    var Spanunit = document.createElement("span");
    var typelabel = create_leavetype_label(record.type);
    row.cells[cellIndex].appendChild(typelabel);
    cellIndex++;

    // 事由
    row.appendChild(document.createElement('td'));
    var Spanmanager = document.createElement("span");
    Spanmanager.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    //Spanmanager.setAttribute("class","badge bg-blue");
    Spanmanager.textContent = record.reason;
    row.cells[cellIndex].appendChild(Spanmanager);
    cellIndex++;

    // 請假時間
    row.appendChild(document.createElement('td'));

    var badge_StartDate = document.createElement("span");
    badge_StartDate.setAttribute("class","label label-info");
    badge_StartDate.textContent = record.startdate;
    row.cells[cellIndex].appendChild(badge_StartDate);

    var badge_StartTime = document.createElement("span");
    badge_StartTime.setAttribute("class","label label-warning");
    badge_StartTime.textContent = record.starttime;
    row.cells[cellIndex].appendChild(badge_StartTime);

    var Spanassetlife = document.createElement("div");
    Spanassetlife.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;text-align:left;");
    Spanassetlife.textContent = ' 到 ';
    row.cells[cellIndex].appendChild(Spanassetlife);

    var badge_EndDate = document.createElement("span");
    badge_EndDate.setAttribute("class","label label-info");
    badge_EndDate.textContent = record.enddate;
    row.cells[cellIndex].appendChild(badge_EndDate);

    var badge_EndTime = document.createElement("span");
    badge_EndTime.setAttribute("class","label label-warning");
    badge_EndTime.textContent = record.endtime;
    row.cells[cellIndex].appendChild(badge_EndTime);

    cellIndex++;

    // 補請假
    row.appendChild(document.createElement('td'));
    var Spanreissue = document.createElement("span");
    if (record.reissue == 0)
    {
        Spanreissue.setAttribute("class","label label-primary");
        Spanreissue.textContent = '否';
    }
    else
    {
        Spanreissue.setAttribute("class","label label-danger");
        Spanreissue.textContent = '是';
    }
    row.cells[cellIndex].appendChild(Spanreissue);
    cellIndex++;

    // upload image
    row.appendChild(document.createElement('td'));
    var DivImage = document.createElement("div");
    DivImage.setAttribute("id","storage_image_"+record.formid);
    if (record.imageids.length > 0)
    {
        fetch_storage_image(record.formid, record.imageids);
    }
    // var Spanstatus = document.createElement("span");
    // Spanstatus.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    // Spanstatus.textContent = record.imageids;
    row.cells[cellIndex].appendChild(DivImage);
    cellIndex++;

    // upload file
    row.appendChild(document.createElement('td'));
    var DivFile = document.createElement("div");
    DivFile.setAttribute("id","storage_file_"+record.formid);
    if (record.fileids.length > 0)
    {
        fetch_storage_file(record.formid, record.fileids);
    }
    // var Spanstatus = document.createElement("span");
    // Spanstatus.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    // Spanstatus.textContent = record.fileids;
    row.cells[cellIndex].appendChild(DivFile);
    cellIndex++;

    // 申請人姓名
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_applyuser_label(record.applyusername));
    cellIndex++;

    // 申請人關係
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_relation_label(record.applyuserrelation));
    cellIndex++;

    // 簽核流程
    row.appendChild(document.createElement('td'));
    var Spansignoffstatus = document.createElement("span");
    Spansignoffstatus.setAttribute("id","signoffstatus_"+record.formid);
    if (record.signoffstatus == 0)
    {
        Spansignoffstatus.textContent = '簽核中';
        Spansignoffstatus.setAttribute("class","badge bg-blue");
    }
    else
    if (record.signoffstatus == 1)
    {
        Spansignoffstatus.textContent = '已簽核';
        Spansignoffstatus.setAttribute("class","badge bg-green");
    }
    else
    if (record.signoffstatus == 99)
    {
        Spansignoffstatus.textContent = '未核准';
        Spansignoffstatus.setAttribute("class","badge bg-red");
    }
    row.cells[cellIndex].appendChild(Spansignoffstatus);
    cellIndex++;

    // 簽核人員
    var rd_singoff = document.createElement('td')
    rd_singoff.setAttribute("id","signoffresponse_"+record.formid);
    row.appendChild(rd_singoff);
    if (record.signoffstatus == 0)
    {
        if (gl_groupid != 3)
        {
            var signoffAction = create_dropdown_SignoffAction(record.formid);
            row.cells[cellIndex].appendChild(signoffAction);
        }
    }
    else
    {
        var Spandatetime = document.createElement("span");
        Spandatetime.setAttribute("class","badge bg-blue");
        Spandatetime.textContent = getUserName(record.signoffuserid);
        row.cells[cellIndex].appendChild(Spandatetime);

        if (gl_groupid != 3)
        {
            var signoffAction = create_dropdown_SignoffAction(record.formid);
            row.cells[cellIndex].appendChild(signoffAction);
        }

    }
    cellIndex++;

    return row;
}

function load_leaveform_response()
{
    gl_signoffrecords = [];
    var formids = '';

    for (var i = 0; i < gl_LeaveFormRecords.length; ++i)
    {
        var record = gl_LeaveFormRecords[i];
        if (i > 0)
            formids += ',';
        formids += record.formid;
    }

    var targetURL = gl_target_server + "/php/applicationform.php";
    var queryString = 'loadleaveformsignoffrecord=1&project_id='+project_id+'&formids='+formids;

    $.getJSON(targetURL, queryString, function(data)
    {
        var index = 0;
        $.each(data, function(itemindex, record)
        {
            gl_signoffrecords.push(record);

        });

        show_leaveform_response();

        show_leaveform_response_image();
    });
}

function show_leaveform_response()
{
    for (var i = 0; i < gl_signoffrecords.length; ++i)
    {
        var record = gl_signoffrecords[i];

        var rd_singoff = document.getElementById("signoffresponse_"+record.formid);

        var uiRespose = create_signoff_respose(record);

        rd_singoff.appendChild(uiRespose);
    }
}

function show_leaveform_response_image()
{
    for (var i = 0; i < gl_signoffrecords.length; ++i)
    {
        var record = gl_signoffrecords[i];

        var targetURL = gl_target_server + "/php/storage.php";
        var queryString = 'loadnotifyimage=1&project_id='+project_id+'&messagehashkey='+record.messagehashkey;

        $.getJSON(targetURL, queryString, function(data)
        {
            $.each(data, function(itemindex, record)
            {
                if (record.imageuid != undefined )
                {
                    if (record.imageuid.length > 0 )
                    {
                        load_PushNotify_image(record);
                    }
                }
            });
        });
    }
}

function load_PushNotify_image(notifyimage)
{
    // console.log("load_PushNotify_image() imageuid:"+notifyimage.imageuid);

    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadsingleimage=1&project_id='+project_id+'&imageuid='+notifyimage.imageuid, function(storageImage)
    {
        for (var i = 0; i < gl_signoffrecords.length; ++i)
        {
            var record = gl_signoffrecords[i];

            if ( record.messagehashkey === notifyimage.messagehashkey )
            {
                var show_img_area = document.getElementById("signoffresponse_"+record.formid);

                var td_filetype = create_data_image(storageImage.id, storageImage.description, storageImage.thumbpath, storageImage.imagetype, storageImage.imagesize);
                //td_filetype.setAttribute("id", ui_id);
                show_img_area.appendChild(td_filetype);
                break;
            }
        }
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function create_signoff_respose(record)
{
    var DivRespose = document.createElement("div");

    var tableRespose = document.createElement('table');
    tableRespose.setAttribute("class","table table-bordered");
    DivRespose.appendChild(tableRespose);

    var tablethead = document.createElement('thead');
    tableRespose.appendChild(tablethead);

    var tablethead_tr = document.createElement('tr');
    tablethead.appendChild(tablethead_tr);

    var tablethead_th = document.createElement('th');
    tablethead_th.setAttribute("class","bg-gray disabled");
    tablethead_th.textContent = '審核說明';
    tablethead_tr.appendChild(tablethead_th);

    var tabletbody = document.createElement('tbody');
    tableRespose.appendChild(tabletbody);

    var tabletbody_tr = document.createElement('tr');
    tabletbody.appendChild(tabletbody_tr);

    var tabletbody_td = document.createElement('td');
    tabletbody_tr.appendChild(tabletbody_td);


    // var col1 = document.createElement('td');

    var SpanRespose = document.createElement("span");
    //SpanRespose.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(54, 127, 169);");
    SpanRespose.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanRespose.textContent = record.comment;
    tabletbody_td.appendChild(SpanRespose);

    return DivRespose;
}

function create_targetdatetime_dropdown_list()
{
    var current_targetdatetime = document.getElementById('current_targetdatetime');
    var targetdatetime_dropdown_menu = document.getElementById('targetdatetime_dropdown_menu');

    var current_targetdatetime_title = document.getElementById('current_targetdatetime_title');
    if (current_targetdatetime_title == undefined)
    {
        var span = document.createElement('span');
        span.setAttribute("id","current_targetdatetime_title");
        span.textContent = "依最近時間";
        span.setAttribute("class","badge bg-blue-active");
        current_targetdatetime.appendChild(span);

        targetdatetime_dropdown_menu.appendChild(create_targetdatetime_dropdown_item(0, "依最近時間"));
        targetdatetime_dropdown_menu.appendChild(create_targetdatetime_dropdown_item(1, "本日"));
        targetdatetime_dropdown_menu.appendChild(create_targetdatetime_dropdown_item(2, "本週"));
        targetdatetime_dropdown_menu.appendChild(create_targetdatetime_dropdown_item(3, "本月"));
        targetdatetime_dropdown_menu.appendChild(create_targetdatetime_dropdown_item(4, "接下來7天"));
        targetdatetime_dropdown_menu.appendChild(create_targetdatetime_dropdown_item(5, "特定期間"));
        if ( gl_groupid != 3)
        {
            targetdatetime_dropdown_menu.appendChild(create_targetdatetime_dropdown_item(6, "使用點名裝置"));
        }
    }
}

function create_targetdatetime_dropdown_item(type, name)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", type);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","targetdatetime_dropdown_item_click(\""+ type +"\", \""+ name +"\");return true;");

    var span = document.createElement('span');
    span.textContent = name;
    if(type == 0)
        span.setAttribute("class","badge bg-blue-active");
    else
    if(type == 1)
        span.setAttribute("class","badge bg-purple-active");
    else
    if(type == 2)
        span.setAttribute("class","badge bg-teal-active");
    else
    if(type == 3)
        span.setAttribute("class","badge bg-green-active");
    else
    if(type == 4)
        span.setAttribute("class","badge bg-yellow-active");
    else
    if(type == 5)
        span.setAttribute("class","badge bg-aqua-active");
    else
    if(type == 6)
    {
        span.setAttribute("class","");
        span.setAttribute("style","display:inline;padding:.3em .3em .3em;font-size:100%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    }

    item_click.appendChild(span);
    item_li.appendChild(item_click);

    return item_li;
}

function targetdatetime_dropdown_item_click(type, name)
{
    var current_targetdatetime = document.getElementById('current_targetdatetime');
    current_targetdatetime.setAttribute("value",type);

    var current_targetdatetime_title = document.getElementById('current_targetdatetime_title');
    current_targetdatetime_title.textContent = name;
    current_targetdatetime_title.setAttribute("style","");

    if(type == 0)
        current_targetdatetime_title.setAttribute("class","badge bg-blue-active");
    else
    if(type == 1)
        current_targetdatetime_title.setAttribute("class","badge bg-purple-active");
    else
    if(type == 2)
        current_targetdatetime_title.setAttribute("class","badge bg-teal-active");
    else
    if(type == 3)
        current_targetdatetime_title.setAttribute("class","badge bg-green-active");
    else
    if(type == 4)
        current_targetdatetime_title.setAttribute("class","badge bg-yellow-active");
    else
    if(type == 5)
    {
        current_targetdatetime_title.setAttribute("class","badge bg-aqua-active");

        var targetdatetime_range_selector = document.getElementById('targetdatetime_range_selector');
        targetdatetime_range_selector.setAttribute("style","display:block;");

        var rollcalldevice_dropdown_selector = document.getElementById('rollcalldevice_dropdown_selector');
        rollcalldevice_dropdown_selector.setAttribute("style","display:none;");

    }
    else
    if(type == 6)
    {
        current_targetdatetime_title.setAttribute("class","");
        current_targetdatetime_title.setAttribute("style","display:inline;padding:.3em .3em .3em;font-size:100%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");

        wait_websocket_event_cardrollcall();
    }

    if(type != 5)
        reload_form_info();
}

function load_rfiddevice()
{
    if (gl_rfiddevices.length > 0 )
    {
        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loaddevice=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, device)
        {
            gl_rfiddevices.push(device);
        });
    })
    .success(function() {

        put_rfiddevice_to_table();

        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_nfclocator()
{
    if (gl_nfclocators.length > 0 )
    {
        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadnfclocator=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, device)
        {
            gl_nfclocators.push(device);
        });

    })
    .success(function() {

        put_nfcdevice_to_table();

        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function put_rfiddevice_to_table()
{
    var rfiddevice_dropdown_menu = document.getElementById('rfiddevice_dropdown_menu');

    // select all button
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_selectall(\"device_uhfreader\");return true;");
    item_li.appendChild(item_click);
    item_click.appendChild(document.createTextNode("全選"));
    rfiddevice_dropdown_menu.appendChild(item_li);

    for (var i = 0; i < gl_rfiddevices.length; ++i)
    {
        var rfiddevice = gl_rfiddevices[i];
        var existed = false;
        //var existed = check_numer_in_list(parseInt(rfiddevice.deviceid), idList);

        // console.log("create_edit_rfid_devicelist_ui() rfiddevice.deviceid:"+rfiddevice.deviceid+" idList:"+idList +" existed:"+existed);

        rfiddevice_dropdown_menu.appendChild(create_dropdown_device_item("uhfreader", rfiddevice.deviceid, parseInt(rfiddevice.deviceid), rfiddevice.name, existed));
    }
}

function put_nfcdevice_to_table()
{
    var nfcdevice_dropdown_menu = document.getElementById('nfcdevice_dropdown_menu');

    // select all button
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_selectall(\"device_nfclocator\");return true;");
    item_li.appendChild(item_click);
    item_click.appendChild(document.createTextNode("全選"));
    nfcdevice_dropdown_menu.appendChild(item_li);

    //var idList = idlist.split(",");

    for (var i = 0; i < gl_nfclocators.length; ++i)
    {
        var nfcdevice = gl_nfclocators[i];
        // var ret = idList.indexOf(nfcdevice.macaddress);
        var existed = false;
        // if (ret >= 0)
        //     existed = true;
        nfcdevice_dropdown_menu.appendChild(create_dropdown_device_item("nfclocator", nfcdevice.deviceid, parseInt(nfcdevice.macaddress), nfcdevice.name, existed));
    }
}

function create_dropdown_device_item(prefix, deviced, datavalue, dataname, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", datavalue);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_device_click(\""+prefix+"\","+ deviced +");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_device_"+prefix+"_"+deviced);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_device_click();return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+dataname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_selectall(prefix)
{
    // console.log("dropdown_selectall() prefix:"+prefix);

    if( prefix === 'device_uhfreader')
    {
        for (var i = 0; i < gl_rfiddevices.length; ++i)
        {
            var rfiddevice = gl_rfiddevices[i];

            var ele = document.getElementById("dropdown_checkbox_"+prefix+"_"+rfiddevice.deviceid);

            if ( ele == undefined )
            {
                // console.log("dropdown_selectall() i:"+i+" ele == undefined ");
                break;
            }
            ele.checked = true;
        }
    }
    else
    if( prefix === 'device_nfclocator')
    {
        for (var i = 0; i < gl_nfclocators.length; ++i)
        {
            var nfcdevice = gl_nfclocators[i];

            var ele = document.getElementById("dropdown_checkbox_"+prefix+"_"+nfcdevice.deviceid);

            if ( ele == undefined )
            {
                // console.log("dropdown_selectall() i:"+i+" ele == undefined ");
                break;
            }
            ele.checked = true;
        }
    }
}

function dropdown_checkbox_device_click(index)
{
    glDropdown_checkbox_device_click = true;
}

var glDropdown_checkbox_device_click = false;
function dropdown_device_click(prefix, index)
{
    if (glDropdown_checkbox_device_click == true)
    {
        glDropdown_checkbox_device_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_device_"+prefix+"_"+index).checked == true)
        document.getElementById("dropdown_checkbox_device_"+prefix+"_"+index).checked = false;
    else
        document.getElementById("dropdown_checkbox_device_"+prefix+"_"+index).checked = true;

    glDropdown_checkbox_device_click = false;
}

function targetdatetime_range_click()
{
    reload_form_info();
}

function create_empty_record_table_item()
{
    var empty_record_table_item = document.getElementById('empty_record_table_item');
    if ( empty_record_table_item != null )
        return;

    var record_table = document.getElementById('APPFORM_TABLE_BODY');
    var row = record_table.insertRow(0);

    row.setAttribute("id", "empty_record_table_item");
    row.setAttribute("bgcolor","#ebebe0");
    var cellIndex = 0;

    // 表單編號
    var formid = Date.now();
    row.appendChild(document.createElement('td'));
    var formid_text = document.createElement('div');
    formid_text.textContent = formid;
    formid_text.setAttribute("id", "new_formid_text");
    row.cells[cellIndex].appendChild(formid_text);
    cellIndex++;

    // 申請時間
    row.appendChild(document.createElement('td'));
    var applytime_text = document.createElement('div');
    applytime_text.textContent = getCurrentDatetime();
    applytime_text.setAttribute("id", "new_applytime_text");
    row.cells[cellIndex].appendChild(applytime_text);
    cellIndex++;

    // 請假人員
    row.appendChild(document.createElement('td'));
    var accountname_text = document.createElement('div');
    accountname_text.textContent = gl_accountname;
    accountname_text.setAttribute("id", "new_accountname_text");
    row.cells[cellIndex].appendChild(accountname_text);
    cellIndex++;

    // 假勤項目
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_dropdown_leave_type_ui(formid));
    cellIndex++;

    // 事由
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_leave_reason"));
    cellIndex++;

    // 請假時間
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_select_datetime_range_ui());
    cellIndex++;

    // 補請假
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_select_reissue_ui());
    cellIndex++;

    // upload image
    row.appendChild(document.createElement('td'));
    var div_upload = create_upload_image_ui("id_new_image_filename");
    row.cells[cellIndex].appendChild(div_upload);
    cellIndex++;

    // upload file
    row.appendChild(document.createElement('td'));
    var div_upload = create_upload_file_ui("id_new_file_filename");
    row.cells[cellIndex].appendChild(div_upload);
    cellIndex++;

    // 申請人姓名
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_leave_name"));
    cellIndex++;

    // 申請人關係
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_leave_relation"));
    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("id","save_new_record_button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_record();");
    Button_save.textContent = "儲存";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;

    record_table.appendChild(row);

    //Date picker
    $('#start_date_input').datepicker({
      autoclose: true,
      format: "yyyy/mm/dd"
    });
    $('#start_date_input').datepicker("setDate", new Date());

    $('#end_date_input').datepicker({
      autoclose: true,
      format: "yyyy/mm/dd"
    });
    $('#end_date_input').datepicker("setDate", new Date());

    //Timepicker
    $('.timepicker').timepicker({
      showInputs: false,
      showMeridian: false
    })

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
    input_text.setAttribute("style", "width: 120px;");
    input_text.textContent = text;

    return input_text;
}

//========================================================================
function create_dropdown_leave_type_ui(formid)
{
    var div_leavetypelist = document.createElement("div");
    div_leavetypelist.setAttribute("class","btn-group");

    var div_button = document.createElement("button");
    div_button.setAttribute("id","dropdown_leave_type");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "假勤項目";
    div_leavetypelist.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_leavetypelist.appendChild(div_ul);

    div_ul.appendChild(create_dropdown_leave_type_item(0, "病假"));
    div_ul.appendChild(create_dropdown_leave_type_item(1, "事假"));
    div_ul.appendChild(create_dropdown_leave_type_item(2, "公假"));
    div_ul.appendChild(create_dropdown_leave_type_item(3, "喪假"));
    div_ul.appendChild(create_dropdown_leave_type_item(99, "其他"));

    return div_leavetypelist;
}

function create_dropdown_leave_type_item(index, showname)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    //item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", index);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_leave_type_click("+index+",\""+ showname +"\");return true;");
    item_click.appendChild(document.createTextNode(showname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_leave_type_click(index, showname)
{
    var div_button = document.getElementById("dropdown_leave_type");
    div_button.setAttribute("data-value", index);
    div_button.textContent = showname;

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);
}
//========================================================================


//========================================================================
function create_dropdown_SignoffAction(formid)
{
    var div_Signofflist = document.createElement("div");
    div_Signofflist.setAttribute("class","btn-group");

    var div_button = document.createElement("button");
    div_button.setAttribute("id","dropdown_Signoff_type_"+formid);
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "簽核項目";
    div_Signofflist.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_Signofflist.appendChild(div_ul);

    div_ul.appendChild(create_dropdown_Signoff_item(formid, 0, "取消"));
    div_ul.appendChild(create_dropdown_Signoff_item(formid, 1, "核准"));
    div_ul.appendChild(create_dropdown_Signoff_item(formid, 99, "不核准"));

    return div_Signofflist;
}

function create_dropdown_Signoff_item(formid, index, showname)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", index);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_Signoff_click(\""+formid+"\","+index+",\""+ showname +"\");return true;");
    item_click.appendChild(document.createTextNode(showname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_Signoff_click(formid, index, showname)
{
    var div_button = document.getElementById("dropdown_Signoff_type_"+formid);
    div_button.setAttribute("data-value", index);

    var Spansignoffstatus = document.getElementById("signoffstatus_"+formid);

    if ( index == 0 )
    {
        div_button.textContent = "簽核項目";
        Spansignoffstatus.textContent = '簽核中';
        Spansignoffstatus.setAttribute("class","badge bg-blue");

        update_Signoff_state(formid, 0);
    }
    else
    if ( index == 1 )
    {
        div_button.textContent = showname;
        Spansignoffstatus.textContent = '已簽核';
        Spansignoffstatus.setAttribute("class","badge bg-green");

        update_Signoff_state(formid, 1);
    }
    else
    if ( index == 99 )
    {
        div_button.textContent = showname;
        Spansignoffstatus.textContent = '未核准';
        Spansignoffstatus.setAttribute("class","badge bg-red");

        update_Signoff_state(formid, 99);
    }

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);
}
//========================================================================

function create_select_datetime_range_ui()
{
    var div_datetime_range = document.createElement("div");

    div_datetime_range.appendChild(create_select_date_ui("start_date_input", "開始日期:"));
    div_datetime_range.appendChild(create_select_time_ui("start_time_input", "開始時間:"));

    div_datetime_range.appendChild(create_select_date_ui("end_date_input", "結束日期:"));
    div_datetime_range.appendChild(create_select_time_ui("end_time_input", "結束時間:"));

    return div_datetime_range;
}

function create_select_date_ui(uiid, title)
{
    var div_datetime_range = document.createElement("div");
    div_datetime_range.setAttribute("class","form-group");

    var div_start_label = document.createElement("label");
    div_start_label.textContent = title;
    div_datetime_range.appendChild(div_start_label);

    var div_start_date = document.createElement("div");
    div_start_date.setAttribute("class","input-group date");
    div_datetime_range.appendChild(div_start_date);

    var div_start_addon = document.createElement("div");
    div_start_addon.setAttribute("class","input-group-addon");
    div_start_date.appendChild(div_start_addon);

    var div_start_addon_icon = document.createElement("i");
    div_start_addon_icon.setAttribute("class","fa fa-calendar");
    div_start_addon.appendChild(div_start_addon_icon);

    var div_start_date_input = document.createElement("input");
    div_start_date_input.setAttribute("type","text");
    div_start_date_input.setAttribute("class","form-control pull-right");
    div_start_date_input.setAttribute("readonly","readonly");
    div_start_date_input.setAttribute("id",uiid);
    div_start_date_input.setAttribute("style", "width: 120px;");
    div_start_date.appendChild(div_start_date_input);

    return div_datetime_range;
}

function create_select_time_ui(uiid, title)
{
    var div_select_time = document.createElement("div");
    div_select_time.setAttribute("class","form-group");

    var div_start_label = document.createElement("label");
    div_start_label.textContent = title;
    div_select_time.appendChild(div_start_label);

    var div_start_date = document.createElement("div");
    div_start_date.setAttribute("class","input-group date");
    div_select_time.appendChild(div_start_date);

    var div_start_addon = document.createElement("div");
    div_start_addon.setAttribute("class","input-group-addon");
    div_start_date.appendChild(div_start_addon);

    var div_start_addon_icon = document.createElement("i");
    div_start_addon_icon.setAttribute("class","fa fa-clock-o");
    div_start_addon.appendChild(div_start_addon_icon);

    var div_start_date_input = document.createElement("input");
    div_start_date_input.setAttribute("type","text");
    div_start_date_input.setAttribute("class","form-control timepicker");
    div_start_date_input.setAttribute("readonly","readonly");
    div_start_date_input.setAttribute("id",uiid);
    div_start_date_input.setAttribute("style", "width: 120px;");
    div_start_date.appendChild(div_start_date_input);

    return div_select_time;
}

function create_select_reissue_ui()
{
    var div_select_reissue = document.createElement("div");
    div_select_reissue.setAttribute("class","form-group");

    div_select_reissue.appendChild(create_radio_ui("select_reissue", "select_reissue_1", "是", false));
    div_select_reissue.appendChild(create_radio_ui("select_reissue", "select_reissue_2", "否", true));

    return div_select_reissue;
}

function create_radio_ui(ui_name, ui_id, showtext, checked)
{
    var div_select_radio = document.createElement("div");
    div_select_radio.setAttribute("class","radio");

    var div_select_radio_label = document.createElement("label");
    div_select_radio.appendChild(div_select_radio_label);

    var div_select_radio_input = document.createElement("input");
    div_select_radio_input.setAttribute("type","radio");
    div_select_radio_input.setAttribute("name", ui_name);
    div_select_radio_input.setAttribute("id", ui_id);

    if (checked)
    {
        div_select_radio_input.checked = true;
    }

    div_select_radio_label.appendChild(div_select_radio_input);
    div_select_radio_label.appendChild(document.createTextNode(showtext));

    return div_select_radio;
}

function create_upload_image_ui(uiid)
{
    var div_upload = document.createElement("div");

    //
    // edit mode function
    //
    var form_layer_edit = document.createElement('form');
    form_layer_edit.setAttribute("method","POST");
    form_layer_edit.setAttribute("enctype","multipart/form-data");
    form_layer_edit.setAttribute("action","php/uploadfile.php");
    div_upload.appendChild(form_layer_edit);

    var div_formgroup = document.createElement("div");
    div_formgroup.setAttribute("class","form-group");
    form_layer_edit.appendChild(div_formgroup);

    var div_form_row_label = document.createElement("label");
    div_form_row_label.setAttribute("class","control-sidebar-subheading");
    div_form_row_label.setAttribute("for","imageUpload");
    div_form_row_label.innerHTML = '上傳檔案';
    div_formgroup.appendChild(div_form_row_label);

    var div_form_row_input = document.createElement("input");
    div_form_row_input.setAttribute("id","imageUpload");
    div_form_row_input.setAttribute("accept","image/*");
    div_form_row_input.setAttribute("type","file");
    div_form_row_input.setAttribute("name","imageUpload");
    div_form_row_input.setAttribute("onchange","imageSelected();");
    div_formgroup.appendChild(div_form_row_input);

    var div_form_fileSize = document.createElement("div");
    div_form_fileSize.setAttribute("id","imageFileSize");
    div_formgroup.appendChild(div_form_fileSize);

    var div_form_fileType = document.createElement("div");
    div_form_fileType.setAttribute("id","imageFileType");
    div_formgroup.appendChild(div_form_fileType);

    var div_form_progressNumber = document.createElement("div");
    div_form_progressNumber.setAttribute("id","imageProgressNumber");
    div_formgroup.appendChild(div_form_progressNumber);

    return div_upload;
}

function create_upload_file_ui(uiid)
{
    var div_upload = document.createElement("div");

    //
    // edit mode function
    //
    var form_layer_edit = document.createElement('form');
    form_layer_edit.setAttribute("method","POST");
    form_layer_edit.setAttribute("enctype","multipart/form-data");
    form_layer_edit.setAttribute("action","php/uploadfile.php");
    div_upload.appendChild(form_layer_edit);

    var div_formgroup = document.createElement("div");
    div_formgroup.setAttribute("class","form-group");
    form_layer_edit.appendChild(div_formgroup);

    var div_form_row_label = document.createElement("label");
    div_form_row_label.setAttribute("class","control-sidebar-subheading");
    div_form_row_label.setAttribute("for","fileUpload");
    div_form_row_label.innerHTML = '上傳檔案';
    div_formgroup.appendChild(div_form_row_label);

    var div_form_row_input = document.createElement("input");
    div_form_row_input.setAttribute("id","fileUpload");
    div_form_row_input.setAttribute("type","file");
    div_form_row_input.setAttribute("name","fileUpload");
    div_form_row_input.setAttribute("onchange","fileSelected();");
    div_formgroup.appendChild(div_form_row_input);

    var div_form_fileSize = document.createElement("div");
    div_form_fileSize.setAttribute("id","fileFileSize");
    div_formgroup.appendChild(div_form_fileSize);

    var div_form_fileType = document.createElement("div");
    div_form_fileType.setAttribute("id","fileFileType");
    div_formgroup.appendChild(div_form_fileType);

    var div_form_progressNumber = document.createElement("div");
    div_form_progressNumber.setAttribute("id","fileProgressNumber");
    div_formgroup.appendChild(div_form_progressNumber);

    return div_upload;
}

function create_new_applicationform_leave()
{
    // hide current record
    for (var i = 0; i < gl_LeaveFormRecords.length; ++i)
    {
        var record = gl_LeaveFormRecords[i];
        var row_formid = document.getElementById(record.formid);
        row_formid.setAttribute("style","display:none;");
    }

    create_empty_record_table_item();

    return false;
}

function save_new_record()
{
    var new_type = document.getElementById("dropdown_leave_type").textContent;
    if( new_type == '假勤項目' )
    {
        alert("請選擇 假勤項目");
        return;
    }

    var new_leave_reason = document.getElementById("input_leave_reason").value;
    if( new_leave_reason.length == 0 )
    {
        alert("請填寫請假事由");
        return;
    }

    var new_leave_name = document.getElementById("input_leave_name").value;
    if( new_leave_name.length == 0 )
    {
        alert("請填寫申請人姓名");
        return;
    }

    var new_leave_relation = document.getElementById("input_leave_relation").value;
    if( new_leave_relation.length == 0 )
    {
        alert("請填寫申請人關係");
        return;
    }

    var save_new_record_button = document.getElementById('save_new_record_button');
    save_new_record_button.disabled = true;

    check_upload_file();

}

function check_upload_file()
{
    gl_uploadfile_uid = '';
    var uploadfile = document.getElementById('fileUpload').files[0];

    if ( uploadfile != undefined )
    {
        gl_uploadfile_uid = sendUploadFileRequest(uploadFileFinihed);
        console.log("check_upload_file() gl_uploadfile_uid:"+gl_uploadfile_uid);
    }
    else
    {
        check_upload_image();
    }
}

function uploadFileFinihed(uploadfile_uid)
{
    console.log("uploadFileFinihed() uploadfile_uid:"+uploadfile_uid);

    if (uploadfile_uid == null)
    {
        alert("上傳檔案失敗!");
        return;
    }

    var file = document.getElementById('fileUpload').files[0];
    var filename = file.name.split(".");
    var new_file_name = filename[0];
    var new_file_description = filename[0];
    var subgroups = "";

    var SendData = "project_id="+project_id;
    SendData += "&createnewfile=1";
    SendData += "&name="+new_file_name;
    SendData += "&description="+new_file_description;
    SendData += "&filename="+file.name;
    SendData += "&fileuid="+uploadfile_uid;
    SendData += "&filetype="+file.type;
    SendData += "&filesize="+file.size;
    SendData += "&subgroups="+subgroups;
    SendData += "&leaveform=1";

    return jQuery.ajax({
     url: '/php/uploadfile.php',
     type: 'POST',
     data: SendData,
     dataType: 'text',
      success:
        function(response)
        {
            setTimeout(check_upload_image, 300);
        },
      error:
        function(xhr, status, error)
        {
            setTimeout(check_upload_image, 300);
        }
    });
}

function check_upload_image()
{
    gl_uploadimage_uid = '';
    var uploadimage = document.getElementById('imageUpload').files[0];

    if ( uploadimage != undefined )
    {
        gl_uploadimage_uid = sendUploadImageRequest(uploadImageFinihed);
        console.log("check_upload_image() gl_uploadimage_uid:"+gl_uploadimage_uid);
    }
    else
    {
        uploadRecord();
    }
}

function uploadImageFinihed(uploadimage_uid)
{
    console.log("uploadImageFinihed() uploadimage_uid:"+uploadimage_uid);

    if (uploadimage_uid == null)
    {
        alert("上傳照片失敗!");
        return;
    }

    var file = document.getElementById('imageUpload').files[0];
    var filename = file.name.split(".");
    var new_file_name = filename[0];
    var new_file_description = filename[0];
    var subgroups = "";

    var SendData = "project_id="+project_id;
    SendData += "&createnewimage=1";
    SendData += "&name="+new_file_name;
    SendData += "&description="+new_file_description;
    SendData += "&filename="+file.name;
    SendData += "&fileuid="+uploadimage_uid;
    SendData += "&filetype="+file.type;
    SendData += "&filesize="+file.size;
    SendData += "&subgroups="+subgroups;
    SendData += "&leaveform=1";

    return jQuery.ajax({
     url: '/php/uploadfile.php',
     type: 'POST',
     data: SendData,
     dataType: 'text',
      success:
        function(response)
        {
            setTimeout(uploadRecord, 300);
        },
      error:
        function(xhr, status, error)
        {
            setTimeout(uploadRecord, 300);
        }
    });
}

function uploadRecord()
{
    var new_formid = document.getElementById("new_formid_text").textContent;
    var new_applytime = document.getElementById("new_applytime_text").textContent;
    var new_accountname = document.getElementById("new_accountname_text").textContent;
    var new_type = document.getElementById("dropdown_leave_type").textContent;
    var new_leave_reason = document.getElementById("input_leave_reason").value;
    var new_start_date = document.getElementById("start_date_input").value;
    var new_start_time = document.getElementById("start_time_input").value;
    var new_end_date = document.getElementById("end_date_input").value;
    var new_end_time = document.getElementById("end_time_input").value;
    var reissue = document.getElementById("select_reissue_1").checked;
    var new_leave_name = document.getElementById("input_leave_name").value;
    var new_leave_relation = document.getElementById("input_leave_relation").value;

    // console.log("uploadRecord() new_formid:"+new_formid);
    // console.log("uploadRecord() new_applytime:"+new_applytime);
    // console.log("uploadRecord() new_accountname:"+new_accountname);
    // console.log("uploadRecord() new_type:"+new_type);
    // console.log("uploadRecord() new_leave_reason:"+new_leave_reason);
    // console.log("uploadRecord() new_start_date:"+new_start_date);
    // console.log("uploadRecord() new_start_time:"+new_start_time);
    // console.log("uploadRecord() new_end_date:"+new_end_date);
    // console.log("uploadRecord() new_end_time:"+new_end_time);
    // console.log("uploadRecord() reissue:"+reissue);
    // console.log("uploadRecord() new_leave_name:"+new_leave_name);
    // console.log("uploadRecord() new_leave_relation:"+new_leave_relation);


    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_leaveform'] = 1;
    jsondata['formid'] = new_formid;
    jsondata['applytime'] = new_applytime;
    jsondata['type'] = new_type;
    jsondata['reason'] = new_leave_reason;
    jsondata['startdate'] = new_start_date;
    jsondata['starttime'] = new_start_time;
    jsondata['enddate'] = new_end_date;
    jsondata['endtime'] = new_end_time;
    if (reissue)
        jsondata['reissue'] = 1;
    else
        jsondata['reissue'] = 0;
    jsondata['applyusername'] = new_leave_name;
    jsondata['applyuserrelation'] = new_leave_relation;
    jsondata['imageids'] = gl_uploadimage_uid;
    jsondata['fileids'] = gl_uploadfile_uid;
    jsondata['signoffstatus'] = 0;
    jsondata['signoffuserid'] = 0;

    var targetURL = gl_target_server + "/php/applicationform.php";

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

            setTimeout(reload_form_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

}

function update_Signoff_state(formid, value)
{
    var current_form = getLeaveForm(formid);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_Signoff_state'] = 1;
    jsondata['formid'] = formid;
    jsondata['userid'] = current_form.userid;
    jsondata['type'] = current_form.type;
    jsondata['reason'] = current_form.reason;
    jsondata['startdate'] = current_form.startdate;
    jsondata['starttime'] = current_form.starttime;
    jsondata['enddate'] = current_form.enddate;
    jsondata['endtime'] = current_form.endtime;
    jsondata['applyusername'] = current_form.applyusername;
    jsondata['signoffstatus'] = value;
    jsondata['signoffuserid'] = gl_userid;
    jsondata['signoffaccountname'] = gl_accountname;

    var targetURL = gl_target_server + "/php/applicationform.php";

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
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function fetch_storage_image(formid, imageids)
{
    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadsingleimage=1&project_id='+project_id+'&imageuid='+imageids, function(storageimage)
    {
        var data_ul = create_data_image(storageimage.id, storageimage.imagename, storageimage.thumbpath, storageimage.imagetype, storageimage.imagesize);
        var ui_image = document.getElementById("storage_image_"+formid);
        ui_image.appendChild(data_ul);

    })
    .success(function() {

    })
    .error(function() {
    })
    .complete(function() {
    });
}

function fetch_storage_file(formid, fileids)
{
    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadsinglefile=1&project_id='+project_id+'&fileuid='+fileids, function(storagefile)
    {
        var data_ul = create_data_file(storagefile.id, storagefile.filename, storagefile.filetype, storagefile.filesize);
        var ui_file = document.getElementById("storage_file_"+formid);
        ui_file.appendChild(data_ul);
    })
    .success(function() {

    })
    .error(function() {
    })
    .complete(function() {
    });
}

function wait_websocket_event_cardrollcall()
{
    if (glWaitWebSocket == 1 )
    {
        return;
    }

    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsystemconfig=1'+'&project_id='+project_id, function(data)
    {
        glWebSocketPort = data.web_socket_port;
        load_cardrollcall_websocket();
    });
}

function load_cardrollcall_websocket()
{
    var wsUri = gl_websocket_server +":"+glWebSocketPort+"/websocket/xystudy_"+project_id+"_2D1531973179180j8teuD5A4A_type|cardrollcallevent";

    console.log("load_cardrollcall_websocket() wsUri:"+wsUri);

    websocket = new WebSocket(wsUri);
    websocket.onmessage = function(evt) { onMessageCardRollCallEvent(evt) };
    websocket.onclose = function(evt) { onCloseCardRollCallEvent(evt) };
    websocket.onopen = function(evt) { onOpenCardRollCallEvent(evt) };
}

function onMessageCardRollCallEvent(evt)
{
    var data = JSON.parse(evt.data);

    // console.log("onMessageCardRollCallEvent() data:"+data);

    //檢查訊息類型
    if(!data.hasOwnProperty('msgType'))
        return;

    if(data.msgType == 'cardrollcallevent')
    {
        console.log("onMessageCardRollCallEvent() cardrollcallevent cardid:" +  data.cardid +" cardtype:" +  data.cardtype +" readerid:" +  data.readerid);

        show_card_user_leavelform(data.cardid, data.cardtype, data.readerid);
    }
}

function onOpenCardRollCallEvent(evt)
{
    console.log("onOpenCardRollCallEvent() Connected to VILS Center!! ");
    glWaitWebSocket = 1; // 0:disconected 1:connected
}

function onCloseCardRollCallEvent(evt)
{
    console.log("onCloseCardRollCallEvent() reconnect to VILS Center!! ");
    glWaitWebSocket = 0; // 0:disconected 1:connected

    // reconnect
    setTimeout(load_cardrollcall_websocket, 2000);
}

function show_card_user_leavelform(cardid, cardtype, readerid)
{
    // 檢查是否有被勾選
    var device_type = "uhfreader";
    if( cardtype == '0' )
        device_type = "nfclocator";
    var checkbox = document.getElementById("dropdown_checkbox_device_"+device_type+"_"+readerid);

    if (checkbox.checked)
    {
        load_card_user_leavelform(cardid, cardtype, readerid);
    }
}

function load_card_user_leavelform(cardid, cardtype, readerid)
{
    console.log("load_card_user_leavelform() cardid="+cardid);

    var bFound = false;
    var user_name = "無資料";
    for (var i = 0; i < gl_user_info.length; i++)
    {
        var cur_user = gl_user_info[i];

        if( cardtype == '0' )
        {
            if ( cur_user.nfccards != undefined )
            {
                if( cur_user.nfccards.includes(cardid) )
                {
                    gl_LoadSingleUser = cur_user.userid;
                    user_name = cur_user.name;
                    bFound = true;
                    break;
                }
            }
        }
        else
        if( cardtype == '1' )
        {
            if ( cur_user.rfidcards != undefined )
            {
                if( cur_user.rfidcards.includes(cardid) )
                {
                    gl_LoadSingleUser = cur_user.userid;
                    user_name = cur_user.name;
                    bFound = true;
                    break;
                }
            }
        }
    }

    var device_name = "";
    if( cardtype == '0' )
    {
        var nfcdevice = getNFCDevice(readerid);
        device_name = nfcdevice.name;
    }
    else
    if( cardtype == '1' )
    {
        var rfiddevice = getRFIDDevice(readerid);
        device_name = rfiddevice.name;
    }

    var cardscan_result_label = document.getElementById("cardscan_result_label");

    if (bFound)
    {
        cardscan_result_label.textContent = device_name+" 已讀取卡片:"+cardid+" 人員:"+user_name;
        reload_form_info();
    }
    else
    {
        cardscan_result_label.textContent = device_name+" 已讀取卡片:"+cardid+" 人員:"+user_name;
    }

}

function create_data_image(id, filename, thumbpath, filetype, filesize)
{
    var data_ul = document.createElement("ul");
    data_ul.setAttribute("class","mailbox-attachments clearfix");

    var data_li = document.createElement("li");
    data_ul.appendChild(data_li);

    var span_icon = document.createElement("span");
    span_icon.setAttribute("class","mailbox-attachment-icon has-img");
    data_li.appendChild(span_icon);

    var span_icon_img = document.createElement("img");
    span_icon_img.setAttribute("src","/php/downloadfile.php?loadthumb=1&project_id="+project_id+"&imageid="+id);
    span_icon_img.setAttribute("alt","Attachment");
    span_icon.appendChild(span_icon_img);

    var div_info = document.createElement("div");
    div_info.setAttribute("class","mailbox-attachment-info");
    data_li.appendChild(div_info);

    var div_info_href = document.createElement("a");
    div_info_href.setAttribute("href","/php/downloadfile.php?project_id="+project_id+"&imageid="+id);
    div_info_href.setAttribute("class","mailbox-attachment-name");
    div_info.appendChild(div_info_href);

    var div_info_href_img = document.createElement("i");
    div_info_href_img.setAttribute("class","fa fa-camera");
    div_info_href.appendChild(div_info_href_img);

    div_info_href.appendChild(document.createTextNode("  " + filename));

    var div_info_href_size = document.createElement("span");
    div_info_href_size.setAttribute("class","mailbox-attachment-size");
    div_info_href_size.textContent = convertFileSize(parseInt(filesize));
    div_info_href.appendChild(div_info_href_size);

    var div_info_href_dl = document.createElement("a");
    div_info_href_dl.setAttribute("href","/php/downloadfile.php?project_id="+project_id+"&imageid="+id);
    div_info_href_dl.setAttribute("class","btn btn-default btn-xs pull-right");
    div_info_href_size.appendChild(div_info_href_dl);

    var div_info_href_dl_img = document.createElement("i");
    div_info_href_dl_img.setAttribute("class","fa fa-cloud-download");
    div_info_href_dl.appendChild(div_info_href_dl_img);

    return data_ul;
}

function convertFileSize(fileSize)
{
    var retFileSIze = '';
    if (fileSize > 1024 * 1024)
        retFileSIze = (Math.round(fileSize * 100 / (1024 * 1024)) / 100).toString() + 'MB';
    else
        retFileSIze = (Math.round(fileSize * 100 / 1024) / 100).toString() + 'KB';
    return retFileSIze;
}

function create_data_file(id, filename, filetype, filesize)
{
    var data_ul = document.createElement("ul");
    data_ul.setAttribute("class","mailbox-attachments clearfix");

    var data_li = document.createElement("li");
    data_ul.appendChild(data_li);

    var span_icon = document.createElement("span");
    span_icon.setAttribute("class","mailbox-attachment-icon");
    data_li.appendChild(span_icon);

    // var span_icon_img = document.createElement("i");
    // span_icon_img.setAttribute("class","fa fa-file-pdf-o");
    // span_icon.appendChild(span_icon_img);
    span_icon.appendChild(create_filetype_icon(filetype));

    var div_info = document.createElement("div");
    div_info.setAttribute("class","mailbox-attachment-info");
    data_li.appendChild(div_info);

    var div_info_href = document.createElement("a");
    div_info_href.setAttribute("href","/php/downloadfile.php?project_id="+project_id+"&fileid="+id);
    div_info_href.setAttribute("class","mailbox-attachment-name");
    div_info.appendChild(div_info_href);

    var div_info_href_img = document.createElement("i");
    div_info_href_img.setAttribute("class","fa fa-paperclip");
    div_info_href.appendChild(div_info_href_img);

    div_info_href.appendChild(document.createTextNode("  " + filename));

    var div_info_href_size = document.createElement("span");
    div_info_href_size.setAttribute("class","mailbox-attachment-size");
    div_info_href_size.textContent = convertFileSize(parseInt(filesize));
    div_info_href.appendChild(div_info_href_size);

    var div_info_href_dl = document.createElement("a");
    div_info_href_dl.setAttribute("href","/php/downloadfile.php?project_id="+project_id+"&fileid="+id);
    div_info_href_dl.setAttribute("class","btn btn-default btn-xs pull-right");
    div_info_href_size.appendChild(div_info_href_dl);

    var div_info_href_dl_img = document.createElement("i");
    div_info_href_dl_img.setAttribute("class","fa fa-cloud-download");
    div_info_href_dl.appendChild(div_info_href_dl_img);

    return data_ul;
}

function create_filetype_icon(filetype)
{
    var span_icon_img = document.createElement("i");

    if ( filetype.includes('video') )
    {
        span_icon_img.setAttribute("class","fa fa-file-movie-o");
    }
    else
    if ( filetype.includes('audio') )
    {
        span_icon_img.setAttribute("class","fa fa-file-audio-o");
    }
    else
    if ( filetype.includes('application/pdf') )
    {
        span_icon_img.setAttribute("class","fa fa-file-pdf-o");
    }
    else
    if ( filetype.includes('application/zip') )
    {
        span_icon_img.setAttribute("class","fa fa-file-zip-o");
    }
    else
    if ( filetype.includes('ms-excel') )
    {
        span_icon_img.setAttribute("class","fa fa-file-excel-o");
    }
    else
    if ( filetype.includes('officedocument.wordprocessingml.document') )
    {
        span_icon_img.setAttribute("class","fa fa-file-word-o");
    }
    else
    if ( filetype.includes('officedocument.presentationml.presentation') )
    {
        span_icon_img.setAttribute("class","fa fa-file-powerpoint-o");
    }
    else
    if ( filetype.includes('image') )
    {
        span_icon_img.setAttribute("class","fa fa-file-photo-o");
    }
    else
    {
        span_icon_img.setAttribute("class","fa fa-file");
    }

    return span_icon_img;
}

function convertFileSize(fileSize)
{
    var retFileSIze = '';
    if (fileSize > 1024 * 1024)
        retFileSIze = (Math.round(fileSize * 100 / (1024 * 1024)) / 100).toString() + 'MB';
    else
        retFileSIze = (Math.round(fileSize * 100 / 1024) / 100).toString() + 'KB';
    return retFileSIze;
}

function create_leavetype_label(context)
{
    var leavetype_span = document.createElement("span");
    leavetype_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a7d0;");
    leavetype_span.textContent = context;
    return leavetype_span;
}

function create_applyuser_label(context)
{
    var applyuser_span = document.createElement("span");
    applyuser_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#f39c12;");
    applyuser_span.textContent = context;
    return applyuser_span;
}

function create_relation_label(context)
{
    var relation_span = document.createElement("span");
    relation_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    relation_span.textContent = context;
    return relation_span;
}

function getUserName(userid)
{
    var userName = '';
    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];

        if ( user.userid == userid )
        {
            userName = user.name;
            break;
        }

    }
    return userName;
}

function getLeaveForm(formid)
{
    var LeaveForm = null;

    for (var i = 0; i < gl_LeaveFormRecords.length; ++i)
    {
        var record = gl_LeaveFormRecords[i];
        if( formid === record.formid )
        {
            LeaveForm = record;
            break;
        }
    }
    return LeaveForm;
}

function getRFIDDevice(deviceid)
{
    var rfiddevice = null;
    for (var i = 0; i < gl_rfiddevices.length; ++i)
    {
        var device = gl_rfiddevices[i];
        if ( device.deviceid == deviceid )
        {
            rfiddevice = device;
            break;
        }
    }
    return rfiddevice;
}

function getNFCDevice(deviceid)
{
    var nfcdevice = null;
    for (var i = 0; i < gl_nfclocators.length; ++i)
    {
        var device = gl_nfclocators[i];
        if ( device.deviceid == deviceid )
        {
            nfcdevice = device;
            break;
        }
    }
    return nfcdevice;
}

function getCurrentDatetime()
{
    var cur_datetime = moment().format('YYYY/MM/DD HH:mm:ss');
    return cur_datetime;
}

function checkbox_filter_click()
{
    reload_form_info();
}

function clear_readerrecord_click()
{
    var cardscan_result_label = document.getElementById("cardscan_result_label");
    cardscan_result_label.textContent = '';
    gl_LoadSingleUser = '';
    reload_form_info();
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

        reload_form_info();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    glCurrentPage++;
    current_page_num_ui.textContent = glCurrentPage;

    reload_form_info();
}

function open_applicationformleave_page()
{
    var win = window.open('/index.php/vilsapplicationformleave/'+project_id, '_blank');
    win.focus();
}
