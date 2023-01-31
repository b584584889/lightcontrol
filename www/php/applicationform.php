<?php

session_start();

require_once("../config.php");
require_once("common.php");
require_once("User.php");

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
        APPFORM_do_GET();
    break;
    case 'PUT':
    break;
    case 'POST':
        APPFORM_do_POST();
    break;
    case 'DELETE':
    break;
}

function APPFORM_do_GET()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $session_userid = $_SESSION["userid"];
    $session_groupid = $_SESSION["groupid"];

    if (isset($_GET['loadleaveformrecord']))
    {
        $offset = 0;
        $count = 0;
        $status = -1;
        $startdate = '';
        $enddate = '';
        $singleuser = '';

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            $offset = $_GET['s'];
            $count = $_GET['count'];
        }
        if (isset($_GET['status']))
        {
            $status = (int)$_GET['status'];
        }
        if (isset($_GET['startdate']))
        {
            $startdate = $_GET['startdate'];
        }
        if (isset($_GET['enddate']))
        {
            $enddate = $_GET['enddate'];
        }
        if (isset($_GET['singleuser']))
        {
            $singleuser = $_GET['singleuser'];
        }

        $subgroups = '';
        if ($session_groupid == 3)
        {
            //$subgroups = getUserSubGroups($project_id, $session_accountid);
            $status = loadleaveformrecord_myself($project_id, $startdate, $enddate, $offset, $count, $status, $session_userid);
        }
        else
        if ( strlen($singleuser) > 0 )
        {
            $status = loadleaveformrecord_myself($project_id, $startdate, $enddate, $offset, $count, $status, $singleuser);
        }
        else
        {
            // get user datagroup
            $subgroups = getUserDataGroups_ById($project_id, $session_userid);
            $subgroupids = explode(",", $subgroups);
            $status = loadleaveformrecord($project_id, $startdate, $enddate, $offset, $count, $status, $subgroupids, $session_userid);
        }
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadleaveformsignoffrecord']))
    {
        if (isset($_GET['formids']))
        {
            $formids = $_GET['formids'];
            //$formidlist = explode(",", $formids);

            $surveyeventlist = getSurveyEventfromLeaveFormSurveyEventRecord($project_id, $formids);
            $surveyresultlist = getSurveyResultFromsurveyEvent($project_id, $surveyeventlist);

            $index = 0;
            foreach ($surveyresultlist as $surveyresult)
            {
                $surveyresult['accountid'] = getAccountIdFromTopic($project_id, $surveyresult['topic']);
                $surveyresultlist[$index] = $surveyresult;
                $index++;
            }

            echo json_encode($surveyresultlist);
        }
        else
        {
            echo '[]';
        }
    }
}


