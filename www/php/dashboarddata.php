<?php
require_once("common.php");

date_default_timezone_set("Asia/Taipei");

if (isset($_GET['loadactivity']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $nodetype = 1;
    if (isset($_GET['nodetype']))
        $nodetype = (int)$_GET['nodetype'];

    //===============================
    //$return_str = loadactivity($project_id, $nodetype);
    //echo json_encode($return_str);
    //===============================
    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_array['nodetype'] = $nodetype;
    $json_string = json_encode($json_array);
    $send_string = 'load_dashboard_activity=' . $json_string;
    $status = getServerInfo($send_string);
    echo $status;
    //===============================
}
else
if (isset($_GET['loadsysteminfo']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_string = json_encode($json_array);
    $send_string = 'load_dashboard_systeminfo=' . $json_string;
    $status = getServerInfo($send_string);
    echo $status;
}
else
if (isset($_GET['loadalert']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $start_datetime = $_GET['startdate'];
    $end_datetime = $_GET['enddate'];

    $return_str = loadalert($project_id, $start_datetime, $end_datetime);
    echo json_encode($return_str);
}
else
if (isset($_GET['loadareauser']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $start_datetime = $_GET['startdate'];
    $end_datetime = $_GET['enddate'];

    $return_str = loadareauser($project_id, $start_datetime, $end_datetime);
    echo json_encode($return_str);
}
else
if (isset($_GET['loadoperationmode']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $start_datetime = $_GET['startdate'];
    $end_datetime = $_GET['enddate'];

    $return_str = loadoperationmode($project_id, $start_datetime, $end_datetime);
    echo json_encode($return_str);
}
else
if (isset($_GET['loadcheckpoint']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $start_datetime = $_GET['startdate'];
    $end_datetime = $_GET['enddate'];

    $return_str = loadcheckpoint($project_id, $start_datetime, $end_datetime);
    echo json_encode($return_str);
}
else
if (isset($_GET['loadinspection']))
{
    $prettystring = 0;
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $start_datetime = $_GET['startdate'];
    $end_datetime = $_GET['enddate'];
    if(isset($_GET['prettystring']))
    {
        $prettystring = (int)$_GET['prettystring'];
    }
    $return_str = loadinspection($project_id, $prettystring, $start_datetime, $end_datetime);
    echo json_encode($return_str);
}
else
if (isset($_GET['loaddevicealive']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_string = json_encode($json_array);
    $send_string = 'loaddevicealive=' . $json_string;
    $status = getServerInfo($send_string);
    echo $status;
}
else
if (isset($_GET['loadsystemservice']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_string = json_encode($json_array);
    $send_string = 'loadsystemservice=' . $json_string;
    $status = getServerInfo($send_string);

    //$status = getServerInfo('loadsystemservice');
    echo $status;
}
else
if (isset($_GET['loadnodemoveoffset']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $start_datetime = $_GET['startdate'];
    $end_datetime = $_GET['enddate'];
    $return_str = loadnodemoveoffset($project_id, $start_datetime, $end_datetime);
    echo json_encode($return_str);
}
else
if (isset($_GET['loadrecentrollcallrecord']))
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    if (isset($_GET['recentdays']))
    {
        $areaid = $_GET['areaid'];
        $startdate = $_GET['startdate'];
        $recentdays = (int)$_GET['recentdays'];

        if (strlen($startdate) > 10)
            $startdate = substr( $startdate, 0, 10 );

        $return_str = loadrecentrollcallrecord($project_id, $areaid, $startdate, $recentdays);
        echo json_encode($return_str);
    }
    else
    {
        echo '[]';
    }
}

function loadactivity($project_id, $nodetype)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname_track']);
    $db = new DB($project_id, 'track');

    $db->query("SELECT issuetime, issueday, devicecount, aliveString, datetime " .
                "FROM DeviceActivityAliveCount " .
                "WHERE nodetype=".$nodetype." ORDER BY datetime DESC limit 1 ");

    $activity = [];
    while($result = $db->fetch_array())
    {
        $activity['devicecount'] = $result['devicecount'];
        $activity['records'] = $result['aliveString'];
        $activity['issueday'] = $result['issueday'];
        $activity['issuetime'] = $result['issuetime'];
        break;
    }
    //$db->close();

    return $activity;
}

function loadareauser($project_id, $start_datetime, $end_datetime)
{
    $db = new DB($project_id, 'track');

    // fetch lastest time
    $db->query("SELECT end_datetime FROM AreaActivityUserCount WHERE end_datetime BETWEEN '".$start_datetime."' AND '".$end_datetime."' ORDER BY end_datetime DESC limit 1 ");
    //$db->query("SELECT end_datetime FROM AreaActivityUserCount ORDER BY end_datetime DESC limit 1 ");

    $end_datetime = '';
    while($result = $db->fetch_array())
    {
        $end_datetime = $result['end_datetime'];
    }

    $db->query("SELECT count, totaltime, areaid, areaname, issueday, issuetime, start_datetime, end_datetime " .
                "FROM AreaActivityUserCount where end_datetime='".$end_datetime."' ");

    $count = 0;
    $records = [];
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['count'] = $result['count'];
        $record['totaltime'] = $result['totaltime'];
        $record['areaid'] = $result['areaid'];
        $record['areaname'] = $result['areaname'];
        $record['issueday'] = $result['issueday'];
        $record['issuetime'] = $result['issuetime'];
        $record['start_datetime'] = $result['start_datetime'];
        $record['end_datetime'] = $result['end_datetime'];
        $records[$count] = $record;
        $count++;
    }
    //$db->close();

    return $records;
}

function loadalert($project_id, $start_datetime, $end_datetime)
{
    $db = new DB($project_id, 'track');

    $db->query("SELECT nodeid, SUM(count) as count, max(max_time) as max_time, min(min_time) as min_time " .
                "from DayTrack " .
                "WHERE datatype=5 AND max_time BETWEEN '".$start_datetime."' AND '".$end_datetime."' GROUP BY nodeid ORDER BY max_time DESC");

    $count = 0;
    $records = [];
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['nodeid'] = $result['nodeid'];
        $record['nodename'] = get_node_name($project_id, $record['nodeid']);
        $record['max_time'] = $result['max_time'];
        $record['min_time'] = $result['min_time'];
        $record['count'] = $result['count'];
        $records[$count] = $record;
        $count++;
    }
    //$db->close();

    return $records;
}

function loadoperationmode($project_id, $start_datetime, $end_datetime)
{
    $records = [];
    //$record1 = loadoperationmode_s(1, $start_datetime, $end_datetime);
    $record1 = loadoperationmode_n($project_id, 1, $start_datetime, $end_datetime);
    $records['1'] = $record1;
    //$record2 = loadoperationmode_s(2, $start_datetime, $end_datetime);
    $record2 = loadoperationmode_n($project_id, 2, $start_datetime, $end_datetime);
    $records['2'] = $record2;
    //$record3 = loadoperationmode_s(3, $start_datetime, $end_datetime);
    $record3 = loadoperationmode_n($project_id, 3, $start_datetime, $end_datetime);
    $records['3'] = $record3;

    return $records;
}

function loadoperationmode_n($project_id, $mode, $start_datetime, $end_datetime)
{
    $db = new DB($project_id, 'track');

    $db->query("SELECT * FROM OperationModeActivityCount WHERE mode=".$mode." AND datetime BETWEEN '".$start_datetime."' AND '".$end_datetime."' ORDER BY datetime DESC limit 1 ");
    //$db->query("SELECT * FROM OperationModeActivityCount WHERE mode=".$mode." ORDER BY datetime DESC limit 1 ");

    $record = [];
    if($result = $db->fetch_array())
    {
        $record['mode'] = $result['mode'];
        $record['totaltime'] = $result['totaltime'];
        $record['count'] =  $result['count'];
        $record['issueday'] =  $result['issueday'];
        $record['issuetime'] =  $result['issuetime'];
    }
    //$db->close();

    return $record;
}

function loadoperationmode_s($project_id, $mode, $start_datetime, $end_datetime)
{
    $db = new DB($project_id, 'track');

    $db->query("SELECT * " .
                "from OperationMode " .
                "WHERE mode=".$mode." AND datetime BETWEEN '".$start_datetime."' AND '".$end_datetime."' ORDER BY datetime DESC");

    $count = 0;
    $nodecount = 0;
    $totaltime = 0;
    $nodeids = [];
    while($result = $db->fetch_array())
    {
        $totaltime += $result['totaltime'];
        $node_id = $result['nodeid'];

        // check if exist
        $i = 0;
        $found = 0;
        $id_size = count($nodeids);
        for($i=0;$i<$id_size;$i++)
        {
            if ( $node_id == $nodeids[$i] )
            {
                $found = 1;
                break;
            }
        }
        if ($found == 0)
        {
            array_push($nodeids, $node_id);
        }

        $count++;
    }
    //$db->close();

    $record = [];
    $record['mode'] = $mode;
    $record['totaltime'] = $totaltime;
    $record['count'] = count($nodeids);

    return $record;
}

function loadcheckpoint($project_id, $start_datetime, $end_datetime)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT eventjson,datetime " .
                "from FiredEvents " .
                "WHERE eventid=999 AND datetime BETWEEN '".$start_datetime."' AND '".$end_datetime."' ORDER BY datetime DESC");
    $count = 0;
    $records = [];
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['eventjson'] = $result['eventjson'];
        $record['datetime'] = $result['datetime'];
        $records[$count] = $record;
        $count++;
    }
    //$db->close();

    return $records;
}

function loadinspection($project_id, $prettystring, $start_datetime, $end_datetime)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT eventjson,datetime " .
                "from FiredEvents " .
                "WHERE eventid=999 AND datetime BETWEEN '".$start_datetime."' AND '".$end_datetime."' ORDER BY datetime DESC");
    $count = 0;
    $records = [];
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['eventjson'] = $result['eventjson'];
        $record['datetime'] = $result['datetime'];
        if ( $prettystring == 1 )
        {
            $prettyjson = getPrettyJSON($project_id, $result['eventjson']);
            $record['prettyjson'] = $prettyjson;
        }

        $records[$count] = $record;
        $count++;
    }

    return $records;
}

