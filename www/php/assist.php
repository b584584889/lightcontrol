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

    if (isset($_GET['loadassist']))
    {
        $assistid = '';

        if (isset($_GET['assistid']))
        {
            $assistid = $_GET['assistid'];
        }

        $session_accountid = $_SESSION["accountid"];
        $session_userid = $_SESSION["userid"];

        if (strlen($assistid) == 0)
        {
            echo '{}';
        }
        else
        {
            $status = loadassist($project_id, $session_userid, $assistid);
            echo json_encode($status);
        }
    }
    else
    if (isset($_GET['loadassists']))
    {
        $count = 0;
        $offset = 0;

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['offset']))
        {
            $offset = (int)$_GET['offset'];
        }

        $session_accountid = $_SESSION["accountid"];
        $session_userid = $_SESSION["userid"];

        $status = loadassists($project_id, $session_userid, $offset, $count);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadassistevent']))
    {
        $assisteventid = '';

        if (isset($_GET['assisteventid']))
        {
            $assisteventid = $_GET['assisteventid'];
        }

        $session_accountid = $_SESSION["accountid"];
        $session_userid = $_SESSION["userid"];

        if (strlen($assisteventid) == 0)
        {
            echo '{}';
        }
        else
        {
            $status = loadassistevent($project_id, $session_userid, $assisteventid);
            echo json_encode($status);
        }
    }
    else
    if (isset($_GET['loadassistevents']))
    {
        $count = 0;
        $offset = 0;

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['offset']))
        {
            $offset = (int)$_GET['offset'];
        }

        $session_accountid = $_SESSION["accountid"];
        $session_userid = $_SESSION["userid"];

        $status = loadassistevents($project_id, $session_userid, $offset, $count);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadassisteventrecords']))
    {
        $count = 0;
        $assistid = '';
        $assisteventid = '';

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['assistid']))
        {
            $assistid = $_GET['assistid'];
        }
        if (isset($_GET['assisteventid']))
        {
            $assisteventid = $_GET['assisteventid'];
        }
        $status = loadassistrecords($project_id, $assistid, $assisteventid, $count);
        echo json_encode($status);
    }
    else
    if (isset($_GET['queryassistconfig']))
    {
        $status = queryassistconfig($project_id);
        echo json_encode($status);
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

    if (isset($_POST['create_new_assist']))
    {
        $assist = [];
        $assist['project_id'] = $project_id;
        $assist['title'] = $_POST['title'];
        $assist['description'] = $_POST['description'];
        $assist['subgroups'] = $_POST['subgroups'];
        $assist['assignusers'] = $_POST['assignusers'];
        $assist['userid'] = $_SESSION["userid"];
        $assist['maingroups'] = $_SESSION["maingroups"];
        // $assist['assignaccounts'] = $_POST['assignaccounts'];
        // $assist['accountid'] = $_SESSION["accountid"];

        save_new_assist($project_id, $assist);

        $json_string = json_encode($assist);
        echo $json_string;
    }
    else
    if (isset($_POST['update_assist']))
    {
        $assist = [];
        $assist['project_id'] = $project_id;
        $assist['id'] = $_POST['id'];
        $assist['title'] = $_POST['title'];
        $assist['description'] = $_POST['description'];
        $assist['subgroups'] = $_POST['subgroups'];
        $assist['assignusers'] = $_POST['assignusers'];
        $assist['userid'] = $_SESSION["userid"];
        $assist['maingroups'] = $_SESSION["maingroups"];
        // $assist['assignaccounts'] = $_POST['assignaccounts'];
        // $assist['accountid'] = $_SESSION["accountid"];

        update_assist($project_id, $assist);

        $json_string = json_encode($assist);
        echo $json_string;
    }
    else
    if (isset($_POST['delete_assist']))
    {
        $assist = [];
        $assist['project_id'] = $project_id;
        $assist['id'] = $_POST['id'];
        $assist['userid'] = $_SESSION["userid"];
        $assist['maingroups'] = $_SESSION["maingroups"];
        // $assist['accountid'] = $_SESSION["accountid"];

        delete_assist($project_id, $assist);

        $json_string = json_encode($assist);
        echo $json_string;
    }
    else
    if (isset($_POST['create_assistevent']))
    {
        $assistevent = [];
        $assistevent['project_id'] = $project_id;
        $assistevent['assistid'] = $_POST['id'];
        $assistevent['description'] = $_POST['description'];
        $assistevent['eventtime'] = $_POST['eventtime'];
        $assistevent['weekday'] = $_POST['weekday'];
        $assistevent['validdate'] = $_POST['validdate'];
        $assistevent['invaliddate'] = $_POST['invaliddate'];
        $assistevent['deadline'] = $_POST['deadline'];
        $assistevent['imageids'] = $_POST['imageids'];
        $assistevent['fileids'] = $_POST['fileids'];
        $assistevent['applyusername'] = $_POST['applyusername'];
        $assistevent['applyuserrelation'] = $_POST['applyuserrelation'];
        $assistevent['userid'] = $_SESSION["userid"];
        $assistevent['maingroups'] = $_SESSION["maingroups"];
        // $assistevent['accountid'] = $_SESSION["accountid"];

        $assistevent['eventid'] = save_new_assistevent($project_id, $assistevent);

        $json_string = json_encode($assistevent);
        $send_string = 'newassistevent=' . $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['update_assistconfig']))
    {
        $assist = [];
        $assist['project_id'] = $project_id;
        $assist['sendNotifyTime'] = $_POST['sendNotifyTime'];

        update_assistconfig($project_id, $assist['sendNotifyTime']);

        sendMessageToServer('updateassistconfig');

        $json_string = json_encode($assist);
        echo $json_string;
    }

}

