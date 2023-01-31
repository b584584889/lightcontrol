

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var gl_updating_element_id = "";
var edit_node_info = false;
var stop_update_anchor_info = false;
var stop_update_tag_info = false;
var stop_update_coord_info = false;
var stop_update_locator_info = false;
var stop_update_gateway_info = false;

var gl_showfilter_showonline = true;
var gl_showfilter_showoffline = true;

var gl_anchor_num = 0;
var gl_tag_num = 0;
var gl_coord_num = 0;
var gl_locator_num = 0;

var gl_anchor_load_index = 0;
var gl_tag_load_index = 0;
var gl_coord_load_index = 0;
var gl_locator_load_index = 0;

var gl_OnLinelist_anchor = [];
var gl_OnLinelist_tag = [];
var gl_OnLinelist_coord = [];
var gl_OnLinelist_locator = [];
var gl_OnLinelist_gateway = [];

var gl_notOnLinelist_anchor = [];
var gl_notOnLinelist_tag = [];
var gl_notOnLinelist_coord = [];
var gl_notOnLinelist_locator = [];
var gl_notOnLinelist_gateway = [];

// var gl_resident_info = [];
// var gl_employee_info = [];

var gl_user_num = 0;
var gl_user_load_index = 0;
var gl_user_info = [];

var glMapLayerList = [];
var gl_prj_info = [];
var gl_tag_prj_info = [];
var gl_anchor_prj_info = [];
var gl_coord_prj_info = [];
var gl_locator_prj_info = [];
var gl_gateway_prj_info = [];

var glSendATcmdTimestamp = getCurrentTimeStamp();

var LOAD_DEVICE_NUM = 50;
var LOAD_USER_NUM = 500;

// start loading
check_loading_status();

function check_loading_status()
{
    // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" CurrentLoadIndex:" + CurrentLoadIndex);
    switch(CurrentLoadIndex)
    {
        case 0:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                load_maplayer_info();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
                //load_resident_info();
                //load_employee_info();
                load_user_info();
                load_node_info_all();
                load_loragateway_info();
                CurrentLoadIndex = 2; // next state
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_prj_info();
                load_node_prj_info();

                CurrentLoadIndex = LOAD_STATE_END;
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

function load_maplayer_info()
{
    glMapLayerList = [];

    var targetURL = gl_target_server + "/php/map.php";
    $.getJSON(targetURL, 'loadmap=1'+'&project_id='+project_id, function(mapsdata)
    {
        $.each(mapsdata, function(index, mapdata)
        {
            glMapLayerList.push(mapdata);
        });

        //console.log("load_map_config() glMapLayerList.length:"+glMapLayerList.length);

        CurrentLoadIndex = CurrentLoadIndex + 1; // next state

    });
    return false;
}

function load_user_info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'usercount=1'+'&project_id='+project_id, function(data)
    {
        gl_user_num = parseInt(data['USER_NUM']);
        gl_user_load_index = 0;

        gl_user_info = [];
    })
    .success(function() {
        setTimeout(load_user, 500);
    })
    .error(function() {
        setTimeout(load_user_info, 1000);
    })
    .complete(function() {
    });
}

