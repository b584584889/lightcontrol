/*
 * Author: ikki Chung
 * Date: 2020.03.20
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_USER_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var glCurrentPage = 1;

var gl_users = [];
var gl_usergroups = [];
var gl_userdatasubgroups = [];
var gl_unknownrfidcards = [];
var gl_unknownnfccards = [];

var pre_input_value = '';
var gl_uploadimage_uid = '';

var glActiveSubgroup = '';

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

                load_usergroup();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_userdatasubgroup();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                //load_user();
                load_user_info();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_user_photo();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_unknownnfccard();
                load_unknownrfidcard();
                create_dropdown_list();
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

function load_usergroup()
{
    gl_usergroups = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadusergroup=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, group)
        {
            group.groupid = parseInt(group.groupid);
            gl_usergroups.push(group);
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
    var input_value = document.getElementById("map_search_text").value;

    // console.log("load_user_info() input_value:"+input_value);

    if (input_value.length > 0)
    {
        pre_input_value = input_value;

        search_text_change(input_value);
    }
    else
    {
        load_user();
    }
}

function load_user()
{
    var offset = (glCurrentPage - 1) * LOAD_USER_NUM;
    var count = LOAD_USER_NUM;

    gl_users = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduser=1&s='+offset+'&count='+count+'&project_id='+project_id+'&subgroup='+glActiveSubgroup, function(data)
    {
        $.each(data, function(index, user)
        {
            gl_users.push(user);
        });
    })
    .success(function() {
        draw_table();
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function(err) {
        console.log(err);
    })
    .complete(function() {
    });
}

function show_user_photo()
{
    // console.log("show_user_photo() gl_users.length:"+gl_users.length);

    for (var i = 0; i < gl_users.length; ++i)
    {
        var user = gl_users[i];

        if ( user.imageuid.length > 0 )
        {
            load_user_photo(user.userid, user.imageuid);
        }
        // else
        // {
        //     console.log("show_user_photo() user.userid:"+user.userid+" imageuid:"+user.imageuid);
        // }
    }
}

function load_user_photo(userid, imageuid)
{
    // console.log("load_user_photo() userid:"+userid+" imageuid:"+imageuid);

    var targetURL = gl_target_server + "/php/storage.php";
    var p = $.getJSON(targetURL, 'loadsingleimage=1&project_id='+project_id+'&imageuid='+imageuid, function(storageImage)
    {
        var ui_id = 'uiid_photo_'+userid;
        var show_img_area_image_ui = document.getElementById(ui_id);
        if ( show_img_area_image_ui != undefined )
        {
            // console.log("load_user_photo() storageImage.accountid:"+ storageImage.accountid +" storageImage.id:"+storageImage.id);
            var user_photo = create_user_photo(storageImage);
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

function load_unknownnfccard()
{
    gl_unknownnfccards = [];

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadunknowncard=1&s=0&count=20&project_id='+project_id, function(data)
    {
        var nfccard_num = 0;
        $.each(data, function(index, device)
        {
            gl_unknownnfccards.push(device);
            nfccard_num++;
        });

    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_unknownrfidcard()
{
    gl_unknownrfidcards = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadunknowncard=1&s=0&count=20&project_id='+project_id, function(data)
    {
        var rfidcard_num = 0;
        $.each(data, function(index, device)
        {
            gl_unknownrfidcards.push(device);
            rfidcard_num++;
        });

    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function create_dropdown_list()
{
    var current_subgroup_title = clearChildView('current_subgroup_title');
    var dropdown_menu_subgroup = clearChildView('dropdown_menu_subgroup');

    if ( glActiveSubgroup.length == 0 )
    {
        var subgroup_span = create_subgroup_label("全部");
        current_subgroup_title.appendChild(subgroup_span);
    }

    dropdown_menu_subgroup.appendChild(create_dropdown_type_item('', "全部"));

    var int_glActiveSubgroup = parseInt(glActiveSubgroup);

    for (var i = 0; i < gl_userdatasubgroups.length; ++i)
    {
        var subgroup = gl_userdatasubgroups[i];

        if (int_glActiveSubgroup == subgroup.subgroupid)
        {
            var subgroup_span = create_subgroup_label(subgroup.subgroupname);
            current_subgroup_title.appendChild(subgroup_span);
        }

        dropdown_menu_subgroup.appendChild(create_dropdown_type_item(subgroup.subgroupid, subgroup.subgroupname));
    }
}

function create_dropdown_type_item(subgroupid, subgroupname)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", subgroupid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","click_dropdown_theme_item(\""+ subgroupid +"\", \""+subgroupname+"\");return true;");

    var subgroup_span = create_subgroup_label(subgroupname);

    item_click.appendChild(subgroup_span);
    item_li.appendChild(item_click);

    return item_li;
}

function click_dropdown_theme_item(subgroupid, subgroupname)
{
    glActiveSubgroup = subgroupid;

    // var current_subgroup_title = clearChildView('current_subgroup_title');
    // var subgroup_span = create_subgroup_label(subgroupname);
    // current_subgroup_title.appendChild(subgroup_span);

    reload_all_info();
}


function search_text_change(input_value)
{
    var offset = (glCurrentPage - 1) * LOAD_USER_NUM;
    var count = LOAD_USER_NUM;

    gl_users = [];

    var SendDate = "searchuser=1&s="+offset+"&count="+count+"&project_id="+project_id+"&text="+input_value+'&subgroup='+glActiveSubgroup;

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
                gl_users.push(user);
            });
            draw_table();
            CurrentLoadIndex = CurrentLoadIndex + 1;
        }
    });
    return false;

}

function draw_table()
{
    //console.log("draw_table() glTagLocation.length:"+glTagLocation.length);

    var record_table = document.getElementById('USER_TABLE');

    while (record_table.rows.length >= 1)
         record_table.deleteRow(0);

     // console.log("draw_table() gl_users.length:"+gl_users.length);


     var offset = (glCurrentPage - 1) * LOAD_USER_NUM;

    for (var i = 0; i < gl_users.length; ++i)
    {
        var user = gl_users[i];

        var row = create_record_table_item(offset + i, user);
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
    var tdName = document.createElement('td');
    tdName.setAttribute("id","uiid_name_"+record.userid);
    //tdName.setAttribute("style","height: 90px;");
    row.appendChild(tdName);
    var SpanName = document.createElement("span");
    //Spanmac.setAttribute("class","badge bg-green-active");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = record.name;
    tdName.appendChild(SpanName);
    cellIndex++;

    // account
    var tdaccount = document.createElement('td');
    tdaccount.setAttribute("id","uiid_account_"+record.userid);
    //tdaccount.setAttribute("style","height: 90px;");
    row.appendChild(tdaccount);
    var Spanaccount  = document.createElement("span");
    //Spanmac.setAttribute("class","badge bg-green-active");
    Spanaccount.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    Spanaccount.textContent = record.account;
    row.cells[cellIndex].appendChild(Spanaccount);
    cellIndex++;

    // user.id
    var tdunit = document.createElement('td');
    tdunit.setAttribute("id","uiid_id_"+record.userid);
    tdunit.textContent = record.userid;
    row.appendChild(tdunit);
    cellIndex++;

    // // unit
    // var tdunit = document.createElement('td');
    // tdunit.setAttribute("id","uiid_unit_"+record.userid);
    // //tdunit.setAttribute("style","height: 90px;");
    // row.appendChild(tdunit);
    // var Spanunit   = document.createElement("span");
    // Spanunit.setAttribute("class","label label-success");
    // Spanunit.textContent = record.unit;
    // row.cells[cellIndex].appendChild(Spanunit);
    // cellIndex++;

    // title
    var tdtitle = document.createElement('td');
    tdtitle.setAttribute("id","uiid_title_"+record.userid);
    //tdtitle.setAttribute("style","height: 90px;");
    row.appendChild(tdtitle);
    var Spantitle  = document.createElement("span");
    if (record.title.length > 0)
    {
        // Spantitle.setAttribute("class","label label-primary");
        Spantitle.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(185, 118, 211);");
    }
    Spantitle.textContent = record.title;
    row.cells[cellIndex].appendChild(Spantitle);
    cellIndex++;

    // 照片
    row.appendChild(document.createElement('td'));
    var Spandatagroup = document.createElement("span");
    Spandatagroup.setAttribute("id","uiid_photo_"+record.userid);
    row.cells[cellIndex].appendChild(Spandatagroup);
    cellIndex++;

    // groupid
    var tdgroupid = document.createElement('td');
    //tdgroupid.setAttribute("id","uiid_groupid_"+record.userid);
    row.appendChild(tdgroupid);

    var Spangroupid  = document.createElement("span");
    Spangroupid.setAttribute("id","uiid_groupid_"+record.userid);
    tdgroupid.appendChild(Spangroupid);

    var groupidui = create_group_label(getGroupName(parseInt(record.groupid)));
    Spangroupid.appendChild(groupidui);

    var group_selected_div = document.createElement("div");
    group_selected_div.setAttribute("id", "input_groupid_selected_"+record.userid);
    tdgroupid.appendChild(group_selected_div);
    cellIndex++;

    // var Spangroupid  = document.createElement("span");
    // Spangroupid.setAttribute("class","label label-warning");
    // Spangroupid.textContent = getGroupName(parseInt(record.groupid));

    // data group
    var tddatagroup = document.createElement('td');
    //tddatagroup.setAttribute("id","uiid_datagroups_"+record.userid);
    row.appendChild(tddatagroup);

    var Spandatagroup  = document.createElement("span");
    Spandatagroup.setAttribute("id","uiid_datagroups_"+record.userid);
    tddatagroup.appendChild(Spandatagroup);

    var datagroupui = createDataGroupUI(record.datagroups);
    Spandatagroup.appendChild(datagroupui);

    var datagroups_selected_div = document.createElement("div");
    datagroups_selected_div.setAttribute("id", "input_datagroups_selected_"+record.userid);
    tddatagroup.appendChild(datagroups_selected_div);
    cellIndex++;
    // var Spandatagroup  = document.createElement("span");
    // Spandatagroup.setAttribute("class","label label-success");
    // Spandatagroup.textContent = record.datagroups;
    // row.cells[cellIndex].appendChild(Spandatagroup);
    // cellIndex++;

    // sub-group
    var tdsubgroup = document.createElement('td');
    //tdsubgroup.setAttribute("id","uiid_subgroups_"+record.userid);
    row.appendChild(tdsubgroup);

    var Spansubgroup  = document.createElement("span");
    Spansubgroup.setAttribute("id","uiid_subgroups_"+record.userid);
    tdsubgroup.appendChild(Spansubgroup);

    var subgroupui = createSubGroupUI(record.subgroups);
    Spansubgroup.appendChild(subgroupui);

    var subgroups_selected_div = document.createElement("div");
    subgroups_selected_div.setAttribute("id", "input_subgroups_selected_"+record.userid);
    tdsubgroup.appendChild(subgroups_selected_div);
    cellIndex++;

    //==================================
    //  rfidcards
    var tdrfidcards = document.createElement('td');
    tdrfidcards.setAttribute("id","uiid_rfidcards_"+record.userid);
    row.appendChild(tdrfidcards);
    var ui_rfidcards = create_ui_rfidcards(record.rfidcards);
    row.cells[cellIndex].appendChild(ui_rfidcards);
    cellIndex++;
    //==================================

    //==================================
    //  nfccards
    var tdnfccards = document.createElement('td');
    tdnfccards.setAttribute("id","uiid_nfccards_"+record.userid);
    row.appendChild(tdnfccards);
    var ui_nfccards = create_ui_nfccards(record.nfccards);
    row.cells[cellIndex].appendChild(ui_nfccards);
    cellIndex++;
    //==================================

    //==================================
    //  uwbcards
    var tduwbcards = document.createElement('td');
    tduwbcards.setAttribute("id","uiid_uwbcards_"+record.userid);
    row.appendChild(tduwbcards);
    var ui_uwbcards = create_ui_uwbcards(record.uwbcards);
    row.cells[cellIndex].appendChild(ui_uwbcards);
    cellIndex++;
    //==================================

    //==================================
    //  sensors
    var tdsensors = document.createElement('td');
    tdsensors.setAttribute("id","uiid_sensors_"+record.userid);
    row.appendChild(tdsensors);
    var ui_sensors = create_ui_sensors(record.sensors);
    row.cells[cellIndex].appendChild(ui_sensors);
    cellIndex++;
    //==================================

    var tdoperation = document.createElement('td');
    row.appendChild(tdoperation);

    var btnQrcode = document.createElement("button");
    btnQrcode.setAttribute("type","button");
    btnQrcode.setAttribute("class","btn btn-default");
    btnQrcode.setAttribute("data-toggle","modal");
    btnQrcode.setAttribute("data-target","#modal-qrcode");
    btnQrcode.setAttribute("onclick","device_qrcode_show(\"2\",\""+ record.name +"\",\""+ record.account +"\",\""+ record.userid +"\");return false;");

    var imgQrcode = document.createElement("i");
    imgQrcode.setAttribute("class","fa fa-qrcode");
    btnQrcode.appendChild(imgQrcode);
    row.cells[cellIndex].appendChild(btnQrcode);

    if (config_enable_edit)
    {

        // Edit button
        var editBtn = document.createElement("button");
        editBtn.setAttribute("id","uiid_editbtn_"+record.userid);
        editBtn.setAttribute("title","Edit this User");
        editBtn.setAttribute("href","javascript:;");
        editBtn.setAttribute("onclick","edit_user(\""+record.userid+"\");return false;");
        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("id","uiid_editimg_"+record.userid);
        imgEdit.setAttribute("class","fa fa-edit");
        editBtn.appendChild(imgEdit);
        row.cells[cellIndex].appendChild(editBtn);

        // topic reset button
        var resetTopicBtn = document.createElement("button");
        resetTopicBtn.setAttribute("style","display:none");
        resetTopicBtn.setAttribute("id","uiid_resettopicbtn_"+record.userid);
        resetTopicBtn.setAttribute("title","Reset Pushnotify Topic");
        resetTopicBtn.setAttribute("href","javascript:;");
        resetTopicBtn.setAttribute("onclick","confirm_reset_user_topic(\""+record.userid+"\", \""+ record.name +"\", \""+ record.account +"\");return false;");
        var imgResetTopic = document.createElement("i");
        imgResetTopic.setAttribute("class","fa fa-unlink");
        resetTopicBtn.appendChild(imgResetTopic);
        row.cells[cellIndex].appendChild(resetTopicBtn);

        // password reset button
        var resetBtn = document.createElement("button");
        resetBtn.setAttribute("style","display:none");
        resetBtn.setAttribute("id","uiid_resetbtn_"+record.userid);
        resetBtn.setAttribute("title","Reset User password");
        resetBtn.setAttribute("href","javascript:;");
        resetBtn.setAttribute("onclick","confirm_reset_user_password(\""+record.userid+"\", \""+ record.name +"\", \""+ record.account +"\");return false;");
        var imgReset = document.createElement("i");
        imgReset.setAttribute("class","fa fa-refresh");
        resetBtn.appendChild(imgReset);
        row.cells[cellIndex].appendChild(resetBtn);

        if (record.account != gl_accountid)
        {
            // delete uesr button
            var deleteBtn = document.createElement("button");
            deleteBtn.setAttribute("style","display:none");
            deleteBtn.setAttribute("id","uiid_deletebtn_"+record.userid);
            deleteBtn.setAttribute("title","Delete This User");
            deleteBtn.setAttribute("href","javascript:;");
            deleteBtn.setAttribute("onclick","confirm_delete_user(\""+record.userid+"\", \""+ record.name +"\", \""+ record.account +"\");return false;");
            var imgDelete = document.createElement("i");
            imgDelete.setAttribute("class","fa fa-user-times");
            deleteBtn.appendChild(imgDelete);
            row.cells[cellIndex].appendChild(deleteBtn);
            cellIndex++;
        }
    }

    return row;
}

function createDataGroupUI(datagroups)
{
    var td_datagroups_div = document.createElement("div");

    var datagroup_div = document.createElement("div");
    td_datagroups_div.appendChild(datagroup_div);

    var datagroupList = datagroups.split(",");

    for(var i=0;i<datagroupList.length;i++)
    {
        var datagroup = datagroupList[i];

        if ( datagroup == '99999' )
        {
            var datagroup_span = create_datagroup_label('全部');
            datagroup_div.appendChild(datagroup_span);
            return td_datagroups_div;
        }
    }



    var datagroup_span = create_datagroup_label('自己');
    datagroup_div.appendChild(datagroup_span);

    // var datagroup_span = document.createElement("span");
    // //datagroup_span.setAttribute("class","label label-primary");
    // datagroup_span.textContent = '自己';
    // datagroup_div.appendChild(datagroup_span);

    for(var i=0;i<datagroupList.length;i++)
    {
        var datagroup = datagroupList[i];

        if ( datagroup.length == 0 )
        {
            continue;
        }

        var datagroupName = getSubGroupName(datagroup);

        if ( datagroupName.length == 0 )
        {
            continue;
        }

        var datagroup_div = document.createElement("div");
        td_datagroups_div.appendChild(datagroup_div);

        var datagroup_span = create_datagroup_label(datagroupName);
        datagroup_div.appendChild(datagroup_span);

        // var datagroup_span = document.createElement("span");
        // //datagroup_span.setAttribute("class","label label-primary");
        // datagroup_span.textContent = getSubGroupName(datagroup);
        // datagroup_div.appendChild(datagroup_span);
    }
    return td_datagroups_div;
}

function createSubGroupUI(subgroups)
{
    var td_subgroups_div = document.createElement("div");

    var subgroupList = subgroups.split(",");
    for(var i=0;i<subgroupList.length;i++)
    {
        var subgroup = subgroupList[i];

        if ( subgroup.length == 0 )
        {
            continue;
        }

        var subgroupName = getSubGroupName(subgroup);

        if ( subgroupName.length == 0 )
        {
            continue;
        }

        var subgroup_div = document.createElement("div");
        td_subgroups_div.appendChild(subgroup_div);

        var subgroup_span = create_subgroup_label(subgroupName);
        subgroup_div.appendChild(subgroup_span);

        // var subgroup_span = document.createElement("span");
        // subgroup_span.setAttribute("class","label label-success");
        // subgroup_span.textContent = getSubGroupName(subgroup);
        // subgroup_div.appendChild(subgroup_span);
    }
    return td_subgroups_div;
}

function create_group_label(context)
{
    var group_span = document.createElement("span");
    group_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#f0ad4e;");
    group_span.textContent = context;
    return group_span;
}

function create_datagroup_label(context)
{
    var datagroup_span = document.createElement("span");
    datagroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(54, 127, 169);");
    datagroup_span.textContent = context;
    return datagroup_span;
}

function create_subgroup_label(context)
{
    var subgroup_span = document.createElement("span");
    subgroup_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    subgroup_span.textContent = context;
    return subgroup_span;
}

function create_ui_rfidcards(rfidcards)
{
    var td_rfidcards_div = document.createElement("div");

    var rfidcardList = rfidcards.split(",");
    for(var i=0;i<rfidcardList.length;i++)
    {
        var rfidcard = rfidcardList[i];

        if ( rfidcard.length == 0 )
        {
            continue;
        }

        var rfidcard_div = document.createElement("div");
        td_rfidcards_div.appendChild(rfidcard_div);

        var Spanrfidcard = document.createElement("span");
        Spanrfidcard.setAttribute("class","label label-primary");
        Spanrfidcard.textContent = rfidcard;
        rfidcard_div.appendChild(Spanrfidcard);
    }
    return td_rfidcards_div;
}

function create_ui_nfccards(nfccards)
{
    var td_nfccards_div = document.createElement("div");

    var nfccardList = nfccards.split(",");
    for(var i=0;i<nfccardList.length;i++)
    {
        var nfccard = nfccardList[i];

        if ( nfccard.length == 0 )
        {
            continue;
        }

        var nfccard_div = document.createElement("div");
        td_nfccards_div.appendChild(nfccard_div);

        var Spannfccard = document.createElement("span");
        Spannfccard.setAttribute("class","label label-success");
        Spannfccard.textContent = nfccard;
        nfccard_div.appendChild(Spannfccard);
    }
    return td_nfccards_div;
}

function create_ui_uwbcards(uwbcards)
{
    var td_uwbcards_div = document.createElement("div");

    var uwbcardList = uwbcards.split(",");
    for(var i=0;i<uwbcardList.length;i++)
    {
        var uwbcard = uwbcardList[i];

        if ( uwbcard.length == 0 )
        {
            continue;
        }

        var uwbcard_div = document.createElement("div");
        td_uwbcards_div.appendChild(uwbcard_div);

        var Spannuwbcard = document.createElement("span");
        Spannuwbcard.setAttribute("class","badge bg-light-blue-active");
        Spannuwbcard.textContent = uwbcard;
        uwbcard_div.appendChild(Spannuwbcard);
    }
    return td_uwbcards_div;
}

function create_ui_sensors(sensors)
{
    var td_sensors_div = document.createElement("div");

    var sensorList = sensors.split(",");
    for(var i=0;i<sensorList.length;i++)
    {
        var sensor = sensorList[i];

        if ( sensor.length == 0 )
        {
            continue;
        }

        var sensordevice_div = document.createElement("div");
        td_sensors_div.appendChild(sensordevice_div);

        var Spansensordevice = document.createElement("span");
        Spansensordevice.setAttribute("class","badge bg-blue");
        Spansensordevice.textContent = sensor;
        sensordevice_div.appendChild(Spansensordevice);
    }
    return td_sensors_div;
}

function create_empty_record_table_item(ui_index)
{
    var empty_record_table_item = document.getElementById('empty_record_table_item');
    if ( empty_record_table_item != null )
        return;

    var record_table = document.getElementById('USER_TABLE');
    var row = record_table.insertRow(0);
    row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id", "empty_record_table_item");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""));
    cellIndex++;

    // name
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_name"));
    cellIndex++;

    // account
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_account"));
    cellIndex++;

    // userid
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // // unit
    // row.appendChild(document.createElement('td'));
    // row.cells[cellIndex].appendChild(create_input_text("input_unit"));
    // cellIndex++;

    // title
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_title"));
    cellIndex++;

    // photo
    row.appendChild(document.createElement('td'));
    var div_upload = create_upload_image_ui();
    row.cells[cellIndex].appendChild(div_upload);
    cellIndex++;

    // group
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_edit_group_ui("input_groupid", '99999', 2));
    var group_selected_div = document.createElement("div");
    group_selected_div.setAttribute("id", "input_groupid_selected_99999");
    row.cells[cellIndex].appendChild(group_selected_div);
    cellIndex++;

    update_selected_group('99999', 2);

    // datagroup
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_edit_datagroup_ui("input_datagroups", '99999', ''));
    var datagroups_selected_div = document.createElement("div");
    datagroups_selected_div.setAttribute("id", "input_datagroups_selected_99999");
    row.cells[cellIndex].appendChild(datagroups_selected_div);
    cellIndex++;

    // subgroup
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_edit_subgroup_ui("input_subgroups", '99999', ''));
    var subgroups_selected_div = document.createElement("div");
    subgroups_selected_div.setAttribute("id", "input_subgroups_selected_99999");
    row.cells[cellIndex].appendChild(subgroups_selected_div);
    cellIndex++;

    // rfidcards
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_rfidcards"));
    cellIndex++;

    // nfccards
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_nfccards"));
    cellIndex++;

    // uwbcards
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_uwbcards"));
    cellIndex++;

    // sensors
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(create_input_text("input_sensors"));
    cellIndex++;

    // save button
    row.appendChild(document.createElement('td'));
    var Button_save = document.createElement("button");
    Button_save.setAttribute("class","btn btn-block btn-primary btn-lg");
    Button_save.setAttribute("onclick","save_new_user();");
    Button_save.textContent = "儲存";
    row.cells[cellIndex].appendChild(Button_save);
    cellIndex++;

    // record_table.appendChild(row);
}

function create_input_text(ui_id, text)
{
    var input_text = document.createElement("textarea");
    input_text.setAttribute("rows","1");
    input_text.setAttribute("id",ui_id);
    input_text.textContent = text;

    return input_text;
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

var gl_edit_userid = 0;
var gl_edit_user_imageuid = 0;

function edit_user(userid)
{
    // console.log("edit_user() userid:"+userid);

    var user = getUser(userid);
    if (user == null)
        return;

    // console.log("edit_user() userid:"+userid +" user.groupid:"+user.groupid);

    // disable another edit button
    for (var i = 0; i < gl_users.length; ++i)
    {
        var anotherUser = gl_users[i];
        if ( anotherUser.userid != userid )
        {
            var anotherEditBtn = document.getElementById("uiid_editbtn_"+anotherUser.userid);
            anotherEditBtn.setAttribute("style","display:none;");
        }
    }

    create_edit_ui("uiid_name", userid, user.name);
    // create_edit_ui("uiid_account", userid, user.account);
    create_edit_ui("uiid_title", userid, user.title);

    gl_edit_user_imageuid = user.imageuid;
    var show_img_area_image_ui = document.getElementById('uiid_photo_'+userid);
    if ( show_img_area_image_ui != undefined )
    {
        if ( gl_edit_user_imageuid.length == 0 )
        {
            clearChildView('uiid_photo_'+userid);
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
            deletePhotoBtn.setAttribute("title","Delete User Photo");
            deletePhotoBtn.setAttribute("href","javascript:;");
            deletePhotoBtn.setAttribute("onclick","delete_user_photo(\""+userid+"\");return false;");
            var imgdelete = document.createElement("i");
            imgdelete.setAttribute("class","fa fa-remove");
            deletePhotoBtn.appendChild(imgdelete);
            show_img_area_image_ui.appendChild(deletePhotoBtn);
        }
    }

    create_edit_group_ui("uiid_groupid", userid, parseInt(user.groupid));
    create_edit_datagroup_ui("uiid_datagroups", userid, user.datagroups);
    create_edit_subgroup_ui("uiid_subgroups", userid, user.subgroups);
    var ui_rfidcards = create_edit_ui("uiid_rfidcards", userid, user.rfidcards);
    var ui_unregister_rfidcards = create_unregister_card_ui("rfid", userid, gl_unknownrfidcards);
    ui_rfidcards.appendChild(ui_unregister_rfidcards);

    var ui_nfccards = create_edit_ui("uiid_nfccards", userid, user.nfccards);
    var ui_unregister_nfccards = create_unregister_card_ui("nfc", userid, gl_unknownnfccards);
    ui_nfccards.appendChild(ui_unregister_nfccards);

    create_edit_ui("uiid_uwbcards", userid, user.uwbcards);
    create_edit_ui("uiid_sensors", userid, user.sensors);

    var editBtn = document.getElementById("uiid_editbtn_"+user.userid);
    editBtn.setAttribute("title","Save this User");
    editBtn.setAttribute("onclick","save_user(\""+user.userid+"\");return false;");
    var imgEdit = document.getElementById("uiid_editimg_"+user.userid);
    imgEdit.setAttribute("class","fa fa-save");

    // topic reset button
    var resetTopicBtn = document.getElementById("uiid_resettopicbtn_"+user.userid);
    resetTopicBtn.setAttribute("style","display:block");

    // password reset button
    var resetBtn = document.getElementById("uiid_resetbtn_"+user.userid);
    resetBtn.setAttribute("style","display:block");

    var deleteBtn = document.getElementById("uiid_deletebtn_"+user.userid);
    if ( deleteBtn != undefined )
        deleteBtn.setAttribute("style","display:block");

    update_selected_group(userid, 0);
    update_selected_datagroup(userid, 0);
    update_selected_subgroup(userid, 0);

}

function delete_user_photo(userid)
{
    gl_edit_user_imageuid = "";

    clearChildView('uiid_photo_'+userid);
    var show_img_area_image_ui = document.getElementById('uiid_photo_'+userid);
    if ( show_img_area_image_ui != undefined )
    {
        var div_upload = create_upload_image_ui();
        show_img_area_image_ui.appendChild(div_upload);
    }
}

function save_user(userid)
{
    gl_edit_userid = userid;
    check_upload_image(uploadEditUserImageFinihed);
}

function uploadEditUserImageFinihed()
{
    // console.log("uploadEditUserImageFinihed()");
    uploadImageFinihed(save_edit_user);
}

function save_edit_user()
{
    var userid = gl_edit_userid;
    gl_edit_userid = 0;

    var value_name = document.getElementById("uiid_name_"+userid+"_edit").value;
    // var value_account = document.getElementById("uiid_account_"+userid+"_edit").value;
    //var value_unit = document.getElementById("uiid_unit_"+userid+"_edit").value;
    var value_title = document.getElementById("uiid_title_"+userid+"_edit").value;

    //console.log("save_user() userid:"+userid);
    //console.log("save_user() value_name:"+value_name);
    //console.log("save_user() value_account:"+value_account);
    //console.log("save_user() value_unit:"+value_unit);
    //console.log("save_user() value_title:"+value_title);

    //var value_gender = getSelectedGender(userid);
    var value_group = getSelectedGroup(userid);
    var value_datagroups = getSelectedDataGroup(userid);
    var value_subgroups = getSelectedSubGroup(userid);
    var value_rfidcards = document.getElementById("uiid_rfidcards_"+userid+"_edit").value;
    var value_nfccards = document.getElementById("uiid_nfccards_"+userid+"_edit").value;
    var value_uwbcards = document.getElementById("uiid_uwbcards_"+userid+"_edit").value;
    var value_sensors = document.getElementById("uiid_sensors_"+userid+"_edit").value;

    value_name = checkValidUserName(value_name);
    // value_account = checkValidUserName(value_account);
    value_rfidcards = checkValidCardid(value_rfidcards);
    value_nfccards = checkValidCardid(value_nfccards);
    value_uwbcards = checkValidCardid(value_uwbcards);
    value_sensors = checkValidCardid(value_sensors);

    // console.log("save_user() value_name:"+value_name);
    // console.log("save_user() value_title:"+value_title);
    // // console.log("save_user() value_account:"+value_account);
    // console.log("save_user() value_group:"+value_group);
    // console.log("save_user() value_datagroups:"+value_datagroups);
    // console.log("save_user() value_subgroups:"+value_subgroups);
    // console.log("save_user() value_rfidcards:"+value_rfidcards);
    // console.log("save_user() value_nfccards:"+value_nfccards);
    // console.log("save_user() value_uwbcards:"+value_uwbcards);
    // console.log("save_user() imageuid:"+gl_uploadimage_uid);

    if ( gl_uploadimage_uid.length == 0 )
        value_imageuid = gl_edit_user_imageuid;
    else
        value_imageuid = gl_uploadimage_uid;

    update_user(userid, value_name, value_title, value_group, value_datagroups, value_subgroups, value_rfidcards, value_nfccards, value_uwbcards, value_sensors, value_imageuid);
}

function checkValidCardid(input)
{
    // str = input.replace(/[^A-Za-z0-9,]*/g, '')
    return input;
}

