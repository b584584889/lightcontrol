<?php

session_start();

require_once("common.php");
require_once("User.php");
require_once("cardrollcall.php");

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

    if (isset($_GET['loadcurriculum']))
    {
        $curriculumid = '';

        if (isset($_GET['curriculumid']))
        {
            $curriculumid = $_GET['curriculumid'];
        }

        $session_userid = $_SESSION["userid"];

        if (strlen($curriculumid) == 0)
        {
            echo '{}';
        }
        else
        {
            $status = loadcurriculum($project_id, $session_userid, $curriculumid);
            echo json_encode($status);
        }
    }
    else
    if (isset($_GET['loadcurriculums']))
    {
        $count = 0;
        $offset = 0;
        $curriculumid = '';

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['offset']))
        {
            $offset = (int)$_GET['offset'];
        }
        if (isset($_GET['curriculumid']))
        {
            $curriculumid = $_GET['curriculumid'];
        }

        if(strlen($curriculumid) > 0)
        {
            $status = loadcurriculumById($project_id, $curriculumid);
            echo json_encode($status);
        }
        else
        {
            $session_userid = $_SESSION["userid"];
            $session_groupid = $_SESSION["groupid"];
            $session_maingroups = $_SESSION["maingroups"];
            $status = loadcurriculums($project_id, $session_userid, $session_groupid, $session_maingroups, $offset, $count);
            echo json_encode($status);
        }
    }
    else
    if (isset($_GET['loadusercurriculums']))
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

        $session_subgroups = $_SESSION["subgroups"];

        $status = loadusercurriculums($project_id, $session_subgroups, $offset, $count);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadsharedcurriculums']))
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

        $session_maingroups = $_SESSION["maingroups"];

        $status = loadMainGroupSharedCurriculums($project_id, $session_maingroups, $offset, $count);
        echo json_encode($status);
    }
    else
    // if (isset($_GET['loadsharedmycurriculums']))
    // {
    //     $session_userid = $_SESSION["userid"];
    //     $session_groupid = $_SESSION["groupid"];
    //     $session_maingroups = $_SESSION["maingroups"];
    //     $session_subgroups = $_SESSION["subgroups"];
    //
    //     if ( $session_groupid == 4 )
    //     {
    //         $status = loadSubGroupSharedmyCurriculums($project_id, $session_subgroups);
    //         echo json_encode($status);
    //     }
    //     else
    //     if ( $session_groupid == 5 )
    //     {
    //         $status = loadMainGroupSharedmyCurriculums($project_id, $session_userid, $session_maingroups);
    //         echo json_encode($status);
    //     }
    //     else
    //     {
    //         echo '[]';
    //     }
    // }
    // else
    if (isset($_GET['loadcurriculumrecord']))
    {
        $count = 0;
        $offset = 0;
        $recordid = '';

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['offset']))
        {
            $offset = (int)$_GET['offset'];
        }

        if (isset($_GET['recordid']))
        {
            $recordid = $_GET['recordid'];
        }

        $session_userid = $_SESSION["userid"];
        $session_groupid = $_SESSION["groupid"];
        $session_maingroups = $_SESSION["maingroups"];
        $session_subgroups = $_SESSION["subgroups"];

        $status = loadcurriculumrecord($project_id, $session_userid, $session_groupid, $session_maingroups, $session_subgroups, $recordid, $offset, $count);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadsharedcurriculumrecords']))
    {
        $count = 0;
        $offset = 0;
        $recordids = '';

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['offset']))
        {
            $offset = (int)$_GET['offset'];
        }

        if (isset($_GET['recordids']))
        {
            $recordids = $_GET['recordids'];
        }

        $status = loadsharedcurriculumrecord($project_id, $recordids, $offset, $count);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadcurriculumprefix']))
    {
        $session_maingroups = $_SESSION["maingroups"];
        $session_userid = $_SESSION["userid"];

        $status = loadcurriculumprefix($project_id, $session_userid, $session_maingroups);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadcurriculumtype']))
    {
        $type = '';

        if (isset($_GET['type']))
        {
            $type = $_GET['type'];
        }

        if(strlen($type) > 0)
        {
            $status = loadcurriculumtypeByType($project_id, $type);
            echo json_encode($status);
        }
        else
        {

            $session_userid = $_SESSION["userid"];
            $session_groupid = $_SESSION["groupid"];
            $session_maingroups = $_SESSION["maingroups"];

            $status = loadcurriculumtype($project_id, $session_userid, $session_groupid, $session_maingroups);
            echo json_encode($status);
        }
    }
    else
    if (isset($_GET['loadcurriculumapply']))
    {
        $records = '';
        if (isset($_GET['records']))
            $records = $_GET['records'];

        $session_userid = $_SESSION["userid"];

        $status = loadcurriculumapply($project_id, $session_userid, $records);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadcurriculumrecordapply']))
    {
        $recordid = '';
        if (isset($_GET['recordid']))
            $recordid = $_GET['recordid'];

        $status = loadcurriculumrecordapply($project_id, $recordid);
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

    if (isset($_POST['create_new_curriculum']))
    {
        $curriculum = [];
        $curriculum['project_id'] = $project_id;
        $curriculum['userid'] = $_SESSION["userid"];
        $curriculum['typeid'] = $_POST['typeid'];
        $curriculum['subgroups'] = $_POST['subgroups'];
        $curriculum['maingroups'] = $_SESSION["maingroups"];
        $curriculum['number'] = $_POST['number'];
        $curriculum['shared'] = $_POST["shared"];
        $curriculum['sharedmaingroups'] = $_POST["sharedmaingroups"];

        save_new_curriculum($project_id, $curriculum);

        $json_string = json_encode($curriculum);
        echo $json_string;
    }
    else
    if (isset($_POST['update_curriculum']))
    {
        $curriculum = [];
        $curriculum['project_id'] = $project_id;
        $curriculum['id'] = $_POST['id'];
        $curriculum['userid'] = $_SESSION["userid"];
        $curriculum['typeid'] = $_POST['typeid'];
        $curriculum['subgroups'] = $_POST['subgroups'];
        $curriculum['maingroups'] = $_SESSION["maingroups"];
        $curriculum['number'] = $_POST['number'];
        $curriculum['shared'] = $_POST["shared"];
        $curriculum['sharedmaingroups'] = $_POST["sharedmaingroups"];

        update_curriculum($project_id, $curriculum);

        $json_string = json_encode($curriculum);
        echo $json_string;
    }
    else
    if (isset($_POST['update_shared_curriculumrecord']))
    {
        $curriculum = [];
        $curriculum['project_id'] = $project_id;
        $curriculum['recordid'] = $_POST['recordid'];
        $curriculum['curriculumid'] = $_POST['curriculumid'];
        $curriculum['subgroups'] = $_POST['subgroups'];

        $curriculumrecord = loadcurriculumrecordByID($project_id, $curriculum['recordid']);
        $merge_subgroups = mergeIds($curriculumrecord['subgroups'], $curriculum['subgroups']);
        $curriculumrecord['subgroups'] = $merge_subgroups;

        update_curriculumrecord($project_id, $curriculumrecord);

        $curriculumExisted = loadSingleCurriculum($project_id, (int)$curriculum['curriculumid']);
        $merge_subgroups = mergeIds($curriculumrecord['subgroups'], $curriculumExisted['subgroups']);
        $curriculumExisted['subgroups'] = $merge_subgroups;

        update_curriculum($project_id, $curriculumExisted);

        $json_string = json_encode($curriculum);
        echo $json_string;
    }
    // else
    // if (isset($_POST['update_shared_curriculum']))
    // {
    //     $curriculum = [];
    //     $curriculum['project_id'] = $project_id;
    //     $curriculum['id'] = $_POST['id'];
    //     $curriculum['curriculumid'] = $_POST['curriculumid'];
    //     $curriculum['userid'] = $_SESSION["userid"];
    //     $curriculum['typeid'] = $_POST['typeid'];
    //     $curriculum['subgroups'] = $_POST['subgroups'];
    //     $curriculum['maingroups'] = $_SESSION["maingroups"];
    //     $curriculum['number'] = $_POST['number'];
    //
    //     update_curriculumshared($project_id, $curriculum);
    //
    //     $json_string = json_encode($curriculum);
    //     echo $json_string;
    // }
    else
    if (isset($_POST['delete_curriculum']))
    {
        $curriculum = [];
        $curriculum['project_id'] = $project_id;
        $curriculum['id'] = $_POST['id'];
        $curriculum['userid'] = $_SESSION["userid"];
        $curriculum['maingroups'] = $_SESSION["maingroups"];

        delete_curriculum($project_id, $curriculum);

        $json_string = json_encode($curriculum);
        echo $json_string;
    }
    else
    if (isset($_POST['create_curriculumtype']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['name'] = $_POST['name'];
        $record['maingroups'] = $_SESSION["maingroups"];

        $record['id'] = create_curriculumtype($project_id, $record);

        $json_string = json_encode($record);
        echo $json_string;
    }
    else
    if (isset($_POST['update_curriculumtype']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['id'] = $_POST['id'];
        $record['name'] = $_POST['name'];
        $record['maingroups'] = $_SESSION["maingroups"];

        update_curriculumtype($project_id, $record);

        $json_string = json_encode($record);
        echo $json_string;
    }
    else
    if (isset($_POST['delete_curriculumtype']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['id'] = $_POST['id'];
        $record['userid'] = $_SESSION["userid"];
        $record['maingroups'] = $_SESSION["maingroups"];

        delete_curriculumtype($project_id, $record);

        $json_string = json_encode($curriculum);
        echo $json_string;
    }
    else
    if (isset($_POST['update_curriculumprefix']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['prefix'] = $_POST['prefix'];
        $record['maingroups'] = $_SESSION["maingroups"];

        update_curriculumprefix($project_id, $record);

        $json_string = json_encode($record);
        echo $json_string;
    }
    else
    if (isset($_POST['update_curriculumrecord']))
    {
        $curriculumrecord = [];
        $curriculumrecord['project_id'] = $project_id;
        $curriculumrecord['id'] = $_POST['id'];
        $curriculumrecord['curriculumid'] = $_POST['curriculumid'];
        $curriculumrecord['shared'] = $_POST['shared'];
        $curriculumrecord['name'] = $_POST['name'];
        // $curriculumrecord['maingroups'] = $_POST["maingroups"];
        $curriculumrecord['subgroups'] = $_POST["subgroups"];
        $curriculumrecord['maingroups'] = $_SESSION["maingroups"];
        $curriculumrecord['number'] = $_POST['number'];
        $curriculumrecord['curriculumtypename'] = $_POST['curriculumtypename'];
        $curriculumrecord['opendate'] = $_POST['opendate'];
        $curriculumrecord['closedate'] = $_POST['closedate'];
        $curriculumrecord['startdate'] = $_POST['startdate'];
        $curriculumrecord['enddate'] = $_POST['enddate'];
        $curriculumrecord['starttime'] = $_POST['starttime'];
        $curriculumrecord['endtime'] = $_POST['endtime'];
        $curriculumrecord['signinstarttime'] = $_POST['signinstarttime'];
        $curriculumrecord['signinendtime'] = $_POST['signinendtime'];
        $curriculumrecord['weekday'] = $_POST['weekday'];
        $curriculumrecord['totalhours'] = $_POST['totalhours'];
        $curriculumrecord['location'] = $_POST['location'];
        $curriculumrecord['description'] = $_POST['description'];
        $curriculumrecord['teacher'] = $_POST['teacher'];
        $curriculumrecord['teacheruserid'] = $_POST['teacheruserid'];
        $curriculumrecord['maxnumber'] = $_POST['maxnumber'];
        $curriculumrecord['autoapproval'] = $_POST['autoapproval'];
        $curriculumrecord['cost'] = $_POST['cost'];
        $curriculumrecord['note'] = $_POST['note'];
        $curriculumrecord['files'] = $_POST['files'];
        $curriculumrecord['status'] = $_POST['status'];
        $curriculumrecord['userid'] = $_SESSION["userid"];
        $curriculumrecord['accountid'] = $_SESSION["accountid"];
        $curriculumrecord['usermaingroups'] = $_SESSION["maingroups"];

        //
        // 檢查是否 有 共享課程, 合併子集合權限
        //
        $curriculum = loadSingleCurriculum($project_id, (int)$curriculumrecord['curriculumid']);
        $merge_subgroups = mergeIds($curriculumrecord['subgroups'], $curriculum['subgroups']);

        $curriculumrecord['subgroups'] = $merge_subgroups;

        if ($curriculumrecord['id'] == 88888)
        {
            $curriculumrecord['id'] = save_new_curriculumrecord($project_id, $curriculumrecord);

            //
            // create new subgroup
            //
            $groupname = $_POST['curriculumtypename'] . "[" . $_POST['number'] ."]";
            $description = '選課課程 ' . $groupname . ' '. $_POST['description'];
            $record = create_new_usersubgroup($project_id, $groupname, $description, 1, $curriculumrecord['id'], $curriculumrecord['enddate']);

            // add_to_user_datagroups($project_id, $_SESSION["userid"], $record['subgroupid']);

            create_new_rfidrollcall_from_curriculumerecord($project_id, $curriculumrecord, $groupname, $record['subgroupid']);

            update_curriculum_recordid($project_id, $curriculumrecord['curriculumid'], $curriculumrecord['id']);
        }
        else
        {
            echo '[1]';
            update_curriculumrecord($project_id, $curriculumrecord);

            echo '[2]';
            update_curriculum_recordid($project_id, $curriculumrecord['curriculumid'], $curriculumrecord['id']);

            $groupname = $_POST['curriculumtypename'] . "[" . $_POST['number'] ."]";

            echo '[3]';
            // check subgroup
            $subgroup = load_subgroup_ByCurriculumRecordID($project_id, $curriculumrecord['id']);

            echo '[5]';

            if ($subgroup['subgroupid'] == null)
            {
                echo '[5-1]';
                $description = '選課課程 ' . $groupname . ' '. $_POST['description'];
                $subgroup = create_new_usersubgroup($project_id, $groupname, $description, 1, $curriculumrecord['id'], $curriculumrecord['enddate']);
            }
            else
            {
                echo '[5-2]';
                $subgroup['type'] = 1;
                $subgroup['curriculumrecordid'] = $curriculumrecord['id'];
                $subgroup['validdate'] = $curriculumrecord['enddate'];
                update_usersubgroup($project_id, $_SESSION["userid"], $subgroup);
            }
            echo '[6]';

            // add_to_user_datagroups($project_id, $_SESSION["userid"], $subgroup['subgroupid']);

            //check rollcall
            $rollcall = load_rollcall_ByCurriculumRecordID($project_id, $curriculumrecord['id']);
            if ($rollcall['rollcallid'] == null)
            {
                // echo 'create_new_rfidrollcall_from_curriculumerecord';
                create_new_rfidrollcall_from_curriculumerecord($project_id, $curriculumrecord, $groupname, $subgroup['subgroupid']);
            }
            else
            {
                $rollcall['StartTime'] = $curriculumrecord['signinstarttime'];
                $rollcall['EndTime'] = $curriculumrecord['signinendtime'];
                $rollcall['StartDate'] = $curriculumrecord['startdate'];
                $rollcall['EndDate'] = $curriculumrecord['enddate'];
                $rollcall['weekday'] = $curriculumrecord['weekday'];
                $rollcall['type'] = 1;
                $rollcall['curriculumrecordid'] = $curriculumrecord['id'];

                update_cardrollcall($project_id, $rollcall);
            }
        }

        $json_string = json_encode($curriculumrecord);
        $send_string = 'updatecurriculumrecord=' . $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['delete_curriculumrecord']))
    {
        $curriculumrecord = [];
        $curriculumrecord['project_id'] = $project_id;
        $curriculumrecord['id'] = $_POST['id'];
        $curriculumrecord['curriculumid'] = $_POST['curriculumid'];

        delete_curriculumrecord($project_id, $curriculumrecord['id']);

        update_curriculum_recordid($project_id, $curriculumrecord['curriculumid'], 0);

        $json_string = json_encode($curriculumrecord);
        $send_string = 'deletecurriculumrecord=' . $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['apply_curriculumrecord']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['recordid'] = $_POST['id'];
        $record['number'] = $_POST['number'];
        $record['curriculumname'] = $_POST['curriculumname'];
        $record['userid'] = $_SESSION["userid"];
        $record['maingroups'] = $_SESSION['maingroups'];

        $recordid = (int)$record['recordid'];

        // check autoapproval
        $curriculumrecord = loadcurriculumrecordByID($project_id, $recordid);

        $approval = false;
        if ( $curriculumrecord['autoapproval'] == 1)
        {
            // count apply user
            $applycount = getCurriculumApplyCount($project_id, $recordid);

            if ( $applycount <= (int)$curriculumrecord['maxnumber'] )
            {
                $approval = true;

                //=======================================
                // add user to subgroup
                add_curriculum_user_to_subgroup($project_id, $recordid, $record['userid']);
                //=======================================
            }
        }

        $record['id'] = apply_curriculumrecord($project_id, $record, $approval);
        $record['teacheruserid'] = $curriculumrecord["teacheruserid"];
        $record['number'] = $record["number"];
        $record['curriculumname'] = $record["curriculumname"];
        $record['approval'] = $approval;

        $json_string = json_encode($record);
        $send_string = 'applycurriculumrecord=' . $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['update_apply_status']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['userid'] = $_POST['userid'];
        $record['recordid'] = $_POST['recordid'];
        $record['status'] = $_POST['status'];
        $record['number'] = $_POST['number'];
        $record['curriculumname'] = $_POST['curriculumname'];
        $record['curriculumtypename'] = $_POST['curriculumtypename'];
        $record['maingroups'] = $_SESSION['maingroups'];

        update_curriculumApplyStatus($project_id, $record['userid'], $record['recordid'], $record['status']);

        $recordid = (int)$record['recordid'];

        if ( $record['status'] == '0' || $record['status'] == '99' )
        {
            //=======================================
            // remove user from subgroup
            remove_curriculum_user_from_subgroup($project_id, $recordid, $record['userid']);
            //=======================================
        }
        else
        if ( $record['status'] == '1' )
        {
            //=======================================
            // add user to subgroup
            add_curriculum_user_to_subgroup($project_id, $recordid, $record['userid']);
            //=======================================
        }


        $json_string = json_encode($record);
        $send_string = 'updatecurriculumapplystatus=' . $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
}

function REST_do_DELETE()
{
}

function loadcurriculum($project_id, $userid, $curriculumid)
{
    $record = [];

    $loadAll = check_user_permisson($project_id, $userid);

    if ($loadAll)
    {
        $record = loadSingleCurriculum($project_id, $curriculumid);
        return $record;
    }
    else
    {
        $record = loadPermissoncurriculum($project_id, $userid, $curriculumid);
        return $record;
    }

    return $record;
}

function loadcurriculumById($project_id, $curriculumid)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM Curriculum WHERE id IN($curriculumid) ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['id'] = $result['id'];
        $record['userid'] = $result['userid'];
        $record['typeid'] = $result['typeid'];
        $record['recordid'] = $result['recordid'];
        $record['subgroups'] = $result['subgroups'];
        $record['maingroups'] = $result['maingroups'];
        $record['number'] = $result['number'];
        $record['shared'] = $result['shared'];
        $record['sharedmaingroups'] = $result['sharedmaingroups'];
        $record['datetime'] = $result['datetime'];

        $records[$count] = $record;
        $count = $count + 1;
    }

    return $records;
}

function loadcurriculums($project_id, $userid, $groupid, $maingroups, $queryoffset, $querycount)
{
    $records = [];

    $loadAll = check_user_permisson($project_id, $userid);

    if ($loadAll)
    {
        $records = loadAllcurriculums($project_id, $queryoffset, $querycount);
        return $records;
    }
    else
    {
        // $records = loadUserOwnCurriculums($project_id, $userid, $queryoffset, $querycount);
        $records = loadPermissoncurriculums($project_id, $userid, $groupid, $maingroups, $queryoffset, $querycount);
        return $records;
    }
}

function loadSingleCurriculum($project_id, $curriculumid)
{
    $record = [];
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM Curriculum WHERE id=$curriculumid ");

    $count = 0;
    if($result = $db->fetch_array())
    {
        $record['id'] = $result['id'];
        $record['userid'] = $result['userid'];
        $record['typeid'] = $result['typeid'];
        $record['recordid'] = $result['recordid'];
        $record['subgroups'] = $result['subgroups'];
        $record['maingroups'] = $result['maingroups'];
        $record['number'] = $result['number'];
        $record['shared'] = $result['shared'];
        $record['sharedmaingroups'] = $result['sharedmaingroups'];
        $record['datetime'] = $result['datetime'];
    }

    return $record;
}

function loadAllcurriculums($project_id, $queryoffset, $querycount)
{
    $records = [];
    $db = new DB($project_id, 'base');

    if ( $querycount == 0 )
        $db->query("SELECT * FROM Curriculum ORDER BY datetime DESC ");
    else
        $db->query("SELECT * FROM Curriculum ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['id'] = $result['id'];
        $record['userid'] = $result['userid'];
        $record['typeid'] = $result['typeid'];
        $record['recordid'] = $result['recordid'];
        $record['subgroups'] = $result['subgroups'];
        $record['maingroups'] = $result['maingroups'];
        $record['number'] = $result['number'];
        $record['shared'] = $result['shared'];
        $record['sharedmaingroups'] = $result['sharedmaingroups'];
        $record['datetime'] = $result['datetime'];
        $records[$count] = $record;
        $count = $count + 1;
    }

    return $records;
}

function loadPermissoncurriculum($project_id, $userid, $curriculumid)
{
    $record = [];
    $db = new DB($project_id, 'base');

    $curUser_subgroups = getUserDataGroups_ById($project_id, $userid);
    $curUser_subgroupids = explode(",", $curUser_subgroups);

    $db->query("SELECT * FROM Curriculum WHERE id=$curriculumid");

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
            $record['userid'] = $result['userid'];
            $record['typeid'] = $result['typeid'];
            $record['recordid'] = $result['recordid'];
            $record['subgroups'] = $result['subgroups'];
            $record['maingroups'] = $result['maingroups'];
            $record['number'] = $result['number'];
            $record['shared'] = $result['shared'];
            $record['sharedmaingroups'] = $result['sharedmaingroups'];
            $record['datetime'] = $result['datetime'];
        }
    }
    return $record;
}

function loadusercurriculums($project_id, $subgroups, $queryoffset, $querycount)
{
    // $sharedrecords = loadUserSubGroupSharedCurriculums($project_id, $subgroups);

    $records1 = loadUserSubGroupCurriculums($project_id, $subgroups);

    // $records = array_merge($records1, $records2);

    return $records1;
}

function loadUserSubGroupCurriculums($project_id, $subgroups)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM Curriculum ");

    $recordcount = 0;
    while($result = $db->fetch_array())
    {
        $found = false;
        $curriculumsubgroups = $result['subgroups'];

        $found = check_match_ids($subgroups, $curriculumsubgroups);

        // if ($found == false)
        // {
        //     foreach ($sharedrecords as $sharedrecord)
        //     {
        //         $found = check_match_ids($subgroups, $sharedrecord['subgroups']);
        //         if ($found)
        //             break;
        //     }
        // }

        if ($found)
        {
            $record = [];
            $record['id'] = $result['id'];
            $record['userid'] = $result['userid'];
            $record['typeid'] = $result['typeid'];
            $record['recordid'] = $result['recordid'];
            $record['subgroups'] = $result['subgroups'];
            $record['maingroups'] = $result['maingroups'];
            $record['number'] = $result['number'];
            $record['datetime'] = $result['datetime'];
            $records[$recordcount] = $record;
            $recordcount = $recordcount + 1;
        }
    }

    return $records;
}

// function loadUserSubGroupSharedCurriculums($project_id, $subgroups)
// {
//     $records = [];
//     $db = new DB($project_id, 'base');
//
//     $db->query("SELECT * FROM CurriculumShared ");
//
//     $$recordcount = 0;
//     while($result = $db->fetch_array())
//     {
//         $found = false;
//         $curriculumsubgroups = $result['subgroups'];
//
//         $found = check_match_ids($subgroups, $curriculumsubgroups);
//
//         if ($found)
//         {
//             $record = [];
//             $record['id'] = $result['id'];
//             $record['curriculumid'] = $result['curriculumid'];
//             $record['userid'] = $result['userid'];
//             $record['typeid'] = $result['typeid'];
//             $record['subgroups'] = $result['subgroups'];
//             $record['maingroups'] = $result['maingroups'];
//             $record['number'] = $result['number'];
//             $record['datetime'] = $result['datetime'];
//             $records[$recordcount] = $record;
//             $recordcount = $recordcount + 1;
//         }
//     }
//
//     return $records;
// }

function loadMainGroupSharedCurriculums($project_id, $maingroups, $queryoffset, $querycount)
{
    $records = [];
    $db = new DB($project_id, 'base');

    // $curUser_maingroupids = explode(",", $maingroups);

    $db->query("SELECT * FROM Curriculum WHERE shared=1 AND recordid <> 0 ");

    $survey_cnt = 0;
    $recordcount = 0;
    while($result = $db->fetch_array())
    {
        $found = false;
        $curriculummaingroups = $result['maingroups'];
        $sharedmaingroups = $result['sharedmaingroups'];

        // exclude myslef maingroup
        $found = check_match_ids($maingroups, $curriculummaingroups);
        if ( $found )
        {
            continue;
        }

        if ($sharedmaingroups == '' || $sharedmaingroups == '99999')
        {
            // ok
            $found = true;
        }
        else
        {
            $found = check_match_ids($maingroups, $sharedmaingroups);
        }

        if ($found == true)
        {
            if ( (($survey_cnt >= $queryoffset) && ($recordcount < $querycount)) || ($querycount == 0) )
            {
                $record = [];
                $record['id'] = $result['id'];
                $record['userid'] = $result['userid'];
                $record['typeid'] = $result['typeid'];
                $record['recordid'] = $result['recordid'];
                $record['subgroups'] = $result['subgroups'];
                $record['maingroups'] = $result['maingroups'];
                $record['number'] = $result['number'];
                $record['shared'] = $result['shared'];
                $record['sharedmaingroups'] = $result['sharedmaingroups'];
                $record['datetime'] = $result['datetime'];
                $records[$recordcount] = $record;
                $recordcount = $recordcount + 1;
            }
            $survey_cnt = $survey_cnt + 1;
        }
    }

    return $records;
}

// function loadSubGroupSharedmyCurriculums($project_id, $subgroups)
// {
//     $records = [];
//     $db = new DB($project_id, 'base');
//
//     $db->query("SELECT * FROM CurriculumShared ");
//
//     $$recordcount = 0;
//     while($result = $db->fetch_array())
//     {
//         $found = check_match_ids($subgroups, $result['subgroups']);
//         if ( $found == false)
//         {
//             continue;
//         }
//
//         $record = [];
//         $record['id'] = $result['id'];
//         $record['curriculumid'] = $result['curriculumid'];
//         $record['userid'] = $result['userid'];
//         $record['typeid'] = $result['typeid'];
//         $record['subgroups'] = $result['subgroups'];
//         $record['maingroups'] = $result['maingroups'];
//         $record['number'] = $result['number'];
//         $record['datetime'] = $result['datetime'];
//         $records[$recordcount] = $record;
//         $recordcount = $recordcount + 1;
//     }
//
//     return $records;
// }

// function loadMainGroupSharedmyCurriculums($project_id, $userid, $maingroups)
// {
//     $records = [];
//     $db = new DB($project_id, 'base');
//
//     $db->query("SELECT * FROM CurriculumShared WHERE userid=$userid ");
//
//     $$recordcount = 0;
//     while($result = $db->fetch_array())
//     {
//         $record = [];
//         $record['id'] = $result['id'];
//         $record['curriculumid'] = $result['curriculumid'];
//         $record['userid'] = $result['userid'];
//         $record['typeid'] = $result['typeid'];
//         $record['subgroups'] = $result['subgroups'];
//         $record['maingroups'] = $result['maingroups'];
//         $record['number'] = $result['number'];
//         $record['datetime'] = $result['datetime'];
//         $records[$recordcount] = $record;
//         $recordcount = $recordcount + 1;
//     }
//
//     return $records;
// }

function check_match_ids($target_ids, $match_ids)
{
    $found = false;

    $targetid_List = explode(",", $target_ids);
    $matchid_List = explode(",", $match_ids);

    foreach ($targetid_List as $targetid)
    {
        if( $targetid == '99999')
        {
            $found = true;
            break;
        }

        $targetid_int = (int)$targetid;

        //
        // check user subgroup
        //
        foreach ($matchid_List as $matchid)
        {
            if( $targetid_int == (int)$matchid)
            {
                $found = true;
                break;
            }
        }
        if ($found == true)
            break;
    }

    return $found;
}

function mergeIds($firstgroups, $secgroups)
{
    $first_List = explode(",", $firstgroups);
    $sec_List = explode(",", $secgroups);

    $count = 0;
    $merge_subgroups = [];

    foreach ($first_List as $targetid)
    {
        $int_targetid = (int)$targetid;

        $found = false;
        foreach ($merge_subgroups as $subgroupid)
        {
            if ( $int_targetid == $subgroupid )
            {
                $found = true;
                break;
            }
        }
        if ( $found == false )
        {
            $merge_subgroups[$count] = $int_targetid;
            $count++;
        }
    }

    foreach ($sec_List as $targetid)
    {
        $int_targetid = (int)$targetid;

        $found = false;
        foreach ($merge_subgroups as $subgroupid)
        {
            if ( $int_targetid == $subgroupid )
            {
                $found = true;
                break;
            }
        }
        if ( $found == false )
        {
            $merge_subgroups[$count] = $int_targetid;
            $count++;
        }
    }
    return implode(",", $merge_subgroups);
}

function loadUserOwnCurriculums($project_id, $userid, $queryoffset, $querycount)
{
    $records = [];
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM Curriculum WHERE userid=$userid ORDER BY datetime DESC ");

    $survey_cnt = 0;
    $recordcount = 0;
    while($result = $db->fetch_array())
    {
        if ( (($survey_cnt >= $queryoffset) && ($recordcount < $querycount)) || ($querycount == 0) )
        {
            $record = [];
            $record['id'] = $result['id'];
            $record['userid'] = $result['userid'];
            $record['typeid'] = $result['typeid'];
            $record['recordid'] = $result['recordid'];
            $record['subgroups'] = $result['subgroups'];
            $record['maingroups'] = $result['maingroups'];
            $record['number'] = $result['number'];
            $record['shared'] = $result['shared'];
            $record['sharedmaingroups'] = $result['sharedmaingroups'];
            $record['datetime'] = $result['datetime'];
            $records[$recordcount] = $record;
            $recordcount = $recordcount + 1;
        }
        $survey_cnt = $survey_cnt + 1;
    }
    return $records;
}

function loadPermissoncurriculums($project_id, $userid, $groupid, $maingroups, $queryoffset, $querycount)
{
    $records = [];
    $db = new DB($project_id, 'base');

    $curUser_subgroups = getUserDataGroups_ById($project_id, $userid);

    // $curUser_subgroupids = explode(",", $curUser_subgroups);

    if ($groupid == 4)
    {
        // 工作人員
        $db->query("SELECT * FROM Curriculum WHERE maingroups IN($maingroups) AND userid=$userid ORDER BY datetime DESC ");
    }
    else
    {
        $db->query("SELECT * FROM Curriculum WHERE maingroups IN($maingroups) ORDER BY datetime DESC ");
    }

    $survey_cnt = 0;
    $recordcount = 0;
    while($result = $db->fetch_array())
    {
        $found = false;
        $subgroups = $result['subgroups'];

        $found = check_match_ids($curUser_subgroups, $subgroups);

        // if ($subgroups == '' || $subgroups == '99999')
        // {
        //     // ok
        //     $found = true;
        // }
        // else
        // {
        //     $subgroup_list = explode(",", $subgroups);
        //     foreach ($subgroup_list as $subgroup)
        //     {
        //         if( $subgroup == '99999')
        //         {
        //             $found = true;
        //             break;
        //         }
        //
        //         //
        //         // check user subgroup
        //         //
        //         foreach ($curUser_subgroupids as $curUser_subgroupid)
        //         {
        //             if( $subgroup == $curUser_subgroupid)
        //             {
        //                 $found = true;
        //                 break;
        //             }
        //         }
        //         if ($found == true)
        //             break;
        //     }
        // }

        if ($found == true)
        {
            // echo ' loadPermissonsurveys $survey_cnt:'.$survey_cnt.' $queryoffset:'.$queryoffset.' $recordcount:'.$recordcount.' $querycount:'.$querycount;
            if ( (($survey_cnt >= $queryoffset) && ($recordcount < $querycount)) || ($querycount == 0) )
            {
                $record = [];
                $record['id'] = $result['id'];
                $record['userid'] = $result['userid'];
                $record['typeid'] = $result['typeid'];
                $record['recordid'] = $result['recordid'];
                $record['subgroups'] = $result['subgroups'];
                $record['maingroups'] = $result['maingroups'];
                $record['number'] = $result['number'];
                $record['shared'] = $result['shared'];
                $record['sharedmaingroups'] = $result['sharedmaingroups'];
                $record['datetime'] = $result['datetime'];
                $records[$recordcount] = $record;
                $recordcount = $recordcount + 1;
            }

            $survey_cnt = $survey_cnt + 1;
        }
    }

    return $records;
}


function save_new_curriculum($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $userid = $record['userid'];
    $typeid = $record['typeid'];
    $subgroups = $record['subgroups'];
    $maingroups = $record['maingroups'];
    $number = $record['number'];
    $shared = $record['shared'];
    $sharedmaingroups = $record['sharedmaingroups'];

    $db->query("INSERT INTO Curriculum(userid, typeid, subgroups, maingroups, number, shared, sharedmaingroups) " .
                "VALUES ($userid, $typeid, '$subgroups', '$maingroups', '$number', $shared, '$sharedmaingroups')");

}

function update_curriculum($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $id = $record['id'];
    $userid = $record['userid'];
    $typeid = $record['typeid'];
    $subgroups = $record['subgroups'];
    $maingroups = $record['maingroups'];
    $number = $record['number'];
    $shared = $record['shared'];
    $sharedmaingroups = $record['sharedmaingroups'];

    $db->query("UPDATE Curriculum SET userid=$userid, typeid=$typeid, subgroups='$subgroups', maingroups='$maingroups', number='$number', shared=$shared, sharedmaingroups='$sharedmaingroups' WHERE id=$id ");

}

function update_curriculum_recordid($project_id, $curriculumid, $curriculumrecordid)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE Curriculum SET recordid=$curriculumrecordid WHERE id=$curriculumid ");
}

