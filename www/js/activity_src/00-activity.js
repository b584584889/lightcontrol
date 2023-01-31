/*
 * Author: ikki Chung
 * Date: 2017.07.13
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


//
// globaa value
//
var allDevicePoints = {};
var allDeviceAliveTime = {};

var glAnchor_List = [];
var glTag_List = [];
var glCoord_List = [];
var glLocator_List = [];

var gl_anchor_num = 0;
var gl_tag_num = 0;
var gl_coord_num = 0;
var gl_locator_num = 0;

var gl_anchor_load_index = 0;
var gl_tag_load_index = 0;
var gl_coord_load_index = 0;
var gl_locator_load_index = 0;

var LOAD_DEVICE_NUM = 20;
var UPDATE_INTERVAL = 1000; // update display every 1 seconds

function create_table_item(prefix, index, ui_index, nodeData, showAlive)
{
    if (nodeData == null)
        return null;

    if (nodeData == undefined)
        return null;

    var node_alive = nodeData[1];
    var type = nodeData[2];
    var node_type = get_node_type(type);
    var node_index = nodeData[0];
    var node_macaddress = nodeData[6];

    var devicealivetime = allDeviceAliveTime[node_macaddress];
    if ( devicealivetime == null )
        devicealivetime = "";

        //console.log("create_table_item() node_macaddress:"+node_macaddress+", node_alive:"+node_alive +", devicealivetime:"+devicealivetime);

    var mpoints = allDevicePoints[node_macaddress];
    if ( mpoints == null )
        mpoints = [];

    var UI_ROW_ID = "row_"+prefix+"_"+node_macaddress;
    var UI_ROW_UPDATETIME_ID = "row_updatetime_"+prefix+"_"+node_macaddress;
    var UI_ROW_ACTIVITY_ID = "row_activity_"+prefix+"_"+node_macaddress;
    var row = document.getElementById(UI_ROW_ID);
    if ( row != null )
    {
        // update time
        var SpanUpdatetime = document.getElementById(UI_ROW_UPDATETIME_ID);
        SpanUpdatetime.textContent = devicealivetime;

        var SpanActivity = document.getElementById(UI_ROW_ACTIVITY_ID);
        //SpanActivity.innerHTML = mpoints.toString();

        //console.log("create_table_item() mpoints:"+mpoints.toString());

        var idname = "#"+UI_ROW_ACTIVITY_ID;
        $(idname).sparkline(mpoints, {width: mpoints.length * 4, tooltipSuffix: ""});

        return null;
    }

    var row = document.createElement('tr');
    row.setAttribute("id", UI_ROW_ID);
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // type + index
    row.appendChild(document.createElement('td'));
    var Span2 = document.createElement("span");

    if (type == '0')
        Span2.setAttribute("class","badge bg-green");
    if (type == '1')
        Span2.setAttribute("class","badge bg-blue");
    if (type == '2')
        Span2.setAttribute("class","badge bg-red");
    if (type == '3')
        Span2.setAttribute("class","badge bg-yellow");

    Span2.textContent = node_type;
    row.cells[cellIndex].appendChild(Span2);
    var SpanIndex = document.createElement("span");
    SpanIndex.setAttribute("class","badge bg-purple-active");
    if (showAlive)
    {
        if (node_index == '-1')
            SpanIndex.setAttribute("class","badge bg-maroon-active");
        SpanIndex.textContent = node_index;
    }
    else
    {
        SpanIndex.textContent = index;
    }
    row.cells[cellIndex].appendChild(SpanIndex);
    cellIndex++;

    //MAC address
    row.appendChild(document.createElement('td'));
    var Spanmac = document.createElement("span");
    Spanmac.setAttribute("class","badge bg-light-yellow");
    Spanmac.textContent = node_macaddress;
    row.cells[cellIndex].appendChild(Spanmac);
    cellIndex++;

    //	Update Time
    row.appendChild(document.createElement('td'));
    var SpanTime = document.createElement("span");
    SpanTime.setAttribute("id", UI_ROW_UPDATETIME_ID);
    SpanTime.setAttribute("class","badge bg-light-yellow");
    SpanTime.textContent = devicealivetime;
    row.cells[cellIndex].appendChild(SpanTime);
    cellIndex++;

    //activity
    row.appendChild(document.createElement('td'));
    var SpanActivity = document.createElement("span");
    SpanActivity.setAttribute("id", UI_ROW_ACTIVITY_ID);
    //SpanActivity.setAttribute("class","sparkline-1");
    //SpanActivity.setAttribute("id","activity"+node_macaddress);
    //SpanActivity.innerHTML = mpoints.toString();
    //SpanActivity.innerHTML = "8,4,0,0,0,0,1,4,4,10,10,10,10,0,0,0,4,6,5,9,10";
    row.cells[cellIndex].appendChild(SpanActivity);
    cellIndex++;

    //<td><span id="activity{{node.macaddress}}">Loading..</span></td>
    //
    var idname = "#"+UI_ROW_ACTIVITY_ID;
    $(idname).sparkline(mpoints, {width: mpoints.length * 4, tooltipSuffix: ""});

    return row;
}

function add_node_to_activity_table(table, prefix, nodelist, start_index)
{
    var show_count = 0;
    for (var i = 0; i < nodelist.length; i++, start_index++)
    {
        var nodeData = nodelist[i];
        var node_macaddress = nodeData[6];
        var node_alive = nodeData[1];

        var mpoints = allDevicePoints[node_macaddress];
        if ( mpoints == null )
            mpoints = [];

        mpoints.push(node_alive);
        if (mpoints.length > 100)
            mpoints.splice(0, 1);

        if (node_alive != "0")
        {
            var dateFormat = 'YYYY-MM-DD HH:mm:ss';
            var local_date = moment.utc().local();
            devicealivetime = local_date.format(dateFormat);
            allDeviceAliveTime[node_macaddress] = devicealivetime;

        }
        //console.log("add_node_to_activity_table() node_macaddress:"+node_macaddress+", node_alive:"+node_alive +", devicealivetime:"+devicealivetime);

        allDevicePoints[node_macaddress] = mpoints;

        var row = create_table_item(prefix, i, start_index, nodeData, true);
        if (row != null)
        {
            table.appendChild(row);
            //$('.sparkline-1').sparkline();
        }
        show_count++;
    }
    return show_count;
}

function drawActivity()
{
    if ( document.getElementById("TAG_NODE_ACTIVITY_TABLE_BODY") == null )
        return;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_anchor_num = parseInt(data['LOC_ANCHOR_NUM']);
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_coord_num = parseInt(data['LOC_COORD_NUM']);
        gl_locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        gl_anchor_load_index = 0;
        gl_tag_load_index = 0;
        gl_coord_load_index = 0;
        gl_locator_load_index = 0;
    })
    .success(function() {
        setTimeout(load_anchor, 10);
        //setTimeout(load_tag, 300);
        //setTimeout(load_coord, 400);
        //setTimeout(load_locator, 500);
        //setTimeout(update_sparkline, 500);
    })
    .error(function() {
        setTimeout(drawActivity, 1000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_anchor()
{
    // if (gl_anchor_load_index >= gl_anchor_num)
    // {
    //     gl_anchor_load_index = 0;
    //     setTimeout(load_anchor, UPDATE_INTERVAL);
    //     return;
    // }
    if (gl_anchor_load_index == 0)
    {
        glAnchor_List = [];
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
            var node = data[key];
            var nodeData = node.split(",");
            glAnchor_List.push(nodeData);
        }

        gl_anchor_load_index += anchor_num;

    })
    .success(function() {
        if (gl_anchor_load_index >= gl_anchor_num)
        {
            gl_anchor_load_index = 0;
            var loc_anchor_table = document.getElementById('ANCHOR_NODE_ACTIVITY_TABLE_BODY');
            // while (loc_anchor_table.rows.length >= 1 && gl_anchor_num > 0)
            // {
            //     loc_anchor_table.deleteRow(0);
            // }
            var index = 0;
            add_node_to_activity_table(loc_anchor_table, "LOC", glAnchor_List, index);

            setTimeout(load_tag, 1);
        }
        else
        {
            setTimeout(load_anchor, 1);
        }
    })
    .error(function() {
        gl_anchor_load_index = 0;
        setTimeout(load_anchor, UPDATE_INTERVAL);
    })
    .complete(function() {

    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_tag()
{
    // if (gl_tag_load_index >= gl_tag_num)
    // {
    //     gl_tag_load_index = 0;
    //     setTimeout(load_tag, UPDATE_INTERVAL);
    //     return;
    // }
    if (gl_tag_load_index == 0)
    {
        glTag_List = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadtag=1&s='+gl_tag_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var tag_num = parseInt(data['LOC_TAG_NUM']);

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_tag_load_index; i < gl_tag_load_index+tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            var node = data[key];
            var nodeData = node.split(",");
            glTag_List.push(nodeData);
        }

        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            gl_tag_load_index = 0;
            var loc_tag_table = document.getElementById('TAG_NODE_ACTIVITY_TABLE_BODY');
            // while (loc_tag_table.rows.length >= 1 && gl_tag_num > 0)
            // {
            //     loc_tag_table.deleteRow(0);
            // }
            var index = 0;
            add_node_to_activity_table(loc_tag_table, "LOC", glTag_List, index);

            setTimeout(load_coord, 1);
        }
        else
        {
            setTimeout(load_tag, 1);
        }
    })
    .error(function() {
        gl_tag_load_index = 0;
        setTimeout(load_tag, UPDATE_INTERVAL);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}


function load_coord()
{
    // if (gl_coord_load_index >= gl_coord_num)
    // {
    //     gl_coord_load_index = 0;
    //     setTimeout(load_coord, UPDATE_INTERVAL);
    //     return;
    // }
    if (gl_coord_load_index == 0)
    {
        glCoord_List = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadcoord=1&s='+gl_coord_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var coord_num = parseInt(data['LOC_COORD_NUM']);

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_coord_load_index; i < gl_coord_load_index+coord_num; ++i)
        {
            var key = 'LOC_COORD_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;
            var node = data[key];
            var nodeData = node.split(",");
            glCoord_List.push(nodeData);
        }

        gl_coord_load_index += coord_num;

    })
    .success(function() {
        if (gl_coord_load_index >= gl_coord_num)
        {
            gl_coord_load_index = 0;
            var loc_coord_table = document.getElementById('COORD_NODE_ACTIVITY_TABLE_BODY');
            // while (loc_coord_table.rows.length >= 1 && gl_coord_num > 0)
            // {
            //     loc_coord_table.deleteRow(0);
            // }
            var index = 0;
            add_node_to_activity_table(loc_coord_table, "LOC", glCoord_List, index);

            setTimeout(load_locator, 1);
        }
        else
        {
            setTimeout(load_coord, 1);
        }
    })
    .error(function() {
        gl_coord_load_index = 0;
        setTimeout(load_coord, UPDATE_INTERVAL);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_locator()
{
    // if (gl_locator_load_index >= gl_locator_num)
    // {
    //     gl_locator_load_index = 0;
    //     setTimeout(load_anchor, UPDATE_INTERVAL);
    //     return;
    // }
    if (gl_locator_load_index == 0)
    {
        glLocator_List = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadlocator=1&s='+gl_locator_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_locator_load_index; i < gl_locator_load_index+locator_num; ++i)
        {
            var key = 'LOC_LOCATOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;
            var node = data[key];
            var nodeData = node.split(",");
            glLocator_List.push(nodeData);
        }

        gl_locator_load_index += locator_num;

    })
    .success(function() {
        if (gl_locator_load_index >= gl_locator_num)
        {
            gl_locator_load_index = 0;
            var loc_locator_table = document.getElementById('LOCATOR_NODE_ACTIVITY_TABLE_BODY');
            // while (loc_locator_table.rows.length >= 1 && gl_locator_num > 0)
            // {
            //     loc_locator_table.deleteRow(0);
            // }
            var index = 0;
            add_node_to_activity_table(loc_locator_table, "LOC", glLocator_List, index);
            //$('.sparkline-1').sparkline();
            setTimeout(load_anchor, UPDATE_INTERVAL);
        }
        else
        {
            setTimeout(load_locator, 1);
        }
    })
    .error(function() {
        gl_locator_load_index = 0;
        setTimeout(load_anchor, UPDATE_INTERVAL);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

 //
 // start draw
 //
 drawActivity();



 function get_node_type(type)
 {
     var node_type = "";
     if (type == '0')
         node_type = "ANCHOR";
     else
     if (type == '1')
         node_type = "TAG";
     else
     if (type == '2')
         node_type = "COORD";
     else
     if (type == '3')
         node_type = "LOCATOR";
     return node_type;
 }

function open_activity_page()
{
    var win = window.open('/index.php/vilsactivity/'+project_id, '_blank');
    win.focus();
}
