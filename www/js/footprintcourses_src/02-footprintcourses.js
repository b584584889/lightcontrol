/*
 * Author: ikki Chung
 * Date: 2019 09 09
 * Description:
 *      SMIMS VILS RTLS Solutions
**/


var glCourse_List = [];

var LOAD_DEVICE_NUM = 20;
var RECORD_PAGE_COUNT = 5;

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;

var glCurrentCourse_clicktime = new Date('2019/01/01');
var glCurrentCourse_id = "";

var glTargetStartDate = null;
var glTargetEndDate = null;

var glCurrentCourse_Records = [];
var glDataLoading = 0;

var glTargetDate = getTodayDate();
var glShowDataMode = 0; // 0: timeline 1: one day


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
                load_courses();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                click_course("");
                CurrentLoadIndex = 2; // next state
            }
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);

                check_cookies();

                PreLoadIndex = CurrentLoadIndex;
            }
        break;
    }

    if ( CurrentLoadIndex != 1 )
        setTimeout(check_loading_status, 100);
    else
    {
        console.log("check_loading_status() Stop.");
    }
}

function load_courses()
{
    var targetURL = gl_target_server + "/php/course.php";
    var p = $.getJSON(targetURL, 'loadcourses=1'+'&project_id='+project_id, function(data)
    {
        $.each(data, function(courseId, value)
        {
            var course = {};
            course.courseId = value['courseId'];
            course.resourceId = value['resourceId'];
            course.title = value['title'];
            course.description = value['description'];
            course.startDate = value['start'];
            course.endDate = value['end'];
            course.startTime = value['startTime'];
            course.endTime = value['endTime'];
            course.startRecur = value['startRecur'];
            course.endRecur = value['endRecur'];
            course.backgroundColor = value['backgroundColor'];
            course.borderColor = value['borderColor'];

            course.allDay = false;
            if (value['allDay'] === '1')
                course.allDay = true;

            glCourse_List.push(course);

            // console.log("courseId:"+course.courseId+" resourceId:"+course.resourceId+" title:"+course.title);

        });
    })
    .success(function() {

        add_courses_to_table();

        CurrentLoadIndex = 1;
    })
    .error(function() {
        CurrentLoadIndex = 1;
    })
    .complete(function() {
        CurrentLoadIndex = 1;
    });
}

