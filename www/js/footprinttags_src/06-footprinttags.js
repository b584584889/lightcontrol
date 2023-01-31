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
var RECORD_PAGE_COUNT = 5;

var LOAD_TAG_DATA_POS = 1;
var LOAD_TAG_DATA_INOUT = 2;
var LOAD_TAG_DATA_GPS = 3;
var LOAD_TAG_DATA_COURSE = 4;
var LOAD_TAG_DATA_ALERT = 5;
var LOAD_TAG_DATA_EVENT = 6;

var NODE_TYPE_TAG = 1;
var NODE_TYPE_LOCATOR = 3;
var NODE_TYPE_SENSOR = 6;
var NODE_TYPE_USER = 2222;

var gl_tag_num = 0;
var gl_locator_num = 0;
var gl_sendor_num = 0;

var gl_tag_load_index = 0;
var gl_coord_load_index = 0;
var gl_sensor_load_index = 0;

var glTag_List = [];
var glLocator_List = [];
var glSensor_List = [];
var glUser_List = [];

var glCurrentTag_clicktime = new Date('2020-01-01');
var glCurrentTag_nodeid = "";
var glCurrentTag_macaddress = "";
var glCurrentNode_type = NODE_TYPE_TAG; // NODE_TYPE_TAG, NODE_TYPE_LOCATOR, NODE_TYPE_SENSOR, NODE_TYPE_USER

var glTargetStartDate = null;
var glTargetEndDate = null;

var glCurrentTag_Records = {};
//var glLoadData_status = [];
var glDataLoading = 0;

//var glCurrentMapIndex = 0;
var glMapDataList = {};
var glRecordDataList = {};
var glHeatMapList = {};

var glTargetDate = getTodayDate();
var glShowDataMode = 0; // 0: timeline 1: one day

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

                if (gl_tag_num > 0)
                    click_tag("");
                else
                    click_user("");

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                check_cookies();

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
        gl_locator_num = parseInt(data['LOC_LOCATOR_NUM']);
        gl_sensor_num = parseInt(data['LOC_SENSOR_NUM']);

        gl_tag_load_index = 0;
        gl_locator_load_index = 0;
        gl_sensor_load_index = 0;

        glTag_List = [];
        glLocator_List = [];
        glSensor_List = [];
    })
    .success(function() {
        setTimeout(load_tag, 10);
        setTimeout(load_locator, 20);
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

        // console.log("tag_num:"+tag_num +", gl_tag_load_index:"+gl_tag_load_index);

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

    //console.log("add_tag_to_table() glTag_List.length:"+glTag_List.length);

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
        tag_nodeid_span.setAttribute("class",'badge bg-blue-active');
        tag_nodeid_span.textContent = node_name;
        tag_nodeid_span.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_td_2.appendChild(tag_nodeid_span);
    }
}

function load_locator()
{
    if (gl_locator_load_index >= gl_locator_num)
    {
        gl_locator_load_index = 0;
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

        // console.log("locator_num:"+locator_num +", gl_locator_load_index:"+gl_locator_load_index);

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
            add_locator_to_table();
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

function add_locator_to_table()
{
    var locator_list_content = document.getElementById('locator_list_content');

    while (locator_list_content.rows.length >= 1)
    {
        locator_list_content.deleteRow(0);
    }

    // console.log("add_locator_to_table() glLocator_List.length:"+glLocator_List.length);

    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var table_tr = document.createElement('tr');
        locator_list_content.appendChild(table_tr);

        var table_td_1 = document.createElement('td');
        table_td_1.textContent = i+1;
        table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        table_td_1.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_1);

        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_node_name_"+ node_nodeid);
        table_td_2.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_2);

        // node id
        var locator_nodeid_span = document.createElement('span');
        locator_nodeid_span.setAttribute("class",'badge bg-yellow-active');
        locator_nodeid_span.textContent = node_name;
        locator_nodeid_span.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_td_2.appendChild(locator_nodeid_span);
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

    if (diffTimeInMS < 500 || glDataLoading == 1)
    {
        //console.log("click_tag() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    var daily_pdf_report_dl_btn = document.getElementById("daily_pdf_report_dl_btn");
    daily_pdf_report_dl_btn.setAttribute("style", "display:none");
    daily_pdf_report_dl_btn.setAttribute("onclick", "location.href=''");

    var daily_pdf_report_gen_btn = document.getElementById("daily_pdf_report_gen_btn");
    daily_pdf_report_gen_btn.setAttribute("style", "display:block");


    glDataLoading = 1;
    glCurrentTag_clicktime = new Date();

    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_nodeid = nodeData[3];
        var node_macaddress = nodeData[6];

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentTag_nodeid.length == 0 || click_node_nodeid === node_nodeid )
        {
            glCurrentTag_nodeid = node_nodeid;
            glCurrentNode_type = NODE_TYPE_TAG;// tag

            glCurrentTag_macaddress = node_macaddress;

            bg_color = "background-color:#CCEEFF;";
        }

        var id_row_index = document.getElementById("id_row_index_"+ node_nodeid);
        var id_row_node_name = document.getElementById("id_row_node_name_"+ node_nodeid);
        id_row_index.setAttribute("style",bg_color);
        id_row_node_name.setAttribute("style",bg_color);
    }

    var status_date = document.getElementById('daily_targetdate');
    if (status_date != undefined)
    {
        status_date.textContent = glTargetDate;
        //status_date.textContent = convertToShowDate(glTargetDate);
    }

    load_device_record();
}

function click_locator(click_node_nodeid)
{
    //console.log("click_tag() click_node_macaddress:" + click_node_macaddress);
    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentTag_clicktime.getTime());

    if (diffTimeInMS < 500 || glDataLoading == 1)
    {
        //console.log("click_tag() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glDataLoading = 1;
    glCurrentTag_clicktime = new Date();

    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var node_nodeid = nodeData[3];

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentTag_nodeid.length == 0 || click_node_nodeid === node_nodeid )
        {
            glCurrentTag_nodeid = node_nodeid;
            glCurrentNode_type = NODE_TYPE_LOCATOR;// locator
            bg_color = "background-color:#CCEEFF;";
        }

        var id_row_index = document.getElementById("id_row_index_"+ node_nodeid);
        var id_row_node_name = document.getElementById("id_row_node_name_"+ node_nodeid);
        id_row_index.setAttribute("style",bg_color);
        id_row_node_name.setAttribute("style",bg_color);
    }

    load_device_record();
}

