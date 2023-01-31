<?php



// function getResidentInfo($project_id)
// {
//     $residents = [];
//
//     // global $_DB;
//     // $db = new DB();
//     // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
//     $db = new DB($project_id, 'base');
//
//     $db->query("SELECT serno,name,roomNo,residentType FROM resident");
//     while($result = $db->fetch_array())
//     {
//         if ( strcmp("1", $result['residentType']) == 0 )
//         {
//
//             $resident = [];
//             $resident['serno'] = $result['serno'];
//             $resident['name'] = $result['name'];
//             $resident['roomNo'] = $result['roomNo'];
//
//             $residents[$result['serno']] = $resident;
//         }
//     }
//     //$db->close();
//
//     return $residents;
// }

// function getEmployeeInfo($project_id)
// {
//     $employees = [];
//
//     // global $_DB;
//     // $db = new DB();
//     // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
//     $db = new DB($project_id, 'base');
//
//     $db->query("SELECT empid,cname,orgName,jobTitle,gender FROM employee");
//     while($result = $db->fetch_array())
//     {
//         $employee = [];
//         $employee['empid'] = $result['empid'];
//         $employee['cname'] = $result['cname'];
//         $employee['orgName'] = $result['orgName'];
//         $employee['jobTitle'] = $result['jobTitle'];
//         $employee['gender'] = $result['gender'];
//
//         $employees[$result['empid']] = $employee;
//     }
//     //$db->close();
//
//     return $employees;
// }

function getUserInfoByAccount($project_id, $account)
{
    $db = new DB($project_id, 'base');
    $db->query("SELECT * FROM User WHERE account='$account' ");

    $UserInfo = [];

    if($result = $db->fetch_array())
    {
        $UserInfo = fetch_user_result($result);
    }

    return $UserInfo;
}

function getUserInfo($project_id, $start, $count)
{
    $Users = [];

    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    if ( $count != 0)
        $db->query("SELECT * FROM User LIMIT $start,$count ");
    else
        $db->query("SELECT * FROM User ");

    while($result = $db->fetch_array())
    {
        $User = fetch_user_result($result);

        $Users[$result['account']] = $User;
    }
    //$db->close();

    return $Users;
}

// function usergroupcount($project_id)
// {
//     $db = new DB($project_id, 'base');
//
//     $db->query("SELECT count(*) as count FROM UserGroup ");
//
//     $count = 0;
//     if ($result = $db->fetch_array())
//     {
//         $count = (int)$result['count'];
//     }
//
//     return $count;
// }

function loadusergroup($project_id, $start, $count)
{
    $records = [];

    $db = new DB($project_id, 'base');

    if ( $count == 0 )
        $db->query("SELECT * FROM UserGroup ORDER BY groupid ASC ");
    else
        $db->query("SELECT * FROM UserGroup ORDER BY groupid ASC LIMIT $start,$count ");

    $datacnt = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['groupid'] = $result['groupid'];
        $record['groupname'] = $result['groupname'];
        $record['permission'] = $result['permission'];
        $record['datetime'] = $result['datetime'];

        $records[$datacnt] = $record;
        $datacnt++;
    }

    return $records;
}

function loadusersubgroup($project_id, $start, $count, $type)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $filter = '';
    if (strlen($type) > 0)
    {
        $filter = "WHERE type=$type";
    }


    if ( $count == 0 )
        $db->query("SELECT * FROM UserSubGroup $filter ORDER BY subgroupid DESC ");
    else
        $db->query("SELECT * FROM UserSubGroup $filter ORDER BY subgroupid DESC LIMIT $start,$count ");
    // if ( $count == 0 )
    //     $db->query("SELECT * FROM UserSubGroup WHERE type=0 ORDER BY subgroupid DESC ");
    // else
    //     $db->query("SELECT * FROM UserSubGroup WHERE type=0 ORDER BY subgroupid DESC LIMIT $start,$count ");

    $datacnt = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['subgroupid'] = $result['subgroupid'];
        $record['subgroupname'] = $result['subgroupname'];
        $record['permission'] = $result['permission'];
        $record['imageuid'] = $result['imageuid'];
        $record['accountids'] = $result['accountids'];
        $record['userids'] = $result['userids'];
        $record['datetime'] = $result['datetime'];
        $record['maingroupid'] = $result['maingroupid'];
        $record['type'] = $result['type'];
        $record['validdate'] = $result['validdate'];
        $record['curriculumrecordid'] = $result['curriculumrecordid'];

        if ( $record['accountids'] == null )
            $record['accountids'] = '';

        if ( $record['userids'] == null )
            $record['userids'] = '';

        if ( $record['imageuid'] == null )
            $record['imageuid'] = '';

        if ( $record['validdate'] == null )
            $record['validdate'] = '';

        $records[$datacnt] = $record;
        $datacnt++;
    }

    return $records;
}

function loadsubgroupbyid($project_id, $subgroupid)
{
    $db = new DB($project_id, 'base');

    $record = [];

    $db->query("SELECT * FROM UserSubGroup WHERE subgroupid=$subgroupid ");

    if($result = $db->fetch_array())
    {
        $record['subgroupid'] = $result['subgroupid'];
        $record['subgroupname'] = $result['subgroupname'];
        $record['permission'] = $result['permission'];
        $record['imageuid'] = $result['imageuid'];
        $record['accountids'] = $result['accountids'];
        $record['userids'] = $result['userids'];
        $record['datetime'] = $result['datetime'];
        $record['maingroupid'] = $result['maingroupid'];
        $record['type'] = $result['type'];
        $record['validdate'] = $result['validdate'];
        $record['curriculumrecordid'] = $result['curriculumrecordid'];

        if ( $record['accountids'] == null )
            $record['accountids'] = '';

        if ( $record['userids'] == null )
            $record['userids'] = '';

        if ( $record['imageuid'] == null )
            $record['imageuid'] = '';

        if ( $record['validdate'] == null )
            $record['validdate'] = '';
    }

    return $record;
}

function loadSubGroupIdByMainGroupId($project_id, $maingroupid)
{
    $db = new DB($project_id, 'base');

    $subgroupid = '';

    $db->query("SELECT subgroupid FROM UserSubGroup WHERE maingroupid=$maingroupid AND type=0 ORDER BY datetime DESC limit 1 ");

    if($result = $db->fetch_array())
    {
        $subgroupid = $result['subgroupid'];
    }
    return $subgroupid;
}

function check_account_existed($project_id, $account)
{
    $db = new DB($project_id, 'base');
    $db->query("SELECT * FROM User WHERE account='$account' ");
    $count = 0;
    $userid = '';
    while($result = $db->fetch_array())
    {
        $userid = $result['userid'];
        $count++;
    }
    return array($count, $userid);
}

function check_maingroup_account_existed($project_id, $maingroupid, $account, $userid)
{
    $db = new DB($project_id, 'base');

    $int_user = (int)$userid;

    if ( strlen($userid) == 0 || $userid == '0' )
        $db->query("SELECT * FROM User WHERE account='$account' ");
    else
        $db->query("SELECT * FROM User WHERE account='$account' AND userid=$int_user ");

    $count = 0;
    $userid = '';
    while($result = $db->fetch_array())
    {
        $maingroups = $result['maingroups'];

        $maingroupList = explode(",", $maingroups);
        foreach ($maingroupList as $maingroup)
        {
            if ($maingroup == $maingroupid)
            {
                $userid = $result['userid'];
                $count++;
            }
        }
    }
    return array($count, $userid);
}

function save_new_user($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $group = $record['group'];
    $account = $record['account'];
    $name = $record['name'];
    $title = $record['title'];
    $maingroups = $record['maingroups'];
    $datagroups = $record['datagroups'];
    $subgroups = $record['subgroups'];
    $rfidcards = $record['rfidcards'];
    $nfccards = $record['nfccards'];
    $uwbcards = $record['uwbcards'];
    $sensors = $record['sensors'];
    $imageuid = $record['imageuid'];

    $rfidcards = str_replace("\n","",$rfidcards);
    $nfccards = str_replace("\n","",$nfccards);
    $uwbcards = str_replace("\n","",$uwbcards);

    $rfidcards = str_replace("\t","",$rfidcards);
    $nfccards = str_replace("\t","",$nfccards);
    $uwbcards = str_replace("\t","",$uwbcards);
    $sensors = str_replace("\t","",$sensors);

    list( $user_rfidcards, $assigned_rfidcards) = check_rfidcards_had_assigned($project_id, $rfidcards, -1);
    list( $user_nfccards, $assigned_nfccards) = check_nfccards_had_assigned($project_id, $nfccards, -1);

    $db->query("INSERT INTO User(groupid, account, name, title, maingroups, datagroups, subgroups, rfidcards, nfccards, uwbcards, sensors, imageuid) " .
                "VALUES ($group, '$account', '$name', '$title', '$maingroups', '$datagroups', '$subgroups', '$user_rfidcards', '$user_nfccards', '$uwbcards', '$sensors', '$imageuid')");

    $newuserid = $db->get_insert_id();

    return array( $newuserid, $user_rfidcards, $assigned_rfidcards, $user_nfccards, $assigned_nfccards);
}

