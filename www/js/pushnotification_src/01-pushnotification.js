/*
 * Author: ikki Chung
 * Date: 2019 10 16
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var LOAD_DEVICE_NUM = 20;
var UPDATE_INTERVAL = 1000; // update display every 1 seconds

var NODE_TYPE_TAG = 1;
var NODE_TYPE_LOCATOR = 3;
var NODE_TYPE_SUBGROUP = 4;
var NODE_TYPE_ROLE = 5;
var NODE_TYPE_USER = 6;

var NODE_TYPE_TAG_SEARCH = 101;
var NODE_TYPE_LOCATOR_SEARCH = 103;
var NODE_TYPE_USER_SEARCH = 106;

var gl_tag_num = 0;
var gl_locator_num = 0;
//var gl_user_num = 0;

var gl_tag_load_index = 0;
var gl_coord_load_index = 0;
//var gl_user_load_index = 0;

var glTag_List = [];
var glLocator_List = [];
var glUser_List = [];
//var glSubGroup_List = [];
var gl_usersubgroups = [];
var gl_userdatasubgroups = [];

var glSurvey_List = [];
var glSendSurveyCount = 0;

var glSend_List = [];

var glCannedMessages_type = 205;// 手動群發 -> 不分類
var glCannedMessages = [];
var gl_uploadimage_uid = '';

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
                load_roles();
                load_surveys();
                load_devices();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_usersubgroup();
                load_subgroup_users();

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_canned_message();
                check_cookies();

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                if ( send_nodeid != null )
                {
                    click_tag(send_nodeid);

                    var inputTitle = document.getElementById("inputTitle");
                    if ( send_title != null )
                        inputTitle.value = send_title;

                    var inputContext = document.getElementById("inputContext");
                    if ( send_message != null )
                        inputContext.value = send_message;
                }
                else
                {
                    click_tag("");
                }

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

function load_devices()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        gl_tag_load_index = 0;
        gl_locator_load_index = 0;

        glTag_List = [];
        glLocator_List = [];
    })
    .success(function() {
        setTimeout(load_tag, 10);
        setTimeout(load_locator, 20);
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_tag()
{
    if (gl_tag_load_index >= gl_tag_num)
    {
        gl_tag_load_index = 0;

        return;
    }
    if (gl_tag_load_index == 0)
    {
        glTag_List = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadtag=1&s='+gl_tag_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var tag_num = parseInt(data['LOC_TAG_NUM']);

        console.log("tag_num:"+tag_num +", gl_tag_load_index:"+gl_tag_load_index);

        for (var i = gl_tag_load_index; i < gl_tag_load_index+tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            var node = data[key];
            var nodeData = node.split(",");
            glTag_List.push(nodeData);
        }

        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            add_tag_to_table();
        }
        else
        {
            setTimeout(load_tag, 1);
        }
    })
    .error(function() {
        gl_tag_load_index = 0;
        setTimeout(load_tag, UPDATE_INTERVAL);
    })
    .complete(function() {
    });
}

function add_tag_to_table()
{
    var tag_list_content = document.getElementById('tag_list_content');

    if (tag_list_content == null)
    {
        return;
    }

    while (tag_list_content.rows.length >= 1)
    {
        tag_list_content.deleteRow(0);
    }

    //console.log("add_tag_to_table() glTag_List.length:"+glTag_List.length);

    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var table_tr = document.createElement('tr');
        tag_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        //table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        table_tr.appendChild(table_td_1);

        var table_td_checkbox = document.createElement('td');
        table_tr.appendChild(table_td_checkbox);

        var ui_checkbox = document.createElement('input');
        ui_checkbox.setAttribute("id","id_row_checkbox_"+ node_nodeid);
        ui_checkbox.setAttribute("type",'checkbox');
        ui_checkbox.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return true;");
        table_td_checkbox.appendChild(ui_checkbox);

        var table_td_nodename = document.createElement('td');
        //table_td_nodename.setAttribute("id","id_row_node_name_"+ node_nodeid);
        table_tr.appendChild(table_td_nodename);

        // node name
        var tag_nodeid_span = document.createElement('span');
        tag_nodeid_span.setAttribute("class",'badge bg-green');
        tag_nodeid_span.textContent = node_name;
        tag_nodeid_span.setAttribute("onclick","checkbox_tag_click(\""+ node_nodeid +"\");return true;");
        table_td_nodename.appendChild(tag_nodeid_span);
    }
}

function load_locator()
{
    if (gl_locator_load_index >= gl_locator_num)
    {
        gl_locator_load_index = 0;
        return;
    }
    if (gl_locator_load_index == 0)
    {
        glLocator_List = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadlocator=1&s='+gl_locator_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        console.log("locator_num:"+locator_num +", gl_locator_load_index:"+gl_locator_load_index);

        for (var i = gl_locator_load_index; i < gl_locator_load_index+locator_num; ++i)
        {
            var key = 'LOC_LOCATOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            var node = data[key];
            var nodeData = node.split(",");
            glLocator_List.push(nodeData);
        }

        gl_locator_load_index += locator_num;

    })
    .success(function() {
        if (gl_locator_load_index >= gl_locator_num)
        {
            add_locator_to_table();
        }
        else
        {
            setTimeout(load_locator, 1);
        }
    })
    .error(function() {
        gl_locator_load_index = 0;
        setTimeout(load_locator, UPDATE_INTERVAL);
    })
    .complete(function() {
    });
}

function add_locator_to_table()
{
    var locator_list_content = document.getElementById('locator_list_content');

    if (locator_list_content == null)
    {
        return;
    }

    while (locator_list_content.rows.length >= 1)
    {
        locator_list_content.deleteRow(0);
    }

    console.log("add_locator_to_table() glLocator_List.length:"+glLocator_List.length);

    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var table_tr = document.createElement('tr');
        locator_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        //table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        // table_td_1.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_1);

        var table_td_checkbox = document.createElement('td');
        table_tr.appendChild(table_td_checkbox);

        var ui_checkbox = document.createElement('input');
        ui_checkbox.setAttribute("id","id_row_checkbox_"+ node_nodeid);
        ui_checkbox.setAttribute("type",'checkbox');
        ui_checkbox.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return true;");
        table_td_checkbox.appendChild(ui_checkbox);

        var table_td_nodename = document.createElement('td');
        //table_td_nodename.setAttribute("id","id_row_node_name_"+ node_nodeid);
        // table_td_nodename.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_nodename);

        // node name
        var locator_nodeid_span = document.createElement('span');
        locator_nodeid_span.setAttribute("class",'badge bg-yellow');
        locator_nodeid_span.textContent = node_name;
        locator_nodeid_span.setAttribute("onclick","checkbox_locator_click(\""+ node_nodeid +"\");return true;");
        table_td_nodename.appendChild(locator_nodeid_span);
    }
}

function load_roles()
{
    var role_list_content = document.getElementById('role_list_content');

    if (role_list_content == null)
    {
        return;
    }

    while (role_list_content.rows.length >= 1)
    {
        role_list_content.deleteRow(0);
    }

    role_list_content.appendChild( create_role_item(1, '0') );
    role_list_content.appendChild( create_role_item(2, '1') );
    role_list_content.appendChild( create_role_item(3, '2') );
    role_list_content.appendChild( create_role_item(4, '3') );
    role_list_content.appendChild( create_role_item(5, '4') );
    role_list_content.appendChild( create_role_item(6, '5') );
    role_list_content.appendChild( create_role_item(7, '6') );

}

function create_role_item(index, role, rolename)
{
    var role_list_content = document.getElementById('role_list_content');

    var rolename = get_role_name(role);

    var table_tr = document.createElement('tr');
    role_list_content.appendChild(table_tr);

    var table_td_1 = document.createElement('td');
    table_td_1.textContent = index;
    table_td_1.setAttribute("id","role_row_"+ role);
    table_tr.appendChild(table_td_1);

    var table_td_checkbox = document.createElement('td');
    table_tr.appendChild(table_td_checkbox);

    var ui_checkbox = document.createElement('input');
    ui_checkbox.setAttribute("id","role_row_checkbox_"+ role);
    ui_checkbox.setAttribute("type",'checkbox');
    ui_checkbox.setAttribute("onclick","click_role(\""+ role +"\");return true;");
    table_td_checkbox.appendChild(ui_checkbox);

    var table_td_rolename = document.createElement('td');
    table_td_rolename.setAttribute("id","role_row_name_"+ role);
    table_tr.appendChild(table_td_rolename);

    // node name
    var rolename_span = document.createElement('span');
    rolename_span.setAttribute("class",'badge bg-purple');
    rolename_span.textContent = rolename;
    rolename_span.setAttribute("onclick","checkbox_role_click(\""+ role +"\");return true;");
    table_td_rolename.appendChild(rolename_span);

    return table_tr;
}

function load_surveys()
{
    var targetURL = gl_target_server + "/php/survey.php";

    glSurvey_List = [];

    $.getJSON(targetURL, 'loadsurveys=1'+'&project_id='+project_id+'&accountid='+gl_accountid, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            glSurvey_List.push(record);
        });
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
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
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaddatasubgroup=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, group)
        {
            gl_userdatasubgroups.push(group);
        });
    })
    .success(function() {
        add_subgroup_to_table();
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function add_subgroup_to_table()
{
    var subgroup_list_content = document.getElementById('subgroup_list_content');

    while (subgroup_list_content.rows.length >= 1)
    {
        subgroup_list_content.deleteRow(0);
    }

    console.log("add_subgroup_to_table() gl_userdatasubgroups.length:"+gl_userdatasubgroups.length);

    for (var i = 0; i < gl_userdatasubgroups.length; i++)
    {
        var subgroup = gl_userdatasubgroups[i];
        subgroup_list_content.appendChild( create_subgroup_item(i+1, subgroup) );
    }
}

function create_subgroup_item(index, subgroup)
{
    var subgroup_list_content = document.getElementById('subgroup_list_content');

    var table_tr = document.createElement('tr');
    subgroup_list_content.appendChild(table_tr);

    var table_td_1 = document.createElement('td');
    table_td_1.textContent = index;
    table_td_1.setAttribute("id","subgroup_row_"+ subgroup.subgroupid);
    table_tr.appendChild(table_td_1);

    var table_td_checkbox = document.createElement('td');
    table_tr.appendChild(table_td_checkbox);

    var ui_checkbox = document.createElement('input');
    ui_checkbox.setAttribute("id","subgroup_row_checkbox_"+ subgroup.subgroupid);
    ui_checkbox.setAttribute("type",'checkbox');
    ui_checkbox.setAttribute("onclick","click_subgroup(\""+ subgroup.subgroupid +"\");return true;");
    table_td_checkbox.appendChild(ui_checkbox);

    var table_td_subgroupname = document.createElement('td');
    table_td_subgroupname.setAttribute("id","subgroup_row_name_"+ subgroup.subgroupid);
    table_tr.appendChild(table_td_subgroupname);

    // node name
    var subgroupname_span = create_subgroup_label(subgroup.subgroupname);
    subgroupname_span.setAttribute("onclick","checkbox_subgroup_click(\""+ subgroup.subgroupid +"\");return true;");
    table_td_subgroupname.appendChild(subgroupname_span);

    // var rolename_span = document.createElement('span');
    // rolename_span.setAttribute("class",'badge bg-green');
    // rolename_span.textContent = subgroup.subgroupname;
    // rolename_span.setAttribute("onclick","checkbox_subgroup_click(\""+ subgroup.subgroupid +"\");return true;");
    // table_td_rolename.appendChild(rolename_span);

    return table_tr;
}

function create_subgroup_label(context)
{
    var subgroup_span = document.createElement("span");
    subgroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    subgroup_span.textContent = context;
    return subgroup_span;
}

function load_subgroup_users()
{
    glUser_List = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadsubgroupuser=1&project_id='+project_id+'&accountid='+gl_accountid, function(data)
    {
        var index = 0;
        $.each(data, function(empid, user)
        {
            glUser_List.push(user);
        });
    })
    .success(function() {
        add_user_to_table();
    })
    .error(function() {
        setTimeout(load_subgroup_users, UPDATE_INTERVAL);
    })
    .complete(function() {
    });
}

function load_canned_message()
{
    glCannedMessages = [];

    var targetURL = gl_target_server + "/php/pushnotify.php";
    $.getJSON(targetURL, 'loadcannedmesages=1'+'&project_id='+project_id, function(data)
    {
        //console.log(data);

        $.each(data, function(index, message)
        {
            //console.log("index:"+index+" message:"+message);
            glCannedMessages.push(message);
        });
    })
    .success(function() {
        add_CannedMessages_to_table();
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function add_user_to_table()
{
    var user_list_content = document.getElementById('user_list_content');

    if (user_list_content == null)
    {
        return ;
    }

    while (user_list_content.rows.length >= 1)
    {
        user_list_content.deleteRow(0);
    }

    console.log("add_user_to_table() glUser_List.length:"+glUser_List.length);

    for (var i = 0; i < glUser_List.length; i++)
    {
        var user = glUser_List[i];
        user_list_content.appendChild( create_user_item(i+1, user) );
    }
}

function create_user_item(index, user)
{
    var user_list_content = document.getElementById('user_list_content');

    var table_tr = document.createElement('tr');
    user_list_content.appendChild(table_tr);

    var table_td_1 = document.createElement('td');
    table_td_1.textContent = index;
    table_tr.appendChild(table_td_1);

    var table_td_checkbox = document.createElement('td');
    table_tr.appendChild(table_td_checkbox);

    var ui_checkbox = document.createElement('input');
    ui_checkbox.setAttribute("id","user_row_checkbox_"+ user.userid);
    ui_checkbox.setAttribute("type",'checkbox');
    ui_checkbox.setAttribute("onclick","click_user(\""+ user.userid +"\");return true;");
    table_td_checkbox.appendChild(ui_checkbox);

    var table_td_rolename = document.createElement('td');
    table_tr.appendChild(table_td_rolename);

    // node name
    var rolename_span = document.createElement('span');
    rolename_span.setAttribute("class",'badge bg-aqua-active');
    rolename_span.textContent = user.name;
    rolename_span.setAttribute("onclick","checkbox_user_click(\""+ user.userid +"\");return true;");
    table_td_rolename.appendChild(rolename_span);

    // subgroup
    var table_td_subgroup = document.createElement('td');
    table_tr.appendChild(table_td_subgroup);

    var subgroups = user.subgroups.split(',');
    for(var i=0;i < subgroups.length;i++)
    {
        var subgroup_name = get_subgroup_name(subgroups[i]);
        if (subgroup_name.length > 0)
        {
            var subgroup_span = create_subgroup_label(subgroup_name);
            table_td_subgroup.appendChild(subgroup_span);
        }
    }

    return table_tr;
}

function add_CannedMessages_to_table()
{
    //console.log("add_CannedMessages_to_table() glCannedMessages.length:"+glCannedMessages.length);
    var cannedmessage_dropdownmenu = document.getElementById('cannedmessage_dropdownmenu');

    while (cannedmessage_dropdownmenu.firstChild)
    {
        cannedmessage_dropdownmenu.removeChild(cannedmessage_dropdownmenu.firstChild);
    }

    for (var i = 0; i < glCannedMessages.length; i++)
    {
        var message = glCannedMessages[i];
        //console.log("add_CannedMessages_to_table() i:"+i+" title:"+message.title);

        var li = document.createElement('li');
        cannedmessage_dropdownmenu.appendChild(li);

        var a = document.createElement('a');
        a.setAttribute("href","javascript:;");
        a.setAttribute("onclick","dropdown_msg_click("+i+");");
        a.textContent = message.menutitle;
        li.appendChild(a);

        //console.log("add_CannedMessages_to_table() message:"+message);
    }
    //console.log("add_CannedMessages_to_table() end");

}


// var glCheckbox_tag_click = false;
function checkbox_tag_click(click_node_nodeid)
{
    // console.log("checkbox_tag_click() click_node_nodeid:" + click_node_nodeid +" glCheckbox_tag_click:"+glCheckbox_tag_click);

    // glCheckbox_tag_click = true;

    var checkbox_ui = document.getElementById("id_row_checkbox_"+ click_node_nodeid);
    if (checkbox_ui == undefined)
    {
        console.log("click_tag() checkbox_ui == undefined");
        return;
    }

    var checked = checkbox_ui.checked;
    if ( checked )
        checkbox_ui.checked = false;
    else
        checkbox_ui.checked = true;

    update_send_list();
}

function click_tag(click_node_nodeid)
{
    // console.log("click_tag() click_node_nodeid:" + click_node_nodeid +" glCheckbox_tag_click:"+glCheckbox_tag_click);
/*
    var checkbox_ui = document.getElementById("id_row_checkbox_"+ click_node_nodeid);
    if (checkbox_ui == undefined)
    {
        console.log("click_tag() checkbox_ui == undefined");
        return;
    }

    //console.log("click_tag() 1 checkbox_ui.checked:"+checkbox_ui.checked);

    if (glCheckbox_tag_click)
    {
    }
    else
    {
        var checked = checkbox_ui.checked;
        if ( checked )
            checkbox_ui.checked = false;
        else
            checkbox_ui.checked = true;
    }

    //console.log("click_tag() 2 checkbox_ui.checked:"+checkbox_ui.checked);

    glCheckbox_tag_click = false;
*/
    update_send_list();

}