function click_sensor(click_sensor_id)
{
    //console.log("click_tag() click_node_macaddress:" + click_node_macaddress);
    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentTag_clicktime.getTime());

    if (diffTimeInMS < 500 || glDataLoading == 1)
    {
        //console.log("click_tag() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glDataLoading = 1;
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

    if (diffTimeInMS < 500 || glDataLoading == 1)
    {
        console.log("click_user() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glDataLoading = 1;
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

    var status_date = document.getElementById('daily_targetdate');
    if (status_date != undefined)
    {
        status_date.textContent = glTargetDate;
        //status_date.textContent = convertToShowDate(glTargetDate);
    }

    load_device_record();
}

function load_device_record()
{
    // console.log("load_device_record() glCurrentTag_nodeid:" + glCurrentTag_nodeid+" glCurrentTag_macaddress:" + glCurrentTag_macaddress);

    var record_box_title = document.getElementById('record_box_title');
    if (record_box_title != undefined)
    {
        record_box_title.textContent = "Loading...";
    }

    glCurrentTag_Records = {};
    // glLoadData_status[LOAD_TAG_DATA_POS] = 0;
    // glLoadData_status[LOAD_TAG_DATA_INOUT] = 0;
    // glLoadData_status[LOAD_TAG_DATA_GPS] = 0;
    // glLoadData_status[LOAD_TAG_DATA_COURSE] = 0;
    // glLoadData_status[LOAD_TAG_DATA_ALERT] = 0;
    // glLoadData_status[LOAD_TAG_DATA_EVENT] = 0;
    glTargetStartDate = null;
    glTargetEndDate = null;

    load_recent_record_express(glCurrentTag_nodeid, glCurrentNode_type);
    //load_recent_record(glCurrentTag_nodeid);

    clear_timeline();
}

function reload_tag_info()
{
    glCurrentTag_Records = {};
    // glLoadData_status[LOAD_TAG_DATA_POS] = 0;
    // glLoadData_status[LOAD_TAG_DATA_INOUT] = 0;
    // glLoadData_status[LOAD_TAG_DATA_GPS] = 0;
    // glLoadData_status[LOAD_TAG_DATA_COURSE] = 0;
    // glLoadData_status[LOAD_TAG_DATA_ALERT] = 0;
    // glLoadData_status[LOAD_TAG_DATA_EVENT] = 0;
    glTargetStartDate = getTargetDateTime( glTargetEndDate, -1);
    glTargetEndDate = getTargetDateTime( glTargetStartDate, RECORD_PAGE_COUNT);

    // console.log("reload_tag_info() nodeid:"+glCurrentTag_nodeid+" glTargetStartDate:" + glTargetStartDate+" glTargetEndDate:" + glTargetEndDate);

    removeElement('reload_btn_id');

    load_recent_record_express(glCurrentTag_nodeid, glCurrentNode_type);
    //load_recent_record(glCurrentTag_nodeid);
}

function load_recent_record_express(nodeid, nodetype)
{
    var targetURL = gl_target_server + "/php/fpnodes.php";

    if (glTargetStartDate == undefined)
    {
        glTargetStartDate = getTargetDateTime( getTargetDate(0), 1);
    }
    // console.log("load_recent_record_express() nodeid:"+nodeid+" glTargetStartDate:" + glTargetStartDate);

    var cur_show_count = RECORD_PAGE_COUNT;
    var cur_StartDate = glTargetStartDate;

    if ( glShowDataMode == 1 )
    {
        cur_show_count = 1;
        cur_StartDate = convertTargetDateTime(glTargetDate);
    }

    var getQuery = $.getJSON(targetURL, 'checkrecentdata=1&device='+nodeid+'&nodetype='+nodetype+'&recentdays='+cur_show_count+'&startdate='+cur_StartDate+'&project_id='+project_id, function(data)
    {
        //console.log("load_recent_record_express() data.length:"+data.length +", data[0].day:"+data[0].day);

        //glTargetStartDate = getTargetDateTime( data[0].day, 1);
        //glTargetEndDate = getTargetDateTime( data[data.length-1].day, 0);
        // console.log("load_recent_record_express() data.days:"+data.days+", max_date:"+data.max_date +", min_date:"+data.min_date);

        glTargetStartDate = data.max_date;
        glTargetEndDate = data.min_date;

        var track_data = data.track_data;

        for (var date in track_data)
        {
            if ( glShowDataMode == 1 )
            {
                if (!(date === glTargetDate))
                {
                    continue;
                }
            }

            // console.log("load_recent_record_express() date:"+date+", track_data.length:"+track_data[date].length);
            var dateData = track_data[date];

            if(glCurrentTag_Records[date] == undefined)
                glCurrentTag_Records[date] = [];

            for(var index=0;index<dateData.length;index++)
            {
                var curData = dateData[index];
                var record = {};
                record.type = -1;

                // console.log("load_recent_record_express() date:"+date+", curData.datatype:"+curData.datatype);
                if ( curData.datatype == LOAD_TAG_DATA_POS )
                {
                    record.type = LOAD_TAG_DATA_POS;
                    record.count = curData.count;
                    record.max_time = curData.max_time;
                    record.min_time = curData.min_time;
                    record.total_time = curData.total_time;
                }
                else
                if ( curData.datatype == LOAD_TAG_DATA_INOUT )
                {
                    record.type = LOAD_TAG_DATA_INOUT;
                    record.count = curData.count;
                    record.max_time = curData.max_time;
                    record.min_time = curData.min_time;
                    record.total_time = curData.total_time;
                    record.area_ids = curData.area_ids;
                    record.area_names = curData.area_names;
                }
                else
                if ( curData.datatype == LOAD_TAG_DATA_GPS )
                {
                    record.type = LOAD_TAG_DATA_GPS;
                    record.count = curData.count;
                    record.max_time = curData.max_time;
                    record.min_time = curData.min_time;
                    record.total_time = curData.total_time;
                }
                else
                if ( curData.datatype == LOAD_TAG_DATA_COURSE )
                {
                    var courseInfo = JSON.parse(curData.course_info);
                    //var courseInfo = curData.course_info;

                    record.type = LOAD_TAG_DATA_COURSE;
                    record.areaid = courseInfo.areaid;
                    record.areaname = courseInfo.areaname;
                    record.courseId = courseInfo.courseId;
                    record.course_start = courseInfo.course_start;
                    record.course_end = courseInfo.course_end;
                    record.course_startTime = courseInfo.course_startTime;
                    record.course_endTime = courseInfo.course_endTime;
                    record.course_title = courseInfo.course_title;
                    record.course_startRecur = courseInfo.course_startRecur;
                    record.course_endRecur = courseInfo.course_endRecur;
                    record.in_max_time = courseInfo.in_max_time;
                    record.in_min_time = courseInfo.in_min_time;
                    record.in_count = courseInfo.in_count;
                    record.out_max_time = courseInfo.out_max_time;
                    record.out_min_time = courseInfo.out_min_time;
                    record.out_count = courseInfo.out_count;
                }
                else
                if ( curData.datatype == LOAD_TAG_DATA_ALERT )
                {
                    record.type = LOAD_TAG_DATA_ALERT;
                    record.count = curData.count;
                    record.max_time = curData.max_time;
                    record.min_time = curData.min_time;
                    record.total_time = curData.total_time;
                }
                else
                if ( curData.datatype == LOAD_TAG_DATA_EVENT )
                {
                    record.type = LOAD_TAG_DATA_EVENT;
                    record.count = curData.count;
                    record.max_time = curData.max_time;
                    record.min_time = curData.min_time;
                    record.total_time = curData.total_time;
                }

                if (record.type != -1)
                    glCurrentTag_Records[date].push(record);
            }
        }

        update_show_data()

    });
}

function removeElement(id)
{
    var elem = document.getElementById(id);
    return elem.parentNode.removeChild(elem);
}

function update_show_data()
{
     show_data_on_timeline();

     glDataLoading = 0;

     var record_box_title = document.getElementById('record_box_title');
     if (record_box_title != undefined)
     {
         var chrTagName = get_node_name(glCurrentTag_nodeid);
         record_box_title.textContent = chrTagName;

         if ( glCurrentNode_type == NODE_TYPE_TAG )
         {
             record_box_title.setAttribute("class","badge bg-blue-active");
         }
         else
         if ( glCurrentNode_type == NODE_TYPE_LOCATOR )
         {
             record_box_title.setAttribute("class","badge bg-yellow-active");
         }
         else
         if ( glCurrentNode_type == NODE_TYPE_SENSOR )
         {
             record_box_title.setAttribute("class","badge bg-green");
         }
         else
         if ( glCurrentNode_type == NODE_TYPE_USER )
         {
             record_box_title.setAttribute("class","badge bg-aqua-active");
         }
     }
}

function show_data_on_timeline()
{
    var keys = Object.keys(glCurrentTag_Records);

    var main_timeline = document.getElementById('main_timeline');

    if(keys.length == 0)
    {
        console.log("show_data_on_timeline() no data!");
        var nodata_btn_ui = create_nodata_btn();
        main_timeline.appendChild(nodata_btn_ui);
        return;
    }

    for(index in keys)
    {
        var key = keys[index];
        var valueList = glCurrentTag_Records[key];
        //console.log("show_data_on_timeline() key:"+key+", valueList.length:"+valueList.length);

        var time_label = create_time_label(key);
        main_timeline.appendChild(time_label);

        var i;
        for(i=0;i<valueList.length;i++)
        {
            //console.log("show_data_on_timeline() ["+i+"] type:"+valueList[i].type+", count:"+valueList[i].count);

            if( valueList[i].type == LOAD_TAG_DATA_POS )
            {
                //console.log("show_data_on_timeline() ["+i+"] type:"+valueList[i].type+", count:"+valueList[i].count);

                var uiid = LOAD_TAG_DATA_POS + "_" + key;
                var pos_data_ui = create_pos_data_info(uiid, key, valueList[i]);
                main_timeline.appendChild(pos_data_ui);
            }
            else
            if( valueList[i].type == LOAD_TAG_DATA_INOUT )
            {
                var uiid = LOAD_TAG_DATA_INOUT + "_" + key;
                var inout_data_ui = create_inout_data_info(uiid, key, valueList[i]);
                main_timeline.appendChild(inout_data_ui);
            }
            else
            if( valueList[i].type == LOAD_TAG_DATA_GPS )
            {
                var uiid = LOAD_TAG_DATA_GPS + "_" + key;
                var gps_data_ui = create_gps_data_info(uiid, key, valueList[i]);
                main_timeline.appendChild(gps_data_ui);
            }
            else
            if( valueList[i].type == LOAD_TAG_DATA_COURSE )
            {
                //console.log("show_data_on_timeline() ["+i+"] key:"+key+", courseId:" +valueList[i].courseId);

                var uiid = LOAD_TAG_DATA_COURSE + "_" + key +"_"+valueList[i].courseId;
                var course_data_ui = create_course_data_info(uiid, key, valueList[i]);
                main_timeline.appendChild(course_data_ui);
            }
            else
            if( valueList[i].type == LOAD_TAG_DATA_ALERT )
            {
                var uiid = LOAD_TAG_DATA_ALERT + "_" + key;
                var alert_data_ui = create_alert_data_info(uiid, key, valueList[i]);
                main_timeline.appendChild(alert_data_ui);
            }
            else
            if( valueList[i].type == LOAD_TAG_DATA_EVENT )
            {
                var uiid = LOAD_TAG_DATA_EVENT + "_" + key;
                var alert_data_ui = create_event_data_info(uiid, key, valueList[i]);
                main_timeline.appendChild(alert_data_ui);
            }
        }
    }

    if ( glShowDataMode == 0 )
    {
        if(keys.length < RECORD_PAGE_COUNT)
        {
            var nodata_btn_ui = create_nodata_btn();
            main_timeline.appendChild(nodata_btn_ui);
        }
        else
        {
            var reload_btn_ui = create_reload_btn();
            main_timeline.appendChild(reload_btn_ui);
        }
    }
}

function clear_timeline()
{
    var main_timeline = document.getElementById('main_timeline');
    while (main_timeline.firstChild)
    {
        main_timeline.removeChild(main_timeline.firstChild);
    }
}

function create_time_label(cur_date)
{
    var time_label = document.createElement("li");
    time_label.setAttribute("class","time-label");

    var time_label_span = document.createElement("span");
    time_label_span.setAttribute("class","bg-yellow");
    time_label_span.textContent = cur_date;
    time_label.appendChild(time_label_span);
    return time_label;
}

function create_pos_data_info(uiid, date, value)
{
    var pos_data_ui = document.createElement("li");
    var img_icon = document.createElement("i");
    img_icon.setAttribute("class","fa fa-paw bg-aqua");
    pos_data_ui.appendChild(img_icon);

    var div_timeline_item = document.createElement("div");
    div_timeline_item.setAttribute("class","timeline-item");
    pos_data_ui.appendChild(div_timeline_item);

    var span_timeline_item_header = create_timeline_time_header("定位資訊");
    div_timeline_item.appendChild(span_timeline_item_header);

    //var stayTime = moment.utc(moment(value.max_time,"YYYY-MM-DD HH:mm:ss").diff(moment(value.min_time,"YYYY-MM-DD HH:mm:ss"))).format("HH:mm:ss")

    var span_timeline_item_body = create_timeline_time_body(uiid, "共 " + value.count +" 筆資料 時間歷程:" + convertPrettyTime(value.total_time));
    div_timeline_item.appendChild(span_timeline_item_body);

    //var area_badge = create_area_badge(value.area_ids, value.area_names);
    //div_timeline_item.appendChild(area_badge);

    var span_timeline_item_footer = create_timeline_time_footer(LOAD_TAG_DATA_POS, date, uiid);
    div_timeline_item.appendChild(span_timeline_item_footer);

    return pos_data_ui;
}

function create_inout_data_info(uiid, date, value)
{
    var inout_data_ui = document.createElement("li");
    var img_icon = document.createElement("i");
    img_icon.setAttribute("class","fa fa-location-arrow bg-blue");
    inout_data_ui.appendChild(img_icon);

    var div_timeline_item = document.createElement("div");
    div_timeline_item.setAttribute("class","timeline-item");
    inout_data_ui.appendChild(div_timeline_item);

    //var span_timeline_item_time = create_timeline_time_item("5 mins ago");
    //div_timeline_item.appendChild(span_timeline_item_time);

    var span_timeline_item_header = create_timeline_time_header("區域進出");
    div_timeline_item.appendChild(span_timeline_item_header);

    //var stayTime = moment.utc(moment(value.max_time,"YYYY-MM-DD HH:mm:ss").diff(moment(value.min_time,"YYYY-MM-DD HH:mm:ss"))).format("HH:mm:ss")

    var span_timeline_item_body = create_timeline_time_body(uiid, "共 " + value.count +" 筆資料 時間歷程:" + convertPrettyTime(value.total_time));
    div_timeline_item.appendChild(span_timeline_item_body);

    var area_badge = create_area_badge(value.area_ids, value.area_names);
    div_timeline_item.appendChild(area_badge);

    // var progress_bar = create_progress_bar(30);
    // div_timeline_item.appendChild(progress_bar);
    //



    var span_timeline_item_footer = create_timeline_time_footer(LOAD_TAG_DATA_INOUT, date, uiid);
    div_timeline_item.appendChild(span_timeline_item_footer);

    return inout_data_ui;
}

function create_gps_data_info(uiid, date, value)
{
    var gps_data_ui = document.createElement("li");
    var img_icon = document.createElement("i");
    img_icon.setAttribute("class","fa fa-map-marker bg-aqua");
    gps_data_ui.appendChild(img_icon);

    var div_timeline_item = document.createElement("div");
    div_timeline_item.setAttribute("class","timeline-item");
    gps_data_ui.appendChild(div_timeline_item);

    //var span_timeline_item_time = create_timeline_time_item("5 mins ago");
    //div_timeline_item.appendChild(span_timeline_item_time);

    var span_timeline_item_header = create_timeline_time_header("GPS 軌跡記錄");
    div_timeline_item.appendChild(span_timeline_item_header);

    var stayTime = moment.utc(moment(value.max_time,"YYYY-MM-DD HH:mm:ss").diff(moment(value.min_time,"YYYY-MM-DD HH:mm:ss"))).format("HH:mm:ss")

    var span_timeline_item_body = create_timeline_time_body(uiid, "共 " + value.count +" 筆資料 時間歷程:" + convertPrettyTime(stayTime));
    div_timeline_item.appendChild(span_timeline_item_body);

    var div_externallink = document.createElement("div");
    div_externallink.setAttribute("class","box-tools pull-right");
    div_timeline_item.appendChild(div_externallink);

    var btn_externallink = document.createElement("button");
    btn_externallink.setAttribute("type","button");
    btn_externallink.setAttribute("class","btn btn-box-tool");
    btn_externallink.setAttribute("onclick","open_footprinttag_gps_page(\""+date+"\", "+glCurrentNode_type+", \""+glCurrentTag_nodeid+"\", \""+glCurrentTag_macaddress+"\");return false;");
    div_externallink.appendChild(btn_externallink);

    var img_externallink = document.createElement("i");
    img_externallink.setAttribute("class","fa fa-external-link");
    btn_externallink.appendChild(img_externallink);

    var span_timeline_item_footer = create_timeline_time_footer(LOAD_TAG_DATA_GPS, date, uiid);
    div_timeline_item.appendChild(span_timeline_item_footer);

    return gps_data_ui;
}

function create_course_data_info(uiid, date, value)
{
    var inout_data_ui = document.createElement("li");
    var img_icon = document.createElement("i");
    img_icon.setAttribute("class","fa fa-calendar bg-teal");
    inout_data_ui.appendChild(img_icon);

    var div_timeline_item = document.createElement("div");
    div_timeline_item.setAttribute("class","timeline-item");
    inout_data_ui.appendChild(div_timeline_item);

    //var span_timeline_item_time = create_timeline_time_item("5 mins ago");
    //div_timeline_item.appendChild(span_timeline_item_time);

    var span_timeline_item_header = create_timeline_time_header("課程紀錄");
    div_timeline_item.appendChild(span_timeline_item_header);

    var span_timeline_item_body = create_timeline_time_body(uiid, "課程時間:" + value.course_startTime +" 到 " + value.course_endTime);
    div_timeline_item.appendChild(span_timeline_item_body);

    var area_badge = create_course_badge(value);
    div_timeline_item.appendChild(area_badge);

    // var progress_bar = create_progress_bar(30);
    // div_timeline_item.appendChild(progress_bar);
    //



    var span_timeline_item_footer = create_timeline_time_footer(LOAD_TAG_DATA_COURSE, date, uiid, value.areaid, value.in_min_time, value.out_max_time);
    div_timeline_item.appendChild(span_timeline_item_footer);

    return inout_data_ui;
}

function create_alert_data_info(uiid, date, value)
{
    var alert_data_ui = document.createElement("li");
    var img_icon = document.createElement("i");
    img_icon.setAttribute("class","fa fa-street-view bg-red");
    alert_data_ui.appendChild(img_icon);

    var div_timeline_item = document.createElement("div");
    div_timeline_item.setAttribute("class","timeline-item");
    alert_data_ui.appendChild(div_timeline_item);

    var span_timeline_item_header = create_timeline_time_header("發出緊急求助訊息");
    div_timeline_item.appendChild(span_timeline_item_header);

    var span_timeline_item_body = create_timeline_time_body(uiid, "總共發出 " + value.count +" 次求助訊息");
    div_timeline_item.appendChild(span_timeline_item_body);

    var span_timeline_item_footer = create_timeline_time_footer(LOAD_TAG_DATA_ALERT, date, uiid);
    div_timeline_item.appendChild(span_timeline_item_footer);


    return alert_data_ui;
}

function create_event_data_info(uiid, date, value)
{
    var alert_data_ui = document.createElement("li");
    var img_icon = document.createElement("i");
    img_icon.setAttribute("class","fa fa-bell bg-red");
    alert_data_ui.appendChild(img_icon);

    var div_timeline_item = document.createElement("div");
    div_timeline_item.setAttribute("class","timeline-item");
    alert_data_ui.appendChild(div_timeline_item);

    var span_timeline_item_header = create_timeline_time_header("觸發事件");
    div_timeline_item.appendChild(span_timeline_item_header);

    var span_timeline_item_body = create_timeline_time_body(uiid, "總共觸發 " + value.count +" 次事件");
    div_timeline_item.appendChild(span_timeline_item_body);

    var span_timeline_item_footer = create_timeline_time_footer(LOAD_TAG_DATA_EVENT, date, uiid);
    div_timeline_item.appendChild(span_timeline_item_footer);

    return alert_data_ui;
}



function create_timeline_time_item(show_msg)
{
    var span_timeline_item_time = document.createElement("span");
    span_timeline_item_time.setAttribute("class","time");

    var img_icon = document.createElement("i");
    img_icon.setAttribute("class","fa fa-clock-o");
    span_timeline_item_time.appendChild(img_icon);

    span_timeline_item_time.appendChild(document.createTextNode(" " + show_msg));

    return span_timeline_item_time;
}

function create_timeline_time_header(show_msg)
{
    var span_timeline_item_header = document.createElement("h3");
    span_timeline_item_header.setAttribute("class","timeline-header");
    span_timeline_item_header.textContent = show_msg;

    return span_timeline_item_header;
}

function create_timeline_time_body(uiid, show_msg)
{
    var div_timeline_item_body = document.createElement("div");
    div_timeline_item_body.setAttribute("class","timeline-body");
    div_timeline_item_body.textContent = show_msg;

    return div_timeline_item_body;
}

function create_timeline_time_footer(type, date, uiid, areaid, in_min_time, out_max_time)
{
    var div_timeline_item_footer = document.createElement("div");
    div_timeline_item_footer.setAttribute("class","timeline-footer");

    var footer_btn_1 = document.createElement("a");
    footer_btn_1.setAttribute("class","btn btn-primary btn-xs");
    footer_btn_1.setAttribute("onclick","load_tag_info_detail("+type+",\""+date+"\",\""+uiid+"\",\""+areaid+"\",\""+in_min_time+"\",\""+out_max_time+"\");");
    footer_btn_1.setAttribute("id","timeline_footer_btn1_" + uiid);
    footer_btn_1.textContent = "查看詳細資料";
    div_timeline_item_footer.appendChild(footer_btn_1);

    div_timeline_item_footer.appendChild(document.createTextNode("   "));

    // if ( type == LOAD_TAG_DATA_INOUT || type == LOAD_TAG_DATA_EVENT)
    // {
    //     var footer_btn_2 = document.createElement("a");
    //     footer_btn_2.setAttribute("class","btn btn-success btn-xs");
    //     footer_btn_2.setAttribute("onclick","create_tag_pdf_report("+type+",\""+date+"\",\""+uiid+"\",\""+areaid+"\",\""+in_min_time+"\",\""+out_max_time+"\");");
    //     footer_btn_2.setAttribute("id","timeline_footer_btn2_" + uiid);
    //     footer_btn_2.textContent = "產生PDF報告";
    //     div_timeline_item_footer.appendChild(footer_btn_2);
    //
    //     var footer_btn_2_spen = document.createElement("spen");
    //     footer_btn_2_spen.setAttribute("id","timeline_footer_btn2_span_" + uiid);
    //     footer_btn_2_spen.textContent = "";
    //     div_timeline_item_footer.appendChild(footer_btn_2_spen);
    // }

    var div_item_footer = document.createElement("div");
    div_item_footer.setAttribute("id","timeline_footer_" + uiid);
    div_timeline_item_footer.appendChild(div_item_footer);

    // var footer_btn_2 = document.createElement("a");
    // footer_btn_2.setAttribute("class","btn btn-danger btn-xs");
    // footer_btn_2.textContent = "Delete";
    // span_timeline_item_footer.appendChild(footer_btn_2);

    return div_timeline_item_footer;
}

function create_area_badge(area_ids, area_names)
{
    var ids = area_ids.split(",");
    var names = area_names.split(",");

    var div_area_badge = document.createElement("div");
    var table_area_badge = document.createElement("table");
    div_area_badge.appendChild(table_area_badge);

    var tr_area_badge = document.createElement("tr");
    table_area_badge.appendChild(tr_area_badge);

    var td_area_badge = document.createElement("td");
    td_area_badge.setAttribute("style","width:50px;text-align:center;");
    td_area_badge.textContent = "區域:";
    tr_area_badge.appendChild(td_area_badge);

    var index = 0;
    for(index = 0;index<ids.length;index++)
    {
        var td_area_badge = document.createElement("td");
        //td_area_badge.setAttribute("style","width:100px;text-align:center;");
        tr_area_badge.appendChild(td_area_badge);

        var area_badge = document.createElement("span");
        area_badge.setAttribute("class","badge bg-purple");
        area_badge.textContent = names[index];

        td_area_badge.appendChild(area_badge);
    }

    return div_area_badge;
}

function create_course_badge(coursedata)
{
    var div_course_badge = document.createElement("div");
    var table_course_badge = document.createElement("table");
    div_course_badge.appendChild(table_course_badge);

    var tr_course_badge = document.createElement("tr");
    table_course_badge.appendChild(tr_course_badge);

    var td_course_name = document.createElement("td");
    td_course_name.setAttribute("style","width:50px;text-align:center;");
    td_course_name.textContent = "課程:";
    tr_course_badge.appendChild(td_course_name);

    var td_course_name = document.createElement("td");
    //td_course_name.textContent = course_name;

    var course_name_badge = document.createElement("span");
    course_name_badge.setAttribute("class","badge bg-light-blue");
    course_name_badge.textContent = coursedata.course_title;
    td_course_name.appendChild(course_name_badge);

    tr_course_badge.appendChild(td_course_name);

    var td_course_name = document.createElement("td");
    td_course_name.setAttribute("style","width:50px;text-align:center;");
    td_course_name.textContent = " 在教室:";
    tr_course_badge.appendChild(td_course_name);

    var td_area_badge = document.createElement("td");
    //td_area_badge.setAttribute("style","width:100px;text-align:center;");
    tr_course_badge.appendChild(td_area_badge);

    var area_badge = document.createElement("span");
    area_badge.setAttribute("class","badge bg-purple");
    area_badge.textContent = coursedata.areaname;
    td_area_badge.appendChild(area_badge);

    var td_course_att = document.createElement("td");
    td_course_att.setAttribute("style","width:50px;text-align:center;");
    td_course_att.textContent = "上課, ";
    tr_course_badge.appendChild(td_course_att);

    var td_course_starttime = document.createElement("td");
    var starttime_list = coursedata.in_min_time.split(" ");
    td_course_starttime.textContent = convertPrettyCourseTime(starttime_list[1]);
    tr_course_badge.appendChild(td_course_starttime);

    var td_course_starttime_1 = document.createElement("td");
    td_course_starttime_1.setAttribute("style","width:80px;text-align:center;");
    td_course_starttime_1.textContent = "開始上課, ";
    tr_course_badge.appendChild(td_course_starttime_1);

    var td_course_endtime = document.createElement("td");
    var endtime_list = coursedata.out_max_time.split(" ");
    td_course_endtime.textContent = convertPrettyCourseTime(endtime_list[1]);
    tr_course_badge.appendChild(td_course_endtime);

    var td_course_endtime_1 = document.createElement("td");
    td_course_endtime_1.setAttribute("style","width:150px;text-align:center;");
    td_course_endtime_1.textContent = "離開課程, 上課總時間:";
    tr_course_badge.appendChild(td_course_endtime_1);

    var stayTime = moment.utc(moment(coursedata.out_max_time,"YYYY-MM-DD HH:mm:ss").diff(moment(coursedata.in_min_time,"YYYY-MM-DD HH:mm:ss"))).format("HH:mm:ss")

    var td_course_stayTime = document.createElement("td");
    td_course_stayTime.textContent = convertPrettyTime(stayTime);
    tr_course_badge.appendChild(td_course_stayTime);

    return div_course_badge;
}

function create_progress_bar(value)
{

    // <div class="progress progress-xs">
    //   <div class="progress-bar progress-bar-danger" style="width: 55%"></div>
    // </div>

    var progress = document.createElement("div");
    progress.setAttribute("class","progress progress-xs");
    progress.setAttribute("style","width:100px");

    var progress_bar = document.createElement("div");
    progress_bar.setAttribute("class","progress-bar progress-bar-danger");
    progress_bar.setAttribute("style","width: 55%");
    progress.appendChild(progress_bar);

    return progress;
    //<input type="text" class="knob" value="30" data-width="90" data-height="90" data-fgColor="#3c8dbc">
    // var inline_Knob = document.createElement("input");
    // inline_Knob.setAttribute("type","text");
    // inline_Knob.setAttribute("class","knob");
    // inline_Knob.setAttribute("value",value);
    // inline_Knob.setAttribute("data-width","90");
    // inline_Knob.setAttribute("data-height","90");
    // inline_Knob.setAttribute("data-fgColor","#3c8dbc");
    // return inline_Knob;
}

function create_reload_btn()
{
    var reload_btn_ui = document.createElement("li");
    reload_btn_ui.setAttribute("id","reload_btn_id");
    reload_btn_ui.setAttribute("class","time-label");

    var reload_btn_span = document.createElement("span");
    reload_btn_span.setAttribute("class","bg-green");
    reload_btn_ui.appendChild(reload_btn_span);

    var reload_btn = document.createElement("button");
    reload_btn.setAttribute("type","button");
    reload_btn.setAttribute("onclick","reload_tag_info();");
    reload_btn.setAttribute("class","btn btn-default btn-lrg ajax");
    reload_btn.setAttribute("title","Ajax Request");
    reload_btn.setAttribute("id","AjaxLoadMode");
    reload_btn.textContent = DEFINE_STRING_reload;
    reload_btn_span.appendChild(reload_btn);

    var img_icon = document.createElement("i");
    img_icon.setAttribute("class","fa fa-spin fa-refresh");
    reload_btn.appendChild(img_icon);

    return reload_btn_ui;
}

function create_nodata_btn()
{
    var nodata_btn_ui = document.createElement("li");

    var nodata_btn_span = document.createElement("span");
    nodata_btn_span.setAttribute("class","bg-red");
    nodata_btn_ui.appendChild(nodata_btn_span);

    var nodata_btn = document.createElement("button");
    nodata_btn.setAttribute("type","button");
    nodata_btn.setAttribute("class","btn btn-default btn-lrg ajax");
    nodata_btn.textContent = DEFINE_STRING_nodata;
    nodata_btn_span.appendChild(nodata_btn);

    return nodata_btn_ui;
}

function load_tag_info_detail(type, date, uiid, areaid, in_min_time, out_max_time)
{
    //console.log("load_tag_info_detail() type:"+type + " ,uiid:"+uiid);

    var footer_btn1_ui = document.getElementById("timeline_footer_btn1_" + uiid);

    if (footer_btn1_ui != undefined)
    {
        if ( footer_btn1_ui.textContent === "讀取中..." )
        {
            return;
        }
        if ( footer_btn1_ui.textContent === "讀取完成" )
        {
            return;
        }

        footer_btn1_ui.textContent = "讀取中...";
    }

    if ( type == LOAD_TAG_DATA_POS )
    {
        console.log("load_tag_info_detail() LOAD_TAG_DATA_POS");

        show_detail_pos_heatmap(uiid, date);
    }
    else
    if ( type == LOAD_TAG_DATA_INOUT )
    {
        console.log("load_tag_info_detail() LOAD_TAG_DATA_INOUT");

        show_detail_areainout(uiid, date);
    }
    else
    if ( type == LOAD_TAG_DATA_GPS )
    {
        console.log("load_tag_info_detail() LOAD_TAG_DATA_GPS");
        show_detail_gps_heatmap(uiid, date);
    }
    else
    if ( type == LOAD_TAG_DATA_COURSE )
    {
        console.log("load_tag_info_detail() LOAD_TAG_DATA_COURSE");
        show_detail_course(uiid, date, areaid, in_min_time, out_max_time);
    }
    else
    if ( type == LOAD_TAG_DATA_ALERT )
    {
        console.log("load_tag_info_detail() LOAD_TAG_DATA_ALERT");
        show_detail_alert(uiid, date);
    }
    else
    if ( type == LOAD_TAG_DATA_EVENT )
    {
        console.log("load_tag_info_detail() LOAD_TAG_DATA_EVENT");
        show_detail_event(uiid, date);
    }
}

function create_tag_pdf_report(type, date, uiid, areaid, in_min_time, out_max_time)
{
    console.log("create_tag_pdf_report() type:"+type + " ,uiid:"+uiid + " ,date:"+date);

    var footer_btn_2_spen = document.getElementById("timeline_footer_btn2_span_" + uiid);
    footer_btn_2_spen.textContent = "產生報表中...";

    var SendDate = "";
    if (type == LOAD_TAG_DATA_INOUT)
        SendDate = "loadareainoutpdfreport=1&id="+glCurrentTag_nodeid+"&project_id="+project_id+"&date=" + date;
    else
    if (type == LOAD_TAG_DATA_EVENT)
        SendDate = "loadeventpdfreport=1&id="+glCurrentTag_nodeid+"&project_id="+project_id+"&date=" + date;

    var targetURL = gl_target_server + "/php/configvils.php";

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
            console.log("get_pdf_report() success:"+result.success+" pdfreport:"+result.pdfreport);

            var pdf_report_dl_link = document.getElementById("timeline_footer_btn2_span_" + uiid);
            while (pdf_report_dl_link.firstChild)
            {
                pdf_report_dl_link.removeChild(pdf_report_dl_link.firstChild);
            }

            if ( result.success === 'true')
            {
                var dl_link = document.createElement("a");
                dl_link.setAttribute("href",result.pdfreport);
                dl_link.setAttribute("target","_blank");
                dl_link.textContent = "下載報表";
                pdf_report_dl_link.appendChild(dl_link);

                var imgPdf = document.createElement("i");
                imgPdf.setAttribute("class","fa fa-file-pdf-o");
                dl_link.appendChild(imgPdf);
            }
            else
            {
                var footer_btn_2_spen = document.getElementById("timeline_footer_btn2_span_" + uiid);
                footer_btn_2_spen.textContent = "產生報表失敗!";
            }
        }
    });
    return false;
}