function add_courses_to_table()
{
    var course_list_content = document.getElementById('course_list_content');

    while (course_list_content.rows.length >= 1)
    {
        course_list_content.deleteRow(0);
    }

    console.log("add_courses_to_table() glCourse_List.length:"+glCourse_List.length);

    for (var i = 0; i < glCourse_List.length; i++)
    {
        var course = glCourse_List[i];

        var table_tr = document.createElement('tr');
        course_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        table_td_1.setAttribute("id","id_row_courseid_"+ course.courseId);
        //table_td_1.setAttribute("style","background-color:#CCEEFF;");
        table_td_1.setAttribute("onclick","click_course(\""+ course.courseId +"\");return false;");
        table_tr.appendChild(table_td_1);

        // title
        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_title_"+ course.courseId);
        table_td_2.setAttribute("onclick","click_course(\""+ course.courseId +"\");return false;");
        table_tr.appendChild(table_td_2);

        var label_title = document.createElement("span");
        label_title.setAttribute("style","display:inline;padding:.1em .6em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;background-color:"+ course.backgroundColor);
        label_title.textContent =  course.title;
        table_td_2.appendChild(label_title);

        // description
        var table_td_3 = document.createElement('td');
        table_td_3.setAttribute("id","id_row_description_"+ course.courseId);
        table_td_3.setAttribute("onclick","click_course(\""+ course.courseId +"\");return false;");
        table_td_3.textContent = course.description;
        table_tr.appendChild(table_td_3);

        // datetime
        var table_td_4 = document.createElement('td');
        table_td_4.setAttribute("id","id_row_datetime_"+ course.courseId);
        table_td_4.setAttribute("onclick","click_course(\""+ course.courseId +"\");return false;");
        //table_td_2.textContent = course.allDay;
        table_tr.appendChild(table_td_4);


        if (course.startRecur.length > 0 && course.endRecur.length > 0)
        {
            var from_date_span = document.createElement('span');
            from_date_span.textContent = "日期從 ";
            table_td_4.appendChild(from_date_span);

            var satrt_date_span = document.createElement('span');
            satrt_date_span.setAttribute("class",'label label-info');
            satrt_date_span.textContent = course.startRecur;
            table_td_4.appendChild(satrt_date_span);

            var to_date_span = document.createElement('span');
            //to_date_span.setAttribute("class",'label label-info');
            to_date_span.textContent = " 到 ";
            table_td_4.appendChild(to_date_span);

            var end_date_span = document.createElement('span');
            end_date_span.setAttribute("class",'label label-info');
            end_date_span.textContent = course.endRecur;
            table_td_4.appendChild(end_date_span);

            var br = document.createElement('br');
            table_td_4.appendChild(br);

            if (course.allDay)
            {
                var allDay_span = document.createElement('span');
                allDay_span.setAttribute("class",'badge bg-green');
                allDay_span.textContent = "All Day";
                table_td_4.appendChild(allDay_span);

                var br = document.createElement('br');
                table_td_4.appendChild(br);
            }
            else
            {
                var from_time_span = document.createElement('span');
                from_time_span.textContent = "時間從 ";
                table_td_4.appendChild(from_time_span);

                var satrt_time_span = document.createElement('span');
                satrt_time_span.setAttribute("class",'label label-success');
                satrt_time_span.textContent = course.startTime;
                table_td_4.appendChild(satrt_time_span);

                var to_time_span = document.createElement('span');
                //to_time_span.setAttribute("class",'label label-success');
                to_time_span.textContent = " 到 ";
                table_td_4.appendChild(to_time_span);

                var end_time_span = document.createElement('span');
                end_time_span.setAttribute("class",'label label-success');
                end_time_span.textContent = course.endTime;
                table_td_4.appendChild(end_time_span);
            }

        }
        else
        {
            var start_date = convertPrettyDateOnly(course.startDate);
            var end_date = convertPrettyDateOnly(course.endDate);

            if ( start_date === end_date )
            {
                var single_day_span = document.createElement('span');
                single_day_span.textContent = "日期 ";
                table_td_4.appendChild(single_day_span);

                var satrt_date_span = document.createElement('span');
                satrt_date_span.setAttribute("class",'label label-warning');
                satrt_date_span.textContent = start_date;
                table_td_4.appendChild(satrt_date_span);
            }
            else
            {
                var from_date_span = document.createElement('span');
                from_date_span.textContent = "日期從 ";
                table_td_4.appendChild(from_date_span);

                var satrt_date_span = document.createElement('span');
                satrt_date_span.setAttribute("class",'label label-info');
                satrt_date_span.textContent = start_date;
                table_td_4.appendChild(satrt_date_span);

                var to_date_span = document.createElement('span');
                //to_date_span.setAttribute("class",'label label-info');
                to_date_span.textContent = " 到 ";
                table_td_4.appendChild(to_date_span);

                var end_date_span = document.createElement('span');
                end_date_span.setAttribute("class",'label label-info');
                end_date_span.textContent = end_date;
                table_td_4.appendChild(end_date_span);
            }

            var br = document.createElement('br');
            table_td_4.appendChild(br);

            if (course.allDay)
            {
                var allDay_span = document.createElement('span');
                allDay_span.setAttribute("class",'badge bg-green');
                allDay_span.textContent = "All Day";
                table_td_4.appendChild(allDay_span);

                var br = document.createElement('br');
                table_td_4.appendChild(br);
            }
            else
            {
                var from_time_span = document.createElement('span');
                from_time_span.textContent = "時間從 ";
                table_td_4.appendChild(from_time_span);

                var satrt_time_span = document.createElement('span');
                satrt_time_span.setAttribute("class",'label label-success');
                satrt_time_span.textContent = convertPrettyTimeOnly(course.startDate);
                table_td_4.appendChild(satrt_time_span);

                var to_time_span = document.createElement('span');
                //to_time_span.setAttribute("class",'label label-success');
                to_time_span.textContent = " 到 ";
                table_td_4.appendChild(to_time_span);

                var end_time_span = document.createElement('span');
                end_time_span.setAttribute("class",'label label-success');
                end_time_span.textContent = convertPrettyTimeOnly(course.endDate);
                table_td_4.appendChild(end_time_span);

            }
        }
    }
}