// var glCheckbox_locator_click = false;
function checkbox_locator_click(click_node_nodeid)
{
    //console.log("checkbox_click_tag() click_node_nodeid:" + click_node_nodeid);
    // glCheckbox_locator_click = true;
    var checkbox_ui = document.getElementById("id_row_checkbox_"+ click_node_nodeid);
    if (checkbox_ui == undefined)
    {
        console.log("click_locator() checkbox_ui == undefined");
        return;
    }

    var checked = checkbox_ui.checked;
    if ( checked )
        checkbox_ui.checked = false;
    else
        checkbox_ui.checked = true;

    update_send_list();
}

function click_locator(click_node_nodeid)
{
    //console.log("click_tag() click_node_nodeid:" + click_node_nodeid);

    // var checkbox_ui = document.getElementById("id_row_checkbox_"+ click_node_nodeid);
    // if (checkbox_ui == undefined)
    // {
    //     console.log("click_locator() checkbox_ui == undefined");
    //     return;
    // }
    //
    // if (!glCheckbox_locator_click)
    // {
    //     var checked = checkbox_ui.checked;
    //     if ( checked )
    //         checkbox_ui.checked = false;
    //     else
    //         checkbox_ui.checked = true;
    // }

    // glCheckbox_locator_click = false;

    update_send_list();
}