function show_detail_pos_heatmap(uiid, target_date)
{
    // console.log("show_detail_pos_heatmap() uiid:"+uiid+" target_date:"+target_date);

    //
    // check if already existed
    //
    var heatmap_id = "record_box_heatmap_pos_"+uiid;
    var heatmap_ui = document.getElementById(heatmap_id);
    if (heatmap_ui != undefined)
    {
        console.log("show_detail_pos_heatmap() heatmap_id:"+heatmap_id +", existed!");
        return;
    }

    var CurrentTag_Records = {};

    //var targetStartDate = getTargetDateTime(target_date, 0);
    //var targetEndDate = getTargetDateTime(target_date, 1);
    //var offset = 0;
    //var count = 30000;

    var targetURL = gl_target_server + "/php/fpnodes.php";
    //$.getJSON(targetURL, 'loadposnodedata=1&project_id='+project_id+'&device='+glCurrentTag_nodeid+'&startdate='+targetStartDate+'&enddate='+targetEndDate+'&offset='+offset+'&count='+count, function(data)
    $.getJSON(targetURL, 'loadposnodedata=1&project_id='+project_id+'&device='+glCurrentTag_macaddress+'&startdate='+target_date, function(data)
    {
        //data.dbdata;
        //data.datetime;

        console.log("show_detail_pos_heatmap() data.length:"+ data.length);

        for(var i=0;i<data.length;i++)
        {
            var value = data[i];

            //console.log("show_detail_pos_heatmap() "+ i +" value:"+value);

            var pos = value['p'];
            var posData = pos.split(",");

            var record = {};

            var maplayer = value['m'];
            if ( maplayer != undefined )
            {
                record.maplayer = parseInt(maplayer);
            }
            else
            {
                record.maplayer = 0;
            }
            record.posX = parseInt(posData[0]);
            record.posY = parseInt(posData[1]);

            var cur_record = CurrentTag_Records[maplayer];
            if (cur_record == undefined)
            {
                CurrentTag_Records[maplayer] = [];
                cur_record = CurrentTag_Records[maplayer];
            }
            cur_record.push(record);
        }

        // console.log("show_detail_pos_heatmap() CurrentTag_Records.length:"+CurrentTag_Records.length);

        put_coord_to_heatmap(uiid, glCurrentTag_nodeid, CurrentTag_Records);

    });
}