// function update_curriculumshared($project_id, $record)
// {
//     $db = new DB($project_id, 'base');
//
//     $id = $record['id'];
//     $curriculumid = $record['curriculumid'];
//     $userid = $record['userid'];
//     $typeid = $record['typeid'];
//     $subgroups = $record['subgroups'];
//     $maingroups = $record['maingroups'];
//     $number = $record['number'];
//
//     if ( strlen($id) == 0 )
//     {
//
//         $db->query("INSERT INTO CurriculumShared(curriculumid, userid, typeid, subgroups, maingroups, number) " .
//                     "VALUES ($curriculumid, $userid, $typeid, '$subgroups', '$maingroups', '$number')");
//
//     }
//     else
//     {
//         $db->query("UPDATE CurriculumShared SET curriculumid=$curriculumid, userid=$userid, typeid=$typeid, subgroups='$subgroups', " .
//                    "maingroups='$maingroups', number='$number' WHERE id=$id ");
//     }
//
// }

function delete_curriculum($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $id = $record['id'];

    $db->query("DELETE FROM Curriculum WHERE id=$id ");
}

function loadcurriculumrecord($project_id, $userid, $groupid, $maingroups, $subgroups, $recordid, $queryoffset, $querycount)
{
    $records = [];
    $db = new DB($project_id, 'base');

    if ( $groupid == 3 )
    {
        // 普通人員
        // show valid date
        $day = date('Y-m-d');
        $db->query("SELECT * FROM CurriculumRecord WHERE opendate <= '$day' AND '$day' <= closedate ORDER BY datetime DESC ");
    }
    else
    if ( $groupid == 4 || $groupid == 5 )
    {
        // 工作人員, 子集合管理者
        $db->query("SELECT * FROM CurriculumRecord WHERE userid=$userid ORDER BY datetime DESC ");
    }
    else
    if ( $groupid == 6 )
    {
        // 主集合管理者
        $db->query("SELECT * FROM CurriculumRecord ORDER BY datetime DESC ");
    }
    else
    {
        $db->query("SELECT * FROM CurriculumRecord ORDER BY datetime DESC ");
    }


    // $maingroupList = explode(",", $maingroups);
    // $subgroupList = explode(",", $subgroups);
    $loadAll = check_user_permisson($project_id, $userid);

    $count = 0;
    $datacount = 0;
    while($result = $db->fetch_array())
    {
        if ($loadAll == false && $groupid == 3)
        {
            // 普通人員
            $valid = check_match_ids($subgroups, $result['subgroups']);
        }
        else
        if ($loadAll == false && $groupid == 4)
        {
            // 工作人員
            $valid = check_match_ids($maingroups, $result['maingroups']);
            if ($valid == true)
            {
                $valid = check_match_ids($subgroups, $result['subgroups']);
            }
        }
        else
        if ($loadAll == false && ($groupid == 5 || $groupid == 6) )
        {
            // 子集合管理者, 主集合管理者
            $valid = check_match_ids($maingroups, $result['maingroups']);
        }
        else
        {
            $valid = $loadAll;
        }

        if ($valid == false)
        {
            continue;
        }

        if ( ($datacount >= $queryoffset && $count <= $querycount) || ($queryoffset == 0 && $querycount == 0))
        {

            $record = [];
            $record['id'] = $result['id'];
            $record['userid'] = $result['userid'];
            $record['curriculumid'] = $result['curriculumid'];
            $record['shared'] = $result['shared'];
            $record['name'] = $result['name'];
            $record['number'] = $result['number'];
            $record['maingroups'] = $result['maingroups'];
            $record['subgroups'] = $result['subgroups'];
            $record['opendate'] = $result['opendate'];
            $record['closedate'] = $result['closedate'];
            $record['startdate'] = $result['startdate'];
            $record['enddate'] = $result['enddate'];
            $record['starttime'] = $result['starttime'];
            $record['endtime'] = $result['endtime'];
            $record['signinstarttime'] = $result['signinstarttime'];
            $record['signinendtime'] = $result['signinendtime'];
            // $record['weekday'] = $result['weekday'];
            $record['totalhours'] = $result['totalhours'];
            $record['location'] = $result['location'];
            $record['description'] = $result['description'];
            $record['teacher'] = $result['teacher'];
            $record['teacheruserid'] = $result['teacheruserid'];
            $record['maxnumber'] = $result['maxnumber'];
            $record['autoapproval'] = $result['autoapproval'];
            $record['cost'] = $result['cost'];
            $record['note'] = $result['note'];
            $record['files'] = $result['files'];
            $record['status'] = $result['status'];
            $record['datetime'] = $result['datetime'];

            if ($result['weekday'] != null)
            {
                $record['weekday'] = explode(",", $result['weekday']);
            }

            $records[$count] = $record;
            $count = $count + 1;
        }

        $datacount++;
    }

    return $records;
}

