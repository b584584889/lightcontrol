/*
 * Author: ikki Chung
 * Date: 01 Nov 2018
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
var glInspectionGroupPath = [[]];
var gl_inspectiongroup_num = 0;
var gl_inspectiongroup_load_index = 0;
var gl_inspectionpoint_load_index = 0;

var gl_active_group_index = 1;
var glTargetStartDate = []; // GroupID as Index
var glTargetEndDate = []; // GroupID as Index

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

                load_inspectionpoint_exist();
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                var inspectionGroup = glInspectionGroups[gl_inspectiongroup_load_index];

                if ( inspectionGroup == undefined )
                {
                    return;
                }

                load_inspectiongroup_dailydata(gl_inspectiongroup_load_index);
                CurrentLoadIndex = 4;
            }
        break;
    }

    if ( CurrentLoadIndex < 4 )
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
        var inspectiongroup_nav_tabs = document.getElementById('inspectiongroup_nav_tabs');
        while (inspectiongroup_nav_tabs.firstChild)
        {
            inspectiongroup_nav_tabs.removeChild(inspectiongroup_nav_tabs.firstChild);
        }

        gl_inspectiongroup_num = 0;
        glInspectionGroups = [];
        var index = 1;
        $.each(data, function(GroupID, value)
        {
            add_exist_inspectiongroup(index, GroupID, gl_inspectiongroup_num, value);

            //
            // 建立table
            //
            var inspectionpoint_table = document.getElementById('inspectionpoint_table_'+GroupID);
            if (inspectionpoint_table == undefined)
            {
                console.log("load_inspectiongroup_dailydata() inspectionpoint_table == undefined");
                create_inspectionpoint_table(GroupID, value, gl_inspectiongroup_num);

                var startdate = glTargetStartDate[GroupID];
                var enddate = glTargetEndDate[GroupID];

                var group_load_index = gl_inspectiongroup_num;
                //Date picker
                $('#Group_Status_Page_'+GroupID).daterangepicker({
                    autoApply: true,
                    manualInputs: false, //SMIMS ikki add
                    startDate: startdate,
                    endDate: enddate,
                    locale: {
                            format: 'YYYY/MM/DD'
                        }

                },
                function(start, end)
                {
                    //console.log("A new date selection was made: " + start.format('YYYY-MM-DD') + ' to ' + end.format('YYYY-MM-DD') );

                    glTargetStartDate[GroupID] = start.format('YYYYMMDD');
                    glTargetEndDate[GroupID] = end.format('YYYYMMDD');

                   load_inspectiongroup_dailydata(group_load_index);

                });

            }
            else
            {
                console.log("load_inspectiongroup_dailydata() inspectionpoint_table existed!");
            }


            glInspectionGroups[gl_inspectiongroup_num] = value;

            if (glTargetStartDate[GroupID] == undefined)
            {
                glTargetStartDate[GroupID] = getTargetDate(-30);
            }
            if (glTargetEndDate[GroupID] == undefined)
            {
                glTargetEndDate[GroupID] = getTargetDate(0);
            }

            console.log("load_inspectiongroup_exist() glTargetStartDate["+GroupID+"]:"+glTargetStartDate[GroupID] +", glTargetEndDate["+GroupID+"]:"+glTargetEndDate[GroupID]);

            index++;
            gl_inspectiongroup_num++;

        });

        gl_inspectiongroup_load_index = 0;
        CurrentLoadIndex = 2;
    });
    return false;
}

function load_inspectiongroup_dailydata(Group_Load_Index)
{
    console.log("load_inspectiongroup_dailydata() Group_Load_Index:"+Group_Load_Index);

    var inspectionGroup = glInspectionGroups[Group_Load_Index];
    if ( inspectionGroup == undefined )
    {
        console.log("load_inspectiongroup_dailydata() inspectionGroup == undefined");
        return;
    }

    var GroupID = inspectionGroup['GroupID'];
    var startdate = glTargetStartDate[GroupID];
    var enddate = glTargetEndDate[GroupID];

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectiongrouphistory=1&project_id='+project_id+'&GroupID='+GroupID+'&startdate='+startdate+'&enddate='+enddate, function(data)
    {
        //
        // clear old table
        //
        var checkpointGroup_table_body = document.getElementById('inspectionpoint_table_body_'+GroupID);
        if (checkpointGroup_table_body != undefined)
        {
            while (checkpointGroup_table_body.firstChild)
            {
                checkpointGroup_table_body.removeChild(checkpointGroup_table_body.firstChild);
            }
        }
        else
        {
            console.log("load_inspectiongroup_dailydata() checkpointGroup_table_body == undefined");
        }

        var index = 1;
        $.each(data, function(key, value)
        {
            add_history_row(GroupID, index, key, value);
            index++;
        });

        var Group_Status_Page = document.getElementById("Group_Status_Page_"+GroupID);
        if (Group_Status_Page != undefined)
        {
            Group_Status_Page.textContent = convertToShowDate(startdate) +" - "+ convertToShowDate(enddate);
        }

    }).success(function()
    {
        load_inspectionpoint_dailydata(Group_Load_Index);
    })
    .error(function() {
    })
    .complete(function() {
    });

    return false;
}

function load_inspectionpoint_exist()
{
    if (gl_inspectionpoint_load_index >= gl_inspectiongroup_num)
    {
        console.log("load_inspectionpoint_exist() Finished!");

        CurrentLoadIndex = 3;
        return;
    }

    var inspectionGroup = glInspectionGroups[gl_inspectionpoint_load_index];
    var GroupID = inspectionGroup['GroupID'];

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectionpoint=1&project_id='+project_id+'&GroupID='+GroupID, function(data)
    {
        var index = 1;
        var PathCount = Object.keys(data).length;
        glInspectionGroupPath[GroupID] = [];

        $.each(data, function(key, value)
        {
            if ( index > 1 )
            {
                // 巡檢路線 資訊
                add_inspectionpoint_table_head_traveltime(GroupID, value);
            }

            // 巡檢點 資訊
            add_inspectionpoint_table_head_point(GroupID, value);

            index++;

            /*
            // 巡檢點 資訊
            add_inspectionpoint_table_head_point(GroupID, value, false);
            // 巡檢路線 資訊
            add_inspectionpoint_table_head_traveltime(GroupID, value, false);
            index++;
            if (index == PathCount + 1)
            {
                add_inspectionpoint_table_head_point(GroupID, value, true);
            }
            */

            glInspectionGroupPath[GroupID].push(value);
        });

        gl_inspectionpoint_load_index++;

    }).success(function() {
        setTimeout(load_inspectionpoint_exist, 10);
    })
    .error(function() {
    })
    .complete(function() {
    });
    return false;
}

