/*
 * Author: ikki Chung
 * Date: 2020 10 07
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var gl_user_nameinfo = [];
var gl_userids = [];
// var gl_useraccount = [];

var gl_assist = {};
var gl_assistevent = {};
var gl_assisteventrecords = [];
var glResponseImage_List = {};

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
                //gl_useraccount = [];

                load_assist(gl_assistid);
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_assistevent(gl_assisteventid);
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_user_nameinfo();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_assistevent();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_assisteventrecords(gl_assisteventid, gl_assistid);
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_user_nameinfo();
            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_assisteventrecords();

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

function load_assist(assistid)
{
    var targetURL = gl_target_server + "/php/assist.php";

    gl_assist = {};

    $.getJSON(targetURL, 'loadassist=1'+'&project_id='+project_id+'&assistid='+assistid, function(data)
    {
        data.id = parseInt(data.id);
        gl_assist = data;

        //console.log("load_assist() gl_assist.accountid:"+gl_assist.accountid);

        add_user(gl_assist.userid);
        // add_useraccount(gl_assist.accountid);

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

function load_assistevent(assisteventid)
{
    var targetURL = gl_target_server + "/php/assist.php";

    gl_assistevent = {};

    $.getJSON(targetURL, 'loadassistevent=1'+'&project_id='+project_id+"&assisteventid="+assisteventid, function(data)
    {
        data.id = parseInt(data.id);
        data.assistid = parseInt(data.assistid);
        gl_assistevent = data;

        add_user(gl_assistevent.userid);
        // add_useraccount(gl_assistevent.accountid);

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

function show_assistevent()
{
    var table_body_ui = document.getElementById('ASSIST_EVENT_TABLE_BODY');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    // console.log("show_assistevents() gl_assistevent_List.length:"+gl_assistevent_List.length);

    show_assistevent_data(table_body_ui, gl_assistevent);

    var calendar_id = "div_calendar_"+gl_assistevent.id;
    var isEditing = false;

    $('#'+calendar_id).datepicker(
    {
        applyid: gl_assistevent.id,
        language: 'zh-TW',
        weekStart: 1,
        beforeShowDay: function (date, applyid)
        {
            return checkValidDate(date, gl_assistevent, isEditing, false);
        }
    });
}

function show_assistevent_data(table_body_ui, record)
{
    var cellIndex = 0;

    // console.log("show_assistevent() assist.title:"+assist.title);

    var row = document.createElement('tr');
    row.setAttribute("id", record.id+"_TABLE_ROW");
    table_body_ui.appendChild(row);

    var row_extra = document.createElement('tr');
    row_extra.setAttribute("id", record.id+"_TABLE_ROW_EXTRA");
    table_body_ui.appendChild(row_extra);

    // 紀錄id
    row.appendChild(document.createElement('td'));
    var titleDiv = document.createElement('div');
    titleDiv.appendChild(document.createTextNode(record.datetime));
    row.cells[cellIndex].appendChild(titleDiv);
    cellIndex++;

    // 事項名稱
    row.appendChild(document.createElement('td'));
    var titleDiv = document.createElement('div');
    titleDiv.appendChild(document.createTextNode(gl_assist.title));
    row.cells[cellIndex].appendChild(titleDiv);
    cellIndex++;

    // 事項說明
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.setAttribute("id", "description_"+record.id);
    descriptionDiv.appendChild(document.createTextNode(gl_assist.description));
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

function load_assisteventrecords(assisteventid, assistid)
{
    var targetURL = gl_target_server + "/php/assist.php";

    gl_assisteventrecords = [];

    $.getJSON(targetURL, 'loadassisteventrecords=1'+'&project_id='+project_id+"&assistid="+assistid+"&assisteventid="+assisteventid+"&count=5", function(data)
    {
        $.each(data, function(itemindex, record)
        {
            add_user(record.userid);
            // add_useraccount(record.accountid);

            gl_assisteventrecords.push(record);
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

function show_assisteventrecords()
{
    var table_body_assisteventrecord = document.getElementById("ASSIST_EVENTRECORD_TABLE_BODY");

    for(var index=0;index<gl_assisteventrecords.length;index++)
    {
        record = gl_assisteventrecords[index];

        show_assistevent_record_item(table_body_assisteventrecord, index, record);

        load_assistevent_record_data(record);
    }
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

    // 負責人
    var record_td_account = document.createElement('td');
    record_td_account.setAttribute("id","record_account_"+record.id);
    record_tr.appendChild(record_td_account);

    // 回報狀態
    var record_td_status = document.createElement('td');
    record_tr.appendChild(record_td_status);

    // 回報訊息
    var record_td_comment = document.createElement('td');
    record_td_comment.setAttribute("id","record_comment_"+record.id);
    record_tr.appendChild(record_td_comment);

    // 回報照片
    var record_td_photo = document.createElement('td');
    record_td_photo.setAttribute("id","record_photo_"+record.id);
    record_tr.appendChild(record_td_photo);

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

function load_assistevent_record_data(record)
{
    //  fetch SurveyEventRecord from record.PushNotifyMessageRecorddbId

    var targetURL = gl_target_server + "/php/survey.php";

    surveyevent = {};

    $.getJSON(targetURL, 'loadsurveyeventrecord=1'+'&project_id='+project_id+'&PushNotifyMessageRecorddbId='+record.PushNotifyMessageRecorddbId, function(data)
    {
        surveyevent = data;
    })
    .success(function() {

        load_assistevent_surveyresult(record, surveyevent);
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_assistevent_surveyresult(record, surveyevent)
{
    // fetch GAESurveyResultData from SurveyEventRecord.hashkey and record.messagehashkey

    var targetURL = gl_target_server + "/php/survey.php";

    surveyresult = {};

    $.getJSON(targetURL, 'loadsurveyresult=1'+'&project_id='+project_id+'&hashkey='+surveyevent.hashkey+'&messagehashkey='+record.messagehashkey, function(data)
    {
        surveyresult = data;
    })
    .success(function() {

        show_assistevent_record_data(record, surveyevent, surveyresult);
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function show_assistevent_record_data(record, surveyevent, surveyresult)
{
    console.log("show_assistevent_record_data() ");

    var record_td_account = document.getElementById("record_account_"+record.id);
    record_td_account.textContent = getUserName(record.userid);

    var record_td_comment = document.getElementById("record_comment_"+record.id);
    record_td_comment.textContent = surveyresult.comment;

    load_reponse_image(record.id, record.messagehashkey);
}

function load_reponse_image(dbId, messagehashkey)
{
    glResponseImage_List[dbId] = [];

    if (messagehashkey.length == 0)
        return;

    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadnotifyimage=1&project_id='+project_id+'&messagehashkey='+messagehashkey, function(data)
    {
        $.each(data, function(index, storageimage)
        {
            glResponseImage_List[dbId].push(storageimage);
        });
    })
    .success(function() {

        show_reponse_image(dbId);

    })
    .error(function() {
    })
    .complete(function() {
    });
}

function show_reponse_image(dbId)
{
    var record_photo_ui = document.getElementById("record_photo_"+dbId);

    var storageImage_List = glResponseImage_List[dbId];

    for(var index=0;index<storageImage_List.length;index++)
    {
        var storageImage = storageImage_List[index];

        console.log("show_reponse_image() description:"+storageImage.description);

        var image_id = "reponse_image_" + dbId +"_" + storageImage.imageuid;

        var image_ui = document.getElementById(image_id);
        if (image_ui == undefined)
        {
            // row_ui.setAttribute("src", thumbimagelink);
            //var image_ui = create_reponse_image_item(dbId, storageImage);
            image_ui = document.createElement('span');
            image_ui.setAttribute("id","reponse_image_" + dbId +"_" + storageImage.imageuid);

            record_photo_ui.appendChild(image_ui);
        }

        update_reponse_image_item(dbId, storageImage);
    }
}


function update_reponse_image_item(dbId, storageImage)
{
    var image_id = "show_img_" + storageImage.imageuid;

    var image_ui = document.getElementById(image_id);
    if (image_ui != undefined)
    {
        return;
    }

    var reponse_image_ui = document.getElementById("reponse_image_" + dbId +"_" + storageImage.imageuid);

    var col_filetype = create_data_image(storageImage.id, storageImage.description, storageImage.thumbpath, storageImage.imagetype, storageImage.imagesize);
    // var col_filetype = create_data_image("使用者回傳照片", dbId, storageImage);
    col_filetype.setAttribute("id",image_id);
    reponse_image_ui.appendChild(col_filetype);
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

// function move_page_backward()
// {
//     var current_page_num_ui = document.getElementById('id_current_page_num');
//
//     if ( glCurrentPage <= 1 )
//     {
//         // do nothing
//     }
//     else
//     {
//         glCurrentPage--;
//         current_page_num_ui.textContent = glCurrentPage;
//         PreLoadIndex = 0;
//         CurrentLoadIndex = 1;
//         check_loading_status();
//     }
// }
//
// function move_page_forward()
// {
//     var current_page_num_ui = document.getElementById('id_current_page_num');
//
//     glCurrentPage++;
//     current_page_num_ui.textContent = glCurrentPage;
//     PreLoadIndex = 0;
//     CurrentLoadIndex = 1;
//     check_loading_status();
// }

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