function APPFORM_do_POST()
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    if (isset($_POST['create_new_leaveform']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['formid'] = $_POST['formid'];
        $record['applytime'] = $_POST['applytime'];
        // $record['accountid'] = $_POST['accountid'];
        $record['userid'] = $_SESSION["userid"];
        $record['type'] = $_POST['type'];
        $record['reason'] = $_POST['reason'];
        $record['startdate'] = $_POST['startdate'];
        $record['starttime'] = $_POST['starttime'];
        $record['enddate'] = $_POST['enddate'];
        $record['endtime'] = $_POST['endtime'];
        $record['reissue'] = $_POST['reissue'];
        $record['applyusername'] = $_POST['applyusername'];
        $record['applyuserrelation'] = $_POST['applyuserrelation'];
        $record['imageids'] = $_POST['imageids'];
        $record['fileids'] = $_POST['fileids'];
        $record['signoffstatus'] = $_POST['signoffstatus'];
        $record['signoffuserid'] = $_POST['signoffuserid'];
        $record['maingroups'] = $_SESSION["maingroups"];

        create_new_leaveform($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'newleaveform=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($record);
    }
    else
    if (isset($_POST['update_Signoff_state']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['formid'] = $_POST['formid'];
        $record['userid'] = $_POST["userid"];
        $record['type'] = $_POST['type'];
        $record['reason'] = $_POST['reason'];
        $record['startdate'] = $_POST['startdate'];
        $record['starttime'] = $_POST['starttime'];
        $record['enddate'] = $_POST['enddate'];
        $record['endtime'] = $_POST['endtime'];
        $record['applyusername'] = $_POST['applyusername'];
        $record['signoffstatus'] = $_POST['signoffstatus'];
        $record['signoffuserid'] = $_POST['signoffuserid'];
        $record['signoffaccountname'] = $_POST['signoffaccountname'];
        $record['maingroups'] = $_SESSION["maingroups"];

        update_Signoff_state($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'leaveformsignoff=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($record);
    }
}

function loadleaveformrecord($project_id, $startdate, $enddate, $offset, $count, $status, $subgroupids, $userid)
{
    $db = new DB($project_id, 'base');

    if ($status >= 0)
    {
        if (strlen($startdate) > 0 && strlen($enddate) > 0)
        {
            $db->query("SELECT * FROM applicationformleave WHERE signoffstatus=$status AND (startdate between '$startdate' AND '$enddate') ORDER BY updatetime DESC ");
        }
        else
        {
            $db->query("SELECT * FROM applicationformleave WHERE signoffstatus=$status ORDER BY updatetime DESC ");
        }
    }
    else
    {
        if (strlen($startdate) > 0 && strlen($enddate) > 0)
        {
            $db->query("SELECT * FROM applicationformleave WHERE startdate between '$startdate' AND '$enddate' ORDER BY updatetime DESC ");
        }
        else
        {
            $db->query("SELECT * FROM applicationformleave ORDER BY updatetime DESC ");
        }
    }

    $addall = false;
    foreach ($subgroupids as $groupid)
    {
        if ( $groupid == '99999' )
        {
            $addall = true;
        }
    }

    $records = [];
    $datacnt = 0;
    $fetchcnt = 0;
    while($result = $db->fetch_array())
    {
        if ($addall == false)
        {
            $canadd = false;
            $file_subgroups = $result['subgroups'];
            if (strlen($file_subgroups) == 0 )
            {
                // only myself can see
                if ( $result['userid'] == $userid )
                {
                    $canadd = true;
                }
            }
            else
            {
                $cur_subgroups = explode(",", $file_subgroups);

                foreach ($subgroupids as $groupid)
                {
                    foreach ($cur_subgroups as $subgroupid)
                    {
                        if ( $groupid == $subgroupid || $subgroupid == '99999' )
                        {
                            $canadd = true;
                            break;
                        }
                    }
                    if ($canadd)
                    {
                        break;
                    }
                }
            }

            if ($canadd == false)
            {
                continue;
            }
        }

        if ( $datacnt >= $offset && $datacnt <= ($offset+$count) )
        {
            $record = [];
            $record['formid'] = $result['formid'];
            $record['applytime'] = $result['applytime'];
            $record['userid'] = $result['userid'];
            $record['type'] = $result['type'];
            $record['reason'] = $result['reason'];
            $record['startdate'] = $result['startdate'];
            $record['starttime'] = $result['starttime'];
            $record['enddate'] = $result['enddate'];
            $record['endtime'] = $result['endtime'];
            $record['reissue'] = $result['reissue'];
            $record['applyusername'] = $result['applyusername'];
            $record['applyuserrelation'] = $result['applyuserrelation'];
            $record['imageids'] = $result['imageids'];
            $record['fileids'] = $result['fileids'];
            $record['signoffstatus'] = $result['signoffstatus'];
            $record['signoffuserid'] = $result['signoffuserid'];
            $record['subgroups'] = $result['subgroups'];
            $record['maingroups'] = $result['maingroups'];
            $record['datetime'] = $result['datetime'];
            $record['updatetime'] = $result['updatetime'];

            $records[$fetchcnt] = $record;
            $fetchcnt++;
        }
        $datacnt++;
    }
    return $records;
}

function loadleaveformrecord_myself($project_id, $startdate, $enddate, $offset, $count, $status, $userid)
{
    $db = new DB($project_id, 'base');

    if ($status >= 0)
    {
        if (strlen($startdate) > 0 && strlen($enddate) > 0)
        {
            $db->query("SELECT * FROM applicationformleave WHERE userid=$userid AND signoffstatus=$status AND (startdate between '$startdate' AND '$enddate') ORDER BY updatetime DESC ");
        }
        else
        {
            $db->query("SELECT * FROM applicationformleave WHERE userid=$userid AND signoffstatus=$status ORDER BY updatetime DESC ");
        }
    }
    else
    {
        if (strlen($startdate) > 0 && strlen($enddate) > 0)
        {
            $db->query("SELECT * FROM applicationformleave WHERE userid=$userid AND startdate between '$startdate' AND '$enddate' ORDER BY updatetime DESC ");
        }
        else
        {
            $db->query("SELECT * FROM applicationformleave WHERE userid=$userid ORDER BY updatetime DESC ");
        }
    }

    $records = [];
    $datacnt = 0;
    $fetchcnt = 0;
    while($result = $db->fetch_array())
    {
        if ( $datacnt >= $offset && $datacnt <= ($offset+$count) )
        {
            $record = [];
            $record['formid'] = $result['formid'];
            $record['applytime'] = $result['applytime'];
            $record['userid'] = $result['userid'];
            $record['type'] = $result['type'];
            $record['reason'] = $result['reason'];
            $record['startdate'] = $result['startdate'];
            $record['starttime'] = $result['starttime'];
            $record['enddate'] = $result['enddate'];
            $record['endtime'] = $result['endtime'];
            $record['reissue'] = $result['reissue'];
            $record['applyusername'] = $result['applyusername'];
            $record['applyuserrelation'] = $result['applyuserrelation'];
            $record['imageids'] = $result['imageids'];
            $record['fileids'] = $result['fileids'];
            $record['signoffstatus'] = $result['signoffstatus'];
            $record['signoffuserid'] = $result['signoffuserid'];
            $record['subgroups'] = $result['subgroups'];
            $record['maingroups'] = $result['maingroups'];
            $record['datetime'] = $result['datetime'];
            $record['updatetime'] = $result['updatetime'];

            $records[$fetchcnt] = $record;
            $fetchcnt++;
        }

        $datacnt++;
    }
    return $records;
}

function create_new_leaveform($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $formid = (int)$record['formid'];
    $applytime = $record['applytime'];
    $userid = $record['userid'];
    $type = $record['type'];
    $reason = $record['reason'];
    $startdate = $record['startdate'];
    $starttime = $record['starttime'];
    $enddate = $record['enddate'];
    $endtime = $record['endtime'];
    $reissue = (int)$record['reissue'];
    $applyusername = $record['applyusername'];
    $applyuserrelation = $record['applyuserrelation'];
    $imageids = $record['imageids'];
    $fileids = $record['fileids'];
    $signoffstatus = (int)$record['signoffstatus'];
    $signoffuserid = (int)$record['signoffuserid'];
    $maingroups = $record['maingroups'];

    $subgroups = getUserSubGroups_ById($project_id, $userid);

    $query_str = "INSERT INTO applicationformleave(formid, accountid, userid, subgroups, type, reason, startdate, starttime, " .
                      "enddate, endtime, reissue, applyusername, applyuserrelation, imageids, fileids, signoffstatus, signoffuserid, maingroups) " .
                      "VALUES ($formid, '', $userid, '$subgroups', '$type', '$reason', '$startdate', '$starttime', " .
                      "'$enddate', '$endtime', $reissue, '$applyusername', '$applyuserrelation', '$imageids', '$fileids', $signoffstatus, $signoffuserid, '$maingroups') ";

    $ret = $db->query($query_str);
    return $ret;
}

function update_Signoff_state($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $formid = (int)$record['formid'];
    $signoffstatus = (int)$record['signoffstatus'];
    $signoffuserid = (int)$record['signoffuserid'];

    $query_str = "UPDATE applicationformleave set signoffstatus=$signoffstatus, signoffuserid=$signoffuserid WHERE formid='$formid' ";

    $ret = $db->query($query_str);
    return $ret;
}

function getSurveyEventfromLeaveFormSurveyEventRecord($project_id, $formids)
{
    $surveyeventlist = [];

    $db = new DB($project_id, 'base');

    $datacnt = 0;
    // foreach ($formidlist as $formid)
    // {
        //$db->query("SELECT * FROM SurveyEventRecord WHERE formid='$formid' AND surveyid=88888888 ");
        $db->query("SELECT * FROM SurveyEventRecord WHERE formid in($formids) AND surveyid=88888888 ");

        while($result = $db->fetch_array())
        {
            $surveyevent = [];
            $surveyevent['formid'] = $result['formid'];
            $surveyevent['hashkey'] = $result['hashkey'];
            $surveyevent['PushNotifyMessageRecorddbId'] = $result['PushNotifyMessageRecorddbId'];
            $surveyeventlist[$datacnt] = $surveyevent;
            $datacnt++;
        }
    // }
    return $surveyeventlist;
}

function getSurveyResultFromsurveyEvent($project_id, $surveyeventlist)
{
    $surveyresultlist = [];

    $db = new DB($project_id, 'base');

    $datacnt = 0;
    foreach ($surveyeventlist as $surveyevent)
    {
        $hashkey = $surveyevent['hashkey'];
        $formid = $surveyevent['formid'];

        $db->query("SELECT * FROM SurveyResult WHERE hashkey='$hashkey' order by datetime DESC ");

        while($result = $db->fetch_array())
        {
            $surveyresult = [];
            $surveyresult['formid'] = $formid;
            $surveyresult['hashkey'] = $result['hashkey'];
            $surveyresult['topic'] = $result['topic'];
            $surveyresult['messagehashkey'] = $result['messagehashkey'];
            $surveyresult['comment'] = $result['comment'];
            $surveyresult['datetime'] = $result['datetime'];
            $surveyresultlist[$datacnt] = $surveyresult;
            $datacnt++;
        }
    }

    return $surveyresultlist;
}

function getAccountIdFromTopic($project_id, $topic)
{
    $AccountId = '';
    $db = new DB($project_id, 'base');

    $db->query("SELECT accountid FROM TargetTopic WHERE topic='$topic' ");

    if($result = $db->fetch_array())
    {
        $AccountId = $result['accountid'];
    }

    return $AccountId;
}

?>
