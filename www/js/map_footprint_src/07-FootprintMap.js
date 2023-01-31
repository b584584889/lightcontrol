/*
 * Author: ikki Chung
 * Date: 19 Jan 2017
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/

//
// start run
//
var glLiveMap = new Indoor2DMap();


moment.locale('zh-tw');

var glTagFTCache = [];

//var glTagFootprints = [[]];
//var glTimeScale = 60*60;
var glTimeSampling = 3;
var glTimeChangedTimer = 0;

// create a new queue
var glUpadteQueue = new Queue();
var glProcessingQueue = false;

var glRangeStartDate = new Date();
var glRangeEndDate = new Date();

var glTags = [];
var gl_tag_num = 0;
var gl_tag_load_index = 0;

var LOAD_DEVICE_NUM = 20;

var url = new URL(window.location.href);
var RangeStartDate = url.searchParams.get("RangeStartDate");
if ( RangeStartDate != undefined)
{
    var curDate = RangeStartDate.split(':');
    var startDate = new Date(parseInt(curDate[0]), parseInt(curDate[1]) - 1, parseInt(curDate[2]), parseInt(curDate[3]), parseInt(curDate[4]), parseInt(curDate[5]), 0);
    //console.log("glRangeStartDate:"+glRangeStartDate);
    //console.log("       startDate:"+startDate);
    glRangeStartDate = startDate;
    //glRangeStartDate.setDate(startDate.getDate());
    //console.log("glRangeStartDate:"+glRangeStartDate);
}
else
{
    glRangeStartDate.setDate(glRangeStartDate.getDate() - 1);
    //console.log("glRangeStartDate == undefined, glRangeStartDate:"+glRangeStartDate);
}

var RangeEndDate = url.searchParams.get("RangeEndDate");
if ( RangeEndDate != undefined)
{
    //var curDate = moment(RangeEndDate).format('YYYY:MM:DD:HH:mm:ss');
    var curDate = RangeEndDate.split(':');
    var endDate = new Date(parseInt(curDate[0]), parseInt(curDate[1]) - 1, parseInt(curDate[2]), parseInt(curDate[3]), parseInt(curDate[4]), parseInt(curDate[5]), 0);
    //console.log("glRangeEndDate:"+glRangeEndDate);
    //console.log("       endDate:"+endDate);
    glRangeEndDate = endDate;
    //glRangeEndDate.setDate(endDate.getDate());
    //console.log("glRangeEndDate:"+glRangeEndDate);
}
else
{
    glRangeEndDate.setDate(glRangeEndDate.getDate() + 1);
    //console.log("glRangeEndDate == undefined, glRangeEndDate:"+glRangeEndDate);
}


load_map_config();


function load_map_config()
{
    //var targetURL = "http://" + window.location.hostname + "/php/configvils.php";
    var targetURL = gl_target_server + "/php/map.php";
    $.getJSON(targetURL, 'loadmap=1'+'&project_id='+project_id, function(mapsdata)
    {
        var data = mapsdata[0];

        glLiveMap.setMapCoeff(data.POSX, data.POSY, data.MAPWIDTH, data.MAPHEIGHT, data.SCALE, data.COEFF);
        glLiveMap.setMapImage(data.FILE, "Indoor 2D Map");
        //glLiveMap.createMapEditor();
        glLiveMap.load_maparea();
        //glLiveMap.setNodeDragCallback(update_map_node_info);
        //glLiveMap.setOrigNodeDragCallback(update_orig_node_info);
        //glLiveMap.setUpdateMapInfoCallback(update_map_info);
        glLiveMap.addOriginMarker(0, 0, 0);

        //runLive();
        load_tags_info();
        //load_tag_footprint();
    });
    return false;
}


var glGroups = new vis.DataSet([
  //{"content": "Tag1", "id": "Tag1Id", "value": 1, className:'openwheel'},
  //{"content": "Tag2", "id": "Tag2Id", "value": 2, className:'rally'},
  //{"content": "Tag3", "id": "Tag3Id", "value": 3, className:'motorcycle'}
]);

// create a dataset with items
// note that months are zero-based in the JavaScript Date object, so month 3 is April
var glItems = new vis.DataSet([
    // {start: new Date(2017, 5, 10), end: new Date(2017, 5, 11), group:"Tag024", className:"openwheel", content:"Argentina",id:"531@motocal.net", title:"531@motocal.net"}
    // {start: new Date(2017, 5, 22), end: new Date(2017, 5, 26), group:"Tag019", className:"rally", content:"Rallye Monte-Carlo",id:"591@motocal.net", title:"531@motocal.net"},
    // {start: new Date(2017, 6, 4), end: new Date(2017, 6, 8), group:"Tag019", className:"motorcycle", content:"Sepang MotoGP Test 1",id:"578@motocal.net", title:"531@motocal.net"},
    // {start: new Date(2017, 6, 12), end: new Date(2017, 6, 16), group:"Tag024", className:"rally", content:"Rally Sweden",id:"592@motocal.net", title:"531@motocal.net"},
    // {start: new Date(2017, 6, 20), end: new Date(2017, 6, 23), group:"Tag024", className:"motorcycle", content:"Australia",id:"616@motocal.net", title:"531@motocal.net"},
    // {start: new Date(2017, 4, 23), end: new Date(2017, 4, 27), group:"Tag019", className:"motorcycle", content:"Sepang MotoGP Test 2",id:"579@motocal.net", title:"531@motocal.net"},
    // {start: new Date(2017, 4, 26), end: new Date(2017, 4, 2), group:"Tag024", className:"touringcar", content:"Clipsal 500 Adelaide",id:"659@motocal.net", title:"531@motocal.net"},
    // {start: new Date(2017, 7, 5), end: new Date(2017, 5, 9), group:"Tag024", className:"rally", content:"Rally Guanajuato Mexico",id:"593@motocal.net", title:"531@motocal.net"},
    // {start: new Date(2017, 7, 6), end: new Date(2017, 5, 9), group:"Tag019", className:"touringcar", content:"Argentina",id:"717@motocal.net", title:"531@motocal.net"}
]);



function load_tags_info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_tag_load_index = 0;
    })
    .success(function() {
        setTimeout(load_tag_footprint, 300);
    })
    .error(function() {
        setTimeout(load_tags_info, 1000);
    })
    .complete(function() {
    });

    //setTimeout(function(){ p.abort(); }, 5000);
}

function load_tag_footprint()
{
    if (gl_tag_load_index >= gl_tag_num)
    {
        gl_tag_load_index = 0;
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

        //console.log("anchor_num:"+anchor_num +", gl_anchor_load_index:"+gl_anchor_load_index);

        for (var i = gl_tag_load_index; i < gl_tag_load_index+tag_num; ++i)
        {
            var key = 'LOC_TAG_INDEX_'+i;
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
            var date = new moment();

            //glGroups.add({"id":nodeid, "content":nodename, "value": i, "visible": true});
            var content = getHtmlGroup(nodename, nodeid);

            var ShowDefault = true;
            if (i != 0)
                ShowDefault = false;

            glGroups.add({"id":nodeid, "content":content, "value": i, "visible": ShowDefault});

            glTags[nodeid] = [];
            glTags[nodeid] = {
                'nodeid':nodeid,
                'macaddress':macaddress,
                'nodename':nodename,
                'posX':posX,
                'posY':posY,
                'posZ':posZ,
                'buildingid':buildingid,
                'floor':floor,
                'date':date
            };
            glLiveMap.updateTagMarker(macaddress, nodeid, nodename, posX, posY, posZ, buildingid, floor, date, false);
            glLiveMap.showTag(nodeid, ShowDefault);//show tag
        }
        gl_tag_load_index += tag_num;

    })
    .success(function() {
        if (gl_tag_load_index >= gl_tag_num)
        {
            gl_tag_load_index = 0;

            runFootprintLive();
        }
        else
        {
            load_tag_footprint();
        }
    })
    .error(function() {
        gl_tag_load_index = 0;
        setTimeout(load_tag_footprint, 5000);
    })
    .complete(function() {
    });
    //setTimeout(function(){ p.abort(); }, 5000);

}

function getHtmlGroup(nodename, nodeid)
{
    //var content = "<img src=\"" + "/images/"+nodename+".png\"" + " width=\"40\" height=\"40\" /><img id=\"updating_"+nodeid+"\" src=\"/images/load.gif\" style=\"display: block; float:right;\" width=\"20\" height=\"20\" /></br>"+nodename;
    //var content = "<img src=\"" + "/images/marker-icon-2x.png\"" + " width=\"40\" height=\"40\" /><img id=\"updating_"+nodeid+"\" src=\"/images/load.gif\" style=\"display: block; float:right;\" width=\"20\" height=\"20\" /></br>"+nodename;
    var content = "<img src=\"" + "/images/marker-icon-2x.png\"" + " width=\"20\"/><img id=\"updating_"+nodeid+"\" src=\"/images/load.gif\" style=\"display: block; float:right;\" width=\"20\" height=\"20\" /></br>"+nodename;
    return content;
}


function updateTags(curDate, usingPlayBackData)
{
    //var targetDate = new Date(curDate);

    // Set the global configs to synchronous
    $.ajaxSetup({
        async: false
    });

    var index = 0;
    for (var tagid in glTags)
    {
        var group = glGroups.get(tagid);
        if (!group.visible)
        {
            continue;
        }
        var tag = glTags[tagid];

        //alert(tagid + ": value="+value);

        if ( glTagFTCache[tagid] == null )
            continue;
        //
        // wait 500 ms
        //
        //setTimeout(function(){updateTag(tag, usingPlayBackData, curDate);}, 500 * index);
        //sleep(500);
        updateTag(tag, usingPlayBackData, curDate);

        index++;
    }

    // Set the global configs back to asynchronous
    $.ajaxSetup({
        async: true
    });
}

function updateTag(tag, usingPlayBackData, curDate)
{
    var value = null;
    if (usingPlayBackData)
    {
        value = glTagFTCache[tag.nodeid].getPlayBackFootPrint(curDate);
    }
    else
    {
        value = glTagFTCache[tag.nodeid].getFootPrint(curDate);

    }
    if (value != null)
    {
        glLiveMap.updateTagMarker(tag.macaddress, tag.nodeid, tag.nodename, value.X, value.Y, value.Z, tag.buildingid, tag.floor, tag.date, false);
    }
}

function sleep(milliseconds)
{
    var start = new Date().getTime();
    for (var i = 0; i < 1e7; i++)
    {
        if ((new Date().getTime() - start) > milliseconds)
        {
            break;
        }
    }
}


function runFootprintLive()
{
    ceate_dropdown_tags();

    updateTimelineBlock_withqueue();

    //
    // refresh every 30 seconds
    //setTimeout(runFootprintLive, 30000);
    //
}

function ceate_dropdown_tags()
{
    var dropdown_tags_ul = document.getElementById("dropdown_tags");

    for (var tagid in glTags)
    {
        var group = glGroups.get(tagid);
        var tag = glTags[tagid];
        var item_li = create_dropdown_item(tagid, tag.nodename);
        dropdown_tags_ul.appendChild(item_li);

        document.getElementById("dropdown_checkbox_"+tagid).checked = group.visible;
    }

}

function create_dropdown_item(nodeid, showtext)
{
    //console.log("create_dropdown_item() nodeid:"+nodeid+", showtext:"+showtext);

    var item_li = document.createElement("li");
    var item_click = document.createElement("a");
    item_click.setAttribute("class","small");
    item_click.setAttribute("tabIndex","-1");
    item_click.setAttribute("data-value",nodeid);
    item_click.setAttribute("href","javascript:;");
    item_click.setAttribute("onclick","dropdown_item_click(\""+ nodeid +"\");return true;");

    var item_checkbox = document.createElement("input");
    item_checkbox.setAttribute("type","checkbox");
    item_checkbox.setAttribute("id","dropdown_checkbox_"+nodeid);
    item_checkbox.setAttribute("onclick","dropdown_checkbox_click(\""+ nodeid +"\");return true;");

    item_click.appendChild(item_checkbox);
    item_click.appendChild(document.createTextNode(" "+showtext));
    item_li.appendChild(item_click);

    return item_li;
}

var glDropdown_checkbox_click = false;
function dropdown_item_click(nodeid)
{
    //console.log("dropdown_item_click() nodeid:"+nodeid);
    var checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_"+nodeid).checked;

    if (glDropdown_checkbox_click)
    {
        // keep the same value
    }
    else
    {
        // toggle value
        document.getElementById("dropdown_checkbox_"+nodeid).checked = !checkedValue_dropdown_checkbox;
        checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_"+nodeid).checked;
    }

    glGroups.update({id: nodeid, visible: checkedValue_dropdown_checkbox});
    glLiveMap.showTag(nodeid, checkedValue_dropdown_checkbox);//show tag
    updateTimelineBlock_withqueue();

    glDropdown_checkbox_click = false;
}

function dropdown_checkbox_click(nodeid)
{
    //console.log("dropdown_checkbox_click() nodeid:"+nodeid);
    glDropdown_checkbox_click = true;

    // var checkedValue_dropdown_checkbox = document.getElementById("dropdown_checkbox_"+nodeid).checked;
    //
    // glGroups.update({id: nodeid, visible: checkedValue_dropdown_checkbox});
    // glLiveMap.showTag(nodeid, true);//show tag
    // updateTimelineBlock_withqueue();
}

// $("#dropdown_tags").click(function()
// {
//     console.log("dropdown_tags() click()");
//     return true;
// });

//
// load_scale_footprint callback function
//
function load_footprint_cb(tagid, queryStartDate, queryEndDate)
{
    //console.log("load_footprint_cb() tagid:"+tagid);

    var tag_updating_icon = document.getElementById("updating_"+tagid);
    if (tag_updating_icon != undefined)
        tag_updating_icon.style.display = "none";

    var length = glUpadteQueue.getLength();
    //console.log("glUpadteQueue.getLength():"+length);

    // get the item at the front of the queue
    //var item = queue.peek();

    if (length > 0)
    {
        var tagid = glUpadteQueue.dequeue();
        if ( glTagFTCache[tagid] == null )
        {
            glTagFTCache[tagid] = new TagFootPrintCacheData();
            glTagFTCache[tagid].node_id = tagid;
        }

        //
        // get from cache
        //
        //glTagFTCache[tagid].load_scale_footprint(tagid, glItems, glTimeScale, queryStartDate, queryEndDate, load_footprint_cb);
        glTagFTCache[tagid].load_sampling_footprint(tagid, glItems, glTimeSampling, queryStartDate, queryEndDate, load_footprint_cb);
    }
    else
    {
        glProcessingQueue = false;
    }
}

//
// update Timeline Block by TimeScale and put to queue
//
function updateTimelineBlock_withqueue()
{
    //console.log("updateTimelineBlock_withqueue(): glProcessingQueue="+glProcessingQueue);
    if (glProcessingQueue == true)
    {
        return;
    }

    glProcessingQueue = true;

    var queryStartDate = formatDate(glRangeStartDate);
    var queryEndDate = formatDate(glRangeEndDate);

    for (var tagid in glTags)
    {
        var group = glGroups.get(tagid);
        if (group.visible)
        {
            //var tag = glTags[tagid];
            //load_footprint(tagid, tmpItems);

            var tag_updating_icon = document.getElementById("updating_"+tagid);
            if (tag_updating_icon != undefined)
                tag_updating_icon.style.display = "block";

            glUpadteQueue.enqueue(tagid);
        }
    }

    glItems.clear();

    //
    // start queue
    //
    load_footprint_cb("", queryStartDate, queryEndDate);

    /*
    var tagid = glUpadteQueue.dequeue();
    if ( glTagFTCache[tagid] == null )
    {
        glTagFTCache[tagid] = new TagFootPrintCacheData();
        glTagFTCache[tagid].node_id = tagid;
    }

    //
    // get from cache
    //
    glTagFTCache[tagid].load_scale_footprint(tagid, glItems, glTimeScale, queryStartDate, queryEndDate, load_footprint_cb);
    */
}

