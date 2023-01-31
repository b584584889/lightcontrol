/*
 * Author: ikki Chung
 * Date: 2020.10.15
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_usermaingroup_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var glCurrentPage = 1;

var gl_user_info = [];
var gl_usermaingroups = [];

var gl_edit_maingroupid = 0;
var gl_edit_maingroup_imageuid = '';
var gl_uploadimage_uid = '';

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

                load_usermaingroup();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_user_info();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                draw_table();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_maingroup_photo();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                // update_user_info();

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

function reload_maingroup_info()
{
    PreLoadIndex = -1;
    CurrentLoadIndex = 0;
    check_loading_status();
}

function load_usermaingroup()
{
    var offset = (glCurrentPage - 1) * LOAD_usermaingroup_NUM;
    var count = LOAD_usermaingroup_NUM;

    gl_usermaingroups = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadmaingroup=1&s='+offset+'&count='+count+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, usermaingroup)
        {
            gl_usermaingroups.push(usermaingroup);
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

function load_user_info()
{
    var queryuser = '';

    var queryCount = 0;
    for (var i = 0; i < gl_usermaingroups.length; ++i)
    {
        var maingroup = gl_usermaingroups[i];

        var useridList = maingroup.userids.split(',');
        for (var idIndex = 0; idIndex < useridList.length; ++idIndex)
        {
            var userid = useridList[idIndex];
            if (userid.length > 0)
            {
                var bFound = check_exist_user(userid);

                if ( bFound == false)
                {
                    if ( queryCount > 0 )
                        queryuser += ',';
                    queryuser += userid;
                    queryCount += 1;
                }
            }
        }

        // var accountidList = maingroup.accountids.split(',');
        // for (var idIndex = 0; idIndex < accountidList.length; ++idIndex)
        // {
        //     var accountid = accountidList[idIndex];
        //     if (accountid.length > 0)
        //     {
        //         var bFound = check_exist_account(accountid);
        //
        //         if ( bFound == false)
        //         {
        //             if ( queryCount > 0 )
        //                 queryuser += ',';
        //             queryuser += accountid;
        //             queryCount += 1;
        //         }
        //     }
        // }
    }

    if (queryuser.length == 0)
    {
        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    //var p = $.getJSON(targetURL, 'loadusername=1&account='+queryuser+'&project_id='+project_id, function(data)
    var p = $.getJSON(targetURL, 'loadusername=1&user='+queryuser+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, user)
        {
            user.userid = parseInt(user.userid);
            gl_user_info.push(user);
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

function check_exist_user(userid)
{
    var target_userid = parseInt(userid);
    //
    // check exist
    //
    var bFound = false;
    for (var j = 0; j < gl_user_info.length; ++j)
    {
        var existed_user = gl_user_info[j];
        if ( existed_user.userid == target_userid )
        {
            bFound = true;
            break;
        }
    }
    return bFound;
}

// function check_exist_account(accountid)
// {
//     //
//     // check exist
//     //
//     var bFound = false;
//     for (var j = 0; j < gl_user_info.length; ++j)
//     {
//         var existed_user = gl_user_info[j];
//         if ( existed_user.accountid === accountid )
//         {
//             bFound = true;
//             break;
//         }
//     }
//     return bFound;
// }

function draw_table()
{
    var record_table = document.getElementById('usermaingroupinfo_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

     console.log("draw_table() gl_usermaingroups.length:"+gl_usermaingroups.length);

    for (var i = 0; i < gl_usermaingroups.length; ++i)
    {
        var group = gl_usermaingroups[i];

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
    tdgroupid.setAttribute("id","uiid_groupname_"+record.id);
    row.appendChild(tdgroupid);

    var Spangroupid = create_subgroup_label(record.name);
    row.cells[cellIndex].appendChild(Spangroupid);
    cellIndex++;

    // 說明
    var tdpermission = document.createElement('td');
    tdpermission.setAttribute("id","uiid_permission_"+record.id);
    tdpermission.textContent = record.permission;
    row.appendChild(tdpermission);
    cellIndex++;

    // 歡迎詞
    var tdwelcommessage = document.createElement('td');
    tdwelcommessage.setAttribute("id","uiid_welcommessage_"+record.id);
    tdwelcommessage.textContent = record.welcommessage;
    row.appendChild(tdwelcommessage);
    cellIndex++;

    // 照片
    var tdpermission = document.createElement('td');
    tdpermission.setAttribute("id","uiid_photo_"+record.id);
    row.appendChild(tdpermission);
    cellIndex++;

    // 自行加入
    var tdautojoin = document.createElement('td');
    tdautojoin.setAttribute("id","uiid_autojoin_"+record.id);
    if ( record.autojoin == '0' )
        tdautojoin.textContent = '不允許自行加入';
    else
    if ( record.autojoin == '1' )
        tdautojoin.textContent = '使用識別碼與帳號加入';
    else
    if ( record.autojoin == '2' )
        tdautojoin.textContent = '可自行加入';

    row.appendChild(tdautojoin);
    cellIndex++;

    // 子集合管理者
    var tdmanager = document.createElement('td');
    //tdmanager.setAttribute("id","uiid_accountids_"+record.id);
    // var accountids = createAccountIDUI(record.accountids);
    tdmanager.setAttribute("id","uiid_userids_"+record.id);
    var userids = createUserIDUI(record.userids);
    tdmanager.appendChild(userids);
    row.appendChild(tdmanager);
    cellIndex++;

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    var btnQrcode = document.createElement("button");
    btnQrcode.setAttribute("type","button");
    btnQrcode.setAttribute("class","btn btn-default");
    btnQrcode.setAttribute("data-toggle","modal");
    btnQrcode.setAttribute("data-target","#modal-qrcode");
    btnQrcode.setAttribute("onclick","device_qrcode_show(\"MAINGROUP\",\""+ record.id +"\",\""+ record.name +"\");return false;");

    var imgQrcode = document.createElement("i");
    imgQrcode.setAttribute("class","fa fa-qrcode");
    btnQrcode.appendChild(imgQrcode);
    row.cells[cellIndex].appendChild(btnQrcode);

    // Edit button
    var editBtn = document.createElement("button");
    editBtn.setAttribute("id","uiid_editbtn_"+record.id);
    editBtn.setAttribute("title","編輯此主集合");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_usermaingroup(\""+record.id+"\");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("id","uiid_editimg_"+record.id);
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    // delete uesr button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("style","display:none");
    deleteBtn.setAttribute("id","uiid_deletebtn_"+record.id);
    deleteBtn.setAttribute("title","刪除此主集合");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","confirm_delete_usermaingroup(\""+record.id+"\", \""+ record.name +"\");return false;");
    var imgDelete = document.createElement("i");
    imgDelete.setAttribute("class","fa fa-user-times");
    deleteBtn.appendChild(imgDelete);
    row.cells[cellIndex].appendChild(deleteBtn);
    cellIndex++;

    return row;
}

function device_qrcode_show(type, maingroupid, maingroupname)
{
    var int_maingroupid = parseInt(maingroupid);

    var device_qrcode_type = document.getElementById("device_qrcode_type");
    device_qrcode_type.textContent = 'Type: 主集合';
    var device_qrcode_name = document.getElementById("device_qrcode_name");
    device_qrcode_name.textContent = '名稱: ' + maingroupname;
    var device_qrcode_macaddress = document.getElementById("device_qrcode_macaddress");
    device_qrcode_macaddress.textContent = '主集合ID: ' + int_maingroupid;

    var SendDate = "loadqrcode=1&type="+type+"&accountid=" + int_maingroupid+"&id=" + int_maingroupid+'&project_id='+project_id;
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

            var device_qrcode_target = document.getElementById("device_qrcode_target");
            device_qrcode_target.textContent = 'target: ' + result.target;

            var device_qrcode_topic = document.getElementById("device_qrcode_topic");
            device_qrcode_topic.textContent = 'topic: ' + result.topic;

            var device_qrcode_datetime = document.getElementById("device_qrcode_datetime");
            device_qrcode_datetime.textContent = '更新時間: ' + result.datetime;

        }
    });

}

function show_maingroup_photo()
{
    for (var i = 0; i < gl_usermaingroups.length; ++i)
    {
        var group = gl_usermaingroups[i];

        if ( group.imageuid.length > 0 )
        {
            load_maingroup_photo(group.id, group.imageuid);
        }
    }
}

function load_maingroup_photo(maingroupid, imageuid)
{
    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadsingleimage=1&project_id='+project_id+'&imageuid='+imageuid, function(storageImage)
    {
        var ui_id = 'uiid_photo_'+maingroupid;
        var show_img_area_image_ui = document.getElementById(ui_id);
        if ( show_img_area_image_ui != undefined )
        {
            // console.log("load_user_photo() storageImage.accountid:"+ storageImage.accountid +" storageImage.id:"+storageImage.id);
            // var user_photo = create_user_photo(storageImage);
            var user_photo = create_image_item(storageImage);
            show_img_area_image_ui.appendChild(user_photo);
        }
    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function create_user_photo(storageImage)
{
    var span_icon_img = document.createElement("img");
    span_icon_img.setAttribute("src","/php/downloadfile.php?loadthumb=1&project_id="+project_id+"&imageid="+storageImage.id);
    span_icon_img.setAttribute("alt",storageImage.description);
    span_icon_img.setAttribute("style","max-width: 64px;");
    return span_icon_img;
}

function create_image_item(storageimage)
{
    //var col = document.createElement("div");
    //col.setAttribute("class","col-xs-6 col-md-4 col-lg-2");

    var ul_image = create_data_image(storageimage.id, storageimage.name, storageimage.thumbpath, storageimage.imagetype, storageimage.imagesize);
    //col.appendChild(ul_image);

    return ul_image;
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
    span_icon_img.setAttribute("style","height:120px; width:auto; max-height:120px; max-width:auto;");
    span_icon.appendChild(span_icon_img);

    var div_info = document.createElement("div");
    div_info.setAttribute("class","mailbox-attachment-info");
    data_li.appendChild(div_info);

    var div_info_href = document.createElement("a");
    div_info_href.setAttribute("href","/php/downloadfile.php?project_id="+project_id+"&imageid="+id);
    div_info_href.setAttribute("class","mailbox-attachment-name");
    div_info.appendChild(div_info_href);

    // var div_info_href_img = document.createElement("i");
    // div_info_href_img.setAttribute("class","fa fa-camera");
    // div_info_href.appendChild(div_info_href_img);

    var fileanme_label = document.createElement("p");
    fileanme_label.setAttribute("class","overflow-ellipsis");
    fileanme_label.setAttribute("style","width: 180px;white-space: nowrap;overflow: hidden;");
    fileanme_label.textContent = filename;
    div_info_href.appendChild(fileanme_label);

    var div_info_href_size = document.createElement("span");
    div_info_href_size.setAttribute("class","mailbox-attachment-size");
    div_info_href_size.setAttribute("style","text-align: center;");
    div_info_href_size.textContent = convertFileSize(parseInt(filesize));
    div_info.appendChild(div_info_href_size);

    // var div_info_href_detail = document.createElement("a");
    // div_info_href_detail.setAttribute("class","btn btn-default btn-xs pull-left");
    // div_info_href_size.appendChild(div_info_href_detail);

    // var div_info_href_detail_img = document.createElement("i");
    // div_info_href_detail_img.setAttribute("title","查看詳細資訊");
    // div_info_href_detail_img.setAttribute("onclick","open_detail_image(\""+id+"\");return false;");
    // div_info_href_detail_img.setAttribute("class","fa fa-search-plus");
    // div_info_href_detail.appendChild(div_info_href_detail_img);

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

function createUserIDUI(userids)
{
    var td_userids_div = document.createElement("div");

    var useridList = userids.split(",");

    for(var i=0;i<useridList.length;i++)
    {
        var userid = useridList[i];

        var userName = getUserName(userid);

        if ( userName.length == 0 )
        {
            continue;
        }

        var user_div = document.createElement("div");
        td_userids_div.appendChild(user_div);

        var user_span = create_account_label(userName);
        user_div.appendChild(user_span);
    }

    return td_userids_div;
}

// function createAccountIDUI(accountids)
// {
//     var td_accountids_div = document.createElement("div");
//
//     var accountidList = accountids.split(",");
//
//     for(var i=0;i<accountidList.length;i++)
//     {
//         var accountid = accountidList[i];
//
//         var accountName = getAccountName(accountid);
//
//         if ( accountName.length == 0 )
//         {
//             continue;
//         }
//
//         var account_div = document.createElement("div");
//         td_accountids_div.appendChild(account_div);
//
//         var account_span = create_account_label(accountName);
//         account_div.appendChild(account_span);
//     }
//
//     return td_accountids_div;
// }

function create_empty_record_table_item(ui_index)
{
    var empty_record_table_item = document.getElementById('empty_record_table_item');
    if ( empty_record_table_item != null )
        return;

    var record_table = document.getElementById('usermaingroupinfo_TABLE');
    var row = record_table.insertRow(0);
    row.setAttribute("bgcolor","#ebebe0");

    //var row = document.createElement('tr');
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

    // 子集合管理者
    row.appendChild(document.createElement('td'));
    var ui_manager_dropdown_list = create_manager_dropdown_list("input_accountids");
    row.cells[cellIndex].appendChild(ui_manager_dropdown_list);
    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_usermaingroup();");
    Button_save.textContent = "儲存";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;

    //record_table.appendChild(row);
}

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.textContent = text;

    return input_text;
}

function create_manager_dropdown_list(uiid)
{
    var ui_manager_dropdown_list = document.createElement("div");
    ui_manager_dropdown_list.setAttribute("class","btn-group");

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "管理人員";
    ui_manager_dropdown_list.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_ul.setAttribute("id","dropdown_managerd");
    ui_manager_dropdown_list.appendChild(div_ul);

    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];

        if ( user.groupid != '3' )
        {
            div_ul.appendChild(create_dropdown_user_item("new", user.name, user.account, false));
        }
    }
    return ui_manager_dropdown_list;
}


function create_dropdown_user_item(groupid, name, account, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", account);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_user_click(\""+groupid+"\", \""+account+"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_"+groupid+"_"+account);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_user_click(\""+groupid+"\", \""+account+"\");return true;");
    item_checkbox.checked = defaultchecked;
    item_click.appendChild(item_checkbox);

    item_click.appendChild(document.createTextNode(" "+name));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_user_click = false;
function dropdown_checkbox_user_click(groupid, account)
{
    glDropdown_checkbox_user_click = true;

    //update_selected_type(account);
}

function dropdown_user_click(groupid, account)
{
    if (glDropdown_checkbox_user_click == true)
    {
        glDropdown_checkbox_user_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_"+groupid+"_"+account).checked == true)
        document.getElementById("dropdown_checkbox_"+groupid+"_"+account).checked = false;
    else
        document.getElementById("dropdown_checkbox_"+groupid+"_"+account).checked = true;

    //update_selected_type(type, userid, index);

    glDropdown_checkbox_user_click = false;
}

function edit_usermaingroup(groupid)
{
    console.log("edit_usermaingroup() groupid:"+groupid);

    var group = getusermaingroup(groupid);
    if (group == null)
    {
        console.log("edit_usermaingroup() group == null");
        return;
    }

    create_edit_ui("uiid_groupname", groupid, group.name);
    create_edit_ui("uiid_permission", groupid, group.permission);
    create_edit_ui("uiid_welcommessage", groupid, group.welcommessage);

    create_edit_autojoin_ui("uiid_autojoin", groupid, group.autojoin);

    //create_edit_ui("uiid_accountids", groupid, group.accountids);
    if (gl_groupid == 1 || gl_groupid == 2 || gl_groupid == 4)
    {
        create_edit_manager_ui("uiid_userids", groupid, group.userids);
        var deleteBtn = document.getElementById("uiid_deletebtn_"+groupid);
        deleteBtn.setAttribute("style","display:block");
    }

    gl_edit_maingroup_imageuid = group.imageuid;
    var show_img_area_image_ui = document.getElementById('uiid_photo_'+groupid);
    if ( show_img_area_image_ui != undefined )
    {
        if ( gl_edit_maingroup_imageuid.length == 0 )
        {
            clearChildView('uiid_photo_'+groupid);
            var div_upload = create_upload_image_ui();
            show_img_area_image_ui.appendChild(div_upload);
        }
        else
        {
            //
            // create delete button
            //
            var deletePhotoBtn = document.createElement("button");
            deletePhotoBtn.setAttribute("style","color: #AF4C50;");
            deletePhotoBtn.setAttribute("title","Delete Maingroup Photo");
            deletePhotoBtn.setAttribute("href","javascript:;");
            deletePhotoBtn.setAttribute("onclick","delete_maingroup_photo(\""+groupid+"\");return false;");
            var imgdelete = document.createElement("i");
            imgdelete.setAttribute("class","fa fa-remove");
            deletePhotoBtn.appendChild(imgdelete);
            show_img_area_image_ui.appendChild(deletePhotoBtn);
        }
    }

    var editBtn = document.getElementById("uiid_editbtn_"+groupid);
    editBtn.setAttribute("title","儲存此主集合");
    editBtn.setAttribute("onclick","save_usermaingroup(\""+groupid+"\");return false;");
    var imgEdit = document.getElementById("uiid_editimg_"+groupid);
    imgEdit.setAttribute("class","fa fa-save");

}

function delete_maingroup_photo(groupid)
{
    gl_edit_maingroup_imageuid = "";

    clearChildView('uiid_photo_'+groupid);
    var show_img_area_image_ui = document.getElementById('uiid_photo_'+groupid);
    if ( show_img_area_image_ui != undefined )
    {
        var div_upload = create_upload_image_ui();
        show_img_area_image_ui.appendChild(div_upload);
    }
}

function create_edit_ui(uiid, groupid, text)
{
    var uiid_name = clearChildView(uiid+"_"+groupid);
    var new_input = create_input_text(uiid+"_"+groupid+"_edit", text);
    uiid_name.appendChild(new_input);
}

function create_edit_autojoin_ui(uiid, groupid, autojoin)
{
    var ui_autojoin = clearChildView(uiid+"_"+groupid);

    var selectelement = document.createElement('div');
    ui_autojoin.appendChild(selectelement);

    var selectList = document.createElement("select");
    selectList.id = uiid+"_"+groupid+"_edit";
    selectelement.appendChild(selectList);

    var option1 = document.createElement("option");
    option1.value = '0';
    option1.text = '不允許自行加入';
    if (autojoin == '0')
        option1.setAttribute("selected", "selected");
    selectList.appendChild(option1);

    var option2 = document.createElement("option");
    option2.value = '1';
    option2.text = '使用識別碼與帳號加入';
    if (autojoin == '1')
        option2.setAttribute("selected", "selected");
    selectList.appendChild(option2);

    var option3 = document.createElement("option");
    option3.value = '2';
    option3.text = '可自行加入';
    if (autojoin == '2')
        option3.setAttribute("selected", "selected");

    selectList.appendChild(option3);
}

function create_edit_manager_ui(uiid, groupid, userids)
{
    var ui_userids = clearChildView(uiid+"_"+groupid);

    var ui_manager_dropdown_list = document.createElement("div");
    ui_manager_dropdown_list.setAttribute("class","btn-group");
    if (ui_userids != null)
        ui_userids.appendChild(ui_manager_dropdown_list);

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "管理人員";
    ui_manager_dropdown_list.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_ul.setAttribute("id","dropdown_managerd");
    ui_manager_dropdown_list.appendChild(div_ul);

    var useridList = userids.split(",");

    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];
        var userid_str = user.userid+'';

        var defaultchecked = (useridList.indexOf(userid_str) >= 0 );
        div_ul.appendChild(create_dropdown_user_item(groupid, user.name, user.account, defaultchecked));

        // if ( user.groupid != '3' )
        // {
        //     //
        //     // 檢查使用者是否擁有 此集合 權限
        //     //
        //     var datagroupList = user.datagroups.split(",");
        //     var existed = check_numer_in_list(parseInt(groupid), datagroupList);
        //     if ( existed )
        //     {
        //         var defaultchecked = (accountidList.indexOf(user.account) >= 0 );
        //         div_ul.appendChild(create_dropdown_user_item(groupid, user.name, user.account, defaultchecked));
        //     }
        // }
    }

    return ui_userids;
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

function getusermaingroup(groupid)
{
    var retusermaingroup = null;
    for (var i = 0; i < gl_usermaingroups.length; ++i)
    {
        var group = gl_usermaingroups[i];

        if ( group.id === groupid )
        {
            retusermaingroup = group;
            break;
        }
    }
    return retusermaingroup;
}

function create_subgroup_label(context)
{
    var subgroup_span = document.createElement("span");
    subgroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    subgroup_span.textContent = context;
    return subgroup_span;
}

function create_account_label(context)
{
    var account_span = document.createElement("span");
    account_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(54, 127, 169);");
    account_span.textContent = context;
    return account_span;
}

function save_new_usermaingroup()
{
    var input_groupname = document.getElementById("input_groupname").value;
    var input_permission = document.getElementById("input_permission").value;
    //var input_accountids = document.getElementById("input_accountids").value;

    if ( input_groupname.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    var input_accountids = '';
    var input_userids = '';

    if (gl_groupid == 1 || gl_groupid == 2 || gl_groupid == 4)
    {
        for (var i = 0; i < gl_user_info.length; ++i)
        {
            var user = gl_user_info[i];

            if ( user.groupid != '3' )
            {
                var checkbox = document.getElementById("dropdown_checkbox_new_"+user.account);
                if (checkbox != undefined)
                {
                    if (checkbox.checked == true)
                    {
                        if ( input_accountids.length > 0 )
                            input_accountids = input_accountids + ',';
                        input_accountids = input_accountids + user.account;

                        if ( input_userids.length > 0 )
                            input_userids = input_userids + ',';
                        input_userids = input_userids + user.userid;
                    }
                }
            }
        }
    }
    else
    {
        input_accountids = gl_accountid;
        input_userids = gl_userid;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_usermaingroup'] = 1;
    jsondata['groupname'] = input_groupname;
    jsondata['permission'] = input_permission;
    jsondata['accountids'] = input_accountids;
    jsondata['userids'] = input_userids;

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

            reload_maingroup_info();
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

}

function save_usermaingroup(groupid)
{
    gl_edit_maingroupid = groupid;

    check_upload_image(uploadEditSubgroupImageFinihed);
}

function check_upload_image(finished_callback)
{
    gl_uploadimage_uid = gl_edit_maingroup_imageuid;
    var uploadimage = document.getElementById('imageUpload');

    if ( uploadimage != undefined )
    {
        if ( uploadimage.files[0] != undefined )
        {
            console.log("check_upload_image() uploadimage.files[0] != undefined ");

            gl_uploadimage_uid = sendUploadImageRequest(finished_callback);
        }
        else
        {
            finished_callback(gl_uploadimage_uid);
        }
    }
    else
    {
        finished_callback(gl_uploadimage_uid);
    }
}

function uploadEditSubgroupImageFinihed()
{
    uploadImageFinihed(update_usermaingroup);
}

function uploadImageFinihed(callback)
{
    console.log("uploadImageFinihed() gl_uploadimage_uid:" + gl_uploadimage_uid);

    if (gl_uploadimage_uid == null)
    {
        alert("上傳照片失敗!");
        return;
    }

    var uploadimage = document.getElementById('imageUpload');

    var uploadimage_file = undefined;
    if ( uploadimage != undefined )
    {
        if ( uploadimage.files[0] == undefined )
        {
            // console.log("uploadImageFinihed() file == undefined, gl_uploadimage_uid:"+gl_uploadimage_uid);
            setTimeout(callback, 100);
            return;
        }
        uploadimage_file = uploadimage.files[0];
    }
    else
    {
        setTimeout(callback, 100);
        return;
    }
    var filename = uploadimage_file.name.split(".");
    var new_file_name = filename[0];
    var new_file_description = "";

    var SendData = "project_id="+project_id;
    SendData += "&createnewimage=1";
    SendData += "&name="+new_file_name;
    SendData += "&description="+new_file_description;
    SendData += "&filename="+uploadimage_file.name;
    SendData += "&fileuid="+gl_uploadimage_uid;
    SendData += "&filetype="+uploadimage_file.type;
    SendData += "&filesize="+uploadimage_file.size;
    SendData += "&subgroups=0";
    SendData += "&leaveform=0";

    return jQuery.ajax({
     url: '/php/uploadfile.php',
     type: 'POST',
     data: SendData,
     dataType: 'text',
      success:
        function(response)
        {
            setTimeout(callback, 300);
        },
      error:
        function(xhr, status, error)
        {
            setTimeout(callback, 300);
        }
    });
}

function update_usermaingroup()
{
    var value_groupname = document.getElementById("uiid_groupname_"+gl_edit_maingroupid+"_edit").value;
    var value_permission = document.getElementById("uiid_permission_"+gl_edit_maingroupid+"_edit").value;
    var value_welcommessage = document.getElementById("uiid_welcommessage_"+gl_edit_maingroupid+"_edit").value;
    //var value_accountids = document.getElementById("uiid_accountids_"+groupid+"_edit").value;

    var value_autojoin = document.getElementById("uiid_autojoin_"+gl_edit_maingroupid+"_edit").value;

    if ( value_groupname.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    var value_accountids = '';
    var value_userids = '';

    if (gl_groupid == 1 || gl_groupid == 2 || gl_groupid == 4)
    {
        for (var i = 0; i < gl_user_info.length; ++i)
        {
            var user = gl_user_info[i];

            if ( user.groupid != '3' )
            {
                var checkbox = document.getElementById("dropdown_checkbox_"+gl_edit_maingroupid+"_"+user.account);
                if (checkbox != undefined)
                {
                    if (checkbox.checked == true)
                    {
                        if ( value_accountids.length > 0 )
                            value_accountids = value_accountids + ',';
                        value_accountids = value_accountids + user.account;

                        if ( value_userids.length > 0 )
                            value_userids = value_userids + ',';
                        value_userids = value_userids + user.userid;
                    }
                }
            }
        }
    }
    else
    {
        value_accountids = gl_accountid;
        value_userids = gl_userid;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_usermaingroup'] = 1;
    jsondata['groupid'] = gl_edit_maingroupid;
    jsondata['groupname'] = value_groupname;
    jsondata['permission'] = value_permission;
    jsondata['accountids'] = value_accountids;
    jsondata['userids'] = value_userids;
    jsondata['imageuid'] = gl_uploadimage_uid;
    jsondata['welcommessage'] = value_welcommessage;
    jsondata['autojoin'] = value_autojoin;

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

            reload_maingroup_info();
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_usermaingroup(groupid, groupname)
{
    var groupid = parseInt(groupid);

    if (confirm('Are you sure you want to delete this user group:'+groupname+' ?'))
    {
        // reset it!
        delete_usermaingroup(groupid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_usermaingroup(groupid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_usermaingroup'] = 1;
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

            reload_maingroup_info();
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function create_new_usermaingroup()
{
    create_empty_record_table_item(gl_usermaingroups.length + 1);

    return false;
}

// $("#createnewusermaingroupbutton").click(function()
// {
//     create_empty_record_table_item(gl_usermaingroups.length + 1);
//
//     return false;
// });

// function getAccountName(accountid)
// {
//     var accountName = '';
//     for (var i = 0; i < gl_user_info.length; ++i)
//     {
//         var user = gl_user_info[i];
//
//         if ( user.account === accountid )
//         {
//             accountName = user.name;
//             break;
//         }
//
//     }
//     return accountName;
// }


function create_upload_image_ui()
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
    var userid = parseInt(userid);

    var userName = '';
    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];

        if ( user.userid === userid )
        {
            userName = user.name;
            break;
        }

    }
    return userName;
}

function check_numer_in_list(targetNumber, dataList)
{
    var existed = false;
    for (var i = 0; i < dataList.length; ++i)
    {
        var data = dataList[i];

        if ( parseInt(data) == targetNumber )
        {
            existed = true;
            break;
        }
    }

    return existed;
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

        reload_maingroup_info();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    glCurrentPage++;
    current_page_num_ui.textContent = glCurrentPage;

    reload_maingroup_info();

}

function open_usermaingroupinfo_page()
{
    var win = window.open('/index.php/vilsusermaingroupinfo/'+project_id, '_blank');
    win.focus();
}