function tag_list_checkbox_click()
{
    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_nodeid = nodeData[3];

        document.getElementById("id_row_checkbox_"+ node_nodeid).checked = document.getElementById("tag_list_checkbox").checked;
    }

    update_send_list();
}

function locator_list_checkbox_click()
{
    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var node_nodeid = nodeData[3];

        document.getElementById("id_row_checkbox_"+ node_nodeid).checked = document.getElementById("locator_list_checkbox").checked;
    }

    update_send_list();
}

function role_list_checkbox_click()
{
    document.getElementById("role_row_checkbox_0").checked = document.getElementById("role_list_checkbox").checked;
    document.getElementById("role_row_checkbox_1").checked = document.getElementById("role_list_checkbox").checked;
    document.getElementById("role_row_checkbox_2").checked = document.getElementById("role_list_checkbox").checked;
    document.getElementById("role_row_checkbox_3").checked = document.getElementById("role_list_checkbox").checked;
    document.getElementById("role_row_checkbox_4").checked = document.getElementById("role_list_checkbox").checked;
    document.getElementById("role_row_checkbox_5").checked = document.getElementById("role_list_checkbox").checked;
    document.getElementById("role_row_checkbox_6").checked = document.getElementById("role_list_checkbox").checked;

    update_send_list();
}

function subgroup_list_checkbox_click()
{
    for (var i = 0; i < gl_userdatasubgroups.length; i++)
    {
        var subgroup = gl_userdatasubgroups[i];

        var id_row_checkbox = document.getElementById("subgroup_row_checkbox_"+ subgroup.subgroupid);

        id_row_checkbox.checked = document.getElementById("subgroup_list_checkbox").checked;
    }

    update_send_list();
}

function user_list_checkbox_click()
{
    for (var i = 0; i < glUser_List.length; i++)
    {
        var user = glUser_List[i];

        var id_row_checkbox = document.getElementById("user_row_checkbox_"+ user.userid);

        id_row_checkbox.checked = document.getElementById("user_list_checkbox").checked;
    }

    update_send_list();
}

function click_role(role)
{
    update_send_list();
}

function checkbox_role_click(role)
{
    var checkbox_ui = document.getElementById("role_row_checkbox_"+ role);
    if (checkbox_ui == undefined)
    {
        console.log("checkbox_role_click() checkbox_ui == undefined");
        return;
    }

    var checked = checkbox_ui.checked;
    if ( checked )
        checkbox_ui.checked = false;
    else
        checkbox_ui.checked = true;

    update_send_list();
}

function click_subgroup(subgroupid)
{
    update_send_list();
}

function checkbox_subgroup_click(subgroupid)
{
    var checkbox_ui = document.getElementById("subgroup_row_checkbox_"+ subgroupid);
    if (checkbox_ui == undefined)
    {
        console.log("checkbox_subgroup_click() checkbox_ui == undefined");
        return;
    }

    var checked = checkbox_ui.checked;
    if ( checked )
        checkbox_ui.checked = false;
    else
        checkbox_ui.checked = true;

    update_send_list();
}

