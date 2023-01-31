/*
 * Author: ikki Chung
 * Date: 2020 10 13
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var NODE_TYPE_SUBGROUP = 4;

var gl_userdatasubgroups = [];

var gl_send_list = [];
var gl_uploadimage_uid = '';
var glCannedMessages_type = 207;// 手動群發 -> 公布欄

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

                load_userdatasubgroup();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                click_subgroup("");

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;


                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 3:
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

        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
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

    return table_tr;
}

function create_subgroup_label(context)
{
    var subgroup_span = document.createElement("span");
    subgroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    subgroup_span.textContent = context;
    return subgroup_span;
}

function click_subgroup(subgroupid)
{
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

function update_send_list()
{
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

    update_send_list_ui();
}

function add_send_node_list(nodeid, nodename, nodetype)
{
    var bFound = false;
    for (var i = 0; i < gl_send_list.length; i++)
    {
        var send_node = gl_send_list[i];
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
        gl_send_list.push(send_node);
    }
}

function delete_send_node_list(nodeid, nodename, nodetype)
{
    for (var i = 0; i < gl_send_list.length; i++)
    {
        var send_node = gl_send_list[i];
        if ( (send_node.nodeid == nodeid) && (send_node.node_name == nodename) &&(send_node.node_type == nodetype) )
        {
            gl_send_list.splice(i, 1);
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

    for (var i = 0; i < gl_send_list.length; i++)
    {
        var send_node = gl_send_list[i];
        add_send_node(send_node_list_ui, send_node.node_name, send_node.node_type);

        if ( i>0 && i%10 == 0 )
        {
            var divspace = document.createElement("div");
            send_node_list_ui.appendChild(divspace);
        }
    }
}

function add_send_node(send_node_list_ui, node_name, nodetype)
{
    var nodeid_span = document.createElement('span');

    if (nodetype == NODE_TYPE_SUBGROUP)
    {
        nodeid_span = create_subgroup_label(node_name);
    }

    send_node_list_ui.appendChild(nodeid_span);

    // space
    var Spanspace = document.createElement("span");
    Spanspace.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;");
    Spanspace.textContent = "  ";
    send_node_list_ui.appendChild(Spanspace);

}

$("#savebillboardbtn").click(function()
{
    var send_title = document.getElementById("inputTitle").value;
    var send_context = document.getElementById("inputContext").value;

    if ( send_title.length == 0 )
    {
        alert("請輸入公告標題!");
        return false;
    }

    if ( send_title.length > 100 )
    {
        alert("請縮短公告標題, 100字以內. 目前字數:"+send_title.length);
        return false;
    }

    if ( send_context.length == 0 )
    {
        alert("請輸入公告內容!");
        return false;
    }

    if ( send_context.length > 3000 )
    {
        alert("請縮短公告內容, 3000字以內.. 目前字數:"+send_context.length);
        return false;
    }

    var subgroup_list = [];

    for (var i = 0; i < gl_send_list.length; i++)
    {
        var send_node = gl_send_list[i];
        if (send_node.node_type == NODE_TYPE_SUBGROUP)
            subgroup_list.push(send_node.nodeid);
    }

    if (subgroup_list.length == 0)
    {
        alert("請選擇公告對象!");
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
        check_send_notification();
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

    var extFile = filename[1].toLowerCase();
    if (extFile == "jpg" || extFile == "jpeg" || extFile == "png" || extFile == "bmp")
    {
        //accepted
    }
    else
    {
        alert("請選擇照片格式 .jpg .jpeg .png .bmp ");
    }

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
            setTimeout(check_send_notification, 300);
        },
      error:
        function(xhr, status, error)
        {
            setTimeout(check_send_notification, 300);
        }
    });
}

function check_send_notification()
{
    var send_Notify = document.getElementById("sendNotify").checked;

    // console.log("check_send_notification() send_Notify:"+send_Notify);

    create_billboard();

    if (send_Notify)
    {
        send_notification();
    }
    else
    {
        alert("已新增公告!");
    }
}

function send_notification()
{
    var send_title = document.getElementById("inputTitle").value;
    var send_context = document.getElementById("inputContext").value;

    var subgroup_list = [];
    var target_list = [];
    var survey_list = [];
    var deadline_list = [];

    for (var i = 0; i < gl_send_list.length; i++)
    {
        var send_node = gl_send_list[i];
        if (send_node.node_type == NODE_TYPE_SUBGROUP)
            subgroup_list.push(send_node.nodeid);
    }


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
    jsondata['title'] = send_title;
    jsondata['context'] = send_context;
    jsondata['type'] = glCannedMessages_type;
    jsondata['imageuid'] = gl_uploadimage_uid;

    document.getElementById("savebillboardbtn").disabled = true;
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
            document.getElementById("savebillboardbtn").disabled = false;
        }
    });
}

function create_billboard()
{
    var billboard_title = document.getElementById("inputTitle").value;
    var billboard_content = document.getElementById("inputContext").value;
    var send_Notify = document.getElementById("sendNotify").checked;
    var sendpushnotify = 0;
    if (send_Notify)
        sendpushnotify = 1;

    var subgroup_list = [];

    for (var i = 0; i < gl_send_list.length; i++)
    {
        var send_node = gl_send_list[i];
        if (send_node.node_type == NODE_TYPE_SUBGROUP)
            subgroup_list.push(send_node.nodeid);
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['createbillboard'] = '1';
    jsondata['sendpushnotify'] = sendpushnotify;
    jsondata['subgrouplist'] = subgroup_list.toString();
    jsondata['title'] = billboard_title;
    jsondata['content'] = billboard_content;
    jsondata['imageuid'] = gl_uploadimage_uid;

    var targetURL = gl_target_server + "/php/pushnotify.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
        },
      error:
        function(xhr, status, error)
        {
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
            document.getElementById("savebillboardbtn").disabled = false;
        }
        else
        if ( data.success === 'false')
        {

            document.getElementById("sendnotification_failed_callout_msg").innerHTML = "<p>發出推播 失敗! "+data.msg+"</p>"

            document.getElementById("sendnotification_sending_callout").style.display = 'none';
            document.getElementById("sendnotification_failed_callout").style.display = 'block';
            document.getElementById("sendnotification_finished_callout").style.display = 'none';
            setTimeout(close_callout, 10000, "sendnotification_failed_callout");
            document.getElementById("savebillboardbtn").disabled = false;
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

function open_billboard_page()
{
    var win = window.open('/index.php/vilsbillboard/'+project_id, '_blank');
    win.focus();
}
