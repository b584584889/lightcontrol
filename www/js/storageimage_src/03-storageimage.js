/*
 * Author: ikki Chung
 * Date: 2020/07/20
 * Description:
 *      SMIMS VILS RTLS Solutions event trigger
 **/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var gl_users = [];
var gl_usersubgroups = [];
var gl_userdatasubgroups = [];
var gl_storageimages = [];
var gl_uploadimage_uid = '';

var gl_reload = 0;
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
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_user();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_usersubgroup();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_storageimage();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                var table_storage_image = document.getElementById("table_storage_image");
                table_storage_image.setAttribute("style","display:none;");

                var image_list_area = document.getElementById("image_list_area");
                image_list_area.setAttribute("style","display:block;");

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_images_to_UI();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                // if (gl_reload == 0)
                // {
                //     enable_datatable_UI();
                // }

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

function reload_storageimage_info()
{
    gl_reload = 1;
    PreLoadIndex = 1;
    CurrentLoadIndex = 2;
    check_loading_status();
}

function load_user()
{
    gl_users = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduser=1&&project_id='+project_id, function(data)
    {
        var user_num = 0;
        $.each(data, function(index, user)
        {
            gl_users.push(user);
            user_num++;
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
    // var p = $.getJSON(targetURL, 'loaddatasubgroup=1&project_id='+project_id+'&type=0', function(data)
    var p = $.getJSON(targetURL, 'loaddatasubgroup=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, group)
        {
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

function load_storageimage()
{
    gl_storageimages = [];

    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadimage=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, storageimage)
        {
            gl_storageimages.push(storageimage);
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

function show_images_to_UI()
{
    clearChildView('image_list_area');

    var image_list_area = document.getElementById('image_list_area');

    var count = 0;
    var row;
    for (var i = 0; i < gl_storageimages.length; ++i)
    {
        var storageimage = gl_storageimages[i];

        if (count == 0)
        {
            row = document.createElement("div");
            row.setAttribute("class","row");
            image_list_area.appendChild(row);
        }

        var col = create_image_item(storageimage);
        row.appendChild(col);

        count++;
        if ( count >= 5 )
            count = 0;
    }
}

function create_image_item(storageimage)
{
    var col = document.createElement("div");
    col.setAttribute("class","col-xs-6 col-md-4 col-lg-2");

    var ul_image = create_data_image(storageimage.id, storageimage.name, storageimage.thumbpath, storageimage.imagetype, storageimage.imagesize);
    col.appendChild(ul_image);

    return col;
}

// function show_files_to_UI()
// {
//     clearChildView('tablebody_storage_image');
//
//     var tablebody_storage_image = document.getElementById('tablebody_storage_image');
//
//     for (var i = 0; i < gl_storageimages.length; ++i)
//     {
//         var storageimage = gl_storageimages[i];
//
//         // console.log("show_files_to_UI() storageimage.id:"+storageimage.id);
//
//         var row = create_table_row(storageimage);
//         tablebody_storage_image.appendChild(row);
//     }
// }
//
// function create_table_row(storageimage)
// {
//     var row = document.createElement("tr");
//
//     var td_name = document.createElement("td");
//     td_name.setAttribute("id","uiid_name_"+storageimage.id);
//     td_name.textContent = storageimage.name;
//     row.appendChild(td_name);
//
//     var td_description = document.createElement("td");
//     td_description.setAttribute("id","uiid_description_"+storageimage.id);
//     td_description.textContent = storageimage.description;
//     row.appendChild(td_description);
//
//     var td_subgroups = create_subgroup(storageimage.subgroups);
//     td_subgroups.setAttribute("id","uiid_subgroup_"+storageimage.id);
//     row.appendChild(td_subgroups);
//
//     var td_filetype = create_data_image(storageimage.id, storageimage.imagename, storageimage.thumbpath, storageimage.imagetype, storageimage.imagesize);
//     row.appendChild(td_filetype);
//
//     var td_account = document.createElement("td");
//     var SpanName = document.createElement("span");
//     SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
//     SpanName.textContent = getAccountName(storageimage.accountid);
//     td_account.appendChild(SpanName);
//     row.appendChild(td_account);
//
//     var td_edittime = document.createElement("td");
//     var Spanedittime  = document.createElement("span");
//     Spanedittime.setAttribute("class","label label-primary");
//     Spanedittime.textContent = storageimage.updatetime;
//     td_edittime.appendChild(Spanedittime);
//     row.appendChild(td_edittime);
//
//     var td_createtime = document.createElement("td");
//     var Spancreatetime  = document.createElement("span");
//     Spancreatetime.setAttribute("class","label label-info");
//     Spancreatetime.textContent = storageimage.datetime;
//     td_createtime.appendChild(Spancreatetime);
//     row.appendChild(td_createtime);
//
//     var td_operation = document.createElement("td");
//     row.appendChild(td_operation);
//
//     if (gl_groupid != 3)
//     {
//         // Edit button
//         var editBtn = document.createElement("button");
//         editBtn.setAttribute("id","uiid_editbtn_"+storageimage.id);
//         editBtn.setAttribute("title","Edit this Image");
//         editBtn.setAttribute("href","javascript:;");
//         editBtn.setAttribute("onclick","edit_image(\""+storageimage.id+"\");return false;");
//         var imgEdit = document.createElement("i");
//         imgEdit.setAttribute("id","uiid_editimg_"+storageimage.id);
//         imgEdit.setAttribute("class","fa fa-edit");
//         editBtn.appendChild(imgEdit);
//         td_operation.appendChild(editBtn);
//
//         // delete uesr button
//         var deleteBtn = document.createElement("button");
//         deleteBtn.setAttribute("style","display:none");
//         deleteBtn.setAttribute("id","uiid_deletebtn_"+storageimage.id);
//         deleteBtn.setAttribute("title","Delete This Image");
//         deleteBtn.setAttribute("href","javascript:;");
//         deleteBtn.setAttribute("onclick","confirm_delete_image(\""+storageimage.id+"\", \""+ storageimage.name +"\");return false;");
//         var imgDelete = document.createElement("i");
//         imgDelete.setAttribute("class","fa fa-user-times");
//         deleteBtn.appendChild(imgDelete);
//         td_operation.appendChild(deleteBtn);
//     }
//
//     return row;
// }
//
// function create_subgroup(subgroups)
// {
//     var d = document.createElement("td");
//
//     var subgroupList = subgroups.split(",");
//
//     if (subgroupList.indexOf('99999') >= 0)
//     {
//         var subgroupL_div = document.createElement("div");
//         subgroupL_div.appendChild(create_subgroup_label('全部'));
//         d.appendChild(subgroupL_div);
//     }
//     else
//     {
//         var count = 0;
//         var subgroupL_div = undefined;
//         for(var index=0;index<subgroupList.length;index++)
//         {
//             var subgroupid = subgroupList[index];
//             var SubgroupName = getSubgroupName(subgroupid);
//             if (SubgroupName.length > 0)
//             {
//                 if ( count == 0 )
//                 {
//                     subgroupL_div = document.createElement("div");
//                     d.appendChild(subgroupL_div);
//                 }
//
//                 subgroupL_div.appendChild(create_subgroup_label(SubgroupName));
//                 subgroupL_div.appendChild(create_empty_spcae());
//
//                 count++;
//                 if ( count >= 5 )
//                 {
//                     count = 0;
//                 }
//             }
//         }
//     }
//
//     return d;
// }


function create_subgroup(subgroups)
{
    var spanSubgroup = document.createElement("span");

    var subgroupList = subgroups.split(",");

    if (subgroupList.indexOf('99999') >= 0)
    {
        spanSubgroup.appendChild(create_subgroup_label('全部'));
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
                    spanSubgroup.appendChild(subgroupL_div);
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

    return spanSubgroup;
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

    var div_info_href_detail = document.createElement("a");
    div_info_href_detail.setAttribute("class","btn btn-default btn-xs pull-left");
    div_info_href_size.appendChild(div_info_href_detail);

    var div_info_href_detail_img = document.createElement("i");
    div_info_href_detail_img.setAttribute("title","查看詳細資訊");
    div_info_href_detail_img.setAttribute("onclick","open_detail_image(\""+id+"\");return false;");
    div_info_href_detail_img.setAttribute("class","fa fa-search-plus");
    div_info_href_detail.appendChild(div_info_href_detail_img);

    var div_info_href_dl = document.createElement("a");
    div_info_href_dl.setAttribute("href","/php/downloadfile.php?project_id="+project_id+"&imageid="+id);
    div_info_href_dl.setAttribute("class","btn btn-default btn-xs pull-right");
    div_info_href_size.appendChild(div_info_href_dl);

    var div_info_href_dl_img = document.createElement("i");
    div_info_href_dl_img.setAttribute("class","fa fa-cloud-download");
    div_info_href_dl.appendChild(div_info_href_dl_img);

    return data_ul;
}

function open_detail_image(imageid)
{
    $("#modal-image_detail").modal('toggle');

    var cur_image = getImage(imageid);

    var file_detail_name = document.getElementById("image_detail_name");
    image_detail_name.textContent = '檔案標題: ' + cur_image.name;

    var image_detail_description = document.getElementById("image_detail_description");
    image_detail_description.textContent = '檔案標說明: ' + cur_image.description;

    var image_detail_filename = document.getElementById("image_detail_filename");
    image_detail_filename.textContent = '檔案名稱: ' + cur_image.imagename;

    var image_detail_filetype = document.getElementById("image_detail_filetype");
    image_detail_filetype.textContent = '檔案型態: ' + cur_image.imagetype;

    var image_detail_subgroups = document.getElementById("image_detail_subgroups");
    image_detail_subgroups.textContent = '檔案子集合權限: ';

    var td_subgroups = create_subgroup(cur_image.subgroups);
    image_detail_subgroups.appendChild(td_subgroups);

    var image_detail_account = document.getElementById("image_detail_account");
    image_detail_account.textContent = '上傳帳號: ';

    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = getAccountName(cur_image.accountid);
    image_detail_account.appendChild(SpanName);


    // var image_detail_updatetime = document.getElementById("image_detail_updatetime");
    // image_detail_updatetime.textContent = '更新時間: ' + cur_file.updatetime;

    var image_detail_uploadtime = document.getElementById("image_detail_uploadtime");
    image_detail_uploadtime.textContent = '上傳時間: ' + cur_image.datetime;

    if ( cur_image.accountid === gl_accountid )
    {
        var delete_button = document.getElementById("delete_button");
        if (delete_button != undefined)
        {
            delete_button.setAttribute("style","display:block;");
            delete_button.setAttribute("onclick","confirm_delete_image(\""+cur_image.id+"\", \""+ cur_image.name +"\");return false;");
        }
    }
    else
    {
        var delete_button = document.getElementById("delete_button");
        if (delete_button != undefined)
        {
            delete_button.setAttribute("style","display:none;");
            delete_button.setAttribute("onclick","");
        }
    }

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

function edit_image(imageid)
{
    var image = getImage(imageid);
    if (image == null)
        return;

    create_edit_ui("uiid_name", imageid, image.name);
    create_edit_ui("uiid_description", imageid, image.description);

    clearChildView("uiid_subgroup_"+imageid);
    var subgrouplist_span = document.getElementById("uiid_subgroup_"+imageid);

    var div_subgrouplist = create_edit_subgrouplist_ui(imageid, image.subgroups);
    subgrouplist_span.appendChild(div_subgrouplist);

    var editBtn = document.getElementById("uiid_editbtn_"+imageid);
    editBtn.setAttribute("title","Save this Image");
    editBtn.setAttribute("onclick","save_image(\""+imageid+"\");return false;");
    var imgEdit = document.getElementById("uiid_editimg_"+imageid);
    imgEdit.setAttribute("class","fa fa-save");

    var deleteBtn = document.getElementById("uiid_deletebtn_"+imageid);
    deleteBtn.setAttribute("style","display:block");

}

function create_edit_ui(uiid, imageid, orig_value)
{
    var ui_name = clearChildView(uiid+"_"+imageid);

    var new_input = create_input_text(uiid+"_"+imageid+"_edit", orig_value);
    ui_name.appendChild(new_input);
}

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.textContent = text;

    return input_text;
}

function enable_datatable_UI()
{
    $('#table_storage_image').DataTable( {
    "language": {
        "lengthMenu": "每頁顯示 _MENU_ 筆",
        "zeroRecords": "沒有任何資料!",
        "info": "目前頁面 _PAGE_ / _PAGES_",
        "infoEmpty": "沒有任何資料!",
        "infoFiltered": "(filtered from _MAX_ total records)",
        "search": "搜尋:",
        "paginate": {
          "previous": "<<",
          "next": ">>"
        }
     },
     "order": [[ 5, "desc"]],
     "bDestroy": true
   })
}

function create_empty_filestorage_image_item()
{
    var empty_record_table_item = document.getElementById('empty_record_table_item');
    if ( empty_record_table_item != null )
    {
        clearChildView('empty_record_table_item');
    }

    var record_table = document.getElementById('tablebody_storage_image');
    var row = record_table.insertRow(0);

    row.setAttribute("id", "empty_record_table_item");
    row.setAttribute("bgcolor","#ebebe0");

    var cellIndex = 0;

    // name
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("id_new_image_name", "相片名稱"));
    cellIndex++;

    // description
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("id_new_image_description", "相片說明"));
    cellIndex++;

    // Subgroup list
    row.appendChild(document.createElement('td'));
    var div_subgrouplist = create_edit_subgrouplist_ui("new", "");
    row.cells[cellIndex].appendChild(div_subgrouplist);

    var subgroups_selected_div = document.createElement("div");
    subgroups_selected_div.setAttribute("id", "new_subgroups_selected");
    row.cells[cellIndex].appendChild(subgroups_selected_div);

    cellIndex++;

    // upload file
    row.appendChild(document.createElement('td'));
    var div_upload = create_upload_image_ui("id_new_image_filename");
    row.cells[cellIndex].appendChild(div_upload);
    cellIndex++;

    row.appendChild(document.createElement('td'));
    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = getAccountName(gl_accountid);
    row.cells[cellIndex].appendChild(SpanName);
    cellIndex++;

    // row.appendChild(document.createElement('td'));
    // cellIndex++;
    //
    // row.appendChild(document.createElement('td'));
    // cellIndex++;

    // save button file
    row.appendChild(document.createElement('td'));

    var div_Upload_button = document.createElement("input");
    div_Upload_button.setAttribute("type","button");
    div_Upload_button.setAttribute("value","Upload");
    div_Upload_button.setAttribute("onclick","startUpload();");

    row.cells[cellIndex].appendChild(div_Upload_button);
    cellIndex++;

}

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("input");
    input_text.setAttribute("type","text");
    input_text.setAttribute("id",ui_id);
    input_text.setAttribute("value",text);

    return input_text;
}

function create_inputarea_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.textContent = text;

    return input_text;
}

function create_edit_subgrouplist_ui(imageid, idlist)
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

    if ( gl_groupid == 1 || gl_groupid == 2 )
    {
        // select all button
        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","dropdown_selectall(\"subgroup\",\""+imageid+"\");return true;");
        item_li.appendChild(item_click);
        item_click.appendChild(document.createTextNode("全選"));
        div_ul.appendChild(item_li);
    }

    var idList = idlist.split(",");

    for (var i = 0; i < gl_userdatasubgroups.length; ++i)
    {
        var usersubgroup = gl_userdatasubgroups[i];
        var existed = check_numer_in_list(parseInt(usersubgroup.subgroupid), idList);
        div_ul.appendChild(create_dropdown_subgroup_item(imageid, i, parseInt(usersubgroup.subgroupid), usersubgroup.subgroupname, existed));
    }

    return div_subgrouplist;
}

function create_dropdown_subgroup_item(imageid, index, datavalue, dataname, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", datavalue);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_subgroup_click(\""+imageid+"\","+ index +");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_subgroup_"+imageid+"_"+index);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_subgroup_click(\""+imageid+"\","+ index +");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+dataname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_checkbox_subgroup_click(imageid, index)
{
    glDropdown_checkbox_subgroup_click = true;
}

var glDropdown_checkbox_subgroup_click = false;
function dropdown_subgroup_click(imageid, index)
{
    if (glDropdown_checkbox_subgroup_click == true)
    {
        glDropdown_checkbox_subgroup_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_subgroup_"+imageid+"_"+index).checked == true)
        document.getElementById("dropdown_checkbox_subgroup_"+imageid+"_"+index).checked = false;
    else
        document.getElementById("dropdown_checkbox_subgroup_"+imageid+"_"+index).checked = true;

    glDropdown_checkbox_subgroup_click = false;

    update_selected_subgroup(imageid);
}

function update_selected_subgroup(imageid)
{
    console.log("update_selected_subgroup() ");

    var subgroups_selected_div = document.getElementById("new_subgroups_selected");
    if (subgroups_selected_div != undefined)
    {
        clearChildView("new_subgroups_selected");
    }

    for (var index = 0; index < gl_userdatasubgroups.length; ++index)
    {
        var subgroup = gl_userdatasubgroups[index];

        var checkbox = document.getElementById("dropdown_checkbox_subgroup_"+imageid+"_"+index);
        if (checkbox == undefined)
            continue;

        if (checkbox.checked)
        {
            var subgroupname_label = create_subgroup_label(subgroup.subgroupname);
            subgroups_selected_div.appendChild(subgroupname_label);
        }
    }
}

function dropdown_selectall(prefix, imageid)
{
    console.log("dropdown_selectall() prefix:"+prefix+" imageid:"+imageid);

    for(var i=0;i<100;i++)
    {
        var ele = document.getElementById("dropdown_checkbox_"+prefix+"_"+imageid+"_"+i);
        if ( ele == undefined )
        {
            console.log("dropdown_selectall() imageid:"+imageid+" i:"+i+" ele == undefined ");
            break;
        }
        ele.checked = true;
    }
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

function create_new_filestorage_image()
{
    var table_storage_image = document.getElementById("table_storage_image");
    table_storage_image.setAttribute("style","display:block;");

    var image_list_area = document.getElementById("image_list_area");
    image_list_area.setAttribute("style","display:none;");

    create_empty_filestorage_image_item();
}

function getSelectedSubgroup(imageid)
{
    var subgroups = [];
    var count = 0;
    for (var index = 0; index < gl_userdatasubgroups.length; ++index)
    {
        var usersubgroup = gl_userdatasubgroups[index];

        if (document.getElementById("dropdown_checkbox_subgroup_"+imageid+"_"+index).checked)
        {
            var subgroupid = parseInt(usersubgroup.subgroupid);
            subgroups.push(subgroupid);
            count++;
        }
    }

    if ( gl_groupid == 1 || gl_groupid == 2 )
    {
        if ( count == gl_userdatasubgroups.length )
        {
            subgroups = [];
            subgroups.push(99999);
        }
    }

    return subgroups.toString();
}

function uploadFinihed(uploadimage_uid)
{
    console.log("uploadFinihed() uploadimage_uid:"+uploadimage_uid);

    if (uploadimage_uid == null)
    {
        alert("上傳照片失敗!");
        return;
    }

    var new_file_name = document.getElementById('id_new_image_name').value;
    var new_file_description = document.getElementById('id_new_image_description').value;
    var subgroups = getSelectedSubgroup('new');

    if ( gl_groupid != 1 &&  gl_groupid != 2)
    {
        if ( subgroups.length == 0 )
        {
            alert("請選擇子集合");
            return;
        }
    }

    var file = document.getElementById('imageUpload').files[0];

    var SendData = "project_id="+project_id;
    SendData += "&createnewimage=1";
    SendData += "&name="+new_file_name;
    SendData += "&description="+new_file_description;
    SendData += "&filename="+file.name;
    SendData += "&fileuid="+uploadimage_uid;
    SendData += "&filetype="+file.type;
    SendData += "&filesize="+file.size;
    SendData += "&subgroups="+subgroups;

    return jQuery.ajax({
     url: '/php/uploadfile.php',
     type: 'POST',
     data: SendData,
     dataType: 'text',
      success:
        function(response)
        {
            gl_uploadimage_uid = '';

            setTimeout(reload_storageimage_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            gl_uploadimage_uid = '';

            setTimeout(reload_storageimage_info, 300);
        }
    });
}

function startUpload()
{
    var subgroups = getSelectedSubgroup('new');

    if ( gl_groupid != 1 &&  gl_groupid != 2)
    {
        if ( subgroups.length == 0 )
        {
            alert("請選擇子集合");
            return;
        }
    }

    gl_uploadimage_uid = sendUploadImageRequest(uploadFinihed);
}

function save_image(imageid)
{
    var new_name = document.getElementById("uiid_name_"+imageid+"_edit").value;
    var new_description = document.getElementById("uiid_description_"+imageid+"_edit").value;
    var subgroups = getSelectedSubgroup(imageid);

    clearChildView("uiid_name_"+imageid);
    clearChildView("uiid_description_"+imageid);
    clearChildView("uiid_subgroup_"+imageid);

    var table = $('#table_storage_image').DataTable();
    table.clear();

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_image'] = 1;
    jsondata['imageid'] = imageid;
    jsondata['name'] = new_name;
    jsondata['description'] = new_description;
    jsondata['subgroups'] = subgroups;

    var targetURL = gl_target_server + "/php/storage.php";

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

            setTimeout(reload_storageimage_info, 500);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);

            setTimeout(reload_storageimage_info, 500);
        }
    });

}

function confirm_delete_image(imageid, imagename)
{
    if (confirm('您確定要刪除此相片:'+imagename+' ?'))
    {
        // delete it!
        delete_image(imageid);

        $("#modal-image_detail").modal('toggle');
    }
    else
    {
        // Do nothing!
    }
}

function delete_image(imageid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_image'] = 1;
    jsondata['imageid'] = imageid;

    var targetURL = gl_target_server + "/php/storage.php";

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
            setTimeout(reload_storageimage_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
            setTimeout(reload_storageimage_info, 300);
        }
    });
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

        if ( subgroup.subgroupid == subgroupid )
        {
            subgroupname = subgroup.subgroupname;
            break;
        }

    }
    return subgroupname;
}

function getAccountName(accountid)
{
    var username = accountid;
    for (var i = 0; i < gl_users.length; ++i)
    {
        var user = gl_users[i];

        if ( parseInt(user.accountid) == accountid )
        {
            username = user.name;
            break;
        }

    }
    return username;
}

function getImage(imageid)
{
    var retimage = null;
    for (var i = 0; i < gl_storageimages.length; ++i)
    {
        var image = gl_storageimages[i];

        if ( image.id == imageid )
        {
            retimage = image;
            break;
        }
    }
    return retimage;
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

function open_storageimage_page()
{
    var win = window.open('/index.php/vilsstorageimage/'+project_id, '_blank');
    win.focus();
}
