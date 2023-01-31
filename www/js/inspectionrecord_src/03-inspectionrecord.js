/*
 * Author: ikki Chung
 * Created on: 2020/03/05
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
var glInspectionGroupPath = [[]];
var gl_inspectiongroup_num = 0;
var gl_inspectiongroup_load_index = 0;
var gl_inspectionpoint_load_index = 0;
var gl_inspectionpointdaily_load_index = 0;
var gl_TagBehavior_group_load_index = 0;
var gl_TagBehavior_load_index = []; // for each group has different index

var gl_active_group_index = 1;
var glTargetDate = [];

var gl_MorrisLineList = [];

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

                load_inspectionpoint_exist();
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_inspectionpoint_dailydata_all();
            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                var checkPointGroup = glInspectionGroups[gl_TagBehavior_group_load_index];

                if ( checkPointGroup == undefined )
                {
                    return;
                }

                gl_TagBehavior_load_index[gl_TagBehavior_group_load_index] = 0;
                load_group_inspectionpoint_TagBehavior(gl_TagBehavior_group_load_index);
                CurrentLoadIndex = 6;
            }
        break;
        case 6:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
            }
        break;
    }

    if ( CurrentLoadIndex < 6 )
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
        $.each(data, function(checkpointGroupID, value)
        {
            add_exist_inspectiongroup(index, checkpointGroupID, gl_inspectiongroup_num, value);
            glInspectionGroups[gl_inspectiongroup_num] = value;

            if (glTargetDate[checkpointGroupID] == undefined)
            {
                glTargetDate[checkpointGroupID] = getTodayDate();
            }
            // console.log("load_inspectiongroup_exist() glTargetDate["+gl_inspectiongroup_num+"]:"+glTargetDate[checkpointGroupID]);

            index++;
            gl_inspectiongroup_num++;

        });

        gl_inspectiongroup_load_index = 0;
        CurrentLoadIndex = 2;
    });
    return false;
}

function load_inspectiongroup_dailydata_all()
{
    //console.log("load_inspectiongroup_dailydata_all() gl_inspectiongroup_load_index:"+gl_inspectiongroup_load_index);

    if (gl_inspectiongroup_load_index >= gl_inspectiongroup_num)
    {
        // console.log("load_inspectiongroup_dailydata_all() Finished!");

        gl_inspectionpoint_load_index = 0;
        CurrentLoadIndex = 3;
        return;
    }

    var checkPointGroup = glInspectionGroups[gl_inspectiongroup_load_index];
    var GroupID = checkPointGroup['GroupID'];
    var today = glTargetDate[GroupID];

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectiongroupdailyrecord=1&project_id='+project_id+'&GroupID='+GroupID+'&date='+today, function(data)
    {
        var inspectionpoint_table = document.getElementById('inspectionpoint_table_'+GroupID);
        if (inspectionpoint_table == undefined)
        {
            create_inspectionpoint_table(GroupID, gl_inspectiongroup_load_index);
        }

        glInspectionGroupDailyRecord[GroupID] = data;

        //
        // status title
        //
        update_status_title(GroupID, checkPointGroup, data.Finished, data.Exceed, data.Score, today);


    }).success(function()
    {
        var current_load_index = gl_inspectiongroup_load_index;

        //Date picker
        $('#datepicker_'+GroupID).datepicker({
          autoclose: true,
          format:"YYYY/MM/DD",
          language:"zh-TW"
        }).on('changeDate', function (selected)
        {
            var minDate = new Date(selected.date.valueOf());
            var target_date = moment(minDate).format('YYYYMMDD');
            console.log('#datepicker_'+GroupID+" changeDate() target_date:"+target_date +", current_load_index:"+current_load_index);

            glTargetDate[GroupID] = target_date;
            var status_date = document.getElementById('datepicker_'+GroupID);
            if (status_date != undefined)
            {
                status_date.textContent = convertToShowDate(target_date);
            }
            //=====================
            load_inspectiongroup_dailydata(current_load_index);
            load_inspectionpoint_dailydata(current_load_index);
            remove_sparkine(GroupID, current_load_index);
            gl_TagBehavior_load_index[current_load_index] = 0;
            load_group_inspectionpoint_TagBehavior(current_load_index);
            //=====================
        });

        gl_inspectiongroup_load_index++;

        setTimeout(load_inspectiongroup_dailydata_all, 10);
    })
    .error(function() {
    })
    .complete(function() {
    });

    return false;
}

function load_inspectiongroup_dailydata(Group_Load_Index)
{
    console.log("load_inspectiongroup_dailydata() Group_Load_Index:"+Group_Load_Index);
    var checkPointGroup = glInspectionGroups[Group_Load_Index];
    var GroupID = checkPointGroup['GroupID'];
    var today = glTargetDate[GroupID];

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectiongroupdailyrecord=1&project_id='+project_id+'&GroupID='+GroupID+'&date='+today, function(data)
    {
        var inspectionpoint_table = document.getElementById('inspectionpoint_table_'+GroupID);
        if (inspectionpoint_table == undefined)
        {
            create_inspectionpoint_table(GroupID, Group_Load_Index);
        }

        //
        // status title
        //
        update_status_title(GroupID, checkPointGroup, data.Finished, data.Exceed, data.Score, today);


    }).success(function()
    {
    })
    .error(function() {
    })
    .complete(function() {
    });

    return false;
}

function  load_inspectionpoint_exist()
{
    if (gl_inspectionpoint_load_index >= gl_inspectiongroup_num)
    {
        console.log("load_inspectionpoint_exist() Finished!");

        gl_inspectionpointdaily_load_index = 0;
        CurrentLoadIndex = 4;
        return;
    }

    var checkPointGroup = glInspectionGroups[gl_inspectionpoint_load_index];
    var GroupID = checkPointGroup['GroupID'];

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectionpoint=1&project_id='+project_id+'&GroupID='+GroupID, function(data)
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

        var index = 1;
        var PathCount = Object.keys(data).length;
        glInspectionGroupPath[GroupID] = [];
        gl_TagBehavior_load_index[gl_inspectionpoint_load_index] = 0;

        $.each(data, function(key, value)
        {
            if ( index > 1 )
            {
                // 巡檢路線 資訊
                add_inspectionpoint_traveltime_row(GroupID, index, value);
            }

            // 巡檢點 資訊
            add_inspectionpoint_row(GroupID, index, value);

            index++;

            /*
            // 巡檢點 資訊
            add_inspectionpoint_row(GroupID, index, value, false);

            // 巡檢路線 資訊
            add_inspectionpoint_traveltime_row(GroupID, index, value, false);

            index++;
            if (index == PathCount + 1)
            {
                add_inspectionpoint_row(GroupID, index, value, true);
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

function load_inspectionpoint_dailydata_all()
{
    //console.log("load_inspectionpoint_dailydata() today:"+today +", gl_inspectionpointdaily_load_index:"+gl_inspectionpointdaily_load_index);

    if (gl_inspectionpointdaily_load_index >= gl_inspectiongroup_num)
    {
        document.getElementById("INSPECTIONRECORD_TABLE_MSG").innerText = "";

        console.log("load_inspectionpoint_dailydata_all() Finished!");

        gl_TagBehavior_group_load_index = 0;
        gl_TagBehavior_load_index[gl_TagBehavior_group_load_index] = 0;
        CurrentLoadIndex = 5;
        return;
    }

    var checkPointGroup = glInspectionGroups[gl_inspectionpointdaily_load_index];
    var GroupID = checkPointGroup['GroupID'];
    var today = glTargetDate[GroupID];

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectionpointdailyrecord=1&project_id='+project_id+'&GroupID='+GroupID+'&date='+today, function(data)
    {

        var index = 1;
        var DailyRecordCount = Object.keys(data).length;

        if ( DailyRecordCount == 0 )
        {
            var status_title = document.getElementById('Group_Status_Title_'+GroupID);
            if (status_title != undefined)
            {
                status_title.setAttribute("class","badge bg-red");
                status_title.textContent = "無紀錄";
            }
        }
        var paths = glInspectionGroupPath[GroupID];
        var PathCount = paths.length;

        // console.log("load_inspectionpoint_dailydata_all() PathCount:"+PathCount);

        $.each(data, function(key, value)
        {
            //console.log("load_inspectionpoint_dailydata() GroupID:"+GroupID +",index:"+index+",key:"+key);

            if (index > 1)
            {
                // 巡檢路線 資訊
                add_traveltime_info(GroupID, index, value);
            }

            // 巡檢點 資訊
            add_inspectionpoint_info(GroupID, index, value);

            index++;

            /*
            // 巡檢點 資訊
            add_inspectionpoint_info(GroupID, index, value, false);

            // 巡檢路線 資訊
            add_traveltime_info(GroupID, index, value, false);

            index++;
            if (index == PathCount + 1)
            {
                add_inspectionpoint_info(GroupID, index, value, true);
            }
            */
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