function load_inspectionpoint_dailydata(Group_Load_Index)
{
    console.log("load_inspectionpoint_dailydata() Group_Load_Index:"+Group_Load_Index);

    var inspectionGroup = glInspectionGroups[Group_Load_Index];
    if ( inspectionGroup == undefined )
    {
        console.log("load_inspectionpoint_dailydata() inspectionGroup == undefined");
        return;
    }

    var GroupID = inspectionGroup['GroupID'];
    var startdate = glTargetStartDate[GroupID];
    var enddate = glTargetEndDate[GroupID];

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectionpointdailyhistory=1&project_id='+project_id+'&GroupID='+GroupID+'&startdate='+startdate+'&enddate='+enddate, function(data)
    {

        console.log("load_inspectionpoint_dailydata() data.length:"+data.length);

        var dailydata = [];

        //
        // group by date
        //
        $.each(data, function(key, value)
        {
            var datetime = value['DateTime'];
            //console.log("load_inspectionpoint_dailydata() GroupID:"+GroupID+",key:"+key+", datetime:"+datetime);
            if ( dailydata[datetime] == undefined )
                dailydata[datetime] = [];

            dailydata[datetime].push(value);
        });

        // each day
        for (var key in dailydata)
        {
            //console.log("load_inspectionpoint_dailydata() key:"+key +", length:"+dailydata[key].length);

            update_table_dailydata(GroupID, key, dailydata[key]);
        }

    }).success(function()
    {

    })
    .error(function() {
    })
    .complete(function() {
    });

    return false;
}

