/*
 * Author: ikki Chung
 * Date: 2019.12.23
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_DEVICE_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;

var gl_tag_num = 0;
var gl_tag_load_index = 0;
var gl_tags = [];

var gl_Current_info_type = '';
var gl_Current_info = '';
var gl_target_tag_id = '';
var gl_target_tag_name = '';

var glTagetTagID = '';
var glTagetLocatorID = '';

var glTagBodyInfo = [];
var glTagBodyRecords = [];
var glLocatorBodyInfo = [];
var glWeightLineChart = null;
var glHeartBeatLineChart = null;

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
                load_abbox();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_tag_bodyinfo();
                load_tag_bodyinfo_records(0);
                load_locator_bodyinfo();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_bodyinfo();
                show_bodyinfo_records(0);

                // jQueryKnob
                //$('.knob').knob();

                //glWeightLineChart.redraw();
                //glHeartBeatLineChart.redraw();

                //line.redraw();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_node_info();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
    }

    if ( CurrentLoadIndex != 4 )
        setTimeout(check_loading_status, 100);
    else
    {
        console.log("check_loading_status() Stop.");
    }
}

function load_abbox()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loadabbox=1', function(data)
    {
        clear_table('abbox_list_content');

        var abbox_list_content = document.getElementById('abbox_list_content');
        var count = 0;
        $.each(data, function(itemindex, nodeinfo)
        {
            var table_tr = document.createElement('tr');
            abbox_list_content.appendChild(table_tr);

            var table_td_nodename = document.createElement('td');
            table_td_nodename.setAttribute("id","id_row_node_name_"+ nodeinfo.nodeid);
            table_td_nodename.setAttribute("onclick","click_abbox(\""+ nodeinfo.nodeid +"\");return false;");
            table_tr.appendChild(table_td_nodename);

            var tag_nodeid_span = document.createElement('span');
            tag_nodeid_span.setAttribute("class",'badge bg-yellow');
            tag_nodeid_span.textContent = nodeinfo.nodename;
            tag_nodeid_span.setAttribute("onclick","click_abbox(\""+ nodeinfo.nodeid +"\");return false;");
            table_td_nodename.appendChild(tag_nodeid_span);

        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {

    })
    .complete(function() {
    });
}


function load_tag_bodyinfo()
{
    if (glTagetLocatorID === '' )
    {
        setTimeout(load_tag_bodyinfo, 1000);
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loadtagbodyinfo=1'+'&targetid='+glTagetLocatorID, function(data)
    {
        // clear_table('NODE_BODYINFO_TABLE');

        var bodyinfoList = [];

        $.each(data, function(itemindex, bodyinfo)
        {
            bodyinfoList.push(bodyinfo);

            //console.log("nodename:"+bodyinfo.nodename+" weight:" +bodyinfo.weight+" heartbeat:"+bodyinfo.heartbeat);
            // var row = create_new_row('NODE_BODYINFO_TABLE');
            //
            // if (bodyinfo.heartbeat == '0')
            // {
            //     var block = create_heartbeat_block(0, bodyinfo.nodenid, bodyinfo.nodename, bodyinfo.heartbeat, bodyinfo.heartbeatTime);
            //     row.appendChild(block);
            // }
            // else
            // {
            //     var block = create_heartbeat_block(1, bodyinfo.nodenid, bodyinfo.nodename, bodyinfo.heartbeat, bodyinfo.heartbeatTime);
            //     row.appendChild(block);
            // }
            //
            // if (bodyinfo.weight == '0')
            // {
            //     var block = create_weight_block(0, bodyinfo.nodenid, bodyinfo.nodename, bodyinfo.weight, bodyinfo.weightTime);
            //     row.appendChild(block);
            // }
            // else
            // {
            //     var block = create_weight_block(1, bodyinfo.nodenid, bodyinfo.nodename, bodyinfo.weight, bodyinfo.weightTime);
            //     row.appendChild(block);
            // }
        });
        glTagBodyInfo[glTagetLocatorID] = bodyinfoList;
    })
    .success(function() {
        setTimeout(load_tag_bodyinfo, 500);
    })
    .error(function() {
        setTimeout(load_tag_bodyinfo, 3000);
    })
    .complete(function() {
    });
}

function load_tag_bodyinfo_records(once)
{
    if (glTagetTagID === '' )
    {
        if(once == 0)
            setTimeout(load_tag_bodyinfo_records, 1000, 0);
        return;
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loadtagbodyinforecords=1'+'&targetid='+glTagetTagID, function(data)
    {
        var bodyinfoList = [];

        $.each(data, function(itemindex, bodyinfo)
        {
            bodyinfoList.push(bodyinfo);
        });
        glTagBodyRecords[glTagetTagID] = bodyinfoList;
    })
    .success(function() {
        if(once == 0)
            setTimeout(load_tag_bodyinfo_records, 30000, 0);
    })
    .error(function() {
        if(once == 0)
            setTimeout(load_tag_bodyinfo_records, 3000, 0);
    })
    .complete(function() {
    });
}

function load_locator_bodyinfo()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loadlocatorbodyinfo=1'+'&targetid='+glTagetLocatorID, function(data)
    {
        // clear_table('LOCATOR_BODYINFO_TABLE');

        var firstNodeId = '';
        $.each(data, function(itemindex, bodyinfo)
        {
            //console.log("nodename:"+bodyinfo.nodename+" weight:" +bodyinfo.weight+" heartbeat:"+bodyinfo.heartbeat);
            // var row = create_new_row('LOCATOR_BODYINFO_TABLE');
            //
            // var block = create_locator_heartbeat_block(bodyinfo.nodenid, bodyinfo.nodename, bodyinfo.heartbeat, bodyinfo.heartbeatTime);
            // row.appendChild(block);
            //
            // var block = create_locator_weight_block(bodyinfo.nodenid, bodyinfo.nodename, bodyinfo.weight, bodyinfo.weightTime);
            // row.appendChild(block);

            //bodyinfoList.push(bodyinfo);

            if ( firstNodeId === '' )
                firstNodeId = bodyinfo.nodeid;

            glLocatorBodyInfo[bodyinfo.nodeid] = bodyinfo;
        });

        if (glTagetLocatorID === '' )
        {
            glTagetLocatorID = firstNodeId;
        }

    })
    .success(function() {
        setTimeout(load_locator_bodyinfo, 500);
    })
    .error(function() {
        setTimeout(load_locator_bodyinfo, 3000);
    })
    .complete(function() {
    });
}

function load_node_info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_tag_load_index = 0;

        gl_tags = [];
    })
    .success(function() {
        setTimeout(load_tag_info, 500);
    })
    .error(function() {
        setTimeout(load_node_info, 1000);
    })
    .complete(function() {
    });
}

function load_tag_info()
{
    if (gl_tag_load_index >= gl_tag_num)
    {
        setTimeout(load_node_info, 5000);
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

        for (var i = gl_tag_load_index; i < gl_tag_load_index+tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
            var node = data[key];
            if (node == undefined)
                continue;
            var nodeData = node.split(",");

            gl_tags.push(nodeData);

        }
        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            var dropdown_tags = document.getElementById('dropdown_tags');
            load_Items_to_newevent_UI("tag", dropdown_tags, gl_tags);
        }
        else
        {
            setTimeout(load_tag_info, 50);
        }
    })
    .error(function() {
    })
    .complete(function() {
    });
}


function load_Items_to_newevent_UI(itemtype, tag_selection_btn_group, items)
{
    var itemid = "";
    var itemname = "";

    if ( items.length > 0 )
    {
        var nodeData = items[0];
        itemid = nodeData[3];
        itemname = nodeData[5];
    }

    for (var i = 0; i < items.length; i++)
    {
        var nodeData = items[i];
        itemid = nodeData[3];
        itemname = nodeData[5];

        var dropdown_li_0 = document.createElement('li');
        tag_selection_btn_group.appendChild(dropdown_li_0);

        var dropdown_li_0_href = document.createElement('a');
        dropdown_li_0.appendChild(dropdown_li_0_href);
        dropdown_li_0_href.setAttribute("href", "javascript:;");
        dropdown_li_0_href.setAttribute("onclick", "dropdown_"+itemtype+"_select("+i+", \""+ itemid +"\", \""+ itemname +"\");return false;");
        dropdown_li_0_href.setAttribute("data-id", itemid);
        dropdown_li_0_href.id = "dropdown_"+itemtype+"_item_"+i;
        var dropdown_li_0_href_text = document.createTextNode(itemname);
        dropdown_li_0_href.appendChild(dropdown_li_0_href_text);
    }
}

function dropdown_tag_select(itemidx, itemid, itemname)
{
    gl_target_tag_id = itemid;
    gl_target_tag_name = itemname;

    $("#target_tag").html(gl_target_tag_name);

    if ( gl_Current_info_type === 'heartbeat')
    {
        //document.getElementById('bodyinfocontent').value = '裝置:'+gl_target_tag_name+' 心率資訊:'+gl_Current_info;
        //document.getElementById('bodyinfocontent').value = '裝置:'+gl_target_tag_name+' 心率資訊:'+gl_Current_info;
    }
    else
    if ( gl_Current_info_type === 'weight')
    {
        //document.getElementById('bodyinfocontent').value = '裝置:'+gl_target_tag_name+' 心率體重資訊:'+gl_Current_info;
    }
}

function show_bodyinfo()
{
    var locatorBodyinfo = glLocatorBodyInfo[glTagetLocatorID];
    var tagBodyinfoList = glTagBodyInfo[glTagetLocatorID];

    if (locatorBodyinfo == undefined)
    {
        setTimeout(show_bodyinfo, 1000);
        return;
    }

    clear_table('NODE_BODYINFO_TABLE');

    var bodyinfo_table_title = document.getElementById('NODE_BODYINFO_TABLE_HEAD');
    bodyinfo_table_title.innerText = locatorBodyinfo.nodename;

    if ( tagBodyinfoList == undefined )
    {
        glTagetTagID = '';
        create_body_info_block(locatorBodyinfo, false);
    }
    else
    {
        if( tagBodyinfoList.length == 0 )
        {
            glTagetTagID = '';
            create_body_info_block(locatorBodyinfo, false);
        }
        else
        {
            //
            // find tags
            //
            for(var indexTag=0;indexTag<tagBodyinfoList.length;indexTag++)
            {
                var tagBodyinfo = tagBodyinfoList[indexTag];
                create_body_info_block(tagBodyinfo, true);

                if (indexTag == 0)
                {
                    glTagetTagID = tagBodyinfo.nodeid;
                }
            }
        }
    }

    setTimeout(show_bodyinfo, 500);
}

function show_bodyinfo_records(once)
{
    var weight_chart = document.getElementById('weight_chart');
    var heartbeat_chart = document.getElementById('heartbeat_chart');
    var bodyinfoList = glTagBodyRecords[glTagetTagID];
    var tagBodyinfoList = glTagBodyInfo[glTagetLocatorID];

    if (bodyinfoList == undefined || glTagetTagID === '')
    {
        weight_chart.setAttribute("style",'display:none');
        heartbeat_chart.setAttribute("style",'display:none');

        if (once == 0)
            setTimeout(show_bodyinfo_records, 1000, 0);
        return;
    }

    if (bodyinfoList.length == 1 )
    {
        weight_chart.setAttribute("style",'display:none');
        heartbeat_chart.setAttribute("style",'display:none');

        if (once == 0)
            setTimeout(show_bodyinfo_records, 1000, 0);
        return;
    }

    //
    // find tags
    //
    var curTagBodyinfo = null;
    for(var indexTag=0;indexTag<tagBodyinfoList.length;indexTag++)
    {
        var tagBodyinfo = tagBodyinfoList[indexTag];
        if (tagBodyinfo.nodeid === glTagetTagID)
        {
            curTagBodyinfo = tagBodyinfo;
            break;
        }
    }

    if ( curTagBodyinfo == null )
    {
        if (once == 0)
            setTimeout(show_bodyinfo_records, 1000, 0);
        return;
    }

    var weight_chart_title = document.getElementById("weight_chart_title");
    var heartbeat_chart_title = document.getElementById("heartbeat_chart_title");
    weight_chart_title.textContent = curTagBodyinfo.nodename + ' 體重紀錄';
    heartbeat_chart_title.textContent = curTagBodyinfo.nodename + ' 心率紀錄';

    var weightinfoList = bodyinfoList[0];
    var heartbeatinfoList = bodyinfoList[1];

    // console.log("weightinfoList.length = "+ weightinfoList.weight.length);
    // console.log("heartbeatinfoList.length = "+ heartbeatinfoList.heartbeat.length);

    var weight_min = 200;
    var weight_max = 0;
    var weightitemlist = [];
    //for(var index=0;index<weightinfoList.weight.length;index++)
    for(var index=weightinfoList.weight.length - 1;index>=0;index--)
    {
        weightinfo = weightinfoList.weight[index];
        var info = [];
        info['item1'] = parseFloat(weightinfo.weight).toFixed(2);
        info['y'] = weightinfo.datetime;
        weightitemlist.push(info);

        if (weight_min > info['item1'] )
            weight_min = info['item1'];
        if (weight_max < info['item1'] )
            weight_max = info['item1'];
    }
    glWeightLineChart.options["ymax"] = parseInt(weight_max) + 2;
    glWeightLineChart.options["ymin"] = parseInt(weight_min) - 2;
    glWeightLineChart.setData(weightitemlist);

    //console.log("weight_max:"+weight_max + " weight_min:"+weight_min);

    var heartbeat_min = 200;
    var heartbeat_max = 0;
    var heartbeatitemlist = [];
    //for(var index=0;index<heartbeatinfoList.heartbeat.length;index++)
    for(var index=heartbeatinfoList.heartbeat.length - 1;index>=0;index--)
    {
        heartbeatinfo = heartbeatinfoList.heartbeat[index];
        var info = [];
        info['item1'] = parseInt(heartbeatinfo.heartbeat);
        info['y'] = heartbeatinfo.datetime;
        heartbeatitemlist.push(info);

        if (heartbeat_min > info['item1'] )
            heartbeat_min = info['item1'];
        if (heartbeat_max < info['item1'] )
            heartbeat_max = info['item1'];
    }
    glHeartBeatLineChart.options["ymax"] = parseInt(heartbeat_max) + 2;
    glHeartBeatLineChart.options["ymin"] = parseInt(heartbeat_min) - 2;
    glHeartBeatLineChart.setData(heartbeatitemlist);

    //console.log("heartbeat_max:"+heartbeat_max + " heartbeat_min:"+heartbeat_min);

    weight_chart.setAttribute("style",'display:block');
    heartbeat_chart.setAttribute("style",'display:block');

    glWeightLineChart.redraw();
    glHeartBeatLineChart.redraw();

    if ( weightinfoList.weight.length >= 1 )
        show_weight_diff('1', curTagBodyinfo.weight, weightinfoList.weight[0]);
    if ( weightinfoList.weight.length >= 2 )
        show_weight_diff('2', curTagBodyinfo.weight, weightinfoList.weight[1]);
    if ( weightinfoList.weight.length >= 3 )
        show_weight_diff('3', curTagBodyinfo.weight, weightinfoList.weight[2]);

    if ( heartbeatinfoList.heartbeat.length >= 1 )
        show_heartbeat_diff('1', curTagBodyinfo.heartbeat, heartbeatinfoList.heartbeat[0]);
    if ( heartbeatinfoList.heartbeat.length >= 2 )
        show_heartbeat_diff('2', curTagBodyinfo.heartbeat, heartbeatinfoList.heartbeat[1]);
    if ( heartbeatinfoList.heartbeat.length >= 3 )
        show_heartbeat_diff('3', curTagBodyinfo.heartbeat, heartbeatinfoList.heartbeat[2]);

    // jQueryKnob
    $('.knob').knob();

    if (once == 0)
        setTimeout(show_bodyinfo_records, 10000, 0);
}

function show_weight_diff(uiTag, cur_weight, weightinfo)
{
    // console.log("show_weight_diff() "+uiTag+" cur_weight:"+cur_weight+" weightinfo.weight:"+weightinfo.weight+" datetime:"+weightinfo.datetime);

    var knob_ui = document.getElementById("weight_knob_"+uiTag);
    var label_ui= document.getElementById("weight_label_"+uiTag);

    knob_ui.setAttribute("value", weightinfo.weight);
    $("#weight_knob_"+uiTag).val(weightinfo.weight);

    var diffWeight = parseFloat(cur_weight).toFixed(2) - parseFloat(weightinfo.weight).toFixed(2);

    if (diffWeight > 0)
        knob_ui.setAttribute("data-fgColor", "#CC1212");
    else
        knob_ui.setAttribute("data-fgColor", "#39CCCC");

    var target_date = moment(weightinfo.datetime, "YYYY-MM-DD HH:mm:ss");
    var diffDays = moment().diff(target_date, 'days');
    var diffHours = moment().diff(target_date, 'hours');
    var diffMinutes = moment().diff(target_date, 'minutes');
    var diffSeconds = moment().diff(target_date, 'seconds');

    if (diffDays > 0)
    {
        label_ui.textContent = diffDays+' 天前';
    }
    else
    if (diffHours > 0)
    {
        label_ui.textContent = diffHours+' 小時前';
    }
    else
    if (diffMinutes > 0)
    {
        label_ui.textContent = diffMinutes+' 分鐘前';
    }
    else
    if (diffSeconds > 0)
    {
        label_ui.textContent = diffSeconds+' 秒前';
    }
    else
    {
        label_ui.textContent = weightinfo.datetime;
    }

}

function show_heartbeat_diff(uiTag, cur_heartbeat, heartbeatinfo)
{
    var knob_ui = document.getElementById("heartbeat_knob_"+uiTag);
    var label_ui = document.getElementById("heartbeat_label_"+uiTag);

    knob_ui.setAttribute("value", heartbeatinfo.heartbeat);
    $("#heartbeat_knob_"+uiTag).val(heartbeatinfo.heartbeat);

    var diffHeartbeat = parseInt(cur_heartbeat) - parseInt(heartbeatinfo.heartbeat);

    // console.log("diffHeartbeat:"+diffHeartbeat + " cur_heartbeat:"+cur_heartbeat+ " heartbeatinfo.heartbeat:"+heartbeatinfo.heartbeat);

    if (diffHeartbeat > 0)
        knob_ui.setAttribute("data-fgColor", "#CC1212");
    else
        knob_ui.setAttribute("data-fgColor", "#39CCCC");

    var target_date = moment(heartbeatinfo.datetime, "YYYY-MM-DD HH:mm:ss");
    var diffDays = moment().diff(target_date, 'days');
    var diffHours = moment().diff(target_date, 'hours');
    var diffMinutes = moment().diff(target_date, 'minutes');
    var diffSeconds = moment().diff(target_date, 'seconds');

    if (diffDays > 0)
    {
        label_ui.textContent = diffDays+' 天前';
    }
    else
    if (diffHours > 0)
    {
        label_ui.textContent = diffHours+' 小時前';
    }
    else
    if (diffMinutes > 0)
    {
        label_ui.textContent = diffMinutes+' 分鐘前';
    }
    else
    if (diffSeconds > 0)
    {
        label_ui.textContent = diffSeconds+' 秒前';
    }
    else
    {
        label_ui.textContent = heartbeatinfo.datetime;
    }

}

function create_body_info_block(bodyinfo, bShowName)
{
    var row = create_new_row('NODE_BODYINFO_TABLE');

    var nodename = 'No TAG';
    if (bShowName == true)
        nodename = bodyinfo.nodename;

    if( bodyinfo.heartbeat == '-1' && bodyinfo.weight == '-0.01' )
    {
        return;
    }

    if (bodyinfo.heartbeat == '0' || bodyinfo.heartbeat == '-1')
    {
        var block = create_heartbeat_block(0, bodyinfo.nodeid, nodename, '0', bodyinfo.heartbeatTime);
        row.appendChild(block);
    }
    else
    {
        var block = create_heartbeat_block(1, bodyinfo.nodeid, nodename, bodyinfo.heartbeat, bodyinfo.heartbeatTime);
        row.appendChild(block);
    }

    if (bodyinfo.weight == '0' || bodyinfo.weight == '-0.01')
    {
        var block = create_weight_block(0, bodyinfo.nodeid, nodename, '0', bodyinfo.weightTime);
        row.appendChild(block);
    }
    else
    {
        var block = create_weight_block(1, bodyinfo.nodeid, nodename, bodyinfo.weight, bodyinfo.weightTime);
        row.appendChild(block);
    }
}

function clear_table(tableid)
{
    var bodyinfo_table = document.getElementById(tableid);

    while (bodyinfo_table.firstChild)
    {
        bodyinfo_table.removeChild(bodyinfo_table.firstChild);
    }
}

function create_new_row(tableid)
{
    var row = document.createElement('div');
    row.setAttribute("class","row");

    var bodyinfo_table = document.getElementById(tableid);
    bodyinfo_table.appendChild(row);
    return row;
}

function create_heartbeat_block(active, nodeid, nodename, heartbeat, heartbeatTime)
{
    var col = document.createElement('div');
    col.setAttribute("class","col-lg-3 col-xs-4");

    var small_box = document.createElement('div');
    if (active == 0)
        small_box.setAttribute("class","small-box bg-gray");
    else
        small_box.setAttribute("class","small-box bg-green");
    col.appendChild(small_box);

    var small_box_inner = document.createElement('div');
    small_box_inner.setAttribute("class","inner");
    small_box.appendChild(small_box_inner);

    var small_box_heartbeat = document.createElement('h3');
    small_box_heartbeat.textContent = heartbeat;
    small_box_inner.appendChild(small_box_heartbeat);

    var small_box_nodename = document.createElement('p');
    small_box_nodename.textContent = nodename;
    small_box_inner.appendChild(small_box_nodename);

    var small_box_heartbeatTime = document.createElement('h6');
    small_box_heartbeatTime.textContent = heartbeatTime;
    small_box_inner.appendChild(small_box_heartbeatTime);

    var small_box_icon = document.createElement('div');
    small_box_icon.setAttribute("class","icon");
    small_box.appendChild(small_box_icon);

    var small_box_icon_img = document.createElement('i');
    small_box_icon_img.setAttribute("class","ion ion-ios-heart-outline");
    small_box_icon.appendChild(small_box_icon_img);

    if (active == 1)
    {
        var small_box_save = document.createElement('a');
        small_box_save.setAttribute("class","small-box-footer");
        small_box_save.setAttribute("onclick","save_bodyinfo_heartbeat_dialog(\""+ nodeid +"\",\""+ nodename +"\",\""+ heartbeat +"\");return false;");
        small_box_save.textContent = '儲存紀錄';
        small_box.appendChild(small_box_save);

        var small_box_save_icon = document.createElement('i');
        small_box_save_icon.setAttribute("class","fa fa-cloud-download");
        small_box_save.appendChild(small_box_save_icon);
    }

    return col;
}

function create_weight_block(active, nodeid, nodename, weight, weightTime)
{
    var col = document.createElement('div');
    col.setAttribute("class","col-lg-3 col-xs-4");

    var small_box = document.createElement('div');
    if (active == 0)
        small_box.setAttribute("class","small-box bg-gray");
    else
        small_box.setAttribute("class","small-box bg-yellow");
    col.appendChild(small_box);

    var small_box_inner = document.createElement('div');
    small_box_inner.setAttribute("class","inner");
    small_box.appendChild(small_box_inner);

    var small_box_weight = document.createElement('h3');
    small_box_weight.textContent = weight;
    small_box_inner.appendChild(small_box_weight);

    var small_box_nodename = document.createElement('p');
    small_box_nodename.textContent = nodename;
    small_box_inner.appendChild(small_box_nodename);

    var small_box_weightTime = document.createElement('h6');
    small_box_weightTime.textContent = weightTime;
    small_box_inner.appendChild(small_box_weightTime);

    var small_box_icon = document.createElement('div');
    small_box_icon.setAttribute("class","icon");
    small_box.appendChild(small_box_icon);

    var small_box_icon_img = document.createElement('i');
    small_box_icon_img.setAttribute("class","ion ion-ios-download-outline");
    small_box_icon.appendChild(small_box_icon_img);

    if (active == 1)
    {
        var small_box_save = document.createElement('a');
        small_box_save.setAttribute("class","small-box-footer");
        small_box_save.setAttribute("href","#");
        small_box_save.setAttribute("onclick","save_bodyinfo_weight_dialog(\""+ nodeid +"\",\""+ nodename +"\",\""+ weight +"\");return false;");
        small_box_save.textContent = '儲存紀錄';
        small_box.appendChild(small_box_save);

        var small_box_save_icon = document.createElement('i');
        small_box_save_icon.setAttribute("class","fa fa-cloud-download");
        small_box_save.appendChild(small_box_save_icon);
    }

    return col;
}

function create_locator_heartbeat_block(nodenid, nodename, heartbeat, heartbeatTime)
{
    var col = document.createElement('div');
    col.setAttribute("class","col-lg-3 col-xs-4");

    var small_box = document.createElement('div');
    small_box.setAttribute("class","small-box bg-green");
    col.appendChild(small_box);

    var small_box_inner = document.createElement('div');
    small_box_inner.setAttribute("class","inner");
    small_box.appendChild(small_box_inner);

    var small_box_heartbeat = document.createElement('h3');
    small_box_heartbeat.textContent = heartbeat;
    small_box_inner.appendChild(small_box_heartbeat);

    var small_box_nodename = document.createElement('p');
    small_box_nodename.textContent = nodename;
    small_box_inner.appendChild(small_box_nodename);

    var small_box_heartbeatTime = document.createElement('h6');
    small_box_heartbeatTime.textContent = heartbeatTime;
    small_box_inner.appendChild(small_box_heartbeatTime);

    var small_box_icon = document.createElement('div');
    small_box_icon.setAttribute("class","icon");
    small_box.appendChild(small_box_icon);

    var small_box_icon_img = document.createElement('i');
    small_box_icon_img.setAttribute("class","ion ion-ios-heart-outline");
    small_box_icon.appendChild(small_box_icon_img);

    var small_box_save = document.createElement('a');
    small_box_save.setAttribute("class","small-box-footer");
    small_box_save.setAttribute("href","#");
    small_box_save.textContent = 'AB Box';
    small_box.appendChild(small_box_save);


    return col;
}

function create_locator_weight_block(nodenid, nodename, weight, weightTime)
{
    var col = document.createElement('div');
    col.setAttribute("class","col-lg-3 col-xs-4");

    var small_box = document.createElement('div');
    small_box.setAttribute("class","small-box bg-yellow");
    col.appendChild(small_box);

    var small_box_inner = document.createElement('div');
    small_box_inner.setAttribute("class","inner");
    small_box.appendChild(small_box_inner);

    var small_box_weight = document.createElement('h3');
    small_box_weight.textContent = weight;
    small_box_inner.appendChild(small_box_weight);

    var small_box_nodename = document.createElement('p');
    small_box_nodename.textContent = nodename;
    small_box_inner.appendChild(small_box_nodename);

    var small_box_weightTime = document.createElement('h6');
    small_box_weightTime.textContent = weightTime;
    small_box_inner.appendChild(small_box_weightTime);

    var small_box_icon = document.createElement('div');
    small_box_icon.setAttribute("class","icon");
    small_box.appendChild(small_box_icon);

    var small_box_icon_img = document.createElement('i');
    small_box_icon_img.setAttribute("class","ion ion-ios-download-outline");
    small_box_icon.appendChild(small_box_icon_img);

    var small_box_save = document.createElement('a');
    small_box_save.setAttribute("class","small-box-footer");
    small_box_save.setAttribute("href","#");
    small_box_save.textContent = 'AB Box';
    small_box.appendChild(small_box_save);

    return col;
}

function click_abbox(click_node_nodeid)
{
    glTagetLocatorID = click_node_nodeid;
}

function save_bodyinfo_weight_dialog(nodeid, nodename, weight)
{
    gl_Current_info_type = 'weight';
    gl_Current_info = weight;
    $('#opensavebodyinfomodqlbtn').click();

    document.getElementById('bodyinfocontentNameId').innerHTML = '體重資訊:';
    document.getElementById('bodyinfocontent').value = weight;

    if (nodename === 'No TAG')
    {
    }
    else
    {
        gl_target_tag_id = nodeid;
        gl_target_tag_name = nodename;
        $("#target_tag").html(gl_target_tag_name);
    }
    // if (nodename === 'No TAG')
    // {
    //     gl_Current_info_type = 'weight';
    //     gl_Current_info = weight;
    //     $('#opensavebodyinfomodqlbtn').click();
    //
    //     document.getElementById('bodyinfocontent').value = '體重資訊:'+weight;
    // }
    // else
    // {
    //     if (confirm('是否確認儲存體重資訊? 裝置:'+nodename+' 體重:' + weight))
    //     {
    //         save_bodyinfo_weight(nodeid, nodename, weight);
    //     }
    //     else
    //     {
    //         // Do nothing!
    //     }
    // }
}

function save_bodyinfo_heartbeat_dialog(nodeid, nodename, heartbeat)
{
    gl_Current_info_type = 'heartbeat';
    gl_Current_info = heartbeat;
    $('#opensavebodyinfomodqlbtn').click();

    document.getElementById('bodyinfocontentNameId').innerHTML = '心率資訊:';
    document.getElementById('bodyinfocontent').value = heartbeat;
    if (nodename === 'No TAG')
    {
    }
    else
    {
        gl_target_tag_id = nodeid;
        gl_target_tag_name = nodename;
        $("#target_tag").html(gl_target_tag_name);
    }

    // if (nodename === 'No TAG')
    // {
    //     gl_Current_info_type = 'heartbeat';
    //     gl_Current_info = heartbeat;
    //     $('#opensavebodyinfomodqlbtn').click();
    //
    //     document.getElementById('bodyinfocontent').value = '心率資訊:'+heartbeat;
    // }
    // else
    // {
    //     if (confirm('是否確認儲存心率資訊? 裝置:'+nodename+' 心率:' + heartbeat))
    //     {
    //         save_bodyinfo_heartbeat(nodeid, nodename, heartbeat);
    //     }
    //     else
    //     {
    //         // Do nothing!
    //     }
    // }
}

$("#saveinfobtn").click(function()
{
    if ( gl_target_tag_id.length == 0 )
    {
        alert("請選擇標籤!");
        return;
    }

    document.getElementById("saveinfobtn").disabled = true;
    document.getElementById("saveinfo_sending_callout").style.display = 'block';
    document.getElementById("saveinfo_failed_callout").style.display = 'none';
    document.getElementById("saveinfo_finished_callout").style.display = 'none';

    gl_Current_info = document.getElementById('bodyinfocontent').value;

    if ( gl_Current_info_type === 'heartbeat' )
    {
        save_bodyinfo_heartbeat(gl_target_tag_id, gl_target_tag_name, gl_Current_info);
    }
    else
    if ( gl_Current_info_type === 'weight' )
    {
        save_bodyinfo_weight(gl_target_tag_id, gl_target_tag_name, gl_Current_info);
    }
});

function save_bodyinfo_weight(nodeid, nodename, weight)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['SAVE_BODYINFO_WEIGHT'] = 1;
    jsondata['nodeid'] = nodeid;
    jsondata['weight'] = weight;

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
                document.getElementById("saveinfo_sending_callout").style.display = 'none';
                document.getElementById("saveinfo_failed_callout").style.display = 'block';
                document.getElementById("saveinfo_finished_callout").style.display = 'none';

                setTimeout(close_callout, 5000, "saveinfo_failed_callout");
            }
            else
            {
                document.getElementById("saveinfo_sending_callout").style.display = 'none';
                document.getElementById("saveinfo_failed_callout").style.display = 'none';
                document.getElementById("saveinfo_finished_callout").style.display = 'block';

                setTimeout(close_callout, 5000, "saveinfo_finished_callout");
            }
            document.getElementById("saveinfobtn").disabled = false;
            setTimeout(load_tag_bodyinfo_records, 1000, 1);
            setTimeout(show_bodyinfo_records, 2000, 1);

        },
    error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);

            document.getElementById("saveinfo_sending_callout").style.display = 'none';
            document.getElementById("saveinfo_failed_callout").style.display = 'block';
            document.getElementById("saveinfo_finished_callout").style.display = 'none';

            setTimeout(close_callout, 5000, "saveinfo_failed_callout");
            document.getElementById("saveinfobtn").disabled = false;

        }
    });

    return false;
}

function close_callout(ui_id)
{
    document.getElementById(ui_id).style.display = 'none';
}

function save_bodyinfo_heartbeat(nodeid, nodename, heartbeat)
{
    var jsondata = {};
    jsondata['project_id'] = ''+project_id;
    jsondata['SAVE_BODYINFO_HEARTBEAT'] = 1;
    jsondata['nodeid'] = nodeid;
    jsondata['heartbeat'] = heartbeat;

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
                document.getElementById("saveinfo_sending_callout").style.display = 'none';
                document.getElementById("saveinfo_failed_callout").style.display = 'block';
                document.getElementById("saveinfo_finished_callout").style.display = 'none';

                setTimeout(close_callout, 5000, "saveinfo_failed_callout");
            }
            else
            {
                document.getElementById("saveinfo_sending_callout").style.display = 'none';
                document.getElementById("saveinfo_failed_callout").style.display = 'none';
                document.getElementById("saveinfo_finished_callout").style.display = 'block';

                setTimeout(close_callout, 5000, "saveinfo_finished_callout");
            }
            document.getElementById("saveinfobtn").disabled = false;
            setTimeout(load_tag_bodyinfo_records, 1000, 1);
            setTimeout(show_bodyinfo_records, 2000, 1);

        },
    error:
        function(xhr, status, error)
        {
            alert("error = " + error);
            alert("xhr.responseText = " + xhr.responseText);

            document.getElementById("saveinfo_sending_callout").style.display = 'none';
            document.getElementById("saveinfo_failed_callout").style.display = 'block';
            document.getElementById("saveinfo_finished_callout").style.display = 'none';

            setTimeout(close_callout, 5000, "saveinfo_failed_callout");
            document.getElementById("saveinfobtn").disabled = false;
        }
    });

    return false;
}

glWeightLineChart = new Morris.Line({
  element          : 'weight_line_chart',
  resize           : true,
  redraw           : true,
  data             : [
    { y: '', item1: 0 },
    // { y: '2016-11-06 16:36:16', item1: 64 },
    // { y: '2016-10-31 16:36:16', item1: 66 }
  ],
  xkey             : 'y',
  ykeys            : ['item1'],
  labels           : ['體重'],
  lineColors       : ['#223377'],
  lineWidth        : 2,
  parseTime        : false,
  hideHover        : 'auto',
  pointSize        : 4,
  pointStrokeColors: ['#223377'],
  gridTextColor    : '#222',
  gridStrokeWidth  : 0.4,
  gridIntegers     : true,
  gridLineColor    : '#efefef',
  gridTextFamily   : 'Open Sans',
  gridTextSize     : 10
});

glHeartBeatLineChart = new Morris.Line({
  element          : 'heartbeat_line_chart',
  resize           : true,
  redraw           : true,
  data             : [
    { y: '', item1: 0 },
    // { y: '2016-11-06 16:36:16', item1: 64 },
    // { y: '2016-10-31 16:36:16', item1: 66 }
  ],
  xkey             : 'y',
  ykeys            : ['item1'],
  labels           : ['心率'],
  lineColors       : ['#227744'],
  lineWidth        : 2,
  parseTime        : false,
  hideHover        : 'auto',
  pointSize        : 4,
  pointStrokeColors: ['#227744'],
  gridTextColor    : '#222',
  gridStrokeWidth  : 0.4,
  gridIntegers     : true,
  gridLineColor    : '#efefef',
  gridTextFamily   : 'Open Sans',
  gridTextSize     : 10
});

function open_bodyinfo_page()
{
    var win = window.open('/index.php/vilsbodyinfo/'+project_id, '_blank');
    win.focus();
}
