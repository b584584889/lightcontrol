<?php
require_once("common.php");

if (isset($_GET['loadtag']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $macaddress = $_GET['loadtag'];
    $targetdate = $_GET['startdate'];
    //$enddate = $_GET['enddate'];

    $footprints = load_footprint($project_id, $macaddress, $targetdate);
    echo json_encode($footprints);

    // if (isset($_GET['sampling']))
    // {
    //     $sampling = $_GET['sampling'];
    //     $start_datetime = $_GET['startdate'];
    //     $end_datetime = $_GET['enddate'];
    //     $count = 10000;
    //     $footprints = load_tag_sampling_footprint($project_id, $tagid, $sampling, $start_datetime, $end_datetime, $count);
    //     echo json_encode($footprints);
    // }
    // else
    // {
    //     echo '[]';
    // }
}
else
if (isset($_GET['checkrecentdata']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $device = $_GET['device'];
    $nodetype = $_GET['nodetype'];

    if (isset($_GET['recentdays']))
    {
        $startdate = $_GET['startdate'];
        $recentdays = (int)$_GET['recentdays'];

        if (strlen($startdate) > 10)
            $startdate = substr( $startdate, 0, 10 );

        $return_str = checkrecentdata($project_id, $device, $nodetype, $startdate, $recentdays);
        echo json_encode($return_str);
    }
}
else
if (isset($_GET['loadposnodedata']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $macaddress = $_GET['device'];
    $targetdate = $_GET['startdate'];
    $footprints = load_footprint($project_id, $macaddress, $targetdate);
    echo json_encode($footprints);

    // $sampling = 1;
    // $start_datetime = $_GET['startdate'];
    // $end_datetime = $_GET['enddate'];
    // $count = $_GET['count'];
    // $footprints = load_tag_sampling_footprint($project_id, $device, $sampling, $start_datetime, $end_datetime, $count);
    // echo json_encode($footprints);
}
else
if (isset($_GET['loadareainout']))
{
    $device = $_GET['device'];
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $startdate = $_GET['startdate'];
    $enddate = $_GET['enddate'];

    if ( isset($_GET['areaid']))
        $areaid = $_GET['areaid'];
    else
        $areaid = '';

    $offset = $_GET['offset'];
    $count = $_GET['count'];

    $return_str = loadareainout($project_id, $device, $areaid, $startdate, $enddate, $offset, $count);
    echo json_encode($return_str);
}
else
if (isset($_GET['loadgpsnode']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $status = getServerInfo('loadgpsnode=' . $project_id);
    echo $status;

}
else
if (isset($_GET['loadgpsnodedata']))
{
    $device = $_GET['device'];
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $startdate = $_GET['startdate'];
    $enddate = $_GET['enddate'];

    $offset = $_GET['offset'];
    $count = $_GET['count'];
    $nodetype = $_GET['nodetype'];

    $return_str = loadtaggpsinfo($project_id, $nodetype, $device, $startdate, $enddate, $offset, $count);
    echo json_encode($return_str);
}
else
if (isset($_GET['loadalert']))
{
    $device = $_GET['device'];
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $startdate = $_GET['startdate'];
    $enddate = $_GET['enddate'];

    $offset = $_GET['offset'];
    $count = $_GET['count'];

    $return_str = loadalert($project_id, $device, $startdate, $enddate, $offset, $count);
    echo json_encode($return_str);
}
else
if (isset($_GET['loadevent']))
{
    $device = $_GET['device'];
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $startdate = $_GET['startdate'];
    $enddate = $_GET['enddate'];

    $offset = $_GET['offset'];
    $count = $_GET['count'];

    $return_str = loadevent($project_id, $device, $startdate, $enddate, $offset, $count);
    echo json_encode($return_str);
}
else
if (isset($_GET['loadposnode']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $status = getServerInfo('loadposnode=' . $project_id);
    echo $status;
}
else
if (isset($_GET['loadcoursetrack']))
{
    $targetdate = $_GET['targetdate'];
    $courseid = $_GET['courseid'];
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $return_str = loadcoursetrack($project_id, $courseid, $targetdate);
    echo json_encode($return_str);
}
else
if (isset($_GET['loadenterarea']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];
    $areaid = $_GET['areaid'];

    $return_str = loadenterarea($project_id, $areaid);
    echo json_encode($return_str);
}

//
// http://localhost/php/fpnodes.php?loadtag=Tag007&sampling=4&startdate=2018-02-08+12:00:00&enddate=2018-02-11+12:00:00
//
// function load_tag_sampling_footprint($tagid, $sampling, $start_datetime, $end_datetime)
// {
//     $json_array = [];
//     $json_array['tagid'] = $tagid;
//     $json_array['sampling'] = $sampling;
//     $json_array['start_datetime'] = $start_datetime;
//     $json_array['end_datetime'] = $end_datetime;
//     $json_string = json_encode($json_array);
//     $status_str = getServerInfo('loadTagSamplingFootprint='.$json_string);
//     return $status_str;
// }

function load_footprint($project_id, $macaddress, $targetdate)
{
    $db = new DB($project_id, 'fp');

    $dbkey = $macaddress . "_" .$targetdate;

    $db->query("SELECT dbdata, datetime from Footprint WHERE dbkey='$dbkey' ");

    $result = $db->fetch_array();

    $fpdata = [];
    $outputdata = [];

    $fpdata = json_decode($result['dbdata']);

    $length = count($fpdata);

    $skip_step = (int)$length/5000;

    // if ($skip_step == 0)
    //     return $fpdata;

    $output_count = 0;
    $skip_count = 0;
    for ($i = 0; $i < $length; $i++)
    {
        if ($skip_count >= $skip_step)
        {
            $outputdata[$output_count] = $fpdata[$i];
            $output_count++;
            $skip_count = 0;
        }
        else
        {
            $skip_count++;
        }
    }

    return $outputdata;
}

// function load_tag_sampling_footprint($project_id, $tagid, $sampling, $start_datetime, $end_datetime, $count)
// {
//     // global $_DB;
//     // $db = new DB();
//     // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname_ts']);
//     $db = new DB($project_id, 'ts');
//
//     $db->query("SELECT posX,posY,datetime,areaid from FootprintAll WHERE nodeid='".$tagid."' AND sampling=".$sampling." AND datetime between '". $start_datetime."' and '".$end_datetime."' LIMIT ". $count);
//
//     $index = 0;
//     $samplings = [];
//
//     while($result = $db->fetch_array())
//     {
//         $sampling = [];
//         $sampling['area'] = get_area_name($project_id, $result['areaid']);
//         $sampling['X'] = $result['posX'];
//         $sampling['Y'] = $result['posY'];
//         //$sampling['Z'] = $result['posZ'];
//         //$sampling['v'] = $result['voltage'];
//         //$sampling['t'] = $result['temperature'];
//         $sampling['datetime'] = $result['datetime'];
//         $samplings[$index] = $sampling;
//         $index++;
//     }
//     //$db->close();
//
//     return $samplings;
// }

function checkrecentdata($project_id, $device, $nodetype, $startdate, $recentdays)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname_track']);
    $db = new DB($project_id, 'track');

    $db->query("SELECT day AS order_day " .
                "FROM DayTrack " .
                "WHERE NodeId='$device' AND nodetype=$nodetype AND day <= '$startdate' GROUP BY order_day ORDER BY order_day DESC LIMIT $recentdays");


                // "WHERE NodeId='$device' AND nodetype=$nodetype AND day <= '$startdate' GROUP BY order_day ORDER BY order_day DESC LIMIT $recentdays");

    $MaxDate = '0000-00-00';
    $MinDate = '9999-99-99';

    $count = 0;
    while($result = $db->fetch_array())
    {
        if ($MaxDate < $result['order_day'])
            $MaxDate = $result['order_day'];
        if ($MinDate > $result['order_day'])
            $MinDate = $result['order_day'];
        $count++;
    }
    //$db->close();

    $records = [];
    $records['days'] = $count;
    $records['max_date'] = $MaxDate;
    $records['min_date'] = $MinDate;

    $track_data = load_trackdata($project_id, $device, $MaxDate, $MinDate);
    $records['track_data'] = $track_data;
    return $records;
}

function load_trackdata($project_id, $device, $MaxDate, $MinDate)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname_track']);
    $db = new DB($project_id, 'track');

    $db->query("SELECT * FROM DayTrack WHERE NodeId='".$device."' AND day BETWEEN '".$MinDate."' AND '".$MaxDate."' ORDER BY day DESC");

    $records = array();
    $count = 0;
    while($result = $db->fetch_array())
    {
        $data_day = $result['day'];
        //if ( $records[$data_day] == null)
        if ( !isset($records[$data_day]))
        {
            $records[$data_day] = array();
        }
        array_push($records[$data_day], $result);
        $count++;
    }
    //$db->close();

    return $records;
}

function loadcoursetrack($project_id, $courseid, $targetdate)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname_track']);
    $db = new DB($project_id, 'track');

    $db->query("SELECT nodeid, count, max_time, min_time, total_time, datetime " .
                "from DayTrack " .
                "WHERE datatype=4 AND course_id='".$courseid."' AND day='".$targetdate."' ORDER BY datetime DESC");

    $records = [];
    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['nodeid'] = $result['nodeid'];
        $record['nodename'] = get_node_name($project_id, $result['nodeid']);
        $record['count'] = $result['count'];
        $record['max_time'] = $result['max_time'];
        $record['min_time'] = $result['min_time'];
        $record['total_time'] = $result['total_time'];
        $record['datetime'] = $result['datetime'];

        $records[$count] = $record;
        $count++;
    }
    //$db->close();

    return $records;

}

//
// SELECT COUNT(dbId) as order_count, DAY(issuetime) as order_day from AreaInOutRecord WHERE PROJECT_ID=1 AND NodeId='Tag0003' GROUP BY order_day;
// SELECT COUNT(dbId) as order_count, SUBSTR( issuetime, 1, 10 ) as order_day from AreaInOutRecord WHERE PROJECT_ID=1 AND NodeId='Tag0302' GROUP BY order_day;
// SELECT COUNT(dbId) as order_count, SUBSTR( issuetime, 1, 10 ) as order_day from AreaInOutRecord WHERE PROJECT_ID=1 AND NodeId='Tag0029' GROUP BY order_day ORDER BY order_day DESC LIMIT 0,10;
//
function loadareainout($project_id, $device, $areaid, $startdate, $enddate, $offset, $count)
{
    $db = new DB($project_id, 'base');

    if($areaid != null)
        $db->query("SELECT * from AreaInOutRecord WHERE PROJECT_ID=" . $project_id . " AND NodeId='" . $device. "' AND AreaId='" . $areaid. "' AND issuetime between '" . $startdate ."' AND '". $enddate ."' ORDER BY issuetime DESC LIMIT ".$offset.",".$count);
    else
        $db->query("SELECT * from AreaInOutRecord WHERE PROJECT_ID=" . $project_id . " AND NodeId='" . $device. "' AND issuetime between '" . $startdate ."' AND '". $enddate ."' ORDER BY issuetime DESC LIMIT ".$offset.",".$count);

    $count = 0;
    $records = [];
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['AreaType'] = $result['AreaType'];
        $record['action'] = $result['action'];
        $record['AreaID'] = $result['AreaID'];
        $record['AreaName'] = $result['AreaName'];
        $record['DataTime'] = $result['DataTime'];

        $records[$count] = $record;
        $count++;
    }
    //$db->close();

    return $records;
}

