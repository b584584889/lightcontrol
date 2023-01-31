/*
 * Author: ikki Chung
 * Date: 2020.03.02
 * Description:
 *      SMIMS VILS RTLS Solutions
 **/


var LOAD_DEVICE_NUM = 20;
var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var gl_tag_num = 0;
var gl_tag_load_index = 0;
var glTag_List = [];

var LOAD_USER_NUM = 500;
var gl_user_num = 0;
var gl_user_load_index = 0;
var gl_user_info = [];

var glCurrentTag_clicktime = new Date('2019/01/01');
var glCurrentTag_nodeid = "";
var glCurrentTag_macaddress = "";
var glCurrentTag_name = "";

var glCurrentUser_clicktime = new Date('2019/01/01');
var glCurrentUser_accountid = "";
var glCurrentUser_name = "";

var glTagBodyRecords = [];
var glUserBodyRecords = [];

var glWeightLineChart = null;
var glHeartBeatLineChart = null;
var glBloodSugarLineChart = null;
var glSystolicBloodPressureLineChart = null;
var glDiastolicBloodPressureLineChart = null;
var glBodyTemperatureLineChart = null;
var glBodyfatLineChart = null;
var glBloodOxygenLineChart = null;

var glStartDate = moment().subtract(6, 'days');
var glEndDate = moment();

var glTagBodyRecords_30days = [];

check_loading_status();

function check_loading_status()
{
    //console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" CurrentLoadIndex:" + CurrentLoadIndex);
    switch(CurrentLoadIndex)
    {
        case 0:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_devices();

                //glWeightLineChart = create_weight_line_chart();
                //glHeartBeatLineChart = create_heartbeat_line_chart();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                create_chart();

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                if (gl_groupid != 3)
                {
                    click_tag("");
                }

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 3:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_bodyinfo_records(1);

                CurrentLoadIndex = CurrentLoadIndex + 1;
            }
        break;
        case 4:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                load_user_info();

            }
        break;
        case 5:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                // console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                if (gl_groupid == 3)
                {
                    click_user(gl_accountid);
                }

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

        gl_tag_load_index = 0;

        glTag_List = [];
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
        add_tag_to_table();
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
        gl_tag_load_index = 0;
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

    if ( tag_list_content == undefined )
        return;

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
        tag_nodeid_span.setAttribute("class",'badge bg-green');
        tag_nodeid_span.textContent = node_name;
        tag_nodeid_span.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        table_td_2.appendChild(tag_nodeid_span);
    }
}

function click_tag(click_node_nodeid)
{
    console.log("click_tag() click_node_nodeid:" + click_node_nodeid);

    glCurrentUser_accountid = "";
    glCurrentUser_name = "";

    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentTag_clicktime.getTime());

    if (diffTimeInMS < 500 )
    {
        console.log("click_tag() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glCurrentTag_clicktime = new Date();

    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_nodeid = nodeData[3];
        var node_name = nodeData[5];
        var node_macaddress = nodeData[6];

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentTag_nodeid.length == 0 || click_node_nodeid === node_nodeid )
        {
            glCurrentTag_nodeid = node_nodeid;
            glCurrentTag_name = node_name;
            glCurrentTag_macaddress = node_macaddress;

            bg_color = "background-color:#CCEEFF;";
        }

        var id_row_index = document.getElementById("id_row_index_"+ node_nodeid);
        var id_row_node_name = document.getElementById("id_row_node_name_"+ node_nodeid);

        if ( id_row_index != undefined )
            id_row_index.setAttribute("style",bg_color);
        if ( id_row_node_name != undefined )
            id_row_node_name.setAttribute("style",bg_color);
    }

    load_tag_bodyinfo_records(1);
}

