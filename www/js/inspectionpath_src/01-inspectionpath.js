/*
 * Author: ikki Chung
 * Created on: 2020/03/03
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
var glDropdown_checkbox_anchor_click = false;

var glInspectionGroups = [];
var gl_inspectiongroup_num = 0;
var gl_inspectiongroup_load_index = 0;

var gl_active_group_index = 1;
var gl_edit_group_info = -1;
var gl_edit_inspection_info_ui_index = -1;
var gl_show_create_inspectionpoint_btn = -1;
var gl_EditMode = false;

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
    }

    if ( CurrentLoadIndex != 3 )
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
        gl_inspectiongroup_load_index = 0;
        glInspectionGroups = [];
        var index = 1;
        $.each(data, function(inspectionGroupID, value)
        {
            add_exist_inspectiongroup_tab(index, inspectionGroupID, value);
            glInspectionGroups[gl_inspectiongroup_num] = value;
            index++;
            gl_inspectiongroup_num++;

        });

        CurrentLoadIndex = 2;
    });
    return false;
}

function load_inspectionpoint_exist()
{
    console.log("load_inspectionpoint_exist() gl_inspectiongroup_load_index:" + gl_inspectiongroup_load_index +", gl_inspectiongroup_num:"+gl_inspectiongroup_num);

    if (gl_inspectiongroup_load_index >= gl_inspectiongroup_num)
    {
        document.getElementById("INSPECTION_TABLE_MSG").innerText = "";

        create_inspectiongroup_newBtn();

        console.log("load_inspectionpoint_exist() Finished!");

        if (gl_edit_group_info == -1)
            gl_EditMode = false;
        else
            gl_EditMode = true;

        CurrentLoadIndex = 3;
        return;
    }

    var InspectionGroup = glInspectionGroups[gl_inspectiongroup_load_index];
    var GroupID = InspectionGroup['GroupID'];

    var targetURL = gl_target_server + "/php/inspectionpath.php";
    $.getJSON(targetURL, 'loadinspectionpoint=1&project_id='+project_id+'&GroupID='+GroupID, function(data)
    {
        var inspectionPoint_table = document.getElementById('inspectionPoint_table_'+GroupID);
        if (inspectionPoint_table == undefined)
        {
            console.log("load_inspectionpoint_exist() create_inspectionPoint_table GroupID:" + GroupID +", gl_inspectiongroup_load_index:"+gl_inspectiongroup_load_index);

            create_inspectionPoint_table(GroupID, gl_inspectiongroup_load_index);
        }

        var inspectionGroup_table_body = document.getElementById('inspectionGroup_table_body_'+GroupID);
        if (inspectionGroup_table_body != undefined)
        {
            console.log("load_inspectionpoint_exist() clear old inspectionGroup_table_body");

            while (inspectionGroup_table_body.firstChild)
            {
                inspectionGroup_table_body.removeChild(inspectionGroup_table_body.firstChild);
            }
        }

        var inspectionPoint_table_body = document.getElementById('inspectionPoint_table_body_'+GroupID);
        if (inspectionPoint_table_body != undefined)
        {
            console.log("load_inspectionpoint_exist() clear old inspectionPoint_table_body_");

            while (inspectionPoint_table_body.firstChild)
            {
                inspectionPoint_table_body.removeChild(inspectionPoint_table_body.firstChild);
            }
        }

        var inspectionGroup = glInspectionGroups[gl_inspectiongroup_load_index];
        if (gl_edit_group_info == GroupID)
        {
            console.log("load_inspectionpoint_exist() add_exist_inspectiongroup_edit_mode");

            add_exist_inspectiongroup_edit_mode(GroupID, gl_inspectiongroup_load_index + 1, inspectionGroup);

            update_selected_inspectors(GroupID);
        }
        else
        {
            console.log("load_inspectionpoint_exist() add_exist_inspectiongroup");

            add_exist_inspectiongroup(GroupID, gl_inspectiongroup_load_index + 1, inspectionGroup);
        }


        var PathCount = Object.keys(data).length;
        var index = 1;
        var NodeId = "";
        var Inspectors = "";
        var Distance = 0;
        var MinTime = 0;
        var MaxTime = 0;
        var Message = 0;
        $.each(data, function(key, value)
        {
            var lastPath = 0;

            if (index == PathCount)
                lastPath = 1;

            console.log("PathCount:"+PathCount+", index:"+index+", lastPath:"+lastPath);

            add_exist_inspectionpoint(GroupID, index, gl_inspectiongroup_load_index + 1, key, value, lastPath);
            //EndNodeId = value['EndNodeId'];
            //EndNodeDistance = value['EndNodeDistance'];
            //EndNodeMinTime = value['EndNodeMinTime'];
            //EndNodeMaxTime = value['EndNodeMaxTime'];
            index++;
        });

        if (gl_show_create_inspectionpoint_btn == 1)
            add_inspectionpoint_CreateBtn(GroupID, index, gl_inspectiongroup_load_index + 1, NodeId, Inspectors, Distance, MinTime, MaxTime, Message);
        else
            add_inspectionpoint_NewBtn(GroupID, index, gl_inspectiongroup_load_index + 1);
        // console.log("load_inspectionpoint_exist() gl_active_group_index:"+gl_active_group_index+", gl_inspectiongroup_load_index:"+gl_inspectiongroup_load_index);


        gl_inspectiongroup_load_index++;

    }).success(function() {

        // console.log("load_inspectionpoint_exist() GroupID:"+GroupID+" gl_inspectiongroup_load_index:"+gl_inspectiongroup_load_index);

        if (gl_inspectiongroup_load_index == gl_active_group_index || gl_inspectiongroup_load_index == 1)
        {
            // console.log("load_inspectionpoint_exist() open this tabs:inspectionPoint_table_"+GroupID+"_tab");

            // open this path
            document.getElementById("inspectionPoint_table_"+GroupID+"_tab").click();
            // gl_CreateNewGroup_index = -1;
        }

        setTimeout(load_inspectionpoint_exist, 10);

        $('.timepicker').timepicker({
          showInputs: false
        })

    })
    .error(function() {
    })
    .complete(function() {
    });
    return false;
}

//
//  nav-tabs for Group name
//
function add_exist_inspectiongroup_tab(ui_index, inspectionGroupID, inspectionGroup)
{
    var inspectiongroup_nav_tabs = document.getElementById('inspectiongroup_nav_tabs');

    var tab = document.createElement('li');

    var ahref = document.createElement('a');
    ahref.setAttribute("href","#inspectionPoint_table_"+inspectionGroupID);
    ahref.setAttribute("data-toggle","tab");
    ahref.setAttribute("id", "inspectionPoint_table_"+inspectionGroupID+"_tab");
    ahref.setAttribute("onclick","inspection_tab_onclick("+ ui_index +", "+ inspectionGroupID +", \""+ inspectionGroup['GroupName'] +"\");return false;");
    var span = document.createElement("span");
    span.setAttribute("class","badge bg-blue-active");
    span.setAttribute("id","inspection_tab_"+inspectionGroupID);
    span.textContent = inspectionGroup['GroupName'];
    ahref.appendChild(span);
    tab.appendChild(ahref);

    inspectiongroup_nav_tabs.appendChild(tab);

    console.log("add_exist_inspectiongroup_tab() ui_index:"+ui_index+", gl_active_group_index:"+gl_active_group_index);

    if (ui_index == gl_active_group_index)
    {
        tab.setAttribute("class","active");
    }
}

function inspection_tab_onclick(ui_index, GroupID, GroupName)
{
    // console.log("inspection_tab_onclick() ui_index:"+ui_index);

    if (gl_EditMode)
    {
        //console.log("map_tab_onclick() ui_index:"+ui_index + " layer_name:"+layer_name);

        if ( gl_edit_group_info != GroupID )
            return;

        var newValue= prompt("Edit Inspection Group Name:", GroupName);
        if ( newValue )
        {
            // console.log("inspection_tab_onclick() newValue:"+newValue);

            var ui = document.getElementById("inspectionPoint_table_"+GroupID+"_tab");
            ui.setAttribute("onclick","inspection_tab_onclick("+ ui_index +", \""+ GroupID +"\", \""+ newValue +"\");return false;");

            var span = document.getElementById("inspection_tab_"+GroupID);
            span.innerHTML = newValue ;
            //
            // update_maplayer_info(layerid, "layer_name", newValue);
        }
    }
    else
    {
        // glCurrentMapIndex = ui_index;
        //
        // var map = glLiveMapList[ui_index];
        // map.refresh();
    }
}

function create_inspectiongroup_newBtn()
{
    var inspectiongroup_nav_tabs = document.getElementById('inspectiongroup_nav_tabs');

    var tab = document.createElement('li');
    var ahref = document.createElement('a');
    ahref.setAttribute("href","javascript:;");
    ahref.setAttribute("onclick","create_inspectiongroup();return false;");
    var span = document.createElement("span");
    span.setAttribute("class","badge bg-red");
    span.textContent = '新增路線+';
    ahref.appendChild(span);
    tab.appendChild(ahref);

    inspectiongroup_nav_tabs.appendChild(tab);
}

//
// For existed Group Info
//
function add_exist_inspectiongroup(GroupID, load_index, inspectionGroup)
{
    var inspectionGroup_table_body = document.getElementById('inspectionGroup_table_body_'+GroupID);

    var cellIndex = 0;
    var row = document.createElement('tr');

    // StartTime
    row.appendChild(document.createElement('td'));
    var span_StartTime = document.createElement("span");
    span_StartTime.setAttribute("class","badge bg-yellow-active");
    span_StartTime.textContent = inspectionGroup['StartTime'];
    row.cells[cellIndex].appendChild(span_StartTime);
    cellIndex++;

    // EndTime
    row.appendChild(document.createElement('td'));
    var span_EndTime = document.createElement("span");
    span_EndTime.setAttribute("class","badge bg-yellow-active");
    span_EndTime.textContent = inspectionGroup['EndTime'];
    row.cells[cellIndex].appendChild(span_EndTime);
    cellIndex++;

    // Inspectors
    row.appendChild(document.createElement('td'));
    var span_Inspectors = create_inspectors_node(inspectionGroup['Inspectors']);
    row.cells[cellIndex].appendChild(span_Inspectors);
    cellIndex++;

    // AlertTime
    row.appendChild(document.createElement('td'));
    var span_AlertTime = document.createElement("span");
    span_AlertTime.setAttribute("class","label label-warning");
    span_AlertTime.textContent = inspectionGroup['AlertTime'] + ' 分鐘前';
    row.cells[cellIndex].appendChild(span_AlertTime);
    cellIndex++;

    // Edit button
    row.appendChild(document.createElement('td'));

    var editBtn = document.createElement("button");
    editBtn.setAttribute("title","Edit this Group");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_inspectiongroup("+ GroupID +",\""+inspectionGroup['GroupName']+"\","+ load_index+ ");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("title","Delete this Group");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","delete_inspectionGroup_confirmbox("+ GroupID +",\""+inspectionGroup['GroupName']+"\");return false;");
    var imgDelete = document.createElement("i");
    imgDelete.setAttribute("class","fa fa-trash-o");
    deleteBtn.appendChild(imgDelete);
    row.cells[cellIndex].appendChild(deleteBtn);

    var AutoFeed = inspectionGroup['AutoFeed'];
    if (AutoFeed === '1')
    {
        var div_AutoFeed = document.createElement("div");
        var item_checkbox_label = document.createElement("label");
        item_checkbox_label.textContent = "使用自動亂數填入巡檢資料";
        div_AutoFeed.appendChild(item_checkbox_label);
        row.cells[cellIndex].appendChild(div_AutoFeed);
    }

    inspectionGroup_table_body.appendChild(row);
}

function create_inspectors_node(Inspectors)
{
    var span_Inspectors = document.createElement("span");
    //span_Inspectors.setAttribute("class","badge bg-light-blue-active");
    //span_Inspectors.textContent = inspectionGroup['Inspectors'];

    var Inspector_nodes = Inspectors.split(",");
    for	(var idx = 0; idx < Inspector_nodes.length; idx++)
    {
        for	(var index = 0; index < glTags.length; index++)
        {
            var nodeData = glTags[index];
            var nodeid = nodeData[3];
            var nodename = nodeData[5];
            if (nodeid === Inspector_nodes[idx])
            {
                var node = create_inspector_node(nodeid, nodename);
                span_Inspectors.appendChild(node);
            }
        }
    }

    return span_Inspectors;
}

//
// For Group Info Edit Mode
//
function add_exist_inspectiongroup_edit_mode(GroupID, load_index, inspectionGroup)
{
    var inspectionGroup_table_body = document.getElementById('inspectionGroup_table_body_'+GroupID);

    var cellIndex = 0;
    var row = document.createElement('tr');

    // StartTime
    row.appendChild(document.createElement('td'));

    // time Picker
    var div_StartTime_TimePicker = document.createElement("div");
    div_StartTime_TimePicker.setAttribute("class","bootstrap-timepicker");
    div_StartTime_TimePicker.setAttribute("style","width:200px;");
    row.cells[cellIndex].appendChild(div_StartTime_TimePicker);

    var div_StartTime = document.createElement("div");
    div_StartTime.setAttribute("class","input-group");
    div_StartTime_TimePicker.appendChild(div_StartTime);
    // input
    var input_StartTime = document.createElement("input");
    input_StartTime.setAttribute("type","text");
    input_StartTime.setAttribute("id","inspectionGroup_StartTime_"+GroupID);
    input_StartTime.setAttribute("readonly","readonly");
    input_StartTime.setAttribute("class","form-control timepicker");
    input_StartTime.setAttribute("value",inspectionGroup['StartTime']);
    div_StartTime.appendChild(input_StartTime);
    // clock icon
    var div_StartTime_clock = document.createElement("div");
    div_StartTime_clock.setAttribute("class","input-group-addon");
    div_StartTime.appendChild(div_StartTime_clock);
    // icon
    var div_StartTime_clock_icon = document.createElement("i");
    div_StartTime_clock_icon.setAttribute("class","fa fa-clock-o");
    div_StartTime_clock.appendChild(div_StartTime_clock_icon);
    cellIndex++;

    // EndTime
    row.appendChild(document.createElement('td'));
    // time Picker
    var div_EndTime_TimePicker = document.createElement("div");
    div_EndTime_TimePicker.setAttribute("class","bootstrap-timepicker");
    div_EndTime_TimePicker.setAttribute("style","width:200px;");
    row.cells[cellIndex].appendChild(div_EndTime_TimePicker);

    var div_EndTime = document.createElement("div");
    div_EndTime.setAttribute("class","input-group");
    div_EndTime_TimePicker.appendChild(div_EndTime);
    // input
    var input_EndTime = document.createElement("input");
    input_EndTime.setAttribute("type","text");
    input_EndTime.setAttribute("id","inspectionGroup_EndTime_"+GroupID);
    input_EndTime.setAttribute("readonly","readonly");
    input_EndTime.setAttribute("class","form-control timepicker");
    input_EndTime.setAttribute("value",inspectionGroup['EndTime']);
    div_EndTime.appendChild(input_EndTime);
    // clock icon
    var div_EndTime_clock = document.createElement("div");
    div_EndTime_clock.setAttribute("class","input-group-addon");
    div_EndTime.appendChild(div_EndTime_clock);
    // icon
    var div_EndTime_clock_icon = document.createElement("i");
    div_EndTime_clock_icon.setAttribute("class","fa fa-clock-o");
    div_EndTime_clock.appendChild(div_EndTime_clock_icon);
    cellIndex++;

    // Inspectors
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);

    var div_Inspectors = document.createElement("div");
    div_Inspectors.setAttribute("class","btn-group");
    // div_Inspectors.setAttribute("id","Inspectors_selection_"+GroupID);
    row.cells[cellIndex].appendChild(div_Inspectors);

    var div_Inspectors_button = document.createElement("button");
    div_Inspectors_button.setAttribute("type","button");
    div_Inspectors_button.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
    div_Inspectors_button.setAttribute("data-toggle","dropdown");
    div_Inspectors_button.textContent = "巡檢員 ";
    div_Inspectors_button.setAttribute("id","dropdown_inspectors_"+GroupID);
    div_Inspectors.appendChild(div_Inspectors_button);

    var div_Inspectors_span = document.createElement("span");
    div_Inspectors_span.setAttribute("class","caret");
    div_Inspectors_button.appendChild(div_Inspectors_span);

    var div_Inspectors_ul = document.createElement("ul");
    div_Inspectors_ul.setAttribute("class","dropdown-menu");
    div_Inspectors_ul.setAttribute("id","dropdown_menu_inspectors_"+GroupID);
    div_Inspectors.appendChild(div_Inspectors_ul);

    var Inspectors = inspectionGroup['Inspectors'].split(",");
    put_Inspectors_to_dropdownlist(GroupID, div_Inspectors_ul, Inspectors);

    // current Inspectors
    var div_cur_Inspectors = document.createElement("div");
    div_cur_Inspectors.setAttribute("id","Inspectors_selected_"+GroupID);
    row.cells[cellIndex].appendChild(div_cur_Inspectors);
    cellIndex++;

    // AlertTime
    row.appendChild(document.createElement('td'));
    var input_Distance = document.createElement("input");
    input_Distance.setAttribute("type","text");
    input_Distance.setAttribute("style","width:50px;");
    input_Distance.setAttribute("class","form-control");
    input_Distance.setAttribute("id","inspectionGroup_AlertTime_"+GroupID);
    input_Distance.setAttribute("value", inspectionGroup['AlertTime']);
    row.cells[cellIndex].appendChild(input_Distance);
    row.cells[cellIndex].appendChild(document.createTextNode("分鐘前"));
    cellIndex++;

    // Save button
    row.appendChild(document.createElement('td'));
    var saveBtn = document.createElement("button");
    saveBtn.setAttribute("title","Edit this Group");
    saveBtn.setAttribute("href","javascript:;");
    saveBtn.setAttribute("onclick","save_inspectiongroup("+ GroupID +",\""+inspectionGroup['GroupName']+"\"," + load_index+ ");return false;");
    var imgSave = document.createElement("i");
    imgSave.setAttribute("class","fa fa-save");
    saveBtn.appendChild(imgSave);
    row.cells[cellIndex].appendChild(saveBtn);

    // AutoFeed 123
    var AutoFeed = inspectionGroup['AutoFeed'];
    var div_AutoFeed = document.createElement("div");
    row.cells[cellIndex].appendChild(div_AutoFeed);

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("id","checkbox_AutoFeed_"+GroupID);
    item_checkbox.setAttribute("type","checkbox");
    if (AutoFeed === '1' )
        item_checkbox.checked = true;
    var item_checkbox_label = document.createElement("label");
    item_checkbox_label.setAttribute("for","checkbox_AutoFeed_"+GroupID);
    item_checkbox_label.textContent = "自動亂數填入巡檢資料";
    div_AutoFeed.appendChild(item_checkbox);
    div_AutoFeed.appendChild(item_checkbox_label);

    cellIndex++;

    inspectionGroup_table_body.appendChild(row);
}

function put_Inspectors_to_dropdownlist(GroupID, div_Inspectors, Inspectors)
{
    for	(var index = 0; index < glTags.length; index++)
    {
        nodeData = glTags[index];
        var macaddress = nodeData[6];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];
        var defaultchecked = false;

        for	(var idx = 0; idx < Inspectors.length; idx++)
        {
            if ( nodeid ===  Inspectors[idx] )
                defaultchecked = true;
        }

        var item_li = create_dropdown_tag_item(GroupID, index, nodeid, nodename, defaultchecked);
        div_Inspectors.appendChild(item_li);
    }
}

function create_dropdown_tag_item(GroupID, tagindex, nodeid, nodename, defaultchecked)
{
    //console.log("create_dropdown_item() nodeid:"+nodeid+", showtext:"+showtext);

    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", tagindex);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_tag_click(\""+GroupID+"\",\""+ tagindex +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_tag_"+tagindex);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_tag_click(\""+GroupID+"\",\""+ tagindex +"\");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+nodename));
    item_li.appendChild(item_click);

    return item_li;
}

function dropdown_checkbox_tag_click(GroupID, tagindex)
{
    glDropdown_checkbox_tag_click = true;
    // console.log("dropdown_checkbox_tag_click() glDropdown_checkbox_tag_click:"+glDropdown_checkbox_tag_click);

    update_selected_inspectors(GroupID);
}

var glDropdown_checkbox_tag_click = false;
function dropdown_tag_click(GroupID, tagindex)
{

    // console.log("dropdown_tag_click() glDropdown_checkbox_tag_click:"+glDropdown_checkbox_tag_click);

    if (glDropdown_checkbox_tag_click == true)
    {
        glDropdown_checkbox_tag_click = false;
        return;
    }

    if (document.getElementById("dropdown_checkbox_tag_"+tagindex).checked == true)
        document.getElementById("dropdown_checkbox_tag_"+tagindex).checked = false;
    else
        document.getElementById("dropdown_checkbox_tag_"+tagindex).checked = true;

    // $("#dropdown_inspectors_"+GroupID).dropdown('toggle');

    // var dropdown_inspectors = document.getElementById("dropdown_inspectors_"+GroupID);

    //dropdown_inspectors.setAttribute("size", "2");

    update_selected_inspectors(GroupID);

    glDropdown_checkbox_tag_click = false;
}

function update_selected_inspectors(GroupID)
{
    var dropdown_inspectors_selected = document.getElementById("Inspectors_selected_"+GroupID);
    while (dropdown_inspectors_selected.firstChild)
    {
        dropdown_inspectors_selected.removeChild(dropdown_inspectors_selected.firstChild);
    }

    for	(var index = 0; index < glTags.length; index++)
    {
        var nodeData = glTags[index];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];
        if (document.getElementById("dropdown_checkbox_tag_"+index).checked)
        {
            var node = create_inspector_node(nodeid, nodename);
            dropdown_inspectors_selected.appendChild(node);
        }
    }
}

function create_inspector_node(nodeid, nodename)
{
    var span_NodeId = document.createElement("span");
    span_NodeId.setAttribute("class","badge bg-light-blue-active");
    span_NodeId.setAttribute("title","id: "+nodeid);
    span_NodeId.textContent = nodename;
    return span_NodeId;
}

//
// For Group Path Info
//
function add_exist_inspectionpoint(GroupID, ui_index, active_tab_index, key, value, lastPath)
{
    var inspectionPoint_table_body = document.getElementById('inspectionPoint_table_body_'+GroupID);

    var PointID = value['PointID'];
    var PointIndex = value['PointIndex'];
    var AnchorNodeId = value['AnchorNodeId'];
    var Distance = value['Distance'];
    var MinTime = value['MinTime'];
    var MaxTime = value['MaxTime'];
    var TravelTime = value['TravelTime'];
    var Message = value['Message'];
    var NodeName = getNodeName(AnchorNodeId);

    console.log("add_exist_inspectionpoint() GroupID:"+GroupID+" PointID:"+PointID+" PointIndex:"+PointIndex+ " gl_edit_inspection_info_ui_index:"+gl_edit_inspection_info_ui_index+ " ui_index:"+ui_index);

    if (ui_index != 1)
    {
        var row1 = add_inspectionpoint_traveltime(GroupID, active_tab_index, PointID, PointIndex, TravelTime);
        inspectionPoint_table_body.appendChild(row1);
    }

    if ( gl_edit_inspection_info_ui_index == ui_index )
    {
        // edit mode
        var row1 = add_inspectionpoint_edit_mode(GroupID, ui_index, active_tab_index, PointID, PointIndex, AnchorNodeId, NodeName, Distance, MinTime, MaxTime, Message, lastPath);
        inspectionPoint_table_body.appendChild(row1);
    }
    else
    {
        var row1 = add_inspectionpoint(GroupID, ui_index, active_tab_index, PointID, PointIndex, AnchorNodeId, NodeName, Distance, MinTime, MaxTime, Message, lastPath);
        inspectionPoint_table_body.appendChild(row1);
    }
}

function add_inspectionpoint(GroupID, ui_index, active_tab_index, PointID, PointIndex, NodeId, NodeName, Distance, MinTime, MaxTime, Message, lastPath)
{
    var cellIndex = 0;
    var row = document.createElement('tr');

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Anchor Node ID
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var span_NodeId = document.createElement("span");
    span_NodeId.setAttribute("class","badge bg-light-blue-active");
    span_NodeId.textContent = NodeName;
    row.cells[cellIndex].appendChild(span_NodeId);
    cellIndex++;

    // travel time rd
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // min_distance rd
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var Span1 = document.createElement("span");
    Span1.setAttribute("class","badge bg-gray-active");
    Span1.textContent = Distance;
    row.cells[cellIndex].appendChild(Span1);
    cellIndex++;

    // min_time rd
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var Span1 = document.createElement("span");
    Span1.setAttribute("class","badge bg-green-active");
    Span1.textContent = MinTime;
    row.cells[cellIndex].appendChild(Span1);
    cellIndex++;

    // max_time rd
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var Span1 = document.createElement("span");
    Span1.setAttribute("class","badge bg-purple-active");
    Span1.textContent = MaxTime;
    row.cells[cellIndex].appendChild(Span1);
    cellIndex++;

    // Message rd
    row.appendChild(document.createElement('td'));
    if (Message != undefined)
        row.cells[cellIndex].appendChild(document.createTextNode(Message));
    else
        row.cells[cellIndex].appendChild(document.createTextNode(""));
    cellIndex++;

    // delete button
    row.appendChild(document.createElement('td'));
    if (lastPath == 1)
    {
        var deleteBtn = document.createElement("button");
        deleteBtn.setAttribute("title","Delete this Inspection Point");
        deleteBtn.setAttribute("href","javascript:;");
        deleteBtn.setAttribute("onclick","delete_inspectionpoint_dialog(\""+ NodeId +"\",\""+ NodeName +"\","+ GroupID +","+ PointID +","+active_tab_index+");return false;");
        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-trash-o");
        deleteBtn.appendChild(imgDelete);
        row.cells[cellIndex].appendChild(deleteBtn);
    }

    // edit button
    var editBtn = document.createElement("button");
    editBtn.setAttribute("title","Edit this Inspection Point");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_inspectionpoint("+ GroupID +","+ ui_index+","+active_tab_index+","+ PointID+","+ PointIndex +");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    return row;
}

function add_inspectionpoint_edit_mode(GroupID, ui_index, active_tab_index, PointID, PointIndex, AnchorNodeId, NodeName, Distance, MinTime, MaxTime, Message, lastPath)
{
    var cellIndex = 0;
    var row = document.createElement('tr');

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Anchor Node ID
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);

    //=============
    // var span_NodeId = document.createElement("span");
    // span_NodeId.setAttribute("class","badge bg-light-blue-active");
    // span_NodeId.textContent = NodeName;
    // row.cells[cellIndex].appendChild(span_NodeId);
    //=============
    var div_NodeId = document.createElement("div");
    div_NodeId.setAttribute("class","btn-group");
    div_NodeId.setAttribute("id","NodeId_selection_"+GroupID);
    load_Items_to_dropdown_UI(GroupID, "edit", "btn-success", div_NodeId, glAnchors, AnchorNodeId, NodeName);
    row.cells[cellIndex].appendChild(div_NodeId);
    //=============
    cellIndex++;

    // travel time rd
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // min_distance input text
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var input_Distance = document.createElement("input");
    input_Distance.setAttribute("type","text");
    input_Distance.setAttribute("class","form-control");
    input_Distance.setAttribute("id","Distance_"+GroupID+"_"+PointIndex);
    input_Distance.setAttribute("value",Distance);
    row.cells[cellIndex].appendChild(input_Distance);
    cellIndex++;

    // min_time  input text
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var input_MinTime = document.createElement("input");
    input_MinTime.setAttribute("type","text");
    input_MinTime.setAttribute("class","form-control");
    input_MinTime.setAttribute("id","MinTime_"+GroupID+"_"+PointIndex);
    input_MinTime.setAttribute("value", MinTime);
    row.cells[cellIndex].appendChild(input_MinTime);
    // var Span1 = document.createElement("span");
    // Span1.setAttribute("class","badge bg-green-active");
    // Span1.textContent = MinTime;
    // row.cells[cellIndex].appendChild(Span1);
    cellIndex++;

    // max_time rd
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var input_MaxTime = document.createElement("input");
    input_MaxTime.setAttribute("type","text");
    input_MaxTime.setAttribute("class","form-control");
    input_MaxTime.setAttribute("id","MaxTime_"+GroupID+"_"+PointIndex);
    input_MaxTime.setAttribute("value", MaxTime);
    row.cells[cellIndex].appendChild(input_MaxTime);
    //var Span1 = document.createElement("span");
    //Span1.setAttribute("class","badge bg-purple-active");
    //Span1.textContent = MaxTime;
    //row.cells[cellIndex].appendChild(Span1);
    cellIndex++;

    // Message
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var input_Message = document.createElement("input");
    input_Message.setAttribute("type","text");
    input_Message.setAttribute("class","form-control");
    input_Message.setAttribute("id","Message_"+GroupID+"_"+PointIndex);
    if ( Message != undefined )
        input_Message.setAttribute("value", Message);
    else
        input_Message.setAttribute("value", "");
    row.cells[cellIndex].appendChild(input_Message);
    cellIndex++;

    // delete button
    row.appendChild(document.createElement('td'));
    if (lastPath == 1)
    {
        var deleteBtn = document.createElement("button");
        deleteBtn.setAttribute("title","Delete this Inspection Point");
        deleteBtn.setAttribute("href","javascript:;");
        deleteBtn.setAttribute("onclick","delete_inspectionpoint_dialog(\""+ AnchorNodeId +"\",\""+ NodeName +"\","+ GroupID +","+ PointID +","+active_tab_index+");return false;");
        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-trash-o");
        deleteBtn.appendChild(imgDelete);
        row.cells[cellIndex].appendChild(deleteBtn);
    }

    // save button
    var saveBtn = document.createElement("button");
    saveBtn.setAttribute("title","Edit this Inspection Point");
    saveBtn.setAttribute("href","javascript:;");
    saveBtn.setAttribute("onclick","update_inspectionpoint("+ GroupID +","+ ui_index+","+active_tab_index+","+ PointID+","+ PointIndex +","+ lastPath+");return false;");
    var imgSave = document.createElement("i");
    imgSave.setAttribute("class","fa fa-save");
    saveBtn.appendChild(imgSave);
    row.cells[cellIndex].appendChild(saveBtn);

    return row;
}

function add_inspectionpoint_traveltime(GroupID, active_tab_index, PointID, PointIndex, TravelTime)
{
    var cellIndex = 0;
    var row = document.createElement('tr');
    row.setAttribute("bgcolor","#ebebe0");

    // empty rd
    row.appendChild(document.createElement('td'));
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

    // Time
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var span_TravelTime = document.createElement("span");
    span_TravelTime.setAttribute("class","badge bg-yellow-active");
    span_TravelTime.textContent = TravelTime;
    row.cells[cellIndex].appendChild(span_TravelTime);
    cellIndex++;

    // min_distance rd
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // min_time rd
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // max_time rd
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // message rd
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // edit button
    row.appendChild(document.createElement('td'));
    var editBtn = document.createElement("button");
    editBtn.setAttribute("title","Edit this travel time");
    editBtn.setAttribute("href","javascript:;");
    editBtn.setAttribute("onclick","edit_traveltime_dialog("+TravelTime+","+ GroupID +","+ PointID+","+ PointIndex +","+active_tab_index+");return false;");
    var imgEdit = document.createElement("i");
    imgEdit.setAttribute("class","fa fa-edit");
    editBtn.appendChild(imgEdit);
    row.cells[cellIndex].appendChild(editBtn);

    return row;
}

function add_inspectionpoint_NewBtn(GroupID, ui_index, active_tab_index)
{
    var inspectionPoint_table_body = document.getElementById('inspectionPoint_table_body_'+GroupID);

    var cellIndex = 0;
    var row = document.createElement('tr');
    row.setAttribute("bgcolor","#ebebe0");

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // Anchors
    row.appendChild(document.createElement('td'));
    var createBtn = document.createElement("button");
    createBtn.setAttribute("title","新增巡檢點");
    createBtn.setAttribute("style","background-color:rgb(236, 240, 245);");
    createBtn.setAttribute("href","javascript:;");
    createBtn.setAttribute("onclick","show_inspectionpoint_CreateBtn("+active_tab_index+");return false;");
    var imgCreate= document.createElement("i");
    imgCreate.setAttribute("class","fa fa-plus");
    imgCreate.textContent = "  新增巡檢點";
    createBtn.appendChild(imgCreate);
    row.cells[cellIndex].appendChild(createBtn);
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    inspectionPoint_table_body.appendChild(row);
    return row;
}

//
// For New Group Path Info
//
function add_inspectionpoint_CreateBtn(GroupID, ui_index, active_tab_index, NodeId, Inspectors, Distance, MinTime, MaxTime, Message)
{
    var inspectionPoint_table_body = document.getElementById('inspectionPoint_table_body_'+GroupID);

    if (ui_index == 1)
    {
        // there does not has any path existed!
        var row1 = create_empty_inspectionpoint(GroupID)
        inspectionPoint_table_body.appendChild(row1);

        var row2 = create_inspectionpoint_selection(GroupID, ui_index, active_tab_index, true, NodeId, Inspectors, Distance, MinTime, MaxTime, Message, ui_index);
        inspectionPoint_table_body.appendChild(row2);
    }
    else
    {
        var row1 = create_travel_time_input(GroupID);
        inspectionPoint_table_body.appendChild(row1);

        var row2 = create_inspectionpoint_selection(GroupID, ui_index + 1, active_tab_index, true, NodeId, Inspectors, Distance, MinTime, MaxTime, Message, ui_index);
        inspectionPoint_table_body.appendChild(row2);
    }

}

function create_empty_inspectionpoint(GroupID)
{
    var cellIndex = 0;
    var row = document.createElement('tr');
    row.setAttribute("bgcolor","#ebebe0");

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // Anchors
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode("尚無巡檢點"));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    return row;
}

function create_inspectionpoint_selection(GroupID, ui_index, active_tab_index, bEnableEditButton, NodeId, Inspectors, Distance, MinTime, MaxTime, Message, newPointIndex)
{
    var cellIndex = 0;
    var row = document.createElement('tr');

    // index
    row.appendChild(document.createElement('td'));
    if ( bEnableEditButton )
        row.cells[cellIndex].appendChild(document.createTextNode(""));
    else
        row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Anchors
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);

    var div_NodeId = document.createElement("div");
    div_NodeId.setAttribute("class","btn-group");
    div_NodeId.setAttribute("id","NodeId_selection_"+GroupID);
    load_Items_to_dropdown_UI(GroupID, "create", "btn-success", div_NodeId, glAnchors, "", "");
    row.cells[cellIndex].appendChild(div_NodeId);
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // min_distance input text
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var input_Distance = document.createElement("input");
    input_Distance.setAttribute("type","text");
    input_Distance.setAttribute("class","form-control");
    input_Distance.setAttribute("id", "Distance_"+GroupID);
    input_Distance.setAttribute("value", 500);
    row.cells[cellIndex].appendChild(input_Distance);
    cellIndex++;

    // min_time  input text
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var input_MinTime = document.createElement("input");
    input_MinTime.setAttribute("type","text");
    input_MinTime.setAttribute("class","form-control");
    input_MinTime.setAttribute("id", "MinTime_"+GroupID);
    input_MinTime.setAttribute("value", 30);
    row.cells[cellIndex].appendChild(input_MinTime);
    cellIndex++;

    // max_time rd
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var input_MaxTime = document.createElement("input");
    input_MaxTime.setAttribute("type","text");
    input_MaxTime.setAttribute("class","form-control");
    input_MaxTime.setAttribute("id", "MaxTime_"+GroupID);
    input_MaxTime.setAttribute("value", 120);
    row.cells[cellIndex].appendChild(input_MaxTime);
    cellIndex++;

    // Message
    var td = document.createElement('td');
    td.setAttribute("align","center");
    row.appendChild(td);
    var input_Message = document.createElement("input");
    input_Message.setAttribute("type","text");
    input_Message.setAttribute("class","form-control");
    input_Message.setAttribute("id", "Message_"+GroupID);
    input_Message.setAttribute("value", "");
    row.cells[cellIndex].appendChild(input_Message);
    cellIndex++;

    // Create button
    row.appendChild(document.createElement('td'));
    if (bEnableEditButton)
    {
        var createBtn = document.createElement("button");
        createBtn.setAttribute("title","儲存巡檢點");
        createBtn.setAttribute("style","background-color:rgb(236, 240, 245);");
        createBtn.setAttribute("href","javascript:;");
        createBtn.setAttribute("onclick","create_inspectionpoint("+ GroupID +","+active_tab_index+","+newPointIndex+");return false;");
        var imgCreate= document.createElement("i");
        imgCreate.setAttribute("class","fa fa-plus");
        imgCreate.textContent = "  儲存巡檢點";
        createBtn.appendChild(imgCreate);
        row.cells[cellIndex].appendChild(createBtn);
    }
    cellIndex++;

    return row;
}

function create_travel_time_input(GroupID)
{
    var cellIndex = 0;
    var row = document.createElement('tr');
    row.setAttribute("bgcolor","#ebebe0");

    // empty td
    row.appendChild(document.createElement('td'));
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

    // Time input text
    row.appendChild(document.createElement('td'));
    var input_TravelTime = document.createElement("input");
    input_TravelTime.setAttribute("type","text");
    input_TravelTime.setAttribute("class","form-control");
    input_TravelTime.setAttribute("id","TravelTime_"+GroupID);
    input_TravelTime.setAttribute("value","500");
    row.cells[cellIndex].appendChild(input_TravelTime);
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    // empty td
    row.appendChild(document.createElement('td'));
    cellIndex++;

    return row;
}

function load_Items_to_dropdown_UI(GroupID, prefix, uitype, tag_selection_btn_group, items, nodeid, nodename)
{
    var itemid = nodeid;
    var itemname = nodename;

    if ( (itemid.length == 0) && (items.length > 0) )
    {
        itemid = items[0].itemid;
        itemname = items[0].itemname;
    }

    var btn_group1_button1 = document.createElement('button');
    tag_selection_btn_group.appendChild(btn_group1_button1);
    btn_group1_button1.setAttribute("class","btn "+uitype);
    btn_group1_button1.setAttribute("data-id", itemid);
    btn_group1_button1.id = prefix +"_dropdown_"+GroupID+"_show";
    var btn_group1_button1_text = document.createTextNode(itemname);
    btn_group1_button1.appendChild(btn_group1_button1_text);

    // dropdown button
    var btn_group1_button2 = document.createElement('button');
    tag_selection_btn_group.appendChild(btn_group1_button2);
    btn_group1_button2.setAttribute("class","btn "+uitype+" dropdown-toggle");
    btn_group1_button2.setAttribute("data-toggle", "dropdown");
    btn_group1_button2.id = prefix +"_dropdown_"+GroupID;

    var btn_group1_button2_span1 = document.createElement('span');
    btn_group1_button2.appendChild(btn_group1_button2_span1);
    btn_group1_button2_span1.setAttribute("class","caret");

    var btn_group1_button2_span2 = document.createElement('span');
    btn_group1_button2.appendChild(btn_group1_button2_span2);
    btn_group1_button2_span2.setAttribute("class","sr-only");

    var dropdown_ul = document.createElement('ul');
    tag_selection_btn_group.appendChild(dropdown_ul);
    dropdown_ul.setAttribute("class", "dropdown-menu");
    dropdown_ul.setAttribute("role", "menu");

    for (var i = 0; i < items.length; i++)
    {
        var dropdown_li_0 = document.createElement('li');
        dropdown_ul.appendChild(dropdown_li_0);

        var dropdown_li_0_href = document.createElement('a');
        dropdown_li_0.appendChild(dropdown_li_0_href);
        dropdown_li_0_href.setAttribute("href", "javascript:;");
        dropdown_li_0_href.setAttribute("onclick", "dropdown_select(\""+prefix+"\","+GroupID+","+ i +");return false;");
        dropdown_li_0_href.setAttribute("data-id", items[i].itemid);
        dropdown_li_0_href.id = prefix +"_dropdown_"+GroupID+"_item_"+i;

        //console.log("load_Items_to_dropdown_UI() items[i].itemname:"+items[i].itemname);

        if (items[i].type == "anchor")
        {
            var span_anchor = document.createElement("span");
            span_anchor.textContent = items[i].itemname;
            span_anchor.setAttribute("class","badge bg-green-active");
            dropdown_li_0_href.appendChild(span_anchor);
        }
        else
        if (items[i].type == "locator")
        {
            var span_anchor = document.createElement("span");
            span_anchor.textContent = items[i].itemname;
            span_anchor.setAttribute("class","badge bg-yellow-active");
            dropdown_li_0_href.appendChild(span_anchor);
        }
    }
}

function dropdown_select(prefix, GroupID, itemidx)
{
    console.log("dropdown_select() prefix:"+prefix+" GroupID:"+GroupID +", itemidx:"+itemidx);

    var select_value = $("#"+prefix+"_dropdown_"+GroupID+"_item_"+itemidx).html();
    var select_id = $("#"+prefix+"_dropdown_"+GroupID+"_item_"+itemidx).attr("data-id");

    $("#"+prefix+"_dropdown_"+GroupID+"_show").html(select_value);
    $("#"+prefix+"_dropdown_"+GroupID+"_show").attr("data-id", select_id);
}

function show_inspectionpoint_CreateBtn(index)
{
    gl_active_group_index = index;
    gl_edit_group_info = -1;
    gl_show_create_inspectionpoint_btn = 1;
    setTimeout(reload_inspectionGroup, 100);
}

function create_inspectionPoint_table(GroupID, load_index)
{
    var inspectiongroup_tab_content = document.getElementById('inspectiongroup_tab_content');

    var div = document.createElement('div');
    if (load_index == 0)
        div.setAttribute("class","tab-pane active");
    else
        div.setAttribute("class","tab-pane");

    div.setAttribute("id","inspectionPoint_table_"+GroupID);

    //=============================
    //
    //  inspection group info
    //
    var tableGroup = document.createElement('table');
    tableGroup.setAttribute("class","table table-bordered");

    var theadGroup = document.createElement("thead");
    tableGroup.appendChild(theadGroup);

    var theadtrGroup = document.createElement("tr");
    theadtrGroup.setAttribute("bgcolor","#cce6ff");
    theadGroup.appendChild(theadtrGroup);

    var theadthGroup_startTime = document.createElement("th");
    theadthGroup_startTime.textContent = '開始巡檢時間';
    theadtrGroup.appendChild(theadthGroup_startTime);

    var theadthGroup_endTime = document.createElement("th");
    theadthGroup_endTime.textContent = '結束巡檢時間';
    theadtrGroup.appendChild(theadthGroup_endTime);

    var theadthGroup_Inspectors = document.createElement("th");
    theadthGroup_Inspectors.textContent = '指派巡檢員';
    theadtrGroup.appendChild(theadthGroup_Inspectors);

    var theadthGroup_AlertTime = document.createElement("th");
    theadthGroup_AlertTime.textContent = '推播提醒時間';
    theadtrGroup.appendChild(theadthGroup_AlertTime);

    var theadthGroup_action = document.createElement("th");
    theadthGroup_action.textContent = '操作';
    theadtrGroup.appendChild(theadthGroup_action);

    var tbodyGroup = document.createElement("tbody");
    tbodyGroup.setAttribute("id","inspectionGroup_table_body_"+GroupID);
    tableGroup.appendChild(tbodyGroup);

    div.appendChild(tableGroup);
    //=============================

    var table = document.createElement('table');
    table.setAttribute("class","table table-bordered");
    div.appendChild(table);

    var thead = document.createElement("thead");
    table.appendChild(thead);

    var theadtr = document.createElement("tr");
    theadtr.setAttribute("bgcolor","#cce6ff");
    thead.appendChild(theadtr);

    var theadth = document.createElement("th");
    theadth.setAttribute("style","width: 10px");
    theadth.textContent = '#';
    theadtr.appendChild(theadth);

    var theadth_startnode = document.createElement("th");
    theadth_startnode.setAttribute("style","width: 200px; text-align:center");
    theadth_startnode.textContent = '巡檢點';
    theadtr.appendChild(theadth_startnode);

    var theadth_trevaltime = document.createElement("th");
    theadth_trevaltime.setAttribute("style","width: 150px; text-align:center");
    theadth_trevaltime.textContent = '巡檢點轉換時間(秒)';
    theadtr.appendChild(theadth_trevaltime);

    var theadth_min_distance = document.createElement("th");
    theadth_min_distance.setAttribute("style","width: 200px; text-align:center");
    theadth_min_distance.textContent = '至少與巡檢點距離(cm):';
    theadtr.appendChild(theadth_min_distance);

    var theadth_min_time = document.createElement("th");
    theadth_min_time.setAttribute("style","width: 150px; text-align:center");
    theadth_min_time.textContent = '至少巡檢時間(秒):';
    theadtr.appendChild(theadth_min_time);

    var theadth_max_time = document.createElement("th");
    theadth_max_time.setAttribute("style","width: 150px; text-align:center");
    theadth_max_time.textContent = '至多巡檢時間(秒):';
    theadtr.appendChild(theadth_max_time);

    var theadth_message = document.createElement("th");
    theadth_message.textContent = '推播提醒訊息';
    theadtr.appendChild(theadth_message);

    var theadth_action = document.createElement("th");
    theadth_max_time.setAttribute("style","width: 150px; text-align:center");
    theadth_action.textContent = '操作';
    theadtr.appendChild(theadth_action);

    var tbody = document.createElement("tbody");
    tbody.setAttribute("id","inspectionPoint_table_body_"+GroupID);
    table.appendChild(tbody);

    inspectiongroup_tab_content.appendChild(div);
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
            setTimeout(load_anchors, 10);
        }
    })
    .error(function() {
        gl_anchor_load_index = 0;
        setTimeout(load_anchors, 1000);
    })
    .complete(function() {
    });

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
}

function dropdown_checkbox_anchor_click(anchorindex)
{
    glDropdown_checkbox_anchor_click = true;
}

var glDropdown_checkbox_anchor_click = false;
function dropdown_anchor_click(anchorindex)
{
    //console.log("dropdown_anchor_click() anchorindex:"+anchorindex);

    document.getElementById("dropdown_checkbox_anchor_"+anchorindex).checked = true;

    glDropdown_checkbox_anchor_click = false;
}

function edit_inspectiongroup(GroupID, GroupName, index)
{
    //console.log("edit_inspectiongroup() GroupID:"+GroupID +", active:"+$('.nav-tabs .active').text());
    console.log("edit_inspectiongroup() GroupID:"+GroupID +", GroupName:"+GroupName +", index:"+index);

    gl_active_group_index = index;
    gl_edit_group_info = GroupID;
    reload_inspectionGroup();
}

function update_inspectionpoint(GroupID, ui_index, active_tab_index, PointID, PointIndex, lastPath)
{
    console.log("update_inspectionpoint() GroupID:"+GroupID +", PointID:"+PointID +", PointIndex:"+PointIndex +", active_tab_index:"+active_tab_index);

    gl_active_group_index = active_tab_index;
    gl_edit_inspection_info_ui_index = -1;


    var id_Distance = "Distance_"+GroupID+"_"+PointIndex;
    var id_MinTime = "MinTime_"+GroupID+"_"+PointIndex;
    var id_MaxTime = "MaxTime_"+GroupID+"_"+PointIndex;
    var id_Message = "Message_"+GroupID+"_"+PointIndex;

    var value_NodeId = $("#edit_dropdown_"+GroupID+"_show").attr("data-id");
    var value_Distance = document.getElementById(id_Distance).value;
    var value_MinTime = document.getElementById(id_MinTime).value;
    var value_MaxTime = document.getElementById(id_MaxTime).value;
    var value_Message = document.getElementById(id_Message).value;

    if (!isANumber(value_Distance) || !isANumber(value_MinTime) || !isANumber(value_MaxTime))
    {
        alert("Error format: 請填入數字!");
        return;
    }

    console.log("update_inspectionpoint() ui_index:"+ui_index+", value_NodeId:"+value_NodeId+", value_Distance:"+value_Distance +", value_MinTime:"+value_MinTime +", value_MaxTime:"+value_MaxTime);

    //
    // update start node only
    //
    update_inspectionpoint_value(GroupID, PointID, value_NodeId, value_Distance, value_MinTime, value_MaxTime, value_Message);

}

function update_inspectionpoint_value(GroupID, PointID, value_NodeId, value_Distance, value_MinTime, value_MaxTime, value_Message)
{
    console.log("update_checkpoint_path_startnode() GroupID:"+GroupID+", PointID:"+PointID);


    var json = {};
    json['UPDATE_POINT'] = 1;
    json['project_id'] = ''+project_id;
    json['GroupID'] = GroupID;
    json['PointID'] = PointID;
    json['AnchorNodeId'] = value_NodeId;
    json['Distance'] = value_Distance;
    json['MinTime'] = value_MinTime;
    json['MaxTime'] = value_MaxTime;
    json['Message'] = value_Message;

    return jQuery.ajax({
    url: '/php/inspectionpath.php',
    type: 'POST',
    contentType: "application/json",
    dataType: "json",
    data: JSON.stringify(json),
    success:
        function(response)
        {
            console.log(response);

            setTimeout(reload_inspectionGroup, 100);
        },
    error:
        function(xhr, status, error)
        {
           console.log(error);
           console.log(xhr.responseText);
           setTimeout(reload_inspectionGroup, 100);
        }
    });
}

function edit_inspectionpoint(GroupID, ui_index, active_tab_index, PointID, PointIndex)
{
    console.log("edit_inspectionpoint() GroupID:"+GroupID +", PointID:"+PointID +", PointIndex:"+PointIndex +", active_tab_index:"+active_tab_index +", ui_index:"+ui_index);

    gl_active_group_index = active_tab_index;
    gl_edit_inspection_info_ui_index = ui_index;
    reload_inspectionGroup();
}


function delete_inspectionGroup_confirmbox(GroupID, GroupName)
{
    console.log("delete_inspectionGroup_confirmbox() GroupID:"+GroupID);

    gl_active_group_index = 1;

    if (confirm('Are you sure you want to delete this inspection group:'+GroupName+' ?'))
    {
        // delete it!
        delete_inspectionGroup(GroupID);
    }
    else
    {
        // Do nothing!
    }
}


function delete_inspectionGroup(inspectionGroupID)
{
    var SendData = "GroupID="+inspectionGroupID;
    SendData += "&project_id="+project_id;
    SendData += "&delete_inspectiongroup=1";

     return jQuery.ajax({
      url: '/php/inspectionpath.php',
      type: 'POST',
      data: SendData,
      dataType: 'text',
    // return jQuery.ajax({
    //  url: '/php/inspectionpath.php?GroupID='+inspectionGroupID+'&project_id='+project_id,
    //  type: 'delete',
    //  contentType: "application/json",
    //  dataType: "json",
    //  data: {},
     success:
      function(response)
      {
           //alert("event_trig_delete success, event_id="+event_id+",result="+result.eventid);
           console.log(JSON.stringify(response));

           gl_edit_group_info = -1;
           gl_active_group_index = 1;
           gl_show_create_inspectionpoint_btn = -1;

           setTimeout(reload_inspectionGroup, 100);
      },
     error:
      function(xhr, status, error)
      {
           console.log(error);
           console.log(xhr.responseText);

           setTimeout(reload_inspectionGroup, 100);
      }
   });
}

function reload_inspectionGroup()
{
    PreLoadIndex = 0;
    CurrentLoadIndex = 1;
    check_loading_status();
}

function create_inspectiongroup()
{
    var newValue= prompt("新增巡檢路線, 路線名稱:","巡檢路線"+ parseInt(gl_inspectiongroup_num+1));
    if ( newValue )
    {
        var json = {};
        json['project_id'] = ''+project_id;
        json['CREATE_GROUP'] = 1;
        json['GroupName'] = newValue;
        json['StartTime'] = "12:00 PM";
        json['EndTime'] = "12:00 PM";
        json['Inspectors'] = "";
        json['AlertTime'] = "30";

        return jQuery.ajax({
        url: '/php/inspectionpath.php',
        type: 'POST',
        contentType: "application/json",
        dataType: "json",
        data: JSON.stringify(json),
        success:
            function(response)
            {
                console.log(response);

                gl_edit_group_info = -1;
                gl_active_group_index = glInspectionGroups.length + 1;
                gl_show_create_inspectionpoint_btn = -1;
                setTimeout(reload_inspectionGroup, 100);
            },
        error:
            function(xhr, status, error)
            {
               console.log(error);
               console.log(xhr.responseText);
            }
        });
    }
}

function save_inspectiongroup(GroupID, GroupName, index)
{
    //console.log("save_inspectiongroup() GroupID:"+GroupID +", active:"+$('.nav-tabs .active').text());
    console.log("save_inspectiongroup() GroupID:"+GroupID +", index:"+index);

    var inspectionGroup_GroupName = document.getElementById("inspection_tab_"+GroupID).innerHTML;
    var inspectionGroup_StartTime = document.getElementById("inspectionGroup_StartTime_"+GroupID).value;
    var inspectionGroup_EndTime = document.getElementById("inspectionGroup_EndTime_"+GroupID).value;
    var inspectionGroup_AlertTime = document.getElementById("inspectionGroup_AlertTime_"+GroupID).value;
    var inspectionGroup_AutoFeed = 0;
    if ( document.getElementById("checkbox_AutoFeed_"+GroupID).checked )
        inspectionGroup_AutoFeed = 1;

    if (!isANumber(inspectionGroup_AlertTime))
    {
        alert("Error format: 推播提醒時間 請填入數字!");
        return;
    }

    var Inspectors = "";
    for	(var index = 0; index < glTags.length; index++)
    {
        var nodeData = glTags[index];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];
        if (document.getElementById("dropdown_checkbox_tag_"+index).checked)
        {
            if(Inspectors.length != 0)
                Inspectors = Inspectors + ",";

            Inspectors = Inspectors + nodeid;
        }
    }

    var json = {};
    json['project_id'] = ''+project_id;
    json['GroupID'] = GroupID;
    json['UPDATE_GROUP'] = 1;
    json['GroupName'] = inspectionGroup_GroupName;
    json['StartTime'] = inspectionGroup_StartTime;
    json['EndTime'] = inspectionGroup_EndTime;
    json['Inspectors'] = Inspectors;
    json['AlertTime'] = inspectionGroup_AlertTime;
    json['AutoFeed'] = inspectionGroup_AutoFeed;

    console.log("save_inspectiongroup() GroupName:"+inspectionGroup_GroupName);
    console.log("save_inspectiongroup() inspectionGroup_StartTime:"+inspectionGroup_StartTime);
    console.log("save_inspectiongroup() inspectionGroup_EndTime:"+inspectionGroup_EndTime);
    console.log("save_inspectiongroup() inspectionGroup_AlertTime:"+inspectionGroup_AlertTime);
    console.log("save_inspectiongroup() Inspectors:"+Inspectors);
    console.log("save_inspectiongroup() inspectionGroup_AutoFeed:"+inspectionGroup_AutoFeed);

    return jQuery.ajax({
    url: '/php/inspectionpath.php',
    type: 'POST',
    contentType: "application/json",
    dataType: "json",
    data: JSON.stringify(json),
    success:
        function(response)
        {
            console.log(response);

            gl_active_group_index = index;
            gl_edit_group_info = -1;
            gl_show_create_inspectionpoint_btn = -1;
            setTimeout(reload_inspectionGroup, 100);
        },
    error:
        function(xhr, status, error)
        {
           console.log(error);
           console.log(xhr.responseText);
        }
    });
}

function create_inspectionpoint(GroupID, active_tab_index, newPointIndex)
{
    var inspectionpoint_anchor = $("#create_dropdown_"+GroupID+"_show").attr("data-id");
    var inspectionpoint_Distance = document.getElementById("Distance_"+GroupID).value;
    var inspectionpoint_MinTime = document.getElementById("MinTime_"+GroupID).value;
    var inspectionpoint_MaxTime = document.getElementById("MaxTime_"+GroupID).value;
    var inspectionpoint_Message = document.getElementById("Message_"+GroupID).value;

    var inspectionpoint_TravelTime = 0;
    var inspectionpoint_TravelTime_ui = document.getElementById("TravelTime_"+GroupID);
    if( inspectionpoint_TravelTime_ui != undefined)
        inspectionpoint_TravelTime = inspectionpoint_TravelTime_ui.value;

    //console.log("create_checkpoint_path() anchor:"+inspectionpoint_anchor+", TravelTime:"+inspectionpoint_TravelTime);
    //console.log("create_checkpoint_path() Distance:"+inspectionpoint_Distance +", MinTime:"+inspectionpoint_MinTime +", MaxTime:"+inspectionpoint_MaxTime);

    var json = {};
    json['CREATE_POINT'] = 1;
    json['project_id'] = ''+project_id;
    json['GroupID'] = GroupID;
    json['PointIndex'] = newPointIndex;
    json['AnchorNodeId'] = inspectionpoint_anchor;
    json['Distance'] = inspectionpoint_Distance;
    json['MinTime'] = inspectionpoint_MinTime;
    json['MaxTime'] = inspectionpoint_MaxTime;
    json['Message'] = inspectionpoint_Message;
    json['TravelTime'] = inspectionpoint_TravelTime;

    return jQuery.ajax({
    url: '/php/inspectionpath.php',
    type: 'POST',
    contentType: "application/json",
    dataType: "json",
    data: JSON.stringify(json),
    success:
        function(response)
        {
            console.log(response);

            gl_active_group_index = active_tab_index;
            gl_show_create_inspectionpoint_btn = -1;
            setTimeout(reload_inspectionGroup, 100);

        },
    error:
        function(xhr, status, error)
        {
           console.log(error);
           console.log(xhr.responseText);
        }
    });

}

function delete_inspectionpoint_dialog(NodeId, NodeName, inspectionGroupID, inspectionPointID, active_tab_index)
{
    if (confirm('Are you sure you want to delete this checkpoint:'+NodeName+'?'))
    {
        // delete it!
        delete_inspectionpoint(inspectionGroupID, inspectionPointID, active_tab_index);
    }
    else
    {
        // Do nothing!
    }
}

function delete_inspectionpoint(inspectionGroupID, inspectionPointID, active_tab_index)
{
    var SendData = "GroupID="+inspectionGroupID;
    SendData += "&PointID="+inspectionPointID;
    SendData += "&project_id="+project_id;
    SendData += "&delete_inspectionpoint=1";

     return jQuery.ajax({
      url: '/php/inspectionpath.php',
      type: 'POST',
      data: SendData,
      dataType: 'text',
    // return jQuery.ajax({
    //  url: '/php/inspectionpath.php?GroupID='+inspectionGroupID+'&PointID='+inspectionPointID+'&project_id='+project_id,
    //  type: 'delete',
    //  contentType: "application/json",
    //  dataType: "json",
    //  data: {},
     success:
      function(response)
      {
           //alert("event_trig_delete success, event_id="+event_id+",result="+result.eventid);
           console.log(JSON.stringify(response));
           gl_active_group_index = active_tab_index;
           gl_show_create_inspectionpoint_btn = -1;
           setTimeout(reload_inspectionGroup, 100);
      },
     error:
      function(xhr, status, error)
      {
           console.log(error);
           console.log(xhr.responseText);
           gl_active_group_index = active_tab_index;
           gl_show_create_inspectionpoint_btn = -1;
           setTimeout(reload_inspectionGroup, 100);
      }
   });
}

function edit_traveltime_dialog(TravelTime, GroupID, PointID, PointIndex, active_tab_index)
{
    var newValue = prompt("編輯巡檢點轉換時間(秒)", TravelTime);
    if ( newValue )
    {
        if (!isANumber(newValue))
        {
            alert("Error format: 請填入數字!");
            return;
        }
        else
        {
            var json = {};
            json['UPDATE_TRAVELTIME'] = 1;
            json['project_id'] = ''+project_id;
            json['GroupID'] = GroupID;
            json['PointID'] = PointID;
            json['PointIndex'] = PointIndex;
            json['TravelTime'] = newValue;

            return jQuery.ajax({
            url: '/php/inspectionpath.php',
            type: 'POST',
            contentType: "application/json",
            dataType: "json",
            data: JSON.stringify(json),
            success:
                function(response)
                {
                    console.log(response);

                    gl_active_group_index = active_tab_index;
                    setTimeout(reload_inspectionGroup, 100);
                },
            error:
                function(xhr, status, error)
                {
                   console.log(error);
                   console.log(xhr.responseText);
                }
            });
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

function isANumber(str)
{
    return !/\D/.test(str);
}

function open_inspection_page()
{
    var win = window.open('/index.php/vilsinspection/'+project_id, '_blank');
    win.focus();
}