function loadsharedcurriculumrecord($project_id, $recordids, $queryoffset, $querycount)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM CurriculumRecord WHERE id IN($recordids) AND shared=1");

    $count = 0;
    $datacount = 0;
    while($result = $db->fetch_array())
    {
        if ( $datacount >= $queryoffset && $count <= $querycount )
        {
            $record = [];
            $record['id'] = $result['id'];
            $record['userid'] = $result['userid'];
            $record['curriculumid'] = $result['curriculumid'];
            $record['shared'] = $result['shared'];
            $record['name'] = $result['name'];
            $record['number'] = $result['number'];
            $record['maingroups'] = $result['maingroups'];
            $record['subgroups'] = $result['subgroups'];
            $record['opendate'] = $result['opendate'];
            $record['closedate'] = $result['closedate'];
            $record['startdate'] = $result['startdate'];
            $record['enddate'] = $result['enddate'];
            $record['starttime'] = $result['starttime'];
            $record['endtime'] = $result['endtime'];
            $record['signinstarttime'] = $result['signinstarttime'];
            $record['signinendtime'] = $result['signinendtime'];
            // $record['weekday'] = $result['weekday'];
            $record['totalhours'] = $result['totalhours'];
            $record['location'] = $result['location'];
            $record['description'] = $result['description'];
            $record['teacher'] = $result['teacher'];
            $record['teacheruserid'] = $result['teacheruserid'];
            $record['maxnumber'] = $result['maxnumber'];
            $record['autoapproval'] = $result['autoapproval'];
            $record['cost'] = $result['cost'];
            $record['note'] = $result['note'];
            $record['files'] = $result['files'];
            $record['status'] = $result['status'];
            $record['datetime'] = $result['datetime'];

            if ($result['weekday'] != null)
            {
                $record['weekday'] = explode(",", $result['weekday']);
            }

            $records[$count] = $record;
            $count = $count + 1;
        }
        $datacount++;
    }

    return $records;
}

