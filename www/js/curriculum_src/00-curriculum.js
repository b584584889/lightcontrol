/*
 * Author: ikki Chung
 * Date: 2020 11 18
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var LOAD_PAGE_NUM = 20;

var gl_user_info = [];
var gl_curriculum_List = [];
var gl_shared_curriculum_List = [];
var gl_shared_curriculumrecord_List = [];
var gl_curriculumtype_List = [];

var gl_usersubgroups = [];
// var gl_usermaingroups = [];
var gl_allmaingroups = [];
var gl_curriculumprefix = "";

var glCurrentPage = 1;
var glSharedCurrentPage = 1;

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

                load_usersubgroup();
                // load_usermaingroup();
                load_allmaingroup();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_CurriculumType();
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

                load_shared_Curriculums();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_shared_CurriculumRecords();
                // load_shared_myCurriculums();
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_CurriculumPrefix();
            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_Curriculums();
                show_shared_CurriculumRecords();
                show_CurriculumType();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 7:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_user_info();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 8:
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

function load_usersubgroup()
{
    gl_usersubgroups = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadusersubgroup=1&project_id='+project_id+'&type=0', function(data)
    {
        $.each(data, function(index, usersubgroup)
        {
            usersubgroup.id = parseInt(usersubgroup.id);
            gl_usersubgroups.push(usersubgroup);
        });
    })
    .success(function() {

    })
    .error(function() {
    })
    .complete(function() {
    });
}

// function load_usermaingroup()
// {
//     gl_usermaingroups = [];
//
//     var targetURL = gl_target_server + "/php/vilsnodes.php";
//     var p = $.getJSON(targetURL, 'loadmaingroup=1&project_id='+project_id, function(data)
//     {
//         $.each(data, function(index, usermaingroup)
//         {
//             usermaingroup.id = parseInt(usermaingroup.id);
//             gl_usermaingroups.push(usermaingroup);
//         });
//     })
//     .success(function() {
//         CurrentLoadIndex = CurrentLoadIndex + 1;
//     })
//     .error(function() {
//     })
//     .complete(function() {
//     });
// }

function load_allmaingroup()
{
    gl_allmaingroups = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadallmaingroup=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, allmaingroup)
        {
            allmaingroup.id = parseInt(allmaingroup.id);
            gl_allmaingroups.push(allmaingroup);
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

function load_Curriculums()
{
    var targetURL = gl_target_server + "/php/curriculum.php";
    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    gl_curriculum_List = [];

    $.getJSON(targetURL, 'loadcurriculums=1'+'&project_id='+project_id+"&offset="+offset+"&count="+count, function(data)
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

function load_shared_Curriculums()
{
    var targetURL = gl_target_server + "/php/curriculum.php";

    gl_shared_curriculum_List = [];

    $.getJSON(targetURL, 'loadsharedcurriculums=1'+'&project_id='+project_id, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            record.id = parseInt(record.id);
            record.userid = parseInt(record.userid);
            record.typeid = parseInt(record.typeid);
            // record.subgroups = '';
            // record.curriculumsharedid = '';
            gl_shared_curriculum_List.push(record);
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

function load_shared_CurriculumRecords()
{
    var targetURL = gl_target_server + "/php/curriculum.php";
    var offset = (glSharedCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    gl_shared_curriculumrecord_List = [];

    var recordids = [];
    for(var index=0;index<gl_shared_curriculum_List.length;index++)
    {
        var shared_curriculum = gl_shared_curriculum_List[index];
        recordids.push(shared_curriculum.recordid);
    }

    var query_recordids = recordids.toString();

    $.getJSON(targetURL, 'loadsharedcurriculumrecords=1'+'&project_id='+project_id+'&recordids='+query_recordids+'&offset='+offset+'&count='+count, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            record.id = parseInt(record.id);
            record.userid = parseInt(record.userid);
            record.curriculumid = parseInt(record.curriculumid);
            record.shared = parseInt(record.shared);
            gl_shared_curriculumrecord_List.push(record);

            // record.id = parseInt(record.id);
            // record.userid = parseInt(record.userid);
            // record.typeid = parseInt(record.typeid);
            // gl_shared_curriculum_List.push(record);
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
//
//             var bFound = false;
//             for(var index=0;index<gl_shared_curriculum_List.length;index++)
//             {
//                 var shared_curriculum = gl_shared_curriculum_List[index];
//                 if ( shared_curriculum.id == record.curriculumid )
//                 {
//                     shared_curriculum.subgroups = record.subgroups;
//                     shared_curriculum.curriculumsharedid = record.id;
//                     gl_shared_curriculum_List[index] = shared_curriculum;
//                     bFound = true;
//                     break;
//                 }
//             }
//             // if (bFound == false)
//             // {
//             //     record.curriculumsharedid = record.id;
//             //     gl_shared_curriculum_List.push(record);
//             // }
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

function load_CurriculumPrefix()
{
    var targetURL = gl_target_server + "/php/curriculum.php";

    $.getJSON(targetURL, 'loadcurriculumprefix=1'+'&project_id='+project_id, function(data)
    {
        gl_curriculumprefix = data.prefix;

        var curriculumprefix_label = document.getElementById("curriculumprefix_label");
        curriculumprefix_label.textContent = "課程代碼:" + gl_curriculumprefix;

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

function show_Curriculums()
{
    var table_body_ui = document.getElementById('CURRICULUM_TABLE_BODY');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    // console.log("show_assists() gl_curriculum_List.length:"+gl_curriculum_List.length);

    for(var index=0;index<gl_curriculum_List.length;index++)
    {
        show_Curriculum(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, gl_curriculum_List[index]);
    }
}

function show_Curriculum(table_body_ui, ui_index, record)
{
    var cellIndex = 0;

    var row = document.createElement('tr');
    row.setAttribute("id", record.id+"_TABLE_ROW");
    if ((ui_index % 2) == 0)
        row.setAttribute("bgcolor","#ebebe0");

    table_body_ui.appendChild(row);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // 發起人
    row.appendChild(document.createElement('td'));
    var userDiv = document.createElement('div');
    userDiv.appendChild(create_user_label(record.id, record.userid));
    row.cells[cellIndex].appendChild(userDiv);
    cellIndex++;

    // 發起單位
    var td_subgroups = create_maingroups(record.maingroups);
    td_subgroups.setAttribute("id","maingroups_"+record.id);
    row.appendChild(td_subgroups);
    cellIndex++;

    // 課程類別
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.setAttribute("id", "type_"+record.id);
    descriptionDiv.appendChild(create_type_label(record.typeid));
    row.cells[cellIndex].appendChild(descriptionDiv);
    cellIndex++;

    // 是否共享
    row.appendChild(document.createElement('td'));
    var selectsharedDiv = document.createElement('div');
    selectsharedDiv.setAttribute("id", "shared_"+record.id);
    if ( record.shared == 1 )
        selectsharedDiv.textContent = '是';
    else
        selectsharedDiv.textContent = '否';
    row.cells[cellIndex].appendChild(selectsharedDiv);
    cellIndex++;

    // 開放對象, 主集合權限
    var td_subgroups = create_maingroups(record.sharedmaingroups);
    td_subgroups.setAttribute("id","sharedmaingroups_"+record.id);
    row.appendChild(td_subgroups);
    cellIndex++;

    // 開放對象, 子集合權限
    var td_subgroups = create_subgroups(record.subgroups);
    td_subgroups.setAttribute("id","subgroups_"+record.id);
    row.appendChild(td_subgroups);
    cellIndex++;

    // 課程號碼
    row.appendChild(document.createElement('td'));
    var numberDiv = document.createElement('div');
    numberDiv.setAttribute("id","number_"+record.id);
    numberDiv.appendChild(document.createTextNode(record.number));
    row.cells[cellIndex].appendChild(numberDiv);
    cellIndex++;

    // 操作
    // Edit button
    row.appendChild(document.createElement('td'));
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id", "edit_curriculum_btn_"+record.id);
    editBtn.setAttribute("title","Edit this Curriculum");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_curriculum("+ record.id +");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id", "edit_curriculum_img_"+record.id);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    // Delete button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("id", "delete_curriculum_btn_"+record.id);
    deleteBtn.setAttribute("style","display:none");
    deleteBtn.setAttribute("title","Delete this Curriculum");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","confirm_delete_curriculum("+ record.id +");return false;");
    var imgdelete = document.createElement("i");
    imgdelete.setAttribute("class","fa fa-trash-o");
    deleteBtn.appendChild(imgdelete);
    row.cells[cellIndex].appendChild(deleteBtn);

    cellIndex++;
}

function show_shared_CurriculumRecords()
{
    var table_body_ui = document.getElementById('SHARED_CURRICULUM_TABLE_BODY');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    // console.log("show_assists() gl_curriculum_List.length:"+gl_curriculum_List.length);

    for(var index=0;index<gl_shared_curriculumrecord_List.length;index++)
    {
        show_shared_CurriculumRecord(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, gl_shared_curriculumrecord_List[index]);
    }
}

function show_shared_CurriculumRecord(table_body_ui, ui_index, record)
{
    var cellIndex = 0;

    var shared_curriculum = getSharedCurriculum(record.curriculumid);

    var row = document.createElement('tr');
    row.setAttribute("id", record.id+"_TABLE_ROW");
    if ((ui_index % 2) == 0)
        row.setAttribute("bgcolor","#ebebe0");

    table_body_ui.appendChild(row);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // 發起人
    row.appendChild(document.createElement('td'));
    var userDiv = document.createElement('div');
    userDiv.appendChild(create_user_label(record.id, record.userid));
    row.cells[cellIndex].appendChild(userDiv);
    cellIndex++;

    // 發起單位
    var td_subgroups = create_maingroups(record.maingroups);
    row.appendChild(td_subgroups);
    cellIndex++;

    // 課程類別
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.appendChild(create_type_label(shared_curriculum.typeid));
    row.cells[cellIndex].appendChild(descriptionDiv);
    cellIndex++;

    // 課程名稱
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.appendChild(document.createTextNode(record.name));
    row.cells[cellIndex].appendChild(descriptionDiv);
    cellIndex++;

    // 開放對象, 子集合權限
    var td_subgroups = create_subgroups(record.subgroups);
    td_subgroups.setAttribute("id","shared_subgroups_"+record.id);
    row.appendChild(td_subgroups);
    cellIndex++;

    // 課程號碼
    row.appendChild(document.createElement('td'));
    var numberDiv = document.createElement('div');
    numberDiv.appendChild(document.createTextNode(record.number));
    row.cells[cellIndex].appendChild(numberDiv);
    cellIndex++;

    // 操作
    // Edit button
    row.appendChild(document.createElement('td'));
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id", "edit_shared_curriculum_btn_"+record.id);
    editBtn.setAttribute("title","Edit this Curriculum");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_shared_curriculum("+ record.id +","+ record.curriculumid +");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id", "edit_shared_curriculum_img_"+record.id);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    cellIndex++;
}

// function show_shared_Curriculums()
// {
//     var table_body_ui = document.getElementById('SHARED_CURRICULUM_TABLE_BODY');
//     while (table_body_ui.firstChild)
//     {
//         table_body_ui.removeChild(table_body_ui.firstChild);
//     }
//
//     for(var index=0;index<gl_shared_curriculum_List.length;index++)
//     {
//         show_shared_Curriculum(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, gl_shared_curriculum_List[index]);
//     }
// }
//
// function show_shared_Curriculum(table_body_ui, ui_index, record)
// {
//     var cellIndex = 0;
//
//     var row = document.createElement('tr');
//     row.setAttribute("id", record.id+"_TABLE_ROW");
//     if ((ui_index % 2) == 0)
//         row.setAttribute("bgcolor","#ebebe0");
//
//     table_body_ui.appendChild(row);
//
//     // index
//     row.appendChild(document.createElement('td'));
//     row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
//     cellIndex++;
//
//     // 發起人
//     row.appendChild(document.createElement('td'));
//     var userDiv = document.createElement('div');
//     userDiv.appendChild(create_user_label(record.id, record.userid));
//     row.cells[cellIndex].appendChild(userDiv);
//     cellIndex++;
//
//     // 發起單位
//     var td_subgroups = create_maingroups(record.maingroups);
//     row.appendChild(td_subgroups);
//     cellIndex++;
//
//     // 課程類別
//     row.appendChild(document.createElement('td'));
//     var descriptionDiv = document.createElement('div');
//     descriptionDiv.appendChild(create_type_label(record.typeid));
//     row.cells[cellIndex].appendChild(descriptionDiv);
//     cellIndex++;
//
//     // 開放對象, 子集合權限
//     var td_subgroups = create_subgroups(record.subgroups);
//     td_subgroups.setAttribute("id","shared_subgroups_"+record.id);
//     row.appendChild(td_subgroups);
//     cellIndex++;
//
//     // 課程號碼
//     row.appendChild(document.createElement('td'));
//     var numberDiv = document.createElement('div');
//     numberDiv.appendChild(document.createTextNode(record.number));
//     row.cells[cellIndex].appendChild(numberDiv);
//     cellIndex++;
//
//     // 操作
//     // Edit button
//     row.appendChild(document.createElement('td'));
//     var editBtn = document.createElement("button");
//     editBtn.setAttribute("id", "edit_shared_curriculum_btn_"+record.id);
//     editBtn.setAttribute("title","Edit this Curriculum");
//     editBtn.setAttribute("href","javascript:;");
//     editBtn.setAttribute("onclick","edit_shared_curriculum("+ record.id +","+ record.curriculumsharedid +");return false;");
//     var imgEdit = document.createElement("i");
//     imgEdit.setAttribute("id", "edit_shared_curriculum_img_"+record.id);
//     imgEdit.setAttribute("class","fa fa-edit");
//     editBtn.appendChild(imgEdit);
//     row.cells[cellIndex].appendChild(editBtn);
//
//     cellIndex++;
// }

function create_select_shared_ui(recordid, shared)
{
    var div_select_reissue = document.createElement("div");
    div_select_reissue.setAttribute("class","form-group");
    // div_select_reissue.setAttribute("id", uiid);

    var select_shared_yes_ui = create_radio_ui(recordid, "select_shared_"+recordid, "select_shared_yes_"+recordid, "是", (shared == 1));
    var select_shared_no_ui = create_radio_ui(recordid, "select_shared_"+recordid, "select_shared_no_"+recordid, "否", (shared == 0));

    div_select_reissue.appendChild(select_shared_yes_ui);
    div_select_reissue.appendChild(select_shared_no_ui);
    //
    // select_shared_yes_ui.addEventListener('change', function() {
    //     div_maingrouplist = document.getElementById("maingrouplist_"+recordid);
    //     if (this.value)
    //     {
    //         div_maingrouplist.setAttribute("style","disaply:block;");
    //     }
    // });
    // select_shared_no_ui.addEventListener('change', function() {
    //     div_maingrouplist = document.getElementById("maingrouplist_"+recordid);
    //     if (this.value)
    //     {
    //         div_maingrouplist.setAttribute("style","disaply:none;");
    //     }
    // });

    return div_select_reissue;
}

function create_radio_ui(recordid, ui_name, ui_id, showtext, checked)
{
    var div_select_radio = document.createElement("div");
    div_select_radio.setAttribute("class","radio");

    var div_select_radio_label = document.createElement("label");
    div_select_radio.appendChild(div_select_radio_label);

    var div_select_radio_input = document.createElement("input");
    div_select_radio_input.setAttribute("type","radio");
    div_select_radio_input.setAttribute("name", ui_name);
    div_select_radio_input.setAttribute("id", ui_id);
    div_select_radio_input.setAttribute("onchange", "share_btn_click(\""+recordid+"\",\""+showtext+"\");");

    if (checked)
    {
        div_select_radio_input.checked = true;
    }

    div_select_radio_label.appendChild(div_select_radio_input);
    div_select_radio_label.appendChild(document.createTextNode(showtext));

    return div_select_radio;
}

function share_btn_click(recordid, showtext)
{
    // console.log("share_btn_click() recordid:"+recordid+" showtext:"+showtext);

    div_maingrouplist = document.getElementById("maingrouplist_"+recordid);
    if (showtext === "是")
    {
        div_maingrouplist.setAttribute("style","display:block;");
    }
    else
    {
        div_maingrouplist.setAttribute("style","display:none;");
    }
}

function edit_curriculum(recordid)
{
    var type_div = clearChildView("type_"+recordid);
    var shared_div = clearChildView("shared_"+recordid);
    var subgroups_td = clearChildView("subgroups_"+recordid);
    var sharedmaingroups_td = clearChildView("sharedmaingroups_"+recordid);
    var number_div = clearChildView("number_"+recordid);

    var editBtn = document.getElementById("edit_curriculum_btn_"+recordid);
    var editImg = document.getElementById("edit_curriculum_img_"+recordid);
    editBtn.setAttribute("title","Save this Curriculum");
    editBtn.setAttribute("onclick","save_curriculum("+ recordid +");return false;");
    editImg.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("delete_curriculum_btn_"+recordid);
    deleteBtn.setAttribute("style","display:block");

    for(var index=0;index<gl_curriculum_List.length;index++)
    {
        var curriculum = gl_curriculum_List[index];
        if( curriculum.id == recordid )
        {
            var div_typelist = create_edit_typelist_ui(recordid, curriculum.typeid);
            type_div.appendChild(div_typelist);

            var sharedUI = create_select_shared_ui(recordid, curriculum.shared);
            shared_div.appendChild(sharedUI);

            var div_subgrouplist = create_edit_subgrouplist_ui(recordid, curriculum.subgroups);
            subgroups_td.appendChild(div_subgrouplist);

            var div_maingrouplist = create_edit_maingrouplist_ui(recordid, curriculum.sharedmaingroups, curriculum.shared);
            sharedmaingroups_td.appendChild(div_maingrouplist);

            number_div.appendChild(create_input_text("input_number_"+recordid, curriculum.number));

            break;
        }
    }
}

function edit_shared_curriculum(recordid, curriculumid)
{
    var shared_subgroups_td = clearChildView("shared_subgroups_"+recordid);

    var editBtn = document.getElementById("edit_shared_curriculum_btn_"+recordid);
    var editImg = document.getElementById("edit_shared_curriculum_img_"+recordid);
    editBtn.setAttribute("title","Save this Curriculum");
    editBtn.setAttribute("onclick","save_shared_curriculum("+ recordid +","+curriculumid+");return false;");
    editImg.setAttribute("class","fa fa-save");

    for(var index=0;index<gl_shared_curriculumrecord_List.length;index++)
    {
        var record = gl_shared_curriculumrecord_List[index];
        if( record.curriculumid == curriculumid )
        {
            var div_subgrouplist = create_edit_subgrouplist_ui(recordid, record.subgroups);
            shared_subgroups_td.appendChild(div_subgrouplist);

            break;
        }
    }
}

function confirm_delete_curriculum(recordid)
{
    if (confirm('Are you sure you want to delete this Curriculum ?'))
    {
        // delete it!
        delete_curriculum(recordid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_curriculum(recordid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_curriculum'] = 1;
    jsondata['id'] = recordid;

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

            setTimeout(reload_page, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function save_curriculum(recordid)
{
    var curCurriculum = undefined;
    for(var index=0;index<gl_curriculum_List.length;index++)
    {
        var curriculum = gl_curriculum_List[index];
        if( curriculum.id == recordid )
        {
            curCurriculum = curriculum;
            break;
        }
    }

    if ( curCurriculum == undefined )
    {
        alert("無此事項紀錄!");
        return;
    }

    var typeid = getSelectedType(recordid);
    var maingroups = getSelectedMaingroup(recordid);
    var subgroups = getSelectedSubgroup(recordid);
    var number = document.getElementById("input_number_"+recordid).value;
    var shared_yes = document.getElementById("select_shared_yes_"+recordid).checked;
    var shared_no = document.getElementById("select_shared_no_"+recordid).checked;

    var indexPrefix = number.indexOf(gl_curriculumprefix);
    if ( indexPrefix != 0 )
    {
        alert("請填入課程代碼:"+gl_curriculumprefix);
        return;
    }

    var shared = 0;
    if ( shared_yes )
        shared = 1;

    // console.log("save_curriculum() typeid:"+typeid);
    // console.log("save_curriculum() maingroups:"+maingroups);
    // console.log("save_curriculum() subgroups:"+subgroups);
    // console.log("save_curriculum() number:"+number);
    // console.log("save_curriculum() shared:"+shared);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_curriculum'] = 1;
    jsondata['id'] = recordid;
    jsondata['typeid'] = typeid;
    jsondata['subgroups'] = subgroups;
    jsondata['number'] = number;
    jsondata['shared'] = shared;
    jsondata['sharedmaingroups'] = maingroups;

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

            setTimeout(reload_page, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

}

function save_shared_curriculum(recordid, curriculumid)
{
    var curCurriculumrecord = undefined;
    for(var index=0;index<gl_shared_curriculumrecord_List.length;index++)
    {
        var record = gl_shared_curriculumrecord_List[index];
        if( record.curriculumid == curriculumid )
        {
            curCurriculumrecord = record;
            break;
        }
    }

    if ( curCurriculumrecord == undefined )
    {
        alert("無此事項紀錄!");
        return;
    }

    var subgroups = getSelectedSubgroup(recordid);

    console.log("save_shared_curriculum() curCurriculumrecord.subgroups:"+curCurriculumrecord.subgroups);
    console.log("save_shared_curriculum() subgroups:"+subgroups);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_shared_curriculumrecord'] = 1;
    jsondata['recordid'] = recordid;
    jsondata['curriculumid'] = curriculumid;
    jsondata['subgroups'] = subgroups;

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

            setTimeout(reload_page, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

}

function show_CurriculumType()
{
    var table_body_ui = document.getElementById('CURRICULUM_TYPE_TABLE_BODY');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    // console.log("show_assists() gl_curriculum_List.length:"+gl_curriculum_List.length);

    for(var index=0;index<gl_curriculumtype_List.length;index++)
    {
        show_curriculumtype(table_body_ui, index+1, gl_curriculumtype_List[index]);
    }
}

function show_curriculumtype(table_body_ui, ui_index, record)
{
    var cellIndex = 0;

    var row = document.createElement('tr');
    row.setAttribute("id", record.id+"_CURRICULUM_TYPE_TABLE_ROW");
    if ((ui_index % 2) == 0)
        row.setAttribute("bgcolor","#ebebe0");

    table_body_ui.appendChild(row);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // 名稱
    row.appendChild(document.createElement('td'));
    var nameDiv = document.createElement('div');
    nameDiv.setAttribute("id", "curriculumtype_name_"+record.id);
    nameDiv.appendChild(document.createTextNode(record.name));
    row.cells[cellIndex].appendChild(nameDiv);
    cellIndex++;

    row.appendChild(document.createElement('td'));
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id", "edit_curriculumtype_btn_"+record.id);
    editBtn.setAttribute("title","Edit this Curriculum Type");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_curriculumtype("+ record.id +");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id", "edit_curriculumtype_img_"+record.id);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    // Delete button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("id", "delete_curriculumtype_btn_"+record.id);
    deleteBtn.setAttribute("style","display:none");
    deleteBtn.setAttribute("title","Delete this Curriculum Type");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","confirm_delete_curriculumtype("+ record.id +");return false;");
    var imgdelete = document.createElement("i");
    imgdelete.setAttribute("class","fa fa-trash-o");
    deleteBtn.appendChild(imgdelete);
    row.cells[cellIndex].appendChild(deleteBtn);

    cellIndex++;
}

function edit_curriculumtype(recordid)
{
    var nameDiv = document.getElementById("curriculumtype_name_"+recordid);

    while (nameDiv.firstChild)
        nameDiv.removeChild(nameDiv.firstChild);

    var editBtn = document.getElementById("edit_curriculumtype_btn_"+recordid);
    var editImg = document.getElementById("edit_curriculumtype_img_"+recordid);
    editBtn.setAttribute("title","Save this Curriculum Type");
    editBtn.setAttribute("onclick","save_curriculumtype("+ recordid +");return false;");
    editImg.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("delete_curriculumtype_btn_"+recordid);
    deleteBtn.setAttribute("style","display:block");

    for(var index=0;index<gl_curriculumtype_List.length;index++)
    {
        var curriculumtype = gl_curriculumtype_List[index];
        if( curriculumtype.id == recordid )
        {
            nameDiv.appendChild(create_input_text("curriculumtype_input_name_"+recordid, curriculumtype.name));
            break;
        }
    }
}

function save_curriculumtype(recordid)
{
    var input_name = document.getElementById("curriculumtype_input_name_"+recordid).value;

    if ( input_name.length == 0 )
    {
        alert("請輸入課程類別");
        return;
    }

    var curCurriculumtype = undefined;
    for(var index=0;index<gl_curriculumtype_List.length;index++)
    {
        var curriculumtype = gl_curriculumtype_List[index];
        if( curriculumtype.id == recordid )
        {
            curCurriculumtype = curriculumtype;
            break;
        }
    }

    if ( curCurriculumtype == undefined )
    {
        alert("無此課程紀錄!");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_curriculumtype'] = 1;
    jsondata['id'] = recordid;
    jsondata['name'] = input_name;

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

            setTimeout(reload_page, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

}

function confirm_delete_curriculumtype(recordid)
{
    if (confirm('Are you sure you want to delete this Curriculum Type ?'))
    {
        // delete it!
        delete_curriculumtype(recordid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_curriculumtype(recordid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_curriculumtype'] = 1;
    jsondata['id'] = recordid;

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

            setTimeout(reload_page, 300);
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

    for(var index=0;index<gl_shared_curriculumrecord_List.length;index++)
    {
        var curriculum = gl_shared_curriculumrecord_List[index];

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
        user_span.textContent = getUserName(curriculum.userid);
    }
    for(var index=0;index<gl_shared_curriculumrecord_List.length;index++)
    {
        var record = gl_shared_curriculumrecord_List[index];
        var user_span = document.getElementById("user_name_"+record.id+'_'+record.userid);
        user_span.textContent = getUserName(record.userid);
    }
}

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.textContent = text;

    return input_text;
}

function create_subgroup_label(context)
{
    var subgroup_span = document.createElement("span");
    subgroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    subgroup_span.textContent = context;
    return subgroup_span;
}

function create_maingroup_label(context)
{
    var maingroup_span = document.createElement("span");
    maingroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    maingroup_span.textContent = context;
    return maingroup_span;
}

function create_empty_spcae()
{
    var Spanspace = document.createElement("span");
    Spanspace.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;");
    Spanspace.textContent = "  ";
    return Spanspace;
}

function create_edit_typelist_ui(recordid, typeid)
{
    var typeselectList = document.createElement("select");
    typeselectList.setAttribute("id", "select_type_"+recordid);

    for(var index=0;index<gl_curriculumtype_List.length;index++)
    {
        var curriculumtype = gl_curriculumtype_List[index];

        var option = document.createElement("option");
        option.text = curriculumtype.name;
        option.value = curriculumtype.id;

        if ( typeid == curriculumtype.id )
            option.setAttribute("selected", "selected");

        typeselectList.appendChild(option);
    }

    return typeselectList;
}

function create_edit_subgrouplist_ui(fileid, idlist)
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

    // select all button
    if ( gl_groupid == 1 || gl_groupid == 2 || gl_groupid == 3 )
    {
        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","dropdown_selectall(\"subgroup\",\""+fileid+"\");return true;");
        item_li.appendChild(item_click);
        item_click.appendChild(document.createTextNode("全選"));
        div_ul.appendChild(item_li);
    }

    var idList = idlist.split(",");

    for (var i = 0; i < gl_usersubgroups.length; ++i)
    {
        var usersubgroup = gl_usersubgroups[i];
        var existed = check_numer_in_list(parseInt(usersubgroup.subgroupid), idList);
        div_ul.appendChild(create_dropdown_subgroup_item(fileid, i, parseInt(usersubgroup.subgroupid), usersubgroup.subgroupname, existed));
    }

    return div_subgrouplist;
}

function create_dropdown_subgroup_item(fileid, index, datavalue, dataname, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", datavalue);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_subgroup_click(\""+fileid+"\","+ index +");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_subgroup_"+fileid+"_"+index);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_subgroup_click(\""+fileid+"\","+ index +");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+dataname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_checkbox_subgroup_click(fileid, index)
{
    glDropdown_checkbox_subgroup_click = true;
}

var glDropdown_checkbox_subgroup_click = false;
function dropdown_subgroup_click(fileid, index)
{
    if (glDropdown_checkbox_subgroup_click == true)
    {
        glDropdown_checkbox_subgroup_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_subgroup_"+fileid+"_"+index).checked == true)
        document.getElementById("dropdown_checkbox_subgroup_"+fileid+"_"+index).checked = false;
    else
        document.getElementById("dropdown_checkbox_subgroup_"+fileid+"_"+index).checked = true;

    glDropdown_checkbox_subgroup_click = false;
}

function dropdown_selectall(prefix, fileid)
{
    console.log("dropdown_selectall() prefix:"+prefix+" fileid:"+fileid);

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

function create_subgroups(subgroups)
{
    var d = document.createElement("td");

    var uiRow = document.createElement("div");
    uiRow.setAttribute("class","row");
    d.appendChild(uiRow);

    var subgroupList = subgroups.split(",");

    if (subgroupList.indexOf('99999') >= 0)
    {
        var subgroupL_div = document.createElement("div");
        subgroupL_div.setAttribute("class","col-md-6");
        subgroupL_div.appendChild(create_subgroup_label('全部'));
        uiRow.appendChild(subgroupL_div);
    }
    else
    {
        var count = 0;
        var subgroupL_div = undefined;
        for(var index=0;index<subgroupList.length;index++)
        {
            var subgroupid = subgroupList[index];
            var SubgroupName = getSubgroupName(subgroupid);
            if (SubgroupName.length > 0)
            {
                //if ( count == 0 )
                //{
                    subgroupL_div = document.createElement("div");
                    subgroupL_div.setAttribute("class","col-md-6");
                    uiRow.appendChild(subgroupL_div);
                //}

                subgroupL_div.appendChild(create_subgroup_label(SubgroupName));
                //subgroupL_div.appendChild(create_empty_spcae());

                count++;
                if ( count >= 5 )
                {
                    count = 0;
                }
            }
        }
    }

    return d;
}

function create_edit_maingrouplist_ui(fileid, idlist, shared)
{
    var div_maingrouplist = document.createElement("div");
    div_maingrouplist.setAttribute("class","btn-group");
    div_maingrouplist.setAttribute("id","maingrouplist_"+fileid);
    if (shared == 0)
        div_maingrouplist.setAttribute("style","display:none;");

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "主集合";
    div_maingrouplist.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_maingrouplist.appendChild(div_ul);

    // select all button
    if ( gl_groupid == 1 || gl_groupid == 2 || gl_groupid == 3 )
    {
        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","dropdown_selectall(\"maingroup\",\""+fileid+"\");return true;");
        item_li.appendChild(item_click);
        item_click.appendChild(document.createTextNode("全選"));
        div_ul.appendChild(item_li);
    }

    var idList = idlist.split(",");

    for (var i = 0; i < gl_allmaingroups.length; ++i)
    {
        var usermaingroup = gl_allmaingroups[i];
        var existed = check_numer_in_list(usermaingroup.id, idList);
        div_ul.appendChild(create_dropdown_maingroup_item(fileid, i, usermaingroup.id, usermaingroup.name, existed));
    }

    return div_maingrouplist;
}

function create_dropdown_maingroup_item(fileid, index, datavalue, dataname, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", datavalue);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_maingroup_click(\""+fileid+"\","+ index +");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_maingroup_"+fileid+"_"+index);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_maingroup_click(\""+fileid+"\","+ index +");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+dataname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_checkbox_maingroup_click(fileid, index)
{
    glDropdown_checkbox_maingroup_click = true;
}

var glDropdown_checkbox_maingroup_click = false;
function dropdown_maingroup_click(fileid, index)
{
    if (glDropdown_checkbox_maingroup_click == true)
    {
        glDropdown_checkbox_maingroup_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_maingroup_"+fileid+"_"+index).checked == true)
        document.getElementById("dropdown_checkbox_maingroup_"+fileid+"_"+index).checked = false;
    else
        document.getElementById("dropdown_checkbox_maingroup_"+fileid+"_"+index).checked = true;

    glDropdown_checkbox_maingroup_click = false;
}

function create_maingroups(maingroups)
{
    var d = document.createElement("td");

    var uiRow = document.createElement("div");
    uiRow.setAttribute("class","row");
    d.appendChild(uiRow);

    var maingroupList = maingroups.split(",");

    if (maingroupList.indexOf('99999') >= 0)
    {
        var maingroupL_div = document.createElement("div");
        maingroupL_div.setAttribute("class","col-md-6");
        maingroupL_div.appendChild(create_maingroup_label('全部'));
        uiRow.appendChild(maingroupL_div);
    }
    else
    {
        var count = 0;
        var maingroupL_div = undefined;
        for(var index=0;index<maingroupList.length;index++)
        {
            var maingroupid = maingroupList[index];
            var MaingroupName = getMaingroupName(parseInt(maingroupid));
            if (MaingroupName.length > 0)
            {
                //if ( count == 0 )
                //{
                    maingroupL_div = document.createElement("div");
                    maingroupL_div.setAttribute("class","col-md-6");
                    d.appendChild(maingroupL_div);
                //}

                maingroupL_div.appendChild(create_subgroup_label(MaingroupName));
                //maingroupL_div.appendChild(create_empty_spcae());

                count++;
                if ( count >= 5 )
                {
                    count = 0;
                }
            }
        }
    }

    return d;
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

function create_type_label(typeid)
{
    var curriculumtype_span = document.createElement('span');
    for(var index=0;index<gl_curriculumtype_List.length;index++)
    {
        var curriculumtype = gl_curriculumtype_List[index];
        if ( curriculumtype.id == typeid )
        {
            curriculumtype_span.setAttribute("class",'badge bg-yellow');
            curriculumtype_span.textContent = curriculumtype.name;
            break;
        }
    }
    return curriculumtype_span;
}

function getSelectedSubgroup(recordid)
{
    var subgroups = [];
    var count = 0;
    for (var index = 0; index < gl_usersubgroups.length; ++index)
    {
        var usersubgroup = gl_usersubgroups[index];

        if (document.getElementById("dropdown_checkbox_subgroup_"+recordid+"_"+index).checked)
        {
            var subgroupid = parseInt(usersubgroup.subgroupid);
            subgroups.push(subgroupid);
            count++;
        }
    }

    if ( gl_groupid == 1 || gl_groupid == 2 )
    {
        if ( count == gl_usersubgroups.length )
        {
            subgroups = [];
            subgroups.push(99999);
        }
    }

    return subgroups.toString();
}

function getSelectedMaingroup(recordid)
{
    var maingroups = [];
    var count = 0;
    for (var index = 0; index < gl_allmaingroups.length; ++index)
    {
        var usermaingroup = gl_allmaingroups[index];

        if (document.getElementById("dropdown_checkbox_maingroup_"+recordid+"_"+index).checked)
        {
            var maingroupid = parseInt(usermaingroup.id);
            maingroups.push(maingroupid);
            count++;
        }
    }

    // if ( gl_groupid == 1 || gl_groupid == 2 )
    // {
    //     if ( count == gl_usermaingroups.length )
    //     {
    //         maingroups = [];
    //         maingroups.push(99999);
    //     }
    // }

    return maingroups.toString();
}

function getSelectedType(recordid)
{
    var e = document.getElementById("select_type_"+recordid);
    var typeid  = e.value;

    return typeid;
}

function check_numer_in_list(targetNumber, dataList)
{
    var existed = false;
    for (var i = 0; i < dataList.length; ++i)
    {
        var data = dataList[i];

        if ( parseInt(data) == targetNumber || parseInt(data) == 99999 )
        {
            existed = true;
            break;
        }
    }

    return existed;
}

function getSubgroupName(subgroupid)
{
    var subgroupname = '';

    if ( subgroupid == '99999' )
    {
        return '全部';
    }

    for (var i = 0; i < gl_usersubgroups.length; ++i)
    {
        var subgroup = gl_usersubgroups[i];

        // console.log("getSubgroupName() subgroup.subgroupid:"+subgroup.subgroupid);

        if ( parseInt(subgroup.subgroupid) == subgroupid )
        {
            subgroupname = subgroup.subgroupname;
            break;
        }

    }
    return subgroupname;
}

function getMaingroupName(maingroupid)
{
    var maingroupname = '';

    if ( maingroupid == '99999' )
    {
        return '全部';
    }

    for (var i = 0; i < gl_allmaingroups.length; ++i)
    {
        var maingroup = gl_allmaingroups[i];

        // console.log("getSubgroupName() subgroup.subgroupid:"+subgroup.subgroupid);

        if ( maingroup.id == maingroupid )
        {
            maingroupname = maingroup.name;
            break;
        }

    }
    return maingroupname;
}

function create_new_curriculum()
{
    create_empty_curriculum_table_item(gl_curriculum_List.length + 1);

    return false;
}

function create_empty_curriculum_table_item(ui_index)
{
    var record_table = document.getElementById('CURRICULUM_TABLE_BODY');
    var input_number = document.getElementById("input_number");
    if (input_number != undefined)
        return;

    var row = record_table.insertRow(0);
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(ui_index));
    cellIndex++;

    // 發起人
    row.appendChild(document.createElement('td'));
    var user_span = document.createElement('span');
    user_span.setAttribute("class",'badge bg-aqua-active');
    user_span.textContent = getUserName(parseInt(gl_userid));
    row.cells[cellIndex].appendChild(user_span);
    cellIndex++;

    // 發起單位
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // type list
    row.appendChild(document.createElement('td'));
    var div_subgrouplist = create_edit_typelist_ui("new", "");
    row.cells[cellIndex].appendChild(div_subgrouplist);
    cellIndex++;

    // 是否共享
    row.appendChild(document.createElement('td'));
    var sharedUI = create_select_shared_ui("new", 0);
    row.cells[cellIndex].appendChild(sharedUI);
    cellIndex++;

    // maingroup list
    row.appendChild(document.createElement('td'));
    var div_maingrouplist = create_edit_maingrouplist_ui("new", "", 0);
    row.cells[cellIndex].appendChild(div_maingrouplist);
    cellIndex++;

    // Subgroup list
    row.appendChild(document.createElement('td'));
    var div_subgrouplist = create_edit_subgrouplist_ui("new", "");
    row.cells[cellIndex].appendChild(div_subgrouplist);
    cellIndex++;

    // 課程號碼
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_number", gl_curriculumprefix));
    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_curriculum();");
    Button_save.textContent = "儲存";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;
}

function save_new_curriculum()
{
    var input_number = document.getElementById("input_number").value;
    // console.log("save_new_curriculum() input_number:"+input_number);

    if ( input_number.length == 0 )
    {
        alert("請輸入課程號碼");
        return;
    }

    var indexPrefix = input_number.indexOf(gl_curriculumprefix);
    if ( indexPrefix != 0 )
    {
        alert("請填入課程代碼:"+gl_curriculumprefix);
        return;
    }

    var subgroups = getSelectedSubgroup('new');
    // console.log("save_new_curriculum() subgroups:"+subgroups);

    if ( subgroups.length == 0 )
    {
        alert("請選擇子集合");
        return;
    }

    var maingroups = getSelectedMaingroup('new');
    // console.log("save_new_curriculum() maingroups:"+maingroups);

    var typeid = getSelectedType('new');
    // console.log("save_new_curriculum() typeid:"+typeid);

    var shared_yes = document.getElementById("select_shared_yes_new").checked;
    var shared_no = document.getElementById("select_shared_no_new").checked;

    var shared = 0;
    if ( shared_yes )
        shared = 1;

    if ( shared_yes )
    {
        if ( maingroups.length == 0 )
        {
            alert("請選擇主集合");
            return;
        }
    }

    // console.log("save_new_curriculum() shared:"+shared);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_curriculum'] = 1;
    jsondata['typeid'] = typeid;
    jsondata['subgroups'] = subgroups;
    jsondata['number'] = input_number;
    jsondata['shared'] = shared;
    jsondata['sharedmaingroups'] = maingroups;

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

            setTimeout(reload_page, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

}

function create_new_curriculumtype()
{
    create_empty_curriculumtype_table_item(gl_curriculumtype_List.length + 1);
    return false;
}

function create_empty_curriculumtype_table_item(ui_index)
{
    var record_table = document.getElementById('CURRICULUM_TYPE_TABLE_BODY');
    var input_name = document.getElementById("new_type_input_name");
    if (input_name != undefined)
        return;

    var row = record_table.insertRow(0);
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(ui_index));
    cellIndex++;

    // 名稱
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_name", ""));
    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_curriculumtype();");
    Button_save.textContent = "儲存";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;
}

function save_new_curriculumtype()
{
    var input_name = document.getElementById("input_name").value;

    console.log("save_new_curriculumtype() input_name:"+input_name);

    if ( input_name.length == 0 )
    {
        alert("請輸入課程類別");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_curriculumtype'] = 1;
    jsondata['name'] = input_name;

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

            setTimeout(reload_page, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function edit_curriculumprefix()
{
    var curriculumprefix_area = clearChildView("curriculumprefix_area");

    var input_prefix = document.createElement("input");
    input_prefix.setAttribute("type","text");
    input_prefix.setAttribute("id","curriculumprefix_text");
    input_prefix.value = gl_curriculumprefix;
    curriculumprefix_area.appendChild(input_prefix);

    var curriculumprefix_btn = document.getElementById("curriculumprefix_btn");
    curriculumprefix_btn.setAttribute("title","儲存課程代碼");
    curriculumprefix_btn.setAttribute("onclick","save_curriculumprefix();return false;");

    var curriculumprefix_img = document.getElementById("curriculumprefix_img");
    curriculumprefix_img.setAttribute("class","fa fa-save");

    return false;
}

function save_curriculumprefix()
{
    var curriculumprefix_text = document.getElementById("curriculumprefix_text");

    if ( curriculumprefix_text.value.length == 0 )
    {
        alert("請輸入課程代碼");
        return;
    }

    gl_curriculumprefix = curriculumprefix_text.value;

    var curriculumprefix_area = clearChildView("curriculumprefix_area");

    var span_prefix = document.createElement("span");
    span_prefix.setAttribute("class","badge bg-aqua-active");
    span_prefix.setAttribute("id","curriculumprefix_label");
    span_prefix.textContent = "課程代碼:"+gl_curriculumprefix;
    curriculumprefix_area.appendChild(span_prefix);

    var curriculumprefix_btn = document.getElementById("curriculumprefix_btn");
    curriculumprefix_btn.setAttribute("title","編輯課程代碼");
    curriculumprefix_btn.setAttribute("onclick","edit_curriculumprefix();return false;");

    var curriculumprefix_img = document.getElementById("curriculumprefix_img");
    curriculumprefix_img.setAttribute("class","fa fa-edit");

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_curriculumprefix'] = 1;
    jsondata['prefix'] = gl_curriculumprefix;

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

            setTimeout(reload_page, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function isNumeric(n)
{
    return n !== '' && !isNaN(parseFloat(n)) && isFinite(n);
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

function getSharedCurriculum(curriculumid)
{
    var shared_curriculum = undefined;

    for(var index=0;index<gl_shared_curriculum_List.length;index++)
    {
        var curriculum = gl_shared_curriculum_List[index];
        if ( curriculum.id == curriculumid )
        {
            shared_curriculum = curriculum;
            return shared_curriculum;
        }
    }

    return shared_curriculum;
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

function move_shared_page_backward()
{
    var current_shared_page_num_ui = document.getElementById('id_current_shared_page_num');

    if ( glSharedCurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        glSharedCurrentPage--;
        current_shared_page_num_ui.textContent = glSharedCurrentPage;
        reload_page();
    }
}

function move_shared_page_forward()
{
    var current_shared_page_num_ui = document.getElementById('id_current_shared_page_num');

    glSharedCurrentPage++;
    current_shared_page_num_ui.textContent = glSharedCurrentPage;
    reload_page();
}

function open_curriculum_page()
{
    var win = window.open('/index.php/vilscurriculum/'+project_id, '_blank');
    win.focus();
}
