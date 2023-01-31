<?php


function create_new_rfidrollcall_from_curriculumerecord($project_id, $curriculumerecord, $rollcallname, $subgroupid)
{
    $rollcall = [];
    $rollcall['project_id'] = $project_id;
    $rollcall['rollcallname'] = $rollcallname;
    $rollcall['StartTime'] = $curriculumerecord['signinstarttime'];
    $rollcall['EndTime'] = $curriculumerecord['signinendtime'];
    $rollcall['StartDate'] = $curriculumerecord['startdate'];
    $rollcall['EndDate'] = $curriculumerecord['enddate'];
    $rollcall['rfiddeviceids'] = '';
    $rollcall['nfclocators'] = '';
    $rollcall['subgroups'] = (int)$subgroupid;
    $rollcall['weekday'] = $curriculumerecord['weekday'];
    $rollcall['presentnotify'] = 0;
    $rollcall['presentcomment'] = '點名已到';
    $rollcall['absentnotify'] = 0;
    $rollcall['absentcomment'] = '點名未到';
    $rollcall['resend'] = 0;
    $rollcall['maingroups'] = $curriculumerecord["usermaingroups"];
    $rollcall['type'] = 1;
    $rollcall['curriculumrecordid'] = $curriculumerecord['id'];

    //
    // find default nfc locator
    //
    $json_array = [];
    $json_array['project_id'] = $project_id;
    $json_array['start'] = 0;
    $json_array['count'] = 10;
    $json_array['groupid'] = $_SESSION["groupid"];
    $json_array['maingroups'] = $_SESSION["maingroups"];
    $json_string = json_encode($json_array);
    $status = getServerInfo('nfcdevicetatus='.$json_string);

    $LoRAMacaddress_array = convertNFCDeviceToLoRAMacaddressArray(json_decode($status));
    $rollcall['nfclocators'] = implode(",", $LoRAMacaddress_array);

    save_new_rfidrollcall($project_id, $rollcall);
}

function convertNFCDeviceToLoRAMacaddressArray($devices)
{
    $LoRAMacaddress_array = [];

    $count = 0;
    foreach($devices as $device)
    {
        $LoRAMacaddress_array[$count] = $device->{'LoRAMacaddress'};
        $count = $count + 1;
    }

    return $LoRAMacaddress_array;
}

function save_new_rfidrollcall($project_id, $rollcall)
{
    $db = new DB($project_id, 'base');

    $rollcallname = $rollcall['rollcallname'];
    $StartTime = $rollcall['StartTime'];
    $EndTime = $rollcall['EndTime'];
    $StartDate = $rollcall['StartDate'];
    $EndDate = $rollcall['EndDate'];
    $rfiddeviceids = $rollcall['rfiddeviceids'];
    $nfclocators = $rollcall['nfclocators'];
    $subgroups = $rollcall['subgroups'];
    $weekday = $rollcall['weekday'];
    $validdate = $rollcall['validdate'];
    $invaliddate = $rollcall['invaliddate'];
    $presentnotify = $rollcall['presentnotify'];
    $presentcomment = $rollcall['presentcomment'];
    $absentnotify = $rollcall['absentnotify'];
    $absentcomment = $rollcall['absentcomment'];
    $resend = $rollcall['resend'];
    $maingroups = $rollcall['maingroups'];
    $type = $rollcall['type'];
    $curriculumrecordid = $rollcall['curriculumrecordid'];

    if ($StartDate == null)
        $StartDate = '';
    if ($EndDate == null)
        $EndDate = '';
    if ($type == null)
        $type = 0;
    if ($curriculumrecordid == null)
        $curriculumrecordid = 0;

    $db->query("INSERT INTO CARDRollCall(rollcallname, StartTime, EndTime, StartDate, EndDate, rfiddeviceids, nfclocators, subgroups, weekday, validdate, invaliddate, presentnotify, presentcomment, absentnotify, absentcomment, resend, maingroups, type, curriculumrecordid) " .
                "VALUES ('$rollcallname', '$StartTime', '$EndTime', '$StartDate', '$EndDate', '$rfiddeviceids', '$nfclocators', '$subgroups', '$weekday', '$validdate', '$invaliddate', $presentnotify, '$presentcomment', $absentnotify, '$absentcomment', $resend, '$maingroups', $type, $curriculumrecordid)");
}

function delete_cardrollcall($project_id, $rollcallid)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM CARDRollCall WHERE rollcallid=$rollcallid ");
}

