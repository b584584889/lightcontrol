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

var gl_user_info = [];
var gl_searchusers = [];
var gl_usersubgroups = [];
var gl_assist_List = [];

var glCurrentPage = 1;

var pre_input_value = '';
var glNewAssistIndex = 0;

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
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_assists();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_assists();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_user_info();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_assist_config();

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
            gl_usersubgroups.push(usersubgroup);
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

function load_assists()
{
    var targetURL = gl_target_server + "/php/assist.php";
    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    gl_assist_List = [];

    $.getJSON(targetURL, 'loadassists=1'+'&project_id='+project_id+"&offset="+offset+"&count="+count, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            gl_assist_List.push(record);
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

function show_assists()
{
    var table_body_ui = document.getElementById('ASSIST_TABLE_BODY');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    // console.log("show_assists() gl_assist_List.length:"+gl_assist_List.length);

    for(var index=0;index<gl_assist_List.length;index++)
    {
        show_assist(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, gl_assist_List[index]);
    }
}

function show_assist(table_body_ui, ui_index, record)
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

    // 事項名稱
    row.appendChild(document.createElement('td'));
    var titleDiv = document.createElement('div');
    titleDiv.setAttribute("id", "title_"+record.id);
    titleDiv.appendChild(document.createTextNode(record.title));
    row.cells[cellIndex].appendChild(titleDiv);
    cellIndex++;

    // 事項說明
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.setAttribute("id", "description_"+record.id);
    descriptionDiv.appendChild(document.createTextNode(record.description));
    row.cells[cellIndex].appendChild(descriptionDiv);
    cellIndex++;

    // 子集合權限
    var td_subgroups = create_subgroup(record.subgroups);
    td_subgroups.setAttribute("id","uiid_subgroup_"+record.id);
    row.appendChild(td_subgroups);
    cellIndex++;

    // 負責人員
    var assignusers_td = document.createElement('td');
    assignusers_td.setAttribute("id","uiid_assignusers_"+record.id);
    row.appendChild(assignusers_td);
    var div_assignusers = create_assignusers(record.id, record.assignusers);
    assignusers_td.appendChild(div_assignusers);
    cellIndex++;
    // var assignaccounts_td = document.createElement('td');
    // assignaccounts_td.setAttribute("id","uiid_assignaccounts_"+record.id);
    // row.appendChild(assignaccounts_td);
    // var div_assignaccounts = create_assignaccounts(record.id, record.assignaccounts);
    // assignaccounts_td.appendChild(div_assignaccounts);
    // cellIndex++;

    // 操作
    // Edit button
    if (gl_groupid == 1 || gl_groupid == 2)
    {
        row.appendChild(document.createElement('td'));
        var editBtn = document.createElement("button");
        editBtn.setAttribute("id", "editbtn_"+record.id);
        editBtn.setAttribute("title","Edit this Assist");
        editBtn.setAttribute("href","javascript:;");
        editBtn.setAttribute("onclick","edit_assist('"+ record.id +"');return false;");
        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("id", "editimg_"+record.id);
        imgEdit.setAttribute("class","fa fa-edit");
        editBtn.appendChild(imgEdit);
        row.cells[cellIndex].appendChild(editBtn);

        // Delete button
        var deleteBtn = document.createElement("button");
        deleteBtn.setAttribute("id", "deletebtn_"+record.id);
        deleteBtn.setAttribute("style","display:none");
        deleteBtn.setAttribute("title","Delete this Assist");
        deleteBtn.setAttribute("href","javascript:;");
        deleteBtn.setAttribute("onclick","confirm_delete_assist('"+ record.id +"');return false;");
        var imgdelete = document.createElement("i");
        imgdelete.setAttribute("class","fa fa-trash-o");
        deleteBtn.appendChild(imgdelete);
        row.cells[cellIndex].appendChild(deleteBtn);

        cellIndex++;
    }

}

function load_user_info()
{
    var queryuser = '';

    var queryCount = 0;
    for(var index=0;index<gl_assist_List.length;index++)
    {
        var assist = gl_assist_List[index];
        //var assignaccountList = assist.assignaccounts.split(",");
        var assignuserList = assist.assignusers.split(",");

        for(var usridx=0;usridx<assignuserList.length;usridx++)
        {
            var userid = parseInt(assignuserList[usridx]);

            //
            // check exist
            //
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

            if ( bFound == false)
            {
                if (userid > 0)
                {
                    if ( queryCount > 0 )
                        queryuser += ',';
                    queryuser += userid;
                    queryCount += 1;
                }
            }
        }
    }

    if (queryuser.length == 0)
    {
        //update_account_name();
        update_user_name();

        return;
    }

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
        // update_account_name();
        update_user_name();
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function update_user_name()
{
    for(var index=0;index<gl_assist_List.length;index++)
    {
        var assist = gl_assist_List[index];
        // var assignaccountList = assist.assignaccounts.split(",");
        var assignuserList = assist.assignusers.split(",");

        for(var usridx=0;usridx<assignuserList.length;usridx++)
        {
            var userid = parseInt(assignuserList[usridx]);

            var user_span = document.getElementById("assign_user_name_"+assist.id+'_'+userid);
            user_span.textContent = getUserName(userid);
        }
    }
}

// function update_account_name()
// {
//     for(var index=0;index<gl_assist_List.length;index++)
//     {
//         var assist = gl_assist_List[index];
//         var assignaccountList = assist.assignaccounts.split(",");
//
//         for(var accidx=0;accidx<assignaccountList.length;accidx++)
//         {
//             var account = assignaccountList[accidx];
//
//             var user_span = document.getElementById("assign_account_name_"+assist.id+'_'+account);
//             var user = getUser(account);
//             if (user != null)
//             {
//                 user_span.textContent = user.name;
//             }
//         }
//     }
// }

function load_assist_config()
{
    var targetURL = gl_target_server + "/php/assist.php";
    var p = $.getJSON(targetURL, 'queryassistconfig=1&project_id='+project_id, function(data)
    {
        var config_sendNotifyTime = document.getElementById("config_sendNotifyTime");
        config_sendNotifyTime.textContent = data.sendNotifyTime;
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function edit_assist(assistid)
{
    // console.log("edit_assist() assistid:"+assistid);

    var titleDiv = document.getElementById("title_"+assistid);
    var descriptionDiv = document.getElementById("description_"+assistid);
    var subgrouplist_span = document.getElementById("uiid_subgroup_"+assistid);
    var assignusers_td = document.getElementById("uiid_assignusers_"+assistid);
    //var assignaccounts_td = document.getElementById("uiid_assignaccounts_"+assistid);

    while (titleDiv.firstChild)
        titleDiv.removeChild(titleDiv.firstChild);
    while (descriptionDiv.firstChild)
        descriptionDiv.removeChild(descriptionDiv.firstChild);
    while (subgrouplist_span.firstChild)
        subgrouplist_span.removeChild(subgrouplist_span.firstChild);
    while (assignusers_td.firstChild)
        assignusers_td.removeChild(assignusers_td.firstChild);
    // while (assignaccounts_td.firstChild)
    //     assignaccounts_td.removeChild(assignaccounts_td.firstChild);

    var editBtn = document.getElementById("editbtn_"+assistid);
    var editImg = document.getElementById("editimg_"+assistid);
    editBtn.setAttribute("title","Save this Assist");
    editBtn.setAttribute("onclick","save_assist('"+ assistid +"');return false;");
    editImg.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("deletebtn_"+assistid);
    deleteBtn.setAttribute("style","display:block");

    for(var index=0;index<gl_assist_List.length;index++)
    {
        var assist = gl_assist_List[index];
        if( assist.id == assistid )
        {
            titleDiv.appendChild(create_input_text("input_title_"+assistid, assist.title));
            descriptionDiv.appendChild(create_input_text("input_description_"+assistid, assist.description));

            var div_subgrouplist = create_edit_subgrouplist_ui(assistid, assist.subgroups);
            subgrouplist_span.appendChild(div_subgrouplist);

            var div_assignuserlist = create_edit_assignuserlist_ui(assistid, assist.assignusers);
            assignusers_td.appendChild(div_assignuserlist);

            // var div_assignaccountlist = create_edit_assignaccountlist_ui(assistid, assist.assignaccounts);
            // assignaccounts_td.appendChild(div_assignaccountlist);

            $source = document.querySelector('#search_user_text_'+assistid);
            $source.addEventListener('input', check_search_user_change);
            $source.recordid = assistid;

            break;
        }
    }
}

function save_assist(assistid)
{
    var input_title = document.getElementById("input_title_"+assistid).value;
    var input_description = document.getElementById("input_description_"+assistid).value;

    if ( input_title.length == 0 )
    {
        alert("請輸入事項名稱");
        return;
    }

    if ( input_description.length == 0 )
    {
        alert("請輸入事項說明");
        return;
    }

    var curAssist = undefined;
    for(var index=0;index<gl_assist_List.length;index++)
    {
        var assist = gl_assist_List[index];
        if( assist.id == assistid )
        {
            curAssist = assist;
            break;
        }
    }

    if ( curAssist == undefined )
    {
        alert("無此事項紀錄!");
        return;
    }
    var subgroups = getSelectedSubgroup(assistid);
    //var assignaccounts = getSelectedAccount(assistid);
    var assignusers = getSelectedUser(assistid);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_assist'] = 1;
    jsondata['id'] = assistid;
    jsondata['title'] = input_title;
    jsondata['description'] = input_description;
    jsondata['subgroups'] = subgroups;
    jsondata['assignusers'] = assignusers;
    // jsondata['assignaccounts'] = assignaccounts;

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

function save_new_assist()
{
    var input_title = document.getElementById("input_title").value;
    var input_description = document.getElementById("input_description").value;

    var subgroups = getSelectedSubgroup('new');
    var assignusers = getSelectedUser('new');
    // var assignaccounts = getSelectedAccount('new');

    console.log("save_new_assist() subgroups:"+subgroups);

    if ( input_title.length == 0 )
    {
        alert("請輸入事項名稱");
        return;
    }

    if ( input_description.length == 0 )
    {
        alert("請輸入事項說明");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_assist'] = 1;
    jsondata['title'] = input_title;
    jsondata['description'] = input_description;
    jsondata['subgroups'] = subgroups;
    jsondata['assignusers'] = assignusers;
    //jsondata['assignaccounts'] = assignaccounts;

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

function confirm_delete_assist(assistid)
{
    if (confirm('Are you sure you want to delete this assist ?'))
    {
        // delete it!
        delete_assist(assistid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_assist(assistid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_assist'] = 1;
    jsondata['id'] = assistid;

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

function create_empty_assist_table_item(ui_index)
{
    var record_table = document.getElementById('ASSIST_TABLE_BODY');
    var input_title = document.getElementById("input_title");
    if (input_title != undefined)
        return;

    var row = record_table.insertRow(0);
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""));
    cellIndex++;

    // 事項名稱
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_title"));
    cellIndex++;

    // 事項說明
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_description"));
    cellIndex++;

    // Subgroup list
    row.appendChild(document.createElement('td'));
    var div_subgrouplist = create_edit_subgrouplist_ui("new", "");
    row.cells[cellIndex].appendChild(div_subgrouplist);
    cellIndex++;

    // 負責人員
    row.appendChild(document.createElement('td'));
    var searchUserDiv = document.createElement('div');
    searchUserDiv.setAttribute("class","btn-group");
    row.cells[cellIndex].appendChild(searchUserDiv);

    var searchUserinput = document.createElement('input');
    searchUserinput.setAttribute("type","text");
    searchUserinput.setAttribute("placeholder","尋找人員");
    searchUserinput.setAttribute("id","search_user_text_new");
    searchUserDiv.appendChild(searchUserinput);

    var searchUserDropdown = document.createElement('ul');
    searchUserDropdown.setAttribute("class","dropdown-menu");
    searchUserDropdown.setAttribute("id","search_user_text_dropdown_new");
    searchUserDiv.appendChild(searchUserDropdown);

    var assignusersDiv = document.createElement('div');
    assignusersDiv.setAttribute("id","assignusers_new");
    row.cells[cellIndex].appendChild(assignusersDiv);
    cellIndex++;

    // var assignaccountsDiv = document.createElement('div');
    // assignaccountsDiv.setAttribute("id","assignaccounts_new");
    // row.cells[cellIndex].appendChild(assignaccountsDiv);
    // cellIndex++;


    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_assist();");
    Button_save.textContent = "儲存";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;
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

function create_empty_spcae()
{
    var Spanspace = document.createElement("span");
    Spanspace.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;");
    Spanspace.textContent = "  ";
    return Spanspace;
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

function create_edit_assignuserlist_ui(assistid, assignusers)
{
    // 負責人員
    var assignusersUI = document.createElement('div');

    var searchUserDiv = document.createElement('div');
    searchUserDiv.setAttribute("class","btn-group");
    assignusersUI.appendChild(searchUserDiv);

    var searchUserinput = document.createElement('input');
    searchUserinput.setAttribute("type","text");
    searchUserinput.setAttribute("placeholder","尋找人員");
    searchUserinput.setAttribute("id","search_user_text_"+assistid);
    searchUserDiv.appendChild(searchUserinput);

    var searchUserDropdown = document.createElement('ul');
    searchUserDropdown.setAttribute("class","dropdown-menu");
    searchUserDropdown.setAttribute("id","search_user_text_dropdown_"+assistid);
    searchUserDiv.appendChild(searchUserDropdown);

    var assignusersDiv = document.createElement('div');
    assignusersDiv.setAttribute("id","assignusers_"+assistid);
    assignusersUI.appendChild(assignusersDiv);

    // var assignaccountsDiv = document.createElement('div');
    // assignaccountsDiv.setAttribute("id","assignaccounts_"+assistid);
    // assignaccountsUI.appendChild(assignaccountsDiv);

    var assignuserList = assignusers.split(",");

    for(var index=0;index<assignuserList.length;index++)
    {
        var userid = parseInt(assignuserList[index]);
        var username = getUserName(userid);
        var user_div = create_clickable_user(assistid, userid, username);
        assignusersDiv.appendChild(user_div);

        // var user = getUser(accountid);
        // var username = '';
        // if (user != null)
        // {
        //     username = user.name;
        //     var user_div = create_clickable_user(assistid, accountid, username);
        //     assignaccountsDiv.appendChild(user_div);
        // }
    }

    return assignusersUI;
}

// function create_edit_assignaccountlist_ui(assistid, assignaccounts)
// {
//     // 負責人員
//     var assignaccountsUI = document.createElement('div');
//
//     var searchUserDiv = document.createElement('div');
//     searchUserDiv.setAttribute("class","btn-group");
//     assignaccountsUI.appendChild(searchUserDiv);
//
//     var searchUserinput = document.createElement('input');
//     searchUserinput.setAttribute("type","text");
//     searchUserinput.setAttribute("placeholder","尋找人員");
//     searchUserinput.setAttribute("id","search_user_text_"+assistid);
//     searchUserDiv.appendChild(searchUserinput);
//
//     var searchUserDropdown = document.createElement('ul');
//     searchUserDropdown.setAttribute("class","dropdown-menu");
//     searchUserDropdown.setAttribute("id","search_user_text_dropdown_"+assistid);
//     searchUserDiv.appendChild(searchUserDropdown);
//
//     var assignaccountsDiv = document.createElement('div');
//     assignaccountsDiv.setAttribute("id","assignaccounts_"+assistid);
//     assignaccountsUI.appendChild(assignaccountsDiv);
//
//     var assignaccountList = assignaccounts.split(",");
//
//     for(var index=0;index<assignaccountList.length;index++)
//     {
//         var accountid = assignaccountList[index];
//         var user = getUser(accountid);
//         var username = '';
//         if (user != null)
//         {
//             username = user.name;
//             var user_div = create_clickable_user(assistid, accountid, username);
//             assignaccountsDiv.appendChild(user_div);
//         }
//     }
//
//     return assignaccountsUI;
// }

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

function create_subgroup(subgroups)
{
    var d = document.createElement("td");

    var subgroupList = subgroups.split(",");

    if (subgroupList.indexOf('99999') >= 0)
    {
        var subgroupL_div = document.createElement("div");
        subgroupL_div.appendChild(create_subgroup_label('全部'));
        d.appendChild(subgroupL_div);
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
                if ( count == 0 )
                {
                    subgroupL_div = document.createElement("div");
                    d.appendChild(subgroupL_div);
                }

                subgroupL_div.appendChild(create_subgroup_label(SubgroupName));
                subgroupL_div.appendChild(create_empty_spcae());

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

function create_assignusers(recordid, assignusers)
{
    var ui_div = document.createElement("div");

    var assignuserList = assignusers.split(",");

    for(var index=0;index<assignuserList.length;index++)
    {
        var assignuser = assignuserList[index];
        var userid = parseInt(assignuser);

        var user_div = document.createElement('div');
        user_div.setAttribute("id",'assign_user_'+recordid+'_'+userid);
        ui_div.appendChild(user_div);

        var user_span = document.createElement('span');
        user_span.setAttribute("class",'badge bg-aqua-active');
        user_span.setAttribute("id",'assign_user_name_'+recordid+'_'+userid);
        user_span.textContent = getUserName(userid);
        user_div.appendChild(user_span);
    }

    return ui_div;
}

// function create_assignaccounts(recordid, assignaccounts)
// {
//     var ui_div = document.createElement("div");
//
//     var assignaccountList = assignaccounts.split(",");
//
//     for(var index=0;index<assignaccountList.length;index++)
//     {
//         var assignaccount = assignaccountList[index];
//
//         var user_div = document.createElement('div');
//         user_div.setAttribute("id",'assign_account_'+recordid+'_'+assignaccount);
//         ui_div.appendChild(user_div);
//
//         var user_span = document.createElement('span');
//         user_span.setAttribute("class",'badge bg-aqua-active');
//         user_span.setAttribute("id",'assign_account_name_'+recordid+'_'+assignaccount);
//         user_span.textContent = assignaccount;
//         user_div.appendChild(user_span);
//     }
//
//     return ui_div;
// }

function getSelectedSubgroup(fileid)
{
    var subgroups = [];
    var count = 0;
    for (var index = 0; index < gl_usersubgroups.length; ++index)
    {
        var usersubgroup = gl_usersubgroups[index];

        if (document.getElementById("dropdown_checkbox_subgroup_"+fileid+"_"+index).checked)
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

function getSelectedUser(recordid)
{
    var userids = [];
    var t = document.getElementById('assignusers_'+recordid).childNodes;
    for(let i = 0; i < t.length; i++)
    {
        var user_ui = t[i];
        var userid = user_ui.id.replace('assign_user_'+recordid+'_', '');
        userids.push(userid);
        console.log("getSelectedUser() userid:"+userid);
    }
    return userids.toString();
}

// function getSelectedAccount(recordid)
// {
//     var accountids = [];
//     var t = document.getElementById('assignaccounts_'+recordid).childNodes;
//     for(let i = 0; i < t.length; i++)
//     {
//         var account_ui = t[i];
//         var accountid = account_ui.id.replace('assign_account_'+recordid+'_', '');
//         accountids.push(accountid);
//         console.log("getSelectedAccount() accountid:"+accountid);
//     }
//     return accountids.toString();
// }

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

function create_new_assist()
{
    create_empty_assist_table_item(gl_assist_List.length + 1);

    $source = document.querySelector('#search_user_text_new');
    $source.addEventListener('input', check_search_user_change);// register for oninput
    $source.recordid = 'new';

    return false;
}


var gl_search_start = 0;
function search_user_change(recordid)
{
    console.log("search_user_change() recordid:"+recordid);

    var input_value = document.getElementById("search_user_text_"+recordid).value;

    if ( input_value === pre_input_value )
    {
        console.log("search_user_change() input_value === pre_input_value, input_value:"+input_value);

        if (input_value === '')
        {
            clearChildView('search_user_text_dropdown_'+recordid);
        }

        return;
    }

    console.log("search_user_change() input_value:"+input_value);

    search_text_change(recordid, input_value);

    return false;
}

function check_search_user_change(evt)
{
    // console.log("check_search_user_change() recordid:"+evt.currentTarget.recordid);

    if ( gl_search_start == 0 )
    {
        gl_search_start = new Date();
    }

    var cur_time = new Date();
    var seconds = (cur_time.getTime() - gl_search_start.getTime()) / 1000;

    if ( seconds >= 1 )
    {
        gl_search_start = 0;
        search_user_change(evt.currentTarget.recordid);
    }
    else
    {
        setTimeout(search_user_change, 500, evt.currentTarget.recordid);
    }
}

function search_text_change(recordid, input_value)
{
    var offset = 0;
    var count = 30;

    gl_searchusers = [];

    var SendDate = "searchuser=1&s="+offset+"&count="+count+"&project_id="+project_id+"&text="+input_value;

    var targetURL = gl_target_server + "/php/vilsnodes.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            var data = JSON.parse(response);
            $.each(data, function(index, user)
            {
                gl_searchusers.push(user);
            });
            update_search_user_table(recordid);
        }
    });
    return false;
}

function update_search_user_table(recordid)
{
    clearChildView('search_user_text_dropdown_'+recordid);
    var search_user_text_dropdown = document.getElementById('search_user_text_dropdown_'+recordid);

    if (search_user_text_dropdown == undefined)
        return;

    // console.log("update_search_user_table() gl_searchusers.length:"+gl_searchusers.length);

    for (var i = 0; i < gl_searchusers.length; ++i)
    {
        var user = gl_searchusers[i];
        search_user_text_dropdown.appendChild( create_user_item(i+1, recordid, user) );
    }

    if (gl_searchusers.length > 0)
    {
        if ($('#search_user_text_dropdown_'+recordid).is(":hidden"))
        {
            $('#search_user_text_dropdown_'+recordid).toggle();
        }
    }
}

function create_user_item(index, recordid, user)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", user.userid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","searchresult_user_click(\""+ recordid +"\", \""+ user.userid +"\", \""+ user.name +"\");return true;");

    item_click.appendChild(document.createTextNode(" "+user.name));
    item_li.appendChild(item_click);

    return item_li;
}

function searchresult_user_click(recordid, userid, username)
{
    console.log("searchresult_user_click() recordid:"+recordid+" userid:"+userid);
    $('#search_user_text_dropdown_'+recordid).toggle();

    var assignusers_ui = document.getElementById('assignusers_'+recordid);

    if (assignusers_ui == undefined)
        return;

    var user_div = create_clickable_user(recordid, userid, username);
    assignusers_ui.appendChild(user_div);

}

function create_clickable_user(recordid, userid, username)
{
    var user_div = document.createElement('div');
    user_div.setAttribute("id",'assign_user_'+recordid+'_'+userid);

    var user_span = document.createElement('span');
    user_span.setAttribute("class",'badge bg-aqua-active');
    user_span.textContent = username;
    user_div.appendChild(user_span);

    // remove button
    var remove_span = document.createElement('span');
    remove_span.setAttribute("class",'badge bg-red');
    remove_span.textContent = '移除';
    remove_span.setAttribute("onclick","remove_user_click(\""+ recordid +"\", \""+ userid +"\");return true;");
    user_div.appendChild(remove_span);

    return user_div;
}

function remove_user_click(recordid, userid)
{
    var ui_item = document.getElementById('assign_user_'+recordid+'_'+userid);
    if ( ui_item != null )
    {
        ui_item.parentElement.removeChild(ui_item);
    }
}

function edit_assist_config()
{
    var config_sendNotifyTime = document.getElementById("config_sendNotifyTime");
    var editconfigbtn = document.getElementById("editconfigbtn");
    var editconfigimg = document.getElementById("editconfigimg");

    var sendNotifyTime = config_sendNotifyTime.textContent;

    clearChildView("config_sendNotifyTime");

    editconfigbtn.setAttribute("title","Save this Assist Config");
    editconfigbtn.setAttribute("onclick","save_assis_config();return false;");
    editconfigimg.setAttribute("class","fa fa-save");

    config_sendNotifyTime.appendChild(create_input_text("input_sendNotifyTime", sendNotifyTime));

}

function save_assis_config()
{
    var input_sendNotifyTime = document.getElementById("input_sendNotifyTime");
    var sendNotifyTime = input_sendNotifyTime.value;

    if ( sendNotifyTime.length == 0 )
    {
        alert("必須輸入 提前通知時間");
        return;
    }

    console.log("save_assis_configt() sendNotifyTime:["+sendNotifyTime+"]");

    if ( isNumeric(sendNotifyTime) == false )
    {
        alert("提前通知時間, 必須是數字");
        return;
    }
    //
    // clearChildView("config_sendNotifyTime");
    // var editconfigbtn = document.getElementById("editconfigbtn");
    // var editconfigimg = document.getElementById("editconfigimg");
    //
    // editconfigbtn.setAttribute("title","Edit this Assist Config");
    // editconfigbtn.setAttribute("onclick","edit_assist_config();return false;");
    // editconfigimg.setAttribute("class","fa fa-edit");

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_assistconfig'] = 1;
    jsondata['sendNotifyTime'] = sendNotifyTime;

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

// function getUser(useraccount)
// {
//     var retuser = null;
//     for (var i = 0; i < gl_user_info.length; ++i)
//     {
//         var user = gl_user_info[i];
//
//         if ( user.account === useraccount )
//         {
//             retuser = user;
//             break;
//         }
//     }
//     return retuser;
// }

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

function open_assist_page()
{
    var win = window.open('/index.php/vilsassist/'+project_id, '_blank');
    win.focus();
}
