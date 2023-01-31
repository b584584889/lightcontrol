<?php

session_start();

require_once("common.php");
require_once("User.php");

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
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    if (isset($_GET['loadsurvey']))
    {
        $surveyid = 0;
        // $accountid = '';

        if (isset($_GET['surveyid']))
        {
            $surveyid = $_GET['surveyid'];
        }
        // if (isset($_GET['accountid']))
        //     $accountid = $_GET['accountid'];

        $session_userid = $_SESSION["userid"];
        $session_groupid = $_SESSION["groupid"];
        $session_maingroups = $_SESSION["maingroups"];

        //$status = loadsurvey($project_id, $accountid, $surveyid);
        $status = loadsurvey($project_id, $session_userid, $session_groupid, $session_maingroups, $surveyid);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadsurveys']))
    {
        $count = 0;
        $offset = 0;
        // $accountid = '';

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['offset']))
        {
            $offset = (int)$_GET['offset'];
        }
        // if (isset($_GET['accountid']))
        //     $accountid = $_GET['accountid'];

        $session_userid = $_SESSION["userid"];

        //$status = loadsurveys($project_id, $accountid, $offset, $count);
        $status = loadsurveys($project_id, $session_userid, $offset, $count);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadsurveyevents']))
    {
        $count = 0;
        $offset = 0;
        // $accountid = '';

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['offset']))
        {
            $offset = (int)$_GET['offset'];
        }
        // if (isset($_GET['accountid']))
        //     $accountid = $_GET['accountid'];

        $session_userid = $_SESSION["userid"];

        $status = loadsurveyevents($project_id, $session_userid, $offset, $count);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadsurveyevent']))
    {
        $surveyid = 0;
        $hashkey = "";
        // $accountid = '';

        if (isset($_GET['surveyid']))
        {
            $surveyid = $_GET['surveyid'];
        }
        if (isset($_GET['hashkey']))
        {
            $hashkey = $_GET['hashkey'];
        }
        // if (isset($_GET['accountid']))
        //     $accountid = $_GET['accountid'];

        $session_userid = $_SESSION["userid"];

        $status = loadsurveyevent($project_id, $session_userid, $surveyid, $hashkey);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadsurveyeventrecord']))
    {
        $PushNotifyMessageRecorddbId = 0;

        if (isset($_GET['PushNotifyMessageRecorddbId']))
        {
            $PushNotifyMessageRecorddbId = (int)$_GET['PushNotifyMessageRecorddbId'];
        }

        if ($PushNotifyMessageRecorddbId == 0)
        {
            echo '{}';
        }
        else
        {
            // echo '{}';
            $status = loadsurveyeventrecord($project_id, $PushNotifyMessageRecorddbId);
            echo json_encode($status);
        }
    }
    else
    if (isset($_GET['loadsurveyresult']))
    {
        $hashkey = '';
        $messagehashkey = '';
        // $loaduser = 0;
        if (isset($_GET['hashkey']))
        {
            $hashkey = $_GET['hashkey'];
        }
        if (isset($_GET['messagehashkey']))
        {
            $messagehashkey = $_GET['messagehashkey'];
        }
        // if (isset($_GET['loaduser']))
        // {
        //     $loaduser = (int)$_GET['loaduser'];
        // }

        $status = loadsurveyresult($project_id, $hashkey, $messagehashkey);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadsurveyresults']))
    {
        $hashkeys = '';
        $loaduser = 0;
        if (isset($_GET['hashkeys']))
        {
            $hashkeys = $_GET['hashkeys'];
        }
        if (isset($_GET['loaduser']))
        {
            $loaduser = (int)$_GET['loaduser'];
        }
        sendMessageToServer('updatesurveyresult');

        $status = loadsurveyresults($project_id, $hashkeys, $loaduser);
        echo json_encode($status);
    }
    else
    if (isset($_GET['updatesurveyresult']))
    {
        sendMessageToServer('updatesurveyresult');
        echo '[]';
    }
}

function REST_do_PUT()
{
}

function REST_do_POST()
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    if (isset($_POST['create_new_survey']))
    {
        $survey = [];
        $survey['project_id'] = $project_id;
        $survey['title'] = $_POST['title'];
        $survey['description'] = $_POST['description'];
        $survey['type'] = $_POST['type'];
        $survey['subgroups'] = $_POST['subgroups'];
        $survey['items'] = $_POST['items'];
        //$survey['accountid'] = $_POST['accountid'];

        // $session_accountid = $_SESSION["accountid"];
        //$session_groupid = $_SESSION["groupid"];
        // $survey['accountid'] = $session_accountid;
        $survey['userid'] = $_SESSION["userid"];
        $survey['maingroups'] = $_SESSION["maingroups"];

        save_new_survey($project_id, $survey);

        $json_string = json_encode($survey);
        echo $json_string;
    }
    else
    if (isset($_POST['update_survey']))
    {
        $survey = [];
        $survey['project_id'] = $project_id;
        $survey['surveyid'] = $_POST['surveyid'];
        $survey['title'] = $_POST['title'];
        $survey['description'] = $_POST['description'];
        $survey['type'] = $_POST['type'];
        $survey['subgroups'] = $_POST['subgroups'];
        $survey['items'] = $_POST['items'];
        // $survey['accountid'] = $_POST['accountid'];

        // $session_accountid = $_SESSION["accountid"];
        //$session_groupid = $_SESSION["groupid"];
        $survey['userid'] = $_SESSION["userid"];
        $survey['maingroups'] = $_SESSION["maingroups"];

        update_survey($project_id, $survey);

        $json_string = json_encode($survey);
        echo $json_string;
    }
    else
    if (isset($_POST['delete_survey']))
    {
        $survey = [];
        $survey['project_id'] = $project_id;
        $survey['surveyid'] = $_POST['surveyid'];

        delete_survey($project_id, $survey);

        $json_string = json_encode($survey);
        echo $json_string;
    }
    // else
    // if (isset($_POST['updatesurveyresult']))
    // {
    //     sendMessageToServer('updatesurveyresult');
    //
    //     echo '[]';
    // }
}

