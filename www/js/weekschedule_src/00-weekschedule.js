/*
 * Author: ikki Chung
 * Date: 2020 07 16
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var gl_usersubgroups = [];
var gl_userdatasubgroups = [];
var gl_subgroup_schedule = [];

var gl_edit_mode = 0;
var gl_current_groupid = 0;

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
             CurrentLoadIndex = CurrentLoadIndex + 1;
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

             load_subgroup_schedule();
         }
     break;
     case 3:
         if ( PreLoadIndex != CurrentLoadIndex )
         {
             console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
             PreLoadIndex = CurrentLoadIndex;

             show_schedule();
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

function load_subgroup_schedule()
{
    gl_subgroup_schedule = [];

    var subgroupids = "";

    for (var i = 0; i < gl_usersubgroups.length; ++i)
    {
        var subgroup = gl_usersubgroups[i];
        if ( i != 0 )
            subgroupids += ',';
        subgroupids += subgroup.subgroupid;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadweekschedule=1&subgroupids='+subgroupids+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, schedule)
        {
            gl_subgroup_schedule.push(schedule.jsonstring);
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });



    // var schedule = {};
    // schedule.subgroupid = '003';
    // schedule.title = '109年度日課表';
    // schedule.class_teacher = '郭志和老師';
    // schedule.classinfo = [];
    //
    // //schedule.class[0] = create_class('08:25 <br>|<br> 09:10', '國語', '英語', '數學', '勞作', '體育');
    //
    // schedule.classinfo[0] = create_class('08:25', '09:10', '國語', '英語', '數學', '勞作', '體育');
    // schedule.classinfo[1] = create_class('09:20', '10:05', '國語', '英語', '數學', '勞作', '體育');
    // schedule.classinfo[2] = create_class('10:15', '11:00', '國語', '英語', '數學', '勞作', '體育');
    // schedule.classinfo[3] = create_class('11:10', '11:55', '國語', '英語', '數學', '勞作', '體育');
    //
    // schedule.classinfo[4] = create_class('13:20', '14:05', '國語', '英語', '數學', '勞作', '體育');
    // schedule.classinfo[5] = create_class('14:15', '15:00', '國語', '英語', '數學', '勞作', '體育');
    // schedule.classinfo[6] = create_class('15:20', '16:05', '國語', '英語', '數學', '勞作', '體育');
    // schedule.classinfo[7] = create_class('16:15', '17:00', '國語', '英語', '數學', '勞作', '體育');
    //
    // // time_7_class.innerHTML = '';
    //
    // gl_subgroup_schedule[0] = schedule;

    // CurrentLoadIndex = CurrentLoadIndex + 1;
}

function create_class(starttime, endtime, monday, tuesday, wednesday, thursday, friday)
{
    var class_info = {};
    class_info.starttime = starttime;
    class_info.endtime = endtime;
    class_info.monday = monday;
    class_info.tuesday = tuesday;
    class_info.wednesday = wednesday;
    class_info.thursday = thursday;
    class_info.friday = friday;
    return class_info;
}

function show_schedule()
{
    var subgroup = gl_userdatasubgroups[0];
    var group_len = gl_userdatasubgroups.length;
    if ( gl_groupid == 3 )
    {
        subgroup = gl_usersubgroups[0];
        group_len = gl_usersubgroups.length;
    }
    else
    {
        subgroup = gl_userdatasubgroups[0];
        group_len = gl_userdatasubgroups.length;
    }

    if ( group_len == 1 )
    {
        var class_title = document.getElementById('class_title');
        class_title.textContent = subgroup.subgroupname;
    }
    else
    {
        if ( gl_groupid == 3 )
        {
            create_dropdown_class(gl_usersubgroups);
        }
        else
        {
            create_dropdown_class(gl_userdatasubgroups);
        }
    }

    update_schedule_view(subgroup.subgroupid);
}

function update_schedule_view(subgroupid)
{
    gl_current_groupid = subgroupid;

    var schedule = get_schedule(subgroupid);

    //
    // clear data
    //
    schedule_title.textContent = "";
    class_teacher.textContent = "";

    for(var i=0;i<8;i++)
    {
        show_class('time', i+1, "");
        show_class('monday', i+1, "");
        show_class('tuesday', i+1, "");
        show_class('wednesday', i+1, "");
        show_class('thursday', i+1, "");
        show_class('friday', i+1, "");
    }

    if (schedule == null)
    {
        // console.log("show_schedule() schedule == null ");

        if ( gl_edit_mode == 1)
        {
            edit_mode()
        }

        return;
    }

    schedule_title.textContent = schedule.title;
    class_teacher.textContent = schedule.class_teacher;

    for(var i=0;i<8;i++)
    {
        var class_info = schedule.classinfo[i];

        show_class('time', i+1, class_info.starttime + '<br>|<br>' + class_info.endtime);
        show_class('monday', i+1, class_info.monday);
        show_class('tuesday', i+1, class_info.tuesday);
        show_class('wednesday', i+1, class_info.wednesday);
        show_class('thursday', i+1, class_info.thursday);
        show_class('friday', i+1, class_info.friday);
    }

    if ( gl_edit_mode == 1)
    {
        edit_mode()
    }
}

function show_class(prefix, day, time_value)
{
    var ui_id = prefix+ "_"+ day +"_class";
    var ui_ele = document.getElementById(ui_id);
    ui_ele.innerHTML = time_value;
}

function create_dropdown_class(subgroups)
{
    var class_title = document.getElementById('class_title');

    var div_btngroup = document.createElement("div");
    div_btngroup.setAttribute("class","btn-group");
    class_title.appendChild(div_btngroup);

    var first_subgroup = subgroups[0];

    var div_button = document.createElement("button");
    div_button.setAttribute("type","button");
    div_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_button.setAttribute("data-toggle","dropdown");
    div_button.setAttribute("id","dropdown_current_class");
    div_button.textContent = first_subgroup.subgroupname;
    div_btngroup.appendChild(div_button);

    var div_span = document.createElement("span");
    div_span.setAttribute("class","caret");
    div_button.appendChild(div_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_btngroup.appendChild(div_ul);

    for (var i = 0; i < subgroups.length; ++i)
    {
        var subgroup = subgroups[i];

        div_ul.appendChild(create_dropdown_class_item(subgroup));
    }
}

function create_dropdown_class_item(subgroup)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", subgroup.subgroupname);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_class_click(\""+ subgroup.subgroupid +"\");return true;");

    item_click.appendChild(document.createTextNode(subgroup.subgroupname));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_class_click(subgroupid)
{
    // console.log("dropdown_class_click() ");

    var div_button = document.getElementById("dropdown_current_class");

    var curSubGroups = gl_userdatasubgroups;
    if ( gl_groupid == 3 )
    {
        curSubGroups = gl_usersubgroups;
    }

    for (var i = 0; i < curSubGroups.length; ++i)
    {
        var subgroup = curSubGroups[i];

        // console.log("dropdown_class_click() subgroupid:"+subgroupid+" subgroup.subgroupid:"+subgroup.subgroupid);

        if ( subgroupid == subgroup.subgroupid )
        {
            // console.log("dropdown_class_click() subgroupname:"+subgroup.subgroupname);

            div_button.textContent = subgroup.subgroupname;

            var div_span = document.createElement("span");
            div_span.setAttribute("class","caret");
            div_button.appendChild(div_span);

            update_schedule_view(subgroup.subgroupid);

            break;
        }
    }
}

function edit_mode()
{
    if (gl_edit_mode == 1)
    {
        return;
    }

    gl_edit_mode = 1;

    var ui_schedule_title = document.getElementById("schedule_title");
    ui_schedule_title.innerHTML = "<a href='javascript:;' onclick='edit_schedule_title(\""+ ui_schedule_title.innerHTML +"\");'>"+ ui_schedule_title.innerHTML +" <i class='fa fa-edit'/></a>";

    var ui_class_teacher = document.getElementById("class_teacher");
    ui_class_teacher.innerHTML = "<a href='javascript:;' onclick='edit_class_teacher(\""+ ui_class_teacher.innerHTML +"\");'>"+ ui_class_teacher.innerHTML +" <i class='fa fa-edit'/></a>";

    for(var i=1;i<=8;i++)
    {
        update_time_edit_mode('time', i);
        update_edit_mode('monday', i);
        update_edit_mode('tuesday', i);
        update_edit_mode('wednesday', i);
        update_edit_mode('thursday', i);
        update_edit_mode('friday', i);
    }
}

function update_time_edit_mode(prefix, day)
{
    var ui_id = prefix+ "_"+ day +"_class";
    var ui_ele = document.getElementById(ui_id);

    var schedule = get_schedule(gl_current_groupid);

    if (schedule == null)
    {
        schedule = create_default_schedule(gl_current_groupid);
    }

    var class_info = schedule.classinfo[day-1];
    //class_info.starttime + '<br>|<br>' + class_info.endtime

    var edit_time_ui = "<a href='javascript:;' onclick='edit_class_starttime(\""+prefix+"\",\""+day+"\",\""+ class_info.starttime +"\");'>"+ class_info.starttime +" <i class='fa fa-edit'></i></a>"
    edit_time_ui += '<br>|<br>';
    edit_time_ui += "<a href='javascript:;' onclick='edit_class_endtime(\""+prefix+"\",\""+day+"\",\""+ class_info.endtime +"\");'>"+ class_info.endtime +" <i class='fa fa-edit'></i></a>"

    ui_ele.innerHTML = edit_time_ui;
}

function update_edit_mode(prefix, day)
{
    var ui_id = prefix+ "_"+ day +"_class";
    var ui_ele = document.getElementById(ui_id);
    ui_ele.innerHTML = "<a href='javascript:;' onclick='edit_class(\""+prefix+"\",\""+day+"\",\""+ ui_ele.textContent +"\");'>"+ ui_ele.textContent +" <i class='fa fa-edit'/></a>";
}

function edit_class_starttime(prefix, day, orig_value)
{
    // console.log("edit_class_starttime() prefix:"+prefix+" day:"+day+" orig_value:"+orig_value);

    var newValue = prompt("輸入課程開始時間:", orig_value);

    update_class_starttime(prefix, day, newValue);

    update_time_edit_mode(prefix, day);
}

function edit_class_endtime(prefix, day, orig_value)
{
    // console.log("edit_class_endtime() prefix:"+prefix+" day:"+day+" orig_value:"+orig_value);

    var newValue = prompt("輸入課程結束時間:", orig_value);

    update_class_endtime(prefix, day, newValue);

    update_time_edit_mode(prefix, day);
}

function edit_class(prefix, day, orig_value)
{
    var newValue= prompt("輸入課程名稱:", orig_value);
    //if ( newValue )
    {
        var ui_id = prefix+ "_"+ day +"_class";
        var ui_ele = document.getElementById(ui_id);

        ui_ele.innerHTML = "<a href='javascript:;' onclick='edit_class(\""+prefix+"\",\""+day+"\",\""+ newValue +"\");'>"+ newValue +" <i class='fa fa-edit'/></a>";


        update_class_info(prefix, day, newValue);
    }
}

function edit_schedule_title(orig_value)
{
    var newValue= prompt("輸入課程年度名稱:", orig_value);
    //if ( newValue )
    {
        var ui_ele = document.getElementById("schedule_title");

        ui_ele.innerHTML = "<a href='javascript:;' onclick='edit_schedule_title(\""+ newValue +"\");'>"+ newValue +" <i class='fa fa-edit'/></a>";

        update_schedule_title(newValue);
    }
}

function edit_class_teacher(orig_value)
{
    var newValue= prompt("輸入班級導師名稱:", orig_value);
    //if ( newValue )
    {
        var ui_ele = document.getElementById("class_teacher");

        ui_ele.innerHTML = "<a href='javascript:;' onclick='edit_class_teacher(\""+ newValue +"\");'>"+ newValue +" <i class='fa fa-edit'/></a>";

        update_class_teacher(newValue);
    }
}

function get_schedule(subgroupid)
{
    var schedule = null;

    // console.log("get_schedule() subgroupid:"+subgroupid+" gl_subgroup_schedule.length:"+gl_subgroup_schedule.length);

    for (var i = 0; i < gl_subgroup_schedule.length; ++i)
    {
        var cur_schedule = gl_subgroup_schedule[i];
        // console.log("get_schedule()  cur_schedule.subgroupid:"+ cur_schedule.subgroupid);

        if ( subgroupid === parseInt(cur_schedule.subgroupid) )
        {
            return cur_schedule;
        }
    }
    return schedule;
}

function update_class_info(prefix, day, newValue)
{
    var schedule = get_schedule(gl_current_groupid);

    if (schedule == null)
    {
        return;
    }

    var class_info = schedule.classinfo[day-1];

    if ( prefix === 'monday')
        class_info.monday = newValue;
    else
    if ( prefix === 'tuesday')
        class_info.tuesday = newValue;
    else
    if ( prefix === 'wednesday')
        class_info.wednesday = newValue;
    else
    if ( prefix === 'thursday')
        class_info.thursday = newValue;
    else
    if ( prefix === 'friday')
        class_info.friday = newValue;

    update_schedule_info();
}

function update_schedule_title(newValue)
{
    var schedule = get_schedule(gl_current_groupid);

    if (schedule == null)
    {
        return;
    }

    schedule.title = newValue;

    update_schedule_info();
}

function update_class_teacher(newValue)
{
    var schedule = get_schedule(gl_current_groupid);

    if (schedule == null)
    {
        return;
    }

    schedule.class_teacher = newValue;

    update_schedule_info();
}

function update_class_starttime(prefix, day, newValue)
{
    var schedule = get_schedule(gl_current_groupid);

    if (schedule == null)
    {
        return;
    }

    var class_info = schedule.classinfo[day-1];

    class_info.starttime = newValue;

    update_schedule_info();
}

function update_class_endtime(prefix, day, newValue)
{
    var schedule = get_schedule(gl_current_groupid);

    if (schedule == null)
    {
        return;
    }

    var class_info = schedule.classinfo[day-1];

    class_info.endtime = newValue;

    update_schedule_info();
}

function update_schedule_info()
{
    var schedule = get_schedule(gl_current_groupid);

    if (schedule == null)
    {
        return;
    }

    var mySchedulejson = JSON.stringify(schedule);

    // console.log("mySchedulejson:"+mySchedulejson);

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_weekschedule'] = 1;
    jsondata['subgroupid'] = gl_current_groupid;
    jsondata['schedule'] = mySchedulejson;

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
            //alert("error = " + error);
            //alert("xhr.responseText = " + xhr.responseText);
        }
    });
}

function create_default_schedule(subgroupid)
{
    var schedule = {};
    schedule.subgroupid = subgroupid;
    schedule.title = '';
    schedule.class_teacher = '';
    schedule.classinfo = [];

    //schedule.class[0] = create_class('08:25 <br>|<br> 09:10', '國語', '英語', '數學', '勞作', '體育');

    schedule.classinfo[0] = create_class('08:25', '09:10', '', '', '', '', '');
    schedule.classinfo[1] = create_class('09:20', '10:05', '', '', '', '', '');
    schedule.classinfo[2] = create_class('10:15', '11:00', '', '', '', '', '');
    schedule.classinfo[3] = create_class('11:10', '11:55', '', '', '', '', '');

    schedule.classinfo[4] = create_class('13:20', '14:05', '', '', '', '', '');
    schedule.classinfo[5] = create_class('14:15', '15:00', '', '', '', '', '');
    schedule.classinfo[6] = create_class('15:20', '16:05', '', '', '', '', '');
    schedule.classinfo[7] = create_class('16:15', '17:00', '', '', '', '', '');

    gl_subgroup_schedule.push(schedule);

    return schedule;
}

function open_weekschedule_page()
{
    var win = window.open('/index.php/vilsweekschedule/'+project_id, '_blank');
    win.focus();
}
