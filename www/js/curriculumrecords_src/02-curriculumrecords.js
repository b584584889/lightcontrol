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

var gl_curriculumtype_List = [];
var gl_curriculumrecord_List = [];
var gl_curriculum_List = [];
// var gl_shared_curriculum_List = [];
var gl_classAreas = [];// put area info
var gl_curriculumapply_List = [];

var gl_user_info = [];
var gl_usermaingroups = [];

var glCurrentPage = 1;
var glReloadTimes = 0;

var gl_uploadfile_uid = '';
var gl_current_record_id = '';
var gl_NewCurriculumRecord = {};

var LOAD_USER_NUM = 500;
var gl_user_load_index = 0;

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

                // load_shared_myCurriculums();

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_CurriculumRecords();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                gl_user_info = [];
                gl_user_load_index = 0;
                load_all_user_info();
            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_classAreas();
                load_user_info();
                // load_usermaingroup();

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

function load_Curriculums()
{
    var targetURL = gl_target_server + "/php/curriculum.php";

    gl_curriculum_List = [];

    $.getJSON(targetURL, 'loadcurriculums=1'+'&project_id='+project_id, function(data)
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

// function load_shared_myCurriculums()
// {
//     var targetURL = gl_target_server + "/php/curriculum.php";
//
//     $.getJSON(targetURL, 'loadsharedmycurriculums=1'+'&project_id='+project_id, function(data)
//     {
//         $.each(data, function(itemindex, record)
//         {
//             record.id = parseInt(record.id);
//             record.curriculumid = parseInt(record.curriculumid);
//             gl_shared_curriculum_List.push(record);
//
//             var curriculum = {};
//             curriculum.id = parseInt(record.curriculumid);
//             curriculum.userid = parseInt(record.userid);
//             curriculum.typeid = parseInt(record.typeid);
//             curriculum.subgroups = record.subgroups;
//             curriculum.maingroups = record.maingroups;
//             curriculum.number = record.number;
//             gl_curriculum_List.push(curriculum);
//
//         });
//     })
//     .success(function() {
//         CurrentLoadIndex = CurrentLoadIndex + 1; // next state
//     })
//     .error(function() {
//         CurrentLoadIndex = CurrentLoadIndex + 1; // next state
//     })
//     .complete(function() {
//     });
// }

function load_classAreas()
{
    //
    // Get Area first
    //
    var targetURL = gl_target_server + "/php/map.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loadLocationArea=1&type=4', function(data)
    {
        var index = 0;
        $.each(data, function(itemindex, area)
        {
            var area_json = JSON.parse(area);
            gl_classAreas[index] = {};
            gl_classAreas[index].itemid = area_json.properties.areaid;
            gl_classAreas[index].itemname = area_json.properties.areaname;
            gl_classAreas[index].itemtype = area_json.properties.areatype;
            index++;
        });

        gl_classAreas[index] = {};
        gl_classAreas[index].itemid = "other";
        gl_classAreas[index].itemname = "其他";
        gl_classAreas[index].itemtype = 0;
        index++;

    }).success(function() {

    })
    .error(function() {

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

    // console.log("show_curriculumrecords() gl_assistevent_List.length:"+gl_assistevent_List.length);

    for(var index=0;index<gl_curriculumrecord_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecord_List[index];
        show_curriculumrecord(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, curriculumrecord);
    }

    $('.timepicker').timepicker({
      showInputs: false,
      minuteStep: 5,
      showMeridian: true
    })
}

function show_curriculumrecord(table_body_ui, ui_index, record)
{
    var cellIndex = 0;

    // console.log("show_curriculumrecord() curriculumid:"+record.curriculumid);
    // console.log("show_curriculumrecord() userid:"+record.userid);

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

    var div_info_href_dl = document.createElement("a");
    div_info_href_dl.setAttribute("id",record.id +"_apply_detail_button");
    div_info_href_dl.setAttribute("title","顯示選課狀況");
    div_info_href_dl.setAttribute("onclick","open_apply_detail("+record.id+");");
    div_info_href_dl.setAttribute("class","btn btn-default btn-xs pull-right");

    var div_info_href_dl_img = document.createElement("i");
    div_info_href_dl_img.setAttribute("id",record.id +"_apply_detail_icon");
    div_info_href_dl_img.setAttribute("class","fa fa-plus text-green");
    div_info_href_dl.appendChild(div_info_href_dl_img);
    row.cells[cellIndex].appendChild(div_info_href_dl);

    cellIndex++;

    // 發起人
    row.appendChild(document.createElement('td'));
    var userDiv = document.createElement('div');
    userDiv.appendChild(create_user_label(curriculum.id, curriculum.userid));
    row.cells[cellIndex].appendChild(userDiv);
    cellIndex++;

    // 編輯人員
    row.appendChild(document.createElement('td'));
    var userDiv = document.createElement('div');
    userDiv.appendChild(create_user_label(record.id, record.userid));
    row.cells[cellIndex].appendChild(userDiv);
    cellIndex++;

    // 課程類別[課程號碼]
    row.appendChild(document.createElement('td'));
    var curriculumTypeName = getCurriculumTypeName(curriculum.typeid);
    var show_curriculum = curriculumTypeName + ' ['+ curriculum.number+']';
    var curriculumTypenameDiv = document.createElement('div');
    curriculumTypenameDiv.setAttribute("id","curriculum_"+record.id);
    curriculumTypenameDiv.appendChild(document.createTextNode(show_curriculum));
    row.cells[cellIndex].appendChild(curriculumTypenameDiv);
    cellIndex++;

    // 課程名稱
    row.appendChild(document.createElement('td'));
    var curriculumnameDiv = document.createElement('div');
    curriculumnameDiv.setAttribute("id","text_curriculumname_"+record.id);
    curriculumnameDiv.appendChild(document.createTextNode(record.name));
    row.cells[cellIndex].appendChild(curriculumnameDiv);
    cellIndex++;

    // 開放對象
    // row.appendChild(document.createElement('td'));
    // var maingroupDiv = document.createElement('div');
    // maingroupDiv.setAttribute("id","maingroup_"+record.id);
    // maingroupDiv.appendChild(create_maingroup_label(record.id, record.maingroups));
    // row.cells[cellIndex].appendChild(maingroupDiv);
    // cellIndex++;

    // 開放選修日期
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_date_picker('開始:', 'opendate_'+record.id, record.opendate, false));
    row.cells[cellIndex].appendChild(create_date_picker('結束:', 'closedate_'+record.id, record.closedate, false));
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

    // 課程簽到時間
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_time_select('開始:', 'signinstarttime_'+record.id, record.signinstarttime, false));
    row.cells[cellIndex].appendChild(create_time_select('結束:', 'signinendtime_'+record.id, record.signinendtime, false));
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
    teacherDiv.setAttribute("id",'dropdown_teacheruserid_'+record.id);
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

    // 預定人數
    row.appendChild(document.createElement('td'));
    var maxnumberDiv = document.createElement('div');
    maxnumberDiv.setAttribute("id","text_maxnumber_"+record.id);
    maxnumberDiv.appendChild(document.createTextNode(record.maxnumber));
    row.cells[cellIndex].appendChild(maxnumberDiv);
    cellIndex++;

    // 自動核準
    row.appendChild(document.createElement('td'));
    var autoapprovalDiv = document.createElement('div');
    autoapprovalDiv.setAttribute("id","text_autoapproval_"+record.id);
    if (record.autoapproval == 0)
    {
        autoapprovalDiv.appendChild(document.createTextNode('手動核準'));
    }
    else
    {
        autoapprovalDiv.appendChild(document.createTextNode('自動核準'));
    }
    row.cells[cellIndex].appendChild(autoapprovalDiv);
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
        Spanonoffstatus.textContent = '停止選課';
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
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id", "edit_curriculumrecord_btn_"+record.id);
    editBtn.setAttribute("title","Edit this Curriculum Record");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_curriculumrecord("+ record.id +");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id", "edit_curriculumrecord_img_"+record.id);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    // Delete button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("id", "delete_curriculumrecord_btn_"+record.id);
    deleteBtn.setAttribute("style","display:none");
    deleteBtn.setAttribute("title","Delete this Curriculum Record");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","confirm_delete_curriculumrecord("+ record.id +");return false;");
    var imgdelete = document.createElement("i");
    imgdelete.setAttribute("class","fa fa-trash-o");
    deleteBtn.appendChild(imgdelete);
    row.cells[cellIndex].appendChild(deleteBtn);

    return 1;
}

function load_all_user_info()
{
    var user_num = 0;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduserbaseinfo=1&s='+gl_user_load_index+'&count='+LOAD_USER_NUM+'&project_id='+project_id, function(data)
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

        console.log("load_all_user_info() user_num:"+user_num);

        if (user_num < LOAD_USER_NUM)
        {
            CurrentLoadIndex = CurrentLoadIndex + 1;
        }
        else
        {
            setTimeout(load_all_user_info, 20);
        }
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .complete(function() {
    });
}

function edit_curriculumrecord(recordid)
{
    var curriculumnameDiv = clearChildView("curriculum_"+recordid);

    var cur_curriculumrecord = get_CurriculumRecord(recordid);
    curriculumnameDiv.appendChild(create_curriculum_select(recordid, cur_curriculumrecord.curriculumid));

    // var maingroupDiv = clearChildView("maingroup_"+recordid);
    // var cur_curriculum = get_curriculum(cur_curriculumrecord.curriculumid);
    // maingroupDiv.appendChild(create_edit_maingrouplist_ui(recordid, cur_curriculum.maingroups, cur_curriculumrecord.maingroups));

    var opendate_ui = document.getElementById("opendate_"+recordid);
    var closedate_ui = document.getElementById("closedate_"+recordid);
    var startdate_ui = document.getElementById("startdate_"+recordid);
    var enddate_ui = document.getElementById("enddate_"+recordid);
    opendate_ui.setAttribute("class","datepicker");
    closedate_ui.setAttribute("class","datepicker");
    startdate_ui.setAttribute("class","datepicker");
    enddate_ui.setAttribute("class","datepicker");

    var starttime_ui = document.getElementById("starttime_"+recordid);
    var endtime_ui = document.getElementById("endtime_"+recordid);
    var signinstarttime_ui = document.getElementById("signinstarttime_"+recordid);
    var signinendtime_ui = document.getElementById("signinendtime_"+recordid);
    starttime_ui.setAttribute("class","form-control timepicker");
    starttime_ui.removeAttribute("disabled");
    endtime_ui.setAttribute("class","form-control timepicker");
    endtime_ui.removeAttribute("disabled");
    signinstarttime_ui.setAttribute("class","form-control timepicker");
    signinstarttime_ui.removeAttribute("disabled");
    signinendtime_ui.setAttribute("class","form-control timepicker");
    signinendtime_ui.removeAttribute("disabled");

    $('.timepicker').timepicker({
      showInputs: false,
      minuteStep: 5,
      showMeridian: true
    })

    // $('.timepicker').timepicker(
    // {
    //     showInputs: false,
    //     showMeridian: false,
    //     showSeconds: false,
    //     minuteStep: 5,
    //     secondStep: 30
    // });

    $('.datepicker').datepicker(
    {
        language: 'zh-TW',
        weekStart: 1
    });

    var weekdaydiv = clearChildView("weekday_"+recordid);
    var weekdayui = create_weekday_ui(recordid, cur_curriculumrecord.weekday, true);
    weekdaydiv.appendChild(weekdayui);

    var curriculumnameDiv = clearChildView("text_curriculumname_"+recordid);
    var totalhoursDiv = clearChildView("text_totalhours_"+recordid);
    var locationDiv = clearChildView("text_location_"+recordid);
    var descriptionDiv = clearChildView("text_description_"+recordid);
    var teacherDiv = clearChildView("text_teacher_"+recordid);
    var maxnumberDiv = clearChildView("text_maxnumber_"+recordid);
    var autoapprovalDiv = clearChildView("text_autoapproval_"+recordid);
    var costDiv = clearChildView("text_cost_"+recordid);
    var noteDiv = clearChildView("text_note_"+recordid);

    curriculumnameDiv.appendChild(create_input_text('curriculumname_'+recordid, cur_curriculumrecord.name));
    totalhoursDiv.appendChild(create_input_text('totalhours_'+recordid, cur_curriculumrecord.totalhours));
    locationDiv.appendChild(create_input_text('location_'+recordid, cur_curriculumrecord.location));
    var locationSelectDiv = document.createElement('div');
    locationSelectDiv.appendChild(create_location_select(recordid, cur_curriculumrecord.location));
    locationDiv.appendChild(locationSelectDiv);

    descriptionDiv.appendChild(create_input_text('description_'+recordid, cur_curriculumrecord.description));

    create_dropdown_user_ui('dropdown_teacheruserid', recordid, cur_curriculumrecord.teacheruserid);

    teacherDiv.appendChild(create_input_text('teacher_'+recordid, cur_curriculumrecord.teacher));
    maxnumberDiv.appendChild(create_input_text('maxnumber_'+recordid, cur_curriculumrecord.maxnumber));
    costDiv.appendChild(create_input_text('cost_'+recordid, cur_curriculumrecord.cost));
    noteDiv.appendChild(create_input_text('note_'+recordid, cur_curriculumrecord.note));

    var input_item = document.createElement('input');
    input_item.setAttribute("id","autoapproval_"+recordid);
    input_item.setAttribute("type","checkbox");
    input_item.setAttribute("class","flat-red");
    input_item.setAttribute("checked",(cur_curriculumrecord.autoapproval == 1));
    autoapprovalDiv.appendChild(input_item);

    if (cur_curriculumrecord.files.length > 0)
    {
        var filesDiv = document.getElementById("storage_file_"+recordid);

        var delete_file_btn = document.createElement('button');
        delete_file_btn.setAttribute("id","remove_record_file_btn_"+recordid);
        delete_file_btn.setAttribute("class","btn btn-danger");
        delete_file_btn.setAttribute("onclick","remove_record_file_click("+recordid+");return false;");
        filesDiv.appendChild(delete_file_btn);

        var delete_file_btn_img = document.createElement('i');
        delete_file_btn_img.setAttribute("class","fa fa-minus");
        delete_file_btn.appendChild(delete_file_btn_img);
    }
    else
    {
        var filesDiv = document.getElementById("storage_file_"+recordid);
        var div_upload = create_upload_file_ui('files_'+recordid);
        filesDiv.appendChild(div_upload);
    }

    var span_onoffstatus = clearChildView("onoffstatus_"+recordid);
    var onoffAction = create_option_onoffAction(recordid, cur_curriculumrecord.status);
    span_onoffstatus.appendChild(onoffAction);

    var editBtn = document.getElementById("edit_curriculumrecord_btn_"+recordid);
    var editImg = document.getElementById("edit_curriculumrecord_img_"+recordid);
    editBtn.setAttribute("title","Save this Curriculum Record");
    editBtn.setAttribute("onclick","save_curriculum_record("+ recordid +");return false;");
    editImg.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("delete_curriculumrecord_btn_"+recordid);
    deleteBtn.setAttribute("style","display:block");
}

function create_dropdown_user_ui(uiid, recordid, teacheruserid)
{
    var ui_userids = clearChildView(uiid+'_'+recordid);
    ui_userids.value = teacheruserid;

    var ui_manager_dropdown_list = create_dropdown_user_list(recordid, teacheruserid);
    ui_userids.appendChild(ui_manager_dropdown_list);

    return ui_userids;
}

function create_dropdown_user_list(recordid, teacheruserid)
{
    var ui_manager_dropdown_list = document.createElement("div");
    ui_manager_dropdown_list.setAttribute("class","btn-group");

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "任課老師";
    ui_manager_dropdown_list.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_ul.setAttribute("id","dropdown_userid");
    ui_manager_dropdown_list.appendChild(div_ul);

    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];

        var defaultchecked = (teacheruserid == user.userid);

        div_ul.appendChild(create_dropdown_user_item(recordid, user.name, user.userid, defaultchecked));
    }

    return ui_manager_dropdown_list;
}