function REST_do_DELETE()
{
}

function save_new_survey($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $db->query("INSERT INTO Survey(title, description, type, userid, maingroups, subgroups) " .
                "VALUES ('".$record['title']."', '".$record['description']."', ".$record['type'].", ".$record['userid'].", '".$record['maingroups']."', '".$record['subgroups']."')");

    $surveyid = $db->get_insert_id();

    if ( is_array($record['items']) )
    {
        foreach ($record['items'] as $item)
        {
            $db->query("INSERT INTO SurveyItem(surveyid, description) " .
                        "VALUES ($surveyid, '$item')");
        }
    }
}

function update_survey($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $title = $record['title'];
    $description = $record['description'];
    $type = $record['type'];
    $surveyid = $record['surveyid'];
    // $accountid = $record['accountid'];
    $userid = $record['userid'];
    $maingroups = $record['maingroups'];
    $subgroups = $record['subgroups'];

    $db->query("UPDATE Survey SET title='$title', description='$description', type=$type, userid=$userid, maingroups='$maingroups', subgroups='$subgroups' WHERE surveyid=$surveyid");

    if ( is_array($record['items']) )
    {
        foreach ($record['items'] as $index => $item)
        {
            $surveyitemid = (int)$item['surveyitemid'];
            $surveyitemdelete = 0;
            if ( isset($item['surveyitemdelete']) )
                $surveyitemdelete = (int)$item['surveyitemdelete'];

            $itemdescription = $item['description'];
            if ( $surveyitemid > 0 )
            {
                if ($surveyitemdelete == 0)
                    $db->query("UPDATE SurveyItem SET description='$itemdescription' WHERE surveyid=$surveyid AND surveyitemid=$surveyitemid ");
                else
                if ($surveyitemdelete == 1)
                    $db->query("DELETE FROM SurveyItem WHERE surveyid=$surveyid AND surveyitemid=$surveyitemid ");
            }
            else
            {
                $db->query("INSERT INTO SurveyItem(surveyid, description) " .
                            "VALUES ($surveyid, '$itemdescription')");
                $surveyitemid = $db->get_insert_id();
                $item['surveyitemid'] = $surveyitemid;
                $record['items'][$index] = $item;
            }
        }
    }
}