function loadcurriculumrecordByID($project_id, $recordid)
{
    $record = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM CurriculumRecord WHERE id=$recordid ");
    if($result = $db->fetch_array())
    {
        $record['id'] = $result['id'];
        $record['userid'] = $result['userid'];
        $record['curriculumid'] = $result['curriculumid'];
        $record['shared'] = $result['shared'];
        $record['name'] = $result['name'];
        $record['number'] = $result['number'];
        $record['maingroups'] = $result['maingroups'];
        $record['subgroups'] = $result['subgroups'];
        $record['opendate'] = $result['opendate'];
        $record['closedate'] = $result['closedate'];
        $record['startdate'] = $result['startdate'];
        $record['enddate'] = $result['enddate'];
        $record['starttime'] = $result['starttime'];
        $record['endtime'] = $result['endtime'];
        $record['signinstarttime'] = $result['signinstarttime'];
        $record['signinendtime'] = $result['signinendtime'];
        $record['totalhours'] = $result['totalhours'];
        $record['location'] = $result['location'];
        $record['description'] = $result['description'];
        $record['teacher'] = $result['teacher'];
        $record['teacheruserid'] = $result['teacheruserid'];
        $record['maxnumber'] = $result['maxnumber'];
        $record['autoapproval'] = $result['autoapproval'];
        $record['cost'] = $result['cost'];
        $record['note'] = $result['note'];
        $record['files'] = $result['files'];
        $record['status'] = $result['status'];
        $record['datetime'] = $result['datetime'];

        if ($result['weekday'] != null)
        {
            $record['weekday'] = explode(",", $result['weekday']);
        }
    }
    return $record;
}

