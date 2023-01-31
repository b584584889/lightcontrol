/*
 * Author: ikki Chung
 * Date: 2019 09 10
 * Description:
 *      SMIMS VILS RTLS Solutions
**/


var glArea_List = [];

var LOAD_DEVICE_NUM = 20;
var RECORD_PAGE_COUNT = 5;

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;

var glCurrentArea_clicktime = new Date('2019/01/01');
var glCurrentArea_id = "";

var glTargetStartDate = null;
var glTargetEndDate = null;

var glCurrentArea_Records = [];
var glDataLoading = 0;


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
                load_maparea();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                click_area("");
                CurrentLoadIndex = 2; // next state
            }
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
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

function load_maparea()
{
    //
    // Get Area first
    //
    var targetURL = gl_target_server + "/php/map.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loadLocationArea=1', function(data)
    {
        var index = 0;
        $.each(data, function(itemindex, area)
        {
            var area_json = JSON.parse(area);

            var area = {};
            area.areaid = area_json.properties.areaid;
            area.areaname = area_json.properties.areaname;
            area.areatype = area_json.properties.areatype;

            glArea_List.push(area);

            console.log("areaid:"+area.areaid+", areaname:"+area.areaname+", areatype:"+area.areatype);

            index++;
        });
    })
    .success(function() {

        add_areas_to_table();

        CurrentLoadIndex = 1;
    })
    .error(function() {
        CurrentLoadIndex = 1;
    })
    .complete(function() {
        CurrentLoadIndex = 1;
    });
}

function add_areas_to_table()
{
    var area_list_content = document.getElementById('area_list_content');

    while (area_list_content.rows.length >= 1)
    {
        area_list_content.deleteRow(0);
    }

    console.log("add_areas_to_table() glArea_List.length:"+glArea_List.length);

    for (var i = 0; i < glArea_List.length; i++)
    {
        var area = glArea_List[i];

        var table_tr = document.createElement('tr');
        area_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        table_td_1.setAttribute("id","id_row_index_"+ area.areaid);
        //table_td_1.setAttribute("style","background-color:#CCEEFF;");
        table_td_1.setAttribute("onclick","click_area(\""+ area.areaid +"\");return false;");
        table_tr.appendChild(table_td_1);

        // areaname
        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_areaname_"+ area.areaid);
        table_td_2.setAttribute("onclick","click_area(\""+ area.areaid +"\");return false;");
        table_tr.appendChild(table_td_2);

        var label_title = document.createElement("span");
        label_title.setAttribute("style","display:inline;padding:.1em .6em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;background-color:#4b4");
        label_title.textContent = area.areaname;
        table_td_2.appendChild(label_title);

        // areatype
        var table_td_3 = document.createElement('td');
        table_td_3.setAttribute("id","id_row_areatype_"+ area.areaid);
        table_td_3.setAttribute("onclick","click_area(\""+ area.areaid +"\");return false;");
        //table_td_3.textContent = get_Areatype(area.areatype);
        table_tr.appendChild(table_td_3);

        var SpanType = document.createElement("span");
        SpanType.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
        if (area.areatype == '0')
            SpanType.setAttribute("class","badge bg-yellow-active");
        if (area.areatype == '1')
            SpanType.setAttribute("class","badge bg-fuchsia");
        if (area.areatype == '2')
            SpanType.setAttribute("class","badge bg-green-active");
        if (area.areatype == '3')
            SpanType.setAttribute("class","badge bg-purple");
        if (area.areatype == '4')
            SpanType.setAttribute("class","badge bg-teal-active");
        if (area.areatype == '5')
            SpanType.setAttribute("class","badge bg-green-active");
        if (area.areatype == '6')
            SpanType.setAttribute("class","badge bg-purple");

            // SpanType.setAttribute("class","badge bg-light-blue-active");
        SpanType.textContent = get_Areatype(area.areatype);
        table_td_3.appendChild(SpanType);

    }
}