function update_user($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $userid = $record['userid'];
    $group = $record['group'];
    //$account = $record['account'];
    $name = $record['name'];
    $title = $record['title'];
    //$maingroups = $record['maingroups'];
    $datagroups = $record['datagroups'];
    $subgroups = $record['subgroups'];
    $rfidcards = $record['rfidcards'];
    $nfccards = $record['nfccards'];
    $uwbcards = $record['uwbcards'];
    $sensors = $record['sensors'];
    $imageuid = $record['imageuid'];

    $rfidcards = str_replace("\n","",$rfidcards);
    $nfccards = str_replace("\n","",$nfccards);
    $uwbcards = str_replace("\n","",$uwbcards);

    $rfidcards = str_replace("\t","",$rfidcards);
    $nfccards = str_replace("\t","",$nfccards);
    $uwbcards = str_replace("\t","",$uwbcards);
    $sensors = str_replace("\t","",$sensors);

    // echo 'rfidcards:'. $rfidcards.'<br>';
    // echo 'nfccards:'. $nfccards.'<br>';

    list( $user_rfidcards, $assigned_rfidcards) = check_rfidcards_had_assigned($project_id, $rfidcards, (int)$userid);
    // echo 'user_rfidcards:'. $user_rfidcards.' assigned_rfidcards:'.$assigned_rfidcards.'<br>';

    list( $user_nfccards, $assigned_nfccards) = check_nfccards_had_assigned($project_id, $nfccards, (int)$userid);
    // echo 'user_nfccards:'. $user_nfccards.' assigned_nfccards:'.$assigned_nfccards.'<br>';

    $db->query("UPDATE User SET groupid=$group, name='$name', title='$title', datagroups='$datagroups', subgroups='$subgroups', rfidcards='$user_rfidcards', nfccards='$user_nfccards', uwbcards='$uwbcards', sensors='$sensors', imageuid='$imageuid'  WHERE userid=$userid ");

    return array( $user_rfidcards, $assigned_rfidcards, $user_nfccards, $assigned_nfccards);
}

function update_user_type($project_id, $account, $userid, $type)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE User SET type=$type WHERE account='$account' AND userid=$userid ");
}

function update_user_imageuid($project_id, $userid, $imageuid)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE User SET imageuid='$imageuid' WHERE userid=$userid ");
}

function update_user_subgroups($project_id, $userid, $subgroups)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE User SET subgroups='$subgroups' WHERE userid=$userid ");
}

// function add_to_user_datagroups($project_id, $userid, $subgroupid )
// {
//     $db = new DB($project_id, 'base');
//
//     $db->query("SELECT datagroups FROM User WHERE userid=$userid ");
//
//     if($result = $db->fetch_array())
//     {
//         $datagroups = $result['datagroups'];
//         $datagroupList = explode(",", $datagroups);
//
//         $found = false;
//         foreach($datagroupList as $datagroup)
//         {
//             if ( (int)$datagroup == (int)$subgroupid )
//             {
//                 $found = true;
//                 break;
//             }
//         }
//
//         if ($found == false)
//         {
//             array_push($datagroupList, (int)$subgroupid);
//             $datagroupsstr = implode(",", $datagroupList);
//             $db->query("UPDATE User SET datagroups='$datagroupsstr' WHERE userid=$userid ");
//         }
//     }
//
// }

function check_rfidcards_had_assigned($project_id, $rfidcards, $userid)
{
    $user_rfidcards = [];
    $assigned_rfidcards = [];

    $rfidcardList = explode(",", $rfidcards);

    // echo 'check_rfidcards_had_assigned() $rfidcards:'. $rfidcards.'<br>';

    foreach($rfidcardList as $rfidcard)
    {
        // echo 'check_rfidcards_had_assigned() foreach $rfidcard:'. $rfidcard.'<br>';

        if (strlen($rfidcard) == 0 )
        {
            // echo 'check_rfidcards_had_assigned() foreach str($rfidcard) == 0 <br>';

            continue;
        }

        $assigned = check_rfidcard_had_assigned($project_id, $rfidcard, $userid);

        if ($assigned == false)
        {
            array_push($user_rfidcards, $rfidcard);
        }
        else
        {
            array_push($assigned_rfidcards, $rfidcard);
        }
    }
    // echo 'check_rfidcards_had_assigned() $user_rfidcards:'. $user_rfidcards.'<br>';
    // echo 'check_rfidcards_had_assigned() $assigned_rfidcards:'. $assigned_rfidcards.'<br>';

    $ret_rfidcards = implode(",", $user_rfidcards);
    $ret_assigned_rfidcards = implode(",", $assigned_rfidcards);

    // echo 'check_rfidcards_had_assigned() $ret_rfidcards:'. $ret_rfidcards.'<br>';
    // echo 'check_rfidcards_had_assigned() $ret_assigned_rfidcards:'. $ret_assigned_rfidcards.'<br>';

    return array( $ret_rfidcards, $ret_assigned_rfidcards);
}

function check_rfidcard_had_assigned($project_id, $rfidcard, $userid)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT count(*) as count FROM RFIDCard WHERE cardid='$rfidcard' AND userid > 0 AND userid != $userid ");

    $count = 0;
    if($result = $db->fetch_array())
    {
        $count = $result['count'];
    }
    if ( $count > 0 )
        return true;
    else
        return false;
}

function check_nfccards_had_assigned($project_id, $nfccards, $userid)
{
    $user_nfccards = [];
    $assigned_nfccards = [];

    $nfccardList = explode(",", $nfccards);

    foreach($nfccardList as $nfccard)
    {
        if (strlen($nfccard) == 0 )
        {
            continue;
        }

        // echo 'check_nfccards_had_assigned() foreach $nfccard:'. $nfccard.'<br>';

        $assigned = check_nfccard_had_assigned($project_id, $nfccard, $userid);
        if ($assigned == false)
        {
            array_push($user_nfccards, $nfccard);
        }
        else
        {
            array_push($assigned_nfccards, $nfccard);
        }
    }
    $ret_nfccards = implode(",", $user_nfccards);
    $ret_assigned_nfccards = implode(",", $assigned_nfccards);

    // echo 'check_nfccards_had_assigned() $ret_nfccards:'. $ret_nfccards.'<br>';
    // echo 'check_nfccards_had_assigned() $ret_assigned_nfccards:'. $ret_assigned_nfccards.'<br>';

    return array( $ret_nfccards, $ret_assigned_nfccards);
}

function check_nfccard_had_assigned($project_id, $cardid, $userid)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT count(*) as count FROM NFCCard WHERE cardid='$cardid' AND userid > 0 AND userid != $userid ");

    $count = 0;
    if($result = $db->fetch_array())
    {
        $count = $result['count'];
    }
    if ( $count > 0 )
        return true;
    else
        return false;
}

function update_userDataGroups($project_id, $userid, $datagroups, $groupid)
{
    $db = new DB($project_id, 'base');

    $updateGroupid = '';
    if (strlen($groupid) > 0 )
    {
        $updateGroupid = ', groupid=' . $groupid;
    }

    $db->query("UPDATE User SET datagroups='$datagroups' $updateGroupid  WHERE userid=$userid ");
}

function delete_user($project_id, $userid)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM User WHERE userid=$userid ");
}

function save_new_usergroup($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $groupname = $record['groupname'];
    $permission = $record['permission'];

    $db->query("INSERT INTO UserGroup(groupname, permission) VALUES ('$groupname', '$permission')");

    $newusergroupid  = $db->get_insert_id();
    return $newusergroupid ;
}

function update_usergroup($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $groupid = $record['groupid'];
    $groupname = $record['groupname'];
    $permission = $record['permission'];

    $db->query("UPDATE UserGroup SET groupname='$groupname', permission='$permission' WHERE groupid=$groupid ");
}

