<?php
session_start();

require_once("common.php");
require_once("User.php");

header('Content-Type: application/json');

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
        REST_do_GET();
    break;
    case 'PUT':
        REST_do_PUT();
    break;
    case 'POST':
        REST_do_POST();
    break;
    case 'DELETE':
        REST_do_DELETE();
    break;
}

function REST_do_GET()
{
    if(isset($_GET['loadcount']))
    {
        do_load_count();
    }
    else
    if(isset($_GET['loadevent']))
    {
        do_load_event();
    }
    else
    {
        Check_Default_GET();
    }
}

function Check_Default_GET()
{
    $unread = 0;
    $prettystring = 0;
    $count = 10;
    $offset = 0;
    $markasread = 0;
    $eventid = '';
    $eventfired = [];

    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    if(isset($_GET['tagalertbutton']))
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_string = json_encode($json_array);
        $send_string = 'tagalert=' . $json_string;
        $alert_str = getServerInfo($send_string);
        // $alert_str = getServerInfo('tagalert');
        $alertfired = packageJSON($alert_str);

        echo json_encode($alertfired);
        return;
    }

    if(isset($_GET['unread']))
    {
        $unread = (int)$_GET['unread'];
    }
    if(isset($_GET['prettystring']))
    {
        $prettystring = (int)$_GET['prettystring'];
    }
    if(isset($_GET['count']))
    {
        $count = (int)$_GET['count'];
    }
    if(isset($_GET['offset']))
    {
        $offset = (int)$_GET['offset'];
    }
    if(isset($_GET['markasread']))
    {
        $markasread = (int)$_GET['markasread'];
    }
    if(isset($_GET['eventid']))
    {
        $eventid = $_GET['eventid'];
    }
    if (isset($_GET['project_id']))
    {
        $project_id = (int)$_GET['project_id'];
        $eventfired = getFiredEvents($project_id, $unread, $prettystring, $count, $offset, $markasread, $eventid);
    }
    else
    {
        $eventfired = getFiredEventsAll($unread, $prettystring, $count, $offset, $markasread);
    }
    echo $eventfired;
}

function do_load_count()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $eventcount = getFiredEventsCount($project_id, $_GET['eventid'], $_GET['nodeid'], $_GET['startdate'], $_GET['enddate']);

    echo json_encode($eventcount);
}

function do_load_event()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $eventfired = getFiredEventsRange($project_id, $_GET['eventid'], $_GET['nodeid'], $_GET['startdate'], $_GET['enddate']);

    echo json_encode($eventfired);
}

function REST_do_PUT()
{
}

function REST_do_POST()
{
    if(isset($_POST['UPDATE_FIREDEVENT_USER']))
    {
        $project_id = (int)$_POST['project_id'];
        $id = $_POST['ID'];
        $empid = $_POST['EMPID'];
        $comment = $_POST['COMMENT'];
        $ret = updateEventUserComment($project_id, $id, $empid, $comment);

        $return_event = [];
        $return_event['result'] = $ret;
        $return_event['id'] = $id;
        $return_event['empid'] = $empid;
        $return_event['comment'] = $comment;

        echo json_encode($return_event);
    }
    else
    if(isset($_POST['UPDATE_FIREDEVENT_CLOSE']))
    {
        $project_id = (int)$_POST['project_id'];
        $id = $_POST['ID'];
        $deviceid = $_POST['DEVICEID'];
        $empid = $_POST['EMPID'];
        $alert_type = $_POST['TYPE'];
        $ret = updateEventUserCloseAction($project_id, $id, $empid, $deviceid);

        $return_event = [];
        $return_event['result'] = $ret;
        $return_event['deviceid'] = $deviceid;
        $return_event['empid'] = $empid;
        $return_event['alert_type'] = $alert_type;

        echo json_encode($return_event);
    }
    else
    if(isset($_POST['updatefiredevent']))
    {
        $project_id = 1;
        $firedeventid = $_POST['firedeventid'];
        $empid = $_POST['empid'];
        $comment = $_POST['comment'];
        $eventjson = $_POST['eventjson'];
        $datetime = $_POST['datetime'];

        updateFiredEvent($project_id, $firedeventid, $empid, $comment, $eventjson, $datetime);
    }
}

