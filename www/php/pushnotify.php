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

    if (isset($_GET['loadpushnotifysetting']))
    {
        $nodeid = $_GET['nodeid'];
        $status = loadpushnotifysetting($project_id, $nodeid);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadtriggermessage']))
    {
        $nodeid = $_GET['nodeid'];
        $status = loadtriggermessage($project_id, $nodeid);
        echo json_encode($status);
    }
    else
    if (isset($_GET['waitpushnotify']))
    {
        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['waitkey'] =$_GET['waitkey'];
        $json_string = json_encode($json_array);
        $status = getServerInfo('waitpushnotify='.$json_string);
        echo $status;
    }
    else
    if (isset($_GET['loadrecords']))
    {
        $count = (int)$_GET['count'];
        $offset = (int)$_GET['offset'];
        $type = '0';
        $searchtargetidList = [];
        $searchdate = '';

        if (isset($_GET['type']))
            $type = $_GET['type'];

        $session_userid = $_SESSION["userid"];
        $session_maingroups = $_SESSION["maingroups"];
        $groupid = $_SESSION["groupid"];

        if (isset($_GET['searchtarget']))
        {
            $searchtarget = $_GET['searchtarget'];

            $searchtargetidList = serach_targetids($project_id, $session_userid, $searchtarget, $session_maingroups);
        }
        if (isset($_GET['searchdate']))
        {
            $searchdate = $_GET['searchdate'];
        }

        $status = loadrecords_new($project_id, $offset, $count, $type, $groupid, $session_userid, $searchtargetidList, $searchdate, $session_maingroups);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadcannedmesages']))
    {
        $status = loadcannedmesages($project_id);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadnotifyimage']))
    {
        $messagehashkey = $_GET['messagehashkey'];

        $status = loadnotifyimage($project_id, $messagehashkey);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadmessagerecord']))
    {
        $dbids = "";
        if (isset($_GET['dbids']))
        {
            $dbids = $_GET['dbids'];
        }
        $status = loadmessagerecords($project_id, $dbids);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadmessagestatus']))
    {
        $hashkeys = "";
        if (isset($_GET['hashkeys']))
        {
            $hashkeys = $_GET['hashkeys'];
        }
        $loadaccount = 0;
        if (isset($_GET['loadaccount']))
        {
            $loadaccount = 1;
        }
        $loadaccountname = 0;
        if (isset($_GET['loadaccountname']))
        {
            $loadaccountname = 1;
        }
        $status = loadmessagestatus($project_id, $hashkeys, $loadaccount, $loadaccountname);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadbillboardrecords']))
    {
        $count = (int)$_GET['count'];
        $offset = (int)$_GET['offset'];
        $searchtitle = '';
        $searchdate = '';

        if (isset($_GET['searchtitle']))
        {
            $searchtitle = $_GET['searchtitle'];
        }
        if (isset($_GET['searchdate']))
        {
            $searchdate = $_GET['searchdate'];
        }

        $accountid = $_SESSION["accountid"];
        $userid = $_SESSION["userid"];
        $groupid = $_SESSION["groupid"];
        $maingroups = $_SESSION["maingroups"];

        $status = loadbillboardrecords($project_id, $userid, $groupid, $maingroups, $searchtitle, $searchdate, $count, $offset);
        echo json_encode($status);
    }
    // else
    // if (isset($_GET['testsend']))
    // {
    //     sendGCM();
    // }
}

/*
// function sendGCM()
// {
//     $url = 'https://fcm.googleapis.com/v1/projects/smimsvils/messages:send';
//
//     // $fields = array (
//     //         'registration_ids' => array (
//     //                 "id"
//     //         ),
//     //         'data' => array (
//     //                 "message" => "message"
//     //         )
//     // );
//
//     $fields = array ('message' => array (
//                     'notification' => array (   'title' => '管理室通知',
//                                                 'body' => '請同仁完成並互相回報.' ),
//                     'data' => array (   'title' => '管理室通知',
//                                         'body' => '請同仁完成並互相回報.',
//                                         'datetime' => '20200811141707',
//                                         'unsubscribe' => '',
//                                         'topic' => 'PdbLq0uk0Q9tz3di',
//                                         'messagehashkey' => 'messagehashkey',
//                                         'imageuid' => '1597126627329',
//                                         'todo' => ''),
//                     'topic' => 'PdbLq0uk0Q9tz3di',
//                     'apns' => array (   'headers' => array ('apns-priority' => '10'),
//                                         'payload' => array ('aps' => array ('badge' => '0'),
//                                                             'datetime' => '20200811141707',
//                                                             'messagehashkey' => 'messagehashkey',
//                                                             'imageuid' => '1597126627329',
//                                                             'todo' => '')
//                                      )
//             ));
//
//
//
//
//     $fields = json_encode ( $fields );
//
//     $headers = array (
//             'Authorization: Bearer ' . 'BPdDouDboDyFOcxnySjefFPVWrRj64_sY8E4CICzTgJ-0qEWe5swgO4G22Ynn4VWigMptGimJHqRC6e9knDk6qE',
//             'Content-Type: application/json'
//     );
//
//     $ch = curl_init ();
//     curl_setopt ( $ch, CURLOPT_URL, $url );
//     curl_setopt ( $ch, CURLOPT_POST, true );
//     curl_setopt ( $ch, CURLOPT_HTTPHEADER, $headers );
//     curl_setopt ( $ch, CURLOPT_RETURNTRANSFER, true );
//     curl_setopt ( $ch, CURLOPT_POSTFIELDS, $fields );
//
//     $result = curl_exec ( $ch );
//     echo $result;
//     curl_close ( $ch );
// }
*/
function REST_do_PUT()
{
}

function REST_do_POST()
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    if (isset($_POST['setpushnotifyprefix']))
    {
        $systemconfig = [];
        $systemconfig['project_id'] = $project_id;
        $systemconfig['PushNotifyPrefix'] = $_POST['setpushnotifyprefix'];
        $systemconfig['PushNotifyWelcomeTitle'] = $_POST['setpushnotifywelcometitle'];
        $systemconfig['PushNotifyWelcomeBody'] = $_POST['setpushnotifywelcomebody'];
        $systemconfig['EnablePushNotify'] = $_POST['setenablepushnotify'];
        $json_string = json_encode($systemconfig);
        sendMessageToServer('updatesystemconfigudpport='.$json_string);

        echo $json_string;
    }
    else
    if (isset($_POST['UPDATE_PUSHNOTIFY']))
    {
        $functions = "";
        $account = "";
        if (isset($_POST['FUNCTIONS']))
            $functions = $_POST['FUNCTIONS'];
        if (isset($_POST['ACCOUNT']))
            $account = $_POST['ACCOUNT'];

        update_PushNotify($project_id, $account, $functions);

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['ACCOUNT'] = $account;
        $json_string = json_encode($json_array);
        sendMessageToServer('updatePushNotify='.$json_string);
    }
    else
    if (isset($_POST['UPDATE_TRIGGERMESSAGE']))
    {
        $functions = "";
        $account = "";
        if (isset($_POST['FUNCTIONS']))
            $functions = $_POST['FUNCTIONS'];
        if (isset($_POST['ACCOUNT']))
            $account = $_POST['ACCOUNT'];

        update_TriggerMessage($project_id, $account, $functions);

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['ACCOUNT'] = $account;
        $json_string = json_encode($json_array);
        sendMessageToServer('updatePushNotifyTriggerMessage='.$json_string);
    }
    // else
    // if (isset($_POST['sendnotification']))
    // {
    //     $json_string = json_encode($_POST);
    //     $status_str = getServerInfo('sendnotification='.$json_string);
    //     echo $status_str;
    // }
    else
    if (isset($_POST['sendmultinotification']))
    {
        if (isset($_POST['subgrouplist']))
        {
            $subgrouplist = $_POST['subgrouplist'];
            $array_subgroup = array_values($subgrouplist);

            $targetlist = [];
            $subgrouptargetlist = [];
            if (isset($_POST['targetlist']))
            {
                $targetlist = array_values($_POST['targetlist']);
            }

            $session_accountid = $_SESSION["accountid"];
            $session_maingroups = $_SESSION["maingroups"];

            $alluser = loadmaingroupuser($project_id, $session_accountid, $session_maingroups);
            $targetcount = count($targetlist);
            $count = 0;
            if ( is_array($array_subgroup) )
            {
                foreach ($array_subgroup as $index => $subgroupid)
                {
                    $int_subgroupid = (int)$subgroupid;

                    //
                    // sene to subgroup manager
                    //
                    $subgrouprecord = loadsubgroupbyid($project_id, $int_subgroupid);
                    $userids = explode(",", $subgrouprecord['userids']);
                    foreach ($userids as $userid)
                    {
                        $targetlist[$targetcount] = $userid;
                        $targetcount = $targetcount + 1;
                    }
                    // $accountids = explode(",", $subgrouprecord['accountids']);
                    // foreach ($accountids as $accountid)
                    // {
                    //     $targetlist[$targetcount] = $accountid;
                    //     $targetcount = $targetcount + 1;
                    // }

                    foreach ($alluser as $userindex => $user)
                    {
                        $subgroups = explode(",", $user['subgroups']);
                        // $found = false;
                        foreach ($subgroups as $user_subgroupid)
                        {
                            $int_user_subgroupid = (int)$user_subgroupid;

                            if ( $int_subgroupid == $int_user_subgroupid )
                            {
                                //
                                // get nodeid from userid
                                //
                                $nodeid = get_node_nodeid_from_userid($project_id, $user['userid']);
                                if (strlen($nodeid) > 0 )
                                {
                                    $subgrouptargetlist[$count] = $nodeid;
                                    $count = $count + 1;
                                }
                                // send to user account also.
                                //$subgrouptargetlist[$count] = $user['account'];
                                $subgrouptargetlist[$count] = $user['userid'];
                                $count = $count + 1;

                                // $found = true;
                                break;
                            }
                        }
                    }
                }
                $_POST['subgrouptargetlist'] = $subgrouptargetlist;
            }
            $_POST['targetlist'] = $targetlist;
        }

        $_POST['accountid'] = $_SESSION["accountid"];
        $_POST['userid'] = $_SESSION["userid"];
        $_POST['maingroups'] = $_SESSION["maingroups"];

        $json_string = json_encode($_POST);

        $status_str = getServerInfo('sendmultinotification='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_POST['createbillboard']))
    {
        $sendpushnotify = $_POST['sendpushnotify'];
        $subgrouplist = $_POST['subgrouplist'];
        $title = $_POST['title'];
        $content = $_POST['content'];
        $imageuid = $_POST['imageuid'];
        $accountid = $_SESSION['accountid'];
        $userid = $_SESSION["userid"];
        $maingroups = $_SESSION["maingroups"];

        create_billboard($project_id, $accountid, $userid, $sendpushnotify, $subgrouplist, $title, $content, $imageuid, $maingroups);

        $json_string = json_encode($_POST);
        echo $json_string;
    }
    else
    if (isset($_POST['delete_billboard']))
    {
        $recordid = $_POST['recordid'];

        delete_billboard($project_id, $recordid);

        $json_string = json_encode($_POST);
        echo $json_string;
    }
}

function REST_do_DELETE()
{
}


function update_PushNotify($project_id, $nodeid, $functions)
{
    $db = new DB($project_id, 'base');
    //====================
    // $ret = $db->query("UPDATE PushNotifyUser SET functions='". $functions ."' WHERE account='".$account."'");
    //====================
    if ( is_array($functions) )
    {
        foreach ($functions as $targetid => $function)
        {
            $ret = $db->query("UPDATE PushNotifySetting SET functions='".$function."' WHERE nodeid='".$nodeid."' and targetid='".$targetid."' ");
            $row = $db->affected_rows();
            if ( $row <= 0 )
            {
                $ret = $db->query("INSERT INTO PushNotifySetting (nodeid, targetid, functions) VALUES ('".$nodeid."','".$targetid."','".$function."')");
            }
        }
    }
    else
    {
        $ret = $db->query("UPDATE PushNotifySetting SET functions='".$functions."' WHERE nodeid='".$nodeid."' and targetid='".$nodeid."' ");
        $row = $db->affected_rows();
        if ( $row <= 0 )
        {
            $ret = $db->query("INSERT INTO PushNotifySetting (nodeid, targetid, functions) VALUES ('".$nodeid."','".$nodeid."','".$functions."')");
        }
    }
    //====================
    return $ret;
}

function update_TriggerMessage($project_id, $nodeid, $functions)
{
    $db = new DB($project_id, 'base');
    if ( is_array($functions) )
    {
        foreach ($functions as $type => $functions)
        {
            //echo 'type:'.$type.' distance:'.$functions['distance'];

            $enable = $functions['enable'];
            $distance = $functions['distance'];
            $title = $functions['title'];
            $message = $functions['message'];

            $ret = $db->query("UPDATE TriggerPushMessage SET enable=$enable, distance=$distance, title='$title', message='$message' WHERE nodeid='$nodeid' and targettype=$type ");
            $row = $db->affected_rows();
            if ( $row <= 0 )
            {
                $ret = $db->query("INSERT INTO TriggerPushMessage (enable, nodeid, targettype, distance, title, message) VALUES ($enable,'$nodeid',$type,$distance,'$title','$message')");
            }
        }
    }
    // else
    // {
    //     echo 'functions:'.$functions;
    // }
}

function loadpushnotifysetting($project_id, $nodeid)
{
    $settings = [];

    $db = new DB($project_id, 'base');

    $count = 0;
    $db->query("SELECT * from PushNotifySetting WHERE nodeid='".$nodeid."' ");
    while($result = $db->fetch_array())
    {
        $setting = [];
        $setting['targetid'] = $result['targetid'];
        $setting['functions'] = $result['functions'];
        $settings[$count] = $setting;
        $count = $count + 1;
    }

    return $settings;
}

function loadtriggermessage($project_id, $nodeid)
{
    $settings = [];

    $db = new DB($project_id, 'base');

    $count = 0;
    $db->query("SELECT * from TriggerPushMessage WHERE nodeid='".$nodeid."' ");
    while($result = $db->fetch_array())
    {
        $setting = [];
        $setting['enable'] = $result['enable'];
        $setting['nodeid'] = $result['nodeid'];
        $setting['targettype'] = $result['targettype'];
        $setting['distance'] = $result['distance'];
        $setting['title'] = $result['title'];
        $setting['message'] = $result['message'];
        $settings[$count] = $setting;
        $count = $count + 1;
    }

    return $settings;
}

function loadrecords_new($project_id, $queryoffset, $querycount, $type, $groupid, $userid, $searchtargetidList, $searchdate, $maingroups)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $loadall = false;

    if ($groupid == 1 || $groupid == 2)
    {
        $userid = 0;
        $loadall = true;
    }
    if ($groupid == 5)
    {
        $userid = 0;
    }

    if ($type == '0' && $userid == 0)
    {
        $db->query("SELECT * from PushNotifyMessageRecord ORDER BY datetime DESC ");
    }
    else
    if ($type == '0')
    {
        if (strlen($searchdate) > 0)
        {
            $db->query("SELECT * from PushNotifyMessageRecord WHERE (userid=$userid OR targetids LIKE '%{$userid}%' ) AND datetime LIKE '%{$searchdate}%' ORDER BY datetime DESC ");
        }
        else
        {
            $db->query("SELECT * from PushNotifyMessageRecord WHERE userid=$userid OR targetids LIKE '%{$userid}%' ORDER BY datetime DESC ");
        }
    }
    else
    if ($userid == 0)
    {
        if (strlen($searchdate) > 0)
        {
            $db->query("SELECT * from PushNotifyMessageRecord WHERE type in ($type) AND datetime LIKE '%{$searchdate}%' ORDER BY datetime DESC ");
        }
        else
        {
            $db->query("SELECT * from PushNotifyMessageRecord WHERE type in ($type) ORDER BY datetime DESC ");
        }

    }
    else
    {
        if (strlen($searchdate) > 0)
        {
            $db->query("SELECT * from PushNotifyMessageRecord WHERE type in ($type) AND (userid=$userid OR targetids LIKE '%{$userid}%') AND datetime LIKE '%{$searchdate}%' ORDER BY datetime DESC ");
        }
        else
        {

            $db->query("SELECT * from PushNotifyMessageRecord WHERE type in ($type) AND (userid=$userid OR targetids LIKE '%{$userid}%') ORDER BY datetime DESC ");
        }
    }

    $maingroupList = explode(",", $maingroups);

    $data_count = 0;
    $valid_count = 0;
    while($result = $db->fetch_array())
    {
        $check_ok = false;
        if (count($searchtargetidList) > 0)
        {
            $check_ok = false;
            $targetidList = explode(",", $result['targetids']);

            foreach ($searchtargetidList as $searchid)
            {
                foreach ($targetidList as $targetid)
                {
                    if( $searchid == $targetid )
                    {
                        $check_ok = true;
                        break;
                    }
                }
                if ($check_ok == true)
                {
                    break;
                }
            }
        }
        else
        {
            if ($userid != 0)
            {
                // 檢查正確是此accountid
                if ( $result['userid'] == $userid )
                {
                    $check_ok = true;
                }
                else
                {
                    $targetidList = explode(",", $result['targetids']);
                    foreach ($targetidList as $targetid)
                    {
                        if( $userid == $targetid )
                        {
                            $check_ok = true;
                            break;
                        }
                    }
                }
            }
            if ($loadall)
            {
                $check_ok = true;
            }
            else
            {
                // check maingroup
                $check_ok = check_valid_maingroup($maingroupList, $result['maingroups']);

                //$check_ok = true;
            }
        }

        if ($check_ok == false)
        {
            continue;
        }

        if ( $data_count < $queryoffset )
        {
            $data_count = $data_count + 1;
            continue;
        }
        $data_count = $data_count + 1;

        if ( $valid_count >= $querycount )
        {
            break;
        }

        $record = [];
        $record['dbId'] = $result['dbId'];
        $record['type'] = $result['type'];
        $record['nodeid'] = $result['nodeid'];
        $record['userid'] = $result['userid'];
        $record['title'] = $result['title'];
        $record['message'] = $result['message'];
        $record['messagehashkey'] = $result['messagehashkey'];
        $record['targetids'] = $result['targetids'];
        $record['subgroupids'] = $result['subgroupids'];
        $record['surveyids'] = $result['surveyids'];
        $record['hashkeys'] = $result['hashkeys'];
        $record['deadlines'] = $result['deadlines'];
        $record['imageuid'] = $result['imageuid'];
        $record['datetime'] = $result['datetime'];
        $record['maingroups'] = $result['maingroups'];

        if ($record['subgroupids'] == null)
            $record['subgroupids'] = '';
        if ($record['deadlines'] == null)
            $record['deadlines'] = '';
        if ($record['imageuid'] == null)
            $record['imageuid'] = '';

        $records[$valid_count] = $record;
        $valid_count = $valid_count + 1;
    }

    return $records;
}

function loadcannedmesages($project_id)
{
    $records = [];
    $db = new DB($project_id, 'base');
    $db->query("SELECT * from CannedMessage ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['dbId'] = $result['dbId'];
        $record['type'] = $result['type'];
        $record['menutitle'] = $result['menutitle'];
        $record['title'] = $result['title'];
        $record['message'] = $result['message'];
        $record['datetime'] = $result['datetime'];
        $records[$count] = $record;
        $count = $count + 1;
    }

    return $records;
}

function loadnotifyimage($project_id, $messagehashkey)
{
    $record = [];
    $db = new DB($project_id, 'base');
    $db->query("SELECT imageuid from PushNotifyMessageRecord WHERE messagehashkey='$messagehashkey' ");

    $count = 0;
    if($result = $db->fetch_array())
    {
        $record['imageuid'] = $result['imageuid'];
    }

    return $record;
}

function loadmessagerecords($project_id, $dbids)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $dbidarray = explode(",", $dbids);

    foreach ($dbidarray as $index => $dbid)
    {
        $itemcount = 0;

        $db->query("SELECT * from PushNotifyMessageRecord WHERE dbId=$dbid ");
        if($result = $db->fetch_array())
        {
            $record = [];
            $record['dbId'] = $result['dbId'];
            $record['type'] = $result['type'];
            $record['nodeid'] = $result['nodeid'];
            $record['userid'] = $result['userid'];
            $record['title'] = $result['title'];
            $record['message'] = $result['message'];
            $record['messagehashkey'] = $result['messagehashkey'];
            $record['targetids'] = $result['targetids'];
            $record['subgroupids'] = $result['subgroupids'];
            $record['surveyids'] = $result['surveyids'];
            $record['hashkeys'] = $result['hashkeys'];
            $record['deadlines'] = $result['deadlines'];
            $record['imageuid'] = $result['imageuid'];
            $record['datetime'] = $result['datetime'];
            $record['maingroups'] = $result['maingroups'];

            if ($record['subgroupids'] == null)
                $record['subgroupids'] = '';
            if ($record['deadlines'] == null)
                $record['deadlines'] = '';
            if ($record['imageuid'] == null)
                $record['imageuid'] = '';

            $records[$itemcount] = $record;
            $itemcount = $itemcount + 1;
        }
    }

    return $records;
}

function loadmessagestatus($project_id, $hashkeys, $loadaccount, $loadaccountname)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $hashkeyarray = explode(",", $hashkeys);

    $itemcount = 0;

    foreach ($hashkeyarray as $index => $hashkey)
    {
        $db->query("SELECT * from PushNotifyMessageStatus WHERE messagehashkey='$hashkey' ");
        while($result = $db->fetch_array())
        {
            $record = [];
            $record['messageid'] = $result['messageid'];
            $record['topic'] = $result['topic'];
            $record['messagehashkey'] = $result['messagehashkey'];
            $record['readed'] = $result['readed'];
            $record['finished'] = $result['finished'];
            $record['updatetime'] = $result['updatetime'];
            $record['datetime'] = $result['datetime'];

            $records[$itemcount] = $record;
            $itemcount = $itemcount + 1;
        }
    }

    if ($loadaccount == 1)
    {
        $itemcount = 0;
        foreach ($records as $index => $record)
        {
            $record['account'] = getAccountIdFromTopic($db, $record['topic']);

            $records[$itemcount] = $record;
            $itemcount = $itemcount + 1;
        }
    }

    if ($loadaccountname == 1)
    {
        $itemcount = 0;
        foreach ($records as $index => $record)
        {
            $record['accountname'] = getAccountNameFromTopic($db, $record['topic']);

            $records[$itemcount] = $record;
            $itemcount = $itemcount + 1;
        }
    }

    return $records;
}

function serach_targetids($project_id, $userid, $searchtarget, $maingroups)
{
    $searchtargetidList = [];

    //==========================
    // $users = search_user($project_id, $searchtarget, 0, 0);
    //==========================
    $users = search_subgroup_user_ById($project_id, $userid, $searchtarget, 0, 0, $maingroups);
    //==========================

    $nodes = search_node($project_id, $searchtarget);

    $count = 0;
    foreach ($users as $index => $user)
    {
        $searchtargetidList[$count] = $user['userid'];
        $count++;
    }

    foreach ($nodes as $index => $node)
    {
        $searchtargetidList[$count] = $node['nodeid'];
        $count++;
    }

    return $searchtargetidList;
}


function search_node($project_id, $search_text)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM NodeInfo WHERE nodename LIKE '%{$search_text}%' OR nodeid LIKE '%{$search_text}%' ORDER BY macaddress ASC ");

    $datacnt = 0;

    while($result = $db->fetch_array())
    {
        $record = [];
        $record['nodeid'] = $result['nodeid'];
        $records[$datacnt] = $record;
        $datacnt++;
    }

    return $records;
}

