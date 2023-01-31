/*
 * Author: ikki Chung
 * Created on: 2020/03/11
 * Description:
 *      SMIMS VILS RTLS Solutions event trigger
 **/

var glTags = [];
var glAnchors = [];

var gl_tag_num = 0;
var gl_anchor_num = 0;
var gl_locator_num = 0;

var gl_tag_load_index = 0;
var gl_anchor_load_index = 0;
var gl_locator_load_index = 0;

var LOAD_DEVICE_NUM = 20;

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;

var glInspectionGroups = [];
var glInspectionGroupDailyRecord = [];
var gl_inspectiongroup_num = 0;
var gl_inspectiongroup_load_index = 0;

var glInspectionPoint = [[]];
var gl_inspectionpointdaily_load_index = 0;
//var gl_TagBehavior_group_load_index = 0;
//var gl_TagBehavior_load_index = []; // for each group has different index

var gl_active_page_index = 1;// 1:daily 2:weekly 3:monthly
var glTargetDate = getTodayDate();

var glWeeklyStartDate = getTargetDate(-6);
var glWeeklyEndDate = getTargetDate(0);

var glMonthlyDate = getTargetMonth(0);
var glMonthlyStartDate = "";
var glMonthlyEndDate = "";

var glDailyTableHeader = [];
var glDailyTableData = [];
var glWeeklyTableHeader = [];
var glWeeklyTableData = [];
var glMonthlyTableHeader = [];
var glMonthlyTableData = [];

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
                load_devices();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_inspectiongroup_exist();
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_inspectiongroup_dailydata_all();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_inspectionpoint_dailydata_all();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                if (gl_active_page_index == 1)
                    show_daily_table();
                else
                if (gl_active_page_index == 2)
                    show_weekly_table();
                else
                if (gl_active_page_index == 3)
                    show_monthly_table();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                document.getElementById("INSPECTION_REPORT_TABLE_MSG").innerText = "";
                PreLoadIndex = CurrentLoadIndex;
            }
        break;
    }

    if ( CurrentLoadIndex < 7 )
        setTimeout(check_loading_status, 100);
    else
    {
        console.log("check_loading_status() Stop.");
    }
}

function load_inspectiongroup_exist()
{
    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectiongroup=1'+'&project_id='+project_id, function(data)
    {
        gl_inspectiongroup_num = 0;
        glInspectionGroups = [];
        var index = 1;
        $.each(data, function(checkpointGroupID, value)
        {
            glInspectionGroups[gl_inspectiongroup_num] = value;

            index++;
            gl_inspectiongroup_num++;

        });

        gl_inspectiongroup_load_index = 0;
        CurrentLoadIndex = CurrentLoadIndex + 1;
    });
    return false;
}

function load_inspectiongroup_dailydata_all()
{
    console.log("load_inspectiongroup_dailydata_all() gl_inspectiongroup_load_index:"+gl_inspectiongroup_load_index);

    if (gl_inspectiongroup_load_index >= gl_inspectiongroup_num)
    {
        console.log("load_inspectiongroup_dailydata_all() Finished!");

        gl_inspectionpointdaily_load_index = 0;
        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }

    var checkPointGroup = glInspectionGroups[gl_inspectiongroup_load_index];
    var GroupID = checkPointGroup['GroupID'];

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectiongroupdailyrecord=1&project_id='+project_id+'&GroupID='+GroupID+'&date='+glTargetDate, function(data)
    {
        glInspectionGroupDailyRecord[GroupID] = data;

    }).success(function()
    {
        var current_load_index = gl_inspectiongroup_load_index;

        gl_inspectiongroup_load_index++;

        setTimeout(load_inspectiongroup_dailydata_all, 10);
    })
    .error(function() {
    })
    .complete(function() {
    });

    return false;
}