function loadnodemoveoffset($project_id, $start_datetime, $end_datetime)
{
    $db = new DB($project_id, 'track');

    // fetch lastest time
    $db->query("SELECT end_datetime FROM NodeMoveOffsetRecord WHERE end_datetime BETWEEN '".$start_datetime."' AND '".$end_datetime."' ORDER BY end_datetime DESC limit 1 ");
    //$db->query("SELECT end_datetime FROM NodeMoveOffsetRecord ORDER BY end_datetime DESC limit 1 ");

    $end_datetime = '';
    while($result = $db->fetch_array())
    {
        $end_datetime = $result['end_datetime'];
    }

    $db->query("SELECT nodetype, nodeid, moveoffset, minutes, issueday, issuetime, start_datetime, end_datetime " .
                "FROM NodeMoveOffsetRecord where end_datetime='".$end_datetime."' order by moveoffset DESC ");

    $count = 0;
    $records = [];
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['nodetype'] = $result['nodetype'];
        $record['nodeid'] = $result['nodeid'];
        $record['nodename'] = get_node_name($project_id, $record['nodeid']);
        $record['moveoffset'] = $result['moveoffset'];
        $record['minutes'] = $result['minutes'];
        $record['issueday'] = $result['issueday'];
        $record['issuetime'] = $result['issuetime'];
        $record['start_datetime'] = $result['start_datetime'];
        $record['end_datetime'] = $result['end_datetime'];
        $records[$count] = $record;
        $count++;
    }
    //$db->close();

    return $records;
}