function getTargetTopic($project_id, $topic)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM TargetTopic WHERE topic='$topic' ");
    // $db->query("SELECT * FROM TargetTopic WHERE topic='$topic' AND userid <> 0 ");

    $TargetTopic = [];
    while($result = $db->fetch_array())
    {
        $TargetTopic['target'] = $result['target'];
        $TargetTopic['topic'] = $result['topic'];
        $TargetTopic['name'] = $result['name'];
        $TargetTopic['accountid'] = $result['accountid'];
        $TargetTopic['maingroupid'] = $result['maingroupid'];
        $TargetTopic['userid'] = $result['userid'];
        if($TargetTopic['userid'] != '0')
            break;
    }
    return $TargetTopic;
}

function getUserIdFromTopic($project_id, $topic)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT userid FROM TargetTopic WHERE topic='$topic' ");

    $userid = '';
    while($result = $db->fetch_array())
    {
        if ( $result['userid'] != '0' )
            $userid = $result['userid'];
    }
    return $userid;
}

function getAccountIdFromTopic($db, $topic)
{
    $db->query("SELECT accountid FROM TargetTopic WHERE topic='$topic' ");

    $accountid = '';
    while($result = $db->fetch_array())
    {
        $accountid = $result['accountid'];
    }
    return $accountid;
}