//
// update Timeline Block by TimeScale
//
function updateTimelineBlock()
{
    var tmpItems = new vis.DataSet([]);

    var queryStartDate = formatDate(glRangeStartDate);
    var queryEndDate = formatDate(glRangeEndDate);

    // Set the global configs to synchronous
    $.ajaxSetup({
        async: false
    });

    for (var tagid in glTags)
    {
        var group = glGroups.get(tagid);
        if (group.visible)
        {
            //var tag = glTags[tagid];
            //load_footprint(tagid, tmpItems);

            glUpadteQueue.enqueue(tagid);

            //var length = glUpadteQueue.getLength();
            //console.log("glUpadteQueue.getLength():"+length);

            if ( glTagFTCache[tagid] == null )
            {
                glTagFTCache[tagid] = new TagFootPrintCacheData();
                glTagFTCache[tagid].node_id = tagid;
            }

            //
            // get from cache
            //
            //glTagFTCache[tagid].load_scale_footprint(tagid, tmpItems, glTimeScale, queryStartDate, queryEndDate, load_footprint_cb);
            glTagFTCache[tagid].load_sampling_footprint(tagid, tmpItems, glTimeSampling, queryStartDate, queryEndDate, load_footprint_cb);

        }
        else
        {
            //alert(tagid+" is hide!");
        }
    }

    var length = glUpadteQueue.getLength();
    console.log("glUpadteQueue.getLength():"+length);

    // Set the global configs back to asynchronous
    $.ajaxSetup({
        async: true
    });

    glItems.clear();
    glItems = tmpItems;
    timeline.setItems(glItems);
}