function show_detail_areainout(uiid, target_date)
{
    // console.log("show_detail_areainout() target_date:"+target_date);

    //
    // check if already existed
    //
    var table_id = "record_box_table_areainout_"+uiid;
    var table_ui = document.getElementById(table_id);
    if (table_ui != undefined)
    {
        console.log("show_detail_areainout() table_id:"+table_id +", existed!");
        return;
    }

    var CurrentTag_Records = [];

    var targetStartDate = getTargetDateTime(target_date, 0);
    var targetEndDate = getTargetDateTime(target_date, 1);
    var offset = 0;
    var count = 300;

    var targetURL = gl_target_server + "/php/fpnodes.php";
    $.getJSON(targetURL, 'loadareainout=1&project_id='+project_id+'&device='+glCurrentTag_nodeid+'&startdate='+targetStartDate+'&enddate='+targetEndDate+'&offset='+offset+'&count='+count, function(data)
    {
        $.each(data, function(key, value)
        {
            var record = {};

            record.AreaType = value['AreaType'];
            record.action = value['action']; //1:in 2:out
            record.AreaID = value['AreaID'];
            record.AreaName = value['AreaName'];
            record.DataTime = value['DataTime'];

            // check if same time, let 2:out in the front
            if ( CurrentTag_Records.length > 0 )
            {
                var pre_record = CurrentTag_Records[ CurrentTag_Records.length - 1];

                if ( pre_record.DataTime === record.DataTime )
                {
                    if ( pre_record.action === '2' && record.action === '1')
                    {
                        // switch

                        // console.log("show_detail_areainout() switch pre_record.AreaName:" + pre_record.AreaName + ", record.AreaName:"+record.AreaName);

                        CurrentTag_Records[ CurrentTag_Records.length - 1] = record;
                        record = pre_record;
                    }
                    else
                    {
                        // do nothing
                    }
                }
            }

            CurrentTag_Records.push(record);
        });

        // console.log("show_detail_areainout() CurrentTag_Records.length:"+CurrentTag_Records.length);

        put_record_to_table(uiid, glCurrentTag_nodeid, CurrentTag_Records);

    });
}

function show_detail_gps_heatmap(uiid, target_date)
{
    // console.log("show_detail_gps_heatmap() uiid:"+uiid+" target_date:"+target_date);

    //
    // check if already existed
    //
    var heatmap_id = "record_box_heatmap_gps_"+uiid;
    var heatmap_ui = document.getElementById(heatmap_id);
    if (heatmap_ui != undefined)
    {
        console.log("show_detail_gps_heatmap() heatmap_id:"+heatmap_id +", existed!");
        return;
    }

    var CurrentTag_Records = [];

    var targetStartDate = getTargetDateTime(target_date, 0);
    var targetEndDate = getTargetDateTime(target_date, 1);
    var offset = 0;
    var count = 30000;

    var targetURL = gl_target_server + "/php/fpnodes.php";
    $.getJSON(targetURL, 'loadgpsnodedata=1&project_id='+project_id+'&device='+glCurrentTag_nodeid+'&nodetype='+glCurrentNode_type+'&startdate='+targetStartDate+'&enddate='+targetEndDate+'&offset='+offset+'&count='+count, function(data)
    {
        $.each(data, function(key, value)
        {
            var record = {};

            record.Lng = value['Lng'];
            record.Lat = value['Lat'];
            record.time = value['time'];

            CurrentTag_Records.push(record);
        });

        // console.log("show_detail_gps_heatmap() CurrentTag_Records.length:"+CurrentTag_Records.length);

        put_record_to_gps_heatmap(uiid, 0, glCurrentTag_nodeid, CurrentTag_Records);

    });
}