function getAccountNameFromTopic($db, $topic)
{
    $db->query("SELECT name FROM TargetTopic WHERE topic='$topic' ");

    $accountname = '';
    while($result = $db->fetch_array())
    {
        $accountname = $result['name'];
    }
    return $accountname;
}

function create_billboard($project_id, $accountid, $userid, $sendpushnotify, $subgrouplist, $title, $content, $imageuid, $maingroups)
{
    $db = new DB($project_id, 'base');

    //$subgroupids = implode(",", $subgrouplist);

    $cmd = "INSERT INTO billboardrecord (accountid, userid, sendpushnotify, PushNotifyMessageRecorddbId, subgroupids, title, content, imageids, fileids, maingroups) VALUES ('$accountid', $userid, $sendpushnotify, 0, '$subgrouplist', '$title', '$content', '$imageuid', '', '$maingroups')";

    $ret = $db->query($cmd);
    //
    // echo 'cmd['.$cmd.']';
    // echo 'ret['.$ret.']';

}

function delete_billboard($project_id, $recoerdid)
{
    $db = new DB($project_id, 'base');

    $ret = $db->query("DELETE FROM billboardrecord WHERE id=$recoerdid");
}

function loadbillboardrecords($project_id, $userid, $groupid, $maingroups, $searchtitle, $searchdate, $count, $offset)
{
    $records = [];

    $user_view_subgroupids = [];
    if ( $groupid != '1' && $groupid != '2' )
    {
        $user_view_subgroupids = load_view_access_subgroupids_ById($project_id, $userid);
        //$user_view_subgroupids = load_view_access_subgroupids($project_id, $accountid);
    }

    $db = new DB($project_id, 'base');

    if (strlen($searchtitle) > 0 && strlen($searchdate) > 0)
    {
        $db->query("SELECT * FROM billboardrecord WHERE title LIKE '%{$searchtitle}%' AND datetime LIKE '%{$searchdate}%' ORDER BY datetime DESC LIMIT $offset,$count  ");
    }
    else
    if (strlen($searchtitle) > 0)
    {
        $db->query("SELECT * FROM billboardrecord WHERE title LIKE '%{$searchtitle}%' ORDER BY datetime DESC LIMIT $offset,$count  ");
    }
    else
    if (strlen($searchdate) > 0)
    {
        $db->query("SELECT * FROM billboardrecord WHERE datetime LIKE '%{$searchdate}%' ORDER BY datetime DESC LIMIT $offset,$count  ");
    }
    else
    {
        $db->query("SELECT * FROM billboardrecord ORDER BY datetime DESC LIMIT $offset,$count ");
    }

    $itemcount = 0;
    while($result = $db->fetch_array())
    {
        $billboard_subgroupids = explode(",", $result['subgroupids']);

        $valid = true;
        foreach ($user_view_subgroupids as $user_subgroupid)
        {
            $valid = false;
            foreach ($billboard_subgroupids as $billboard_subgroupid)
            {
                $billboard_subgroupid_int = (int)$billboard_subgroupid;

                if ( $user_subgroupid == $billboard_subgroupid_int )
                {
                    $valid = true;
                    break;
                }
            }
            if ($valid)
            {
                break;
            }
        }
        if (!$valid)
        {
            continue;
        }

        $record = [];
        $record['id'] = $result['id'];
        $record['sendpushnotify'] = $result['sendpushnotify'];
        $record['PushNotifyMessageRecorddbId'] = $result['PushNotifyMessageRecorddbId'];
        $record['userid'] = $result['userid'];
        $record['subgroupids'] = $result['subgroupids'];
        $record['title'] = $result['title'];
        $record['content'] = $result['content'];
        $record['imageids'] = $result['imageids'];
        $record['fileids'] = $result['fileids'];
        $record['maingroups'] = $result['maingroups'];
        $record['datetime'] = $result['datetime'];

        $records[$itemcount] = $record;
        $itemcount = $itemcount + 1;
    }

    return $records;
}

?>
