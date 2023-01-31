/*
 * Author: ikki Chung
 * Date: 2021 01 19
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var LOAD_PAGE_NUM = 20;

var gl_curriculumrecord_List = [];
var gl_curriculumapply_List = [];
var gl_curriculum_List = [];
var gl_curriculumtype_List = [];

var gl_user_info = [];

var glCurrentPage = 1;
var glReloadTimes = 0;

var gl_uploadfile_uid = '';
var gl_current_record_id = '';
var gl_NewCurriculumRecord = {};

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

                load_CurriculumType();

            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_CurriculumRecords();
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

                load_CurriculumApply();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_CurriculumRecords();

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
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

function load_CurriculumType()
{
    var targetURL = gl_target_server + "/php/curriculum.php";

    gl_curriculumtype_List = [];

    $.getJSON(targetURL, 'loadcurriculumtype=1'+'&project_id='+project_id, function(data)
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

function load_CurriculumRecords()
{
    var targetURL = gl_target_server + "/php/curriculum.php";
    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    gl_curriculumrecord_List = [];

    $.getJSON(targetURL, 'loadcurriculumrecord=1'+'&project_id='+project_id+"&offset="+offset+"&count="+count, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            record.id = parseInt(record.id);
            record.userid = parseInt(record.userid);
            record.teacheruserid = parseInt(record.teacheruserid);
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

function load_Curriculums()
{
    var targetURL = gl_target_server + "/php/curriculum.php";

    gl_curriculum_List = [];

    $.getJSON(targetURL, 'loadusercurriculums=1'+'&project_id='+project_id, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            record.id = parseInt(record.id);
            record.userid = parseInt(record.userid);
            record.typeid = parseInt(record.typeid);
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

function show_CurriculumRecords()
{
    var table_body_ui = document.getElementById('CURRICULUM_RECORD_TABLE_BODY');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    // console.log("show_curriculumrecords() gl_curriculumrecord_List.length:"+gl_curriculumrecord_List.length);

    for(var index=0;index<gl_curriculumrecord_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecord_List[index];
        show_curriculumrecord(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, curriculumrecord);
    }

}

function show_curriculumrecord(table_body_ui, ui_index, record)
{
    var cellIndex = 0;

    console.log("show_curriculumrecord() curriculumid:"+record.curriculumid);
    console.log("show_curriculumrecord() userid:"+record.userid);

    var curriculum = get_curriculum(record.curriculumid);
    if (curriculum == null)
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
    cellIndex++;

    // 課程類別[課程號碼]
    row.appendChild(document.createElement('td'));
    var curriculumTypeName = getCurriculumTypeName(curriculum.typeid);
    var show_curriculum = curriculumTypeName + ' ['+ curriculum.number+']';
    var curriculumnameDiv = document.createElement('div');
    curriculumnameDiv.setAttribute("id","curriculum_"+record.id);
    curriculumnameDiv.appendChild(document.createTextNode(show_curriculum));
    row.cells[cellIndex].appendChild(curriculumnameDiv);
    cellIndex++;

    // 課程名稱
    row.appendChild(document.createElement('td'));
    var curriculumnameDiv = document.createElement('div');
    curriculumnameDiv.setAttribute("id","curriculumname_"+record.id);
    curriculumnameDiv.appendChild(document.createTextNode(record.name));
    row.cells[cellIndex].appendChild(curriculumnameDiv);
    cellIndex++;

    // 課程日期
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_date_picker('開始:', 'startdate_'+record.id, record.startdate, false));
    row.cells[cellIndex].appendChild(create_date_picker('結束:', 'enddate_'+record.id, record.enddate, false));
    cellIndex++;

    // 課程時間
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_time_select('開始:', 'starttime_'+record.id, record.starttime, false));
    row.cells[cellIndex].appendChild(create_time_select('結束:', 'endtime_'+record.id, record.endtime, false));
    cellIndex++;

    // 課程週間日
    row.appendChild(document.createElement('td'));
    var td_date = create_weekday_select("weekday_"+record.id, record.id, record.weekday, false);
    row.cells[cellIndex].appendChild(td_date);
    cellIndex++;

    // 總時數(小時)
    row.appendChild(document.createElement('td'));
    var totalhoursDiv = document.createElement('div');
    totalhoursDiv.setAttribute("id","text_totalhours_"+record.id);
    totalhoursDiv.appendChild(document.createTextNode(record.totalhours));
    row.cells[cellIndex].appendChild(totalhoursDiv);
    cellIndex++;

    // 地點
    row.appendChild(document.createElement('td'));
    var locationDiv = document.createElement('div');
    locationDiv.setAttribute("id","text_location_"+record.id);
    locationDiv.appendChild(document.createTextNode(record.location));
    row.cells[cellIndex].appendChild(locationDiv);
    cellIndex++;

    // 內容
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.setAttribute("id","text_description_"+record.id);
    descriptionDiv.appendChild(document.createTextNode(record.description));
    row.cells[cellIndex].appendChild(descriptionDiv);
    cellIndex++;

    // 任課老師
    row.appendChild(document.createElement('td'));
    var teacherDiv = document.createElement('div');
    var user_span = document.createElement('span');
    user_span.setAttribute("class",'badge bg-green-active');
    user_span.setAttribute("id",'teacheruser_name_'+record.id+'_'+record.teacheruserid);
    user_span.textContent = getUserName(record.teacheruserid);
    teacherDiv.appendChild(user_span);
    row.cells[cellIndex].appendChild(teacherDiv);
    cellIndex++;

    // 任課老師簡介
    row.appendChild(document.createElement('td'));
    var teacherDiv = document.createElement('div');
    teacherDiv.setAttribute("id","text_teacher_"+record.id);
    var teacherLilnkDiv = document.createElement('div');
    teacherLilnkDiv.innerHTML = linkify(record.teacher);
    teacherDiv.appendChild(teacherLilnkDiv);
    row.cells[cellIndex].appendChild(teacherDiv);
    cellIndex++;

    // 費用
    row.appendChild(document.createElement('td'));
    var costDiv = document.createElement('div');
    costDiv.setAttribute("id","text_cost_"+record.id);
    costDiv.appendChild(document.createTextNode(record.cost));
    row.cells[cellIndex].appendChild(costDiv);
    cellIndex++;

    // 備註
    row.appendChild(document.createElement('td'));
    var noteDiv = document.createElement('div');
    noteDiv.setAttribute("id","text_note_"+record.id);
    noteDiv.appendChild(document.createTextNode(record.note));
    row.cells[cellIndex].appendChild(noteDiv);
    cellIndex++;

    // 簡章下載
    row.appendChild(document.createElement('td'));
    var filesDiv = document.createElement('div');
    filesDiv.setAttribute("id","storage_file_"+record.id);
    if (record.files.length > 0)
    {
        fetch_storage_file(record.id, record.files);
    }
    row.cells[cellIndex].appendChild(filesDiv);
    cellIndex++;

    // 狀態
    row.appendChild(document.createElement('td'));
    var onoffstatusDiv = document.createElement("span");
    onoffstatusDiv.setAttribute("id","onoffstatus_"+record.id);
    row.cells[cellIndex].appendChild(onoffstatusDiv);

    var Spanonoffstatus = document.createElement("span");
    if (record.status == 0)
    {
        Spanonoffstatus.textContent = '本課程暫時停止接受選課，請在截止日前重新嘗試選課';
        // Spanonoffstatus.textContent = '停止選課';
        Spanonoffstatus.setAttribute("class","badge bg-red");
    }
    else
    if (record.status == 1)
    {
        Spanonoffstatus.textContent = '開放選課';
        Spanonoffstatus.setAttribute("class","badge bg-green");
    }
    onoffstatusDiv.appendChild(Spanonoffstatus);
    cellIndex++;

    // 操作
    row.appendChild(document.createElement('td'));
    if (record.status == 1)
    {
        var status = getApplyStatus(record.id);
        if (status == 0)
        {
            // already apply
            var Spanonoffstatus = document.createElement("span");
            Spanonoffstatus.textContent = '等待審核';
            Spanonoffstatus.setAttribute("class","badge bg-red");
            row.cells[cellIndex].appendChild(Spanonoffstatus);
        }
        else
        if (status == 1)
        {
            // already apply and proved
            var Spanonoffstatus = document.createElement("span");
            Spanonoffstatus.textContent = '已加入課程';
            Spanonoffstatus.setAttribute("class","badge bg-green");
            row.cells[cellIndex].appendChild(Spanonoffstatus);
        }
        else
        if (status == 99)
        {
            // already apply and proved
            var Spanonoffstatus = document.createElement("span");
            Spanonoffstatus.textContent = '未核准';
            Spanonoffstatus.setAttribute("class","badge bg-red");
            row.cells[cellIndex].appendChild(Spanonoffstatus);
        }
        else
        {
            var editBtn = document.createElement("button");
            editBtn.setAttribute("id", "apply_curriculumrecord_btn_"+record.id);
            editBtn.setAttribute("title","加入課程");
            editBtn.setAttribute("href","javascript:;");
            editBtn.setAttribute("onclick","confirm_curriculumrecord("+ record.id +");return false;");
            editBtn.textContent = "加入課程";
            row.cells[cellIndex].appendChild(editBtn);
        }
    }

    return 1;
}

function load_user_info()
{
    var queryuserList = [];

    var bFound = check_userinfo_existed(parseInt(gl_userid));
    if ( bFound == false)
    {
        queryuserList.push(gl_userid);
    }

    for(var index=0;index<gl_curriculumrecord_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecord_List[index];

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
            if (curriculumrecord.userid > 0)
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

    for(var index=0;index<gl_curriculumapply_List.length;index++)
    {
        var curriculumapply = gl_curriculumapply_List[index];

        //
        // check exist
        //
        var bFound = check_userinfo_existed(curriculumapply.userid);
        if ( bFound == false)
        {
            if (curriculumapply.userid > 0)
            {
                var bFound = false;
                for(var j=0;j<queryuserList.length;j++)
                {
                    if ( queryuserList[j] == curriculumapply.userid )
                    {
                        bFound = true;
                        break;
                    }
                }
                if (!bFound)
                {
                    queryuserList.push(curriculumapply.userid);
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
    var p = $.getJSON(targetURL, 'queryusers=1&userid='+queryuser+'&project_id='+project_id, function(data)
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

function update_user_name()
{
    for(var index=0;index<gl_curriculumrecord_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecord_List[index];
        var user_span = document.getElementById("teacheruser_name_"+curriculumrecord.id+'_'+curriculumrecord.teacheruserid);

        // console.log("update_user_name() teacheruser_name_"+curriculumrecord.id+'_'+curriculumrecord.teacheruserid);
        if (user_span != undefined)
            user_span.textContent = getUserName(curriculumrecord.teacheruserid);
    }
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

function confirm_curriculumrecord(recordid)
{
    if (confirm('是否確認加入課程?'))
    {
        apply_curriculumrecord(recordid);
    }
    else
    {
        // Do nothing!
    }
}

function apply_curriculumrecord(recordid)
{
    var curriculumrecord = get_curriculumrecord(recordid);
    var curriculum = get_curriculum(curriculumrecord.curriculumid);
    var curriculumTypeName = getCurriculumTypeName(curriculum.typeid);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['apply_curriculumrecord'] = 1;
    jsondata['id'] = recordid;
    jsondata['number'] = curriculum.number;
    jsondata['curriculumname'] = curriculumTypeName;

    var targetURL = gl_target_server + "/php/curriculum.php";

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

            setTimeout(reload_curriculumrecords, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
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

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.setAttribute("style", "width: 120px;");
    input_text.textContent = text;

    return input_text;
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

function create_date_picker(label_text, uiid, default_value, isEditing)
{
    var div_DatePicker = document.createElement("div");
    div_DatePicker.setAttribute("style","width:150px;");

    var div_inputgroup = document.createElement("div");
    div_inputgroup.setAttribute("class","input-group");
    div_DatePicker.appendChild(div_inputgroup);

    var div_label = document.createElement("div");
    div_label.setAttribute("class","input-group-addon");
    div_label.innerHTML = label_text;
    div_inputgroup.appendChild(div_label);

    var input_Date = document.createElement("input");
    input_Date.setAttribute("type","text");
    input_Date.setAttribute("id",uiid);
    input_Date.setAttribute("style",'width:100px;');
    input_Date.setAttribute("readonly","readonly");
    if (isEditing)
    {
        input_Date.setAttribute("class","datepicker");
    }
    input_Date.setAttribute("data-date-format","yyyy-mm-dd");
    input_Date.setAttribute("value",default_value);
    div_inputgroup.appendChild(input_Date);

    return div_DatePicker;
}

function create_time_select(label_text, ui_id, default_value, isEditing)
{
    var div_time_select = document.createElement("div");

    div_time_select.appendChild(create_time_picker(label_text, ui_id, default_value, isEditing));

    return div_time_select;
}

function create_time_picker(label_text, ui_id, timetext, isEditing)
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
    div_clock.innerHTML = label_text;
    div_inputgroup.appendChild(div_clock);

    // icon
    // var div_clock_icon = document.createElement("i");
    // div_clock_icon.setAttribute("class","fa fa-clock-o");
    // div_clock.appendChild(div_clock_icon);

    // input
    var input_Time = document.createElement("input");
    input_Time.setAttribute("type","text");
    input_Time.setAttribute("id",ui_id);
    input_Time.setAttribute("readonly","readonly");
    if (isEditing)
    {
        input_Time.setAttribute("class","form-control timepicker");
    }
    else
    {
        input_Time.setAttribute("class","form-control");
    }
    input_Time.setAttribute("value",timetext);
    div_inputgroup.appendChild(input_Time);

    return div_TimePicker;
}

function create_weekday_select(ui_id, recordid, validdays, isEditing)
{
    var weekdaydiv = document.createElement("div");
    weekdaydiv.setAttribute("id",ui_id);

    var weekdayui = create_weekday_ui(recordid, validdays, isEditing);
    weekdaydiv.appendChild(weekdayui);

    return weekdaydiv;
}

function create_weekday_ui(recordid, validdays, isEditing)
{
    var weekdaybox = document.createElement("div");
    weekdaybox.setAttribute("class","box");
    weekdaybox.setAttribute("style","background-image: linear-gradient(to bottom, #a9dbde, #e4eded);");

    var weekdayboxheader = document.createElement("div");
    weekdayboxheader.setAttribute("class","box-header with-border");
    weekdaybox.appendChild(weekdayboxheader);

    var weekdayboxheadertitle = document.createElement("div");
    weekdayboxheadertitle.appendChild(document.createTextNode("課程週間日"));
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
        var checkboxui = create_checkbox(recordid, day, checked, isEditing);
        boxtabletbodytd.appendChild(checkboxui);
        boxtabletbodytr.appendChild(boxtabletbodytd);
    }

    return weekdaybox;
}

function create_checkbox(recordid, day, checked, isEditing)
{
    var inputui = document.createElement("input");
    inputui.setAttribute("id","uiid_checkbox_"+day+"_"+recordid);
    inputui.setAttribute("type","checkbox");
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

function reload_curriculumrecords()
{
    // console.log("reload_curriculumrecords() glReloadTimes:"+glReloadTimes);

    PreLoadIndex = 0;
    CurrentLoadIndex = 1;

    setTimeout(check_loading_status, 50);
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

function get_curriculumrecord(recordid)
{
    for(var index=0;index<gl_curriculumrecord_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecord_List[index];
        if ( curriculumrecord.id == recordid )
        {
            return curriculumrecord;
        }
    }

    return null;
}

function get_curriculum(curriculumid)
{
    for(var index=0;index<gl_curriculum_List.length;index++)
    {
        var curriculum = gl_curriculum_List[index];

        if ( curriculum.id == curriculumid )
        {
            return curriculum;
        }
    }
    return null;
}

function getApplyStatus(recordid)
{
    var status = -1;

    for(var index=0;index<gl_curriculumapply_List.length;index++)
    {
        var curriculumapply = gl_curriculumapply_List[index];

        if ( curriculumapply.curriculumrecordid == recordid )
        {
            return curriculumapply.status;
        }
    }

    return status;
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

function isNumeric(str)
{
    // return !/\D/.test(str);
    return (str !== '') && !isNaN(parseFloat(str)) && isFinite(str);
}

function linkify(inputText)
{
    var replacedText, replacePattern1, replacePattern2, replacePattern3;

    //URLs starting with http://, https://, or ftp://
    replacePattern1 = /(\b(https?|ftp):\/\/[-A-Z0-9+&@#\/%?=~_|!:,.;]*[-A-Z0-9+&@#\/%=~_|])/gim;
    replacedText = inputText.replace(replacePattern1, '<a href="$1" target="_blank">$1</a>');

    //URLs starting with "www." (without // before it, or it'd re-link the ones done above).
    replacePattern2 = /(^|[^\/])(www\.[\S]+(\b|$))/gim;
    replacedText = replacedText.replace(replacePattern2, '$1<a href="http://$2" target="_blank">$2</a>');

    //Change email addresses to mailto:: links.
    replacePattern3 = /(([a-zA-Z0-9\-\_\.])+@[a-zA-Z\_]+?(\.[a-zA-Z]{2,6})+)/gim;
    replacedText = replacedText.replace(replacePattern3, '<a href="mailto:$1">$1</a>');

    return replacedText;
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

function open_curriculumrecords_page()
{
    var win = window.open('/index.php/vilscurriculumrecords/'+project_id, '_blank');
    win.focus();
}