function loadenterarea($project_id, $areaid)
{
    $db = new DB($project_id, 'base');
    $db->query("SELECT NodeId, action, MAX(DataTime) AS entertime FROM AreaInOutRecord WHERE PROJECT_ID=$project_id AND AreaId='$areaid' GROUP BY NodeId, action ORDER BY entertime DESC ");

    $count = 0;
    $records = [];
    $datamap = [];
    while($result = $db->fetch_array())
    {
        $NodeId = $result['NodeId'];
        if ( !isset($datamap[$NodeId]) )
        {
            $datamap[$NodeId] = [];
            $datamap[$NodeId]['action'] = $result['action'];
            $datamap[$NodeId]['NodeId'] = $result['NodeId'];
            $datamap[$NodeId]['entertime'] = $result['entertime'];

            if ($result['action'] == '1')
            {
                $record = [];
                $record['NodeId'] = $result['NodeId'];
                $record['action'] = $result['action'];
                $record['entertime'] = $result['entertime'];

                $records[$count] = $record;
                $count++;
            }
        }
    }

    return $records;
}

function loadtaggpsinfo($project_id, $nodetype, $device, $startdate, $enddate, $offset, $count)
{
    $db = new DB($project_id, 'base');

    // $db->query("SELECT * from GPSInfoRecord WHERE PROJECT_ID=" . $project_id . " AND NodeId='" . $device. "' AND DataTime >= '" . $startdate ."' AND DataTime < '". $enddate ."' ORDER BY DataTime DESC LIMIT ".$offset.",".$count);
    $db->query("SELECT * from GPSInfoRecord WHERE nodetype=" . $nodetype . " AND NodeId='" . $device. "' AND DataTime >= '" . $startdate ."' AND DataTime < '". $enddate ."' ORDER BY DataTime DESC LIMIT ".$offset.",".$count);

    $count = 0;
    $records = [];
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['Lng'] = $result['Longitude'];
        $record['Lat'] = $result['Latitude'];
        $record['time'] = $result['DataTime'];

        $records[$count] = $record;
        $count++;
    }
    //$db->close();

    return $records;
}

