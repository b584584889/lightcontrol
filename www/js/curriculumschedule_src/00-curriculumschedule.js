/*
 * Author: ikki Chung
 * Date: 2020 07 16
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var gl_curriculumrecord_List = [];
var gl_curriculumapply_List = [];
var gl_curriculumrecordapply_List = [];
var gl_curriculum_List = [];
var gl_curriculumtype_List = [];

var gl_user_info = [];
var gCurrentWeek = 0;

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

             load_CurriculumRecords();
         }
     break;
     case 1:
         if ( PreLoadIndex != CurrentLoadIndex )
         {
             console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
             PreLoadIndex = CurrentLoadIndex;

             load_CurriculumApply();
         }
     break;
     case 2:
         if ( PreLoadIndex != CurrentLoadIndex )
         {
             console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
             PreLoadIndex = CurrentLoadIndex;

             load_Curriculums();
         }
     break;
     case 3:
         if ( PreLoadIndex != CurrentLoadIndex )
         {
             console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
             PreLoadIndex = CurrentLoadIndex;

             load_CurriculumType();
         }
     break;
     case 4:
         if ( PreLoadIndex != CurrentLoadIndex )
         {
             console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
             PreLoadIndex = CurrentLoadIndex;

             show_schedule();
         }
     break;
     case 5:
         if ( PreLoadIndex != CurrentLoadIndex )
         {
             console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
             PreLoadIndex = CurrentLoadIndex;

             load_user_info();

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

function load_CurriculumRecords()
{
    var targetURL = gl_target_server + "/php/curriculum.php";

    gl_curriculumrecord_List = [];

    $.getJSON(targetURL, 'loadcurriculumrecord=1'+'&project_id='+project_id, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            record.id = parseInt(record.id);
            record.userid = parseInt(record.userid);
            record.curriculumid = parseInt(record.curriculumid);
            gl_curriculumrecord_List.push(record);
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

function load_CurriculumApply()
{
    var targetURL = gl_target_server + "/php/curriculum.php";

    var recordList = [];
    for(var index=0;index<gl_curriculumrecord_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecord_List[index];
        recordList.push(curriculumrecord.id);
    }

    var queryRecords = recordList.toString();

    gl_curriculumapply_List = [];

    $.getJSON(targetURL, 'loadcurriculumapply=1'+'&project_id='+project_id+'&records='+queryRecords, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            record.id = parseInt(record.id);
            record.userid = parseInt(record.userid);
            record.curriculumrecordid = parseInt(record.curriculumrecordid);
            record.status = parseInt(record.status);
            gl_curriculumapply_List.push(record);
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

function load_Curriculums()
{
    gl_curriculumrecordapply_List = [];
    var queryCurriculumList = [];

    for(var index=0;index<gl_curriculumrecord_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecord_List[index];

        var bApply = false;
        for(var i=0;i<gl_curriculumapply_List.length;i++)
        {
            var curriculumapply = gl_curriculumapply_List[i];
            if ( curriculumrecord.id == curriculumapply.curriculumrecordid && curriculumapply.status == 1)
            {
                gl_curriculumrecordapply_List.push(curriculumrecord);
                bApply = true;
                break;
            }
        }

        if (bApply)
        {
            var bFound = false;
            for(var j=0;j<queryCurriculumList.length;j++)
            {
                if ( queryCurriculumList[j] == curriculumrecord.curriculumid )
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                queryCurriculumList.push(curriculumrecord.curriculumid);
            }
        }
    }

    var queryCurriculum = queryCurriculumList.toString();

    var targetURL = gl_target_server + "/php/curriculum.php";

    gl_curriculum_List = [];

    $.getJSON(targetURL, 'loadcurriculums=1'+'&project_id='+project_id+'&curriculumid='+queryCurriculum, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            record.id = parseInt(record.id);
            record.userid = parseInt(record.userid);
            record.typeid = parseInt(record.typeid);
            record.recordid = parseInt(record.recordid);
            gl_curriculum_List.push(record);
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

function load_CurriculumType()
{
    var queryCurriculumTypeList = [];

    for(var index=0;index<gl_curriculum_List.length;index++)
    {
        var curriculum = gl_curriculum_List[index];

        var bFound = false;
        for(var j=0;j<queryCurriculumTypeList.length;j++)
        {
            if ( queryCurriculumTypeList[j] == curriculum.typeid )
            {
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            queryCurriculumTypeList.push(curriculum.typeid);
        }
    }

    var queryCurriculumType = queryCurriculumTypeList.toString();

    var targetURL = gl_target_server + "/php/curriculum.php";

    gl_curriculumtype_List = [];

    $.getJSON(targetURL, 'loadcurriculumtype=1'+'&project_id='+project_id+'&type='+queryCurriculumType, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            record.id = parseInt(record.id);
            gl_curriculumtype_List.push(record);
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

function show_schedule()
{

    show_week_schedule();

    CurrentLoadIndex = CurrentLoadIndex + 1; // next state
}

function show_week_schedule()
{
    var weekDays = getCurrentWeek(gCurrentWeek);

    // console.log(weekDays);

    var showCurrentWeek = weekDays.showDate[0] + ' ~ ' + weekDays.showDate[4];

    var current_week_ui = document.getElementById('id_current_week');
    current_week_ui.textContent = showCurrentWeek;

    var monday_date_ui = document.getElementById('monday_date');
    monday_date_ui.textContent = weekDays.showDate[0];
    var tuesday_date_ui = document.getElementById('tuesday_date');
    tuesday_date_ui.textContent = weekDays.showDate[1];
    var wednesday_date_ui = document.getElementById('wednesday_date');
    wednesday_date_ui.textContent = weekDays.showDate[2];
    var thursday_date_ui = document.getElementById('thursday_date');
    thursday_date_ui.textContent = weekDays.showDate[3];
    var friday_date_ui = document.getElementById('friday_date');
    friday_date_ui.textContent = weekDays.showDate[4];

    var weekday_1_class = create_weekday_schedule(weekDays.Date[0], '1');// monday
    var weekday_2_class = create_weekday_schedule(weekDays.Date[1], '2');// tuesday
    var weekday_3_class = create_weekday_schedule(weekDays.Date[2], '3');// wednesday
    var weekday_4_class = create_weekday_schedule(weekDays.Date[3], '4');// thursday
    var weekday_5_class = create_weekday_schedule(weekDays.Date[4], '5');// friday

    var monday_class = clearChildView("monday_class");
    var tuesday_class = clearChildView("tuesday_class");
    var wednesday_class = clearChildView("wednesday_class");
    var thursday_class = clearChildView("thursday_class");
    var friday_class = clearChildView("friday_class");

    monday_class.appendChild(weekday_1_class);
    tuesday_class.appendChild(weekday_2_class);
    wednesday_class.appendChild(weekday_3_class);
    thursday_class.appendChild(weekday_4_class);
    friday_class.appendChild(weekday_5_class);

}

function create_weekday_schedule(targetDate, weekday)
{
    var weekdayclass_div = document.createElement("div");

    var today_curriculum = [];

    for(var index=0;index<gl_curriculumrecordapply_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecordapply_List[index];

        if ( targetDate >= curriculumrecord.startdate && targetDate <= curriculumrecord.enddate )
        {
            for(var widx=0;widx<curriculumrecord.weekday.length;widx++)
            {
                var recordweekday = curriculumrecord.weekday[widx];
                if ( weekday === recordweekday )
                {
                    today_curriculum.push(curriculumrecord);
                    break;
                }
            }
        }
    }

    //
    // sort by starttime
    //
    var sorted_curriculum = [];

    while(today_curriculum.length > 0)
    {
        var record = undefined;
        var record_index = -1;

        for(var i=0;i<today_curriculum.length;i++)
        {
            var record_data = today_curriculum[i];
            if ( record == undefined )
            {
                record = record_data;
                record_index = i;
            }
            else
            {
                var result = compareFormatTiime(record.starttime, record_data.starttime);
                if ( result > 0 )
                {
                    record = record_data;
                    record_index = i;
                }
            }
        }
        sorted_curriculum.push(record);
        today_curriculum.splice(record_index, 1);

        // console.log("create_weekday_schedule() splice:"+record_index +" today_curriculum.length:"+today_curriculum.length);
    }


    for(var index=0;index<sorted_curriculum.length;index++)
    {
        var curriculumrecord = sorted_curriculum[index];

        // console.log("targetDate:"+targetDate+ " weekday:"+weekday +" curriculumrecord.name:"+curriculumrecord.name);

        var curriculumcol = create_curriculum_class(curriculumrecord);
        weekdayclass_div.appendChild(curriculumcol);
    }

    return weekdayclass_div;
}

function compareFormatTiime(firsttime, secondtime)
{
    var time_1 = moment(firsttime, 'hh:mm A');
    var time_2 = moment(secondtime, 'hh:mm A');

    return (time_1 > time_2);
}

function create_curriculum_class(curriculumrecord)
{
    var curriculumRow = document.createElement("div");
    curriculumRow.setAttribute("class","row");
    curriculumRow.setAttribute("style","text-align: center;");


    var curriculumCol = document.createElement("div");
    curriculumCol.setAttribute("class","col-sm-8");
    curriculumCol.setAttribute("style","margin-left: auto;margin-right: auto;display: block;");
    curriculumRow.appendChild(curriculumCol);

    var ul_filetype = create_class_info(curriculumrecord);
    curriculumCol.appendChild(ul_filetype);

    return curriculumRow;
}

function create_class_info(curriculumrecord)
{
    var data_ul = document.createElement("ul");
    data_ul.setAttribute("class","mailbox-attachments clearfix");

    var data_li = document.createElement("li");
    data_ul.appendChild(data_li);

    var backgroundColor = getBackgroundColor(curriculumrecord.curriculumid);
    var textColor = getTextColor(curriculumrecord.curriculumid);
    var locationtextColor = getLocationTextColor(curriculumrecord.curriculumid);

    var div_info = document.createElement("div");
    div_info.setAttribute("class","mailbox-attachment-info");
    div_info.setAttribute("style","background-color:"+backgroundColor+";");
    data_li.appendChild(div_info);

    var fileanme_label = document.createElement("p");
    fileanme_label.setAttribute("class","overflow-ellipsis");
    fileanme_label.setAttribute("style","white-space: nowrap;overflow: hidden;font-weight:bold;color:"+textColor+";");
    fileanme_label.textContent = curriculumrecord.name;
    div_info.appendChild(fileanme_label);

    var div_info_starttime = document.createElement("span");
    div_info_starttime.setAttribute("class","mailbox-attachment-size");
    div_info_starttime.setAttribute("style","text-align: center;color:"+textColor+";");
    div_info_starttime.textContent = curriculumrecord.starttime;
    div_info.appendChild(div_info_starttime);

    var div_info_endtime = document.createElement("span");
    div_info_endtime.setAttribute("class","mailbox-attachment-size");
    div_info_endtime.setAttribute("style","text-align: center;color:"+textColor+";");
    div_info_endtime.textContent = curriculumrecord.endtime;
    div_info.appendChild(div_info_endtime);

    var div_info_href_size = document.createElement("span");
    div_info_href_size.setAttribute("class","mailbox-attachment-size");
    div_info_href_size.setAttribute("style","text-align:center;font-weight:bold;color:"+locationtextColor+";");
    div_info_href_size.textContent = '地點:' + curriculumrecord.location;
    div_info.appendChild(div_info_href_size);

    var div_info_href_detail = document.createElement("a");
    div_info_href_detail.setAttribute("class","btn btn-default btn-xs pull-left");
    div_info_href_size.appendChild(div_info_href_detail);

    var div_info_href_detail_img = document.createElement("i");
    div_info_href_detail_img.setAttribute("title","查看詳細資訊");
    div_info_href_detail_img.setAttribute("onclick","open_detail_record(\""+curriculumrecord.id+"\");return false;");
    div_info_href_detail_img.setAttribute("class","fa fa-search-plus");
    div_info_href_detail.appendChild(div_info_href_detail_img);

    if (curriculumrecord.files.length > 0)
    {
        var div_info_href_dl = document.createElement("a");
        div_info_href_dl.setAttribute("title","下載檔案");
        div_info_href_dl.setAttribute("href","/php/downloadfile.php?project_id="+project_id+"&fileid="+curriculumrecord.files);
        div_info_href_dl.setAttribute("class","btn btn-default btn-xs pull-right");
        div_info_href_size.appendChild(div_info_href_dl);

        var div_info_href_dl_img = document.createElement("i");
        div_info_href_dl_img.setAttribute("class","fa fa-cloud-download");
        div_info_href_dl.appendChild(div_info_href_dl_img);
    }

    return data_ul;
}

function open_detail_record(curriculumrecord_id)
{
    $("#modal-record_detail").modal('toggle');

    var curriculumrecord = get_curriculumrecord(curriculumrecord_id);
    var curriculum = get_curriculum(curriculumrecord.curriculumid);
    var curriculumTypeName = getCurriculumTypeName(curriculum.typeid);

    var record_detail_name = document.getElementById("record_detail_name");
    record_detail_name.textContent = '課程名稱: ' + curriculumrecord.name;

    var record_detail_type = document.getElementById("record_detail_type");
    record_detail_type.textContent = '課程類別[課程號碼]: ' + curriculumTypeName+'['+ curriculumrecord.number+']';

    var record_detail_time = document.getElementById("record_detail_time");
    record_detail_time.textContent = '課程時間: ' + curriculumrecord.starttime +' ~ ' + curriculumrecord.endtime;

    var record_detail_signintime = document.getElementById("record_detail_signintime");
    record_detail_signintime.textContent = '課程簽到時間: ' + curriculumrecord.signinstarttime +' ~ ' + curriculumrecord.signinendtime;

    var record_detail_totalhours = document.getElementById("record_detail_totalhours");
    record_detail_totalhours.textContent = '總時數(小時): ' + curriculumrecord.totalhours;

    var record_detail_location = document.getElementById("record_detail_location");
    record_detail_location.textContent = '地點: ' + curriculumrecord.location;

    var record_detail_description = document.getElementById("record_detail_description");
    record_detail_description.textContent = '內容: ' + curriculumrecord.description;

    var record_detail_teacher = document.getElementById("record_detail_teacher");
    record_detail_teacher.textContent = '任課老師: ';

    console.log("teacheruserid:"+curriculumrecord.teacheruserid);

    var user_span = document.createElement('span');
    user_span.setAttribute("class",'badge bg-aqua-active');
    user_span.textContent = getUserName(curriculumrecord.teacheruserid);
    record_detail_teacher.appendChild(user_span);

    var record_detail_cost = document.getElementById("record_detail_cost");
    record_detail_cost.textContent = '費用: ' + curriculumrecord.cost;

    var record_detail_note = document.getElementById("record_detail_note");
    record_detail_note.textContent = '備註: ' + curriculumrecord.note;

}

function getBackgroundColor(curriculumid)
{
    var backgroundColor = '#33ff56';

    var curriculum = get_curriculum(curriculumid);

    var uiIndex = curriculum.typeid % 10;

    switch(uiIndex)
    {
        default:
        case 0:
            backgroundColor = '#fe8a71';
        break;
        case 1:
            backgroundColor = '#f6cd61';
        break;
        case 2:
            backgroundColor = '#2ab7ca';
        break;
        case 3:
            backgroundColor = '#fe9c8f';
        break;
        case 4:
            backgroundColor = '#851e3e';
        break;
        case 5:
            backgroundColor = '#f9caa7';
        break;
        case 6:
            backgroundColor = '#33ff56';
        break;
        case 7:
            backgroundColor = '#3da4ab';
        break;
        case 8:
            backgroundColor = '#0e9aa7';
        break;
        case 9:
            backgroundColor = '#4a4e4d';
        break;
    }

    return backgroundColor;
}

function getTextColor(curriculumid)
{
    var textColor = '#33ff56';

    var curriculum = get_curriculum(curriculumid);

    var uiIndex = curriculum.typeid % 10;

    switch(uiIndex)
    {
        default:
        case 0:
            textColor = '#336621';
        break;
        case 1:
            textColor = '#336621';
        break;
        case 2:
            textColor = '#336621';
        break;
        case 3:
            textColor = '#336621';
        break;
        case 4:
            textColor = '#aaccdd';
        break;
        case 5:
            textColor = '#336621';
        break;
        case 6:
            textColor = '#336621';
        break;
        case 7:
            textColor = '#336621';
        break;
        case 8:
            textColor = '#336621';
        break;
        case 9:
            textColor = '#336621';
        break;
    }

    return textColor;
}

function getLocationTextColor(curriculumid)
{
    var locationTextColor = '#33ff56';

    var curriculum = get_curriculum(curriculumid);

    var uiIndex = curriculum.typeid % 10;

    switch(uiIndex)
    {
        default:
        case 0:
            locationTextColor = '#443311';
        break;
        case 1:
            locationTextColor = '#443311';
        break;
        case 2:
            locationTextColor = '#443311';
        break;
        case 3:
            locationTextColor = '#443311';
        break;
        case 4:
            locationTextColor = '#aaccdd';
        break;
        case 5:
            locationTextColor = '#443311';
        break;
        case 6:
            locationTextColor = '#443311';
        break;
        case 7:
            locationTextColor = '#443311';
        break;
        case 8:
            locationTextColor = '#443311';
        break;
        case 9:
            locationTextColor = '#443311';
        break;
    }

    return locationTextColor;
}

function load_user_info()
{
    var queryuserList = [];

    var bFound = check_userinfo_existed(parseInt(gl_userid));
    if ( bFound == false)
    {
        queryuserList.push(gl_userid);
    }

    for(var index=0;index<gl_curriculumrecordapply_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecordapply_List[index];

        //
        // check exist
        //
        var bFound = check_userinfo_existed(curriculumrecord.userid);
        if ( bFound == false)
        {
            if (curriculumrecord.userid > 0)
            {
                var bFound = false;
                for(var j=0;j<queryuserList.length;j++)
                {
                    if ( queryuserList[j] == curriculumrecord.userid )
                    {
                        bFound = true;
                        break;
                    }
                }
                if (!bFound)
                {
                    queryuserList.push(curriculumrecord.userid);
                }
            }
        }

        bFound = check_userinfo_existed(curriculumrecord.teacheruserid);
        if ( bFound == false)
        {
            if (curriculumrecord.teacheruserid > 0)
            {
                var bFound = false;
                for(var j=0;j<queryuserList.length;j++)
                {
                    if ( queryuserList[j] == curriculumrecord.teacheruserid )
                    {
                        bFound = true;
                        break;
                    }
                }
                if (!bFound)
                {
                    queryuserList.push(curriculumrecord.teacheruserid);
                }
            }
        }
    }

    if (queryuserList.length == 0)
    {
        update_user_name();

        return;
    }

    var queryuser = queryuserList.toString();

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadusername=1&user='+queryuser+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, user)
        {
            user.userid = parseInt(user.userid);
            gl_user_info.push(user);
        });
    })
    .success(function() {
        update_user_name();
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function check_userinfo_existed(userid)
{
    var bFound = false;
    for (var j = 0; j < gl_user_info.length; ++j)
    {
        var existed_user = gl_user_info[j];
        if ( existed_user.userid == userid )
        {
            bFound = true;
            break;
        }
    }
    return bFound;
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

function get_curriculumrecord(id)
{
    for(var index=0;index<gl_curriculumrecord_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecord_List[index];
        if (curriculumrecord.id == id)
        {
            return curriculumrecord;
        }
    }
    return undefined;
}

function get_curriculum(curriculumid)
{
    for(var index=0;index<gl_curriculum_List.length;index++)
    {
        var curriculum = gl_curriculum_List[index];

        if (curriculumid == 0)
        {
            return curriculum;
        }
        else
        if ( curriculum.id == curriculumid )
        {
            return curriculum;
        }
    }

    return curriculum;
}

function getCurriculumTypeName(typeid)
{
    var typename = "";
    for(var index=0;index<gl_curriculumtype_List.length;index++)
    {
        var curriculumtype = gl_curriculumtype_List[index];

        // console.log("getCurriculumTypeName() typeid:"+typeid+" curriculumtype.typeid:"+curriculumtype.id);

        if( typeid == curriculumtype.id )
        {
            typename = curriculumtype.name;
            break;
        }
    }
    return typename;
}

function update_user_name()
{
    for(var index=0;index<gl_curriculumrecordapply_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecordapply_List[index];
        var user_span = document.getElementById("user_name_"+curriculumrecord.id+'_'+curriculumrecord.userid);
        if (user_span != undefined)
            user_span.textContent = getUserName(curriculumrecord.userid);
        var user_span1 = document.getElementById("teacheruser_name_"+curriculumrecord.id+'_'+curriculumrecord.teacheruserid);
        if (user_span1 != undefined)
            user_span1.textContent = getUserName(curriculumrecord.teacheruserid);
    }
}

function getCurrentWeek(gCurrentWeek)
{
    var currentDate = moment();

    //var weekStart = currentDate.clone().startOf('isoWeek');
    //var weekEnd = currentDate.clone().endOf('isoWeek');

    var weekStart = currentDate.clone().add(gCurrentWeek, 'weeks').startOf('isoWeek');
    // var weekEnd = moment().add(1, 'weeks').endOf('isoWeek');

    var days = {};
    days.showDate = [];
    days.Date = [];

    for (var i = 0; i <= 6; i++)
    {
        // days.push(moment(weekStart).add(i, 'days').format("MMMM Do,dddd"));
        days.showDate.push(moment(weekStart).add(i, 'days').format("MM/DD"));
        days.Date.push(moment(weekStart).add(i, 'days').format("YYYY-MM-DD"));
    }
    //console.log(days);

    return days;
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

function move_week_backward()
{
    // var current_week_ui = document.getElementById('id_current_week');

    gCurrentWeek--;
    //current_week_ui.textContent = glCurrentPage;
    show_week_schedule();

}

function move_week_forward()
{
    // var current_week_ui = document.getElementById('id_current_week');

    gCurrentWeek++;
    //current_week_ui.textContent = glCurrentPage;
    show_week_schedule();
}

function open_weekschedule_page()
{
    var win = window.open('/index.php/vilsweekschedule/'+project_id, '_blank');
    win.focus();
}