function create_chart()
{
    var show_chart_area = document.getElementById("show_chart_area");

    var weight_chart = create_chart_block("weight_chart", '體重紀錄', 'bg-teal-gradient');
    show_chart_area.appendChild(weight_chart);
    var heartbeat_chart = create_chart_block("heartbeat_chart", '心率紀錄', 'bg-aqua-gradient');
    show_chart_area.appendChild(heartbeat_chart);
    var bloodsugar_chart = create_chart_block("bloodsugar_chart", '血糖紀錄', 'bg-red-gradient disabled');
    show_chart_area.appendChild(bloodsugar_chart);
    var systolicbloodpressure_chart = create_chart_block("systolicbloodpressure_chart", '收縮壓紀錄', 'bg-yellow-gradient');
    show_chart_area.appendChild(systolicbloodpressure_chart);
    var diastolicbloodpressure_chart = create_chart_block("diastolicbloodpressure_chart", '舒張壓紀錄', 'bg-green-gradient');
    show_chart_area.appendChild(diastolicbloodpressure_chart);
    var bodytemperature_chart = create_chart_block("bodytemperature_chart", '體溫紀錄', 'bg-light-blue-gradient');
    show_chart_area.appendChild(bodytemperature_chart);
    var bodyfat_chart = create_chart_block("bodyfat_chart", '體脂率紀錄', 'bg-purple-gradient');
    show_chart_area.appendChild(bodyfat_chart);
    var bloodoxygen_chart = create_chart_block("bloodoxygen_chart", '血氧紀錄', 'bg-maroon-gradient');
    show_chart_area.appendChild(bloodoxygen_chart);

    glWeightLineChart = create_line_chart('weight_chart_line_chart', '體重', '#223377');
    glHeartBeatLineChart = create_line_chart('heartbeat_chart_line_chart', '心率', '#227744');
    glBloodSugarLineChart = create_line_chart('bloodsugar_chart_line_chart', '血糖', '#227744');
    glSystolicBloodPressureLineChart = create_line_chart('systolicbloodpressure_chart_line_chart', '收縮壓', '#227744');
    glDiastolicBloodPressureLineChart = create_line_chart('diastolicbloodpressure_chart_line_chart', '舒張壓', '#227744');
    glBodyTemperatureLineChart = create_line_chart('bodytemperature_chart_line_chart', '體溫', '#227744');
    glBodyfatLineChart = create_line_chart('bodyfat_chart_line_chart', '體脂率', '#227744');
    glBloodOxygenLineChart = create_line_chart('bloodoxygen_chart_line_chart', '血氧', '#227744');

}

function create_chart_block(uiid, title, bgcolor)
{
    // var chart = document.createElement('row');
    // chart.setAttribute("class", "row");
    // chart.setAttribute("style", "display:none;");
    // chart.setAttribute("id", uiid);

    var chart_col = document.createElement('div');
    chart_col.setAttribute("class", "col-xs-12 col-sm-12 col-md-6 col-lg-6");
    chart_col.setAttribute("id", uiid);
    // chart.appendChild(chart_col);

    var chart_col_box = document.createElement('div');
    chart_col_box.setAttribute("class", "box box-solid " + bgcolor);
    chart_col.appendChild(chart_col_box);

    var chart_col_box_header = document.createElement('div');
    chart_col_box_header.setAttribute("class", "box-header");
    chart_col_box.appendChild(chart_col_box_header);

    var chart_col_box_header_i = document.createElement('i');
    chart_col_box_header_i.setAttribute("class", "fa fa-th");
    chart_col_box_header.appendChild(chart_col_box_header_i);

    var chart_col_box_header_title = document.createElement('h3');
    chart_col_box_header_title.setAttribute("class", "box-title");
    chart_col_box_header_title.setAttribute("id", uiid + "_title");
    chart_col_box_header_title.textContent = title;
    chart_col_box_header.appendChild(chart_col_box_header_title);


    var chart_col_box_body = document.createElement('div');
    chart_col_box_body.setAttribute("class", "box-body border-radius-none");
    chart_col_box.appendChild(chart_col_box_body);

    var chart_col_box_body_chart = document.createElement('div');
    chart_col_box_body_chart.setAttribute("class", "chart");
    chart_col_box_body_chart.setAttribute("style", "height: 300px;");
    chart_col_box_body_chart.setAttribute("id", uiid + "_line_chart");
    chart_col_box_body.appendChild(chart_col_box_body_chart);

    return chart_col;
    // return chart;
}