function click_user(userid)
{
    update_send_list();
}

function checkbox_user_click(userid)
{
    var checkbox_ui = document.getElementById("user_row_checkbox_"+ userid);
    if (checkbox_ui == undefined)
    {
        console.log("checkbox_user_click() checkbox_ui == undefined");
        return;
    }

    var checked = checkbox_ui.checked;
    if ( checked )
        checkbox_ui.checked = false;
    else
        checkbox_ui.checked = true;

    update_send_list();
}

function update_send_list()
{
    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_nodeid = nodeData[3];
        var role = nodeData[4];
        var node_name = nodeData[5];

        var id_row_checkbox = document.getElementById("id_row_checkbox_"+ node_nodeid);
        if (id_row_checkbox != null)
        {
            if ( id_row_checkbox.checked )
            {
                //add_send_node(send_node_list_ui, node_name, NODE_TYPE_TAG);
                add_send_node_list(node_nodeid, node_name, NODE_TYPE_TAG);
            }
            else
            {
                delete_send_node_list(node_nodeid, node_name, NODE_TYPE_TAG);
            }
        }

        var role_row_checkbox = document.getElementById("role_row_checkbox_"+ role);
        if (role_row_checkbox != null)
        {
            if ( role_row_checkbox.checked )
            {
                //add_send_node(send_node_list_ui, node_name, NODE_TYPE_ROLE);
                add_send_node_list(node_nodeid, node_name, NODE_TYPE_ROLE);
            }
            else
            {
                delete_send_node_list(node_nodeid, node_name, NODE_TYPE_ROLE);

                var search_target_checkbox = document.getElementById("search_target_checkbox_"+ node_nodeid);
                if ( search_target_checkbox != undefined )
                {
                    if ( search_target_checkbox.checked )
                    {
                        //add_send_node(send_node_list_ui, node_name, NODE_TYPE_TAG);
                        add_send_node_list(node_nodeid, node_name, NODE_TYPE_TAG_SEARCH);
                    }
                    else
                    {
                        delete_send_node_list(node_nodeid, node_name, NODE_TYPE_TAG_SEARCH);
                    }
                }
            }
        }
    }

    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var node_nodeid = nodeData[3];
        var node_name = nodeData[5];

        var id_row_checkbox = document.getElementById("id_row_checkbox_"+ node_nodeid);
        if (id_row_checkbox != null)
        {
            if ( id_row_checkbox.checked)
            {
                //add_send_node(send_node_list_ui, node_name, NODE_TYPE_LOCATOR);
                add_send_node_list(node_nodeid, node_name, NODE_TYPE_LOCATOR);
            }
            else
            {
                delete_send_node_list(node_nodeid, node_name, NODE_TYPE_LOCATOR);

                var search_target_checkbox = document.getElementById("search_target_checkbox_"+ node_nodeid);
                if ( search_target_checkbox != undefined )
                {
                    if ( search_target_checkbox.checked )
                    {
                        //add_send_node(send_node_list_ui, node_name, NODE_TYPE_LOCATOR);
                        add_send_node_list(node_nodeid, node_name, NODE_TYPE_LOCATOR_SEARCH);
                    }
                    else
                    {
                        delete_send_node_list(node_nodeid, node_name, NODE_TYPE_LOCATOR_SEARCH);
                    }
                }
            }
        }
    }

    for (var i = 0; i < gl_userdatasubgroups.length; i++)
    {
        var subgroup = gl_userdatasubgroups[i];

        var id_row_checkbox = document.getElementById("subgroup_row_checkbox_"+ subgroup.subgroupid);
        if (id_row_checkbox != null)
        {
            if ( id_row_checkbox.checked)
            {
                add_send_node_list(subgroup.subgroupid, subgroup.subgroupname, NODE_TYPE_SUBGROUP);
            }
            else
            {
                delete_send_node_list(subgroup.subgroupid, subgroup.subgroupname, NODE_TYPE_SUBGROUP);
            }
        }
    }

    for (var i = 0; i < glUser_List.length; i++)
    {
        var user = glUser_List[i];

        var id_row_checkbox = document.getElementById("user_row_checkbox_"+ user.userid);
        if (id_row_checkbox != null)
        {
            if ( id_row_checkbox.checked)
            {
                add_send_node_list(user.userid, user.name, NODE_TYPE_USER);
            }
            else
            {
                delete_send_node_list(user.userid, user.name, NODE_TYPE_USER);
// 123
                var search_target_checkbox = document.getElementById("search_target_checkbox_"+ user.userid);
                if ( search_target_checkbox != undefined )
                {
                    if ( search_target_checkbox.checked )
                    {
                        add_send_node_list(user.userid, user.name, NODE_TYPE_USER_SEARCH);
                    }
                    else
                    {
                        delete_send_node_list(user.userid, user.name, NODE_TYPE_USER_SEARCH);
                    }
                }

            }
        }
    }

    update_send_list_ui();
}

function add_send_node_list(nodeid, nodename, nodetype)
{
    var bFound = false;
    for (var i = 0; i < glSend_List.length; i++)
    {
        var send_node = glSend_List[i];
        if ( (send_node.nodeid == nodeid) && (send_node.node_name == nodename) &&(send_node.node_type == nodetype) )
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        var send_node = {};
        send_node.nodeid = nodeid;
        send_node.node_name = nodename;
        send_node.node_type = nodetype;
        glSend_List.push(send_node);
    }
}

function delete_send_node_list(nodeid, nodename, nodetype)
{
    for (var i = 0; i < glSend_List.length; i++)
    {
        var send_node = glSend_List[i];
        if ( (send_node.nodeid == nodeid) && (send_node.node_name == nodename) &&(send_node.node_type == nodetype) )
        {
            glSend_List.splice(i, 1);
            break;
        }
    }
}

function update_send_list_ui()
{
    var send_node_list_ui = document.getElementById("send_node_list_ui");
    while (send_node_list_ui.firstChild)
    {
        send_node_list_ui.removeChild(send_node_list_ui.firstChild);
    }

    for (var i = 0; i < glSend_List.length; i++)
    {
        var send_node = glSend_List[i];
        add_send_node(send_node_list_ui, send_node.node_name, send_node.node_type);
    }
}

function add_send_node(send_node_list_ui, node_name, nodetype)
{
    var nodeid_span = document.createElement('span');
    if (nodetype == NODE_TYPE_TAG)
    {
        nodeid_span.setAttribute("class",'badge bg-green');
        nodeid_span.textContent = node_name;
    }
    else
    if (nodetype == NODE_TYPE_TAG_SEARCH)
    {
        nodeid_span.setAttribute("class",'badge bg-green');
        nodeid_span.textContent = node_name;
    }
    else
    if (nodetype == NODE_TYPE_LOCATOR)
    {
        nodeid_span.setAttribute("class",'badge bg-yellow');
        nodeid_span.textContent = node_name;
    }
    else
    if (nodetype == NODE_TYPE_LOCATOR_SEARCH)
    {
        nodeid_span.setAttribute("class",'badge bg-yellow');
        nodeid_span.textContent = node_name;
    }
    else
    if (nodetype == NODE_TYPE_ROLE)
    {
        nodeid_span.setAttribute("class",'badge bg-purple');
        nodeid_span.textContent = node_name;
    }
    else
    if (nodetype == NODE_TYPE_SUBGROUP)
    {
        nodeid_span = create_subgroup_label(node_name);
    }
    else
    if (nodetype == NODE_TYPE_USER)
    {
        nodeid_span.setAttribute("class",'badge bg-aqua-active');
        nodeid_span.textContent = node_name;
    }
    else
    if (nodetype == NODE_TYPE_USER_SEARCH)
    {
        nodeid_span.setAttribute("class",'badge bg-aqua-active');
        nodeid_span.textContent = node_name;
    }

    send_node_list_ui.appendChild(nodeid_span);
}