function load_user()
{
    if (gl_user_load_index >= gl_user_num)
    {
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduser=1&s='+gl_user_load_index+'&count='+LOAD_USER_NUM+'&project_id='+project_id, function(data)
    {
        var user_num = 0;
        $.each(data, function(index, user)
        {
            user.userid = parseInt(user.userid);
            gl_user_info.push(user);
            user_num++;
        });

        gl_user_load_index += user_num;

    })
    .success(function() {
        if (gl_user_load_index >= gl_user_num)
        {
        }
        else
        {
            setTimeout(load_user, 50);
        }
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_node_info_all()
{
    // fetch device count
    if ( document.getElementById("LOC_TAG_NODE_TABLE") == null )
    {

        console.log("load_node_info_all() LOC_TAG_NODE_TABLE == null ");
        return;
    }

    if ( gl_activetab === 'activetab_tags' ||
         gl_activetab === 'activetab_anchors' ||
         gl_activetab === 'activetab_coords' ||
         gl_activetab === 'activetab_locators')
    {
    }
    else
    {
        setTimeout(load_node_info_all, 1000);
        return;
    }


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


        gl_OnLinelist_anchor = [];
        gl_notOnLinelist_anchor = [];
        gl_OnLinelist_tag = [];
        gl_notOnLinelist_tag = [];
        gl_OnLinelist_coord= [];
        gl_notOnLinelist_coord = [];
        gl_OnLinelist_locator= [];
        gl_notOnLinelist_locator = [];

        //console.log("gl_anchor_num:"+gl_anchor_num+", gl_tag_num:"+gl_tag_num+", gl_coord_num:"+gl_coord_num+", gl_locator_num:"+gl_locator_num);

    })
    .success(function() {
        setTimeout(load_tag_info, 500);
    })
    .error(function() {
        setTimeout(load_node_info_all, 1000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}

function clear_anchor_table()
{
    var loc_anchor_table = document.getElementById('LOC_ANCHOR_NODE_TABLE');

    while (loc_anchor_table.rows.length >= 1 && gl_anchor_num >= 0)
    {
        loc_anchor_table.deleteRow(0);
    }
}

function clear_tag_table()
{
    var loc_tag_table = document.getElementById('LOC_TAG_NODE_TABLE');

    while (loc_tag_table.rows.length >= 1 && gl_tag_num >= 0)
    {
        loc_tag_table.deleteRow(0);
    }
}

function clear_coord_table()
{
    var loc_coord_table = document.getElementById('LOC_COORD_NODE_TABLE');

    while (loc_coord_table.rows.length >= 1 && gl_coord_num >= 0)
    {
        loc_coord_table.deleteRow(0);
    }
}

function clear_locator_table()
{
    var loc_locator_table = document.getElementById('LOC_LOCATOR_NODE_TABLE');

    while (loc_locator_table.rows.length >= 1 && gl_locator_num >= 0)
    {
        loc_locator_table.deleteRow(0);
    }
}

function put_anchor_to_table()
{
    var loc_anchor_table = document.getElementById('LOC_ANCHOR_NODE_TABLE');

    var index = 0;
    if (gl_showfilter_showonline)
        index += add_node_to_table(loc_anchor_table, "LOC", gl_OnLinelist_anchor, index);
    if (gl_showfilter_showoffline)
        index += add_node_to_table(loc_anchor_table, "LOC", gl_notOnLinelist_anchor, index);
}

function put_tag_to_table()
{
    var loc_tag_table = document.getElementById('LOC_TAG_NODE_TABLE');

    var index = 0;
    if (gl_showfilter_showonline)
        index += add_node_to_table(loc_tag_table, "LOC", gl_OnLinelist_tag, index);
    if (gl_showfilter_showoffline)
        index += add_node_to_table(loc_tag_table, "LOC", gl_notOnLinelist_tag, index);
}

function put_coord_to_table()
{
    var loc_coord_table = document.getElementById('LOC_COORD_NODE_TABLE');

    var index = 0;
    if (gl_showfilter_showonline)
        index += add_node_to_table(loc_coord_table, "LOC", gl_OnLinelist_coord, index);
    if (gl_showfilter_showoffline)
        index += add_node_to_table(loc_coord_table, "LOC", gl_notOnLinelist_coord, index);
}

function put_locator_to_table()
{
    var loc_locator_table = document.getElementById('LOC_LOCATOR_NODE_TABLE');

    var index = 0;
    if (gl_showfilter_showonline)
        index += add_node_to_table(loc_locator_table, "LOC", gl_OnLinelist_locator, index);
    if (gl_showfilter_showoffline)
        index += add_node_to_table(loc_locator_table, "LOC", gl_notOnLinelist_locator, index);
}

function load_anchor_info()
{
    //console.log("gl_anchor_num:"+gl_anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

    if ( gl_activetab === 'activetab_anchors' )
    {
    }
    else
    {
        load_coord_info();
        return;
    }

    if (gl_anchor_load_index >= gl_anchor_num)
    {
        // if (edit_node_info)
        // {
        //     document.getElementById("deleteofflineanchorbutton").style.display = 'block';
        // }
        // else
        // {
        //     document.getElementById("deleteofflineanchorbutton").style.display = 'none';
        // }
        load_coord_info();
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadanchor=1&s='+gl_anchor_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var anchor_num = parseInt(data['LOC_ANCHOR_NUM']);
        if (gl_anchor_load_index == 0 && anchor_num == 0)
        {
            gl_anchor_num = 0;
            gl_anchor_load_index = 0;
        }
        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_anchor_load_index; i < gl_anchor_load_index+anchor_num; ++i)
        {
            var key = 'LOC_ANCHOR_INDEX_'+i;
            var node = data[key];
            //alert("loc_tag node="+ node );
            if (node == undefined)
                continue;
            var nodeData = node.split(",");
            var nodeindex = nodeData[0];
            var alive = nodeData[1];

            if (nodeindex === '-1' || alive === '0')
                gl_notOnLinelist_anchor.push(nodeData);
            else
                gl_OnLinelist_anchor.push(nodeData);
        }
        gl_anchor_load_index += anchor_num;

    })
    .success(function() {
        if (gl_anchor_load_index >= gl_anchor_num)
        {
            if (!stop_update_anchor_info)
            {
                clear_anchor_table();
                put_anchor_to_table();
            }
            stop_update_anchor_info = edit_node_info;

            load_coord_info();
        }
        else
        {
            load_anchor_info();
        }
    })
    .error(function() {
        load_coord_info();
    })
    .complete(function() {
        // if (edit_node_info)
        // {
        //     document.getElementById("deleteofflineanchorbutton").style.display = 'block';
        // }
        // else
        // {
        //     document.getElementById("deleteofflineanchorbutton").style.display = 'none';
        // }
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_tag_info()
{
    //console.log("gl_tag_num:"+gl_tag_num +", gl_tag_load_index:"+gl_tag_load_index);

    if ( gl_activetab === 'activetab_tags' )
    {
    }
    else
    {
        load_anchor_info();
        return;
    }

    if (gl_tag_load_index >= gl_tag_num)
    {
        // if (edit_node_info)
        // {
        //     document.getElementById("deleteofflinetagbutton").style.display = 'block';
        // }
        // else
        // {
        //     document.getElementById("deleteofflinetagbutton").style.display = 'none';
        // }
        load_anchor_info();
        return;
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
        //console.log("tag_num:"+tag_num +", gl_tag_load_index:"+gl_tag_load_index);

        for (var i = gl_tag_load_index; i < gl_tag_load_index+tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            var node = data[key];
            //alert("loc_tag node="+ node );
            if (node == undefined)
                continue;
            var nodeData = node.split(",");
            var nodeindex = nodeData[0];
            var alive = nodeData[1];

            if (nodeindex === '-1' || alive === '0')
                gl_notOnLinelist_tag.push(nodeData);
            else
                gl_OnLinelist_tag.push(nodeData);

        }
        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            //console.log("edit_node_info:"+edit_node_info+", stop_update_tag_info:"+stop_update_tag_info);

            if (!stop_update_tag_info)
            {
                clear_tag_table();
                put_tag_to_table();
            }

            if (edit_node_info)
            {
                stop_update_tag_info = true;
            }
            else
            {
                stop_update_tag_info = false;
            }

            load_anchor_info();
        }
        else
        {
            load_tag_info();
        }
    })
    .error(function() {
        load_anchor_info();
    })
    .complete(function() {
        // if (edit_node_info)
        // {
        //     document.getElementById("deleteofflinetagbutton").style.display = 'block';
        // }
        // else
        // {
        //     document.getElementById("deleteofflinetagbutton").style.display = 'none';
        // }
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_coord_info()
{
    //console.log("gl_coord_num:"+gl_coord_num +", gl_coord_load_index:"+gl_coord_load_index);
    if ( gl_activetab === 'activetab_coords' )
    {
    }
    else
    {
        load_locator_info();
        return;
    }

    if (gl_coord_load_index >= gl_coord_num)
    {
        // if (edit_node_info)
        // {
        //     document.getElementById("deleteofflinecoordbutton").style.display = 'block';
        // }
        // else
        // {
        //     document.getElementById("deleteofflinecoordbutton").style.display = 'none';
        // }

        load_locator_info();
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadcoord=1&s='+gl_coord_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var coord_num = parseInt(data['LOC_COORD_NUM']);

        if (gl_coord_load_index == 0 && coord_num == 0)
        {
            gl_coord_num = 0;
            gl_coord_load_index = 0;
        }
        //console.log("coord_num:"+coord_num +", gl_coord_load_index:"+gl_coord_load_index);

        for (var i = gl_coord_load_index; i < gl_coord_load_index+coord_num; ++i)
        {
            var key = 'LOC_COORD_INDEX_'+i;
            var node = data[key];
            //alert("loc_tag node="+ node );
            if (node == undefined)
                continue;
            var nodeData = node.split(",");
            var nodeindex = nodeData[0];
            var alive = nodeData[1];

            if (nodeindex === '-1' || alive === '0')
                gl_notOnLinelist_coord.push(nodeData);
            else
                gl_OnLinelist_coord.push(nodeData);
        }
        gl_coord_load_index += coord_num;

    })
    .success(function() {
        if (gl_coord_load_index >= gl_coord_num)
        {

            if (!stop_update_coord_info)
            {
                clear_coord_table();
                put_coord_to_table();
            }

            stop_update_coord_info = edit_node_info;

            load_locator_info();
        }
        else
        {
            load_coord_info();
        }
    })
    .error(function() {
        load_locator_info();
    })
    .complete(function() {
        // if (edit_node_info)
        // {
        //     document.getElementById("deleteofflinecoordbutton").style.display = 'block';
        // }
        // else
        // {
        //     document.getElementById("deleteofflinecoordbutton").style.display = 'none';
        // }
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_locator_info()
{
    //console.log("gl_locator_num:"+gl_locator_num +", gl_locator_load_index:"+gl_locator_load_index);
    // console.log(" load_locator_info() gl_activetab:"+gl_activetab);

    if ( gl_activetab === 'activetab_locators' )
    {
    }
    else
    {
        setTimeout(load_node_info_all, 500);
        return;
    }

    if (gl_locator_load_index >= gl_locator_num)
    {
        //console.log("Load all devices finished!!!!");
        // document.getElementById("LOC_NODE_TABLE_MSG").innerText = "";

        if (edit_node_info)
        {
            // Show Create Locator button
            document.getElementById("createnewlocatorbutton").style.display = 'block';
            // document.getElementById("deleteofflinelocatorbutton").style.display = 'block';
        }
        else
        {
            document.getElementById("createnewlocatorbutton").style.display = 'none';
            // document.getElementById("deleteofflinelocatorbutton").style.display = 'none';
        }

        if (gl_locator_num == 0)
            clear_locator_table();
        setTimeout(load_node_info_all, 500);
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadlocator=1&s='+gl_locator_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var locator_num = parseInt(data['LOC_LOCATOR_NUM']);
        //console.log("locator_num:"+locator_num +", gl_locator_load_index:"+gl_locator_load_index);

        if (gl_locator_load_index == 0 && locator_num == 0)
        {
            gl_locator_num = 0;
            gl_locator_load_index = 0;
        }

        for (var i = gl_locator_load_index; i < gl_locator_load_index+locator_num; ++i)
        {
            var key = 'LOC_LOCATOR_INDEX_'+i;
            var node = data[key];
            //alert("loc_tag node="+ node );
            if (node == undefined)
                continue;
            var nodeData = node.split(",");
            var nodeindex = nodeData[0];
            var alive = nodeData[1];

            if (nodeindex === '-1' || alive === '0')
                gl_notOnLinelist_locator.push(nodeData);
            else
                gl_OnLinelist_locator.push(nodeData);
        }
        gl_locator_load_index += locator_num;

    })
    .success(function() {
        if (gl_locator_load_index >= gl_locator_num)
        {

            if (!stop_update_locator_info)
            {
                clear_locator_table();
                put_locator_to_table();

                //console.log("Load all devices finished!!!!");
                // document.getElementById("LOC_NODE_TABLE_MSG").innerText = "";
            }

            if (edit_node_info)
            {
                stop_update_locator_info = true;
            }
            else
            {
                stop_update_locator_info = false;
            }

            setTimeout(load_node_info_all, 500);
        }
        else
        {
            load_locator_info();
        }
    })
    .error(function() {
        setTimeout(load_node_info_all, 500);
    })
    .complete(function() {
        if (edit_node_info)
        {
            // Show Create Locator button
            document.getElementById("createnewlocatorbutton").style.display = 'block';
            // document.getElementById("deleteofflinelocatorbutton").style.display = 'block';
        }
        else
        {
            document.getElementById("createnewlocatorbutton").style.display = 'none';
            // document.getElementById("deleteofflinelocatorbutton").style.display = 'none';
        }
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_loragateway_info()
{

    if ( gl_activetab === 'activetab_loragateways' )
    {
    }
    else
    {
        setTimeout(load_loragateway_info, 1000);
        return;
    }

    gl_OnLinelist_gateway = [];
    gl_notOnLinelist_gateway = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadloragateway=1'+'&project_id='+project_id, function(data)
    {
        var index = 0;
        $.each(data, function(gwid, nodeData)
        {
            var alive = nodeData['alive'];
            var ping = nodeData['ping'];
            var mqtt = nodeData['mqtt'];

            if( alive === '0' && ping === '0' && mqtt === '0' )
                gl_notOnLinelist_gateway.push(nodeData);
            else
                gl_OnLinelist_gateway.push(nodeData);

        });

    })
    .success(function() {

        if (!stop_update_gateway_info)
        {
            clear_gateway_table();
            put_gateway_to_table();
        }

        stop_update_gateway_info = edit_node_info;

        setTimeout(load_loragateway_info, 10000);
    })
    .error(function() {
        setTimeout(load_loragateway_info, 10000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 50000);
}

function load_prj_info()
{
    gl_prj_info = [];

    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'loadallconfig=1'+'&project_id='+project_id , function(data)
    {
        var count = 0;
        $.each(data, function(prjid, value)
        {
            //console.log("prjid:"+prjid + " value:"+value);

            gl_prj_info[count] = value;
            count++;
        });
        //console.log("gl_prj_info.length:"+gl_prj_info.length);
    })
    .success(function() {

        setTimeout(load_prj_info, 60000);
    })
    .error(function() {
        setTimeout(load_prj_info, 10000);
    })
    .complete(function() {
    });
}

function load_node_prj_info()
{
    load_tag_prj_info();
    load_anchor_prj_info();
    load_coord_prj_info();
    load_locator_prj_info();
    load_gateway_prj_info();

    setTimeout(load_node_prj_info, 60000);
}

function load_tag_prj_info()
{
    gl_tag_prj_info = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'loadtagprjinfo=1', function(data)
    {
        $.each(data, function(prjid, nodelist)
        {
            //console.log('prjid:'+prjid);
            $.each(nodelist, function(idx, node)
            {
                var nodeid = node.nodeid;
                if ( gl_tag_prj_info[nodeid] == undefined )
                    gl_tag_prj_info[nodeid] = [];

                gl_tag_prj_info[nodeid].push(parseInt(prjid));
            });
        });
    });
}

function load_anchor_prj_info()
{
    gl_anchor_prj_info = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'loadanchorprjinfo=1', function(data)
    {
        $.each(data, function(prjid, nodelist)
        {
            //console.log('prjid:'+prjid);
            $.each(nodelist, function(idx, node)
            {
                var nodeid = node.nodeid;
                if ( gl_anchor_prj_info[nodeid] == undefined )
                    gl_anchor_prj_info[nodeid] = [];

                gl_anchor_prj_info[nodeid].push(parseInt(prjid));
            });
        });
    });
}

function load_coord_prj_info()
{
    gl_coord_prj_info = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'loadcoordprjinfo=1', function(data)
    {
        $.each(data, function(prjid, nodelist)
        {
            //console.log('prjid:'+prjid);
            $.each(nodelist, function(idx, node)
            {
                var nodeid = node.nodeid;
                if ( gl_coord_prj_info[nodeid] == undefined )
                    gl_coord_prj_info[nodeid] = [];

                gl_coord_prj_info[nodeid].push(parseInt(prjid));
            });
        });
    });
}

function load_locator_prj_info()
{
    gl_locator_prj_info = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'loadlocatorprjinfo=1', function(data)
    {
        $.each(data, function(prjid, nodelist)
        {
            //console.log('prjid:'+prjid);
            $.each(nodelist, function(idx, node)
            {
                var nodeid = node.nodeid;
                if ( gl_locator_prj_info[nodeid] == undefined )
                    gl_locator_prj_info[nodeid] = [];

                gl_locator_prj_info[nodeid].push(parseInt(prjid));
            });
        });
    });
}

function load_gateway_prj_info()
{
    gl_gateway_prj_info = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'loadgatewayprjinfo=1', function(data)
    {
        $.each(data, function(prjid, nodelist)
        {
            //console.log('prjid:'+prjid);
            $.each(nodelist, function(idx, node)
            {
                var gwid = node.gwid;
                if ( gl_gateway_prj_info[gwid] == undefined )
                    gl_gateway_prj_info[gwid] = [];

                gl_gateway_prj_info[gwid].push(parseInt(prjid));
            });
        });
    });
}

function clear_gateway_table()
{
    var loc_gateway_table = document.getElementById('LOC_LORAGATEWAY_NODE_TABLE');

    while (loc_gateway_table.rows.length >= 1)
    {
        loc_gateway_table.deleteRow(0);
    }
}

function put_gateway_to_table()
{
    var loc_gateway_table = document.getElementById('LOC_LORAGATEWAY_NODE_TABLE');

    var ui_index = 0;
    for (var i = 0; i < gl_OnLinelist_gateway.length; i++)
    {
        add_gateway_to_table(loc_gateway_table, ui_index, gl_OnLinelist_gateway[i]);
        ui_index++;
    }
    for (var i = 0; i < gl_notOnLinelist_gateway.length; i++)
    {
        add_gateway_to_table(loc_gateway_table, ui_index, gl_notOnLinelist_gateway[i]);
        ui_index++;
    }
}

function add_gateway_to_table(loc_gateway_table, ui_index, nodeData)
{
    if (nodeData == null)
        return 0;

    var alive = nodeData['alive'];
    var ping = nodeData['ping'];
    var mqtt = nodeData['mqtt'];
    var gwid = nodeData['gwid'];
    var sf = nodeData['sf'];
    var gwip = nodeData['gwip'];
    var repeater = nodeData['repeater'];
    var channel = nodeData['channel'];
    var maplayer = nodeData['maplayer'];
    var posX = nodeData['posX'];
    var posY = nodeData['posY'];
    var posZ = nodeData['posZ'];
    var verifykey = nodeData['verifykey'];

    var row = document.createElement('tr');
    row.setAttribute("id",gwid+"_TABLE_ROW");

    var online = true;
    if( alive === '0' && ping === '0' && mqtt === '0' )
    {
        row.setAttribute("bgcolor","#a3c2c2");
        online = false;
    }
    else
    {
        online = true;
        row.setAttribute("bgcolor","#FFFFFF");
    }

    if (gl_showfilter_showonline && gl_showfilter_showoffline)
    {
        // all show
    }
    else
    if (gl_showfilter_showonline && !online)
    {
        return 0;
    }
    else
    if (gl_showfilter_showoffline && online)
    {
        return 0;
    }

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Alive
    row.appendChild(document.createElement('td'));
    if ( alive != '0' )
    {
        var SpanAlive = document.createElement("span");
        SpanAlive.setAttribute("class","badge bg-fuchsia");
        SpanAlive.textContent = alive;
        row.cells[cellIndex].appendChild(SpanAlive);
    }
    cellIndex++;

    // ping
    row.appendChild(document.createElement('td'));
    if ( ping != '0' )
    {
        var SpanPing = document.createElement("span");
        SpanPing.setAttribute("class","badge bg-fuchsia");
        SpanPing.textContent = ping;
        row.cells[cellIndex].appendChild(SpanPing);
    }
    cellIndex++;

    // MQTT
    row.appendChild(document.createElement('td'));
    if ( mqtt != '0' )
    {
        var SpanPing = document.createElement("span");
        SpanPing.setAttribute("class","badge bg-fuchsia");
        SpanPing.textContent = mqtt;
        row.cells[cellIndex].appendChild(SpanPing);
    }
    cellIndex++;

    // verifykey
    var tdverifykey = document.createElement('td');
    row.appendChild(tdverifykey);
    var SpanVerifykey  = document.createElement("span");
    SpanVerifykey.textContent = verifykey;
    row.cells[cellIndex].appendChild(SpanVerifykey);
    cellIndex++;

    // gwid
    row.appendChild(document.createElement('td'));
    var Spangwid = document.createElement("span");
    Spangwid.setAttribute("class","badge bg-light-blue-active");
    Spangwid.textContent = gwid;
    row.cells[cellIndex].appendChild(Spangwid);
    cellIndex++;

    // IP
    row.appendChild(document.createElement('td'));
    var SpanIP = document.createElement("span");
    SpanIP.setAttribute("class","badge bg-yellow-active");
    SpanIP.textContent = gwip;
    row.cells[cellIndex].appendChild(SpanIP);
    cellIndex++;

    // Poistion
    row.appendChild(document.createElement('td'));
    var SpanPos = document.createElement("span");
    SpanPos.innerHTML = '('+posX+','+posY+','+posZ+')';
    row.cells[cellIndex].appendChild(SpanPos);
    cellIndex++;

    // map layer
    row.appendChild(document.createElement('td'));
    var MapLayerName = get_maplayer_name(maplayer);

    // edit icon
    if (edit_node_info)
    {
        var div = document.createElement("div");
        div.setAttribute("class","btn-group");

        var div_btn = document.createElement("button");
        div_btn.setAttribute("type","button");
        div_btn.setAttribute("class","btn btn-block btn-primary btn-xs dropdown-toggle");
        div_btn.setAttribute("data-toggle","dropdown");
        div_btn.setAttribute("id","dropdown_cur_maplayer_"+gwid);
        div_btn.textContent = MapLayerName;
        div.appendChild(div_btn);

        var div_btn_span = document.createElement("span");
        div_btn_span.setAttribute("class","caret");
        div_btn.appendChild(div_btn_span);

        var div_ul = document.createElement("ul");
        div_ul.setAttribute("class","dropdown-menu");
        div_ul.setAttribute("id","dropdown_maplayer_"+gwid);
        div.appendChild(div_ul);

        //
        // create dropdopwn list
        //
        add_maplayer_dropdown_list(div_ul, "gateway", gwid, gwid, "gateway", maplayer);

        row.cells[cellIndex].appendChild(div);
    }
    else
    {
        var span_maplayer = document.createElement("span");
        span_maplayer.setAttribute("id","gateway_"+gwid+"_MAPLAYER");
        span_maplayer.setAttribute("class","label label-primary");
        span_maplayer.textContent = MapLayerName;
        row.cells[cellIndex].appendChild(span_maplayer);
    }
    cellIndex++;

    // sf
    row.appendChild(document.createElement('td'));
    var Spansf = document.createElement("span");
    Spansf.setAttribute("class","badge bg-maroon-active");
    Spansf.textContent = sf;
    row.cells[cellIndex].appendChild(Spansf);
    cellIndex++;

    // repeater
    row.appendChild(document.createElement('td'));
    var Spanrepeater = document.createElement("span");
    Spanrepeater.setAttribute("class","badge bg-gray-active");
    Spanrepeater.textContent = repeater;
    row.cells[cellIndex].appendChild(Spanrepeater);
    cellIndex++;

    // channel
    row.appendChild(document.createElement('td'));

    var channels = channel.split(",");
    for(var i = 0; i < channels.length; i++)
    {
        var Spanchannel = document.createElement("span");
        Spanchannel.setAttribute("class","badge bg-purple-active");
        Spanchannel.textContent = channels[i];
        row.cells[cellIndex].appendChild(Spanchannel);
    }
    cellIndex++;

    // info
    row.appendChild(document.createElement('td'));

    //console.log("add_gateway_to_table() edit_node_info:"+edit_node_info);

    if(edit_node_info)
    {
        // delete button
        var deleteBtn = document.createElement("button");
        deleteBtn.setAttribute("type","button");
        deleteBtn.setAttribute("class","btn btn-default");
        deleteBtn.setAttribute("onclick","delete_gateway_dialog(\""+ gwid +"\");return false;");

        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-trash-o");
        deleteBtn.appendChild(imgDelete);

        row.cells[cellIndex].appendChild(deleteBtn);
    }
    cellIndex++;

    if(edit_node_info)
    {
        var loc_tab_loragateway_prj = document.getElementById('loc_tab_loragateway_prj');
        loc_tab_loragateway_prj.setAttribute("style","display:block;");

        row.appendChild(document.createElement('td'));
        var prj_dropdown = create_prj_dropdown("gateway", gwid, gwid, "gateway");
        row.cells[cellIndex].appendChild(prj_dropdown);
        cellIndex++;
    }
    else
    {
        var loc_tab_loragateway_prj = document.getElementById('loc_tab_loragateway_prj');
        loc_tab_loragateway_prj.setAttribute("style","display:none;");
    }

    loc_gateway_table.appendChild(row);

    return 1;
}

function create_LOC_table_item(prefix, index, ui_index, nodeData)
{
    if (nodeData == null)
        return null;

    if (nodeData == undefined)
        return null;

    var type = nodeData[2];

    if ( type === '0' ) // anchor
    {
        return create_table_anchor_item(prefix, index, ui_index, nodeData);
    }
    else
    if ( type === '1' ) // tag
    {
        return create_table_tag_item(prefix, index, ui_index, nodeData);
    }
    else
    if (type == '2') // coordinator
    {
        return create_table_coordinator_item(prefix, index, ui_index, nodeData);
    }
    else
    if (type == '3') // locator
    {
        return create_table_locator_item(prefix, index, ui_index, nodeData);
    }
}

function create_table_anchor_item(prefix, index, ui_index, nodeData)
{
    var nodeindex = nodeData[0];
    var alive = nodeData[1];
    var type = nodeData[2];
    var nodeid = nodeData[3];
    var role = nodeData[4];
    var nodename = nodeData[5];
    var macaddress = nodeData[6];
    var node_type = get_node_type(type);
    var nodetype = 'anchor';
    var maplayer = nodeData[20];
    var verifykey = nodeData[23];

    var row = document.createElement('tr');
    row.setAttribute("id", macaddress+"_TABLE_ROW");

    if ( alive === '0' )
        row.setAttribute("bgcolor","#a3c2c2");
    else
        row.setAttribute("bgcolor","#FFFFFF");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Alive
    row.appendChild(document.createElement('td'));
    if ( alive != '0' )
    {
        var SpanAlive = document.createElement("span");
        SpanAlive.setAttribute("class","badge bg-fuchsia");
        SpanAlive.textContent = alive;
        row.cells[cellIndex].appendChild(SpanAlive);
    }
    cellIndex++;

    // Node ID
    // row.appendChild(document.createElement('td'));
    // var span_nodeid = document.createElement("span");
    // span_nodeid.setAttribute("id",prefix+"_"+macaddress+"_NODID");
    // span_nodeid.setAttribute("class","badge bg-light-blue-active");
    // span_nodeid.textContent = nodeid;
    // row.cells[cellIndex].appendChild(span_nodeid);
    // cellIndex++;

    // Node Name
    row.appendChild(document.createElement('td'));
    var span_nodename = document.createElement("span");
    span_nodename.setAttribute("id",prefix+"_"+macaddress+"_NODNAME");
    span_nodename.setAttribute("class","badge bg-yellow-active");
    span_nodename.textContent = nodename;
    row.cells[cellIndex].appendChild(span_nodename);
    // edit icon
    if (edit_node_info)
    {
        //href="javascript:;" onclick="event_trig_tag_select({{ outer_loop.index }}, {{ loop.index }});return false;"
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("id",prefix+"_"+macaddress+"_NODNAME_EDIT");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_node_name(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodename+"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;

    // type + index
    row.appendChild(document.createElement('td'));
    var Span2 = document.createElement("span");
    Span2.setAttribute("class","badge bg-green-active");// anchor
    Span2.textContent = node_type;
    row.cells[cellIndex].appendChild(Span2);
    var SpanIndex = document.createElement("span");
    SpanIndex.setAttribute("class","badge bg-purple-active");
    if (nodeindex == '-1')
        SpanIndex.setAttribute("class","badge bg-maroon-active");
    SpanIndex.textContent = nodeindex;
    row.cells[cellIndex].appendChild(SpanIndex);
    cellIndex++;

    // verifykey
    var tdverifykey = document.createElement('td');
    row.appendChild(tdverifykey);
    var SpanVerifykey  = document.createElement("span");
    SpanVerifykey.textContent = verifykey;
    row.cells[cellIndex].appendChild(SpanVerifykey);
    cellIndex++;

    //MAC address
    row.appendChild(document.createElement('td'));
    var Span1 = document.createElement("span");
    Span1.setAttribute("class","badge bg-gray-active");
    Span1.textContent = macaddress;
    row.cells[cellIndex].appendChild(Span1);
    cellIndex++;

    // Poistion
    var position = '('+nodeData[7]+','+nodeData[8]+','+nodeData[9]+')';
    row.appendChild(document.createElement('td'));

    var SpanPos = document.createElement("span");
    SpanPos.setAttribute("id",prefix+"_"+macaddress+"_POS");
    SpanPos.innerHTML = position;
    row.cells[cellIndex].appendChild(SpanPos);

    // edit icon
    if (edit_node_info)
    {
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("id",prefix+"_"+macaddress+"_LINK");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_node_position(\"anchor\",\""+ macaddress +"\",\""+ nodeData[7]+"\",\""+nodeData[8]+"\",\""+nodeData[9] +"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;

    // map layer
    row.appendChild(document.createElement('td'));
    var MapLayerName = get_maplayer_name(maplayer);

    // edit icon
    if (edit_node_info)
    {
        var div = document.createElement("div");
        div.setAttribute("class","btn-group");

        var div_btn = document.createElement("button");
        div_btn.setAttribute("type","button");
        div_btn.setAttribute("class","btn btn-block btn-primary btn-xs dropdown-toggle");
        div_btn.setAttribute("data-toggle","dropdown");
        div_btn.setAttribute("id","dropdown_cur_maplayer_"+macaddress);
        div_btn.textContent = MapLayerName;
        div.appendChild(div_btn);

        var div_btn_span = document.createElement("span");
        div_btn_span.setAttribute("class","caret");
        div_btn.appendChild(div_btn_span);

        var div_ul = document.createElement("ul");
        div_ul.setAttribute("class","dropdown-menu");
        div_ul.setAttribute("id","dropdown_maplayer_"+macaddress);
        div.appendChild(div_ul);

        //
        // create dropdopwn list
        //
        add_maplayer_dropdown_list(div_ul, prefix, nodeid, macaddress, nodetype, maplayer);

        row.cells[cellIndex].appendChild(div);
    }
    else
    {
        var span_maplayer = document.createElement("span");
        span_maplayer.setAttribute("id",prefix+"_"+macaddress+"_MAPLAYER");
        span_maplayer.setAttribute("class","label label-primary");
        span_maplayer.textContent = MapLayerName;
        row.cells[cellIndex].appendChild(span_maplayer);
    }
    cellIndex++;

    // Info
    var info = '';
    if(nodeData[10].length > 0)
        info += nodeData[10]+'v ';
    if(nodeData[11].length > 0)
        info += nodeData[11]+'°C ';
    if(nodeData[12].length > 0)
        info += ' V'+nodeData[12];

    if (nodeData[19].length > 0) // anchor
    {
        info += ' panid:' + nodeData[19];
    }
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(info));

    if(edit_node_info)
    {
        // delete button
        var deleteBtn = document.createElement("button");
        deleteBtn.setAttribute("type","button");
        deleteBtn.setAttribute("class","btn btn-default");
        deleteBtn.setAttribute("onclick","delete_node_dialog(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodename +"\");return false;");

        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-trash-o");
        deleteBtn.appendChild(imgDelete);

        row.cells[cellIndex].appendChild(deleteBtn);
    }
    cellIndex++;

    if(edit_node_info)
    {
        var loc_tab_anchor_prj = document.getElementById('loc_tab_anchor_prj');
        loc_tab_anchor_prj.setAttribute("style","display:block;");

        row.appendChild(document.createElement('td'));
        var prj_dropdown = create_prj_dropdown(prefix, nodeid, macaddress, nodetype);
        row.cells[cellIndex].appendChild(prj_dropdown);
        cellIndex++;
    }
    else
    {
        var loc_tab_anchor_prj = document.getElementById('loc_tab_anchor_prj');
        loc_tab_anchor_prj.setAttribute("style","display:none;");
    }

    return row;
}

function create_table_tag_item(prefix, index, ui_index, nodeData)
{
    var nodeindex = nodeData[0];
    var alive = nodeData[1];
    var type = nodeData[2];
    var nodeid = nodeData[3];
    var role = nodeData[4];
    var nodename = nodeData[5];
    var macaddress = nodeData[6];
    var node_type = get_node_type(type);
    var nodetype = 'tag';
    var maplayer = nodeData[36];
    var verifykey = nodeData[41];

    var row = document.createElement('tr');
    row.setAttribute("id", macaddress+"_TABLE_ROW");

    if ( alive === '0' )
        row.setAttribute("bgcolor","#a3c2c2");
    else
        row.setAttribute("bgcolor","#FFFFFF");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Alive
    row.appendChild(document.createElement('td'));
    if ( alive != '0' )
    {
        var SpanAlive = document.createElement("span");
        SpanAlive.setAttribute("class","badge bg-fuchsia");
        SpanAlive.textContent = alive;
        row.cells[cellIndex].appendChild(SpanAlive);
    }
    cellIndex++;

    // Node ID
    // row.appendChild(document.createElement('td'));
    // var span_nodeid = document.createElement("span");
    // span_nodeid.setAttribute("id",prefix+"_"+macaddress+"_NODID");
    // span_nodeid.setAttribute("class","badge bg-light-blue-active");
    // span_nodeid.textContent = nodeid;
    // row.cells[cellIndex].appendChild(span_nodeid);
    // cellIndex++;

    // Node Name
    row.appendChild(document.createElement('td'));
    var span_nodename = document.createElement("span");
    span_nodename.setAttribute("id",prefix+"_"+macaddress+"_NODNAME");
    span_nodename.setAttribute("class","badge bg-yellow-active");
    span_nodename.textContent = nodename;
    row.cells[cellIndex].appendChild(span_nodename);
    // edit icon
    if (edit_node_info)
    {
        //href="javascript:;" onclick="event_trig_tag_select({{ outer_loop.index }}, {{ loop.index }});return false;"
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("id",prefix+"_"+macaddress+"_NODNAME_EDIT");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_node_name(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodename+"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;

    // type + index
    row.appendChild(document.createElement('td'));
    var Span2 = document.createElement("span");
    Span2.setAttribute("class","badge bg-blue-active"); // tag
    Span2.textContent = node_type;
    row.cells[cellIndex].appendChild(Span2);
    var SpanIndex = document.createElement("span");
    SpanIndex.setAttribute("class","badge bg-purple-active");
    if (nodeindex == '-1')
        SpanIndex.setAttribute("class","badge bg-maroon-active");
    SpanIndex.textContent = nodeindex;
    row.cells[cellIndex].appendChild(SpanIndex);
    cellIndex++;

    if (GL_CONFIG_SHOW_ROLE_INFO == 1)
    {
        // Role
        row.appendChild(document.createElement('td'));
        var ShowName = get_role_name(role);

        // edit icon
        if (edit_node_info)
        {
            var div = document.createElement("div");
            div.setAttribute("class","btn-group");

            var div_btn = document.createElement("button");
            div_btn.setAttribute("type","button");
            div_btn.setAttribute("class","btn btn-block btn-primary btn-xs dropdown-toggle");
            div_btn.setAttribute("data-toggle","dropdown");
            div_btn.setAttribute("id","dropdown_cur_role_"+macaddress);
            div_btn.textContent = ShowName;
            div.appendChild(div_btn);

            var div_btn_span = document.createElement("span");
            div_btn_span.setAttribute("class","caret");
            div_btn.appendChild(div_btn_span);

            var div_ul = document.createElement("ul");
            div_ul.setAttribute("class","dropdown-menu");
            div_ul.setAttribute("id","dropdown_role_"+macaddress);
            div.appendChild(div_ul);

            //
            // create dropdopwn list
            //
            add_role_dropdown_list(div_ul, prefix, nodeid, macaddress, "tag", role);

            row.cells[cellIndex].appendChild(div);
        }
        else
        {
            var SpanRole = document.createElement("span");
            SpanRole.setAttribute("id",prefix+"_"+macaddress+"_ROLE");
            SpanRole.setAttribute("class","badge bg-green-active");
            SpanRole.textContent = ShowName;
            row.cells[cellIndex].appendChild(SpanRole);
        }
        cellIndex++;
    }

    row.appendChild(document.createElement('td'));

    var userid = nodeData[39];
    var item = create_userItem(nodetype, nodeid, macaddress, userid);
    if (item != null)
        row.cells[cellIndex].appendChild(item);
    cellIndex++;

    // verifykey
    var tdverifykey = document.createElement('td');
    row.appendChild(tdverifykey);
    var SpanVerifykey  = document.createElement("span");
    SpanVerifykey.textContent = verifykey;
    row.cells[cellIndex].appendChild(SpanVerifykey);
    cellIndex++;

    //MAC address
    row.appendChild(document.createElement('td'));
    var Span1 = document.createElement("span");
    Span1.setAttribute("class","badge bg-gray-active");
    Span1.textContent = macaddress;
    row.cells[cellIndex].appendChild(Span1);
    cellIndex++;

    // Poistion
    var position = '('+nodeData[7]+','+nodeData[8]+','+nodeData[9]+')';
    row.appendChild(document.createElement('td'));

    var SpanPos = document.createElement("span");
    SpanPos.setAttribute("id",prefix+"_"+macaddress+"_POS");
    SpanPos.innerHTML = position;
    row.cells[cellIndex].appendChild(SpanPos);
    cellIndex++;

    if (GL_CONFIG_SHOW_GPS_INFO == 1)
    {
        // GPS
        row.appendChild(document.createElement('td'));
        //var GPS = 'Longitude:'+nodeData[24]+', Latitude:'+nodeData[25];
        var GPS = '經緯度('+nodeData[24]+','+nodeData[25]+')';
        var SpanGPS = document.createElement("span");
        SpanGPS.setAttribute("id",prefix+"_"+macaddress+"_GPS");
        SpanGPS.innerHTML = GPS;
        row.cells[cellIndex].appendChild(SpanGPS);

        var Latitude = parseInt(nodeData[25]);
        var Longitude = parseInt(nodeData[24]);
        if (Latitude != 0 && Longitude != 0)
        {
            var gmapLink = document.createElement("a");
            gmapLink.setAttribute("href","javascript:;");
            gmapLink.setAttribute("onclick","open_gmap("+ nodeData[25] +","+ nodeData[24]+");return false;");
            var imggmapLink = document.createElement("i");
            imggmapLink.setAttribute("class","fa fa-map-marker");
            gmapLink.appendChild(imggmapLink);
            row.cells[cellIndex].appendChild(gmapLink);
        }
        cellIndex++;

    }

    // map layer
    row.appendChild(document.createElement('td'));
    var MapLayerName = get_maplayer_name(maplayer);

    // edit icon
    if (edit_node_info)
    {
        var div = document.createElement("div");
        div.setAttribute("class","btn-group");

        var div_btn = document.createElement("button");
        div_btn.setAttribute("type","button");
        div_btn.setAttribute("class","btn btn-block btn-primary btn-xs dropdown-toggle");
        div_btn.setAttribute("data-toggle","dropdown");
        div_btn.setAttribute("id","dropdown_cur_maplayer_"+macaddress);
        div_btn.textContent = MapLayerName;
        div.appendChild(div_btn);

        var div_btn_span = document.createElement("span");
        div_btn_span.setAttribute("class","caret");
        div_btn.appendChild(div_btn_span);

        var div_ul = document.createElement("ul");
        div_ul.setAttribute("class","dropdown-menu");
        div_ul.setAttribute("id","dropdown_maplayer_"+macaddress);
        div.appendChild(div_ul);

        //
        // create dropdopwn list
        //
        add_maplayer_dropdown_list(div_ul, prefix, nodeid, macaddress, nodetype, maplayer);

        row.cells[cellIndex].appendChild(div);
    }
    else
    {
        var span_maplayer = document.createElement("span");
        span_maplayer.setAttribute("id",prefix+"_"+macaddress+"_MAPLAYER");
        span_maplayer.setAttribute("class","label label-primary");
        span_maplayer.textContent = MapLayerName;
        row.cells[cellIndex].appendChild(span_maplayer);
    }
    cellIndex++;

    // Info
    var info = '';
    if(nodeData[10].length > 0)
        info += nodeData[10]+'v ';
    if(nodeData[11].length > 0)
        info += nodeData[11]+'°C ';
    if(nodeData[12].length > 0)
        info += ' V'+nodeData[12];

    if (nodeData[21].length > 0) // tag
    {
        info += ' panid:' + nodeData[21];
    }

    if ( nodeData[27].length > 0) // tag
    {
        if ( nodeData[27] == '0' )
            info += ' ';
        else
        if ( nodeData[27] == '1' )
            info += ' UWB';
        else
        if ( nodeData[27] == '2' )
            info += ' LoRa_Ranging';
        else
        if ( nodeData[27] == '3' )
            info += ' GPS';
    }

    if (nodeData[29].length > 0) // tag
    {
        info += ' gwid:' + nodeData[29];
    }

    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(info));

    //
    // show QR Code
    //
    var btnQrcode = document.createElement("button");
    btnQrcode.setAttribute("type","button");
    btnQrcode.setAttribute("class","btn btn-default");
    btnQrcode.setAttribute("data-toggle","modal");
    btnQrcode.setAttribute("data-target","#modal-qrcode");
    btnQrcode.setAttribute("onclick","device_qrcode_show(\""+ type +"\",\""+ nodename +"\",\""+ macaddress +"\");return false;");

    var imgQrcode = document.createElement("i");
    imgQrcode.setAttribute("class","fa fa-qrcode");
    btnQrcode.appendChild(imgQrcode);
    row.cells[cellIndex].appendChild(btnQrcode);

    if(edit_node_info)
    {
        // delete button
        var deleteBtn = document.createElement("button");
        deleteBtn.setAttribute("type","button");
        deleteBtn.setAttribute("class","btn btn-default");
        deleteBtn.setAttribute("onclick","delete_node_dialog(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodename +"\");return false;");

        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-trash-o");
        deleteBtn.appendChild(imgDelete);

        row.cells[cellIndex].appendChild(deleteBtn);
    }
    cellIndex++;

    if (nodeData[15].length > 0)
    {
        // Ranging List
        //<span class="sparkline-1">8,4,0,0,0,0,1,4,4,10,10,10,10,0,0,0,4,6,5,9,10</span>
        row.appendChild(document.createElement('td'));
        //row.cells[cellIndex].appendChild(document.createTextNode(nodeData[12]));

        var nodeindexs = nodeData[15].split(":");
        //for(var node in nodeindexs)
        for(var i = 0; i < nodeindexs.length; i++)
        {
            var nodeIndex = document.createElement("span");
            nodeIndex.setAttribute("class","badge bg-aqua");
            nodeIndex.textContent = nodeindexs[i];
            row.cells[cellIndex].appendChild(nodeIndex);
        }
        cellIndex++;
    }

    if(edit_node_info)
    {
        var loc_tab_tag_prj = document.getElementById('loc_tab_tag_prj');
        loc_tab_tag_prj.setAttribute("style","display:block;");

        row.appendChild(document.createElement('td'));
        var prj_dropdown = create_prj_dropdown(prefix, nodeid, macaddress, nodetype);
        row.cells[cellIndex].appendChild(prj_dropdown);
        cellIndex++;
    }
    else
    {
        var loc_tab_tag_prj = document.getElementById('loc_tab_tag_prj');
        loc_tab_tag_prj.setAttribute("style","display:none;");
    }

    return row;
}

function create_userItem(nodetype, nodeid, macaddress, userid)
{
    var ShowName = findUserName(userid);
    // edit mode
    if (edit_node_info)
    {
        var div = document.createElement("div");
        div.setAttribute("class","btn-group");

        var div_btn = document.createElement("button");
        div_btn.setAttribute("type","button");
        //div_btn.setAttribute("class","btn btn-default btn-sm dropdown-toggle");
        div_btn.setAttribute("class","btn btn-block btn-primary btn-xs dropdown-toggle");
        div_btn.setAttribute("data-toggle","dropdown");
        div_btn.setAttribute("id","dropdown_cur_user_"+macaddress);
        div_btn.textContent = ShowName;
        div.appendChild(div_btn);

        var div_btn_span = document.createElement("span");
        div_btn_span.setAttribute("class","caret");
        div_btn.appendChild(div_btn_span);

        var div_ul = document.createElement("ul");
        div_ul.setAttribute("class","dropdown-menu");
        div_ul.setAttribute("id","dropdown_user_"+macaddress);
        div.appendChild(div_ul);

        //
        // create dropdopwn list
        //
        add_user_dropdown_list(div_ul, nodetype, nodeid, macaddress, userid);

        return div;
    }
    else
    {
        if (ShowName.length > 0)
        {
            var Span1 = document.createElement("span");
            Span1.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(54, 127, 169);");
            Span1.textContent = ShowName;
            return Span1;
        }
    }
    return null;
}

function create_table_coordinator_item(prefix, index, ui_index, nodeData)
{
    var nodeindex = nodeData[0];
    var alive = nodeData[1];
    var type = nodeData[2];
    var nodeid = nodeData[3];
    var role = nodeData[4];
    var nodename = nodeData[5];
    var macaddress = nodeData[6];
    var node_type = get_node_type(type);
    var nodetype = 'coord';
    var maplayer = nodeData[26];
    var verifykey = nodeData[29];

    var row = document.createElement('tr');
    row.setAttribute("id", macaddress+"_TABLE_ROW");

    if ( alive === '0' )
        row.setAttribute("bgcolor","#a3c2c2");
    else
        row.setAttribute("bgcolor","#FFFFFF");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Alive
    row.appendChild(document.createElement('td'));
    if ( alive != '0' )
    {
        var SpanAlive = document.createElement("span");
        SpanAlive.setAttribute("class","badge bg-fuchsia");
        SpanAlive.textContent = alive;
        row.cells[cellIndex].appendChild(SpanAlive);
    }
    cellIndex++;

    var ping = nodeData[20];
    // ping
    row.appendChild(document.createElement('td'));
    if ( ping != '0' )
    {
        var SpanPing = document.createElement("span");
        SpanPing.setAttribute("class","badge bg-fuchsia");
        SpanPing.textContent = ping;
        row.cells[cellIndex].appendChild(SpanPing);
    }
    cellIndex++;

    // // Node ID
    // row.appendChild(document.createElement('td'));
    // var span_nodeid = document.createElement("span");
    // span_nodeid.setAttribute("id",prefix+"_"+macaddress+"_NODID");
    // span_nodeid.setAttribute("class","badge bg-light-blue-active");
    // span_nodeid.textContent = nodeid;
    // row.cells[cellIndex].appendChild(span_nodeid);
    // cellIndex++;

    // Node Name
    row.appendChild(document.createElement('td'));
    var span_nodename = document.createElement("span");
    span_nodename.setAttribute("id",prefix+"_"+macaddress+"_NODNAME");
    span_nodename.setAttribute("class","badge bg-yellow-active");
    span_nodename.textContent = nodename;
    row.cells[cellIndex].appendChild(span_nodename);
    // edit icon
    if (edit_node_info)
    {
        //href="javascript:;" onclick="event_trig_tag_select({{ outer_loop.index }}, {{ loop.index }});return false;"
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("id",prefix+"_"+macaddress+"_NODNAME_EDIT");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_node_name(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodename+"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;

    // type + index
    row.appendChild(document.createElement('td'));
    var Span2 = document.createElement("span");
    Span2.setAttribute("class","badge bg-red-active"); // coord
    Span2.textContent = node_type;
    row.cells[cellIndex].appendChild(Span2);
    var SpanIndex = document.createElement("span");
    SpanIndex.setAttribute("class","badge bg-purple-active");
    if (nodeindex == '-1')
        SpanIndex.setAttribute("class","badge bg-maroon-active");
    SpanIndex.textContent = nodeindex;
    row.cells[cellIndex].appendChild(SpanIndex);
    cellIndex++;

    // verifykey
    var tdverifykey = document.createElement('td');
    row.appendChild(tdverifykey);
    var SpanVerifykey  = document.createElement("span");
    SpanVerifykey.textContent = verifykey;
    row.cells[cellIndex].appendChild(SpanVerifykey);
    cellIndex++;

    //MAC address
    row.appendChild(document.createElement('td'));
    var Span1 = document.createElement("span");
    Span1.setAttribute("class","badge bg-gray-active");
    Span1.textContent = macaddress;
    row.cells[cellIndex].appendChild(Span1);
    cellIndex++;

    // Poistion
    var position = '('+nodeData[7]+','+nodeData[8]+','+nodeData[9]+')';
    row.appendChild(document.createElement('td'));

    var SpanPos = document.createElement("span");
    SpanPos.setAttribute("id",prefix+"_"+macaddress+"_POS");
    SpanPos.innerHTML = position;
    row.cells[cellIndex].appendChild(SpanPos);

    // edit icon
    if (edit_node_info)
    {
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("id",prefix+"_"+macaddress+"_LINK");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_node_position(\"coord\",\""+ macaddress +"\",\""+ nodeData[7]+"\",\""+nodeData[8]+"\",\""+nodeData[9] +"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;

    // map layer
    row.appendChild(document.createElement('td'));
    var MapLayerName = get_maplayer_name(maplayer);

    // edit icon
    if (edit_node_info)
    {
        var div = document.createElement("div");
        div.setAttribute("class","btn-group");

        var div_btn = document.createElement("button");
        div_btn.setAttribute("type","button");
        div_btn.setAttribute("class","btn btn-block btn-primary btn-xs dropdown-toggle");
        div_btn.setAttribute("data-toggle","dropdown");
        div_btn.setAttribute("id","dropdown_cur_maplayer_"+macaddress);
        div_btn.textContent = MapLayerName;
        div.appendChild(div_btn);

        var div_btn_span = document.createElement("span");
        div_btn_span.setAttribute("class","caret");
        div_btn.appendChild(div_btn_span);

        var div_ul = document.createElement("ul");
        div_ul.setAttribute("class","dropdown-menu");
        div_ul.setAttribute("id","dropdown_maplayer_"+macaddress);
        div.appendChild(div_ul);

        //
        // create dropdopwn list
        //
        add_maplayer_dropdown_list(div_ul, prefix, nodeid, macaddress, nodetype, maplayer);

        row.cells[cellIndex].appendChild(div);
    }
    else
    {
        var span_maplayer = document.createElement("span");
        span_maplayer.setAttribute("id",prefix+"_"+macaddress+"_MAPLAYER");
        span_maplayer.setAttribute("class","label label-primary");
        span_maplayer.textContent = MapLayerName;
        row.cells[cellIndex].appendChild(span_maplayer);
    }
    cellIndex++;
    // Info
    var info = '';
    if(nodeData[10].length > 0)
        info += nodeData[10]+'v ';
    if(nodeData[11].length > 0)
        info += nodeData[11]+'°C ';
    if(nodeData[12].length > 0)
        info += ' V'+nodeData[12];

    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(info));

    if(edit_node_info)
    {
        // delete button
        var deleteBtn = document.createElement("button");
        deleteBtn.setAttribute("type","button");
        deleteBtn.setAttribute("class","btn btn-default");
        deleteBtn.setAttribute("onclick","delete_node_dialog(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodename +"\");return false;");

        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-trash-o");
        deleteBtn.appendChild(imgDelete);

        row.cells[cellIndex].appendChild(deleteBtn);
    }
    cellIndex++;

    // IP
    row.appendChild(document.createElement('td'));
    if (nodeData[16].length > 0)
    {
        var IPspan = document.createElement("span");
        //nodeIndex.setAttribute("class","badge bg-aqua");
        IPspan.textContent = nodeData[16];
        row.cells[cellIndex].appendChild(IPspan);
    }
    cellIndex++;

    // App Version
    row.appendChild(document.createElement('td'));
    var showText = ""
    if (nodeData[17].length > 0)
        showText += nodeData[17];
    if (nodeData[19].length > 0)
        showText = showText +" PANID:"+nodeData[19];
    var AppVersionspan = document.createElement("span");
    AppVersionspan.textContent = showText;
    row.cells[cellIndex].appendChild(AppVersionspan);
    cellIndex++;

    // CoordRunTime
    row.appendChild(document.createElement('td'));
    if (nodeData[21].length > 0)
    {
        var TIMEspan = document.createElement("span");
        //TIMEspan.textContent = nodeData[21];
        var secs = parseFloat(nodeData[21]);
        // console.log("secs:"+secs);
        // secs = parseFloat(secs / 1000);
        // console.log("secs:"+secs);
        // console.log("hhmmss(secs):"+hhmmss(secs));
        // //TIMEspan.textContent = convertPrettyTime(hhmmss(secs));
        TIMEspan.textContent = msToDuration(secs);
        row.cells[cellIndex].appendChild(TIMEspan);
    }
    cellIndex++;

    // black list
    row.appendChild(document.createElement('td'));

    var blacklistspan = document.createElement("span");
    blacklistspan.setAttribute("id",prefix+"_"+macaddress+"_BLACKLIST");
    blacklistspan.textContent = nodeData[22];
    row.cells[cellIndex].appendChild(blacklistspan);

    // edit icon
    if (edit_node_info)
    {
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("id",prefix+"_"+macaddress+"_BLACKLIST_EDIT");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_node_blacklist(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodeData[22]+"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }

    cellIndex++;

    // white list
    row.appendChild(document.createElement('td'));

    var whitelistspan = document.createElement("span");
    blacklistspan.setAttribute("id",prefix+"_"+macaddress+"_WHITELIST");
    whitelistspan.textContent = nodeData[23];
    row.cells[cellIndex].appendChild(whitelistspan);


    // edit icon
    if (edit_node_info)
    {
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("id",prefix+"_"+macaddress+"_WHITELIST_EDIT");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_node_whitelist(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodeData[23]+"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;


    // joinRangingRange
    row.appendChild(document.createElement('td'));

    var joinRangingRangespan = document.createElement("span");
    joinRangingRangespan.setAttribute("class","label label-warning");
    joinRangingRangespan.textContent = nodeData[24];
    row.cells[cellIndex].appendChild(joinRangingRangespan);

    // edit icon
    if (edit_node_info)
    {
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_node_joinRangingRange(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodeData[24]+"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;


    // joinNotRangingRange
    row.appendChild(document.createElement('td'));

    var joinNotRangingRangespan = document.createElement("span");
    joinNotRangingRangespan.setAttribute("class","label label-warning");
    joinNotRangingRangespan.textContent = nodeData[25];
    row.cells[cellIndex].appendChild(joinNotRangingRangespan);

    // edit icon
    if (edit_node_info)
    {
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_node_joinNotRangingRange(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodeData[25]+"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;

    //
    // reset button
    //
    row.appendChild(document.createElement('td'));

    // <div class="btn-group">
    //     <button class="btn btn-block btn-warning btn-lg" id="createnewlocatorbutton" style="display:none"><i class="fa fa-plus"></i>Create New Locator</button>
    //     <button class="btn btn-block btn-danger btn-lg" id="deleteofflinelocatorbutton" style="display:none">Delete All Offline Locator</button>
    // </div>

    var btnDiv = document.createElement("div");
    btnDiv.setAttribute("class","btn-group");
    row.cells[cellIndex].appendChild(btnDiv);

    var buttonResetm4 = document.createElement("button");
    buttonResetm4.setAttribute("class","btn btn-block btn-danger btn-xs");
    buttonResetm4.setAttribute("onclick","reset_m4_dialog(\""+ macaddress +"\",\""+ nodename+"\");return false;");
    buttonResetm4.textContent = "Reset M4";
    btnDiv.appendChild(buttonResetm4);

    var buttonResetm0 = document.createElement("button");
    buttonResetm0.setAttribute("class","btn btn-block btn-danger btn-xs");
    buttonResetm0.setAttribute("onclick","reset_m0_dialog(\""+ macaddress +"\",\""+ nodename+"\");return false;");
    buttonResetm0.textContent = "Reset M0";
    btnDiv.appendChild(buttonResetm0);

    cellIndex++;

    if(edit_node_info)
    {
        var loc_tab_coord_prj = document.getElementById('loc_tab_coord_prj');
        loc_tab_coord_prj.setAttribute("style","display:block;");

        row.appendChild(document.createElement('td'));
        var prj_dropdown = create_prj_dropdown(prefix, nodeid, macaddress, nodetype);
        row.cells[cellIndex].appendChild(prj_dropdown);
        cellIndex++;
    }
    else
    {
        var loc_tab_coord_prj = document.getElementById('loc_tab_coord_prj');
        loc_tab_coord_prj.setAttribute("style","display:none;");
    }


    return row;
}

function create_table_locator_item(prefix, index, ui_index, nodeData)
{
    var nodeindex = nodeData[0];
    var alive = nodeData[1];
    var type = nodeData[2];
    var nodeid = nodeData[3];
    var role = nodeData[4];
    var nodename = nodeData[5];
    var macaddress = nodeData[6];
    var node_type = get_node_type(type);
    var nodetype = 'locator';
    var maplayer = nodeData[26];
    var sensorAlert = nodeData[31];
    var Longitude = nodeData[33];
    var Latitude = nodeData[34];
    var verifykey = nodeData[35];
    var AoAInfo = "";

    var version = macaddress[1];
    if ( version == '7' )
    //if(nodeData[30] != undefined)
    {
        // AoA Node info
        AoAInfo = nodeData[30];
    }

    var row = document.createElement('tr');
    row.setAttribute("id", macaddress+"_TABLE_ROW");

    if ( alive === '0' )
        row.setAttribute("bgcolor","#a3c2c2");
    else
        row.setAttribute("bgcolor","#FFFFFF");

    var cellIndex = 0;

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Alive
    row.appendChild(document.createElement('td'));
    if ( alive != '0' )
    {
        var SpanAlive = document.createElement("span");
        SpanAlive.setAttribute("class","badge bg-fuchsia");
        SpanAlive.textContent = alive;
        row.cells[cellIndex].appendChild(SpanAlive);
    }
    cellIndex++;

    // Node ID
    // row.appendChild(document.createElement('td'));
    // var span_nodeid = document.createElement("span");
    // span_nodeid.setAttribute("id",prefix+"_"+macaddress+"_NODID");
    // span_nodeid.setAttribute("class","badge bg-light-blue-active");
    // span_nodeid.textContent = nodeid;
    // row.cells[cellIndex].appendChild(span_nodeid);
    // cellIndex++;

    // Node Name
    row.appendChild(document.createElement('td'));
    var span_nodename = document.createElement("span");
    span_nodename.setAttribute("id",prefix+"_"+macaddress+"_NODNAME");
    span_nodename.setAttribute("class","badge bg-yellow-active");
    span_nodename.textContent = nodename;
    row.cells[cellIndex].appendChild(span_nodename);
    // edit icon
    if (edit_node_info)
    {
        //href="javascript:;" onclick="event_trig_tag_select({{ outer_loop.index }}, {{ loop.index }});return false;"
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("id",prefix+"_"+macaddress+"_NODNAME_EDIT");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_node_name(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodename+"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;

    // type + index
    row.appendChild(document.createElement('td'));
    var Span2 = document.createElement("span");
    Span2.setAttribute("class","badge bg-yellow-active"); // locator
    Span2.textContent = node_type;
    row.cells[cellIndex].appendChild(Span2);
    var SpanIndex = document.createElement("span");
    SpanIndex.setAttribute("class","badge bg-purple-active");
    if (nodeindex == '-1')
        SpanIndex.setAttribute("class","badge bg-maroon-active");
    SpanIndex.textContent = nodeindex;
    row.cells[cellIndex].appendChild(SpanIndex);
    cellIndex++;

    // buttontype
    var buttontype = nodeData[4];
    var ShowName = get_buttontype_name(buttontype);

    row.appendChild(document.createElement('td'));

    // edit icon
    if (edit_node_info)
    {
        var div = document.createElement("div");
        div.setAttribute("class","btn-group");

        var div_btn = document.createElement("button");
        div_btn.setAttribute("type","button");
        div_btn.setAttribute("class","btn btn-block btn-primary btn-xs dropdown-toggle");
        div_btn.setAttribute("data-toggle","dropdown");
        div_btn.setAttribute("id","dropdown_cur_buttontype_"+macaddress);
        div_btn.textContent = ShowName;
        div.appendChild(div_btn);

        var div_btn_span = document.createElement("span");
        div_btn_span.setAttribute("class","caret");
        div_btn.appendChild(div_btn_span);

        var div_ul = document.createElement("ul");
        div_ul.setAttribute("class","dropdown-menu");
        div_ul.setAttribute("id","dropdown_buttontype_"+macaddress);
        div.appendChild(div_ul);

        //
        // create dropdopwn list
        //
        add_buttontype_dropdown_list(div_ul, prefix, nodeid, macaddress, "locator", buttontype);

        row.cells[cellIndex].appendChild(div);
    }
    else
    {
        if (ShowName.length > 0)
        {
            var Span1 = document.createElement("span");
            if (buttontype === '0')
                Span1.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(204, 51, 153);");
            else
            if (buttontype === '1')
                Span1.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(119, 179, 0);");
            else
            if (buttontype === '2')
                Span1.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(91, 160, 218);");
            else
            if (buttontype === '3')
                Span1.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(35, 91, 210);");
            else
            if (buttontype === '4')
                Span1.setAttribute("style","display:inline;padding:.2em .3em .3em;font-size:95%;font-weight:700;line-height:1;position:relative;top:-1px;color:#fff;border-radius:2px;white-space:nowrap;background-color:rgb(85, 101, 30);");
            Span1.textContent = ShowName;
            row.cells[cellIndex].appendChild(Span1);
        }
    }
    cellIndex++;

    row.appendChild(document.createElement('td'));

    var userid = nodeData[32];
    var item = create_userItem(nodetype, nodeid, macaddress, userid);
    if (item != null)
        row.cells[cellIndex].appendChild(item);
    cellIndex++;

    // verifykey
    var tdverifykey = document.createElement('td');
    row.appendChild(tdverifykey);
    var SpanVerifykey  = document.createElement("span");
    SpanVerifykey.textContent = verifykey;
    row.cells[cellIndex].appendChild(SpanVerifykey);
    cellIndex++;

    //MAC address
    row.appendChild(document.createElement('td'));
    var Span1 = document.createElement("span");
    Span1.setAttribute("class","badge bg-gray-active");
    Span1.textContent = macaddress;
    row.cells[cellIndex].appendChild(Span1);
    cellIndex++;

    // LoRA address
    row.appendChild(document.createElement('td'));
    var Span1 = document.createElement("span");
    Span1.setAttribute("id",prefix+"_"+macaddress+"_LORAMACADDRESS");
    Span1.setAttribute("class","badge bg-gray-active");
    Span1.textContent = nodeData[22];
    row.cells[cellIndex].appendChild(Span1);
    // edit icon
    if (edit_node_info)
    {
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("id",prefix+"_"+macaddress+"_LORAMACADDRESS_EDIT");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_LoRA_macaddress(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodeData[22]+"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;

    // Poistion
    var position = '('+nodeData[7]+','+nodeData[8]+','+nodeData[9]+')';
    row.appendChild(document.createElement('td'));

    var SpanPos = document.createElement("span");
    SpanPos.setAttribute("id",prefix+"_"+macaddress+"_POS");
    SpanPos.innerHTML = position;
    row.cells[cellIndex].appendChild(SpanPos);

    // edit icon
    if (edit_node_info)
    {
        //href="javascript:;" onclick="event_trig_tag_select({{ outer_loop.index }}, {{ loop.index }});return false;"
        var linkEdit = document.createElement("a");
        linkEdit.setAttribute("id",prefix+"_"+macaddress+"_LINK");
        linkEdit.setAttribute("href","javascript:;");
        linkEdit.setAttribute("onclick","edit_node_position(\"locator\",\""+ macaddress +"\",\""+ nodeData[7]+"\",\""+nodeData[8]+"\",\""+nodeData[9] +"\");return false;");

        var imgEdit = document.createElement("i");
        imgEdit.setAttribute("class","fa fa-edit");
        linkEdit.appendChild(imgEdit);

        row.cells[cellIndex].appendChild(linkEdit);
    }
    cellIndex++;

    // Sensor alert
    row.appendChild(document.createElement('td'));
    if (edit_node_info)
    {
        var div_Alert  = document.createElement("div");
        row.cells[cellIndex].appendChild(div_Alert);

        var item_checkbox = document.createElement("input");
        item_checkbox.setAttribute("id",prefix+"_"+macaddress+"_SensorAlert");
        item_checkbox.setAttribute("type","checkbox");
        item_checkbox.setAttribute("onclick","checkbox_locator_SensorAlert_click(\""+macaddress+"\");return true;");
        if ( sensorAlert == 1 )
            item_checkbox.checked = true;
        var item_checkbox_label = document.createElement("label");
        item_checkbox_label.setAttribute("for",prefix+"_"+macaddress+"_SensorAlert");
        item_checkbox_label.textContent = "啟用入侵警示";
        div_Alert.appendChild(item_checkbox);
        div_Alert.appendChild(item_checkbox_label);
    }
    else
    {
        var SpanSensorAlert = document.createElement("span");
        SpanSensorAlert.setAttribute("id",prefix+"_"+macaddress+"_SensorAlert");
        row.cells[cellIndex].appendChild(SpanSensorAlert);
        if ( sensorAlert == 1 )
        {
            SpanSensorAlert.textContent = "ON";
            SpanSensorAlert.setAttribute("class","badge bg-green");
        }
        else
        {
            SpanSensorAlert.textContent = "OFF";
            SpanSensorAlert.setAttribute("class","badge bg-red");
        }
    }
    cellIndex++;


    // map layer
    row.appendChild(document.createElement('td'));
    var MapLayerName = get_maplayer_name(maplayer);

    // edit icon
    if (edit_node_info)
    {
        var div = document.createElement("div");
        div.setAttribute("class","btn-group");

        var div_btn = document.createElement("button");
        div_btn.setAttribute("type","button");
        div_btn.setAttribute("class","btn btn-block btn-primary btn-xs dropdown-toggle");
        div_btn.setAttribute("data-toggle","dropdown");
        div_btn.setAttribute("id","dropdown_cur_maplayer_"+macaddress);
        div_btn.textContent = MapLayerName;
        div.appendChild(div_btn);

        var div_btn_span = document.createElement("span");
        div_btn_span.setAttribute("class","caret");
        div_btn.appendChild(div_btn_span);

        var div_ul = document.createElement("ul");
        div_ul.setAttribute("class","dropdown-menu");
        div_ul.setAttribute("id","dropdown_maplayer_"+macaddress);
        div.appendChild(div_ul);

        //
        // create dropdopwn list
        //
        add_maplayer_dropdown_list(div_ul, prefix, nodeid, macaddress, nodetype, maplayer);

        row.cells[cellIndex].appendChild(div);
    }
    else
    {
        var span_maplayer = document.createElement("span");
        span_maplayer.setAttribute("id",prefix+"_"+macaddress+"_MAPLAYER");
        span_maplayer.setAttribute("class","label label-primary");
        span_maplayer.textContent = MapLayerName;
        row.cells[cellIndex].appendChild(span_maplayer);
    }
    cellIndex++;

    row.appendChild(document.createElement('td'));
    // Info
    var info = '';
    var version = nodeData[12];
    var panid = nodeData[19];
    var mode = nodeData[25]; // Opertaion_Mode

    if(nodeData[10].length > 0)
        info += nodeData[10]+'v ';
    if(nodeData[11].length > 0)
        info += nodeData[11]+'°C ';
    if(version.length > 0)
        info += ' V'+version;

    if (panid.length > 0) // locator
    {
        info += ' panid:' + panid;
    }

    if (mode.length > 0) // Opertaion_Mode
    {
        info += ' mode:' + mode;
    }

    if ( nodeData[23].length > 0)
        info += ' gwid:' + nodeData[23];

    if ( AoAInfo.length > 0)
        info += ' ' + AoAInfo;

    info += ' ';

    if ( Longitude.length > 1 && Latitude.length > 1)
    {
        info += ' 經緯度('+Longitude+','+Latitude+')';
    }

    row.cells[cellIndex].appendChild(document.createTextNode(info));

    var LatitudeValue = parseInt(Latitude);
    var LongitudeValue = parseInt(Longitude);
    if (LatitudeValue != 0 && LongitudeValue != 0)
    {
        var btnQrcode = document.createElement("button");
        var gmapLink = document.createElement("a");
        gmapLink.setAttribute("href","javascript:;");
        gmapLink.setAttribute("onclick","open_gmap("+ Latitude +","+ Longitude+");return false;");
        var imggmapLink = document.createElement("i");
        imggmapLink.setAttribute("class","fa fa-map-marker");
        gmapLink.appendChild(imggmapLink);
        btnQrcode.appendChild(gmapLink);
        row.cells[cellIndex].appendChild(btnQrcode);
    }

    //
    // show QR Code
    //
    var btnQrcode = document.createElement("button");
    btnQrcode.setAttribute("type","button");
    btnQrcode.setAttribute("title","Show QR Code");
    btnQrcode.setAttribute("class","btn btn-default");
    btnQrcode.setAttribute("data-toggle","modal");
    btnQrcode.setAttribute("data-target","#modal-qrcode");
    btnQrcode.setAttribute("onclick","device_qrcode_show(\""+ type +"\",\""+ nodename +"\",\""+ macaddress +"\");return false;");

    var imgQrcode = document.createElement("i");
    imgQrcode.setAttribute("class","fa fa-qrcode");
    imgQrcode.setAttribute("title","Show QR Code");
    btnQrcode.appendChild(imgQrcode);

    row.cells[cellIndex].appendChild(btnQrcode);

    //
    // show At command modal
    //
    var btnAtcmd = document.createElement("button");
    btnAtcmd.setAttribute("type","button");
    btnAtcmd.setAttribute("title","AT Command");
    btnAtcmd.setAttribute("class","btn btn-default");
    btnAtcmd.setAttribute("data-toggle","modal");
    btnAtcmd.setAttribute("data-target","#modal-atcommand");
    btnAtcmd.setAttribute("onclick","device_atcommand_show(\""+ type +"\",\""+ nodename +"\",\""+ macaddress +"\",\""+ version +"\",\""+ panid +"\",\""+ mode +"\");return false;");

    var imgAtcmd = document.createElement("i");
    imgAtcmd.setAttribute("class","fa fa-terminal");
    imgAtcmd.setAttribute("title","AT Command");
    btnAtcmd.appendChild(imgAtcmd);

    row.cells[cellIndex].appendChild(btnAtcmd);

    if(edit_node_info)
    {
        //
        // AoA 法線座標點
        //
        if (nodeid.indexOf('AOA') !== -1)
        {
            var normalVectorBtn = document.createElement("button");
            normalVectorBtn.setAttribute("type","button");
            normalVectorBtn.setAttribute("class","btn btn-default");
            normalVectorBtn.setAttribute("onclick","set_aoa_normal_vector(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodename +"\");return false;");

            var imgNormalVector = document.createElement("i");
            imgNormalVector.setAttribute("class","fa fa-long-arrow-up");
            normalVectorBtn.appendChild(imgNormalVector);

            row.cells[cellIndex].appendChild(normalVectorBtn);
        }

        // delete button
        var deleteBtn = document.createElement("button");
        deleteBtn.setAttribute("type","button");
        deleteBtn.setAttribute("class","btn btn-default");
        deleteBtn.setAttribute("onclick","delete_node_dialog(\""+ macaddress +"\",\""+ node_type.toLowerCase() +"\",\""+ nodename +"\");return false;");

        var imgDelete = document.createElement("i");
        imgDelete.setAttribute("class","fa fa-trash-o");
        deleteBtn.appendChild(imgDelete);

        row.cells[cellIndex].appendChild(deleteBtn);

    }
    cellIndex++;

    if(edit_node_info)
    {
        var loc_tab_locator_prj = document.getElementById('loc_tab_locator_prj');
        loc_tab_locator_prj.setAttribute("style","display:block;");

        row.appendChild(document.createElement('td'));

        var prj_dropdown = create_prj_dropdown(prefix, nodeid, macaddress, nodetype);
        row.cells[cellIndex].appendChild(prj_dropdown);
        cellIndex++;
    }
    else
    {
        var loc_tab_locator_prj = document.getElementById('loc_tab_locator_prj');
        loc_tab_locator_prj.setAttribute("style","display:none;");
    }

    return row;
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
    else
    if (type == '3')
        node_type = "LOCATOR";
    return node_type;
}

function get_role_name(role)
{
    var role_name = "未設定";
    if (role == '0')
        role_name = "人員訪客";
    else
    if (role == '1')
        role_name = "巡檢員";
    else
    if (role == '2')
        role_name = "設備";
    else
    if (role == '3')
        role_name = "VIP貴賓";
    else
    if (role == '4')
        role_name = "工作人員";
    else
    if (role == '5')
        role_name = "醫師";
    else
    if (role == '6')
        role_name = "護理師";
    return role_name;
}

function get_buttontype_name(buttontype)
{
    var buttontype_name = "緊急按鈕";
    if (buttontype == '0')
        buttontype_name = "緊急按鈕";
    else
    if (buttontype == '1')
        buttontype_name = "呼喚鈕";
    else
    if (buttontype == '2')
        buttontype_name = "開關裝置";
    else
    if (buttontype == '3')
        buttontype_name = "訊號採集器";
    else
    if (buttontype == '4')
        buttontype_name = "巡檢器";
    return buttontype_name;
}

function get_maplayer_name(maplayerid)
{
    var maplayer_name = '';

    for (var i=0; i<glMapLayerList.length; i++)
    {
        var MapLayerInfo = glMapLayerList[i];
        if ( MapLayerInfo.layerid == maplayerid )
        {
            maplayer_name = MapLayerInfo.layer_name;
            break;
        }
    }

    if (maplayer_name.length == 0)
    {
        var MapLayerInfo = glMapLayerList[0];
        maplayer_name = MapLayerInfo.layer_name;
    }

    return maplayer_name;
}

function add_node_to_table(table, prefix, nodelist, start_index)
{
    var show_count = 0;

    // get from index
    var nodenum = nodelist.length;
    for (var j = 0; j < nodenum; j++)
    {
        for (var i = 0; i < nodelist.length; i++)
        {
            if ( parseInt(nodelist[i][0]) == j)
            {
                var row = create_LOC_table_item(prefix, i, start_index, nodelist[i]);
                table.appendChild(row);
                //sortedList.push(nodelist[i]);
                nodelist.splice(i, 1);
                i--;
                start_index++;
                show_count++;
            }
        }
    }

    // show another
    for (var i = 0; i < nodelist.length; i++, start_index++)
    {
        var row = create_LOC_table_item(prefix, i, start_index, nodelist[i]);
        table.appendChild(row);
        show_count++;
    }
    return show_count;
}

function hasWhiteSpace(s)
{
  return s.indexOf(' ') >= 0;
}

function alphanumeric(inputtxt)
{
    var letters = /^[0-9a-zA-Z]+$/;
    if (letters.test(inputtxt))
    {
        return true;
    }
    else
    {
        return false;
    }
}

function edit_node_id(macaddress, type, curID)
{
    var newValue= prompt("Edit Node ID:(alphanumeric characters only)", curID);
    if ( newValue )
    {
        if ( hasWhiteSpace(newValue) )
    	{
    		alert("Whitespace is not allowed for Node ID.");
    	}
    	else
        if ( !alphanumeric(newValue) )
        {
            alert('Please input alphanumeric characters only');
        }
        else
    	if (newValue.length == 0)
    	{
    		alert("Node id is not valid.");
    	}
        else
        {
            var element_id = "LOC_"+macaddress+"_NODID";
            //span_nodeid.setAttribute("class","badge bg-light-blue");
            document.getElementById(element_id).innerText = newValue;
            //document.getElementById(element_id).innerHTML = 'Sync Local Devices to Server <i class=fa fa-fw fa-cloud-upload"></i>';
            update_node_info(macaddress, type, newValue, "", "", "", "", "", "", "", "", "");
        }
    }
}


function edit_taf_id(macaddress, type, curTAFID)
{
    //alert("edit_taf_id() MAC:"+macaddress +", type:"+type+",curTAFID:"+curTAFID);
    var newValue = prompt("Edit Node TAF ID:(100 ~ 200)", curTAFID);
    if ( newValue )
    {
        if ( hasWhiteSpace(newValue) )
    	{
    		alert("Whitespace is not allowed for TAF ID.");
    	}
    	else
        if ( !alphanumeric(newValue) )
        {
            alert('Please input alphanumeric characters only');
        }
        else
    	if (newValue.length == 0)
    	{
    		alert("TAF id is not valid.");
    	}
        else
        {
            var element_id = "LOC_"+macaddress+"_TAFID";
            //span_nodeid.setAttribute("class","badge bg-light-blue");
            document.getElementById(element_id).innerText = newValue;
            //document.getElementById(element_id).innerHTML = 'Sync Local Devices to Server <i class=fa fa-fw fa-cloud-upload"></i>';
            update_node_info(macaddress, type, "", newValue, "", "", "", "", "", "", "", "");
        }
    }
}

function edit_node_name(macaddress, type, curName)
{
    var newValue= prompt("Edit Node name:(Human readable name)", curName);
    if ( newValue )
    {
        var element_id = "LOC_"+macaddress+"_NODNAME";
        //span_nodeid.setAttribute("class","badge bg-light-blue");
        document.getElementById(element_id).innerText = newValue;

        update_node_info(macaddress, type, "", "", "", "", newValue, "", "", "", "", "");

    }
}

function edit_LoRA_macaddress(macaddress, type, curMacaddress)
{
    var newValue= prompt("Edit LoRA macaddress:", curMacaddress);
    if ( newValue )
    {
        var element_id = "LOC_"+macaddress+"_LORAMACADDRESS";
        //span_nodeid.setAttribute("class","badge bg-light-blue");
        document.getElementById(element_id).innerText = newValue;

        update_node_info(macaddress, type, "", "", "", "", "", "", "", "", newValue, "");

    }
}

function edit_node_buildingid(macaddress, type, curBuildingID)
{
    var newValue= prompt("Edit Building ID:", curBuildingID);
    if ( newValue )
    {
        var element_id = "LOC_"+macaddress+"_BUILDINGID";
        document.getElementById(element_id).innerText = newValue;
        var element_floor_id = "LOC_"+macaddress+"_BUILDINGFLOOR";
        var floor = document.getElementById(element_floor_id).innerText;

        update_node_info(macaddress, type, "", "", newValue, floor, "", "", "", "", "", "");
    }
}

function edit_node_buildingfloor(macaddress, type, curBuildingFloor)
{
    var newValue= prompt("Edit Building Floor:", curBuildingFloor);
    if ( newValue )
    {
        var element_id = "LOC_"+macaddress+"_BUILDINGFLOOR";
        document.getElementById(element_id).innerText = newValue;
        var element_build_id = "LOC_"+macaddress+"_BUILDINGID";
        var build = document.getElementById(element_build_id).innerText;

        update_node_info(macaddress, type, "", "", build, newValue, "", "", "", "", "", "");
    }
}

function edit_node_blacklist(macaddress, type, curblacklist)
{
    var newValue= prompt("Edit black list:", curblacklist);
    if ( newValue != null)
    {
        // var element_id = "LOC_"+macaddress+"_BLACKLIST";
        //
        // document.getElementById(element_id).innerText = newValue;
        update_node_blacklist(macaddress, newValue);
    }
}

function edit_node_whitelist(macaddress, type, curwhitelist)
{
    var newValue = prompt("Edit white list:", curwhitelist);

    if ( newValue != null)
    {
        //var element_id = "LOC_"+macaddress+"_WHITELIST";
        //document.getElementById(element_id).innerText = newValue;

        update_node_whitelist(macaddress, newValue);
    }
}

function edit_node_joinRangingRange(macaddress, type, curjoinRangingRange)
{
    var newValue = prompt("Edit join Ranging Range:", curjoinRangingRange);

    if ( newValue != null)
    {
        update_node_joinRangingRange(macaddress, newValue);
    }
}

function edit_node_joinNotRangingRange(macaddress, type, curjoinNotRangingRange)
{
    var newValue = prompt("Edit join Not Ranging Range:", curjoinNotRangingRange);

    if ( newValue != null)
    {
        update_node_joinNotRangingRange(macaddress, newValue);
    }
}

function edit_node_position(type, macaddress, posX, posY, posZ)
{
    //alert("MAC:"+macaddress +", position:"+posX+","+posY+","+posZ);

    var value = ""+posX+","+posY+","+posZ;
    var newValue= prompt("Edit positin: (format: X,Y,Z)", value);
    if ( newValue )
    {
        //alert("MAC:"+macaddress +", value:"+posX+","+posY+","+posZ +", newValue="+newValue);
        //check valid
        var check1 = newValue.split(",");
        if (check1.length != 3)
        {
            alert("Error format: need X,Y,Z");
            return;
        }

        var position = newValue.split(",");

        gl_updating_element_id = ""+macaddress;
        if ( type === 'locator' || type === 'anchor' || type === 'coord')
        {
            var pos_id = "LOC_"+gl_updating_element_id+"_POS";
            var link_id = "LOC_"+gl_updating_element_id+"_LINK";
            document.getElementById(pos_id).innerText = "("+newValue+")";
            document.getElementById(link_id).setAttribute("onclick","edit_node_position(\""+type+"\",\""+ macaddress +"\",\""+ position[0]+"\",\""+position[1]+"\",\""+position[2] +"\");return false;");
        }
        else
        {
            node_info_updating();
        }

        update_node_info(macaddress, type, "", "", "", "", "", position[0], position[1], position[2], "", "");
    }
}

function edit_role(prefix, nodeid, macaddress, type, role)
{
    var newValue= prompt("Edit Role: (0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員)", role);
    if ( newValue )
    {
        if (newValue != '0' && newValue != '1' && newValue != '2' && newValue != '3' && newValue != '4')
        {
            alert("Error format: need 0,1,2,3 or 4");
            return;
        }

        var element_id = "LOC_"+macaddress+"_ROLE";
        document.getElementById(element_id).innerText = get_role_name(newValue);
        update_node_info(macaddress, type, "", newValue, "", "", "", "", "", "", "", "");

        var dropdown_id = "dropdown_tag_"+macaddress;
        var dropdown_ui = document.getElementById(dropdown_id);
        while (dropdown_ui.firstChild)
        {
            dropdown_ui.removeChild(dropdown_ui.firstChild);
        }

        // if ( newValue == '4' )
        // {
        //     add_employee_dropdown_list(dropdown_ui, prefix, nodeid, macaddress, "tag", "-1");
        // }
        // else
        // if ( newValue == '0' )
        // {
        //     add_resident_dropdown_list(dropdown_ui, prefix, nodeid, macaddress, "tag", "-1");
        // }
    }
}

function node_info_updating()
{
    if (gl_updating_element_id.length == 0)
        return;

    var pos_id = "LOC_"+gl_updating_element_id+"_POS";
    var link_id = "LOC_"+gl_updating_element_id+"_LINK";

    document.getElementById(pos_id).innerText = "Updating...";
    document.getElementById(link_id).style.visibility = 'hidden';
    setTimeout(node_info_updating, 1000);
}

function update_node_info(macaddress, type, nodeid, tafid, buildingid, buildingfloor, nodename, posx, posy, posz, loramacaddress, maplayerid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['EDIT_MODE'] = 1;
    jsondata['MAC'] = macaddress;
    jsondata['TYPE'] = type;
    if ( nodeid.length > 0 )
        jsondata['NODEID'] = nodeid;
    if ( tafid.length > 0 )
        jsondata['TAFID'] = tafid;
    if ( buildingid.length > 0 )
        jsondata['BUILDINGID'] = buildingid;
    if ( buildingfloor.length > 0 )
        jsondata['BUILDINGFLOOR'] = buildingfloor;
    if ( nodename.length > 0 )
        jsondata['NODENAME'] = nodename;
    if ( posx.length > 0 )
        jsondata['POSX'] = posx;
    if ( posy.length > 0 )
        jsondata['POSY'] = posy;
    if ( posz.length > 0 )
        jsondata['POSZ'] = posz;
    if ( loramacaddress.length > 0 )
        jsondata['LORAMACADDRESS'] = loramacaddress;
    if ( maplayerid.length > 0 )
        jsondata['maplayerid'] = maplayerid;

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
            else
            {
                //alert("update success! response = " + JSON.stringify(response));
                //var pos_id = gl_updating_element_id+"_POS";
                //var position = '('+jsondata['POSX']+','+jsondata['POSY']+','+jsondata['POSZ']+')';
                //document.getElementById(pos_id).innerText = position;
            }

            gl_updating_element_id = "";
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
            gl_updating_element_id = "";
        }
   });
   return false;
}

function update_node_blacklist(macaddress, newValue)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;

    if (newValue.length == 0)
    {
        jsondata['CLEAR_BLACKLIST'] = 1;
        jsondata['MAC'] = macaddress;
    }
    else
    {
        jsondata['UPDATE_BLACKLIST'] = 1;
        jsondata['MAC'] = macaddress;
        jsondata['LIST'] = newValue;
    }

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
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });

   return false;
}

function update_node_whitelist(macaddress, newValue)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;

    if (newValue.length == 0)
    {
        jsondata['CLEAR_WHITELIST'] = 1;
        jsondata['MAC'] = macaddress;
    }
    else
    {
        jsondata['UPDATE_WHITELIST'] = 1;
        jsondata['MAC'] = macaddress;
        jsondata['LIST'] = newValue;
    }

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
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });

   return false;
}

function update_node_joinRangingRange(macaddress, newValue)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['UPDATE_JOINRANGINGRANGE'] = 1;
    jsondata['MAC'] = macaddress;
    jsondata['RANGE'] = newValue;

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
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });

   return false;
}

function update_node_joinNotRangingRange(macaddress, newValue)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['UPDATE_JOINNOTRANGINGRANGE'] = 1;
    jsondata['MAC'] = macaddress;
    jsondata['RANGE'] = newValue;

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
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });

   return false;
}

function update_locator_buttontype(macaddress, type, buttontype)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['UPDATE_LOCATOR_BUTTONTYPE'] = 1;
    jsondata['MAC'] = macaddress;
    jsondata['TYPE'] = type;
    jsondata['BUTTONTYPE'] = buttontype;

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
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });

   return false;
}

function update_locator_SensorAlert(macaddress, enable)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['UPDATE_LOCATOR_SENSORALERT'] = 1;
    jsondata['MAC'] = macaddress;
    jsondata['TYPE'] = 'locator';
    if (enable)
        jsondata['SENSORALERT'] = 1;
    else
        jsondata['SENSORALERT'] = 0;

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
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });

   return false;
}

function update_node_prj(nodeid, macaddress, nodetype, target_projectid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['UPDATE_NODE_PEROJECT'] = 1;
    jsondata['MAC'] = macaddress;
    jsondata['TYPE'] = nodetype;
    jsondata['target_projectid'] = target_projectid;

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
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });

   return false;
}

function update_node_user(macaddress, nodetype, userid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['UPDATE_NODE_USER'] = 1;
    jsondata['MAC'] = macaddress;
    jsondata['TYPE'] = nodetype;
    jsondata['userid'] = userid;

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
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });

   return false;
}

