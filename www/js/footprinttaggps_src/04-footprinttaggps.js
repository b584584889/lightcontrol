/*
 * Author: ikki Chung
 * Date: 2021 01 11
 * Description:
 *      SMIMS VILS RTLS Solutions
**/

var PreLoadIndex = -1;
var CurrentLoadIndex = 0;
var LOAD_STATE_END = 9999;

var LOAD_DEVICE_NUM = 20;
var UPDATE_INTERVAL = 1000; // update display every 1 seconds
var RECORD_PAGE_COUNT = 5;

var LOAD_TAG_DATA_GPS = 3;

var NODE_TYPE_TAG = 1;
var NODE_TYPE_LOCATOR = 3;
var NODE_TYPE_SENSOR = 6;
var NODE_TYPE_USER = 2222;

var glTag = {};
var glLocator = {};
var glSensor = {};
var glUser = {};

var glCurrentTag_Records = {};

var glMapDataList = {};
var glRecordDataList = {};
var glHeatMapList = {};

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

                load_node_info();
            }
        break;
        case 1:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                show_detail_gps_heatmap(query_date);

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
            }
        break;
        case 2:
            if ( PreLoadIndex != CurrentLoadIndex )
            {
                console.log("check_loading_status() PreLoadIndex:" + PreLoadIndex +" != CurrentLoadIndex:" + CurrentLoadIndex);
                PreLoadIndex = CurrentLoadIndex;

                CurrentLoadIndex = CurrentLoadIndex + 1; // next state
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

function load_node_info()
{
    var main_timeline = document.getElementById('main_timeline');
    if (main_timeline != undefined)
    {
        var time_label = create_time_label(query_date);
        main_timeline.appendChild(time_label);
    }

    if ( query_type == NODE_TYPE_TAG )
    {
        console.log("load_node_info() NODE_TYPE_TAG");

        load_tag_info();
    }
    else
    if ( query_type == NODE_TYPE_LOCATOR )
    {
        console.log("load_node_info() NODE_TYPE_LOCATOR");

        load_locator_info();
    }
    else
    if ( query_type == NODE_TYPE_SENSOR )
    {
        console.log("load_node_info() NODE_TYPE_SENSOR");

        load_sensor_info();
    }
    else
    if ( query_type == NODE_TYPE_USER )
    {
        console.log("load_node_info() NODE_TYPE_USER");

        load_user_info();
    }
}

function load_tag_info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var queryParameter = 'loadtag=1&project_id='+project_id+'&nodeid='+query_nodeid;

    var p = $.getJSON(targetURL, queryParameter, function(data)
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
            var node_name = nodeData[5];

            glTag = nodeData;

            var record_box_title = document.getElementById('record_box_title');
            if (record_box_title != undefined)
            {
                record_box_title.textContent = node_name;
            }
        }
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_locator_info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var queryParameter = 'loadlocator=1&project_id='+project_id+'&nodeid='+query_nodeid;

    var p = $.getJSON(targetURL, queryParameter, function(data)
    {
        var locator_num = parseInt(data['LOC_LOCATOR_NUM']);

        for (var i = gl_locator_load_index; i < gl_locator_load_index+locator_num; ++i)
        {
            var key = 'LOC_LOCATOR_INDEX_'+i;
            if (data[key] == undefined || data[key] == null)
                continue;

            var node = data[key];
            var nodeData = node.split(",");
            var node_name = nodeData[5];

            glLocator = nodeData;

            var record_box_title = document.getElementById('record_box_title');
            if (record_box_title != undefined)
            {
                record_box_title.textContent = node_name;
            }
        }
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_sensor_info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var queryParameter = 'loadsensordevice=1&project_id='+project_id+'&nodeid='+query_nodeid;

    var p = $.getJSON(targetURL, queryParameter, function(data)
    {
        $.each(data, function(index, device)
        {
            glSensor = device;

            var record_box_title = document.getElementById('record_box_title');
            if (record_box_title != undefined)
            {
                record_box_title.textContent = glSensor.name;
            }
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function load_user_info()
{
    var targetURL = gl_target_server + "/php/vilsnodes.php";
    var queryParameter = 'loaduserbaseinfo=1&project_id='+project_id+'&userid='+query_nodeid;

    var p = $.getJSON(targetURL, queryParameter, function(data)
    {
        $.each(data, function(index, user)
        {
            user.userid = parseInt(user.userid);
            glUser = user;

            var record_box_title = document.getElementById('record_box_title');
            if (record_box_title != undefined)
            {
                record_box_title.textContent = glUser.name;
            }
        });
    })
    .success(function() {
        CurrentLoadIndex = CurrentLoadIndex + 1; // next state
    })
    .error(function() {
    })
    .complete(function() {
    });
}

function show_detail_gps_heatmap(target_date)
{
    var CurrentTag_Records = [];

    var targetStartDate = getTargetDateTime(target_date, 0);
    var targetEndDate = getTargetDateTime(target_date, 1);
    var offset = 0;
    var count = 30000;

    var targetURL = gl_target_server + "/php/fpnodes.php";
    $.getJSON(targetURL, 'loadgpsnodedata=1&project_id='+project_id+'&device='+query_nodeid+'&nodetype='+query_type+'&startdate='+targetStartDate+'&enddate='+targetEndDate+'&offset='+offset+'&count='+count, function(data)
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

        put_record_to_gps_heatmap('record_box_heatmap_gps', query_nodeid, CurrentTag_Records);

    });
}

function put_record_to_gps_heatmap(uiid, nodeid, records)
{
    var gps_heatmap = new HeatMap(uiid);

    var m_records = records.map(function (p) { return [p['Lat'], p['Lng']]; });
    gps_heatmap.addRecords(m_records);

    console.log("put_record_to_gps_heatmap() records.length:"+records.length);

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
}

function show_info()
{
    var daily_pdf_report_dl_btn = document.getElementById("daily_pdf_report_dl_btn");
    daily_pdf_report_dl_btn.setAttribute("style", "display:none");
    daily_pdf_report_dl_btn.setAttribute("onclick", "location.href=''");

    var daily_pdf_report_gen_btn = document.getElementById("daily_pdf_report_gen_btn");
    daily_pdf_report_gen_btn.setAttribute("style", "display:block");

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