function click_area(areaId)
{
    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentArea_clicktime.getTime());

    if (diffTimeInMS < 500 || glDataLoading == 1)
    {
        console.log("click_area() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glDataLoading = 1;
    glCurrentArea_clicktime = new Date();

    for (var i = 0; i < glArea_List.length; i++)
    {
        var area = glArea_List[i];

        // console.log("click_course() i:"+i +" courseId:"+courseId +" course.courseId:"+course.courseId +" glCurrentArea_id:"+glCurrentArea_id);
        var id_row_index = document.getElementById("id_row_index_"+ area.areaid);
        var id_row_areaname = document.getElementById("id_row_areaname_"+ area.areaid);
        var id_row_areatype = document.getElementById("id_row_areatype_"+ area.areaid);

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentArea_id.length == 0 || area.areaid === areaId )
        {
            glCurrentArea_id = area.areaid;
            bg_color = "background-color:#CCEEFF;";
            // console.log("click_course() i:"+i +" courseId:"+courseId +" course.courseId:"+course.courseId +" glCurrentArea_id:"+glCurrentArea_id);
        }
        id_row_index.setAttribute("style",bg_color);
        id_row_areaname.setAttribute("style",bg_color);
        id_row_areatype.setAttribute("style",bg_color);
    }

    load_area_record();
}

function load_area_record()
{
    console.log("load_area_record() glCurrentArea_id:" + glCurrentArea_id);

    var record_box_title = document.getElementById('record_box_title');
    if (record_box_title != undefined)
    {
        record_box_title.textContent = "Loading...";
    }

    glCurrentArea_Records = [];
    glTargetStartDate = null;
    glTargetEndDate = null;

    load_recent_record_express(glCurrentArea_id);

    clear_timeline();
}

function load_recent_record_express(areaId)
{
    if (glTargetStartDate == undefined)
    {
        glTargetStartDate = getAreaTargetDate(0);
    }

    console.log("load_recent_record_express() areaId:"+areaId+" glTargetStartDate:" + glTargetStartDate);

    var targetURL = gl_target_server + "/php/dashboarddata.php";
    $.getJSON(targetURL, 'loadrecentrollcallrecord=1&areaid='+areaId+'&recentdays='+RECORD_PAGE_COUNT+'&startdate='+glTargetStartDate+'&project_id='+project_id, function(data)
    {
        // console.log("load_recent_record_express() data.length:"+data.length +", data[0].day:"+data[0].day);

        glTargetEndDate = glTargetStartDate;
        var EndDateNum = convertTimelineDateToNumber(glTargetEndDate);

        for (var index in data)
        {
            var dateData = data[index];
            var issueday = dateData.issueday;

            var record = {};
            record.issueday = issueday;
            record.data = dateData.data;

            var d1 = convertTimelineDateToNumber(issueday);

            if (d1 < EndDateNum)
            {
                EndDateNum = d1;
                glTargetEndDate = issueday;
            }

            glCurrentArea_Records.push(record);
        }

        console.log("load_recent_record_express() length:"+glCurrentArea_Records.length+", glTargetStartDate:"+glTargetStartDate +", glTargetEndDate:"+glTargetEndDate);

        update_show_data()
    });

}

function update_show_data()
{
    show_data_on_timeline();

    glDataLoading = 0;

    var record_box_title = document.getElementById('record_box_title');
    if (record_box_title != undefined)
    {
        var chrAreaName = "";
        for (var i = 0; i < glArea_List.length; i++)
        {
            var area = glArea_List[i];

            if ( glCurrentArea_id === area.areaid )
            {
                chrAreaName = area.areaname;
            }
        }
        record_box_title.textContent = chrAreaName;
    }
}