function click_course(courseId)
{
    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentCourse_clicktime.getTime());

    if (diffTimeInMS < 500 || glDataLoading == 1)
    {
        console.log("click_course() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glDataLoading = 1;
    glCurrentCourse_clicktime = new Date();

    for (var i = 0; i < glCourse_List.length; i++)
    {
        var course = glCourse_List[i];

        // console.log("click_course() i:"+i +" courseId:"+courseId +" course.courseId:"+course.courseId +" glCurrentCourse_id:"+glCurrentCourse_id);
        var id_row_courseid = document.getElementById("id_row_courseid_"+ course.courseId);
        var id_row_title = document.getElementById("id_row_title_"+ course.courseId);
        var id_row_description = document.getElementById("id_row_description_"+ course.courseId);
        var id_row_datetime = document.getElementById("id_row_datetime_"+ course.courseId);

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentCourse_id.length == 0 || course.courseId === courseId )
        {
            glCurrentCourse_id = course.courseId;
            bg_color = "background-color:#CCEEFF;";
            // console.log("click_course() i:"+i +" courseId:"+courseId +" course.courseId:"+course.courseId +" glCurrentCourse_id:"+glCurrentCourse_id);
        }
        id_row_courseid.setAttribute("style",bg_color);
        id_row_title.setAttribute("style",bg_color);
        id_row_description.setAttribute("style",bg_color);
        id_row_datetime.setAttribute("style",bg_color);
    }

    var status_date = document.getElementById('daily_targetdate');
    if (status_date != undefined)
    {
        status_date.textContent = convertToTimelineDate(glTargetDate);
    }

    load_course_record();
}

function load_course_record()
{
    console.log("load_course_record() glCurrentCourse_id:" + glCurrentCourse_id);

    var record_box_title = document.getElementById('record_box_title');
    if (record_box_title != undefined)
    {
        record_box_title.textContent = "Loading...";
    }

    glCurrentCourse_Records = [];
    glTargetStartDate = null;
    glTargetEndDate = null;

    load_recent_record_express(glCurrentCourse_id);

    clear_timeline();
}

function load_recent_record_express(courseId)
{
    if (glTargetStartDate == undefined)
    {
        glTargetStartDate = getCourseTargetDate(0);
    }

    //console.log("load_recent_record_express() courseId:"+courseId+" glTargetStartDate:" + glTargetStartDate);

    var cur_show_count = RECORD_PAGE_COUNT;
    var cur_StartDate = glTargetStartDate;

    if ( glShowDataMode == 1 )
    {
        cur_show_count = 1;
        cur_StartDate = glTargetDate;
    }

    var targetURL = gl_target_server + "/php/course.php";
    $.getJSON(targetURL, 'checkrecentrollcallrecord=1&courseId='+courseId+'&recentdays='+cur_show_count+'&startdate='+cur_StartDate+'&project_id='+project_id, function(data)
    {
        //console.log("load_recent_record_express() data.length:"+data.length +", data[0].day:"+data[0].day);

        glTargetEndDate = glTargetStartDate;
        var EndDateNum = Number(glTargetEndDate);

        for (var index in data)
        {
            var dateData = data[index];
            var date = dateData.infoOfDate;

            if ( glShowDataMode == 1 )
            {
                if (!(date === glTargetDate))
                {
                    continue;
                }
            }

            //if(glCurrentCourse_Records[date] == undefined)
            //    glCurrentCourse_Records[date] = [];

            var record = {};
            record.date = date;
            record.tags = dateData.tags;
            record.tagnames = dateData.tagnames;

            var d1 = Number(date);

            if (d1 < EndDateNum)
            {
                EndDateNum = d1;
                glTargetEndDate = date;
            }

            glCurrentCourse_Records.push(record);
        }

        //console.log("load_recent_record_express() length:"+glCurrentCourse_Records.length+", glTargetStartDate:"+glTargetStartDate +", glTargetEndDate:"+glTargetEndDate);

        update_show_data()
    });

}

function reload_course_info()
{
    glCurrentCourse_Records = [];
    glTargetStartDate = getCourseTargetDateV1( glTargetEndDate, -1);
    glTargetEndDate = getCourseTargetDateV1( glTargetStartDate, RECORD_PAGE_COUNT);

    //console.log("reload_course_info() glCurrentCourse_id:"+glCurrentCourse_id+" glTargetStartDate:" + glTargetStartDate+" glTargetEndDate:" + glTargetEndDate);

    removeElement('reload_btn_id');

    load_recent_record_express(glCurrentCourse_id);
}

function load_course_rollcall_detail(date, uiid)
{
    var footer_btn1_ui = document.getElementById("timeline_footer_btn1_" + uiid);

    if (footer_btn1_ui != undefined)
    {
        if ( footer_btn1_ui.textContent === "讀取中..." )
        {
            return;
        }
        if ( footer_btn1_ui.textContent === "讀取完成" )
        {
            return;
        }

        footer_btn1_ui.textContent = "讀取中...";
    }

    show_detail_course(uiid, date);
}

