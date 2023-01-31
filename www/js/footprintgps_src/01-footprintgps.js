/*
 * Author: ikki Chung
 * Date: 2019 06 12
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var LOAD_DEVICE_NUM = 20;
var UPDATE_INTERVAL = 1000; // update display every 1 seconds
var RECORD_PAGE_COUNT = 10;

var NODE_TYPE_TAG = 1;
var NODE_TYPE_LOCATOR = 3;
var NODE_TYPE_SENSOR = 6;
var NODE_TYPE_USER = 2222;

var gl_tag_num = 0;
var gl_sendor_num = 0;

var gl_tag_load_index = 0;
var gl_sensor_load_index = 0;

var glTag_List = [];
var glSensor_List = [];
var glUser_List = [];

var glTargetStartDate = null;
var glTargetEndDate = null;

var glCurrentTag_nodeid = "";
var glCurrentNode_type = NODE_TYPE_TAG; // NODE_TYPE_TAG, NODE_TYPE_LOCATOR, NODE_TYPE_SENSOR, NODE_TYPE_USER

var glCurrentTag_clicktime = new Date('2019/01/01');
var glCurrentTag_Records = [];

var glCurrentRecord_page = 0;

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

                load_user();

            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                create_record_table();
                click_tag("");

                CurrentLoadIndex = 2; // next state
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;
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

function load_devices()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'devicescount=1'+'&project_id='+project_id, function(data)
    {
        gl_tag_num = parseInt(data['LOC_TAG_NUM']);
        gl_sensor_num = parseInt(data['LOC_SENSOR_NUM']);

        gl_tag_load_index = 0;
        gl_sensor_load_index = 0;

        glTag_List = [];
        glSensor_List = [];

    })
    .success(function() {
        setTimeout(load_tag, 10);
        setTimeout(load_sensor, 30);
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
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
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

        //console.log("tag_num:"+tag_num +", gl_tag_load_index:"+gl_tag_load_index);

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
            add_tag_to_table();
            CurrentLoadIndex = CurrentLoadIndex + 1; // next state
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

function add_tag_to_table()
{
    var tag_list_content = document.getElementById('tag_list_content');

    while (tag_list_content.rows.length >= 1)
    {
        tag_list_content.deleteRow(0);
    }

    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var table_tr = document.createElement('tr');
        tag_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        table_td_1.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_1);

        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_node_name_"+ node_nodeid);
        table_td_2.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_2);

        // node id
        var tag_nodeid_span = document.createElement('span');
        tag_nodeid_span.setAttribute("class",'badge bg-green');
        tag_nodeid_span.textContent = node_name;
        tag_nodeid_span.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_td_2.appendChild(tag_nodeid_span);
    }
}

function load_sensor()
{
    if (gl_sensor_load_index >= gl_sensor_num)
    {
        gl_sensor_load_index = 0;
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
            gl_sensor_load_index += 1;
            glSensor_List.push(device);
        });
    })
    .success(function() {
        if (gl_sensor_load_index >= gl_sensor_num)
        {
            add_sensor_to_table();
        }
        else
        {
            setTimeout(load_sensor, 50);
        }
    })
    .error(function() {
        gl_sensor_load_index = 0;
        setTimeout(load_sensor, 300);
    })
    .complete(function() {
    });
}

function add_sensor_to_table()
{
    var sensor_list_content = document.getElementById('sensor_list_content');

    while (sensor_list_content.rows.length >= 1)
    {
        sensor_list_content.deleteRow(0);
    }

    // console.log("add_locator_to_table() glLocator_List.length:"+glLocator_List.length);

    for (var i = 0; i < glSensor_List.length; i++)
    {
        var sensorData = glSensor_List[i];

        var table_tr = document.createElement('tr');
        sensor_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        table_td_1.setAttribute("id","id_row_index_"+ sensorData.id);
        table_td_1.setAttribute("onclick","click_sensor(\""+ sensorData.id +"\");return false;");
        table_tr.appendChild(table_td_1);

        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_node_name_"+ sensorData.id);
        table_td_2.setAttribute("onclick","click_sensor(\""+ sensorData.id +"\");return false;");
        table_tr.appendChild(table_td_2);

        // sensor name
        var sensor_name_span = document.createElement('span');
        sensor_name_span.setAttribute("class",'badge bg-green-active');
        sensor_name_span.textContent = sensorData.name;
        sensor_name_span.setAttribute("onclick","click_sensor(\""+ sensorData.id +"\");return false;");
        table_td_2.appendChild(sensor_name_span);
    }
}

function load_user()
{
    glUser_List = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduserbaseinfo=1&&project_id='+project_id, function(data)
    {
        $.each(data, function(index, user)
        {
            user.userid = parseInt(user.userid);
            glUser_List.push(user);
        });
    })
    .success(function() {

        add_user_to_table();

        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function add_user_to_table()
{
    var user_list_content = document.getElementById('user_list_content');

    while (user_list_content.rows.length >= 1)
    {
        user_list_content.deleteRow(0);
    }

    // console.log("add_locator_to_table() glLocator_List.length:"+glLocator_List.length);

    for (var i = 0; i < glUser_List.length; i++)
    {
        var userData = glUser_List[i];

        var table_tr = document.createElement('tr');
        user_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        table_td_1.setAttribute("id","id_row_index_"+ userData.userid);
        table_td_1.setAttribute("onclick","click_user("+ userData.userid +");return false;");
        table_tr.appendChild(table_td_1);

        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_node_name_"+ userData.userid);
        table_td_2.setAttribute("onclick","click_user("+ userData.userid +");return false;");
        table_tr.appendChild(table_td_2);

        // userData name
        var sensor_name_span = document.createElement('span');
        sensor_name_span.setAttribute("class",'badge bg-aqua-active');
        sensor_name_span.textContent = userData.name;
        sensor_name_span.setAttribute("onclick","click_user("+ userData.userid +");return false;");
        table_td_2.appendChild(sensor_name_span);
    }
}

function click_tag(click_node_nodeid)
{
    //console.log("click_tag() click_node_macaddress:" + click_node_macaddress);
    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentTag_clicktime.getTime());

    if (diffTimeInMS < 500)
    {
        //console.log("click_tag() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glCurrentTag_clicktime = new Date();

    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_nodeid = nodeData[3];

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentTag_nodeid.length == 0 || click_node_nodeid === node_nodeid )
        {
            glCurrentTag_nodeid = node_nodeid;
            glCurrentNode_type = NODE_TYPE_TAG;// tag
            bg_color = "background-color:#CCEEFF;";
        }

        var id_row_index = document.getElementById("id_row_index_"+ node_nodeid);
        var id_row_node_name = document.getElementById("id_row_node_name_"+ node_nodeid);
        id_row_index.setAttribute("style",bg_color);
        id_row_node_name.setAttribute("style",bg_color);
    }

    glCurrentRecord_page = 0;
    load_device_record();
}

function click_sensor(click_sensor_id)
{
    //console.log("click_tag() click_node_macaddress:" + click_node_macaddress);
    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentTag_clicktime.getTime());

    if (diffTimeInMS < 500)
    {
        //console.log("click_tag() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glCurrentTag_clicktime = new Date();

    for (var i = 0; i < glSensor_List.length; i++)
    {
        var sensorData = glSensor_List[i];

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentTag_nodeid.length == 0 || click_sensor_id === sensorData.id )
        {
            glCurrentTag_nodeid = sensorData.id;
            glCurrentNode_type = NODE_TYPE_SENSOR;// sensor
            bg_color = "background-color:#CCEEFF;";
        }

        var id_row_index = document.getElementById("id_row_index_"+ sensorData.id);
        var id_row_node_name = document.getElementById("id_row_node_name_"+ sensorData.id);
        id_row_index.setAttribute("style",bg_color);
        id_row_node_name.setAttribute("style",bg_color);
    }

    load_device_record();
}

function click_user(click_user_id)
{
    console.log("click_user() click_user_id:" + click_user_id);
    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentTag_clicktime.getTime());

    if (diffTimeInMS < 500)
    {
        console.log("click_user() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glCurrentTag_clicktime = new Date();

    for (var i = 0; i < glUser_List.length; i++)
    {
        var userData = glUser_List[i];

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentTag_nodeid.length == 0 || click_user_id == userData.userid )
        {
            glCurrentTag_nodeid = userData.userid;
            glCurrentNode_type = NODE_TYPE_USER;// user
            bg_color = "background-color:#CCEEFF;";
        }

        var id_row_index = document.getElementById("id_row_index_"+ userData.userid);
        var id_row_node_name = document.getElementById("id_row_node_name_"+ userData.userid);
        id_row_index.setAttribute("style",bg_color);
        id_row_node_name.setAttribute("style",bg_color);
    }

    load_device_record();
}

function create_record_table()
{
    var record_box_content = document.getElementById('record_box_content');

    var divTabPane = document.createElement('div');
    divTabPane.setAttribute("id","record_box_table");
    record_box_content.appendChild(divTabPane);

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
    spanTitle.setAttribute("id","record_box_table_title");
    boxTitle.appendChild(spanTitle);

    var spanTitleStartTime = document.createElement("span");
    spanTitleStartTime.setAttribute("class","badge bg-yellow-active");
    spanTitleStartTime.setAttribute("id","record_StartEndTime");
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
    aTools1.setAttribute("onclick","move_date_forward();return false;");
    aTools1.textContent = '\xAB'; // html &laquo;
    liTools1.appendChild(aTools1);

    // 'Page'
    var liTools2 = document.createElement("li");
    ulTools.appendChild(liTools2);

    var aTools2 = document.createElement("a");
    aTools2.setAttribute("href","#");
    aTools2.setAttribute("id","record_Status_Page");
    aTools2.textContent = "";
    liTools2.appendChild(aTools2);

    // '>'
    var liTools3 = document.createElement("li");
    ulTools.appendChild(liTools3);

    var aTools3 = document.createElement("a");
    aTools3.setAttribute("href","javascript:;");
    aTools3.setAttribute("onclick","move_date_backward();return false;");
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
    theadtrGroup.setAttribute("id","record_table_head");
    theadGroup.appendChild(theadtrGroup);

    var theadth = document.createElement("th");
    theadth.setAttribute("style","width: 10px");
    theadth.textContent = '#';
    theadtrGroup.appendChild(theadth);

    var theadthAreaName = document.createElement("th");
    theadthAreaName.setAttribute("style","width: 150px");
    theadthAreaName.textContent = '經度';
    theadtrGroup.appendChild(theadthAreaName);

    var theadth_EnterTime = document.createElement("th");
    theadth_EnterTime.setAttribute("style","width: 200px; text-align:center");
    theadth_EnterTime.textContent = '緯度';
    theadtrGroup.appendChild(theadth_EnterTime);

    var theadth_LeaveTime = document.createElement("th");
    theadth_LeaveTime.setAttribute("style","text-align:center");
    theadth_LeaveTime.textContent = '時間';
    theadtrGroup.appendChild(theadth_LeaveTime);

    var tbodyGroup = document.createElement("tbody");
    tbodyGroup.setAttribute("id","record_table_body");
    tableGroup.appendChild(tbodyGroup);

    if (glTargetStartDate == undefined)
    {
        glTargetStartDate = getTargetDate(-3);
    }
    if (glTargetEndDate == undefined)
    {
        glTargetEndDate = getTargetDate(0);
    }

    // Date picker
    $('#record_Status_Page').daterangepicker({
        autoApply: true,
        manualInputs: false, //SMIMS ikki add
        startDate: glTargetStartDate,
        endDate: glTargetEndDate,
        locale: {
                format: 'YYYY/MM/DD'
            }

    },
    function(start, end)
    {
        // console.log("A new date selection was made: " + start.format('YYYY-MM-DD') + ' to ' + end.format('YYYY-MM-DD') );

        glTargetStartDate = start.format('YYYYMMDD');
        glTargetEndDate = end.format('YYYYMMDD');

        load_device_record();

    });

}

function move_date_forward()
{
    var end_date = glTargetEndDate;
    var cur_date = moment(end_date, "YYYYMMDD");
    var new_end_date = cur_date.add(3, 'day').format('YYYYMMDD');
    var new_start_date = end_date;

    glTargetStartDate = new_start_date;
    glTargetEndDate = new_end_date;

    console.log("move_date_forward()");

    glCurrentRecord_page = 0;
    load_device_record();
}

function move_date_backward()
{
    var start_date = glTargetStartDate;
    var cur_date = moment(start_date, "YYYYMMDD");
    var new_start_date = cur_date.subtract(3, 'day').format('YYYYMMDD');
    var new_end_date = start_date;

    glTargetStartDate = new_start_date;
    glTargetEndDate = new_end_date;

    console.log("move_date_backward()");

    glCurrentRecord_page = 0;
    load_device_record();
}

function load_device_record()
{
    console.log("load_device_record() glCurrentTag_nodeid:" + glCurrentTag_nodeid);

    // var chrTagName = "";
    // for (var i = 0; i < glTag_List.length; i++)
    // {
    //     var nodeData = glTag_List[i];
    //     var node_name = nodeData[5];
    //     var node_nodeid = nodeData[3];
    //
    //     if ( glCurrentTag_nodeid === node_nodeid )
    //     {
    //         chrTagName = node_name;
    //     }
    // }

    var record_box_title = document.getElementById('record_box_title');
    if (record_box_title != undefined)
    {
        record_box_title.textContent = get_node_name(glCurrentTag_nodeid);
    }
    var record_Status_Page = document.getElementById("record_Status_Page");
    if (record_Status_Page != undefined)
    {
        record_Status_Page.textContent = convertToShowDate(glTargetStartDate) +" - "+ convertToShowDate(glTargetEndDate);
    }
    var record_table_body = document.getElementById('record_table_body');
    if (record_table_body != undefined)
    {
        while (record_table_body.rows.length >= 1)
        {
            record_table_body.deleteRow(0);
        }
    }
    var record_box_move_page_index = document.getElementById('record_box_move_page_index');
    if (record_box_move_page_index != undefined)
    {
        record_box_move_page_index.textContent = glCurrentRecord_page + 1;
    }

    var targetStartDate = getTargetDateTime(glTargetStartDate, 0);
    var targetEndDate = getTargetDateTime(glTargetEndDate, 1);
    var offset = glCurrentRecord_page * RECORD_PAGE_COUNT;
    var count = RECORD_PAGE_COUNT;

    var targetURL = gl_target_server + "/php/fpnodes.php";
    $.getJSON(targetURL, 'loadgpsnodedata=1&project_id='+project_id+'&device='+glCurrentTag_nodeid+'&nodetype='+glCurrentNode_type+'&startdate='+targetStartDate+'&enddate='+targetEndDate+'&offset='+offset+'&count='+count, function(data)
    {

        glCurrentTag_Records = [];
        //glCurrentRecord_page = 0;

        $.each(data, function(key, value)
        {
            var record = {};

            record.Lng = value['Lng'];
            record.Lat = value['Lat'];
            record.time = value['time'];

            // console.log("load_device_record() nodeid:"+glCurrentTag_nodeid+", Lng:"+record.Lng+", Lat:"+record.Lat+", time:"+record.time);

            glCurrentTag_Records.push(record);
        });

        console.log("load_device_record() glCurrentTag_Records.length:"+glCurrentTag_Records.length);

        put_record_to_table(glCurrentTag_nodeid, glCurrentTag_Records, glCurrentRecord_page);

    }).success(function(){})
    .error(function(){})
    .complete(function(){});

}

function put_record_to_table(nodeid, records, page)
{
    var ui_index = 0;
    for(index in records)
    {
        var record = records[index];
        create_gps_info_row(ui_index, record);
        ui_index++;
    }
}

function create_gps_info_row(ui_index, record)
{
    var record_table_body = document.getElementById('record_table_body');
    if (record_table_body == undefined)
    {
        console.log("create_gps_info_row() record_table_body == undefined");
        return ;
    }

    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","table_row_"+ui_index);
    record_table_body.appendChild(row);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Longitude
    var table_td_Longitude_td = document.createElement('td');
    row.appendChild(table_td_Longitude_td);
    var table_td_Longitude_span = document.createElement('span');
    table_td_Longitude_span.setAttribute("class",'badge bg-light-blue');
    table_td_Longitude_span.textContent = record.Lng;
    // table_td_Longitude_td.appendChild(table_td_Longitude_span);
    row.cells[cellIndex].appendChild(table_td_Longitude_span);
    cellIndex++;

    // Latitude
    var table_td_Latitude_td = document.createElement('td');
    row.appendChild(table_td_Latitude_td);
    var table_td_Latitude_span = document.createElement('span');
    table_td_Latitude_span.setAttribute("class",'badge bg-light-blue');
    table_td_Latitude_span.textContent = record.Lat;
    // table_td_Latitude_td.appendChild(table_td_Latitude_span);
    row.cells[cellIndex].appendChild(table_td_Latitude_span);
    cellIndex++;

    // time
    var table_td_leaveltime_td = document.createElement('td');
    row.appendChild(table_td_leaveltime_td);
    // row.cells[cellIndex].appendChild(table_td_leaveltime_td);
    // cellIndex++;

    var gmapLink = document.createElement("a");
    gmapLink.setAttribute("href","javascript:;");
    gmapLink.setAttribute("onclick","open_gmap("+ record.Lat +","+ record.Lng+");return false;");
    var imggmapLink = document.createElement("i");
    imggmapLink.setAttribute("class","fa fa-map-marker");
    gmapLink.appendChild(imggmapLink);
    row.cells[cellIndex].appendChild(gmapLink);

    var table_td_space_span = document.createElement('span');
    table_td_space_span.textContent = "       ";
    row.cells[cellIndex].appendChild(table_td_space_span);

    var table_td_leaveltime_span = document.createElement('span');
    table_td_leaveltime_span.setAttribute("class",'badge bg-purple');
    table_td_leaveltime_span.textContent = record.time;
    table_td_leaveltime_td.appendChild(table_td_leaveltime_span);
    cellIndex++;
}

function create_area_row(ui_index, record)
{
    var record_table_body = document.getElementById('record_table_body');
    if (record_table_body == undefined)
    {
        console.log("create_area_inout_row() record_table_body == undefined");
        return ;
    }

    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","table_row_"+ui_index);
    record_table_body.appendChild(row);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Area
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(record.AreaName));
    cellIndex++;

    // Enter Time
    var table_td_entertime_td = document.createElement('td')
    row.appendChild(table_td_entertime_td);
    if (record.action == '1')
    {
        var table_td_entertime_span = document.createElement('span');
        table_td_entertime_span.setAttribute("class",'badge bg-light-blue');
        table_td_entertime_span.textContent = record.DataTime;
        table_td_entertime_td.appendChild(table_td_entertime_span);
    }
    cellIndex++;

    // Leave Time
    var table_td_leaveltime_td = document.createElement('td')
    row.appendChild(table_td_leaveltime_td);
    if (record.action == '2')
    {
        var table_td_leaveltime_span = document.createElement('span');
        table_td_leaveltime_span.setAttribute("class",'badge bg-purple');
        table_td_leaveltime_span.textContent = record.DataTime;
        table_td_leaveltime_td.appendChild(table_td_leaveltime_span);
    }
    cellIndex++;

    // stay Time
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""));
    cellIndex++;
}

function get_node_name(nodeid)
{
    var nodename = '';
    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        if ( nodeid === node_nodeid )
        {
            nodename = node_name;
        }
    }

    if ( nodename === '')
    {
        for (var i = 0; i < glSensor_List.length; i++)
        {
            var sensorData = glSensor_List[i];
            if ( nodeid === sensorData.id )
            {
                nodename = sensorData.name;
            }
        }
    }

    if ( nodename === '')
    {
        for (var i = 0; i < glUser_List.length; i++)
        {
            var userData = glUser_List[i];
            if ( nodeid == userData.userid )
            {
                nodename = userData.name;
            }
        }
    }

    if ( nodename === '')
        nodename = nodeid;
    return nodename;
}

function record_box_move_page_forward()
{
    if ( glCurrentRecord_page <= 0 )
    {
        console.log("record_box_move_page_forward() no more data!!");
        // not more data
        return;
    }
    else
    {
        glCurrentRecord_page--;
        load_device_record();
    }
}

function record_box_move_page_backward()
{
    if ( glCurrentTag_Records.length < RECORD_PAGE_COUNT)
    {
        console.log("record_box_move_page_backward() no more data!!");
        // not more data
        return;
    }
    else
    {
        glCurrentRecord_page++;
        load_device_record();
    }
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

function getTargetDateTime(inDate, addDays)
{
    var cur_date = moment(inDate, "YYYY-MM-DD");
    //var fullDatetime = cur_date.format('YYYY-MM-DD HH:mm:ss');

    var targetDate = cur_date.add(addDays, 'd').format('YYYY-MM-DD HH:mm:ss');
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

function open_gmap(Latitude, Longitude)
{
    var targeturi = "http://www.google.com/maps/place/"+Latitude+','+Longitude;
    var win = window.open(targeturi, '_blank');
    win.focus();
}

function open_footprintgps_page()
{
    var win = window.open('/index.php/vilsfootprintgps/'+project_id, '_blank');
    win.focus();
}