function load_tag_bodyinfo_records(once)
{
    if (glCurrentTag_nodeid === '' )
    {
        console.log("load_tag_bodyinfo_records() glCurrentTag_nodeid === ''");
        return 0;
    }

    var StartDate = glStartDate.format("YYYY/MM/DD HH:mm:ss");
    var EndDate = glEndDate.format("YYYY/MM/DD HH:mm:ss");

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loadtagbodyinforecords=1'+'&targetid='+glCurrentTag_nodeid+'&StartDate='+StartDate+'&EndDate='+EndDate, function(data)
    {
        var bodyinfoList = [];

        $.each(data, function(itemindex, bodyinfo)
        {
            bodyinfoList.push(bodyinfo);
        });
        glTagBodyRecords[glCurrentTag_nodeid] = bodyinfoList;
    })
    .success(function() {

        show_bodyinfo_records(once);

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

function show_bodyinfo_records(once)
{
    var weight_chart = document.getElementById('weight_chart');
    var heartbeat_chart = document.getElementById('heartbeat_chart');

    var bodyinfoList = glTagBodyRecords[glCurrentTag_nodeid];

    if (bodyinfoList == undefined || glCurrentTag_nodeid === '')
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
    var curTaginfo = null;
    for (var i = 0; i < glTag_List.length; i++)
    {
        var nodeData = glTag_List[i];
        var node_nodeid = nodeData[3];
        //var nodename = nodeData[5];
        //var node_macaddress = nodeData[6];

        if ( node_nodeid ===  glCurrentTag_nodeid)
        {
            curTaginfo = nodeData;
            break;
        }
    }

    if ( curTaginfo == null )
    {
        console.log("load_tag_bodyinfo_records() curTaginfo == null");
        return;
    }

    var nodename = nodeData[5];

    var weight_chart_title = document.getElementById("weight_chart_title");
    var heartbeat_chart_title = document.getElementById("heartbeat_chart_title");
    weight_chart_title.textContent = nodename + ' 體重紀錄';
    heartbeat_chart_title.textContent = nodename + ' 心率紀錄';

    var weightinfoList = bodyinfoList[0];
    var heartbeatinfoList = bodyinfoList[1];

    $("#weight_chart_line_chart").empty();
    $("#heartbeat_chart_line_chart").empty();

    //glWeightLineChart = create_weight_line_chart();
    //glHeartBeatLineChart = create_heartbeat_line_chart();
    glWeightLineChart = create_line_chart('weight_chart_line_chart', '體重', '#223377');
    glHeartBeatLineChart = create_line_chart('heartbeat_chart_line_chart', '心率', '#227744');

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

    // jQueryKnob
    $('.knob').knob();

    if (once == 0)
        setTimeout(show_bodyinfo_records, 10000, 0);
}

function load_user_info()
{
    var user_num = 0;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var p = $.getJSON(targetURL, 'loaduser=1&s='+gl_user_load_index+'&count='+LOAD_USER_NUM+'&project_id='+project_id, function(data)
    {
        $.each(data, function(index, user)
        {
            user.userid = parseInt(user.userid);
            gl_user_info.push(user);

            user_num += 1;
        });

        gl_user_load_index += user_num;
    })
    .success(function() {
        if (user_num < LOAD_USER_NUM)
        {
            add_user_to_table();
            CurrentLoadIndex = CurrentLoadIndex + 1;
        }
        else
        {
            setTimeout(load_user_info, 20);
        }
    })
    .error(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1;
    })
    .complete(function() {
    });
}

function add_user_to_table()
{
    var user_list_content = document.getElementById('user_list_content');

    if (user_list_content == null)
    {
        return ;
    }

    while (user_list_content.rows.length >= 1)
    {
        user_list_content.deleteRow(0);
    }

    console.log("add_user_to_table() gl_user_info.length:"+gl_user_info.length);

    for (var i = 0; i < gl_user_info.length; i++)
    {
        var user = gl_user_info[i];
        user_list_content.appendChild( create_user_item(i+1, user) );
    }
}

function create_user_item(index, user)
{
    var user_list_content = document.getElementById('user_list_content');

    var table_tr = document.createElement('tr');
    user_list_content.appendChild(table_tr);

    var table_td_1 = document.createElement('td');
    table_td_1.textContent = index;
    table_td_1.setAttribute("id","user_row_"+ user.account);
    table_td_1.setAttribute("onclick","click_user(\""+ user.account +"\");return true;");
    table_tr.appendChild(table_td_1);

    var table_td_username = document.createElement('td');
    table_td_username.setAttribute("id","user_row_name_"+ user.account);
    table_td_username.setAttribute("onclick","click_user(\""+ user.account +"\");return true;");
    table_tr.appendChild(table_td_username);

    // user name
    var username_span = document.createElement('span');
    username_span.setAttribute("class",'badge bg-aqua-active');
    username_span.textContent = user.name;
    username_span.setAttribute("onclick","click_user(\""+ user.account +"\");return true;");
    table_td_username.appendChild(username_span);

    return table_tr;
}

function click_user(click_account)
{
    // console.log("click_user() click_account:" + click_account +" gl_user_info.length:"+gl_user_info.length);

    glCurrentTag_nodeid = "";
    glCurrentTag_macaddress = "";
    glCurrentTag_name = "";

    var currentdate = new Date();
    const diffTimeInMS = Math.abs(currentdate.getTime() - glCurrentUser_clicktime.getTime());

    if (diffTimeInMS < 500 )
    {
        console.log("click_user() diffTimeInMS:" + diffTimeInMS);
        return;
    }

    glCurrentUser_clicktime = new Date();

    glCurrentTag_nodeid = '';
    glCurrentTag_name = '';
    glCurrentTag_macaddress = '';

    for (var i = 0; i < gl_user_info.length; i++)
    {
        var user = gl_user_info[i];

        var bg_color = "background-color:#FFFFFF;";
        if ( glCurrentUser_accountid.length == 0 || click_account === user.account )
        {
            glCurrentUser_accountid = user.account;
            glCurrentUser_name = user.name;

            bg_color = "background-color:#CCEEFF;";
        }

        var id_row_index = document.getElementById("user_row_"+ user.account);
        var id_row_account_name = document.getElementById("user_row_name_"+ user.account);

        if (id_row_index != undefined)
            id_row_index.setAttribute("style", bg_color);
        if (id_row_account_name != undefined)
            id_row_account_name.setAttribute("style", bg_color);
    }

    load_user_bodyinfo_records(1);

}

function load_user_bodyinfo_records(once)
{
    if (glCurrentUser_accountid === '' )
    {
        return 0;
    }

    var StartDate = glStartDate.format("YYYY-MM-DD HH:mm:ss");
    var EndDate = glEndDate.format("YYYY-MM-DD HH:mm:ss");

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loaduserbodyinforecords=1'+'&accountid='+glCurrentUser_accountid+'&StartDate='+StartDate+'&EndDate='+EndDate, function(data)
    {
        var bodyinfoList = {};

        $.each(data, function(itemindex, bodyinfo)
        {
            // console.log("load_user_bodyinfo_records() itemindex:"+itemindex+" bodyinfo:"+bodyinfo);
            bodyinfoList[itemindex] = bodyinfo;
        });
        glUserBodyRecords[glCurrentUser_accountid] = bodyinfoList;
    })
    .success(function() {

        show_user_bodyinfo_records(once);

        if(once == 0)
            setTimeout(load_user_bodyinfo_records, 30000, 0);
    })
    .error(function() {
        if(once == 0)
            setTimeout(load_user_bodyinfo_records, 3000, 0);
    })
    .complete(function() {
    });
}