function show_data_on_timeline()
{
    var main_timeline = document.getElementById('main_timeline');

    if(glCurrentArea_Records.length == 0)
    {
        var nodata_btn_ui = create_nodata_btn();
        main_timeline.appendChild(nodata_btn_ui);
        return;
    }

    for(var index in glCurrentArea_Records)
    {
        var areaDateData = glCurrentArea_Records[index];

        var time_label = create_time_label(areaDateData.issueday);
        main_timeline.appendChild(time_label);

        //console.log("show_data_on_timeline() date:"+courseDateData.date+", courseDateData.tags.length:"+courseDateData.tags.length);

        var uiid = "table_" + areaDateData.issueday;
        var area_rollcall_ui = create_area_rollcall_data_info(uiid, areaDateData.issueday, areaDateData);
        main_timeline.appendChild(area_rollcall_ui);

    }

    if(glCurrentArea_Records.length < RECORD_PAGE_COUNT)
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

function create_area_rollcall_data_info(uiid, date, areaDateData)
{
    var area_rollcall_ui = document.createElement("li");
    var img_icon = document.createElement("i");
    img_icon.setAttribute("class","fa fa-location-arrow bg-blue");
    area_rollcall_ui.appendChild(img_icon);

    var div_timeline_item = document.createElement("div");
    div_timeline_item.setAttribute("class","timeline-item");
    area_rollcall_ui.appendChild(div_timeline_item);

    var span_timeline_item_header = create_timeline_time_header("區域進出紀錄");
    div_timeline_item.appendChild(span_timeline_item_header);

    var span_timeline_item_body = create_timeline_time_body(uiid, "共 " + areaDateData.data.length +" 個人員");
    div_timeline_item.appendChild(span_timeline_item_body);

    var tag_badge = create_tag_badge(areaDateData.data);
    div_timeline_item.appendChild(tag_badge);

    var span_timeline_item_footer = create_timeline_time_footer(date, uiid);
    div_timeline_item.appendChild(span_timeline_item_footer);

    return area_rollcall_ui;
}

function reload_rollcall_info()
{
    glCurrentArea_Records = [];
    glTargetStartDate = getTargetDateTime( glTargetEndDate, -1);
    glTargetEndDate = getTargetDateTime( glTargetStartDate, RECORD_PAGE_COUNT);

    console.log("reload_rollcall_info() glCurrentArea_id:"+glCurrentArea_id+" glTargetStartDate:" + glTargetStartDate+" glTargetEndDate:" + glTargetEndDate);

    removeElement('reload_btn_id');

    load_recent_record_express(glCurrentArea_id);
}

function load_area_rollcall_detail(date, uiid)
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

    show_detail_rollcall(uiid, date);
}

function show_detail_rollcall(uiid, target_date)
{
    var query_date = convertToTimelineDate(target_date);
    console.log("show_detail_rollcall() uiid:"+uiid +", query_date:"+query_date);

    //
    // check if already existed
    //
    var table_id = "record_box_table_area_"+uiid;
    var table_ui = document.getElementById(table_id);
    if (table_ui != undefined)
    {
        console.log("show_detail_rollcall() table_id:"+table_id +", existed!");
        return;
    }

    for(var index in glCurrentArea_Records)
    {
        var areaDateData = glCurrentArea_Records[index];

        if( target_date === areaDateData.issueday)
        {
            put_record_to_table(uiid, glCurrentArea_id, areaDateData.data);
            break;
        }
    }
}

function put_record_to_table(uiid, areaid, records)
{
    var pre_record = null;

    var target_id = "timeline_footer_" + uiid;
    var footer_timeline = document.getElementById(target_id);

    var table_ui = create_record_table_rollcall(uiid);
    footer_timeline.appendChild(table_ui);

    var rows = records.length;
    var table_height = (rows + 1) * 50 + 50;
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
    table_td_entertime_span.textContent = convertPrettyRollCallTime(record.min_time);
    table_td_entertime_td.appendChild(table_td_entertime_span);
    cellIndex++;

    // Leave Time
    var table_td_leaveltime_td = document.createElement('td')
    row.appendChild(table_td_leaveltime_td);
    var table_td_leaveltime_span = document.createElement('span');
    //table_td_leaveltime_span.setAttribute("class",'badge bg-purple');
    table_td_leaveltime_span.setAttribute("class",'label label-info');
    table_td_leaveltime_span.textContent = convertPrettyRollCallTime(record.max_time);
    table_td_leaveltime_td.appendChild(table_td_leaveltime_span);
    cellIndex++;

    //var stayTime = moment.utc(moment(record.DataTime,"YYYY-MM-DD HH:mm:ss").diff(moment(record.DataTime,"YYYY-MM-DD HH:mm:ss"))).format("HH:mm:ss")

    // stay Time
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(convertPrettyTime(convertFormatTime(record.totaltime))));
    cellIndex++;

    return row;
}

