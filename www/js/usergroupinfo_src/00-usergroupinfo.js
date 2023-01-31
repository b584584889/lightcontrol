/*
 * Author: ikki Chung
 * Date: 2020.03.20
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


// var LOAD_USERGROUP_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;

//var gl_usergroup_num = 0;
//var gl_usergroup_load_index = 0;
var gl_usergroups = [];

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

                //load_usergroup_info();
                load_usergroup();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                draw_table();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
    }

    if ( CurrentLoadIndex != 2 )
        setTimeout(check_loading_status, 100);
    else
    {
        console.log("check_loading_status() Stop.");
    }
}

// function load_usergroup_info()
// {
//     var targetURL = gl_target_server + "/php/vilsnodes.php";
//     var p = $.getJSON(targetURL, 'usergroupcount=1'+'&project_id='+project_id, function(data)
//     {
//         gl_usergroup_num = parseInt(data['USERGROUP_NUM']);
//         gl_usergroup_load_index = 0;
//
//         gl_usergroups = [];
//     })
//     .success(function() {
//         setTimeout(load_usergroup, 500);
//     })
//     .error(function() {
//         setTimeout(load_usergroup_info, 1000);
//     })
//     .complete(function() {
//     });
// }

// function load_usergroup()
// {
//     if (gl_usergroup_load_index >= gl_usergroup_num)
//     {
//         return;
//     }
//
//     var targetURL = gl_target_server + "/php/vilsnodes.php";
//     var p = $.getJSON(targetURL, 'loadusergroup=1&s='+gl_usergroup_load_index+'&count='+LOAD_USERGROUP_NUM+'&project_id='+project_id, function(data)
//     {
//         var usergroup_num = 0;
//         $.each(data, function(index, usergroup)
//         {
//             gl_usergroups.push(usergroup);
//             usergroup_num++;
//         });
//
//         gl_usergroup_load_index += usergroup_num;
//
//     })
//     .success(function() {
//         if (gl_usergroup_load_index >= gl_usergroup_num)
//         {
//             CurrentLoadIndex = CurrentLoadIndex + 1;
//         }
//         else
//         {
//             setTimeout(load_usergroup, 50);
//         }
//     })
//     .error(function() {
//     })
//     .complete(function() {
//     });
// }

function load_usergroup()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadusergroup=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, usergroup)
        {
            gl_usergroups.push(usergroup);
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

function draw_table()
{
    var record_table = document.getElementById('USERGROUPINFO_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

     console.log("draw_table() gl_usergroups.length:"+gl_usergroups.length);

    for (var i = 0; i < gl_usergroups.length; ++i)
    {
        var group = gl_usergroups[i];

        var row = create_record_table_item(i, group);
        record_table.appendChild(row);
    }
}

function create_record_table_item(ui_index, record)
{
    if (record == null)
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
    var tdgroupid = document.createElement('td');
    tdgroupid.setAttribute("id","uiid_groupname_"+record.groupid);
    row.appendChild(tdgroupid);

    var Spangroupid = create_group_label(record.groupname);
    row.cells[cellIndex].appendChild(Spangroupid);
    cellIndex++;

    // var Spangroupid  = document.createElement("span");
    // Spangroupid.setAttribute("class","label label-warning");
    // Spangroupid.textContent = record.groupname;
    // row.cells[cellIndex].appendChild(Spangroupid);
    // cellIndex++;

    // 權限
    var tdpermission = document.createElement('td');
    tdpermission.setAttribute("id","uiid_permission_"+record.groupid);
    tdpermission.textContent = record.permission;
    row.appendChild(tdpermission);
    // var Spanpermission  = document.createElement("span");
    // Spanpermission.setAttribute("class","label label-info");
    // Spanpermission.textContent = record.permission;
    // row.cells[cellIndex].appendChild(Spanpermission);
    cellIndex++;

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    // Edit button
    // var editBtn = document.createElement("button");
    // editBtn.setAttribute("id","uiid_editbtn_"+record.groupid);
    // editBtn.setAttribute("title","Edit this UserGroup");
    // editBtn.setAttribute("href","javascript:;");
    // editBtn.setAttribute("onclick","edit_usergroup(\""+record.groupid+"\");return false;");
    // var imgEdit = document.createElement("i");
    // imgEdit.setAttribute("id","uiid_editimg_"+record.groupid);
    // imgEdit.setAttribute("class","fa fa-edit");
    // editBtn.appendChild(imgEdit);
    // row.cells[cellIndex].appendChild(editBtn);
    //
    // // delete uesr button
    // var deleteBtn = document.createElement("button");
    // deleteBtn.setAttribute("style","display:none");
    // deleteBtn.setAttribute("id","uiid_deletebtn_"+record.groupid);
    // deleteBtn.setAttribute("title","Delete This UserGroup");
    // deleteBtn.setAttribute("href","javascript:;");
    // deleteBtn.setAttribute("onclick","confirm_delete_usergroup(\""+record.groupid+"\", \""+ record.groupname +"\");return false;");
    // var imgDelete = document.createElement("i");
    // imgDelete.setAttribute("class","fa fa-user-times");
    // deleteBtn.appendChild(imgDelete);
    // row.cells[cellIndex].appendChild(deleteBtn);
    // cellIndex++;

    return row;
}

function create_empty_record_table_item(ui_index)
{
    var empty_record_table_item = document.getElementById('empty_record_table_item');
    if ( empty_record_table_item != null )
        return;

    var record_table = document.getElementById('USERGROUPINFO_TABLE');

    var row = document.createElement('tr');
    row.setAttribute("id", "empty_record_table_item");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // groupname
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_groupname"));
    cellIndex++;

    // permission
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_permission"));
    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_usergroup();");
    Button_save.textContent = "儲存";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;

    record_table.appendChild(row);
}

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.textContent = text;

    return input_text;
}

function edit_usergroup(groupid)
{
    console.log("edit_usergroup() groupid:"+groupid);

    var group = getUserGroup(groupid);
    if (group == null)
    {
        console.log("edit_usergroup() group == null");
        return;
    }

    create_edit_ui("uiid_groupname", groupid, group.groupname);
    create_edit_ui("uiid_permission", groupid, group.permission);

    var editBtn = document.getElementById("uiid_editbtn_"+groupid);
    editBtn.setAttribute("title","Save this UserGroup");
    editBtn.setAttribute("onclick","save_usergroup(\""+groupid+"\");return false;");
    var imgEdit = document.getElementById("uiid_editimg_"+groupid);
    imgEdit.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("uiid_deletebtn_"+groupid);
    deleteBtn.setAttribute("style","display:block");

}

function create_edit_ui(uiid, groupid, text)
{
    var uiid_name = clearChildView(uiid+"_"+groupid);
    var new_input = create_input_text(uiid+"_"+groupid+"_edit", text);
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

function create_group_label(context)
{
    var group_span = document.createElement("span");
    group_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#f0ad4e;");
    group_span.textContent = context;
    return group_span;
}

function getUserGroup(groupid)
{
    var retusergroup = null;
    for (var i = 0; i < gl_usergroups.length; ++i)
    {
        var group = gl_usergroups[i];

        if ( group.groupid === groupid )
        {
            retusergroup = group;
            break;
        }

    }
    return retusergroup;
}

function save_new_usergroup()
{
    var input_groupname = document.getElementById("input_groupname").value;
    var input_permission = document.getElementById("input_permission").value;
    if ( input_groupname.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_usergroup'] = 1;
    jsondata['groupname'] = input_groupname;
    jsondata['permission'] = input_permission;

    var targetURL = gl_target_server + "/php/vilsnodes.php";

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

            CurrentLoadIndex = 0;
            setTimeout(check_loading_status, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function save_usergroup(groupid)
{
    var value_groupname = document.getElementById("uiid_groupname_"+groupid+"_edit").value;
    var value_permission = document.getElementById("uiid_permission_"+groupid+"_edit").value;

    if ( value_groupname.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_usergroup'] = 1;
    jsondata['groupid'] = groupid;
    jsondata['groupname'] = value_groupname;
    jsondata['permission'] = value_permission;

    var targetURL = gl_target_server + "/php/vilsnodes.php";

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

            CurrentLoadIndex = 0;
            setTimeout(check_loading_status, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_usergroup(groupid, groupname)
{
    var groupid = parseInt(groupid);
    if (groupid == 1 || groupid == 2 || groupid == 3)
    {
        alert("Can not delet this group!!!");

        return;
    }

    if (confirm('Are you sure you want to delete this user group:'+groupname+' ?'))
    {
        // reset it!
        delete_usergroup(groupid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_usergroup(groupid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_usergroup'] = 1;
    jsondata['groupid'] = parseInt(groupid);

    var targetURL = gl_target_server + "/php/vilsnodes.php";

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
            CurrentLoadIndex = 0;
            setTimeout(check_loading_status, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

$("#createnewusergroupbutton").click(function()
{
    create_empty_record_table_item(gl_usergroups.length + 1);

    return false;
});

function open_usergroupinfo_page()
{
    var win = window.open('/index.php/vilsusergroupinfo/'+project_id, '_blank');
    win.focus();
}