function update_cardrollcall($project_id, $rollcall)
{
    $db = new DB($project_id, 'base');

    $rollcallid = $rollcall['rollcallid'];
    $rollcallname = $rollcall['rollcallname'];
    $StartTime = $rollcall['StartTime'];
    $EndTime = $rollcall['EndTime'];
    $StartDate = $rollcall['StartDate'];
    $EndDate = $rollcall['EndDate'];
    $rfiddeviceids = $rollcall['rfiddeviceids'];
    $nfclocators = $rollcall['nfclocators'];
    $subgroups = $rollcall['subgroups'];
    $weekday = $rollcall['weekday'];
    $validdate = $rollcall['validdate'];
    $invaliddate = $rollcall['invaliddate'];
    $presentnotify = $rollcall['presentnotify'];
    $presentcomment = $rollcall['presentcomment'];
    $absentnotify = $rollcall['absentnotify'];
    $absentcomment = $rollcall['absentcomment'];
    $resend = $rollcall['resend'];
    $maingroups = $rollcall['maingroups'];
    $type = $rollcall['type'];
    $curriculumrecordid = $rollcall['curriculumrecordid'];

    if ($StartDate == null)
        $StartDate = '';
    if ($EndDate == null)
        $EndDate = '';
    if ($type == null)
        $type = 0;
    if ($curriculumrecordid == null)
        $curriculumrecordid = 0;

    if(is_array($weekday))
    {
        $weekday_str = implode(",", $weekday);
        $weekday = $weekday_str;
    }

    $validdate = '';
    if(is_array($rollcall['validdate']))
    {
        $validdate_str = implode(",", $rollcall['validdate']);
        $validdate = $validdate_str;
    }
    else
    {
        $validdate = $rollcall['validdate'];
    }

    $invaliddate = '';
    if(is_array($rollcall['invaliddate']))
    {
        $invaliddate_str = implode(",", $rollcall['invaliddate']);
        $invaliddate = $invaliddate_str;
    }
    else
    {
        $invaliddate = $rollcall['invaliddate'];
    }

    // echo '$validdate:['.$validdate.']';
    // echo '$invaliddate:['.$invaliddate.']';

    $db->query("UPDATE CARDRollCall SET rollcallname='$rollcallname', StartTime='$StartTime', EndTime='$EndTime', StartDate='$StartDate', EndDate='$EndDate', " .
               "rfiddeviceids='$rfiddeviceids', nfclocators='$nfclocators', subgroups='$subgroups', weekday='$weekday', " .
               "validdate='$validdate', invaliddate='$invaliddate', presentnotify=$presentnotify, presentcomment='$presentcomment', " .
               "absentnotify=$absentnotify, absentcomment='$absentcomment', resend=$resend, maingroups='$maingroups', type=$type, curriculumrecordid=$curriculumrecordid " .
               "WHERE rollcallid=$rollcallid");
}

function load_rollcall_ByCurriculumRecordID($project_id, $curriculumrecordid)
{
    $cmd = "SELECT * from CARDRollCall WHERE curriculumrecordid=$curriculumrecordid ";

    $rollcall = do_loadcardrollcall($project_id, $cmd);

    return $rollcall;
}

function loadcardrollcall($project_id, $rollcallid)
{
    $cmd = "SELECT * from CARDRollCall WHERE rollcallid=$rollcallid ";

    return do_loadcardrollcall($project_id, $cmd);
}

function do_loadcardrollcall($project_id, $cmd)
{
    //
    // check data group permssion
    //
    //$UserInfo = getUserInfoByAccount($project_id, $_SESSION["accountid"]);
    $UserInfo = loadsingleuser_ById($project_id, $_SESSION["userid"]);
    $CurUserDataGroupids = explode(",", $UserInfo['datagroups']);

    $db = new DB($project_id, 'base');

    $db->query($cmd);

    $rollcall = [];

    if($result = $db->fetch_array())
    {
        $rollcall['rollcallid'] = $result['rollcallid'];
        $rollcall['rollcallname'] = $result['rollcallname'];
        $rollcall['StartTime'] = $result['StartTime'];
        $rollcall['EndTime'] = $result['EndTime'];
        $rollcall['StartDate'] = $result['StartDate'];
        $rollcall['EndDate'] = $result['EndDate'];
        $rollcall['rfiddeviceids'] = $result['rfiddeviceids'];
        $rollcall['nfclocators'] = $result['nfclocators'];
        $rollcall['subgroups'] = $result['subgroups'];
        $rollcall['presentnotify'] = $result['presentnotify'];
        $rollcall['presentcomment'] = $result['presentcomment'];
        $rollcall['absentnotify'] = $result['absentnotify'];
        $rollcall['absentcomment'] = $result['absentcomment'];
        $rollcall['resend'] = $result['resend'];
        $rollcall['maingroups'] = $result['maingroups'];
        $rollcall['type'] = $result['type'];
        $rollcall['curriculumrecordid'] = $result['curriculumrecordid'];

        $rollcall['datetime'] = $result['datetime'];
        $rollcall['updatetime'] = $result['updatetime'];
        $rollcall['weekday'] = [];
        $rollcall['validdate'] = [];
        $rollcall['invaliddate'] = [];
        if ($result['weekday'] != null)
        {
            $rollcall['weekday'] = explode(",", $result['weekday']);
        }
        if ($result['validdate'] != null)
        {
            $rollcall['validdate'] = explode(",", $result['validdate']);
        }
        if ($result['invaliddate'] != null)
        {
            $rollcall['invaliddate'] = explode(",", $result['invaliddate']);
        }

        $subgroups = explode(",", $rollcall['subgroups']);

        $found = check_user_datagroup_permssion($CurUserDataGroupids, $subgroups);

        if ( !$found )
        {
            $rollcall = [];
        }
    }
    return $rollcall;
}

