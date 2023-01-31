/*
 * Author: ikki Chung
 * Date: 2019.09.03
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var LOAD_DEVICE_NUM = 20;
var UPDATE_INTERVAL = 1000; // update display every 1 seconds
var RECORD_PAGE_COUNT = 5;

var DEVICE_TYPE_TAG = 0;
var DEVICE_TYPE_LOCATOR = 1;
var DEVICE_TYPE_EMPLOYEE = 2;

var TAG_FUNCTION_NUM = 11;
var TAG_FUNCTION_START_NUM = 1;

var LOCATOR_FUNCTION_NUM = 6;
var LOCATOR_FUNCTION_START_NUM = 101;

var EMPLOYEE_FUNCTION_NUM = 9;
var EMPLOYEE_FUNCTION_START_NUM = 1;

var gl_tag_num = 0;
var gl_locator_num = 0;

var gl_tag_load_index = 0;
var gl_coord_load_index = 0;

var glTag_List = [];
var glLocator_List = [];
var glEmployee_List = [];

var glCurrentDevice_clicktime = new Date('2019/01/01');
var glCurrentDevice_nodeid = "";
var glCurrentDevice_macaddress = "";
var glCurrentDevice_type = DEVICE_TYPE_TAG;

var glCurrentDevice_Records = {};
var glCurrentDevice_TriggerMessage_Records = {};
var glDataLoading = 0;

var glPushNotifyPrefix = "";
var glPushNotifyWelcomeTitle = "";
var glPushNotifyWelcomeBody = "";
var glEnablePushNotify = "0";

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
                load_devices();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_system_topic();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                click_tag("");
                CurrentLoadIndex = 3; // next state
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                check_cookies();

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
        glEmployee_List = [];
    })
    .success(function() {
        setTimeout(load_tag, 10);
        setTimeout(load_locator, 20);
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_tag()
{
    // if (gl_tag_load_index >= gl_tag_num)
    // {
    //     gl_tag_load_index = 0;
    //     return;
    // }
    if (gl_tag_load_index == 0)
    {
        glTag_List = [];
        glEmployee_List = [];
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
            var role = nodeData[4];// 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
            if ( role === '4' )
                glEmployee_List.push(nodeData);
            else
                glTag_List.push(nodeData);
        }

        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            add_tag_to_table();
            add_Employee_to_table();
            CurrentLoadIndex = 1; // next state
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
        var role = nodeData[4];// 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師

        var table_tr = document.createElement('tr');
        tag_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        table_td_1.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_1);

        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_node_name_"+ node_nodeid);
        table_td_2.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_2);

        // node id
        var tag_nodeid_span = document.createElement('span');
        tag_nodeid_span.setAttribute("class",'badge bg-green');
        tag_nodeid_span.textContent = node_name;
        tag_nodeid_span.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_td_2.appendChild(tag_nodeid_span);

        var table_td_3 = document.createElement('td');
        table_td_3.textContent = get_role_name(role);
        table_tr.appendChild(table_td_3);

    }
}

function add_Employee_to_table()
{
    var employee_list_content = document.getElementById('employee_list_content');

    while (employee_list_content.rows.length >= 1)
    {
        employee_list_content.deleteRow(0);
    }

    //console.log("add_tag_to_table() glTag_List.length:"+glTag_List.length);

    for (var i = 0; i < glEmployee_List.length; i++)
    {
        var nodeData = glEmployee_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var table_tr = document.createElement('tr');
        employee_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        table_td_1.setAttribute("onclick","click_employee(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_1);

        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_node_name_"+ node_nodeid);
        table_td_2.setAttribute("onclick","click_employee(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_2);

        // node id
        var tag_nodeid_span = document.createElement('span');
        tag_nodeid_span.setAttribute("class",'badge bg-green');
        tag_nodeid_span.textContent = node_name;
        tag_nodeid_span.setAttribute("onclick","click_employee(\""+ node_nodeid +"\");return false;");
        table_td_2.appendChild(tag_nodeid_span);
    }
}

function load_locator()
{
    // if (gl_locator_load_index >= gl_locator_num)
    // {
    //     gl_locator_load_index = 0;
    //     return;
    // }
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
        var buttontype = nodeData[4];

        var table_tr = document.createElement('tr');
        locator_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        table_td_1.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_1);

        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_node_name_"+ node_nodeid);
        table_td_2.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_2);

        // node id
        var locator_nodeid_span = document.createElement('span');
        locator_nodeid_span.setAttribute("class",'badge bg-yellow');
        locator_nodeid_span.textContent = node_name;
        locator_nodeid_span.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_td_2.appendChild(locator_nodeid_span);


        var table_td_3 = document.createElement('td');
        table_td_3.textContent = get_buttontype_name(buttontype);
        table_tr.appendChild(table_td_3);

    }
}

function load_system_topic()
{
    document.getElementById("PushNotifyPrefixInputText").value = "Loading...";
    document.getElementById("PushNotifyWelcomeTitleInputText").value = "Loading...";
    document.getElementById("PushNotifyWelcomeBodyInputText").value = "Loading...";
    document.getElementById('EnablePushNotifyCheckBtn').checked = false;

    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsystemconfig=1'+'&project_id='+project_id, function(data)
    {
        glPushNotifyPrefix = data.PushNotifyPrefix;
        glPushNotifyWelcomeTitle = data.PushNotifyWelcomeTitle;
        glPushNotifyWelcomeBody = data.PushNotifyWelcomeBody;
        glEnablePushNotify = data.EnablePushNotify;

        document.getElementById("PushNotifyPrefixInputText").value = data.PushNotifyPrefix;
        document.getElementById("PushNotifyWelcomeTitleInputText").value = data.PushNotifyWelcomeTitle;
        document.getElementById("PushNotifyWelcomeBodyInputText").value = data.PushNotifyWelcomeBody;

        if ( data.EnablePushNotify == "1" )
            document.getElementById('EnablePushNotifyCheckBtn').click();
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
        CurrentLoadIndex = 2; // next state
    });
}

function click_tag(click_node_nodeid)
{
    //console.log("click_tag() click_node_macaddress:" + click_node_macaddress);
    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentDevice_clicktime.getTime());

    if (diffTimeInMS < 500 || glDataLoading == 1)
    {
        //console.log("click_tag() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glDataLoading = 1;
    glCurrentDevice_clicktime = new Date();

    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_nodeid = nodeData[3];
        var node_macaddress = nodeData[6];

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentDevice_nodeid.length == 0 || click_node_nodeid === node_nodeid )
        {
            glCurrentDevice_type = DEVICE_TYPE_TAG;
            glCurrentDevice_nodeid = node_nodeid;
            glCurrentDevice_macaddress = node_macaddress;
            bg_color = "background-color:#CCEEFF;";
        }

        var id_row_index = document.getElementById("id_row_index_"+ node_nodeid);
        var id_row_node_name = document.getElementById("id_row_node_name_"+ node_nodeid);
        id_row_index.setAttribute("style",bg_color);
        id_row_node_name.setAttribute("style",bg_color);
    }

    load_device_record();
}

function click_locator(click_node_nodeid)
{
    //console.log("click_tag() click_node_macaddress:" + click_node_macaddress);
    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentDevice_clicktime.getTime());

    if (diffTimeInMS < 500 || glDataLoading == 1)
    {
        //console.log("click_tag() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glDataLoading = 1;
    glCurrentDevice_clicktime = new Date();

    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var node_nodeid = nodeData[3];
        var node_macaddress = nodeData[6];

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentDevice_nodeid.length == 0 || click_node_nodeid === node_nodeid )
        {
            glCurrentDevice_type = DEVICE_TYPE_LOCATOR;
            glCurrentDevice_nodeid = node_nodeid;
            glCurrentDevice_macaddress = node_macaddress;
            bg_color = "background-color:#CCEEFF;";
        }

        var id_row_index = document.getElementById("id_row_index_"+ node_nodeid);
        var id_row_node_name = document.getElementById("id_row_node_name_"+ node_nodeid);
        id_row_index.setAttribute("style",bg_color);
        id_row_node_name.setAttribute("style",bg_color);
    }

    load_device_record();
}


function click_employee(click_node_nodeid)
{
    //console.log("click_tag() click_node_macaddress:" + click_node_macaddress);
    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentDevice_clicktime.getTime());

    if (diffTimeInMS < 500 || glDataLoading == 1)
    {
        //console.log("click_tag() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glDataLoading = 1;
    glCurrentDevice_clicktime = new Date();

    for (var i = 0; i < glEmployee_List.length; i++)
    {
        var nodeData = glEmployee_List[i];
        var node_nodeid = nodeData[3];
        var node_macaddress = nodeData[6];

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentDevice_nodeid.length == 0 || click_node_nodeid === node_nodeid )
        {
            glCurrentDevice_type = DEVICE_TYPE_EMPLOYEE;
            glCurrentDevice_nodeid = node_nodeid;
            glCurrentDevice_macaddress = node_macaddress;
            bg_color = "background-color:#CCEEFF;";
        }

        var id_row_index = document.getElementById("id_row_index_"+ node_nodeid);
        var id_row_node_name = document.getElementById("id_row_node_name_"+ node_nodeid);
        id_row_index.setAttribute("style",bg_color);
        id_row_node_name.setAttribute("style",bg_color);
    }

    load_device_record();
}

function load_device_record()
{
    // console.log("load_device_record() nodeid:" + glCurrentDevice_nodeid +" macaddress:"+glCurrentDevice_macaddress +" type:"+glCurrentDevice_type);

    var record_box_title = document.getElementById('record_box_title');
    if (record_box_title != undefined)
    {
        record_box_title.textContent = "Loading...";
    }

    glCurrentDevice_Records = {};
    glCurrentDevice_TriggerMessage_Records = {};

    glTargetStartDate = null;
    glTargetEndDate = null;

    load_PushNotify_setting(glCurrentDevice_nodeid, glCurrentDevice_macaddress, glCurrentDevice_type);

}

function load_PushNotify_setting(nodeid, macaddress, type)
{
    var targetURL = gl_target_server + "/php/pushnotify.php";
    var p = $.getJSON(targetURL, 'loadpushnotifysetting=1'+'&project_id='+project_id+'&nodeid='+nodeid, function(data)
    {
        $.each(data, function(index, value)
        {
            if (value.functions.length > 0)
                glCurrentDevice_Records[value.targetid] = value.functions.split(",");
            else
                glCurrentDevice_Records[value.targetid] = [];
            //console.log("targetid:"+value.targetid+" glCurrentDevice_Records[value.targetid].length:"+glCurrentDevice_Records[value.targetid].length);
        });
    })
    .success(function() {

        setTimeout(load_PushNotify_triggerMessage, 10, nodeid, macaddress, type);
    })
    .error(function() {
    })
    .complete(function() {
    });
}


function load_PushNotify_triggerMessage(nodeid, macaddress, type)
{
    var targetURL = gl_target_server + "/php/pushnotify.php";
    var p = $.getJSON(targetURL, 'loadtriggermessage=1'+'&project_id='+project_id+'&nodeid='+nodeid, function(data)
    {
        $.each(data, function(index, value)
        {
            //console.log("value:"+value);
            if (glCurrentDevice_TriggerMessage_Records[value.nodeid] == undefined)
                glCurrentDevice_TriggerMessage_Records[value.nodeid] = [];
            glCurrentDevice_TriggerMessage_Records[value.nodeid].push(value);
        });
    })
    .success(function() {

        var record_box_title = document.getElementById('record_box_title');
        if (record_box_title != undefined)
        {
            record_box_title.textContent = get_node_name(nodeid, type);
            if ( glCurrentDevice_type == DEVICE_TYPE_LOCATOR)
            {
                record_box_title.setAttribute("class","badge bg-yellow");
            }
            else
            {
                record_box_title.setAttribute("class","badge bg-green");
            }
        }

        create_PushNotify_setting(nodeid, macaddress, type);

    })
    .error(function() {
    })
    .complete(function() {
        glDataLoading = 0;
    });
}

function create_PushNotify_setting(nodeid, macaddress, type)
{
    // console.log("create_PushNotify_setting() nodeid:" + nodeid +" macaddress:"+macaddress +" type:"+type);

    var notify_table = document.getElementById('PUSH_NOTIFY_TABLE_BODY');

    while (notify_table.rows.length >= 1)
    {
        notify_table.deleteRow(0);
    }

    var row = create_topic_item(nodeid, macaddress, type);
    notify_table.appendChild(row);

    var row = create_setting_item(nodeid, type);
    notify_table.appendChild(row);

    if ( type == DEVICE_TYPE_EMPLOYEE )
    {
        var row = create_tag_list_item(nodeid, type);
        notify_table.appendChild(row);

        var row = create_locator_list_item(nodeid, type);
        notify_table.appendChild(row);
    }
    else
    if ( type == DEVICE_TYPE_LOCATOR )
    {
        var row = create_target_list_item(nodeid, type);
        notify_table.appendChild(row);
    }
}

function get_node_name(nodeid, type)
{
    var nodename = nodeid;
    if ( type == DEVICE_TYPE_TAG )
    {
        nodename = get_node_name_fromlist(glTag_List, nodeid);
    }
    else
    if ( type == DEVICE_TYPE_LOCATOR )
    {
        nodename = get_node_name_fromlist(glLocator_List, nodeid);
    }
    else
    if ( type == DEVICE_TYPE_EMPLOYEE )
    {
        nodename = get_node_name_fromlist(glEmployee_List, nodeid);
    }

    return nodename;
}

function get_node_name_fromlist(nodelist, nodeid)
{
    var nodename = nodeid;

    for (var i = 0; i < nodelist.length; i++)
    {
        var nodeData = nodelist[i];
        var node_nodeid = nodeData[3];

        if( nodeid === node_nodeid )
        {
            nodename = nodeData[5];
            break;
        }
    }

    return nodename;
}

function create_topic_item(nodeid, macaddress, type)
{
    var row = document.createElement('tr');
    row.setAttribute("bgcolor","#cce6ff");

    var cellIndex = 0;

    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode("TOPIC"));
    cellIndex++;

    var topic_type = "TAG";
    if ( type == DEVICE_TYPE_TAG )
        topic_type = "TAG";
    if ( type == DEVICE_TYPE_LOCATOR )
        topic_type = "LOCATOR";
    if ( type == DEVICE_TYPE_EMPLOYEE )
        topic_type = "USER";

    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(glPushNotifyPrefix+"_"+topic_type+"_"+macaddress));
    cellIndex++;

    //
    // show QR Code
    //
    var nodename = get_node_name(nodeid, type);

    row.appendChild(document.createElement('td'));
    var btnQrcode = document.createElement("button");
    btnQrcode.setAttribute("type","button");
    btnQrcode.setAttribute("class","btn btn-default");
    btnQrcode.setAttribute("data-toggle","modal");
    btnQrcode.setAttribute("data-target","#modal-qrcode");
    btnQrcode.setAttribute("onclick","device_qrcode_show(\""+ type +"\",\""+ nodename +"\",\""+ macaddress +"\");return false;");

    var imgQrcode = document.createElement("i");
    imgQrcode.setAttribute("class","fa fa-qrcode");
    btnQrcode.appendChild(imgQrcode);
    row.cells[cellIndex].appendChild(btnQrcode);
    cellIndex++;

    return row;
}

function create_setting_item(nodeid, type)
{
    var row = document.createElement('tr');
    row.setAttribute("bgcolor","#ebebe0");

    var cellIndex = 0;

    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode("SETTING"));
    cellIndex++;

    var node_td = undefined;
    if ( type == DEVICE_TYPE_TAG )
    {
        node_td = create_tag_setting_item(nodeid);
    }
    else
    if ( type == DEVICE_TYPE_LOCATOR )
    {
        node_td = create_locator_setting_item(nodeid);
    }
    else
    if ( type == DEVICE_TYPE_EMPLOYEE )
    {
        node_td = create_employee_setting_item(nodeid);
    }
    if (node_td == undefined)
        return;
    row.appendChild(node_td);
    cellIndex++;

    row.appendChild(document.createElement('td'));

    var linkEdit = document.createElement("a");
    linkEdit.setAttribute("id","EDIT_BTN_"+nodeid);
    linkEdit.setAttribute("href","javascript:;");
    linkEdit.setAttribute("onclick","edit_function(\""+nodeid+"\"," + type + " );return false;");

    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","EDIT_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-edit");
    linkEdit.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(linkEdit);
    cellIndex++;

    return row;
}

function create_tag_setting_item(nodeid)
{
    var tag_td = document.createElement('td');

    var functions = glCurrentDevice_Records[nodeid];
    if (functions == undefined)
        functions = [];
    // console.log("create_tag_setting_item() nodeid:"+nodeid+" functions:"+functions);

    for (var i = TAG_FUNCTION_START_NUM; i < (TAG_FUNCTION_START_NUM + TAG_FUNCTION_NUM); i++)
    {
        var function_name = get_tag_function_name(i);
        var checkbox_item_1 = create_function_item(i, function_name, nodeid, functions);
        tag_td.appendChild(checkbox_item_1);
    }

    return tag_td;
}

function create_locator_setting_item(nodeid)
{
    var locator_td = document.createElement('td');

    var functions = glCurrentDevice_Records[nodeid];
    if (functions == undefined)
        functions = [];

    for (var i = LOCATOR_FUNCTION_START_NUM; i < (LOCATOR_FUNCTION_START_NUM + LOCATOR_FUNCTION_NUM); i++)
    {
        var function_name = get_locator_function_name(i);
        var checkbox_item_1 = create_function_item(i, function_name, nodeid, functions);
        locator_td.appendChild(checkbox_item_1);
    }

    return locator_td;
}

function create_employee_setting_item(nodeid)
{
    var employee_td = document.createElement('td');

    var functions = glCurrentDevice_Records[nodeid];
    if (functions == undefined)
        functions = [];

    for (var i = EMPLOYEE_FUNCTION_START_NUM; i < (EMPLOYEE_FUNCTION_START_NUM + EMPLOYEE_FUNCTION_NUM); i++)
    {
        var function_name = get_employee_function_name(i);
        var checkbox_item_1 = create_function_item(i, function_name, nodeid, functions);
        employee_td.appendChild(checkbox_item_1);
    }

    return employee_td;
}

function create_tag_list_item(nodeid, type)
{
    var row = document.createElement('tr');
    row.setAttribute("bgcolor","#cce6ff");

    var cellIndex = 0;

    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode("訂閱Tag列表"));
    cellIndex++;

    var tag_list_td = document.createElement('td');
    row.appendChild(tag_list_td);

    // create table
    var tag_list_table = document.createElement('table');
    tag_list_table.setAttribute("class","table table-bordered");
    tag_list_td.appendChild(tag_list_table);
    cellIndex++;

    var tag_list_table_body = document.createElement('tbody');
    tag_list_table.appendChild(tag_list_table_body);

    var tag_list_row = document.createElement('tr');
    tag_list_table_body.appendChild(tag_list_row);

    for(var i=0;i<glTag_List.length;i++)
    {
        var nodeData = glTag_List[i];
        var node_nodeid = nodeData[3];
        var node_nodename = nodeData[5];

        var functions = glCurrentDevice_Records[node_nodeid];
        if (functions == undefined)
            functions = [];

        if ( (i != 0) && (i%5 == 0) )
        {
            tag_list_row = document.createElement('tr');
            tag_list_table_body.appendChild(tag_list_row);
        }

        var tag_td = document.createElement('td');
        tag_list_row.appendChild(tag_td);

        var checked = true;
        var enable = false;

        // console.log("create_tag_list_item() nodeid:"+nodeid+" node_nodeid:"+node_nodeid +" functions.length:"+functions.length);

        if ( functions.length == 0 )
            checked = false;

        var input_item = document.createElement('input');
        input_item.setAttribute("id","checkbox_taglist_"+i+"_"+nodeid);
        input_item.setAttribute("type","checkbox");
        input_item.setAttribute("class","flat-red");
        if ( checked )
            input_item.setAttribute("checked",true);
        if (!enable)
        {
            input_item.setAttribute("disabled",true);
        }
        tag_td.appendChild(input_item);

        var tag_ui_span = document.createElement('span');
        tag_ui_span.setAttribute("onclick","taglist_onclick(\""+nodeid+"\", "+i+");return false;");
        tag_ui_span.setAttribute("id","badge_taglist_"+i+"_"+nodeid);
        if ( checked )
        {
            tag_ui_span.setAttribute("class",'badge bg-green');
        }
        else
        {
            tag_ui_span.setAttribute("class",'badge bg-teal');
            tag_ui_span.setAttribute("style",'opacity:0.3;'); // disable
        }
        tag_ui_span.textContent = node_nodename;
        tag_td.appendChild(tag_ui_span);

        //
        // dropdown button
        //
        var dropdown_btn = create_taglist_dropdown_list(nodeid, i, functions, checked);
        tag_td.appendChild(dropdown_btn);
    }

    row.appendChild(document.createElement('td'));

    var linkEdit = document.createElement("a");
    linkEdit.setAttribute("id","EDIT_TAGLIST_BTN_"+nodeid);
    linkEdit.setAttribute("href","javascript:;");
    linkEdit.setAttribute("onclick","edit_taglist_function(\""+nodeid+"\");return false;");

    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","EDIT_TAGLIST_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-edit");
    linkEdit.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(linkEdit);

    // 全選
    var selectAllEdit = document.createElement("button");
    selectAllEdit.setAttribute("type","button");
    selectAllEdit.setAttribute("style","display:none");
    selectAllEdit.setAttribute("id","SEL_ALL_TAGLIST_BTN_"+nodeid);
    selectAllEdit.setAttribute("href","javascript:;");
    selectAllEdit.setAttribute("onclick","select_all_taglist_function(\""+nodeid+"\");return false;");
    selectAllEdit.textContent = "全選";
    row.cells[cellIndex].appendChild(selectAllEdit);

    // 全不選
    var unSelectAllEdit = document.createElement("button");
    unSelectAllEdit.setAttribute("type","button");
    unSelectAllEdit.setAttribute("style","display:none");
    unSelectAllEdit.setAttribute("id","UNSEL_ALL_TAGLIST_BTN_"+nodeid);
    unSelectAllEdit.setAttribute("href","javascript:;");
    unSelectAllEdit.setAttribute("onclick","unselect_all_taglist_function(\""+nodeid+"\");return false;");
    unSelectAllEdit.textContent = "全不選";
    row.cells[cellIndex].appendChild(unSelectAllEdit);

    cellIndex++;

    return row;
}

function create_locator_list_item(nodeid, type)
{
    var row = document.createElement('tr');
    row.setAttribute("bgcolor","#ebebe0");

    var cellIndex = 0;

    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode("訂閱Locator列表"));
    cellIndex++;

    var locator_list_td = document.createElement('td');
    row.appendChild(locator_list_td);

    // create table
    var locator_list_table = document.createElement('table');
    locator_list_table.setAttribute("class","table table-bordered");
    locator_list_td.appendChild(locator_list_table);
    cellIndex++;

    var locator_list_table_body = document.createElement('tbody');
    locator_list_table.appendChild(locator_list_table_body);

    var locator_list_row = document.createElement('tr');
    locator_list_table_body.appendChild(locator_list_row);

    for(var i=0;i<glLocator_List.length;i++)
    {
        var nodeData = glLocator_List[i];
        var node_nodeid = nodeData[3];
        var node_nodename = nodeData[5];

        var functions = glCurrentDevice_Records[node_nodeid];
        if (functions == undefined)
            functions = [];

        if ( (i != 0) && (i%5 == 0) )
        {
            locator_list_row = document.createElement('tr');
            locator_list_table_body.appendChild(locator_list_row);
        }

        var locator_td = document.createElement('td');
        locator_list_row.appendChild(locator_td);

        var checked = true;
        var enable = false;

        if ( functions.length == 0 )
            checked = false;

        var input_item = document.createElement('input');
        input_item.setAttribute("id","checkbox_locatorlist_"+i+"_"+nodeid);
        input_item.setAttribute("type","checkbox");
        input_item.setAttribute("class","flat-red");
        if ( checked )
            input_item.setAttribute("checked",true);
        if (!enable)
        {
            input_item.setAttribute("disabled",true);
        }
        locator_td.appendChild(input_item);

        var locator_ui_span = document.createElement('span');
        locator_ui_span.setAttribute("onclick","locatorlist_onclick(\""+nodeid+"\", "+i+");return false;");
        locator_ui_span.setAttribute("id","badge_locatorlist_"+i+"_"+nodeid);
        if ( checked )
        {
            locator_ui_span.setAttribute("class",'badge bg-yellow');
        }
        else
        {
            locator_ui_span.setAttribute("class",'badge bg-yellow');
            locator_ui_span.setAttribute("style",'opacity:0.3;'); // disable
        }
        locator_ui_span.textContent = node_nodename;
        locator_td.appendChild(locator_ui_span);

        //
        // dropdown button
        //
        var dropdown_btn = create_locatorlist_dropdown_list(nodeid, i, functions, checked);
        locator_td.appendChild(dropdown_btn);

    }

    row.appendChild(document.createElement('td'));

    var linkEdit = document.createElement("a");
    linkEdit.setAttribute("id","EDIT_LOCATORLIST_BTN_"+nodeid);
    linkEdit.setAttribute("href","javascript:;");
    linkEdit.setAttribute("onclick","edit_locatorlist_function(\""+nodeid+"\");return false;");

    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","EDIT_LOCATORLIST_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-edit");
    linkEdit.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(linkEdit);

    // 全選
    var selectAllEdit = document.createElement("button");
    selectAllEdit.setAttribute("type","button");
    selectAllEdit.setAttribute("style","display:none");
    selectAllEdit.setAttribute("id","SEL_ALL_LOCATORLIST_BTN_"+nodeid);
    selectAllEdit.setAttribute("href","javascript:;");
    selectAllEdit.setAttribute("onclick","select_all_locatorlist_function(\""+nodeid+"\");return false;");
    selectAllEdit.textContent = "全選";
    row.cells[cellIndex].appendChild(selectAllEdit);

    // 全不選
    var unSelectAllEdit = document.createElement("button");
    unSelectAllEdit.setAttribute("type","button");
    unSelectAllEdit.setAttribute("style","display:none");
    unSelectAllEdit.setAttribute("id","UNSEL_ALL_LOCATORLIST_BTN_"+nodeid);
    unSelectAllEdit.setAttribute("href","javascript:;");
    unSelectAllEdit.setAttribute("onclick","unselect_all_locatorlist_function(\""+nodeid+"\");return false;");
    unSelectAllEdit.textContent = "全不選";
    row.cells[cellIndex].appendChild(unSelectAllEdit);


    cellIndex++;


    return row;
}

function create_target_list_item(nodeid, type)
{
    var row = document.createElement('tr');
    row.setAttribute("bgcolor","#cce6ff");

    var cellIndex = 0;

    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode("不特定對象推播設定"));
    cellIndex++;

    var target_list_td = document.createElement('td');
    row.appendChild(target_list_td);

    // create table
    var target_list_table = document.createElement('table');
    target_list_table.setAttribute("class","table table-bordered");
    target_list_td.appendChild(target_list_table);
    cellIndex++;

    var target_list_head = createTargetListHead();
    target_list_table.appendChild(target_list_head);

    var target_list_table_body = document.createElement('tbody');
    target_list_table_body.setAttribute("bgcolor","#c4d7de");
    target_list_table.appendChild(target_list_table_body);

    var records = glCurrentDevice_TriggerMessage_Records[nodeid];

    for(var type=0;type<7;type++)
    {
        var bFound = false;
        if (records != null)
        {
            //console.log("records.length="+records.length);
            for(var index=0;index<records.length;index++)
            {
                var record = records[index];
                if ( record.targettype == type )
                {
                    //console.log("record.targettype="+record.targettype);
                    var enable = false;
                    if ( record.enable == 1)
                        enable = true;
                    var target_list_setting_row = createTargetListSetting(type, nodeid, enable, record.distance, record.title, record.message );
                    target_list_table_body.appendChild(target_list_setting_row);
                    bFound = true;
                    break;
                }
            }
        }

        if ( bFound == false)
        {
            var target_list_setting_row = createTargetListSetting(type, nodeid, false, 1000, "推播訊息 Title", "推播訊息內容");
            target_list_table_body.appendChild(target_list_setting_row);
        }
    }

    row.appendChild(document.createElement('td'));

    var linkEdit = document.createElement("a");
    linkEdit.setAttribute("id","EDIT_TARGETLIST_BTN_"+nodeid);
    linkEdit.setAttribute("href","javascript:;");
    linkEdit.setAttribute("onclick","edit_targetlist_function(\""+nodeid+"\");return false;");

    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","EDIT_TARGETLIST_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-edit");
    linkEdit.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(linkEdit);

    return row;
}

function create_function_item(function_id, function_name, nodeid, functions)
{
    var checkbox_item = null;
    var item_index = functions.indexOf(''+function_id);

    //console.log("create_function_item item:"+item+", item_index:"+item_index);

    if ( item_index > -1 )
    {
        checkbox_item = create_checkbox_item(function_id, function_name, nodeid, true, false);
    }
    else
    {
        checkbox_item = create_checkbox_item(function_id, function_name, nodeid, false, false);
    }
    return checkbox_item;
}

function create_checkbox_item(function_id, function_name, nodeid, checked, enable)
{
    var checkbox_item = document.createElement('div');
    checkbox_item.setAttribute("class","form-group");

    var p_item = document.createElement('p');
    if ( checked )
        p_item.setAttribute("class","text-green");
    else
        p_item.setAttribute("class","text-muted");
    checkbox_item.appendChild(p_item);

    var label_item = document.createElement('label');
    p_item.appendChild(label_item);

    var input_item = document.createElement('input');
    input_item.setAttribute("id","checkbox_"+function_id+"_"+nodeid);
    input_item.setAttribute("type","checkbox");
    input_item.setAttribute("class","flat-red");
    if ( checked )
        input_item.setAttribute("checked",true);
    if (!enable)
    {
        input_item.setAttribute("disabled",true);
    }
    label_item.appendChild(input_item);

    label_item.appendChild(document.createTextNode(" " + function_name));

    return checkbox_item;
}

function createTargetListHead()
{
    var table_head = document.createElement('thead');
    table_head.setAttribute("bgcolor","#98cbfc");

    var table_row = document.createElement('tr');
    table_head.appendChild(table_row);

    var target_type_enable = document.createElement('th');
    target_type_enable.textContent = "啟用功能";
    target_type_enable.setAttribute("style","width:80px;");
    table_row.appendChild(target_type_enable);

    var target_type_th = document.createElement('th');
    target_type_th.textContent = "裝置類型";
    table_row.appendChild(target_type_th);

    var distance_th = document.createElement('th');
    distance_th.textContent = "觸發距離";
    table_row.appendChild(distance_th);

    var title_th = document.createElement('th');
    title_th.textContent = "推播訊息 Title";
    table_row.appendChild(title_th);

    var message_th = document.createElement('th');
    message_th.textContent = "推播訊息內容";
    table_row.appendChild(message_th);

    return table_head;
}

function createTargetListSetting(type, nodeid, enable, distance, title, message)
{
    var target_list_setting_row = document.createElement('tr');
    if ((type % 2) == 1)
        target_list_setting_row.setAttribute("bgcolor","#d5d8da");

    var target_list_setting_td_enable = document.createElement('td');
    target_list_setting_row.appendChild(target_list_setting_td_enable);

    var checkbox_item = create_checkbox_item('TargetList_'+type, "", nodeid, enable, false);
    target_list_setting_td_enable.appendChild(checkbox_item);

    var target_list_setting_td_type = document.createElement('td');
    target_list_setting_td_type.textContent = get_target_type_name(""+type);
    target_list_setting_row.appendChild(target_list_setting_td_type);

    var target_list_setting_td_distance = document.createElement('td');
    //target_list_setting_td_distance.setAttribute("id","TargetList_"+type+"_"+nodeid+"_distance");
    //target_list_setting_td_distance.textContent = '100';
    target_list_setting_row.appendChild(target_list_setting_td_distance);

    var div_distance = document.createElement('strong');
    div_distance.setAttribute("id","TargetList_"+type+"_"+nodeid+"_distance");
    div_distance.setAttribute("style","font-weight:bold;");
    div_distance.innerHTML = distance;
    //div_distance.setAttribute("onclick","TargetList_distance_onclick("+ type +", \""+ nodeid +"\");return false;");
    target_list_setting_td_distance.appendChild(div_distance);


    var target_list_setting_td_title = document.createElement('td');
    // target_list_setting_td_title.setAttribute("id","TargetList_"+type+"_"+nodeid+"_title");
    // target_list_setting_td_title.textContent = "推播訊息 title";
    target_list_setting_row.appendChild(target_list_setting_td_title);

    var div_title = document.createElement('strong');
    div_title.setAttribute("id","TargetList_"+type+"_"+nodeid+"_title");
    div_title.setAttribute("style","font-weight:bold;");
    div_title.innerHTML = title;
    //div_title.setAttribute("onclick","TargetList_title_onclick("+ type +", \""+ nodeid +"\");return false;");
    target_list_setting_td_title.appendChild(div_title);

    var target_list_setting_td_message = document.createElement('td');
    // target_list_setting_td_message.setAttribute("id","TargetList_"+type+"_"+nodeid+"_message");
    // target_list_setting_td_message.textContent = "推播訊息內容";
    target_list_setting_row.appendChild(target_list_setting_td_message);

    var div_message = document.createElement('strong');
    div_message.setAttribute("id","TargetList_"+type+"_"+nodeid+"_message");
    div_message.setAttribute("style","font-weight:bold;");
    div_message.innerHTML = message;
    //div_message.setAttribute("onclick","TargetList_message_onclick("+ type +", \""+ nodeid +"\");return false;");
    target_list_setting_td_message.appendChild(div_message);

    return target_list_setting_row;
}

function get_tag_function_name(function_id)
{
    var name = "";
    switch (function_id)
    {
        case 1:
            name = "非設備文字訊息, 如醫囑等";
        break;
        case 2:
            name = "緊急求助與取消通知";
        break;
        case 3:
            name = "低電量通知";
        break;
        case 4:
            name = "進入(離開)危險區通知, 身份為工作人員時不通知";
        break;
        case 5:
            name = "巡檢相關通知, 身份為巡檢員時才有通知";
        break;
        case 6:
            name = "未正常配戴通知";
        break;
        case 7:
            name = "進出區域事件, 需要在事件列表設定才有通知";
        break;
        case 8:
            name = "課程點名通知, 包含上課中進出通知";
        break;
        case 9:
            name = "GPS活動超過區域範圍通知";
        break;
        case 10:
            name = "不特定對象推播";
        break;
        case 11:
            name = "設備日誌相關通知";
        break;
    }
    return name;
}

function get_locator_function_name(function_id)
{
    var name = "";
    switch (function_id)
    {
        case 101:
            name = "非設備文字訊息, 如醫囑等";
        break;
        case 102:
            name = "緊急求助與取消通知";
        break;
        case 103:
            name = "呼喚與取消通知";
        break;
        case 104:
            name = "醫生進出病房與病床區域通知";
        break;
        case 105:
            name = "斷電警報";
        break;
        case 106:
            name = "周邊模組訊息，如不明物體移動、煙霧探測";
        break;
        // case '4':
        //     name = "不特定對象推播, 巡檢或病房相關通知";
        // break;
    }
    return name;
}

function get_employee_function_name(function_id)
{
    var name = "";
    switch (function_id)
    {
        case 1:
            name = "非設備文字訊息, 如醫囑等";
        break;
        case 2:
            name = "緊急求助與取消通知";
        break;
        case 3:
            name = "低電量通知";
        break;
        case 4:
            name = "進入(離開)危險區通知, 身份為工作人員時不通知";
        break;
        case 5:
            name = "巡檢相關通知, 身份為巡檢員時才有通知";
        break;
        case 6:
            name = "未正常配戴通知";
        break;
        case 7:
            name = "進出區域事件, 需要在事件列表設定才有通知";
        break;
        case 8:
            name = "課程點名通知, 包含上課中進出通知";
        break;
        case 9:
            name = "GPS活動超過區域範圍通知";
        break;
    }
    return name;
}

function get_target_type_name(type)
{
    var name = "";
    switch (type)
    {
        case '0':
            name = "人員訪客";
        break;
        case '1':
            name = "巡檢員";
        break;
        case '2':
            name = "設備";
        break;
        case '3':
            name = "VIP貴賓";
        break;
        case '4':
            name = "工作人員";
        break;
        case '5':
            name = "醫師";
        break;
        case '6':
            name = "護理師";
        break;
    }
    return name;
}

function edit_function(nodeid, type)
{
    if ( type == DEVICE_TYPE_TAG )
    {
        enable_edit_functions(nodeid, TAG_FUNCTION_START_NUM, TAG_FUNCTION_NUM);
    }
    else
    if ( type == DEVICE_TYPE_LOCATOR )
    {
        enable_edit_functions(nodeid, LOCATOR_FUNCTION_START_NUM, LOCATOR_FUNCTION_NUM);
    }
    else
    if ( type == DEVICE_TYPE_EMPLOYEE )
    {
        enable_edit_functions(nodeid, EMPLOYEE_FUNCTION_START_NUM, EMPLOYEE_FUNCTION_NUM);
    }

    var linkEdit = document.getElementById("EDIT_BTN_"+nodeid);
    linkEdit.setAttribute("onclick","save_function(\""+nodeid+"\","+type+" );return false;");

    var imgEdit = document.getElementById("EDIT_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-save");
}

function enable_edit_functions(nodeid, start, count)
{
    for (var i = start; i < (start + count); i++)
    {
        var input_item = document.getElementById("checkbox_"+i+"_"+nodeid);
        input_item.removeAttribute("disabled");
    }
}

function edit_taglist_function(nodeid)
{
    for (var index = 0; index < glTag_List.length; index++)
    {
        var input_item = document.getElementById("checkbox_taglist_"+index+"_"+nodeid);
        if (input_item != undefined)
            input_item.removeAttribute("disabled");

        var dropdown_button = document.getElementById("dropdown_cur_tag_"+index+"_"+nodeid);
        if (dropdown_button != undefined)
            dropdown_button.removeAttribute("disabled");

        // dropdown list
        for (var i = TAG_FUNCTION_START_NUM; i < (TAG_FUNCTION_START_NUM + TAG_FUNCTION_NUM); i++)
        {
            var item_checkbox = document.getElementById("dropdown_checkbox_tag_function_"+i+"_"+index+'_'+nodeid);
            if (item_checkbox != undefined)
                item_checkbox.removeAttribute("disabled");
        }
    }

    var linkEdit = document.getElementById("EDIT_TAGLIST_BTN_"+nodeid);
    linkEdit.setAttribute("onclick","save_taglist_function(\""+nodeid+"\");return false;");

    var imgEdit = document.getElementById("EDIT_TAGLIST_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-save");

    var selectAllEdit = document.getElementById("SEL_ALL_TAGLIST_BTN_"+nodeid);
    selectAllEdit.setAttribute("style","display:block");

    var unSelectAllEdit = document.getElementById("UNSEL_ALL_TAGLIST_BTN_"+nodeid);
    unSelectAllEdit.setAttribute("style","display:block");
}

function edit_locatorlist_function(nodeid)
{
    for (var index = 0; index < glLocator_List.length; index++)
    {
        var input_item = document.getElementById("checkbox_locatorlist_"+index+"_"+nodeid);
        if (input_item != undefined)
            input_item.removeAttribute("disabled");

        var dropdown_button = document.getElementById("dropdown_cur_locator_"+index+"_"+nodeid);
        if (dropdown_button != undefined)
            dropdown_button.removeAttribute("disabled");

        // dropdown list
        for (var i = LOCATOR_FUNCTION_START_NUM; i < (LOCATOR_FUNCTION_START_NUM + LOCATOR_FUNCTION_NUM); i++)
        {
            var item_checkbox = document.getElementById("dropdown_checkbox_locator_function_"+i+"_"+index+'_'+nodeid);
            if (item_checkbox != undefined)
                item_checkbox.removeAttribute("disabled");
        }
    }

    var linkEdit = document.getElementById("EDIT_LOCATORLIST_BTN_"+nodeid);
    linkEdit.setAttribute("onclick","save_locatorlist_function(\""+nodeid+"\");return false;");

    var imgEdit = document.getElementById("EDIT_LOCATORLIST_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-save");

    var selectAllEdit = document.getElementById("SEL_ALL_LOCATORLIST_BTN_"+nodeid);
    selectAllEdit.setAttribute("style","display:block");

    var unSelectAllEdit = document.getElementById("UNSEL_ALL_LOCATORLIST_BTN_"+nodeid);
    unSelectAllEdit.setAttribute("style","display:block");
}

function edit_targetlist_function(nodeid)
{
    var records = glCurrentDevice_TriggerMessage_Records[nodeid];

    for(var type=0;type<7;type++)
    {
        var input_item = document.getElementById("checkbox_TargetList_"+type+"_"+nodeid);
        if (input_item != undefined)
            input_item.removeAttribute("disabled");

        var distance = '';
        var title = '';
        var message = '';

        if (records != null)
        {
            for(var index=0;index<records.length;index++)
            {
                var record = records[index];
                if ( record.targettype == type )
                {
                    distance = record.distance;
                    title = record.title;
                    message = record.message;
                    break;
                }
            }
        }

        var ui_distance = document.getElementById("TargetList_"+type+"_"+nodeid+"_distance");
        ui_distance.setAttribute("style","font-weight:bold;border:thin dotted red;");
        ui_distance.setAttribute("onclick","TargetList_distance_onclick("+ type +", \""+ nodeid +"\", \""+distance+"\");return false;");

        var ui_title= document.getElementById("TargetList_"+type+"_"+nodeid+"_title");
        ui_title.setAttribute("style","font-weight:bold;border:thin dotted red;");
        ui_title.setAttribute("onclick","TargetList_title_onclick("+ type +", \""+ nodeid +"\", \""+title+"\");return false;");

        var ui_message = document.getElementById("TargetList_"+type+"_"+nodeid+"_message");
        ui_message.setAttribute("style","font-weight:bold;border:thin dotted red;");
        ui_message.setAttribute("onclick","TargetList_message_onclick("+ type +", \""+ nodeid +"\", \""+message+"\");return false;");
    }

    var linkEdit = document.getElementById("EDIT_TARGETLIST_BTN_"+nodeid);
    linkEdit.setAttribute("onclick","save_targetlist_function(\""+nodeid+"\");return false;");

    var imgEdit = document.getElementById("EDIT_TARGETLIST_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-save");
}

function save_taglist_function(nodeid)
{
    var linkEdit = document.getElementById("EDIT_TAGLIST_BTN_"+nodeid);
    linkEdit.setAttribute("onclick","edit_taglist_function(\""+nodeid+"\");return false;");

    var imgEdit = document.getElementById("EDIT_TAGLIST_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-edit");

    var selectAllEdit = document.getElementById("SEL_ALL_TAGLIST_BTN_"+nodeid);
    selectAllEdit.setAttribute("style","display:none");

    var unSelectAllEdit = document.getElementById("UNSEL_ALL_TAGLIST_BTN_"+nodeid);
    unSelectAllEdit.setAttribute("style","display:none");

    var functions = get_taglist_functions(nodeid, TAG_FUNCTION_START_NUM, TAG_FUNCTION_NUM);

    //console.log("save_taglist_function() nodeid:"+nodeid +" functions:"+functions);

    update_PushNotify(nodeid, functions);

}

function get_taglist_functions(nodeid, function_start, function_count)
{
    var functions = {};
    for (var index = 0; index < glTag_List.length; index++)
    {
        var nodeData = glTag_List[index];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var curTagFunction = [];
// 123
        var input_item = document.getElementById("checkbox_taglist_"+index+"_"+nodeid);
        if( input_item.checked )
        {
            // dropdown list
            for (var i = function_start; i < (function_start + function_count); i++)
            {
                var item_checkbox = document.getElementById("dropdown_checkbox_tag_function_"+i+"_"+index+'_'+nodeid);
                if (item_checkbox != undefined)
                {
                    if( item_checkbox.checked )
                    {
                        curTagFunction.push(''+i);
                    }
                }
            }
        }

        functions[node_nodeid] = curTagFunction.toString();

        //console.log("get_taglist_functions() node_nodeid:"+node_nodeid+" curTagFunction:"+curTagFunction.toString());
    }
    //console.log("functions.length:"+functions.length);
    return functions;
}

function save_locatorlist_function(nodeid)
{
    var linkEdit = document.getElementById("EDIT_LOCATORLIST_BTN_"+nodeid);
    linkEdit.setAttribute("onclick","edit_locatorlist_function(\""+nodeid+"\");return false;");

    var imgEdit = document.getElementById("EDIT_LOCATORLIST_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-edit");

    var selectAllEdit = document.getElementById("SEL_ALL_LOCATORLIST_BTN_"+nodeid);
    selectAllEdit.setAttribute("style","display:none");

    var unSelectAllEdit = document.getElementById("UNSEL_ALL_LOCATORLIST_BTN_"+nodeid);
    unSelectAllEdit.setAttribute("style","display:none");

    var functions = get_locatorlist_functions(nodeid, LOCATOR_FUNCTION_START_NUM, LOCATOR_FUNCTION_NUM);

    update_PushNotify(nodeid, functions);

}

function get_locatorlist_functions(nodeid, function_start, function_count)
{
    var functions = {};
    for (var index = 0; index < glLocator_List.length; index++)
    {
        var nodeData = glLocator_List[index];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var curLocatorFunction = [];

        var input_item = document.getElementById("checkbox_locatorlist_"+index+"_"+nodeid);
        if( input_item.checked )
        {
            // dropdown list
            for (var i = function_start; i < (function_start+function_count); i++)
            {
                var item_checkbox = document.getElementById("dropdown_checkbox_locator_function_"+i+"_"+index+'_'+nodeid);
                if (item_checkbox != undefined)
                {
                    if( item_checkbox.checked )
                    {
                        curLocatorFunction.push(''+i);
                    }
                }
            }
            //console.log(node_nodeid+":"+curLocatorFunction);
        }

        functions[node_nodeid] = curLocatorFunction.toString();
    }
    //console.log("functions.length:"+functions.length);
    return functions;
}

function save_targetlist_function(nodeid)
{
    var linkEdit = document.getElementById("EDIT_TARGETLIST_BTN_"+nodeid);
    linkEdit.setAttribute("onclick","edit_targetlist_function(\""+nodeid+"\");return false;");

    var imgEdit = document.getElementById("EDIT_TARGETLIST_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-edit");

    var functions = get_targetlist_functions(nodeid);

    update_TriggerMessage(nodeid, functions);

}

function get_targetlist_functions(nodeid)
{
    var functions = {};
    for(var type=0;type<7;type++)
    {
        var strType = ""+type;
        var newdata = {};

        console.log("save_targetlist_function() enable type:"+type);

        var distance = document.getElementById("TargetList_"+type+"_"+nodeid+"_distance").textContent;
        var title = document.getElementById("TargetList_"+type+"_"+nodeid+"_title").textContent;
        var message = document.getElementById("TargetList_"+type+"_"+nodeid+"_message").textContent;

        console.log("save_targetlist_function() distance:"+distance +" title:"+title +" message:"+message);


        var input_item = document.getElementById("checkbox_TargetList_"+type+"_"+nodeid);
        if( input_item.checked )
        {
            newdata['enable'] = 1;
            newdata['distance'] = distance;
            newdata['title'] = title;
            newdata['message'] = message;

            functions[strType] = newdata;
        }
        else
        {
            newdata['enable'] = 0;
            newdata['distance'] = distance;
            newdata['title'] = title;
            newdata['message'] = message;

            functions[strType] = newdata;
        }
    }
    return functions;
}

function taglist_onclick(nodeid, index)
{
    var ui_id = "checkbox_taglist_"+index+"_"+nodeid;
    var ui_checkbox = document.getElementById(ui_id);
    var ui_disable = ui_checkbox.getAttribute("disabled");
    if (!ui_disable)
    {
        ui_checkbox.checked = !ui_checkbox.checked;

        var tag_ui_span = document.getElementById("badge_taglist_"+index+"_"+nodeid);
        if(ui_checkbox.checked)
            tag_ui_span.setAttribute("style",'opacity:1.0;'); // disable
        else
            tag_ui_span.setAttribute("style",'opacity:0.3;'); // disable
        // console.log("locatorlist_onclick() ui_id:"+ui_id+" ui_checkbox.checked:"+ui_checkbox.checked);
    }
}

function locatorlist_onclick(nodeid, index)
{
    var ui_id = "checkbox_locatorlist_"+index+"_"+nodeid;
    var ui_checkbox = document.getElementById(ui_id);
    var ui_disable = ui_checkbox.getAttribute("disabled");
    if (!ui_disable)
    {
        ui_checkbox.checked = !ui_checkbox.checked;

        var locator_ui_span = document.getElementById("badge_locatorlist_"+index+"_"+nodeid);
        if(ui_checkbox.checked)
            locator_ui_span.setAttribute("style",'opacity:1.0;'); // disable
        else
            locator_ui_span.setAttribute("style",'opacity:0.3;'); // disable
        // console.log("locatorlist_onclick() ui_id:"+ui_id+" ui_checkbox.checked:"+ui_checkbox.checked);
    }
}

function select_all_taglist_function(nodeid)
{
    for (var i = 0; i < glTag_List.length; i++)
    {
        var ui_checkbox = document.getElementById("checkbox_taglist_"+i+"_"+nodeid);
        ui_checkbox.checked = true;
        var tag_ui_span = document.getElementById("badge_taglist_"+i+"_"+nodeid);
        tag_ui_span.setAttribute("style",'opacity:1.0;');
    }
}

function unselect_all_taglist_function(nodeid)
{
    for (var i = 0; i < glTag_List.length; i++)
    {
        var ui_checkbox = document.getElementById("checkbox_taglist_"+i+"_"+nodeid);
        ui_checkbox.checked = false;
        var tag_ui_span = document.getElementById("badge_taglist_"+i+"_"+nodeid);
        tag_ui_span.setAttribute("style",'opacity:0.3;');
    }
}

function select_all_locatorlist_function(nodeid)
{
    for (var i = 0; i < glLocator_List.length; i++)
    {
        var ui_checkbox = document.getElementById("checkbox_locatorlist_"+i+"_"+nodeid);
        ui_checkbox.checked = true;
        var locator_ui_span = document.getElementById("badge_locatorlist_"+i+"_"+nodeid);
        locator_ui_span.setAttribute("style",'opacity:1.0;');
    }
}

function unselect_all_locatorlist_function(nodeid)
{
    for (var i = 0; i < glLocator_List.length; i++)
    {
        var ui_checkbox = document.getElementById("checkbox_locatorlist_"+i+"_"+nodeid);
        ui_checkbox.checked = false;
        var locator_ui_span = document.getElementById("badge_locatorlist_"+i+"_"+nodeid);
        locator_ui_span.setAttribute("style",'opacity:0.3;');
    }
}

function create_taglist_dropdown_list(nodeid, index, functions, checked)
{
    var dropdown_div = document.createElement('div');
    dropdown_div.setAttribute("class",'btn-group');

    var dropdown_button = document.createElement('button');
    dropdown_button.setAttribute("type",'button');
    dropdown_button.setAttribute("class",'btn btn-default btn-sm dropdown-toggle');
    dropdown_button.setAttribute("data-toggle",'dropdown');
    dropdown_button.setAttribute("id",'dropdown_cur_tag_'+index+'_'+nodeid);
    if (!checked)
        dropdown_button.setAttribute("disabled",true);
    dropdown_div.appendChild(dropdown_button);

    var dropdown_span = document.createElement('span');
    dropdown_span.setAttribute("class",'caret');
    dropdown_button.appendChild(dropdown_span);

    var dropdown_ul = document.createElement('ul');
    dropdown_ul.setAttribute("class",'dropdown-menu');
    dropdown_ul.setAttribute("id",'dropdown_tag_'+index+'_'+nodeid);
    dropdown_div.appendChild(dropdown_ul);

    for(var i=TAG_FUNCTION_START_NUM; i< (TAG_FUNCTION_START_NUM + TAG_FUNCTION_NUM);i++)
    {
        var item_index = functions.indexOf(''+i);
        var checked = true;
        if (item_index < 0)
            checked = false;

        var item_li = create_dropdown_tag_item(i, nodeid, index, get_tag_function_name(i), checked);
        dropdown_ul.appendChild(item_li);
    }

    return dropdown_div;
}

function create_dropdown_tag_item(function_index, nodeid, index, FunctionName, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", function_index);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_tag_function_click("+function_index+",\""+ nodeid +"\", "+ index +");return true;");

    // console.log("create_dropdown_tag_item() function_index:"+function_index+" nodeid:"+nodeid+" index:"+index+" FunctionName:"+FunctionName);

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_tag_function_"+function_index+"_"+index+'_'+nodeid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_tag_function_click("+function_index+",\""+ nodeid +"\", "+ index +");return true;");
    item_checkbox.setAttribute("disabled",true);
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);

    var label_item = document.createElement('label');
    if ( defaultchecked )
        label_item.setAttribute("class","text-green");
    else
        label_item.setAttribute("class","text-muted");
    label_item.appendChild(document.createTextNode(" "+FunctionName));
    item_click.appendChild(label_item);

    //item_click.appendChild(document.createTextNode(" "+FunctionName));

    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_tag_function_click = false;
function dropdown_tag_function_click(function_index, nodeid, index)
{
    if(glDropdown_checkbox_tag_function_click)
    {
        glDropdown_checkbox_tag_function_click = false;
        return;
    }

    var ui_elem = document.getElementById("dropdown_checkbox_tag_function_"+function_index+'_'+index+'_'+nodeid);
    var checked = ui_elem.checked;
    //console.log("dropdown_tag_function_click() checked:"+checked);
    if ( checked )
        ui_elem.checked = false;
    else
        ui_elem.checked = true;

    glDropdown_checkbox_tag_function_click = false;
}

function dropdown_checkbox_tag_function_click(function_index, nodeid, index)
{
    glDropdown_checkbox_tag_function_click = true;
}

function create_locatorlist_dropdown_list(nodeid, index, functions, checked)
{
    var dropdown_div = document.createElement('div');
    dropdown_div.setAttribute("class",'btn-group');

    var dropdown_button = document.createElement('button');
    dropdown_button.setAttribute("type",'button');
    dropdown_button.setAttribute("class",'btn btn-default btn-sm dropdown-toggle');
    dropdown_button.setAttribute("data-toggle",'dropdown');
    dropdown_button.setAttribute("id",'dropdown_cur_locator_'+index+'_'+nodeid);
    if (!checked)
        dropdown_button.setAttribute("disabled",true);
    dropdown_div.appendChild(dropdown_button);

    var dropdown_span = document.createElement('span');
    dropdown_span.setAttribute("class",'caret');
    dropdown_button.appendChild(dropdown_span);

    var dropdown_ul = document.createElement('ul');
    dropdown_ul.setAttribute("class",'dropdown-menu');
    dropdown_ul.setAttribute("id",'dropdown_locator_'+index+'_'+nodeid);
    dropdown_div.appendChild(dropdown_ul);

    for(var i=LOCATOR_FUNCTION_START_NUM;i<(LOCATOR_FUNCTION_START_NUM + LOCATOR_FUNCTION_NUM);i++)
    {
        var item_index = functions.indexOf(''+i);
        var checked = true;
        if (item_index < 0)
            checked = false;

        var item_li = create_dropdown_locator_item(i, nodeid, index, get_locator_function_name(i), checked);
        dropdown_ul.appendChild(item_li);
    }

    return dropdown_div;
}

function create_dropdown_locator_item(function_index, nodeid, index, FunctionName, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", function_index);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_locator_function_click("+function_index+",\""+ nodeid +"\", "+ index +");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_locator_function_"+function_index+'_'+index+'_'+nodeid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_locator_function_click("+function_index+",\""+ nodeid +"\", "+ index +");return true;");
    item_checkbox.setAttribute("disabled",true);
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);

    var label_item = document.createElement('label');
    if ( defaultchecked )
        label_item.setAttribute("class","text-green");
    else
        label_item.setAttribute("class","text-muted");
    label_item.appendChild(document.createTextNode(" "+FunctionName));
    item_click.appendChild(label_item);

    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_locator_function_click = false;
function dropdown_locator_function_click(function_index, nodeid, index)
{
    if(glDropdown_checkbox_locator_function_click)
    {
        glDropdown_checkbox_locator_function_click = false;
        return;
    }

    var ui_elem = document.getElementById("dropdown_checkbox_locator_function_"+function_index+'_'+index+'_'+nodeid);
    var checked = ui_elem.checked;
    //console.log("dropdown_tag_function_click() checked:"+checked);
    if ( checked )
        ui_elem.checked = false;
    else
        ui_elem.checked = true;

    glDropdown_checkbox_locator_function_click = false;
}

function dropdown_checkbox_locator_function_click(function_index, nodeid, index)
{
    glDropdown_checkbox_locator_function_click = true;
}

function TargetList_distance_onclick(type, nodeid, distance)
{
    var newValue= prompt("Edit Distance:", distance);
    if ( newValue )
    {
        console.log("TargetList_distance_onclick() newValue:"+newValue);

        var ui = document.getElementById("TargetList_"+type+"_"+nodeid+"_distance");
        ui.innerHTML = newValue;
        ui.setAttribute("onclick","TargetList_distance_onclick("+ type +", \""+ nodeid +"\", \""+ newValue +"\");return false;");
    }
}

function TargetList_title_onclick(type, nodeid, title)
{
    var newValue= prompt("Edit Title:", title);
    if ( newValue )
    {
        console.log("TargetList_title_onclick() newValue:"+newValue);

        var ui = document.getElementById("TargetList_"+type+"_"+nodeid+"_title");
        ui.innerHTML = newValue;
        ui.setAttribute("onclick","TargetList_title_onclick("+ type +", \""+ nodeid +"\", \""+ newValue +"\");return false;");
    }
}

function TargetList_message_onclick(type, nodeid, message)
{
    var newValue= prompt("Edit Message:", message);
    if ( newValue )
    {
        console.log("TargetList_message_onclick() newValue:"+newValue);

        var ui = document.getElementById("TargetList_"+type+"_"+nodeid+"_message");
        ui.innerHTML = newValue;
        ui.setAttribute("onclick","TargetList_message_onclick("+ type +", \""+ nodeid +"\", \""+ newValue +"\");return false;");
    }
}

function save_function(nodeid, type)
{
    var linkEdit = document.getElementById("EDIT_BTN_"+nodeid);
    linkEdit.setAttribute("onclick","edit_function(\""+nodeid+"\", "+type+");return false;");

    var imgEdit = document.getElementById("EDIT_IMG_"+nodeid);
    imgEdit.setAttribute("class","fa fa-edit");

    var functions = [];
    if ( type == DEVICE_TYPE_TAG )
    {
        functions = get_new_functions(nodeid, TAG_FUNCTION_START_NUM, TAG_FUNCTION_NUM);
    }
    else
    if ( type == DEVICE_TYPE_LOCATOR )
    {
        functions = get_new_functions(nodeid, LOCATOR_FUNCTION_START_NUM, LOCATOR_FUNCTION_NUM);
    }
    else
    if ( type == DEVICE_TYPE_EMPLOYEE )
    {
        functions = get_new_functions(nodeid, EMPLOYEE_FUNCTION_START_NUM, EMPLOYEE_FUNCTION_NUM);
    }

    update_PushNotify(nodeid, functions.toString());
}

function get_new_functions(nodeid, start, count)
{
    var functions = [];

    for (var i = start; i < (start + count); i++)
    {
        var input_item = document.getElementById("checkbox_"+i+"_"+nodeid);
        if(input_item.checked)
            functions.push(''+i);
    }

    return functions;
}

function update_PushNotify(nodeid, functions)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['UPDATE_PUSHNOTIFY'] = 1;
    jsondata['FUNCTIONS'] = functions;
    jsondata['ACCOUNT'] = nodeid;

    //console.log("update_PushNotify() nodeid:"+nodeid+", functions:"+functions);

    var targetURL = gl_target_server + "/php/pushnotify.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
            load_device_record();

            if ( response.status == 400 )
            {
                alert("update Error! response = " + JSON.stringify(response));
            }
        },
      error:
        function(xhr, status, error)
        {
            load_device_record();
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });


   return false;
}

function update_TriggerMessage(nodeid, functions)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['UPDATE_TRIGGERMESSAGE'] = 1;
    //jsondata['FUNCTIONS'] = JSON.stringify(functions);
    jsondata['FUNCTIONS'] = functions;
    jsondata['ACCOUNT'] = nodeid;

    console.log("update_TriggerMessage() nodeid:"+nodeid+", functions:"+JSON.stringify(functions));

    var targetURL = gl_target_server + "/php/pushnotify.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
            load_device_record();

            if ( response.status == 400 )
            {
                alert("update Error! response = " + JSON.stringify(response));
            }
        },
      error:
        function(xhr, status, error)
        {
            load_device_record();
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });


   return false;
}

$("#pushnotifyconfigupdatebtn").click(function()
{
    var PushNotifyPrefix = document.getElementById('PushNotifyPrefixInputText').value;
    var PushNotifyWelcomeTitle = document.getElementById('PushNotifyWelcomeTitleInputText').value;
    var PushNotifyWelcomeBody = document.getElementById('PushNotifyWelcomeBodyInputText').value;

    var EnablePushNotify = 0;
    if ( document.getElementById('EnablePushNotifyCheckBtn').checked )
        EnablePushNotify = 1;

    document.getElementById("pushnotifyconfigupdatebtn").disabled = true;

    var SendDate = "";
    SendDate += 'project_id='+project_id;
    SendDate += "&";
    SendDate += "setpushnotifyprefix=" + PushNotifyPrefix;
    SendDate += "&";
    SendDate += "setpushnotifywelcometitle=" + PushNotifyWelcomeTitle;
    SendDate += "&";
    SendDate += "setpushnotifywelcomebody=" + PushNotifyWelcomeBody;
    SendDate += "&";
    SendDate += "setenablepushnotify=" + EnablePushNotify;

    var targetURL = gl_target_server + "/php/pushnotify.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
        },
      error:
        function(xhr, status, error)
        {
            //document.getElementById("syncnode_success").innerText = "";
            //document.getElementById("syncnode_failed").innerText = xhr.responseText;
        },
      complete:
        function()
        {
            check_pushnotifyconfig_update();
        }
    });

    return false;
});


function check_pushnotifyconfig_update()
{
    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadsystemconfig=1'+'&project_id='+project_id, function(data)
    {
        var difference = 0;

        difference += (document.getElementById("PushNotifyPrefixInputText").value != data.PushNotifyPrefix);
        difference += (document.getElementById("PushNotifyWelcomeTitleInputText").value != data.PushNotifyWelcomeTitle);
        difference += (document.getElementById("PushNotifyWelcomeBodyInputText").value != data.PushNotifyWelcomeBody);

        if ( data.EnablePushNotify == "1" && !document.getElementById('EnablePushNotifyCheckBtn').checked )
            difference += 1;
        if ( data.EnablePushNotify == "0" && document.getElementById('EnablePushNotifyCheckBtn').checked )
            difference += 1;

        console.log("check_pushnotifyconfig_update() difference:"+difference);

        if ( difference > 0 )
        {
            document.getElementById("pushnotifyconfig_warning_callout").style.display = 'block';
            setTimeout(check_pushnotifyconfig_update, 1000);
        }
        else
        {
            console.log("check_pushnotifyconfig_update() update successfully!");

            document.getElementById("pushnotifyconfig_warning_callout").style.display = 'none';
            document.getElementById("pushnotifyconfigupdate_callout").style.display = 'block';
            setTimeout(close_callout, 2500, "pushnotifyconfigupdate_callout");

            document.getElementById("pushnotifyconfigupdatebtn").disabled = false;
        }
    });
}

function close_callout(ui_id)
{
    document.getElementById(ui_id).style.display = 'none';
}


function add_search_tag_to_table(searchtext)
{
    var search_list_content = document.getElementById('search_list_content');

    while (search_list_content.rows.length >= 1)
    {
        search_list_content.deleteRow(0);
    }

    //console.log("add_tag_to_table() glTag_List.length:"+glTag_List.length);

    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var ret = node_name.indexOf(searchtext);
        if ( ret < 0 )
            continue;

        var table_tr = document.createElement('tr');
        search_list_content.appendChild(table_tr);

        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_node_name_"+ node_nodeid);
        table_td_2.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_2);

        // node id
        var tag_nodeid_span = document.createElement('span');
        tag_nodeid_span.setAttribute("class",'badge bg-green');
        tag_nodeid_span.textContent = node_name;
        tag_nodeid_span.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_td_2.appendChild(tag_nodeid_span);
    }
}

function loc_tab_search_text_change(e)
{
    var input_value = document.getElementById("loc_tab_search_text").value;

     //console.log("input_value:"+input_value);

     add_search_tag_to_table(input_value);

     setCookie("vilspushnotify_search_text", input_value, 10);
}

function device_qrcode_show(type, device_name, device_macaddress)
{
    console.log('device_qrcode_show() type:'+type+" device_name:"+device_name+" device_macaddress:"+device_macaddress);

    var getType = "TAG";
    if (type === '1')
        getType = "TAG";
    if (type === '2')
        getType = "USER";
    if (type === '3')
        getType = "LOCATOR";


    var device_qrcode_type = document.getElementById("device_qrcode_type");
    device_qrcode_type.textContent = getType;
    var device_qrcode_name = document.getElementById("device_qrcode_name");
    device_qrcode_name.textContent = device_name;
    var device_qrcode_macaddress = document.getElementById("device_qrcode_macaddress");
    device_qrcode_macaddress.textContent = device_macaddress;

    var SendDate = "loadqrcode=1&type="+getType+"&accountid=" + device_macaddress+"&id=" + device_macaddress+'&project_id='+project_id;
    var targetURL = gl_target_server + "/php/configvils.php";

    //console.log("get_qrcode() "+targetURL);

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            //console.log("sync_ERP_account() response:"+response);

            var result = JSON.parse(response);
            //console.log("sync_ERP_account() success:"+result.success+" qrcode:"+result.qrcode);

            var qrcode = document.getElementById("device_qrcode_ui");
            qrcode.setAttribute("src", result.qrcode);

        }
    });

}

function check_cookies()
{
    var input_value = getCookie("vilspushnotify_search_text");

    document.getElementById("loc_tab_search_text").value = input_value;

    add_search_tag_to_table(input_value);
}

const $source = document.querySelector('#loc_tab_search_text');
$source.addEventListener('input', loc_tab_search_text_change);// register for oninput

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

function get_buttontype_name(buttontype)
{
    var buttontype_name = "緊急按鈕";
    if (buttontype == '0')
        buttontype_name = "緊急按鈕";
    else
    if (buttontype == '1')
        buttontype_name = "呼喚鈕";
    else
    if (buttontype == '2')
        buttontype_name = "開關裝置";
    else
    if (buttontype == '3')
        buttontype_name = "訊號採集器";
    return buttontype_name;
}

// function load_PushNotifyUser()
// {
//     if ( document.getElementById("PUSH_NOTIFY_TABLE") == null )
//         return;
//
//     var targetURL = gl_target_server + "/php/configvils.php";
//     var p = $.getJSON(targetURL, 'loadpushnotifyuser=1'+'&project_id='+project_id, function(data)
//     {
//         var notify_table = document.getElementById('PUSH_NOTIFY_TABLE_BODY');
//
//         while (notify_table.rows.length >= 1)
//         {
//             notify_table.deleteRow(0);
//         }
//
//         for (var index in data)
//         {
//             var record = data[index];
//             console.log("index:"+index +" user:"+record.user +" topic:"+record.topic +" functions:"+record.functions+" success:"+record.success);
//
//             var row = create_item(index, record);
//             if (row != null)
//                 notify_table.appendChild(row);
//
//         }
//     })
//     .success(function() {
//     })
//     .error(function() {
//     })
//     .complete(function() {
//     });
// }

function open_pushnotify_page()
{
    var win = window.open('/index.php/vilspushnotify/'+project_id, '_blank');
    win.focus();
}