function show_detail_course(uiid, target_date, areaid, in_min_time, out_max_time)
{
    // console.log("show_detail_course() areaid:"+areaid +", in_min_time:"+in_min_time+", out_max_time:"+out_max_time);

    //
    // check if already existed
    //
    var table_id = "record_box_table_course_"+uiid;
    var table_ui = document.getElementById(table_id);
    if (table_ui != undefined)
    {
        console.log("show_detail_course() table_id:"+table_id +", existed!");
        return;
    }

    var CurrentTag_Records = [];
    var offset = 0;
    var count = 300;

    var targetURL = gl_target_server + "/php/fpnodes.php";
    $.getJSON(targetURL, 'loadareainout=1&project_id='+project_id+'&device='+glCurrentTag_nodeid+'&areaid='+areaid+'&startdate='+in_min_time+'&enddate='+out_max_time+'&offset='+offset+'&count='+count, function(data)
    {
        $.each(data, function(key, value)
        {
            var record = {};

            record.AreaType = value['AreaType'];
            record.action = value['action']; //1:in 2:out
            record.AreaID = value['AreaID'];
            record.AreaName = value['AreaName'];
            record.DataTime = value['DataTime'];

            // check if same time, let 2:out in the front
            if ( CurrentTag_Records.length > 0 )
            {
                var pre_record = CurrentTag_Records[ CurrentTag_Records.length - 1];

                if ( pre_record.DataTime === record.DataTime )
                {
                    if ( pre_record.action === '2' && record.action === '1')
                    {
                        // switch

                        // console.log("show_detail_course() switch pre_record.AreaName:" + pre_record.AreaName + ", record.AreaName:"+record.AreaName);

                        CurrentTag_Records[ CurrentTag_Records.length - 1] = record;
                        record = pre_record;
                    }
                    else
                    {
                        // do nothing
                    }
                }
            }

            CurrentTag_Records.push(record);
        });

        // console.log("show_detail_course() CurrentTag_Records.length:"+CurrentTag_Records.length);

        put_record_to_table(uiid, glCurrentTag_nodeid, CurrentTag_Records);

    });
}

function show_detail_alert(uiid, target_date)
{
    // console.log("show_detail_alert() uiid:"+uiid +", target_date:"+target_date);

    //
    // check if already existed
    //
    var table_id = "record_box_table_alert_"+uiid;
    var table_ui = document.getElementById(table_id);
    if (table_ui != undefined)
    {
        console.log("show_detail_alert() table_id:"+table_id +", existed!");
        return;
    }

    var targetStartDate = getTargetDateTime(target_date, 0);
    var targetEndDate = getTargetDateTime(target_date, 1);
    var CurrentTag_Records = [];
    var offset = 0;
    var count = 300;

    var targetURL = gl_target_server + "/php/fpnodes.php";
    $.getJSON(targetURL, 'loadalert=1&project_id='+project_id+'&device='+glCurrentTag_nodeid+'&startdate='+targetStartDate+'&enddate='+targetEndDate+'&offset='+offset+'&count='+count, function(data)
    {
        $.each(data, function(key, value)
        {
            var record = {};

            record.alertid = value['alertid'];
            record.nodetype = value['nodetype'];
            record.time = value['time'];

            CurrentTag_Records.push(record);

        });

        // console.log("show_detail_course() CurrentTag_Records.length:"+CurrentTag_Records.length);

        put_alert_to_table(uiid, glCurrentTag_nodeid, CurrentTag_Records);
    });

}

function show_detail_event(uiid, target_date)
{
    // console.log("show_detail_event() uiid:"+uiid +", target_date:"+target_date);

    //
    // check if already existed
    //
    var table_id = "record_box_table_event_"+uiid;
    var table_ui = document.getElementById(table_id);
    if (table_ui != undefined)
    {
        console.log("show_detail_event() table_id:"+table_id +", existed!");
        return;
    }

    var targetStartDate = getTargetDateTime(target_date, 0);
    var targetEndDate = getTargetDateTime(target_date, 1);
    var CurrentTag_Records = [];
    var offset = 0;
    var count = 300;

    var targetURL = gl_target_server + "/php/fpnodes.php";
    $.getJSON(targetURL, 'loadevent=1&project_id='+project_id+'&device='+glCurrentTag_nodeid+'&startdate='+targetStartDate+'&enddate='+targetEndDate+'&offset='+offset+'&count='+count, function(data)
    {
        $.each(data, function(key, value)
        {
            var record = {};

            record.eventid = value['eventid'];
            record.eventjson = value['eventjson'];
            record.nodeid = value['nodeid'];
            record.areaid = value['areaid'];
            record.action = value['action'];
            record.issuetime = value['issuetime'];
            record.nodename = value['nodename'];
            record.areaname = value['areaname'];
            record.issuename = value['issuename'];
            record.time = value['time'];

            if (record.eventid === '999')
            {
                record.TagNodeId = value['TagNodeId'];
                record.GroupID = value['GroupID'];
                record.TagNodeName = value['TagNodeName'];
                record.NodeIdName = value['NodeIdName'];
                record.nodename = value['nodename'];
                record.nodename = value['nodename'];
            }

            CurrentTag_Records.push(record);

        });

        // console.log("show_detail_event() CurrentTag_Records.length:"+CurrentTag_Records.length);

        put_event_to_table(uiid, glCurrentTag_nodeid, CurrentTag_Records);
    });

}

function put_coord_to_heatmap(uiid, nodeid, records)
{
    var target_id = "timeline_footer_" + uiid;
    var body_timeline = document.getElementById(target_id);

    var map_nav_tabs_id = "map_nav_tabs_" + uiid;
    var map_tab_content_id = "map_tab_content_" + uiid;

    var map_nav_tabs = document.getElementById(map_nav_tabs_id);
    if ( map_nav_tabs == undefined )
    {
        var nav_tabs_custom = document.createElement("div");
        nav_tabs_custom.setAttribute("class","nav-tabs-custom");
        body_timeline.appendChild(nav_tabs_custom);

        var map_nav_tabs = document.createElement("ul");
        map_nav_tabs.setAttribute("class","nav nav-tabs");
        map_nav_tabs.setAttribute("id","map_nav_tabs_" + uiid);
        nav_tabs_custom.appendChild(map_nav_tabs);

        var map_tab_content = document.createElement("div");
        map_tab_content.setAttribute("class","tab-content");
        map_tab_content.setAttribute("id","map_tab_content_" + uiid);
        nav_tabs_custom.appendChild(map_tab_content);
    }

    // var table_ui = create_record_heatmap_gps(uiid);
    // body_timeline.appendChild(table_ui);
    //
    // var gps_heatmap = new HeatMap("record_box_heatmap_gps_"+uiid);

    var targetURL = gl_target_server + "/php/map.php";
    $.getJSON(targetURL, 'loadmap=1'+'&project_id='+project_id, function(mapsdata)
    {

        var map_nav_tabs = document.getElementById(map_nav_tabs_id);
        while (map_nav_tabs.firstChild)
        {
            map_nav_tabs.removeChild(map_nav_tabs.firstChild);
        }
        var map_tab_content = document.getElementById(map_tab_content_id);
        while (map_tab_content.firstChild)
        {
            map_tab_content.removeChild(map_tab_content.firstChild);
        }

        var count = 0;
        $.each(mapsdata, function(index, mapdata)
        {
            var m_records = [];

            var cur_record = records[mapdata.layerid];
            if (cur_record == undefined )
            {
                cur_record = records["0"];
            }

            if (cur_record != undefined )
            {
                m_records = cur_record.map(
                    function (p)
                    {
                        //console.log("put_coord_to_heatmap() p.posY:"+p.posY+", p.posX:"+p.posX);

                        var NodeLatitude = parseFloat(mapdata.WGS48OriginY) + centimeterToLatitude(parseFloat(p.posY));
                        var NodeLongitude = parseFloat(mapdata.WGS48OriginX) + centimeterToLongitude(NodeLatitude, parseFloat(p.posX));

                        //console.log("put_coord_to_heatmap() NodeLatitude:"+NodeLatitude+", NodeLongitude:"+NodeLongitude);

                        return [NodeLatitude, NodeLongitude];
                    });
            }

            if (m_records.length == 0)
            {
                m_records.push([parseFloat(mapdata.WGS48OriginY), parseFloat(mapdata.WGS48OriginX)]);
            }

            // console.log("put_coord_to_heatmap() uiid:"+uiid+" index:"+index+" m_records.length:"+m_records.length);

            add_map(uiid, index, mapdata, m_records);

            count++;
        });

        /*
        var data = mapsdata[0];

        var targetURL = gl_target_server + "/php/configvils.php";
        $.getJSON(targetURL, 'loadprojectconfig=1'+'&project_id='+project_id , function(configdata)
        {
            var m_records = records.map(
                function (p)
                {
                    //console.log("put_coord_to_heatmap() p.posY:"+p.posY+", p.posX:"+p.posX);

                    var NodeLatitude = parseFloat(configdata.WGS48OriginY) + centimeterToLatitude(parseFloat(p.posY));
                    var NodeLongitude = parseFloat(configdata.WGS48OriginX) + centimeterToLongitude(NodeLatitude, parseFloat(p.posX));

                    //console.log("put_coord_to_heatmap() NodeLatitude:"+NodeLatitude+", NodeLongitude:"+NodeLongitude);

                    return [NodeLatitude, NodeLongitude];
                });

            gps_heatmap.setMapCoeff(data.POSX, data.POSY, data.MAPWIDTH, data.MAPHEIGHT, data.SCALE, data.COEFF, configdata.WGS48OriginX, configdata.WGS48OriginY, configdata.MAP_North);
            gps_heatmap.setMapImage(data.FILE, "Indoor 2D Map");
            gps_heatmap.addRecords(m_records);

            update_footer_btn1_ui(uiid, "讀取完成");

        });
        */
        update_footer_btn1_ui(uiid, "讀取完成");
    });
}


function add_map(uiid, ui_index, mapdata, m_records)
{
    var new_tab = create_map_tab(uiid, ui_index, mapdata);

    var map_nav_tabs = document.getElementById("map_nav_tabs_" + uiid);
    map_nav_tabs.appendChild(new_tab);

    var new_content = create_map_content(uiid, ui_index, mapdata);

    var map_tab_content = document.getElementById("map_tab_content_" + uiid);
    map_tab_content.appendChild(new_content);

    //var table_ui = create_record_heatmap_gps(uiid, ui_index);
    //new_content.appendChild(table_ui);

    // console.log("add_map() uiid:"+uiid+" ui_index:"+ui_index+" m_records.length:"+m_records.length);

    //var id = uiid + "_" + ui_index;
    var mapid = "mapid_"+ uiid + "_" + ui_index;

    glMapDataList[mapid] = mapdata;
    glRecordDataList[mapid] = m_records;

    //if (ui_index == 0)
    // {
        //var gps_heatmap = new HeatMap("record_box_heatmap_gps_"+uiid+"_"+ui_index);
        var gps_heatmap = new HeatMap(mapid);

        // console.log("add_map() MAPWIDTH:"+mapdata.MAPWIDTH);

        gps_heatmap.setMapCoeff(mapdata.POSX, mapdata.POSY, mapdata.MAPWIDTH, mapdata.MAPHEIGHT, mapdata.SCALE, mapdata.COEFF, mapdata.WGS48OriginX, mapdata.WGS48OriginY, mapdata.MAP_North);
        gps_heatmap.setMapImage(mapdata.FILE, "Indoor 2D Map");
        gps_heatmap.addRecords(m_records);

        //glHeatMapList.push(gps_heatmap);
        glHeatMapList[mapid] = gps_heatmap;
    // }
}

