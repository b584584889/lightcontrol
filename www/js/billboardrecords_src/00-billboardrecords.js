/*
 * Author: ikki Chung
 * Date: 2020 09 29
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var glCurrentPage = 1;
var LOAD_PAGE_NUM = 20;

var gl_usersubgroups = [];
// var gl_userdatasubgroups = [];
var gl_billboard_List = [];

var pre_input_title_value = '';
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

                load_usersubgroup();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_billboard_records();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_billboard_records();

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
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
        //load_userdatasubgroup();
    })
    .error(function() {
    })
    .complete(function() {
    });
}

// function load_userdatasubgroup()
// {
//     gl_userdatasubgroups = [];
//
//     var targetURL = gl_target_server + "/php/vilsnodes.php";
//     var p = $.getJSON(targetURL, 'loaddatasubgroup=1&project_id='+project_id, function(data)
//     {
//         $.each(data, function(index, subgroup)
//         {
//             subgroup.subgroupid = parseInt(subgroup.subgroupid);
//             gl_userdatasubgroups.push(subgroup);
//         });
//     })
//     .success(function() {
//         CurrentLoadIndex = CurrentLoadIndex + 1; // next state
//     })
//     .error(function() {
//     })
//     .complete(function() {
//     });
// }

function load_billboard_records()
{
    var input_title_value = document.getElementById("search_billboard_title_text").value;

    var input_date_value = document.getElementById("search_billboard_date_text").value;

    pre_input_title_value = input_title_value;
    pre_input_date_value = input_date_value;

    var targetURL = gl_target_server + "/php/pushnotify.php";
    var offset = (glCurrentPage - 1) * LOAD_PAGE_NUM;
    var count = LOAD_PAGE_NUM;

    gl_billboard_List = [];

    var querytext = 'loadbillboardrecords=1'+'&project_id='+project_id+"&offset="+offset+"&count="+count;

    if(input_title_value.length > 0)
    {
        querytext = querytext + '&searchtitle=' + input_title_value;
    }
    if(input_date_value.length > 0)
    {
        querytext = querytext + '&searchdate=' + input_date_value;
    }

    $.getJSON(targetURL, querytext, function(data)
    {
        $.each(data, function(itemindex, record)
        {
            gl_billboard_List.push(record);
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

function show_billboard_records()
{
    var table_body_ui = document.getElementById('table_content');
    while (table_body_ui.firstChild)
    {
        table_body_ui.removeChild(table_body_ui.firstChild);
    }

    for(var index=0;index<gl_billboard_List.length;index++)
    {
        show_billboard(table_body_ui, (glCurrentPage-1)*LOAD_PAGE_NUM + index+1, gl_billboard_List[index]);
    }
}

function show_billboard(table_body_ui, ui_index, record)
{
    var cellIndex = 0;

    var row = document.createElement('tr');
    row.setAttribute("id", record.id+"_TABLE_ROW");
    if ((ui_index % 2) == 0)
        row.setAttribute("bgcolor","#ebebe0");

    table_body_ui.appendChild(row);

    var row_extra = document.createElement('tr');
    row_extra.setAttribute("id", record.id+"_TABLE_ROW_EXTRA");
    table_body_ui.appendChild(row_extra);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    if ( record.imageids.length > 0 )
    {
        var div_info_href_dl = document.createElement("a");
        div_info_href_dl.setAttribute("id",record.id +"_billboard_detail_button");
        div_info_href_dl.setAttribute("title","顯示詳細資訊");
        div_info_href_dl.setAttribute("onclick","open_billboard_detail(\""+record.id+"\", \""+record.imageids+"\");");
        div_info_href_dl.setAttribute("class","btn btn-default btn-xs pull-right");

        var div_info_href_dl_img = document.createElement("i");
        div_info_href_dl_img.setAttribute("id",record.id +"_billboard_detail_icon");
        div_info_href_dl_img.setAttribute("class","fa fa-plus text-green");
        div_info_href_dl.appendChild(div_info_href_dl_img);
        row.cells[cellIndex].appendChild(div_info_href_dl);
    }
    cellIndex++;

    // 標題
    row.appendChild(document.createElement('td'));
    var titleDiv = document.createElement('div');
    titleDiv.setAttribute("id", "title_"+record.id);
    titleDiv.appendChild(document.createTextNode(record.title));
    row.cells[cellIndex].appendChild(titleDiv);
    cellIndex++;

    // 內容
    row.appendChild(document.createElement('td'));
    var descriptionDiv = document.createElement('div');
    descriptionDiv.setAttribute("id", "description_"+record.id);
    descriptionDiv.appendChild(document.createTextNode(record.content));
    row.cells[cellIndex].appendChild(descriptionDiv);
    cellIndex++;

    // 公告對象
    var td_subgroups = create_subgroup(record.subgroupids);
    td_subgroups.setAttribute("id","uiid_subgroup_"+record.id);
    row.appendChild(td_subgroups);
    cellIndex++;

    // 公告時間
    row.appendChild(document.createElement('td'));
    var badge_date = document.createElement("span");
    badge_date.setAttribute("class","label label-info");
    badge_date.textContent = record.datetime;
    row.cells[cellIndex].appendChild(badge_date);
    cellIndex++;

    if (gl_groupid != 3)
    {
        // 刪除
        row.appendChild(document.createElement('td'));
        var deleteBtn = document.createElement("button");
        deleteBtn.setAttribute("id","uiid_deletebtn_"+record.id);
        deleteBtn.setAttribute("title","Delete This Record");
        deleteBtn.setAttribute("href","javascript:;");
        deleteBtn.setAttribute("onclick","confirm_delete_record(\""+record.title+"\", \""+record.id+"\");return false;");
        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-trash");
        deleteBtn.appendChild(imgDelete);
        row.cells[cellIndex].appendChild(deleteBtn);
        cellIndex++;
    }
}

function open_billboard_detail(id, imageids)
{
    var div_info_href_dl = document.getElementById(id+"_billboard_detail_button");
    div_info_href_dl.setAttribute("title","關閉詳細資訊");
    div_info_href_dl.setAttribute("onclick","close_billboard_detail(\""+id+"\", \""+imageids+"\");");

    var div_info_href_dl_img = document.getElementById(id+"_billboard_detail_icon");
    div_info_href_dl_img.setAttribute("class","fa fa-minus text-red");


    var row_extra = document.getElementById(id+"_TABLE_ROW_EXTRA");
    row_extra.setAttribute("bgcolor","#ebebe0");

    // index
    row_extra.appendChild(document.createElement('td'));

    var rd_notify_image = document.createElement('td')
    rd_notify_image.setAttribute("colspan","4");
    row_extra.appendChild(rd_notify_image);

    var row_notify_image = document.createElement('div')
    row_notify_image.setAttribute("class","row");
    row_notify_image.setAttribute("id","notify_image_"+id);
    rd_notify_image.appendChild(row_notify_image);

    load_billboard_images(id, imageids);
}

function close_billboard_detail(id, imageids)
{
    var div_info_href_dl = document.getElementById(id+"_billboard_detail_button");
    div_info_href_dl.setAttribute("title","顯示詳細資訊");
    div_info_href_dl.setAttribute("onclick","open_billboard_detail(\""+id+"\", \""+imageids+"\");");

    var div_info_href_dl_img = document.getElementById(id+"_billboard_detail_icon");
    div_info_href_dl_img.setAttribute("class","fa fa-plus text-green");

    clearChildView(id+"_TABLE_ROW_EXTRA");
}

function load_billboard_images(id, imageids)
{
    var imageuidlist = imageids.split(",");

    for(var i=0;i<imageuidlist.length;i++)
    {
        var imageuid = imageuidlist[i];

        load_billboard_image(id, imageuid);
    }
}

function load_billboard_image(id, imageuid)
{
    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadsingleimage=1&project_id='+project_id+'&imageuid='+imageuid, function(storageImage)
    {

        var ui_id = 'show_img_area_'+imageuid;
        var show_img_area_image_ui = document.getElementById(ui_id);
        if ( show_img_area_image_ui == undefined )
        {

            // console.log("load_PushNotify_image() show_img_area_image_ui("+ show_img_area_image_ui +")  == undefined");

            var show_img_area = document.getElementById("notify_image_"+id);

            var col_filetype = create_data_image("公告照片", id, storageImage);
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

    // var item_user_span = document.createElement("span");
    // data_li.appendChild(item_user_span);
    //
    // var ui_type = create_UI_record_devices(storageImage.accountid);
    // item_user_span.appendChild(ui_type);

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

function create_subgroup(subgroupids)
{
    var d = document.createElement("td");

    var subgroupList = subgroupids.split(",");

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
            var subgroupid = parseInt(subgroupList[index]);
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

var gl_search_title_start = 0;
function search_billboard_title_text_change()
{
    var input_value = document.getElementById("search_billboard_title_text").value;

    if ( input_value === pre_input_title_value )
    {
        console.log("search_billboard_title_text_change() input_value === pre_input_title_value, input_value:"+input_value);
        return;
    }

    gl_search_title_start = 0;

    glCurrentPage = 1;
    reload_page();

    return false;
}

function check_search_billboard_title_text(e)
{
    if ( gl_search_title_start == 0 )
    {
        gl_search_title_start = new Date();
    }

    var cur_time = new Date();
    var seconds = (cur_time.getTime() - gl_search_title_start.getTime()) / 1000;

    // console.log("check_search_text_change() seconds:"+seconds);

    if ( CurrentLoadIndex != LOAD_STATE_END )
    {
        console.log("check_search_billboard_title_text() CurrentLoadIndex != LOAD_STATE_END");

        setTimeout(search_billboard_title_text_change, 500);
        return;
    }

    if ( seconds >= 1 )
    {
        gl_search_title_start = 0;
        search_billboard_title_text_change();
    }
    else
    {
        setTimeout(search_billboard_title_text_change, 500);
    }
}

var gl_search_date_start = 0;
function search_billboard_date_text_change()
{
    var input_value = document.getElementById("search_billboard_date_text").value;

    if ( input_value === pre_input_title_value )
    {
        console.log("search_billboard_date_text_change() input_value === pre_input_title_value, input_value:"+input_value);
        return;
    }

    gl_search_date_start = 0;

    glCurrentPage = 1;
    reload_page();

    return false;
}

function check_search_billboard_date_text(e)
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
        console.log("check_search_billboard_date_text() CurrentLoadIndex != LOAD_STATE_END");

        setTimeout(search_billboard_date_text_change, 500);
        return;
    }

    if ( seconds >= 1 )
    {
        gl_search_date_start = 0;
        search_billboard_date_text_change();
    }
    else
    {
        setTimeout(search_billboard_date_text_change, 500);
    }
}


function confirm_delete_record(title, recordid)
{
    if (confirm('確定刪除公告事項:'+title+'   ?'))
    {
        // delete it!
        delete_record(recordid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_record(recordid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_billboard'] = 1;
    jsondata['recordid'] = recordid;

    var targetURL = gl_target_server + "/php/pushnotify.php";

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

            glCurrentPage = 1;
            reload_page();
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

const $source = document.querySelector('#search_billboard_title_text');
$source.addEventListener('input', check_search_billboard_title_text);

const $source_date = document.querySelector('#search_billboard_date_text');
$source_date.addEventListener('input', check_search_billboard_date_text);

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

function reload_page()
{
    PreLoadIndex = 0;
    CurrentLoadIndex = 1;
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

function open_billboardrecords_page()
{
    var win = window.open('/index.php/vilsbillboardrecords/'+project_id, '_blank');
    win.focus();
}