function show_detail_course(uiid, target_date)
{
    var query_date = convertToTimelineDate(target_date);
    console.log("show_detail_course() uiid:"+uiid +", query_date:"+query_date);

    //
    // check if already existed
    //
    var table_id = "record_box_table_course_"+uiid;
    var table_ui = document.getElementById(table_id);
    if (table_ui != undefined)
    {
        console.log("show_detail_course() table_id:"+table_id +", existed!");
        return;
    }

    var CurrentCourse_Records = [];


    var targetURL = gl_target_server + "/php/fpnodes.php";
    $.getJSON(targetURL, 'loadcoursetrack=1&courseid='+glCurrentCourse_id+'&targetdate='+query_date+'&project_id='+project_id, function(data)
    {
        $.each(data, function(key, value)
        {
            var record = {};
            record.nodeid = value['nodeid'];
            record.nodename = value['nodename'];
            record.count = value['count'];
            record.max_time = value['max_time'];
            record.min_time = value['min_time'];
            record.total_time = value['total_time'];

            CurrentCourse_Records.push(record);
        });

        console.log("show_detail_course() CurrentCourse_Records.length:"+CurrentCourse_Records.length);

        put_record_to_table(uiid, glCurrentCourse_id, CurrentCourse_Records);

    });
}

function put_record_to_table(uiid, courseid, records)
{
    var pre_record = null;

    var target_id = "timeline_footer_" + uiid;
    var footer_timeline = document.getElementById(target_id);

    var table_ui = create_record_table_rollcall(uiid);
    footer_timeline.appendChild(table_ui);

    var rows = records.length;
    var table_height = (rows + 1) * 50 + 100;
    if (rows > 5)
        table_height = 300;

    table_ui.setAttribute("style", "height:"+table_height+"px;overflow:scroll");

    var table_body_id = "record_table_rollcall_body_" + uiid;
    var table_body = document.getElementById(table_body_id);

    var ui_index = 1;
    for(index in records)
    {
        var record = records[index];

        var row = create_course_rollcall_row(ui_index, record);
        table_body.appendChild(row);

        ui_index++;
    }

    update_footer_btn1_ui(uiid, "讀取完成");

    if (rows == 0)
    {
        var record = {};
        record.nodeid = '';
        record.nodename = '無資料';
        record.count = '';
        record.max_time = '';
        record.min_time = '';
        record.total_time = '';
        var row = create_course_rollcall_row(0, record);
        table_body.appendChild(row);
        return;
    }

}

function create_course_rollcall_row(ui_index, record)
{
    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","table_row_"+ui_index);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // name
    row.appendChild(document.createElement('td'));
    //row.cells[cellIndex].appendChild(document.createTextNode(record.nodename));
    var table_td_name_span = document.createElement('span');
    table_td_name_span.setAttribute("class",'badge bg-green');
    table_td_name_span.textContent = record.nodename;
    row.cells[cellIndex].appendChild(table_td_name_span);
    cellIndex++;

    // Enter Time
    var table_td_entertime_td = document.createElement('td')
    row.appendChild(table_td_entertime_td);
    var table_td_entertime_span = document.createElement('span');
    //table_td_entertime_span.setAttribute("class",'badge bg-light-blue');
    table_td_entertime_span.setAttribute("class",'label label-warning');
    table_td_entertime_span.textContent = convertPrettyCourseTime(record.min_time);
    table_td_entertime_td.appendChild(table_td_entertime_span);
    cellIndex++;

    // Leave Time
    var table_td_leaveltime_td = document.createElement('td')
    row.appendChild(table_td_leaveltime_td);
    var table_td_leaveltime_span = document.createElement('span');
    //table_td_leaveltime_span.setAttribute("class",'badge bg-purple');
    table_td_leaveltime_span.setAttribute("class",'label label-info');
    table_td_leaveltime_span.textContent = convertPrettyCourseTime(record.max_time);
    table_td_leaveltime_td.appendChild(table_td_leaveltime_span);
    cellIndex++;

    //var stayTime = moment.utc(moment(record.DataTime,"YYYY-MM-DD HH:mm:ss").diff(moment(record.DataTime,"YYYY-MM-DD HH:mm:ss"))).format("HH:mm:ss")

    // stay Time
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(convertPrettyTime(record.total_time)));
    cellIndex++;

    return row;
}