function map_tab_onclick(uiid, ui_index, layerid, layer_name)
{
    //glCurrentMapIndex = ui_index;

    //var id = uiid + "_" + ui_index;
    var mapid = "mapid_"+ uiid + "_" + ui_index;

    // console.log("map_tab_onclick() mapid:"+mapid);

    var map = glHeatMapList[mapid];

    if ( map != undefined )
    {
        map.refresh();
    }
}

function create_map_tab(uiid, ui_index, mapdata)
{
    var new_tab = document.createElement('li');
    new_tab.setAttribute("id","map_tab_li_"+uiid+"_"+ui_index);
    if (ui_index == 0)
    {
        new_tab.setAttribute("class","active");
    }

    var ahref = document.createElement('a');
    ahref.setAttribute("href","#map_tab_"+uiid+"_"+ui_index);
    ahref.setAttribute("data-toggle","tab");
    ahref.setAttribute("id","map_tab_href_"+uiid+"_"+ui_index);

    ahref.setAttribute("onclick","map_tab_onclick(\""+ uiid +"\",\""+ ui_index +"\", \""+ mapdata.layerid +"\", \""+ mapdata.layer_name +"\");return false;");
    var span = document.createElement("span");
    span.setAttribute("class","badge bg-blue-active");
    span.setAttribute("id","layer_name_"+uiid+"_"+ui_index);
    span.textContent = mapdata.layer_name;
    ahref.appendChild(span);
    new_tab.appendChild(ahref);

    return new_tab;
}

function create_map_content(uiid, ui_index, mapdata)
{
    var divTabPane = document.createElement('div');
    if (ui_index == 0)
        divTabPane.setAttribute("class","tab-pane active");
    else
        divTabPane.setAttribute("class","tab-pane");
    divTabPane.setAttribute("id","map_tab_"+uiid+"_"+ui_index);

    //
    // layer info
    //
    var div_layer_info = document.createElement('div');
    divTabPane.appendChild(div_layer_info);

    var div_desc = document.createElement('strong');
    div_desc.innerHTML = mapdata.layer_description + '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    div_desc.setAttribute("style","font-weight:bold;");
    div_layer_info.appendChild(div_desc);

    var div_GPS_icon = document.createElement('i');
    div_GPS_icon.setAttribute("class","fa fa-map-marker margin-r-5");
    div_GPS_icon.setAttribute("style","color:#1a1;");
    div_layer_info.appendChild(div_GPS_icon);

    var div_GPS_X = document.createElement('strong');
    div_GPS_X.innerHTML = mapdata.WGS48OriginX +'&nbsp;&nbsp;&nbsp';
    div_GPS_X.setAttribute("style","color:#1a1;");
    div_layer_info.appendChild(div_GPS_X);

    var div_GPS_Y = document.createElement('strong');
    div_GPS_Y.innerHTML = mapdata.WGS48OriginY +'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    div_GPS_Y.setAttribute("style","color:#1a1;");
    div_layer_info.appendChild(div_GPS_Y);

    var div_North_icon = document.createElement('i');
    div_North_icon.setAttribute("class","fa fa-compass margin-r-5");
    div_North_icon.setAttribute("style","color:#811;");
    div_layer_info.appendChild(div_North_icon);

    var div_North = document.createElement('strong');
    div_North.innerHTML = mapdata.MAP_North +'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    div_North.setAttribute("style","color:#822;");
    div_layer_info.appendChild(div_North);

    var div_building = document.createElement('strong');
    div_building.innerHTML = 'Building:&nbsp;' + mapdata.BuildingID +'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;';
    div_building.setAttribute("style","color:#328;");
    div_layer_info.appendChild(div_building);

    var div_floor = document.createElement('strong');
    div_floor.innerHTML = 'Floor:&nbsp;' + mapdata.BuildingFloor;
    div_floor.setAttribute("style","color:#328;");
    div_layer_info.appendChild(div_floor);

    //
    // map layer
    //
    var pad = document.createElement('pad');
    pad.setAttribute("class","pad");
    divTabPane.appendChild(pad);
    var div_map = document.createElement('div');
    div_map.setAttribute("id","mapid_"+uiid+"_"+ui_index);
    div_map.setAttribute("style","height:85vh");
    pad.appendChild(div_map);

    return divTabPane;
}

function put_record_to_table(uiid, nodeid, records)
{
    var pre_record = null;

    var target_id = "timeline_footer_" + uiid;
    var body_timeline = document.getElementById(target_id);

    var table_ui = create_record_table_areainout(uiid);
    body_timeline.appendChild(table_ui);

    var rows = records.length;
    var table_height = (rows + 1) * 50;
    if (rows > 5)
        table_height = 300;

    table_ui.setAttribute("style", "height:"+table_height+"px;overflow:scroll");

    var table_body_id = "record_table_areainout_body_" + uiid;
    var table_body = document.getElementById(table_body_id);

    var ui_index = 0;
    for(index in records)
    {
        var record = records[index];
        //console.log("put_record_to_table() AreaType:"+record.AreaType+", action:"+record.action+", AreaName:"+record.AreaName+", DataTime:"+record.DataTime);

        if ( pre_record == null )
        {
            pre_record = record;
        }
        else
        {
            if ( pre_record.AreaID === record.AreaID )
            {
                if ( pre_record.DataTime === record.DataTime )
                {
                    var row = create_area_inout_row(ui_index, pre_record, record);
                    table_body.appendChild(row);
                    ui_index++;

                    pre_record = null; // clear record
                }
                else
                if ( pre_record.action === '2' && record.action === '1' )
                {
                    var row = create_area_inout_row(ui_index, pre_record, record);
                    table_body.appendChild(row);
                    ui_index++;

                    pre_record = null; // clear record
                }
                else
                {
                    // creat a row for single event, only in or out
                    var row = create_area_row(ui_index, pre_record);
                    table_body.appendChild(row);
                    ui_index++;

                    pre_record = record;
                }
            }
            else
            {
                // creat a row for single event, only in or out

                var row = create_area_row(ui_index, pre_record);
                table_body.appendChild(row);
                ui_index++;

                pre_record = record;
            }
        }
    }

    if ( pre_record != null )
    {
        var row = create_area_row(ui_index, pre_record);
        table_body.appendChild(row);
        ui_index++;
    }

    update_footer_btn1_ui(uiid, "讀取完成");

    if (ui_index == 0)
    {
        var nodata_record = {};
        nodata_record.AreaType = 0;
        nodata_record.action = 0; //1:in 2:out
        nodata_record.AreaID = 0;
        nodata_record.AreaName = '無資料';
        nodata_record.DataTime = "";
        var row = create_area_inout_row(0, nodata_record, nodata_record);
        table_body.appendChild(row);
        return;
    }

}

function put_record_to_gps_heatmap(uiid, ui_index, nodeid, records)
{
    var target_id = "timeline_footer_" + uiid;
    var body_timeline = document.getElementById(target_id);

    var table_ui = create_record_heatmap_gps(uiid, ui_index);
    body_timeline.appendChild(table_ui);

    var gps_heatmap = new HeatMap("record_box_heatmap_gps_"+uiid+"_"+ui_index);

    var m_records = records.map(function (p) { return [p['Lat'], p['Lng']]; });

    gps_heatmap.addRecords(m_records);

    var record_time_pre = '';
    var point_pre = [];
    var points = [];
    var datetimes = [];
    for(var index=0;index<records.length;index++)
    {
        var record = records[index];
        var point = [record['Lat'], record['Lng']];

        var record_time = moment(record['time'], "YYYY-MM-DD HH:mm:ss");
        if ( record_time_pre != '' )
        {
            var diff_mins = record_time_pre.diff(record_time, 'minutes');
            if ( diff_mins >= 5 )
            {

                // console.log("put_record_to_gps_heatmap() diff_mins:"+diff_mins);

                //
                // create new line
                //
                gps_heatmap.addLineRecords(points, datetimes);

                //
                // drae dash line
                //
                var dashpoints = [];
                dashpoints.push(point_pre);
                dashpoints.push(point);
                gps_heatmap.addDashLine(dashpoints, datetimes);

                points = [];
                datetimes = [];
            }
            else
            {
                var distKM = calcCrow(point_pre[0], point_pre[1], point[0], point[1]);
                // console.log("put_record_to_gps_heatmap() distKM:"+distKM);
                if ( distKM > 0.5 )
                {

                    //
                    // create new line
                    //
                    gps_heatmap.addLineRecords(points, datetimes);

                    //
                    // drae dash line
                    //
                    var dashpoints = [];
                    dashpoints.push(point_pre);
                    dashpoints.push(point);
                    gps_heatmap.addWarningDashLine(dashpoints, datetimes);

                    points = [];
                    datetimes = [];
                }
            }
        }

        points.push(point);
        datetimes.push(record['time']);

        record_time_pre = record_time;
        point_pre = point;
    }
    if (points.length > 0)
        gps_heatmap.addLineRecords(points, datetimes);


    update_footer_btn1_ui(uiid, "讀取完成");

}

function put_alert_to_table(uiid, nodeid, records)
{
    var target_id = "timeline_footer_" + uiid;
    var body_timeline = document.getElementById(target_id);

    var table_ui = create_record_table_alert(uiid);
    body_timeline.appendChild(table_ui);

    var rows = records.length;
    var table_height = (rows + 1) * 50;
    if (rows > 5)
        table_height = 300;

    table_ui.setAttribute("style", "height:"+table_height+"px;overflow:scroll");

    var table_body_id = "record_table_alert_body_" + uiid;
    var table_body = document.getElementById(table_body_id);

    var ui_index = 0;
    for(index in records)
    {
        var record = records[index];
        var row = create_alert_row(ui_index, record);
        table_body.appendChild(row);
        ui_index++;
    }

    update_footer_btn1_ui(uiid, "讀取完成");
}

function put_event_to_table(uiid, nodeid, records)
{
    var target_id = "timeline_footer_" + uiid;
    var body_timeline = document.getElementById(target_id);

    var table_ui = create_record_table_event(uiid);
    body_timeline.appendChild(table_ui);

    var rows = records.length;
    var table_height = (rows + 1) * 50;
    if (rows > 5)
        table_height = 300;

    table_ui.setAttribute("style", "height:"+table_height+"px;overflow:scroll");

    var table_body_id = "record_table_event_body_" + uiid;
    var table_body = document.getElementById(table_body_id);

    var ui_index = 0;
    for(index in records)
    {
        var record = records[index];
        var row = create_event_row(ui_index, record);
        table_body.appendChild(row);
        ui_index++;
    }

    update_footer_btn1_ui(uiid, "讀取完成");
}

function update_footer_btn1_ui(uiid, ui_msg)
{
    var footer_btn1_ui = document.getElementById("timeline_footer_btn1_" + uiid);

    if (footer_btn1_ui != undefined)
    {
        footer_btn1_ui.textContent = ui_msg;
    }
}