function create_dropdown_user_item(recordid, name, userid, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", userid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_user_click(\""+recordid+"\","+userid+");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_"+recordid+"_"+userid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_user_click(\""+recordid+"\","+userid+");return true;");
    item_checkbox.checked = defaultchecked;
    item_click.appendChild(item_checkbox);

    item_click.appendChild(document.createTextNode(" "+name));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_user_click = false;
function dropdown_checkbox_user_click(recordid, userid)
{
    glDropdown_checkbox_user_click = true;
}

function dropdown_user_click(recordid, userid)
{
    var ui_userids = document.getElementById('dropdown_teacheruserid_'+recordid);
    if ( ui_userids != undefined)
    {
        ui_userids.value = userid;
        // console.log("dropdown_user_click() ui_userids.value:"+ui_userids.value);
    }

    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];

        // unchecked all
        var ui_echeckbox = document.getElementById("dropdown_checkbox_"+recordid+"_"+user.userid);
        if ( ui_echeckbox != undefined && userid != user.userid)
        {
            ui_echeckbox.checked = false;
        }
    }

    if (glDropdown_checkbox_user_click == true)
    {
        glDropdown_checkbox_user_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_"+recordid+"_"+userid).checked == true)
        document.getElementById("dropdown_checkbox_"+recordid+"_"+userid).checked = false;
    else
        document.getElementById("dropdown_checkbox_"+recordid+"_"+userid).checked = true;

    glDropdown_checkbox_user_click = false;
}