function show_user_bodyinfo_records(once)
{
    var weight_chart = document.getElementById('weight_chart');
    var heartbeat_chart = document.getElementById('heartbeat_chart');
    var bloodsugar_chart = document.getElementById('bloodsugar_chart');
    var systolicbloodpressure_chart = document.getElementById("systolicbloodpressure_chart");
    var diastolicbloodpressure_chart = document.getElementById("diastolicbloodpressure_chart");
    var bodytemperature_chart = document.getElementById("bodytemperature_chart");
    var bodyfat_chart = document.getElementById("bodyfat_chart");
    var bloodoxygen_chart = document.getElementById("bloodoxygen_chart");

    var bodyinfoList = glUserBodyRecords[glCurrentUser_accountid];

    weight_chart.setAttribute("style",'display:none');
    heartbeat_chart.setAttribute("style",'display:none');
    bloodsugar_chart.setAttribute("style",'display:none');
    systolicbloodpressure_chart.setAttribute("style",'display:none');
    diastolicbloodpressure_chart.setAttribute("style",'display:none');
    bodytemperature_chart.setAttribute("style",'display:none');
    bodyfat_chart.setAttribute("style",'display:none');
    bloodoxygen_chart.setAttribute("style",'display:none');

    if (bodyinfoList == undefined || glCurrentUser_accountid === '')
    {
        if (once == 0)
            setTimeout(show_user_bodyinfo_records, 1000, 0);
        return;
    }

    if (bodyinfoList.length == 1 )
    {
        if (once == 0)
            setTimeout(show_user_bodyinfo_records, 1000, 0);
        return;
    }

    //
    // find user
    //
    var curUserInfo = null;
    for (var i = 0; i < gl_user_info.length; i++)
    {
        var user = gl_user_info[i];

        if ( glCurrentUser_accountid === user.account)
        {
            curUserInfo = user;
            break;
        }
    }

    if ( curUserInfo == null )
    {
        return;
    }

    var weight_chart_title = document.getElementById("weight_chart_title");
    var heartbeat_chart_title = document.getElementById("heartbeat_chart_title");
    var bloodsugar_chart_title = document.getElementById("bloodsugar_chart_title");
    var systolicbloodpressure_chart_title = document.getElementById("systolicbloodpressure_chart_title");
    var diastolicbloodpressure_chart_title = document.getElementById("diastolicbloodpressure_chart_title");
    var bodytemperature_chart_title = document.getElementById("bodytemperature_chart_title");
    var bodyfat_chart_title = document.getElementById("bodyfat_chart_title");
    var bloodoxygen_chart_title = document.getElementById("bloodoxygen_chart_title");

    weight_chart_title.textContent = curUserInfo.name + ' 體重紀錄';
    heartbeat_chart_title.textContent = curUserInfo.name + ' 心率紀錄';
    bloodsugar_chart_title.textContent = curUserInfo.name + ' 血糖紀錄';
    systolicbloodpressure_chart_title.textContent = curUserInfo.name + ' 收縮壓紀錄';
    diastolicbloodpressure_chart_title.textContent = curUserInfo.name + ' 舒張壓紀錄';
    bodytemperature_chart_title.textContent = curUserInfo.name + ' 體溫紀錄';
    bodyfat_chart_title.textContent = curUserInfo.name + ' 體脂率紀錄';
    bloodoxygen_chart_title.textContent = curUserInfo.name + ' 血氧紀錄';

    var weightinfoList = bodyinfoList.weight;
    var heartbeatinfoList = bodyinfoList.heartbeat;
    var bloodsugarinfoList = bodyinfoList.bloodsugar;
    var systolicbloodpressureinfoList = bodyinfoList.systolicbloodpressure;
    var diastolicbloodpressureinfoList = bodyinfoList.diastolicbloodpressure;
    var bodytemperatureinfoList = bodyinfoList.bodytemperature;
    var bodyfatinfoList = bodyinfoList.bodyfat;
    var bloodoxygeninfoList = bodyinfoList.bloodoxygen;

    $("#weight_chart_line_chart").empty();
    $("#heartbeat_chart_line_chart").empty();
    $("#bloodsugar_chart_line_chart").empty();
    $("#systolicbloodpressure_chart_line_chart").empty();
    $("#diastolicbloodpressure_chart_line_chart").empty();
    $("#bodytemperature_chart_line_chart").empty();
    $("#bodyfat_chart_line_chart").empty();
    $("#bloodoxygen_chart_line_chart").empty();

    if (bodyinfoList.weight.length > 0)
    {
        glWeightLineChart = create_line_chart('weight_chart_line_chart', '體重', '#223377');
        adjust_chart_max_min(glWeightLineChart, bodyinfoList.weight, -1, -1);
        weight_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.heartbeat.length > 0)
    {
        glHeartBeatLineChart = create_line_chart('heartbeat_chart_line_chart', '心率', '#227744');
        adjust_chart_max_min(glHeartBeatLineChart, bodyinfoList.heartbeat, 50, 180);
        heartbeat_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.bloodsugar.length > 0)
    {
        glBloodSugarLineChart = create_line_chart('bloodsugar_chart_line_chart', '血糖', '#227744');
        adjust_chart_max_min(glBloodSugarLineChart, bodyinfoList.bloodsugar, -1, -1);
        bloodsugar_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.systolicbloodpressure.length > 0)
    {
        glSystolicBloodPressureLineChart = create_line_chart('systolicbloodpressure_chart_line_chart', '收縮壓', '#227744');
        adjust_chart_max_min(glSystolicBloodPressureLineChart, bodyinfoList.systolicbloodpressure, -1, -1);
        systolicbloodpressure_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.diastolicbloodpressure.length > 0)
    {
        glDiastolicBloodPressureLineChart = create_line_chart('diastolicbloodpressure_chart_line_chart', '舒張壓', '#227744');
        adjust_chart_max_min(glDiastolicBloodPressureLineChart, bodyinfoList.diastolicbloodpressure, -1, -1);
        diastolicbloodpressure_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.bodytemperature.length > 0)
    {
        glBodyTemperatureLineChart = create_line_chart('bodytemperature_chart_line_chart', '體溫', '#227744');
        adjust_chart_max_min(glBodyTemperatureLineChart, bodyinfoList.bodytemperature, -1, -1);
        bodytemperature_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.bodyfat.length > 0)
    {
        glBodyfatLineChart = create_line_chart('bodyfat_chart_line_chart', '體脂率', '#227744');
        adjust_chart_max_min(glBodyfatLineChart, bodyinfoList.bodyfat, -1, -1);
        bodyfat_chart.setAttribute("style",'display:block');
    }
    if (bodyinfoList.bloodoxygen.length > 0)
    {
        glBloodOxygenLineChart = create_line_chart('bloodoxygen_chart_line_chart', '血氧', '#227744');
        adjust_chart_max_min(glBloodOxygenLineChart, bodyinfoList.bloodoxygen, 90, 99);
        bloodoxygen_chart.setAttribute("style",'display:block');
    }

    glWeightLineChart.redraw();
    glHeartBeatLineChart.redraw();
    glBloodSugarLineChart.redraw();
    glSystolicBloodPressureLineChart.redraw();
    glDiastolicBloodPressureLineChart.redraw();
    glBodyTemperatureLineChart.redraw();
    glBodyfatLineChart.redraw();
    glBloodOxygenLineChart.redraw();

    // jQueryKnob
    $('.knob').knob();

    if (once == 0)
        setTimeout(show_user_bodyinfo_records, 10000, 0);
}