function create_record_table_areainout(uiid)
{
    var divTabPane = document.createElement('div');
    divTabPane.setAttribute("id","record_box_table_areainout_"+uiid);

    //
    // Box
    //
    var boxGroup = document.createElement('div');
    boxGroup.setAttribute("class","box");
    divTabPane.appendChild(boxGroup);

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
    theadthAreaName.textContent = '區域名稱';
    theadtrGroup.appendChild(theadthAreaName);

    var theadth_EnterTime = document.createElement("th");
    theadth_EnterTime.setAttribute("style","width: 200px; text-align:center");
    theadth_EnterTime.textContent = '進入時間';
    theadtrGroup.appendChild(theadth_EnterTime);

    var theadth_LeaveTime = document.createElement("th");
    theadth_LeaveTime.setAttribute("style","width: 200px; text-align:center");
    theadth_LeaveTime.textContent = '離開時間';
    theadtrGroup.appendChild(theadth_LeaveTime);

    var theadth_StayTime = document.createElement("th");
    //theadth_StayTime.setAttribute("style","width: 50px; text-align:center");
    theadth_StayTime.textContent = '停留時間';
    theadtrGroup.appendChild(theadth_StayTime);

    var tbodyGroup = document.createElement("tbody");
    tbodyGroup.setAttribute("id","record_table_areainout_body_" + uiid);
    tableGroup.appendChild(tbodyGroup);

    return divTabPane;
}

function create_record_heatmap_gps(uiid, ui_index)
{
    var divTabPane = document.createElement('div');
    divTabPane.setAttribute("id","record_box_heatmap_gps_"+uiid+"_"+ui_index);
    divTabPane.setAttribute("style","height:500px");

    return divTabPane;
}

function create_area_inout_row(ui_index, pre_record, record)
{
    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","table_row_"+ui_index);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    // Area
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(pre_record.AreaName));
    cellIndex++;

    // Enter Time
    var table_td_entertime_td = document.createElement('td')
    row.appendChild(table_td_entertime_td);
    var table_td_entertime_span = document.createElement('span');
    table_td_entertime_span.setAttribute("class",'badge bg-light-blue');
    table_td_entertime_span.textContent = record.DataTime;
    table_td_entertime_td.appendChild(table_td_entertime_span);
    cellIndex++;

    // Leave Time
    var table_td_leaveltime_td = document.createElement('td')
    row.appendChild(table_td_leaveltime_td);
    var table_td_leaveltime_span = document.createElement('span');
    table_td_leaveltime_span.setAttribute("class",'badge bg-purple');
    table_td_leaveltime_span.textContent = pre_record.DataTime;
    table_td_leaveltime_td.appendChild(table_td_leaveltime_span);
    cellIndex++;

    var stayTime = moment.utc(moment(pre_record.DataTime,"YYYY-MM-DD HH:mm:ss").diff(moment(record.DataTime,"YYYY-MM-DD HH:mm:ss"))).format("HH:mm:ss")

    // stay Time
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(stayTime));
    cellIndex++;

    return row;
}

function create_area_row(ui_index, record)
{
    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","table_row_"+ui_index);

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

    return row;
}

function create_record_table_alert(uiid)
{
    var divTabPane = document.createElement('div');
    divTabPane.setAttribute("id","record_box_table_alert_"+uiid);

    //
    // Box
    //
    var boxGroup = document.createElement('div');
    boxGroup.setAttribute("class","box");
    divTabPane.appendChild(boxGroup);

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

    var theadth_AlertTime = document.createElement("th");
    theadth_AlertTime.setAttribute("style","width: 150px; text-align:center");
    theadth_AlertTime.textContent = '發出求助時間';
    theadtrGroup.appendChild(theadth_AlertTime);

    // empty
    var theadth = document.createElement("th");
    theadtrGroup.appendChild(theadth);

    var tbodyGroup = document.createElement("tbody");
    tbodyGroup.setAttribute("id","record_table_alert_body_" + uiid);
    tableGroup.appendChild(tbodyGroup);

    return divTabPane;

}

function create_alert_row(ui_index, record)
{
    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","table_row_"+ui_index);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    var table_td_alerttime_td = document.createElement('td')
    row.appendChild(table_td_alerttime_td);
    var table_td_alerttime_span = document.createElement('span');
    table_td_alerttime_span.setAttribute("class",'badge bg-light-blue');
    table_td_alerttime_span.textContent = record.time;
    table_td_alerttime_td.appendChild(table_td_alerttime_span);
    cellIndex++;

    // empty
    row.appendChild(document.createElement('td'));
    cellIndex++;

    return row;
}

function create_record_table_event(uiid)
{
    var divTabPane = document.createElement('div');
    divTabPane.setAttribute("id","record_box_table_event_"+uiid);

    //
    // Box
    //
    var boxGroup = document.createElement('div');
    boxGroup.setAttribute("class","box");
    divTabPane.appendChild(boxGroup);

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

    var theadth_AlertTime = document.createElement("th");
    theadth_AlertTime.setAttribute("style","width: 150px; text-align:center");
    theadth_AlertTime.textContent = '觸發事件時間';
    theadtrGroup.appendChild(theadth_AlertTime);


    var theadth_Event = document.createElement("th");
    theadth_Event.setAttribute("style","text-align:left");
    theadth_Event.textContent = '事件';
    theadtrGroup.appendChild(theadth_Event);

    // empty
    // var theadth = document.createElement("th");
    // theadtrGroup.appendChild(theadth);

    var tbodyGroup = document.createElement("tbody");
    tbodyGroup.setAttribute("id","record_table_event_body_" + uiid);
    tableGroup.appendChild(tbodyGroup);

    return divTabPane;

}

function create_event_row(ui_index, record)
{
    var cellIndex = 0;
    var row = document.createElement('tr');
    if (ui_index % 2 == 0)
        row.setAttribute("bgcolor","#ebebe0");
    row.setAttribute("id","table_row_"+ui_index);

    // index
    row.appendChild(document.createElement('td'));
    row.cells[cellIndex].appendChild(document.createTextNode(""+ui_index));
    cellIndex++;

    var table_td_alerttime_td = document.createElement('td')
    row.appendChild(table_td_alerttime_td);
    var table_td_alerttime_span = document.createElement('span');
    table_td_alerttime_span.setAttribute("class",'badge bg-light-blue');
    table_td_alerttime_span.textContent = record.time;
    table_td_alerttime_td.appendChild(table_td_alerttime_span);
    cellIndex++;

    var table_td_event_td = document.createElement('td')

    console.log("create_event_row() ui_index:"+ui_index+", record.eventid:"+record.eventid);

    if ( record.eventid === '999' )
    {
        var table_td_TagNodeName_span = document.createElement('span');
        table_td_TagNodeName_span.setAttribute("class",'badge bg-green');
        table_td_TagNodeName_span.textContent = record.TagNodeName;
        table_td_event_td.appendChild(table_td_TagNodeName_span);

        table_td_event_td.appendChild(document.createTextNode("   " + convert_action_name(record.action) + "   巡檢點"));

        var table_td_NodeIdName_span = document.createElement('span');
        table_td_NodeIdName_span.setAttribute("class",'badge bg-yellow');
        table_td_NodeIdName_span.textContent = record.NodeIdName;
        table_td_event_td.appendChild(table_td_NodeIdName_span);
    }
    else
    {
        var new_html = record.nodename+' ';
        new_html += convert_action_name(record.action)+' ';
        new_html += record.areaname+' ';
        new_html += convert_IssueTime_name(record.issuename)+' ';

        table_td_event_td.textContent = new_html;
    }

    row.appendChild(table_td_event_td);
    // var table_td_event_span = document.createElement('span');
    // table_td_event_span.setAttribute("class",'badge bg-light-blue');
    // table_td_event_span.textContent = record.eventjson;
    // table_td_event_td.appendChild(table_td_event_span);
    cellIndex++;


    // empty
    // row.appendChild(document.createElement('td'));
    // cellIndex++;

    return row;
}


function move_daily_forward()
{
    var cur_date = moment(glTargetDate, "YYYY-MM-DD");
    var target_date = cur_date.add(1, 'day').format('YYYY-MM-DD');

    glTargetDate = target_date;
    //console.log("move_date_forward() GroupID:"+GroupID +", today:"+today +", target_date:"+target_date);

    var status_date = document.getElementById('daily_targetdate');
    if (status_date != undefined)
    {
        status_date.textContent = glTargetDate;
        //status_date.textContent = convertToShowDate(glTargetDate);
    }

    // update UI
    load_device_record();
}

function move_daily_backward()
{
    var cur_date = moment(glTargetDate, "YYYY-MM-DD");
    var target_date = cur_date.subtract(1, 'day').format('YYYY-MM-DD');

    glTargetDate = target_date;
    //console.log("move_date_forward() GroupID:"+GroupID +", today:"+today +", target_date:"+target_date);

    var status_date = document.getElementById('daily_targetdate');
    if (status_date != undefined)
    {
        status_date.textContent = glTargetDate;
        //status_date.textContent = convertToShowDate(glTargetDate);
    }

    // update UI
    load_device_record();
}


//Date picker
$('#daily_targetdate').datepicker({
  autoclose: true,
  format:"YYYY-MM-DD",
  language:"zh-TW"
}).on('changeDate', function (selected)
{
    var minDate = new Date(selected.date.valueOf());
    glTargetDate = moment(minDate).format('YYYY-MM-DD');
    //console.log("#daily_targetdate changeDate() target_date:"+target_date);
    var status_date = document.getElementById('daily_targetdate');
    if (status_date != undefined)
    {
        status_date.textContent = glTargetDate;
    }

    // update UI
    load_device_record();
});

