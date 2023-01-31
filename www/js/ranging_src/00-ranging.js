
/*
 * Author: ikki Chung
 * Date: 2019.06.11
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

//
// globa value
//
var glAllTags = [];
var glAllAnchors = [];// put anchor info
var glAllTagRanging = {};

var gl_tag_num = 0;
var gl_locator_num = 0;
var gl_anchor_num = 0;

var gl_tag_load_index = 0;
var gl_locator_load_index = 0;
var gl_anchor_load_index = 0;

var glTag_List = [];
var glLocator_List = [];
var glAnchor_List = [];

var glCurrentLoadingState = 0;// 0:play  1:pause
var MAX_RANGING_RESULT = 20;
var LOAD_DEVICE_NUM = 20;

load_devices();

function load_devices()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_locator_num = parseInt(data['LOC_LOCATOR_NUM']);
        gl_anchor_num = parseInt(data['LOC_ANCHOR_NUM']);

        gl_tag_load_index = 0;
        gl_locator_load_index = 0;
        gl_anchor_load_index = 0;

        glTag_List = [];
        glLocator_List = [];
        glAnchor_List = [];
    })
    .success(function() {
        setTimeout(load_tag, 10);
    })
    .error(function() {
    })
    .complete(function() {
    });
}


function load_tag()
{
    if (gl_tag_load_index >= gl_tag_num)
    {
        gl_tag_load_index = 0;
        setTimeout(load_locator, 20);
        return;
    }
    if (gl_tag_load_index == 0)
    {
        glTag_List = [];
    }

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadtag=1&s='+gl_tag_load_index+'&count='+LOAD_DEVICE_NUM+'&project_id='+project_id, function(data)
    {
        var tag_num = parseInt(data['LOC_TAG_NUM']);

        console.log("tag_num:"+tag_num +", gl_tag_load_index:"+gl_tag_load_index);

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
            setTimeout(load_locator, 20);
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
}


function load_locator()
{
    if (gl_locator_load_index >= gl_locator_num)
    {
        gl_locator_load_index = 0;
        setTimeout(load_anchor, 10);
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

        console.log("locator_num:"+locator_num +", gl_locator_load_index:"+gl_locator_load_index);

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
            setTimeout(load_anchor, 10);
        }
        else
        {
            setTimeout(load_locator, 1);
        }
    })
    .error(function() {
        gl_locator_load_index = 0;
        setTimeout(load_locator, UPDATE_INTERVAL);
    })
    .complete(function() {
    });
}


function load_anchor()
{
    if (gl_anchor_load_index >= gl_anchor_num)
    {
        gl_anchor_load_index = 0;
        setTimeout(load_node_ranging, 10);
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

        //console.log("load_anchors() anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_anchor_load_index; i < gl_anchor_load_index+anchor_num; ++i)
        {
            var key = 'LOC_ANCHOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            //console.log("load_anchors() key:"+key +", data[key]:"+data[key]);

            // split data
            var node = data[key];
            var nodeData = node.split(",");
            glLocator_List.push(nodeData);
        }

        gl_anchor_load_index += anchor_num;

    })
    .success(function() {

        //console.log("load_anchors() success gl_anchor_load_index:"+gl_anchor_load_index +", gl_anchor_num:"+gl_anchor_num);

        if (gl_anchor_load_index >= gl_anchor_num)
        {
            setTimeout(load_node_ranging, 10);
        }
        else
        {
            setTimeout(load_anchor, 10);
        }
    })
    .error(function() {

        console.log("load_anchors() error gl_anchor_load_index:"+gl_anchor_load_index +", gl_anchor_num:"+gl_anchor_num);

        gl_anchor_load_index = 0;
        setTimeout(load_locator, 10);
    })
    .complete(function() {
    });
}

function load_node_ranging()
{
    if ( document.getElementById("RANGING_TABLE_TABS") == null )
        return;

    //var targetURL = "https://" + window.location.hostname + "/cgi-bin/vilsnodes";
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loadranging=1'+'&project_id='+project_id, function(data)
    {
        var showMsg = "";

        $.each(data, function(tagkey, value)
        {
            var existed = checkTagExist(tagkey);
            if (existed)
            {
                var found = false;
                for (var i = 0; i < glAllTags.length; i++)
                {
                    if ( glAllTags[i] == tagkey )
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    glAllTags.push(tagkey);

                if (glAllTagRanging[tagkey] == undefined)
                    glAllTagRanging[tagkey] = [];
                var curTagRanging = glAllTagRanging[tagkey];

                //var distance = {};
                //var timestamp = {};

                var rangings = JSON.parse(value);
                $.each(rangings, function(idx, value)
                {
                    // check exist anchor

                    var datakeys = Object.keys(value);
                    //console.log("load_node_ranging() tagkey:"+tagkey+" idx:"+idx + " datakeys:"+datakeys);

                    var ranging = "";
                    var chrAnchor = "";
                    var chrAngle = "";
                    var chrTime = "";
                    for(key in datakeys)
                    {
                        curKey = datakeys[key];

                        if( curKey === 'time' )
                        {
                            var timelist = value[curKey].split(" ");
                            if( timelist.length > 1 )
                                chrTime = timelist[1];
                            else
                                chrTime = value[curKey];
                        }
                        else
                        if( curKey === 'a' )
                        {
                            chrAngle = value[curKey];
                        }
                        else
                        {
                            chrAnchor = curKey;
                            ranging = value[curKey];
                        }
                    }

                    var anchorExisted = checkAnchorExist(chrAnchor);
                    if (anchorExisted)
                    {
                        var found = false;
                        for (var i = 0; i < glAllAnchors.length; i++)
                        {
                            if ( glAllAnchors[i] == chrAnchor )
                            {
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                            glAllAnchors.push(chrAnchor);

                        //
                        // find current data index
                        //
                        if (curTagRanging[chrAnchor] == undefined)
                        {
                            curTagRanging[chrAnchor] = {};
                            curTagRanging[chrAnchor].index = MAX_RANGING_RESULT;
                            curTagRanging[chrAnchor].data = [];
                            curTagRanging[chrAnchor].count = 0;
                        }
                        var currentRangingIndex = curTagRanging[chrAnchor].index;
                        currentRangingIndex++;
                        if (currentRangingIndex >= MAX_RANGING_RESULT)
                        {
                            currentRangingIndex = 0;
                        }
                        curTagRanging[chrAnchor].index = currentRangingIndex;
                        if (curTagRanging[chrAnchor].data[currentRangingIndex] == undefined)
                        {
                            curTagRanging[chrAnchor].data[currentRangingIndex] = {};
                        }

                        curTagRanging[chrAnchor].data[currentRangingIndex].ranging = ranging;
                        curTagRanging[chrAnchor].data[currentRangingIndex].angle = chrAngle;
                        curTagRanging[chrAnchor].data[currentRangingIndex].timestamp = chrTime;
                        curTagRanging[chrAnchor].count++;
                    }

                    //console.log("load_node_ranging() tagkey:"+tagkey+ " chrAnchor:"+chrAnchor + " currentRangingIndex:"+currentRangingIndex+ " ranging:"+ranging +" chrTime:"+chrTime);
                });
            }
        });

        if (glCurrentLoadingState == 0)
            create_ranging_table();
    })
    .success(function() {
        setTimeout(load_node_ranging, 500);
    })
    .error(function() {
        setTimeout(load_node_ranging, 3000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}


//
// create ranging table
//
function create_ranging_table()
{
    if ( (glAllTags.length + glAllAnchors.length) == 0 )
    {
        document.getElementById("RANGING_TABLE_MSG").innerText = "No new Record";
        return;
    }
    else
        document.getElementById("RANGING_TABLE_MSG").innerText = "";

    var ranging_table_tabs = document.getElementById('RANGING_TABLE_TABS');
    var ranging_table_content = document.getElementById('RANGING_TABLE_CONTENT');


    //
    // create nav-tabs
    //
    for (var j = 0; j < glAllTags.length; j++)
    {
        var tagkey = glAllTags[j];
        create_tab(j, ranging_table_tabs, tagkey);
    }

    //
    // create tab-pane
    //
    for (var j = 0; j < glAllTags.length; j++)
    {
        var tagkey = glAllTags[j];
        create_tab_pane(j, ranging_table_content, tagkey);
    }
}

function create_tab(index, ranging_table_tabs, tagkey)
{
    var tab_tag = document.getElementById('tab_' + tagkey);
    if (tab_tag == null)
    {
        var tab_li = document.createElement('li');
        if (index == 0)
            tab_li.setAttribute("class","active");
        tab_li.setAttribute("id",'tab_' + tagkey);
        ranging_table_tabs.appendChild(tab_li);
        var tab_li_a = document.createElement('a');
        tab_li_a.setAttribute("href",'#tab_tag_' + tagkey);
        tab_li_a.setAttribute("data-toggle","tab");
        tab_li.appendChild(tab_li_a);

        // tag name
        var tab_li_a_span = document.createElement('span');
        tab_li_a_span.setAttribute("class","badge bg-blue-active");
        tab_li_a_span.textContent = getTagName(tagkey);
        tab_li_a.appendChild(tab_li_a_span);

        // tag nodeid
        // var tab_li_a_span = document.createElement('span');
        // tab_li_a_span.setAttribute("class","badge bg-blue-active");
        // tab_li_a_span.textContent = tagkey;
        // tab_li_a.appendChild(tab_li_a_span);

    }
    return tab_tag;
}

function create_tab_pane(index, ranging_table_content, tagkey)
{
    var tab_tag_pan = document.getElementById('tab_tag_' + tagkey);
    if (tab_tag_pan == null)
    {
        var tab_tag_pan = document.createElement('div');
        if (index == 0)
            tab_tag_pan.setAttribute("class",'tab-pane active');
        else
            tab_tag_pan.setAttribute("class",'tab-pane');
        tab_tag_pan.setAttribute("id",'tab_tag_' + tagkey);
        ranging_table_content.appendChild(tab_tag_pan);

        var row_div = document.createElement('div');
        row_div.setAttribute("class",'row');
        row_div.setAttribute("id",'tab_row_tag_' + tagkey);
        tab_tag_pan.appendChild(row_div);
    }

    var tab_row = document.getElementById('tab_row_tag_' + tagkey);
    if (tab_tag_pan == null)
    {
        return;
    }

    //
    // valid anchors
    //
    var curTagRanging = glAllTagRanging[tagkey];
    var anchors = Object.keys(curTagRanging);
    for(idx in anchors)
    {
        chrAnchor = anchors[idx];

        var ui_key = 'tab_col_' + tagkey +'_'+chrAnchor;

        var tab_col_table_content = document.getElementById(ui_key);
        if (tab_col_table_content == null)
        {
            var tab_col_1 = document.createElement('div');
            tab_col_1.setAttribute("class",'col-xs-4 col-sm-3 col-md-2 col-lg-2');
            tab_row.appendChild(tab_col_1);

            var tab_col_box = document.createElement('div');
            tab_col_box.setAttribute("class",'box');
            tab_col_1.appendChild(tab_col_box);

            var tab_col_box_header = document.createElement('div');
            tab_col_box_header.setAttribute("class",'box-header with-border');
            tab_col_box.appendChild(tab_col_box_header);

            // anchor name
            var tab_col_box_header_anchor_span = document.createElement('span');
            tab_col_box_header_anchor_span.setAttribute("class",'badge bg-green');
            tab_col_box_header_anchor_span.textContent = getAnchorName(chrAnchor);
            tab_col_box_header.appendChild(tab_col_box_header_anchor_span);

            // anchor nodeid
            // var tab_col_box_header_anchor_span = document.createElement('span');
            // tab_col_box_header_anchor_span.setAttribute("class",'badge bg-green');
            // tab_col_box_header_anchor_span.textContent = chrAnchor;
            // tab_col_box_header.appendChild(tab_col_box_header_anchor_span);

            // data count
            var tab_col_box_header_count_span = document.createElement('span');
            tab_col_box_header_count_span.setAttribute("class",'badge bg-purple');
            tab_col_box_header_count_span.setAttribute("id",'box_header_count_' + ui_key);
            tab_col_box_header_count_span.textContent = 0;
            tab_col_box_header.appendChild(tab_col_box_header_count_span);


            var tab_col_box_body = document.createElement('div');
            tab_col_box_body.setAttribute("class",'box-body');
            tab_col_box.appendChild(tab_col_box_body);

            var tab_col_table = document.createElement('table');
            tab_col_table.setAttribute("class",'table');
            tab_col_box_body.appendChild(tab_col_table);

            var tab_col_table_tr = document.createElement('tr');
            tab_col_table.appendChild(tab_col_table_tr);

            var tab_col_table_th_time = document.createElement('th');
            //tab_col_table_th_time.setAttribute("bgcolor","#dd2233");
            //tab_col_table_th_time.setAttribute("style",'width:10px');
            tab_col_table_tr.appendChild(tab_col_table_th_time);

            var tab_col_table_th_time_span = document.createElement('span');
            tab_col_table_th_time_span.setAttribute("class",'badge bg-blue');
            tab_col_table_th_time_span.textContent = 'Timestamp';
            tab_col_table_th_time.appendChild(tab_col_table_th_time_span);

            // var tab_col_table_th_anchor = document.createElement('th');
            // tab_col_table_tr.appendChild(tab_col_table_th_anchor);
            //
            // var tab_col_table_th_anchor_span = document.createElement('span');
            // tab_col_table_th_anchor_span.setAttribute("class",'badge bg-green');
            // tab_col_table_th_anchor_span.textContent = chrAnchor;
            // tab_col_table_th_anchor.appendChild(tab_col_table_th_anchor_span);

            var tab_col_table_content = document.createElement('div');
            tab_col_table_content.setAttribute("id",ui_key);
            tab_col_table.appendChild(tab_col_table_content);
        }

        tab_col_table_content = document.getElementById(ui_key);
        tab_col_box_header_count_span = document.getElementById('box_header_count_' + ui_key);
        tab_col_box_header_count_span.textContent = curTagRanging[chrAnchor].count;

        // clear olddata
        while (tab_col_table_content.firstChild)
        {
            tab_col_table_content.removeChild(tab_col_table_content.firstChild);
        }

        var currentRangingIndex = curTagRanging[chrAnchor].index;
        for (var i = 0; i < curTagRanging[chrAnchor].data.length; i++)
        {
            var curData = curTagRanging[chrAnchor].data[i];

            var offset = (currentRangingIndex - i);
            if(offset < 0)
                offset += 20;
            var opacityvalue = 1 - offset * 0.04;

            var row = document.createElement('tr');
            tab_col_table_content.appendChild(row);

            var tdTime = document.createElement('td');
            if ( i == currentRangingIndex)
                tdTime.setAttribute("bgcolor","#00cccc");
            row.appendChild(tdTime);

            var target_date = moment(curData.timestamp, "HH:mm:ss");
            //var diffDays = moment().diff(target_date, 'days');
            //var diffHours = moment().diff(target_date, 'hours');
            //var diffMinutes = moment().diff(target_date, 'minutes');
            var diffSeconds = moment().diff(target_date, 'seconds');

            var SpanTime = document.createElement("span");

            if (diffSeconds >= 60)
            {
                // console.log("curData.timestamp:" +curData.timestamp+" target_date:"+target_date+  " diffSeconds:"+diffSeconds);
                SpanTime.setAttribute("class","badge bg-red");
            }
            else
               SpanTime.setAttribute("class","badge bg-light-yellow");
            SpanTime.setAttribute("style","opacity:"+opacityvalue);
            SpanTime.textContent = curData.timestamp;
            tdTime.appendChild(SpanTime);

            var tdRanging = document.createElement('td');
            if ( i == currentRangingIndex)
                tdRanging.setAttribute("bgcolor","#00cccc");
            row.appendChild(tdRanging);

            var SpanRanging = document.createElement("span");
            SpanRanging.setAttribute("class","badge bg-yellow");
            SpanRanging.setAttribute("style","opacity:"+opacityvalue);
            SpanRanging.textContent = curData.ranging;
            tdRanging.appendChild(SpanRanging);

            if( curData.angle != "0.00")
            {
                var SpanAngle = document.createElement("span");
                SpanAngle.setAttribute("class","badge bg-fuchsia");
                SpanAngle.setAttribute("style","opacity:"+opacityvalue);
                SpanAngle.textContent = curData.angle;
                tdRanging.appendChild(SpanAngle);
            }
        }
    }
}

function toggle_ranging_loading()
{
    if (glCurrentLoadingState == 1)
    {
        glCurrentLoadingState = 0; // play
        var ranging_state_button = document.getElementById('ranging_state_button');
        ranging_state_button.setAttribute("class","glyphicon glyphicon-pause");
    }
    else
    {
        var ranging_state_button = document.getElementById('ranging_state_button');
        ranging_state_button.setAttribute("class","glyphicon glyphicon-play");
        glCurrentLoadingState = 1; // pause
    }
}

function checkTagExist(nodeid)
{
    var existed = false;
    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        if ( nodeid === node_nodeid )
        {
            existed = true;
            return existed;
        }
    }
    return existed;
}

function checkAnchorExist(nodeid)
{
    var existed = false;
    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        if ( nodeid === node_nodeid )
        {
            existed = true;
            return existed;
        }
    }

    for (var i = 0; i < glAnchor_List.length; i++)
    {
        var nodeData = glAnchor_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        if ( nodeid === node_nodeid )
        {
            existed = true;
            return existed;
        }
    }

    return existed;
}

function getTagName(nodeid)
{
    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        if ( nodeid === node_nodeid )
            return node_name;

    }
    return nodeid;
}

function getAnchorName(nodeid)
{
    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        if ( nodeid === node_nodeid )
            return node_name;
    }

    for (var i = 0; i < glAnchor_List.length; i++)
    {
        var nodeData = glAnchor_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        if ( nodeid === node_nodeid )
            return node_name;
    }

    return nodeid;
}

function open_ranging_page()
{
    var win = window.open('/index.php/vilsranging/'+project_id, '_blank');
    win.focus();
}