function delete_usergroup($project_id, $groupid)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM UserGroup WHERE groupid=$groupid ");
}

function create_new_usersubgroup($project_id, $groupname, $description, $type, $curriculumrecordid, $validdate)
{
    $record = [];
    $record['project_id'] = $project_id;
    $record['groupname'] = $groupname;
    $record['permission'] = $description;
    $record['accountids'] =$_SESSION["accountid"];
    $record['userids'] = $_SESSION["userid"];
    $record['type'] = $type;
    $record['curriculumrecordid'] = $curriculumrecordid;
    $record['validdate'] = $validdate;
    $record['imageuid'] = '';

    $maingroups = explode(",", $_SESSION['maingroups']);
    if (count($maingroups) > 0)
        $record['maingroupid'] = $maingroups[0];
    else
        $record['maingroupid'] = '1';

    $session_userid = $_SESSION["userid"];

    $record['subgroupid'] = save_new_usersubgroup($project_id, $session_userid, $record);

    return $record;
}

function save_new_usersubgroup($project_id, $userid, $record)
{
    $db = new DB($project_id, 'base');

    $groupname = $record['groupname'];
    $permission = $record['permission'];
    $accountids = $record['accountids'];
    $userids = $record['userids'];
    $maingroupid = $record['maingroupid'];
    $imageuid = $record['imageuid'];
    $type = $record['type'];
    $curriculumrecordid = $record['curriculumrecordid'];
    $validdate = $record['validdate'];

    if ($type == null)
        $type = 0;
    if ($curriculumrecordid == null)
        $curriculumrecordid = 0;
    if ($validdate == null)
        $validdate = '9999-12-31';

    $db->query("INSERT INTO UserSubGroup(subgroupname, permission, accountids, userids, maingroupid, imageuid, type, curriculumrecordid, validdate) " .
               "VALUES ('$groupname', '$permission', '$accountids', '$userids', $maingroupid, '$imageuid', $type, $curriculumrecordid, '$validdate')");

    $newusergroupid  = $db->get_insert_id();

    //
    // add new subgroup to user datagroup
    //
    check_userDataGroups($project_id, $userid, $userids, (string)$newusergroupid);

    //
    // add new subgroup to account datagroup
    //
    // $foundSameAccount = false;
    // $new_accountids = explode(",", $accountids);
    // foreach( $new_accountids as $curaccountid )
    // {
    //     $datagroups = getUserDataGroups($project_id, $curaccountid);
    //     $datagroupids = explode(",", $datagroups);
    //
    //     array_push($datagroupids, (string)$newusergroupid);
    //
    //     $datagroups = implode(",", $datagroupids);
    //
    //     update_userDataGroups($project_id, $curaccountid, $datagroups);
    //
    //     if ($curaccountid == $accountid)
    //     {
    //         $foundSameAccount = true;
    //     }
    // }
    //
    // if ($foundSameAccount == false)
    // {
    //     $datagroups = getUserDataGroups($project_id, $accountid);
    //     $datagroupids = explode(",", $datagroups);
    //
    //     array_push($datagroupids, (string)$newusergroupid);
    //
    //     $datagroups = implode(",", $datagroupids);
    //
    //     update_userDataGroups($project_id, $accountid, $datagroups);
    // }

    return $newusergroupid ;
}

function check_userDataGroups($project_id, $userid, $userids, $newusergroupid)
{
    $foundSameUser = false;
    $new_userids = explode(",", $userids);
    foreach( $new_userids as $curuserid )
    {
        $int_userid = (int)$curuserid;
        $datagroups = getUserDataGroups_ById($project_id, $int_userid);
        $datagroupids = explode(",", $datagroups);

        $found = false;
        foreach( $datagroupids as $existed_groupid )
        {
            if ( $existed_groupid == $newusergroupid )
            {
                $found = true;
                break;
            }
        }
        if ($found == false)
        {
            array_push($datagroupids, $newusergroupid);
        }

        $datagroups = implode(",", $datagroupids);

        $newGoupID = '';
        if ($int_userid == $userid)
        {
            $foundSameUser = true;
        }
        else
        {
            $newGoupID = '5';
        }

        update_userDataGroups($project_id, $curuserid, $datagroups, $newGoupID);
    }

    if ($foundSameUser == false)
    {
        $datagroups = getUserDataGroups_ById($project_id, $userid);
        $datagroupids = explode(",", $datagroups);

        $found = false;
        foreach( $datagroupids as $existed_groupid )
        {
            if ( $existed_groupid == $newusergroupid )
            {
                $found = true;
                break;
            }
        }
        if ($found == false)
        {
            array_push($datagroupids, $newusergroupid);
        }

        $datagroups = implode(",", $datagroupids);

        update_userDataGroups($project_id, $userid, $datagroups, "");
    }
}

function update_usersubgroup($project_id, $userid, $record)
{
    $db = new DB($project_id, 'base');

    $subgroupid= $record['subgroupid'];
    $subgroupname= $record['subgroupname'];
    $permission = $record['permission'];
    $imageuid = $record['imageuid'];
    $accountids = $record['accountids'];
    $userids = $record['userids'];
    $maingroupid = $record['maingroupid'];
    $type = $record['type'];
    $curriculumrecordid = $record['curriculumrecordid'];
    $validdate = $record['validdate'];

    if ($type == null)
        $type = 0;
    if ($curriculumrecordid == null)
        $curriculumrecordid = 0;
    if ($validdate == null)
        $validdate = '9999-12-31';

    $updateAccountids = "";
    if ( strlen($accountids) > 0 )
    {
        $updateAccountids = " accountids='".$accountids ."',";
    }
    $updateUserids = "";
    if ( strlen($userids) > 0 )
    {
        $updateUserids = " userids='".$userids ."',";

        check_userDataGroups($project_id, $userid, $userids, $subgroupid);
    }

    $db->query("UPDATE UserSubGroup SET subgroupname='$subgroupname', permission='$permission', imageuid='$imageuid', " .
               $updateAccountids . $updateUserids . " maingroupid='$maingroupid', type=$type, " .
               "curriculumrecordid=$curriculumrecordid, validdate='$validdate' WHERE subgroupid=$subgroupid ");
}

function create_usersubgroup($project_id, $maingroupid, $groupname, $accountids, $userids, $imageuid)
{
    $db = new DB($project_id, 'base');

    $db->query("INSERT INTO UserSubGroup(subgroupname, maingroupid, permission, accountids, userids, imageuid) VALUES ('$groupname', $maingroupid, '$groupname', '$accountids', '$userids', '$imageuid')");

    $newid = $db->get_insert_id();

    return $newid;
}

function delete_usersubgroup($project_id, $groupid)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM UserSubGroup WHERE subgroupid=$groupid");
}

function update_usermaingroup($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $groupid = $record['groupid'];
    $groupname = $record['groupname'];
    $permission = $record['permission'];
    $accountids = $record['accountids'];
    $userids = $record['userids'];
    $imageuid = $record['imageuid'];
    $autojoin = $record['autojoin'];
    $welcommessage = $record['welcommessage'];

    $db->query("UPDATE UserMainGroup SET name='$groupname', permission='$permission', accountids='$accountids', userids='$userids', imageuid='$imageuid', autojoin=$autojoin, welcommessage='$welcommessage' WHERE id=$groupid ");
}

function create_usermaingroup($project_id, $groupname, $accountids, $userids, $imageuid)
{
    $db = new DB($project_id, 'base');

    $db->query("INSERT INTO UserMainGroup(name, permission, accountids, userids, imageuid) VALUES ('$groupname', '$groupname', '$accountids', '$userids', '$imageuid')");

    $newid = $db->get_insert_id();

    return $newid;
}

function delete_usermaingroup($project_id, $groupid)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM UserMainGroup WHERE id=$groupid ");
}

function getSubGroupUserCount($project_id, $accountid, $maingroups)
{
    $datagroups = getUserDataGroups($project_id, $accountid);
    $datagroupids = explode(",", $datagroups);

    $alluser = loadmaingroupuser($project_id, $accountid, $maingroups);

    $count = 0;
    foreach ($alluser as $user)
    {
        $found = false;
        $subgroups = explode(",", $user['subgroups']);
        foreach ($datagroupids as $groupid)
        {
            if ( $groupid == '99999' )
            {
                $found = true;
                break;
            }

            foreach ($subgroups as $subgroupid)
            {
                if ( $groupid == $subgroupid )
                {
                    $found = true;
                    break;
                }
            }
            if ($found)
            {
                break;
            }
        }
        if ($found)
        {
            $count = $count + 1;
        }
    }

    return $count;
}