function add_search_tag_to_table(searchtext)
{
    if ( searchtext == undefined )
        return;

    var search_list_content = document.getElementById('search_list_content');

    while (search_list_content.rows.length >= 1)
    {
        search_list_content.deleteRow(0);
    }

    var searchtext_lc = searchtext.toLowerCase();

    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var targetname = node_name.toLowerCase();
        var targetid = node_nodeid.toLowerCase();

        var ret = targetname.indexOf(searchtext_lc);
        if ( ret < 0 )
        {
            ret = targetid.indexOf(searchtext_lc);
            if ( ret < 0 )
            {
                continue;
            }
        }

        var table_tr = document.createElement('tr');
        search_list_content.appendChild(table_tr);

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

    for (var i = 0; i < glLocator_List.length; i++)
    {
        var nodeData = glLocator_List[i];
        var node_name = nodeData[5];
        var node_nodeid = nodeData[3];

        var ret = node_name.indexOf(searchtext);
        if ( ret < 0 )
            continue;

        var table_tr = document.createElement('tr');
        search_list_content.appendChild(table_tr);

        var table_td_2 = document.createElement('td');
        table_td_2.setAttribute("id","id_row_node_name_"+ node_nodeid);
        table_td_2.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_tr.appendChild(table_td_2);

        // node id
        var tag_nodeid_span = document.createElement('span');
        tag_nodeid_span.setAttribute("class",'badge bg-yellow-active');
        tag_nodeid_span.textContent = node_name;
        tag_nodeid_span.setAttribute("onclick","click_locator(\""+ node_nodeid +"\");return false;");
        table_td_2.appendChild(tag_nodeid_span);
    }

    for (var i = 0; i < glUser_List.length; i++)
    {
        var userData = glUser_List[i];

        var ret = userData.name.indexOf(searchtext);
        if ( ret < 0 )
            continue;

        var table_tr = document.createElement('tr');
        search_list_content.appendChild(table_tr);

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

    for (var i = 0; i < glSensor_List.length; i++)
    {
        var sensorData = glSensor_List[i];

        var ret = sensorData.name.indexOf(searchtext);
        if ( ret < 0 )
            continue;

        var table_tr = document.createElement('tr');
        search_list_content.appendChild(table_tr);

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

function loc_tab_search_text_change(e)
{
    var input_value = document.getElementById("loc_tab_search_text").value;

     //console.log("input_value:"+input_value);

     add_search_tag_to_table(input_value);

     setCookie("footprinttags_search_text", input_value, 10);
}

function check_cookies()
{
    var input_value = getCookie("footprinttags_search_text");

    document.getElementById("loc_tab_search_text").value = input_value;

    add_search_tag_to_table(input_value);
}

const $source = document.querySelector('#loc_tab_search_text');
$source.addEventListener('input', loc_tab_search_text_change);// register for oninput

function setCookie(name, value, days)
{
    var expires = "";
    if (days)
    {
        var date = new Date();
        date.setTime(date.getTime() + (days*24*60*60*1000));
        expires = "; expires=" + date.toUTCString();
    }
    document.cookie = name + "=" + (value || "")  + expires + "; path=/";
}

function getCookie(name)
{
    var nameEQ = name + "=";
    var ca = document.cookie.split(';');
    for(var i=0;i < ca.length;i++)
    {
        var c = ca[i];
        while (c.charAt(0)==' ')
            c = c.substring(1,c.length);
        if (c.indexOf(nameEQ) == 0)
        {
            return c.substring(nameEQ.length,c.length);
        }
    }
    return null;
}

function eraseCookie(name)
{
    document.cookie = name+'=; Max-Age=-99999999;';
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
        for (var i = 0; i < glLocator_List.length; i++)
        {
            var nodeData = glLocator_List[i];
            var node_name = nodeData[5];
            var node_nodeid = nodeData[3];

            if ( nodeid === node_nodeid )
            {
                nodename = node_name;
            }
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

function click_show_target_date()
{
    console.log("click_show_target_date()");

    var checkbox_ui = document.getElementById("show_target_date");
    if (checkbox_ui == undefined)
    {
        console.log("click_show_target_date() checkbox_ui == undefined");
        return;
    }

    var checked = checkbox_ui.checked;

    if (checked)
    {
        glShowDataMode = 1; // 0: timeline 1: one day
        document.getElementById("daily_pdf_report_UI").style.display = 'block';
    }
    else
    {
        glShowDataMode = 0; // 0: timeline 1: one day
        document.getElementById("daily_pdf_report_UI").style.display = 'none';
    }

    // update UI
    load_device_record();
}

function get_daily_pdf_report()
{
    var daily_pdf_report_status = document.getElementById("daily_pdf_report_status");
    daily_pdf_report_status.textContent = "產生報表中...";

    var SendDate = "loadtagdailypdfreport=1&id="+glCurrentTag_nodeid+"&project_id="+project_id+"&date=" + glTargetDate;

    var targetURL = gl_target_server + "/php/configvils.php";

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
            console.log("get_pdf_report() success:"+result.success+" pdfreport:"+result.pdfreport);

            var pdf_report_dl_link = document.getElementById("daily_pdf_report_status");
            while (pdf_report_dl_link.firstChild)
            {
                pdf_report_dl_link.removeChild(pdf_report_dl_link.firstChild);
            }

            if ( result.success === 'true')
            {
                var daily_pdf_report_dl_btn = document.getElementById("daily_pdf_report_dl_btn");
                daily_pdf_report_dl_btn.setAttribute("style", "display:block");
                daily_pdf_report_dl_btn.setAttribute("onclick", "location.href='" +result.pdfreport +"'");

                var daily_pdf_report_gen_btn = document.getElementById("daily_pdf_report_gen_btn");
                daily_pdf_report_gen_btn.setAttribute("style", "display:none");


                // var dl_link = document.createElement("a");
                // dl_link.setAttribute("href",result.pdfreport);
                // dl_link.textContent = "下載報表";
                // pdf_report_dl_link.appendChild(dl_link);
                //
                // var imgPdf = document.createElement("i");
                // imgPdf.setAttribute("class","fa fa-file-pdf-o");
                // dl_link.appendChild(imgPdf);
            }
            else
            {
                var footer_btn_2_spen = document.getElementById("daily_pdf_report_status");
                footer_btn_2_spen.textContent = "產生報表失敗!";
            }
        }
    });
    return false;
}

// get today date
function getTodayDate()
{
    //var Today = new Date();
    //today = ""+ Today.getFullYear() + (Today.getMonth()+1) + Today.getDate();

    var today = moment().format('YYYY-MM-DD');
    //today = "20181218";
    return today;
}

function getTargetDate(addDays)
{
    var targetDate = moment().add(addDays, 'd').format('YYYY-MM-DD');
    return targetDate;
}

function getTargetDateTime(inDate, addDays)
{
    var cur_date = moment(inDate, "YYYY-MM-DD");
    //var fullDatetime = cur_date.format('YYYY-MM-DD HH:mm:ss');

    var targetDate = cur_date.add(addDays, 'd').format('YYYY-MM-DD HH:mm:ss');
    return targetDate;
}

function convertTargetDateTime(inDate)
{
    var cur_date = moment(inDate, "YYYY-MM-DD");
    var targetDate = cur_date.format('YYYY-MM-DD HH:mm:ss');
    return targetDate;
}

function convertToShowDate(inDate)
{
    var cur_date = moment(inDate, "YYYY-MM-DD");
    var outDate = cur_date.format('YYYY/MM/DD');

    return outDate;
}

function convertRecordTime(inDate)
{
    var cur_date = moment(inDate, "YYYY-MM-DD HH:mm:ss");
    var outDate = cur_date.format('HH:mm:ss');

    return outDate;
}

function convertPrettyTime(inDate)
{
    var cur_date = moment(inDate, "HH:mm:ss");
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var perty_str = "";
    if (hour > 0)
        perty_str += hour + "小時";
    if (minutes > 0)
        perty_str += minutes + "分";
    if (seconds > 0)
        perty_str += seconds + "秒";

    return perty_str;
}

function convertPrettyCourseTime(inTime)
{
    var cur_date = moment(inTime, "HH:mm:ss");
    var hour = cur_date.hour();
    var minutes = cur_date.minutes();
    var seconds = cur_date.seconds();

    var perty_str = "";
    if (hour > 0)
        perty_str += hour + "點";
    if (minutes > 0)
        perty_str += minutes + "分";
    if (seconds > 0)
        perty_str += seconds + "秒";

    return perty_str;
}

//浮點數相乘
function FloatMul(arg1, arg2)
{
  var m = 0, s1 = arg1.toString(), s2 = arg2.toString();
  try { m += s1.split(".")[1].length; } catch (e) { }
  try { m += s2.split(".")[1].length; } catch (e) { }
  return Number(s1.replace(".", "")) * Number(s2.replace(".", "")) / Math.pow(10, m);
}

//浮點數相除
function FloatDiv(arg1, arg2)
{
  var t1 = 0, t2 = 0, r1, r2;
  try { t1 = arg1.toString().split(".")[1].length } catch (e) { }
  try { t2 = arg2.toString().split(".")[1].length } catch (e) { }
  r1 = Number(arg1.toString().replace(".", ""));
  r2 = Number(arg2.toString().replace(".", ""));
  return (r1 / r2) * Math.pow(10, t2 - t1);
}

function rotatePoint(latlngPoint, angleDeg, latlngCenter)
{
    var angleRad = angleDeg * Math.PI / 180;

    //console.log("rotatePoint() angleDeg:"+angleDeg +", angleRad:"+angleRad);
   //console.log("rotatePoint() Math.cos(angleRad):"+Math.cos(angleRad) +", Math.sin(angleRad):"+Math.sin(angleRad));

    var lngRotate = Math.cos(angleRad)*(latlngPoint.lng - latlngCenter.lng) - Math.sin(angleRad)*(latlngPoint.lat-latlngCenter.lat) + latlngCenter.lng;
    var latRotate = Math.sin(angleRad)*(latlngPoint.lng - latlngCenter.lng) + Math.cos(angleRad)*(latlngPoint.lat-latlngCenter.lat) + latlngCenter.lat;
    return (new L.latLng(latRotate, lngRotate));
}

//
//    Latitude: 1 deg = 110.574 km
//    Longitude: 1 deg = 111.320*cos(latitude) km
//
function centimeterToLatitude(distenceincm)
{
    var lat_diff = FloatDiv(distenceincm, 11057400);  //利用距離的比例來算出緯度上的比例

	return lat_diff;
}

function centimeterToLongitude(latitude, distenceincm)
{
    var lon_distance = 11132000 * Math.cos(latitude * Math.PI/180); //算出該緯度上的經度長度
    var lon_diff = FloatDiv(distenceincm, lon_distance); //利用距離的比例來算出經度上的比例
    return lon_diff;
}
//This function takes in latitude and longitude of two location and returns the distance between them as the crow flies (in km)
function calcCrow(lat1, lon1, lat2, lon2)
{
  var R = 6371; // km
  var dLat = toRad(lat2-lat1);
  var dLon = toRad(lon2-lon1);
  var lat1 = toRad(lat1);
  var lat2 = toRad(lat2);

  var a = Math.sin(dLat/2) * Math.sin(dLat/2) +
    Math.sin(dLon/2) * Math.sin(dLon/2) * Math.cos(lat1) * Math.cos(lat2);
  var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));
  var d = R * c;
  return d;
}

// Converts numeric degrees to radians
function toRad(Value)
{
    return Value * Math.PI / 180;
}

function isANumber(str)
{
    return !/\D/.test(str);
}

function open_gmap(Latitude, Longitude)
{
    var targeturi = "http://www.google.com/maps/place/"+Latitude+','+Longitude;
    var win = window.open(targeturi, '_blank');
    win.focus();
}

function convert_action_name(action_type)
{
    if (action_type == "Enter" || action_type == "enter")
    {
        return "進入";
    }
    else
    if (action_type == "Leave" || action_type == "leave")
    {
        return "離開";
    }
    else
    if (action_type == "Stay")
    {
        return "停留";
    }
    else
    if (action_type == "Alert")
    {
        return "按下警急按鈕";
    }
    else
    if (action_type == "Cancel" || action_type == "cancel")
    {
        return "按下取消按鈕";
    }
    else
    if (action_type == "Finish")
    {
        return "完成";
    }
    else
    if (action_type == "Exceed")
    {
        return "超過巡檢時間";
    }
    else
    if (action_type == "shorttime")
    {
        return "巡檢時間過短";
    }
    else
    if (action_type == "longtime")
    {
        return "巡檢時間過長";
    }
    else
        return action_type;
}

function convert_IssueTime_name(IssueTime)
{
    if (IssueTime == "Immediately")
    {
        return "立刻";
    }
    else
    if (IssueTime == "1 minute")
    {
        return "1分鐘";
    }
    else
    if (IssueTime == "5 minutes")
    {
        return "5分鐘";
    }
    else
    if (IssueTime == "10 minutes")
    {
        return "10分鐘";
    }
    else
    {
        return IssueTime;
    }
}

function open_footprinttag_gps_page(date, type, nodeid, macaddress)
{
    var win = window.open('/index.php/vilsfootprinttaggps/'+project_id+'?date='+date+'&type='+type+'&nodeid='+nodeid+'&macaddress='+macaddress, '_blank');
    win.focus();
}

function open_footprinttags_page()
{
    var win = window.open('/index.php/vilsfootprinttags/'+project_id, '_blank');
    win.focus();
}