function update_table_dailydata(GroupID, date, dailydata)
{
    var table_row_date = document.getElementById("table_row_"+ GroupID+"_"+date);
    if ( table_row_date == undefined)
    {
        return;
    }
    var index = 1;

    // each path
    for (var idx in dailydata)
    {
        var daydata = dailydata[idx];
        console.log("load_inspectionpoint_dailydata() daydata index:"+index+", PointID:"+daydata['PointID']+", Checked:"+daydata['Checked']+", Finished:"+daydata['Finished']+", Exceed:"+daydata['Exceed']);

        var curPath = getCheckPointPath(GroupID, daydata['PointID']);
        if ( curPath == undefined)
        {
            continue;
        }

        var AnchorNodeId = "AnchorNodeId_"+GroupID+"_"+curPath['PointIndex']+"_"+curPath['AnchorNodeId'];
        //var EndAnchorNodeId = "AnchorNodeId_"+GroupID+"_"+curPath['PointIndex']+"_"+curPath['EndNodeId'];
        var PathId = "PATH_"+GroupID+"_"+curPath['PointIndex'];

        var span_AnchorNodeId = document.getElementById(AnchorNodeId);
        //var span_EndAnchorNodeId = document.getElementById(EndAnchorNodeId);
        var span_Path = document.getElementById(PathId);


        if (index > 1)
        {
            if ( span_Path != undefined)
            {
                console.log("load_inspectionpoint_dailydata() PathId:"+PathId);
                //console.log("load_inspectionpoint_dailydata() span_EndAnchorNodeId == undefined");
                var Path_UI = "PathStatus_"+GroupID+"_"+curPath['PointIndex']+"_"+date;
                var ui_existed = document.getElementById(Path_UI);
                if (ui_existed == undefined)
                {
                    var td = document.createElement('td');
                    td.setAttribute("align","center");
                    table_row_date.appendChild(td);

                    var span_Status = getStatusPath(daydata);
                    span_Status.setAttribute("id", Path_UI);
                    td.appendChild(span_Status);
                }
            }
        }

        if ( span_AnchorNodeId != undefined)
        {
            //console.log("load_inspectionpoint_dailydata() StartAnchorNodeId:"+StartAnchorNodeId);
            //console.log("load_inspectionpoint_dailydata() span_StartAnchorNodeId == undefined");

            var Inspector_UI = "Inspector_"+GroupID+"_"+curPath['PointIndex']+"_"+date;
            var ui_existed = document.getElementById(Inspector_UI);
            if (ui_existed == undefined)
            {
                var td = document.createElement('td');
                td.setAttribute("align","center");
                table_row_date.appendChild(td);

                var span_NodeId = document.createElement("span");
                span_NodeId.setAttribute("class","badge bg-light-blue-active");
                span_NodeId.setAttribute("id",Inspector_UI);
                span_NodeId.textContent = getInspectorName(daydata['Inspector']);
                td.appendChild(span_NodeId);

                var span_Status = getStatusNode(daydata['Status'], daydata['EnterTime'],daydata['LeaveTime']);
                span_Status.setAttribute("id","Status_"+GroupID+"_"+curPath['PointIndex']+"_"+date);
                td.appendChild(span_Status);
            }
        }

/*
        if ( span_EndAnchorNodeId != undefined)
        {
            console.log("load_inspectionpoint_dailydata() EndAnchorNodeId:"+EndAnchorNodeId);
            //console.log("load_inspectionpoint_dailydata() span_EndAnchorNodeId == undefined");
            var EndTagNodeId_UI = "EndTagNodeId_"+GroupID+"_"+curPath['PointIndex']+"_"+date;
            var ui_existed = document.getElementById(EndTagNodeId_UI);
            if (ui_existed == undefined)
            {
                var td = document.createElement('td');
                td.setAttribute("align","center");
                table_row_date.appendChild(td);

                var span_NodeId = document.createElement("span");
                span_NodeId.setAttribute("class","badge bg-light-blue-active");
                span_NodeId.setAttribute("id",EndTagNodeId_UI);
                span_NodeId.textContent = daydata['EndTagNodeId'];
                td.appendChild(span_NodeId);

                var span_Status = getStatusNode(daydata['EndTagStatus'], daydata['EndTagEnterTime'], daydata['EndTagLeaveTime']);
                span_Status.setAttribute("id","EndTagStatus_"+GroupID+"_"+curPath['PointIndex']+"_"+date);
                td.appendChild(span_Status);
            }
        }

*/
        index++;
    }
}