function adjust_chart_max_min(uiLineChart, datainfoList, default_min, default_max)
{
    var data_min = 200;
    var data_max = 0;
    var dataitemlist = [];

    for(var index = datainfoList.length - 1; index>=0; index--)
    {
        var datainfo = datainfoList[index];
        var item_value = parseFloat(datainfo.data);

        var info = [];
        info['item1'] = item_value.toFixed(2);
        info['y'] = datainfo.datetime;
        dataitemlist.push(info);

        // console.log("adjust_chart_max_min() index:"+index+"  data_min:"+data_min+" data_max:"+data_max+" item_value:"+item_value);

        if (data_min > item_value )
        {
            // console.log("adjust_chart_max_min() index:"+index+" data_min > item_value");
            data_min = item_value;
        }
        if (data_max < item_value )
        {
            // console.log("adjust_chart_max_min() index:"+index+" data_max < item_value");
            data_max = item_value;
        }
    }

    if (default_max < 0)
        uiLineChart.options["ymax"] = parseInt(data_max) + 2;
    else
        uiLineChart.options["ymax"] = default_max;

    if (default_min < 0)
        uiLineChart.options["ymin"] = parseInt(data_min) - 2;
    else
        uiLineChart.options["ymin"] = default_min;

    uiLineChart.setData(dataitemlist);
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

// function create_weight_line_chart()
// {
//     var weightLineChart = new Morris.Line({
//       element          : 'weight_line_chart',
//       resize           : true,
//       redraw           : true,
//       data             : [
//         { y: '', item1: 0 },
//         // { y: '2016-11-06 16:36:16', item1: 64 },
//         // { y: '2016-10-31 16:36:16', item1: 66 }
//       ],
//       xkey             : 'y',
//       ykeys            : ['item1'],
//       labels           : ['體重'],
//       lineColors       : ['#223377'],
//       lineWidth        : 2,
//       parseTime        : false,
//       hideHover        : 'auto',
//       pointSize        : 4,
//       pointStrokeColors: ['#223377'],
//       gridTextColor    : '#222',
//       gridStrokeWidth  : 0.4,
//       gridIntegers     : true,
//       gridLineColor    : '#efefef',
//       gridTextFamily   : 'Open Sans',
//       gridTextSize     : 10
//     });
//
//     return weightLineChart;
// }
//
// function create_heartbeat_line_chart()
// {
//     var heartBeatLineChart = new Morris.Line({
//       element          : 'heartbeat_line_chart',
//       resize           : true,
//       redraw           : true,
//       data             : [
//         { y: '', item1: 0 },
//         // { y: '2016-11-06 16:36:16', item1: 64 },
//         // { y: '2016-10-31 16:36:16', item1: 66 }
//       ],
//       xkey             : 'y',
//       ykeys            : ['item1'],
//       labels           : ['心率'],
//       lineColors       : ['#227744'],
//       lineWidth        : 2,
//       parseTime        : false,
//       hideHover        : 'auto',
//       pointSize        : 4,
//       pointStrokeColors: ['#227744'],
//       gridTextColor    : '#222',
//       gridStrokeWidth  : 0.4,
//       gridIntegers     : true,
//       gridLineColor    : '#efefef',
//       gridTextFamily   : 'Open Sans',
//       gridTextSize     : 10
//     });
//
//     return heartBeatLineChart;
// }

function create_line_chart(uiid, show_label, lineColors)
{
    var uiLineChart = new Morris.Line({
      element          : uiid,
      resize           : true,
      redraw           : true,
      data             : [
        { y: '', item1: 0 },
        // { y: '2016-11-06 16:36:16', item1: 64 },
        // { y: '2016-10-31 16:36:16', item1: 66 }
      ],
      xkey             : 'y',
      ykeys            : ['item1'],
      labels           : [show_label],
      lineColors       : [lineColors],
      lineWidth        : 2,
      parseTime        : false,
      hideHover        : 'auto',
      pointSize        : 4,
      pointStrokeColors: [lineColors],
      gridTextColor    : '#222',
      gridStrokeWidth  : 0.4,
      gridIntegers     : true,
      gridLineColor    : '#efefef',
      gridTextFamily   : 'Open Sans',
      gridTextSize     : 10
    });

    return uiLineChart;
}

//Date range as a button
$('#daterange-btn').daterangepicker(
  {
    ranges   : {
      '今天'       : [moment(), moment()],
      '昨天'   : [moment().subtract(1, 'days'), moment().subtract(1, 'days')],
      '最近7天' : [moment().subtract(6, 'days'), moment()],
      '最近30天': [moment().subtract(29, 'days'), moment()],
      '這個月'  : [moment().startOf('month'), moment().endOf('month')],
      '上個月'  : [moment().subtract(1, 'month').startOf('month'), moment().subtract(1, 'month').endOf('month')]
    },
    startDate: glStartDate,
    endDate  : glEndDate,
    opens: 'right',
    autoApply: true,
    // locale: { cancelLabel: 'Cancel', applyLabel: '應用' }
  },
  function (start, end) {
    $('#daterange-btn span').html(start.format('YYYY/MM/DD ') + ' - ' + end.format('YYYY/MM/DD'))
  }
)

$('#daterange-btn').on('apply.daterangepicker', function(ev, picker)
{
    glStartDate = picker.startDate;
    glEndDate = picker.endDate;

    // check is tag or user
    if (glCurrentTag_nodeid.length > 0)
    {
        load_tag_bodyinfo_records(1);
    }
    else
    if (glCurrentUser_accountid.length > 0)
    {
        load_user_bodyinfo_records(1);
    }

    // console.log(picker.startDate.format('YYYY/MM/DD'));
    // console.log(picker.endDate.format('YYYY/MM/DD'));
});


function add_search_tag_to_table(searchtext)
{
    var search_list_content = document.getElementById('search_list_content');

    if ( search_list_content == undefined )
    {
        return;
    }

    while (search_list_content.rows.length >= 1)
    {
        search_list_content.deleteRow(0);
    }

    //console.log("add_tag_to_table() glTag_List.length:"+glTag_List.length);

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
        //
        // var table_td_1 = document.createElement('td');
        // table_td_1.textContent = i+1;
        // table_td_1.setAttribute("id","id_row_index_"+ node_nodeid);
        // table_td_1.setAttribute("onclick","click_tag(\""+ node_nodeid +"\");return false;");
        // table_tr.appendChild(table_td_1);

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

function add_search_user_to_table(searchtext)
{
    var search_list_content = document.getElementById('search_list_content');
    if ( search_list_content == undefined )
    {
        return;
    }
    //console.log("add_tag_to_table() glTag_List.length:"+glTag_List.length);

    var searchtext_lc = searchtext.toLowerCase();

    for (var i = 0; i < gl_user_info.length; i++)
    {
        var userinfo = gl_user_info[i];

        var targetname = userinfo.name.toLowerCase();
        var targetid = userinfo.account.toLowerCase();

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
        table_td_2.setAttribute("id","id_row_node_name_"+ targetid);
        table_td_2.setAttribute("onclick","click_user(\""+ targetid +"\");return false;");
        table_tr.appendChild(table_td_2);

        var tag_nodeid_span = document.createElement('span');
        tag_nodeid_span.setAttribute("class",'badge bg-aqua-active');
        tag_nodeid_span.textContent = targetname;
        tag_nodeid_span.setAttribute("onclick","click_user(\""+ targetid +"\");return false;");
        table_td_2.appendChild(tag_nodeid_span);
    }
}

function loc_tab_search_text_change(e)
{
    var input_value = document.getElementById("loc_tab_search_text").value;

     //console.log("input_value:"+input_value);

     add_search_tag_to_table(input_value);
     add_search_user_to_table(input_value);

     setCookie("search_text", input_value, 10);
}

function get_pdf_report()
{
    var pdf_report_dl_link = document.getElementById("pdf_report_status");
    pdf_report_dl_link.textContent = "產生報表中...";

    if (glCurrentTag_nodeid != '' )
    {
        get_tag_pdf_report();
    }
    else
    if (glCurrentUser_accountid != '' )
    {
        get_user_pdf_report();
    }
    else
    {
        pdf_report_dl_link.textContent = "無此標籤!";
        return 0;
    }
}

function get_tag_pdf_report()
{
    if (glCurrentTag_nodeid === '' )
    {
        pdf_report_dl_link.textContent = "無此標籤!";
        return 0;
    }

    var StartDate = moment().subtract(29, 'days').format("YYYY/MM/DD HH:mm:ss");
    var EndDate = moment().format("YYYY/MM/DD HH:mm:ss");

    if ( glTagBodyRecords_30days[glCurrentTag_nodeid] != undefined )
    {
        setTimeout(generate_tag_pdf_report, 10, StartDate, EndDate);
        return 0;
    }

    glTagBodyRecords_30days[glCurrentTag_nodeid] = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loadtagbodyinforecords=1'+'&targetid='+glCurrentTag_nodeid+'&StartDate='+StartDate+'&EndDate='+EndDate, function(data)
    {
        var bodyinfoList = [];

        $.each(data, function(itemindex, bodyinfo)
        {
            bodyinfoList.push(bodyinfo);
        });

        glTagBodyRecords_30days[glCurrentTag_nodeid] = bodyinfoList;
    })
    .success(function() {

        setTimeout(generate_tag_pdf_report, 10, StartDate, EndDate);
    })
    .error(function() {
        var pdf_report_dl_link = document.getElementById("pdf_report_status");
        pdf_report_dl_link.textContent = "無法取得資料!";
    })
    .complete(function() {
    });
}

function get_user_pdf_report()
{
    if (glCurrentUser_accountid === '' )
    {
        pdf_report_dl_link.textContent = "無此使用者!";
        return 0;
    }

    var StartDate = moment().subtract(29, 'days').format("YYYY-MM-DD HH:mm:ss");
    var EndDate = moment().format("YYYY-MM-DD HH:mm:ss");

    if ( glTagBodyRecords_30days[glCurrentUser_accountid] != undefined )
    {
        setTimeout(generate_user_pdf_report, 10, StartDate, EndDate);
        return 0;
    }

    glTagBodyRecords_30days[glCurrentUser_accountid] = [];

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    $.getJSON(targetURL, 'project_id='+project_id+'&loaduserbodyinforecords=1'+'&accountid='+glCurrentUser_accountid+'&StartDate='+StartDate+'&EndDate='+EndDate, function(data)
    {
        var bodyinfoList = [];

        $.each(data, function(itemindex, bodyinfo)
        {
            bodyinfoList.push(bodyinfo);
        });

        glTagBodyRecords_30days[glCurrentUser_accountid] = bodyinfoList;
    })
    .success(function() {

        setTimeout(generate_user_pdf_report, 10, StartDate, EndDate);
    })
    .error(function() {
        var pdf_report_dl_link = document.getElementById("pdf_report_status");
        pdf_report_dl_link.textContent = "無法取得資料!";
    })
    .complete(function() {
    });

}

function generate_tag_pdf_report(StartDate, EndDate)
{
    var bodyinfoList = glTagBodyRecords_30days[glCurrentTag_nodeid];

    var weightinfoList = bodyinfoList[0];
    var heartbeatinfoList = bodyinfoList[1];

    var startdate = moment(StartDate).format("YYYY/MM/DD");
    var enddate = moment(EndDate).format("YYYY/MM/DD");

    var json = {};
    json['project_id'] = ''+project_id;
    json['bodyinfopdfreport'] = 1;
    json['nodeid'] = glCurrentTag_nodeid;
    json['nodename'] = glCurrentTag_name;
    json['startdate'] = startdate;
    json['enddate'] = enddate;
    json['weight'] = weightinfoList.weight;
    json['heartbeat'] = heartbeatinfoList.heartbeat;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      contentType: "application/json",
      dataType: "json",
      data: JSON.stringify(json),
      success:
        function(result)
        {
            //console.log("get_pdf_report() response:"+response);

            //var result = JSON.parse(response);
            //console.log("get_pdf_report() success:"+result.success+" pdfreport:"+result.pdfreport);

            var pdf_report_dl_link = document.getElementById("pdf_report_status");
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
                pdf_report_dl_link.textContent = "產生報表失敗!";
            }
        }
    });
    return false;
}