function create_record_table_rollcall(uiid)
{
    var divTabPane = document.createElement('div');
    divTabPane.setAttribute("id","record_box_table_course_"+uiid);

    //
    // Box
    //
    var boxGroup = document.createElement('div');
    boxGroup.setAttribute("class","box");
    divTabPane.appendChild(boxGroup);

    //
    // box-body
    //
    var boxBody = document.createElement("div");
    boxBody.setAttribute("class","box-body no-padding");
    boxGroup.appendChild(boxBody);

    // table
    var tableGroup = document.createElement('table');
    tableGroup.setAttribute("class","table table-bordered");
    boxBody.appendChild(tableGroup);

    var theadGroup = document.createElement("thead");
    tableGroup.appendChild(theadGroup);

    var theadtrGroup = document.createElement("tr");
    theadtrGroup.setAttribute("bgcolor","#cce6ff");
    theadtrGroup.setAttribute("id","record_table_head");
    theadGroup.appendChild(theadtrGroup);

    var theadth = document.createElement("th");
    theadth.setAttribute("style","width: 10px");
    theadth.textContent = '#';
    theadtrGroup.appendChild(theadth);

    var theadthName = document.createElement("th");
    theadthName.setAttribute("style","width: 150px");
    theadthName.textContent = '人員名稱';
    theadtrGroup.appendChild(theadthName);

    var theadth_EnterTime = document.createElement("th");
    theadth_EnterTime.setAttribute("style","width: 120px; text-align:center");
    theadth_EnterTime.textContent = '開始上課時間';
    theadtrGroup.appendChild(theadth_EnterTime);

    var theadth_LeaveTime = document.createElement("th");
    theadth_LeaveTime.setAttribute("style","width: 120px; text-align:center");
    theadth_LeaveTime.textContent = '離開課程時間';
    theadtrGroup.appendChild(theadth_LeaveTime);

    var theadth_StayTime = document.createElement("th");
    //theadth_StayTime.setAttribute("style","width: 50px; text-align:center");
    theadth_StayTime.textContent = '上課總時間';
    theadtrGroup.appendChild(theadth_StayTime);

    var tbodyGroup = document.createElement("tbody");
    tbodyGroup.setAttribute("id","record_table_rollcall_body_" + uiid);
    tableGroup.appendChild(tbodyGroup);

    return divTabPane;
}

function update_footer_btn1_ui(uiid, ui_msg)
{
    var footer_btn1_ui = document.getElementById("timeline_footer_btn1_" + uiid);

    if (footer_btn1_ui != undefined)
    {
        footer_btn1_ui.textContent = ui_msg;
    }
}

function update_show_data()
{
    show_data_on_timeline();

    glDataLoading = 0;

    var record_box_title = document.getElementById('record_box_title');
    if (record_box_title != undefined)
    {
        var chrCourseName = "";
        for (var i = 0; i < glCourse_List.length; i++)
        {
            var course = glCourse_List[i];

            if ( glCurrentCourse_id === course.courseId )
            {
                chrCourseName = course.title;
            }
        }
        record_box_title.textContent = chrCourseName;
    }
}

function show_data_on_timeline()
{
    //var keys = Object.keys(glCurrentCourse_Records);

    var main_timeline = document.getElementById('main_timeline');

    if(glCurrentCourse_Records.length == 0)
    {
        //console.log("show_data_on_timeline() no data!");
        var nodata_btn_ui = create_nodata_btn();
        main_timeline.appendChild(nodata_btn_ui);
        return;
    }

    for(var index in glCurrentCourse_Records)
    {
        var courseDateData = glCurrentCourse_Records[index];

        var time_label = create_time_label(convertToTimelineDate(courseDateData.date));
        main_timeline.appendChild(time_label);

        //console.log("show_data_on_timeline() date:"+courseDateData.date+", courseDateData.tags.length:"+courseDateData.tags.length);

        var uiid = "table_" + courseDateData.date;
        var course_rollcall_ui = create_course_rollcall_data_info(uiid, courseDateData.date, courseDateData);
        main_timeline.appendChild(course_rollcall_ui);

    }

    if ( glShowDataMode == 0 )
    {
        if(glCurrentCourse_Records.length < RECORD_PAGE_COUNT)
        {
            var nodata_btn_ui = create_nodata_btn();
            main_timeline.appendChild(nodata_btn_ui);
        }
        else
        {
            var reload_btn_ui = create_reload_btn();
            main_timeline.appendChild(reload_btn_ui);
        }
    }
}