function load_inspectionpoint_dailydata_all()
{
    console.log("load_inspectionpoint_dailydata_all() gl_inspectionpointdaily_load_index:"+gl_inspectionpointdaily_load_index);

    if (gl_inspectionpointdaily_load_index >= gl_inspectiongroup_num)
    {
        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }

    var inspectionGroup = glInspectionGroups[gl_inspectionpointdaily_load_index];
    var GroupID = inspectionGroup['GroupID'];

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectionpointdailyrecord=1&project_id='+project_id+'&GroupID='+GroupID+'&date='+glTargetDate, function(data)
    {
        glInspectionPoint[GroupID] = [];
        $.each(data, function(key, value)
        {
            glInspectionPoint[GroupID].push(value);
        });

        gl_inspectionpointdaily_load_index++;
    }).success(function()
    {
        setTimeout(load_inspectionpoint_dailydata_all, 10);
    })
    .error(function() {
    })
    .complete(function() {
    });
    return false;
}

function load_devices()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_anchor_num = parseInt(data['LOC_ANCHOR_NUM']);
        gl_locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        gl_tag_load_index = 0;
        gl_anchor_load_index = 0;
        gl_locator_load_index = 0;
    })
    .success(function() {
        setTimeout(load_tags, 10);
    })
    .error(function() {
        setTimeout(load_devices, 1000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_tags()
{
    if (gl_tag_load_index >= gl_tag_num)
    {
        gl_tag_load_index = 0;
        setTimeout(load_anchors, 10);
        return;
    }
    if (gl_tag_load_index == 0)
    {
        glTags = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadtag=1&s='+gl_tag_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var tag_num = parseInt(data['LOC_TAG_NUM']);

        if (gl_tag_load_index == 0 && tag_num == 0)
        {
            gl_tag_num = 0;
            gl_tag_load_index = 0;
        }

        for (var i = gl_tag_load_index; i < gl_tag_load_index+tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            var node = data[key];
            if (node == undefined)
                continue;
            var nodeData = node.split(",");

            // Tag role 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
            var tafid = nodeData[4];

            if (tafid === '1')
                glTags.push(nodeData);
        }
        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            gl_tag_load_index = 0;
            setTimeout(load_anchors, 10);
        }
        else
        {
            setTimeout(load_tags, 10);
        }
    })
    .error(function() {
        gl_tag_load_index = 0;
        setTimeout(load_tags, 1000);
    })
    .complete(function() {
    });
}

function load_anchors()
{
    if (gl_anchor_load_index >= gl_anchor_num)
    {
        gl_anchor_load_index = 0;
        setTimeout(load_locators, 10);
        return;
    }
    if (gl_anchor_load_index == 0)
    {
        glAnchors = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadanchor=1&s='+gl_anchor_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var anchor_num = parseInt(data['LOC_ANCHOR_NUM']);

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_anchor_load_index; i < gl_anchor_load_index+anchor_num; ++i)
        {
            var key = 'LOC_ANCHOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            // split data
            var node = data[key];
            var nodeData = node.split(",");

            glAnchors[i] = {};
            glAnchors[i].itemid = nodeData[3];
            glAnchors[i].itemname = nodeData[5];
            glAnchors[i].posX = nodeData[7];
            glAnchors[i].posY = nodeData[8];
            glAnchors[i].posZ = nodeData[9];
            glAnchors[i].type = "anchor";
        }

        gl_anchor_load_index += anchor_num;

    })
    .success(function() {
        if (gl_anchor_load_index >= gl_anchor_num)
        {
            gl_anchor_load_index = 0;
            setTimeout(load_locators, 10);
        }
        else
        {
            load_anchors();
        }
    })
    .error(function() {
        gl_anchor_load_index = 0;
        setTimeout(load_locators, 10);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);

}