function load_inspectionpoint_dailydata(Group_Load_Index)
{
    console.log("load_inspectionpoint_dailydata() Group_Load_Index:"+Group_Load_Index);

    var checkPointGroup = glInspectionGroups[Group_Load_Index];
    if ( checkPointGroup == undefined )
    {
        console.log("load_inspectionpoint_dailydata() checkPointGroup == undefined");
        return;
    }
    var GroupID = checkPointGroup['GroupID'];
    var today = glTargetDate[GroupID];

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectionpointdailyrecord=1&project_id='+project_id+'&GroupID='+GroupID+'&date='+today, function(data)
    {

        var DailyRecordCount = Object.keys(data).length;

        var paths = glInspectionGroupPath[GroupID];
        var PathCount = paths.length;

        // console.log("load_inspectionpoint_dailydata() DailyRecordCount:"+DailyRecordCount +", PathCount:"+PathCount);

        if (DailyRecordCount == 0)
        {
            clear_GroupPathInfo(GroupID);
        }

        var index = 1;
        $.each(data, function(key, value)
        {
            console.log("load_inspectionpoint_dailydata() GroupID:"+GroupID +",index:"+index+",key:"+key);

            if ( index > 1 )
            {
                // 巡檢路線 資訊
                add_traveltime_info(GroupID, index, value);
            }

            // 巡檢點 資訊
            add_inspectionpoint_info(GroupID, index, value);

            index++;

            /*
            // 巡檢點 資訊
            add_inspectionpoint_info(GroupID, index, value, false);

            // 巡檢路線 資訊
            add_traveltime_info(GroupID, index, value, false);

            index++;
            if (index == PathCount + 1)
            {
                add_inspectionpoint_info(GroupID, index, value, true);
            }
            */
        });
    }).success(function()
    {
    })
    .error(function() {
    })
    .complete(function() {
    });

    return false;

}