function loadalert($project_id, $device, $startdate, $enddate, $offset, $count)
{
    $deviceMAC = get_node_macaddress($project_id, $device);

    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT * from AlertRecord WHERE macaddress='" . $deviceMAC. "' AND issuetime between '".$startdate."' and '".$enddate."' ORDER BY issuetime DESC LIMIT ".$offset.",".$count);

    $count = 0;
    $records = [];
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['alertid'] = $result['alertid'];
        $record['nodetype'] = $result['nodetype'];
        $record['time'] = $result['issuetime'];

        $records[$count] = $record;
        $count++;
    }
    //$db->close();

    return $records;
}

function loadevent($project_id, $device, $startdate, $enddate, $offset, $count)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT * " .
                "from FiredEvents " .
                "WHERE PROJECT_ID=" . $project_id. " AND datetime between '".$startdate."' and '".$enddate."' ORDER BY datetime DESC");

    // eventid=25 {"projectid":"1","eventid":"25","nodeid":"Tag0026","action":"Enter","areaid":"area1611","issuetime":"0"}
    // eventid=999 {"type":"checkpointgroup","projectid":"1","GroupID":"85","GroupName":"巡檢路線6","action":"exceed","issuetime":"2019-06-14 16:30:02"}
    // eventid=999 {"type":"checkpoint","projectid":"1","GroupID":"85","PathIndex":"1","TagNodeId":"Tag0126","action":"longtime","NodeId":"Locator0177","issuetime":"2019-06-06 16:15:33"}
    // eventid=999 {"type":"checkpointpath","projectid":"1","GroupID":"15","PathIndex":"1","TagNodeId":"","action":"exceed","StartNodeId":"Locator0013","EndNodeId":"Locator1001","issuetime":"2019-04-29 10:01:25"}
    // eventid=9999 {"projectid":"1","eventid":"9999","nodeid":"Locator0169","action":"Cancel","areaid":"","issuetime":"0","locatorid":"Locator0169"}
    // eventid=9999 {"projectid":"1","eventid":"9999","nodeid":"Locator0169","action":"Alert","areaid":"","issuetime":"0","locatorid":"Locator0169"}
    // eventid=9999 {"projectid":"1","eventid":"9999","nodeid":"Tag0145","action":"Enter","areaid":"area6330","issuetime":"0","locatorid":""}


    $count = 0;
    $records = [];
    while($result = $db->fetch_array())
    {
        $eventid = $result['eventid'];
        $eventjson = json_decode($result['eventjson']);

        if ( $eventid == 9999)
        {
            if ( strcmp($device, $eventjson->{'nodeid'}) == 0 )
            {
                $record = [];
                $record['eventid'] = $result['eventid'];
                $record['eventjson'] = $result['eventjson'];
                $record['nodeid'] = $eventjson->{'nodeid'};
                $record['areaid'] = $eventjson->{'areaid'};
                $record['action'] = $eventjson->{'action'};
                $record['issuetime'] = $eventjson->{'issuetime'};
                $record['time'] = $result['datetime'];
                $records[$count] = $record;
                $count++;
            }
        }
        else
        if ( $eventid == 999 && strcmp($eventjson->{'type'}, 'inspectionpoint') == 0 || strcmp($eventjson->{'type'}, 'inspectionpointtravel') == 0)
        {
            if ( strcmp($device, $eventjson->{'Inspector'}) == 0 )
            {
                $record = [];
                $record['eventid'] = $result['eventid'];
                $record['eventjson'] = $result['eventjson'];
                $record['TagNodeId'] = $eventjson->{'Inspector'};
                $record['GroupID'] = $eventjson->{'GroupID'};
                $record['action'] = $eventjson->{'action'};
                $record['NodeId'] = $eventjson->{'NodeId'};
                $record['issuetime'] = $eventjson->{'issuetime'};
                $record['time'] = $result['datetime'];
                $records[$count] = $record;
                $count++;
            }
            // if ( strcmp($device, $eventjson->{'TagNodeId'}) == 0 )
            // {
            //     $record = [];
            //     $record['eventid'] = $result['eventid'];
            //     $record['eventjson'] = $result['eventjson'];
            //     $record['TagNodeId'] = $eventjson->{'TagNodeId'};
            //     $record['GroupID'] = $eventjson->{'GroupID'};
            //     $record['action'] = $eventjson->{'action'};
            //     $record['NodeId'] = $eventjson->{'NodeId'};
            //     $record['issuetime'] = $eventjson->{'issuetime'};
            //     $record['time'] = $result['datetime'];
            //     $records[$count] = $record;
            //     $count++;
            // }
        }
        else
        if ( strcmp($eventjson->{'nodeid'}, $device) == 0 ||
             strcmp($eventjson->{'locatorid'}, $device) == 0 )
        {
            $record = [];
            $record['eventid'] = $result['eventid'];
            $record['eventjson'] = $result['eventjson'];
            $record['nodeid'] = $eventjson->{'nodeid'};
            $record['areaid'] = $eventjson->{'areaid'};
            $record['action'] = $eventjson->{'action'};
            $record['issuetime'] = $eventjson->{'issuetime'};
            $record['time'] = $result['datetime'];
            $records[$count] = $record;
            $count++;
        }
    }
    //$db->close();

    $index = 0;
    for($index=0;$index<$count;$index++)
    {
        $record = $records[$index];
        if ( $record['TagNodeId'] )
            $record['TagNodeName'] = get_node_name($project_id, $record['TagNodeId']);
        if ( $record['NodeId'] )
        {
            $record['NodeIdName'] = get_node_name($project_id, $record['NodeId']);
            if ($record['NodeIdName'] == $record['NodeId'])
            {
                $record['NodeIdName'] = get_user_name($project_id, $record['NodeId']);
            }
        }
        $record['nodename'] = get_node_name($project_id, $record['nodeid']);
        if ($record['nodename'] == $record['nodeid'])
        {
            $record['nodename'] = get_user_name($project_id, $record['nodeid']);
        }

        $record['areaname'] = get_area_name($project_id, $record['areaid']);
        $record['issuename'] = get_issuetime_name($record['issuetime']);
        $records[$index] = $record;
    }


    return $records;
}


?>