function checkValidUserName(input)
{
    str = input.replace(/(\r\n|\n|\r)/gm, '')
    return str;
}

function create_edit_ui(uiid, userid, text)
{
    var uiid_name = clearChildView(uiid+"_"+userid);

    var ui_new_input = document.createElement("div");
    var new_input = create_input_text(uiid+"_"+userid+"_edit", text);
    ui_new_input.appendChild(new_input);

    uiid_name.appendChild(ui_new_input);

    return uiid_name;
}

function create_unregister_card_ui(type, userid, unregister_cards)
{
    var ui_unregister_rfidcards = document.createElement("div");
    ui_unregister_rfidcards.setAttribute("class","btn-group");

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.textContent = "未註冊卡片";
    ui_unregister_rfidcards.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_ul.setAttribute("id","dropdown_card_"+type);
    ui_unregister_rfidcards.appendChild(div_ul);

    for (var i = 0; i < unregister_cards.length; ++i)
    {
        var card = unregister_cards[i];
        div_ul.appendChild(create_dropdown_card_item(userid, type, i, card));
    }

    return ui_unregister_rfidcards;
}

function create_dropdown_card_item(userid, type, index, card)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", card.cardid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_card_click(\""+type+"\",\""+userid+"\",\""+ card.cardid +"\");return true;");

    // var item_checkbox = document.createElement("input");
    // item_checkbox.setAttribute("type","checkbox");
    // item_checkbox.setAttribute("id","dropdown_checkbox_"+type+"_"+userid+"_"+index);
    // item_checkbox.setAttribute("onclick","dropdown_checkbox_group_click(\""+type+"\",\""+userid+"\","+ index +");return true;");
    // item_checkbox.checked = defaultchecked;
    // item_click.appendChild(item_checkbox);

    item_click.appendChild(document.createTextNode(card.cardid));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_card_click(type, userid, cardid)
{
    console.log("dropdown_card_click() type:"+type+" userid:"+userid+" cardid:"+cardid);

    if( type === "rfid" )
    {
        var ui_edit = document.getElementById("uiid_rfidcards_"+userid+"_edit");
        var value_rfidcards = ui_edit.value;
        if (value_rfidcards.length > 0)
        {
            value_rfidcards += ",";
        }

        value_rfidcards += cardid;
        ui_edit.value = value_rfidcards;

        // console.log("dropdown_card_click() 1 gl_unknownrfidcards.length:"+gl_unknownrfidcards.length);

        clearChildView("dropdown_card_"+type);
        var div_ul = document.getElementById("dropdown_card_"+type);

        //
        // remove from list
        //
        for (var i = 0; i < gl_unknownrfidcards.length; ++i)
        {
            var card = gl_unknownrfidcards[i];
            if ( cardid === card.cardid )
            {
                gl_unknownrfidcards.splice(i, 1);
                i--;
            }
            else
            {
                div_ul.appendChild(create_dropdown_card_item(userid, type, i, card));
            }
        }

        // console.log("dropdown_card_click() 2 gl_unknownrfidcards.length:"+gl_unknownrfidcards.length);

    }
    else
    if( type === "nfc" )
    {
        var ui_edit = document.getElementById("uiid_nfccards_"+userid+"_edit");
        var value_nfccards = ui_edit.value;
        if (value_nfccards.length > 0)
        {
            value_nfccards += ",";
        }

        value_nfccards += cardid;
        ui_edit.value = value_nfccards;

        clearChildView("dropdown_card_"+type);
        var div_ul = document.getElementById("dropdown_card_"+type);

        //
        // remove from list
        //
        for (var i = 0; i < gl_unknownnfccards.length; ++i)
        {
            var card = gl_unknownnfccards[i];
            if ( cardid === card.cardid )
            {
                gl_unknownnfccards.splice(i, 1);
                i--;
            }
            else
            {
                div_ul.appendChild(create_dropdown_card_item(userid, type, i, card));
            }
        }
    }

}