function delete_node_dialog(macaddress, type, nodename)
{
    if (confirm('Are you sure you want to delete this node '+type+':'+nodename+' ?'))
    {
        // delete it!
        //alert('delete node '+type+':'+nodename+':'+macaddress);
        delete_node(macaddress, type);
    }
    else
    {
        // Do nothing!
    }
}

function delete_node(macaddress, type)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['DELETE_NODE'] = 1;
    jsondata['MAC'] = macaddress;
    jsondata['TYPE'] = type;

    stop_update_tag_info = false;
    stop_update_locator_info = false;

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
                alert("DELETE Error! response = " + JSON.stringify(response));
            }
            else
            {
                var target_id = macaddress+"_TABLE_ROW";
                var element = document.getElementById(target_id);
                element.parentNode.removeChild(element);

                //alert("update success! response = " + JSON.stringify(response));
                //var pos_id = gl_updating_element_id+"_POS";
                //var position = '('+jsondata['POSX']+','+jsondata['POSY']+','+jsondata['POSZ']+')';
                //document.getElementById(pos_id).innerText = position;
            }
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });
   return false;
}

function delete_gateway_dialog(gatewayid)
{
    if (confirm('Are you sure you want to delete this gatewayid:'+gatewayid+' ?'))
    {
        // delete it!
        delete_gateway(gatewayid);
    }
    else
    {
        // Do nothing!
    }
}