$("#sendnotificationbtn").click(function()
{
    var send_title = document.getElementById("inputTitle").value;
    var send_context = document.getElementById("inputContext").value;

    if ( send_title.length == 0 )
    {
        alert("請輸入推播標題!");
        return false;
    }

    if ( send_context.length == 0 )
    {
        alert("請輸入推播內容!");
        return false;
    }

    var target_list = [];
    var subgroup_list = [];

    for (var i = 0; i < glSend_List.length; i++)
    {
        var send_node = glSend_List[i];
        if (send_node.node_type == NODE_TYPE_SUBGROUP)
            subgroup_list.push(send_node.nodeid);
        else
        if (send_node.node_type == NODE_TYPE_USER)
            target_list.push(send_node.nodeid);
        else
            target_list.push(send_node.nodeid);
    }

    if ( (target_list.length == 0) && (subgroup_list.length == 0) )
    {
        alert("請選擇推播對象!");
        return false;
    }

    check_upload_image();

    return false;
});

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
        send_notification();
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

    var imageDescription = document.getElementById("imageDescription").value;
    var file = document.getElementById('imageUpload').files[0];
    var filename = file.name.split(".");
    var new_file_name = filename[0];
    var new_file_description = imageDescription;
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
    SendData += "&leaveform=0";

    return jQuery.ajax({
     url: '/php/uploadfile.php',
     type: 'POST',
     data: SendData,
     dataType: 'text',
      success:
        function(response)
        {
            setTimeout(send_notification, 300);
        },
      error:
        function(xhr, status, error)
        {
            setTimeout(send_notification, 300);
        }
    });
}

function send_notification()
{
    var send_title = document.getElementById("inputTitle").value;
    var send_context = document.getElementById("inputContext").value;

    var target_list = [];
    var subgroup_list = [];

    for (var i = 0; i < glSend_List.length; i++)
    {
        var send_node = glSend_List[i];
        if (send_node.node_type == NODE_TYPE_SUBGROUP)
            subgroup_list.push(send_node.nodeid);
        else
        if (send_node.node_type == NODE_TYPE_USER)
            target_list.push(send_node.nodeid);
        else
            target_list.push(send_node.nodeid);
    }

    var survey_list = get_send_surveys();
    var deadline_list = get_send_surveys_deadline();

    console.log("survey_list:" + survey_list);
    console.log("deadline_list:" + deadline_list);

    document.getElementById("sendnotification_sending_callout").style.display = 'block';
    document.getElementById("sendnotification_failed_callout").style.display = 'none';
    document.getElementById("sendnotification_finished_callout").style.display = 'none';

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['sendmultinotification'] = '1';
    jsondata['subgrouplist'] = subgroup_list;
    jsondata['targetlist'] = target_list;
    jsondata['surveylist'] = survey_list;
    jsondata['deadlinelist'] = deadline_list;
    //jsondata['accountid'] = gl_accountid;
    jsondata['title'] = send_title;
    jsondata['context'] = send_context;
    jsondata['type'] = glCannedMessages_type;
    jsondata['imageuid'] = gl_uploadimage_uid;

    document.getElementById("sendnotificationbtn").disabled = true;
    var targetURL = gl_target_server + "/php/pushnotify.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
            // console.log("response:" + response);

            var status = JSON.parse(response);
            // console.log("status:" + status);

            setTimeout(wait_pushnotify_finish, 1, status.waitkey);

        },
      error:
        function(xhr, status, error)
        {
            console.log("error = " + error);
            console.log("xhr.responseText = " + xhr.responseText);

            document.getElementById("sendnotification_sending_callout").style.display = 'none';
            document.getElementById("sendnotification_failed_callout").style.display = 'block';
            document.getElementById("sendnotification_finished_callout").style.display = 'none';

            setTimeout(close_callout, 5000, "sendnotification_failed_callout");
            document.getElementById("sendnotificationbtn").disabled = false;
        }
    });

}

function close_callout(ui_id)
{
    document.getElementById(ui_id).style.display = 'none';
}