function getSubGroupUserCount_ById($project_id, $userid, $maingroups)
{
    $datagroups = getUserDataGroups_ById($project_id, $userid);
    $datagroupids = explode(",", $datagroups);

    $alluser = loadmaingroupuser_ById($project_id, $userid, $maingroups);

    $count = 0;
    foreach ($alluser as $user)
    {
        $found = false;
        $subgroups = explode(",", $user['subgroups']);
        foreach ($datagroupids as $groupid)
        {
            if ( $groupid == '99999' )
            {
                $found = true;
                break;
            }

            foreach ($subgroups as $subgroupid)
            {
                if ( $groupid == $subgroupid )
                {
                    $found = true;
                    break;
                }
            }
            if ($found)
            {
                break;
            }
        }
        if ($found)
        {
            $count = $count + 1;
        }
    }

    return $count;
}

function loadsubgroupuser($project_id, $accountid, $maingroups)
{
    $records = [];

    $datagroups = getUserDataGroups($project_id, $accountid);

    $datagroupids = explode(",", $datagroups);

    $alluser = loadmaingroupuser($project_id, $accountid, $maingroups);

    $count = 0;
    foreach ($alluser as $user)
    {
        $found = false;

        if (strlen($datagroups) > 0)
        {
            $subgroups = explode(",", $user['subgroups']);
            foreach ($datagroupids as $groupid)
            {
                if ( $groupid == '99999' )
                {
                    $found = true;
                    break;
                }

                foreach ($subgroups as $subgroupid)
                {
                    if ( $groupid == $subgroupid )
                    {
                        $found = true;
                        break;
                    }
                }
                if ($found)
                {
                    break;
                }
            }
        }

        if ($found)
        {
            $records[$count] = $user;
            $count = $count + 1;
        }
        else
        {
            if ( $user['account'] == $accountid )
            {
                $records[$count] = $user;
                $count = $count + 1;
            }
        }
    }

    return $records;
}

function loadsubgroupuser_ById($project_id, $userid, $maingroups)
{

    $records = [];

    $datagroups = getUserDataGroups_ById($project_id, $userid);

    $datagroupids = explode(",", $datagroups);

    $alluser = loadmaingroupuser_ById($project_id, $userid, $maingroups);

    $count = 0;
    $ct = 0;
    foreach ($alluser as $user)
    {
        $found = false;

        $ct = strlen($datagroups);
        if (strlen($datagroups) > 0)
        {
            // $ct++;
            $subgroups = explode(",", $user['subgroups']);
            foreach ($datagroupids as $groupid)
            {
                // $ct[$user['account']] = $groupid;
                if ( $groupid == '99999' )
                {
                    $found = true;
                    break;
                }

                foreach ($subgroups as $subgroupid)
                {
                    if ( $groupid == $subgroupid )
                    {
                        $found = true;
                        break;
                    }
                }
                if ($found)
                {
                    break;
                }
            }
        }

        if ($found)
        {
            $records[$count] = $user;
            $count = $count + 1;
        }
        else
        {
            if ( $user['userid'] == $userid )
            {
                $records[$count] = $user;
                $count = $count + 1;
            }
        }
    }
    // writeLogFile([$ct], "logfiletest.txt");

    return $records;
}

function getUserDataGroups($project_id, $accountid)
{
    $datagroups = '';

    $db = new DB($project_id, 'base');

    $db->query("SELECT datagroups FROM User WHERE account='$accountid' ");

    if($result = $db->fetch_array())
    {
        $datagroups = $result['datagroups'];
    }

    return $datagroups;
}

function getUserSubGroups($project_id, $accountid)
{
    $subgroups = '';

    $db = new DB($project_id, 'base');

    $db->query("SELECT subgroups FROM User WHERE account='$accountid' ");

    if($result = $db->fetch_array())
    {
        $subgroups = $result['subgroups'];
    }

    return $subgroups;
}

function getUserDataGroups_ById($project_id, $userid)
{
    $datagroups = '';

    $db = new DB($project_id, 'base');
    $db->query("SELECT datagroups FROM User WHERE userid=$userid ");

    if($result = $db->fetch_array())
    {
        // writeLogFile([$result['datagroups']], "logfiletest.txt");
        $datagroups = $result['datagroups'];
    }

    return $datagroups;
}

function getUserSubGroups_ById($project_id, $userid)
{
    $subgroups = '';

    $db = new DB($project_id, 'base');

    $db->query("SELECT subgroups FROM User WHERE userid=$userid ");

    if($result = $db->fetch_array())
    {
        $subgroups = $result['subgroups'];
    }

    return $subgroups;
}

function load_maingroup($project_id, $query_accountid, $query_userid, $query_groupid)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM UserMainGroup ORDER BY datetime DESC ");

    $cnt = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['id'] = $result['id'];
        $record['name'] = $result['name'];
        $record['permission'] = $result['permission'];
        $record['accountids'] = $result['accountids'];
        $record['userids'] = $result['userids'];
        $record['datetime'] = $result['datetime'];
        $record['imageuid'] = $result['imageuid'];
        $record['autojoin'] = $result['autojoin'];
        $record['welcommessage'] = $result['welcommessage'];

        if ($record['accountids'] == null)
            $record['accountids'] = '';
        if ($record['userids'] == null)
            $record['userids'] = '';
        if ($record['imageuid'] == null)
            $record['imageuid'] = '';
        if ($record['welcommessage'] == null)
            $record['welcommessage'] = '歡迎加入'.$record['name'];

        $found = false;
        if($query_groupid == 6)
        {
            // 主集合管理者, 檢查是否是此主集合管理者
            $accountids = explode(",", $record['accountids']);
            foreach ($accountids as $account)
            {
                if( $account == $query_accountid )
                {
                    $found = true;
                    break;
                }
            }

            if (!$found)
            {
                $userids = explode(",", $record['userids']);
                foreach ($userids as $userid)
                {
                    if( $userid == $query_userid )
                    {
                        $found = true;
                        break;
                    }
                }
            }
        }
        else
        {
            $found = true;
        }

        if ($found)
        {
            $records[$cnt] = $record;
            $cnt = $cnt + 1;
        }
    }

    return $records;
}

function load_maingroupname($project_id, $maingroupids)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM UserMainGroup WHERE id in($maingroupids) ");

    $cnt = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['id'] = $result['id'];
        $record['name'] = $result['name'];
        $records[$cnt] = $record;
        $cnt = $cnt + 1;
    }

    return $records;
}

function load_allmaingroupname($project_id)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM UserMainGroup ");

    $cnt = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['id'] = $result['id'];
        $record['name'] = $result['name'];
        $records[$cnt] = $record;
        $cnt = $cnt + 1;
    }

    return $records;
}

function load_maingroup_ById($project_id, $maingroupid)
{
    $record = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM UserMainGroup WHERE id=$maingroupid ");

    if($result = $db->fetch_array())
    {
        $record['id'] = $result['id'];
        $record['name'] = $result['name'];
        $record['permission'] = $result['permission'];
        $record['accountids'] = $result['accountids'];
        $record['userids'] = $result['userids'];
        $record['datetime'] = $result['datetime'];
        $record['imageuid'] = $result['imageuid'];
        $record['autojoin'] = $result['autojoin'];
        $record['welcommessage'] = $result['welcommessage'];

        if ($record['accountids'] == null)
            $record['accountids'] = '';
        if ($record['userids'] == null)
            $record['userids'] = '';
        if ($record['imageuid'] == null)
            $record['imageuid'] = '';
        if ($record['welcommessage'] == null)
            $record['welcommessage'] = '歡迎加入'.$record['name'];
    }

    return $record;
}

function load_subgroup_ById($project_id, $subgroupid)
{
    $record = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM UserSubGroup WHERE subgroupid=$subgroupid");
    if($result = $db->fetch_array())
    {
        $record['subgroupid'] = $result['subgroupid'];
        $record['subgroupname'] = $result['subgroupname'];
        $record['permission'] = $result['permission'];
        $record['accountids'] = $result['accountids'];
        $record['userids'] = $result['userids'];
        $record['datetime'] = $result['datetime'];
        $record['imageuid'] = $result['imageuid'];
        $record['maingroupid'] = $result['maingroupid'];
        $record['type'] = $result['type'];
        $record['validdate'] = $result['validdate'];
        $record['curriculumrecordid'] = $result['curriculumrecordid'];

        if ($record['accountids'] == null)
            $record['accountids'] = '';
        if ($record['userids'] == null)
            $record['userids'] = '';
        if ($record['imageuid'] == null)
            $record['imageuid'] = '';
        if ( $record['validdate'] == null )
            $record['validdate'] = '';
    }

    return $record;
}