// function to make all groups visible again
function showAllGroups()
{
    glGroups.forEach(function(group)
    {
        glGroups.update({id: group.id, visible: true});
        glLiveMap.showTag(group.id, true);//show tag
    })
    updateTimelineBlock_withqueue();
};

//
// convert Date to string format YYYY-MM-DD hh:mm:ss
//
function formatDate(targeDate)
{
    var strDate = targeDate.getFullYear()+'-'
        +(""+ (targeDate.getMonth() + 101)).substr(1)+'-'
        +(""+ (targeDate.getDate() + 100)).substr(1)+' '
        +(""+ (targeDate.getHours() + 100)).substr(1)+':'
        +(""+ (targeDate.getMinutes() + 100)).substr(1)+':'
        +(""+ (targeDate.getSeconds() + 100)).substr(1);
    return strDate;
}

    // create visualization
    var container = document.getElementById('visualization');
    var options = {
        // option groupOrder can be a property name or a sort function
        // the sort function must compare two groups and return a value
        //     > 0 when a > b
        //     < 0 when a < b
        //       0 when a == b
        groupOrder: function (a, b)
        {
            return a.value - b.value;
        },
        groupOrderSwap: function (a, b, glGroups)
        {
            var v = a.value;
            a.value = b.value;
            b.value = v;
        },
        groupTemplate: function(group)
        {
            var container = document.createElement('div');
            var label = document.createElement('span');
            label.innerHTML = group.content + ' ';
            container.insertAdjacentElement('afterBegin',label);
            // var hide = document.createElement('button');
            // hide.innerHTML = 'hide';
            // hide.style.fontSize = 'small';
            // hide.addEventListener('click',function()
            // {
            //     glGroups.update({id: group.id, visible: false});
            //     glLiveMap.showTag(group.id, false);//hide tag
            // });
            // container.insertAdjacentElement('beforeEnd',hide);
            return container;
        },
        // format:
        // {
        //     majorLabels: function(date, scale, step)
        //     {
        //         return moment(date).format('MMMM Do YYYY, h:mm:ss a');
        //         //return new Date(date);
        //         //return  (Math.round(ago * step / divider)) + " " + scale + "s ago"
        //     }
        // },
        tooltip:
        {
            followMouse: true
        },
        orientation: 'both',
        editable: false,
        groupEditable: false,
        start: glRangeStartDate,
        end: glRangeEndDate,
        zoomMax: 1000 * 60 * 60 * 24 * 360, // 360 days
        zoomMin: 1000, // 1 second
        showCurrentTime: true
    };

    //console.log("glRangeStartDate:"+glRangeStartDate);
    //console.log("glRangeEndDate:"+glRangeEndDate);
    //console.log("options:"+options);


    var timeline = new vis.Timeline(container);
    timeline.setOptions(options);
    timeline.setGroups(glGroups);
    timeline.setItems(glItems);

    var CurTimeCursorId = 1234;
    timeline.addCustomTime(new Date(), CurTimeCursorId);

    // add event listener
    timeline.on('timechange', function (event)
    {

        // var curTimeChangedTimer = new Date();
        // var diffMilliSeconds = parseInt(curTimeChangedTimer - glTimeChangedTimer);
        //
        // document.getElementById("customTime").innerHTML = "curTimeChangedTimer:" + curTimeChangedTimer+", glTimeChangedTimer:" + glTimeChangedTimer+
        //         ", diffMilliSeconds:" + diffMilliSeconds;
        //
        // if ( diffMilliSeconds < 300 )
        // {
        //     glTimeChangedTimer = curTimeChangedTimer;
        // }
        // else
        // {
        //     glTimeChangedTimer = curTimeChangedTimer;

            //document.getElementById("customTime").innerHTML = "指定時間: " + event.time;
            document.getElementById("customTime").innerHTML = "指定時間: " + moment(event.time).format('YYYY MMMM Do, h:mm:ss a');
            var curDate = moment(event.time).format('YYYY-MM-DD HH:mm:ss');
            updateTags(curDate, false);
        // }
        // var item = items.get("531@motocal.net");
        // if (event.time > item.start)
        // {
        //     item.end = new Date(event.time);
        //     var now = new Date();
        //     if (event.time < now)
        //     {
        //         item.content = "Dynamic event (past)";
        //         item.className = 'past';
        //     }
        //     else
        //     if (event.time > now)
        //     {
        //         item.content = "Dynamic event (future)";
        //         item.className = 'future';
        //     }
        //     else
        //     {
        //         item.content = "Dynamic event (now)";
        //         item.className = 'now';
        //     }
        //
        //     items.update(item);
        // }
    });


    timeline.on('click', function (properties)
    {
        //properties['time'];
        //properties['snappedTime'];
        //alert("properties['time']="+properties['time']);
        // timeline.setCustomTime(properties['time'], 1234);
        // document.getElementById("customTime").innerHTML = "Custom Time: " + properties['time'];
    })

    timeline.on('doubleClick', function (properties)
    {
        //properties['time'];
        //properties['snappedTime'];
        //alert("properties['time']="+properties['time']);

        //var curDate = new Date(properties['time']);
        var curDate = moment(properties['time']).format('YYYY-MM-DD HH:mm:ss');
        timeline.setCustomTime(properties['time'], CurTimeCursorId);
        //document.getElementById("customTime").innerHTML = "指定時間: " + curDate;
        document.getElementById("customTime").innerHTML = "指定時間: " + moment(properties['time']).format('YYYY MMMM Do, h:mm:ss a');

        updateTags(curDate, true);

    })

    timeline.on('rangechange', function (properties)
    {
        //logEvent('rangechange', properties);
    });

    timeline.on('rangechanged', function (properties)
    {
        //logEvent('rangechanged', properties);
        //var startDate = new Date(properties['start']);
        //var endDate = new Date(properties['end']);

            glRangeStartDate = new Date(properties['start']);
            glRangeEndDate = new Date(properties['end']);

            var strStartDate = moment(glRangeStartDate).format('YYYY:MM:DD:HH:mm:ss');
            var strEndDate = moment(glRangeEndDate).format('YYYY:MM:DD:HH:mm:ss');

            var newURL = UpdateQueryString("RangeStartDate", strStartDate, window.location.href);
            //console.log(newURL);

            newURL = UpdateQueryString("RangeEndDate", strEndDate, newURL);
            //console.log(newURL);
            //window.location = newURL;
            window.history.pushState({path:newURL}, '', newURL)


            //var diffSeconds = parseInt((properties['end'] - properties['start']) / 1000);
            var diffMinutes = parseInt((properties['end'] - properties['start']) / (1000 * 60));
            var diffHours = parseInt((properties['end'] - properties['start']) / (1000 * 60 * 60));
            var diffDays = parseInt((properties['end'] - properties['start']) / (1000 * 60 * 60 * 24));
            //var diffSeconds = parseInt((value['datetime'] - endDate) / 1000);

            //alert("diffDays="+diffDays+",diffHours="+diffHours+",diffSeconds="+diffSeconds);

            //var preTimeScale = glTimeScale;
            var preTimeSampling = glTimeSampling;

            if (diffDays >= 60) // 60 days
            {
                glTimeSampling = 10; // 1 day
            }
            else
            if (diffDays >= 30) // 30 days
            {
                glTimeSampling = 9; // every 12 Hours
            }
            else
            if (diffDays >= 20) // 20 days
            {
                glTimeSampling = 8; // every 6 Hours
            }
            else
            if (diffDays >= 10) // 10 days
            {
                glTimeSampling = 7; // every 1 Hours
            }
            else
            if (diffDays >= 3)
            {
                glTimeSampling = 6; // every 30 Minutes
            }
            else
            if (diffDays >= 1)
            {
                glTimeSampling = 5; // every 10 Minutes
            }
            else
            if (diffHours >= 6)
            {
                glTimeSampling = 4; // every 5 Minutes
            }
            else
            if (diffHours >= 3)
            {
                glTimeSampling = 3; // every 60 second
            }
            else
            if (diffHours >= 1)
            {
                glTimeSampling = 2;// every 30 second
            }
            else
            {
                glTimeSampling = 1;// every 1 second
            }



            var strStartDate = formatDate(glRangeStartDate);
            var strEndDate = formatDate(glRangeEndDate);

            //document.getElementById("messagelog").innerHTML = "diffDays:" + diffDays+", diffHours:" + diffHours+
            //        ", diffMinutes:" + diffMinutes+", glTimeScale:" + glTimeScale;// +
                    //", strStartDate:" + strStartDate+", strEndDate:" + strEndDate;

            //
            //
            //
            //if (preTimeScale != glTimeScale)
                updateTimelineBlock_withqueue();

    });



    // set a custom range from -2 minute to +3 minutes current time
    var start = new Date((new Date()).getTime() - 2 * 60 * 1000);
    var end   = new Date((new Date()).getTime() + 3 * 60 * 1000);
    timeline.setWindow(start, end, {animation: false});




