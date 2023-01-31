/*
 * Author: ikki Chung
 * Date: 2020.07.07
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_PAGE_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;
var glCurrentPage = 1;

var glReloadCount = 0;

var gl_user_info = [];
var gl_usersubgroups = [];
var gl_rfiddevices = [];
var gl_nfclocators = [];
var gl_cardrollcall = {};
var gl_cardrollcallevent = {};
var gl_cardrollcallrecord = [];
var gl_userrollcallrecord = [];

var gl_all_subgroup_totalcount = 0;
var gl_all_subgroup_presentcount = 0;
var gl_all_subgroup_absentcount = 0;

var LOAD_USER_NUM = 500;
var gl_user_load_index = 0;

var gl_enable_batch_present = false;
var gl_batch_present_user = [];

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

                $('#modal-loading').modal('show');

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

                load_rfiddevice();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_nfclocator();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cardrollcall(rollcallid);
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cardrollcallevent(eventid);
            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                draw_cardrollcallevent_table();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 7:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_cardrollcallrecord(rollcallid, eventid);
            }
        break;
        case 8:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                // draw_userrollcallrecord_table();
                draw_user_subgroup_table();

                $('#modal-loading').modal('hide');
                CurrentLoadIndex = LOAD_STATE_END;
            }
        break;
    }

    if ( CurrentLoadIndex != LOAD_STATE_END )
    {
        setTimeout(check_loading_status, 100);
    }
    else
    {
        if (gl_enable_batch_present)
        {
            glReloadCount = 0;
        }
        else
        {
            if (glReloadCount > 0)
            {
                glReloadCount = glReloadCount - 1;
                setTimeout(reload_rollcall_info, 5000);
            }
            else
            {
                setTimeout(reload_rollcall_info, 30000);
            }
        }

        console.log("check_loading_status() Stop.");
    }
}

function reload_rollcall_info()
{
    if (gl_enable_batch_present)
    {
        return;
    }

    PreLoadIndex = 4;
    CurrentLoadIndex = 5;
    check_loading_status();
}

function reload_rollcall_ui()
{
    PreLoadIndex = 7;
    CurrentLoadIndex = 8;
    check_loading_status();
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

function load_usersubgroup()
{
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
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_rfiddevice()
{
    gl_rfiddevices = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loaddevice=1&project_id='+project_id, function(data)
    {
        var rfiddevice_num = 0;
        $.each(data, function(index, device)
        {
            gl_rfiddevices.push(device);
            rfiddevice_num++;
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

function load_nfclocator()
{
    gl_nfclocators = [];

    var targetURL = gl_target_server + "/php/nfclocator.php";
    var p = $.getJSON(targetURL, 'loadnfclocator=1&project_id='+project_id, function(data)
    {
        var rfiddevice_num = 0;
        $.each(data, function(index, device)
        {
            gl_nfclocators.push(device);
            rfiddevice_num++;
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

function load_cardrollcall(rollcallid)
{
    gl_cardrollcall = {};

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcardrollcall=1&project_id='+project_id+'&rollcallid='+rollcallid, function(data)
    {
        gl_cardrollcall = data;

    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_cardrollcallevent(eventid)
{
    gl_cardrollcallevent = {};

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcardrollcallevent=1&project_id='+project_id+'&eventid='+eventid, function(data)
    {
        gl_cardrollcallevent = data;
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_cardrollcallrecord(rollcallid, eventid)
{
    gl_cardrollcallrecord = [];
    gl_userrollcallrecord = [];

    var targetURL = gl_target_server + "/php/rfiddevice.php";
    var p = $.getJSON(targetURL, 'loadcardrollcallrecord=1&&project_id='+project_id+'&rollcallid='+rollcallid+'&eventid='+eventid, function(data)
    {
        $.each(data, function(index, record)
        {
            gl_cardrollcallrecord.push(record);

            put_to_user_rollcall_record(record);

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

function put_to_user_rollcall_record(record)
{
    var curRecord = [];
    var bFound = false;
    for (var i = 0; i < gl_userrollcallrecord.length; ++i)
    {
        var existed_record = gl_userrollcallrecord[i];
        if (existed_record.rollcalleventid == record.rollcalleventid)
        {
            curRecord = existed_record;
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        curRecord = [];
        curRecord.rollcalleventid = record.rollcalleventid;
        curRecord.rollcallid = record.rollcallid;
        curRecord.userrecord = [];
        gl_userrollcallrecord.push(curRecord);
    }

    var curUserRecord = [];
    bFound = false;
    for (var i = 0; i < curRecord.userrecord.length; ++i)
    {
        var user_record = curRecord.userrecord[i];
        if ( user_record.userid == record.userid )
        {
            curUserRecord = user_record;
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        curUserRecord = [];
        curUserRecord.userid = record.userid;
        curUserRecord.card = [];
        curUserRecord.present = 0;
        curRecord.userrecord.push(curUserRecord);
    }

    curUserRecord.card.push(record);

    if (record.present == '1')
        curUserRecord.present = 1;

    // console.log("put_to_user_rollcall_record() record.userid:"+record.userid +" card.length:"+curUserRecord.card.length);
}

function draw_cardrollcallevent_table()
{
    var record_table = document.getElementById('USER_ROLLCALLRESULT_TABLE');

    while (record_table.rows.length >= 1)
        record_table.deleteRow(0);

    // console.log("draw_table() gl_cardrollcallevent.rollcallname:"+gl_cardrollcallevent.rollcallname);

    var row = create_event_table_item(0, gl_cardrollcallevent);
    if (row != null)
    {
        record_table.appendChild(row);
    }
}

function create_event_table_item(ui_index, record)
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
    tdName.setAttribute("id","uiid_name_"+record.eventid);
    row.appendChild(tdName);
    var SpanName = document.createElement("span");
    SpanName.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanName.textContent = record.rollcallname;
    tdName.appendChild(SpanName);
    cellIndex++;

    // startdatetime
    var tdcardid = document.createElement('td');
    tdcardid.setAttribute("id","uiid_cardid_"+record.eventid);
    row.appendChild(tdcardid);
    // var SpanStartDateTime  = document.createElement("span");
    // SpanStartDateTime.setAttribute("class","label label-primary");
    // SpanStartDateTime.textContent = record.StartDateTime;
    // row.cells[cellIndex].appendChild(SpanStartDateTime);
    var badge_StartDate = document.createElement("span");
    badge_StartDate.setAttribute("class","label label-info");
    badge_StartDate.textContent = convertPrettyDateOnly(record.StartDateTime);
    row.cells[cellIndex].appendChild(badge_StartDate);

    var badge_StartTime = document.createElement("span");
    badge_StartTime.setAttribute("class","label label-warning");
    badge_StartTime.textContent = convertPrettyTimeOnly(record.StartDateTime);
    row.cells[cellIndex].appendChild(badge_StartTime);
    cellIndex++;

    // EndDateTime
    var tduserid = document.createElement('td');
    tduserid.setAttribute("id","uiid_userid_"+record.recordid);
    row.appendChild(tduserid);
    // var SpanEndDateTime  = document.createElement("span");
    // SpanEndDateTime.setAttribute("class","label label-primary");
    // SpanEndDateTime.textContent = record.EndDateTime;
    // row.cells[cellIndex].appendChild(SpanEndDateTime);
    var badge_EndDate = document.createElement("span");
    badge_EndDate.setAttribute("class","label label-info");
    badge_EndDate.textContent = convertPrettyDateOnly(record.EndDateTime);
    row.cells[cellIndex].appendChild(badge_EndDate);

    var badge_EndTime = document.createElement("span");
    badge_EndTime.setAttribute("class","label label-warning");
    badge_EndTime.textContent = convertPrettyTimeOnly(record.EndDateTime);
    row.cells[cellIndex].appendChild(badge_EndTime);
    cellIndex++;

    // totalcount
    row.appendChild(document.createElement('td'));
    var Spantotalcount  = document.createElement("span");
    Spantotalcount.setAttribute("id","uiid_total_"+record.recordid);
    Spantotalcount.setAttribute("class","label label-primary");
    // Spantotalcount.textContent = record.totalcount;
    Spantotalcount.textContent = gl_all_subgroup_totalcount;
    row.cells[cellIndex].appendChild(Spantotalcount);
    cellIndex++;

    var gl_all_subgroup_absentcount = 0;
    // presentcount
    row.appendChild(document.createElement('td'));
    var Spanppresentcount  = document.createElement("span");
    Spanppresentcount.setAttribute("id","uiid_present_"+record.recordid);
    Spanppresentcount.textContent = gl_all_subgroup_presentcount;
    if (gl_all_subgroup_presentcount == 0)
    {
        Spanppresentcount.setAttribute("class","label label-danger");
        //Spanppresentcount.textContent = record.presentcount;
    }
    else
    {
        Spanppresentcount.setAttribute("class","label label-primary");
        //Spanppresentcount.textContent = record.presentcount;
    }
    row.cells[cellIndex].appendChild(Spanppresentcount);
    cellIndex++;

    var absentCount = record.totalcount - record.presentcount;
    row.appendChild(document.createElement('td'));
    var Spanppresentcount  = document.createElement("span");
    Spanppresentcount.setAttribute("id","uiid_absent_"+record.recordid);
    Spanppresentcount.textContent = gl_all_subgroup_absentcount;
    if (absentCount == 0)
    {
        Spanppresentcount.setAttribute("class","label label-primary");
        //Spanppresentcount.textContent = absentCount;
    }
    else
    {
        Spanppresentcount.setAttribute("class","label label-danger");
        //Spanppresentcount.textContent = absentCount;
    }
    row.cells[cellIndex].appendChild(Spanppresentcount);
    cellIndex++;

    return row;
}

function draw_user_subgroup_table()
{
    if ( document.getElementById("USER_ROLLCALLRECORD_TABLE_TABS") == null )
        return;

    var rollcallrecord_table_tabs = document.getElementById('USER_ROLLCALLRECORD_TABLE_TABS');
    var rollcallrecord_table_content = document.getElementById('USER_ROLLCALLRECORD_TABLE_CONTENT');

    // console.log("draw_user_subgroup_table() subgroups:"+gl_cardrollcall.subgroups);

    if ( gl_cardrollcall.subgroups == undefined )
        return;

    var subgroups_orig = gl_cardrollcall.subgroups.split(",");
    var  subgroups = [];
    $.each(subgroups_orig, function(i, el){
        if($.inArray(el,  subgroups) === -1)  subgroups.push(el);
    });

    gl_all_subgroup_totalcount = 0;
    gl_all_subgroup_presentcount = 0;
    gl_all_subgroup_absentcount = 0;

    //
    // create nav-tabs
    //

    // 點名未到頁面
    var ui_index = 0;
    create_rollcallrecord_tab(ui_index, 1, rollcallrecord_table_tabs, 99999, "點名未到");
    create_rollcallrecord_tab_pane_not_preset(ui_index, rollcallrecord_table_content, 99999);

    ui_index++;

    // console.log("draw_user_subgroup_table() subgroups.length:"+subgroups.length);

    for (var i = 0; i < subgroups.length; i++)
    {
        var subgroupid = parseInt(subgroups[i]);

        // console.log("draw_user_subgroup_table() gl_usersubgroups.length:"+gl_usersubgroups.length +" subgroupid:"+subgroupid);

        for (var j = 0; j < gl_usersubgroups.length; j++)
        {
            var usersubgroup = gl_usersubgroups[j];

            if ( subgroupid == usersubgroup.subgroupid )
            {
                // console.log("draw_user_subgroup_table() create_rollcallrecord_tab ui_index:"+ ui_index +" subgroupid:"+subgroupid + " usersubgroupid:"+usersubgroupid);

                create_rollcallrecord_tab(ui_index, 0, rollcallrecord_table_tabs, usersubgroup.subgroupid, usersubgroup.subgroupname);

                create_not_preset_item(ui_index, usersubgroup.subgroupid, usersubgroup.subgroupname);

                //
                // create tab-pane
                //
                create_rollcallrecord_tab_pane(ui_index, rollcallrecord_table_content, usersubgroup.subgroupid);

                ui_index++;
            }
        }
    }

    var ui_total = document.getElementById("uiid_total_"+gl_cardrollcallevent.recordid);
    var ui_present = document.getElementById("uiid_present_"+gl_cardrollcallevent.recordid);
    var ui_absent = document.getElementById("uiid_absent_"+gl_cardrollcallevent.recordid);

    ui_total.textContent = gl_all_subgroup_totalcount;
    ui_present.textContent = gl_all_subgroup_presentcount;
    ui_absent.textContent = gl_all_subgroup_absentcount;

    // console.log("draw_user_subgroup_table() gl_all_subgroup_presentcount:"+ gl_all_subgroup_presentcount +" gl_all_subgroup_absentcount:"+gl_all_subgroup_absentcount + " gl_all_subgroup_totalcount:"+gl_all_subgroup_totalcount);

}

function create_rollcallrecord_tab(ui_index, type, rollcallrecord_table_tabs, subgroupid, subgroupname)
{
    var tab_tag = document.getElementById('tab_' + subgroupid);
    if (tab_tag == null)
    {
        var tab_li = document.createElement('li');
        if (ui_index == 0)
            tab_li.setAttribute("class","active");
        tab_li.setAttribute("id",'tab_' + subgroupid);
        rollcallrecord_table_tabs.appendChild(tab_li);

        var tab_li_a = document.createElement('a');
        tab_li_a.setAttribute("href",'#tab_subgroup_' + subgroupid);
        tab_li_a.setAttribute("data-toggle","tab");
        tab_li.appendChild(tab_li_a);

        // tag name
        var tab_li_a_span = document.createElement('span');
        if (type == 1)
            tab_li_a_span.setAttribute("class","badge bg-red-active");
        else
            tab_li_a_span.setAttribute("class","badge bg-blue-active");
        tab_li_a_span.textContent = subgroupname;
        tab_li_a.appendChild(tab_li_a_span);
    }
    return tab_tag;
}

function create_rollcallrecord_tab_pane(ui_index, rollcallrecord_table_content, subgroupid)
{
    var tab_subgroup_pan = document.getElementById('tab_subgroup_' + subgroupid);
    if (tab_subgroup_pan == null)
    {
        var tab_subgroup_pan = document.createElement('div');
        if (ui_index == 0)
            tab_subgroup_pan.setAttribute("class",'tab-pane active table-responsive');
        else
            tab_subgroup_pan.setAttribute("class",'tab-pane table-responsive');
        tab_subgroup_pan.setAttribute("id",'tab_subgroup_' + subgroupid);

        rollcallrecord_table_content.appendChild(tab_subgroup_pan);
    }

    var tab_subgroup_table = document.getElementById('tab_subgroup_table_' + subgroupid);
    if (tab_subgroup_table == null)
    {
        // console.log("create_rollcallrecord_tab_pane() ui_index:"+ui_index+" subgroupid:"+subgroupid);

        var tab_subgroup_table = document.createElement('table');
        tab_subgroup_table.setAttribute("id",'tab_subgroup_table_' + subgroupid);
        tab_subgroup_table.setAttribute("class",'table table-bordered');

        var table_thead = document.createElement('thead');
        tab_subgroup_table.appendChild(table_thead);

        var table_thead_tr = document.createElement('tr');
        table_thead_tr.setAttribute("style",'background-color:#cce6ff;');
        table_thead.appendChild(table_thead_tr);

        var table_thead_tr_th_1 = create_th('#');
        table_thead_tr_th_1.setAttribute("style",'width: 10px;');
        table_thead_tr.appendChild(table_thead_tr_th_1);
        table_thead_tr.appendChild(create_th('使用者'));
        table_thead_tr.appendChild(create_th('卡片ID'));
        table_thead_tr.appendChild(create_th('點名裝置'));
        table_thead_tr.appendChild(create_th('點名結果'));
        table_thead_tr.appendChild(create_th('點名時間'));
        table_thead_tr.appendChild(create_th('操作'));

        var table_tbody = document.createElement('tbody');
        table_tbody.setAttribute("id",'tab_subgroup_table_content' + subgroupid);
        tab_subgroup_table.appendChild(table_tbody);

        tab_subgroup_pan.appendChild(tab_subgroup_table);
    }

    var tab_subgroup_table_content = document.getElementById('tab_subgroup_table_content' + subgroupid);
    if (tab_subgroup_table_content == null)
    {
        return;
    }
    else
    {
        clearChildView('tab_subgroup_table_content' + subgroupid);
    }

    var subgroup_presentcount = 0;
    var subgroup_absentcount = 0;
    var subgroup_totalcount = 0;

    clearChildView('not_preset_item_user_' + subgroupid);
    var ui_not_preset_item_user = document.getElementById("not_preset_item_user_"+subgroupid);

    var user_index = 0;
    for (var i = 0; i < gl_userrollcallrecord.length; ++i)
    {
        var record = gl_userrollcallrecord[i];

        // console.log("create_rollcallrecord_tab_pane() rollcalleventid:"+record.rollcalleventid+" rollcallid:"+ record.rollcallid +" userrecord.length:"+record.userrecord.length);

        for (var j = 0; j < record.userrecord.length; ++j)
        {
            var user_record = record.userrecord[j];

            var valid = check_user_subgroup(user_record.userid, subgroupid);

            if ( valid )
            {
                // console.log("create_rollcallrecord_tab_pane() user_record.userid:"+user_record.userid+" valid:"+valid +" user_record.present:"+user_record.present);

                var row = create_user_record_table_item(user_index, record, user_record);
                tab_subgroup_table_content.appendChild(row);
                user_index++;

                if (user_record.present == 1)
                {
                    gl_all_subgroup_presentcount += 1;
                    subgroup_presentcount += 1;
                }
                else
                {
                    gl_all_subgroup_absentcount += 1;
                    subgroup_absentcount += 1;

                    var cardid = '';
                    if (user_record.card.length > 0)
                    {
                        var user_card = user_record.card[0];
                        cardid = user_card.cardid;
                    }

                    var divuser = create_not_present_user(user_record.userid, cardid);
                    ui_not_preset_item_user.appendChild(divuser);

                    // space
                    var Spanspace = document.createElement("span");
                    Spanspace.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;");
                    Spanspace.textContent = "  ";
                    ui_not_preset_item_user.appendChild(Spanspace);

                    if ( (subgroup_absentcount > 0 ) && (subgroup_absentcount % 5) == 0)
                    {
                        var divspace = document.createElement("div");
                        ui_not_preset_item_user.appendChild(divspace);
                    }

                }

                gl_all_subgroup_totalcount++;
                subgroup_totalcount++;
            }
        }


        // if ( subgroup_absentcount != subgroup_totalcount )
        // {
        //     console.log("create_rollcallrecord_tab_pane() gl_all_subgroup_presentcount:"+gl_all_subgroup_presentcount+" gl_all_subgroup_absentcount:"+gl_all_subgroup_absentcount +" gl_all_subgroup_totalcount:"+gl_all_subgroup_totalcount);
        //     console.log("create_rollcallrecord_tab_pane() subgroup_presentcount:"+subgroup_presentcount+" subgroup_absentcount:"+subgroup_absentcount +" subgroup_totalcount:"+subgroup_totalcount+" ->"+ (gl_all_subgroup_totalcount - subgroup_totalcount));
        // }


    }

    //
    // 在 點名未到 頁面新增 手動點名 按鈕
    //
    if (ui_index == 1)
    {
        var intime = check_rollcall_intime();
        if (intime)
        {

            var td_action = document.getElementById("not_preset_item_action_"+subgroupid);
            var checkBtn = document.createElement("button");
            checkBtn.setAttribute("title","批次設定點名已到");
            checkBtn.setAttribute("href","javascript:;");
            checkBtn.setAttribute("onclick","enable_batch_present();return false;");
            checkBtn.setAttribute("id","enable_batch_present_btn");
            var imgCheck = document.createElement("i");
            imgCheck.setAttribute("class","fa fa-user-plus");
            imgCheck.setAttribute("id","enable_batch_present_icon");
            checkBtn.appendChild(imgCheck);
            td_action.appendChild(checkBtn);

            if(gl_enable_batch_present)
            {
                checkBtn.setAttribute("title","設定點名已到");
                checkBtn.setAttribute("onclick","save_batch_present();return false;");
                imgCheck.setAttribute("class","fa fa-save");
            }
        }
    }

    var Spantotalcount = document.getElementById("not_preset_item_total_"+subgroupid);
    var Spanppresentcount = document.getElementById("not_preset_item_present_"+subgroupid);
    var Spanabsentcount = document.getElementById("not_preset_item_absent_"+subgroupid);

    if (Spantotalcount != undefined)
    {
        Spantotalcount.textContent = subgroup_totalcount;
    }
    if (Spanppresentcount != undefined)
    {
        if (subgroup_presentcount == 0)
        {
            Spanppresentcount.setAttribute("class","label label-danger");
            Spanppresentcount.textContent = subgroup_presentcount;
        }
        else
        {
            Spanppresentcount.setAttribute("class","label label-primary");
            Spanppresentcount.textContent = subgroup_presentcount;
        }
    }
    if (Spanabsentcount != undefined)
    {
        if (subgroup_absentcount == 0)
        {
            Spanabsentcount.setAttribute("class","label label-primary");
            Spanabsentcount.textContent = subgroup_absentcount;
        }
        else
        {
            Spanabsentcount.setAttribute("class","label label-danger");
            Spanabsentcount.textContent = subgroup_absentcount;
        }
    }

}

function create_rollcallrecord_tab_pane_not_preset(ui_index, rollcallrecord_table_content, subgroupid)
{
    var tab_subgroup_pan = document.getElementById('tab_subgroup_' + subgroupid);
    if (tab_subgroup_pan == null)
    {
        var tab_subgroup_pan = document.createElement('div');
        if (ui_index == 0)
            tab_subgroup_pan.setAttribute("class",'tab-pane active table-responsive');
        else
            tab_subgroup_pan.setAttribute("class",'tab-pane table-responsive');
        tab_subgroup_pan.setAttribute("id",'tab_subgroup_' + subgroupid);

        rollcallrecord_table_content.appendChild(tab_subgroup_pan);
    }

    var tab_subgroup_table = document.getElementById('tab_subgroup_table_' + subgroupid);
    if (tab_subgroup_table == null)
    {
        // console.log("create_rollcallrecord_tab_pane() ui_index:"+ui_index+" subgroupid:"+subgroupid);

        var tab_subgroup_table = document.createElement('table');
        tab_subgroup_table.setAttribute("id",'tab_subgroup_table_' + subgroupid);
        tab_subgroup_table.setAttribute("class",'table table-bordered');

        var table_thead = document.createElement('thead');
        tab_subgroup_table.appendChild(table_thead);

        var table_thead_tr = document.createElement('tr');
        table_thead_tr.setAttribute("style",'background-color:#cce6ff;');
        table_thead.appendChild(table_thead_tr);

        var table_thead_tr_th_1 = create_th('#');
        table_thead_tr_th_1.setAttribute("style",'width: 10px;');
        table_thead_tr.appendChild(table_thead_tr_th_1);

        var table_thead_tr_subgroup = create_th('子集合');
        table_thead_tr_subgroup.setAttribute("style",'width: 150px;');
        table_thead_tr.appendChild(table_thead_tr_subgroup);

        var table_thead_tr_total = create_th('應到總數');
        table_thead_tr_total.setAttribute("style",'width: 150px;');
        table_thead_tr.appendChild(table_thead_tr_total);

        var table_thead_tr_present = create_th('實到總數');
        table_thead_tr_present.setAttribute("style",'width: 150px;');
        table_thead_tr.appendChild(table_thead_tr_present);

        var table_thead_tr_absent = create_th('未到總數');
        table_thead_tr_absent.setAttribute("style",'width: 150px;');
        table_thead_tr.appendChild(table_thead_tr_absent);

        table_thead_tr.appendChild(create_th('未到人員'));

        table_thead_tr.appendChild(create_th('操作'));

        var table_tbody = document.createElement('tbody');
        table_tbody.setAttribute("id",'tab_subgroup_table_content' + subgroupid);
        tab_subgroup_table.appendChild(table_tbody);

        tab_subgroup_pan.appendChild(tab_subgroup_table);
    }

    var tab_subgroup_table_content = document.getElementById('tab_subgroup_table_content' + subgroupid);
    if (tab_subgroup_table_content == null)
    {
        return;
    }
    else
    {
        clearChildView('tab_subgroup_table_content' + subgroupid);
    }
}

function create_not_preset_item(ui_index, subgroupid, subgroupname)
{
    var tab_subgroup_table_content = document.getElementById('tab_subgroup_table_content99999');
    if (tab_subgroup_table_content == null)
    {
        console.log("create_not_preset_item() tab_subgroup_table_content == null");
        return;
    }

    var not_preset_item = document.getElementById('not_preset_item_'+subgroupid);
    if (not_preset_item != null)
    {
        console.log("create_not_preset_item() not_preset_item == null");
        return;
    }

    var row = document.createElement('tr');
    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","not_preset_item_row_"+subgroupid);

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // subgroup
    var tdsubgroup = document.createElement('td');
    row.appendChild(tdsubgroup);
    var Spantdsubgroup  = document.createElement("span");
    Spantdsubgroup.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    Spantdsubgroup.textContent = subgroupname;
    row.cells[cellIndex].appendChild(Spantdsubgroup);
    cellIndex++;

    // totalcount
    var totalcount = 0;
    row.appendChild(document.createElement('td'));
    var Spantotalcount  = document.createElement("span");
    Spantotalcount.setAttribute("id","not_preset_item_total_"+subgroupid);
    Spantotalcount.setAttribute("class","label label-primary");
    Spantotalcount.textContent = totalcount;
    row.cells[cellIndex].appendChild(Spantotalcount);
    cellIndex++;

    // presentcount
    var presentcount = 0;
    row.appendChild(document.createElement('td'));
    var Spanppresentcount  = document.createElement("span");
    Spanppresentcount.setAttribute("id","not_preset_item_present_"+subgroupid);
    if (presentcount == 0)
    {
        Spanppresentcount.setAttribute("class","label label-danger");
        Spanppresentcount.textContent = presentcount;
    }
    else
    {
        Spanppresentcount.setAttribute("class","label label-primary");
        Spanppresentcount.textContent = presentcount;
    }
    row.cells[cellIndex].appendChild(Spanppresentcount);
    cellIndex++;

    var absentCount = totalcount - presentcount;
    row.appendChild(document.createElement('td'));
    var Spanabsentcount  = document.createElement("span");
    Spanabsentcount.setAttribute("id","not_preset_item_absent_"+subgroupid);
    if (absentCount == 0)
    {
        Spanabsentcount.setAttribute("class","label label-primary");
        Spanabsentcount.textContent = absentCount;
    }
    else
    {
        Spanabsentcount.setAttribute("class","label label-danger");
        Spanabsentcount.textContent = absentCount;
    }
    row.cells[cellIndex].appendChild(Spanabsentcount);
    cellIndex++;

    // user
    var tdsubgroup = document.createElement('td');
    row.appendChild(tdsubgroup);
    var SpanUser  = document.createElement("span");
    SpanUser.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    SpanUser.setAttribute("id","not_preset_item_user_"+subgroupid);
    SpanUser.textContent = "";
    row.cells[cellIndex].appendChild(SpanUser);
    cellIndex++;

    var td_action = document.createElement('td');
    td_action.setAttribute("id","not_preset_item_action_"+subgroupid);
    row.appendChild(td_action);

    tab_subgroup_table_content.appendChild(row);
}

function create_not_present_user(userid, cardid)
{

    var UseName = getUseName(parseInt(userid));
    var UseAccount = getUseAccount(userid);
    // var divuser = document.createElement("div");
    var Spanuser  = document.createElement("span");
    Spanuser.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    //Spanuser.setAttribute("class","badge bg-light-blue-active");
    //Spanuser.setAttribute("class","label label-success");
    // Spanuser.setAttribute("style","font-size:14px; font-weight:bold;line-height:2; color:#330033;background-color:#00a7d0;");
    Spanuser.textContent = UseName + " [ " +UseAccount+" ] ";
    // divuser.appendChild(Spanuser);

    if(gl_enable_batch_present)
    {
        var ui_checkbox = document.createElement('input');
        ui_checkbox.setAttribute("id","id_row_checkbox_"+ userid);
        ui_checkbox.setAttribute("type",'checkbox');
        ui_checkbox.setAttribute("onclick","click_user();return true;");
        Spanuser.appendChild(ui_checkbox);
        Spanuser.setAttribute("onclick","click_user_name(\""+ userid +"\", \""+ cardid +"\");return true;");
    }

    return Spanuser;
}

var gl_click_user = false;
function click_user()
{
    gl_click_user = true;
}

function click_user_name(userid, cardid)
{
    console.log("click_user_name() userid:"+userid +" cardid:"+cardid);

    var ui_checkbox = document.getElementById('id_row_checkbox_'+ userid);

    if (gl_click_user)
    {
        // already changed
    }
    else
    {
        var checked = ui_checkbox.checked;
        if ( checked )
            ui_checkbox.checked = false;
        else
            ui_checkbox.checked = true;
    }

    if ( ui_checkbox.checked )
    {
        add_to_presentuser(userid, cardid);
    }
    else
    {
        remove_from_presentuser(userid, cardid);
    }

    gl_click_user = false;
}

function create_th(text)
{
    var table_thead_tr_th = document.createElement('th');
    table_thead_tr_th.textContent = text;
    return table_thead_tr_th;
}

function draw_userrollcallrecord_table()
{
    var record_table = document.getElementById('USER_ROLLCALLRECORD_TABLE');

    while (record_table.rows.length >= 1)
        record_table.deleteRow(0);

    // console.log("draw_table() gl_userrollcallrecord.length:"+gl_userrollcallrecord.length);

    var ui_index = 0;
    for (var i = 0; i < gl_userrollcallrecord.length; ++i)
    {
        var record = gl_userrollcallrecord[i];
        // console.log("draw_table() rollcalleventid:"+record.rollcalleventid+" rollcallid:"+ record.rollcallid +" userrecord.length:"+record.userrecord.length);

        for (var j = 0; j < record.userrecord.length; ++j)
        {
            var user_record = record.userrecord[j];

            var row = create_user_record_table_item(ui_index, record, user_record);
            record_table.appendChild(row);
            ui_index++;
        }
    }
}

function create_record_table_item(ui_index, record)
{
    if (record == null)
        return null;

    var row = document.createElement('tr');
    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    var UseName = '';
    var UseAccount = '';
    var cellIndex = 0;

    var rollcall = gl_cardrollcall;

    if (record.userid > 0)
    {
        UseName = getUseName(parseInt(record.userid));
        UseAccount = getUseAccount(record.userid);
    }

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+(ui_index + 1)));
    cellIndex++;

    // card id
    var tdcardid = document.createElement('td');
    row.appendChild(tdcardid);
    var Spancardid  = document.createElement("span");
    if (record.cardid.length > 0)
    {
        Spancardid.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    }
    Spancardid.textContent = record.cardid;
    row.cells[cellIndex].appendChild(Spancardid);
    cellIndex++;

    // 點名裝置
    row.appendChild(document.createElement('td'));
    var ui_rollcalldevices = createRollCallDeviceList(record);
    row.cells[cellIndex].appendChild(ui_rollcalldevices);

    // if ( record.rfiddeviceid.length > 0 )
    // {
    //     var rfiddevice_div = document.createElement("div");
    //     var label = create_rfiddevice_label(getRFIDDeviceName(record.rfiddeviceid));
    //     rfiddevice_div.appendChild(label);
    //     row.cells[cellIndex].appendChild(rfiddevice_div);
    // }
    // else
    // if ( record.nfclocator.length > 0 )
    // {
    //     var nfclocator_div = document.createElement("div");
    //     var label = create_nfclocator_label(getNFCLocatorDeviceName(record.nfclocator));
    //     nfclocator_div.appendChild(label);
    //     row.cells[cellIndex].appendChild(nfclocator_div);
    // }
    cellIndex++;

    // userid
    var tduserid = document.createElement('td');
    row.appendChild(tduserid);
    var Spanuserid  = document.createElement("span");
    if (record.userid > 0)
    {
        Spanuserid.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
        Spanuserid.textContent = UseName + " [ "+ UseAccount+" ]";
    }
    row.cells[cellIndex].appendChild(Spanuserid);
    cellIndex++;

    // present
    var tdpresent = document.createElement('td');
    row.appendChild(tdpresent);
    var Spanpresent  = document.createElement("span");
    if (record.present == 0)
    {
        Spanpresent.setAttribute("class","label label-danger");
        //Spanpresent.textContent = record.present;
        if (rollcall != null)
            Spanpresent.textContent = rollcall.absentcomment;

    }
    else
    {
        Spanpresent.setAttribute("class","label label-primary");
        //Spanpresent.textContent = record.present;
        if (rollcall != null)
            Spanpresent.textContent = rollcall.presentcomment;
    }
    row.cells[cellIndex].appendChild(Spanpresent);
    cellIndex++;

    // time
    var tddatetime = document.createElement('td');
    row.appendChild(tddatetime);

    var badge_EndDate = document.createElement("span");
    badge_EndDate.setAttribute("class","label label-info");
    if (record.present != 0)
    {
        badge_EndDate.textContent = convertPrettyDateOnly(record.datetime);
    }
    row.cells[cellIndex].appendChild(badge_EndDate);

    var badge_EndTime = document.createElement("span");
    badge_EndTime.setAttribute("class","label label-warning");
    if (record.present != 0)
    {
        badge_EndTime.textContent = convertPrettyTimeOnly(record.datetime);
    }
    row.cells[cellIndex].appendChild(badge_EndTime);
    cellIndex++;

    // 操作
    row.appendChild(document.createElement('td'));
    if (record.present == 0)
    {
        var intime = check_rollcall_intime();
        if (intime)
        {
            var checkBtn = document.createElement("button");
            checkBtn.setAttribute("title","設定點名已到");
            checkBtn.setAttribute("href","javascript:;");
            checkBtn.setAttribute("onclick","confirm_set_present(\""+record.userid+ "\",\""+record.cardid+"\", \""+UseName+"\");return false;");
            var imgCheck = document.createElement("i");
            imgCheck.setAttribute("class","fa fa-user-plus");
            checkBtn.appendChild(imgCheck);
            row.cells[cellIndex].appendChild(checkBtn);
        }
    }
    cellIndex++;

    return row;
}

function create_user_record_table_item(ui_index, record, user_record)
{
    if (record == null)
        return null;

    var row = document.createElement('tr');
    if ((ui_index % 2) == 1)
        row.setAttribute("bgcolor","#ebebe0");

    var cellIndex = 0;

    // record.rollcalleventid = record.rollcalleventid;
    // record.rollcallid = record.rollcallid;
    // record.userrecord = [];

    // user_record = [];
    // user_record.userid = record.userid;
    // user_record.card = [];

    var rollcall = gl_cardrollcall;

    var UseName = "";
    var UseAccount = "";
    if (user_record.userid > 0)
    {
        UseName = getUseName(parseInt(user_record.userid));
        UseAccount = getUseAccount(user_record.userid);
    }

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+(ui_index + 1)));
    cellIndex++;

    // userid
    var tduserid = document.createElement('td');
    //tduserid.setAttribute("id","uiid_userid_"+record.recordid);
    row.appendChild(tduserid);
    var Spanuserid  = document.createElement("span");
    var userid = parseInt(user_record.userid);
    if (userid > 0)
    {
        Spanuserid.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
        Spanuserid.textContent = UseName +" [ "+UseAccount+" ] ";
    }
    row.cells[cellIndex].appendChild(Spanuserid);
    cellIndex++;

    //console.log("create_user_record_table_item() user_record.card.length:"+user_record.card.length);

    // card id
    row.appendChild(document.createElement('td'));
    for (var i = 0; i < user_record.card.length; ++i)
    {
        var user_card = user_record.card[i];
        var divcardid  = document.createElement("div");
        row.cells[cellIndex].appendChild(divcardid);

        var Spancardid  = document.createElement("span");
        if (user_card.cardid.length > 0)
        {
            Spancardid.setAttribute("style","font-size:14px; font-weight:bold;line-height:2; color:#330033;");
        }
        Spancardid.textContent = user_card.cardid;
        divcardid.appendChild(Spancardid);
    }
    cellIndex++;

    // 點名裝置
    row.appendChild(document.createElement('td'));
    for (var i = 0; i < user_record.card.length; ++i)
    {
        var user_card = user_record.card[i];
        var divDevice  = document.createElement("div");
        row.cells[cellIndex].appendChild(divDevice);

        var ui_rollcalldevices = createRollCallDeviceList(user_card);

        divDevice.appendChild(ui_rollcalldevices);
    }
    cellIndex++;

    // present
    row.appendChild(document.createElement('td'));
    for (var i = 0; i < user_record.card.length; ++i)
    {
        var user_card = user_record.card[i];
        var divpresent = document.createElement("div");
        row.cells[cellIndex].appendChild(divpresent);

        //console.log("create_user_record_table_item() rollcall.absentcomment :"+rollcall.absentcomment );

        if (user_card.present == "0")
        {
            divpresent.appendChild(create_absent_label(rollcall.absentcomment));
        }
        else
        {
            divpresent.appendChild(create_present_label(rollcall.presentcomment));
        }
    }
    cellIndex++;

    // time
    row.appendChild(document.createElement('td'));
    for (var i = 0; i < user_record.card.length; ++i)
    {
        var user_card = user_record.card[i];
        var divpresenttime = document.createElement("div");
        row.cells[cellIndex].appendChild(divpresenttime);

        if (user_card.present == 0)
        {
            divpresenttime.appendChild(create_absent_label("無"));
        }
        else
        {
            divpresenttime.appendChild(create_present_label(user_card.datetime));
        }
    }
    cellIndex++;

    // 操作
    row.appendChild(document.createElement('td'));
    for (var i = 0; i < user_record.card.length; ++i)
    {
        var user_card = user_record.card[i];
        var divoperation = document.createElement("div");
        row.cells[cellIndex].appendChild(divoperation);

        if (user_card.present == 0)
        {
            var intime = check_rollcall_intime();
            if (intime)
            {
                var checkBtn = document.createElement("button");
                checkBtn.setAttribute("title","設定點名已到");
                checkBtn.setAttribute("href","javascript:;");
                checkBtn.setAttribute("onclick","confirm_set_present(\""+user_record.userid+ "\",\""+user_card.cardid+"\", \""+UseName+"\");return false;");
                var imgCheck = document.createElement("i");
                imgCheck.setAttribute("class","fa fa-user-plus");
                checkBtn.appendChild(imgCheck);
                divoperation.appendChild(checkBtn);
            }
        }
    }
    cellIndex++;

    return row;
}

function createRollCallDeviceList(rollcall)
{
    var span_devicelist = document.createElement("span");
    span_devicelist.setAttribute("id","uiid_devicelist_"+rollcall.rollcallid);

    if ( rollcall.rfiddeviceids.length > 0 )
    {
        var span_rfid_devicelist = create_rfid_devicelist_ui(rollcall.rollcallid, rollcall.rfiddeviceids);
        span_devicelist.appendChild(span_rfid_devicelist);
    }

    if ( rollcall.nfclocators.length > 0 )
    {
        var span_nfc_devicelist = create_nfc_devicelist_ui(rollcall.rollcallid, rollcall.nfclocators);
        span_devicelist.appendChild(span_nfc_devicelist);
    }

    if ( rollcall.rfiddeviceids.length == 0 && rollcall.nfclocators.length == 0 )
    {
        var label = create_no_device_label("無");
        span_devicelist.appendChild(label);
    }

    return span_devicelist;
}

function create_rfid_devicelist_ui(rollcallid, rfiddeviceids)
{
    var rfiddeviceidList = rfiddeviceids.split(",");
    var rfiddevice_span = document.createElement("span");
    rfiddevice_span.setAttribute("id","uiid_uhfreader_"+rollcallid);

    for(var index=0;index<rfiddeviceidList.length;index++)
    {
        var rfiddevice_div = document.createElement("div");
        var label = create_rfiddevice_label(getRFIDDeviceName(rfiddeviceidList[index]));
        rfiddevice_div.appendChild(label);
        rfiddevice_span.appendChild(rfiddevice_div);
    }

    return rfiddevice_span;
}

function create_nfc_devicelist_ui(rollcallid, nfclocators)
{
    var nfclocatorList = nfclocators.split(",");
    var nfclocator_span = document.createElement("span");
    nfclocator_span.setAttribute("id","uiid_nfclocator_"+rollcallid);

    for(var index=0;index<nfclocatorList.length;index++)
    {
        var nfclocator_div = document.createElement("div");
        var label = create_nfclocator_label(getNFCLocatorDeviceName(nfclocatorList[index]));
        nfclocator_div.appendChild(label);
        nfclocator_span.appendChild(nfclocator_div);
    }

    return nfclocator_span;
}

function create_rfiddevice_label(context)
{
    var rfiddevice_span = document.createElement("span");
    //rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a7d0;");
    rfiddevice_span.textContent = context;
    return rfiddevice_span;
}

function create_nfclocator_label(context)
{
    var rfiddevice_span = document.createElement("span");
    //rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#f39c12;");
    rfiddevice_span.textContent = context;
    return rfiddevice_span;
}

function create_no_device_label(context)
{
    var rfiddevice_span = document.createElement("span");
    //rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#a12409;");
    rfiddevice_span.textContent = context;
    return rfiddevice_span;
}

function create_present_label(context)
{
    var rfiddevice_span = document.createElement("span");
    //rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#3c8dbc;");
    rfiddevice_span.textContent = context;
    return rfiddevice_span;
}

function create_absent_label(context)
{
    var rfiddevice_span = document.createElement("span");
    //rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#00a65a;");
    rfiddevice_span.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:2;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:#dd4b39;");
    rfiddevice_span.textContent = context;
    return rfiddevice_span;
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

function check_rollcall_intime()
{
    var intime = false;
    var StartDateTime = moment(gl_cardrollcallevent.StartDateTime, "YYYY-MM-DD HH:mm:ss");
    var EndDateTime = moment(gl_cardrollcallevent.EndDateTime, "YYYY-MM-DD HH:mm:ss");
    var CurDateTime = moment();

    // console.log("check_rollcall_intime() gl_cardrollcallevent.StartDateTime:"+gl_cardrollcallevent.StartDateTime);
    // console.log("check_rollcall_intime() gl_cardrollcallevent.EndDateTime:"+gl_cardrollcallevent.EndDateTime);
    // console.log("check_rollcall_intime() CurDateTime:"+CurDateTime);

    if ( (CurDateTime >= StartDateTime) && (CurDateTime <= EndDateTime) )
    {
        intime = true;
    }

    return intime;
}

function enable_batch_present()
{
    console.log("enable_batch_present() ");

    gl_batch_present_user = [];

    gl_enable_batch_present = true;

    var checkBtn = document.getElementById("enable_batch_present_btn");
    var imgCheck = document.getElementById("enable_batch_present_icon");

    checkBtn.setAttribute("title","設定點名已到");
    checkBtn.setAttribute("onclick","save_batch_present();return false;");
    imgCheck.setAttribute("class","fa fa-save");

    reload_rollcall_ui();
}

function add_to_presentuser(userid, cardid)
{
    var found = false;
    for (var i = 0; i < gl_batch_present_user.length; ++i)
    {
        var present_user = gl_batch_present_user[i];

        if (present_user.userid == userid)
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        var present_user = {};
        present_user.userid = userid;
        present_user.cardid = cardid;
        gl_batch_present_user.push(present_user);
    }
    console.log("add_to_presentuser() gl_batch_present_user.length:"+gl_batch_present_user.length);
}

function remove_from_presentuser(userid, cardid)
{
    var found = false;
    for (var i = 0; i < gl_batch_present_user.length; ++i)
    {
        var present_user = gl_batch_present_user[i];

        if (present_user.userid == userid)
        {
            found = true;
            gl_batch_present_user.splice(i, 1);
            break;
        }
    }

    console.log("remove_from_presentuser() gl_batch_present_user.length:"+gl_batch_present_user.length);
}

function save_batch_present()
{
    console.log("save_batch_present() ");

    if (confirm('是否要批次設定點名已到?'))
    {
        batch_set_present();
    }
    else
    {
        // Do nothing!
        return ;
    }
}

function batch_set_present()
{
    gl_enable_batch_present = false;

    var userids = [];
    var cardids = [];

    for (var i = 0; i < gl_batch_present_user.length; ++i)
    {
        var present_user = gl_batch_present_user[i];
        userids.push(present_user.userid);
        cardids.push(present_user.cardid);
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_rollcall_record_batch'] = 1;
    jsondata['rollcallid'] = rollcallid;
    jsondata['eventid'] = eventid;
    jsondata['userids'] = userids.toString();
    jsondata['cardids'] = cardids.toString();

    var targetURL = gl_target_server + "/php/rfiddevice.php";

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

            var checkBtn = document.getElementById("enable_batch_present_btn");
            var imgCheck = document.getElementById("enable_batch_present_icon");

            checkBtn.setAttribute("title","批次設定點名已到");
            checkBtn.setAttribute("onclick","enable_batch_present();return false;");
            imgCheck.setAttribute("class","fa fa-user-plus");

            gl_batch_present_user = [];
            glReloadCount = 5;
            setTimeout(reload_rollcall_info, 1000);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

}

function set_present(userid, cardid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_rollcall_record'] = 1;
    jsondata['rollcallid'] = rollcallid;
    jsondata['eventid'] = eventid;
    jsondata['cardid'] = cardid;
    jsondata['userid'] = userid;

    var targetURL = gl_target_server + "/php/rfiddevice.php";

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

            glReloadCount = 5;
            setTimeout(reload_rollcall_info, 5000);
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function confirm_set_present(userid, cardid, UseName)
{
    if (confirm('是否要設定'+UseName+'點名已到?'))
    {
        set_present(userid, cardid);
    }
    else
    {
        // Do nothing!
    }
}

function create_userrollcallresult_xlsx_report()
{
    var xlsxreport_status = document.getElementById("xlsxreport_status");
    xlsxreport_status.textContent = "產生報表中...";

    var SendDate = "loaduserrollcallresultxlsxreport=1&project_id="+project_id+"&rollcallid="+rollcallid+"&eventid="+eventid;

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
            console.log("create_userrollcallresult_xlsx_report() success:"+result.success+" xlsxreport:"+result.xlsxreport);

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

function check_user_subgroup(userid, subgroupid)
{
    var valid = false;
    var int_userid = parseInt(userid);
    var pure_subgroupid = parseInt(subgroupid) +'';

    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];

        if ( parseInt(user.userid) == userid )
        {
            var subgroups = user.subgroups.split(",");
            var ret = subgroups.indexOf(pure_subgroupid);
            if (ret >= 0)
                valid = true;
            break;
        }
    }

    return valid;
}

function getUseName(userid)
{
    var username = userid;
    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];

        if ( parseInt(user.userid) == userid )
        {
            username = user.name;
            break;
        }

    }
    return username;
}

function getUseAccount(userid)
{
    var useraccount= userid;
    for (var i = 0; i < gl_user_info.length; ++i)
    {
        var user = gl_user_info[i];

        if ( parseInt(user.userid) == userid )
        {
            useraccount = user.account;
            break;
        }

    }
    return useraccount;
}

function getRFIDDeviceName(deviceid)
{
    var RFIDDeviceName = deviceid;
    for (var i = 0; i < gl_rfiddevices.length; ++i)
    {
        var rfiddevice = gl_rfiddevices[i];

        if ( rfiddevice.deviceid === deviceid )
        {
            RFIDDeviceName = rfiddevice.name;
            break;
        }

    }
    return RFIDDeviceName;
}

function getNFCLocatorDeviceName(macaddress)
{
    var NFCLocatorName = macaddress;
    for (var i = 0; i < gl_nfclocators.length; ++i)
    {
        var nfclocator = gl_nfclocators[i];

        if ( nfclocator.macaddress === macaddress )
        {
            NFCLocatorName = nfclocator.name;
            break;
        }

    }
    return NFCLocatorName;
}

function convertPrettyDateOnly(inTime)
{
    //var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset("+08:00");
    var cur_date = moment(inTime, "YYYY-MM-DD HH:mm:ss");
    var year = cur_date.format('YYYY');
    var month = cur_date.format('M');
    var day = cur_date.format('D');

    var perty_str = "";
    perty_str = year + "年" + month + "月" + day + "日 ";
    return perty_str;
}

function convertPrettyTimeOnly(inTime)
{
    //var cur_date = moment.utc(inTime, "YYYY-MM-DD HH:mm:ss").utcOffset("+08:00");
    var cur_date = moment(inTime, "YYYY-MM-DD HH:mm:ss");
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

function open_new_page(openurl)
{
    var win = window.open(openurl, '_blank');
    win.focus();
}

function open_cardrollcallevent_page()
{
    var win = window.open('/index.php/vilscardrollcallevent/'+project_id, '_blank');
    win.focus();
}
