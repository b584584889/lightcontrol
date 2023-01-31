<?php
require_once("common.php");

header('Content-Type: application/json');

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
        CHECKPOINTPATH_do_GET();
    break;
    case 'PUT':
        CHECKPOINTPATH_do_PUT();
    break;
    case 'POST':
        CHECKPOINTPATH_do_POST();
    break;
    case 'DELETE':
        CHECKPOINTPATH_do_DELETE();
    break;
}

function CHECKPOINTPATH_do_GET()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    if (isset($_GET['loadcheckpointgroup']))
    {
        $return_str = loadcheckpointgroup($project_id);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadcheckpointpath']))
    {
        $return_str = loadcheckpointpath($project_id, $_GET['GroupID']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadcheckpointgroupdailyrecord']))
    {
        $return_str = loadcheckpointgroupdailyrecord($project_id, $_GET['GroupID'], $_GET['date']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadcheckpointpathdailyrecord']))
    {
        $return_str = loadcheckpointpathdailyrecord($project_id, $_GET['GroupID'], $_GET['date']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadcheckpointTagBehavior']))
    {
        $return_str = loadcheckpointTagBehavior($project_id, $_GET['GroupID'], $_GET['PathIndex'], $_GET['Anchor'], $_GET['date']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadcheckpointgrouphistory']))
    {
        $return_str = loadcheckpointgrouphistory($project_id, $_GET['GroupID'], $_GET['startdate'], $_GET['enddate']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadcheckpointpathdailyhistory']))
    {
        $return_str = loadcheckpointpathdailyhistory($project_id, $_GET['GroupID'], $_GET['startdate'], $_GET['enddate']);
        echo json_encode($return_str);
    }
}

function CHECKPOINTPATH_do_PUT()
{
}

function CHECKPOINTPATH_do_POST()
{
    $json = file_get_contents('php://input');
    $obj = json_decode($json);

    if (is_array($obj))
    {
        $issues = [];
        $issues['is_array'] = 1;
        echo json_encode($issues);
    }
    else
    if ($obj->CREATE_GROUP == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['GroupName'] = $obj->GroupName;
        $json_array['StartTime'] = $obj->StartTime;
        $json_array['EndTime'] = $obj->EndTime;

        $json_string = json_encode($json_array);
        $send_string = 'createcheckpointgroup=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($json_array);
    }
    else
    if ($obj->UPDATE_GROUP == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['GroupID'] = $obj->GroupID;
        $json_array['GroupName'] = $obj->GroupName;
        $json_array['StartTime'] = $obj->StartTime;
        $json_array['EndTime'] = $obj->EndTime;

        $json_string = json_encode($json_array);
        $send_string = 'updatecheckpointgroup=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($json_array);
    }
    else
    if ($obj->CREATE_PATH == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['GroupID'] = $obj->GroupID;
        $json_array['PathIndex'] = $obj->PathIndex;
        $json_array['StartNodeId'] = $obj->StartNode;
        $json_array['StartNodeDistance'] = $obj->StartNodeDistance;
        $json_array['StartNodeMinTime'] = $obj->StartNodeMinTime;
        $json_array['StartNodeMaxTime'] = $obj->StartNodeMaxTime;
        $json_array['EndNodeId'] = $obj->EndNode;
        $json_array['EndNodeDistance'] = $obj->EndNodeDistance;
        $json_array['EndNodeMinTime'] = $obj->EndNodeMinTime;
        $json_array['EndNodeMaxTime'] = $obj->EndNodeMaxTime;
        $json_array['TravelTime'] = $obj->TravelTime;

        $json_string = json_encode($json_array);
        $send_string = 'createcheckpointpath=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($json_array);
    }
    else
    if ($obj->UPDATE_PATH_STARTNODE == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['GroupID'] = $obj->GroupID;
        $json_array['PathIndex'] = $obj->PathIndex;
        $json_array['StartNodeDistance'] = $obj->StartNodeDistance;
        $json_array['StartNodeMinTime'] = $obj->StartNodeMinTime;
        $json_array['StartNodeMaxTime'] = $obj->StartNodeMaxTime;

        $json_string = json_encode($json_array);
        $send_string = 'updatecheckpointpathstartnode=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($json_array);
    }
    else
    if ($obj->UPDATE_PATH_ENDNODE == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['GroupID'] = $obj->GroupID;
        $json_array['PathIndex'] = $obj->PathIndex;
        $json_array['EndNodeDistance'] = $obj->EndNodeDistance;
        $json_array['EndNodeMinTime'] = $obj->EndNodeMinTime;
        $json_array['EndNodeMaxTime'] = $obj->EndNodeMaxTime;

        $json_string = json_encode($json_array);
        $send_string = 'updatecheckpointpathendnode=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($json_array);
    }
    else
    if ($obj->UPDATE_TRAVELTIME == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['PathID'] = $obj->PathID;
        $json_array['TravelTime'] = $obj->TravelTime;

        $json_string = json_encode($json_array);
        $send_string = 'updatecheckpointpathtraveltime=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($json_array);
    }
    else
    {
        $issues = [];
        $issues['action'] = 'none';
        echo json_encode($issues);
    }

}

function CHECKPOINTPATH_do_DELETE()
{
    //global $_DB;

    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $event = [];
    if (isset($_GET['PathID']))
    {
        $json_array = [];
        $json_array['project_id'] =  $project_id;
        $json_array['GroupID'] = (int)$_GET['GroupID'];
        $json_array['PathID'] = (int)$_GET['PathID'];

        $json_string = json_encode($json_array);
        $send_string = 'deletecheckpointpath=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($json_array);
    }
    else
    if (isset($_GET['GroupID']))
    {
        $json_array = [];
        $json_array['project_id'] =  $project_id;
        $json_array['GroupID'] = (int)$_GET['GroupID'];

        $json_string = json_encode($json_array);
        $send_string = 'deletecheckpointgroup=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($json_array);
    }
}

function loadcheckpointgroup($project_id)
{
    $checkpointgroups = [];

    //global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT * from CheckPointGroup WHERE PROJECT_ID=" . $project_id);
    while($result = $db->fetch_array())
    {
        $checkpointgroup = [];
        $checkpointgroup['GroupID'] = $result['GroupID'];
        $checkpointgroup['PROJECT_ID'] = $result['PROJECT_ID'];
        $checkpointgroup['GroupName'] = $result['GroupName'];
        $checkpointgroup['StartTime'] = $result['StartTime'];
        $checkpointgroup['EndTime'] = $result['EndTime'];
        $checkpointgroup['issuetime'] = $result['issuetime'];

        $checkpointgroups[$result['GroupID']] = $checkpointgroup;
    }
    //$db->close();

    return $checkpointgroups;
}

function loadcheckpointpath($project_id, $GroupID)
{
    $checkpointpaths = [];

    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM CheckPointPath WHERE PROJECT_ID=".$project_id." AND GroupID=".$GroupID." ORDER BY PathIndex ASC");
    while($result = $db->fetch_array())
    {
        $checkpointpath = [];
        $checkpointpath['PathID'] = $result['PathID'];
        $checkpointpath['GroupID'] = $result['GroupID'];
        $checkpointpath['PathIndex'] = $result['PathIndex'];
        $checkpointpath['PROJECT_ID'] = $result['PROJECT_ID'];
        $checkpointpath['StartNodeId'] = $result['StartNodeId'];
        $checkpointpath['StartNodeDistance'] = $result['StartNodeDistance'];
        $checkpointpath['StartNodeMinTime'] = $result['StartNodeMinTime'];
        $checkpointpath['StartNodeMaxTime'] = $result['StartNodeMaxTime'];
        $checkpointpath['EndNodeId'] = $result['EndNodeId'];
        $checkpointpath['EndNodeDistance'] = $result['EndNodeDistance'];
        $checkpointpath['EndNodeMinTime'] = $result['EndNodeMinTime'];
        $checkpointpath['EndNodeMaxTime'] = $result['EndNodeMaxTime'];
        $checkpointpath['TravelTime'] = $result['TravelTime'];

        $checkpointpaths[$result['PathID']] = $checkpointpath;
    }
    //$db->close();

    return $checkpointpaths;
}

function loadcheckpointgroupdailyrecord($project_id, $GroupID, $date)
{
    $groupdailyrecord = [];

    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM CheckPointGroupDailyRecord WHERE PROJECT_ID=".$project_id." AND GroupID=".$GroupID." AND DateTime='".$date."' ");
    while($result = $db->fetch_array())
    {
        $groupdailyrecord['GroupID'] = $result['GroupID'];
        $groupdailyrecord['PROJECT_ID'] = $result['PROJECT_ID'];
        $groupdailyrecord['Finished'] = $result['Finished'];
        $groupdailyrecord['Exceed'] = $result['Exceed'];
        $groupdailyrecord['DateTime'] = $result['DateTime'];
    }
    //$db->close();

    return $groupdailyrecord;
}

function loadcheckpointpathdailyrecord($project_id, $GroupID, $date)
{
    $checkpointpathdailyrecords = array();

    $validPath = loadcheckpointpath($project_id, $GroupID);

    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM CheckPointPathDailyRecord WHERE PROJECT_ID=".$project_id." AND GroupID=".$GroupID." AND DateTime='".$date."' ");
    while($result = $db->fetch_array())
    {

        //
        // check valid PathID
        //
        $Found = false;
        foreach ($validPath as $path)
        {
            if ( $path['PathID'] == $result['PathID'] )
            {
                $Found = true;
                break;
            }
        }

        if (!$Found)
        {
            continue;
        }

        //
        // check if has same PathID
        //
        $Found = false;
        foreach ($checkpointpathdailyrecords as $record)
        {
            if ( $record['PathID'] == $result['PathID'] )
            {
                $Found = true;
                break;
            }
        }

        if ($Found)
        {
            continue;
        }

        $dailyrecord = [];
        $dailyrecord['DailyRecordID'] = $result['DailyRecordID'];
        $dailyrecord['PROJECT_ID'] = $result['PROJECT_ID'];
        $dailyrecord['GroupID'] = $result['GroupID'];
        $dailyrecord['PathID'] = $result['PathID'];
        $dailyrecord['Checked'] = $result['Checked'];
        $dailyrecord['Finished'] = $result['Finished'];
        $dailyrecord['Exceed'] = $result['Exceed'];
        $dailyrecord['StartTagRecordNodeId'] = $result['StartTagRecordNodeId'];
        $dailyrecord['StartTagStatus'] = $result['StartTagStatus'];
        $dailyrecord['StartTagNodeId'] = $result['StartTagNodeId'];
        $dailyrecord['StartTagEnterTime'] = $result['StartTagEnterTime'];
        $dailyrecord['StartTagLeaveTime'] = $result['StartTagLeaveTime'];
        $dailyrecord['EndTagRecordNodeId'] = $result['EndTagRecordNodeId'];
        $dailyrecord['EndTagStatus'] = $result['EndTagStatus'];
        $dailyrecord['EndTagNodeId'] = $result['EndTagNodeId'];
        $dailyrecord['EndTagEnterTime'] = $result['EndTagEnterTime'];
        $dailyrecord['EndTagLeaveTime'] = $result['EndTagLeaveTime'];
        $dailyrecord['DateTime'] = $result['DateTime'];

        array_push($checkpointpathdailyrecords, $dailyrecord);

    }
    //$db->close();

    return $checkpointpathdailyrecords;
}

function loadcheckpointTagBehavior($project_id, $GroupID, $PathIndex, $Anchor, $date)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT TagNodeId, Distance, issuetime FROM CheckPointTagBehaviorData WHERE PROJECT_ID=".$project_id." AND GroupID=".$GroupID." AND PathIndex=".$PathIndex." AND AnchorNodeId='".$Anchor."' AND DateTime='".$date."' ");

    $count = 0;
    $TagBehaviors = [];

    $TagBehaviorMap = [];
    while($result = $db->fetch_array())
    {
        $TagBehavior = [];
        $TagBehavior['Distance'] = $result['Distance'];
        $TagBehavior['issuetime'] = $result['issuetime'];
        $TagBehaviorMap[$result['issuetime']] = $TagBehavior;
    }

    $data_length = count($TagBehaviorMap);
    $skip_step = $data_length / 1000;
    $skip_count = 0;

    // convert to array
    foreach ($TagBehaviorMap as $TagBehavior)
    {
        if ($skip_count < $skip_step)
        {
            $skip_count++;
            continue;
        }
        $skip_count = 0;

        $TagBehaviors[$count] = $TagBehavior;
        $count += 1;
    }
    //$db->close();

    return $TagBehaviors;
}

function loadcheckpointgrouphistory($project_id, $GroupID, $startdate, $enddate)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT * from CheckPointGroupDailyRecord WHERE PROJECT_ID=" . $project_id . " AND GroupID=" . $GroupID. " AND DateTime >= '" . $startdate ."' AND DateTime <= '". $enddate ."' ORDER BY DateTime DESC");

    $count = 0;
    $hisotrys = [];
    while($result = $db->fetch_array())
    {
        $groupdailyrecord = [];
        $groupdailyrecord['Finished'] = $result['Finished'];
        $groupdailyrecord['Exceed'] = $result['Exceed'];
        $groupdailyrecord['DateTime'] = $result['DateTime'];

        $hisotrys[$count] = $groupdailyrecord;
        $count++;
    }
    //$db->close();

    return $hisotrys;
}

function loadcheckpointpathdailyhistory($project_id, $GroupID, $startdate, $enddate)
{
    $GroupID = (int)$_GET['GroupID'];
    $startdate = $_GET['startdate'];
    $enddate = $_GET['enddate'];

    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT * from CheckPointPathDailyRecord WHERE PROJECT_ID=" . $project_id . " AND GroupID=" . $GroupID. " AND DateTime >= '" . $startdate ."' AND DateTime <= '" . $enddate ."'");

    $index = 0;
    $pathhisotrys = [];

    while($result = $db->fetch_array())
    {
        $dailyrecord = [];
        $dailyrecord['PathID'] = $result['PathID'];
        $dailyrecord['Checked'] = $result['Checked'];
        $dailyrecord['Finished'] = $result['Finished'];
        $dailyrecord['Exceed'] = $result['Exceed'];
        $dailyrecord['StartTagRecordNodeId'] = $result['StartTagRecordNodeId'];
        $dailyrecord['StartTagNodeId'] = $result['StartTagNodeId'];
        $dailyrecord['StartTagStatus'] = $result['StartTagStatus'];
        $dailyrecord['StartTagEnterTime'] = $result['StartTagEnterTime'];
        $dailyrecord['StartTagLeaveTime'] = $result['StartTagLeaveTime'];
        $dailyrecord['EndTagRecordNodeId'] = $result['EndTagRecordNodeId'];
        $dailyrecord['EndTagNodeId'] = $result['EndTagNodeId'];
        $dailyrecord['EndTagStatus'] = $result['EndTagStatus'];
        $dailyrecord['EndTagEnterTime'] = $result['EndTagEnterTime'];
        $dailyrecord['EndTagLeaveTime'] = $result['EndTagLeaveTime'];
        $dailyrecord['DateTime'] = $result['DateTime'];
        $pathhisotrys[$index] = $dailyrecord;
        $index++;
    }
    //$db->close();

    return $pathhisotrys;
}


?>