function loadsurvey($project_id, $userid, $groupid, $maingroups, $surveyid)
{
    //$loadAll = check_account_permisson($project_id, $accountid);
    $loadAll = check_user_permisson($project_id, $userid);
    $records = [];
    $db = new DB($project_id, 'base');

    if ($loadAll)
    {
        $db->query("SELECT * from Survey WHERE surveyid=$surveyid ");
    }
    else
    {
        $db->query("SELECT * from Survey WHERE surveyid=$surveyid AND (userid=$userid OR subgroups LIKE '%99999%' OR maingroups LIKE '%$maingroups%' OR maingroups=''  OR maingroups='1' )");
    }

    if($result = $db->fetch_array())
    {
        $record['surveyid'] = $result['surveyid'];
        $record['title'] = $result['title'];
        $record['description'] = $result['description'];
        $record['type'] = $result['type'];
        // $record['accountid'] = $result['accountid'];
        $record['userid'] = $result['userid'];
        $record['maingroups'] = $result['maingroups'];
        $record['subgroups'] = $result['subgroups'];
        if ($record['subgroups'] == '')
            $record['subgroups'] = '99999';

        $record['DateTime'] = $result['DateTime'];
        $record['items'] = [];
        $records[0] = $record;


        $db->query("SELECT * from SurveyItem WHERE surveyid=$surveyid ORDER BY surveyitemid ASC ");

        $itemcount = 0;
        while($result = $db->fetch_array())
        {
            $item = [];
            $item['surveyitemid'] = $result['surveyitemid'];
            $item['description'] = $result['description'];
            $item['DateTime'] = $result['DateTime'];
            $record['items'][$itemcount] = $item;
            $records[0] = $record;
            $itemcount = $itemcount + 1;
        }
    }

    return $records;
}

function loadsurveys($project_id, $userid, $queryoffset, $querycount)
{
    $records = [];

    //$loadAll = check_account_permisson($project_id, $accountid);
    $loadAll = check_user_permisson($project_id, $userid);

    if ($loadAll)
    {
        $records = loadAllsurveys($project_id, $queryoffset, $querycount);
        return $records;
    }
    else
    {
        $records = loadPermissonsurveys($project_id, $userid, $queryoffset, $querycount);
        return $records;
    }
}

function loadAllsurveys($project_id, $queryoffset, $querycount)
{
    $records = [];
    $db = new DB($project_id, 'base');

    if ( $querycount == 0 )
        $db->query("SELECT * from Survey ORDER BY datetime DESC ");
    else
        $db->query("SELECT * from Survey ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['surveyid'] = $result['surveyid'];
        $record['title'] = $result['title'];
        $record['description'] = $result['description'];
        $record['type'] = $result['type'];
        // $record['accountid'] = $result['accountid'];
        $record['userid'] = $result['userid'];
        $record['maingroups'] = $result['maingroups'];
        $record['subgroups'] = $result['subgroups'];
        if ($record['subgroups'] == '')
            $record['subgroups'] = '99999';
        $record['DateTime'] = $result['DateTime'];
        $record['items'] = [];
        $records[$count] = $record;
        $count = $count + 1;
    }

    foreach ($records as $index => $record)
    {
        $itemcount = 0;
        $db->query("SELECT * from SurveyItem WHERE surveyid=".$record['surveyid']." ORDER BY surveyitemid ASC ");
        while($result = $db->fetch_array())
        {
            $item = [];
            $item['surveyitemid'] = $result['surveyitemid'];
            $item['description'] = $result['description'];
            $item['DateTime'] = $result['DateTime'];
            $record['items'][$itemcount] = $item;
            $records[$index] = $record;
            $itemcount = $itemcount + 1;
        }
    }

    return $records;
}

function loadPermissonsurveys($project_id, $userid, $queryoffset, $querycount)
{
    $records = [];
    $db = new DB($project_id, 'base');

    // $curUser_subgroups = getUserSubGroups($project_id, $accountid);
    $curUser_subgroups = getUserDataGroups_ById($project_id, $userid);
    $curUser_subgroupids = explode(",", $curUser_subgroups);

    // echo '$curUser_subgroups:'.$curUser_subgroups;

    $db->query("SELECT * from Survey ORDER BY datetime DESC ");

    $survey_cnt = 0;
    $recordcount = 0;
    while($result = $db->fetch_array())
    {
        $found = false;
        $subgroups = $result['subgroups'];
        $maingroups = $result['maingroups'];

        //if ($subgroups == '99999')
        if ($subgroups == '' || $subgroups == '99999' || $maingroups == '1')
        {
            // ok
            $found = true;
        }
        else
        {
            // echo ' loadPermissonsurveys not all ok surveyid:' . $result['surveyid'].'    ';
            $subgroup_list = explode(",", $subgroups);
            foreach ($subgroup_list as $subgroup)
            {
                if( $subgroup == '99999')
                {
                    $found = true;
                    break;
                }

                //
                // check user subgroup
                //
                foreach ($curUser_subgroupids as $curUser_subgroupid)
                {
                    if( $subgroup == $curUser_subgroupid)
                    {
                        $found = true;
                        break;
                    }
                }
                if ($found == true)
                    break;
            }
        }

        if ($found == true)
        {
            // echo ' loadPermissonsurveys $survey_cnt:'.$survey_cnt.' $queryoffset:'.$queryoffset.' $recordcount:'.$recordcount.' $querycount:'.$querycount;
            if ( (($survey_cnt >= $queryoffset) && ($recordcount < $querycount)) || ($querycount == 0) )
            {
                $record = [];
                $record['surveyid'] = $result['surveyid'];
                $record['title'] = $result['title'];
                $record['description'] = $result['description'];
                $record['type'] = $result['type'];
                // $record['accountid'] = $result['accountid'];
                $record['userid'] = $result['userid'];
                $record['maingroups'] = $result['maingroups'];
                $record['subgroups'] = $result['subgroups'];
                if ($record['subgroups'] == '')
                    $record['subgroups'] = '99999';
                $record['DateTime'] = $result['DateTime'];
                $record['items'] = [];
                $records[$recordcount] = $record;
                $recordcount = $recordcount + 1;
            }

            $survey_cnt = $survey_cnt + 1;
        }
    }

    foreach ($records as $index => $record)
    {
        $itemcount = 0;
        $db->query("SELECT * from SurveyItem WHERE surveyid=".$record['surveyid']." ORDER BY surveyitemid ASC ");
        while($result = $db->fetch_array())
        {
            $item = [];
            $item['surveyitemid'] = $result['surveyitemid'];
            $item['description'] = $result['description'];
            $item['DateTime'] = $result['DateTime'];
            $record['items'][$itemcount] = $item;
            $records[$index] = $record;
            $itemcount = $itemcount + 1;
        }
    }

    return $records;
}