function loadrecentrollcallrecord($project_id, $areaid, $startdate, $recentdays)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname_track']);
    $db = new DB($project_id, 'track');

    $db->query("SELECT count(nodeid), issueday " .
                "FROM AreaRollCall " .
                "WHERE areaid='" . $areaid. "' AND issueday <= '".$startdate."' GROUP BY issueday ORDER BY issueday DESC LIMIT ".$recentdays);

    $count = 0;
    $records = [];
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['issueday'] = $result['issueday'];
        //$record['areaid'] = $areaid;
        $record['data'] = [];
        $records[$count] = $record;
        $count++;
    }

    for($i=0;$i<$count;$i++)
    {
        $record = $records[$i];
        $issueday = $record['issueday'];

        $db->query("SELECT nodeid, sum(totaltime) as totaltime, max(issuetime) as max_time, min(issuetime) as min_time  " .
                    "FROM AreaRollCall " .
                    "WHERE areaid='" . $areaid. "' AND issueday='".$issueday."' GROUP BY nodeid ");

        $data_count = 0;
        $datas = [];
        while($result = $db->fetch_array())
        {
            $data = [];
            $data['nodeid'] = $result['nodeid'];
            $data['nodename'] = get_node_name($project_id, $result['nodeid']);
            $data['max_time'] = $result['max_time'];
            $data['min_time'] = $result['min_time'];
            $data['totaltime'] = $result['totaltime'];
            $datas[$data_count] = $data;
            $data_count++;
        }

        $record['data'] = $datas;
        $records[$i] = $record;
    }
    //$db->close();

    return $records;
}