function loadcardrollcalls($project_id, $queryoffset, $querycount)
{
    //
    // check data group permssion
    //
    //$UserInfo = getUserInfoByAccount($project_id, $_SESSION["accountid"]);
    $UserInfo = loadsingleuser_ById($project_id, $_SESSION["userid"]);
    $CurUserDataGroupids = explode(",", $UserInfo['datagroups']);
    $CurUserSubGroupids = explode(",", $UserInfo['subgroups']);

    $rollcalls = [];

    $db = new DB($project_id, 'base');

    //==========================
    $db->query("SELECT * from CARDRollCall ORDER BY datetime ASC ");
    //==========================
    // if ( $querycount == 0 )
    //     $db->query("SELECT * from CARDRollCall ORDER BY datetime ASC ");
    // else
    //     $db->query("SELECT * from CARDRollCall ORDER BY datetime ASC LIMIT $queryoffset, $querycount ");
    //==========================

    $count = 0;
    while($result = $db->fetch_array())
    {
        $subgroups = explode(",", $result['subgroups']);

        $found = check_user_datagroup_permssion($CurUserSubGroupids, $subgroups);
        if (!$found)
        {
            $found = check_user_datagroup_permssion($CurUserDataGroupids, $subgroups);
        }

        if ( $found )
        {
            if ( $querycount == 0 )
            {
                // get all
            }
            else
            if ( $count < $queryoffset )
            {
                // skip this data
                $count = $count + 1;
                continue;
            }

            $rollcall = [];
            $rollcall['rollcallid'] = $result['rollcallid'];
            $rollcall['rollcallname'] = $result['rollcallname'];
            $rollcall['StartTime'] = $result['StartTime'];
            $rollcall['EndTime'] = $result['EndTime'];
            $rollcall['StartDate'] = $result['StartDate'];
            $rollcall['EndDate'] = $result['EndDate'];
            $rollcall['rfiddeviceids'] = $result['rfiddeviceids'];
            $rollcall['nfclocators'] = $result['nfclocators'];
            $rollcall['subgroups'] = $result['subgroups'];
            $rollcall['presentnotify'] = $result['presentnotify'];
            $rollcall['presentcomment'] = $result['presentcomment'];
            $rollcall['absentnotify'] = $result['absentnotify'];
            $rollcall['absentcomment'] = $result['absentcomment'];
            $rollcall['resend'] = $result['resend'];
            $rollcall['maingroups'] = $result['maingroups'];
            $rollcall['type'] = $result['type'];
            $rollcall['curriculumrecordid'] = $result['curriculumrecordid'];

            $rollcall['datetime'] = $result['datetime'];
            $rollcall['updatetime'] = $result['updatetime'];
            $rollcall['weekday'] = [];
            $rollcall['validdate'] = [];
            $rollcall['invaliddate'] = [];
            if ($result['weekday'] != null)
            {
                $rollcall['weekday'] = explode(",", $result['weekday']);
            }
            if ($result['validdate'] != null)
            {
                $rollcall['validdate'] = explode(",", $result['validdate']);
            }
            if ($result['invaliddate'] != null)
            {
                $rollcall['invaliddate'] = explode(",", $result['invaliddate']);
            }

            $rollcalls[$count] = $rollcall;

            $count = $count + 1;

            if ( $querycount > 0 )
            {
                if ( $count >= ($queryoffset + $querycount) )
                {
                    break;
                }
            }
        }
    }

    return $rollcalls;
}