function REST_do_DELETE()
{
}

function loadassist($project_id, $userid, $assistid)
{
    $record = [];

    $loadAll = check_user_permisson($project_id, $userid);

    if ($loadAll)
    {
        $record = loadSingleAssist($project_id, $assistid);
        return $record;
    }
    else
    {
        $record = loadPermissonassist($project_id, $userid, $assistid);
        return $record;
    }

    return $record;
}

function loadassists($project_id, $userid, $queryoffset, $querycount)
{
    $records = [];

    $loadAll = check_user_permisson($project_id, $userid);

    if ($loadAll)
    {
        $records = loadAllassists($project_id, $queryoffset, $querycount);
        return $records;
    }
    else
    {
        $records = loadPermissonassists($project_id, $userid, $queryoffset, $querycount);
        return $records;
    }
}

function loadSingleAssist($project_id, $assistid)
{
    $record = [];
    $db = new DB($project_id, 'base');

    $db->query("SELECT * from assist WHERE id=$assistid ORDER BY datetime DESC ");

    $count = 0;
    if($result = $db->fetch_array())
    {
        $record['id'] = $result['id'];
        $record['title'] = $result['title'];
        $record['description'] = $result['description'];
        $record['subgroups'] = $result['subgroups'];
        if ($record['subgroups'] == '')
            $record['subgroups'] = '99999';
        $record['assignusers'] = $result['assignusers'];
        $record['userid'] = $result['userid'];
        $record['maingroups'] = $result['maingroups'];
        // $record['assignaccounts'] = $result['assignaccounts'];
        // $record['accountid'] = $result['accountid'];
        $record['datetime'] = $result['datetime'];
    }

    return $record;
}

function loadAllassists($project_id, $queryoffset, $querycount)
{
    $records = [];
    $db = new DB($project_id, 'base');

    if ( $querycount == 0 )
        $db->query("SELECT * from assist ORDER BY datetime DESC ");
    else
        $db->query("SELECT * from assist ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['id'] = $result['id'];
        $record['title'] = $result['title'];
        $record['description'] = $result['description'];
        $record['subgroups'] = $result['subgroups'];
        if ($record['subgroups'] == '')
            $record['subgroups'] = '99999';
        $record['assignusers'] = $result['assignusers'];
        $record['userid'] = $result['userid'];
        $record['maingroups'] = $result['maingroups'];
        // $record['assignaccounts'] = $result['assignaccounts'];
        // $record['accountid'] = $result['accountid'];
        $record['datetime'] = $result['datetime'];
        $records[$count] = $record;
        $count = $count + 1;
    }

    return $records;
}

function loadPermissonassist($project_id, $userid, $assistid)
{
    $record = [];
    $db = new DB($project_id, 'base');

    $curUser_subgroups = getUserDataGroups_ById($project_id, $userid);
    $curUser_subgroupids = explode(",", $curUser_subgroups);

    $db->query("SELECT * from assist WHERE id=$assistid");

    if ($result = $db->fetch_array() )
    {
        $found = false;
        $subgroups = $result['subgroups'];
        if ($subgroups == '' || $subgroups == '99999')
        {
            // ok
            $found = true;
        }
        else
        {
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
            $record['id'] = $result['id'];
            $record['title'] = $result['title'];
            $record['description'] = $result['description'];
            $record['subgroups'] = $result['subgroups'];
            if ($record['subgroups'] == '')
                $record['subgroups'] = '99999';
            $record['assignusers'] = $result['assignusers'];
            $record['userid'] = $result['userid'];
            $record['maingroups'] = $result['maingroups'];
            // $record['assignaccounts'] = $result['assignaccounts'];
            // $record['accountid'] = $result['accountid'];
            $record['datetime'] = $result['datetime'];
        }
    }
    return $record;
}