function getPrettyJSON($project_id, $eventjson)
{
    $event = json_decode($eventjson);

    //if(is_null($event->type))
    if(!isset($event->type))
    {
        return getEventPrettyJSON($project_id, $event);
    }
    else
    if($event->type == 'inspectionpoint')
    {
        return getInspectionPointPrettyJSON($project_id, $event);
    }
    else
    if($event->type == 'inspectionpointtravel')
    {
        return getInspectionPointTravelPrettyJSON($project_id, $event);
    }
    else
    if($event->type == 'inspectionGroup')
    {
        return getInspectionGroupPrettyJSON($event);
    }
    else
    {
        return getEventPrettyJSON($project_id, $event);
    }
}

function getEventPrettyJSON($project_id, $eventjson)
{
    // global $_DB;
    // // convert from id to name
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    // node name
    $nodeid = $eventjson->nodeid;
    $node_name = $nodeid;
    $db->query("SELECT nodename from NodeInfo WHERE nodeid='$nodeid'");
    while($result = $db->fetch_array())
    {
        $node_name = $result['nodename'];
    }

    if( strcmp($nodeid, 'CARD') == 0 || strlen($nodeid) == 0)
    {
        $node_name = '員工卡片';
    }

    // action name
    $actions = get_action_list();
    $action_name = $actions[$eventjson->action];

    // action name
    $areaid = $eventjson->areaid;
    $area_name = $areaid;
    if (strlen($areaid) > 0 )
    {
        $db->query("SELECT areaname from maplayerarea WHERE areaid='$areaid' ");
        while($result = $db->fetch_array())
        {
            $area_name = $result['areaname'];
        }
    }

    $locatorname = '';
    //$db->query("SELECT nodename from NodeInfo WHERE nodeid='$eventjson->locatorid' AND nodetype=3");
    $db->query("SELECT nodename from NodeInfo WHERE nodeid='$eventjson->locatorid'");
    while($result = $db->fetch_array())
    {
        $locatorname = $result['nodename'];
    }
    //$db->close();

    // issue time
    $issuetimes = get_issuetime_list();
    $time_name = $issuetimes[$eventjson->issuetime];

    $prettyjson['nodename'] = $node_name;
    $prettyjson['actionname'] = $action_name;
    $prettyjson['areaname'] = $area_name;
    $prettyjson['timename'] = $time_name;
    $prettyjson['locatorname'] = $locatorname;

    return $prettyjson;
}