var glPlayMode = 0;//pause
var glPlayBackSpeed = 1;
function FootprintPlayBackLoop()
{
    var curTime = timeline.getCustomTime(CurTimeCursorId);
    curTime.setSeconds(curTime.getSeconds() + 1 * glPlayBackSpeed);
    timeline.setCustomTime(curTime, CurTimeCursorId);

    //alert("curTime:"+curTime);

    var curDate = moment(curTime).format('YYYY-MM-DD HH:mm:ss');
    document.getElementById("customTime").innerHTML = "指定時間: " + moment(curTime).format('YYYY MMMM Do, h:mm:ss a');

    updateTags(curDate, true);

    if(glPlayMode == 1)
        setTimeout(FootprintPlayBackLoop, 1000);
}

$("#FootprintPlayBtn").click(function()
{
    //alert("glPlayMode:"+glPlayMode);
    if (glPlayMode == 0)
    {
        glPlayMode = 1;
        FootprintPlayBackLoop();
    }

    return false;
});

$("#FootprintPauseBtn").click(function()
{
    //alert("glPlayMode:"+glPlayMode);
    glPlayMode = 0;
    return false;
});

function setPlaybackspeed(playspeed)
{
    switch(playspeed)
    {
        default:
        case 1:
            glPlayBackSpeed = 1;
            document.getElementById("FootprintPlaySpeed").innerHTML = "x1 speed";
        break;
        case 4:
            glPlayBackSpeed = 4;
            document.getElementById("FootprintPlaySpeed").innerHTML = "x4 speed";
        break;
        case 16:
            glPlayBackSpeed = 16;
            document.getElementById("FootprintPlaySpeed").innerHTML = "x16 speed";
        break;
        case 60:
            glPlayBackSpeed = 60;
            document.getElementById("FootprintPlaySpeed").innerHTML = "x60 speed";
        break;
    }
}

function UpdateQueryString(key, value, url)
{
    if (!url) url = window.location.href;
    var re = new RegExp("([?&])" + key + "=.*?(&|#|$)(.*)", "gi"),
        hash;

    if (re.test(url)) {
        if (typeof value !== 'undefined' && value !== null)
            return url.replace(re, '$1' + key + "=" + value + '$2$3');
        else {
            hash = url.split('#');
            url = hash[0].replace(re, '$1$3').replace(/(&|\?)$/, '');
            if (typeof hash[1] !== 'undefined' && hash[1] !== null)
                url += '#' + hash[1];
            return url;
        }
    }
    else {
        if (typeof value !== 'undefined' && value !== null) {
            var separator = url.indexOf('?') !== -1 ? '&' : '?';
            hash = url.split('#');
            url = hash[0] + separator + key + '=' + value;
            if (typeof hash[1] !== 'undefined' && hash[1] !== null)
                url += '#' + hash[1];
            return url;
        }
        else
            return url;
    }
}

function open_footprint_page()
{
    var win = window.open('/index.php/vilsfootprint/'+project_id, '_blank');
    win.focus();
}