function delete_gateway(gatewayid)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['DELETE_GATEWAY'] = 1;
    jsondata['ID'] = gatewayid;

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
                alert("DELETE Error! response = " + JSON.stringify(response));
            }
            else
            {
                var target_id = gatewayid+"_TABLE_ROW";
                var element = document. getElementById(target_id);
                element.parentNode.removeChild(element);
            }
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });
   return false;
}

function delete_all_offline_device(nodetype)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['DELETE_ALL_OFFLINE_DEVICE'] = 1;
    jsondata['TYPE'] = nodetype;

    stop_update_tag_info = false;
    stop_update_locator_info = false;

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
                alert("DELETE Error! response = " + JSON.stringify(response));
            }
            else
            {
                if (nodetype === 'tag')
                {
                    clear_tag_table();
                    gl_tag_num = 0;
                    gl_tag_load_index = 0;
                }
                if (nodetype === 'anchor')
                {
                    clear_anchor_table();
                    gl_anchor_num = 0;
                    gl_anchor_load_index = 0;
                }
                if (nodetype === 'coord')
                {
                    clear_coord_table();
                    gl_coord_num = 0;
                    gl_coord_load_index = 0;
                }
                if (nodetype === 'locator')
                {
                    clear_locator_table();
                    gl_locator_load_index = 0;
                    locator_num = 0;
                }
                //lert("delete_all_offline_device() finished!");
            }
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });
   return false;
}