function loadcardrollcallrecord_singleevent($project_id, $rollcallid, $eventid)
{
    $rollcallrecords = [];

    $rollcalls = loadcardrollcalls($project_id, 0, 0);

    $found = check_user_rollcall_permssion($rollcalls, $rollcallid);

    if (!$found)
    {
        return $rollcallrecords;
    }

    $db = new DB($project_id, 'base');

    $db->query("SELECT * from CARDRollCallRecord WHERE rollcallid=$rollcallid AND rollcalleventid=$eventid ");
    $count = 0;
    while($result = $db->fetch_array())
    {
        $rollcallrecord = [];
        $rollcallrecord['rollcallid'] = $result['rollcallid'];
        $rollcallrecord['rollcalleventid'] = $result['rollcalleventid'];
        $rollcallrecord['cardid'] = $result['cardid'];
        $rollcallrecord['userid'] = $result['userid'];
        $rollcallrecord['nodeid'] = $result['nodeid'];
        //$rollcallrecord['rfiddeviceid'] = $result['rfiddeviceid'];
        //$rollcallrecord['nfclocator'] = $result['nfclocator'];
        $rollcallrecord['rfiddeviceids'] = $result['rfiddeviceids'];
        $rollcallrecord['nfclocators'] = $result['nfclocators'];
        $rollcallrecord['present'] = $result['present'];
        $rollcallrecord['datetime'] = $result['datetime'];
        $rollcallrecord['updatetime'] = $result['updatetime'];
        $rollcallrecord['maingroups'] = $result['maingroups'];

        //$rfiddeviceids = $result['rfiddeviceids'];
        //$nfclocators = $result['nfclocators'];
        //$rollcallrecord['rfiddeviceids'] = explode(",", $rfiddeviceids);
        //$rollcallrecord['nfclocators'] = explode(",", $nfclocators);
        //if ( $rollcallrecord['nfclocator'] == null )
        //{
        //    $rollcallrecord['nfclocator'] = '';
        //}

        $rollcallrecords[$count] = $rollcallrecord;
        $count = $count + 1;
    }
    return $rollcallrecords;
}

function loadcardrollcallrecord($project_id, $queryoffset, $querycount)
{
    $rollcallrecords = [];

    $rollcalls = loadcardrollcalls($project_id, 0, 0);

    //
    // check data group permssion
    //
    //$UserInfo = getUserInfoByAccount($project_id, $_SESSION["accountid"]);
    $UserInfo = loadsingleuser_ById($project_id, $_SESSION["userid"]);
    $userid = (int)$UserInfo['userid'];
    $groupid = (int)$UserInfo['groupid'];
    $readOwnOnly = false;
    if ( $groupid == 3 )
    {
        $readOwnOnly = true;
    }

    //echo 'userid:'.$userid.'   groupid:'.$groupid.'   readOwnOnly:'.$readOwnOnly;

    $db = new DB($project_id, 'base');

    //=================================
    $db->query("SELECT * from CARDRollCallRecord ORDER BY datetime DESC ");
    //=================================
    // if ( $querycount == 0 )
    //     $db->query("SELECT * from CARDRollCallRecord ORDER BY datetime DESC ");
    // else
    //     $db->query("SELECT * from CARDRollCallRecord ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");
    //=================================

    $count = 0;
    while($result = $db->fetch_array())
    {
        $found = check_user_rollcall_permssion($rollcalls, $result['rollcallid']);

        if ( $found )
        {
            if ( $querycount == 0 )
            {
                // get all
            }
            else
            if ( $count < $queryoffset )
            {
                // skip this data
                $count = $count + 1;
                continue;
            }

            if ($readOwnOnly)
            {
                if ( $userid != (int)$result['userid'] )
                {
                    continue;
                }
            }

            $rollcallrecord = [];
            $rollcallrecord['rollcallid'] = $result['rollcallid'];
            $rollcallrecord['rollcalleventid'] = $result['rollcalleventid'];
            $rollcallrecord['cardid'] = $result['cardid'];
            $rollcallrecord['userid'] = $result['userid'];
            $rollcallrecord['nodeid'] = $result['nodeid'];
            //$rollcallrecord['rfiddeviceid'] = $result['rfiddeviceid'];
            //$rollcallrecord['nfclocator'] = $result['nfclocator'];
            $rollcallrecord['rfiddeviceids'] = $result['rfiddeviceids'];
            $rollcallrecord['nfclocators'] = $result['nfclocators'];
            $rollcallrecord['present'] = $result['present'];
            $rollcallrecord['datetime'] = $result['datetime'];
            $rollcallrecord['updatetime'] = $result['updatetime'];
            $rollcallrecord['maingroups'] = $result['maingroups'];

            //$rollcallrecord['rfiddeviceids'] = explode(",", $rfiddeviceids);
            //$rollcallrecord['nfclocators'] = explode(",", $nfclocators);

            // if ( $rollcallrecord['nfclocator'] == null )
            // {
            //     $rollcallrecord['nfclocator'] = '';
            // }

            $rollcallrecords[$count] = $rollcallrecord;

            $count = $count + 1;
            if ( $querycount > 0 )
            {
                if ( $count >= ($queryoffset + $querycount) )
                {
                    break;
                }
            }
        }
    }

    return $rollcallrecords;
}