function getStatusNode(Status, EnterTime, LeaveTime)
{
    var span_Status = document.createElement("span");
    var time_record = convertRecordTime(EnterTime) +" -> "+ convertRecordTime(LeaveTime);
    span_Status.setAttribute("title",time_record);
    if ( Status == '0')
    {
        span_Status.setAttribute("title","");
        span_Status.setAttribute("class","badge bg-gray");
        span_Status.textContent = '無紀錄';
    }
    else
    if ( Status == '1' )
    {
        span_Status.setAttribute("class","badge bg-green");
        span_Status.textContent = '完成';
    }
    else
    if ( Status == '2' )
    {
        span_Status.setAttribute("class","badge bg-red");
        span_Status.textContent = '巡檢時間過短';
    }
    else
    if ( Status == '3' )
    {
        span_Status.setAttribute("class","badge bg-yellow-active");
        span_Status.textContent = '巡檢時間過長';
    }

    return span_Status;
}

function getStatusPath(daydata)
{
    Finished = daydata['Finished'];
    Exceed = daydata['Exceed'];
    Checked = daydata['Checked'];

    var span_Status = document.createElement("span");
    if ( Finished == 0 && Exceed == 0 && Checked == 0)
    {
        span_Status.setAttribute("class","badge bg-gray");
        span_Status.textContent = '無紀錄';
    }
    else
    {
        var time_record = "";
        time_record = convertRecordTime(daydata['StartTagEnterTime']) +" -> "+ convertRecordTime(daydata['EndTagEnterTime']);
        span_Status.setAttribute("title",time_record);

        if (Checked == 1 && Exceed == 1)
        {
            span_Status.setAttribute("class","badge bg-red");
            span_Status.textContent = '巡檢路線逾時';
        }
        else
        if (Checked == 1 && Finished == 1)
        {
            span_Status.setAttribute("class","badge bg-green");
            span_Status.textContent = '巡檢路線完成';
        }
        else
        if (Checked == 1)
        {
            span_Status.setAttribute("class","badge bg-green");
            span_Status.textContent = '巡檢路線已檢查';
        }
    }
    return span_Status;
}

function getCheckPointPath(GroupID, PointID)
{
    var paths = glInspectionGroupPath[GroupID];
    var curPath = null;
    var index;
    for(index = 0;index < paths.length;index++)
    {
        if ( paths[index]['PointID'] == PointID )
        {
            curPath = paths[index];
            break;
        }
    }
    return curPath;
}
//
//  nav-tabs for Group name
//
function add_exist_inspectiongroup(ui_index, GroupID, load_index, value)
{
    var inspectiongroup_nav_tabs = document.getElementById('inspectiongroup_nav_tabs');

    var tab = document.createElement('li');
    if (ui_index == gl_active_group_index)
    {
        tab.setAttribute("class","active");
    }
    var ahref = document.createElement('a');
    ahref.setAttribute("href","#inspectionpoint_table_"+GroupID);
    ahref.setAttribute("data-toggle","tab");
    ahref.setAttribute("onclick","group_tab_onclick("+ GroupID +","+load_index+");return false;");
    var span = document.createElement("span");
    span.setAttribute("class","badge bg-blue-active");
    span.textContent = value['GroupName'];
    ahref.appendChild(span);
    tab.appendChild(ahref);

    inspectiongroup_nav_tabs.appendChild(tab);
}