function create_edit_gender_ui(uiid, userid, gender)
{
    var ui_gender = clearChildView(uiid+"_"+userid);

    var div_btngroup = document.createElement("div");
    div_btngroup.setAttribute("class","btn-group");
    if (ui_gender != null)
        ui_gender.appendChild(div_btngroup);

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    //div_button.setAttribute("id","dropdown_gender_"+userid);
    div_button.textContent = "性別";
    div_btngroup.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    //div_ul.setAttribute("id","dropdown_menu_gender_"+userid);
    div_btngroup.appendChild(div_ul);

    div_ul.appendChild(create_dropdown_gender_item(userid, 0, '0', '未知', (gender === '0')));
    div_ul.appendChild(create_dropdown_gender_item(userid, 1, '1',  '男', (gender === '1')));
    div_ul.appendChild(create_dropdown_gender_item(userid, 2, '2',  '女', (gender === '2')));

    return div_btngroup;
}

function create_dropdown_gender_item(userid, index, datavalue, dataname, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", datavalue);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_gender_click(\""+userid+"\","+ index +");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_gender_"+userid+"_"+index);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_gender_click(\""+userid+"\","+ index +");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(dataname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_checkbox_gender_click(userid, index)
{
    glDropdown_checkbox_gender_click = true;

    update_selected_gender(userid, index);
}

var glDropdown_checkbox_gender_click = false;
function dropdown_gender_click(userid, index)
{
    if (glDropdown_checkbox_gender_click == true)
    {
        glDropdown_checkbox_gender_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_gender_"+userid+"_"+index).checked == true)
        document.getElementById("dropdown_checkbox_gender_"+userid+"_"+index).checked = false;
    else
        document.getElementById("dropdown_checkbox_gender_"+userid+"_"+index).checked = true;

    update_selected_gender(userid, index);

    glDropdown_checkbox_gender_click = false;
}

function update_selected_gender(userid, uiindex)
{
    var count=0;
    for (var index=0;index<=2;index++)
    {
        if (uiindex == index)
        {
            if (document.getElementById("dropdown_checkbox_gender_"+userid+"_"+index).checked == false)
            {
                count++;
            }
        }
        else
        {
            count++;
            document.getElementById("dropdown_checkbox_gender_"+userid+"_"+index).checked = false;
        }
    }

    if ( count == 3 )
    {
        // all uncheck , must at least one checked
        document.getElementById("dropdown_checkbox_gender_"+userid+"_"+uiindex).checked = true;
    }
}

function create_edit_group_ui(uiid, userid, groupid)
{
    var ui_gender = clearChildView(uiid+"_"+userid);

    var div_btngroup = document.createElement("div");
    div_btngroup.setAttribute("class","btn-group");
    if (ui_gender != null)
        ui_gender.appendChild(div_btngroup);

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    //div_button.setAttribute("id","dropdown_group_"+userid);
    div_button.textContent = "權限群組";
    div_btngroup.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    //div_ul.setAttribute("id","dropdown_menu_group_"+userid);
    div_btngroup.appendChild(div_ul);

    for (var i = 0; i < gl_usergroups.length; ++i)
    {
        var group = gl_usergroups[i];
        var target_groupid = parseInt(group.groupid);

        // console.log("create_edit_group_ui() target_groupid:"+target_groupid+" groupid:"+groupid+" gl_groupid:"+gl_groupid);

        if ( gl_groupid == 1 || gl_groupid == 2 )
        {
            div_ul.appendChild(create_dropdown_group_item(userid, "group", i, target_groupid, group.groupname, (groupid == target_groupid)));
        }
        else
        {
            if (target_groupid != 1 && target_groupid != 2)
            {
                div_ul.appendChild(create_dropdown_group_item(userid, "group", i, target_groupid, group.groupname, (groupid == target_groupid)));
            }
        }
    }

    return div_btngroup;
}

function create_edit_datagroup_ui(uiid, userid, datagroups)
{
    var ui_datagroup = clearChildView(uiid+"_"+userid);

    var div_btngroup = document.createElement("div");
    div_btngroup.setAttribute("class","btn-group");
    if (ui_datagroup != null)
        ui_datagroup.appendChild(div_btngroup);

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    //div_button.setAttribute("id","dropdown_group_"+userid);
    div_button.textContent = "資料權限群組";
    div_btngroup.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    //div_ul.setAttribute("id","dropdown_menu_group_"+userid);
    div_btngroup.appendChild(div_ul);

    var datagroupList = datagroups.split(",");

    // 全部
    if ( gl_groupid == 1 || gl_groupid == 2 )
    {
        var existed = check_numer_in_list(99999, datagroupList);
        div_ul.appendChild(create_dropdown_group_item(userid, "datagroup", 0, 99999, '全部', existed));
    }

    for (var i = 0; i < gl_userdatasubgroups.length; ++i)
    {
        var subgroup = gl_userdatasubgroups[i];
        var existed = check_numer_in_list(subgroup.subgroupid, datagroupList);
        div_ul.appendChild(create_dropdown_group_item(userid, "datagroup", i+1, subgroup.subgroupid, subgroup.subgroupname, existed));
    }

    return div_btngroup;
}

function create_edit_subgroup_ui(uiid, userid, subgroups)
{
    var ui_subgroup = clearChildView(uiid+"_"+userid);

    var div_btngroup = document.createElement("div");
    div_btngroup.setAttribute("class","btn-group");
    if (ui_subgroup != null)
        ui_subgroup.appendChild(div_btngroup);

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    //div_button.setAttribute("id","dropdown_group_"+userid);
    div_button.textContent = "子集合";
    div_btngroup.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    //div_ul.setAttribute("id","dropdown_menu_group_"+userid);
    div_btngroup.appendChild(div_ul);

    var subgroupList = subgroups.split(",");

    for (var i = 0; i < gl_userdatasubgroups.length; ++i)
    {
        var subgroup = gl_userdatasubgroups[i];
        var existed = check_numer_in_list(subgroup.subgroupid, subgroupList);
        div_ul.appendChild(create_dropdown_group_item(userid, "subgroup", i, subgroup.subgroupid, subgroup.subgroupname, existed));
    }

    return div_btngroup;
}

function create_dropdown_group_item(userid, type, index, datavalue, dataname, defaultchecked)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", datavalue);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_group_click(\""+type+"\",\""+userid+"\","+ index +");return true;");

    var item_checkbox = document.createElement("input");
    if(type === 'group')
    {
        item_checkbox.setAttribute("type","radio");
        item_checkbox.setAttribute("name","name_"+type+"_"+userid);
    }
    else
    {
        item_checkbox.setAttribute("type","checkbox");
    }
    item_checkbox.setAttribute("id","dropdown_checkbox_"+type+"_"+userid+"_"+index);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_group_click(\""+type+"\",\""+userid+"\","+ index +");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+dataname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_checkbox_group_click(type, userid, index)
{
    glDropdown_checkbox_group_click = true;

    update_selected_type(type, userid, index);
}

var glDropdown_checkbox_group_click = false;
function dropdown_group_click(type, userid, index)
{
    if (glDropdown_checkbox_group_click == true)
    {
        glDropdown_checkbox_group_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_"+type+"_"+userid+"_"+index).checked == true)
        document.getElementById("dropdown_checkbox_"+type+"_"+userid+"_"+index).checked = false;
    else
        document.getElementById("dropdown_checkbox_"+type+"_"+userid+"_"+index).checked = true;

    update_selected_type(type, userid, index);

    glDropdown_checkbox_group_click = false;
}

function update_selected_type(type, userid, uiindex)
{
    if ( type == "group" )
    {
        update_selected_group(userid, uiindex);
    }
    else
    if ( type == "datagroup" )
    {
        update_selected_datagroup(userid, uiindex);
    }
    else
    if ( type == "subgroup" )
    {
        update_selected_subgroup(userid, uiindex);
    }
}

function update_selected_group(userid, uiindex)
{
    console.log("update_selected_group() userid:"+userid+" uiindex:"+uiindex);

    var group_selected_div = document.getElementById("input_groupid_selected_"+userid);
    if (group_selected_div != undefined)
    {
        clearChildView("input_groupid_selected_"+userid);
    }
    else
    {
        console.log("update_selected_group() group_selected_div == undefined input_groupid_selected_"+userid);
    }

    var checked_count = 0;
    for (var index = 0; index < gl_usergroups.length; ++index)
    {
        var group = gl_usergroups[index];

        var checkbox = document.getElementById("dropdown_checkbox_group_"+userid+"_"+index);
        if (checkbox == undefined)
            continue;

        // if (uiindex == index)
        // {
            if (checkbox.checked)
            {
                checked_count++;

                var groupname_label = create_group_label(group.groupname);
                group_selected_div.appendChild(groupname_label);
            }
        // }
        // else
        // {
        //     checkbox.checked = false;
        // }
    }

    if ( checked_count == 0 )
    {
        // all uncheck , must at least one checked
        var checkbox = document.getElementById("dropdown_checkbox_group_"+userid+"_"+uiindex);
        if (checkbox != undefined)
        {
            checkbox.checked = true;

            var group = gl_usergroups[uiindex];
            var groupname_label = create_group_label(group.groupname);
            group_selected_div.appendChild(groupname_label);
        }
    }
}

function update_selected_datagroup(userid, uiindex)
{
    console.log("update_selected_datagroup() userid:"+userid+" uiindex:"+uiindex);

    var datagroups_selected_div = document.getElementById("input_datagroups_selected_"+userid);
    if (datagroups_selected_div != undefined)
    {
        clearChildView("input_datagroups_selected_"+userid);
    }

    for (var index = 0; index < gl_userdatasubgroups.length; ++index)
    {
        var subgroup = gl_userdatasubgroups[index];

        var checkbox = document.getElementById("dropdown_checkbox_datagroup_"+userid+"_"+(index+1));
        if (checkbox == undefined)
            continue;

        if (checkbox.checked)
        {
            var subgroupname_label = create_datagroup_label(subgroup.subgroupname);
            datagroups_selected_div.appendChild(subgroupname_label);
        }
    }
}

function update_selected_subgroup(userid, uiindex)
{
    console.log("update_selected_subgroup() userid:"+userid+" uiindex:"+uiindex);

    var subgroups_selected_div = document.getElementById("input_subgroups_selected_"+userid);
    if (subgroups_selected_div != undefined)
    {
        clearChildView("input_subgroups_selected_"+userid);
    }

    for (var index = 0; index < gl_userdatasubgroups.length; ++index)
    {
        var subgroup = gl_userdatasubgroups[index];

        var checkbox = document.getElementById("dropdown_checkbox_subgroup_"+userid+"_"+index);
        if (checkbox == undefined)
            continue;

        if (checkbox.checked)
        {
            var subgroupname_label = create_subgroup_label(subgroup.subgroupname);
            subgroups_selected_div.appendChild(subgroupname_label);
        }
    }
}

function getSelectedGender(userid)
{
    var gender = 0;

    if (document.getElementById("dropdown_checkbox_gender_"+userid+"_0").checked)
        gender = 0;
    if (document.getElementById("dropdown_checkbox_gender_"+userid+"_1").checked)
        gender = 1;
    if (document.getElementById("dropdown_checkbox_gender_"+userid+"_2").checked)
        gender = 2;

    return gender;
}

function getSelectedGroup(userid)
{
    var groupid = 0;
    for (var index = 0; index < gl_usergroups.length; ++index)
    {
        var group = gl_usergroups[index];
        var ui_item = document.getElementById("dropdown_checkbox_group_"+userid+"_"+index);
        if ( ui_item != undefined )
        {
            if (ui_item.checked)
            {
                groupid = parseInt(group.groupid);
                break;
            }
        }
    }
    return groupid;
}

function getSelectedDataGroup(userid)
{
    var datagroups = [];

    var ui_item = document.getElementById("dropdown_checkbox_datagroup_"+userid+"_0");
    if ( ui_item != undefined )
    {
        if (ui_item.checked)
        {
            // console.log("getSelectedDataGroup() userid:"+userid+" groupid:99999");
            datagroups.push(99999);
        }
    }

    // console.log("getSelectedDataGroup() userid:"+userid+" datagroups.length:"+datagroups.length);

    for (var index = 0; index < gl_userdatasubgroups.length; ++index)
    {
        var subgroup = gl_userdatasubgroups[index];

        if (document.getElementById("dropdown_checkbox_datagroup_"+userid+"_"+(index+1)).checked)
        {
            //groupid = parseInt(subgroup.subgroupid);
            // console.log("getSelectedDataGroup() userid:"+userid+" groupid:"+groupid);
            datagroups.push(subgroup.subgroupid);
        }
    }

    // console.log("getSelectedDataGroup() userid:"+userid+" datagroups.length:"+datagroups.length);

    return datagroups.toString();
}

function getSelectedSubGroup(userid)
{
    var subgroups = [];
    for (var index = 0; index < gl_userdatasubgroups.length; ++index)
    {
        var subgroup = gl_userdatasubgroups[index];

        if (document.getElementById("dropdown_checkbox_subgroup_"+userid+"_"+index).checked)
        {
            //groupid = parseInt(subgroup.subgroupid);
            subgroups.push(subgroup.subgroupid);
        }
    }
    return subgroups.toString();
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

function create_new_user()
{
    create_empty_record_table_item(gl_users.length + 1);

    return false;
}

// $("#createnewuserbutton").click(function()
// {
//     create_empty_record_table_item(gl_users.length + 1);
//
//     return false;
// });

function save_new_user()
{
    check_upload_image(uploadNewUserImageFinihed);
}

function check_upload_image(finished_callback)
{
    gl_uploadimage_uid = '';
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

function uploadNewUserImageFinihed(uploadimage_uid)
{
    gl_uploadimage_uid = uploadimage_uid;

    uploadImageFinihed(upload_new_user);
}

function uploadImageFinihed(callback)
{
    console.log("uploadImageFinihed() gl_uploadimage_uid:" + gl_uploadimage_uid);

    if (gl_uploadimage_uid == null)
    {
        alert("上傳照片失敗!");
        return;
    }

    var ui_input_name = document.getElementById("input_name");
    if (ui_input_name == undefined)
    {
        ui_input_name = document.getElementById("uiid_name_"+gl_edit_userid+"_edit");
    }
    var input_name = ui_input_name.value;;

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
    var new_file_description = input_name;
    var subgroups = "";

    var SendData = "project_id="+project_id;
    SendData += "&createnewimage=1";
    SendData += "&name="+new_file_name;
    SendData += "&description="+new_file_description;
    SendData += "&filename="+uploadimage_file.name;
    SendData += "&fileuid="+gl_uploadimage_uid;
    SendData += "&filetype="+uploadimage_file.type;
    SendData += "&filesize="+uploadimage_file.size;
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
            setTimeout(callback, 300);
        },
      error:
        function(xhr, status, error)
        {
            setTimeout(callback, 300);
        }
    });
}

function upload_new_user()
{
    var input_name = document.getElementById("input_name").value;
    var input_account = document.getElementById("input_account").value;
    //var input_unit = document.getElementById("input_unit").value;
    var input_title = document.getElementById("input_title").value;
    //var input_gender = document.getElementById("input_gender").value;
    //var input_group = document.getElementById("input_group").value;
    //var input_gender = getSelectedGender('99999');
    var input_group = getSelectedGroup('99999');
    var input_datagroups = getSelectedDataGroup('99999');
    var input_subgroups = getSelectedSubGroup('99999');
    var input_rfidcards = document.getElementById("input_rfidcards").value;
    var input_nfccards = document.getElementById("input_nfccards").value;
    var input_uwbcards = document.getElementById("input_uwbcards").value;
    var input_sensors = document.getElementById("input_sensors").value;

    if ( input_name.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    if ( input_account.length == 0 )
    {
        alert("請輸入帳號");
        return;
    }

    if ( gl_groupid != 1 &&  gl_groupid != 2)
    {
        if ( input_subgroups.length == 0 )
        {
            alert("請選擇子集合");
            return;
        }
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['create_new_user'] = 1;
    jsondata['name'] = input_name;
    jsondata['account'] = input_account;
    //jsondata['unit'] = input_unit;
    jsondata['title'] = input_title;
    //jsondata['gender'] = input_gender;
    jsondata['group'] = input_group;
    jsondata['datagroups'] = input_datagroups;
    jsondata['subgroups'] = input_subgroups;
    jsondata['rfidcards'] = input_rfidcards;
    jsondata['nfccards'] = input_nfccards;
    jsondata['uwbcards'] = input_uwbcards;
    jsondata['sensors'] = input_sensors;
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

            var objJSON = JSON.parse(response);

            if ( objJSON.existed == true)
            {
                alert("此用戶已存在 : " + input_account +' 登入請用識別碼:' + objJSON.userid);
            }

            if ( objJSON.assigned_rfidcards.length > 0 )
            {
                alert("RFID 卡片已有用戶使用 : " + objJSON.assigned_rfidcards);
            }
            if ( objJSON.assigned_nfccards.length > 0 )
            {
                alert("NFC 卡片已有用戶使用 : " + objJSON.assigned_nfccards);
            }

            setTimeout(reload_user_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function update_user(userid, value_name, value_title, value_group, value_datagroups, value_subgroups, value_rfidcards, value_nfccards, value_uwbcards, value_sensors, imageuid)
{
    // console.log("update_user() userid:"+userid);

    if ( value_name.length == 0 )
    {
        alert("請輸入名稱");
        return;
    }

    var user_edit = getUser(userid);

    if ((gl_groupid != 1 &&  gl_groupid != 2) && (gl_accountid != user_edit.account))
    {
        if ( value_subgroups.length == 0 )
        {
            alert("請選擇子集合");
            return;
        }
    }

    // if ( value_account.length == 0 )
    // {
    //     alert("請輸入帳號");
    //     return;
    // }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_user'] = 1;
    jsondata['userid'] = userid;
    jsondata['name'] = value_name;
    //jsondata['account'] = value_account;
    //jsondata['unit'] = value_unit;
    jsondata['title'] = value_title;
    //jsondata['gender'] = value_gender;
    jsondata['group'] = value_group;
    jsondata['datagroups'] = value_datagroups;
    jsondata['subgroups'] = value_subgroups;
    jsondata['rfidcards'] = value_rfidcards;
    jsondata['nfccards'] = value_nfccards;
    jsondata['uwbcards'] = value_uwbcards;
    jsondata['sensors'] = value_sensors;
    jsondata['imageuid'] = imageuid;

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

            var objJSON = JSON.parse(response);

            if ( objJSON.assigned_rfidcards.length > 0 )
            {
                alert("RFID 卡片已有用戶使用 : " + objJSON.assigned_rfidcards);
            }
            if ( objJSON.assigned_nfccards.length > 0 )
            {
                alert("NFC 卡片已有用戶使用 : " + objJSON.assigned_nfccards);
            }
            setTimeout(reload_user_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_reset_user_topic(userid, username, account)
{
    if (confirm('Are you sure you want to reset this user pushnotify topic:'+username+' ?'))
    {
        // reset it!
        reset_user_topic(userid, account);
    }
    else
    {
        // Do nothing!
    }
}

function reset_user_topic(userid, account)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['reset_user_topic'] = 1;
    jsondata['userid'] = parseInt(userid);
    jsondata['account'] = account;

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

        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_reset_user_password(userid, username, account)
{
    if (confirm('Are you sure you want to reset this user password:'+username+' ?'))
    {
        var newValue= prompt("Please enter new password:[空白代表系統將自動產生]");
        // if ( newValue )
        // {
        //     //console.log("confirm_reset_user_password() newValue:"+newValue);
        // }
        // else
        // {
        //     //console.log("confirm_reset_user_password() no newValue");
        // }

        // reset it!
        reset_user_password(userid, account, newValue);
    }
    else
    {
        // Do nothing!
    }
}

function reset_user_password(userid, account, newValue)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['reset_user_password'] = 1;
    jsondata['userid'] = parseInt(userid);
    jsondata['account'] = account;
    jsondata['password'] = newValue;

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

        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_delete_user(userid, username, account)
{
    if (confirm('Are you sure you want to delete this user:'+username+' ?'))
    {
        // delete it!
        delete_user(userid, account);
    }
    else
    {
        // Do nothing!
    }
}

function delete_user(userid, account)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['delete_user'] = 1;
    jsondata['userid'] = parseInt(userid);
    jsondata['account'] = account;

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
            setTimeout(reload_user_info, 300);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function getUser(userid)
{
    var retuser = null;
    for (var i = 0; i < gl_users.length; ++i)
    {
        var user = gl_users[i];

        if ( user.userid === userid )
        {
            retuser = user;
            break;
        }

    }
    return retuser;
}

function getGroupName(groupid)
{
    var name = "未知";
    for (var i = 0; i < gl_usergroups.length; ++i)
    {
        var group = gl_usergroups[i];

        if ( parseInt(group.groupid) == groupid )
        {
            name = group.groupname;
            break;
        }

    }
    return name;
}

function getSubGroupName(subgroupid)
{
    //console.log("getSubGroupName() subgroupid:"+subgroupid);

    if ( parseInt(subgroupid) == 99999 )
    {
        return '全部';
    }

    var name = "";
    for (var i = 0; i < gl_userdatasubgroups.length; ++i)
    {
        var group = gl_userdatasubgroups[i];

        //console.log("getSubGroupName() group.subgroupid:"+group.subgroupid);
        if ( group.subgroupid == parseInt(subgroupid) )
        {
            name = group.subgroupname;
            break;
        }
    }
    return name;
}

function getGenderName(gender)
{
    var name = "男";
    switch(gender)
    {
        default:
        case "0":
            name = "未知";
        break;
        case "1":
            name = "男";
        break;
        case "2":
            name = "女";
        break;
    }
    return name;
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

function reload_user_info()
{
    PreLoadIndex = 1;
    CurrentLoadIndex = 2;
    check_loading_status();
}

function reload_all_info()
{
    PreLoadIndex = -1;
    CurrentLoadIndex = 0;
    check_loading_status();
}

function device_qrcode_show(type, accountname, accountid, userid)
{
    console.log('device_qrcode_show() type:'+type+" accountname:"+accountname+" accountid:"+accountid);

    var getType = "TAG";
    if (type === '1')
        getType = "TAG";
    if (type === '2')
        getType = "USER";
    if (type === '3')
        getType = "LOCATOR";

    var device_qrcode_type = document.getElementById("device_qrcode_type");
    device_qrcode_type.textContent = 'Type: ' + getType;
    var device_qrcode_name = document.getElementById("device_qrcode_name");
    device_qrcode_name.textContent = '名稱: ' + accountname;
    var device_qrcode_macaddress = document.getElementById("device_qrcode_macaddress");
    device_qrcode_macaddress.textContent = '帳號: ' + accountid;

    var SendDate = "loadqrcode=1&type="+getType+"&accountid=" + accountid+"&id=" + userid+'&project_id='+project_id;
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

function read_xlsx_input()
{
    $('#file-input').trigger('click');
}

function inputfile_onchange()
{
    var file_input = document.getElementById("file-input");
    // getting a hold of the file reference
    var file = file_input.files[0];

    console.log("inputfile_onchange() file.name:"+file.name+" file.size:"+file.size+" file.type:"+file.type);

    let formData = new FormData();
    formData.append("uploadxlsxfile", file);
    formData.append('project_id', ''+project_id);
    formData.append('fileinfo', 'userinfo');

    var targetURL = gl_target_server + "/php/vilsnodes.php";

    jQuery.ajax({
        url: targetURL,
        type: 'POST',
        processData: false,
        contentType: false,
        data: formData,
        success:
        function(response)
        {
            var result = JSON.parse(response);

            console.log("inputfile_onchange() success:"+result.success+" msg:"+result.msg+" issuefile:"+result.issuefile);

            setTimeout(reload_all_info, 100);

            if (result.issuefile != undefined && result.issuefile.length > 0)
            {
                alert("部分資料有誤!請查看檔案!");

                var xlsxreport_status = document.getElementById("xlsxreport_status");
                while (xlsxreport_status.firstChild)
                {
                    xlsxreport_status.removeChild(xlsxreport_status.firstChild);
                }

                var daily_xlsx_report_dl_btn = document.createElement("button");
                daily_xlsx_report_dl_btn.setAttribute("type", 'button');
                daily_xlsx_report_dl_btn.setAttribute("class", 'btn btn-app');
                daily_xlsx_report_dl_btn.setAttribute("style", "width:90px;height:100px");
                daily_xlsx_report_dl_btn.setAttribute("onclick", "open_new_page('" +result.issuefile +"');");
                xlsxreport_status.appendChild(daily_xlsx_report_dl_btn);

                var imgPdf = document.createElement("img");
                imgPdf.setAttribute("src", "/images/xlsx-dl-icon-1.png");
                imgPdf.setAttribute("alt", "Download report");
                imgPdf.setAttribute("title", "Download report");
                imgPdf.setAttribute("height", "80");
                imgPdf.setAttribute("width", "80");
                daily_xlsx_report_dl_btn.appendChild(imgPdf);
            }
            else
                alert(result.msg);

        },
        error:
        function(xhr, status, error)
        {

        }
    });

}

function create_user_xlsx_report()
{
    var xlsxreport_status = document.getElementById("xlsxreport_status");
    xlsxreport_status.textContent = "產生報表中...";

    var SendDate = "loaduserinfoxlsxreport=1&project_id="+project_id;

    var targetURL = gl_target_server + "/php/vilsnodes.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            var result = JSON.parse(response);
            console.log("create_user_xlsx_report() success:"+result.success+" xlsxreport:"+result.xlsxreport);

            var xlsxreport_status = document.getElementById("xlsxreport_status");
            while (xlsxreport_status.firstChild)
            {
                xlsxreport_status.removeChild(xlsxreport_status.firstChild);
            }

            if ( result.success === 'true')
            {
                var daily_xlsx_report_dl_btn = document.createElement("button");
                daily_xlsx_report_dl_btn.setAttribute("type", 'button');
                daily_xlsx_report_dl_btn.setAttribute("class", 'btn btn-app');
                daily_xlsx_report_dl_btn.setAttribute("style", "width:90px;height:100px");
                daily_xlsx_report_dl_btn.setAttribute("onclick", "open_new_page('" +result.xlsxreport +"');");
                xlsxreport_status.appendChild(daily_xlsx_report_dl_btn);

                var imgPdf = document.createElement("img");
                imgPdf.setAttribute("src", "/images/xlsx-dl-icon-1.png");
                imgPdf.setAttribute("alt", "Download report");
                imgPdf.setAttribute("title", "Download report");
                imgPdf.setAttribute("height", "80");
                imgPdf.setAttribute("width", "80");
                daily_xlsx_report_dl_btn.appendChild(imgPdf);
            }
            else
            {
                var xlsxreport_status = document.getElementById("xlsxreport_status");
                xlsxreport_status.textContent = "產生報表失敗!";
            }
        }
    });
    return false;
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

        reload_user_info();
    }
}

function move_page_forward()
{
    var current_page_num_ui = document.getElementById('id_current_page_num');

    glCurrentPage++;
    current_page_num_ui.textContent = glCurrentPage;

    reload_user_info();

}

var gl_search_start = 0;
function map_search_text_change()
{
    var input_value = document.getElementById("map_search_text").value;

    if ( input_value === pre_input_value )
    {
        console.log("check_search_text_change() input_value === pre_input_value, input_value:"+input_value);
        return;
    }

    glCurrentPage = 1;
    gl_search_start = 0;

    reload_user_info();

    return false;
}

function check_search_text_change(e)
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
        console.log("check_search_text_change() CurrentLoadIndex != LOAD_STATE_END");

        setTimeout(map_search_text_change, 500);
        return;
    }

    if ( seconds >= 1 )
    {
        gl_search_start = 0;
        map_search_text_change();
    }
    else
    {
        setTimeout(map_search_text_change, 500);
    }
}


const $source = document.querySelector('#map_search_text');
$source.addEventListener('input', check_search_text_change);// register for oninput

function open_new_page(openurl)
{
    var win = window.open(openurl, '_blank');
    win.focus();
}

function open_userinfo_page()
{
    var win = window.open('/index.php/vilsuserinfo/'+project_id, '_blank');
    win.focus();
}