function load_locators()
{
    if (gl_locator_load_index >= gl_locator_num)
    {
        gl_locator_load_index = 0;

        CurrentLoadIndex = CurrentLoadIndex + 1;
        return;
    }
    if (gl_locator_load_index == 0)
    {
        glLocator_List = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadlocator=1&s='+gl_locator_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);
        //var anchor_size = glAnchors.length;
        var anchor_size = gl_anchor_num;
        for (var i = gl_locator_load_index; i < gl_locator_load_index+locator_num; ++i)
        {
            var key = 'LOC_LOCATOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            // split data
            var node = data[key];
            var nodeData = node.split(",");

            glAnchors[anchor_size + i] = {};
            glAnchors[anchor_size + i].itemid = nodeData[3];
            glAnchors[anchor_size + i].itemname = nodeData[5];
            glAnchors[anchor_size + i].posX = nodeData[7];
            glAnchors[anchor_size + i].posY = nodeData[8];
            glAnchors[anchor_size + i].posZ = nodeData[9];
            glAnchors[anchor_size + i].type = "locator";
        }

        gl_locator_load_index += locator_num;

    })
    .success(function() {
        if (gl_locator_load_index >= gl_locator_num)
        {
            gl_locator_load_index = 0;

            CurrentLoadIndex = CurrentLoadIndex + 1;
        }
        else
        {
            load_locators();
        }

    })
    .error(function() {
        gl_locator_load_index = 0;
        CurrentLoadIndex = CurrentLoadIndex + 1;
        setTimeout(load_locators, 1000);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function fetch_inspector_score(prefix, inspector, inspectorname, startDate, endDate)
{
    // console.log("fetch_inspector_score() inspector:"+inspector+" startDate:"+startDate+" endDate:"+endDate);

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectorscore=1&project_id='+project_id+'&inspector='+inspector+'&startdate='+startDate+'&enddate='+endDate, function(data)
    {
        var all_count = 0;
        var all_score = 0;
        var target_data = {};

        // if ( prefix === 'weekly' )
        // {
        //     glWeeklyTableData[inspector] = {};
        //     // target_data = glWeeklyTableData[inspector];
        // }
        // else
        // if ( prefix === 'monthly' )
        // {
        //     glMonthlyTableData[inspector] = {};
        //     // target_data = glMonthlyTableData[inspector];
        // }

        target_data['巡檢員'] = inspectorname;

        $.each(data, function(targetdate, value)
        {
            var date_uiid = prefix+"_table_row_"+ inspector+"_"+targetdate;
            var date_ui = document.getElementById(date_uiid);

            var score_total = parseFloat(value.score_total);
            var count = parseFloat(value.count);

            if (value.count === 0)
            {
                date_ui.textContent = "--";
            }
            else
            {
                var score = score_total/count;
                all_score += score;
                all_count += 1;

                date_ui.textContent = score.toFixed(1);
            }

            if ( prefix === 'weekly' )
            {
                var cur_date = moment(targetdate, "YYYYMMDD").format('MM/DD');
                target_data[cur_date] = date_ui.textContent;
            }
            else
            if ( prefix === 'monthly' )
            {
                var cur_date = moment(targetdate, "YYYYMMDD").format('DD');
                target_data[cur_date] = date_ui.textContent;
            }
        });

        if (all_count != 0)
        {
            var average_score = all_score / all_count;
            var score_uiid = prefix+"_table_row_"+ inspector+"_score";
            var average_score_ui = document.getElementById(score_uiid);
            average_score_ui.textContent = average_score.toFixed(1);

            target_data['平均分'] = average_score_ui.textContent;
        }
        else
        {
            target_data['平均分'] = "--";
        }

        if ( prefix === 'weekly' )
        {
            glWeeklyTableData.push(target_data);
        }
        else
        if ( prefix === 'monthly' )
        {
            glMonthlyTableData.push(target_data);
        }

    }).success(function()
    {
    })
    .error(function() {
    })
    .complete(function() {
    });
}


function show_daily_table()
{
    var status_date = document.getElementById('daily_targetdate');
    if (status_date != undefined)
    {
        status_date.textContent = convertToShowDate(glTargetDate);
    }

    var table_header = document.getElementById('DAILY_REPORT_TABLE_HEADER');
    while (table_header.firstChild)
    {
        table_header.removeChild(table_header.firstChild);
    }

    glDailyTableHeader = [];
    glDailyTableHeader.push("巡檢員");
    var th = create_table_header_column("巡檢員", "#223344", "#223344", "#cce6ff");
    table_header.appendChild(th);

    for(var index=0; index<gl_inspectiongroup_num; index++)
    {
        var InspectionGroup = glInspectionGroups[index];

        glDailyTableHeader.push(InspectionGroup['GroupName']);
        var th = create_table_header_column(InspectionGroup['GroupName'], "#cccccc", "#cccccc", "#223344");
        table_header.appendChild(th);
    }

    glDailyTableHeader.push("平均分");
    var th = create_table_header_column("平均分", "#223344", "#223344", "#ff9933");
    table_header.appendChild(th);


    var table_body = document.getElementById('DAILY_REPORT_TABLE');
    while (table_body.firstChild)
    {
        table_body.removeChild(table_body.firstChild);
    }

    glDailyTableData = [];

    for (var i = 0; i < glTags.length; i++)
    {
        var nodeData = glTags[i];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];

        var bodytr = document.createElement("tr");
        table_body.appendChild(bodytr);

        //glDailyTableData[nodename] = {};
        //var target_data = glDailyTableData[nodename];
        var target_data = {};
        target_data['巡檢員'] = nodename;

        var col = create_table_body_column(nodename, "#223344", "#223344", "#fefefe");
        bodytr.appendChild(col);

        var total_count = 0;
        var total_score = 0;

        for(var index=0; index<gl_inspectiongroup_num; index++)
        {
            var InspectionGroup = glInspectionGroups[index];
            var GroupID = InspectionGroup['GroupID'];
            var GroupName = InspectionGroup['GroupName'];
            var InspectionGroupDailyRecord = glInspectionGroupDailyRecord[GroupID];
            var InspectionPointList = glInspectionPoint[GroupID];
            var Inspectors = InspectionGroup['Inspectors'].split(",");

            var isInspector = Inspectors.indexOf(nodeid);

            target_data[GroupName] = [];

            if (InspectionPointList != undefined && InspectionPointList.length > 0)
            {
                var InspectionPoint = InspectionPointList[0];
                var score = InspectionGroupDailyRecord['Score'];
                var Inspector = InspectionPoint['Inspector'];

                if ( nodeid === Inspector )
                {
                    total_score += parseFloat(score);
                    total_count++;

                    target_data[GroupName] = score;
                    var col = create_table_body_column(score, "#00aa00", "#223344", "#fefefe");
                    bodytr.appendChild(col);
                }
                else
                if ( isInspector >= 0 )
                {
                    target_data[GroupName] = "--";
                    var col = create_table_body_column("--", "#223344", "#223344", "#eeeeee");
                    bodytr.appendChild(col);
                }
                else
                {
                    target_data[GroupName] = "N/A";
                    var col = create_table_body_column("N/A", "#223344", "#223344", "#eeeeee");
                    bodytr.appendChild(col);
                }
            }
        }

        if(total_count > 0)
        {
            target_data['平均分'] = ""+(total_score/total_count);
            //glDailyTableData[nodename].push(""+(total_score/total_count));
            var col = create_table_body_column(total_score/total_count, "#00aa00", "#223344", "#fefefe");
            bodytr.appendChild(col);
        }
        else
        {
            target_data['平均分'] = "N/A";
            //glDailyTableData[nodename].push("N/A");
            var col = create_table_body_column("N/A", "#00aa00", "#223344", "#fefefe");
            bodytr.appendChild(col);
        }

        glDailyTableData.push(target_data);
    }
}

