/*
 * Author: ikki Chung
 * Date: 2020.04.27
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_usersubgroup_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var glCurrentPage = 1;

var gl_user_info = [];
var gl_usersubgroups = [];
var gl_userdatasubgroups = [];

var gl_edit_subgroupid = 0;
var gl_edit_maingroupid = 0;
var gl_edit_subgroup_imageuid = '';
var gl_uploadimage_uid = '';

var LOAD_USER_NUM = 500;
var gl_user_load_index = 0;

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

                gl_user_info = [];
                load_user_info();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_usersubgroup();
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

                show_subgroup_photo();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                // update_account_info();
                update_user_info();

                check_nfclocator_info();

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

function reload_subgroup_info()
{
    PreLoadIndex = 0;
    CurrentLoadIndex = 1;
    check_loading_status();
}

function load_usersubgroup()
{
    var offset = (glCurrentPage - 1) * LOAD_usersubgroup_NUM;
    var count = LOAD_usersubgroup_NUM;

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
    gl_userdatasubgroups = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaddatasubgroup=1&project_id='+project_id+'&type=0', function(data)
    {
        $.each(data, function(index, group)
        {
            group.subgroupid = parseInt(group.subgroupid);
            gl_userdatasubgroups.push(group);
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
    var user_num = 0;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduser=1&s='+gl_user_load_index+'&count='+LOAD_USER_NUM+'&project_id='+project_id, function(data)
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

        console.log("load_user_info() user_num:"+user_num);

        if (user_num < LOAD_USER_NUM)
        {
            CurrentLoadIndex = CurrentLoadIndex + 1;
        }
        else
        {
            setTimeout(load_user_info, 20);
        }
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .complete(function() {
    });
}

// function update_account_info()
// {
//     for (var i = 0; i < gl_userdatasubgroups.length; ++i)
//     {
//         var subgroup = gl_userdatasubgroups[i];
//
//         // console.log("update_account_info() subgroupid:"+subgroup.subgroupid+" accountids:"+subgroup.accountids);
//
//         var ui = document.getElementById("uiid_accountids_"+subgroup.subgroupid);
//         var accountids = createAccountIDUI(subgroup.accountids);
//         ui.appendChild(accountids);
//     }
// }

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

function update_user_info()
{
    for (var i = 0; i < gl_userdatasubgroups.length; ++i)
    {
        var subgroup = gl_userdatasubgroups[i];

        var ui = document.getElementById("uiid_userids_"+subgroup.subgroupid);
        var userids = createUserIDUI(subgroup.userids);
        ui.appendChild(userids);
    }
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

function check_nfclocator_info()
{
    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadnfclocatorcount=1&project_id='+project_id, function(data)
    {
        // console.log("check_nfclocator_info() data.count:"+data.count +" gl_usersubgroups.length:"+gl_usersubgroups.length);

        var btnCreateNewSubGroup = document.getElementById('btnCreateNewSubGroup');

        if (btnCreateNewSubGroup != undefined)
        {
            if (gl_groupid == 1 || gl_groupid == 2)
            {
                // do nothing
                btnCreateNewSubGroup.setAttribute("class","btn btn-success btn-xs");
            }
            else
            {
                if ( gl_userdatasubgroups.length > (data.count * 3 - 1) )
                {
                    // disable create new subgroup
                    btnCreateNewSubGroup.setAttribute("class","hidden btn btn-success btn-xs");
                }
                else
                {
                    btnCreateNewSubGroup.setAttribute("class","btn btn-success btn-xs");
                }
            }
        }
    })
    .success(function() {

    })
    .error(function() {
    })
    .complete(function() {
    });
}

function draw_table()
{
    var record_table = document.getElementById('usersubgroupinfo_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

     console.log("draw_table() gl_userdatasubgroups.length:"+gl_userdatasubgroups.length);

    for (var i = 0; i < gl_userdatasubgroups.length; ++i)
    {
        var group = gl_userdatasubgroups[i];

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
    tdgroupid.setAttribute("id","uiid_groupname_"+record.subgroupid);
    row.appendChild(tdgroupid);

    var Spangroupid = create_subgroup_label(record.subgroupname);
    row.cells[cellIndex].appendChild(Spangroupid);
    cellIndex++;

    // 權限
    var tdpermission = document.createElement('td');
    tdpermission.setAttribute("id","uiid_permission_"+record.subgroupid);
    tdpermission.textContent = record.permission;
    row.appendChild(tdpermission);
    cellIndex++;

    // 照片
    var tdpermission = document.createElement('td');
    tdpermission.setAttribute("id","uiid_photo_"+record.subgroupid);
    row.appendChild(tdpermission);
    cellIndex++;

    // 子集合管理者
    var tdmanager = document.createElement('td');
    // tdmanager.setAttribute("id","uiid_accountids_"+record.subgroupid);
    tdmanager.setAttribute("id","uiid_userids_"+record.subgroupid);
    // tdmanager.textContent = record.accountids;
    row.appendChild(tdmanager);
    cellIndex++;

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    var btnQrcode = document.createElement("button");
    btnQrcode.setAttribute("type","button");
    btnQrcode.setAttribute("class","btn btn-default");
    btnQrcode.setAttribute("data-toggle","modal");
    btnQrcode.setAttribute("data-target","#modal-qrcode");
    btnQrcode.setAttribute("onclick","device_qrcode_show(\"SUBGROUP\",\""+ record.subgroupid +"\",\""+ record.subgroupname +"\");return false;");

    var imgQrcode = document.createElement("i");
    imgQrcode.setAttribute("class","fa fa-qrcode");
    btnQrcode.appendChild(imgQrcode);
    row.cells[cellIndex].appendChild(btnQrcode);

    if (config_enable_edit)
    {

        // Edit button
        var editBtn = document.createElement("button");
        editBtn.setAttribute("id","uiid_editbtn_"+record.subgroupid);
        editBtn.setAttribute("title","Edit this usersubgroup");
        editBtn.setAttribute("href","javascript:;");
        editBtn.setAttribute("onclick","edit_usersubgroup(\""+record.subgroupid+"\");return false;");
        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("id","uiid_editimg_"+record.subgroupid);
        imgEdit.setAttribute("class","fa fa-edit");
        editBtn.appendChild(imgEdit);
        row.cells[cellIndex].appendChild(editBtn);

        if (ui_index != 0 )
        {
            // delete uesr button
            var deleteBtn = document.createElement("button");
            deleteBtn.setAttribute("style","display:none");
            deleteBtn.setAttribute("id","uiid_deletebtn_"+record.subgroupid);
            deleteBtn.setAttribute("title","Delete This usersubgroup");
            deleteBtn.setAttribute("href","javascript:;");
            deleteBtn.setAttribute("onclick","confirm_delete_usersubgroup(\""+record.subgroupid+"\", \""+ record.subgroupname +"\");return false;");
            var imgDelete = document.createElement("i");
            imgDelete.setAttribute("class","fa fa-user-times");
            deleteBtn.appendChild(imgDelete);
            row.cells[cellIndex].appendChild(deleteBtn);
        }
        cellIndex++;
    }


    return row;
}

function device_qrcode_show(type, subgroupid, subgroupname)
{

    var int_subgroupid = parseInt(subgroupid);

    var device_qrcode_type = document.getElementById("device_qrcode_type");
    device_qrcode_type.textContent = 'Type: 子集合';
    var device_qrcode_name = document.getElementById("device_qrcode_name");
    device_qrcode_name.textContent = '名稱: ' + subgroupname;
    var device_qrcode_macaddress = document.getElementById("device_qrcode_macaddress");
    device_qrcode_macaddress.textContent = '子集合ID: ' + int_subgroupid;

    var SendDate = "loadqrcode=1&type="+type+"&accountid=" + int_subgroupid+"&id=" + int_subgroupid+'&project_id='+project_id;
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

function create_empty_record_table_item(ui_index)
{
    var empty_record_table_item = document.getElementById('empty_record_table_item');
    if ( empty_record_table_item != null )
        return;

    var record_table = document.getElementById('usersubgroupinfo_TABLE');
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

    // 照片
    row.appendChild(document.createElement('td'));
    var div_upload = create_upload_image_ui();
    row.cells[cellIndex].appendChild(div_upload);
    cellIndex++;

    // 子集合管理者
    row.appendChild(document.createElement('td'));
    var default_checked = (gl_groupid != 1 && gl_groupid != 2);
    var ui_manager_dropdown_list = create_manager_dropdown_list("input_accountids", default_checked);
    row.cells[cellIndex].appendChild(ui_manager_dropdown_list);
    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_usersubgroup();");
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

function create_manager_dropdown_list(uiid, default_checked)
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
            div_ul.appendChild(create_dropdown_user_item("new", user.name, user.account, default_checked));
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

function show_subgroup_photo()
{
    for (var i = 0; i < gl_userdatasubgroups.length; ++i)
    {
        var group = gl_userdatasubgroups[i];
        if ( group.imageuid.length > 0 )
        {
            load_subgroup_photo(group.subgroupid, group.imageuid);
        }
    }
}

function load_subgroup_photo(subgroupid, imageuid)
{
    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadsingleimage=1&project_id='+project_id+'&imageuid='+imageuid, function(storageImage)
    {
        var ui_id = 'uiid_photo_'+subgroupid;
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

function edit_usersubgroup(groupid)
{
    console.log("edit_usersubgroup() groupid:"+groupid);

    var group = getusersubgroup(groupid);
    if (group == null)
    {
        console.log("edit_usersubgroup() group == null");
        return;
    }

    create_edit_ui("uiid_groupname", groupid, group.subgroupname);
    create_edit_ui("uiid_permission", groupid, group.permission);
    //create_edit_ui("uiid_accountids", groupid, group.accountids);
    //create_edit_manager_ui("uiid_accountids", groupid, group.accountids);

    gl_edit_subgroup_imageuid = group.imageuid;

    if ( gl_groupid == 1 || gl_groupid == 2 || gl_groupid == 6)
    {
        create_edit_manager_ui("uiid_userids", groupid, group.userids);

        var show_img_area_image_ui = document.getElementById('uiid_photo_'+groupid);
        if ( show_img_area_image_ui != undefined )
        {
            if ( gl_edit_subgroup_imageuid.length == 0 )
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
                deletePhotoBtn.setAttribute("title","Delete Subgroup Photo");
                deletePhotoBtn.setAttribute("href","javascript:;");
                deletePhotoBtn.setAttribute("onclick","delete_subgroup_photo(\""+groupid+"\");return false;");
                var imgdelete = document.createElement("i");
                imgdelete.setAttribute("class","fa fa-remove");
                deletePhotoBtn.appendChild(imgdelete);
                show_img_area_image_ui.appendChild(deletePhotoBtn);
            }
        }

        var deleteBtn = document.getElementById("uiid_deletebtn_"+groupid);
        if (deleteBtn != undefined)
            deleteBtn.setAttribute("style","display:block");
    }

    var editBtn = document.getElementById("uiid_editbtn_"+groupid);
    editBtn.setAttribute("title","Save this usersubgroup");
    editBtn.setAttribute("onclick","save_usersubgroup(\""+groupid+"\", \""+group.maingroupid+"\");return false;");
    var imgEdit = document.getElementById("uiid_editimg_"+groupid);
    imgEdit.setAttribute("class","fa fa-save");
}

function delete_subgroup_photo(groupid)
{
    gl_edit_subgroup_imageuid = "";

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

    //var accountidList = accountids.split(",");
    var useridList = userids.split(",");

    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];

        if ( user.groupid != '3' )
        {
            //
            // 檢查使用者是否擁有 此集合 權限
            //
            var datagroupList = user.datagroups.split(",");
            var existed = check_numer_in_list(parseInt(groupid), datagroupList);
            if ( existed )
            {
                var userid_str = user.userid+'';
                //var defaultchecked = (accountidList.indexOf(user.account) >= 0 );
                var defaultchecked = (useridList.indexOf(userid_str) >= 0 );

                div_ul.appendChild(create_dropdown_user_item(groupid, user.name, user.account, defaultchecked));
            }
        }
    }

    return ui_userids;
}

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

function getusersubgroup(groupid)
{
    var retusersubgroup = null;

    var int_groupid = parseInt(groupid);

    for (var i = 0; i < gl_userdatasubgroups.length; ++i)
    {
        var group = gl_userdatasubgroups[i];

        if ( group.subgroupid == int_groupid )
        {
            retusersubgroup = group;
            break;
        }
    }
    return retusersubgroup;
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

function save_new_usersubgroup()
{
    check_upload_image(uploadNewSubGroupImageFinihed);
}

function uploadNewSubGroupImageFinihed(uploadimage_uid)
{
    gl_uploadimage_uid = uploadimage_uid;

    uploadImageFinihed(upload_new_usersubgroup);
}

function upload_new_usersubgroup()
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

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_usersubgroup'] = 1;
    jsondata['groupname'] = input_groupname;
    jsondata['permission'] = input_permission;
    jsondata['accountids'] = input_accountids;
    jsondata['userids'] = input_userids;
    jsondata['imageuid'] = gl_uploadimage_uid;

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

            reload_subgroup_info();
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function save_usersubgroup(groupid, maingroupid)
{
    gl_edit_subgroupid = groupid;
    gl_edit_maingroupid = maingroupid;

    check_upload_image(uploadEditSubgroupImageFinihed);
}

function check_upload_image(finished_callback)
{
    gl_uploadimage_uid = gl_edit_subgroup_imageuid;
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
    uploadImageFinihed(update_usersubgroup);
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
    SendData += "&subgroups="+gl_edit_subgroupid;
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

function update_usersubgroup()
{
    groupid = gl_edit_subgroupid;
    maingroupid = gl_edit_maingroupid;

    var value_groupname = document.getElementById("uiid_groupname_"+groupid+"_edit").value;
    var value_permission = document.getElementById("uiid_permission_"+groupid+"_edit").value;
    //var value_accountids = document.getElementById("uiid_accountids_"+groupid+"_edit").value;

    if ( value_groupname.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    var value_accountids = '';
    var value_userids = '';

    if ( gl_groupid == 1 || gl_groupid == 2 || gl_groupid == 6 )
    {
        for (var i = 0; i < gl_user_info.length; ++i)
        {
            var user = gl_user_info[i];

            if ( user.groupid != '3' )
            {
                var checkbox = document.getElementById("dropdown_checkbox_"+groupid+"_"+user.account);
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

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_usersubgroup'] = 1;
    jsondata['groupid'] = groupid;
    jsondata['groupname'] = value_groupname;
    jsondata['permission'] = value_permission;
    jsondata['imageuid'] = gl_uploadimage_uid;
    jsondata['accountids'] = value_accountids;
    jsondata['userids'] = value_userids;
    jsondata['maingroupid'] = maingroupid;

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

            reload_subgroup_info();
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_usersubgroup(groupid, groupname)
{
    var groupid = parseInt(groupid);

    if (confirm('Are you sure you want to delete this user group:'+groupname+' ?'))
    {
        // reset it!
        delete_usersubgroup(groupid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_usersubgroup(groupid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_usersubgroup'] = 1;
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

            reload_subgroup_info();
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function create_new_usersubgroup()
{
    create_empty_record_table_item(gl_userdatasubgroups.length + 1);

    return false;
}

function getAccountName(accountid)
{
    var accountName = '';
    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];

        if ( user.account === accountid )
        {
            accountName = user.name;
            break;
        }

    }
    return accountName;
}

function getUserName(userid)
{
    var userid = parseInt(userid);

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

        reload_subgroup_info();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    glCurrentPage++;
    current_page_num_ui.textContent = glCurrentPage;

    reload_subgroup_info();

}

function open_usersubgroupinfo_page()
{
    var win = window.open('/index.php/vilsusersubgroupinfo/'+project_id, '_blank');
    win.focus();
}