function set_aoa_normal_vector(macaddress, type, nodename)
{
    var newValue = prompt("設定 AOA 法線座標點(X,Y,Z)與傾側角度(Roll angle):", '0,0,0,0');

    if ( newValue != null)
    {
        var position = newValue.split(",");
        if (position.length != 4)
        {
            alert("Error format: need X,Y,Z,Roll angle");
            return;
        }

        update_aoa_normal_vector(macaddress, type, position[0], position[1], position[2], position[3]);
    }
}

function update_aoa_normal_vector(macaddress, type, posx, posy, posz, roll)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['update_aoa_normal_vector'] = 1;
    jsondata['MAC'] = macaddress;
    jsondata['POSX'] = posx;
    jsondata['POSY'] = posy;
    jsondata['POSZ'] = posz;
    jsondata['ROLL'] = roll;

    // console.log("update_aoa_normal_vector() macaddress:"+macaddress+", posx:"+posx +" posy:"+posy+" posz:" + posz + " roll:"+roll);

    var targetURL = gl_target_server + "/php/vilsnodes.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
   });
   return false;
}

$("#toggleeditnodebtn").click(function()
{
    //console.log("toggleeditnodebtn");

    if (edit_node_info)
        edit_node_info = false;
    else
        edit_node_info = true;

    //load_loragateway_info();

   return false;
});