function create_table_header_column(text_content, text_color, border_color, bg_color)
{
    var theadth = document.createElement("th");
    theadth.setAttribute("bgcolor", bg_color);
    //theadtr.setAttribute("style","width: 50px");
    theadth.setAttribute("style","border-color:"+border_color+";color:"+text_color);
    theadth.textContent = text_content;
    return theadth;
}

function create_table_body_column(text_content, text_color, border_color, bg_color)
{
    var bodycol = document.createElement("td");
    bodycol.setAttribute("bgcolor", bg_color);
    //bodycol.setAttribute("style","width: 50px");
    bodycol.setAttribute("style","border-color:"+border_color+";color:"+text_color);
    bodycol.textContent = text_content;
    return bodycol;
}

function show_weekly_table()
{
    var status_date = document.getElementById("weekly_targetdate");
    if (status_date != undefined)
    {
        status_date.textContent = convertToShowDate(glWeeklyEndDate) +" - "+ convertToShowDate(glWeeklyStartDate);
    }

    var table_header = document.getElementById('WEEKLY_REPORT_TABLE_HEADER');
    while (table_header.firstChild)
    {
        table_header.removeChild(table_header.firstChild);
    }

    glWeeklyTableHeader = [];

    glWeeklyTableHeader.push("巡檢員");
    var th = create_table_header_column("巡檢員", "#223344", "#223344", "#cce6ff");
    table_header.appendChild(th);

    glWeeklyTableHeader.push("平均分");
    var th = create_table_header_column("平均分", "#223344", "#223344", "#ff9933");
    table_header.appendChild(th);

    for(var index=0; index<7; index++)
    {
        var start_date = moment(glWeeklyStartDate, "YYYYMMDD");
        var show_date = start_date.add(index, 'day').format('MM/DD');

        glWeeklyTableHeader.push(show_date);
        var th = create_table_header_column(show_date, "#cccccc", "#cccccc", "#223344");
        table_header.appendChild(th);
    }

    var table_body = document.getElementById('WEEKLY_REPORT_TABLE');
    while (table_body.firstChild)
    {
        table_body.removeChild(table_body.firstChild);
    }

    glWeeklyTableData = [];

    for (var i = 0; i < glTags.length; i++)
    {
        var nodeData = glTags[i];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];

        var bodytr = document.createElement("tr");
        bodytr.setAttribute("id","weekly_table_row_"+ nodeid);
        table_body.appendChild(bodytr);

        var col = create_table_body_column(nodename, "#223344", "#223344", "#fefefe");
        bodytr.appendChild(col);

        var col = create_table_body_column("--", "#00aa00", "#223344", "#fefefe");
        col.setAttribute("id","weekly_table_row_"+ nodeid+"_score");
        bodytr.appendChild(col);

        for(var index=0; index<7; index++)
        {
            var start_date = moment(glWeeklyStartDate, "YYYYMMDD");
            var show_date = start_date.add(index, 'day').format('YYYYMMDD');

            var col = create_table_body_column("--", "#00aa00", "#223344", "#fefefe");
            col.setAttribute("id","weekly_table_row_"+ nodeid+"_"+show_date);
            bodytr.appendChild(col);
        }

        fetch_inspector_score("weekly", nodeid, nodename, glWeeklyStartDate, glWeeklyEndDate);
    }
}