function save_new_curriculumrecord($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $userid = $record['userid'];
    $curriculumid = $record['curriculumid'];
    $shared = $record['shared'];
    $name = $record['name'];
    $maingroups = $record['maingroups'];
    $subgroups = $record['subgroups'];
    $number = $record['number'];
    $opendate = $record['opendate'];
    $closedate = $record['closedate'];
    $startdate = $record['startdate'];
    $enddate = $record['enddate'];
    $starttime = $record['starttime'];
    $endtime = $record['endtime'];
    $signinstarttime = $record['signinstarttime'];
    $signinendtime = $record['signinendtime'];
    $weekday = $record['weekday'];
    $totalhours = $record['totalhours'];
    $location = $record['location'];
    $description = $record['description'];
    $teacher = $record['teacher'];
    $teacheruserid = $record['teacheruserid'];
    $maxnumber = $record['maxnumber'];
    $autoapproval = $record['autoapproval'];
    $cost = $record['cost'];
    $note = $record['note'];
    $files = $record['files'];
    $status = $record['status'];

    $weekdayString = '';
    if (is_array($weekday))
        $weekdayString = implode(",", $weekday);
    else
        $weekdayString = $weekday;

    $db->query("INSERT INTO CurriculumRecord(userid, curriculumid, shared, name, maingroups, subgroups, number, opendate, closedate, startdate, enddate, " .
                "starttime, endtime, signinstarttime, signinendtime, weekday, totalhours, location, description, teacher, teacheruserid, " .
                "maxnumber, autoapproval, cost, note, files, status) " .
                "VALUES ($userid, $curriculumid, $shared, '$name', '$maingroups', '$subgroups', '$number', '$opendate', '$closedate', '$startdate', '$enddate', " .
                "'$starttime','$endtime', '$signinstarttime', '$signinendtime', '$weekdayString', '$totalhours', '$location', '$description', '$teacher', $teacheruserid, " .
                "$maxnumber, $autoapproval, '$cost', '$note', '$files', $status)");

    $newid = $db->get_insert_id();
    return $newid;
}