function REST_do_DELETE()
{
}

function getFiredEventsAll($unread, $prettystring, $count, $offset, $markasread)
{
    return getFiredEvents(-1, $unread, $prettystring, $count, $offset, $markasread, "");
}

function getFiredEvents($project_id, $unread, $prettystring, $count, $offset, $markasread, $eventid)
{
    $return_str = loadFiredEvents($project_id, $unread, $prettystring, $count, $offset, $markasread, $eventid);
    return json_encode($return_str);
}

function loadFiredEvents($project_id, $unread, $prettystring, $count, $offset, $markasread, $eventid)
{
    $maingroups = $_SESSION["maingroups"];
    $userid = $_SESSION["userid"];

    $loadAll = check_user_permisson($project_id, $userid);

    $loadFilter = '';
    if ( $loadAll == false )
    {
        $loadFilter = " AND maingroupid in($maingroups) ";
    }

    if ( strlen($eventid) > 0 )
    {
        $loadFilter = $loadFilter . " AND eventid = $eventid ";
    }

    if ($project_id < 0)
        $db = new DB(1, 'base');
    else
        $db = new DB($project_id, 'base');

    if ($project_id < 0)
    {
        if ( $unread == 1 )
        {
            $db->query("SELECT * FROM FiredEvents WHERE readdatetime IS NULL $loadFilter ORDER BY datetime DESC LIMIT $offset,$count");
        }
        else
        {
            $db->query("SELECT * FROM FiredEvents $loadFilter ORDER BY datetime DESC LIMIT $offset,$count");
        }
    }
    else
    {
        if ( $unread == 1 )
        {
            $db->query("SELECT * FROM FiredEvents WHERE PROJECT_ID=$project_id AND readdatetime IS NULL $loadFilter ORDER BY datetime DESC LIMIT $offset,$count");
        }
        else
        {
            $db->query("SELECT * FROM FiredEvents WHERE PROJECT_ID=$project_id $loadFilter ORDER BY datetime DESC LIMIT $offset,$count");
        }
    }

    $maingroups = explode(",", $_SESSION['maingroups']);
    $maingroupid = '1';
    if (count($maingroups) > 0)
        $maingroupid = $maingroups[0];

    // $load_subgroup_type = true;
    // if ( $maingroupid == '1' )
    // {
    //     $load_subgroup_type = false;
    // }

    $index = 0;
    $FiredEvents = [];

    while($result = $db->fetch_array())
    {
        $prettyjson = '';

        $event_decoded = json_decode($result['eventjson']);

        // $valid = true;
        // if ($load_subgroup_type)
        // {
        //     if($event_decoded->type == 'nfccard')
        //     {
        //
        //     }
        //     else
        //     {
        //         $valid = false;
        //     }
        // }
        //
        // if (!$valid)
        // {
        //     continue;
        // }


        if ( $prettystring == 1 )
        {
            $prettyjson = getPrettyJSON($project_id, $event_decoded);
        }
        if ( $markasread == 1 )
        {
            markFiredEvent($project_id, $result['id']);
        }

        // skip eventid 7777
        // if ( $result['eventid'] == '7777' )
        // {
        //     continue;
        // }

        $event= [];
        $event['firedeventid'] = $result['id'];
        $event['eventid'] = $result['eventid'];
        $event['projectid'] = (string)((int)$result['PROJECT_ID']);
        $event['eventjson'] = $event_decoded;
        $event['prettyjson'] = $prettyjson;
        if ($result['empid'] == null)
            $event['empid'] = '';
        else
            $event['empid'] = $result['empid'];
        if ($result['comment'] == null)
            $event['comment'] = '';
        else
            $event['comment'] = $result['comment'];
        $event['datetime'] = $result['datetime'];
        $FiredEvents[$index] = $event;
        $index++;
    }
    ////$db->close();

    return $FiredEvents;
}

function getPrettyJSON($project_id, $event)
{
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
    if($event->type == 'rfidcard')
    {
        return getRFIDCardPrettyJSON($project_id, $event);
    }
    else
    if($event->type == 'nfccard')
    {
        return getNFCCadpPrettyJSON($project_id, $event);
    }
    else
    {
        return getEventPrettyJSON($project_id, $event);
    }
}