function load_subgroup_ByCurriculumRecordID($project_id, $curriculumrecordid)
{
    $record = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM UserSubGroup WHERE curriculumrecordid=$curriculumrecordid");
    if($result = $db->fetch_array())
    {
        $record['subgroupid'] = $result['subgroupid'];
        $record['subgroupname'] = $result['subgroupname'];
        $record['permission'] = $result['permission'];
        $record['accountids'] = $result['accountids'];
        $record['userids'] = $result['userids'];
        $record['datetime'] = $result['datetime'];
        $record['imageuid'] = $result['imageuid'];
        $record['maingroupid'] = $result['maingroupid'];
        $record['type'] = $result['type'];
        $record['validdate'] = $result['validdate'];
        $record['curriculumrecordid'] = $result['curriculumrecordid'];

        if ($record['accountids'] == null)
            $record['accountids'] = '';
        if ($record['userids'] == null)
            $record['userids'] = '';
        if ($record['imageuid'] == null)
            $record['imageuid'] = '';
        if ( $record['validdate'] == null )
            $record['validdate'] = '';
    }

    return $record;
}

function load_view_access_subgroupids_ById($project_id, $userid)
{
    $targetgroupids = [];

    $datagroups = getUserDataGroups_ById($project_id, $userid);
    $subgroups = getUserSubGroups_ById($project_id, $userid);

    $datagroupids = explode(",", $datagroups);
    $subgroupids = explode(",", $subgroups);

    //$targetgroupids = $datagroupids;

    foreach ($datagroupids as $datagroupid)
    {
        array_push($targetgroupids, (int)$datagroupid);
    }

    // skip same groupid
    foreach ($subgroupids as $subgroupid)
    {
        $found = false;
        $int_subgroupid = (int)$subgroupid;
        foreach ($targetgroupids as $targetid)
        {
            if ( $int_subgroupid == $targetid )
            {
                $found = true;
                break;
            }
        }
        if ($found == false)
        {
            array_push($targetgroupids, $int_subgroupid);
        }
    }
    return $targetgroupids;
}

function load_view_access_subgroupids($project_id, $accountid)
{
    $targetgroupids = [];

    $datagroups = getUserDataGroups($project_id, $accountid);
    $subgroups = getUserSubGroups($project_id, $accountid);

    $datagroupids = explode(",", $datagroups);
    $subgroupids = explode(",", $subgroups);

    //$targetgroupids = $datagroupids;

    foreach ($datagroupids as $datagroupid)
    {
        array_push($targetgroupids, (int)$datagroupid);
    }

    // skip same groupid
    foreach ($subgroupids as $subgroupid)
    {
        $found = false;
        $int_subgroupid = (int)$subgroupid;
        foreach ($targetgroupids as $targetid)
        {
            if ( $int_subgroupid == $targetid )
            {
                $found = true;
                break;
            }
        }
        if ($found == false)
        {
            array_push($targetgroupids, $int_subgroupid);
        }
    }
    return $targetgroupids;
}

function load_view_access_subgroup($project_id, $accountid)
{
    $records = [];

    $targetgroupids = load_view_access_subgroupids($project_id, $accountid);

    $AllSubGroup = loadusersubgroup($project_id, 0, 0, '');

    $count = 0;
    foreach ($targetgroupids as $groupid)
    {
        $addall = false;
        if ( $groupid == '99999' )
        {
            $addall = true;
        }

        $int_groupid = (int)$groupid;
        foreach ($AllSubGroup as $subgroup)
        {
            if ( $addall )
            {
                $records[$count] = $subgroup;
                $count = $count + 1;
                continue;
            }

            $int_subgroupid = (int)$subgroup['subgroupid'];
            if ( $int_groupid == $int_subgroupid)
            {
                $records[$count] = $subgroup;
                $count = $count + 1;
                break;
            }
        }
        if ( $addall )
        {
            break;
        }
    }

    return $records;
}

function loadsubgroup($project_id, $userid, $type)
{
    $records = [];

    // $datagroups = getUserDataGroups($project_id, $accountid);
    $datagroups = getUserDataGroups_ById($project_id, $userid);

    $datagroupids = explode(",", $datagroups);

    $AllSubGroup = loadusersubgroup($project_id, 0, 0, $type);

    $count = 0;
    foreach ($datagroupids as $groupid)
    {
        $addall = false;
        if ( $groupid == '99999' )
        {
            $addall = true;
        }

        $int_groupid = (int)$groupid;
        foreach ($AllSubGroup as $subgroup)
        {
            if ( $addall )
            {
                $records[$count] = $subgroup;
                $count = $count + 1;
                continue;
            }

            $int_subgroupid = $subgroup['subgroupid'];
            if ( $int_groupid == $int_subgroupid)
            {
                $records[$count] = $subgroup;
                $count = $count + 1;
                break;
            }
        }
        if ( $addall )
        {
            break;
        }
    }

    return $records;
}

function loaddatagroup($project_id, $userid, $type)
{
    $records = [];

    // $subgroups = getUserSubGroups($project_id, $accountid);
    $subgroups = getUserSubGroups_ById($project_id, $userid);
    $subgroupids = explode(",", $subgroups);

    $AllSubGroup = loadusersubgroup($project_id, 0, 0, $type);

    $count = 0;
    foreach ($subgroupids as $groupid)
    {
        $addall = false;
        if ( $groupid == '99999' )
        {
            $addall = true;
        }

        $int_groupid = (int)$groupid;
        foreach ($AllSubGroup as $subgroup)
        {
            if ( $addall )
            {
                $records[$count] = $subgroup;
                $count = $count + 1;
                continue;
            }

            $int_subgroupid = $subgroup['subgroupid'];
            if ( $int_groupid == $int_subgroupid)
            {
                $records[$count] = $subgroup;
                $count = $count + 1;
                break;
            }
        }
        if ( $addall )
        {
            break;
        }
    }

    return $records;
}

function check_user_datagroup_permssion($CurUserDataGroupids, $subgroups)
{
    $found = false;

    foreach ($CurUserDataGroupids as $groupid)
    {
        if ( $groupid == '99999' )
        {
            $found = true;
            break;
        }
        foreach ($subgroups as $subgroupid)
        {
            if ( $groupid == $subgroupid )
            {
                $found = true;
                break;
            }
        }
        if ($found)
        {
            break;
        }
    }

    return $found;
}

function check_user_rollcall_permssion($rollcalls, $rollcallid)
{
    $found = false;
    foreach ($rollcalls as $rollcall)
    {
        if ( $rollcall['rollcallid'] == $rollcallid )
        {
            $found = true;
            break;
        }
    }

    return $found;
}

function fetch_user_result($result)
{
    $record = [];

    $record['userid'] = $result['userid'];
    $record['groupid'] = $result['groupid'];
    $record['name'] = $result['name'];
    $record['account'] = $result['account'];
    $record['title'] = $result['title'];
    $record['datetime'] = $result['datetime'];
    $record['maingroups'] = $result['maingroups'];
    $record['datagroups'] = $result['datagroups'];
    $record['subgroups'] = $result['subgroups'];
    $record['rfidcards'] = $result['rfidcards'];
    $record['nfccards'] = $result['nfccards'];
    $record['uwbcards'] = $result['uwbcards'];
    $record['sensors'] = $result['sensors'];
    $record['imageuid'] = $result['imageuid'];
    $record['maplayer'] = $result['maplayer'];
    $record['posX'] = $result['posX'];
    $record['posY'] = $result['posY'];
    $record['posZ'] = $result['posZ'];
    $record['Longitude'] = $result['Longitude'];
    $record['Latitude'] = $result['Latitude'];
    $record['AreaId'] = $result['AreaId'];
    $record['AreaName'] = $result['AreaName'];

    if ($record['maingroups'] == null)
    {
        $record['maingroups'] = '1';
    }
    if ($record['datagroups'] == null)
    {
        $record['datagroups'] = '';
    }
    if ($record['subgroups'] == null)
    {
        $record['subgroups'] = '';
    }
    if ($record['rfidcards'] == null)
    {
        $record['rfidcards'] = '';
    }
    if ($record['nfccards'] == null)
    {
        $record['nfccards'] = '';
    }
    if ($record['uwbcards'] == null)
    {
        $record['uwbcards'] = '';
    }
    if ($record['sensors'] == null)
    {
        $record['sensors'] = '';
    }

    if ($result['cardid'] != null)
    {
        if (strlen($record['rfidcards']) == 0 )
        {
            $record['rfidcards'] = $result['cardid'];
        }
    }

    if ($record['title'] == null)
    {
        $record['title'] = '';
    }
    if ($record['imageuid'] == null)
    {
        $record['imageuid'] = '';
    }

    if ($record['Longitude'] == null)
    {
        $record['Longitude'] = '';
    }
    if ($record['Latitude'] == null)
    {
        $record['Latitude'] = '';
    }

    if ($record['AreaId'] == null)
    {
        $record['AreaId'] = '';
    }
    if ($record['AreaName'] == null)
    {
        $record['AreaName'] = '';
    }

    return $record;
}