function loadcardrollcallevent($project_id, $eventid)
{
    $rollcallevent = [];

    $rollcalls = loadcardrollcalls($project_id, 0, 0);

    $db = new DB($project_id, 'base');

    $db->query("SELECT * from CARDRollCallEvent WHERE eventid=$eventid ");

    if($result = $db->fetch_array())
    {
        $found = check_user_rollcall_permssion($rollcalls, $result['rollcallid']);

        if ($found)
        {
            $rollcallevent['eventid'] = $result['eventid'];
            $rollcallevent['rollcallid'] = $result['rollcallid'];
            $rollcallevent['rollcallname'] = $result['rollcallname'];
            $rollcallevent['StartDateTime'] = $result['StartDateTime'];
            $rollcallevent['EndDateTime'] = $result['EndDateTime'];
            $rollcallevent['totalcount'] = $result['totalcount'];
            $rollcallevent['presentcount'] = $result['presentcount'];
            $rollcallevent['datetime'] = $result['datetime'];
            $rollcallevent['updatetime'] = $result['updatetime'];
            $rollcallrecord['maingroups'] = $result['maingroups'];
        }
    }
    return $rollcallevent;
}

function loadcardrollcallevents($project_id, $queryoffset, $querycount)
{
    $rollcallevents = [];

    // maingroups
    $maingroupList = explode(",", $_SESSION['maingroups']);

    $userid = $_SESSION['userid'];
    $loadAll = check_user_permisson($project_id, $userid);

    $db = new DB($project_id, 'base');

    if ( $querycount == 0 )
        $db->query("SELECT * from CARDRollCallEvent ORDER BY datetime DESC ");
    else
        $db->query("SELECT * from CARDRollCallEvent ORDER BY datetime DESC LIMIT $queryoffset, $querycount ");

    $count = 0;
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

        $rollcallevent = [];
        $rollcallevent['eventid'] = $result['eventid'];
        $rollcallevent['rollcallid'] = $result['rollcallid'];
        $rollcallevent['rollcallname'] = $result['rollcallname'];
        $rollcallevent['StartDateTime'] = $result['StartDateTime'];
        $rollcallevent['EndDateTime'] = $result['EndDateTime'];
        $rollcallevent['totalcount'] = $result['totalcount'];
        $rollcallevent['presentcount'] = $result['presentcount'];
        $rollcallevent['datetime'] = $result['datetime'];
        $rollcallevent['updatetime'] = $result['updatetime'];
        $rollcallrecord['maingroups'] = $result['maingroups'];
        $rollcallevents[$count] = $rollcallevent;
        $count = $count + 1;
    }

    return $rollcallevents;
}

function loadcardrollcallsubgroupevent($project_id, $eventids)
{
    $rollcallsubgroupevent = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * from CARDRollCallSubGroupEvent WHERE eventid in ($eventids) ");

    while($result = $db->fetch_array())
    {
        $eventid = $result['eventid'];

        if ( $rollcallsubgroupevent[$eventid] == null)
        {
            $rollcallsubgroupevent[$eventid] = [];
        }
        $count = count($rollcallsubgroupevent[$eventid]);

        $subgroupevent = [];
        $subgroupevent['eventid'] = $result['eventid'];
        $subgroupevent['rollcallid'] = $result['rollcallid'];
        $subgroupevent['subgroupid'] = $result['subgroupid'];
        $subgroupevent['rollcallname'] = $result['rollcallname'];
        $subgroupevent['StartDateTime'] = $result['StartDateTime'];
        $subgroupevent['EndDateTime'] = $result['EndDateTime'];
        $subgroupevent['totalcount'] = $result['totalcount'];
        $subgroupevent['presentcount'] = $result['presentcount'];
        $subgroupevent['datetime'] = $result['datetime'];
        $subgroupevent['updatetime'] = $result['updatetime'];
        $subgroupevent['maingroups'] = $result['maingroups'];

        $rollcallsubgroupevent[$eventid][$count] = $subgroupevent;

    }
    return $rollcallsubgroupevent;
}

?>