function create_inspectionpoint_table(GroupID, GroupData, load_index)
{
    var inspectiongroup_tab_content = document.getElementById('inspectiongroup_tab_content');

    var divTabPane = document.createElement('div');
    if (load_index == 0)
        divTabPane.setAttribute("class","tab-pane active");
    else
        divTabPane.setAttribute("class","tab-pane");
    divTabPane.setAttribute("id","inspectionpoint_table_"+GroupID);
    inspectiongroup_tab_content.appendChild(divTabPane);

    //
    // Box
    //
    var boxGroup = document.createElement('div');
    boxGroup.setAttribute("class","box");
    divTabPane.appendChild(boxGroup);

    //
    // box header
    //
    var boxHeader = document.createElement('div');
    boxHeader.setAttribute("class","box-header");
    boxGroup.appendChild(boxHeader);

    // box-title
    var boxTitle = document.createElement('H3');
    boxTitle.setAttribute("class","box-title");
    boxHeader.appendChild(boxTitle);

    var spanTitle = document.createElement("span");
    spanTitle.setAttribute("class","badge bg-green");
    spanTitle.setAttribute("id","Group_Status_Title_"+GroupID);
    //spanTitle.textContent = "巡檢已完成";
    boxTitle.appendChild(spanTitle);

    var spanTitleStartTime = document.createElement("span");
    spanTitleStartTime.setAttribute("class","badge bg-yellow-active");
    spanTitleStartTime.setAttribute("id","Group_StartEndTime_"+GroupID);
    spanTitleStartTime.textContent = GroupData['StartTime'] +" -> "+ GroupData['EndTime'];
    boxTitle.appendChild(spanTitleStartTime);


    // box-tools
    var divTools = document.createElement("div");
    divTools.setAttribute("class","box-tools");
    boxHeader.appendChild(divTools);

    var ulTools = document.createElement("ul");
    ulTools.setAttribute("class","pagination pagination-sm no-margin pull-right");
    divTools.appendChild(ulTools);

    // '<'
    var liTools1 = document.createElement("li");
    ulTools.appendChild(liTools1);

    var aTools1 = document.createElement("a");
    aTools1.setAttribute("href","javascript:;");
    aTools1.setAttribute("onclick","move_date_backward("+ GroupID +","+load_index+");return false;");
    aTools1.textContent = '\xAB'; // html &laquo;
    liTools1.appendChild(aTools1);

    // 'Page'
    var liTools2 = document.createElement("li");
    ulTools.appendChild(liTools2);

    var aTools2 = document.createElement("a");
    aTools2.setAttribute("href","#");
    aTools2.setAttribute("id","Group_Status_Page_"+GroupID);
    aTools2.textContent = "";
    liTools2.appendChild(aTools2);

    // '>'
    var liTools3 = document.createElement("li");
    ulTools.appendChild(liTools3);

    var aTools3 = document.createElement("a");
    aTools3.setAttribute("href","javascript:;");
    aTools3.setAttribute("onclick","move_date_forward("+ GroupID +","+load_index+");return false;");
    aTools3.textContent = '\xBB'; //html &raquo;
    liTools3.appendChild(aTools3);

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
    theadtrGroup.setAttribute("id","table_head_"+ GroupID);
    theadGroup.appendChild(theadtrGroup);

    var theadth = document.createElement("th");
    theadth.setAttribute("style","width: 10px");
    theadth.textContent = '#';
    theadtrGroup.appendChild(theadth);

    var theadthPointIndex = document.createElement("th");
    theadthPointIndex.setAttribute("style","width: 50px");
    theadthPointIndex.textContent = 'Date';
    theadtrGroup.appendChild(theadthPointIndex);

    var theadth_status = document.createElement("th");
    theadth_status.setAttribute("style","width: 100px; text-align:center");
    theadth_status.textContent = '記錄';
    theadtrGroup.appendChild(theadth_status);

    var theadth_score = document.createElement("th");
    theadth_score.setAttribute("style","width: 100px; text-align:center");
    theadth_score.textContent = '分數';
    theadtrGroup.appendChild(theadth_score);

    var tbodyGroup = document.createElement("tbody");
    tbodyGroup.setAttribute("id","inspectionpoint_table_body_"+GroupID);
    tableGroup.appendChild(tbodyGroup);

}

function add_inspectionpoint_table_head_point(GroupID, value)
{
    var theadtrGroup = document.getElementById('table_head_'+GroupID);
    if (theadtrGroup == undefined)
    {
        console.log("add_checkpoint_table_head() theadtrGroup == undefined");
        return ;
    }

    // Anchor Node ID
    var theadth_node = document.createElement("th");
    theadth_node.setAttribute("style","text-align:center");

    var span_NodeId = document.createElement("span");
    span_NodeId.setAttribute("class","badge bg-light-blue-active");
    span_NodeId.setAttribute("id","AnchorNodeId_"+GroupID+"_"+value['PointIndex']+"_"+value['AnchorNodeId']);
    span_NodeId.textContent = getNodeName(value['AnchorNodeId']);
    theadth_node.appendChild(span_NodeId);
    theadtrGroup.appendChild(theadth_node);
}

function add_inspectionpoint_table_head_traveltime(GroupID, value)
{
    var theadtrGroup = document.getElementById('table_head_'+GroupID);
    if (theadtrGroup == undefined)
    {
        console.log("add_checkpoint_table_head() theadtrGroup == undefined");
        return ;
    }

    // icon
    var theadth_node = document.createElement("th");
    theadth_node.setAttribute("style","text-align:center");
    theadth_node.setAttribute("id","PATH_"+GroupID+"_"+value['PointIndex']);

    var span_arrow = document.createElement("span");
    span_arrow.setAttribute("class","badge bg-aqua");
    theadth_node.appendChild(span_arrow);
    var icon_arrow = document.createElement("i");
    icon_arrow.setAttribute("class","fa fa-long-arrow-right");
    span_arrow.appendChild(icon_arrow);

    theadtrGroup.appendChild(theadth_node);
}