function getInspectionPointPrettyJSON($project_id, $eventjson)
{
    $db = new DB($project_id, 'base');

    // node name
    $Inspector = $eventjson->Inspector;
    $node_name = $Inspector;
    $db->query("SELECT nodename from NodeInfo WHERE nodeid='$Inspector'");
    while($result = $db->fetch_array())
    {
        $node_name = $result['nodename'];
    }

    // action name
    $actions = get_action_list();
    $action_name = $actions[$eventjson->action];

    // anchor name
    $AnchorNodeId = $eventjson->NodeId;
    $anchor_node_name = $AnchorNodeId;
    $db->query("SELECT nodename from NodeInfo WHERE nodeid='$AnchorNodeId'");
    while($result = $db->fetch_array())
    {
        $anchor_node_name = $result['nodename'];
    }
    //$db->close();

    $prettyjson = [];
    $prettyjson['type'] = 'inspectionpoint';
    $prettyjson['tagname'] = $node_name;
    $prettyjson['actionname'] = $action_name;
    $prettyjson['inspectionpointname'] = $anchor_node_name;
    $prettyjson['locatorname'] = '';

    return $prettyjson;
}

function getInspectionPointTravelPrettyJSON($project_id, $eventjson)
{
    $db = new DB($project_id, 'base');

    // node name
    $Inspector = $eventjson->Inspector;
    $node_name = $Inspector;
    $db->query("SELECT nodename from NodeInfo WHERE nodeid='$Inspector'");
    while($result = $db->fetch_array())
    {
        $node_name = $result['nodename'];
    }

    // action name
    $actions = get_action_list();
    $action_name = $actions[$eventjson->action];

    // start anchor name
    $start_anchor_nodeid = $eventjson->StartNodeId;
    if ($eventjson->StartNodeId == null)
        $start_anchor_nodeid = $eventjson->AnchorNodeId;
    $start_anchor_node_name = $start_anchor_nodeid;
    $db->query("SELECT nodename from NodeInfo WHERE nodeid='$start_anchor_nodeid'");
    while($result = $db->fetch_array())
    {
        $start_anchor_node_name = $result['nodename'];
    }

    // end anchor name
    $end_anchor_nodeid = $eventjson->EndNodeId;
    if ($eventjson->EndNodeId == null)
        $start_anchor_nodeid = $eventjson->AnchorNodeId;
    $end_anchor_node_name = $end_anchor_nodeid;
    $db->query("SELECT nodename from NodeInfo WHERE nodeid='$end_anchor_nodeid'");
    while($result = $db->fetch_array())
    {
        $end_anchor_node_name = $result['nodename'];
    }
    //$db->close();

    $prettyjson = [];
    $prettyjson['type'] = 'inspectionpointtravel';
    $prettyjson['tagname'] = $node_name;
    $prettyjson['actionname'] = $action_name;
    $prettyjson['startinspectionpointname'] = $start_anchor_node_name;
    $prettyjson['endinspectionpointname'] = $end_anchor_node_name;

    return $prettyjson;
}

function getInspectionGroupPrettyJSON($eventjson)
{
    // action name
    $actions = get_action_list();
    $action_name = $actions[$eventjson->action];

    $prettyjson = [];
    $prettyjson['type'] = 'inspectionGroup';
    $prettyjson['groupname'] = $eventjson->GroupName;
    $prettyjson['actionname'] = $action_name;

    return $prettyjson;
}
?>