// function loadPermissonsurveys($project_id, $accountid, $groupid, $queryoffset, $querycount)
// {
//     $records = [];
//     $db = new DB($project_id, 'base');
//
//     if ( $querycount == 0 )
//         $db->query("SELECT * from Survey WHERE accountid='$accountid' ORDER BY datetime DESC ");
//     else
//         $db->query("SELECT * from Survey WHERE accountid='$accountid' ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");
//
//     $count = 0;
//     while($result = $db->fetch_array())
//     {
//         $record = [];
//         $record['surveyid'] = $result['surveyid'];
//         $record['title'] = $result['title'];
//         $record['description'] = $result['description'];
//         $record['type'] = $result['type'];
//         $record['accountid'] = $result['accountid'];
//         $record['subgroups'] = $result['subgroups'];
//         if ($record['subgroups'] == '')
//             $record['subgroups'] = '99999';
//         $record['DateTime'] = $result['DateTime'];
//         $record['items'] = [];
//         $records[$count] = $record;
//         $count = $count + 1;
//     }
//
//     foreach ($records as $index => $record)
//     {
//         $itemcount = 0;
//         $db->query("SELECT * from SurveyItem WHERE surveyid=".$record['surveyid']." ORDER BY surveyitemid ASC ");
//         while($result = $db->fetch_array())
//         {
//             $item = [];
//             $item['surveyitemid'] = $result['surveyitemid'];
//             $item['description'] = $result['description'];
//             $item['DateTime'] = $result['DateTime'];
//             $record['items'][$itemcount] = $item;
//             $records[$index] = $record;
//             $itemcount = $itemcount + 1;
//         }
//     }
//
//     return $records;
// }

function delete_survey($project_id, $survey)
{
    $surveyid = $survey['surveyid'];

    $db = new DB($project_id, 'base');
    $db->query("DELETE FROM Survey WHERE surveyid=$surveyid ");
}

function loadsurveyevent($project_id, $userid, $surveyid, $hashkey)
{
    // $loadAll = check_account_permisson($project_id, $accountid);
    $loadAll = check_user_permisson($project_id, $userid);
    $records = [];
    $db = new DB($project_id, 'base');

    if ($loadAll)
    {
        $db->query("SELECT * from SurveyEventRecord WHERE surveyid=$surveyid AND hashkey='$hashkey' ");
    }
    else
    {
        // $db->query("SELECT * from SurveyEventRecord WHERE surveyid=$surveyid AND hashkey='$hashkey' AND userid=$userid ");
        $db->query("SELECT * from SurveyEventRecord WHERE surveyid=$surveyid AND hashkey='$hashkey' AND (userid=$userid OR maingroups LIKE '%$maingroups%' OR maingroups='' OR maingroups='1' )");

    }

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['eventid'] = $result['eventid'];
        $record['surveyid'] = $result['surveyid'];
        $record['hashkey'] = $result['hashkey'];
        $record['deadline'] = $result['deadline'];
        // $record['accountid'] = $result['accountid'];
        $record['userid'] = $result['userid'];
        $record['maingroups'] = $result['maingroups'];
        //$record['PushNotifyMessagedbId'] = $result['PushNotifyMessagedbId'];
        $record['PushNotifyMessageRecorddbId'] = $result['PushNotifyMessageRecorddbId'];
        $record['DateTime'] = $result['DateTime'];
        $records[$count] = $record;
        $count = $count + 1;
    }

    return $records;
}