function add_history_row(GroupID, ui_index, key, value)
{
    var checkpointGroup_table_body = document.getElementById('inspectionpoint_table_body_'+GroupID);
    if (checkpointGroup_table_body == undefined)
    {
        console.log("add_history_row() checkpointGroup_table_body == undefined");
        return ;
    }
    var TargetDate = value['DateTime'];
    var Finished = value['Finished'];
    var Exceed = value['Exceed'];
    var Score = value['Score'];

    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","table_row_"+ GroupID+"_"+TargetDate);
    checkpointGroup_table_body.appendChild(row);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Date
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(convertToShowDate(TargetDate)));
    cellIndex++;

    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var span_Status = document.createElement("span");

    if ( Finished == 1 )
    {
        span_Status.setAttribute("class","badge bg-green");
        span_Status.textContent = "巡檢已完成";
    }
    else
    if ( Exceed == 1 )
    {
        span_Status.setAttribute("class","badge bg-red");
        span_Status.textContent = "巡檢已逾時";
    }
    else
    {
        span_Status.setAttribute("class","badge bg-light-blue");
        span_Status.textContent = "巡檢尚未開始";
    }
    row.cells[cellIndex].appendChild(span_Status);
    cellIndex++;

    var td = document.createElement('td');
    td.setAttribute("align","center");
    td.appendChild(document.createTextNode(Score));
    row.appendChild(td);
    cellIndex++;
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

        CurrentLoadIndex = 1;
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

            CurrentLoadIndex = 1;
        }
        else
        {
            load_locators();
        }
    })
    .error(function() {
        gl_locator_load_index = 0;
        CurrentLoadIndex = 1;
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function group_tab_onclick(GroupID, group_load_index)
{
    //console.log("group_tab_onclick() GroupID:"+GroupID +", group_load_index:"+group_load_index);

    load_inspectiongroup_dailydata(group_load_index);
}

function move_date_forward(GroupID, group_load_index)
{
    var end_date = glTargetEndDate[GroupID];
    var cur_date = moment(end_date, "YYYYMMDD");
    var new_end_date = cur_date.add(30, 'day').format('YYYYMMDD');
    var new_start_date = end_date;

    glTargetStartDate[GroupID] = new_start_date;
    glTargetEndDate[GroupID] = new_end_date;

    //console.log("move_date_forward() GroupID:"+GroupID +", new_start_date:"+new_start_date +", new_end_date:"+new_end_date);

    load_inspectiongroup_dailydata(group_load_index);
}

function move_date_backward(GroupID, group_load_index)
{
    var start_date = glTargetStartDate[GroupID];
    var cur_date = moment(start_date, "YYYYMMDD");
    var new_start_date = cur_date.subtract(30, 'day').format('YYYYMMDD');
    var new_end_date = start_date;

    glTargetStartDate[GroupID] = new_start_date;
    glTargetEndDate[GroupID] = new_end_date;

    //console.log("move_date_backward() GroupID:"+GroupID +", new_start_date:"+new_start_date +", new_end_date:"+new_end_date);

    load_inspectiongroup_dailydata(group_load_index);
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
    var Today = new Date();
    today = ""+ Today.getFullYear() + (Today.getMonth()+1) + Today.getDate();

    //today = "20181218";
    return today;
}

function getTargetDate(addDays)
{
    var targetDate = moment().add(addDays, 'd').format('YYYYMMDD');
    return targetDate;
}

function convertToShowDate(inDate)
{
    var cur_date = moment(inDate, "YYYYMMDD");
    var outDate = cur_date.format('YYYY/MM/DD');

    return outDate;
}

function convertRecordTime(inDate)
{
    var cur_date = moment(inDate, "YYYY-MM-DD HH:mm:ss");
    var outDate = cur_date.format('HH:mm:ss');

    return outDate;
}

function open_inspection_history_page()
{
    var win = window.open('/index.php/vilsinspectionhistory/'+project_id, '_blank');
    win.focus();
}