function show_monthly_table()
{
    var status_date = document.getElementById("monthly_targetdate");
    if (status_date != undefined)
    {
        status_date.textContent = convertToShowMonth(glMonthlyDate);
    }

    var table_header = document.getElementById('MONTHLY_REPORT_TABLE_HEADER');
    while (table_header.firstChild)
    {
        table_header.removeChild(table_header.firstChild);
    }

    glMonthlyTableHeader = [];

    glMonthlyTableHeader.push("巡檢員");
    var th = create_table_header_column("巡檢員", "#223344", "#223344", "#cce6ff");
    table_header.appendChild(th);

    glMonthlyTableHeader.push("平均分");
    var th = create_table_header_column("平均分", "#223344", "#223344", "#ff9933");
    table_header.appendChild(th);

    var target_month = moment(glMonthlyDate, "YYYYMM");
    var target_MonthStart = moment(target_month).startOf('month');
    var target_MonthEnd = moment(target_month).endOf('month');

    glMonthlyStartDate = moment(target_MonthStart).format('YYYYMMDD');
    glMonthlyEndDate = moment(target_MonthEnd).format('YYYYMMDD');

    // console.log("target_MonthStart:"+target_MonthStart+" target_MonthEnd:"+target_MonthEnd);

    var index = 1;
    for(;target_MonthStart <= target_MonthEnd;)
    {
        var show_date = moment(target_MonthStart).format('DD');
        target_MonthStart = moment(target_MonthStart).add(1, 'day');

        // console.log("show_date:"+show_date);

        glMonthlyTableHeader.push(show_date);
        var th = create_table_header_column(show_date, "#cccccc", "#cccccc", "#223344");
        table_header.appendChild(th);

        index++;
        if (index >= 40)
            break;
    }

    var table_body = document.getElementById('MONTHLY_REPORT_TABLE');
    while (table_body.firstChild)
    {
        table_body.removeChild(table_body.firstChild);
    }

    glMonthlyTableData = [];

    for (var i = 0; i < glTags.length; i++)
    {
        var nodeData = glTags[i];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];

        var bodytr = document.createElement("tr");
        bodytr.setAttribute("id","monthly_table_row_"+ nodeid);
        table_body.appendChild(bodytr);

        var col = create_table_body_column(nodename, "#223344", "#223344", "#fefefe");
        bodytr.appendChild(col);

        var col = create_table_body_column("--", "#00aa00", "#223344", "#fefefe");
        col.setAttribute("id","monthly_table_row_"+ nodeid+"_score");
        bodytr.appendChild(col);

        var target_month = moment(glMonthlyDate, "YYYYMM");
        var target_MonthStart = moment(target_month).startOf('month');
        var target_MonthEnd = moment(target_month).endOf('month');

        var index = 1;
        for(;target_MonthStart <= target_MonthEnd;)
        {
            var show_date = moment(target_MonthStart).format('YYYYMMDD');
            target_MonthStart = moment(target_MonthStart).add(1, 'day');

            //var start_date = moment(glWeeklyStartDate, "YYYYMMDD");
            //var show_date = start_date.add(index, 'day').format('YYYYMMDD');

            var col = create_table_body_column("--", "#00aa00", "#223344", "#fefefe");
            col.setAttribute("id","monthly_table_row_"+ nodeid+"_"+show_date);
            bodytr.appendChild(col);

            index++;
            if (index >= 40)
                break;
        }

        var target_month = moment(glMonthlyDate, "YYYYMM");
        var target_MonthStart = moment(target_month).startOf('month');
        var target_MonthEnd = moment(target_month).endOf('month');
        fetch_inspector_score("monthly", nodeid, nodename, target_MonthStart.format('YYYYMMDD'), target_MonthEnd.format('YYYYMMDD'));
    }

}