function loadsingleuser($project_id, $query_accountid)
{
    $record = [];

    $db = new DB($project_id, 'base');

    if ( $db->query("SELECT * FROM User WHERE account='$query_accountid' ") == TRUE)
    {
        if($result = $db->fetch_array())
        {
            $record = fetch_user_result($result);
        }
    }

    return $record;
}

function loadsingleuser_ById($project_id, $userid)
{
    $record = [];

    $db = new DB($project_id, 'base');

    if ( $db->query("SELECT * FROM User WHERE userid=$userid ") == TRUE)
    {
        if($result = $db->fetch_array())
        {
            $record = fetch_user_result($result);
        }
    }

    return $record;
}

function loaduser($project_id, $userid, $start, $count, $subgroup, $maingroups)
{
    $records = [];
    // $subgroupuser = loadsubgroupuser($project_id, $accountid, $maingroups);
    $subgroupuser = loadsubgroupuser_ById($project_id, $userid, $maingroups);
    // writeLogFile(["loaduser->userid::".$userid.",maingroups::$maingroups"], 'logfiletest.txt');

    $subgroup_int = (int)$subgroup;
    $fileter_subgroup = false;
    if (strlen($subgroup) > 0)
    {
        $fileter_subgroup = true;
    }

    $recordcount = 0;
    $userdatacount = 0;
    $ct = 0;
    foreach ($subgroupuser as $user)
    {
        $ct++;
        $valid = true;
        $manager = false;
        if ($fileter_subgroup)
        {
            $valid = false;
            $subgroups = explode(",", $user['subgroups']);
            foreach ($subgroups as $groupid)
            {
                if ( $subgroup_int == (int)$groupid )
                {
                    $valid = true;
                    break;
                }
            }
            if (!$valid)
            {
                $datagroups = explode(",", $user['datagroups']);
                foreach ($datagroups as $groupid)
                {
                    if ( $subgroup_int == (int)$groupid )
                    {
                        $valid = true;
                        $manager = true;
                        break;
                    }
                }
            }
        }
        if (!$valid)
        {
            continue;
        }

        if ( $start == 0 && $count == 0)
        {
            $valid = true;
        }
        else
        if ( ($userdatacount >= $start) && ($recordcount < $count))
        {
            $valid = true;
        }
        else
        {
            $valid = false;
        }

        if ( $valid )
        {
            if ($manager)
            {
                // put to first one
                $new_records = [];
                $new_records[0] = $user;
                $tmp_records = array_merge($new_records, $records);
                $records = $tmp_records;
            }
            else
            {
                $records[$recordcount] = $user;
            }
            $recordcount++;
        }

        $userdatacount++;
    }
    return $records;
}

function loaduser_ById($project_id, $userid, $start, $count, $subgroup, $maingroups)
{
    $records = [];
    //$subgroupuser = loadsubgroupuser($project_id, $accountid, $maingroups);
    $subgroupuser = loadsubgroupuser_ById($project_id, $userid, $maingroups);

    $subgroup_int = (int)$subgroup;
    $fileter_subgroup = false;
    if (strlen($subgroup) > 0)
    {
        $fileter_subgroup = true;
    }

    $recordcount = 0;
    $userdatacount = 0;
    foreach ($subgroupuser as $user)
    {
        $valid = true;
        if ($fileter_subgroup)
        {
            $valid = false;
            $subgroups = explode(",", $user['subgroups']);
            foreach ($subgroups as $groupid)
            {
                if ( $subgroup_int == (int)$groupid )
                {
                    $valid = true;
                    break;
                }
            }
        }
        if (!$valid)
        {
            continue;
        }

        if ( $start == 0 && $count == 0)
        {
            $records[$recordcount] = $user;
            $recordcount++;
        }
        else
        if ( ($userdatacount >= $start) && ($recordcount < $count))
        {
            $records[$recordcount] = $user;
            $recordcount++;
        }

        $userdatacount++;
    }
    return $records;
}

function loaduserbaseinfo($project_id, $session_userid, $start, $count, $maingroups, $userid)
{
    $records = [];
    $subgroupuser = loadsubgroupuser_ById($project_id, $session_userid, $maingroups);

    $recordcount = 0;
    $userdatacount = 0;
    foreach ($subgroupuser as $user)
    {
        $valid = false;
        if ( strlen($userid) > 0)
        {
            if ( (int)$user['userid'] == (int)$userid )
            {
                $valid = true;
            }
        }
        else
        if ( $start == 0 && $count == 0)
        {
            $valid = true;
        }
        else
        if ( ($userdatacount >= $start) && ($recordcount < $count))
        {
            $valid = true;
        }

        if ( $valid )
        {
            $record = [];
            $record['userid'] = $user['userid'];
            $record['name'] = $user['name'];
            $record['account'] = $user['account'];
            if (strlen($user['sensors']) > 0)
                $record['sensors'] = $user['sensors'];

            $records[$recordcount] = $record;
            $recordcount++;
        }
        $userdatacount++;
    }
    return $records;
}

function loaduserpoistioninfo($project_id, $accountid, $start, $count, $maingroups)
{
    $records = [];
    $subgroupuser = loadsubgroupuser($project_id, $accountid, $maingroups);

    $recordcount = 0;
    $userdatacount = 0;
    foreach ($subgroupuser as $user)
    {
        if ( $start == 0 && $count == 0)
        {
            $record = [];
            $record['userid'] = $user['userid'];
            $record['name'] = $user['name'];
            $record['account'] = $user['account'];
            $record['maplayer'] = $user['maplayer'];
            $record['posX'] = $user['posX'];
            $record['posY'] = $user['posY'];
            $record['posZ'] = $user['posZ'];
            $record['Longitude'] = $user['Longitude'];
            $record['Latitude'] = $user['Latitude'];
            $record['AreaId'] = $user['AreaId'];
            $record['AreaName'] = $user['AreaName'];
            if (strlen($user['uwbcards']) > 0)
                $record['uwbcards'] = $user['uwbcards'];
            if (strlen($user['sensors']) > 0)
                $record['sensors'] = $user['sensors'];

            $records[$recordcount] = $record;
            $recordcount++;
        }
        else
        if ( ($userdatacount >= $start) && ($recordcount < $count))
        {
            $record = [];
            $record['userid'] = $user['userid'];
            $record['name'] = $user['name'];
            $record['account'] = $user['account'];
            $record['maplayer'] = $user['maplayer'];
            $record['posX'] = $user['posX'];
            $record['posY'] = $user['posY'];
            $record['posZ'] = $user['posZ'];
            $record['Longitude'] = $user['Longitude'];
            $record['Latitude'] = $user['Latitude'];
            $record['AreaId'] = $user['AreaId'];
            $record['AreaName'] = $user['AreaName'];
            if (strlen($user['uwbcards']) > 0)
                $record['uwbcards'] = $user['uwbcards'];
            if (strlen($user['sensors']) > 0)
                $record['sensors'] = $user['sensors'];

            $records[$recordcount] = $record;
            $recordcount++;
        }

        $userdatacount++;
    }
    return $records;
}
// 123
function queryUsers($project_id, $userid, $maingroups)
{
    $records = [];

    $maingroupList = explode(",", $maingroups);

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM User WHERE userid in($userid) ");

    $datacnt = 0;
    while($result = $db->fetch_array())
    {
        $valid = check_valid_maingroup($maingroupList, $result['maingroups']);

        if ($valid == false)
        {
            continue;
        }

        $record = fetch_user_result($result);

        $records[$datacnt] = $record;
        $datacnt++;
    }

    return $records;
}