function create_course_rollcall_data_info(uiid, date, courseDateData)
{
    var course_rollcall_ui = document.createElement("li");
    var img_icon = document.createElement("i");
    img_icon.setAttribute("class","fa fa-calendar bg-teal");
    course_rollcall_ui.appendChild(img_icon);

    var div_timeline_item = document.createElement("div");
    div_timeline_item.setAttribute("class","timeline-item");
    course_rollcall_ui.appendChild(div_timeline_item);

    var span_timeline_item_header = create_timeline_time_header("課程紀錄");
    div_timeline_item.appendChild(span_timeline_item_header);

    var span_timeline_item_body = create_timeline_time_body(uiid, "共 " + courseDateData.tags.length +" 個學員");
    div_timeline_item.appendChild(span_timeline_item_body);

    var tag_badge = create_tag_badge(courseDateData.tags, courseDateData.tagnames);
    div_timeline_item.appendChild(tag_badge);

    var span_timeline_item_footer = create_timeline_time_footer(date, uiid);
    div_timeline_item.appendChild(span_timeline_item_footer);

    return course_rollcall_ui;
}

function create_timeline_time_header(show_msg)
{
    var span_timeline_item_header = document.createElement("h3");
    span_timeline_item_header.setAttribute("class","timeline-header");
    span_timeline_item_header.textContent = show_msg;

    return span_timeline_item_header;
}

function create_timeline_time_body(uiid, show_msg)
{
    var div_timeline_item_body = document.createElement("div");
    div_timeline_item_body.setAttribute("class","timeline-body");
    div_timeline_item_body.textContent = show_msg;

    return div_timeline_item_body;
}

function create_timeline_time_footer(date, uiid)
{
    var div_timeline_item_footer = document.createElement("div");
    div_timeline_item_footer.setAttribute("class","timeline-footer");

    var footer_btn_1 = document.createElement("a");
    footer_btn_1.setAttribute("class","btn btn-primary btn-xs");
    footer_btn_1.setAttribute("onclick","load_course_rollcall_detail(\""+date+"\",\""+uiid+"\");");
    footer_btn_1.setAttribute("id","timeline_footer_btn1_" + uiid);
    footer_btn_1.textContent = "查看詳細資料";
    div_timeline_item_footer.appendChild(footer_btn_1);

    var div_item_footer = document.createElement("div");
    div_item_footer.setAttribute("id","timeline_footer_" + uiid);
    div_timeline_item_footer.appendChild(div_item_footer);

    return div_timeline_item_footer;
}

function create_tag_badge(tags, tagnames)
{
    var div_tag_badge = document.createElement("div");
    var table_tag_badge = document.createElement("table");
    div_tag_badge.appendChild(table_tag_badge);

    var tr_tag_badge = document.createElement("tr");
    table_tag_badge.appendChild(tr_tag_badge);

    var td_tag_badge = document.createElement("td");
    td_tag_badge.setAttribute("style","width:50px;text-align:center;");
    td_tag_badge.textContent = "學員:";
    tr_tag_badge.appendChild(td_tag_badge);

    var index = 0;
    for(index = 0;index<tags.length;index++)
    {
        if ( (index != 0) && (index%5==0))
        {
            tr_tag_badge = document.createElement("tr");
            table_tag_badge.appendChild(tr_tag_badge);
        }

        var td_tag_badge = document.createElement("td");
        //td_tag_badge.setAttribute("style","width:100px;text-align:center;");
        tr_tag_badge.appendChild(td_tag_badge);

        var tag_badge = document.createElement("span");
        //tag_badge.setAttribute("class","badge bg-purple");
        tag_badge.setAttribute("class","badge bg-green");
        tag_badge.textContent = tagnames[index];

        td_tag_badge.appendChild(tag_badge);
    }

    return div_tag_badge;
}

function create_nodata_btn()
{
    var nodata_btn_ui = document.createElement("li");

    var nodata_btn_span = document.createElement("span");
    nodata_btn_span.setAttribute("class","bg-red");
    nodata_btn_ui.appendChild(nodata_btn_span);

    var nodata_btn = document.createElement("button");
    nodata_btn.setAttribute("type","button");
    nodata_btn.setAttribute("class","btn btn-default btn-lrg ajax");
    nodata_btn.textContent = DEFINE_STRING_nodata;
    nodata_btn_span.appendChild(nodata_btn);

    return nodata_btn_ui;
}