function update_curriculumrecord($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $id = $record['id'];
    $userid = $record['userid'];
    $curriculumid = $record['curriculumid'];
    $shared = $record['shared'];
    $name = $record['name'];
    $maingroups = $record['maingroups'];
    $subgroups = $record['subgroups'];
    $number = $record['number'];
    $opendate = $record['opendate'];
    $closedate = $record['closedate'];
    $startdate = $record['startdate'];
    $enddate = $record['enddate'];
    $starttime = $record['starttime'];
    $endtime = $record['endtime'];
    $signinstarttime = $record['signinstarttime'];
    $signinendtime = $record['signinendtime'];
    $weekday = $record['weekday'];
    $totalhours = $record['totalhours'];
    $location = $record['location'];
    $description = $record['description'];
    $teacher = $record['teacher'];
    $teacheruserid = $record['teacheruserid'];
    $maxnumber = $record['maxnumber'];
    $autoapproval = $record['autoapproval'];
    $cost = $record['cost'];
    $note = $record['note'];
    $files = $record['files'];
    $status = $record['status'];

    $weekdayString = '';
    if (is_array($weekday))
        $weekdayString = implode(",", $weekday);
    else
        $weekdayString = $weekday;

    $db->query("UPDATE CurriculumRecord SET userid=$userid, curriculumid=$curriculumid, shared=$shared, name='$name', maingroups='$maingroups', subgroups='$subgroups', number='$number', " .
                "opendate='$opendate', closedate='$closedate', startdate='$startdate', enddate='$enddate', " .
                "starttime='$starttime', endtime='$endtime', signinstarttime='$signinstarttime', signinendtime='$signinendtime', weekday='$weekdayString', " .
                "totalhours='$totalhours', location='$location', description='$description', teacher='$teacher', teacheruserid=$teacheruserid, " .
                "maxnumber=$maxnumber, autoapproval=$autoapproval, cost='$cost', note='$note', files='$files', status=$status WHERE id=$id ");

}

