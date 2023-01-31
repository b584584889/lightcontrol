/*
 * Author: ikki Chung
 * Date: 19 Jan 2017
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

var Node_Updateing = false;
var glLiveCount = 100;
var glAnchor_List = [];
var glTag_List = [];
var glCoord_List = [];
var glLocator_List = [];
var glSensor_List = [];
var glUser_List = [];
var glUserActivity_List = [];
var glUserBodyRecords = {};
var glActivityCharts = {};

var gl_anchor_num = 0;
var gl_tag_num = 0;
var gl_coord_num = 0;
var gl_locator_num = 0;
var gl_sensor_num = 0;

var gl_anchor_load_index = 0;
var gl_tag_load_index = 0;
var gl_coord_load_index = 0;
var gl_locator_load_index = 0;
var gl_sensor_load_index = 0;
var gl_user_load_index = 0;

var gl_tag_alert_count = 0;
var gl_locator_alert_count = 0;

var gl_bodyinfo_load_index = 0;

//
// globa value
//
var gl_rollcall_all_tag = [];
var gl_rollcall_area_tag = [];
var gl_rollcall_area_user = [];
var gl_rollcall_none_tag = [];
var gl_rollcall_tag_load_index = 0;
var gl_rollcall_areaid = "";
var gl_rollcall_areaname = "";
var gl_rollcall_tag_num = 0;
var LOAD_DEVICE_NUM = 20;

//
// start run
//
//var glLiveMap = new Indoor2DMap();
var glCurrentMapIndex = 0;
var glLiveMapList = [];
var glEditMode = false;
var glRefreshAll = false;

var glShowDefaultNode = true;
var glShowDefaultNodeId = show_nodeid;
var glShowDefaultNodeMapLayer = 0;

var gl_usermaingroups = [];


load_usermaingroup();

function load_usermaingroup()
{
    gl_usermaingroups = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadmaingroup=1&project_id='+project_id, function(data)
    {
        $.each(data, function(index, usermaingroup)
        {
            usermaingroup.id = parseInt(usermaingroup.id);
            gl_usermaingroups.push(usermaingroup);
        });
    })
    .success(function() {
        load_map_config(0);
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_map_config(index)
{
    glEditMode = false;
    glRefreshAll = true;

    glCurrentMapIndex = index;
    glLiveMapList = [];

    var targetURL = gl_target_server + "/php/map.php";
    $.getJSON(targetURL, 'loadmap=1'+'&project_id='+project_id, function(mapsdata)
    {
        var map_nav_tabs = document.getElementById('map_nav_tabs');
        while (map_nav_tabs.firstChild)
        {
            map_nav_tabs.removeChild(map_nav_tabs.firstChild);
        }
        var map_tab_content = document.getElementById('map_tab_content');
        while (map_tab_content.firstChild)
        {
            map_tab_content.removeChild(map_tab_content.firstChild);
        }

        //console.log("load_map_config() glCurrentMapIndex:"+glCurrentMapIndex + " mapsdata.length:"+mapsdata.length);

        var count = 0;
        $.each(mapsdata, function(curindex, mapdata)
        {
            // if ( index > 0 )
            //     return;

            add_map(curindex, mapdata);
            count++;
        });

        if (GL_CONFIG_EDIT_2DMAP == 1)
        {
            add_create_new_layer_btn(count);
        }

        runLive_all();

    });
    return false;
}

function add_map(ui_index, mapdata)
{
    var new_tab = create_map_tab(ui_index, mapdata);

    var map_nav_tabs = document.getElementById('map_nav_tabs');
    map_nav_tabs.appendChild(new_tab);

    var new_content = create_map_content(ui_index, mapdata);

    var map_tab_content = document.getElementById('map_tab_content');
    map_tab_content.appendChild(new_content);

    var mapid = "mapid_"+ui_index;
    var liveMap = new Indoor2DMap(mapid);
    glLiveMapList.push(liveMap);

    liveMap.setMapCoeff(mapdata.POSX, mapdata.POSY, mapdata.MAPWIDTH, mapdata.MAPHEIGHT, mapdata.SCALE, mapdata.COEFF, mapdata.WGS48OriginX, mapdata.WGS48OriginY, mapdata.MAP_North);
    liveMap.setMapImage(mapdata.FILE, "Indoor 2D Map " + ui_index);
    liveMap.setMapLayerInfo(mapdata.layerid);
    liveMap.addOriginMarker(0, 0, 0);

    if (GL_CONFIG_2DMAP_SHOW_GMAP == 1)
       liveMap.showGoolgeMap();

    if (GL_CONFIG_EDIT_2DMAP == 1)
       liveMap.createMapEditor(editmode_click);

    liveMap.load_maparea();
    liveMap.setNodeDragCallback(update_map_node_info);
    liveMap.setOrigNodeDragCallback(update_orig_node_info);
    liveMap.setUpdateMapInfoCallback(update_map_info);
    liveMap.setNodeClickCallback(node_click);

}

function create_map_tab(ui_index, mapdata)
{
    var new_tab = document.createElement('li');
    new_tab.setAttribute("id","map_tab_li_"+ui_index);
    if (ui_index == glCurrentMapIndex)
    {
        new_tab.setAttribute("class","active");
    }

    var ahref = document.createElement('a');
    ahref.setAttribute("href","#map_tab_"+ui_index);
    ahref.setAttribute("data-toggle","tab");
    ahref.setAttribute("id","map_tab_href_"+ui_index);

    ahref.setAttribute("onclick","map_tab_onclick("+ ui_index +", \""+ mapdata.layerid +"\", \""+ mapdata.layer_name +"\");return false;");
    var span = document.createElement("span");
    span.setAttribute("class","badge bg-blue-active");
    span.setAttribute("id","layer_name_"+ui_index);
    span.textContent = mapdata.layer_name;
    ahref.appendChild(span);
    new_tab.appendChild(ahref);

    return new_tab;
}

function create_map_content(ui_index, mapdata)
{
    var divTabPane = document.createElement('div');
    if (ui_index == glCurrentMapIndex)
        divTabPane.setAttribute("class","tab-pane active");
    else
        divTabPane.setAttribute("class","tab-pane");
    divTabPane.setAttribute("id","map_tab_"+ui_index);
    divTabPane.setAttribute("style","height:100%;");

    var maingroupinfo = getMainGroupInfo(mapdata.maingroupid);

    //
    // layer info
    //
    var div_layer_info = document.createElement('div');
    divTabPane.appendChild(div_layer_info);

    if (maingroupinfo != undefined)
    {
        var div_maingroup = document.createElement('strong');
        div_maingroup.innerHTML = '主集合:' +maingroupinfo.name+'[ ID:'+maingroupinfo.id+' ]' + '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
        div_maingroup.setAttribute("style","font-weight:bold;");
        div_layer_info.appendChild(div_maingroup);
    }

    var div_desc = document.createElement('strong');
    div_desc.innerHTML = mapdata.layer_description + '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    div_desc.setAttribute("style","font-weight:bold;");
    div_desc.setAttribute("id","layer_description_"+ui_index);
    div_desc.setAttribute("onclick","layer_description_onclick("+ ui_index +", \""+ mapdata.layerid +"\", \""+ mapdata.layer_description +"\");return false;");
    div_layer_info.appendChild(div_desc);

    var div_GPS_icon = document.createElement('i');
    div_GPS_icon.setAttribute("class","fa fa-map-marker margin-r-5");
    div_GPS_icon.setAttribute("style","color:#1a1;");
    div_layer_info.appendChild(div_GPS_icon);

    var div_GPS_X = document.createElement('strong');
    div_GPS_X.innerHTML = mapdata.WGS48OriginX +'&nbsp;&nbsp;&nbsp';
    div_GPS_X.setAttribute("id","WGS48OriginX_"+ui_index);
    div_GPS_X.setAttribute("onclick","WGS48OriginX_onclick("+ ui_index +", \""+ mapdata.layerid +"\", \""+ mapdata.WGS48OriginX +"\");return false;");
    div_GPS_X.setAttribute("style","color:#1a1;");
    div_layer_info.appendChild(div_GPS_X);

    var div_GPS_Y = document.createElement('strong');
    div_GPS_Y.innerHTML = mapdata.WGS48OriginY +'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    div_GPS_Y.setAttribute("id","WGS48OriginY_"+ui_index);
    div_GPS_Y.setAttribute("onclick","WGS48OriginY_onclick("+ ui_index +", \""+ mapdata.layerid +"\", \""+ mapdata.WGS48OriginY +"\");return false;");
    div_GPS_Y.setAttribute("style","color:#1a1;");
    div_layer_info.appendChild(div_GPS_Y);

    var div_North_icon = document.createElement('i');
    div_North_icon.setAttribute("class","fa fa-compass margin-r-5");
    div_North_icon.setAttribute("style","color:#811;");
    div_layer_info.appendChild(div_North_icon);

    var div_North = document.createElement('strong');
    div_North.innerHTML = mapdata.MAP_North +'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    div_North.setAttribute("id","MAP_North_"+ui_index);
    div_North.setAttribute("onclick","MAP_North_onclick("+ ui_index +", \""+ mapdata.layerid +"\", \""+ mapdata.MAP_North +"\");return false;");
    div_North.setAttribute("style","color:#822;");
    div_layer_info.appendChild(div_North);

    var div_building = document.createElement('strong');
    div_building.innerHTML = 'Building:&nbsp;' + mapdata.BuildingID +'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    div_building.setAttribute("id","BuildingID_"+ui_index);
    div_building.setAttribute("onclick","BuildingID_onclick("+ ui_index +", \""+ mapdata.layerid +"\", \""+ mapdata.BuildingID +"\");return false;");
    div_building.setAttribute("style","color:#328;");
    div_layer_info.appendChild(div_building);

    var div_floor = document.createElement('strong');
    div_floor.innerHTML = 'Floor:&nbsp;' + mapdata.BuildingFloor;
    div_floor.setAttribute("id","BuildingFloor_"+ui_index);
    div_floor.setAttribute("onclick","BuildingFloor_onclick("+ ui_index +", \""+ mapdata.layerid +"\", \""+ mapdata.BuildingFloor +"\");return false;");
    div_floor.setAttribute("style","color:#328;");
    div_layer_info.appendChild(div_floor);

    // delete button
    var deleteBtn = document.createElement("button");
    deleteBtn.setAttribute("id","DeleteMapLayer_Btn_"+ui_index);
    deleteBtn.setAttribute("style","display:none;");
    deleteBtn.setAttribute("title","Delete this Group");
    deleteBtn.setAttribute("class","btn btn-danger pull-right");
    deleteBtn.setAttribute("href","javascript:;");
    deleteBtn.setAttribute("onclick","delete_maplayer_onclick("+ ui_index +",\""+mapdata.layerid+"\", \""+ mapdata.layer_name +"\");return false;");
    var imgDelete = document.createElement("i");
    imgDelete.setAttribute("class","fa fa-trash-o");
    deleteBtn.appendChild(imgDelete);
    divTabPane.appendChild(deleteBtn);

    //
    // edit mode function
    //
    var form_layer_edit = document.createElement('form');
    form_layer_edit.setAttribute("id","formContent_"+ui_index);
    form_layer_edit.setAttribute("method","POST");
    form_layer_edit.setAttribute("enctype","multipart/form-data");
    form_layer_edit.setAttribute("onsubmit","return submit_newmap(this, "+ui_index+",\""+mapdata.layerid+"\");");
    divTabPane.appendChild(form_layer_edit);

    // change map picture
    var div_formgroup = document.createElement("div");
    div_formgroup.setAttribute("id","ChangeMap_"+ui_index);
    div_formgroup.setAttribute("class","form-group");
    div_formgroup.setAttribute("style","display:none;");
    form_layer_edit.appendChild(div_formgroup);

    var div_formgroup_label = document.createElement("label");
    div_formgroup_label.setAttribute("class","control-sidebar-subheading");
    div_formgroup_label.innerHTML = '更換地圖';
    div_formgroup.appendChild(div_formgroup_label);

    var div_formgroup_input = document.createElement("input");
    div_formgroup_input.setAttribute("id","imageUpload_"+ui_index);
    div_formgroup_input.setAttribute("type","file");
    div_formgroup_input.setAttribute("name","imageUpload");
    div_formgroup_label.appendChild(div_formgroup_input);
    // icon
    var div_formgroup_img = document.createElement("img");
    div_formgroup_img.setAttribute("id","uploadIm_"+ui_index);
    div_formgroup_img.setAttribute("src","/images/load.gif");
    div_formgroup_img.setAttribute("style","display: none;");
    div_formgroup_label.appendChild(div_formgroup_img);

    var div_formgroup_p = document.createElement("p");
    div_formgroup_p.setAttribute("id","upload_msg_"+ui_index);
    div_formgroup_p.setAttribute("class","help-block");
    div_formgroup_label.appendChild(div_formgroup_p);

    var div_formgroup_button = document.createElement("button");
    div_formgroup_button.setAttribute("class","btn btn-primary");
    div_formgroup_label.appendChild(div_formgroup_button);

    var div_formgroup_i = document.createElement("i");
    div_formgroup_i.setAttribute("class","fa fa-cloud-upload");
    div_formgroup_button.appendChild(div_formgroup_i);

    //
    // map layer
    //
    var pad = document.createElement('pad');
    pad.setAttribute("class","pad");
    divTabPane.appendChild(pad);
    var div_map = document.createElement('div');
    div_map.setAttribute("id","mapid_"+ui_index);
    // div_map.setAttribute("style","height:85vh");
    div_map.setAttribute("style","height:95%;");
    pad.appendChild(div_map);

    return divTabPane;
}

function add_create_new_layer_btn(ui_index)
{
    var new_tab = document.createElement('li');
    new_tab.setAttribute("id","map_tab_li_"+ui_index);

    var ahref = document.createElement('a');
    ahref.setAttribute("onclick","create_tab_onclick("+ ui_index +");return false;");

    var span = document.createElement("span");
    span.setAttribute("class","badge bg-red");
    span.textContent = '新增地圖+';
    ahref.appendChild(span);
    new_tab.appendChild(ahref);

    var map_nav_tabs = document.getElementById('map_nav_tabs');
    map_nav_tabs.appendChild(new_tab);
}

function runLive_all()
{
    if ( glLiveMapList[glCurrentMapIndex] == null )
        return;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_anchor_num = parseInt(data['LOC_ANCHOR_NUM']);
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_coord_num = parseInt(data['LOC_COORD_NUM']);
        gl_locator_num = parseInt(data['LOC_LOCATOR_NUM']);
        gl_sensor_num = parseInt(data['LOC_SENSOR_NUM']);

        gl_anchor_load_index = 0;
        gl_tag_load_index = 0;
        gl_coord_load_index = 0;
        gl_locator_load_index = 0;
        gl_sensor_load_index = 0;

        glRefreshAll = false;
    })
    .success(function() {
        setTimeout(runLive_anchor, 200);
        setTimeout(runLive_tag, 300);
        setTimeout(runLive_coord, 400);
        setTimeout(runLive_locator, 500);
        setTimeout(runLive_sensor, 100);
        setTimeout(runLive_user, 200);
        // setTimeout(runLive_userActiviry, 1000);
        // setTimeout(runLive_bodyinfo_records, 5000);
    })
    .error(function() {
        setTimeout(runLive_all, 5000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}

function runLive_anchor()
{
    if (glRefreshAll)
    {
        console.log("runLive_anchor() glRefreshAll");
        return;
    }

    if (Node_Updateing || glLiveMapList[glCurrentMapIndex].editmode())
    {
        setTimeout(runLive_anchor, 3000);
        return;
    }

    if (gl_anchor_load_index >= gl_anchor_num)
    {
        gl_anchor_load_index = 0;
        setTimeout(runLive_anchor, 30000);
        return;
    }
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

            // split data
            var nodeData = data[key].split(",");
            var macaddress = nodeData[6];
            var nodeid = nodeData[3];
            var nodename = nodeData[5];
            var posX = nodeData[7];
            var posY = nodeData[8];
            var posZ = nodeData[9];
            var buildingid = nodeData[13];
            var floor = nodeData[14];
            var maplayer = nodeData[20];


            if (maplayer == '0')
                maplayer = getFirstMapLayer();

            var date = new moment();

            if (glRefreshAll)
            {
                console.log("runLive_anchor() glRefreshAll");
                return;
            }

            for(var index=0;index<glLiveMapList.length;index++)
            {
                //macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date
                glLiveMapList[index].updateAnchorMarker(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer);
            }

            glAnchor_List.push(nodeData);

            // if ( parent.$('#rightcontentIframe')[0] != undefined )
            // {
            //     if ( typeof parent.$('#rightcontentIframe')[0].contentWindow.show_device_info === "function")
            //     {
            //         if (parent.$('#rightcontentIframe')[0].contentWindow.CUR_Show_Decice_ID == nodeid)
            //             parent.$('#rightcontentIframe')[0].contentWindow.show_device_info("Anchor", nodeData);
            //     }
            // }
        }

        gl_anchor_load_index += anchor_num;

    })
    .success(function() {
        if (glRefreshAll)
        {
            console.log("runLive_anchor() glRefreshAll");
            return;
        }

        if (gl_anchor_load_index >= gl_anchor_num)
        {
            gl_anchor_load_index = 0;
            setTimeout(runLive_anchor, 30000);
        }
        else
        {
            runLive_anchor();
        }
    })
    .error(function() {
        gl_anchor_load_index = 0;
        setTimeout(runLive_anchor, 30000);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function runLive_tag()
{
    if (glRefreshAll)
    {
        console.log("runLive_tag() glRefreshAll");
        return;
    }

    if (Node_Updateing || glLiveMapList[glCurrentMapIndex].editmode())
    {
        //console.log("runLive_tag() Node_Updateing:"+Node_Updateing+", glLiveMapList[glCurrentMapIndex].editmode():"+glLiveMapList[glCurrentMapIndex].editmode());
        setTimeout(runLive_tag, 1000);
        return;
    }

    if (gl_tag_load_index >= gl_tag_num)
    {
        gl_tag_load_index = 0;
        gl_tag_alert_count = 0;
        setTimeout(runLive_tag, 300);

        return;
    }
    if (gl_tag_load_index == 0)
    {
        glTag_List = [];
    }

    var checkedValueShowTrack = document.getElementById('showtrack').checked;
    glLiveMapList[glCurrentMapIndex].setShowTrack(checkedValueShowTrack);

    var checkedValueUISmoothing = document.getElementById('uismoothingcheckbtn').checked;
    glLiveMapList[glCurrentMapIndex].setUISmoothing(checkedValueUISmoothing);

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadtag=1&s='+gl_tag_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var tag_num = parseInt(data['LOC_TAG_NUM']);

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);
        var tagAlertMapLayerIndex = -1;
        var updateTagList= false;
        for (var i = gl_tag_load_index; i < gl_tag_load_index+tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            // split data
            var nodeData = data[key].split(",");
            var alive = nodeData[1];
            var macaddress = nodeData[6];
            var nodeid = nodeData[3];
            var nodename = nodeData[5];
            var posX = nodeData[7];
            var posY = nodeData[8];
            var posZ = nodeData[9];
            var buildingid = nodeData[13];
            var floor = nodeData[14];
            var areaid = nodeData[19];
            var areaname = nodeData[20];
            var alert = parseInt(nodeData[22]);
            var areatype = nodeData[23];
            var Longitude = parseFloat(nodeData[24]);
            var Latitude = parseFloat(nodeData[25]);
            var exclude_rollcall_AreaId = nodeData[26];
            var Opertaion_Mode = nodeData[27]; // :None 1:UWB Ranging 2:LoRa Ranging 3:GPS
            var area_unknown = nodeData[28];
            var voltage_low = parseInt(nodeData[32]);
            var asleep = parseInt(nodeData[35]);
            var maplayer = nodeData[36];

            if (maplayer == '0')
                maplayer = getFirstMapLayer();

            if (glShowDefaultNodeId != undefined)
            {
                if (glShowDefaultNodeId === nodeid)
                {
                    glShowDefaultNodeMapLayer = maplayer;
                }
            }

            if (glRefreshAll)
            {
                console.log("runLive_tag() glRefreshAll");
                return;
            }

            var date = new moment();
            var showGPSPos = false;
            //if (Longitude != 0.0 && Latitude != 0.0)
            if (Opertaion_Mode == '3')
            {
                showGPSPos = true;

                for(var index=0;index<glLiveMapList.length;index++)
                {
                    glLiveMapList[index].setTagLngLat(nodeid, Longitude, Latitude, buildingid, floor);
                }
            }

            var alertType = alert;

            if ( alertType != 0 )
            {
                gl_tag_alert_count += 1;
            }

            for(var index=0;index<glLiveMapList.length;index++)
            {
                //macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date
                var found = glLiveMapList[index].updateTagMarker(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, showGPSPos, maplayer);
                if (!found)
                {
                    updateTagList = true;
                }

                if ((glLiveMapList[index].mMapLayerId == maplayer) || ( (index == 0) && (maplayer == '0')))
                {
                    if ( (tagAlertMapLayerIndex < 0) && (alert != 0))
                    {
                        // check user stop alert
                        var alertStatus = glLiveMapList[index].NodeAlertFunction[nodeid];
                        if (alertStatus == 1)
                        {
                            tagAlertMapLayerIndex = index;
                        }
                    }

                    glLiveMapList[index].setTagAlert(nodeid, alertType);
                    glLiveMapList[index].setTagLocation(nodeid, alertType, areaid, areaname, areatype, alive, exclude_rollcall_AreaId, area_unknown, showGPSPos, voltage_low, asleep);
                }
            }

            glTag_List.push(nodeData);
        }

        if (updateTagList)
        {
            create_dropdown_taglist();
        }

        gl_tag_load_index += tag_num;

        if (tagAlertMapLayerIndex >= 0)
        {
            // switch alert maplayer
            $('#map_tab_href_'+tagAlertMapLayerIndex).trigger('click');
        }

    })
    .success(function() {

        if (glRefreshAll)
        {
            console.log("runLive_tag() glRefreshAll");
            return;
        }

        if (gl_tag_load_index >= gl_tag_num)
        {
            if ( gl_tag_alert_count == 0 )
            {
                glLiveMapList[glCurrentMapIndex].check_tag_layer_alertcircle();
            }

            gl_tag_load_index = 0;
            gl_tag_alert_count = 0;
            setTimeout(runLive_tag, 300);

            if (glShowDefaultNode)
            {
                if (glShowDefaultNodeId.length > 0)
                {
                    var found = dropdown_tag_click(glShowDefaultNodeId, glShowDefaultNodeMapLayer);
                    if (found == true)
                    {
                        console.log("runLive_tag() glShowDefaultNodeMapLayer:"+glShowDefaultNodeMapLayer + " glShowDefaultNodeId:"+glShowDefaultNodeId);

                        glShowDefaultNode = false;
                        glShowDefaultNodeId = '';
                    }
                }

            }
        }
        else
        {
            runLive_tag();
        }
    })
    .error(function() {
        gl_tag_load_index = 0;
        gl_tag_alert_count = 0;
        setTimeout(runLive_tag, 300);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function runLive_coord()
{
    if (glRefreshAll)
    {
        console.log("runLive_coord() glRefreshAll");
        return;
    }

    if (Node_Updateing || glLiveMapList[glCurrentMapIndex].editmode())
    {
        setTimeout(runLive_coord, 3000);
        return;
    }

    if (gl_coord_load_index >= gl_coord_num)
    {
        gl_coord_load_index = 0;
        setTimeout(runLive_coord, 30000);
        return;
    }
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

            // split data
            var nodeData = data[key].split(",");
            var macaddress = nodeData[6];
            var nodeid = nodeData[3];
            var nodename = nodeData[5];
            var posX = nodeData[7];
            var posY = nodeData[8];
            var posZ = nodeData[9];
            var buildingid = nodeData[13];
            var floor = nodeData[14];
            var maplayer = nodeData[26];

            if (maplayer == '0')
                maplayer = getFirstMapLayer();

            var date = new moment();
            if (glRefreshAll)
            {
                console.log("runLive_coord() glRefreshAll");
                return;
            }

            for(var index=0;index<glLiveMapList.length;index++)
            {
                //macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date
                glLiveMapList[index].updateCoordinatorMarker(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer);
            }

            glCoord_List.push(nodeData);

            // if ( parent.$('#rightcontentIframe')[0] != undefined )
            // {
            //     if ( typeof parent.$('#rightcontentIframe')[0].contentWindow.show_device_info === "function")
            //     {
            //         if (parent.$('#rightcontentIframe')[0].contentWindow.CUR_Show_Decice_ID == nodeid)
            //             parent.$('#rightcontentIframe')[0].contentWindow.show_device_info("Coordinator", nodeData);
            //     }
            // }
        }

        gl_coord_load_index += coord_num;

    })
    .success(function() {

        if (glRefreshAll)
        {
            console.log("runLive_coord() glRefreshAll");
            return;
        }

        if (gl_coord_load_index >= gl_coord_num)
        {
            gl_coord_load_index = 0;
            setTimeout(runLive_coord, 10000);
        }
        else
        {
            runLive_coord();
        }
    })
    .error(function() {
        gl_coord_load_index = 0;
        setTimeout(runLive_coord, 300);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function runLive_locator()
{
    if (glRefreshAll)
    {
        console.log("runLive_locator() glRefreshAll");
        return;
    }

    if (Node_Updateing || glLiveMapList[glCurrentMapIndex].editmode())
    {
        setTimeout(runLive_locator, 3000);
        return;
    }

    if (gl_locator_load_index >= gl_locator_num)
    {
        gl_locator_load_index = 0;
        gl_locator_alert_count = 0;
        setTimeout(runLive_locator, 300);
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
        var locatorAlertMapLayerIndex = -1;
        var updateLocatorList = false;
        for (var i = gl_locator_load_index; i < gl_locator_load_index+locator_num; ++i)
        {
            var key = 'LOC_LOCATOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            // split data
            var nodeData = data[key].split(",");
            var alive = parseInt(nodeData[1]);
            var macaddress = nodeData[6];
            var nodeid = nodeData[3];
            var nodename = nodeData[5];
            var posX = nodeData[7];
            var posY = nodeData[8];
            var posZ = nodeData[9];
            var buildingid = nodeData[13];
            var floor = nodeData[14];
            var alert = parseInt(nodeData[20]);
            var loramacaddress = nodeData[22];
            var maplayer = nodeData[26];
            var poweronTimestamp = nodeData[27];
            var poweroffTimestamp = nodeData[28];
            var aliveTime = nodeData[29];

            // var poweron_date = moment(poweronTimestamp, "YYYY-MM-DD HH:mm:ss");
            var poweroff_date = moment(poweroffTimestamp, "YYYY-MM-DD HH:mm:ss");
            var alive_date = moment(aliveTime, "YYYY-MM-DD HH:mm:ss");

            // console.log("aliveTime:"+aliveTime +", poweronTimestamp:"+poweronTimestamp +", poweroffTimestamp:"+poweroffTimestamp);

            var poweroff = 0;
            if ( poweroff_date > alive_date )
            {
                // console.log("Power Off > alive ");
                poweroff = 1;
            }
            // if ( poweroff_date > poweron_date )
            // {
            //     // console.log("Power Off > poweron ");
            // }


            if (maplayer == '0')
                maplayer = getFirstMapLayer();

            if (glShowDefaultNodeId != undefined)
            {
                if (glShowDefaultNodeId === nodeid)
                {
                    glShowDefaultNodeMapLayer = maplayer;
                }

                if (glShowDefaultNodeId === macaddress)
                {
                    glShowDefaultNodeId = nodeid;
                    glShowDefaultNodeMapLayer = maplayer;
                }

                if (glShowDefaultNodeId === loramacaddress)
                {
                    glShowDefaultNodeId = nodeid;
                    glShowDefaultNodeMapLayer = maplayer;
                }
            }

            var date = new moment();

            if (glRefreshAll)
            {
                console.log("runLive_locator() glRefreshAll");
                return;
            }

            var areaViewtype = alert;

            if ( alert != 0 )
            {
                // console.log("runLive_locator() alert nodeid:"+nodeid);

                gl_locator_alert_count += 1;
            }

            for(var index=0;index<glLiveMapList.length;index++)
            {
                //macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date
                var found = glLiveMapList[index].updateLocatorMarker(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, maplayer);
                if (!found)
                {
                    updateLocatorList = true;
                }

                if ((glLiveMapList[index].mMapLayerId == maplayer) || ( (index == 0) && (maplayer == '0') ))
                {
                    if ( (locatorAlertMapLayerIndex < 0) && (alert != 0) )
                    {
                        // check user stop alert
                        var alertStatus = glLiveMapList[index].NodeAlertFunction[nodeid];
                        if (alertStatus == 1)
                        {
                            locatorAlertMapLayerIndex = index;
                        }
                    }

                    glLiveMapList[index].setLocatorAlert(nodeid, areaViewtype, alive, poweroff);
                }
            }

            glLocator_List.push(nodeData);

            // if ( parent.$('#rightcontentIframe')[0] != undefined )
            // {
            //     if ( typeof parent.$('#rightcontentIframe')[0].contentWindow.show_device_info === "function")
            //     {
            //         if (parent.$('#rightcontentIframe')[0].contentWindow.CUR_Show_Decice_ID == nodeid)
            //             parent.$('#rightcontentIframe')[0].contentWindow.show_device_info("Locator", nodeData);
            //     }
            // }
        }

        if (updateLocatorList)
        {
            create_dropdown_locatorlist();
        }

        gl_locator_load_index += locator_num;

        if (locatorAlertMapLayerIndex >= 0)
        {
            // switch alert maplayer
            // console.log("runLive_locator() #map_tab_href_"+locatorAlertMapLayerIndex);
            $('#map_tab_href_'+locatorAlertMapLayerIndex).trigger('click');
        }
    })
    .success(function() {

        if (glRefreshAll)
        {
            console.log("runLive_locator() glRefreshAll");
            return;
        }

        if (gl_locator_load_index >= gl_locator_num)
        {
            if (gl_locator_alert_count == 0)
            {
                glLiveMapList[glCurrentMapIndex].check_locator_layer_alertcircle();
            }

            gl_locator_load_index = 0;
            gl_locator_alert_count = 0;
            setTimeout(runLive_locator, 300);

            if (glShowDefaultNode)
            {
                if (glShowDefaultNodeId.length > 0)
                {
                    var found = dropdown_locator_click(glShowDefaultNodeId, glShowDefaultNodeMapLayer);
                    if (found == true)
                    {
                        console.log("runLive_locator() glShowDefaultNodeMapLayer:"+glShowDefaultNodeMapLayer + " glShowDefaultNodeId:"+glShowDefaultNodeId);

                        glShowDefaultNode = false;
                        glShowDefaultNodeId = '';
                    }
                }

            }
        }
        else
        {
            runLive_locator();
        }
    })
    .error(function() {
        gl_locator_load_index = 0;
        gl_locator_alert_count = 0;
        setTimeout(runLive_locator, 300);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);
}

function runLive_sensor()
{
    if (glRefreshAll)
    {
        console.log("runLive_sensor() glRefreshAll");
        return;
    }

    if (Node_Updateing || glLiveMapList[glCurrentMapIndex].editmode())
    {
        setTimeout(runLive_sensor, 3000);
        return;
    }

    if (gl_sensor_load_index >= gl_sensor_num)
    {
        gl_sensor_load_index = 0;
        setTimeout(runLive_sensor, 5000);
        return;
    }
    if (gl_sensor_load_index == 0)
    {
        glSensor_List = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadsensordevice=1&s='+gl_sensor_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, device)
        {
            for(var index=0;index<glLiveMapList.length;index++)
            {
                var maplayer = getFirstMapLayer();
                device.maplayer = maplayer;
                glLiveMapList[index].updateSensorMarker(device.id, device.name, device.type, device.Longitude, device.Latitude, maplayer, device.Battery, device.unknownLocation);

                if (glShowDefaultNodeId != undefined)
                {
                    if (glShowDefaultNodeId === device.id)
                    {
                        glShowDefaultNodeMapLayer = maplayer;
                    }
                }
            }

            gl_sensor_load_index += 1;
            glSensor_List.push(device);
        });
    })
    .success(function() {

        if (glRefreshAll)
        {
            console.log("runLive_sensor() glRefreshAll");
            return;
        }

        if (gl_sensor_load_index >= gl_sensor_num)
        {
            gl_sensor_load_index = 0;
            setTimeout(runLive_sensor, 5000);

            if (glShowDefaultNode)
            {
                if (glShowDefaultNodeId.length > 0)
                {
                    for (var i = 0; i < glSensor_List.length; i++)
                    {
                        var sensorData = glSensor_List[i];
                        if ( sensorData.id === glShowDefaultNodeId )
                        {
                            console.log("runLive_sensor() glShowDefaultNodeMapLayer:"+glShowDefaultNodeMapLayer + " glShowDefaultNodeId:"+glShowDefaultNodeId);

                            searchresult_sensor_click(glShowDefaultNodeId, glShowDefaultNodeMapLayer);

                            glShowDefaultNode = false;
                            glShowDefaultNodeId = '';
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            setTimeout(runLive_sensor, 50);
        }
    })
    .error(function() {
        gl_sensor_load_index = 0;
        setTimeout(runLive_sensor, 300);
    })
    .complete(function() {
    });
}

function runLive_user()
{
    if (glRefreshAll)
    {
        console.log("runLive_user() glRefreshAll");
        return;
    }

    if (Node_Updateing || glLiveMapList[glCurrentMapIndex].editmode())
    {
        setTimeout(runLive_user, 3000);
        return;
    }

    if (gl_user_load_index == 0)
    {
        glUser_List = [];
    }

    var load_user_num = 0;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduserpoistioninfo=1&s='+gl_user_load_index+'&count='+500+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, userinfo)
        {
            userinfo.userid = parseInt(userinfo.userid);

            for(var index=0;index<glLiveMapList.length;index++)
            {
                // var maplayer = getFirstMapLayer();
                // device.maplayer = maplayer;
                glLiveMapList[index].updateUserMarker(userinfo.userid, userinfo.name, userinfo.maplayer, userinfo.posX, userinfo.posY, userinfo.posZ, userinfo.Longitude, userinfo.Latitude);

                if (glShowDefaultNodeId != undefined)
                {
                    if (isANumber(glShowDefaultNodeId))
                    {
                        if (parseInt(glShowDefaultNodeId) == userinfo.userid)
                        {
                            console.log("runLive_user() glShowDefaultNodeMapLayer:"+glShowDefaultNodeMapLayer + " glShowDefaultNodeId:"+glShowDefaultNodeId);

                            glShowDefaultNodeMapLayer = userinfo.maplayer;
                        }
                    }
                }
            }

            load_user_num += 1;
            gl_user_load_index += 1;
            glUser_List.push(userinfo);
        });
    })
    .success(function() {

        if (glRefreshAll)
        {
            console.log("runLive_user() glRefreshAll");
            return;
        }

        if (load_user_num == 0)
        {
            create_dropdown_userlist();

            gl_user_load_index = 0;
            setTimeout(runLive_user, 5000);

            if (glShowDefaultNode)
            {
                // console.log("runLive_sensor() glShowDefaultNodeMapLayer:"+glShowDefaultNodeMapLayer + " glShowDefaultNodeId:"+glShowDefaultNodeId);
                if (glShowDefaultNodeId.length > 0)
                {
                    searchresult_user_click(glShowDefaultNodeId, glShowDefaultNodeMapLayer);
                }

                glShowDefaultNode = false;
                glShowDefaultNodeId = '';
            }
        }
        else
        {
            setTimeout(runLive_user, 50);
        }
    })
    .error(function() {
        gl_user_load_index = 0;
        setTimeout(runLive_user, 300);
    })
    .complete(function() {
    });
}

// function runLive_userActiviry()
// {
//     if (glRefreshAll)
//     {
//         glUserActivity_List = [];
//         console.log("runLive_userActiviry() glRefreshAll");
//         return;
//     }
//
//     if (Node_Updateing || glLiveMapList[glCurrentMapIndex].editmode())
//     {
//         setTimeout(runLive_userActiviry, 3000);
//         return;
//     }
//
//     if (glUserActivity_List.length == 0)
//     {
//         collect_user_sensor();
//     }
//
//     if (glUserActivity_List.length == 0)
//     {
//         setTimeout(runLive_userActiviry, 5000);
//         return;
//     }
//
//     // update_user_area();
//
//     setTimeout(runLive_userActiviry, 1000);
// }

// function update_user_area()
// {
//     var UserActivityDashBoard = document.getElementById("UserActivityDashBoard");
//     var divRow = undefined;
//
//     var showCount = 0;
//     for	(var index = 0; index < glUserActivity_List.length; index++)
//     {
//         var userActivity = glUserActivity_List[index];
//         for	(var j = 0; j < userActivity.uwbcards.length; j++)
//         {
//             var uwbcard = userActivity.uwbcards[j];
//             for (var i = 0; i < glTag_List.length; ++i)
//             {
//                 var nodeData = glTag_List[i];
//                 var macaddress = nodeData[6];
//                 var areaId = nodeData[19];
//                 var areaName = nodeData[20];
//                 var activity = nodeData[40];
//                 if( uwbcard == macaddress)
//                 {
//                     // console.log("update_user_area() username:" + userActivity.username +" macaddress:" + macaddress +" areaName:" + areaName);
//                     userActivity.areaId = areaId;
//                     userActivity.areaName = areaName;
//                     userActivity.activity = activity;
//                     break;
//                 }
//             }
//         }
//         if ( showCount == 0 )
//         {
//             var rowId = 'ActivityDashBoardRow_'+index;
//             divRow = document.getElementById(rowId);
//             if (divRow == undefined)
//             {
//                 divRow = document.createElement("div");
//                 divRow.setAttribute("id", rowId);
//                 divRow.setAttribute("class","row");
//                 divRow.setAttribute("style","padding:10px;");
//                 UserActivityDashBoard.appendChild(divRow);
//             }
//         }
//
//         //var colId = 'ActivityDashBoardCol_'+index+'_'+showCount;
//         var colId = 'ActivityDashBoardCol_'+userActivity.userid;
//         updateUserActivity(divRow, colId, userActivity);
//
//         showCount++;
//         if (showCount >= 2)
//         {
//             showCount = 0;
//         }
//     }
// }

// function updateUserActivity(divRow, colId, userActivity)
// {
//     var divCol = document.getElementById(colId);
//     if (divCol == undefined)
//     {
//         divCol = document.createElement("div");
//         divCol.setAttribute("id", colId);
//         divCol.setAttribute("class","col-lg-6 col-xs-6");
//         divRow.appendChild(divCol);
//     }
//
//     var boxId = colId+'_box';
//     var box = document.getElementById(boxId);
//     if (box == undefined)
//     {
//         box = document.createElement("div");
//         box.setAttribute("id", boxId);
//         divCol.appendChild(box);
//     }
//     //box.setAttribute("class","small-box bg-red");
//     box.setAttribute("class","small-box bg-green");
//     //box.setAttribute("class","small-box bg-yellow");
//
//     // inner
//     var innerId = colId+'_inner';
//     var inner = document.getElementById(innerId);
//     if (inner == undefined)
//     {
//         inner = document.createElement("div");
//         inner.setAttribute("id", innerId);
//         inner.setAttribute("class","inner");
//         box.appendChild(inner);
//     }
//
//     var usernameId = colId+'_username';
//     var username = document.getElementById(usernameId);
//     if (username == undefined)
//     {
//         username = document.createElement("h4");
//         username.setAttribute("id", usernameId);
//         inner.appendChild(username);
//     }
//     username.textContent = userActivity.username;
//
//     var bodyinfoId = colId+'_bodyinfo';
//     var bodyinfo = document.getElementById(bodyinfoId);
//     if (bodyinfo == undefined)
//     {
//         bodyinfo = document.createElement("p");
//         bodyinfo.setAttribute("id", bodyinfoId);
//         inner.appendChild(bodyinfo);
//         bodyinfo.textContent = '心率: 血氧:';
//     }
//
//     var locationId = colId+'_location';
//     var location = document.getElementById(locationId);
//     if (location == undefined)
//     {
//         location = document.createElement("p");
//         location.setAttribute("id", locationId);
//         inner.appendChild(location);
//     }
//     location.textContent = '位置:' + userActivity.areaName;
//
//     // icon
//     var iconId = colId+'_icon';
//     var icon = document.getElementById(iconId);
//     if (icon == undefined)
//     {
//         icon = document.createElement("div");
//         icon.setAttribute("id", iconId);
//         icon.setAttribute("class","icon");
//         box.appendChild(icon);
//     }
//
//     var imgId = colId+'_img';
//     var img = document.getElementById(imgId);
//     if (img == undefined)
//     {
//         img = document.createElement("img");
//         img.setAttribute("id", imgId);
//         img.setAttribute("style","height:50px;width:50px;opacity:0.5;");
//         icon.appendChild(img);
//     }
//
//     var alert_type = 0;
//     if (userActivity.activity == 'LAYING')
//     {
//         alert_type = 2;
//         img.setAttribute("src","/images/man-fall.png");
//         box.setAttribute("class","small-box bg-red");
//     }
//     else
//     if (userActivity.activity.indexOf('WALKING') >= 0)
//     {
//         img.setAttribute("src","/images/man-walking.png");
//     }
//     else
//     {
//         img.setAttribute("src","/images/man-stand.png");
//     }
//
//     // footer
//     var footerId = colId+'_footer';
//     var footer = document.getElementById(footerId);
//     if (footer == undefined)
//     {
//         footer = document.createElement("a");
//         footer.setAttribute("id", footerId);
//         footer.setAttribute("href","#");
//         footer.setAttribute("class","small-box-footer");
//         box.appendChild(footer);
//     }
//
//     var chartID = colId + '_chart';
//     var chart = document.getElementById(chartID);
//     if (chart == undefined)
//     {
//         chart = document.createElement("div");
//         chart.setAttribute("id", chartID);
//         chart.setAttribute("style","height: 180px; background-color:#445566;");
//         footer.appendChild(chart);
//     }
//
//     var bodyinfoList = glUserBodyRecords[userActivity.account];
//     if (bodyinfoList != undefined)
//     {
//         var dataitemlist = [];
//         var heartbeatinfoList = bodyinfoList.heartbeat;
//         var bloodoxygeninfoList = bodyinfoList.bloodoxygen;
//
//         var heartbeat_new = 90;
//         var bloodoxygen_new = 99;
//
//         for(var index = heartbeatinfoList.length - 1; index>=0; index--)
//         {
//             var bloodoxygeninfo = bloodoxygeninfoList[index];
//             var heartbeatinfo = heartbeatinfoList[index];
//             var bloodoxygen_value = parseFloat(bloodoxygeninfo.data);
//             var heartbeat_value = parseFloat(heartbeatinfo.data);
//
//             var info = [];
//             info['y'] = bloodoxygeninfo.datetime;
//             info['bloodoxygen'] = bloodoxygen_value.toFixed(0);
//             info['heartbeat'] = heartbeat_value.toFixed(0);
//             dataitemlist.push(info);
//
//             if (index == heartbeatinfoList.length - 1)
//             {
//                 heartbeat_new = info['heartbeat'];
//                 bloodoxygen_new = info['bloodoxygen'];
//                 bodyinfo.textContent = '心率:'+ info['heartbeat'] +' 血氧:'+ info['bloodoxygen'];
//             }
//         }
//
//         if (alert_type != 2)
//             alert_type = check_user_alert_type(heartbeat_new, bloodoxygen_new);
//         if (alert_type == 2)
//         {
//             box.setAttribute("class","small-box bg-red");
//         }
//         else
//         if (alert_type == 1)
//         {
//             box.setAttribute("class","small-box bg-yellow");
//         }
//
//         if (dataitemlist.length > 0)
//         {
//             var area = glActivityCharts[chartID];
//             if (area == undefined)
//             {
//                 // console.log("updateUserActivity() userActivity.account:"+userActivity.account+" area == undefined");
//
//                 area = new Morris.Area({
//                   element: chartID,
//                   resize: true,
//                   redraw: true,
//                   // data: dataitemlist,
//                   data: [
//                     {y: '', bloodoxygen: 80, heartbeat: 80}],
//                   // data: [
//                   //   {y: '2021-05-23 12:43:20', bloodoxygen: 99, heartbeat: 130},
//                   //   {y: '2021-05-23 12:43:21', bloodoxygen: 99, heartbeat: 120},
//                   //   {y: '2021-05-23 12:43:22', bloodoxygen: 98, heartbeat: 100},
//                   //   {y: '2021-05-23 12:43:23', bloodoxygen: 97, heartbeat: 90},
//                   //   {y: '2021-05-23 12:43:24', bloodoxygen: 95, heartbeat: 100},
//                   //   {y: '2021-05-23 12:43:25', bloodoxygen: 90, heartbeat: 110},
//                   //   {y: '2021-05-23 12:43:26', bloodoxygen: 88, heartbeat: 130},
//                   //   {y: '2021-05-23 12:43:27', bloodoxygen: 85, heartbeat: 140},
//                   //   {y: '2021-05-23 12:43:28', bloodoxygen: 80, heartbeat: 150},
//                   //   {y: '2021-05-23 12:43:29', bloodoxygen: 85, heartbeat: 160}
//                   // ],
//                   xkey: 'y',
//                   ykeys: ['heartbeat', 'bloodoxygen'],
//                   ymin: 80,
//                   ymax: 200,
//                   behaveLikeLine: true,
//                   fillOpacity: 0.6,
//                   labels: ['心率', '血氧'],
//                   pointFillColors:['#ffffff'],
//                   pointStrokeColors: ['#558dbc'],
//                   lineColors: ['#3c8dbc', '#aa3344'],
//                   hideHover: 'auto'
//                 });
//                 glActivityCharts[chartID] = area;
//             }
//
//             // console.log("updateUserActivity() userActivity.account:"+userActivity.account+" chartID:" + chartID +" dataitemlist.length:" + dataitemlist.length);
//             // $("#"+chartID).empty();
//             area.setData(dataitemlist);
//             // area.redraw();
//
//         }
//     }
//
//     return divCol;
// }

// function check_user_alert_type(heartbeat, bloodoxygen)
// {
//     var alert_type = 0;// 0: normal, 1: warning 2: danger
//     var SPO2_WARNING_MIN = 90;
//     var SPO2_WARNING_MAX = 95;
//     var SPO2_DANGER_MIN = 1;
//     var SPO2_DANGER_MAX = 89;
//     var HR_WARNING_MIN = 120;
//     var HR_WARNING_MAX = 180;
//     var HR_DANGER_MIN = 181;
//     var HR_DANGER_MAX = 254;
//
//     if ( heartbeat >= HR_WARNING_MIN && heartbeat <= HR_WARNING_MAX )
//     {
//         alert_type = 1;
//     }
//     else
//     if ( heartbeat >= HR_DANGER_MIN && heartbeat <= HR_DANGER_MAX )
//     {
//         alert_type = 2;
//     }
//
//     if ( alert_type >= 2 )
//         return alert_type;
//
//     if ( bloodoxygen >= SPO2_WARNING_MIN && bloodoxygen <= SPO2_WARNING_MAX )
//     {
//         if (alert_type <= 1)
//         {
//             alert_type = 1;
//         }
//     }
//     else
//     if ( bloodoxygen >= SPO2_DANGER_MIN && bloodoxygen <= SPO2_DANGER_MAX )
//     {
//         alert_type = 2;
//     }
//
//     return alert_type;
// }

// function runLive_bodyinfo_records()
// {
//     var loStartDate = moment().subtract(1, 'days');
//     var loEndDate = moment();
//
//     var StartDate = loStartDate.format("YYYY-MM-DD HH:mm:ss");
//     var EndDate = loEndDate.format("YYYY-MM-DD HH:mm:ss");
//
//     if ( gl_bodyinfo_load_index >= glUserActivity_List.length )
//     {
//         gl_bodyinfo_load_index = 0;
//         setTimeout(runLive_bodyinfo_records, 5000);
//         return;
//     }
//
//     var userActivity = glUserActivity_List[gl_bodyinfo_load_index];
//
//     var targetURL = gl_target_server + "/php/vilsnodes.php";
//     $.getJSON(targetURL, 'project_id='+project_id+'&loaduserbodybaseinforecords=1'+'&accountid='+userActivity.account+'&StartDate='+StartDate+'&EndDate='+EndDate, function(data)
//     {
//         var bodyinfoList = {};
//         var account = userActivity.account;
//
//         $.each(data, function(itemindex, bodyinfo)
//         {
//             bodyinfoList[itemindex] = bodyinfo;
//         });
//         glUserBodyRecords[account] = bodyinfoList;
//     })
//     .success(function() {
//     });
//
//
//     gl_bodyinfo_load_index++;
//
//     setTimeout(runLive_bodyinfo_records, 100);
// }

// function collect_user_sensor()
// {
//     for	(var index = 0; index < glUser_List.length; index++)
//     {
//         var userData = glUser_List[index];
//         if (userData.sensors != undefined)
//         {
//             // console.log("runLive_userActiviry() name:" + userData.name +" sensors:" + userData.sensors);
//
//             var sensors = userData.sensors.split(",");
//             for	(var j = 0; j < sensors.length; j++)
//             {
//                 var sensorid = sensors[j];
//                 for	(var k = 0; k < glSensor_List.length; k++)
//                 {
//                     var sensorData = glSensor_List[k];
//                     if( sensorData.id === sensorid)
//                     {
//                         if( sensorData.type == 'ProMOS Watch')
//                         {
//                             console.log("collect_user_sensor() name:" + userData.name +" sensorid:" + sensorid);
//
//                             var useractivity = {};
//                             useractivity.userid = userData.userid;
//                             useractivity.account = userData.account;
//                             useractivity.username = userData.name;
//                             useractivity.sensorid = sensorData.id;
//                             if (userData.uwbcards != undefined)
//                                 useractivity.uwbcards = userData.uwbcards.split(",");
//                             else
//                                 useractivity.uwbcards = [];
//                             useractivity.activity = 'WALKING';
//
//                             glUserActivity_List.push(useractivity);
//                         }
//                         break;
//                     }
//                 }
//             }
//         }
//     }
// }

function create_dropdown_userlist()
{
    var dropdown_tags_ul = document.getElementById("dropdown_tags");

    // console.log("create_dropdown_userlist() glUser_List.length:"+glUser_List.length);

    for	(var index = 0; index < glUser_List.length; index++)
    {
        var userData = glUser_List[index];
        var item_li = create_dropdown_user_item(index, userData.userid, userData.name, userData.maplayer);
        if (item_li != null)
            dropdown_tags_ul.appendChild(item_li);
    }
}

function create_dropdown_user_item(userindex, userid, username, maplayer)
{
    var item_li = document.getElementById("dropdown_checkbox_user_"+userid);
    if ( item_li != undefined )
    {
        return null;
    }

    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", userid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_user_click(\""+ userid +"\", \""+ maplayer +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_user_"+userid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_user_click(\""+ userid +"\");return true;");
    item_checkbox.checked = false;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+username));
    item_li.appendChild(item_click);

    return item_li;
}

var PosDiff = [];
function test_calculate_Standard_Deviation(TargetPosNodeId, RealPosNodeId)
{
    var TargetPosNode = null;
    var RealPosNode = null;
    for (var i = 0; i < glTag_List.length; ++i)
    {
        var nodeData = glTag_List[i];
        var nodeid = nodeData[3];
        if (nodeid == TargetPosNodeId)
        {
            TargetPosNode = nodeData;
        }
        else
        if (nodeid == RealPosNodeId)
        {
            RealPosNode = nodeData;
        }
    }
    if (TargetPosNode == null || RealPosNode == null)
    {
        return;
    }

    var diffDistance = calculate_distance(Number(TargetPosNode[7]), Number(TargetPosNode[8]), Number(TargetPosNode[9]),
                            Number(RealPosNode[7]), Number(RealPosNode[8]), Number(RealPosNode[9]));

    var index;
    var total_m = 0;
    for(index=0; index < 20-1; index++)
    {
        // remove oldest data
        if (PosDiff[index + 1] == undefined)
            PosDiff[index + 1] = 0;
        PosDiff[index] = PosDiff[index + 1];
        total_m += PosDiff[index];
    }
    PosDiff[20 - 1] = diffDistance;
    total_m += diffDistance;

    var meaning_value = total_m / 20;

    var total = 0;
    for(index=0; index < 20-1; index++)
    {
        total += Math.pow(PosDiff[index] - meaning_value, 2);
    }

    var StandardDeviation = Math.sqrt(total / 30);

    //console.log("diffDistance:" + diffDistance +", StandardDeviation:"+StandardDeviation);

    return StandardDeviation;
}

function calculate_distance(X1, Y1, Z1, X2, Y2, Z2)
{
    var valueX = X1 - X2;
    var valueY = Y1 - Y2;
    var valueZ = Z1 - Z2;

    //console.log("calculate_distance() valueX:" + valueX +", valueY:"+valueY);

    var distance = Math.sqrt(valueX * valueX, valueY * valueY, valueZ * valueZ);
    return distance;
}

function areaname_save()
{
    var curMap = glLiveMapList[glCurrentMapIndex];
    curMap.areaname_save(curMap);
}

function map_tab_onclick(ui_index, layerid, layer_name)
{
    //console.log("map_tab_onclick() ui_index:"+ui_index);

    if (glEditMode)
    {
        //console.log("map_tab_onclick() ui_index:"+ui_index + " layer_name:"+layer_name);
        if ( glEditMode != true)
            return;

        if ( ui_index != glCurrentMapIndex )
            return;

        var newValue= prompt("Edit Layer Name:", layer_name);
        if ( newValue )
        {
            console.log("map_tab_onclick() newValue:"+newValue);

            var ui = document.getElementById("map_tab_href_"+ui_index);
            ui.setAttribute("onclick","map_tab_onclick("+ ui_index +", \""+ layerid +"\", \""+ newValue +"\");return false;");

            var span = document.getElementById("layer_name_"+ui_index);
            span.innerHTML = newValue ;

            update_maplayer_info(layerid, "layer_name", newValue);
        }
    }
    else
    {
        glCurrentMapIndex = ui_index;

        var map = glLiveMapList[ui_index];
        map.refresh();
    }
}

function create_tab_onclick(ui_index)
{
    if (glEditMode)
        return;

    //console.log("create_tab_onclick() ui_index:"+ui_index +", glCurrentMapIndex:"+glCurrentMapIndex);
    var newValue= prompt("新增地圖圖層, 地圖名稱:","地圖"+ parseInt(glLiveMapList.length+1));
    if ( newValue )
    {
        var show_index = glLiveMapList.length;
        create_maplayer(newValue, create_finish, show_index);

        //load_map_config(glLiveMapList.length+1);
    }
}

function create_finish(show_index)
{
    load_map_config(show_index);
}

function delete_maplayer_onclick(ui_index, layerid, layer_name)
{
    if (confirm('Are you sure you want to delete this Map Layer:'+layer_name+' ?'))
    {
        // delete it!
        delete_maplayer(layerid, delete_finish);
    }
    else
    {
        // Do nothing!
    }
}

function delete_finish()
{
    load_map_config(0);
}

function editmode_click(mode)
{
    //console.log("editmode_click() mode:"+mode +", glCurrentMapIndex:"+glCurrentMapIndex);

    glEditMode = mode;

    for(var i=0;i<glLiveMapList.length;i++)
    {
        var tab = document.getElementById("map_tab_li_"+i);
        var ahref = document.getElementById("map_tab_href_"+i);

        //if ( mode == true && i != glCurrentMapIndex)
        if ( mode == true )
        {
            if ( i == glCurrentMapIndex )
            {
                enable_editmode(glCurrentMapIndex);
            }
            else
            {
                tab.setAttribute("class","disabled");
                ahref.setAttribute("data-toggle","");
            }
        }
        else
        {
            disable_editmode(i);

            if ( i == glCurrentMapIndex)
                tab.setAttribute("class","active");
            else
                tab.setAttribute("class","");
            ahref.setAttribute("data-toggle","tab");
        }
    }
}

function enable_editmode(ui_index)
{
    var div_desc = document.getElementById("layer_description_"+ui_index);
    div_desc.setAttribute("style","font-weight:bold;border:thin dotted red;");

    var div_GPS_X = document.getElementById("WGS48OriginX_"+ui_index);
    div_GPS_X.setAttribute("style","border:thin dotted red;");

    var div_GPS_Y = document.getElementById("WGS48OriginY_"+ui_index);
    div_GPS_Y.setAttribute("style","border:thin dotted red;");

    var div_North = document.getElementById("MAP_North_"+ui_index);
    div_North.setAttribute("style","border:thin dotted red;");

    var div_building = document.getElementById("BuildingID_"+ui_index);
    div_building.setAttribute("style","border:thin dotted red;");

    var div_floor = document.getElementById("BuildingFloor_"+ui_index);
    div_floor.setAttribute("style","border:thin dotted red;");

    var ui_ChangeMap = document.getElementById("ChangeMap_"+ui_index);
    ui_ChangeMap.setAttribute("style","display:block;");

    var ui_DeleteMapLayer = document.getElementById("DeleteMapLayer_Btn_"+ui_index);
    ui_DeleteMapLayer.setAttribute("style","display:block;");
}

function disable_editmode(ui_index)
{
    var div_desc = document.getElementById("layer_description_"+ui_index);
    div_desc.setAttribute("style","font-weight:bold;");

    var div_GPS_X = document.getElementById("WGS48OriginX_"+ui_index);
    div_GPS_X.setAttribute("style","border:;");

    var div_GPS_Y = document.getElementById("WGS48OriginY_"+ui_index);
    div_GPS_Y.setAttribute("style","border:;");

    var div_North = document.getElementById("MAP_North_"+ui_index);
    div_North.setAttribute("style","border:;");

    var div_building = document.getElementById("BuildingID_"+ui_index);
    div_building.setAttribute("style","border:;");

    var div_floor = document.getElementById("BuildingFloor_"+ui_index);
    div_floor.setAttribute("style","border:;");

    var ui_ChangeMap = document.getElementById("ChangeMap_"+ui_index);
    ui_ChangeMap.setAttribute("style","display:none;");

    var ui_DeleteMapLayer = document.getElementById("DeleteMapLayer_Btn_"+ui_index);
    ui_DeleteMapLayer.setAttribute("style","display:none;");
}

function layer_description_onclick(ui_index, layerid, layer_description)
{
    if ( glEditMode != true)
        return;

    //console.log("layer_description_onclick() ui_index:"+ui_index + " layer_description:"+layer_description);

    var newValue= prompt("Edit Layer Description:", layer_description);
    if ( newValue )
    {
        console.log("layer_description_onclick() newValue:"+newValue);

        var ui = document.getElementById("layer_description_"+ui_index);
        ui.innerHTML = newValue + '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
        ui.setAttribute("onclick","layer_description_onclick("+ ui_index +", \""+ layerid +"\", \""+ newValue +"\");return false;");

        update_maplayer_info(layerid, "layer_description", newValue);

    }
}

function WGS48OriginX_onclick(ui_index, layerid, WGS48OriginX)
{
    //console.log("WGS48OriginX_onclick() ui_index:"+ui_index + " layerid:"+layerid + " WGS48OriginX:"+WGS48OriginX);
    if ( glEditMode != true)
        return;
    var newValue= prompt("Edit GPS WGS48O Origin Longitude:", WGS48OriginX);
    if ( newValue )
    {
        console.log("WGS48OriginX_onclick() newValue:"+newValue);

        var ui = document.getElementById("WGS48OriginX_"+ui_index);
        ui.innerHTML = newValue + '&nbsp;&nbsp;&nbsp';
        ui.setAttribute("onclick","WGS48OriginX_onclick("+ ui_index +", \""+ layerid +"\", \""+ newValue +"\");return false;");

        update_maplayer_info(layerid, "WGS48OriginX", newValue);

    }
}

function WGS48OriginY_onclick(ui_index, layerid, WGS48OriginY)
{
    // console.log("WGS48OriginY_onclick() ui_index:"+ui_index + " layerid:"+layerid + " WGS48OriginY:"+WGS48OriginY);
    if ( glEditMode != true)
        return;
    var newValue= prompt("Edit GPS WGS48O Origin Latitude:", WGS48OriginY);
    if ( newValue )
    {
        console.log("WGS48OriginY_onclick() newValue:"+newValue);

        var ui = document.getElementById("WGS48OriginY_"+ui_index);
        ui.innerHTML = newValue + '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
        ui.setAttribute("onclick","WGS48OriginY_onclick("+ ui_index +", \""+ layerid +"\", \""+ newValue +"\");return false;");

        update_maplayer_info(layerid, "WGS48OriginY", newValue);

    }
}

function MAP_North_onclick(ui_index, layerid, MAPNorth)
{
    //console.log("MAP_North_onclick() ui_index:"+ui_index + " layerid:"+layerid + " MAPNorth:"+MAPNorth);
    if ( glEditMode != true)
        return;
    var newValue= prompt("Edit GPS WGS48O Origin North:", MAPNorth);
    if ( newValue )
    {
        console.log("MAP_North_onclick() newValue:"+newValue);

        var ui = document.getElementById("MAP_North_"+ui_index);
        ui.innerHTML = newValue + '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
        ui.setAttribute("onclick","MAP_North_onclick("+ ui_index +", \""+ layerid +"\", \""+ newValue +"\");return false;");

        update_maplayer_info(layerid, "MAP_North", newValue);

    }
}

function BuildingID_onclick(ui_index, layerid, BuildingID)
{
    //console.log("BuildingID_onclick() ui_index:"+ui_index + " layerid:"+layerid + " BuildingID:"+BuildingID);
    if ( glEditMode != true)
        return;
    var newValue= prompt("Edit Building:", BuildingID);
    if ( newValue )
    {
        console.log("BuildingID_onclick() newValue:"+newValue);

        var ui = document.getElementById("BuildingID_"+ui_index);
        ui.innerHTML = 'Building:&nbsp;' + newValue + '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
        ui.setAttribute("onclick","BuildingID_onclick("+ ui_index +", \""+ layerid +"\", \""+ newValue +"\");return false;");

        update_maplayer_info(layerid, "BuildingID", newValue);
    }
}

function BuildingFloor_onclick(ui_index, layerid, BuildingFloor)
{
    //console.log("BuildingFloor_onclick() ui_index:"+ui_index + " layerid:"+layerid + " BuildingFloor:"+BuildingFloor);
    if ( glEditMode != true)
        return;
    var newValue= prompt("Edit Building Floor:", BuildingFloor);
    if ( newValue )
    {
        console.log("BuildingFloor_onclick() newValue:"+newValue);

        var ui = document.getElementById("BuildingFloor_"+ui_index);
        ui.innerHTML = 'Floor:&nbsp;' + newValue + '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
        ui.setAttribute("onclick","BuildingFloor_onclick("+ ui_index +", \""+ layerid +"\", \""+ newValue +"\");return false;");

        update_maplayer_info(layerid, "BuildingFloor", newValue);
    }
}

function node_click(target)
{
    //console.log("node_click() target.type:"+target.type + ", target.nodeid:"+target.nodeid);

    // if ( parent.$('#rightcontentIframe')[0] != undefined )
    // {
    //     if ( typeof parent.$('#rightcontentIframe')[0].contentWindow.show_device_info === "function")
    //     {
    //         parent.$('#rightcontentIframe')[0].contentWindow.CUR_Show_Decice_ID = target.nodeid;
    //         //console.log("node_click() CUR_Show_Decice_ID:"+parent.$('#rightcontentIframe')[0].contentWindow.CUR_Show_Decice_ID);
    //
    //         for (var i = 0; i < glAnchor_List.length; ++i)
    //         {
    //             var nodeData = glAnchor_List[i];
    //             var nodeid = nodeData[3];
    //             if (target.nodeid == nodeid)
    //                 parent.$('#rightcontentIframe')[0].contentWindow.show_device_info("Anchor", nodeData);
    //         }
    //
    //         for (var i = 0; i < glTag_List.length; ++i)
    //         {
    //             var nodeData = glTag_List[i];
    //             var nodeid = nodeData[3];
    //             if (target.nodeid == nodeid)
    //                 parent.$('#rightcontentIframe')[0].contentWindow.show_device_info("Tag", nodeData);
    //         }
    //
    //         for (var i = 0; i < glCoord_List.length; ++i)
    //         {
    //             var nodeData = glCoord_List[i];
    //             var nodeid = nodeData[3];
    //             if (target.nodeid == nodeid)
    //                 parent.$('#rightcontentIframe')[0].contentWindow.show_device_info("Coordinator", nodeData);
    //         }
    //
    //         for (var i = 0; i < glLocator_List.length; ++i)
    //         {
    //             var nodeData = glLocator_List[i];
    //             var nodeid = nodeData[3];
    //             if (target.nodeid == nodeid)
    //                 parent.$('#rightcontentIframe')[0].contentWindow.show_device_info("Locator", nodeData);
    //         }
    //
    //     }
    // }
}

 //
 // Preventing the pull-to-refresh effect in Chrome
 //
 window.addEventListener('load', function() {
   var preventPullToRefreshCheckbox = document.getElementById('preventPullToRefresh');
   var preventOverscrollGlowCheckbox = document.getElementById("preventOverscrollGlow");
   var preventScrollCheckbox = document.getElementById("preventScroll");

   var maybePreventPullToRefresh = false;
   var lastTouchY = 0;
   var touchstartHandler = function(e) {
     if (e.touches.length != 1) return;
     lastTouchY = e.touches[0].clientY;
     // Pull-to-refresh will only trigger if the scroll begins when the
     // document's Y offset is zero.
     maybePreventPullToRefresh =
         preventPullToRefreshCheckbox.checked &&
         window.pageYOffset == 0;
   }

   var touchmoveHandler = function(e) {
     var touchY = e.touches[0].clientY;
     var touchYDelta = touchY - lastTouchY;
     lastTouchY = touchY;

     if (maybePreventPullToRefresh) {
       // To suppress pull-to-refresh it is sufficient to preventDefault the
       // first overscrolling touchmove.
       maybePreventPullToRefresh = false;
       if (touchYDelta > 0) {
         e.preventDefault();
         return;
       }
     }

     if (preventScrollCheckbox.checked) {
       e.preventDefault();
       return;
     }

     if (preventOverscrollGlowCheckbox.checked) {
       if (window.pageYOffset == 0 && touchYDelta > 0) {
         e.preventDefault();
         return;
       }
     }
   }

   document.addEventListener('touchstart', touchstartHandler, {passive: false });
   document.addEventListener('touchmove', touchmoveHandler, {passive: false });
 });

var DUMP_TIME = 10;
var DumpTime = DUMP_TIME;
function DumpRangingResult()
{
    if ( DumpTime >= DUMP_TIME )
    {
        var timeout = document.getElementById("dumprangingresulttimeout").value;
        DUMP_TIME = timeout;
        DumpTime = timeout;

        document.getElementById("dumprangingresultbtn").disabled = true;
        document.getElementById("dumprangingresultbtn").innerHTML = 'Dump Ranging Result(' + DumpTime + ' Seconds)';
        document.getElementById("dumprangingresulttimeout").disabled = true;

        var targetURL = gl_target_server + "/php/vilsnodes.php";
        var jsondata = {};
        jsondata['project_id'] = ''+project_id;
        jsondata['DUMP_RANGING_RESULT'] = timeout;

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

        DumpTime--;
        setTimeout(DumpRangingResult, 1000);
    }
    else
    if ( DumpTime >= 0 )
    {
        document.getElementById("dumprangingresultbtn").disabled = true;
        document.getElementById("dumprangingresultbtn").innerHTML = 'Dump Ranging Result(' + DumpTime + ' Seconds)';
        document.getElementById("dumprangingresulttimeout").disabled = true;

        DumpTime--;
        setTimeout(DumpRangingResult, 1000);
    }
    else
    {
        DumpTime = DUMP_TIME;
        document.getElementById("dumprangingresultbtn").disabled = false;
        document.getElementById("dumprangingresultbtn").innerHTML = 'Dump Ranging Result(' + DumpTime + ' Seconds)';
        document.getElementById("dumprangingresulttimeout").disabled = false;
    }
}

function DumpAoARangingLog()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['dumplogtype'] = 'PrintAoACirclePoint';

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
}

// var gl_rollcall_all_tag = [];
// var gl_rollcall_area_tag = [];
// var gl_rollcall_area_user = [];
// var gl_rollcall_none_tag = [];
// var gl_rollcall_tag_num = 0;
// var gl_rollcall_tag_load_index = 0;
// var gl_rollcall_areaid = "";
// var gl_rollcall_areaname = "";

function fetch_rollcall(areaId, areaName)
{
    // gl_all_tag = [];
    // gl_area_tag = [];
    // gl_none_tag = [];
    // gl_tag_num = 0;
    // gl_tag_load_index = 0;
    gl_rollcall_all_tag = [];
    gl_rollcall_area_tag = [];
    gl_rollcall_area_user = [];
    gl_rollcall_none_tag = [];
    gl_rollcall_tag_num = 0;
    gl_rollcall_tag_load_index = 0;
    gl_rollcall_areaid = "";
    gl_rollcall_areaname = "";

    //
    // start load location
    //
    gl_rollcall_areaid = areaId;
    gl_rollcall_areaname = areaName;

    load_rollcall_Tags_Info();

}

function show_rollcall_info()
{

    // var showmsg = "區域:"+gl_rollcall_areaname+ "<br>應到:"+gl_all_tag.length+"<br>實到:"+gl_area_tag.length+"<br>未到:"+gl_none_tag.length;
    var presentCount = gl_rollcall_area_tag.length + gl_rollcall_area_user.length;
    var showmsg = "區域:"+gl_rollcall_areaname+ "<br>人數:"+presentCount;

    var popupID = "maparea_popup_"+gl_rollcall_areaid;
    var popupUI = document.getElementById(popupID);
    if ( popupUI == null )
    {
        console.log(popupID + " is null");
    }
    else
    {
        popupUI.innerHTML = showmsg;
    }

    console.log(showmsg);
}

function load_rollcall_Tags_Info()
{
    // console.log("load_rollcall_Tags_Info() gl_rollcall_areaid:"+gl_rollcall_areaid +" gl_rollcall_areaname:"+gl_rollcall_areaname);

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_rollcall_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_rollcall_tag_load_index = 0;
    })
    .success(function() {
        setTimeout(load_rollcall_Tags, 10);
    })
    .error(function() {
        setTimeout(load_rollcall_Tags_Info, 1000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_rollcall_Tags()
{
    if (gl_rollcall_tag_load_index >= gl_rollcall_tag_num)
    {
        gl_rollcall_tag_load_index = 0;
        load_rollcall_User_Info();
        return;
    }
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadtagrollcall=1&s='+gl_rollcall_tag_load_index+'&count='+LOAD_DEVICE_NUM+'&areaid='+gl_rollcall_areaid+'&project_id='+project_id, function(data)
    {
        var tag_num = parseInt(data['LOC_TAG_NUM']);

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_rollcall_tag_load_index; i < gl_rollcall_tag_load_index+tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;
            // split data
            var node = data[key];
            var nodeData = node.split(",");
            var alive = nodeData[1];
            var areaid = nodeData[6];
            var areaname = nodeData[7];
            var AreaAction = nodeData[8]; // 0:normal 1:in  2:out
            var asleep = nodeData[10];
            var areaType5Id = nodeData[11];
            var areaType5Name = nodeData[12];

            //console.log("load_Tags()  nodeid:"+nodeData[3] +" areaid:"+areaid +" gl_rollcall_areaid:"+gl_rollcall_areaid);
            gl_rollcall_all_tag.push(nodeData);
            // if ( areaid == gl_rollcall_areaid )
            // if ( (areaid == gl_rollcall_areaid) && (AreaAction != '2') && (alive != '0') && (area_unknown != '1'))
            if ( ((areaid == gl_rollcall_areaid) || (areaType5Id == gl_rollcall_areaid) )&& (AreaAction == '1'))
            {
                if ( (alive == '0') && (asleep == '0') )
                    gl_rollcall_none_tag.push(nodeData);
                else
                    gl_rollcall_area_tag.push(nodeData);
            }
            else
                gl_rollcall_none_tag.push(nodeData);
        }
        gl_rollcall_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_rollcall_tag_load_index >= gl_rollcall_tag_num)
        {
            gl_rollcall_tag_load_index = 0;

            // show_rollcall_info();
            load_rollcall_User_Info();
        }
        else
        {
            setTimeout(load_rollcall_Tags, 10);
        }
    })
    .error(function() {
        gl_rollcall_tag_load_index = 0;
    })
    .complete(function() {
    });

}

function load_rollcall_User_Info()
{
    var targetURL = gl_target_server + "/php/fpnodes.php";

    var getQuery = $.getJSON(targetURL, 'loadenterarea=1&areaid='+gl_rollcall_areaid+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, value)
        {
            var newuser = [];
            newuser.NodeId = value.NodeId;
            newuser.entertime = value.entertime;
            gl_rollcall_area_user.push(newuser);
        });
    })
    .success(function() {
    // console.log("load_area_user_info() success glCurrentArea:"+glCurrentArea);
    })
    .error(function() {
    // console.log("load_area_user_info() error glCurrentArea:"+glCurrentArea);
    })
    .complete(function() {
        show_rollcall_info();
    });
}

function create_dropdown_taglist()
{
    //var tagList = glLiveMapList[glCurrentMapIndex].getTagList();

    //console.log("create_dropdown_taglist() tagList.length:"+tagList.length);

    var dropdown_tags_ul = document.getElementById("dropdown_tags");
    while (dropdown_tags_ul.hasChildNodes())
    {
        dropdown_tags_ul.removeChild(dropdown_tags_ul.firstChild);
    }

    for	(var index = 0; index < glTag_List.length; index++)
    {
        var nodeData = glTag_List[index];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];
        var maplayer = nodeData[36];
        //console.log("Tag:"+exist_node);
        var item_li = create_dropdown_tag_item(index, nodeid, nodename, maplayer);
        dropdown_tags_ul.appendChild(item_li);
    }
}

function create_dropdown_tag_item(tagindex, nodeid, nodename, maplayer)
{
    //console.log("create_dropdown_item() nodeid:"+nodeid+", tagindex:"+tagindex);

    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", nodeid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_tag_click(\""+ nodeid +"\", \""+ maplayer +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_tag_"+nodeid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_tag_click(\""+ nodeid +"\");return true;");
    item_checkbox.checked = false;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+nodename));
    item_li.appendChild(item_click);

    return item_li;
}

function create_dropdown_locatorlist()
{
    //var locatorList = glLiveMapList[glCurrentMapIndex].getLocatorList();

    //console.log("create_dropdown_locatorlist() locatorList.length:"+locatorList.length);

    var dropdown_locators_ul = document.getElementById("dropdown_locators");
    while (dropdown_locators_ul.hasChildNodes())
    {
        dropdown_locators_ul.removeChild(dropdown_locators_ul.firstChild);
    }

    for	(var index = 0; index < glLocator_List.length; index++)
    {
        var nodeData = glLocator_List[index];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];
        var maplayer = nodeData[26];

        var item_li = create_dropdown_locator_item(index, nodeid, nodename, maplayer);
        dropdown_locators_ul.appendChild(item_li);
    }
}

function create_dropdown_locator_item(locatorindex, nodeid, nodename, maplayer)
{
    //console.log("create_dropdown_item() nodeid:"+nodeid+", tagindex:"+tagindex);

    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value", nodeid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_locator_click(\""+ nodeid +"\", \""+ maplayer +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_locator_"+nodeid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_locator_click(\""+ nodeid +"\");return true;");
    item_checkbox.checked = false;

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+nodename));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_tag_click = false;
function dropdown_tag_click(nodeid, maplayer)
{
    console.log("dropdown_item_click() nodeid:"+nodeid +" maplayer:"+maplayer);

    if (document.getElementById("dropdown_checkbox_tag_"+nodeid) != undefined)
    {
        var checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_tag_"+nodeid).checked;

        if (glDropdown_checkbox_tag_click)
        {
            // keep the same value
        }
        else
        {
            // toggle value
            document.getElementById("dropdown_checkbox_tag_"+nodeid).checked = !checkedValue_dropdown_checkbox;
            checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_tag_"+nodeid).checked;
        }
    }
    else
    {
        return false;
    }

    // find tag in which maplaye
    for(var index=0;index<glLiveMapList.length;index++)
    {
        if( glLiveMapList[index].mMapLayerId === maplayer || ( (index == 0) && (maplayer == '0')))
        {
            $('#map_tab_href_'+index).trigger('click');

            glLiveMapList[index].setTagPopup(nodeid, checkedValue_dropdown_checkbox);
            console.log("dropdown_tag_click() nodeid:"+nodeid +", show:"+checkedValue_dropdown_checkbox);
            break;
        }
    }

    glDropdown_checkbox_tag_click = false;
    return true;
}

function dropdown_checkbox_tag_click(nodeid)
{
    glDropdown_checkbox_tag_click = true;
}


var glDropdown_checkbox_locator_click = false;
function dropdown_locator_click(nodeid, maplayer)
{
    //console.log("dropdown_locator_click() nodeid:"+nodeid);

    var checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_locator_"+nodeid);
    if (checkedValue_dropdown_checkbox == undefined)
        return false;

    var checkedValue_dropdown_checkvalue = checkedValue_dropdown_checkbox.checked;

    if (glDropdown_checkbox_locator_click)
    {
        // keep the same value
    }
    else
    {
        // toggle value
        checkedValue_dropdown_checkbox.checked = !checkedValue_dropdown_checkvalue;
        checkedValue_dropdown_checkvalue = checkedValue_dropdown_checkbox.checked;
    }

    // find locator in which maplaye
    for(var index=0;index<glLiveMapList.length;index++)
    {
        if( glLiveMapList[index].mMapLayerId === maplayer || ( (index == 0) && (maplayer == '0')))
        {
            $('#map_tab_href_'+index).trigger('click');

            glLiveMapList[glCurrentMapIndex].setLocatorPopup(nodeid, checkedValue_dropdown_checkvalue);
            //console.log("dropdown_locator_click() nodeid:"+nodeid +", show:"+checkedValue_dropdown_checkbox);
            break;
        }
    }

    glDropdown_checkbox_locator_click = false;
}

function dropdown_checkbox_locator_click(nodeid)
{
    //console.log("dropdown_checkbox_locator_click() nodeid:"+nodeid);

    glDropdown_checkbox_locator_click = true;
}

var glDropdown_checkbox_user_click = false;
function dropdown_user_click(userid, maplayer)
{
    console.log("dropdown_user_click() userid:"+userid +" maplayer:"+maplayer);

    if (document.getElementById("dropdown_checkbox_user_"+userid) != undefined)
    {
        var checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_user_"+userid).checked;

        if (glDropdown_checkbox_user_click)
        {
            // keep the same value
        }
        else
        {
            // toggle value
            document.getElementById("dropdown_checkbox_user_"+userid).checked = !checkedValue_dropdown_checkbox;
            checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_user_"+userid).checked;
        }
    }
    else
    {
        return false;
    }

    // find tag in which maplaye
    for(var index=0;index<glLiveMapList.length;index++)
    {
        if( glLiveMapList[index].mMapLayerId === maplayer || ( (index == 0) && (maplayer == '0')))
        {
            $('#map_tab_href_'+index).trigger('click');

            glLiveMapList[index].setUserPopup(userid, checkedValue_dropdown_checkbox);
            console.log("dropdown_user_click() userid:"+userid +", show:"+checkedValue_dropdown_checkbox);
            break;
        }
    }

    glDropdown_checkbox_user_click = false;
    return true;
}

function dropdown_checkbox_user_click(userid)
{
    glDropdown_checkbox_user_click = true;
}

function searchresult_tag_click(nodeid, maplayer)
{
    console.log("searchresult_tag_click() nodeid:"+nodeid);

    // find tag in which maplaye
    for(var index=0;index<glLiveMapList.length;index++)
    {
        if( glLiveMapList[index].mMapLayerId === maplayer || ( (index == 0) && (maplayer == '0')))
        {
            $('#map_tab_href_'+index).trigger('click');
            glLiveMapList[index].setTagPopup(nodeid, true);
            break;
        }
    }

    //if ($('#dropdown_searchresult').is(":hidden"))
    //{
        //$('#dropdown_searchresult').dropdown('toggle');
    //}
        $('#dropdown_searchresult').toggle();
}

function searchresult_locator_click(nodeid, maplayer)
{
    console.log("searchresult_locator_click() nodeid:"+nodeid);

    // find locator in which maplaye
    for(var index=0;index<glLiveMapList.length;index++)
    {
        if( glLiveMapList[index].mMapLayerId === maplayer || ( (index == 0) && (maplayer == '0')))
        {
            $('#map_tab_href_'+index).trigger('click');
            glLiveMapList[glCurrentMapIndex].setLocatorPopup(nodeid, true);
            break;
        }
    }

    //$('#dropdown_searchresult').dropdown('toggle');
    $('#dropdown_searchresult').toggle();
}

function searchresult_sensor_click(id, maplayer)
{
    console.log("searchresult_sensor_click() id:"+id+" maplayer:"+maplayer);

    // find locator in which maplaye
    for(var index=0;index<glLiveMapList.length;index++)
    {
        if( glLiveMapList[index].mMapLayerId === maplayer || ( (index == 0) && (maplayer == '0')))
        {
            $('#map_tab_href_'+index).trigger('click');
            glLiveMapList[glCurrentMapIndex].setSensorPopup(id, true);
            break;
        }
    }

    //$('#dropdown_searchresult').dropdown('toggle');
    $('#dropdown_searchresult').toggle();
}

function searchresult_user_click(id, maplayer)
{
    console.log("searchresult_user_click() id:"+id+" maplayer:"+maplayer);

    // find locator in which maplaye
    for(var index=0;index<glLiveMapList.length;index++)
    {
        if( glLiveMapList[index].mMapLayerId === maplayer || ( (index == 0) && (maplayer == '0')))
        {
            $('#map_tab_href_'+index).trigger('click');
            glLiveMapList[glCurrentMapIndex].setUserPopup(id, true);
            break;
        }
    }

    //$('#dropdown_searchresult').dropdown('toggle');
    $('#dropdown_searchresult').toggle();
}

function getFirstMapLayer()
{
    return glLiveMapList[0].mMapLayerId;
}

function start_alert(nodeid)
{
    console.log("LiveMap.js start_alert() nodeid:"+nodeid);

    for(var index=0;index<glLiveMapList.length;index++)
    {
        glLiveMapList[index].setStartAlert(nodeid);
    }
}

function stop_alert(nodeid)
{
    console.log("LiveMap.js stop_alert() nodeid:"+nodeid);

    for(var index=0;index<glLiveMapList.length;index++)
    {
        glLiveMapList[glCurrentMapIndex].setStopAlert(nodeid);
    }
}

function show_sensor(id)
{
    console.log("LiveMap.js show_sensor() id:"+id);
}

function map_search_text_onfocus(id)
{
    var input_value = document.getElementById("map_search_text").value;

    add_search_result_to_table(input_value);
}

function map_search_text_change(e)
{
    var input_value = document.getElementById("map_search_text").value;

    add_search_result_to_table(input_value);
}

const $source = document.querySelector('#map_search_text');
$source.addEventListener('input', map_search_text_change);// register for oninput

function add_search_result_to_table(searchtext)
{
    var dropdown_searchresult = document.getElementById('dropdown_searchresult');
    while (dropdown_searchresult.hasChildNodes())
    {
        dropdown_searchresult.removeChild(dropdown_searchresult.firstChild);
    }

    var searchtext_lc = searchtext.toLowerCase();

    var index = 0;
    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];
        var maplayer = nodeData[36];

        var targetname = nodename.toLowerCase();
        var targetid = nodeid.toLowerCase();

        var ret = targetname.indexOf(searchtext_lc);
        if ( ret < 0 )
        {
            ret = targetid.indexOf(searchtext_lc);
            if ( ret < 0 )
            {
                continue;
            }
        }

        // console.log("add_search_result_to_table() Tag nodename:"+nodename);

        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("tabIndex","-1");
        item_click.setAttribute("data-value", nodeid);
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","searchresult_tag_click(\""+ nodeid +"\", \""+ maplayer +"\");return true;");

        item_click.appendChild(document.createTextNode(" "+nodename));
        item_li.appendChild(item_click);


        //var item_li = create_dropdown_tag_item(index, nodeid, nodename, maplayer);
        dropdown_searchresult.appendChild(item_li);

        index++;
    }

    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var nodeid = nodeData[3];
        var nodename = nodeData[5];
        var maplayer = nodeData[26];

        var ret = nodename.indexOf(searchtext);
        if ( ret < 0 )
            continue;

        // console.log("add_search_result_to_table() Locator nodename:"+nodename);

        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("tabIndex","-1");
        item_click.setAttribute("data-value", nodeid);
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","searchresult_locator_click(\""+ nodeid +"\", \""+ maplayer +"\");return true;");

        item_click.appendChild(document.createTextNode(" "+nodename));
        item_li.appendChild(item_click);

        //var item_li = create_dropdown_locator_item(index, nodeid, nodename, maplayer);
        dropdown_searchresult.appendChild(item_li);

        index++;
    }

    for (var i = 0; i < glSensor_List.length; i++)
    {
        var sensorData = glSensor_List[i];

        var ret = sensorData.name.indexOf(searchtext);
        if ( ret < 0 )
            continue;

        // console.log("add_search_result_to_table() Sensor name:"+sensorData.name);

        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("tabIndex","-1");
        item_click.setAttribute("data-value", sensorData.id);
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","searchresult_sensor_click(\""+ sensorData.id +"\", \""+ sensorData.maplayer +"\");return true;");

        item_click.appendChild(document.createTextNode(" "+sensorData.name));
        item_li.appendChild(item_click);

        //var item_li = create_dropdown_locator_item(index, nodeid, nodename, maplayer);
        dropdown_searchresult.appendChild(item_li);

        index++;
    }

    for (var i = 0; i < glUser_List.length; i++)
    {
        var userData = glUser_List[i];

        var ret = userData.name.indexOf(searchtext);
        if ( ret < 0 )
            continue;

        var item_li = document.createElement("li");
        var item_click = document.createElement("a");
        item_click.setAttribute("class","small");
        item_click.setAttribute("tabIndex","-1");
        item_click.setAttribute("data-value", userData.userid);
        item_click.setAttribute("href","javascript:;");
        item_click.setAttribute("onclick","searchresult_user_click(\""+ userData.userid +"\", \""+ userData.maplayer +"\");return true;");

        item_click.appendChild(document.createTextNode(" "+userData.name));
        item_li.appendChild(item_click);

        //var item_li = create_dropdown_locator_item(index, nodeid, nodename, maplayer);
        dropdown_searchresult.appendChild(item_li);

        index++;
    }

    // Toggle dropdown if not already visible:
    if (index > 0)
    {
        if ($('#dropdown_searchresult').is(":hidden"))
        {
            $('#dropdown_searchresult').toggle();
            //$('#dropdown_searchresult').dropdown('toggle');
        }
    }

    // for (var i = 0; i < glCoord_List.length; i++)
    // {
    //     var nodeData = glCoord_List[i];
    //     var nodeid = nodeData[3];
    //     var nodename = nodeData[5];
    //     var maplayer = nodeData[26];
    //
    //     var ret = nodename.indexOf(searchtext);
    //     if ( ret < 0 )
    //         continue;
    //
    //     console.log("add_search_result_to_table() Coord nodename:"+nodename);
    //
    //     var item_li = create_dropdown_tag_item(index, nodeid, nodename, maplayer);
    //     dropdown_searchresult.appendChild(item_li);
    //
    //     index++;
    // }
    //
    // for (var i = 0; i < glAnchor_List.length; i++)
    // {
    //     var nodeData = glAnchor_List[i];
    //     var nodeid = nodeData[3];
    //     var nodename = nodeData[5];
    //     var maplayer = nodeData[20];
    //
    //     var ret = nodename.indexOf(searchtext);
    //     if ( ret < 0 )
    //         continue;
    //
    //     console.log("add_search_result_to_table() Anchor nodename:"+nodename);
    //
    //     var item_li = create_dropdown_tag_item(index, nodeid, nodename, maplayer);
    //     dropdown_searchresult.appendChild(item_li);
    //
    //     index++;
    // }

}

function getMainGroupInfo(maingroupid)
{
    var maingroupinfo = undefined;
    var int_maingroupid = parseInt(maingroupid);

    for (var i = 0; i < gl_usermaingroups.length; ++i)
    {
        var maingroup = gl_usermaingroups[i];

        if ( int_maingroupid == maingroup.id )
        {
            return maingroup;
        }
    }
    return maingroupinfo;
}

function markclick(id)
{
    // console.log("LiveMap markclick() id:"+id);

    glShowDefaultNode = true;
    glShowDefaultNodeId = id;
}

function isANumber(str)
{
    return !/\D/.test(str);
}

function open_log_page()
{
    var win = window.open('/index.php/log', '_blank');
    win.focus();
}

function open_2dmap_page()
{
    var win = window.open('/index.php/vils2dmap/'+project_id, '_blank');
    win.focus();
}