function remove_record_file_click(recordid)
{
    var filesDiv = clearChildView("storage_file_"+recordid);

    var cur_curriculumrecord = get_CurriculumRecord(recordid);
    cur_curriculumrecord.files = '';

    var div_upload = create_upload_file_ui('files_'+recordid);
    filesDiv.appendChild(div_upload);

    var removebutton_ui = document.getElementById('remove_record_file_btn_' + recordid);
    if ( removebutton_ui != undefined )
    {
        removebutton_ui.setAttribute("style",'display:none;');
    }
}

function create_option_onoffAction(recordid, status)
{
    var onoffAction = document.createElement("select");
    onoffAction.setAttribute("id", "status_select_"+recordid);

    var optionOff = document.createElement("option");
    optionOff.text = '停止選課';
    optionOff.value = 0;
    if ( status == 0 )
        optionOff.setAttribute("selected", "selected");
    onoffAction.appendChild(optionOff);

    var optionOn = document.createElement("option");
    optionOn.text = '開放選課';
    optionOn.value = 1;
    if ( status == 1 )
        optionOn.setAttribute("selected", "selected");
    onoffAction.appendChild(optionOn);

    return onoffAction;
}

function open_apply_detail(recordid)
{
    var div_info_href_dl = document.getElementById(recordid+"_apply_detail_button");
    div_info_href_dl.setAttribute("title","關閉選課狀況");
    div_info_href_dl.setAttribute("onclick","close_apply_detail("+recordid+");");

    var div_info_href_dl_img = document.getElementById(recordid+"_apply_detail_icon");
    div_info_href_dl_img.setAttribute("class","fa fa-minus text-red");

    var row_extra = document.getElementById(recordid+"_TABLE_ROW_EXTRA");
    row_extra.setAttribute("bgcolor","#e0ebe7");

    // index
    row_extra.appendChild(document.createElement('td'));

    var rd_apply_table = document.createElement('td')
    rd_apply_table.setAttribute("colspan","10");
    row_extra.appendChild(rd_apply_table);

    var rd_empty = document.createElement('td')
    rd_empty.setAttribute("colspan","9");
    row_extra.appendChild(rd_empty);

    //
    // user apply table
    //
    var table_apply = document.createElement('table')
    table_apply.setAttribute("class","table table-bordered");
    table_apply.setAttribute("style","background-color:rgb(216, 230, 215);");
    rd_apply_table.appendChild(table_apply);

    var table_head_apply = document.createElement('thead')
    table_apply.appendChild(table_head_apply);

    var table_head_tr_apply = document.createElement('tr')
    table_head_tr_apply.setAttribute("style","background-color:rgb(196, 190, 15);");
    table_head_apply.appendChild(table_head_tr_apply);

    var table_head_th_apply_1 = document.createElement('th')
    table_head_th_apply_1.textContent = '#';
    table_head_tr_apply.appendChild(table_head_th_apply_1);

    var table_head_th_apply_2 = document.createElement('th')
    table_head_th_apply_2.textContent = '學員';
    table_head_tr_apply.appendChild(table_head_th_apply_2);

    // var table_head_th_apply_3 = document.createElement('th')
    // table_head_th_apply_3.textContent = '主集合';
    // table_head_tr_apply.appendChild(table_head_th_apply_3);

    // 簽核流程
    var table_head_th_apply_4 = document.createElement('th')
    table_head_th_apply_4.textContent = '簽核狀態';
    table_head_tr_apply.appendChild(table_head_th_apply_4);

    var table_head_th_apply_5 = document.createElement('th')
    table_head_th_apply_5.textContent = '操作';
    table_head_tr_apply.appendChild(table_head_th_apply_5);

    var table_body_apply = document.createElement('tbody')
    table_body_apply.setAttribute("id","applytable_"+recordid);
    table_apply.appendChild(table_body_apply);

    load_apply_user(recordid);
}

function close_apply_detail(recordid)
{
    var div_info_href_dl = document.getElementById(recordid+"_apply_detail_button");
    div_info_href_dl.setAttribute("title","顯示選課狀況");
    div_info_href_dl.setAttribute("onclick","open_apply_detail("+recordid+");");

    var div_info_href_dl_img = document.getElementById(recordid+"_apply_detail_icon");
    div_info_href_dl_img.setAttribute("class","fa fa-plus text-green");

    clearChildView(recordid+"_TABLE_ROW_EXTRA");
}

function load_apply_user(recordid)
{
    var table_body_apply = document.getElementById("applytable_" + recordid);
    if (table_body_apply == undefined)
    {
        return;
    }

    var targetURL = gl_target_server + "/php/curriculum.php";
    $.getJSON(targetURL, 'loadcurriculumrecordapply=1'+'&project_id='+project_id+'&recordid='+recordid, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            record.userid = parseInt(record.userid);
            record.curriculumrecordid = parseInt(record.curriculumrecordid);
            record.status = parseInt(record.status);
            gl_curriculumapply_List.push(record);

            var apply_ui = create_applyuser(itemindex, recordid, record);
            table_body_apply.appendChild(apply_ui);

        });
    })
    .success(function() {

        load_user_info();
        // load_usermaingroup();
    })
    .error(function() {

    })
    .complete(function() {
    });
}