function tab_daily_onclick()
{
    console.log("tab_daily_onclick()");

    gl_active_page_index = 1;
    show_daily_table();
}

function tab_weekly_onclick()
{
    console.log("tab_weekly_onclick()");

    gl_active_page_index = 2;
    show_weekly_table();
}

function tab_monthly_onclick()
{
    console.log("tab_monthly_onclick()");

    gl_active_page_index = 3;
    show_monthly_table();
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
        status_date.textContent = convertToShowDate(glTargetDate);
    }

    gl_inspectiongroup_load_index = 0;
    CurrentLoadIndex = 2;
    check_loading_status();
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
        status_date.textContent = convertToShowDate(glTargetDate);
    }

    gl_inspectiongroup_load_index = 0;
    CurrentLoadIndex = 2;
    check_loading_status();
}

function move_weekly_forward()
{
    var end_date = moment(glWeeklyEndDate, "YYYYMMDD");
    var start_date = moment(glWeeklyStartDate, "YYYYMMDD");
    var target_end_date = end_date.add(7, 'day').format('YYYYMMDD');
    var target_start_date = start_date.add(7, 'day').format('YYYYMMDD');

    glWeeklyEndDate = target_end_date;
    glWeeklyStartDate = target_start_date;

    show_weekly_table();
}

function move_weekly_backward()
{
    var end_date = moment(glWeeklyEndDate, "YYYYMMDD");
    var start_date = moment(glWeeklyStartDate, "YYYYMMDD");
    var target_end_date = end_date.subtract(7, 'day').format('YYYYMMDD');
    var target_start_date = start_date.subtract(7, 'day').format('YYYYMMDD');

    glWeeklyEndDate = target_end_date;
    glWeeklyStartDate = target_start_date;

    show_weekly_table();
}

