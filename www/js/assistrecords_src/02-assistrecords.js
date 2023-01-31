/*
 * Author: ikki Chung
 * Date: 2020 09 29
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var LOAD_PAGE_NUM = 20;

var gl_user_nameinfo = [];
var gl_userids = [];
// var gl_useraccount = [];

var gl_assist_List = [];
var gl_assistevent_List = [];

var glCurrentPage = 1;
var glReloadTimes = 0;

var gl_uploadfile_uid = '';
var gl_uploadimage_uid = '';
var gl_NewApply = {};

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

                gl_user_nameinfo = [];
                gl_userids = [];
                // gl_useraccount = [];

                load_assists();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_assistevents();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                //load_signoff_user_info();
                load_user_nameinfo();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_assistevents();

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

// function load_user_info()
// {
//     var user_num = 0;
//
//     var targetURL = gl_target_server + "/php/vilsnodes.php";
//     var p = $.getJSON(targetURL, 'loaduser=1&s='+gl_user_load_index+'&count='+LOAD_USER_NUM+'&project_id='+project_id, function(data)
//     {
//         $.each(data, function(index, user)
//         {
//             user.userid = parseInt(user.userid);
//             gl_user_info.push(user);
//
//             user_num += 1;
//         });
//
//         gl_user_load_index += user_num;
//     })
//     .success(function() {
//         if (user_num < LOAD_USER_NUM)
//         {
//             CurrentLoadIndex = CurrentLoadIndex + 1;
//         }
//         else
//         {
//             setTimeout(load_user_info, 20);
//         }
//     })
//     .error(function() {
//         CurrentLoadIndex = CurrentLoadIndex + 1;
//     })
//     .complete(function() {
//     });
// }

function load_assists()
{
    var targetURL = gl_target_server + "/php/assist.php";

    gl_assist_List = [];

    $.getJSON(targetURL, 'loadassists=1'+'&project_id='+project_id, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            record.id = parseInt(record.id);
            gl_assist_List.push(record);

            add_user(record.userid);
            //add_useraccount(record.accountid);

        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .complete(function() {
    });
}

function load_assistevents()
{
    var targetURL = gl_target_server + "/php/assist.php";
    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    gl_assistevent_List = [];
    gl_signoffuser = [];

    $.getJSON(targetURL, 'loadassistevents=1'+'&project_id='+project_id+"&offset="+offset+"&count="+count, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            record.id = parseInt(record.id);
            record.assistid = parseInt(record.assistid);
            gl_assistevent_List.push(record);

            add_user(record.userid);
            //add_useraccount(record.accountid);

            // var found = false;
            // for (var i = 0; i < gl_signoffuser.length; ++i)
            // {
            //     var signoffuser = gl_signoffuser[i];
            //     if ( signoffuser === record.signoffaccountid )
            //     {
            //         found = true;
            //         break;
            //     }
            // }
            // if ( found == false && record.signoffaccountid.length > 0)
            // {
            //     gl_signoffuser.push(record.signoffaccountid);
            // }

        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .complete(function() {
    });
}

function load_user_nameinfo()
{
    if ( gl_userids.length == 0 )
    {
        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }
    else
    {
        var queryUser = gl_userids.toString();

        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var p = $.getJSON(targetURL, 'loadusername=1&project_id='+project_id+'&user='+queryUser, function(data)
        {
            $.each(data, function(index, user)
            {
                user.userid = parseInt(user.userid);
                gl_user_nameinfo.push(user);
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

function add_user(userid)
{
    var found = false;
    for (var i = 0; i < gl_userids.length; ++i)
    {
        var gl_userid = gl_userids[i];
        if ( gl_userid == userid )
        {
            found = true;
            break;
        }
    }
    if ( found == false && userid > 0)
    {
        gl_userids.push(userid);
    }
}

// function add_useraccount(accountid)
// {
//     var found = false;
//     for (var i = 0; i < gl_useraccount.length; ++i)
//     {
//         var useraccount = gl_useraccount[i];
//         if ( useraccount === accountid )
//         {
//             found = true;
//             break;
//         }
//     }
//     if ( found == false && accountid.length > 0)
//     {
//         gl_useraccount.push(accountid);
//     }
// }

// function load_signoff_user_info()
// {
//     if ( gl_signoffuser.length == 0 )
//     {
//         CurrentLoadIndex = CurrentLoadIndex + 1;
//         return;
//     }
//     else
//     {
//         var queryUser = gl_signoffuser.toString();
//
//         var targetURL = gl_target_server + "/php/vilsnodes.php";
//         var p = $.getJSON(targetURL, 'loadusername=1&project_id='+project_id+'&account='+queryUser, function(data)
//         {
//             $.each(data, function(index, user)
//             {
//                 gl_user_info.push(user);
//             });
//         })
//         .success(function() {
//             CurrentLoadIndex = CurrentLoadIndex + 1;
//         })
//         .error(function() {
//         })
//         .complete(function() {
//         });
//     }
// }

function show_assistevents()
{
    var table_body_ui = document.getElementById('ASSIST_EVENT_TABLE_BODY');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    // console.log("show_assistevents() gl_assistevent_List.length:"+gl_assistevent_List.length);

    for(var index=0;index<gl_assistevent_List.length;index++)
    {
        var assistevent = gl_assistevent_List[index];
        show_assistevent(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, assistevent);

        var calendar_id = "div_calendar_"+assistevent.id;
        var isEditing = false;

        $('#'+calendar_id).datepicker(
        {
            applyid: assistevent.id,
            language: 'zh-TW',
            weekStart: 1,
            beforeShowDay: function (date, applyid)
            {
                var assistevent = get_assistevent(applyid);
                return checkValidDate(date, assistevent, isEditing, false);
            }
        });
    }

}

function show_assistevent(table_body_ui, ui_index, record)
{
    var cellIndex = 0;

    // console.log("show_assistevent() assistid:"+record.assistid);

    var assist = get_assist(record.assistid);
    if (assist == null)
    {
        return 0;
    }

    // console.log("show_assistevent() assist.title:"+assist.title);

    var row = document.createElement('tr');
    row.setAttribute("id", record.id+"_TABLE_ROW");
    table_body_ui.appendChild(row);

    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    var row_extra = document.createElement('tr');
    row_extra.setAttribute("id", record.id+"_TABLE_ROW_EXTRA");
    table_body_ui.appendChild(row_extra);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    if ( record.signoffstatus == 1 )
    {
        var div_info_href_dl = document.createElement("a");
        div_info_href_dl.setAttribute("id",record.id +"_assist_record_button");
        div_info_href_dl.setAttribute("title","顯示協助紀錄");
        div_info_href_dl.setAttribute("onclick","open_assist_record(\""+record.id+"\", \""+record.assistid+"\");");
        div_info_href_dl.setAttribute("class","btn btn-default btn-xs pull-right");

        var div_info_href_dl_img = document.createElement("i");
        div_info_href_dl_img.setAttribute("id",record.id +"_assist_record_icon");
        div_info_href_dl_img.setAttribute("class","fa fa-plus text-green");
        div_info_href_dl.appendChild(div_info_href_dl_img);
        row.cells[cellIndex].appendChild(div_info_href_dl);
    }
    cellIndex++;

    // 紀錄id
    row.appendChild(document.createElement('td'));
    var titleDiv = document.createElement('div');
    titleDiv.appendChild(document.createTextNode(record.datetime));
    row.cells[cellIndex].appendChild(titleDiv);
    cellIndex++;

    // 事項名稱
    row.appendChild(document.createElement('td'));
    var titleDiv = document.createElement('div');
    titleDiv.appendChild(document.createTextNode(assist.title));
    row.cells[cellIndex].appendChild(titleDiv);
    cellIndex++;

    // 事項說明
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.setAttribute("id", "description_"+record.id);
    descriptionDiv.appendChild(document.createTextNode(assist.description));
    row.cells[cellIndex].appendChild(descriptionDiv);
    cellIndex++;

    // 申請說明
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.setAttribute("id", "apply_description_"+record.id);
    descriptionDiv.appendChild(document.createTextNode(record.description));
    row.cells[cellIndex].appendChild(descriptionDiv);
    cellIndex++;

    // 協助對象
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(getUserName(record.userid)));
    cellIndex++;


    // 每日協助時間
    row.appendChild(document.createElement('td'));
    var eventtime = record.eventtime.split(",");
    for(var i=0;i<eventtime.length;i++)
    {
        row.cells[cellIndex].appendChild(create_time_item(eventtime[i]));
    }
    cellIndex++;

    // 協助日期
    var td_Date = document.createElement('td')
    row.appendChild(td_Date);
    var weekdaydiv = document.createElement("div");
    weekdaydiv.setAttribute("id","div_weekday_"+record.id);
    td_Date.appendChild(weekdaydiv);
    var weekdayui = create_weekday_ui(record.id, record.weekday, false);
    weekdaydiv.appendChild(weekdayui);
    var div_Date = document.createElement("div");
    div_Date.setAttribute("id","div_calendar_"+record.id);
    td_Date.appendChild(div_Date);
    var div_comment = document.createElement("div");
    td_Date.appendChild(div_comment);
    var span_disableddate = document.createElement("span");
    span_disableddate.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-image: linear-gradient(to bottom, #cc4400, #994411);");
    span_disableddate.textContent = '日期';
    div_comment.appendChild(span_disableddate);

    var span_disabledtext = document.createElement("span");
    span_disabledtext.textContent = ':代表不需協助日期';
    div_comment.appendChild(span_disabledtext);
    cellIndex++;

    // 附件照片
    row.appendChild(document.createElement('td'));
    var DivImage = document.createElement("div");
    DivImage.setAttribute("id","storage_image_"+record.id);
    if (record.imageids.length > 0)
    {
        fetch_storage_image(record.id, record.imageids);
    }
    row.cells[cellIndex].appendChild(DivImage);
    cellIndex++;

    // 附件檔案
    row.appendChild(document.createElement('td'));
    var DivFile = document.createElement("div");
    DivFile.setAttribute("id","storage_file_"+record.id);
    if (record.fileids.length > 0)
    {
        fetch_storage_file(record.id, record.fileids);
    }
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

    // 狀態
    row.appendChild(document.createElement('td'));
    var Spansignoffstatus = document.createElement("span");
    Spansignoffstatus.setAttribute("id","signoffstatus_"+record.id);
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

    return 1;
}

function create_time_item(value)
{
    var div_Time = document.createElement("div");
    var span_Time = document.createElement("span");
    span_Time.setAttribute("class","badge bg-yellow-active");
    span_Time.textContent = value;
    div_Time.appendChild(span_Time);
    return div_Time;
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

function open_assist_record(assisteventid, assistid)
{
    console.log("open_assist_record() assisteventid:"+assisteventid+" assistid:"+assistid);

    var div_info_href_dl = document.getElementById(assisteventid+"_assist_record_button");
    div_info_href_dl.setAttribute("title","關閉協助紀錄");
    div_info_href_dl.setAttribute("onclick","close_assist_record(\""+assisteventid+"\", \""+assistid+"\");");

    var div_info_href_dl_img = document.getElementById(assisteventid+"_assist_record_icon");
    div_info_href_dl_img.setAttribute("class","fa fa-minus text-red");

    var row_extra = document.getElementById(assisteventid+"_TABLE_ROW_EXTRA");
    row_extra.setAttribute("bgcolor","#d7e9e4");

    // index
    row_extra.appendChild(document.createElement('td'));

    // assist record table
    var rd_assistrecord= document.createElement('td')
    rd_assistrecord.setAttribute("colspan","12");
    row_extra.appendChild(rd_assistrecord);

    var table_assistrecord = document.createElement('table')
    table_assistrecord.setAttribute("class","table table-bordered");
    //table_assistrecord.setAttribute("style","background-color:rgb(236, 240, 245);");
    rd_assistrecord.appendChild(table_assistrecord);

    var table_head_assistrecord = document.createElement('thead')
    table_assistrecord.appendChild(table_head_assistrecord);

    var table_head_tr_assistrecord = document.createElement('tr')
    table_head_tr_assistrecord.setAttribute("style","background-color:rgb(236, 240, 245);");
    table_head_assistrecord.appendChild(table_head_tr_assistrecord);

    var table_head_th_assistrecord_index = document.createElement('th')
    table_head_th_assistrecord_index.textContent = '#';
    table_head_th_assistrecord_index.setAttribute("style","width: 30px;");
    table_head_tr_assistrecord.appendChild(table_head_th_assistrecord_index);

    var table_head_th_assistrecord_time = document.createElement('th')
    table_head_th_assistrecord_time.textContent = '預約時間';
    table_head_th_assistrecord_time.setAttribute("style","width: 200px;");
    table_head_tr_assistrecord.appendChild(table_head_th_assistrecord_time);

    var table_head_th_assistrecord_sendtime = document.createElement('th')
    table_head_th_assistrecord_sendtime.textContent = '通知負責人時間';
    table_head_th_assistrecord_sendtime.setAttribute("style","width: 200px;");
    table_head_tr_assistrecord.appendChild(table_head_th_assistrecord_sendtime);

    var table_head_th_assistrecord_status = document.createElement('th')
    table_head_th_assistrecord_status.textContent = '回報狀態';
    table_head_tr_assistrecord.appendChild(table_head_th_assistrecord_status);

    var table_head_th_assistrecord_reporttime = document.createElement('th')
    table_head_th_assistrecord_reporttime.textContent = '回報時間';
    table_head_tr_assistrecord.appendChild(table_head_th_assistrecord_reporttime);

    // var table_head_th_assistrecord_detail = document.createElement('th')
    // table_head_th_assistrecord_detail.textContent = '操作';
    // table_head_th_assistrecord_detail.setAttribute("style","width: 100px;");
    // table_head_tr_assistrecord.appendChild(table_head_th_assistrecord_detail);

    var table_body_assistrecord = document.createElement('tbody')
    table_body_assistrecord.setAttribute("id","assistrecord_table_"+assisteventid);
    table_assistrecord.appendChild(table_body_assistrecord);


    load_AssistEventRecord(assisteventid, assistid);

}

function close_assist_record(assisteventid, assistid)
{
    //console.log("close_message_detail()  messagehashkey:"+messagehashkey+" surveyids:"+surveyids +" hashkeys:"+hashkeys+" imageuid:"+imageuid);

    var div_info_href_dl = document.getElementById(assisteventid+"_assist_record_button");
    div_info_href_dl.setAttribute("title","顯示協助紀錄");
    div_info_href_dl.setAttribute("onclick","open_assist_record(\""+assisteventid+"\", \""+assistid+"\");");

    var div_info_href_dl_img = document.getElementById(assisteventid+"_assist_record_icon");
    div_info_href_dl_img.setAttribute("class","fa fa-plus text-green");

    clearChildView(assisteventid+"_TABLE_ROW_EXTRA");
}

function load_AssistEventRecord(assisteventid, assistid)
{
    var targetURL = gl_target_server + "/php/assist.php";

    var assisteventrecords = [];

    $.getJSON(targetURL, 'loadassisteventrecords=1'+'&project_id='+project_id+"&assistid="+assistid+"&assisteventid="+assisteventid+"&count=5", function(data)
    {
        $.each(data, function(itemindex, record)
        {
            assisteventrecords.push(record);
        });
    })
    .success(function() {
        // console.log("load_AssistEventRecord() assisteventrecords.length:" + assisteventrecords.length);
        show_AssistEventRecord(assisteventid, assistid, assisteventrecords);
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function show_AssistEventRecord(assisteventid, assistid, records)
{
    var table_body_assistrecord = document.getElementById("assistrecord_table_"+assisteventid);

    // console.log("show_AssistEventRecord() table_body_assistrecord:"+table_body_assistrecord+" records.length:"+records.length +" assisteventid:"+assisteventid+" assistid:"+assistid);

    for(var index=0;index<records.length;index++)
    {
        record = records[index];

        show_assistevent_record_item(table_body_assistrecord, index, record);
    }

    var record_tr = document.createElement('tr');
    table_body_assistrecord.appendChild(record_tr);

    var record_td = document.createElement('td');
    record_td.setAttribute("colspan","5");
    record_tr.appendChild(record_td);

    var checkBtn = document.createElement("button");
    checkBtn.setAttribute("title","查看詳細資訊");
    checkBtn.setAttribute("href","javascript:;");
    checkBtn.textContent = "查看更多詳細資訊  ";
    checkBtn.setAttribute("onclick","show_assisteventrecord_detail("+ project_id +",\""+assisteventid+"\",\""+assistid+"\");return false;");
    var imgCheck = document.createElement("i");
    imgCheck.setAttribute("class","fa fa-calendar-check-o ");
    checkBtn.appendChild(imgCheck);
    record_td.appendChild(checkBtn);

}

function show_assistevent_record_item(ui_table, ui_index, record)
{
    var record_tr = document.createElement('tr');
    ui_table.appendChild(record_tr);

    if ((ui_index % 2) == 1)
        record_tr.setAttribute("bgcolor","#f8f6ef");

    // index
    var record_td_index = document.createElement('td');
    record_td_index.textContent = ui_index + 1;
    record_tr.appendChild(record_td_index);

    // 預約時間
    var record_td_time = document.createElement('td');
    record_td_time.textContent = record.eventday + ' ' + record.eventtime;
    record_tr.appendChild(record_td_time);

    // 通知負責人時間
    var record_td_sendtime = document.createElement('td');
    record_td_sendtime.textContent = record.datetime;
    record_tr.appendChild(record_td_sendtime);

    // 回報狀態
    var record_td_status = document.createElement('td');
    record_tr.appendChild(record_td_status);

    // 回報時間
    var record_td_reporttime = document.createElement('td');
    record_tr.appendChild(record_td_reporttime);


    if ( record.results.length == 0 )
    {
        record_td_status.textContent = '尚未回報';
        record_td_reporttime.textContent = '';
    }
    else
    {
        if ( record.results === "1,0" )
            record_td_status.textContent = '已完成';
        else
            record_td_status.textContent = '未完成';
        record_td_reporttime.textContent = record.updatetime;
    }
}

function show_assisteventrecord_detail(project_id, assisteventid, assistid)
{
    var win = window.open('/index.php/vilsassistrecorddetail/'+project_id+'?assisteventid='+assisteventid+'&assistid='+assistid, '_blank');
    win.focus();
}

function create_empty_assist_table_item(ui_index)
{
    var record_table = document.getElementById('ASSIST_EVENT_TABLE_BODY');
    var assist_description_new = document.getElementById("assist_description_new");
    if (assist_description_new != undefined)
    {
        // already existed!
        return;
    }

    gl_NewApply.applyid = 88888;
    gl_NewApply.weekday = [];
    gl_NewApply.validdate = [];
    gl_NewApply.invaliddate = [];
    var targetDate = AddDays(30);
    var deadline = targetDate.getFullYear() + "-" + ( '0' + (targetDate.getMonth()+1) ).slice( -2 ) + "-" + ( '0' + targetDate.getDate() ).slice( -2 );
    gl_NewApply.deadline = deadline;

    // console.log("gl_NewApply.deadline:"+gl_NewApply.deadline);

    var row = record_table.insertRow(0);
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""));
    cellIndex++;

    // 申請時間
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""));
    cellIndex++;

    // 事項名稱
    row.appendChild(document.createElement('td'));
    var dropdown_assist = create_dropdown_assist('new');
    row.cells[cellIndex].appendChild(dropdown_assist);
    var assist_span = document.createElement('span');
    assist_span.setAttribute("class",'badge bg-aqua-active');
    assist_span.setAttribute("id",'assist_title_new');
    assist_span.setAttribute("assistid", '');
    row.cells[cellIndex].appendChild(assist_span);
    cellIndex++;

    // 事項說明
    row.appendChild(document.createElement('td'));
    var assist_description_span = document.createElement('span');
    assist_description_span.setAttribute("id",'assist_description_new');
    row.cells[cellIndex].appendChild(assist_description_span);
    cellIndex++;

    // 申請說明
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("apply_description_new", ""));
    cellIndex++;

    // 協助對象
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(gl_accountname +' [ '+gl_accountid+' ]'));
    cellIndex++;

    // 每日協助時間
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_time_select("apply_time_new"));
    cellIndex++;

    // 協助日期
    //row.appendChild(document.createElement('td'));
    //row.cells[cellIndex].appendChild(create_weekday_select("apply_date_new"));
    var td_date = create_weekday_select("apply_date_new");
    row.appendChild(td_date);
    cellIndex++;

    // 附件照片
    row.appendChild(document.createElement('td'));
    var div_upload = create_upload_image_ui("apply_image_filename_new");
    row.cells[cellIndex].appendChild(div_upload);
    cellIndex++;

    // 附件檔案
    row.appendChild(document.createElement('td'));
    var div_upload = create_upload_file_ui("apply_file_filename_new");
    row.cells[cellIndex].appendChild(div_upload);
    cellIndex++;

    // 申請人姓名
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("apply_name_new"));
    cellIndex++;

    // 申請人關係
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("apply_relation_new"));
    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("id","save_new_apply_button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_apply();");
    Button_save.textContent = "提出申請";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;


    $('.timepicker').timepicker(
    {
        showInputs: false
    });

    var calendar_id = "div_calendar_"+gl_NewApply.applyid;

    var isEditing = true;
    $('#'+calendar_id).datepicker(
    {
        applyid: gl_NewApply.applyid,
        language: 'zh-TW',
        weekStart: 1,
        beforeShowDay: function (date, applyid)
        {
            // console.log("beforeShowDay() applyid:"+applyid);

            var assistevent = get_assistevent(applyid);

            return checkValidDate(date, assistevent, isEditing, false);
        }
    }).on('changeDate', function (ev)
    {
        // console.log("changeDate() ev.applyid:"+ev.applyid +" ev.date:"+ev.date);

        if (ev.applyid != undefined && ev.date != undefined)
        {
            var assistevent = get_assistevent(ev.applyid);
            checkValidDate(ev.date, assistevent, isEditing, true);

            $('#'+calendar_id).datepicker("update", ev.date);
        }
    });
}

function AddDays(days)
{
    let _now = new Date();
    let _addDate = new Date(_now);
    _addDate.setDate(_addDate.getDate() + days);
    return _addDate;
}

function create_dropdown_assist(assisteventid)
{
    var div_assist = document.createElement("div");
    div_assist.setAttribute("class","btn-group");

    var div_button = document.createElement("button");
    div_button.setAttribute("id","dropdown_assist_"+assisteventid);
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "協助事項";
    div_assist.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_assist.appendChild(div_ul);

    for(var index=0;index<gl_assist_List.length;index++)
    {
        var assist = gl_assist_List[index];

        div_ul.appendChild(create_dropdown_assist_item(assisteventid, assist.id, assist.title, assist.description));
    }
    return div_assist;
}

function create_dropdown_assist_item(assisteventid, assistid, showname, showdescription)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", assistid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_assist_click(\""+assisteventid+"\","+assistid+",\""+ showname +"\",\""+ showdescription +"\");return true;");
    item_click.appendChild(document.createTextNode(showname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_assist_click(assisteventid, assistid, showname, showdescription)
{
    console.log("dropdown_assist_click() assisteventid:"+assisteventid+" assistid:"+assistid+" showname:"+showname);
    var assist_title_new = document.getElementById('assist_title_new');
    if (assist_title_new != null)
    {
        assist_title_new.setAttribute("assistid", assistid);
        assist_title_new.textContent = showname;
    }

    var assist_description_new = document.getElementById('assist_description_new');
    if (assist_description_new != null)
    {
        assist_description_new.textContent = showdescription;
    }
}

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.setAttribute("style", "width: 120px;");
    input_text.textContent = text;

    return input_text;
}

function create_time_select(ui_id)
{
    var div_time_select = document.createElement("div");

    div_time_select.appendChild(create_time_picker(ui_id, "07:00 AM"));

    var div_time_selected = document.createElement("div");
    div_time_selected.setAttribute("id", ui_id);
    div_time_select.appendChild(div_time_selected);

    return div_time_select;
}

function create_weekday_select(ui_id)
{
    var td_Date = document.createElement('td');

    var weekdaydiv = document.createElement("div");
    weekdaydiv.setAttribute("id",ui_id);
    td_Date.appendChild(weekdaydiv);

    var weekdayui = create_weekday_ui(gl_NewApply.applyid, gl_NewApply.weekday, true);
    weekdaydiv.appendChild(weekdayui);

    var div_Date = document.createElement("div");
    div_Date.setAttribute("id","div_calendar_"+gl_NewApply.applyid);
    td_Date.appendChild(div_Date);

    var div_comment = document.createElement("div");
    td_Date.appendChild(div_comment);

    var span_disableddate = document.createElement("span");
    span_disableddate.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-image: linear-gradient(to bottom, #cc4400, #994411);");
    span_disableddate.textContent = '日期';
    div_comment.appendChild(span_disableddate);

    var span_disabledtext = document.createElement("span");
    span_disabledtext.textContent = ':代表不需協助日期';
    div_comment.appendChild(span_disabledtext);

    var span_next30days = document.createElement("div");
    span_next30days.textContent = '只可申請未來30天內協助事項';
    div_comment.appendChild(span_next30days);

    return td_Date;
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

function create_time_picker(ui_id, timetext)
{
    // time Picker
    var div_TimePicker = document.createElement("div");
    div_TimePicker.setAttribute("class","bootstrap-timepicker");
    div_TimePicker.setAttribute("style","width:180px;");

    var div_inputgroup = document.createElement("div");
    div_inputgroup.setAttribute("class","input-group");
    div_TimePicker.appendChild(div_inputgroup);

    // clock icon
    var div_clock = document.createElement("div");
    div_clock.setAttribute("class","input-group-addon");
    div_inputgroup.appendChild(div_clock);
    // icon
    var div_clock_icon = document.createElement("i");
    div_clock_icon.setAttribute("class","fa fa-clock-o");
    div_clock.appendChild(div_clock_icon);

    // input
    var input_Time = document.createElement("input");
    input_Time.setAttribute("type","text");
    input_Time.setAttribute("id","new_select_time");
    input_Time.setAttribute("readonly","readonly");
    input_Time.setAttribute("class","form-control timepicker");
    input_Time.setAttribute("value",timetext);
    div_inputgroup.appendChild(input_Time);

    // confirm button icon
    var div_clock = document.createElement("div");
    div_clock.setAttribute("class","input-group-addon");
    div_clock.setAttribute("style","background-color:green;color:white;");
    div_clock.setAttribute("onclick","add_new_time('"+ui_id+"','new_select_time');");
    div_clock.textContent = "新增";
    div_inputgroup.appendChild(div_clock);


    return div_TimePicker;
}

function add_new_time(target_ui_id, src_ui_id)
{
    var target_ui = document.getElementById(target_ui_id);
    var src_ui = document.getElementById(src_ui_id);

    //
    // 檢查是否已重複
    //
    if ( target_ui != null )
    {
        var t = target_ui.childNodes;
        //console.log("add_new_time() t.length:"+t.length);
        var bFound = false;
        for(let i = 0; i < t.length; i++)
        {
            var node = t[i];
            var time = node.getAttribute('timevalue');
            //console.log("add_new_time() node.id:"+node.id +" time:"+time);

            if ( src_ui.value === time )
            {
                bFound = true;
                break;
            }
        }
        if ( bFound )
        {
            alert(src_ui.value + " 時間已指定!");
            return ;
        }
    }

    var time_id = new Date().getTime();
    var time_ui_id = 'apply_time_'+time_id;

    var div_Time = document.createElement("div");
    div_Time.setAttribute("id", time_ui_id);
    div_Time.setAttribute("timevalue", src_ui.value);
    target_ui.appendChild(div_Time);

    var span_Time = document.createElement("span");
    span_Time.setAttribute("class","badge bg-yellow-active");
    span_Time.textContent = src_ui.value;
    div_Time.appendChild(span_Time);

    // remove button
    var remove_span = document.createElement('span');
    remove_span.setAttribute("class",'badge bg-red');
    remove_span.textContent = '移除';
    remove_span.setAttribute("onclick","remove_time_click('"+ time_ui_id +"');return true;");
    div_Time.appendChild(remove_span);

}

function remove_time_click(time_ui_id)
{
    var ui_item = document.getElementById(time_ui_id);
    if ( ui_item != null )
    {
        ui_item.parentElement.removeChild(ui_item);
    }
}

function create_weekday_ui(applyid, validdays, isEditing)
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
        var checkboxui = create_checkbox(applyid, day, checked, isEditing);
        boxtabletbodytd.appendChild(checkboxui);
        boxtabletbodytr.appendChild(boxtabletbodytd);
    }

    return weekdaybox;
}

function create_checkbox(applyid, day, checked, isEditing)
{
    var inputui = document.createElement("input");
    inputui.setAttribute("id","uiid_checkbox_"+day+"_"+applyid);
    inputui.setAttribute("type","checkbox");
    inputui.setAttribute("onclick","weekday_checkbox_click(\""+applyid+"\",\""+day+"\")");
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

function weekday_checkbox_click(applyid, day)
{
    var checkd = document.getElementById("uiid_checkbox_"+day+"_"+applyid).checked;

    var assistevent = get_assistevent(applyid);

    var existed_index = assistevent.weekday.indexOf(day);
    if (checkd)
    {
        if (existed_index < 0)
        {
            assistevent.weekday.push(day);
            // 重新檢查 validdate  與  invaliddate
            check_invaliddate(assistevent, day);
        }
    }
    else
    {
        if (existed_index >= 0)
        {
            assistevent.weekday.splice(existed_index, 1);
            // 重新檢查 validdate  與  invaliddate
            check_validdate(assistevent, day);
        }
    }

    console.log("weekday_checkbox_click() day:"+day +" applyid:"+applyid +" checkd:"+checkd +" weekday:"+assistevent.weekday);

    var calendar_id = "div_calendar_"+applyid;
    $('#'+calendar_id).datepicker('setDate', null);

}

// 重新檢查 validdate  與  invaliddate
function check_validdate(assistevent, day)
{
    console.log("check_validdate() assistevent.validdate.length:"+assistevent.validdate.length);

    // 檢查是否有 日期 在不要點名 週間
    var index = assistevent.validdate.length;
    while (index--)
    {
        var existed_date = assistevent.validdate[index];
        const date = moment(existed_date);
        const dow = date.day();

        console.log("check_validdate() existed_date:"+existed_date +" dow:"+dow +" day:"+day);

        if ( dow == day )
        {
            // 移除這個日期
            console.log("check_validdate() 移除這個日期 existed_date:"+existed_date);
            assistevent.validdate.splice(index, 1);
        }
    }
}

function check_invaliddate(assistevent, day)
{
    console.log("check_invaliddate() assistevent.invaliddate.length:"+assistevent.invaliddate.length);

    // 檢查是否有 日期 在要點名 週間
    var index = assistevent.invaliddate.length;
    while (index--)
    {
        var existed_date = assistevent.invaliddate[index];
        const date = moment(existed_date);
        const dow = date.day();

        console.log("check_invaliddate() existed_date:"+existed_date +" dow:"+dow +" day:"+day);

        if ( dow == day )
        {
            // 移除這個日期
            console.log("check_invaliddate() 移除這個日期 existed_date:"+existed_date);
            assistevent.invaliddate.splice(index, 1);
        }
    }
}

function checkValidDate(date, assistevent, isEditing, isToggleSelect)
{
    var targetDate = date.getFullYear() + "-" + ( '0' + (date.getMonth()+1) ).slice( -2 ) + "-" + ( '0' + date.getDate() ).slice( -2 );

    var day = date.getDay();
    var existed_index = assistevent.weekday.indexOf(''+day);

    //
    // check daealine
    //
    //console.log("checkValidDate(): "+date +"  > "+assistevent.deadline);
    if ( targetDate > assistevent.deadline )
    {
        //console.log("checkValidDate(): "+targetDate +"  > "+assistevent.deadline);

        return {enabled:isEditing, classes:'active'};
    }

    if ( existed_index >= 0 )
    {
        // console.log("checkValidDate(): 1 isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index);

        if (isToggleSelect)
        {
            var isActive = true;
            // 要點名週間
            // 檢查是否在 assistevent.invaliddate
            var indexOfinvaliddate = assistevent.invaliddate.indexOf(targetDate);
            if (indexOfinvaliddate >= 0)
            {
                // 從 assistevent.invaliddate 移除
                //console.log("checkValidDate(): 1 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 移除 不要點名");
                assistevent.invaliddate.splice(indexOfinvaliddate, 1);
                isActive = false;
            }
            else
            {
                // 加入 不要點名
                //console.log("checkValidDate(): 1 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 加入 不要點名");
                assistevent.invaliddate.push(targetDate);
            }

            if (isActive == false)
            {
                // 檢查是否在 assistevent.validdate
                var indexOfvaliddate = assistevent.validdate.indexOf(targetDate);
                if (indexOfvaliddate < 0)
                {
                    // 加入 要點名
                    //console.log("checkValidDate(): 1 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 加入 不要點名");
                    assistevent.validdate.push(targetDate);
                }
            }
            else
            {
                // 從 assistevent.validdate 移除
                var indexOfvaliddate = assistevent.validdate.indexOf(targetDate);
                if (indexOfvaliddate >= 0)
                {
                    //console.log("checkValidDate(): 1 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 移除 要點名");
                    assistevent.validdate.splice(indexOfvaliddate, 1);
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
            // console.log("checkValidDate(): 1 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" noactive");

            // 要點名週間
            // 檢查是否在 assistevent.invaliddate
            var indexOfinvaliddate = assistevent.invaliddate.indexOf(targetDate);
            if (indexOfinvaliddate >= 0)
            {
                // console.log("checkValidDate(): 設定不點名");
                // 設定不點名
                return {enabled:isEditing, classes:'active'};
            }
            else
            {
                // console.log("checkValidDate(): 設定要點名");
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
            // 檢查是否在 assistevent.validdate
            var indexOfvaliddate = assistevent.validdate.indexOf(targetDate);
            if (indexOfvaliddate >= 0)
            {
                // 移除 要點名
                // 從 assistevent.validdate 移除
                //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 移除 要點名");
                assistevent.validdate.splice(indexOfvaliddate, 1);
                isActive = true;
            }
            else
            {
                // 加入 要點名
                //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 加入 要點名");
                assistevent.validdate.push(targetDate);
            }

            if (isActive)
            {
                // 檢查是否在 assistevent.invaliddate
                var indexOfinvaliddate = assistevent.invaliddate.indexOf(targetDate);
                if (indexOfinvaliddate < 0)
                {
                    // 加入 不要點名
                    //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 加入 不要點名");
                    assistevent.invaliddate.push(targetDate);
                }
            }
            else
            {
                // 從 assistevent.invaliddate 移除
                var indexOfinvaliddate = assistevent.invaliddate.indexOf(targetDate);
                if (indexOfinvaliddate >= 0)
                {
                    //console.log("checkValidDate(): 2 "+targetDate+" day:"+day+" isToggleSelect:"+isToggleSelect +" existed_index:"+existed_index +" 移除 不要點名");
                    assistevent.invaliddate.splice(indexOfinvaliddate, 1);
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
            // 檢查是否在 assistevent.validdate
            var indexOfvaliddate = assistevent.validdate.indexOf(targetDate);
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

    // return {enabled:isEditing};
}

function apply_new_assist()
{
    create_empty_assist_table_item(gl_assistevent_List.length + 1);

    return false;
}

function save_new_apply()
{
    var assist_title_new = document.getElementById('assist_title_new');
    if (assist_title_new != null)
    {
        var assistid = assist_title_new.getAttribute("assistid");
        if ( assistid.length == 0 )
        {
            alert("請選擇 協助事項");
            return ;
        }
    }
    else
    {
        return ;
    }

    var apply_description_new = document.getElementById("apply_description_new").value;
    if( apply_description_new.length == 0 )
    {
        alert("請填寫 申請說明");
        return;
    }

    // 每日協助時間
    var apply_time_new = document.getElementById('apply_time_new');
    var nodes = apply_time_new.childNodes;
    if ( nodes.length == 0 )
    {
        alert("請選擇 每日協助時間");
        return;
    }

    // 申請人姓名
    var apply_name_new = document.getElementById("apply_name_new").value;
    if( apply_name_new.length == 0 )
    {
        alert("請填寫申請人姓名");
        return;
    }

    // 申請人關係
    var apply_relation_new = document.getElementById("apply_relation_new").value;
    if( apply_relation_new.length == 0 )
    {
        alert("請填寫申請人關係");
        return;
    }


    var save_new_apply_button = document.getElementById('save_new_apply_button');
    save_new_apply_button.disabled = true;

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
    var assist_title_new = document.getElementById('assist_title_new');
    var assistid = assist_title_new.getAttribute("assistid");
    var apply_description_new = document.getElementById("apply_description_new").value;
    var apply_name_new = document.getElementById("apply_name_new").value;
    var apply_relation_new = document.getElementById("apply_relation_new").value;

    var applytime = [];
    var apply_time_new = document.getElementById('apply_time_new');
    var nodes = apply_time_new.childNodes;
    for(let i = 0; i < nodes.length; i++)
    {
        var node = nodes[i];
        var time = node.getAttribute('timevalue');
        applytime.push(time);
    }

    var weekday = gl_NewApply.weekday.toString();
    var validdate = gl_NewApply.validdate.toString();
    var invaliddate = gl_NewApply.invaliddate.toString();
    //var deadline = gl_NewApply.deadline.getFullYear() + "-" + ( '0' + (gl_NewApply.deadline.getMonth()+1) ).slice( -2 ) + "-" + ( '0' + gl_NewApply.deadline.getDate() ).slice( -2 );
    var deadline = gl_NewApply.deadline;

    // console.log("uploadRecord() assistid:"+assistid);
    // console.log("uploadRecord() apply_description_new:"+apply_description_new);
    // console.log("uploadRecord() applytime:"+applytime.toString());
    // console.log("uploadRecord() weekday:"+weekday);
    // console.log("uploadRecord() validdate:"+validdate);
    // console.log("uploadRecord() deadline:"+deadline);
    // console.log("uploadRecord() invaliddate:"+invaliddate);
    // console.log("uploadRecord() gl_uploadimage_uid:"+gl_uploadimage_uid);
    // console.log("uploadRecord() gl_uploadfile_uid:"+gl_uploadfile_uid);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_assistevent'] = 1;
    jsondata['id'] = assistid;
    jsondata['description'] = apply_description_new;
    jsondata['eventtime'] = applytime.toString();
    jsondata['weekday'] = weekday;
    jsondata['validdate'] = validdate;
    jsondata['invaliddate'] = invaliddate;
    jsondata['deadline'] = deadline;
    jsondata['imageids'] = gl_uploadimage_uid;
    jsondata['fileids'] = gl_uploadfile_uid;
    jsondata['applyusername'] = apply_name_new;
    jsondata['applyuserrelation'] = apply_relation_new;

    var targetURL = gl_target_server + "/php/assist.php";

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
            setTimeout(reload_assistevents, 300);
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

function reload_assistevents()
{
    // console.log("reload_assistevents() glReloadTimes:"+glReloadTimes);

    PreLoadIndex = 0;
    CurrentLoadIndex = 1;

    setTimeout(check_loading_status, 50);
}


function getUserName(userid)
{
    var userName = '';
    for (var i = 0; i < gl_user_nameinfo.length; ++i)
    {
        var user = gl_user_nameinfo[i];

        if ( user.userid == userid )
        {
            userName = user.name;
            break;
        }

    }
    return userName;
}

// function getUserName(accountid)
// {
//     var UserName = "";
//
//     var accountid_lc = accountid.toLowerCase();
//
//     for (var i = 0; i < gl_user_nameinfo.length; i++)
//     {
//         // console.log("getUserName() accountid:"+accountid +" gl_user_info[i].account:"+gl_user_info[i].account);
//
//         var glaccountid_lc = gl_user_nameinfo[i].account.toLowerCase();
//
//         if ( accountid_lc === glaccountid_lc)
//         {
//             UserName = gl_user_nameinfo[i].name;
//             break;
//         }
//     }
//
//     return UserName;
// }

function get_assist(assistid)
{
    for(var index=0;index<gl_assist_List.length;index++)
    {
        var assist = gl_assist_List[index];

        if ( assist.id == assistid )
        {
            return assist;
        }
    }
    return null;
}

function get_assistevent(eventid)
{
    for(var index=0;index<gl_assistevent_List.length;index++)
    {
        var assistevent = gl_assistevent_List[index];
        if ( assistevent.id == eventid )
        {
            return assistevent;
        }
    }

    if ( eventid == 88888 )
    {
        return gl_NewApply;
    }

    return null;
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
        PreLoadIndex = 0;
        CurrentLoadIndex = 1;
        check_loading_status();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    glCurrentPage++;
    current_page_num_ui.textContent = glCurrentPage;
    PreLoadIndex = 0;
    CurrentLoadIndex = 1;
    check_loading_status();
}

function show_assistresult(project_id, assistid, hashkey)
{
    var win = window.open('/index.php/vilsassistresult/'+project_id+'?assistid='+assistid+'&hashkey='+hashkey, '_blank');
    win.focus();
}

function open_assistrecords_page()
{
    var win = window.open('/index.php/vilsassistrecords/'+project_id, '_blank');
    win.focus();
}