$("#createnewlocatorbutton").click(function()
{

    var newValue= prompt("New Locator UWB MAC Address: (format:053000000000xxxx)","053000000000xxxx");
    if ( newValue )
    {
        //alert("MAC:"+macaddress +", value:"+posX+","+posY+","+posZ +", newValue="+newValue);
        //check valid
        if (newValue.length != 16 || !isANumber(newValue))
        {
            alert("Error format: need 053000000000xxxx");
            return;
        }
        var newID = newValue.substring(12);
        //var loramacaddress = "0000000000050012";
        var loramacaddress = "000000000005" + newID;

        update_node_info(newValue, "locator", "Locator"+newID, "0", "1", "1", "Locator"+newID, "0", "0", "0", loramacaddress, "");
        //update_node_info(macaddress, type, nodeid, tafid, buildingid, buildingfloor, nodename, posx, posy, posz, loramacaddress)
    }

   return false;
});

// $("#deleteofflinelocatorbutton").click(function()
// {
//     if (confirm('Are you sure you want to delete All Offline Locators ?'))
//     {
//         delete_all_offline_device("locator");
//     }
//     return false;
// });

// $("#deleteofflinecoordbutton").click(function()
// {
//     if (confirm('Are you sure you want to delete All Offline Coordinators ?'))
//     {
//         delete_all_offline_device("coord");
//     }
//     return false;
// });
//
// $("#deleteofflineanchorbutton").click(function()
// {
//     if (confirm('Are you sure you want to delete All Offline Anchors ?'))
//     {
//         delete_all_offline_device("anchor");
//     }
//     return false;
// });