function create_reload_btn()
{
    var reload_btn_ui = document.createElement("li");
    reload_btn_ui.setAttribute("id","reload_btn_id");
    reload_btn_ui.setAttribute("class","time-label");

    var reload_btn_span = document.createElement("span");
    reload_btn_span.setAttribute("class","bg-green");
    reload_btn_ui.appendChild(reload_btn_span);

    var reload_btn = document.createElement("button");
    reload_btn.setAttribute("type","button");
    reload_btn.setAttribute("onclick","reload_course_info();");
    reload_btn.setAttribute("class","btn btn-default btn-lrg ajax");
    reload_btn.setAttribute("title","Ajax Request");
    reload_btn.setAttribute("id","AjaxLoadMode");
    reload_btn.textContent = DEFINE_STRING_reload;
    reload_btn_span.appendChild(reload_btn);

    var img_icon = document.createElement("i");
    img_icon.setAttribute("class","fa fa-spin fa-refresh");
    reload_btn.appendChild(img_icon);

    return reload_btn_ui;
}

function clear_timeline()
{
    var main_timeline = document.getElementById('main_timeline');
    while (main_timeline.firstChild)
    {
        main_timeline.removeChild(main_timeline.firstChild);
    }
}

function create_time_label(cur_date)
{
    var time_label = document.createElement("li");
    time_label.setAttribute("class","time-label");

    var time_label_span = document.createElement("span");
    time_label_span.setAttribute("class","bg-yellow");
    time_label_span.textContent = cur_date;
    time_label.appendChild(time_label_span);
    return time_label;
}

function removeElement(id)
{
    var elem = document.getElementById(id);
    return elem.parentNode.removeChild(elem);
}

function convertPrettyDateOnly(inTime)
{
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


function add_search_tag_to_table(searchtext)
{
    var search_list_content = document.getElementById('search_list_content');

    while (search_list_content.rows.length >= 1)
    {
        search_list_content.deleteRow(0);
    }

    for (var i = 0; i < glCourse_List.length; i++)
    {
        var course = glCourse_List[i];

        var ret = course.title.indexOf(searchtext);
        if ( ret < 0 )
            continue;

        var table_tr = document.createElement('tr');
        search_list_content.appendChild(table_tr);

        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_course_name_"+ course.courseId);
        table_td_2.setAttribute("onclick","click_course(\""+ course.courseId +"\");return false;");
        table_tr.appendChild(table_td_2);

        // node id
        var tag_nodeid_span = document.createElement('span');
        tag_nodeid_span.setAttribute("class",'badge bg-green');
        tag_nodeid_span.textContent = course.title;
        tag_nodeid_span.setAttribute("onclick","click_course(\""+ course.courseId +"\");return false;");
        table_td_2.appendChild(tag_nodeid_span);
    }
}

const $source = document.querySelector('#tab_search_text');
$source.addEventListener('input', course_search_text_change);// register for oninput

function course_search_text_change(e)
{
    var input_value = document.getElementById("tab_search_text").value;

     //console.log("input_value:"+input_value);

     add_search_tag_to_table(input_value);

     setCookie("course_search_text", input_value, 10);
}

function check_cookies()
{
    var input_value = getCookie("course_search_text");

    document.getElementById("tab_search_text").value = input_value;

    add_search_tag_to_table(input_value);
}

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


function click_show_target_date()
{
    console.log("click_show_target_date()");

    var checkbox_ui = document.getElementById("show_target_date");
    if (checkbox_ui == undefined)
    {
        console.log("click_show_target_date() checkbox_ui == undefined");
        return;
    }

    var checked = checkbox_ui.checked;

    if (checked)
    {
        glShowDataMode = 1; // 0: timeline 1: one day
        document.getElementById("course_pdf_report_UI").style.display = 'block';
    }
    else
    {
        glShowDataMode = 0; // 0: timeline 1: one day
        document.getElementById("course_pdf_report_UI").style.display = 'none';
    }

    // update UI
    load_course_record();
}

function get_course_pdf_report()
{
    var course_pdf_report_status = document.getElementById("course_pdf_report_status");
    course_pdf_report_status.textContent = "產生報表中...";
    var query_date = convertToTimelineDate(glTargetDate);

    var json = {};
    json['project_id'] = ''+project_id;
    json['loadcoursepdfreport'] = 1;
    json['courseId'] = glCurrentCourse_id;
    json['date'] = query_date;

    var targetURL = gl_target_server + "/php/course.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      contentType: "application/json",
      dataType: "json",
      data: JSON.stringify(json),
      success:
        function(result)
        {
            //console.log("sync_ERP_account() response:"+response);

            //var result = JSON.parse(response);
            console.log("get_course_pdf_report() success:"+result.success+" pdfreport:"+result.pdfreport);

            var pdf_report_dl_link = document.getElementById("course_pdf_report_status");
            while (pdf_report_dl_link.firstChild)
            {
                pdf_report_dl_link.removeChild(pdf_report_dl_link.firstChild);
            }

            if ( result.success === 'true')
            {
                var dl_link = document.createElement("a");
                dl_link.setAttribute("href",result.pdfreport);
                dl_link.setAttribute("target","_blank");
                dl_link.textContent = "下載報表";
                pdf_report_dl_link.appendChild(dl_link);

                var imgPdf = document.createElement("i");
                imgPdf.setAttribute("class","fa fa-file-pdf-o");
                dl_link.appendChild(imgPdf);
            }
            else
            {
                var footer_btn_2_spen = document.getElementById("course_pdf_report_status");
                footer_btn_2_spen.textContent = "產生報表失敗!";
            }
        }
    });
    return false;
}