function create_applyuser(itemindex, recordid, record)
{
    var cellIndex = 0;

    var row = document.createElement('tr');

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(itemindex+1));
    cellIndex++;

    // 名稱
    row.appendChild(document.createElement('td'));

    var userDiv = document.createElement('div');
    var user_span = document.createElement('span');
    user_span.setAttribute("class",'badge bg-green-active');
    user_span.setAttribute("id",'user_name_'+recordid+'_'+record.userid);
    user_span.textContent = getUserName(record.userid);
    userDiv.appendChild(user_span);
    row.cells[cellIndex].appendChild(userDiv);
    cellIndex++;

    // 主集合
    // row.appendChild(document.createElement('td'));
    // var maingroupDiv = document.createElement('div');
    // maingroupDiv.setAttribute("id","applymaingroup_"+recordid+"_"+record.userid);
    // maingroupDiv.textContent = '';
    // row.cells[cellIndex].appendChild(maingroupDiv);
    // cellIndex++;

    // 簽核流程
    row.appendChild(document.createElement('td'));
    var Spansignoffstatus = document.createElement("span");
    Spansignoffstatus.setAttribute("id","signoffstatus_"+recordid+"_"+record.userid);
    if (record.status == 0)
    {
        Spansignoffstatus.textContent = '簽核中';
        Spansignoffstatus.setAttribute("class","badge bg-blue");
    }
    else
    if (record.status == 1)
    {
        Spansignoffstatus.textContent = '已簽核';
        Spansignoffstatus.setAttribute("class","badge bg-green");
    }
    else
    if (record.status == 99)
    {
        Spansignoffstatus.textContent = '未核准';
        Spansignoffstatus.setAttribute("class","badge bg-red");
    }
    row.cells[cellIndex].appendChild(Spansignoffstatus);
    cellIndex++;

    // 操作
    row.appendChild(document.createElement('td'));

    var div_Signofflist = document.createElement("div");
    div_Signofflist.setAttribute("class","btn-group");

    var div_button = document.createElement("button");
    div_button.setAttribute("id","dropdown_Signoff_type_"+recordid+"_"+record.userid);
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

    div_ul.appendChild(create_dropdown_Signoff_item(record.userid, recordid, 0, "取消"));
    div_ul.appendChild(create_dropdown_Signoff_item(record.userid, recordid, 1, "核准"));
    div_ul.appendChild(create_dropdown_Signoff_item(record.userid, recordid, 99, "不核准"));

    row.cells[cellIndex].appendChild(div_Signofflist);
    cellIndex++;

    return row;
}

