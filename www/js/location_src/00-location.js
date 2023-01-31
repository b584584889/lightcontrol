/*
 * Author: ikki Chung
 * Date: 2017.12.06
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


//
// globa value
//
var glAllAreas = [];// put area info
var glTagLocation = []; // map table , glTagLocation[Areaname] = tagList;
var glUserLocation = [];

var gl_tag_num = 0;
var gl_tag_load_index = 0;
var gl_user_load_index = 0;
var LOAD_DEVICE_NUM = 20;

//
// start load location
//
load_maparea();

function create_location_table_item(ui_index, area, areatype, areaid, nodelist, userlist)
{
    if (area == null)
        return null;

    if (nodelist == undefined && userlist == undefined)
        return null;

    var total_count = 0;
    if (nodelist != undefined)
    {
        total_count += nodelist.length;
    }

    if (userlist != undefined)
    {
        total_count += userlist.length;
    }

    var row = document.createElement('tr');
    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Area name
    var tdName = document.createElement('td');
    tdName.setAttribute("style","height: 90px;");
    row.appendChild(tdName);
    var Spanmac = document.createElement("span");
    //Spanmac.setAttribute("class","badge bg-green-active");
    Spanmac.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    Spanmac.textContent = area;
    row.cells[cellIndex].appendChild(Spanmac);
    cellIndex++;

    // Area type
    var tdType = document.createElement('td');
    tdName.setAttribute("style","height: 90px;");
    row.appendChild(tdType);
    var SpanType = document.createElement("span");
    //Spanmac.setAttribute("class","badge bg-green-active");
    SpanType.setAttribute("style","font-size:14px; font-weight:bold; color:#330033;");
    if (areatype == '0')
        SpanType.setAttribute("class","badge bg-yellow-active");
    if (areatype == '1')
        SpanType.setAttribute("class","badge bg-fuchsia");
    if (areatype == '2')
        SpanType.setAttribute("class","badge bg-green-active");
    if (areatype == '3')
        SpanType.setAttribute("class","badge bg-purple");
    if (areatype == '4')
        SpanType.setAttribute("class","badge bg-teal-active");
    if (areatype == '5')
        SpanType.setAttribute("class","badge bg-green-active");
    if (areatype == '6')
        SpanType.setAttribute("class","badge bg-purple");


        // SpanType.setAttribute("class","badge bg-light-blue-active");
    SpanType.textContent = get_Areatype(areatype);

    if (!(area === 'Unknown') && (areatype == '0' || areatype == '1' || areatype == '2' ||areatype == '3' ||areatype == '4' ||areatype == '5' ||areatype == '6'))
    {
        var btnRollCall = document.createElement('button');
        btnRollCall.setAttribute("class","btn btn-danger pull-right");
        btnRollCall.setAttribute("onclick","window.open('"+gl_target_server+"/index.php/vilsrollcall/"+project_id+"?areaid="+areaid+"','_blank');");
        btnRollCall.textContent = "產生報告";
        row.cells[cellIndex].appendChild(btnRollCall);
    }

    row.cells[cellIndex].appendChild(SpanType);
    cellIndex++;


    // Node Count
    row.appendChild(document.createElement('td'));

    //========
    var DivColorPaletteSet = document.createElement("div");
    DivColorPaletteSet.setAttribute("class","color-palette-set");
    row.cells[cellIndex].appendChild(DivColorPaletteSet);

    // color-palette Orange
    var DivColorPalette1 = document.createElement("div");
    //DivColorPalette1.setAttribute("class","bg-orange disabled color-palette");
    DivColorPalette1.setAttribute("style","text-align:center;");
    DivColorPaletteSet.appendChild(DivColorPalette1);

    var Span1 = document.createElement("span");
    Span1.textContent = total_count;
    Span1.setAttribute("style","font-size:18px; font-weight:bold; color:#003300;");
    DivColorPalette1.appendChild(Span1);

    //========
    cellIndex++;

    // node id / name
    row.appendChild(document.createElement('td'));

    if (nodelist != undefined)
    {
        var tagsCol = create_tag_node(nodelist);
        row.cells[cellIndex].appendChild(tagsCol);
    }
    if (userlist != undefined)
    {
        var usersCol = create_user_node(userlist);
        row.cells[cellIndex].appendChild(usersCol);
    }

    cellIndex++;

    return row;
}

function create_tag_node(nodelist)
{
    var tagsCol = document.createElement("div");

    for (var i = 0; i < nodelist.length; ++i)
    {
        var nodeData = nodelist[i];

        var nodeid = nodeData[3];
        var nodename = decodeURIComponent(nodeData[5]);
        var area_unknown = nodeData[28];

        var connected = true;
        if (nodeData[0] == '-1')
            connected = false;

        var alive = nodeData[1];

        var BlockColor = "bg-teal";
        if (connected == true)
        {
            BlockColor = "bg-purple";
        }
        // if ( alive == '0' || area_unknown == '1')
        if ( alive == '0')
        {
            BlockColor = "bg-light-blue";
        }

        var DivCol = document.createElement("div");
        DivCol.setAttribute("class","col-sm-4 col-md-2");
        tagsCol.appendChild(DivCol);

        var DivColorPaletteSet = document.createElement("div");
        DivColorPaletteSet.setAttribute("class","color-palette-set");
        DivCol.appendChild(DivColorPaletteSet);

        //========
        // color-palette 1
        var DivColorPalette1 = document.createElement("div");
        DivColorPalette1.setAttribute("class",BlockColor + " disabled color-palette");
        DivColorPalette1.setAttribute("style","text-align:center;");
        DivColorPaletteSet.appendChild(DivColorPalette1);

        var Span1 = document.createElement("span");
        Span1.textContent = nodeid;
        Span1.setAttribute("style","font-size:18px; font-weight:bold; color:#e6f5ff;");
        DivColorPalette1.appendChild(Span1);
        //========

        //========
        // color-palette 2
        var DivColorPalette2 = document.createElement("div");
        DivColorPalette2.setAttribute("class",BlockColor + " color-palette");
        DivColorPalette2.setAttribute("style","text-align:center;");
        DivColorPaletteSet.appendChild(DivColorPalette2);

        var Span2 = document.createElement("span");
        Span2.textContent = nodename;
        Span2.setAttribute("style","font-size:16px; font-weight:bold; color:#e6f5ff;");
        DivColorPalette2.appendChild(Span2);
        //========

        //========
        // color-palette 3
        var DivColorPalette3 = document.createElement("div");
        DivColorPalette3.setAttribute("class",BlockColor + "-active color-palette");
        DivColorPalette3.setAttribute("style","text-align:center;");
        DivColorPaletteSet.appendChild(DivColorPalette3);

        var Span3 = document.createElement("span");
        Span3.textContent = alive;
        Span3.setAttribute("style","font-size:14px; font-weight:bold; color:#e6f5ff;");
        DivColorPalette3.appendChild(Span3);
        //========

        //========
        // color-palette 4
        var DivColorPalette4 = document.createElement("div");
        DivColorPalette4.setAttribute("class",BlockColor + "-active color-palette");
        DivColorPalette4.setAttribute("style","text-align:center;");
        DivColorPaletteSet.appendChild(DivColorPalette4);

        var Span4 = document.createElement("span");
        Span4.textContent = "";
        //Span3.setAttribute("style","font-size:14px; font-weight:bold; color:#020202;");

        var Span4_href = document.createElement("a");
        Span4_href.setAttribute("href","javascript:;");
        Span4_href.setAttribute("onclick","show_on_2dmap(\""+nodeid+"\");return false;");
        Span4_href.setAttribute("style","font-size:14px; color:#eeeeee;");
        Span4_href.title = '跳轉到2D地圖';
        Span4_href.textContent = '查看地圖';
        Span4.appendChild(Span4_href);

        DivColorPalette4.appendChild(Span4);
        //========

        //========
        // color-palette 5
        var DivColorPalette5 = document.createElement("div");
        DivColorPalette5.setAttribute("style","text-align:center; color:#e6f5ff;");
        DivColorPaletteSet.appendChild(DivColorPalette5);

        var Span5 = document.createElement("span");
        Span5.textContent = "-";
        Span5.setAttribute("style","font-size:14px; font-weight:bold; color:#e6f5ff;");
        DivColorPalette5.appendChild(Span5);
        //========

    }
    return tagsCol;
}

function create_user_node(userlist)
{
    var usersCol = document.createElement("div");

    for (var i = 0; i < userlist.length; ++i)
    {
        var user = userlist[i];
        var BlockColor = "bg-purple";

        var DivCol = document.createElement("div");
        DivCol.setAttribute("class","col-sm-4 col-md-2");
        usersCol.appendChild(DivCol);

        var DivColorPaletteSet = document.createElement("div");
        DivColorPaletteSet.setAttribute("class","color-palette-set");
        DivCol.appendChild(DivColorPaletteSet);

        //========
        // color-palette 1
        var DivColorPalette1 = document.createElement("div");
        DivColorPalette1.setAttribute("class",BlockColor + " disabled color-palette");
        DivColorPalette1.setAttribute("style","text-align:center;");
        DivColorPaletteSet.appendChild(DivColorPalette1);

        var Span1 = document.createElement("span");
        Span1.textContent = user.userid;
        Span1.setAttribute("style","font-size:18px; font-weight:bold; color:#e6f5ff;");
        DivColorPalette1.appendChild(Span1);
        //========

        //========
        // color-palette 2
        var DivColorPalette2 = document.createElement("div");
        DivColorPalette2.setAttribute("class",BlockColor + " color-palette");
        DivColorPalette2.setAttribute("style","text-align:center;");
        DivColorPaletteSet.appendChild(DivColorPalette2);

        var Span2 = document.createElement("span");
        Span2.textContent = user.name;
        Span2.setAttribute("style","font-size:16px; font-weight:bold; color:#e6f5ff;");
        DivColorPalette2.appendChild(Span2);
        //========

        //========
        // color-palette 3
        var DivColorPalette3 = document.createElement("div");
        DivColorPalette3.setAttribute("class",BlockColor + "-active color-palette");
        DivColorPalette3.setAttribute("style","text-align:center;");
        DivColorPaletteSet.appendChild(DivColorPalette3);

        var Span3 = document.createElement("span");
        // Span3.textContent = 1;
        Span3.setAttribute("style","font-size:14px; font-weight:bold; color:#e6f5ff;");
        DivColorPalette3.appendChild(Span3);
        //========

        //========
        // color-palette 4
        var DivColorPalette4 = document.createElement("div");
        DivColorPalette4.setAttribute("class",BlockColor + "-active color-palette");
        DivColorPalette4.setAttribute("style","text-align:center;");
        DivColorPaletteSet.appendChild(DivColorPalette4);

        var Span4 = document.createElement("span");
        Span4.textContent = "";
        //Span3.setAttribute("style","font-size:14px; font-weight:bold; color:#020202;");

        var Span4_href = document.createElement("a");
        Span4_href.setAttribute("href","javascript:;");
        Span4_href.setAttribute("onclick","show_user_on_2dmap(\""+user.userid+"\");return false;");
        Span4_href.setAttribute("style","font-size:14px; color:#eeeeee;");
        Span4_href.title = '跳轉到2D地圖';
        Span4_href.textContent = '查看地圖';
        Span4.appendChild(Span4_href);

        DivColorPalette4.appendChild(Span4);
        //========

        //========
        // color-palette 5
        var DivColorPalette5 = document.createElement("div");
        DivColorPalette5.setAttribute("style","text-align:center; color:#e6f5ff;");
        DivColorPaletteSet.appendChild(DivColorPalette5);

        var Span5 = document.createElement("span");
        Span5.textContent = "-";
        Span5.setAttribute("style","font-size:14px; font-weight:bold; color:#e6f5ff;");
        DivColorPalette5.appendChild(Span5);
        //========
    }

    return usersCol;
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
            glAllAreas[index] = {};
            glAllAreas[index].itemid = area_json.properties.areaid;
            glAllAreas[index].itemname = area_json.properties.areaname;
            glAllAreas[index].itemtype = area_json.properties.areatype;

            //glTagLocation[glAllAreas[index].itemname] = [];
            glTagLocation[glAllAreas[index].itemid] = [];

            // console.log("itemid:"+glAllAreas[index].itemid+", itemname:"+glAllAreas[index].itemname+", itemtype:"+glAllAreas[index].itemtype);

            index++;
        });

        //
        // sory by areaname
        //
        sortAreaList();

        // add default empty area
        glAllAreas[index] = {};
        glAllAreas[index].itemid = "Unknown";
        glAllAreas[index].itemname = "Unknown";
        glAllAreas[index].itemtype = 0;
        glTagLocation["Unknown"] = [];

        updateLocation();
    });
}

function load_Tags_Info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_tag_load_index = 0;
    })
    .success(function() {
        clear_old_data();
        setTimeout(load_Tags, 10);
    })
    .error(function() {
        setTimeout(load_Tags_Info, 1000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}


function load_Tags()
{
    if (gl_tag_load_index >= gl_tag_num)
    {
        gl_tag_load_index = 0;
        draw_table();
        setTimeout(load_Tags_Info, 500); // update display every 500 ms
        return;
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
            // split data
            var node = data[key];
            var nodeData = node.split(",");
            var alive = nodeData[1];
            var nodename = nodeData[5];
            var areaid = nodeData[19];
            var areaname = nodeData[20];
            var area_unknown = nodeData[28];
            var area_Type5Id = nodeData[33];
            var area_Type5Name = nodeData[34];
            var asleep = nodeData[35];

            //if (areaname.length == 0 || (alive === '0') || (area_unknown === '1'))
            if ((areaname.length == 0 && area_Type5Name.length == 0 ) || (area_unknown === '1'))
            {
                areaid = "Unknown";
                areaname = "Unknown";
                area_Type5Name = "";
                area_Type5Id = "";
            }
            else
            if ( (alive == '0') && (asleep == '0') )
            {
                areaid = "Unknown";
                areaname = "Unknown";
                area_Type5Name = "";
                area_Type5Id = "";
            }

            // console.log("updateLocation() nodename:"+nodename+" areaname:"+areaname+" areaid:"+areaid+" area_Type5Name:"+area_Type5Name+" area_Type5Id:"+area_Type5Id);

            if ( area_Type5Id.length > 0 )
            {
                if ( glTagLocation[areaid] != undefined )
                    glTagLocation[areaid].push(nodeData);

                if ( areaid === area_Type5Id )
                {
                    // same area
                }
                else
                {
                    if ( glTagLocation[area_Type5Id] != undefined )
                        glTagLocation[area_Type5Id].push(nodeData);
                }
            }
            else
            {
                if ( glTagLocation[areaid] != undefined )
                {
                    glTagLocation[areaid].push(nodeData);
                }
                else
                {
                    glTagLocation["Unknown"].push(nodeData);
                }
            }
        }
        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            gl_tag_load_index = 0;
            draw_table();
            setTimeout(load_Tags_Info, 500); // update display every 500 ms
        }
        else
        {
            setTimeout(load_Tags, 10);
        }
    })
    .error(function() {
        gl_tag_load_index = 0;
        setTimeout(load_Tags, 1000);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);

}

function runLive_user()
{
    if (gl_user_load_index == 0)
    {
        glUserLocation = [];
    }

    var load_user_num = 0;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduserpoistioninfo=1&s='+gl_user_load_index+'&count='+500+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, userinfo)
        {
            load_user_num += 1;
            gl_user_load_index += 1;

            var areaid = userinfo.AreaId;
            var areaname = userinfo.AreaName;

            if ( areaid != undefined &&  areaid.length > 0 )
            {
                if (glUserLocation[areaid] == undefined)
                    glUserLocation[areaid] = [];

                glUserLocation[areaid].push(userinfo);
            }
            else
            {
                if (glUserLocation["Unknown"] == undefined)
                    glUserLocation["Unknown"] = [];
                glUserLocation["Unknown"].push(userinfo);
            }
        });
    })
    .success(function() {

        if (load_user_num < 500)
        {
            gl_user_load_index = 0;
            setTimeout(runLive_user, 1000);
        }
        else
        {
            setTimeout(runLive_user, 50);
        }
    })
    .error(function() {
        gl_user_load_index = 0;
        setTimeout(runLive_user, 500);
    })
    .complete(function() {
    });
}

function updateLocation()
{
    load_Tags_Info();
    runLive_user();
}

function draw_table()
{
    //console.log("draw_table() glTagLocation.length:"+glTagLocation.length);

    var loc_table = document.getElementById('NODE_LOCATION_TABLE_BODY');

    while (loc_table.rows.length >= 1)
         loc_table.deleteRow(0);

    for (var i = 0; i < glAllAreas.length; ++i)
    {
        var area = glAllAreas[i];
        //console.log("draw_table() glTagLocation["+area.itemname+"].length:"+glTagLocation[area.itemname].length);

        //var list_tag = glTagLocation[area.itemname];
        var list_tag = glTagLocation[area.itemid];
        var list_user = glUserLocation[area.itemid];
        var row = create_location_table_item(i, area.itemname, area.itemtype, area.itemid, list_tag, list_user);
        loc_table.appendChild(row);
    }
}

//
// sory by areaname
//
function sortAreaList()
{
    for (var i = 0; i < glAllAreas.length; i++)
    {
        var area1 = glAllAreas[i];

        for (var j = i+1; j < glAllAreas.length; j++)
        {
            var area2 = glAllAreas[j];

            if (area1.itemname > area2.itemname)
            {
                // swap
                var tmpitemid = area1.itemid;
                var tmpitemname = area1.itemname;
                var tmpitemtype = area1.itemtype;
                area1.itemid = area2.itemid;
                area1.itemname = area2.itemname;
                area1.itemtype = area2.itemtype;
                glAllAreas[i] = area1;

                glAllAreas[j].itemid = tmpitemid;
                glAllAreas[j].itemname = tmpitemname;
                glAllAreas[j].itemtype = tmpitemtype;
            }
        }
    }
}

function clear_old_data()
{
    for (var i = 0; i < glAllAreas.length; ++i)
    {
        var area = glAllAreas[i];
        glTagLocation[area.itemid] = [];
        //glTagLocation[area.itemname] = [];
    }
}

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
    return node_type;
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

function show_on_2dmap(nodeid)
{
    window.open("/index.php/vils2dmap/"+project_id+"?nodeid="+nodeid,"_self")
}

function show_user_on_2dmap(userid)
{
    window.open("/index.php/vils2dmap/"+project_id+"?userid="+userid,"_self")
}

function open_location_page()
{
    var win = window.open('/index.php/vilslocation/'+project_id, '_blank');
    win.focus();
}