function loadPermissonassists($project_id, $userid, $queryoffset, $querycount)
{
    $records = [];
    $db = new DB($project_id, 'base');

    $curUser_subgroups = getUserDataGroups_ById($project_id, $userid);
    $curUser_subgroupids = explode(",", $curUser_subgroups);

    $db->query("SELECT * from assist ORDER BY datetime DESC ");

    $survey_cnt = 0;
    $recordcount = 0;
    while($result = $db->fetch_array())
    {
        $found = false;
        $subgroups = $result['subgroups'];
        if ($subgroups == '' || $subgroups == '99999')
        {
            // ok
            $found = true;
        }
        else
        {
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
                $record['id'] = $result['id'];
                $record['title'] = $result['title'];
                $record['description'] = $result['description'];
                $record['subgroups'] = $result['subgroups'];
                if ($record['subgroups'] == '')
                    $record['subgroups'] = '99999';
                $record['assignusers'] = $result['assignusers'];
                $record['userid'] = $result['userid'];
                $record['maingroups'] = $result['maingroups'];
                // $record['assignaccounts'] = $result['assignaccounts'];
                // $record['accountid'] = $result['accountid'];
                $record['datetime'] = $result['datetime'];
                $records[$recordcount] = $record;
                $recordcount = $recordcount + 1;
            }

            $survey_cnt = $survey_cnt + 1;
        }
    }

    return $records;
}


function save_new_assist($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $title = $record['title'];
    $description = $record['description'];
    $subgroups = $record['subgroups'];
    $userid = $record['userid'];
    $assignusers = $record['assignusers'];
    $maingroups = $record['maingroups'];
    // $accountid = $record['accountid'];
    // $assignaccounts = $record['assignaccounts'];

    $db->query("INSERT INTO assist(title, description, subgroups, userid, assignusers, maingroups) " .
                "VALUES ('$title', '$description', '$subgroups', $userid, '$assignusers', '$maingroups')");

}

function update_assist($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $id = $record['id'];
    $title = $record['title'];
    $description = $record['description'];
    $subgroups = $record['subgroups'];
    $userid = $record['userid'];
    $assignusers = $record['assignusers'];
    $maingroups = $record['maingroups'];
    // $accountid = $record['accountid'];
    // $assignaccounts = $record['assignaccounts'];

    $db->query("UPDATE assist SET title='$title', description='$description', subgroups='$subgroups', userid='$userid', assignusers='$assignusers', maingroups='$maingroups' WHERE id=$id");
    // $db->query("UPDATE assist SET title='$title', description='$description', accountid='$accountid', subgroups='$subgroups', assignaccounts='$assignaccounts' WHERE id=$id");

}

function delete_assist($project_id, $record)
{
    $id = $record['id'];
    $accountid = $record['accountid'];

    $db = new DB($project_id, 'base');
    $db->query("DELETE FROM assist WHERE id=$id ");
}

function save_new_assistevent($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $assistid = $record['assistid'];
    $userid = $record['userid'];
    $description = $record['description'];
    $eventtime = $record['eventtime'];
    $weekday = $record['weekday'];
    $validdate = $record['validdate'];
    $invaliddate = $record['invaliddate'];
    $deadline = $record['deadline'];
    $imageids = $record['imageids'];
    $fileids = $record['fileids'];
    $applyusername = $record['applyusername'];
    $applyuserrelation = $record['applyuserrelation'];
    $maingroups = $record['maingroups'];

    $db->query("INSERT INTO assistevent(assistid, description, userid, eventtime, weekday, validdate, invaliddate, deadline, imageids, fileids, applyusername, applyuserrelation, signoffaccountid, maingroups) " .
                "VALUES ($assistid, '$description', $userid, '$eventtime', '$weekday', '$validdate', '$invaliddate', '$deadline', '$imageids', '$fileids', '$applyusername', '$applyuserrelation', '', 'maingroups')");

    $eventid = $db->get_insert_id();
    return $eventid;
}