function create_dropdown_Signoff_item(userid, recordid, index, showname)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", index);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_Signoff_click("+userid+","+recordid+","+index+",\""+ showname +"\");return true;");
    item_click.appendChild(document.createTextNode(showname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_Signoff_click(userid, recordid, index, showname)
{
    var div_button = document.getElementById("dropdown_Signoff_type_"+recordid+"_"+userid);
    div_button.setAttribute("data-value", index);

    var Spansignoffstatus = document.getElementById("signoffstatus_"+recordid+"_"+userid);

    if ( index == 0 )
    {
        div_button.textContent = "簽核項目";
        Spansignoffstatus.textContent = '簽核中';
        Spansignoffstatus.setAttribute("class","badge bg-blue");

        update_Signoff_state(userid, recordid, 0);
    }
    else
    if ( index == 1 )
    {
        div_button.textContent = showname;
        Spansignoffstatus.textContent = '已簽核';
        Spansignoffstatus.setAttribute("class","badge bg-green");

        update_Signoff_state(userid, recordid, 1);
    }
    else
    if ( index == 99 )
    {
        div_button.textContent = showname;
        Spansignoffstatus.textContent = '未核准';
        Spansignoffstatus.setAttribute("class","badge bg-red");

        update_Signoff_state(userid, recordid, 99);
    }

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);
}

function update_Signoff_state(userid, recordid, value)
{
    var curriculumrecord = get_CurriculumRecord(recordid);
    var curriculum = get_curriculum(curriculumrecord.curriculumid);
    var curriculumTypeName = getCurriculumTypeName(curriculum.typeid);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_apply_status'] = 1;
    jsondata['userid'] = userid;
    jsondata['recordid'] = recordid;
    jsondata['status'] = value;
    jsondata['number'] = curriculum.number;
    jsondata['curriculumname'] = curriculumrecord.name;
    jsondata['curriculumtypename'] = curriculumTypeName;

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
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_curriculumrecord(recordid)
{
    if (confirm('Are you sure you want to delete this Curriculum Record ?'))
    {
        // delete it!
        delete_curriculumrecord(recordid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_curriculumrecord(recordid)
{
    var curriculumrecord = get_CurriculumRecord(recordid);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_curriculumrecord'] = 1;
    jsondata['id'] = recordid;
    jsondata['curriculumid'] = curriculumrecord.curriculumid;

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

function load_user_info()
{
    var queryuserList = [];

    var bFound = check_userinfo_existed(parseInt(gl_userid));
    if ( bFound == false)
    {
        queryuserList.push(gl_userid);
    }
    for(var index=0;index<gl_curriculum_List.length;index++)
    {
        var curriculum = gl_curriculum_List[index];

        //
        // check exist
        //
        var bFound = check_userinfo_existed(curriculum.userid);
        if ( bFound == false)
        {
            if (curriculum.userid > 0)
            {
                var bFound = false;
                for(var j=0;j<queryuserList.length;j++)
                {
                    if ( queryuserList[j] == curriculum.userid )
                    {
                        bFound = true;
                        break;
                    }
                }
                if (!bFound)
                {
                    queryuserList.push(curriculum.userid);
                }
            }
        }
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

// function load_usermaingroup()
// {
//     var queryMaingroupList = [];
//
//     for(var index=0;index<gl_curriculum_List.length;index++)
//     {
//         var curriculum = gl_curriculum_List[index];
//
//         var maingroupList = curriculum.maingroups.split(",");
//
//         for (var i = 0; i < maingroupList.length; ++i)
//         {
//             var maingroup = maingroupList[i];
//
//             var bFound = false;
//             for(var j=0;j<queryMaingroupList.length;j++)
//             {
//                 if ( queryMaingroupList[j] == maingroup )
//                 {
//                     bFound = true;
//                     break;
//                 }
//             }
//             if (!bFound)
//             {
//                 queryMaingroupList.push(maingroup);
//             }
//         }
//     }
//
//     // for (var i = 0; i < gl_user_info.length; ++i)
//     // {
//     //     var user = gl_user_info[i];
//     //
//     //     var maingroupList = user.maingroups.split(",");
//     //
//     //     for (var j = 0; j < maingroupList.length; ++j)
//     //     {
//     //         var maingroup = maingroupList[j];
//     //
//     //         var bFound = false;
//     //         for(var k=0;k<queryMaingroupList.length;k++)
//     //         {
//     //             if ( queryMaingroupList[k] == maingroup )
//     //             {
//     //                 bFound = true;
//     //                 break;
//     //             }
//     //         }
//     //         if (!bFound)
//     //         {
//     //             queryMaingroupList.push(maingroup);
//     //         }
//     //     }
//     // }
//
//     var querymaingroup = queryMaingroupList.toString();
//
//     gl_usermaingroups = [];
//
//     var targetURL = gl_target_server + "/php/vilsnodes.php";
//     var p = $.getJSON(targetURL, 'loadmaingroupname=1&project_id='+project_id+'&maingroupids='+querymaingroup, function(data)
//     {
//         $.each(data, function(index, usermaingroup)
//         {
//             usermaingroup.id = parseInt(usermaingroup.id);
//             gl_usermaingroups.push(usermaingroup);
//         });
//     })
//     .success(function() {
//         update_maingroup_name();
//     })
//     .error(function() {
//     })
//     .complete(function() {
//     });
// }

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

function update_user_name()
{
    for(var index=0;index<gl_curriculum_List.length;index++)
    {
        var curriculum = gl_curriculum_List[index];
        var user_span = document.getElementById("user_name_"+curriculum.id+'_'+curriculum.userid);
        if (user_span != undefined)
            user_span.textContent = getUserName(curriculum.userid);
    }

    for(var index=0;index<gl_curriculumrecord_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecord_List[index];
        var user_span = document.getElementById("user_name_"+curriculumrecord.id+'_'+curriculumrecord.userid);
        if (user_span != undefined)
            user_span.textContent = getUserName(curriculumrecord.userid);
        var user_span1 = document.getElementById("teacheruser_name_"+curriculumrecord.id+'_'+curriculumrecord.teacheruserid);
        if (user_span1 != undefined)
            user_span1.textContent = getUserName(curriculumrecord.teacheruserid);
    }
    for(var index=0;index<gl_curriculumapply_List.length;index++)
    {
        var curriculumapply = gl_curriculumapply_List[index];
        var user_span = document.getElementById("user_name_"+curriculumapply.curriculumrecordid+'_'+curriculumapply.userid);
        if (user_span != undefined)
            user_span.textContent = getUserName(curriculumapply.userid);
    }
}

// function update_maingroup_name()
// {
//     for(var index=0;index<gl_curriculumrecord_List.length;index++)
//     {
//         var curriculumrecord = gl_curriculumrecord_List[index];
//
//         var maingroupList = curriculumrecord.maingroups.split(",");
//
//         for (var i = 0; i < maingroupList.length; ++i)
//         {
//             var maingroup = maingroupList[i];
//
//             var maingroupInfo = getMainGroupInfo(maingroup);
//
//             var maingroup_span = document.getElementById("maingroup_name_"+curriculumrecord.id+'_'+maingroup);
//             if (maingroupInfo != undefined)
//             {
//                 var maingroup_div = document.getElementById("maingroup_"+curriculumrecord.id+'_'+maingroup);
//                 maingroup_div.setAttribute("style","display:block;");
//
//                 maingroup_span.textContent = maingroupInfo.name;
//             }
//         }
//     }
// }

function load_subgroup_info()
{
    var querysubgroup = '';
    var queryCount = 0;
    for(var index=0;index<gl_curriculum_List.length;index++)
    {
        var curriculum = gl_curriculum_List[index];

        var subgroup_List = curriculum.subgroups.split(",");
        for(var j=0;j<subgroup_List.length;j++)
        {
            var subgroupid = subgroup_List[j];

            //
            // check exist
            //
            var bFound = check_subgroups_existed(subgroupid);
            if ( bFound == false)
            {
                if ( queryCount > 0 )
                    querysubgroup += ',';
                querysubgroup += subgroupid;
                queryCount += 1;
            }
        }
    }

    if (querysubgroup.length == 0)
    {
        update_subgroups_name();

        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'querysubgroups=1&subgroupid='+querysubgroup+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, subgroup)
        {
            subgroup.userid = parseInt(subgroup.subgroupid);
            gl_subgroups_info.push(subgroup);
        });
    })
    .success(function() {
        update_subgroups_name();
    })
    .error(function() {
    })
    .complete(function() {
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

function create_new_curriculumrecord()
{
    create_empty_curriculumrecord_table_item(gl_curriculumrecord_List.length + 1);

    return false;
}

function create_empty_curriculumrecord_table_item(ui_index)
{
    var record_table = document.getElementById('CURRICULUM_RECORD_TABLE_BODY');
    var curriculumrecord_new = document.getElementById("curriculumrecord_new");
    if (curriculumrecord_new != undefined)
    {
        // already existed!
        return;
    }

    gl_NewCurriculumRecord.id = 88888;
    gl_NewCurriculumRecord.weekday = [];
    gl_NewCurriculumRecord.files = '';

    // console.log("gl_NewApply.deadline:"+gl_NewApply.deadline);

    var row = record_table.insertRow(0);
    row.setAttribute("id","curriculumrecord_new");
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(ui_index));
    cellIndex++;

    // 發起人
    row.appendChild(document.createElement('td'));
    var userDiv = document.createElement('div');
    //userDiv.appendChild(create_user_label(gl_NewCurriculumRecord.id, gl_userid));
    row.cells[cellIndex].appendChild(userDiv);
    cellIndex++;

    // 編輯人
    row.appendChild(document.createElement('td'));
    var userDiv = document.createElement('div');
    userDiv.appendChild(create_user_label(gl_NewCurriculumRecord.id, gl_userid));
    row.cells[cellIndex].appendChild(userDiv);
    cellIndex++;

    // 課程 選擇
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.appendChild(create_curriculum_select(gl_NewCurriculumRecord.id, 0));
    row.cells[cellIndex].appendChild(descriptionDiv);
    cellIndex++;

    // 課程名稱
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text('curriculumname_'+gl_NewCurriculumRecord.id, ""));
    cellIndex++;

    // 開放對象
    // row.appendChild(document.createElement('td'));
    // var maingroupDiv = document.createElement('div');
    // maingroupDiv.setAttribute("id","maingroup_"+gl_NewCurriculumRecord.id);
    // row.cells[cellIndex].appendChild(maingroupDiv);
    // var cur_curriculum = get_curriculum(0);
    // var div_maingrouplist = create_edit_maingrouplist_ui(gl_NewCurriculumRecord.id, cur_curriculum.maingroups, "");
    // maingroupDiv.appendChild(div_maingrouplist);
    // cellIndex++;

    // 開放選修日期起、至
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_date_picker('開始:', 'opendate_'+gl_NewCurriculumRecord.id, '', true));
    row.cells[cellIndex].appendChild(create_date_picker('結束:', 'closedate_'+gl_NewCurriculumRecord.id, '', true));
    cellIndex++;

    // 課程日期起、至
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_date_picker('開始:', 'startdate_'+gl_NewCurriculumRecord.id, '', true));
    row.cells[cellIndex].appendChild(create_date_picker('結束:', 'enddate_'+gl_NewCurriculumRecord.id, '', true));
    cellIndex++;

    // 課程時間
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_time_select('開始:', 'starttime_'+gl_NewCurriculumRecord.id, '09:00 AM', true));
    row.cells[cellIndex].appendChild(create_time_select('結束:', 'endtime_'+gl_NewCurriculumRecord.id, '09:30 AM', true));
    cellIndex++;

    // 課程週間日
    row.appendChild(document.createElement('td'));
    var td_date = create_weekday_select("weekday_"+gl_NewCurriculumRecord.id, gl_NewCurriculumRecord.id, '', true);
    row.cells[cellIndex].appendChild(td_date);
    cellIndex++;

    // 課程簽到時間
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_time_select('開始:', 'signinstarttime_'+gl_NewCurriculumRecord.id, '09:00 AM', true));
    row.cells[cellIndex].appendChild(create_time_select('結束:', 'signinendtime_'+gl_NewCurriculumRecord.id, '09:30 AM', true));
    cellIndex++;

    // 總時數(小時)
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text('totalhours_'+gl_NewCurriculumRecord.id, ""));
    cellIndex++;

    // 地點
    row.appendChild(document.createElement('td'));
    var inputText = create_input_text('location_'+gl_NewCurriculumRecord.id, "")
    row.cells[cellIndex].appendChild(inputText);
    var locationSelectDiv = document.createElement('div');
    locationSelectDiv.appendChild(create_location_select(gl_NewCurriculumRecord.id, ""));
    row.cells[cellIndex].appendChild(locationSelectDiv);
    cellIndex++;

    // 內容
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text('description_'+gl_NewCurriculumRecord.id, ""));
    cellIndex++;

    // 任課老師
    row.appendChild(document.createElement('td'));
    var teacherDiv = document.createElement('div');
    teacherDiv.setAttribute("id",'dropdown_teacheruserid_'+gl_NewCurriculumRecord.id);
    var ui_manager_dropdown_list = create_dropdown_user_list(gl_NewCurriculumRecord.id, "");
    teacherDiv.appendChild(ui_manager_dropdown_list);
    row.cells[cellIndex].appendChild(teacherDiv);
    cellIndex++;

    // 任課老師簡介
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text('teacher_'+gl_NewCurriculumRecord.id, ""));
    cellIndex++;

    // 預定人數
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text('maxnumber_'+gl_NewCurriculumRecord.id, ""));
    cellIndex++;

    // 自動核準
    row.appendChild(document.createElement('td'));
    var input_item = document.createElement('input');
    input_item.setAttribute("id","autoapproval_"+gl_NewCurriculumRecord.id);
    input_item.setAttribute("type","checkbox");
    input_item.setAttribute("class","flat-red");
    input_item.setAttribute("checked",true);
    row.cells[cellIndex].appendChild(input_item);
    row.cells[cellIndex].appendChild(document.createTextNode("自動核準"));
    cellIndex++;

    // 費用
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text('cost_'+gl_NewCurriculumRecord.id, ""));
    cellIndex++;

    // 備註
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text('note_'+gl_NewCurriculumRecord.id, ""));
    cellIndex++;

    // 簡章下載
    row.appendChild(document.createElement('td'));
    var div_upload = create_upload_file_ui('files_'+gl_NewCurriculumRecord.id);
    row.cells[cellIndex].appendChild(div_upload);
    cellIndex++;

    // 狀態
    row.appendChild(document.createElement('td'));
    var onoffAction = create_option_onoffAction(gl_NewCurriculumRecord.id, 0);
    row.cells[cellIndex].appendChild(onoffAction);
    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("id","save_new_curriculum_record_button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_curriculum_record("+gl_NewCurriculumRecord.id+");");
    Button_save.textContent = "新增課程";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;

    $('.timepicker').timepicker({
      showInputs: false,
      minuteStep: 5,
      showMeridian: true
    })

    $('.datepicker').datepicker(
    {
        language: 'zh-TW',
        weekStart: 1
    });
}