function getUserImageuidFromUserid($project_id, $userid)
{
    $imageuid = '';

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM User WHERE userid in($userid) ");
    while($result = $db->fetch_array())
    {
        $imageuid = $result['imageuid'];
    }

    return $imageuid;
}

function getSubGroupImageuidFromId($project_id, $subgroupid)
{
    $imageuid = '';

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM UserSubGroup WHERE subgroupid in($subgroupid) ");
    while($result = $db->fetch_array())
    {
        $imageuid = $result['imageuid'];
    }

    return $imageuid;
}

function getMainGroupImageuidFromId($project_id, $maingroupid)
{
    $imageuid = '';

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM UserMainGroup WHERE id in($maingroupid) ");
    while($result = $db->fetch_array())
    {
        $imageuid = $result['imageuid'];
    }

    return $imageuid;
}

function queryUsersByAccount($project_id, $account)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $accountlist = explode(",", $account);
    $accounts = '\'' . implode("','", $accountlist).'\'';

    $db->query("SELECT * FROM User WHERE account in($accounts) ");

    $datacnt = 0;
    while($result = $db->fetch_array())
    {
        $record = fetch_user_result($result);

        $records[$datacnt] = $record;
        $datacnt++;
    }

    return $records;
}

function loadmaingroupuser($project_id, $accountid, $maingroups)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM User ORDER BY userid ASC ");

    $maingroupList = explode(",", $maingroups);
    $loadAll = check_account_permisson($project_id, $accountid);

    $datacnt = 0;
    while($result = $db->fetch_array())
    {
        if ($loadAll == false)
        {
            $valid = check_valid_maingroup($maingroupList, $result['maingroups']);
            if ($valid == false)
            {
                continue;
            }
        }

        $record = fetch_user_result($result);

        $records[$datacnt] = $record;
        $datacnt++;
    }

    return $records;
}

function loadmaingroupuser_ById($project_id, $userid, $maingroups)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM User ORDER BY userid ASC ");

    $maingroupList = explode(",", $maingroups);
    $loadAll = check_user_permisson($project_id, $userid);

    $datacnt = 0;
    while($result = $db->fetch_array())
    {
        if ($loadAll == false)
        {
            $valid = check_valid_maingroup($maingroupList, $result['maingroups']);

            if ($valid == false)
            {
                continue;
            }
        }

        $record = fetch_user_result($result);

        $records[$datacnt] = $record;
        $datacnt++;
    }

    return $records;
}

function check_valid_maingroup($maingroupList, $target_maingroups)
{
    $valid = false;

    if ( $target_maingroups == null )
        return false;

    $targetList = explode(",", $target_maingroups);

    foreach ($maingroupList as $maingroup)
    {
        if ($maingroup == '1')
        {
            $valid = true;
        }
        else
        {
            foreach ($targetList as $target)
            {
                if ( $maingroup == $target )
                {
                    $valid = true;
                    break;
                }
            }
        }
        if ($valid)
            break;
    }

    return $valid;
}


function search_user($project_id, $search_text, $start, $count)
{
    $records = [];

    $db = new DB($project_id, 'base');

    if ($count == 0)
        $db->query("SELECT * FROM User WHERE name LIKE '%{$search_text}%' OR account LIKE '%{$search_text}%' ORDER BY userid ASC LIMIT $start,5000 ");
    else
        $db->query("SELECT * FROM User WHERE name LIKE '%{$search_text}%' OR account LIKE '%{$search_text}%' ORDER BY userid ASC LIMIT $start,$count ");

    $datacnt = 0;

    while($result = $db->fetch_array())
    {
        $record = fetch_user_result($result);

        $records[$datacnt] = $record;
        $datacnt++;
    }

    return $records;
}

function search_subgroup_user($project_id, $userid, $search_text, $start, $count, $maingroups)
{
    $records = [];

    $mySubgroupUsers = loaduser($project_id, $userid, 0, 0, '', $maingroups);

    $recordcount = 0;
    $userdatacount = 0;
    foreach ($mySubgroupUsers as $user)
    {
        $name = $user['name'];
        $account = $user['account'];

        $found = false;
        if (stripos($name, $search_text) !== false)
        {
            $found = true;
        }
        else
        if (stripos($account, $search_text) !== false)
        {
            $found = true;
        }

        if ($found)
        {
            if ( $start == 0 && $count == 0)
            {
                $records[$recordcount] = $user;
                $recordcount++;
            }
            else
            if ( ($userdatacount >= $start) && ($recordcount < $count))
            {
                $records[$recordcount] = $user;
                $recordcount++;
            }
        }
        $userdatacount++;
    }

    return $records;
}

function search_subgroup_user_ById($project_id, $userid, $search_text, $start, $count, $maingroups)
{
    $records = [];

    $mySubgroupUsers = loaduser_ById($project_id, $userid, 0, 0, '', $maingroups);

    $recordcount = 0;
    $userdatacount = 0;
    foreach ($mySubgroupUsers as $user)
    {
        $name = $user['name'];
        $account = $user['account'];

        $found = false;
        if (stripos($name, $search_text) !== false)
        {
            $found = true;
        }
        else
        if (stripos($account, $search_text) !== false)
        {
            $found = true;
        }

        if ($found)
        {
            if ( $start == 0 && $count == 0)
            {
                $records[$recordcount] = $user;
                $recordcount++;
            }
            else
            if ( ($userdatacount >= $start) && ($recordcount < $count))
            {
                $records[$recordcount] = $user;
                $recordcount++;
            }
        }
        $userdatacount++;
    }

    return $records;
}

function delete_rfidcard_from_User($project_id, $cardid)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT userid,rfidcards FROM User WHERE rfidcards LIKE '%{$cardid}%' ");

    $datacnt = 0;

    while($result = $db->fetch_array())
    {
        $user = [];
        $user['userid'] = $result['userid'];
        $user['rfidcards'] = $result['rfidcards'];

        $records[$datacnt] = $user;
        $datacnt++;
    }

    $datacnt = 0;
    foreach ($records as $user)
    {
        $cardcnt = 0;
        $rfidcards_old = explode(",", $user['rfidcards']);
        $rfidcards_new = [];
        foreach ($rfidcards_old as $rfidcard)
        {
            if( $rfidcard != $cardid )
            {
                $rfidcards_new[$cardcnt] = $rfidcard;
                $cardcnt++;
            }
        }
        $user['rfidcards'] = implode(",",$rfidcards_new);
        $records[$datacnt] = $user;
        $datacnt++;

    }

    foreach ($records as $user)
    {
        $userid = $user['userid'];
        $rfidcards = $user['rfidcards'];
        $db->query("UPDATE User SET rfidcards='$rfidcards' WHERE userid= $userid");
    }
}

function delete_nfccard_from_User($project_id, $cardid)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT userid,nfccards FROM User WHERE nfccards LIKE '%{$cardid}%' ");

    $datacnt = 0;

    while($result = $db->fetch_array())
    {
        $user = [];
        $user['userid'] = $result['userid'];
        $user['nfccards'] = $result['nfccards'];

        $records[$datacnt] = $user;
        $datacnt++;
    }

    $datacnt = 0;
    foreach ($records as $user)
    {
        $cardcnt = 0;
        $nfccards_old = explode(",", $user['nfccards']);
        $nfccards_new = [];
        foreach ($nfccards_old as $nfccard)
        {
            if( $nfccard != $cardid )
            {
                $nfccards_new[$cardcnt] = $nfccard;
                $cardcnt++;
            }
        }
        $user['nfccards'] = implode(",",$nfccards_new);
        $records[$datacnt] = $user;
        $datacnt++;

    }

    foreach ($records as $user)
    {
        $userid = $user['userid'];
        $nfccards = $user['nfccards'];
        $db->query("UPDATE User SET nfccards='$nfccards' WHERE userid= $userid");
    }
}