function move_monthly_forward()
{
    var month_date = moment(glMonthlyDate, "YYYYMM");
    var target_month_date = month_date.add(1, 'month').format('YYYYMM');

    glMonthlyDate = target_month_date;

    show_monthly_table();
}

function move_monthly_backward()
{
    var month_date = moment(glMonthlyDate, "YYYYMM");
    var target_month_date = month_date.subtract(1, 'month').format('YYYYMM');

    glMonthlyDate = target_month_date;

    show_monthly_table();
}

function get_pdf_report(type, TableHeader, TableData, StartDate, EndDate)
{
    var pdf_report_dl_link = document.getElementById(type+"_pdf_report_status");
    pdf_report_dl_link.textContent = "產生報表中...";

    var json = {};
    json['project_id'] = ''+project_id;
    json['pdfreport'] = 1;
    json['startdate'] = StartDate;
    json['enddate'] = EndDate;
    json['type'] = type;
    json['header'] = TableHeader;
    json['data'] = {};

    for (var i in TableData)
    {
        json['data'][i] = TableData[i];
    }

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      contentType: "application/json",
      dataType: "json",
      data: JSON.stringify(json),
      success:
        function(result)
        {
            //console.log("get_pdf_report() response:"+response);

            //var result = JSON.parse(response);
            //console.log("get_pdf_report() success:"+result.success+" pdfreport:"+result.pdfreport);

            var pdf_report_dl_link = document.getElementById(type+"_pdf_report_status");
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
                pdf_report_dl_link.textContent = "產生報表失敗!";
            }
        }
    });
    return false;
}

function get_daily_pdf_report()
{
    get_pdf_report("daily", glDailyTableHeader, glDailyTableData, glTargetDate, glTargetDate);
}

function get_weekly_pdf_report()
{
    get_pdf_report("weekly", glWeeklyTableHeader, glWeeklyTableData, glWeeklyStartDate, glWeeklyEndDate);
}

function get_monthly_pdf_report()
{
    get_pdf_report("monthly", glMonthlyTableHeader, glMonthlyTableData, glMonthlyStartDate, glMonthlyEndDate);
}

function getNodeName(NodeId)
{
    var NodeName = NodeId;

    for (var i = 0; i < glAnchors.length; i++)
    {
        if ( glAnchors[i].itemid == NodeId)
        {
            NodeName = glAnchors[i].itemname;
            break;
        }
    }

    return NodeName;
}

function getInspectorName(InspectorId)
{
    var NodeName = InspectorId;

    for (var i = 0; i < glTags.length; i++)
    {
        var nodeData = glTags[i];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];

        if ( nodeid === InspectorId)
        {
            NodeName = nodename;
            break;
        }
    }

    return NodeName;
}

function isANumber(str)
{
    return !/\D/.test(str);
}

// get today date
function getTodayDate()
{
    var Today = moment(new Date());
    //var today = ""+ Today.getFullYear() + (Today.getMonth()+1) + Today.getDate();
    var today = Today.format('YYYYMMDD');

    //today = "20181218";
    return today;
}

function getTargetDate(addDays)
{
    var targetDate = moment().add(addDays, 'd').format('YYYYMMDD');
    return targetDate;
}

function getTargetMonth(addMonths)
{
    var targetDate = moment().add(addMonths, 'm').format('YYYYMM');
    return targetDate;
}

function convertToShowDate(inDate)
{
    var cur_date = moment(inDate, "YYYYMMDD");
    var outDate = cur_date.format('YYYY/MM/DD');

    return outDate;
}

function convertToShowMonth(inMonth)
{
    var cur_date = moment(inMonth, "YYYYMM");
    var outDate = cur_date.format('YYYY/MM');

    return outDate;
}

function convertRecordTime(inDate)
{
    var cur_date = moment(inDate, "YYYY-MM-DD HH:mm:ss");
    var outDate = cur_date.format('HH:mm:ss');

    return outDate;
}

function open_inspection_report_page()
{
    var win = window.open('/index.php/vilsinspectionreport/'+project_id, '_blank');
    win.focus();
}