// $("#deleteofflinetagbutton").click(function()
// {
//     if (confirm('Are you sure you want to delete All Offline Tags ?'))
//     {
//         delete_all_offline_device("tag");
//     }
//     return false;
// });

function reset_m4_dialog(macaddress, nodname)
{
    if (confirm('Are you sure you want to reset M4 of this device:'+nodname+' ?'))
    {
        // reset it!
        reset_m4(macaddress);
    }
    else
    {
        // Do nothing!
    }
}

function reset_m4(macaddress)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['RESET_M4'] = 1;
    jsondata['MAC'] = macaddress;

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
                alert("DELETE Error! response = " + JSON.stringify(response));
            }
        },
    error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

    return false;
}

function reset_m0_dialog(macaddress, nodname)
{
    if (confirm('Are you sure you want to reset M0 of this device:'+nodname+' ?'))
    {
        // reset it!
        reset_m0(macaddress);
    }
    else
    {
        // Do nothing!
    }
}

function reset_m0(macaddress)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['RESET_M0'] = 1;
    jsondata['MAC'] = macaddress;

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
                alert("DELETE Error! response = " + JSON.stringify(response));
            }
        },
    error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });

    return false;
}

var glDropdown_checkbox_filter_click = false;
function dropdown_filter_click(filertype)
{
    var checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_filter_"+filertype).checked;

    if (glDropdown_checkbox_filter_click)
    {
        // keep the same value
    }
    else
    {
        // toggle value
        document.getElementById("dropdown_checkbox_filter_"+filertype).checked = !checkedValue_dropdown_checkbox;
        checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_filter_"+filertype).checked;
    }

    if (filertype === 'show_online' )
    {
        gl_showfilter_showonline = checkedValue_dropdown_checkbox;
    }
    else
    if (filertype === 'show_offline' )
    {
        gl_showfilter_showoffline = checkedValue_dropdown_checkbox;
    }

    glDropdown_checkbox_filter_click = false;
}

function dropdown_checkbox_filter_click(filertype)
{
    glDropdown_checkbox_filter_click = true;
}

function msToDuration(ms)
{
    var duration = moment.duration(ms, 'milliseconds');

    var days = parseInt(duration.asDays());
    var hours = parseInt(duration.asHours());
    var minutes = parseInt(duration.asMinutes());
    var seconds = parseInt(duration.asSeconds());

    //console.log(days+":"+hours+":"+minutes+":"+seconds);

    var days_hours = days * 24;
    hours = hours - days_hours;

    var hours_mins = (days_hours + hours) * 60;
    minutes = minutes - hours_mins;

    var mins_secs = (hours_mins + minutes) * 60;
    seconds = seconds - mins_secs;

    //console.log(days+":"+hours+":"+minutes+":"+seconds);

    var perty_str = "";
    if (days > 0)
        perty_str += days + "天";
    if (hours > 0)
        perty_str += hours + "小時";
    if (minutes > 0)
        perty_str += minutes + "分";
    if (seconds > 0)
        perty_str += seconds + "秒";
    return perty_str;
}

function pad(num)
{
    return ("0"+num).slice(-2);
}

function hhmmss(secs)
{
      var minutes = Math.floor(secs / 60);
      secs = secs%60;
      var hours = Math.floor(minutes/60)
      minutes = minutes%60;
      var days = Math.floor(hours/24)
      hours = hours%24;
      var months = Math.floor(days/30)
      days = days%30;
      return `00/${pad(months)}/${pad(days)} ${pad(hours)}:${pad(minutes)}:${pad(secs)}`;
      // return pad(hours)+":"+pad(minutes)+":"+pad(secs); for old browsers
}

function convertPrettyTime(inDate)
{
    var cur_date = moment(inDate, "MM/dd HH:mm:ss");
    var day = cur_date.day();
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    console.log("inDate:"+inDate+", cur_date:"+cur_date);

    var perty_str = "";
    if (day > 0)
        perty_str += day + "天";
    if (hour > 0)
        perty_str += hour + "小時";
    if (minutes > 0)
        perty_str += minutes + "分";
    if (seconds > 0)
        perty_str += seconds + "秒";

    return perty_str;
}

function add_user_dropdown_list(dropdown_tags_ul, nodetype, nodeid, macaddress, userid)
{
    var item_li = create_dropdown_user_item("-1", "不指定", false, nodetype, nodeid, macaddress);
    dropdown_tags_ul.appendChild(item_li);

    for (var i = 0; i < gl_user_info.length; i++)
    {
        var defaultchecked = false;
        if ( gl_user_info[i].userid == userid )
            defaultchecked = true;

        var item_li = create_dropdown_user_item(gl_user_info[i].userid, gl_user_info[i].name, defaultchecked, nodetype, nodeid, macaddress);
        dropdown_tags_ul.appendChild(item_li);
    }
}

function create_dropdown_user_item(userid, name, defaultchecked, nodetype, nodeid, macaddress)
{
    //console.log("create_dropdown_tag_emp_item() prefix:"+prefix+" nodeid:"+nodeid+" macaddress:"+macaddress+" type:"+type+" serno:"+serno);

    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", userid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_user_click(\""+ nodetype +"\", \""+ nodeid +"\", \""+ macaddress +"\", \""+ userid +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_user_"+userid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_user_click(\""+ nodetype +"\", \""+ nodeid +"\", \""+ macaddress +"\", \""+ userid +"\");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+name));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_user_click = false;
function dropdown_user_click(nodetype, nodeid, macaddress, userid)
{

    console.log("dropdown_user_click() nodetype:"+nodetype+" nodeid:"+nodeid+" macaddress:"+macaddress+" userid:"+userid);

    // uncheck anothers
    var index;
    var showText = "";
    for	(index = 0; index < gl_user_info.length; index++)
    {
        if (userid == gl_user_info[index].userid)
        {
            showText = gl_user_info[index].name;
            document.getElementById("dropdown_checkbox_user_"+gl_user_info[index].userid).checked = true;
        }
        else
            document.getElementById("dropdown_checkbox_user_"+gl_user_info[index].userid).checked = false;
    }

    //var nodename_id = prefix+"_"+macaddress+"_NODNAME";
    if (userid == '-1')
    {
        console.log("dropdown_user_click() nodename_id:"+nodename_id);

        document.getElementById("dropdown_checkbox_user_-1").checked = true;
        //document.getElementById(nodename_id).textContent = nodeid;
        document.getElementById("dropdown_cur_user_"+macaddress).innerHTML = '<span class="caret"></span>';
    }
    else
    {
        //document.getElementById(nodename_id).textContent = showText;
        document.getElementById("dropdown_cur_user_"+macaddress).innerHTML = showText + '<span class="caret"></span>';
    }

    console.log("dropdown_user_click() macaddress:"+macaddress+" nodetype:"+nodetype+" -> userid:"+userid);

    update_node_user(macaddress, nodetype, userid);

    glDropdown_checkbox_user_click = false;
}

function dropdown_checkbox_user_click(prefix, nodeid, macaddress, type, userid)
{
    glDropdown_checkbox_user_click = true;
}


//============================================
//  buttontype
function add_buttontype_dropdown_list(dropdown_buttontype_ul, prefix, nodeid, macaddress, type, buttontype)
{
    var item_li_0 = create_dropdown_buttontype_item('0', get_buttontype_name('0'), (buttontype === '0'), prefix, nodeid, macaddress, type);
    dropdown_buttontype_ul.appendChild(item_li_0);

    var item_li_1 = create_dropdown_buttontype_item('1', get_buttontype_name('1'), (buttontype === '1'), prefix, nodeid, macaddress, type);
    dropdown_buttontype_ul.appendChild(item_li_1);

    var item_li_2 = create_dropdown_buttontype_item('2', get_buttontype_name('2'), (buttontype === '2'), prefix, nodeid, macaddress, type);
    dropdown_buttontype_ul.appendChild(item_li_2);

    var item_li_3 = create_dropdown_buttontype_item('3', get_buttontype_name('3'), (buttontype === '3'), prefix, nodeid, macaddress, type);
    dropdown_buttontype_ul.appendChild(item_li_3);

    var item_li_4 = create_dropdown_buttontype_item('4', get_buttontype_name('4'), (buttontype === '4'), prefix, nodeid, macaddress, type);
    dropdown_buttontype_ul.appendChild(item_li_4);
}

function create_dropdown_buttontype_item(buttontype, buttontype_name, defaultchecked, prefix, nodeid, macaddress, type)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", buttontype);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_buttontype_click(\""+ prefix +"\", \""+ nodeid +"\", \""+ macaddress +"\", \""+ type +"\", \""+ buttontype +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_buttontype_"+macaddress+"_"+buttontype);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_buttontype_click(\""+ prefix +"\", \""+ nodeid +"\", \""+ macaddress +"\", \""+ type +"\", \""+ buttontype +"\");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(buttontype_name));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_buttontype_click = false;
function dropdown_buttontype_click(prefix, nodeid, macaddress, type, buttontype)
{
    console.log("dropdown_buttontype_click() macaddress:"+macaddress+" type:"+type+" -> buttontype:"+buttontype);

    // uncheck anothers
    var showText = "";
    if ( buttontype === '0' )
    {
        showText = get_buttontype_name('0');
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_0").checked = true;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_1").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_2").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_3").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_4").checked = false;
    }
    else
    if ( buttontype === '1' )
    {
        showText = get_buttontype_name('1');
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_0").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_1").checked = true;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_2").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_3").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_4").checked = false;
    }
    else
    if ( buttontype === '2' )
    {
        showText = get_buttontype_name('2');
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_0").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_1").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_2").checked = true;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_3").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_4").checked = false;
    }
    else
    if ( buttontype === '3' )
    {
        showText = get_buttontype_name('3');
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_0").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_1").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_2").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_3").checked = true;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_4").checked = false;
    }
    else
    if ( buttontype === '4' )
    {
        showText = get_buttontype_name('4');
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_0").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_1").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_2").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_3").checked = false;
        document.getElementById("dropdown_checkbox_buttontype_"+macaddress+"_4").checked = true;
    }

    document.getElementById("dropdown_cur_buttontype_"+macaddress).innerHTML = showText + '<span class="caret"></span>';

    update_locator_buttontype(macaddress, type, buttontype);

    glDropdown_checkbox_buttontype_click = false;
}

function dropdown_checkbox_buttontype_click(prefix, nodeid, macaddress, type, empid)
{
    glDropdown_checkbox_buttontype_click = true;
}
//============================================


//============================================
//  tag role
function add_role_dropdown_list(dropdown_role_ul, prefix, nodeid, macaddress, type, role)
{
    var item_li_0 = create_dropdown_role_item('0', get_role_name('0'), (role === '0'), prefix, nodeid, macaddress, type);
    dropdown_role_ul.appendChild(item_li_0);

    var item_li_1 = create_dropdown_role_item('1', get_role_name('1'), (role === '1'), prefix, nodeid, macaddress, type);
    dropdown_role_ul.appendChild(item_li_1);

    var item_li_2 = create_dropdown_role_item('2', get_role_name('2'), (role === '2'), prefix, nodeid, macaddress, type);
    dropdown_role_ul.appendChild(item_li_2);

    var item_li_3 = create_dropdown_role_item('3', get_role_name('3'), (role === '3'), prefix, nodeid, macaddress, type);
    dropdown_role_ul.appendChild(item_li_3);

    var item_li_4 = create_dropdown_role_item('4', get_role_name('4'), (role === '4'), prefix, nodeid, macaddress, type);
    dropdown_role_ul.appendChild(item_li_4);

    var item_li_5 = create_dropdown_role_item('5', get_role_name('5'), (role === '5'), prefix, nodeid, macaddress, type);
    dropdown_role_ul.appendChild(item_li_5);

    var item_li_6 = create_dropdown_role_item('6', get_role_name('6'), (role === '6'), prefix, nodeid, macaddress, type);
    dropdown_role_ul.appendChild(item_li_6);
}

function create_dropdown_role_item(role_type, role_name, defaultchecked, prefix, nodeid, macaddress, type)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", role_type);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_role_click(\""+ prefix +"\", \""+ nodeid +"\", \""+ macaddress +"\", \""+ type +"\", \""+ role_type +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_role_"+macaddress+"_"+role_type);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_role_click(\""+ prefix +"\", \""+ nodeid +"\", \""+ macaddress +"\", \""+ type +"\", \""+ role_type +"\");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(role_name));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_role_click = false;
function dropdown_role_click(prefix, nodeid, macaddress, type, role_type)
{
    console.log("dropdown_role_click() macaddress:"+macaddress+" type:"+type+" -> role_type:"+role_type);

    // uncheck anothers
    var showText = get_role_name(role_type);

    document.getElementById("dropdown_checkbox_role_"+macaddress+"_0").checked = false;
    document.getElementById("dropdown_checkbox_role_"+macaddress+"_1").checked = false;
    document.getElementById("dropdown_checkbox_role_"+macaddress+"_2").checked = false;
    document.getElementById("dropdown_checkbox_role_"+macaddress+"_3").checked = false;
    document.getElementById("dropdown_checkbox_role_"+macaddress+"_4").checked = false;
    document.getElementById("dropdown_checkbox_role_"+macaddress+"_5").checked = false;
    document.getElementById("dropdown_checkbox_role_"+macaddress+"_6").checked = false;

    document.getElementById("dropdown_checkbox_role_"+macaddress+"_"+role_type).checked = true;

    document.getElementById("dropdown_cur_role_"+macaddress).innerHTML = showText + '<span class="caret"></span>';

    update_node_info(macaddress, type, "", role_type, "", "", "", "", "", "", "", "");

    glDropdown_checkbox_role_click = false;
}

function dropdown_checkbox_role_click(prefix, nodeid, macaddress, type, role_type)
{
    glDropdown_checkbox_role_click = true;
}
//============================================



//============================================
//  map layer
function add_maplayer_dropdown_list(dropdown_ul, prefix, nodeid, macaddress, nodetype, maplayerid)
{
    for (var i=0; i<glMapLayerList.length; i++)
    {
        var MapLayerInfo = glMapLayerList[i];

        var item_li = create_dropdown_maplayer_item(MapLayerInfo.layerid, MapLayerInfo.layer_name, (MapLayerInfo.layerid === maplayerid), prefix, nodeid, macaddress, nodetype);
        dropdown_ul.appendChild(item_li);
    }
}

function create_dropdown_maplayer_item(layerid, layer_name, defaultchecked, prefix, nodeid, macaddress, nodetype)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", layerid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_maplayer_click(\""+ prefix +"\", \""+ nodeid +"\", \""+ macaddress +"\", \""+ nodetype +"\", \""+ layerid +"\", \""+ layer_name +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_maplayer_"+macaddress+"_"+layerid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_maplayer_click(\""+ prefix +"\", \""+ nodeid +"\", \""+ macaddress +"\", \""+ nodetype +"\", \""+ layerid +"\", \""+ layer_name +"\");return true;");
    item_checkbox.checked = defaultchecked;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(layer_name));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_maplayer_click = false;