function delete_curriculumrecord($project_id, $recordid)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM CurriculumRecord WHERE id=$recordid ");
}

function loadcurriculumprefix($project_id, $userid, $maingroups)
{
    $record = [];
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM CurriculumPrefix WHERE maingroupid in($maingroups) ");


    if($result = $db->fetch_array())
    {
        $record['maingroupid'] = $result['maingroupid'];
        $record['prefix'] = $result['prefix'];
        $record['userid'] = $result['userid'];
        $record['datetime'] = $result['datetime'];
    }

    if ( $record['prefix'] == null )
    {
        $maingroupList = explode(",", $maingroups);

        while($record['prefix'] == null)
        {
            //
            // find unique prefix
            //
            $prefix = generateRandomString();

            $db->query("SELECT * FROM CurriculumPrefix WHERE prefix='$prefix' ");
            if($db->fetch_array())
            {

            }
            else
            {
                $record['maingroupid'] = $maingroupList[0];
                $record['prefix'] = $prefix;
                $record['userid'] = $userid;
                $record['datetime'] = '';

                // create new one
                $db->query("INSERT INTO CurriculumPrefix(maingroupid, prefix, userid) VALUES($maingroupList[0], '$prefix', $userid) ");

                break;
            }
        }
    }

    return $record;
}