function delete_nfccard_from_maingroupUser($project_id, $cardid, $maingroups)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT userid,nfccards,maingroups FROM User WHERE nfccards LIKE '%{$cardid}%' ");

    $datacnt = 0;

    $target_maingroupList = explode(",", $maingroups);

    while($result = $db->fetch_array())
    {
        //
        // check if in same maingroup
        //
        $found = false;
        $user_maingroupList = explode(",", $result['maingroups']);
        foreach ($user_maingroupList as $user_maingroup)
        {
            foreach ($target_maingroupList as $target_maingroup)
            {
                if ( (int)$user_maingroup == (int)$target_maingroup )
                {
                    $found = true;
                }
            }
            if ($found)
                break;
        }

        if (!$found)
        {
            continue;
        }

        $user = [];
        $user['userid'] = $result['userid'];
        $user['nfccards'] = $result['nfccards'];

        $records[$datacnt] = $user;
        $datacnt++;
    }

    $datacnt = 0;
    foreach ($records as $user)
    {
        $cardcnt = 0;
        $nfccards_old = explode(",", $user['nfccards']);
        $nfccards_new = [];
        foreach ($nfccards_old as $nfccard)
        {
            if( $nfccard != $cardid )
            {
                $nfccards_new[$cardcnt] = $nfccard;
                $cardcnt++;
            }
        }
        $user['nfccards'] = implode(",",$nfccards_new);
        $records[$datacnt] = $user;
        $datacnt++;

    }

    foreach ($records as $user)
    {
        $userid = $user['userid'];
        $nfccards = $user['nfccards'];
        $db->query("UPDATE User SET nfccards='$nfccards' WHERE userid= $userid");
    }
}

function loadusername($project_id, $accounts)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $cnt = 0;
    $query_state = '';
    foreach ($accounts as $account)
    {
        if ($cnt > 0)
        {
            $query_state .= ' OR ';
        }

        $query_state .= " account='$account' ";

        $cnt++;
    }

    $db->query("SELECT userid,account,name,title FROM User WHERE $query_state ");

    $datacnt = 0;

    while($result = $db->fetch_array())
    {
        $user = [];
        $user['userid'] = $result['userid'];
        $user['account'] = $result['account'];
        $user['name'] = $result['name'];
        $user['title'] = $result['title'];

        if ($user['title'] == null)
        {
            $user['title'] = '';
        }

        $records[$datacnt] = $user;
        $datacnt++;
    }

    return $records;
}

function loadusername_byuserids($project_id, $userids)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $cnt = 0;
    $query_state = '';
    foreach ($userids as $userid)
    {
        if ($cnt > 0)
        {
            $query_state .= ' OR ';
        }

        $query_state .= " userid=$userid ";

        $cnt++;
    }

    $db->query("SELECT userid,account,name,title FROM User WHERE $query_state ");

    $datacnt = 0;

    while($result = $db->fetch_array())
    {
        $user = [];
        $user['userid'] = $result['userid'];
        $user['account'] = $result['account'];
        $user['name'] = $result['name'];
        $user['title'] = $result['title'];

        if ($user['title'] == null)
        {
            $user['title'] = '';
        }

        $records[$datacnt] = $user;
        $datacnt++;
    }

    return $records;
}

function check_user_permisson($project_id, $userid)
{
    $loadAll = false;

    $session_groupid = $_SESSION["groupid"];
    if ($session_groupid == 1 || $session_groupid == 2)
    {
        return true;
    }

    $datagroups = getUserDataGroups_ById($project_id, $userid);

    $datagroupids = explode(",", $datagroups);
    foreach ($datagroupids as $groupid)
    {
        if ( $groupid == '99999' )
        {
            $loadAll = true;
            break;
        }
    }

    return $loadAll;
}

function check_account_permisson($project_id, $accountid)
{
    $loadAll = false;

    $session_groupid = $_SESSION["groupid"];
    if ($session_groupid == 1 || $session_groupid == 2)
    {
        return true;
    }

    $datagroups = getUserDataGroups($project_id, $accountid);
    $datagroupids = explode(",", $datagroups);
    foreach ($datagroupids as $groupid)
    {
        if ( $groupid == '99999' )
        {
            $loadAll = true;
            break;
        }
    }

    return $loadAll;
}

//
// register table
//
function check_account_email_existed($project_id, $email)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM register WHERE email='$email' ");

    $existed = false;
    $register = [];
    if ($result = $db->fetch_array())
    {
        $existed = true;
        $register['email'] = $result['email'];
        $register['pwd'] = $result['pwd'];
        $register['token'] = $result['token'];
        $register['subgroupname'] = $result['subgroupname'];
        $register['send'] = $result['send'];
        $register['valid'] = $result['valid'];
    }

    return array($existed, $register);
}

function create_regitser($project_id, $register_email, $pwd, $token, $subgroupname)
{
    $db = new DB($project_id, 'base');

    $db->query("INSERT INTO register(email, pwd, token, subgroupname, send, valid) VALUES ('$register_email', '$pwd', '$token', '$subgroupname', 1, 0)");

    $newid = $db->get_insert_id();
    return $newid;
}

function update_register($project_id, $registerinfo)
{
    $db = new DB($project_id, 'base');

    $email = $registerinfo['email'];
    $pwd = $registerinfo['pwd'];
    $token = $registerinfo['token'];
    $subgroupname = $registerinfo['subgroupname'];
    $send = $registerinfo['send'];
    $valid = $registerinfo['valid'];

    $db->query("UPDATE register SET pwd='$pwd', token='$token', subgroupname='$subgroupname', send=$send, valid=$valid WHERE email='$email' ");
}

function delete_register($project_id, $email)
{
    $db = new DB($project_id, 'base');
    $db->query("DELETE FROM register WHERE email='$email' ");
}

function loaduserregisterdata($project_id, $offset, $count)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM register ORDER BY datetime DESC LIMIT $offset,$count ");

    $records = [];
    $datacnt = 0;
    while ($result = $db->fetch_array())
    {
        $register = [];
        $register['email'] = $result['email'];
        $register['subgroupname'] = $result['subgroupname'];
        $register['send'] = $result['send'];
        $register['valid'] = $result['valid'];
        $register['datetime'] = $result['datetime'];

        $records[$datacnt] = $register;
        $datacnt++;
    }

    return $records;
}


function create_new_account($project_id, $registerinfo)
{
    //
    // 新增 主集合管理者 身份
    // groupid=6, account=email, name=email, pwd=pwd, maingroups=新增, subgroups=新增
    //
    $db = new DB($project_id, 'base');

    $email = $registerinfo['email'];
    $pwd = $registerinfo['pwd'];
    $subgroupname = $registerinfo['subgroupname'];

    $endIndex = strpos($email, '@');
    $name = substr($email, 0, $endIndex);

    $maingroups = create_usermaingroup($project_id, $subgroupname, $email, '', '');

    $datagroups = create_usersubgroup($project_id, $maingroups, $subgroupname, $email, '', '');

    //  groupid:6主集合管理者 int type: 0: vilscenter user 1:已註冊, 2:已購買點名裝置
    $cmd = "INSERT INTO User(groupid, type, account, name, pwd, maingroups, datagroups) VALUES (6, 1, '$email', '$name', '$pwd', '$maingroups', '$datagroups')";

    $db->query($cmd);

    $userid = $db->get_insert_id();

    $maingrouprecord = [];
    $maingrouprecord['groupid'] = $maingroups;
    $maingrouprecord['groupname'] = $subgroupname;
    $maingrouprecord['permission'] = $subgroupname;
    $maingrouprecord['accountids'] = $email;
    $maingrouprecord['userids'] = $userid;
    update_usermaingroup($project_id, $maingrouprecord);

    $subgrouprecord = [];
    $subgrouprecord['subgroupid'] = $datagroups;
    $subgrouprecord['subgroupname'] = $subgroupname;
    $subgrouprecord['permission'] = $subgroupname;
    $subgrouprecord['accountids'] = $email;
    $subgrouprecord['userids'] = $userid;
    $subgrouprecord['imageuid'] = '';
    $subgrouprecord['maingroupid'] = $maingroups;
    $subgrouprecord['type'] = 0;
    $subgrouprecord['validdate'] = '9999-12-31';
    $subgrouprecord['curriculumrecordid'] = 0;
    update_usersubgroup($project_id, $userid, $subgrouprecord);

    return array($userid, $datagroups);
}

function getTopicFromUserid($project_id, $userid)
{
    $targettopic = [];
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM TargetTopic WHERE userid=$userid ");

    if($result = $db->fetch_array())
    {
        $targettopic['target'] = $result['target'];
        $targettopic['topic'] = $result['topic'];
        $targettopic['name'] = $result['name'];
        $targettopic['userid'] = $result['userid'];
        $targettopic['maingroupid'] = $result['maingroupid'];
    }

    return $targettopic;
}
?>