function clear_GroupPathInfo(GroupID)
{
    var status_title = document.getElementById('Group_Status_Title_'+GroupID);
    if (status_title != undefined)
    {
        status_title.setAttribute("class","badge bg-red");
        status_title.textContent = "無紀錄";
    }

    // clear checkpoint_info
    var paths = glInspectionGroupPath[GroupID];
    var index;
    for(index = 0;index <= paths.length; index++)
    {
        var curPath;
        if ( index == paths.length )
            curPath = paths[index - 1];
        else
            curPath = paths[index];

        // console.log("load_inspectionpoint_dailydata() index:"+index +", PointIndex:"+curPath['PointIndex']);

        var ui_index = index + 1;

        var span_NodeId = document.getElementById("Inspector_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
        if (span_NodeId != undefined)
        {
            span_NodeId.textContent = '';
        }
        var span_Status = document.getElementById("Status_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
        if (span_Status != undefined)
        {
            span_Status.setAttribute("class","badge bg-gray");
            span_Status.textContent = '無紀錄';
            span_Status.setAttribute("title","");
        }

        var span_PathStatus = document.getElementById("PathStatus_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
        if (span_PathStatus != undefined)
        {
            span_PathStatus.setAttribute("class","badge bg-gray");
            span_PathStatus.textContent = '無紀錄';
            span_PathStatus.setAttribute("title","");
        }
    }

}

function load_group_inspectionpoint_TagBehavior(Group_Load_Index)
{
    // console.log("load_group_inspectionpoint_TagBehavior() Group_Load_Index:"+Group_Load_Index);

    var checkPointGroup = glInspectionGroups[Group_Load_Index];
    if ( checkPointGroup == undefined )
    {
        console.log("load_group_inspectionpoint_TagBehavior() checkPointGroup == undefined");
        return;
    }
    var TagBehavior_load_index = gl_TagBehavior_load_index[Group_Load_Index];

    //console.log("load_group_inspectionpoint_TagBehavior() TagBehavior_load_index:"+TagBehavior_load_index);
    var GroupID = checkPointGroup['GroupID'];
    var checkPointPaths = glInspectionGroupPath[GroupID];

    if ( TagBehavior_load_index >= checkPointPaths.length )
    {
        console.log("load_group_inspectionpoint_TagBehavior() TagBehavior_load_index:"+TagBehavior_load_index +" >= checkPointPaths.length:"+checkPointPaths.length);
        return;
    }

    var curPath = checkPointPaths[TagBehavior_load_index];
    var PointIndex = curPath['PointIndex'];
    var today = glTargetDate[GroupID];

    // console.log("load_group_inspectionpoint_TagBehavior() today:"+today+", Group_Load_Index:"+Group_Load_Index+", TagBehavior_load_index:"+TagBehavior_load_index);

    var ValidCount = 0;
    var LoadNodeID = "";
    var LoadNodeUIId = "";
    var LoadNodeUIElement = "";

    var AnchorNodeUIId = "sparkline_"+GroupID+"_"+PointIndex+"_"+curPath['AnchorNodeId'];
    //console.log("StartNodeUIId:" + StartNodeUIId);

    var valid = false;
    var td_sparkline_AnchorNode = document.getElementById("td_"+ AnchorNodeUIId);

    if ( td_sparkline_AnchorNode != undefined )
    {
        LoadNodeID = curPath['AnchorNodeId'];
        LoadNodeUIId = AnchorNodeUIId;

        //console.log("load_group_inspectionpoint_TagBehavior() StartNode <1> ");

        var td_sparkline_AnchorNode = document.getElementById(AnchorNodeUIId);
        if ( td_sparkline_AnchorNode != undefined )
        {
            //console.log("load_group_inspectionpoint_TagBehavior() StartNode <2> ");

            if ( td_sparkline_AnchorNode.value == undefined )
            {
                //console.log("load_group_inspectionpoint_TagBehavior() StartNode <3> ");

                //console.log("sparkline_StartNode_node.value == undefined StartNodeId:"+ curPath['StartNodeId']);
                ValidCount++;
            }
            else
            {
                //console.log("load_group_inspectionpoint_TagBehavior() StartNode <4> ");
            }
        }
        else
        {
            //console.log("load_group_inspectionpoint_TagBehavior() StartNode <5> ");
            ValidCount++;
        }
    }
    else
    {
        //console.log("load_group_inspectionpoint_TagBehavior() StartNode <6> ");
    }
/*
    var EndNodeUIId = "sparkline_"+GroupID+"_"+PointIndex+"_"+curPath['EndNodeId'];
    //console.log("EndNodeUIId:" + EndNodeUIId);
    var td_sparkline_EndNode = document.getElementById("td_"+ EndNodeUIId);
    //var sparkline_EndNode_node = document.getElementById(EndNodeUIId);

    if ( td_sparkline_EndNode != undefined )
    {
        //console.log("load_group_inspectionpoint_TagBehavior() EndNode <1> ");

        if (ValidCount == 0)
        {
            //console.log("load_group_inspectionpoint_TagBehavior() EndNode <2> ");
            LoadNodeID = curPath['EndNodeId'];
            LoadNodeUIId = EndNodeUIId;
        }

        var sparkline_EndNode_node = document.getElementById(EndNodeUIId);
        if ( sparkline_EndNode_node != undefined )
        {
            //console.log("load_group_inspectionpoint_TagBehavior() EndNode <3> ");

            if ( sparkline_EndNode_node.value == undefined )
            {
                //console.log("load_group_inspectionpoint_TagBehavior() EndNode <4> ");
               //console.log("sparkline_StartNode_node.value == undefined StartNodeId:"+ curPath['StartNodeId']);
                ValidCount++;
            }
            else
            {
                //console.log("load_group_inspectionpoint_TagBehavior() EndNode <5> ");
            }
        }
        else
        {
            //console.log("load_group_inspectionpoint_TagBehavior() EndNode <6> ");
            ValidCount++;
        }
    }
    else
    {
        //console.log("load_group_inspectionpoint_TagBehavior() EndNode <7> ");
    }
*/
    // console.log("load_group_inspectionpoint_TagBehavior LoadNodeID:" + LoadNodeID +", ValidCount:"+ValidCount+", LoadNodeUIId:"+LoadNodeUIId);

    fetch_inspectionpoint_TagBehavior(project_id, GroupID, today, PointIndex, LoadNodeID, LoadNodeUIId, function()
        {
            // callback
            var path_id = GroupID+"_"+PointIndex+"_"+LoadNodeID;
            var div_progress = document.getElementById("Progress_"+path_id);
            if (div_progress != undefined)
            {
                div_progress.setAttribute("style","display:none");
            }

            if (ValidCount <= 1)
            {
                TagBehavior_load_index++;
                gl_TagBehavior_load_index[Group_Load_Index] = TagBehavior_load_index;
            }
            else
            {
                // load again
            }

            if ( checkPointPaths.length == TagBehavior_load_index)
            {
                console.log("load_group_inspectionpoint_TagBehavior() Finished! GroupID:"+GroupID +", TagBehavior_load_index:"+TagBehavior_load_index +", checkPointPaths.length:"+checkPointPaths.length);
                CurrentLoadIndex = 6;
            }
            else
            {
                setTimeout(load_group_inspectionpoint_TagBehavior, 1, Group_Load_Index);
            }
        });

}


function fetch_inspectionpoint_TagBehavior(project_id, GroupID, today, PointIndex, LoadNodeID, LoadNodeUIId, callback)
{

    // console.log("fetch_inspectionpoint_TagBehavior() "+LoadNodeUIId +", start load:"+new Date().toLocaleString());

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectionTagBehavior=1&project_id='+project_id+'&GroupID='+GroupID+'&date='+today+'&PointIndex='+PointIndex+'&Anchor='+LoadNodeID, function(data)
    {

        // console.log("fetch_inspectionpoint_TagBehavior() LoadNodeUIId:"+LoadNodeUIId +", data.length:"+data.length +", "+new Date().toLocaleString());

        var sparkline_Node_node = document.getElementById(LoadNodeUIId);
        if ( sparkline_Node_node == undefined )
        {
            //console.log("EndNodeId undefined EndNodeId:" + curPath['EndNodeId']);
            var td_sparkline = document.getElementById("td_"+ LoadNodeUIId);
            if (td_sparkline != undefined)
            {
                sparkline_Node_node = create_sparkline_div(GroupID, PointIndex, LoadNodeID);
                td_sparkline.appendChild(sparkline_Node_node);
            }
        }

        if ( sparkline_Node_node != undefined )
        {
            sparkline_Node_node.value = "loaded";

            if (data.length > 0)
            {
                sparkline_Node_node.setAttribute("style","height: 200px;");

                var line = new Morris.Line({
                  element: LoadNodeUIId,
                  resize: true,
                  redraw: true,
                  data: data,
                  xkey: 'issuetime',
                  ykeys: ['Distance'],
                  labels: ['Distance'],
                  lineColors: ['#3c8dbc'],
                  hideHover: 'auto'
                });

                //console.log("fetch_inspectionpoint_TagBehavior() before gl_MorrisLineList.length:"+gl_MorrisLineList.length);
                // remove old one
                for (var i = 0; i < gl_MorrisLineList.length; i++)
                {
                    //console.log("fetch_inspectionpoint_TagBehavior() i:"+ i +" gl_MorrisLineList[i].options.element:"+gl_MorrisLineList[i].options.element);
                    if ( gl_MorrisLineList[i].options.element == LoadNodeUIId)
                    {
                        //console.log("fetch_inspectionpoint_TagBehavior() splice("+i+")");
                        gl_MorrisLineList.splice(i, 1);
                        break;
                    }
                }
                gl_MorrisLineList.push(line);

                //console.log("fetch_inspectionpoint_TagBehavior() gl_MorrisLineList.length:"+gl_MorrisLineList.length);
            }
            else
            {
                nodata_Node_node = create_nodata_div(GroupID, PointIndex, LoadNodeID);
                sparkline_Node_node.appendChild(nodata_Node_node);
            }

            // console.log("fetch_inspectionpoint_TagBehavior() finished! load:"+new Date().toLocaleString());

        }
    }).success(function() {

        if (typeof callback === "function")
        {
            callback();
        }
    })
    .error(function() {
    })
    .complete(function() {
    });
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

function create_inspectionpoint_table(GroupID, load_index)
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
    spanTitle.textContent = "";
    boxTitle.appendChild(spanTitle);

    var spanTitleStartTime = document.createElement("span");
    spanTitleStartTime.setAttribute("class","badge bg-yellow-active");
    spanTitleStartTime.setAttribute("id","Group_StartEndTime_"+GroupID);
    spanTitleStartTime.textContent = "";
    boxTitle.appendChild(spanTitleStartTime);

    var spanTitleScore = document.createElement("span");
    spanTitleScore.setAttribute("class","badge bg-light-green");
    spanTitleScore.setAttribute("id","Group_Score_"+GroupID);
    spanTitleScore.textContent = "";
    boxTitle.appendChild(spanTitleScore);

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

    // 'Date'
    var liTools2 = document.createElement("li");
    ulTools.appendChild(liTools2);

    var aTools2 = document.createElement("a");
    aTools2.setAttribute("href","#");
    aTools2.setAttribute("id","datepicker_"+GroupID);
    //aTools2.setAttribute("id","Group_Status_Date_"+GroupID);
    aTools2.textContent = "19750418";
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
    theadGroup.appendChild(theadtrGroup);

    // var theadth = document.createElement("th");
    // theadth.setAttribute("style","width: 10px");
    // theadth.textContent = '#';
    // theadtrGroup.appendChild(theadth);

    var theadthPointIndex = document.createElement("th");
    theadthPointIndex.setAttribute("style","width: 50px");
    theadthPointIndex.textContent = '編號';
    theadtrGroup.appendChild(theadthPointIndex);

    var theadth_anchornode = document.createElement("th");
    theadth_anchornode.setAttribute("style","width: 100px; text-align:center");
    theadth_anchornode.textContent = '巡檢點';
    theadtrGroup.appendChild(theadth_anchornode);

    var theadth_tagnode = document.createElement("th");
    theadth_tagnode.setAttribute("style","width: 100px; text-align:center");
    theadth_tagnode.textContent = '巡檢員';
    theadtrGroup.appendChild(theadth_tagnode);

    var theadth_status = document.createElement("th");
    theadth_status.setAttribute("style","width: 50px; text-align:center");
    theadth_status.textContent = '記錄';
    theadtrGroup.appendChild(theadth_status);

    var theadth_score = document.createElement("th");
    theadth_score.setAttribute("style","width: 50px; text-align:center");
    theadth_score.textContent = '分數';
    theadtrGroup.appendChild(theadth_score);

    var theadth_behaivor = document.createElement("th");
    theadth_behaivor.setAttribute("style","text-align:center");
    theadth_behaivor.textContent = '活動';
    theadtrGroup.appendChild(theadth_behaivor);

    var tbodyGroup = document.createElement("tbody");
    tbodyGroup.setAttribute("id","inspectionpoint_table_body_"+GroupID);
    tableGroup.appendChild(tbodyGroup);

}

function update_status_title_msg(GroupID, message)
{

    var GroupDailyRecord = glInspectionGroupDailyRecord[GroupID];

    var status_title = document.getElementById('Group_Status_Title_'+GroupID);
    if (status_title != undefined)
    {
        if ( GroupDailyRecord.Finished == 1 )
        {
            status_title.setAttribute("class","badge bg-green");
            status_title.textContent = "巡檢已完成";
        }
        else
        if ( GroupDailyRecord.Exceed == 1 )
        {
            status_title.setAttribute("class","badge bg-red");
            status_title.textContent = "巡檢已逾時";
        }
        else
        {
            status_title.setAttribute("class","badge bg-light-blue");
            status_title.textContent = message;
        }
    }
}

function update_status_title(GroupID, GroupData, Finished, Exceed, Score, targetDate)
{
    var status_title = document.getElementById('Group_Status_Title_'+GroupID);
    var time_title = document.getElementById('Group_StartEndTime_'+GroupID);
    var score_title = document.getElementById('Group_Score_'+GroupID);

    time_title.textContent = GroupData['StartTime'] +" -> "+ GroupData['EndTime'];
    if (status_title != undefined)
    {
        if ( Finished == 1 )
        {
            status_title.setAttribute("class","badge bg-green");
            status_title.textContent = "巡檢已完成";
        }
        else
        if ( Exceed == 1 )
        {
            status_title.setAttribute("class","badge bg-red");
            status_title.textContent = "巡檢已逾時";
        }
        else
        {
            status_title.setAttribute("class","badge bg-light-blue");
            status_title.textContent = "巡檢尚未開始";
        }
    }
    //var status_date = document.getElementById('Group_Status_Date_'+GroupID);
    var status_date = document.getElementById('datepicker_'+GroupID);
    if (status_date != undefined)
    {
        status_date.textContent = convertToShowDate(targetDate);
    }

    score_title.textContent = "分數: " + Score;
}

function add_inspectionpoint_row(GroupID, ui_index, value)
{
    var checkpointGroup_table_body = document.getElementById('inspectionpoint_table_body_'+GroupID);
    if (checkpointGroup_table_body == undefined)
    {
        return ;
    }

    var curPath = value;

    var cellIndex = 0;
    var row = document.createElement('tr');
    checkpointGroup_table_body.appendChild(row);

    // index
    // row.appendChild(document.createElement('td'));
    // row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    // cellIndex++;

    // PointIndex
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+curPath['PointIndex']));
    cellIndex++;

    // Anchor Node ID
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var span_NodeId = document.createElement("span");
    span_NodeId.setAttribute("class","badge bg-light-blue-active");
    span_NodeId.setAttribute("id","AnchorNodeId_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
    span_NodeId.textContent = getNodeName(curPath['AnchorNodeId']);
    row.cells[cellIndex].appendChild(span_NodeId);
    cellIndex++;

    // Tag Node ID
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var span_NodeId = document.createElement("span");
    span_NodeId.setAttribute("class","badge bg-light-blue-active");
    span_NodeId.setAttribute("id","Inspector_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
    row.cells[cellIndex].appendChild(span_NodeId);
    cellIndex++;

    //console.log("add_inspectionpoint_row() span_NodeId:"+"Inspector_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);

    // status
    // 0:None 1:match 2:Too Short 3:Too Long
    //var status = value['StartTagStatus'];

    // status
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var span_Status = document.createElement("span");
    span_Status.setAttribute("id","Status_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
    row.cells[cellIndex].appendChild(span_Status);
    cellIndex++;

    // score
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var span_Score = document.createElement("span");
    span_Score.setAttribute("id","Score_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
    row.cells[cellIndex].appendChild(span_Score);
    cellIndex++;

    //console.log("add_inspectionpoint_row() span_Status:"+"Status_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);

    var td_sparkline = document.createElement('td')
    var div_sparkline = null;
    var path_id = "";
    path_id = GroupID+"_"+curPath['PointIndex']+"_"+curPath['AnchorNodeId'];
    td_sparkline.setAttribute("id","td_sparkline_"+path_id);
    row.appendChild(td_sparkline);

    var div_progress = document.createElement('div');
    div_progress.setAttribute("class","progress progress-sm active");
    div_progress.setAttribute("id","Progress_"+path_id);
    var div_progress_bar = document.createElement('div');
    div_progress_bar.setAttribute("class","progress-bar progress-bar-success progress-bar-striped");
    div_progress_bar.setAttribute("role","progressbar");
    div_progress_bar.setAttribute("aria-valuenow","20");
    div_progress_bar.setAttribute("aria-valuemin","0");
    div_progress_bar.setAttribute("aria-valuemax","100");
    div_progress_bar.setAttribute("style","width:100%");
    div_progress.appendChild(div_progress_bar);
    row.cells[cellIndex].appendChild(div_progress);
    //<span class="sr-only">20% Complete</span>

    cellIndex++;
}

function add_inspectionpoint_traveltime_row(GroupID, ui_index, value)
{
    var checkpointGroup_table_body = document.getElementById('inspectionpoint_table_body_'+GroupID);
    if (checkpointGroup_table_body == undefined)
    {
        return ;
    }
    var curPath = value;

    var cellIndex = 0;
    var row = document.createElement('tr');
    row.setAttribute("bgcolor","#ebebe0");
    checkpointGroup_table_body.appendChild(row);

    // index
    // row.appendChild(document.createElement('td'));
    // row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    // cellIndex++;

    // PointIndex
    row.appendChild(document.createElement('td'));
    // row.cells[cellIndex].appendChild(document.createTextNode(""+curPath['PointIndex']));
    cellIndex++;

    // icon
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var span_arrow = document.createElement("span");
    span_arrow.setAttribute("class","badge bg-aqua");
    row.cells[cellIndex].appendChild(span_arrow);
    var icon_arrow = document.createElement("i");
    icon_arrow.setAttribute("class","fa fa-long-arrow-down");
    span_arrow.appendChild(icon_arrow);
    cellIndex++;

    var td = document.createElement('td');
    row.appendChild(td);
    cellIndex++;

    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var span_Status = document.createElement("span");
    span_Status.setAttribute("id","PathStatus_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
    row.cells[cellIndex].appendChild(span_Status);
    cellIndex++;

}

function add_inspectionpoint_info(GroupID, ui_index, value)
{
    var paths = glInspectionGroupPath[GroupID];
    var curPath = null;
    var index;
    for(index = 0;index < paths.length;index++)
    {
        if ( paths[index]['PointID'] == value['PointID'] )
        {
            curPath = paths[index];
            break;
        }
    }
    if ( curPath == null )
    {
        console.log("add_inspectionpoint_info() curPath == null");
        return;
    }

    var span_NodeId = document.getElementById("Inspector_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
    var span_Status = document.getElementById("Status_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
    var span_Score = document.getElementById("Score_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
    //console.log("add_inspectionpoint_info() span_NodeId:"+"Inspector_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
    //console.log("add_inspectionpoint_info() span_Status:"+"Status_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);

    if ( span_NodeId == null || span_Status == null || span_Score == null )
    {
        //console.log("add_inspectionpoint_info() span_NodeId == null");
        return;
    }

    // if ( span_Status == null)
    // {
    //     //console.log("add_inspectionpoint_info() span_Status == null");
    //     return;
    // }

    // status
    // 0:None 1:match 2:Too Short 3:Too Long
    var status = value['Status'];
    var Inspector = value['Inspector'];
    var RecordNodeId = value['RecordNodeId'];
    var time_record = convertRecordTime(value['EnterTime']) +" -> "+ convertRecordTime(value['LeaveTime']);

    span_NodeId.textContent = getInspectorName(Inspector);
    span_Score.textContent =  value['Score'];

    span_Status.setAttribute("title", time_record);
    if ( status == '0')
    {
        span_Status.setAttribute("title","");
        if (RecordNodeId.length > 0)
        {
            span_Status.setAttribute("class","badge bg-green");
            span_Status.textContent = '巡檢中';

            update_status_title_msg(GroupID, '巡檢中');
        }
        else
        {
            span_Status.setAttribute("class","badge bg-gray");
            span_Status.textContent = '無紀錄';
        }
    }
    else
    if ( status == '1' )
    {
        span_Status.setAttribute("class","badge bg-green");
        span_Status.textContent = '完成';
    }
    else
    if ( status == '2' )
    {
        span_Status.setAttribute("class","badge bg-red");
        span_Status.textContent = '巡檢時間過短';
        update_status_title_msg(GroupID, '巡檢時間過短');
    }
    else
    if ( status == '3' )
    {
        span_Status.setAttribute("class","badge bg-yellow-active");
        span_Status.textContent = '巡檢時間過長';
        update_status_title_msg(GroupID, '巡檢時間過長');
    }
}

function create_nodata_div(GroupID, PointIndex, AnchorNodeId)
{
    var div_nodata = document.createElement("div");
    var span_nodata= document.createElement("span");
    span_nodata.setAttribute("class","badge bg-gray");
    span_nodata.textContent = '無紀錄';
    div_nodata.setAttribute("id","nodata_"+GroupID+"_"+PointIndex+"_"+AnchorNodeId);
    div_nodata.appendChild(span_nodata);
    return div_nodata;
}

function create_sparkline_div(GroupID, PointIndex, AnchorNodeId)
{
    var div_sparkline = document.createElement("div");
    div_sparkline.setAttribute("class","chart");
    //div_sparkline.setAttribute("style","height: 200px;");
    div_sparkline.setAttribute("id","sparkline_"+GroupID+"_"+PointIndex+"_"+AnchorNodeId);
    return div_sparkline;
}

function add_traveltime_info(GroupID, ui_index, value)
{
    var paths = glInspectionGroupPath[GroupID];
    var curPath = null;
    var index;
    for(index = 0;index < paths.length;index++)
    {
        if ( paths[index]['PointID'] == value['PointID'] )
        {
            curPath = paths[index];
            break;
        }
    }
    if ( curPath == null )
    {
        console.log("add_inspectionpoint_info() curPath == null");
        return;
    }

    var span_Status = document.getElementById("PathStatus_"+GroupID+"_"+curPath['PointIndex']+"_"+ui_index);
    if ( span_Status == null )
    {
        console.log("add_inspectionpoint_info() span_Status == null");
        return;
    }


    if ( value['Finished'] == 0 && value['Exceed'] == 0 && value['Checked'] == 0)
    {
        // check tag
        var TagName = value['RecordNodeId'];

        if ( TagName.length > 0 )
        {
            span_Status.setAttribute("class","badge bg-green");
            span_Status.textContent = '巡檢中';
        }
        else
        {
            span_Status.setAttribute("class","badge bg-gray");
            span_Status.textContent = '無紀錄';
        }
        // span_Status.setAttribute("class","badge bg-gray");
        // span_Status.textContent = '無紀錄';
    }
    else
    {
        var time_record = "";
        time_record = convertRecordTime(value['EnterTime']) +" -> "+ convertRecordTime(value['LeaveTime']);
        span_Status.setAttribute("title",time_record);
        if (value['Checked'] == 1 && value['Exceed'] == 1)
        {
            span_Status.setAttribute("class","badge bg-red");
            span_Status.textContent = '巡檢路線逾時';
        }
        else
        if (value['Checked'] == 1 && value['Finished'] == 1)
        {
            span_Status.setAttribute("class","badge bg-green");
            span_Status.textContent = '巡檢路線完成';
        }
        else
        if (value['Checked'] == 1)
        {
            span_Status.setAttribute("class","badge bg-green");
            span_Status.textContent = '巡檢路線已檢查';
        }
        else
        {
            //  value['Checked'] == 0 && value['Finished'] == 1
            span_Status.setAttribute("class","badge bg-green");
            span_Status.textContent = '巡檢中';
        }
    }
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

    console.log("load_locators(): gl_locator_num:"+gl_locator_num+" gl_locator_load_index:"+gl_locator_load_index);

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
        setTimeout(load_locators, 1000);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function group_tab_onclick(GroupID, group_load_index)
{
    var today = glTargetDate[GroupID];
    console.log("group_tab_onclick() GroupID:"+GroupID +", today:"+today);

    load_group_inspectionpoint_TagBehavior(group_load_index);
}

function move_date_forward(GroupID, group_load_index)
{
    var today = glTargetDate[GroupID];
    var cur_date = moment(today, "YYYYMMDD");
    var target_date = cur_date.add(1, 'day').format('YYYYMMDD');

    console.log("move_date_forward() GroupID:"+GroupID +", today:"+today +", target_date:"+target_date);

    glTargetDate[GroupID] = target_date;
    //var status_date = document.getElementById('Group_Status_Date_'+GroupID);
    var status_date = document.getElementById('datepicker_'+GroupID);
    if (status_date != undefined)
    {
        status_date.textContent = convertToShowDate(target_date);
    }

    remove_sparkine(GroupID, group_load_index);

    //=====================
    //gl_inspectiongroup_load_index = group_load_index;
    //PreLoadIndex = 1;
    //CurrentLoadIndex = 2;
    //check_loading_status();
    //=====================
    load_inspectiongroup_dailydata(group_load_index);
    load_inspectionpoint_dailydata(group_load_index);
    gl_TagBehavior_load_index[group_load_index] = 0;
    load_group_inspectionpoint_TagBehavior(group_load_index);
    //=====================

}

function move_date_backward(GroupID, group_load_index)
{
    var today = glTargetDate[GroupID];
    var cur_date = moment(today, "YYYYMMDD");
    var target_date = cur_date.subtract(1, 'day').format('YYYYMMDD');

    console.log("move_date_backward() GroupID:"+GroupID +", today:"+today +", target_date:"+target_date);

    glTargetDate[GroupID] = target_date;
    //var status_date = document.getElementById('Group_Status_Date_'+GroupID);
    var status_date = document.getElementById('datepicker_'+GroupID);
    if (status_date != undefined)
    {
        status_date.textContent = convertToShowDate(target_date);
    }

    remove_sparkine(GroupID, group_load_index);

    //=====================
    //gl_inspectiongroup_load_index = group_load_index;
    //PreLoadIndex = 1;
    //CurrentLoadIndex = 2;
    //check_loading_status();
    //=====================
    load_inspectiongroup_dailydata(group_load_index);
    load_inspectionpoint_dailydata(group_load_index);
    gl_TagBehavior_load_index[group_load_index] = 0;
    load_group_inspectionpoint_TagBehavior(group_load_index);
    //=====================

}

function remove_sparkine(GroupID, Group_Load_Index)
{
    var checkPointPaths = glInspectionGroupPath[GroupID];
    var index;
    for(index = 0;index < checkPointPaths.length;index++)
    {
        var curPath = checkPointPaths[index];
        var PointIndex = curPath['PointIndex'];
        var AnchorNodeUIId = "sparkline_"+GroupID+"_"+PointIndex+"_"+curPath['AnchorNodeId'];

        var sparkline_AnchorNode_node = document.getElementById(AnchorNodeUIId);
        if ( sparkline_AnchorNode_node != undefined )
        {
            sparkline_AnchorNode_node.value = undefined;
            //console.log("remove_sparkine() StartNodeUIId:"+StartNodeUIId +", sparkline_StartNode_node:"+sparkline_StartNode_node);
            // remove old one
            remove_sparkine_from_list(AnchorNodeUIId);

            var path_id = GroupID+"_"+PointIndex+"_"+curPath['AnchorNodeId'];
            var div_progress = document.getElementById("Progress_"+path_id);
            if (div_progress != undefined)
            {
                div_progress.setAttribute("style","display:bloxk");
            }

            sparkline_AnchorNode_node.parentElement.removeChild(sparkline_AnchorNode_node);
        }

        // var sparkline_EndNode_node = document.getElementById(EndNodeUIId);
        // if ( sparkline_EndNode_node != undefined )
        // {
        //     sparkline_EndNode_node.value = undefined;
        //     //console.log("remove_sparkine() EndNodeUIId:"+EndNodeUIId +", sparkline_EndNode_node:"+sparkline_EndNode_node);
        //
        //     // remove old one
        //     remove_sparkine_from_list(EndNodeUIId);
        //
        //     var path_id = GroupID+"_"+PointIndex+"_"+curPath['EndNodeId'];
        //     var div_progress = document.getElementById("Progress_"+path_id);
        //     if (div_progress != undefined)
        //     {
        //         div_progress.setAttribute("style","display:bloxk");
        //     }
        //
        //     sparkline_EndNode_node.parentElement.removeChild(sparkline_EndNode_node);
        // }
    }
}

function remove_sparkine_from_list(LoadNodeUIId)
{
    // remove old one
    for (var i = 0; i < gl_MorrisLineList.length; i++)
    {
        //console.log("fetch_inspectionpoint_TagBehavior() i:"+ i +" gl_MorrisLineList[i].options.element:"+gl_MorrisLineList[i].options.element);
        if ( gl_MorrisLineList[i].options.element == LoadNodeUIId)
        {
            console.log("remove_sparkine_from_list() splice("+i+") LoadNodeUIId:"+LoadNodeUIId);
            gl_MorrisLineList.splice(i, 1);
            break;
        }
    }
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

$(window).resize(function ()
{
    //console.log("window.resize() gl_MorrisLineList.length:"+gl_MorrisLineList.length);
    for (var i = 0; i < gl_MorrisLineList.length; i++)
    {
        gl_MorrisLineList[i].redraw();
    }
});

function open_inspection_record_page()
{
    var win = window.open('/index.php/vilsinspectionrecord/'+project_id, '_blank');
    win.focus();
}