function generateRandomString($length = 2)
{
    $characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $charactersLength = strlen($characters);
    $randomString = '';
    for ($i = 0; $i < $length; $i++) {
        $randomString .= $characters[rand(0, $charactersLength - 1)];
    }
    return $randomString;
}

function loadcurriculumtypeByType($project_id, $type)
{
    $records = [];
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM CurriculumType WHERE id IN($type) ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['id'] = $result['id'];
        $record['name'] = $result['name'];
        $record['maingroups'] = $result['maingroups'];
        $record['datetime'] = $result['datetime'];

        $records[$count] = $record;
        $count = $count + 1;
    }

    return $records;
}

function loadcurriculumtype($project_id, $userid, $groupid, $maingroups)
{
    $records = [];
    $db = new DB($project_id, 'base');

    // $maingroupList = explode(",", $maingroups);
    // $loadAll = check_user_permisson($project_id, $userid);

    $db->query("SELECT * FROM CurriculumType ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        // if ($loadAll == false)
        // {
        //     $valid = check_valid_maingroup($maingroupList, $result['maingroups']);
        //     if ($valid == false)
        //     {
        //         continue;
        //     }
        // }

        $record = [];
        $record['id'] = $result['id'];
        $record['name'] = $result['name'];
        $record['maingroups'] = $result['maingroups'];
        $record['datetime'] = $result['datetime'];

        $records[$count] = $record;
        $count = $count + 1;
    }

    return $records;
}

function loadcurriculumapply($project_id, $userid, $recordids)
{
    $records = [];
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM CurriculumApply WHERE curriculumrecordid IN($recordids) AND userid=$userid ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['userid'] = $result['userid'];
        $record['curriculumrecordid'] = $result['curriculumrecordid'];
        $record['status'] = $result['status'];

        $records[$count] = $record;
        $count = $count + 1;
    }

    return $records;
}

function loadcurriculumrecordapply($project_id, $recordid)
{
    $records = [];
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM CurriculumApply WHERE curriculumrecordid=$recordid ");

    $count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['userid'] = $result['userid'];
        $record['curriculumrecordid'] = $result['curriculumrecordid'];
        $record['status'] = $result['status'];

        $records[$count] = $record;
        $count = $count + 1;
    }
    return $records;
}

function create_curriculumtype($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $name = $record['name'];
    $maingroups = $record['maingroups'];

    $db->query("INSERT INTO CurriculumType(name, maingroups) VALUES ('$name', '$maingroups') ");

    $newid = $db->get_insert_id();
    return $newid;
}

function update_curriculumtype($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $id = $record['id'];
    $name = $record['name'];
    $maingroups = $record['maingroups'];

    $db->query("UPDATE CurriculumType SET name='$name',maingroups='$maingroups' WHERE id=$id ");
}

function delete_curriculumtype($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $id = $record['id'];

    $db->query("DELETE FROM CurriculumType WHERE id=$id ");
}

function update_curriculumprefix($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $prefix = $record['prefix'];
    $maingroups = $record['maingroups'];

    $db->query("UPDATE CurriculumPrefix SET prefix='$prefix' WHERE maingroupid in($maingroups) ");
}

function apply_curriculumrecord($project_id, $record, $approval)
{
    $db = new DB($project_id, 'base');

    $userid = $record['userid'];
    $curriculumrecordid = $record['recordid'];
    $status = 0;
    if ($approval)
        $status = 1;

    $db->query("INSERT INTO CurriculumApply(userid, curriculumrecordid, status) VALUES ($userid, $curriculumrecordid, $status) ON DUPLICATE KEY UPDATE status=VALUES(status)");

    return $curriculumrecordid;
}

function getCurriculumApplyCount($project_id, $recordid)
{
    $count = 0;

    $db = new DB($project_id, 'base');

    $db->query("SELECT COUNT(*) as count FROM CurriculumApply WHERE curriculumrecordid=$recordid ");

    if($result = $db->fetch_array())
    {
        $count = $result['count'];
    }

    return $count;
}

function update_curriculumApplyStatus($project_id, $userid, $recordid, $status )
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE CurriculumApply SET status=$status WHERE userid=$userid AND curriculumrecordid=$recordid ");
}

function add_curriculum_user_to_subgroup($project_id, $recordid, $userid)
{
    $subgroup = load_subgroup_ByCurriculumRecordID($project_id, $recordid);
    if ($subgroup['subgroupid'] != null)
    {
        $subgroupid = (int)$subgroup['subgroupid'];
        $user = loadsingleuser_ById($project_id, $userid);
        if ($user['subgroups'] != null)
        {
            $found = false;
            $subgroupList = explode(",", $user['subgroups']);
            foreach ($subgroupList as $groupid)
            {
                if ( $subgroupid == (int)$groupid )
                {
                    $found = true;
                    break;
                }
            }
            if ($found == false)
            {
                array_push($subgroupList, $subgroupid);
                $subgroups = implode(",", $subgroupList);
                update_user_subgroups($project_id, $userid, $subgroups);
            }
        }
    }
}

function remove_curriculum_user_from_subgroup($project_id, $recordid, $userid)
{
    $subgroup = load_subgroup_ByCurriculumRecordID($project_id, $recordid);
    if ($subgroup['subgroupid'] != null)
    {
        $subgroupid = (int)$subgroup['subgroupid'];
        $user = loadsingleuser_ById($project_id, $userid);
        if ($user['subgroups'] != null)
        {
            $subgroupValid = [];
            $subgroupList = explode(",", $user['subgroups']);
            foreach ($subgroupList as $groupid)
            {
                if ( $subgroupid != (int)$groupid )
                {
                    array_push($subgroupValid, $groupid);
                }
            }
            $user_subgroups = implode(",", $subgroupValid);
            update_user_subgroups($project_id, $userid, $user_subgroups);
        }
    }
}

?>