function generate_user_pdf_report(StartDate, EndDate)
{
    //var bodyinfoList = glTagBodyRecords_30days[glCurrentUser_accountid];
    var bodyinfoList = glUserBodyRecords[glCurrentUser_accountid];

    var weightinfoList = bodyinfoList.weight;
    var heartbeatinfoList = bodyinfoList.heartbeat;
    var bloodsugarinfoList = bodyinfoList.bloodsugar;
    var systolicbloodpressureinfoList = bodyinfoList.systolicbloodpressure;
    var diastolicbloodpressureinfoList = bodyinfoList.diastolicbloodpressure;
    var bodytemperatureinfoList = bodyinfoList.bodytemperature;
    var bodyfatinfoList = bodyinfoList.bodyfat;
    var bloodoxygeninfoList = bodyinfoList.bloodoxygen;

    var startdate = moment(StartDate).format("YYYY/MM/DD");
    var enddate = moment(EndDate).format("YYYY/MM/DD");

    var json = {};
    json['project_id'] = ''+project_id;
    json['userbodyinfopdfreport'] = 1;
    json['accountid'] = glCurrentUser_accountid;
    json['username'] = glCurrentUser_name;
    json['startdate'] = startdate;
    json['enddate'] = enddate;
    json['weight'] = bodyinfoList.weight;
    json['heartbeat'] = bodyinfoList.heartbeat;
    json['bloodsugar'] = bodyinfoList.bloodsugar;
    json['systolicbloodpressure'] = bodyinfoList.systolicbloodpressure;
    json['diastolicbloodpressure'] = bodyinfoList.diastolicbloodpressure;
    json['bodytemperature'] = bodyinfoList.bodytemperature;
    json['bodyfat'] = bodyinfoList.bodyfat;
    json['bloodoxygen'] = bodyinfoList.bloodoxygen;

    var targetURL = gl_target_server + "/php/vilsnodes.php";
    jQuery.ajax({
      url: targetURL,
      type: 'POST',
      contentType: "application/json",
      dataType: "json",
      data: JSON.stringify(json),
      success:
        function(result)
        {
            //console.log("get_pdf_report() response:"+response);

            //var result = JSON.parse(response);
            //console.log("get_pdf_report() success:"+result.success+" pdfreport:"+result.pdfreport);

            var pdf_report_dl_link = document.getElementById("pdf_report_status");
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
                pdf_report_dl_link.textContent = "產生報表失敗!";
            }
        }
    });
    return false;
}

function check_cookies()
{
    var input_value = getCookie("search_text");
    if ( input_value == null )
    {
        return;
    }

    var loc_tab_search_text = document.getElementById("loc_tab_search_text");
    if ( loc_tab_search_text != null )
        loc_tab_search_text.value = input_value;

    add_search_tag_to_table(input_value);
    add_search_user_to_table(input_value);

}

const $source = document.querySelector('#loc_tab_search_text');
if ($source != null)
{
    $source.addEventListener('input', loc_tab_search_text_change);// register for oninput
}

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

function open_bodyinfo_page()
{
    var win = window.open('/index.php/vilsbodyinforecord/'+project_id, '_blank');
    win.focus();
}