function loadassistevent($project_id, $userid, $assisteventid)
{
    $record = [];
    $db = new DB($project_id, 'base');

    $db->query("SELECT * from assistevent WHERE id=$assisteventid ORDER BY datetime DESC ");

    if($result = $db->fetch_array())
    {
        $record['id'] = $result['id'];
        $record['assistid'] = $result['assistid'];
        $record['userid'] = $result['userid'];
        $record['description'] = $result['description'];
        // $record['accountid'] = $result['accountid'];
        $record['eventtime'] = $result['eventtime'];
        $record['weekday'] = $result['weekday'];
        $record['validdate'] = $result['validdate'];
        $record['invaliddate'] = $result['invaliddate'];
        $record['deadline'] = $result['deadline'];
        $record['imageids'] = $result['imageids'];
        $record['fileids'] = $result['fileids'];
        $record['applyusername'] = $result['applyusername'];
        $record['applyuserrelation'] = $result['applyuserrelation'];
        $record['signoffstatus'] = $result['signoffstatus'];
        $record['signoffuserid'] = $result['signoffuserid'];
        // $record['signoffaccountid'] = $result['signoffaccountid'];
        $record['maingroups'] = $result['maingroups'];
        $record['datetime'] = $result['datetime'];

        if ($record['signoffuserid'] == null)
            $record['signoffuserid'] = '';
    }
    return $record;
}

function loadassistevents($project_id, $userid, $queryoffset, $querycount)
{
    $loadAll = check_user_permisson($project_id, $userid);
    $records = [];
    $db = new DB($project_id, 'base');

    if ($loadAll)
    {
        if ( $querycount == 0 )
            $db->query("SELECT * from assistevent ORDER BY datetime DESC ");
        else
            $db->query("SELECT * from assistevent ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");
    }
    else
    {
        if ( $querycount == 0 )
            $db->query("SELECT * from assistevent WHERE userid=$userid ORDER BY datetime DESC ");
        else
            $db->query("SELECT * from assistevent WHERE userid=$userid ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");
    }

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['id'] = $result['id'];
        $record['assistid'] = $result['assistid'];
        $record['userid'] = $result['userid'];
        $record['description'] = $result['description'];
        $record['eventtime'] = $result['eventtime'];
        $record['weekday'] = $result['weekday'];
        $record['validdate'] = $result['validdate'];
        $record['invaliddate'] = $result['invaliddate'];
        $record['deadline'] = $result['deadline'];
        $record['imageids'] = $result['imageids'];
        $record['fileids'] = $result['fileids'];
        $record['applyusername'] = $result['applyusername'];
        $record['applyuserrelation'] = $result['applyuserrelation'];
        $record['signoffstatus'] = $result['signoffstatus'];
        $record['signoffuserid'] = $result['signoffuserid'];
        // $record['signoffaccountid'] = $result['signoffaccountid'];
        $record['maingroups'] = $result['maingroups'];
        $record['datetime'] = $result['datetime'];

        if ($record['signoffuserid'] == null)
            $record['signoffuserid'] = '';

        $records[$count] = $record;
        $count = $count + 1;
    }

    return $records;
}

function loadassistrecords($project_id, $assistid, $assisteventid, $querycount)
{
    $records = [];
    $db = new DB($project_id, 'base');

    if ( $querycount == 0 )
        $db->query("SELECT * from assisteventrecord WHERE assistid=$assistid AND assisteventid=$assisteventid ORDER BY datetime DESC ");
    else
        $db->query("SELECT * from assisteventrecord WHERE assistid=$assistid AND assisteventid=$assisteventid ORDER BY datetime DESC LIMIT $querycount ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['id'] = $result['id'];
        $record['assistid'] = $result['assistid'];
        $record['assisteventid'] = $result['assisteventid'];
        $record['PushNotifyMessageRecorddbId'] = $result['PushNotifyMessageRecorddbId'];
        $record['userid'] = $result['userid'];
        $record['messagehashkey'] = $result['messagehashkey'];
        $record['results'] = $result['results'];
        $record['eventday'] = $result['eventday'];
        $record['eventtime'] = $result['eventtime'];
        $record['maingroups'] = $result['maingroups'];
        $record['datetime'] = $result['datetime'];
        $record['updatetime'] = $result['updatetime'];

        // if ($record['userid'] == null)
        //     $record['userid'] = 0;

        $records[$count] = $record;
        $count = $count + 1;
    }

    return $records;
}

function queryassistconfig($project_id)
{
    $db = new DB($project_id, 'base');

    $record = [];
    $record['sendNotifyTime'] = 480;

    $db->query("SELECT sendNotifyTime from systemconfig WHERE CONFIG_ID=1 ");

    if($result = $db->fetch_array())
    {
        $record['sendNotifyTime'] = $result['sendNotifyTime'];
    }

    return $record;
}

function update_assistconfig($project_id, $sendNotifyTime)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE systemconfig SET sendNotifyTime=$sendNotifyTime  WHERE CONFIG_ID=1 ");
}

?>
