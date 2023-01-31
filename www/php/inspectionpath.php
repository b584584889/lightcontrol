<?php
require_once("common.php");

header('Content-Type: application/json');

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
        INSPECTIONPATH_do_GET();
    break;
    case 'PUT':
        INSPECTIONPATH_do_PUT();
    break;
    case 'POST':
        INSPECTIONPATH_do_POST();
    break;
    case 'DELETE':
        INSPECTIONPATH_do_DELETE();
    break;
}

function INSPECTIONPATH_do_GET()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    if (isset($_GET['loadinspectiongroup']))
    {
        $return_str = loadinspectiongroup($project_id);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadinspectionpoint']))
    {
        $return_str = loadinspectionpoint($project_id, $_GET['GroupID']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadinspectiongroupdailyrecord']))
    {
        $return_str = loadinspectiongroupdailyrecord($project_id, $_GET['GroupID'], $_GET['date']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadinspectionpointdailyrecord']))
    {
        $return_str = loadinspectionpointdailyrecord($project_id, $_GET['GroupID'], $_GET['date']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadinspectionTagBehavior']))
    {
        $return_str = loadinspectionTagBehavior($project_id, $_GET['GroupID'], $_GET['PointIndex'], $_GET['Anchor'], $_GET['date']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadinspectiongrouphistory']))
    {
        $return_str = loadinspectiongrouphistory($project_id, $_GET['GroupID'], $_GET['startdate'], $_GET['enddate']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadinspectionpointdailyhistory']))
    {
        $return_str = loadinspectionpointdailyhistory($project_id, $_GET['GroupID'], $_GET['startdate'], $_GET['enddate']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadinspectorscore']))
    {
        $return_str = loadinspectorscore($project_id, $_GET['inspector'], $_GET['startdate'], $_GET['enddate']);
        echo json_encode($return_str);
    }

}

function INSPECTIONPATH_do_PUT()
{
}

function INSPECTIONPATH_do_POST()
{
    $json = file_get_contents('php://input');
    $obj = json_decode($json);

    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

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
        $json_array['Inspectors'] = $obj->Inspectors;
        $json_array['AlertTime'] = $obj->AlertTime;

        $json_string = json_encode($json_array);
        $send_string = 'createinspectiongroup=' . $json_string;
        sendMessageToServer($send_string);

        createinspectiongroup($obj->project_id, $obj->GroupName, $obj->StartTime, $obj->EndTime, $obj->Inspectors, $obj->AlertTime);

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
        $json_array['Inspectors'] = $obj->Inspectors;
        $json_array['AlertTime'] = $obj->AlertTime;
        $json_array['AutoFeed'] = $obj->AutoFeed;

        $json_string = json_encode($json_array);
        $send_string = 'updateinspectiongroup=' . $json_string;
        sendMessageToServer($send_string);

        updateinspectiongroup($obj->project_id, $obj->GroupID, $obj->GroupName, $obj->StartTime, $obj->EndTime, $obj->Inspectors, $obj->AlertTime, $obj->AutoFeed);

        echo json_encode($json_array);
    }
    else
    if ($obj->CREATE_POINT == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['GroupID'] = $obj->GroupID;
        $json_array['PointIndex'] = $obj->PointIndex;
        $json_array['AnchorNodeId'] = $obj->AnchorNodeId;
        $json_array['Distance'] = $obj->Distance;
        $json_array['MinTime'] = $obj->MinTime;
        $json_array['MaxTime'] = $obj->MaxTime;
        $json_array['Message'] = $obj->Message;
        $json_array['TravelTime'] = $obj->TravelTime;

        $json_string = json_encode($json_array);
        $send_string = 'createinspectionpoint=' . $json_string;
        sendMessageToServer($send_string);

        createinspectionpoint($obj->project_id, $obj->GroupID, $obj->PointIndex, $obj->AnchorNodeId, $obj->Distance, $obj->MinTime, $obj->MaxTime, $obj->Message, $obj->TravelTime);

        echo json_encode($json_array);
    }
    else
    if ($obj->UPDATE_POINT == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['GroupID'] = $obj->GroupID;
        $json_array['PointID'] = $obj->PointID;
        $json_array['AnchorNodeId'] = $obj->AnchorNodeId;
        $json_array['Distance'] = $obj->Distance;
        $json_array['MinTime'] = $obj->MinTime;
        $json_array['MaxTime'] = $obj->MaxTime;
        $json_array['Message'] = $obj->Message;

        $json_string = json_encode($json_array);
        $send_string = 'updateinspectionpoint=' . $json_string;
        sendMessageToServer($send_string);

        updateinspectionpoint($obj->project_id, $obj->GroupID, $obj->PointID, $obj->AnchorNodeId, $obj->Distance, $obj->MinTime, $obj->MaxTime, $obj->Message);

        echo json_encode($json_array);
    }
    // else
    // if ($obj->CREATE_PATH == 1)
    // {
    //     $json_array = [];
    //     $json_array['project_id'] = $obj->project_id;
    //     $json_array['GroupID'] = $obj->GroupID;
    //     $json_array['PathIndex'] = $obj->PathIndex;
    //     $json_array['StartNodeId'] = $obj->StartNode;
    //     $json_array['StartNodeDistance'] = $obj->StartNodeDistance;
    //     $json_array['StartNodeMinTime'] = $obj->StartNodeMinTime;
    //     $json_array['StartNodeMaxTime'] = $obj->StartNodeMaxTime;
    //     $json_array['EndNodeId'] = $obj->EndNode;
    //     $json_array['EndNodeDistance'] = $obj->EndNodeDistance;
    //     $json_array['EndNodeMinTime'] = $obj->EndNodeMinTime;
    //     $json_array['EndNodeMaxTime'] = $obj->EndNodeMaxTime;
    //     $json_array['TravelTime'] = $obj->TravelTime;
    //
    //     $json_string = json_encode($json_array);
    //     $send_string = 'createinspectionpoint=' . $json_string;
    //     sendMessageToServer($send_string);
    //
    //     echo json_encode($json_array);
    // }
    // else
    // if ($obj->UPDATE_PATH_STARTNODE == 1)
    // {
    //     $json_array = [];
    //     $json_array['project_id'] = $obj->project_id;
    //     $json_array['GroupID'] = $obj->GroupID;
    //     $json_array['PathIndex'] = $obj->PathIndex;
    //     $json_array['StartNodeDistance'] = $obj->StartNodeDistance;
    //     $json_array['StartNodeMinTime'] = $obj->StartNodeMinTime;
    //     $json_array['StartNodeMaxTime'] = $obj->StartNodeMaxTime;
    //
    //     $json_string = json_encode($json_array);
    //     $send_string = 'updateinspectionpoint=' . $json_string;
    //     sendMessageToServer($send_string);
    //
    //     echo json_encode($json_array);
    // }
    // else
    // if ($obj->UPDATE_PATH_ENDNODE == 1)
    // {
    //     $json_array = [];
    //     $json_array['project_id'] = $obj->project_id;
    //     $json_array['GroupID'] = $obj->GroupID;
    //     $json_array['PathIndex'] = $obj->PathIndex;
    //     $json_array['EndNodeDistance'] = $obj->EndNodeDistance;
    //     $json_array['EndNodeMinTime'] = $obj->EndNodeMinTime;
    //     $json_array['EndNodeMaxTime'] = $obj->EndNodeMaxTime;
    //
    //     $json_string = json_encode($json_array);
    //     $send_string = 'updateinspectionpoint=' . $json_string;
    //     sendMessageToServer($send_string);
    //
    //     echo json_encode($json_array);
    // }
    else
    if ($obj->UPDATE_TRAVELTIME == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['GroupID'] = $obj->GroupID;
        $json_array['PointID'] = $obj->PointID;
        $json_array['PointIndex'] = $obj->PointIndex;
        $json_array['TravelTime'] = $obj->TravelTime;

        $json_string = json_encode($json_array);
        $send_string = 'updateinspectionpoint=' . $json_string;
        sendMessageToServer($send_string);

        updateinspectionpoint_traveltime($obj->project_id, $obj->GroupID, $obj->PointID, $obj->PointIndex, $obj->TravelTime);

        echo json_encode($json_array);
    }
    else
    if ($obj->pdfreport == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['type'] = $obj->type;
        $json_array['startdate'] = $obj->startdate;
        $json_array['enddate'] = $obj->enddate;
        $json_array['header'] = $obj->header;
        $json_array['data'] = $obj->data;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('inspectionpdfreport='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_POST['delete_inspectiongroup']))
    {
        $event = [];
        if (isset($_POST['GroupID']))
        {
            $json_array = [];
            $json_array['project_id'] =  $project_id;
            $json_array['GroupID'] = (int)$_POST['GroupID'];

            $json_string = json_encode($json_array);
            $send_string = 'deleteinspectiongroup=' . $json_string;
            sendMessageToServer($send_string);

            deleteinspectiongroup($project_id, (int)$_POST['GroupID']);

            echo json_encode($json_array);
        }
    }
    else
    if (isset($_POST['delete_inspectionpoint']))
    {
        $event = [];
        if (isset($_POST['PointID']))
        {
            $json_array = [];
            $json_array['project_id'] =  $project_id;
            $json_array['GroupID'] = (int)$_POST['GroupID'];
            $json_array['PointID'] = (int)$_POST['PointID'];

            $json_string = json_encode($json_array);
            $send_string = 'deleteinspectionpoint=' . $json_string;
            sendMessageToServer($send_string);

            deleteinspectionpoint($project_id, (int)$_POST['GroupID'], (int)$_POST['PointID']);

            echo json_encode($json_array);
        }
    }
    else
    {
        $issues = [];
        $issues['action'] = 'none';
        echo json_encode($issues);
    }

}

function INSPECTIONPATH_do_DELETE()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $event = [];
    if (isset($_GET['PointID']))
    {
        $json_array = [];
        $json_array['project_id'] =  $project_id;
        $json_array['GroupID'] = (int)$_GET['GroupID'];
        $json_array['PointID'] = (int)$_GET['PointID'];

        $json_string = json_encode($json_array);
        $send_string = 'deleteinspectionpoint=' . $json_string;
        sendMessageToServer($send_string);

        deleteinspectionpoint($project_id, (int)$_GET['GroupID'], (int)$_GET['PointID']);

        echo json_encode($json_array);
    }
    else
    if (isset($_GET['GroupID']))
    {
        $json_array = [];
        $json_array['project_id'] =  $project_id;
        $json_array['GroupID'] = (int)$_GET['GroupID'];

        $json_string = json_encode($json_array);
        $send_string = 'deleteinspectiongroup=' . $json_string;
        sendMessageToServer($send_string);

        deleteinspectiongroup($project_id, (int)$_GET['GroupID']);

        echo json_encode($json_array);
    }
}

function createinspectiongroup($project_id, $GroupName, $StartTime, $EndTime, $Inspectors, $AlertTime)
{
    $db = new DB($project_id, 'base');

    $ret = $db->query("INSERT INTO InspectionGroup(GroupName, StartTime, EndTime, Inspectors, AlertTime) VALUES ('$GroupName', '$StartTime', '$EndTime', '$Inspectors', $AlertTime) ");

    return $ret;
}

function updateinspectiongroup($project_id, $GroupID, $GroupName, $StartTime, $EndTime, $Inspectors, $AlertTime, $AutoFeed)
{
    $db = new DB($project_id, 'base');

    $ret = $db->query("UPDATE InspectionGroup SET GroupName='$GroupName', StartTime='$StartTime', EndTime='$EndTime', Inspectors='$Inspectors', AlertTime=$AlertTime, AutoFeed=$AutoFeed WHERE GroupID=$GroupID ");

    return $ret;
}

function deleteinspectiongroup($project_id, $GroupID)
{
    $db = new DB($project_id, 'base');

    $ret = $db->query("DELETE FROM InspectionGroup WHERE GroupID=$GroupID ");

    return $ret;
}

function createinspectionpoint($project_id, $GroupID, $PointIndex, $AnchorNodeId, $Distance, $MinTime, $MaxTime, $Message, $TravelTime)
{
    $db = new DB($project_id, 'base');

    $ret = $db->query("INSERT INTO InspectionPoint(GroupID, PointIndex, AnchorNodeId, Distance, MinTime, MaxTime, Message, TravelTime) VALUES ($GroupID, $PointIndex, '$AnchorNodeId', $Distance, $MinTime, $MaxTime, '$Message', $TravelTime) ");

    return $ret;
}

function updateinspectionpoint_traveltime($project_id, $GroupID, $PointID, $PointIndex, $TravelTime)
{
    $db = new DB($project_id, 'base');

    $ret = $db->query("UPDATE InspectionPoint SET TravelTime=$TravelTime WHERE GroupID=$GroupID AND PointID=$PointID ");

    return $ret;
}

function updateinspectionpoint($project_id, $GroupID, $PointID, $AnchorNodeId, $Distance, $MinTime, $MaxTime, $Message)
{
    $db = new DB($project_id, 'base');

    $ret = $db->query("UPDATE InspectionPoint SET AnchorNodeId='$AnchorNodeId', Distance=$Distance, MinTime=$MinTime, MaxTime=$MaxTime, Message='$Message' WHERE GroupID=$GroupID AND PointID=$PointID ");

    return $ret;
}

function deleteinspectionpoint($project_id, $GroupID, $PointID)
{
    $db = new DB($project_id, 'base');

    $ret = $db->query("DELETE FROM InspectionPoint WHERE GroupID=$GroupID AND PointID=$PointID");

    return $ret;
}

function loadinspectiongroup($project_id)
{
    $inspectiongroups = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * from InspectionGroup ");
    while($result = $db->fetch_array())
    {
        $inspectiongroup = [];
        $inspectiongroup['GroupID'] = $result['GroupID'];
        $inspectiongroup['GroupName'] = $result['GroupName'];
        $inspectiongroup['StartTime'] = $result['StartTime'];
        $inspectiongroup['EndTime'] = $result['EndTime'];
        $inspectiongroup['Inspectors'] = $result['Inspectors'];
        $inspectiongroup['AlertTime'] = $result['AlertTime'];
        $inspectiongroup['AutoFeed'] = $result['AutoFeed'];
        $inspectiongroup['issuetime'] = $result['issuetime'];

        $inspectiongroups[$result['GroupID']] = $inspectiongroup;
    }

    return $inspectiongroups;
}

function loadinspectionpoint($project_id, $GroupID)
{
    $inspectionpoints = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM InspectionPoint WHERE GroupID=".$GroupID." ORDER BY PointIndex ASC");
    while($result = $db->fetch_array())
    {
        $inspectionpoint = [];
        $inspectionpoint['PointID'] = $result['PointID'];
        $inspectionpoint['GroupID'] = $result['GroupID'];
        $inspectionpoint['PointIndex'] = $result['PointIndex'];
        $inspectionpoint['AnchorNodeId'] = $result['AnchorNodeId'];
        $inspectionpoint['Distance'] = $result['Distance'];
        $inspectionpoint['MinTime'] = $result['MinTime'];
        $inspectionpoint['MaxTime'] = $result['MaxTime'];
        $inspectionpoint['Message'] = $result['Message'];
        $inspectionpoint['TravelTime'] = $result['TravelTime'];

        $inspectionpoints[$result['PointID']] = $inspectionpoint;
    }

    return $inspectionpoints;
}

function loadinspectiongroupdailyrecord($project_id, $GroupID, $date)
{
    $groupdailyrecord = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM InspectionGroupDailyRecord WHERE GroupID=".$GroupID." AND DateTime='".$date."' ");
    while($result = $db->fetch_array())
    {
        $groupdailyrecord['GroupID'] = $result['GroupID'];
        $groupdailyrecord['Score'] = $result['Score'];
        $groupdailyrecord['Finished'] = $result['Finished'];
        $groupdailyrecord['Exceed'] = $result['Exceed'];
        $groupdailyrecord['Checked'] = $result['Checked'];
        $groupdailyrecord['DateTime'] = $result['DateTime'];
    }

    return $groupdailyrecord;
}

function loadinspectionpointdailyrecord($project_id, $GroupID, $date)
{
    $inspectionpointdailyrecords = array();

    $validPath = loadinspectionpoint($project_id, $GroupID);

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM InspectionPointDailyRecord WHERE GroupID=".$GroupID." AND DateTime='".$date."' ");
    while($result = $db->fetch_array())
    {

        //
        // check valid PathID
        //
        $Found = false;
        foreach ($validPath as $path)
        {
            if ( $path['PointID'] == $result['PointID'] )
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
        foreach ($inspectionpointdailyrecords as $record)
        {
            if ( $record['PointID'] == $result['PointID'] )
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
        $dailyrecord['GroupID'] = $result['GroupID'];
        $dailyrecord['PointID'] = $result['PointID'];
        $dailyrecord['Score'] = $result['Score'];
        $dailyrecord['Checked'] = $result['Checked'];
        $dailyrecord['Finished'] = $result['Finished'];
        $dailyrecord['Exceed'] = $result['Exceed'];
        $dailyrecord['Inspector'] = $result['Inspector'];
        $dailyrecord['RecordNodeId'] = $result['RecordNodeId'];
        $dailyrecord['EnterTime'] = $result['EnterTime'];
        $dailyrecord['LeaveTime'] = $result['LeaveTime'];
        $dailyrecord['Status'] = $result['Status'];
        // $dailyrecord['StartTagRecordNodeId'] = $result['StartTagRecordNodeId'];
        // $dailyrecord['StartTagStatus'] = $result['StartTagStatus'];
        // $dailyrecord['StartTagNodeId'] = $result['StartTagNodeId'];
        // $dailyrecord['StartTagEnterTime'] = $result['StartTagEnterTime'];
        // $dailyrecord['StartTagLeaveTime'] = $result['StartTagLeaveTime'];
        // $dailyrecord['EndTagRecordNodeId'] = $result['EndTagRecordNodeId'];
        // $dailyrecord['EndTagStatus'] = $result['EndTagStatus'];
        // $dailyrecord['EndTagNodeId'] = $result['EndTagNodeId'];
        // $dailyrecord['EndTagEnterTime'] = $result['EndTagEnterTime'];
        // $dailyrecord['EndTagLeaveTime'] = $result['EndTagLeaveTime'];
        $dailyrecord['DateTime'] = $result['DateTime'];

        array_push($inspectionpointdailyrecords, $dailyrecord);

    }
    //$db->close();

    return $inspectionpointdailyrecords;
}

function loadinspectionTagBehavior($project_id, $GroupID, $PointIndex, $Anchor, $date)
{
    $count = 0;
    $TagBehaviors = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT Inspector, Distance, issuetime FROM InspectorBehaviorData WHERE GroupID=".$GroupID." AND PointIndex=".$PointIndex." AND AnchorNodeId='".$Anchor."' AND DateTime='".$date."' ");

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

    return $TagBehaviors;
}

function loadinspectiongrouphistory($project_id, $GroupID, $startdate, $enddate)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT * from InspectionGroupDailyRecord WHERE GroupID=" . $GroupID. " AND DateTime >= '" . $startdate ."' AND DateTime <= '". $enddate ."' ORDER BY DateTime DESC");

    $count = 0;
    $hisotrys = [];
    while($result = $db->fetch_array())
    {
        $groupdailyrecord = [];
        $groupdailyrecord['Finished'] = $result['Finished'];
        $groupdailyrecord['Exceed'] = $result['Exceed'];
        $groupdailyrecord['DateTime'] = $result['DateTime'];
        $groupdailyrecord['Score'] = $result['Score'];

        $hisotrys[$count] = $groupdailyrecord;
        $count++;
    }

    return $hisotrys;
}

function loadinspectionpointdailyhistory($project_id, $GroupID, $startdate, $enddate)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT * from InspectionPointDailyRecord WHERE GroupID=" . $GroupID. " AND DateTime >= '" . $startdate ."' AND DateTime <= '" . $enddate ."'");

    $index = 0;
    $pathhisotrys = [];

    while($result = $db->fetch_array())
    {
        $dailyrecord = [];
        $dailyrecord['PointID'] = $result['PointID'];
        $dailyrecord['Checked'] = $result['Checked'];
        $dailyrecord['Finished'] = $result['Finished'];
        $dailyrecord['Exceed'] = $result['Exceed'];
        $dailyrecord['Score'] = $result['Score'];
        $dailyrecord['Checked'] = $result['Checked'];
        $dailyrecord['Finished'] = $result['Finished'];
        $dailyrecord['Exceed'] = $result['Exceed'];
        $dailyrecord['Inspector'] = $result['Inspector'];
        $dailyrecord['RecordNodeId'] = $result['RecordNodeId'];
        $dailyrecord['EnterTime'] = $result['EnterTime'];
        $dailyrecord['LeaveTime'] = $result['LeaveTime'];
        $dailyrecord['Status'] = $result['Status'];
        $dailyrecord['DateTime'] = $result['DateTime'];
        $pathhisotrys[$index] = $dailyrecord;
        $index++;
    }

    return $pathhisotrys;
}


function loadinspectorscore($project_id, $inspector, $startdate, $enddate)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT DateTime, SUM(Score) as score_total, COUNT(Score) as count FROM InspectionPointDailyRecord WHERE Inspector='$inspector' AND DateTime BETWEEN '$startdate' AND '$enddate' GROUP BY DateTime");

    $pathhisotrys = [];

    while($result = $db->fetch_array())
    {
        $dailyrecord = [];
        $dailyrecord['DateTime'] = $result['DateTime'];
        $dailyrecord['score_total'] = $result['score_total'];
        $dailyrecord['count'] = $result['count'];
        $pathhisotrys[$result['DateTime']] = $dailyrecord;
    }

    return $pathhisotrys;
}

?>