function create_curriculum_select(recordid, default_id)
{
    var selectList = document.createElement("select");
    selectList.setAttribute("id", "curriculum_select_"+recordid);

    for(var index=0;index<gl_curriculum_List.length;index++)
    {
        var curriculum = gl_curriculum_List[index];

        if (default_id == 0 && curriculum.recordid == 0)
        {
            // new curriculumrecord
            var curriculumTypeName = getCurriculumTypeName(curriculum.typeid);

            var option = document.createElement("option");
            option.text = curriculumTypeName + ' ['+ curriculum.number+']';
            option.value = curriculum.id;
            if ( default_id == curriculum.id )
                option.setAttribute("selected", "selected");

            selectList.appendChild(option);
        }
        else
        if (default_id == curriculum.id)
        {
            // only one select
            var curriculumTypeName = getCurriculumTypeName(curriculum.typeid);
            var option = document.createElement("option");
            option.text = curriculumTypeName + ' ['+ curriculum.number+']';
            option.value = curriculum.id;
            option.setAttribute("selected", "selected");

            selectList.appendChild(option);
        }
    }

    // selectList.onchange = function(e)
    // {
    //     var maingroupDiv = clearChildView("maingroup_"+recordid);
    //     var cur_curriculum = get_curriculum(this.value);
    //     var div_maingrouplist = create_edit_maingrouplist_ui(recordid, cur_curriculum.maingroups, "");
    //     maingroupDiv.appendChild(div_maingrouplist);
    // };

    return selectList;
}

// function create_edit_maingrouplist_ui(fileid, valididlist, idlist)
// {
//     var div_maingrouplist = document.createElement("div");
//     div_maingrouplist.setAttribute("class","btn-group");
//
//     var div_button = document.createElement("button");
//     div_button.setAttribute("type","button");
//     div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
//     div_button.setAttribute("data-toggle","dropdown");
//     div_button.textContent = "主集合";
//     div_maingrouplist.appendChild(div_button);
//
//     var div_span = document.createElement("span");
//     div_span.setAttribute("class","caret");
//     div_button.appendChild(div_span);
//
//     var div_ul = document.createElement("ul");
//     div_ul.setAttribute("class","dropdown-menu");
//     div_maingrouplist.appendChild(div_ul);
//
// // console.log("create_edit_maingrouplist_ui() fileid:"+fileid+" valididlist:"+valididlist+" idlist:"+idlist);
//
//     // select all button
//     if ( gl_groupid == 1 || gl_groupid == 2 || gl_groupid == 3 )
//     {
//         var item_li = document.createElement("li");
//         var item_click = document.createElement("a");
//         item_click.setAttribute("class","small");
//         item_click.setAttribute("href","javascript:;");
//         item_click.setAttribute("onclick","dropdown_selectall(\"maingroup\",\""+fileid+"\");return true;");
//         item_li.appendChild(item_click);
//         item_click.appendChild(document.createTextNode("全選"));
//         div_ul.appendChild(item_li);
//     }
//
//     var idList = idlist.split(",");
//     var valididlist = valididlist.split(",");
//
//     for (var i = 0; i < gl_usermaingroups.length; ++i)
//     {
//         var usermaingroup = gl_usermaingroups[i];
//         var valid = check_numer_in_list(usermaingroup.id, valididlist);
//
//         // console.log("create_edit_maingrouplist_ui() usermaingroup.id:"+usermaingroup.id+" valid:"+valid);
//
//         if (valid)
//         {
//             var existed = check_numer_in_list(usermaingroup.id, idList);
//             div_ul.appendChild(create_dropdown_maingroup_item(fileid, i, usermaingroup.id, usermaingroup.name, existed));
//         }
//     }
//
//     return div_maingrouplist;
// }

// function create_dropdown_maingroup_item(fileid, index, datavalue, dataname, defaultchecked)
// {
//     var item_li = document.createElement("li");
//     var item_click = document.createElement("a");
//     item_click.setAttribute("class","small");
//     item_click.setAttribute("tabIndex","-1");
//     item_click.setAttribute("data-value", datavalue);
//     item_click.setAttribute("href","javascript:;");
//     item_click.setAttribute("onclick","dropdown_maingroup_click(\""+fileid+"\","+ index +");return true;");
//
//     var item_checkbox = document.createElement("input");
//     item_checkbox.setAttribute("type","checkbox");
//     item_checkbox.setAttribute("id","dropdown_checkbox_maingroup_"+fileid+"_"+index);
//     item_checkbox.setAttribute("onclick","dropdown_checkbox_maingroup_click(\""+fileid+"\","+ index +");return true;");
//     item_checkbox.checked = defaultchecked;
//
//     item_click.appendChild(item_checkbox);
//     item_click.appendChild(document.createTextNode(" "+dataname));
//     item_li.appendChild(item_click);
//
//     return item_li;
// }
//
// function dropdown_checkbox_maingroup_click(fileid, index)
// {
//     glDropdown_checkbox_maingroup_click = true;
// }
//
// var glDropdown_checkbox_maingroup_click = false;
// function dropdown_maingroup_click(fileid, index)
// {
//     if (glDropdown_checkbox_maingroup_click == true)
//     {
//         glDropdown_checkbox_maingroup_click = false;
//         return;
//     }
//
//     if (document.getElementById("dropdown_checkbox_maingroup_"+fileid+"_"+index).checked == true)
//         document.getElementById("dropdown_checkbox_maingroup_"+fileid+"_"+index).checked = false;
//     else
//         document.getElementById("dropdown_checkbox_maingroup_"+fileid+"_"+index).checked = true;
//
//     glDropdown_checkbox_maingroup_click = false;
// }

function dropdown_selectall(prefix, fileid)
{
    // console.log("dropdown_selectall() prefix:"+prefix+" fileid:"+fileid);

    for(var i=0;i<100;i++)
    {
        var ele = document.getElementById("dropdown_checkbox_"+prefix+"_"+fileid+"_"+i);
        if ( ele == undefined )
        {
            console.log("dropdown_selectall() fileid:"+fileid+" i:"+i+" ele == undefined ");
            break;
        }
        ele.checked = true;
    }
}

// function getSelectedMaingroup(recordid)
// {
//     var maingroups = [];
//     var count = 0;
//     for (var index = 0; index < gl_usermaingroups.length; ++index)
//     {
//         var usermaingroup = gl_usermaingroups[index];
//
//         var dropdown_checkbox_maingroup = document.getElementById("dropdown_checkbox_maingroup_"+recordid+"_"+index);
//         if (dropdown_checkbox_maingroup != undefined)
//         {
//             if (dropdown_checkbox_maingroup.checked)
//             {
//                 var maingroupid = parseInt(usermaingroup.id);
//                 maingroups.push(maingroupid);
//                 count++;
//             }
//         }
//     }
//
//     // if ( gl_groupid == 1 || gl_groupid == 2 )
//     // {
//     //     if ( count == gl_usermaingroups.length )
//     //     {
//     //         maingroups = [];
//     //         maingroups.push(99999);
//     //     }
//     // }
//
//     return maingroups.toString();
// }