function wait_pushnotify_finish(waitkey)
{
    var targetURL = gl_target_server + "/php/pushnotify.php";
    $.getJSON(targetURL, 'waitpushnotify=1'+'&project_id='+project_id+'&waitkey='+waitkey, function(data)
    {

        if ( data.success == undefined )
        {
            setTimeout(wait_pushnotify_finish, 1000, waitkey);
        }
        else
        if ( data.success === 'true')
        {
            document.getElementById("sendnotification_sending_callout").style.display = 'none';
            document.getElementById("sendnotification_failed_callout").style.display = 'none';
            document.getElementById("sendnotification_finished_callout").style.display = 'block';
            setTimeout(close_callout, 5000, "sendnotification_finished_callout");
            document.getElementById("sendnotificationbtn").disabled = false;
        }
        else
        if ( data.success === 'false')
        {

            document.getElementById("sendnotification_failed_callout_msg").innerHTML = "<p>發出推播 失敗! "+data.msg+"</p>"

            document.getElementById("sendnotification_sending_callout").style.display = 'none';
            document.getElementById("sendnotification_failed_callout").style.display = 'block';
            document.getElementById("sendnotification_finished_callout").style.display = 'none';
            setTimeout(close_callout, 10000, "sendnotification_failed_callout");
            document.getElementById("sendnotificationbtn").disabled = false;
        }
        else
        if ( data.success === 'wait')
        {
            setTimeout(wait_pushnotify_finish, 1000, waitkey);
        }

    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function dropdown_msg_click(msg_index)
{
    // console.log("dropdown_msg_click() msg_index:"+msg_index);

    var showMessage = glCannedMessages[msg_index];

    glCannedMessages_type = showMessage.type;

    var cannedmessage_type_title = document.getElementById("cannedmessage_type_title");
    cannedmessage_type_title.textContent = showMessage.menutitle;

    var inputTitle = document.getElementById("inputTitle");
    inputTitle.value = showMessage.title;

    var inputContext = document.getElementById("inputContext");
    inputContext.value = showMessage.message;
}

function get_role_name(role)
{
    var role_name = "未設定";
    if (role == '0')
        role_name = "人員訪客";
    else
    if (role == '1')
        role_name = "巡檢員";
    else
    if (role == '2')
        role_name = "設備";
    else
    if (role == '3')
        role_name = "VIP貴賓";
    else
    if (role == '4')
        role_name = "工作人員";
    else
    if (role == '5')
        role_name = "醫師";
    else
    if (role == '6')
        role_name = "護理師";
    return role_name;
}

function checkbox_target_click(click_node_nodeid)
{
    var checkbox_ui = document.getElementById("search_target_checkbox_"+ click_node_nodeid);
    if (checkbox_ui == undefined)
    {
        console.log("checkbox_target_click() checkbox_ui == undefined");
        return;
    }

    var checked = checkbox_ui.checked;
    if ( checked )
        checkbox_ui.checked = false;
    else
        checkbox_ui.checked = true;

    update_send_list();
}

function click_target(click_node_nodeid)
{
    update_send_list();
}

function add_search_target_to_table(searchtext)
{
    if ( searchtext == null )
        return;

    var search_list_content = document.getElementById('search_list_content');

    while (search_list_content.rows.length >= 1)
    {
        search_list_content.deleteRow(0);
    }

    var searchtext_lc = searchtext.toLowerCase();

    //console.log("add_tag_to_table() glTag_List.length:"+glTag_List.length);
    var index = 1;
    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var targetname = node_name.toLowerCase();
        var targetid = node_nodeid.toLowerCase();

        var ret = targetname.indexOf(searchtext_lc);
        if ( ret < 0 )
        {
            ret = targetid.indexOf(searchtext_lc);
            if ( ret < 0 )
            {
                continue;
            }
        }

        var existed = check_searchresult_exist(node_nodeid, node_name, NODE_TYPE_TAG_SEARCH);

        var table_tr = document.createElement('tr');
        search_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = index;
        //table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        table_tr.appendChild(table_td_1);

        var table_td_checkbox = document.createElement('td');
        table_tr.appendChild(table_td_checkbox);

        var ui_checkbox = document.createElement('input');
        ui_checkbox.setAttribute("id","search_target_checkbox_"+ node_nodeid);
        ui_checkbox.setAttribute("type",'checkbox');
        ui_checkbox.setAttribute("onclick","click_target(\""+ node_nodeid +"\");return true;");
        if (existed)
        {
            ui_checkbox.setAttribute("checked",'checked');
        }
        table_td_checkbox.appendChild(ui_checkbox);

        var table_td_nodename = document.createElement('td');
        table_tr.appendChild(table_td_nodename);

        // node name
        var tag_nodeid_span = document.createElement('span');
        tag_nodeid_span.setAttribute("class",'badge bg-green');
        tag_nodeid_span.textContent = node_name;
        tag_nodeid_span.setAttribute("onclick","checkbox_target_click(\""+ node_nodeid +"\");return true;");
        table_td_nodename.appendChild(tag_nodeid_span);

        index++;
    }

    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var targetname = node_name.toLowerCase();
        var targetid = node_nodeid.toLowerCase();

        var ret = targetname.indexOf(searchtext_lc);
        if ( ret < 0 )
        {
            ret = targetid.indexOf(searchtext_lc);
            if ( ret < 0 )
            {
                continue;
            }
        }

        var existed = check_searchresult_exist(node_nodeid, node_name, NODE_TYPE_LOCATOR_SEARCH);

        var table_tr = document.createElement('tr');
        search_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = index;
        //table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        // table_td_1.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_1);

        var table_td_checkbox = document.createElement('td');
        table_tr.appendChild(table_td_checkbox);

        var ui_checkbox = document.createElement('input');
        ui_checkbox.setAttribute("id","search_target_checkbox_"+ node_nodeid);
        ui_checkbox.setAttribute("type",'checkbox');
        ui_checkbox.setAttribute("onclick","click_target(\""+ node_nodeid +"\");return true;");
        if (existed)
        {
            ui_checkbox.setAttribute("checked",'checked');
        }
        table_td_checkbox.appendChild(ui_checkbox);

        var table_td_nodename = document.createElement('td');
        //table_td_nodename.setAttribute("id","id_row_node_name_"+ node_nodeid);
        // table_td_nodename.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_nodename);

        // node name
        var locator_nodeid_span = document.createElement('span');
        locator_nodeid_span.setAttribute("class",'badge bg-yellow');
        locator_nodeid_span.textContent = node_name;
        locator_nodeid_span.setAttribute("onclick","checkbox_target_click(\""+ node_nodeid +"\");return true;");
        table_td_nodename.appendChild(locator_nodeid_span);

        index++;
    }

    for (var i = 0; i < glUser_List.length; i++)
    {
        var user = glUser_List[i];
        var userid = user.userid;
        var useraccount = user.account;
        var username = user.name;

        var targetaccount = useraccount.toLowerCase();
        var targetname = username.toLowerCase();

        var ret = targetname.indexOf(searchtext_lc);
        if ( ret < 0 )
        {
            ret = targetaccount.indexOf(searchtext_lc);
            if ( ret < 0 )
            {
                continue;
            }
        }

        var existed = check_searchresult_exist(useraccount, username, NODE_TYPE_USER_SEARCH);

        var table_tr = document.createElement('tr');
        search_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = index;
        //table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        table_tr.appendChild(table_td_1);

        var table_td_checkbox = document.createElement('td');
        table_tr.appendChild(table_td_checkbox);

        var ui_checkbox = document.createElement('input');
        ui_checkbox.setAttribute("id","search_target_checkbox_"+ userid);
        ui_checkbox.setAttribute("type",'checkbox');
        ui_checkbox.setAttribute("onclick","click_target(\""+ userid +"\");return true;");
        if (existed)
        {
            ui_checkbox.setAttribute("checked",'checked');
        }
        table_td_checkbox.appendChild(ui_checkbox);

        var table_td_nodename = document.createElement('td');
        table_tr.appendChild(table_td_nodename);

        // node name
        var tag_nodeid_span = document.createElement('span');
        tag_nodeid_span.setAttribute("class",'badge bg-aqua-active');
        tag_nodeid_span.textContent = username;
        tag_nodeid_span.setAttribute("onclick","checkbox_target_click(\""+ userid +"\");return true;");
        table_td_nodename.appendChild(tag_nodeid_span);

        index++;
    }

}

function check_searchresult_exist(nodeid, nodename, nodetype)
{
    var bFound = false;
    for (var i = 0; i < glSend_List.length; i++)
    {
        var send_node = glSend_List[i];
        if ( (send_node.nodeid == nodeid) && (send_node.node_name == nodename) &&(send_node.node_type == nodetype) )
        {
            bFound = true;
            break;
        }
    }
    return bFound;
}

function create_survey_click()
{
    var show_index = glSendSurveyCount;
    if (show_index >= glSurvey_List.length)
        show_index = glSurvey_List.length - 1;

    var survey = glSurvey_List[show_index];

    var survey_tablebody = document.getElementById("survey_table_body");

    var survey_tr = document.createElement('tr');
    survey_tr.setAttribute("id","survey_row_"+glSendSurveyCount);

    var survey_td_num = document.createElement('td');
    survey_td_num.setAttribute("id","survey_row_index_"+glSendSurveyCount);
    survey_tr.appendChild(survey_td_num);
    survey_td_num.appendChild(document.createTextNode(glSendSurveyCount+1));

    var survey_td_dropdown = document.createElement('td');
    survey_tr.appendChild(survey_td_dropdown);

    //==================
    var survey_info = create_survey_info(glSendSurveyCount, show_index);
    if( survey_info == null )
    {
        alert("無待辦事項, 請新增待辦事項.");
        return ;
    }

    survey_tablebody.appendChild(survey_tr);
    survey_td_dropdown.appendChild(survey_info);
    //==================
    //var survey_dropdown = create_survey_dropdown('survey_dropdown_'+glSendSurveyCount, show_index);
    //survey_td_dropdown.appendChild(survey_dropdown);
    //==================

    var survey_td_type = document.createElement('td');
    survey_td_type.setAttribute("id","survey_type_"+glSendSurveyCount);
    survey_tr.appendChild(survey_td_type);
    if ( survey.type === '0' )
        survey_td_type.appendChild(document.createTextNode("單選"));
    if ( survey.type === '1' )
        survey_td_type.appendChild(document.createTextNode("複選"));

    var survey_td_item = document.createElement('td');
    survey_td_item.setAttribute("id","survey_item_"+glSendSurveyCount);
    survey_tr.appendChild(survey_td_item);

    var survey_items = show_survey_item(show_index);
    survey_td_item.appendChild(survey_items);

    var survey_td_operation = document.createElement('td');
    survey_tr.appendChild(survey_td_operation);

    var survey_bthgroup = document.createElement('div');
    survey_bthgroup.setAttribute("class","btn-group");
    survey_td_operation.appendChild(survey_bthgroup);

    var survey_button = document.createElement('button');
    survey_button.setAttribute("class","btn btn-block btn-danger");
    survey_button.setAttribute("onclick","delete_survey_item_click("+glSendSurveyCount+");return false;");
    survey_bthgroup.appendChild(survey_button);

    var survey_img = document.createElement('i');
    survey_img.setAttribute("class","fa fa-minus");
    survey_button.appendChild(survey_img);

    var today = moment();
    var deadline = moment(today).add(1, 'days');

    $('#datetimepicker_deadline_'+glSendSurveyCount).datetimepicker(
        {
            defaultDate: deadline,
            format:'YYYY-MM-DD HH:mm:ss',
            ignoreReadonly: true
        }
    );

    glSendSurveyCount++;

    // 重新調整 index
    rearrange_index();

}

function create_survey_info(glSendSurveyCount, show_index)
{
    var survey_info = document.createElement('div');

    var survey_table = document.createElement('table');
    survey_table.setAttribute("class","table");
    survey_info.appendChild(survey_table);

    var survey_tablehead = document.createElement('thead');
    survey_tablehead.setAttribute("style","display:none");
    survey_table.appendChild(survey_tablehead);

    var survey_tablehead_tr = document.createElement('tr');
    survey_tablehead.appendChild(survey_tablehead_tr);

    var survey_tablehead_th_0 = document.createElement('th');
    //survey_tablehead_th_0.setAttribute("style","width: 10px");
    survey_tablehead_tr.appendChild(survey_tablehead_th_0);

    var survey_tablehead_th_1 = document.createElement('th');
    survey_tablehead_tr.appendChild(survey_tablehead_th_1);

    var survey_tablebody = document.createElement('tbody');
    survey_table.appendChild(survey_tablebody);

    //===============
    var survey_tr = document.createElement('tr');
    survey_tablebody.appendChild(survey_tr);

    var survey_td_0 = document.createElement('td');
    survey_td_0.setAttribute("style","text-align:right;");
    survey_tr.appendChild(survey_td_0);
    survey_td_0.appendChild(document.createTextNode('選擇待辦事項:'));

    var survey_td_1 = document.createElement('td');
    survey_tr.appendChild(survey_td_1);

    var survey_dropdown = create_survey_dropdown('survey_dropdown_'+glSendSurveyCount, show_index);
    if (survey_dropdown == null)
        return null;
    survey_td_1.appendChild(survey_dropdown);
    //===============

    //===============
    var deadline_tr = document.createElement('tr');
    survey_tablebody.appendChild(deadline_tr);

    var deadline_td_0 = document.createElement('td');
    deadline_td_0.setAttribute("style","text-align:right;");
    deadline_tr.appendChild(deadline_td_0);
    deadline_td_0.appendChild(document.createTextNode('選擇回報截止時間:'));

    var deadline_td_1 = document.createElement('td');
    deadline_tr.appendChild(deadline_td_1);

    var deadline_form = document.createElement('div');
    deadline_form.setAttribute("class","form-group");
    deadline_form.setAttribute("style","margin-left:0px;");
    deadline_td_1.appendChild(deadline_form);

    var deadline_div = document.createElement('div');
    deadline_div.setAttribute("class","input-group date");
    deadline_div.setAttribute("id","datetimepicker_deadline_"+glSendSurveyCount);
    deadline_div.setAttribute("style","width:200px;");
    deadline_form.appendChild(deadline_div);

    var deadline_input = document.createElement('input');
    deadline_input.setAttribute("type","text");
    deadline_input.setAttribute("readonly","true");
    deadline_input.setAttribute("class","form-control");
    deadline_input.setAttribute("id","survey_deadline_"+glSendSurveyCount);
    deadline_div.appendChild(deadline_input);

    var deadline_span = document.createElement('span');
    deadline_span.setAttribute("class","input-group-addon");
    deadline_div.appendChild(deadline_span);

    var deadline_glyphicon = document.createElement('span');
    deadline_glyphicon.setAttribute("class","glyphicon glyphicon-calendar");
    deadline_span.appendChild(deadline_glyphicon);
    //===============

    return survey_info;
}

function create_survey_dropdown(uiid, default_index)
{
    var survey = glSurvey_List[default_index];

    if ( survey == undefined )
        return null;

    var survey_dropdown = document.createElement('div');
    survey_dropdown.setAttribute("style","margin-left:0px;");
    survey_dropdown.setAttribute("class","btn-group");

    var survey_btn1 = document.createElement('button');
    survey_btn1.setAttribute("id", uiid);
    survey_btn1.setAttribute("type","button");
    survey_btn1.setAttribute("data-id", survey.surveyid);
    survey_btn1.setAttribute("class","btn btn-default btn-flat");
    survey_btn1.textContent = survey.title;
    survey_dropdown.appendChild(survey_btn1);

    var survey_btn2 = document.createElement('button');
    survey_btn2.setAttribute("type","button");
    survey_btn2.setAttribute("class","btn btn-default btn-flat dropdown-toggle");
    survey_btn2.setAttribute("data-toggle","dropdown");
    survey_dropdown.appendChild(survey_btn2);

    var survey_btn2_span = document.createElement('span');
    survey_btn2_span.setAttribute("class","caret");
    survey_btn2.appendChild(survey_btn2_span);

    var survey_btn2_span2 = document.createElement('span');
    survey_btn2_span2.setAttribute("class","sr-only");
    survey_btn2.appendChild(survey_btn2_span2);

    var survey_ul = document.createElement('ul');
    survey_ul.setAttribute("role","menu");
    survey_ul.setAttribute("class","dropdown-menu");
    survey_dropdown.appendChild(survey_ul);

    for(var index=0;index<glSurvey_List.length;index++)
    {
        var cur_survey = glSurvey_List[index];

        var survey_li = document.createElement('li');
        survey_ul.appendChild(survey_li);

        var survey_li_a = document.createElement('a');
        survey_li_a.setAttribute("href","javascript:;");
        survey_li_a.setAttribute("onclick","survey_title_click(\""+uiid+"\", "+default_index+", "+index+");");
        survey_li_a.textContent = cur_survey.title;
        survey_li.appendChild(survey_li_a);

    }
    return survey_dropdown;
}

function show_survey_item(show_index)
{
    var survey = glSurvey_List[show_index];

    var survey_item = document.createElement('div');
    //survey_item.setAttribute("id","survey_item_"+surveyid);

    var survey_table = document.createElement('table');
    survey_table.setAttribute("class","table");
    survey_item.appendChild(survey_table);

    var survey_tablehead = document.createElement('thead');
    survey_tablehead.setAttribute("style","display:none");
    survey_table.appendChild(survey_tablehead);

    var survey_tablehead_tr = document.createElement('tr');
    survey_tablehead.appendChild(survey_tablehead_tr);

    var survey_tablehead_th_0 = document.createElement('th');
    survey_tablehead_th_0.setAttribute("style","width: 10px");
    survey_tablehead_tr.appendChild(survey_tablehead_th_0);

    var survey_tablehead_th_1 = document.createElement('th');
    survey_tablehead_tr.appendChild(survey_tablehead_th_1);

    var survey_tablebody = document.createElement('tbody');
    //survey_tablebody.setAttribute("id","survey_table_body_"+surveyid);
    survey_table.appendChild(survey_tablebody);

    for(var index=0;index<survey.items.length;index++)
    {
        var item = survey.items[index];
        var survey_tr = document.createElement('tr');
        survey_tablebody.appendChild(survey_tr);

        var survey_td_0 = document.createElement('td');
        survey_tr.appendChild(survey_td_0);
        survey_td_0.appendChild(document.createTextNode(index+1));

        var survey_td_1 = document.createElement('td');
        survey_tr.appendChild(survey_td_1);
        survey_td_1.appendChild(document.createTextNode(item.description));

        var survey_td_2 = document.createElement('td');
        survey_tr.appendChild(survey_td_2);
    }

    return survey_item;
}

function survey_title_click(uiid, ui_index, index)
{
    var survey = glSurvey_List[index];

    var survey_btn1 = document.getElementById(uiid);
    survey_btn1.setAttribute("data-id", survey.surveyid);
    survey_btn1.textContent = survey.title;

    var survey_td_type = document.getElementById("survey_type_"+ui_index);
    while (survey_td_type.firstChild)
    {
        survey_td_type.removeChild(survey_td_type.firstChild);
    }
    if ( survey.type === '0' )
        survey_td_type.appendChild(document.createTextNode("單選"));
    if ( survey.type === '1' )
        survey_td_type.appendChild(document.createTextNode("複選"));


    var survey_td_item = document.getElementById("survey_item_"+ui_index);
    while (survey_td_item.firstChild)
    {
        survey_td_item.removeChild(survey_td_item.firstChild);
    }
    var survey_items = show_survey_item(index);
    survey_td_item.appendChild(survey_items);
}

function delete_survey_item_click(ui_index)
{
    var survey_tr = document.getElementById("survey_row_"+ui_index);
    survey_tr.setAttribute("style","display:none");

    // 重新調整 index
    rearrange_index();
}

function rearrange_index()
{
    // 重新調整 index
    var uiindex = 1;
    for(var index=0; index<glSendSurveyCount;index++)
    {
        var survey_tr = document.getElementById("survey_row_"+index);
        var survey_td_num = document.getElementById("survey_row_index_"+index);
        if (survey_tr != undefined)
        {
            if ( isHidden(survey_tr) )
            {

            }
            else
            {
                while (survey_td_num.firstChild)
                {
                    survey_td_num.removeChild(survey_td_num.firstChild);
                }

                survey_td_num.appendChild(document.createTextNode(uiindex));

                uiindex++;
            }
        }
    }
}

function isHidden(el)
{
    var style = window.getComputedStyle(el);

    return (style.display === 'none');
}

function get_send_surveys()
{
    var survey_list = [];

    for(var index=0; index<glSendSurveyCount;index++)
    {
        var survey_tr = document.getElementById("survey_row_"+index);
        if (survey_tr != undefined)
        {
            if ( isHidden(survey_tr) )
            {

            }
            else
            {
                var surveyid = $("#survey_dropdown_"+index).attr("data-id");
                survey_list.push(surveyid);
            }
        }
    }

    return survey_list;
}

function get_send_surveys_deadline()
{
    var deadline_list = [];

    for(var index=0; index<glSendSurveyCount;index++)
    {
        var survey_tr = document.getElementById("survey_row_"+index);
        if (survey_tr != undefined)
        {
            if ( isHidden(survey_tr) )
            {

            }
            else
            {
                var survey_deadline = document.getElementById("survey_deadline_"+index);
                deadline_list.push(survey_deadline.value);
            }
        }
    }

    return deadline_list;
}

function loc_tab_search_text_change(e)
{
    var input_value = document.getElementById("loc_tab_search_text").value;

     //console.log("input_value:"+input_value);

     if ( input_value != null )
     {
         add_search_target_to_table(input_value);

         setCookie("search_text", input_value, 10);
     }
}

const $source = document.querySelector('#loc_tab_search_text');
if ( $source != null )
{
    $source.addEventListener('input', loc_tab_search_text_change);// register for oninput
}

function remove_uploadfile_click()
{
    document.getElementById("imageUpload").value = "";
    //document.getElementById('imageUpload').files[0] = undefined;
    document.getElementById('imageFileSize').innerHTML = ' ';
    document.getElementById('imageFileType').innerHTML = ' ';
    document.getElementById('imageProgressNumber').innerHTML = '';

    var removebutton_ui = document.getElementById('removebutton');
    if ( removebutton_ui != undefined )
    {
        removebutton_ui.setAttribute("style",'display:none;');
    }
}

function get_subgroup_name(subgroupid)
{
    var subgroupid_int = parseInt(subgroupid);
    var subgroupname = '';

    for (var i = 0; i < gl_usersubgroups.length; i++)
    {
        var subgroupExisted = gl_usersubgroups[i];
        
        // console.log("subgroupExisted_int:"+subgroupExisted_int +" subgroupid_int:"+subgroupid_int);

        if ( subgroupExisted.subgroupid == subgroupid_int )
        {
            subgroupname = subgroupExisted.subgroupname;
            break;
        }
    }

    return subgroupname;
}

function check_cookies()
{
    var input_value = getCookie("search_text");

    var loc_tab_search_text = document.getElementById("loc_tab_search_text");

    if ( loc_tab_search_text != null )
    {
        loc_tab_search_text.value = input_value;
        add_search_target_to_table(input_value);
    }
}

function setCookie(name, value, days)
{
    var expires = "";
    if (days)
    {
        var date = new Date();
        date.setTime(date.getTime() + (days*24*60*60*1000));
        expires = "; expires=" + date.toUTCString();
    }
    document.cookie = name + "=" + (value || "")  + expires + "; path=/";
}

function getCookie(name)
{
    var nameEQ = name + "=";
    var ca = document.cookie.split(';');
    for(var i=0;i < ca.length;i++)
    {
        var c = ca[i];
        while (c.charAt(0)==' ')
            c = c.substring(1,c.length);
        if (c.indexOf(nameEQ) == 0)
        {
            return c.substring(nameEQ.length,c.length);
        }
    }
    return null;
}

function eraseCookie(name)
{
    document.cookie = name+'=; Max-Age=-99999999;';
}

function open_pushnotification_page()
{
    var win = window.open('/index.php/vilspushnotification/'+project_id, '_blank');
    win.focus();
}