function dropdown_maplayer_click(prefix, nodeid, macaddress, nodetype, layerid, layer_name)
{
    console.log("dropdown_maplayer_click() macaddress:"+macaddress+" nodetype:"+nodetype+" -> layerid:"+layerid);

    for (var i=0; i<glMapLayerList.length; i++)
    {
        var MapLayerInfo = glMapLayerList[i];
        document.getElementById("dropdown_checkbox_maplayer_"+macaddress+"_"+MapLayerInfo.layerid).checked = false;
    }
    document.getElementById("dropdown_checkbox_maplayer_"+macaddress+"_"+layerid).checked = true;

    document.getElementById("dropdown_cur_maplayer_"+macaddress).innerHTML = layer_name + '<span class="caret"></span>';

    update_node_info(macaddress, nodetype, "", "", "", "", "", "", "", "", "", layerid);

    glDropdown_checkbox_maplayer_click = false;
}

function dropdown_checkbox_maplayer_click(prefix, nodeid, macaddress, nodetype, layerid, layer_name)
{
    glDropdown_checkbox_maplayer_click = true;
}
//============================================


//============================================
function create_prj_dropdown(prefix, nodeid, macaddress, nodetype)
{
    //var block = document.createTextNode(nodeid+'_'+macaddress+'_'+nodetype);
    var prj_dropdown = document.createElement("div");
    prj_dropdown.setAttribute("class","btn-group");

    var div_btn = document.createElement("button");
    div_btn.setAttribute("type","button");
    div_btn.setAttribute("class","btn btn-block btn-primary btn-xs dropdown-toggle");
    div_btn.setAttribute("data-toggle","dropdown");
    div_btn.textContent = "";
    prj_dropdown.appendChild(div_btn);

    var div_btn_span = document.createElement("span");
    div_btn_span.setAttribute("class","caret");
    div_btn.appendChild(div_btn_span);

    var div_ul = document.createElement("ul");
    div_ul.setAttribute("class","dropdown-menu");
    div_ul.setAttribute("id","dropdown_prj_"+macaddress);
    prj_dropdown.appendChild(div_ul);

    for (var i = 0; i < gl_prj_info.length; i++)
    {
        var defaultchecked = check_existed_in_prj(gl_prj_info[i].PROJECT_ID, nodeid, nodetype);
        var item_li = create_dropdown_prj_item(gl_prj_info[i].PROJECT_ID, gl_prj_info[i].PROJECT_NAME, defaultchecked, prefix, nodeid, macaddress, nodetype);
        div_ul.appendChild(item_li);
    }

    return prj_dropdown;
}

function create_dropdown_prj_item(projectid, projectname, defaultchecked, prefix, nodeid, macaddress, nodetype)
{
    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", projectid);
    item_click.setAttribute("id","dropdown_label_prj_"+macaddress+"_"+projectid);
    if (defaultchecked)
    {
        item_click.setAttribute("disabled","true");
    }
    else
    {
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","dropdown_prj_click(\""+ prefix +"\", \""+ nodeid +"\", \""+ macaddress +"\", \""+ nodetype +"\", \""+ projectid +"\", \""+ projectname +"\");return true;");
    }

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_prj_"+macaddress+"_"+projectid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_prj_click();return true;");
    item_checkbox.checked = defaultchecked;
    if (defaultchecked)
    {
        item_checkbox.setAttribute("disabled","true");
    }

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(projectname));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_prj_click = false;
function dropdown_prj_click(prefix, nodeid, macaddress, nodetype, projectid, projectname)
{
    //console.log("dropdown_prj_click() macaddress:"+macaddress+" nodetype:"+nodetype+" -> projectid:"+projectid);
    var item_label = document.getElementById("dropdown_label_prj_"+macaddress+"_"+projectid);
    var item_checkbox = document.getElementById("dropdown_checkbox_prj_"+macaddress+"_"+projectid);

    if (confirm('要將 '+nodeid+' 複製至專案 ' + projectname+ '?'))
    {
        update_node_prj(nodeid, macaddress, nodetype, projectid);

        item_checkbox.checked = true;
        item_checkbox.setAttribute("disabled","true");
        item_label.removeAttribute("href");
        item_label.removeAttribute("onclick");
    }
    else
    {
        item_checkbox.checked = false;
        item_checkbox.removeAttribute("disabled");
    }

    glDropdown_checkbox_prj_click = false;
}

function dropdown_checkbox_prj_click()
{
    glDropdown_checkbox_prj_click = true;
}

function check_existed_in_prj(PROJECT_ID, nodeid, nodetype)
{
    var defaultchecked = false;
    var cur_node_list = gl_tag_prj_info;
    if ( nodetype == 'anchor')
        cur_node_list = gl_anchor_prj_info;
    if ( nodetype == 'tag')
        cur_node_list = gl_tag_prj_info;
    if ( nodetype == 'coord')
        cur_node_list = gl_coord_prj_info;
    if ( nodetype == 'locator')
        cur_node_list = gl_locator_prj_info;
    if ( nodetype == 'gateway')
        cur_node_list = gl_gateway_prj_info;

    if ( cur_node_list[nodeid] != undefined )
    {
        var prjid = parseInt(PROJECT_ID);
        for (var i = 0; i < cur_node_list[nodeid].length; i++)
        {
            if ( prjid == cur_node_list[nodeid][i] )
            {
                return true;
            }
        }
    }
    return defaultchecked;
}
//============================================

function findUserName(userid)
{
    var UserName = "";

    for (var i = 0; i < gl_user_info.length; i++)
    {
        if ( userid == gl_user_info[i].userid)
        {
            //console.log('findUserName() gl_user_info[i].userid:'+gl_user_info[i].userid);

            UserName = gl_user_info[i].name;
            break;
        }
    }
    return UserName;
}

function device_qrcode_show(type, device_name, device_macaddress)
{
    console.log('device_qrcode_show() type:'+type+" device_name:"+device_name+" device_macaddress:"+device_macaddress);

    var getType = "TAG";
    if (type === '1')
        getType = "TAG";
    if (type === '3')
        getType = "LOCATOR";


    var device_qrcode_type = document.getElementById("device_qrcode_type");
    device_qrcode_type.textContent = 'Type: ' + getType;
    var device_qrcode_name = document.getElementById("device_qrcode_name");
    device_qrcode_name.textContent = '名稱: ' + device_name;
    var device_qrcode_macaddress = document.getElementById("device_qrcode_macaddress");
    device_qrcode_macaddress.textContent = '硬體位址: ' + device_macaddress;

    var SendDate = "loadqrcode=1&type="+getType+"&accountid=" + device_macaddress+"&id=" + device_macaddress+'&project_id='+project_id;
    var targetURL = gl_target_server + "/php/configvils.php";

    //console.log("get_qrcode() "+targetURL);

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: SendDate,
      dataType: 'text',
      success:
        function(response)
        {
            //console.log("sync_ERP_account() response:"+response);

            var result = JSON.parse(response);
            //console.log("sync_ERP_account() success:"+result.success+" qrcode:"+result.qrcode);

            var qrcode = document.getElementById("device_qrcode_ui");
            qrcode.setAttribute("src", result.qrcode);

            var device_qrcode_target = document.getElementById("device_qrcode_target");
            device_qrcode_target.textContent = 'target: ' + result.target;

            var device_qrcode_topic = document.getElementById("device_qrcode_topic");
            device_qrcode_topic.textContent = 'topic: ' + result.topic;

            var device_qrcode_datetime = document.getElementById("device_qrcode_datetime");
            device_qrcode_datetime.textContent = '更新時間: ' + result.datetime;

        }
    });

}

function device_atcommand_show(type, device_name, device_macaddress, version, panid, mode)
{
    console.log('device_atcommand_show() type:'+type+" device_name:"+device_name+" device_macaddress:"+device_macaddress);

    var device_name_atcmd = document.getElementById("device_name_atcmd");
    device_name_atcmd.textContent = 'name:'+device_name;
    var device_macaddress_atcmd = document.getElementById("device_macaddress_atcmd");
    device_macaddress_atcmd.textContent = device_macaddress;
    var device_version_atcmd = document.getElementById("device_version_atcmd");
    device_version_atcmd.textContent = 'version:'+version;
    var device_panid_atcmd = document.getElementById("device_panid_atcmd");
    device_panid_atcmd.textContent = 'panid:'+panid;
    var device_mode_atcmd = document.getElementById("device_mode_atcmd");
    device_mode_atcmd.textContent = 'mode:'+mode;
}

function dropdown_atcmd_click(cmd)
{
    var inputAtcmd = document.getElementById("inputAtcmd");
    inputAtcmd.value = cmd;
}

$("#sendAtcmdbtn").click(function()
{
    var send_Atcmd = document.getElementById("inputAtcmd").value;

    if ( send_Atcmd.length == 0 )
    {
        alert("請輸入指令!");
        return;
    }
    if ( send_Atcmd.indexOf('AT+') < 0 )
    {
        alert("請輸入有效指令!");
        return;
    }

    document.getElementById("sendAtcmd_sending_callout").style.display = 'block';
    document.getElementById("sendAtcmd_failed_callout").style.display = 'none';
    document.getElementById("sendAtcmd_finished_callout").style.display = 'none';
    document.getElementById("sendAtcmd_reponse_msg_callout").style.display = 'none';
    document.getElementById("sendAtcmd_reponse_msg_callout").innerHTML = "<p></p>"

    var device_macaddress_atcmd = document.getElementById("device_macaddress_atcmd");
    device_macaddress = device_macaddress_atcmd.textContent;

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['sendatcmd'] = '1';
    jsondata['atcmd'] = send_Atcmd;
    jsondata['macaddress'] = device_macaddress;

    document.getElementById("sendAtcmdbtn").disabled = true;
    var targetURL = gl_target_server + "/php/configvils.php";

    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      data: jsondata,
      success:
        function(response)
        {
            console.log("response:" + response);

            var status = JSON.parse(response);
            console.log("status:" + status);

            glSendATcmdTimestamp = getCurrentTimeStamp();
            setTimeout(wait_atcmd_finish, 1, status.waitkey);

        },
      error:
        function(xhr, status, error)
        {
            console.log("error = " + error);
            console.log("xhr.responseText = " + xhr.responseText);

            document.getElementById("sendAtcmd_sending_callout").style.display = 'none';
            document.getElementById("sendAtcmd_failed_callout").style.display = 'block';
            document.getElementById("sendAtcmd_finished_callout").style.display = 'none';

            setTimeout(close_callout, 5000, "sendAtcmd_failed_callout");
            document.getElementById("sendAtcmdbtn").disabled = false;
        }
    });
    return false;

});

function close_callout(ui_id)
{
    document.getElementById(ui_id).style.display = 'none';
}

function wait_atcmd_finish(waitkey)
{
    var currentTime = getCurrentTimeStamp();
    var diffTime = currentTime - glSendATcmdTimestamp;

    // console.log("wait_atcmd_finish() diffTime = " + diffTime);

    if (diffTime >= 30)
    {
        document.getElementById("sendAtcmd_reponse_msg_callout").style.display = 'none';
        document.getElementById("sendAtcmd_reponse_msg_callout").innerHTML = "<p></p>"
        document.getElementById("sendAtcmd_failed_msg_callout").innerHTML = "<p>發出推播 失敗! 等待時間過久!</p>"

        document.getElementById("sendAtcmd_sending_callout").style.display = 'none';
        document.getElementById("sendAtcmd_failed_callout").style.display = 'block';
        document.getElementById("sendAtcmd_finished_callout").style.display = 'none';
        setTimeout(close_callout, 10000, "sendAtcmd_failed_callout");
        document.getElementById("sendAtcmdbtn").disabled = false;

        return;
    }

    var targetURL = gl_target_server + "/php/configvils.php";
    $.getJSON(targetURL, 'waitatcmd=1'+'&project_id='+project_id+'&waitkey='+waitkey, function(data)
    {

        if ( data.success == undefined )
        {
            setTimeout(wait_atcmd_finish, 1000, waitkey);
        }
        else
        if ( data.success === 'true')
        {
            document.getElementById("sendAtcmd_reponse_msg_callout").style.display = 'block';
            document.getElementById("sendAtcmd_reponse_msg_callout").innerHTML = "<p>"+data.msg+"</p>"

            document.getElementById("sendAtcmd_sending_callout").style.display = 'none';
            document.getElementById("sendAtcmd_failed_callout").style.display = 'none';
            document.getElementById("sendAtcmd_finished_callout").style.display = 'block';
            setTimeout(close_callout, 5000, "sendAtcmd_finished_callout");
            document.getElementById("sendAtcmdbtn").disabled = false;
        }
        else
        if ( data.success === 'false')
        {
            document.getElementById("sendAtcmd_reponse_msg_callout").style.display = 'none';
            document.getElementById("sendAtcmd_reponse_msg_callout").innerHTML = "<p></p>"
            document.getElementById("sendAtcmd_failed_msg_callout").innerHTML = "<p>發出推播 失敗! "+data.msg+"</p>"

            document.getElementById("sendAtcmd_sending_callout").style.display = 'none';
            document.getElementById("sendAtcmd_failed_callout").style.display = 'block';
            document.getElementById("sendAtcmd_finished_callout").style.display = 'none';
            setTimeout(close_callout, 10000, "sendAtcmd_failed_callout");
            document.getElementById("sendAtcmdbtn").disabled = false;
        }
        else
        if ( data.success === 'wait')
        {
            document.getElementById("sendAtcmd_reponse_msg_callout").style.display = 'block';
            document.getElementById("sendAtcmd_reponse_msg_callout").innerHTML = "<p>"+data.msg+"</p>"

            setTimeout(wait_atcmd_finish, 1000, waitkey);
        }

    })
    .success(function() {
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function checkbox_locator_SensorAlert_click(macaddress)
{
    var ui_id = "LOC_"+macaddress+"_SensorAlert";
    console.log("checkbox_locator_SensorAlert_click() macaddress:"+macaddress+" checked:"+document.getElementById(ui_id).checked);

    update_locator_SensorAlert(macaddress, document.getElementById(ui_id).checked);

}

function verify_new_device(type, verifykey_uiid, verify_error_uiid, verify_pass_uiid )
{
    var verifykey = document.getElementById(verifykey_uiid).value;

    if ( verifykey.length != 8 )
    {
        alert("請輸入正確機器驗證碼");
        return;
    }

    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['verify_new_device'] = 1;
    jsondata['type'] = type;
    jsondata['verifykey'] = verifykey;

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
            else
            {
                var result = JSON.parse(response);
                // console.log('verify_error:' + result.verify_error);
                // console.log('verify_pass:' + result.verify_pass);

                var ui_verify_error = document.getElementById(verify_error_uiid);
                var ui_verify_pass = document.getElementById(verify_pass_uiid);
                if (result.verify_error.length > 0)
                {
                    ui_verify_error.textContent = result.verify_error;

                    ui_verify_pass.setAttribute("style","display:none;");
                    ui_verify_error.setAttribute("style","display:block;");
                }
                else
                if (result.verify_pass.length > 0)
                {
                    ui_verify_pass.textContent = result.verify_pass;

                    ui_verify_pass.setAttribute("style","display:block;");
                    ui_verify_error.setAttribute("style","display:none;");
                }
            }
        },
      error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);
        }
    });
}



function isANumber(str)
{
    return !/\D/.test(str);
}

function getCurrentTimeStamp()
{
    return Math.floor(Date.now() / 1000);
}

function device_tab_onclick(tabname)
{
    console.log("device_tab_onclick() tabname:"+tabname);

    gl_activetab = 'activetab_' + tabname;

}

function open_gmap(Latitude, Longitude)
{
    var targeturi = "http://www.google.com/maps/place/"+Latitude+','+Longitude;
    var win = window.open(targeturi, '_blank');
    win.focus();
}

function open_dvices_page()
{
    var win = window.open('/index.php/vilsdevices/'+project_id, '_blank');
    win.focus();
}
