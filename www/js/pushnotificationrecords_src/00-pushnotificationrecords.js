/*
 * Author: ikki Chung
 * Date: 2020 01 03
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var UPDATE_INTERVAL = 1000; // update display every 1 seconds

var LOAD_DEVICE_NUM = 100;
var LOAD_PAGE_NUM = 20;

var gl_tag_num = 0;
var gl_anchor_num = 0;
var gl_locator_num = 0;
// var gl_user_num = 0;

var gl_tag_load_index = 0;
var gl_anchor_load_index = 0;
var gl_coord_load_index = 0;
// var gl_user_load_index = 0;

var glTag_List = [];
var glAnchor_List = [];
var glLocator_List = [];
var glUser_List = [];
var glSensorInfo_List = [];

var gl_userdatasubgroups = [];

var glCurrentPage = {};
var glRecord_List = {};
var glResponseImage_List = {};
var glMessageStatus_List = [];

// var glActiveNodeType = '0'; // 0:Tag 1:Locator

var glActiveRecordType = 1; // 1, "手動(群發)"
var glActiveRecordMsgType = '1111'; // "1111", "聯絡簿"

var pre_input_value = '';
var pre_input_date_value = '';

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

                create_dropdown_list();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_devices();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_user();
                load_sensor();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_userdatasubgroup();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_records();
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_records();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_message_status();
            }
        break;
        case 7:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_message_status();

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

function reload_status()
{
    PreLoadIndex = 2;
    CurrentLoadIndex = 3;
    check_loading_status();
}

function create_dropdown_list()
{
    var current_type_title = document.getElementById('current_type_title');
    var dropdown_menu_type = document.getElementById('dropdown_menu_type');

    var span = document.createElement('span');
    span.setAttribute("id","current_type_title_item");
    span.textContent = "卡片點名";
    span.setAttribute("class","badge bg-blue-active");
    current_type_title.appendChild(span);

    dropdown_menu_type.appendChild(create_dropdown_type_item(1, "卡片點名"));
    dropdown_menu_type.appendChild(create_dropdown_type_item(2, "手動(群發)"));
    dropdown_menu_type.appendChild(create_dropdown_type_item(3, "事務流程"));
    dropdown_menu_type.appendChild(create_dropdown_type_item(4, "設備觸發"));
    dropdown_menu_type.appendChild(create_dropdown_type_item(5, "巡檢相關通知"));
    dropdown_menu_type.appendChild(create_dropdown_type_item(6, "不特定對象推播"));
    dropdown_menu_type.appendChild(create_dropdown_type_item(99, "ALL"));

    glActiveRecordType = 1;
    update_dropdown_theme_item(1, "卡片點名");
}

function create_dropdown_type_item(type, name)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", type);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","click_dropdown_theme_item("+ type +", \""+name+"\");return true;");

    var span = document.createElement('span');
    span.textContent = name;
    if ( type == 1 )
        span.setAttribute("class","badge bg-blue-active");
    else
    if ( type == 2 )
        span.setAttribute("class","badge bg-yellow-active");
    else
    if ( type == 3 )
        span.setAttribute("class","badge bg-green-active");
    else
    if ( type == 4 )
        span.setAttribute("class","badge bg-aqua-active");
    else
    if ( type == 5 )
        span.setAttribute("class","badge bg-purple-active");
    else
        span.setAttribute("class","badge bg-red-active");

    item_click.appendChild(span);
    item_li.appendChild(item_click);

    return item_li;
}

function create_dropdown_theme_item(itemtype, type, msgtype, name)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", type);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_theme_item_click(\""+ itemtype +"\", \""+ type +"\", \""+ msgtype +"\", \""+ name +"\");return true;");

    var span = document.createElement('span');
    span.textContent = name;
    if ( type == 1 )
        span.setAttribute("class","badge bg-blue-active");
    else
    if ( type == 2 )
        span.setAttribute("class","badge bg-yellow-active");
    else
    if ( type == 3 )
        span.setAttribute("class","badge bg-green-active");
    else
    if ( type == 4 )
        span.setAttribute("class","badge bg-aqua-active");
    else
    if ( type == 5 )
        span.setAttribute("class","badge bg-purple-active");
    else
    if ( type == 6 )
        span.setAttribute("class","badge bg-blue");
    else
    if ( type == 7 )
        span.setAttribute("class","badge bg-teal-active");
    else
    if ( type == 8 )
        span.setAttribute("class","badge bg-yellow");
    else
    if ( type == 9 )
        span.setAttribute("class","badge bg-teal-dsiabled");
    else
    if ( type == 10 )
        span.setAttribute("class","badge bg-aqua-active");
    else
        span.setAttribute("class","badge bg-red-active");

    item_click.appendChild(span);
    item_li.appendChild(item_click);

    return item_li;
}

function click_dropdown_theme_item(type, name)
{
    glActiveRecordType = type;
    update_dropdown_theme_item(type, name);

    glCurrentPage[glActiveRecordMsgType] = 1;

    reload_status();
}

function update_dropdown_theme_item(type, name)
{
    var current_type_title_item = document.getElementById('current_type_title_item');
    current_type_title_item.textContent = name;

    var current_theme_title = clearChildView('current_theme_title');
    var dropdown_menu_theme = clearChildView('dropdown_menu_theme');

    var spanTheme = document.createElement('span');
    spanTheme.setAttribute("id","current_theme_title_item");
    spanTheme.setAttribute("class","badge bg-blue-active");
    current_theme_title.appendChild(spanTheme);

    if ( type == 1 )
    {
        current_type_title_item.setAttribute("class","badge bg-blue-active");
        spanTheme.textContent = "卡片點名通知";

        glActiveRecordMsgType = '12';
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 1, "12", "卡片點名通知"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 99, "12", "ALL"));
    }
    else
    if ( type == 2 )
    {
        current_type_title_item.setAttribute("class","badge bg-yellow-active");
        spanTheme.textContent = "聯絡簿";

        glActiveRecordMsgType = '201';
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 1, "201", "聯絡簿"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 2, "202", "師生交流"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 3, "203", "電子投票"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 4, "204", "工作指示"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 5, "206", "協助事項回報"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 6, "207", "公布欄"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 7, "205", "不分類"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 99, "201,202,203,204,205,206,207", "ALL"));
    }
    else
    if ( type == 3 )
    {
        current_type_title_item.setAttribute("class","badge bg-green-active");
        spanTheme.textContent = "請假相關申請";

        glActiveRecordMsgType = '2222';
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 1, "2222", "請假相關申請"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 2, "3333", "選課相關申請"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 99, "2222,3333", "ALL"));
    }
    else
    if ( type == 4 )
    {
        current_type_title_item.setAttribute("class","badge bg-aqua-active");
        spanTheme.textContent = "Tag 緊急求助與取消通知";

        glActiveRecordMsgType = '2';
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 1, "2", "Tag 緊急求助與取消通知"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 2, "3", "低電量通知"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 3, "4", "進入(離開)危險區域通知"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 4, "6", "未正常配戴通知"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 5, "7", "進出區域事件"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 6, "9", "GPS 活動超過區域範圍通知"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 7, "102", "Locator 緊急求助與取消通知"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 8, "103", "呼喚取消通知"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 9, "105", "斷電警報"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 10, "106", "週邊模組訊息"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 11, "800", "人員緊急求助"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 99, "2,3,4,6,7,9,102,103,105,106,800", "ALL"));
    }
    else
    if ( type == 5 )
    {
        current_type_title_item.setAttribute("class","badge bg-purple-active");
        spanTheme.textContent = "巡檢相關通知";

        glActiveRecordMsgType = '5';
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 1, "5", "巡檢相關通知"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 99, "5", "ALL"));
    }
    else
    if ( type == 6 )
    {
        current_type_title_item.setAttribute("class","badge bbg-blue");
        spanTheme.textContent = "不特定對象推播";

        glActiveRecordMsgType = '10';
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 1, "10", "不特定對象推播"));
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 99, "10", "ALL"));
    }
    else
    {
        current_type_title_item.setAttribute("class","badge bg-red-active");
        spanTheme.textContent = "ALL";

        glActiveRecordMsgType = '0';
        dropdown_menu_theme.appendChild(create_dropdown_theme_item(type, 99, "0", "ALL"));
    }
}

function dropdown_theme_item_click(itemtype, type, msgtype, name)
{
    update_dropdown_theme_title(itemtype, type, name);

    if ( glActiveRecordMsgType == type )
    {
        return;
    }

    glActiveRecordType = itemtype;
    glActiveRecordMsgType = msgtype;

    var CurrentPage = glCurrentPage[glActiveRecordMsgType];
    if (CurrentPage == undefined)
    {
        glCurrentPage[glActiveRecordMsgType] = 1;
        CurrentPage = 1;
    }

    var current_page_num_ui = document.getElementById('id_current_page_num');
    current_page_num_ui.textContent = CurrentPage;

    reload_status();
}

function update_dropdown_theme_title(itemtype, type, name)
{
    var current_theme_title = clearChildView('current_theme_title');

    var spanTheme = document.createElement('span');
    spanTheme.setAttribute("id","current_theme_title_item");
    spanTheme.textContent = name;
    current_theme_title.appendChild(spanTheme);

    if ( type == 1 )
        spanTheme.setAttribute("class","badge bg-blue-active");
    else
    if ( type == 2 )
        spanTheme.setAttribute("class","badge bg-yellow-active");
    else
    if ( type == 3 )
        spanTheme.setAttribute("class","badge bg-green-active");
    else
    if ( type == 4 )
        spanTheme.setAttribute("class","badge bg-aqua-active");
    else
    if ( type == 5 )
        spanTheme.setAttribute("class","badge bg-purple-active");
    else
    if ( type == 6 )
        spanTheme.setAttribute("class","badge bg-blue");
    else
    if ( type == 7 )
        spanTheme.setAttribute("class","badge bg-teal-active");
    else
    if ( type == 8 )
        spanTheme.setAttribute("class","badge bg-yellow");
    else
    if ( type == 9 )
        spanTheme.setAttribute("class","badge bg-teal-dsiabled");
    else
    if ( type == 10 )
        spanTheme.setAttribute("class","badge bg-aqua-active");
    else
        spanTheme.setAttribute("class","badge bg-red-active");
}

function load_devices()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_anchor_num = parseInt(data['LOC_ANCHOR_NUM']);
        gl_locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        gl_tag_load_index = 0;
        gl_anchor_load_index = 0;
        gl_locator_load_index = 0;

        glTag_List = [];
        glAnchor_List = [];
        glLocator_List = [];
    })
    .success(function() {
        setTimeout(load_tag, 10);
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
        setTimeout(load_anchors, 10);
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
            //var role = nodeData[4];// 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
            glTag_List.push(nodeData);
        }

        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            setTimeout(load_anchors, 10);
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

function load_anchors()
{
    if (gl_anchor_load_index >= gl_anchor_num)
    {
        gl_anchor_load_index = 0;
        setTimeout(load_locator, 10);
        return;
    }
    if (gl_anchor_load_index == 0)
    {
        glAnchor_List = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadanchor=1&s='+gl_anchor_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var anchor_num = parseInt(data['LOC_ANCHOR_NUM']);

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_anchor_load_index; i < gl_anchor_load_index+anchor_num; ++i)
        {
            var key = 'LOC_ANCHOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            // split data
            var node = data[key];
            var nodeData = node.split(",");

            glAnchor_List.push(nodeData);
        }

        gl_anchor_load_index += anchor_num;

    })
    .success(function() {
        if (gl_anchor_load_index >= gl_anchor_num)
        {
            gl_anchor_load_index = 0;
            setTimeout(load_locator, 10);
        }
        else
        {
            setTimeout(load_anchors, 10);
        }
    })
    .error(function() {
        gl_anchor_load_index = 0;
        setTimeout(load_anchors, 1000);
    })
    .complete(function() {
    });

}

function load_locator()
{
    if (gl_locator_load_index >= gl_locator_num)
    {
        gl_locator_load_index = 0;
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
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
            CurrentLoadIndex = CurrentLoadIndex + 1; // next state
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

function load_user()
{
    glUser_List = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduserbaseinfo=1&&project_id='+project_id, function(data)
    {
        $.each(data, function(index, user)
        {
            glUser_List.push(user);
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

function load_sensor()
{
    glSensorInfo_List = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadsensordevicebaseinfo=1&&project_id='+project_id, function(data)
    {
        $.each(data, function(id, device)
        {
            var sensorinfo = {};
            sensorinfo.id = id;
            sensorinfo.name = device.name;
            glSensorInfo_List.push(sensorinfo);
        });
    })
    .success(function() {
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
            group.subgroupid = parseInt(group.subgroupid);
            gl_userdatasubgroups.push(group);
        });
    })
    .success(function() {

        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_records()
{
    var input_value = document.getElementById("search_notify_target_text").value;
    // console.log("load_records() input_value:"+input_value);

    var input_date_value = document.getElementById("search_notify_date_text").value;
    // console.log("load_records() input_date_value:"+input_date_value);

    pre_input_value = input_value;
    pre_input_date_value = input_date_value;

    var targetURL = gl_target_server + "/php/pushnotify.php";
    var CurrentPage = glCurrentPage[glActiveRecordMsgType];
    if (CurrentPage == undefined)
    {
        glCurrentPage[glActiveRecordMsgType] = 1;
        CurrentPage = 1;
    }

    var offset = (CurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    glRecord_List[glActiveRecordMsgType] = [];

    var querytext = 'loadrecords=1'+'&project_id='+project_id+"&offset="+offset+"&count="+count+"&type="+glActiveRecordMsgType;

    if(input_value.length > 0)
    {
        querytext = querytext + '&searchtarget=' + input_value;
    }
    if(input_date_value.length > 0)
    {
        querytext = querytext + '&searchdate=' + input_date_value;
    }

    $.getJSON(targetURL, querytext, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            glRecord_List[glActiveRecordMsgType].push(record);
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

function show_records()
{
    var table_body_ui = document.getElementById('table_content');

    //console.log("show_records() id:"+'loc_tab_'+glActiveNodeType+'_'+glActiveRecordMsgType+'_BODY');

    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    var CurrentPage = glCurrentPage[glActiveRecordMsgType];

    for(var index=0;index<glRecord_List[glActiveRecordMsgType].length;index++)
    {
        show_record(table_body_ui, (CurrentPage-1)*LOAD_PAGE_NUM + index+1, glRecord_List[glActiveRecordMsgType][index]);
    }
}

function show_record(table_body_ui, ui_index, record)
{
    var cellIndex = 0;

    var row = document.createElement('tr');
    row.setAttribute("id", record.dbId+"_TABLE_ROW");
    table_body_ui.appendChild(row);

    var row_extra = document.createElement('tr');
    row_extra.setAttribute("id", record.dbId+"_TABLE_ROW_EXTRA");
    table_body_ui.appendChild(row_extra);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    if ( record.surveyids.length > 0 || record.imageuid.length > 0 )
    {
        var div_info_href_dl = document.createElement("a");
        div_info_href_dl.setAttribute("id",record.dbId +"_message_detail_button");
        div_info_href_dl.setAttribute("title","顯示詳細資訊");
        div_info_href_dl.setAttribute("onclick","open_message_detail(\""+record.dbId+"\", \""+record.messagehashkey+"\",\""+record.surveyids+"\",\""+record.hashkeys+"\",\""+record.imageuid+"\");");
        div_info_href_dl.setAttribute("class","btn btn-default btn-xs pull-right");

        var div_info_href_dl_img = document.createElement("i");
        div_info_href_dl_img.setAttribute("id",record.dbId +"_message_detail_icon");
        div_info_href_dl_img.setAttribute("class","fa fa-plus text-green");
        div_info_href_dl.appendChild(div_info_href_dl_img);
        row.cells[cellIndex].appendChild(div_info_href_dl);
    }
    cellIndex++;

    // 推播類型
    row.appendChild(document.createElement('td'));
    var ui_type = create_UI_record_type_ids(record.type, record.targetids);
    row.cells[cellIndex].appendChild(ui_type);
    cellIndex++;

    // 觸發裝置
    row.appendChild(document.createElement('td'));
    var ui_type = create_UI_record_device(record.nodeid);
    row.cells[cellIndex].appendChild(ui_type);
    cellIndex++;

    // 推播對象
    row.appendChild(document.createElement('td'));
    var ui_targetids = create_UI_record_devices(record.targetids);
    row.cells[cellIndex].appendChild(ui_targetids);
    var ui_subgroups = create_UI_record_subgroups(record.subgroupids);
    row.cells[cellIndex].appendChild(ui_subgroups);
    cellIndex++;

    // 已讀取
    row.appendChild(document.createElement('td'));
    var ui_read_div = document.createElement("div");
    ui_read_div.setAttribute("id",record.messagehashkey +"_message_read");
    row.cells[cellIndex].appendChild(ui_read_div);
    cellIndex++;

    // Title
    row.appendChild(document.createElement('td'));
    var SpanTitle = document.createElement("span");
    //SpanTitle.setAttribute("class","badge bg-fuchsia");
    SpanTitle.textContent = record.title;
    row.cells[cellIndex].appendChild(SpanTitle);
    cellIndex++;

    // 訊息內容
    row.appendChild(document.createElement('td'));
    var SpanMessage = document.createElement("span");
    //SpanMessage.setAttribute("class","badge bg-fuchsia");
    SpanMessage.textContent = record.message;
    row.cells[cellIndex].appendChild(SpanMessage);
    cellIndex++;

    // 時間
    row.appendChild(document.createElement('td'));
    var badge_date = document.createElement("span");
    badge_date.setAttribute("class","label label-info");
    badge_date.textContent = record.datetime;
    row.cells[cellIndex].appendChild(badge_date);
    cellIndex++;

    // 操作
    row.appendChild(document.createElement('td'));
    var resend_btn = create_resend_button(record);
    //var a_resend = document.createElement("a");
    //badge_date.setAttribute("class","label label-info");
    //badge_date.textContent = record.datetime;
    row.cells[cellIndex].appendChild(resend_btn);
    cellIndex++;

}

function load_message_status()
{
    glMessageStatus_List = [];

    var queryhashkeys = '';
    var queryCount = 0;
    for(var i=0;i<glRecord_List[glActiveRecordMsgType].length;i++)
    {
        var record = glRecord_List[glActiveRecordMsgType][i];
        if ( queryCount > 0 )
            queryhashkeys += ',';
        queryhashkeys += record.messagehashkey;
        queryCount += 1;
    }

    var targetURL = gl_target_server + "/php/pushnotify.php";

    var querytext = 'loadmessagestatus=1'+'&project_id='+project_id+'&hashkeys='+queryhashkeys+"&loadaccount=1";

    $.getJSON(targetURL, querytext, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            glMessageStatus_List.push(record);
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

function show_message_status()
{
    for(var i=0;i<glMessageStatus_List.length;i++)
    {
        var record = glMessageStatus_List[i];
        if ( record.readed == '1' )
        {
            var ui_read_div = document.getElementById(record.messagehashkey +"_message_read");
            var ui_span = create_UI_record_device(record.account);

            ui_read_div.appendChild(ui_span);
        }
    }
}

function create_UI_record_type_ids(type, nodeids)
{
    var nodeidList = nodeids.split(",");

    if (nodeidList.length > 0)
    {
        return create_UI_record_type(type, nodeidList[0]);
    }
}

function create_UI_record_type(type, nodeid)
{
    var ui_span = document.createElement("span");

    // TAG_PUSHNOTIFY_FUNCTION_1 = 1, // 非設備文字訊息, 如醫囑等
    // TAG_PUSHNOTIFY_FUNCTION_2,     // 緊急求助與取消通知
    // TAG_PUSHNOTIFY_FUNCTION_3,     // 低電量通知
    // TAG_PUSHNOTIFY_FUNCTION_4,     // 進入(離開)危險區通知, 身份為工作人員時不通知
    // TAG_PUSHNOTIFY_FUNCTION_5,     // 巡檢相關通知, 身份為巡檢員時才有通知
    // TAG_PUSHNOTIFY_FUNCTION_6,     // 未正常配戴通知
    // TAG_PUSHNOTIFY_FUNCTION_7,     // 進出區域事件, 需要在事件列表設定才有通知
    // TAG_PUSHNOTIFY_FUNCTION_8,     // 課程點名通知, 包含上課中進出通知
    // TAG_PUSHNOTIFY_FUNCTION_9,       // GPS活動超過區域範圍通知
    // TAG_PUSHNOTIFY_FUNCTION_10       // 不特定對象推播
    // TAG_PUSHNOTIFY_FUNCTION_11,       // 周邊模組訊息，如不明物體移動、煙霧探測
    // TAG_PUSHNOTIFY_FUNCTION_12       // RFID 卡片點名

    // LOCATOR_PUSHNOTIFY_FUNCTION_1 = 101, // 非設備文字訊息, 如醫囑等, 手動發送
    // LOCATOR_PUSHNOTIFY_FUNCTION_2,     // 緊急求助與取消通知
    // LOCATOR_PUSHNOTIFY_FUNCTION_3,     // 呼喚與取消通知
    // LOCATOR_PUSHNOTIFY_FUNCTION_4,     // 醫生進出病房與病床區域通知
    // LOCATOR_PUSHNOTIFY_FUNCTION_5,     // 斷電警報
    // LOCATOR_PUSHNOTIFY_FUNCTION_6     // 周邊模組訊息，如不明物體移動、煙霧探測

    // CARD_PUSHNOTIFY_FUNCTION_1 = 500 // 卡片點名

    // USER_PUSHNOTIFY_FUNCTION_1 = 800 // 人員緊急求助

    switch(type)
    {
        default:
            ui_span.textContent = '未知 tag type='+type;
        break;
        case '1111':
            ui_span.textContent = '手動群發';
        break;
        case '1':
            ui_span.textContent = '手動發送';
        break;
        case '2':
            ui_span.textContent = '緊急求助與取消通知';
            //ui_span.textContent = 'Tag事件觸發';
        break;
        case '3':
            ui_span.textContent = '低電量通知';
        break;
        case '4':
            ui_span.textContent = '進入(離開)危險區通知';
        break;
        case '5':
            ui_span.textContent = '巡檢相關通知';
        break;
        case '6':
            ui_span.textContent = '未正常配戴通知';
        break;
        case '7':
            ui_span.textContent = '進出區域事件';
        break;
        case '8':
            ui_span.textContent = '課程點名通知';
        break;
        case '9':
            ui_span.textContent = 'GPS活動超過區域範圍通知';
        break;
        case '10':
            ui_span.textContent = '不特定對象推播';
        break;
        case '12':
            ui_span.textContent = '人員點名';
        break;
        case '2222':
            ui_span.textContent = '請假相關申請';
        break;
        case '3333':
            ui_span.textContent = '選課相關申請';
        break;
        case '201':
            ui_span.textContent = '聯絡簿';
        break;
        case '202':
            ui_span.textContent = '師生交流';
        break;
        case '203':
            ui_span.textContent = '電子投票';
        break;
        case '204':
            ui_span.textContent = '工作指示';
        break;
        case '205':
            ui_span.textContent = '不分類';
        break;
        case '206':
            ui_span.textContent = '協助事項回報';
        break;
        case '207':
            ui_span.textContent = '公布欄';
        break;
        case '101':
            ui_span.textContent = '手動發送';
        break;
        case '102':
            ui_span.textContent = '緊急求助與取消通知';
        break;
        case '103':
            ui_span.textContent = '呼喚與取消通知';
        break;
        case '104':
            ui_span.textContent = '醫生進出病房與病床區域通知';
        break;
        case '105':
            ui_span.textContent = '斷電警報';
        break;
        case '106':
            ui_span.textContent = '周邊模組訊息';
        break;
        case '500':
            ui_span.textContent = '卡片點名';
        break;
        case '800':
            ui_span.textContent = '人員緊急求助';
        break;
    }

    return ui_span;
}

function create_UI_record_devices(nodeids)
{
    var ui_div = document.createElement("div");
    var nodeidList = nodeids.split(",");

    // console.log("create_UI_record_devices() nodeids:"+nodeids+" nodeidList.length:"+nodeidList.length +" gl_groupid:"+gl_groupid +" accountid:"+accountid);

    for(var index=0;index<nodeidList.length;index++)
    {
        var nodeid = nodeidList[index];

        // console.log("create_UI_record_devices() nodeid:"+nodeid);

        if ( gl_groupid == 3 )
        {
            if ( nodeid === gl_accountid )
            {
                // console.log("create_UI_record_devices() nodeid === accountid");

                var ui_span = create_UI_record_device(nodeid);
                ui_div.appendChild(ui_span);
            }
            else
            if ( nodeid === gl_userid )
            {
                var ui_span = create_UI_record_device(nodeid);
                ui_div.appendChild(ui_span);
            }
        }
        else
        {
            var ui_span = create_UI_record_device(nodeid);
            ui_div.appendChild(ui_span);
        }
    }

    return ui_div;
}

function create_UI_record_device(nodeid)
{
    var ui_span = document.createElement("span");

    if ( nodeid.length == 0 )
    {
        return ui_span;
    }

    var node = get_tag_node(nodeid);
    var node_type = 0;// 0:tag 1:anchor 2:locator 3:user

    var nodename = nodeid;
    var role = '0';

    if (node == null)
    {
        node = get_locator_node(nodeid);
        if (node == null)
        {
            node = get_anchor_node(nodeid);

            if (node == null)
            {
                node = get_user_node(nodeid);
                if (node == null)
                {
                    node = get_sensor_node(nodeid);
                    if (node != null)
                    {
                        node_type = 5;// 0:tag 1:anchor 2:locator 3:user 5:sensor
                        ui_span.setAttribute("class","badge bg-green");
                        nodename = node.name;
                        role = '0';
                    }
                }
                else
                {
                    node_type = 3;// 0:tag 1:anchor 2:locator 3:user
                    ui_span.setAttribute("class","badge bg-aqua-active");
                    nodename = node.name;
                    role = '0';
                }
            }
            else
            {
                node_type = 1;// 0:tag 1:anchor 2:locator
                ui_span.setAttribute("class","badge bg-green");
                nodename = node[5];
                role = node[4];
            }
        }
        else
        {
            node_type = 2;// 0:tag 1:anchor 2:locator
            ui_span.setAttribute("class","badge bg-yellow-active");
            nodename = node[5];
            role = node[4];
        }
    }
    else
    {
        ui_span.setAttribute("class","badge bg-light-blue-active");
        nodename = node[5];
        role = node[4];
    }

    if (node == null)
    {
        console.log("create_UI_record_device() nodeid:"+nodeid+"  node == null");
        //ui_span.textContent = nodeid;
        return ui_span;
    }

    // console.log("create_UI_record_device() nodeid:"+nodeid+"  node_type:"+node_type+" nodename:"+nodename +" role:"+role);

    //
    // 工作人員
    //
    if ( (node_type == 0) && (role === '4') )
    {
        ui_span.textContent = nodename+'/工作人員';
    }
    else
    {
        ui_span.textContent = nodename;
    }

    return ui_span;
}

function create_UI_record_subgroups(subgroupids)
{
    var ui_div = document.createElement("div");
    var subgroupList = subgroupids.split(",");

    for (var iId = 0; iId < subgroupList.length; iId++)
    {
        var subgroupid = parseInt(subgroupList[iId]);

        var bFound = false;
        for (var i = 0; i < gl_userdatasubgroups.length; i++)
        {
            var cur_subgroup = gl_userdatasubgroups[i];

            if ( cur_subgroup.subgroupid == subgroupid )
            {
                var ui_span = document.createElement("span");
                ui_span.setAttribute("class","badge bg-yellow-active");
                ui_span.textContent = cur_subgroup.subgroupname;
                ui_div.appendChild(ui_span);
                bFound = true;
                break;
            }
        }
    }
    return ui_div;
}

function create_resend_button(record)
{
     var resend_btn = document.createElement("button");
     resend_btn.setAttribute("type","button");
     resend_btn.setAttribute("class","btn btn-default");
     resend_btn.setAttribute("title","重發一次");
     resend_btn.setAttribute("onclick","open_pushnotification_page("+record.dbId+");return false;");

     var imgLink = document.createElement("i");
     imgLink.setAttribute("class","fa fa-external-link");
     imgLink.setAttribute("title","重發一次");
     resend_btn.appendChild(imgLink);

     return resend_btn;
}

function open_message_detail(dbId, messagehashkey, surveyids, hashkeys, imageuid)
{
    // console.log("open_message_detail()  messagehashkey:"+messagehashkey+" surveyids:"+surveyids +" hashkeys:"+hashkeys+" imageuid:"+imageuid);

    var div_info_href_dl = document.getElementById(dbId+"_message_detail_button");
    div_info_href_dl.setAttribute("title","關閉詳細資訊");
    div_info_href_dl.setAttribute("onclick","close_message_detail(\""+dbId+"\", \""+messagehashkey+"\",\""+surveyids+"\",\""+hashkeys+"\",\""+imageuid+"\");");

    var div_info_href_dl_img = document.getElementById(dbId+"_message_detail_icon");
    div_info_href_dl_img.setAttribute("class","fa fa-minus text-red");

    var row_extra = document.getElementById(dbId+"_TABLE_ROW_EXTRA");
    row_extra.setAttribute("bgcolor","#ebebe0");

    // index
    row_extra.appendChild(document.createElement('td'));

    var rd_notify_image = document.createElement('td')
    //rd_notify_image.setAttribute("id","notify_image_"+dbId);
    rd_notify_image.setAttribute("colspan","2");
    row_extra.appendChild(rd_notify_image);

    var row_notify_image = document.createElement('div')
    row_notify_image.setAttribute("class","row");
    row_notify_image.setAttribute("id","notify_image_"+dbId);
    rd_notify_image.appendChild(row_notify_image);

    var rd_reponse_image = document.createElement('td')
    rd_reponse_image.setAttribute("id","reponse_image_"+dbId);
    rd_reponse_image.setAttribute("colspan","2");
    row_extra.appendChild(rd_reponse_image);

    var rd_surveys = document.createElement('td')
    //rd_surveys.setAttribute("id","surveys_"+dbId);
    rd_surveys.setAttribute("colspan","3");
    row_extra.appendChild(rd_surveys);

    if (surveyids.length > 0)
    {
        // survey record table
        var table_survey = document.createElement('table')
        table_survey.setAttribute("class","table table-bordered");
        rd_surveys.appendChild(table_survey);

        var table_head_survey = document.createElement('thead')
        table_survey.appendChild(table_head_survey);

        var table_head_tr_survey = document.createElement('tr')
        table_survey.setAttribute("style","background-color:rgb(236, 240, 245);");
        table_head_survey.appendChild(table_head_tr_survey);

        var table_head_th_survey_1 = document.createElement('th')
        table_head_th_survey_1.textContent = '代辦事項名稱';
        table_head_tr_survey.appendChild(table_head_th_survey_1);

        var table_head_th_survey_2 = document.createElement('th')
        table_head_th_survey_2.textContent = '事項列表';
        table_head_tr_survey.appendChild(table_head_th_survey_2);

        var table_head_th_survey_3 = document.createElement('th')
        table_head_th_survey_3.textContent = '操作';
        table_head_tr_survey.appendChild(table_head_th_survey_3);

        var table_body_survey = document.createElement('tbody')
        table_body_survey.setAttribute("id","surveys_"+dbId);
        table_survey.appendChild(table_body_survey);
    }


    load_notify_image(dbId, imageuid);

    load_reponse_image(dbId, messagehashkey);

    load_surveys(dbId, surveyids, hashkeys);

    load_survey_results(dbId, hashkeys);
}

function close_message_detail(dbId, messagehashkey, surveyids, hashkeys, imageuid)
{
    //console.log("close_message_detail()  messagehashkey:"+messagehashkey+" surveyids:"+surveyids +" hashkeys:"+hashkeys+" imageuid:"+imageuid);

    var div_info_href_dl = document.getElementById(dbId+"_message_detail_button");
    div_info_href_dl.setAttribute("title","顯示詳細資訊");
    div_info_href_dl.setAttribute("onclick","open_message_detail(\""+dbId+"\", \""+messagehashkey+"\",\""+surveyids+"\",\""+hashkeys+"\",\""+imageuid+"\");");

    var div_info_href_dl_img = document.getElementById(dbId+"_message_detail_icon");
    div_info_href_dl_img.setAttribute("class","fa fa-plus text-green");

    clearChildView(dbId+"_TABLE_ROW_EXTRA");
}

function load_notify_image(dbId, imageuid)
{
    // console.log("load_notify_image() dbId:"+dbId+" imageuid:"+imageuid);

    var imageuidlist = imageuid.split(",");

    // console.log("load_notify_image() imageuidlist.length:"+imageuidlist.length);

    for(var i=0;i<imageuidlist.length;i++)
    {
        var uid = imageuidlist[i];

        if ( uid.length > 0 )
        {
            load_PushNotify_image(dbId, uid);
        }
    }
}

function load_PushNotify_image(dbId, imageuid)
{
    //console.log("load_PushNotify_image() dbId:"+dbId+" imageuid:"+imageuid);

    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadsingleimage=1&project_id='+project_id+'&imageuid='+imageuid, function(storageImage)
    {

        var ui_id = 'show_img_area_'+imageuid;
        var show_img_area_image_ui = document.getElementById(ui_id);
        if ( show_img_area_image_ui == undefined )
        {

            // console.log("load_PushNotify_image() show_img_area_image_ui("+ show_img_area_image_ui +")  == undefined");

            var show_img_area = document.getElementById("notify_image_"+dbId);

            var col_filetype = create_data_image("推播發送照片", dbId, storageImage);
            col_filetype.setAttribute("id", ui_id);
            show_img_area.appendChild(col_filetype);
        }
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

//var col_filetype = create_data_image(storageImage.id, "使用者回傳照片", storageImage.description, storageImage.thumbpath, storageImage.imagetype, storageImage.imagesize);

function create_data_image(title, dbId, storageImage)
{
    var col = document.createElement("div");
    col.setAttribute("class","col-xs-12 col-md-6 col-lg-4");

    var data_ul = document.createElement("ul");
    //data_ul.setAttribute("style","list-style-type:none;background-color:blue;");
    data_ul.setAttribute("class","mailbox-attachments clearfix");
    col.appendChild(data_ul);

    // var data_li_title = document.createElement("li");
    // data_li_title.setAttribute("style","background-color:white;");
    // data_ul.appendChild(data_li_title);

    var data_li = document.createElement("li");
    data_li.setAttribute("style","background-color:white;");
    data_ul.appendChild(data_li);

    var span_title = document.createElement("span");
    span_title.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    span_title.textContent = title;
    //span_title.appendChild(document.createTextNode("  " + title));
    data_li.appendChild(span_title);

    var item_user_span = document.createElement("span");
    data_li.appendChild(item_user_span);

    var ui_type = create_UI_record_devices(storageImage.accountid);
    item_user_span.appendChild(ui_type);


    var div_datetime_info = document.createElement("div");
    data_li.appendChild(div_datetime_info);

    var badge_date = document.createElement("span");
    badge_date.setAttribute("class","label label-info");
    badge_date.textContent = convertPrettyDateOnly(storageImage.updatetime, "+00:00");
    div_datetime_info.appendChild(badge_date);

    var badge_time = document.createElement("span");
    badge_time.setAttribute("class","label label-warning");
    badge_time.textContent = convertPrettyTimeOnly(storageImage.updatetime, "+00:00");
    div_datetime_info.appendChild(badge_time);

    var span_icon = document.createElement("span");
    span_icon.setAttribute("class","mailbox-attachment-icon has-img");
    data_li.appendChild(span_icon);

    var span_icon_img = document.createElement("img");
    span_icon_img.setAttribute("src","/php/downloadfile.php?loadthumb=1&project_id="+project_id+"&imageid="+storageImage.id);
    span_icon_img.setAttribute("alt","Attachment");
    span_icon.appendChild(span_icon_img);

    var div_info = document.createElement("div");
    div_info.setAttribute("class","mailbox-attachment-info");
    data_li.appendChild(div_info);

    var div_info_href = document.createElement("a");
    div_info_href.setAttribute("href","/php/downloadfile.php?project_id="+project_id+"&imageid="+storageImage.id);
    div_info_href.setAttribute("class","mailbox-attachment-name");
    div_info.appendChild(div_info_href);

    // var div_info_href_img = document.createElement("i");
    // div_info_href_img.setAttribute("class","fa fa-camera");
    // div_info_href.appendChild(div_info_href_img);

    div_info_href.appendChild(document.createTextNode("  " + storageImage.description));

    var div_info_href_size = document.createElement("span");
    div_info_href_size.setAttribute("class","mailbox-attachment-size");
    div_info_href_size.textContent = convertFileSize(parseInt(storageImage.imagesize));
    div_info_href.appendChild(div_info_href_size);

    var div_info_href_dl = document.createElement("a");
    div_info_href_dl.setAttribute("href","/php/downloadfile.php?project_id="+project_id+"&imageid="+storageImage.id);
    div_info_href_dl.setAttribute("class","btn btn-default btn-xs pull-right");
    div_info_href_size.appendChild(div_info_href_dl);

    var div_info_href_dl_img = document.createElement("i");
    div_info_href_dl_img.setAttribute("class","fa fa-cloud-download");
    div_info_href_dl.appendChild(div_info_href_dl_img);

    return col;
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
    var reponse_image_ui = document.getElementById("reponse_image_"+dbId);

    var storageImage_List = glResponseImage_List[dbId];

    for(var index=0;index<storageImage_List.length;index++)
    {
        var storageImage = storageImage_List[index];

        console.log("show_reponse_image() description:"+storageImage.description);

        // var thumbimagelink = gl_target_server + "/php/downloadfile.php?imageuid="+ storageImage.imageuid +"&loadthumb=1"

        // console.log("show_reponse_image() thumbimagelink:"+thumbimagelink);

        var image_id = "reponse_image_" + dbId +"_" + storageImage.imageuid;

        var image_ui = document.getElementById(image_id);
        if (image_ui == undefined)
        {
            // row_ui.setAttribute("src", thumbimagelink);
            //var image_ui = create_reponse_image_item(dbId, storageImage);
            image_ui = document.createElement('span');
            image_ui.setAttribute("id","reponse_image_" + dbId +"_" + storageImage.imageuid);

            reponse_image_ui.appendChild(image_ui);
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

    //var col_filetype = create_data_image(storageImage.id, "使用者回傳照片", storageImage.description, storageImage.thumbpath, storageImage.imagetype, storageImage.imagesize);
    var col_filetype = create_data_image("使用者回傳照片", dbId, storageImage);
    col_filetype.setAttribute("id",image_id);
    reponse_image_ui.appendChild(col_filetype);
}

function load_surveys(dbId, surveyids, hashkeys)
{
    //console.log("load_surveys() surveyids:"+surveyids);

    var surveyids_id = "surveys_" + dbId;
    var table_body_survey = document.getElementById(surveyids_id);
    if (table_body_survey == undefined)
    {
        return;
    }

    var surveyidlist = surveyids.split(",");
    var hashkeylist = hashkeys.split(",");

    for(var i=0;i<surveyidlist.length;i++)
    {
        var surveyid = surveyidlist[i];
        var hashkey = hashkeylist[i];

        var survey_ui = create_survey(dbId, surveyid, hashkey);
        table_body_survey.appendChild(survey_ui);

        load_survey_to_ui(dbId, surveyid);
    }
}

function create_survey(dbId, surveyid, hashkey)
{
    var cellIndex = 0;

    var row = document.createElement('tr');
    //row.setAttribute("id", dbId+"_SURVEY_TABLE_ROW");

    // 事項名稱
    row.appendChild(document.createElement('td'));
    var titleDiv = document.createElement('div');
    titleDiv.setAttribute("id",dbId+"_"+surveyid+"_title");
    row.cells[cellIndex].appendChild(titleDiv);
    cellIndex++;

    // 事項列表
    row.appendChild(document.createElement('td'));
    var survey_item = document.createElement('div');
    survey_item.setAttribute("id",dbId+"_"+surveyid+"_item");
    row.cells[cellIndex].appendChild(survey_item);
    cellIndex++;

    // 操作
    row.appendChild(document.createElement('td'));

    var checkBtn = document.createElement("button");
    checkBtn.setAttribute("title","查看詳細資訊");
    checkBtn.setAttribute("href","javascript:;");
    //checkBtn.setAttribute("id",dbId+"_"+surveyid+"_surveyresult");
    checkBtn.setAttribute("onclick","show_surveyresult("+ project_id +",\""+surveyid+"\",\""+hashkey+"\");return false;");
    var imgCheck = document.createElement("i");
    imgCheck.setAttribute("class","fa fa-calendar-check-o ");
    checkBtn.appendChild(imgCheck);
    row.cells[cellIndex].appendChild(checkBtn);

    cellIndex++;

    return row;
}

function load_survey_to_ui(dbId, surveyid)
{
    var targetURL = gl_target_server + "/php/survey.php";

    glSurvey_List = [];

    $.getJSON(targetURL, 'loadsurvey=1'+'&project_id='+project_id+'&surveyid='+surveyid, function(data)
    {
        $.each(data, function(index, survey)
        {
            //console.log("survey.title:"+survey.title);

            var titleDiv = document.getElementById(dbId+"_"+surveyid+"_title");
            if (titleDiv != undefined)
            {
                titleDiv.textContent = survey.title;
            }

            var survey_item_Div = document.getElementById(dbId+"_"+surveyid+"_item");
            if (survey_item_Div != undefined)
            {
                var survey_item = show_survey_item(survey.surveyid, survey.items, dbId);
                survey_item_Div.appendChild(survey_item);
            }
        });
    })
    .success(function() {

    })
    .error(function() {

    })
    .complete(function() {
    });
}

function show_survey_item(surveyid, items, dbId)
{
    var survey_item = document.createElement('div');
    survey_item.setAttribute("id","survey_item_"+surveyid);

    var survey_table = document.createElement('table');
    survey_item.appendChild(survey_table);

    var survey_tablehead = document.createElement('thead');
    survey_table.appendChild(survey_tablehead);

    var survey_tablehead_tr = document.createElement('tr');
    survey_tablehead.appendChild(survey_tablehead_tr);

    var survey_tablehead_th_0 = document.createElement('th');
    survey_tablehead_th_0.setAttribute("style","width: 40px");
    survey_tablehead_tr.appendChild(survey_tablehead_th_0);

    var survey_tablehead_th_1 = document.createElement('th');
    survey_tablehead_tr.appendChild(survey_tablehead_th_1);

    var survey_tablebody = document.createElement('tbody');
    survey_tablebody.setAttribute("id","survey_table_body_"+surveyid);
    survey_table.appendChild(survey_tablebody);

    for(var index=0;index<items.length;index++)
    {
        var item = items[index];
        var survey_tr = document.createElement('tr');
        survey_tablebody.appendChild(survey_tr);

        var survey_td_0 = document.createElement('td');
        survey_tr.appendChild(survey_td_0);

        var survey_td_0_span = document.createElement("span");
        survey_td_0_span.setAttribute("class","label label-danger");
        survey_td_0_span.setAttribute("id","survey_item_"+dbId+"_"+index);
        survey_td_0_span.textContent = '0';
        survey_td_0.appendChild(survey_td_0_span);

        // console.log("show_survey_item() id:survey_item_"+dbId+"_"+index);

        var survey_td_1 = document.createElement('td');
        survey_tr.appendChild(survey_td_1);
        survey_td_1.appendChild(document.createTextNode(item.description));
    }

    // comment
    var survey_tr = document.createElement('tr');
    survey_tablebody.appendChild(survey_tr);

    var survey_td_0 = document.createElement('td');
    survey_tr.appendChild(survey_td_0);

    survey_td_0.appendChild(document.createTextNode("說明:"));

    var survey_td_comment = document.createElement('td');
    survey_td_comment.setAttribute("id","survey_item_comment_"+dbId);
    survey_tr.appendChild(survey_td_comment);

    return survey_item;
}

function load_survey_results(dbId, hashkeys)
{
    var targetURL = gl_target_server + "/php/survey.php";

    var SurveyResult_List = [];

    $.getJSON(targetURL, 'loadsurveyresults=1'+'&project_id='+project_id+"&hashkeys="+hashkeys, function(data)
    {
        $.each(data, function(hashkey, record)
        {
            var result = {};
            result.hashkey = hashkey;
            result.record = record;
            SurveyResult_List.push(result);
        });

        show_surveyresults(dbId, SurveyResult_List);

    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function show_surveyresults(dbId, SurveyResult_List)
{
    for(var index=0;index<SurveyResult_List.length;index++)
    {
        var surveyResult = SurveyResult_List[index];

        var UserResults = [];
        var AllResults = [];
        var AllComments = [];

        for(var rindex=0;rindex<surveyResult.record.length;rindex++)
        {
            var record = surveyResult.record[rindex];
            var result = record.results.split(",");

            if ( record.comment.length > 0 )
                AllComments.push(record.comment);

            if (UserResults[record.topic] == undefined)
            {
                UserResults[record.topic] = 1;
            }
            else
            {
                continue;
            }

            for(var i=0;i<result.length;i++)
            {
                var data = parseInt(result[i]);
                if (AllResults[i] == undefined)
                    AllResults[i] = 0;
                AllResults[i] += data;
            }
        }
        // console.log("show_surveyresults() AllResults.length:"+AllResults.length);

        for(var rindex=0;rindex<AllResults.length;rindex++)
        {
            // console.log("show_surveyresults() rindex:"+rindex+" AllResults="+AllResults[rindex]);
            // console.log("show_surveyresults() id:survey_item_"+dbId+"_"+rindex);

            var survey_td_0_span = document.getElementById("survey_item_"+dbId+"_"+rindex);
            if (survey_td_0_span != undefined)
            {
                survey_td_0_span.textContent = AllResults[rindex];
                if (AllResults[rindex] == 0)
                {
                    survey_td_0_span.setAttribute("class","label label-danger");
                }
                else
                {
                    survey_td_0_span.setAttribute("class","label label-info");
                }
            }
            // else
            // {
            //     console.log("show_surveyresults() survey_td_0_span == undefined");
            // }
        }

        var survey_td_comment = document.getElementById("survey_item_comment_"+dbId);
        if ( survey_td_comment == null )
        {
            continue;
        }

        while (survey_td_comment.firstChild)
        {
            survey_td_comment.removeChild(survey_td_comment.firstChild);
        }

        for(var rindex=0;rindex<AllComments.length;rindex++)
        {
            if (survey_td_comment != undefined)
            {
                var survey_td_comment_span = document.createElement("span");
                survey_td_comment_span.setAttribute("class","label label-primary");
                survey_td_comment.appendChild(survey_td_comment_span);
                survey_td_comment_span.textContent = AllComments[rindex];
            }
        }
    }

}

function show_surveyresult(project_id, surveyid, hashkey)
{
    var win = window.open('/index.php/vilssurveyresult/'+project_id+'?surveyid='+surveyid+'&hashkey='+hashkey, '_blank');
    win.focus();
}

function get_tag_node(nodeid)
{
    var node = null;

    for(var i=0;i<glTag_List.length;i++)
    {
        var nodeData = glTag_List[i];
        //var nodeid = nodeData[3];
        //var nodename = nodeData[5];
        //var macaddress = nodeData[6];
        if ( nodeid === nodeData[3] || nodeid === nodeData[6] )
        {
            node = nodeData;
            break;
        }
    }

    return node;
}

function get_locator_node(nodeid)
{
    var node = null;

    for(var i=0;i<glLocator_List.length;i++)
    {
        var nodeData = glLocator_List[i];
        //var nodeid = nodeData[3];
        //var nodename = nodeData[5];
        //var macaddress = nodeData[6];
        if ( nodeid === nodeData[3] || nodeid === nodeData[6] )
        {
            node = nodeData;
            break;
        }
    }

    return node;
}

function get_anchor_node(nodeid)
{
    var node = null;

    for(var i=0;i<glAnchor_List.length;i++)
    {
        var nodeData = glAnchor_List[i];
        //var nodeid = nodeData[3];
        //var nodename = nodeData[5];
        //var macaddress = nodeData[6];
        if ( nodeid === nodeData[3] || nodeid === nodeData[6] )
        {
            node = nodeData;
            break;
        }
    }

    return node;
}

function get_user_node(nodeid)
{
    var node = null;

    for(var i=0;i<glUser_List.length;i++)
    {
        var userData = glUser_List[i];

        //console.log("get_user_node() userData.account:"+userData.account +" nodeid:"+nodeid);

        if ( nodeid === userData.account )
        {
            node = userData;
            break;
        }

        // check userid
        if ( !isNaN(nodeid) )
        {
            var user_id = parseInt(nodeid);
            if ( user_id == userData.userid )
            {
                node = userData;
                break;
            }
        }
    }

    return node;
}

function get_sensor_node(nodeid)
{
    var node = null;

    for(var i=0;i<glSensorInfo_List.length;i++)
    {
        var sensorInfo = glSensorInfo_List[i];
        if ( nodeid === sensorInfo.id )
        {
            node = sensorInfo;
            break;
        }
    }

    return node;
}

// function getUserNameByTopic(topic)
// {
//     var username = '';
//     for(var index=0;index<glSurveyResult_List.length;index++)
//     {
//         var surveyResult = glSurveyResult_List[index];
//
//         for(var rindex=0;rindex<surveyResult.record.length;rindex++)
//         {
//             var record = surveyResult.record[rindex];
//
//             if ( topic === record.topic )
//             {
//                 username = record.name;
//                 break;
//             }
//         }
//         if (username.length > 0)
//             break;
//     }
//     return username;
// }

function move_page_backward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    var CurrentPage = glCurrentPage[glActiveRecordMsgType];

    if ( CurrentPage <= 1 )
    {
        // do nothing
    }
    else
    {
        CurrentPage--;
        current_page_num_ui.textContent = CurrentPage;

        glCurrentPage[glActiveRecordMsgType] = CurrentPage;

        PreLoadIndex = 2;
        CurrentLoadIndex = 3;
        check_loading_status();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    var CurrentPage = glCurrentPage[glActiveRecordMsgType];

    CurrentPage++;
    current_page_num_ui.textContent = CurrentPage;
    glCurrentPage[glActiveRecordMsgType] = CurrentPage;

    PreLoadIndex = 2;
    CurrentLoadIndex = 3;
    check_loading_status();
}

function open_pushnotification_page(dbId)
{
    for(var i=0;i<glRecord_List[glActiveRecordMsgType].length;i++)
    {
        var record = glRecord_List[glActiveRecordMsgType][i];
        if( record.dbId == dbId )
        {
            if ( parent.$('#rightcontentIframe')[0] != undefined )
                parent.send_pushnotification(project_id, record.nodeid, record.title, record.message);
            else
                window.open("/index.php/vilspushnotification/"+project_id+"?nodeid="+record.nodeid+"&title="+record.title+"&message="+record.message,"_self")
            break;
        }
    }
}

var gl_search_start = 0;
function search_notify_target_text_change()
{
    var input_value = document.getElementById("search_notify_target_text").value;

    if ( input_value === pre_input_value )
    {
        console.log("search_notify_target_text_change() input_value === pre_input_value, input_value:"+input_value);
        return;
    }

    glCurrentPage[glActiveRecordMsgType] = 1;

    gl_search_start = 0;
    reload_status();

    return false;
}

function check_search_notify_target_text(e)
{
    if ( gl_search_start == 0 )
    {
        gl_search_start = new Date();
    }

    var cur_time = new Date();
    var seconds = (cur_time.getTime() - gl_search_start.getTime()) / 1000;

    // console.log("check_search_text_change() seconds:"+seconds);

    if ( CurrentLoadIndex != LOAD_STATE_END )
    {
        console.log("check_search_notify_target_text() CurrentLoadIndex != LOAD_STATE_END");

        setTimeout(search_notify_target_text_change, 500);
        return;
    }

    if ( seconds >= 1 )
    {
        gl_search_start = 0;
        search_notify_target_text_change();
    }
    else
    {
        setTimeout(search_notify_target_text_change, 500);
    }
}


var gl_search_date_start = 0;
function search_notify_date_text_change()
{
    var input_date_value = document.getElementById("search_notify_date_text").value;

    if ( input_date_value === pre_input_date_value )
    {
        console.log("search_notify_date_text_change() input_date_value === pre_input_date_value, input_date_value:"+input_date_value);
        return;
    }

    glCurrentPage[glActiveRecordMsgType] = 1;

    gl_search_date_start = 0;
    reload_status();

    return false;
}

function check_search_notify_date_text(e)
{
    if ( gl_search_date_start == 0 )
    {
        gl_search_date_start = new Date();
    }

    var cur_time = new Date();
    var seconds = (cur_time.getTime() - gl_search_date_start.getTime()) / 1000;

    // console.log("check_search_text_change() seconds:"+seconds);

    if ( CurrentLoadIndex != LOAD_STATE_END )
    {
        console.log("check_search_notify_date_text() CurrentLoadIndex != LOAD_STATE_END");

        setTimeout(search_notify_date_text_change, 500);
        return;
    }

    if ( seconds >= 1 )
    {
        gl_search_date_start = 0;
        search_notify_date_text_change();
    }
    else
    {
        setTimeout(search_notify_date_text_change, 500);
    }
}

const $source = document.querySelector('#search_notify_target_text');
$source.addEventListener('input', check_search_notify_target_text);// register for oninput

const $source_date = document.querySelector('#search_notify_date_text');
$source_date.addEventListener('input', check_search_notify_date_text);// register for oninput


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

function convertPrettyDateOnly(inTime, offset)
{
    var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset(offset);
    var year = cur_date.format('YYYY');
    var month = cur_date.format('M');
    var day = cur_date.format('D');

    var perty_str = "";
    perty_str = year + "年" + month + "月" + day + "日 ";
    return perty_str;
}

function convertPrettyTimeOnly(inTime, offset)
{
    var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset(offset);
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var perty_str = "";
    if (hour >= 0)
        perty_str += hour + "點";
    if (minutes >= 0)
        perty_str += minutes + "分";
    if (seconds >= 0)
        perty_str += seconds + "秒";

    return perty_str;
}

function open_pushnotificationrecords_page()
{
    var win = window.open('/index.php/vilspushnotificationrecords/'+project_id, '_blank');
    win.focus();
}