function create_location_select(recordid, default_name)
{
    var selectList = document.createElement("select");
    selectList.setAttribute("id", "location_select_"+recordid);

    var bFoundName = false;
    var firstName = "";
    for(var index=0;index<gl_classAreas.length;index++)
    {
        var classArea = gl_classAreas[index];

        var option = document.createElement("option");
        option.text = classArea.itemname;
        option.value = classArea.itemname;

        if (firstName.length == 0)
        {
            firstName = classArea.itemname;
        }

        if ( default_name === classArea.itemname )
        {
            bFoundName = true;
            option.setAttribute("selected", "selected");
        }

        selectList.appendChild(option);
    }
    var inputText = document.getElementById('location_'+recordid);
    if (bFoundName || default_name.length == 0)
    {
        inputText.setAttribute("style", "display:none;");
    }
    else
    {
        inputText.setAttribute("style", "display:block;");
    }

    if (default_name.length == 0)
    {
        default_name = firstName;
    }

    inputText.value = default_name;

    selectList.onchange = function(e)
    {
        var inputText = document.getElementById('location_'+recordid);
        if (this.value === '其他')
        {
            inputText.setAttribute("style", "display:block;");
        }
        else
        {
            inputText.setAttribute("style", "display:none;");
            inputText.value = this.value;
        }

        // console.log(this.value);
    };

    return selectList;
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

function getCurriculumType(typeid)
{
    var curriculumtypeRet = undefined;
    for(var index=0;index<gl_curriculumtype_List.length;index++)
    {
        var curriculumtype = gl_curriculumtype_List[index];

        // console.log("getCurriculumTypeName() typeid:"+typeid+" curriculumtype.typeid:"+curriculumtype.id);
        if (typeid == 0)
        {
            curriculumtypeRet = curriculumtype;
            break;
        }
        else
        if( typeid == curriculumtype.id )
        {
            curriculumtypeRet = curriculumtype;
            break;
        }
    }
    return curriculumtypeRet;
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

    // for(var index=0;index<gl_shared_curriculum_List.length;index++)
    // {
    //     var curriculum = gl_shared_curriculum_List[index];
    //
    //     if (curriculumid == 0)
    //     {
    //         return curriculum;
    //     }
    //     else
    //     if ( curriculum.id == curriculumid )
    //     {
    //         return curriculum;
    //     }
    // }

    return curriculum;
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
    input_Time.setAttribute("class","form-control timepicker");
    if (isEditing)
    {
        // input_Time.setAttribute("disabled","");
    }
    else
    {
        input_Time.setAttribute("disabled","true");
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
    inputui.setAttribute("onclick","weekday_checkbox_click(\""+recordid+"\",\""+day+"\")");
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

function weekday_checkbox_click(recordid, day)
{
    var checkd = document.getElementById("uiid_checkbox_"+day+"_"+recordid).checked;

    console.log("weekday_checkbox_click() day:"+day +" recordid:"+recordid +" checkd:"+checkd);

    var curriculumRecord = get_CurriculumRecord(recordid);

    if (curriculumRecord == undefined)
        return;

    var existed_index = curriculumRecord.weekday.indexOf(day);
    if (checkd)
    {
        if (existed_index < 0)
        {
            curriculumRecord.weekday.push(day);
        }
    }
    else
    {
        if (existed_index >= 0)
        {
            curriculumRecord.weekday.splice(existed_index, 1);
        }
    }
}

function get_CurriculumRecord(recordid)
{
    for(var index=0;index<gl_curriculumrecord_List.length;index++)
    {
        var curriculumrecord = gl_curriculumrecord_List[index];
        if ( recordid == curriculumrecord.id )
        {
            return curriculumrecord;
        }
    }

    if ( recordid == 88888 )
    {
        return gl_NewCurriculumRecord;
    }

    return undefined;
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

function save_curriculum_record(recordid)
{

    var curriculumname_ui_value = document.getElementById('curriculumname_'+recordid).value;
    var opendate_ui_value = document.getElementById('opendate_'+recordid).value;
    var closedate_ui_value = document.getElementById('closedate_'+recordid).value;
    var startdate_ui_value = document.getElementById('startdate_'+recordid).value;
    var enddate_ui_value = document.getElementById('enddate_'+recordid).value;
    var starttime_ui_value = document.getElementById('starttime_'+recordid).value;
    var endtime_ui_value = document.getElementById('endtime_'+recordid).value;
    // var weekday_ui = document.getElementById('weekday_'+recordid);
    var signinstarttime_ui_value = document.getElementById('signinstarttime_'+recordid).value;
    var signinendtime_ui_value = document.getElementById('signinendtime_'+recordid).value;
    var totalhours_ui_value = document.getElementById('totalhours_'+recordid).value;
    var location_ui_value = document.getElementById('location_'+recordid).value;
    var description_ui_value = document.getElementById('description_'+recordid).value;
    var teacher_ui_value = document.getElementById('teacher_'+recordid).value;
    var maxnumber_ui_value = document.getElementById('maxnumber_'+recordid).value;
    // var autoapproval_ui = document.getElementById('autoapproval_'+recordid);
    var cost_ui_value = document.getElementById('cost_'+recordid).value;
    var note_ui_value = document.getElementById('note_'+recordid).value;
    // var filesl_ui = document.getElementById('files_'+recordid);

    var teacheruserid_ui_value = '';
    var ui_userids = document.getElementById('dropdown_teacheruserid_'+recordid);
    if ( ui_userids != undefined && ui_userids.value != undefined)
    {
        teacheruserid_ui_value = ui_userids.value;
    }

    console.log("save_curriculum_record() teacheruserid_ui_value:"+teacheruserid_ui_value);

    // var maingroups = getSelectedMaingroup(recordid);
    // if ( maingroups.length == 0 )
    // {
    //     alert("請選擇主集合");
    //     return;
    // }

    if( curriculumname_ui_value.length == 0)
    {
        alert("請填入 課程名稱");
        return;
    }
    if( opendate_ui_value.length == 0 || closedate_ui_value.length == 0)
    {
        alert("請選擇 開放選修日期");
        return;
    }
    if( startdate_ui_value.length == 0 || enddate_ui_value.length == 0)
    {
        alert("請選擇 課程日期");
        return;
    }
    if( starttime_ui_value.length == 0 || endtime_ui_value.length == 0)
    {
        alert("請選擇 課程時間");
        return;
    }
    if( signinstarttime_ui_value.length == 0 || signinendtime_ui_value.length == 0)
    {
        alert("請選擇 課程簽到時間");
        return;
    }

    var curriculumRecord = get_CurriculumRecord(recordid);
    if ( curriculumRecord.weekday.length == 0)
    {
        alert("請設定 課程週間日");
        return;
    }

    if( totalhours_ui_value.length == 0)
    {
        alert("請設定 總時數(小時)");
        return;
    }

    if( isNumeric(totalhours_ui_value) == false)
    {
        alert("總時數(小時) 請正確填入數字");
        return;
    }

    if( location_ui_value.length == 0)
    {
        alert("請設定 地點");
        return;
    }
    if( description_ui_value.length == 0)
    {
        alert("請設定 內容");
        return;
    }

    if( teacheruserid_ui_value.length == 0)
    {
        alert("請設定 任課老師");
        return;
    }

    if( maxnumber_ui_value.length == 0)
    {
        alert("請設定 預定人數");
        return;
    }
    if( isNumeric(maxnumber_ui_value) == false)
    {
        alert("預定人數 請正確填入數字");
        return;
    }

    if( cost_ui_value.length == 0)
    {
        alert("請設定 費用");
        return;
    }
    if( isNumeric(cost_ui_value) == false)
    {
        alert("費用 請正確填入數字");
        return;
    }

    // if( note_ui_value.length == 0)
    // {
    //     alert("請設定 備註");
    //     return;
    // }

    var save_new_curriculum_record_button = document.getElementById('save_new_curriculum_record_button');
    if (save_new_curriculum_record_button != undefined)
        save_new_curriculum_record_button.disabled = true;

    var editBtn = document.getElementById("edit_curriculumrecord_btn_"+recordid);
    if (editBtn != undefined)
        editBtn.disabled = true;

    var deleteBtn = document.getElementById("delete_curriculumrecord_btn_"+recordid);
    if (deleteBtn != undefined)
        deleteBtn.disabled = true;

    gl_current_record_id = recordid;

    check_upload_file();
}

function check_upload_file()
{
    gl_uploadfile_uid = '';

    var uploadfile_ui = document.getElementById('fileUpload');

    if ( uploadfile_ui != undefined )
    {
        var uploadfile = uploadfile_ui.files[0];

        if ( uploadfile != undefined )
        {
            gl_uploadfile_uid = sendUploadFileRequest(uploadFileFinihed);
            console.log("check_upload_file() gl_uploadfile_uid:"+gl_uploadfile_uid);
        }
        else
        {
            uploadRecord();
        }
    }
    else
    {
        uploadRecord();
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
    var recordid = gl_current_record_id;

    var curriculumname = document.getElementById('curriculumname_'+recordid).value;
    var curriculumid = document.getElementById("curriculum_select_"+recordid).value;
    var opendate_ui_value = document.getElementById('opendate_'+recordid).value;
    var closedate_ui_value = document.getElementById('closedate_'+recordid).value;
    var startdate_ui_value = document.getElementById('startdate_'+recordid).value;
    var enddate_ui_value = document.getElementById('enddate_'+recordid).value;
    var starttime_ui_value = document.getElementById('starttime_'+recordid).value;
    var endtime_ui_value = document.getElementById('endtime_'+recordid).value;
    var signinstarttime_ui_value = document.getElementById('signinstarttime_'+recordid).value;
    var signinendtime_ui_value = document.getElementById('signinendtime_'+recordid).value;
    var totalhours_ui_value = document.getElementById('totalhours_'+recordid).value;
    var location_ui_value = document.getElementById('location_'+recordid).value;
    var description_ui_value = document.getElementById('description_'+recordid).value;
    var teacher_ui_value = document.getElementById('teacher_'+recordid).value;
    var maxnumber_ui_value = document.getElementById('maxnumber_'+recordid).value;
    var autoapproval_ui = document.getElementById('autoapproval_'+recordid);
    var cost_ui_value = document.getElementById('cost_'+recordid).value;
    var note_ui_value = document.getElementById('note_'+recordid).value;

    var status_ui_value = 0;
    var ui_status = document.getElementById("status_select_"+recordid);
    if (ui_status != undefined)
    {
        status_ui_value = ui_status.value
    }

    var teacheruserid_ui_value = '';
    var ui_userids = document.getElementById('dropdown_teacheruserid_'+recordid);
    if ( ui_userids != undefined && ui_userids.value != undefined)
    {
        teacheruserid_ui_value = ui_userids.value;
    }

    var curriculum = get_curriculum(curriculumid);
    var curriculumrecord = get_CurriculumRecord(recordid);
    // var maingroups = getSelectedMaingroup(recordid);
    var subgroups = curriculum.subgroups;

    var uploadfile_uid = gl_uploadfile_uid + ''; // to string

    // console.log("uploadRecord() gl_current_record_id:"+gl_current_record_id +" curriculumrecord:"+curriculumrecord);
    // console.log("uploadRecord() uploadfile_uid:"+uploadfile_uid +' uploadfile_uid.length:'+uploadfile_uid.length);

    var weekday_value = curriculumrecord.weekday.toString();

    var autoapproval_value = 0;
    if ( autoapproval_ui.checked )
        autoapproval_value = 1;

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_curriculumrecord'] = 1;
    jsondata['id'] = recordid;
    jsondata['curriculumid'] = curriculumid;
    jsondata['shared'] = curriculum.shared;
    jsondata['name'] = curriculumname;
    // jsondata['maingroups'] = maingroups;
    jsondata['subgroups'] = subgroups;
    jsondata['number'] = curriculum.number;
    jsondata['curriculumtypename'] = getCurriculumTypeName(curriculum.typeid);
    jsondata['opendate'] = opendate_ui_value;
    jsondata['closedate'] = closedate_ui_value;
    jsondata['startdate'] = startdate_ui_value;
    jsondata['enddate'] = enddate_ui_value;
    jsondata['starttime'] = starttime_ui_value;
    jsondata['endtime'] = endtime_ui_value;
    jsondata['signinstarttime'] = signinstarttime_ui_value;
    jsondata['signinendtime'] = signinendtime_ui_value;
    jsondata['weekday'] = weekday_value;
    jsondata['totalhours'] = totalhours_ui_value;
    jsondata['location'] = location_ui_value;
    jsondata['description'] = description_ui_value;
    jsondata['teacher'] = teacher_ui_value;
    jsondata['teacheruserid'] = teacheruserid_ui_value;
    jsondata['maxnumber'] = maxnumber_ui_value;
    jsondata['autoapproval'] = autoapproval_value;
    jsondata['cost'] = cost_ui_value;
    jsondata['note'] = note_ui_value;
    jsondata['status'] = status_ui_value;
    if ( uploadfile_uid.length > 0 )
        jsondata['files'] = uploadfile_uid;
    else
        jsondata['files'] = curriculumrecord.files;

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

function getMainGroupInfo(maingroupid)
{
    var maingroupinfo = undefined;
    var int_maingroupid = parseInt(maingroupid);

    for (var i = 0; i < gl_usermaingroups.length; ++i)
    {
        var maingroup = gl_usermaingroups[i];

        if ( int_maingroupid == maingroup.id )
        {
            return maingroup;
        }
    }
    return maingroupinfo;
}

function create_user_label(recordid, userid)
{
    var ui_div = document.createElement("div");

    var user_div = document.createElement('div');
    user_div.setAttribute("id",'user_'+recordid+'_'+userid);
    ui_div.appendChild(user_div);

    var user_span = document.createElement('span');
    user_span.setAttribute("class",'badge bg-aqua-active');
    user_span.setAttribute("id",'user_name_'+recordid+'_'+userid);
    user_span.textContent = getUserName(userid);
    user_div.appendChild(user_span);

    return ui_div;
}

// function create_maingroup_label(recordid, maingroups)
// {
//     var ui_div = document.createElement("div");
//
//     var maingroupList = maingroups.split(",");
//
//     // if (maingroups.indexOf('99999') >= 0)
//     // {
//     //     var maingroup_div = document.createElement("div");
//     //     maingroup_div.appendChild(create_maingroupname_label('全部'));
//     //     ui_div.appendChild(maingroup_div);
//     // }
//     // else
//     // {
//         for (var i = 0; i < maingroupList.length; ++i)
//         {
//             var maingroup = maingroupList[i];
//
//             var maingroupInfo = getMainGroupInfo(maingroup);
//
//             var maingroup_div = document.createElement('div');
//             maingroup_div.setAttribute("id",'maingroup_'+recordid+'_'+maingroup);
//             maingroup_div.setAttribute("style","display:none;");
//             ui_div.appendChild(maingroup_div);
//
//             // var maingroup_span = document.createElement('span');
//             // // maingroup_span.setAttribute("class",'badge bg-green-active');
//             // maingroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
//             // maingroup_span.setAttribute("id",'maingroup_name_'+recordid+'_'+maingroup);
//             // if ( maingroupInfo != undefined)
//             //     maingroup_span.textContent = maingroupInfo.name;
//             // else
//             //     maingroup_span.textContent = maingroup;
//
//             var groupName = maingroup;
//             if ( maingroupInfo != undefined)
//                 groupName = maingroupInfo.name;
//             var maingroup_span = create_maingroupname_label(groupName);
//             maingroup_span.setAttribute("id",'maingroup_name_'+recordid+'_'+maingroup);
//             maingroup_div.appendChild(maingroup_span);
//
//         }
//     // }
//
//     return ui_div;
// }

// function create_maingroupname_label(context)
// {
//     var maingroup_span = document.createElement("span");
//     maingroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
//     maingroup_span.textContent = context;
//     return maingroup_span;
// }

function check_numer_in_list(targetNumber, dataList)
{
    var existed = false;

    if (dataList.length == 0)
    {
        existed = true;
    }

    for (var i = 0; i < dataList.length; ++i)
    {
        var data = dataList[i];

// console.log("check_numer_in_list() targetNumber:"+targetNumber+" data:"+data);

        if ( parseInt(data) == targetNumber || parseInt(data) == 99999 )
        {
            existed = true;
            break;
        }
    }

    return existed;
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