function create_record_table_rollcall(uiid)
{
    var divTabPane = document.createElement('div');
    divTabPane.setAttribute("id","record_box_table_area_"+uiid);

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
    theadth_EnterTime.textContent = '最早進入時間';
    theadtrGroup.appendChild(theadth_EnterTime);

    var theadth_LeaveTime = document.createElement("th");
    theadth_LeaveTime.setAttribute("style","width: 120px; text-align:center");
    theadth_LeaveTime.textContent = '最晚離開時間';
    theadtrGroup.appendChild(theadth_LeaveTime);

    var theadth_StayTime = document.createElement("th");
    //theadth_StayTime.setAttribute("style","width: 50px; text-align:center");
    theadth_StayTime.textContent = '停留總時間';
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
    footer_btn_1.setAttribute("onclick","load_area_rollcall_detail(\""+date+"\",\""+uiid+"\");");
    footer_btn_1.setAttribute("id","timeline_footer_btn1_" + uiid);
    footer_btn_1.textContent = "查看詳細資料";
    div_timeline_item_footer.appendChild(footer_btn_1);

    var div_item_footer = document.createElement("div");
    div_item_footer.setAttribute("id","timeline_footer_" + uiid);
    div_timeline_item_footer.appendChild(div_item_footer);

    return div_timeline_item_footer;
}

function create_tag_badge(tags)
{
    var div_tag_badge = document.createElement("div");
    var table_tag_badge = document.createElement("table");
    div_tag_badge.appendChild(table_tag_badge);

    var tr_tag_badge = document.createElement("tr");
    table_tag_badge.appendChild(tr_tag_badge);

    var td_tag_badge = document.createElement("td");
    td_tag_badge.setAttribute("style","width:50px;text-align:center;");
    td_tag_badge.textContent = "人員:";
    tr_tag_badge.appendChild(td_tag_badge);

    var index = 0;
    for(index = 0;index<tags.length;index++)
    {
        var tag = tags[index];

        var td_tag_badge = document.createElement("td");
        //td_tag_badge.setAttribute("style","width:100px;text-align:center;");
        tr_tag_badge.appendChild(td_tag_badge);

        var tag_badge = document.createElement("span");
        //tag_badge.setAttribute("class","badge bg-purple");
        tag_badge.setAttribute("class","badge bg-green");
        tag_badge.textContent = tag.nodename;

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
    reload_btn.setAttribute("onclick","reload_rollcall_info();");
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
    if (hour > 0)
        perty_str += hour + "點";
    if (minutes > 0)
        perty_str += minutes + "分";
    if (seconds > 0)
        perty_str += seconds + "秒";

    return perty_str;
}

// get today date
function getTodayDate()
{
    var Today = new Date();
    today = ""+ Today.getFullYear() + (Today.getMonth()+1) + Today.getDate();

    //today = "20181218";
    return today;
}

function convertToTimelineDate(inDate)
{
    var cur_date = moment(inDate, "YYYYMMDD");
    var outDate = cur_date.format('YYYY-MM-DD');

    return outDate;
}

function convertTimelineDateToNumber(inDate)
{
    var cur_date = moment(inDate, "YYYY-MM-DD");
    var outDate = cur_date.format('YYYYMMDD');

    return Number(outDate);
}

function getAreaTargetDate(addDays)
{
    var targetDate = moment().add(addDays, 'd').format('YYYY-MM-DD');
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

function convertPrettyRollCallTime(inTime)
{
    var cur_date = moment(inTime, "HH:mm");
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var perty_str = "";
    if (hour >= 0)
        perty_str += hour + "點";
    if (minutes >= 0)
        perty_str += minutes + "分";

    return perty_str;
}

function convertFormatTime(seconds)
{
    const formatted = moment.utc(seconds*1000).format('HH:mm:ss');
    return formatted;
}

function get_Areatype(type)
{
    var area_type = "";
    if (type == '0')
        area_type = "普通";
    else
    if (type == '1')
        area_type = "電子圍欄";
    else
    if (type == '2')
        area_type = "電子點名";
    else
    if (type == '3')
        area_type = "房間";
    else
    if (type == '4')
        area_type = "教室";
    else
    if (type == '5')
        area_type = "病房";
    else
    if (type == '6')
        area_type = "病床";
    return area_type;
}

function open_footprintareas_page()
{
    var win = window.open('/index.php/vilsfootprintareas/'+project_id, '_blank');
    win.focus();
}