function loadsurveyeventrecord($project_id, $PushNotifyMessageRecorddbId)
{
    $record = [];
    $db = new DB($project_id, 'base');

    $db->query("SELECT * from SurveyEventRecord WHERE PushNotifyMessageRecorddbId=$PushNotifyMessageRecorddbId ");

    if($result = $db->fetch_array())
    {
        $record['eventid'] = $result['eventid'];
        $record['surveyid'] = $result['surveyid'];
        $record['hashkey'] = $result['hashkey'];
        $record['deadline'] = $result['deadline'];
        // $record['accountid'] = $result['accountid'];
        $record['userid'] = $result['userid'];
        $record['maingroups'] = $result['maingroups'];
        $record['PushNotifyMessageRecorddbId'] = $result['PushNotifyMessageRecorddbId'];
        $record['DateTime'] = $result['DateTime'];
    }

    return $record;
}

function loadsurveyevents($project_id, $userid, $queryoffset, $querycount)
{
    //$loadAll = check_account_permisson($project_id, $accountid);
    $loadAll = check_user_permisson($project_id, $userid);
    $records = [];
    $db = new DB($project_id, 'base');

    if ($loadAll)
    {
        if ( $querycount == 0 )
            $db->query("SELECT * from SurveyEventRecord ORDER BY datetime DESC ");
        else
            $db->query("SELECT * from SurveyEventRecord ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");
    }
    else
    {
        if ( $querycount == 0 )
            $db->query("SELECT * from SurveyEventRecord WHERE userid=$userid ORDER BY datetime DESC ");
        else
            $db->query("SELECT * from SurveyEventRecord WHERE userid=$userid ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");
    }

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['eventid'] = $result['eventid'];
        $record['surveyid'] = $result['surveyid'];
        $record['hashkey'] = $result['hashkey'];
        $record['deadline'] = $result['deadline'];
        // $record['accountid'] = $result['accountid'];
        $record['userid'] = $result['userid'];
        $record['maingroups'] = $result['maingroups'];
        $record['PushNotifyMessageRecorddbId'] = $result['PushNotifyMessageRecorddbId'];
        $record['DateTime'] = $result['DateTime'];
        $records[$count] = $record;
        $count = $count + 1;
    }

    return $records;
}

function loadsurveyresult($project_id, $hashkey, $messagehashkey)
{
    $record = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * From SurveyResult WHERE hashkey='$hashkey' AND messagehashkey='$messagehashkey' ORDER BY updatetime DESC LIMIT 1 ");

    if($result = $db->fetch_array())
    {
        $record['topic'] = $result['topic'];
        $record['messagehashkey'] = $result['messagehashkey'];
        $record['comment'] = $result['comment'];
        $record['results'] = $result['results'];
        $record['datetime'] = $result['datetime'];
    }

    return $record;
}

function loadsurveyresults($project_id, $hashkeys, $loaduser)
{
    $results = [];

    $db = new DB($project_id, 'base');

    $hashkeyarray = explode(",", $hashkeys);

    foreach ($hashkeyarray as $index => $hashkey)
    {
        $records = [];
        $itemcount = 0;
        $db->query("SELECT * from SurveyResult WHERE hashkey='$hashkey' order by datetime DESC ");
        while($result = $db->fetch_array())
        {
            $record = [];
            $record['topic'] = $result['topic'];
            $record['messagehashkey'] = $result['messagehashkey'];
            $record['comment'] = $result['comment'];
            $record['results'] = $result['results'];
            $record['datetime'] = $result['datetime'];

            $records[$itemcount] = $record;
            $itemcount = $itemcount + 1;
        }

        $results[$hashkey] = $records;
    }

    if ($loaduser == 1)
    {
        foreach ($results as $hashkey => $records)
        {
            foreach ($records as $index => $record)
            {
                $record['targetid'] = $record['topic'];

                $db->query("SELECT target,name from TargetTopic WHERE topic='".$record['topic'] ."' ");
                if($result = $db->fetch_array())
                {
                    $record['targetid'] = $result['target'];
                    $record['name'] = $result['name'];
                    $records[$index] = $record;
                }

                $records[$index] = $record;
                $results[$hashkey] = $records;
            }
            $results[$hashkey] = $records;
        }
    }

    return $results;
}

?>