function markFiredEvent($project_id, $firedeventid)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    date_default_timezone_set('Asia/Taipei');
    $date = date('Y-m-d H:i:s');
    $ret = $db->query("UPDATE FiredEvents SET readdatetime='$date' WHERE id=$firedeventid");
    //$db->close();

    return $ret;
}

function updateEventUserComment($project_id, $firedeventid, $empid, $comment)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $ret = $db->query("UPDATE FiredEvents SET empid='$empid', comment='$comment' WHERE id=$firedeventid");
    //$db->close();

    return $ret;
}

function updateEventUserCloseAction($project_id, $firedeventid, $empid, $deviceid)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    date_default_timezone_set('Asia/Taipei');
    $date = date('Y-m-d H:i:s');
    $ret = $db->query("UPDATE FiredEvents SET empid='$empid', timeend='$date'  WHERE id=$firedeventid");
    //$db->close();

    return $ret;
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
    if($result = $db->fetch_array())
    {
        $node_name = $result['nodename'];
    }
    else
    {
        $db->query("SELECT name from NFCDevice WHERE macaddress='$nodeid'");
        if($result = $db->fetch_array())
        {
            $node_name = $result['name'];
        }
        else
        {
            $db->query("SELECT name from sensordevice WHERE id='$nodeid'");
            if($result = $db->fetch_array())
            {
                $node_name = $result['name'];
            }
            else
            {
                $db->query("SELECT name from User WHERE userid=$nodeid");
                if($result = $db->fetch_array())
                {
                    $node_name = $result['name'];
                }
            }
        }
    }

    if( strcmp($nodeid, 'CARD') == 0 || strlen($nodeid) == 0)
    {
        $node_name = '員工卡片';
    }

    // action name
    $actions = get_action_list();
    $action_name = $actions[$eventjson->action];
    if ($action_name == null)
        $action_name = '';

    // action name
    $areaid = $eventjson->areaid;
    $area_name = $areaid;
    if (strlen($areaid) > 0 )
    {
        $db->query("SELECT areaname FROM maplayerarea WHERE areaid='".$areaid."' ");
        while($result = $db->fetch_array())
        {
            $area_name = $result['areaname'];
        }
    }

    $locatorname = '';
    //$db->query("SELECT nodename from NodeInfo WHERE nodeid='$eventjson->locatorid' AND nodetype=3");
    $db->query("SELECT nodename from NodeInfo WHERE nodeid='$eventjson->locatorid'");
    if($result = $db->fetch_array())
    {
        $locatorname = $result['nodename'];
    }

    if (strlen($locatorname) == 0 )
    {
        $db->query("SELECT name from NFCDevice WHERE macaddress='$eventjson->locatorid'");
        if($result = $db->fetch_array())
        {
            $locatorname = $result['name'];
        }
        else
        {
            $db->query("SELECT name from sensordevice WHERE id='$eventjson->locatorid'");
            if($result = $db->fetch_array())
            {
                $locatorname = $result['name'];
            }
        }
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

function getRFIDCardPrettyJSON($project_id, $eventjson)
{
    $actions = get_action_list();
    $action_name = $actions[$eventjson->action];

    $prettyjson = [];
    $prettyjson['type'] = 'rfidcard';
    $prettyjson['cardid'] = $eventjson->cardid;
    $prettyjson['actionname'] = $action_name;
    if ($eventjson->loramacaddress != null)
    {
        $db = new DB($project_id, 'base');

        $prettyjson['loramacaddress'] = $eventjson->loramacaddress;
        $prettyjson['locatorname'] = $eventjson->loramacaddress;

        $db->query("SELECT name from RFIDDevice WHERE LoRAMacaddress='$eventjson->loramacaddress'");
        while($result = $db->fetch_array())
        {
            $prettyjson['locatorname'] = $result['name'];
        }
    }
    return $prettyjson;
}

function getNFCCadpPrettyJSON($project_id, $eventjson)
{
    $actions = get_action_list();
    $action_name = $actions[$eventjson->action];

    $prettyjson = [];
    $prettyjson['type'] = 'nfccard';
    $prettyjson['cardid'] = $eventjson->cardid;
    $prettyjson['actionname'] = $action_name;
    if ($eventjson->loramacaddress != null)
    {
        $db = new DB($project_id, 'base');

        $prettyjson['loramacaddress'] = $eventjson->loramacaddress;
        $prettyjson['locatorname'] = $eventjson->loramacaddress;

        $db->query("SELECT name from NFCDevice WHERE LoRAMacaddress='$eventjson->loramacaddress'");
        while($result = $db->fetch_array())
        {
            $prettyjson['locatorname'] = $result['name'];
        }
    }

    return $prettyjson;
}

function updateFiredEvent($project_id, $firedeventid, $empid, $comment, $eventjson, $datetime)
{
    $db = new DB($project_id, 'base');

    $ret = $db->query("UPDATE FiredEvents SET empid='$empid', comment='$comment', eventjson='$eventjson', datetime='$datetime'  WHERE id=$firedeventid");

}

function getFiredEventsCount($project_id, $eventid, $nodeid, $startdate, $enddate)
{
    $db = new DB($project_id, 'base');

    $maingroups = $_SESSION["maingroups"];
    $userid = $_SESSION["userid"];

    $loadAll = check_user_permisson($project_id, $userid);

    $loadFilter = '';
    if ( $loadAll == false )
    {
        $loadFilter = " maingroupid in($maingroups) ";
    }

    if ( strlen($eventid) > 0 )
    {
        if ( strlen($loadFilter) != 0 )
        {
            $loadFilter = $loadFilter . " AND";
        }

        $loadFilter = $loadFilter . " eventid = '$eventid' ";
    }

    if ( strlen($loadFilter) != 0 )
    {
        $loadFilter = $loadFilter . " AND";
    }

    $loadFilter = $loadFilter . " datetime BETWEEN '$startdate' AND '$enddate'";

    $prettyjson = [];
    // $prettyjson['maingroups'] = $maingroups;
    // $prettyjson['userid'] = $userid;
    // $prettyjson['nodeid'] = $nodeid;
    // $prettyjson['loadAll'] = $loadAll;
    // $prettyjson['loadFilter'] = $loadFilter;

    // $db->query("SELECT count(*) as count from FiredEvents WHERE $loadFilter ");
    // if($result = $db->fetch_array())
    // {
    //     $prettyjson['count'] = $result['count'];
    // }

    $count = 0;
    $db->query("SELECT * from FiredEvents WHERE $loadFilter ");
    while($result = $db->fetch_array())
    {
        $event_decoded = json_decode($result['eventjson']);

        if (strcmp($event_decoded->nodeid, $nodeid) == 0)
        {
            $count += 1;
        }
    }

    $prettyjson['count'] = $count;

    return $prettyjson;
}

function getFiredEventsRange($project_id, $eventid, $sensorid, $startdate, $enddate)
{
    $db = new DB($project_id, 'base');

    $maingroups = $_SESSION["maingroups"];
    $userid = $_SESSION["userid"];

    $loadAll = check_user_permisson($project_id, $userid);

    $loadFilter = '';
    if ( $loadAll == false )
    {
        $loadFilter = " maingroupid in($maingroups) ";
    }

    if ( strlen($eventid) > 0 )
    {
        if ( strlen($loadFilter) != 0 )
        {
            $loadFilter = $loadFilter . " AND";
        }

        $loadFilter = $loadFilter . " eventid = '$eventid' ";
    }

    if ( strlen($loadFilter) != 0 )
    {
        $loadFilter = $loadFilter . " AND";
    }

    $loadFilter = $loadFilter . " datetime BETWEEN '$startdate' AND '$enddate'";

    $count = 0;
    $eventjson = [];
    // $eventjson['maingroups'] = $maingroups;
    // $eventjson['userid'] = $userid;
    // $eventjson['sensorid'] = $sensorid;
    // $eventjson['loadAll'] = $loadAll;
    // $eventjson['loadFilter'] = $loadFilter;

    $db->query("SELECT * from FiredEvents WHERE $loadFilter ");

    while($result = $db->fetch_array())
    {
        $event_decoded = json_decode($result['eventjson']);

        if (strcmp($event_decoded->nodeid, $sensorid) == 0)
        {
            $eventjson[$count] = $result;
            $count += 1;
        }
    }

    return $eventjson;
}

?>