function move_daily_forward()
{
    var cur_date = moment(glTargetDate, "YYYYMMDD");
    var target_date = cur_date.add(1, 'day').format('YYYYMMDD');

    glTargetDate = target_date;
    //console.log("move_date_forward() GroupID:"+GroupID +", today:"+today +", target_date:"+target_date);

    var status_date = document.getElementById('daily_targetdate');
    if (status_date != undefined)
    {
        status_date.textContent = convertToTimelineDate(glTargetDate);
    }

    // update UI
    load_course_record();
}

function move_daily_backward()
{
    var cur_date = moment(glTargetDate, "YYYYMMDD");
    var target_date = cur_date.subtract(1, 'day').format('YYYYMMDD');

    glTargetDate = target_date;
    //console.log("move_date_forward() GroupID:"+GroupID +", today:"+today +", target_date:"+target_date);

    var status_date = document.getElementById('daily_targetdate');
    if (status_date != undefined)
    {
        status_date.textContent = convertToTimelineDate(glTargetDate);
    }

    // update UI
    load_course_record();
}

//Date picker
$('#daily_targetdate').datepicker({
  autoclose: true,
  format:"YYYY-MM-DD",
  language:"zh-TW"
}).on('changeDate', function (selected)
{
    var minDate = new Date(selected.date.valueOf());
    glTargetDate = moment(minDate).format('YYYYMMDD');
    //console.log("#daily_targetdate changeDate() target_date:"+target_date);
    var status_date = document.getElementById('daily_targetdate');
    if (status_date != undefined)
    {
        status_date.textContent = convertToTimelineDate(glTargetDate);
    }

    // update UI
    load_course_record();
});

// get today date
function getTodayDate()
{
    var today = moment().format('YYYYMMDD');
    return today;
}

function convertTargetDateTime(inDate)
{
    var cur_date = moment(inDate, "YYYYMMDD");
    var targetDate = cur_date.format('YYYY-MM-DD HH:mm:ss');
    return targetDate;
}

function convertToTimelineDate(inDate)
{
    var cur_date = moment(inDate, "YYYYMMDD");
    var outDate = cur_date.format('YYYY-MM-DD');

    return outDate;
}

function getCourseTargetDate(addDays)
{
    var targetDate = moment().add(addDays, 'd').format('YYYYMMDD');
    return targetDate;
}

function getCourseTargetDateV1(inDate, addDays)
{
    var cur_date = moment(inDate, "YYYYMMDD");
    var targetDate = cur_date.add(addDays, 'd').format('YYYYMMDD');
    return targetDate;
}

function getTargetDateTime(inDate, addDays)
{
    var cur_date = moment(inDate, "YYYY-MM-DD");
    //var fullDatetime = cur_date.format('YYYY-MM-DD HH:mm:ss');

    var targetDate = cur_date.add(addDays, 'd').format('YYYY-MM-DD HH:mm:ss');
    return targetDate;
}

function convertPrettyTime(inDate)
{
    if (inDate.length == 0)
        return "";

    var cur_date = moment(inDate, "HH:mm:ss");
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var perty_str = "";
    if (hour > 0)
        perty_str += hour + "小時";
    if (minutes > 0)
        perty_str += minutes + "分";
    if (seconds > 0)
        perty_str += seconds + "秒";

    return perty_str;
}

function convertPrettyCourseTime(inTime)
{
    if (inTime.length == 0)
        return "";

    // console.log("convertPrettyCourseTime() inTime:"+inTime);

    var cur_date = moment(inTime, "YYYY-MM-DD HH:mm:ss");
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var perty_str = "";
    // if (hour != NaN)
        perty_str += hour + "點";
    //if (minutes > 0)
        perty_str += minutes + "分";
    //if (seconds > 0)
        perty_str += seconds + "秒";

    return perty_str;
}

function open_footprintcourses_page()
{
    var win = window.open('/index.php/vilsfootprintcourses/'+project_id, '_blank');
    win.focus();
}
