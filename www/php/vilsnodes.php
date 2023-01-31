<?php

session_start();

require_once("common.php");
//require_once("User.php");
require_once("configvils.php");

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
        VILSNODES_do_GET();
    break;
    case 'PUT':
        VILSNODES_do_PUT();
    break;
    case 'POST':
        VILSNODES_do_POST();
    break;
    case 'DELETE':
        VILSNODES_do_DELETE();
    break;
}

function VILSNODES_do_GET()
{
    if (isset($_GET['loadranging']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];

        $json_string = json_encode($json_array);

        $status_str = getServerInfo('loadranging='.$json_string);
        $status = packageJSON($status_str);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadcardscan']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];

        $json_string = json_encode($json_array);

        $status_str = getServerInfo('loadcardscan='.$json_string);
        $status = packageJSON($status_str);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadtag']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            $json_array['start'] = $_GET['s'];
            $json_array['count'] = $_GET['count'];
        }

        if (isset($_GET['nodeid']))
        {
            $json_array['nodeid'] = $_GET["nodeid"];
        }

        $json_string = json_encode($json_array);
        $status_str = getServerInfo('tagstatus='.$json_string);
        $status = packageJSON($status_str);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadtagrollcall']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];

        if (isset($_GET['s']) && isset($_GET['count']) && isset($_GET['areaid']))
        {
            //$status_str = getServerInfo('tagrollcallstatus='. $_GET['s'] . ',' . $_GET['count']. ',' . $_GET['areaid']);
            $json_array['start'] = $_GET['s'];
            $json_array['count'] = $_GET['count'];
            $json_array['areaid'] = $_GET['areaid'];
        }
        else
        {
            //$status_str = getServerInfo('tagrollcallstatus');
        }
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('tagrollcallstatus='.$json_string);
        $status = packageJSON($status_str);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadanchor']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            //$status_str = getServerInfo('anchorstatus='. $_GET['s'] . ',' . $_GET['count']);
            $json_array['start'] = $_GET['s'];
            $json_array['count'] = $_GET['count'];
        }
        else
        {
            //$status_str = getServerInfo('anchorstatus');
        }
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('anchorstatus='.$json_string);
        $status = packageJSON($status_str);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadcoord']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            // $status_str = getServerInfo('coordstatus='. $_GET['s'] . ',' . $_GET['count']);
            $json_array['start'] = $_GET['s'];
            $json_array['count'] = $_GET['count'];
        }
        else
        {
            // $status_str = getServerInfo('coordstatus');
        }
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('coordstatus='.$json_string);
        $status = packageJSON($status_str);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadlocator']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            // $status_str = getServerInfo('locatorstatus='. $_GET['s'] . ',' . $_GET['count']);
            $json_array['start'] = $_GET['s'];
            $json_array['count'] = $_GET['count'];
        }

        if (isset($_GET['nodeid']))
        {
            $json_array['nodeid'] = $_GET["nodeid"];
        }

        $json_string = json_encode($json_array);
        $status_str = getServerInfo('locatorstatus='.$json_string);
        $status = packageJSON($status_str);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadaoa']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            // $status_str = getServerInfo('locatorstatus='. $_GET['s'] . ',' . $_GET['count']);
            $json_array['start'] = $_GET['s'];
            $json_array['count'] = $_GET['count'];
        }
        else
        {
            //$status_str = getServerInfo('locatorstatus');
        }
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('aoastatus='.$json_string);
        $status = packageJSON($status_str);
        echo json_encode($status);
    }
    else
    if (isset($_GET['devicescount']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];

        $json_string = json_encode($json_array);
        $status_str = getServerInfo('devicescount='.$json_string);

        $status = packageJSON($status_str);
        echo json_encode($status);
    }
    else
    if (isset($_GET['loadloragateway']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];

        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadloragateway='.$json_string);
        //$status_str = getServerInfo('loadloragateway');
        echo $status_str;
    }
    // else
    // if (isset($_GET['residentinfo']))
    // {
    //     $project_id = 1;
    //     if (isset($_GET['project_id']))
    //         $project_id = (int)$_GET['project_id'];
    //
    //     $residents = getResidentInfo($project_id);
    //     echo json_encode($residents);
    // }
    // else
    // if (isset($_GET['employeeinfo']))
    // {
    //     $project_id = 1;
    //     if (isset($_GET['project_id']))
    //         $project_id = (int)$_GET['project_id'];
    //
    //     $Employees = getEmployeeInfo($project_id);
    //     echo json_encode($Employees);
    // }
    else
    if (isset($_GET['userinfo']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $start = 0;
        $count = 0;

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            // $status_str = getServerInfo('locatorstatus='. $_GET['s'] . ',' . $_GET['count']);
            $start = $_GET['s'];
            $count = $_GET['count'];
        }

        $Users = getUserInfo($project_id, $start, $count);
        echo json_encode($Users);
    }
    else
    if (isset($_GET['loadtagbodyinfo']))
    {
        $project_id = 1;
        $targetid = '';
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];
        if (isset($_GET['targetid']))
            $targetid = $_GET['targetid'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['targetid'] = $targetid;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadtagbodyinfo='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadtagbodyinforecords']))
    {
        $project_id = 1;
        $targetid = '';
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];
        if (isset($_GET['targetid']))
            $targetid = $_GET['targetid'];

        $StartDate = '';
        if (isset($_GET['StartDate']))
            $StartDate = $_GET['StartDate'];
        $EndDate = '';
        if (isset($_GET['EndDate']))
            $EndDate = $_GET['EndDate'];

        if( (strlen($StartDate) > 0) && (strlen($EndDate) > 0) )
        {
            $status = get_tagbodyinfo_bydate($project_id, $targetid, $StartDate, $EndDate);
            echo json_encode($status);
        }
        else
        {
            $status = get_tagbodyinfo($project_id, $targetid, 10);
            echo json_encode($status);
        }


        // $json_array = [];
        // $json_array['project_id'] = $project_id;
        // $json_array['targetid'] = $targetid;
        // $json_string = json_encode($json_array);
        // $status_str = getServerInfo('loadtagbodyinforecords='.$json_string);
        // echo $status_str;
    }
    else
    if (isset($_GET['loaduserbodyinforecords']))
    {
        $project_id = 1;
        $accountid = '';
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];
        if (isset($_GET['accountid']))
            $accountid = $_GET['accountid'];

        $StartDate = '';
        if (isset($_GET['StartDate']))
            $StartDate = $_GET['StartDate'];
        $EndDate = '';
        if (isset($_GET['EndDate']))
            $EndDate = $_GET['EndDate'];

        if( (strlen($StartDate) > 0) && (strlen($EndDate) > 0) )
        {
            $status = get_userbodyinfo_bydate($project_id, $accountid, $StartDate, $EndDate);
            echo json_encode($status);
        }
        else
        {
            echo '[]';
        }
    }
    else
    if (isset($_GET['loaduserbodybaseinforecords']))
    {
        $project_id = 1;
        $accountid = '';
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];
        if (isset($_GET['accountid']))
            $accountid = $_GET['accountid'];

        $StartDate = '';
        if (isset($_GET['StartDate']))
            $StartDate = $_GET['StartDate'];
        $EndDate = '';
        if (isset($_GET['EndDate']))
            $EndDate = $_GET['EndDate'];

        if( (strlen($StartDate) > 0) && (strlen($EndDate) > 0) )
        {
            $status = get_userbodybaseinfo_bydate($project_id, $accountid, $StartDate, $EndDate);
            echo json_encode($status);
        }
        else
        {
            echo '[]';
        }
    }
    else
    if (isset($_GET['loadabbox']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadabbox='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadlocatorbodyinfo']))
    {
        $project_id = 1;
        $targetid = '';
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];
        if (isset($_GET['targetid']))
            $targetid = $_GET['targetid'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['targetid'] = $targetid;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadlocatorbodyinfo='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadtagprjinfo']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadtagprjinfo='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadanchorprjinfo']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadanchorprjinfo='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadcoordprjinfo']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadcoordprjinfo='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadlocatorprjinfo']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];
        $json_string = json_encode($json_array);
        // writeLogFile([$json_string],'logfiletest.txt');
        $status_str = getServerInfo('loadlocatorprjinfo='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadgatewayprjinfo']))
    {
        $json_array = [];
        $json_array['project_id'] = $_GET['project_id'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadgatewayprjinfo='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadaoarangingcloudpoint']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadaoarangingcloudpoint='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadaoaanglevectorcloudpoint']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadaoaanglevectorcloudpoint='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadaoaanglecloudpoint']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadaoaanglecloudpoint='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadforacaredata']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadforacaredata='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadunknownpositiontag']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadunknownpositiontag='.$json_string);
        echo $status_str;
    }
    else
    if (isset($_GET['loadmaintenancerecord']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $start = 0;
        $count = 0;

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            $start = (int)$_GET['s'];
            $count = (int)$_GET['count'];
        }
        $return_str = loadmaintenancerecord($project_id, $start, $count);
        echo json_encode($return_str);
    }
    // else
    // if (isset($_GET['usergroupcount']))
    // {
    //     $project_id = 1;
    //     if (isset($_GET['project_id']))
    //         $project_id = (int)$_GET['project_id'];
    //
    //     $count = usergroupcount($project_id);
    //
    //     $json_array = [];
    //     $json_array['USERGROUP_NUM'] = $count;
    //     echo json_encode($json_array);
    // }
    else
    if (isset($_GET['loadusergroup']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $start = 0;
        $count = 0;

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            $start = (int)$_GET['s'];
            $count = (int)$_GET['count'];
        }
        $return_str = loadusergroup($project_id, $start, $count);
        echo json_encode($return_str);
    }
    // else
    // if (isset($_GET['usersubgroupcount']))
    // {
    //     $project_id = 1;
    //     if (isset($_GET['project_id']))
    //         $project_id = (int)$_GET['project_id'];
    //
    //     $count = usersubgroupcount($project_id);
    //
    //     $json_array = [];
    //     $json_array['USERSUBGROUP_NUM'] = $count;
    //     echo json_encode($json_array);
    // }
    else
    if (isset($_GET['loadusersubgroup']))
    {
        $project_id = 1;
        $type = '';

        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        if (isset($_GET['type']))
            $type = $_GET['type'];

        $session_userid = $_SESSION["userid"];
        $session_groupid = (int)$_SESSION["groupid"];

        if ($session_groupid == 3)
        {
            $return_str = loaddatagroup($project_id, $session_userid, $type);
            echo json_encode($return_str);
        }
        else
        {
            $return_str = loadsubgroup($project_id, $session_userid, $type);
            echo json_encode($return_str);
        }
    }
    else
    if (isset($_GET['loadsubgroup']))
    {
        $project_id = 1;

        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $session_accountid = $_SESSION["accountid"];
        $session_groupid = (int)$_SESSION["groupid"];
        $session_userid = $_SESSION["userid"];

        $return_str = load_view_access_subgroupids_ById($project_id, $session_userid);
        //$return_str = load_view_access_subgroup($project_id, $session_accountid);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadmaingroup']))
    {
        $project_id = 1;

        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $session_accountid = $_SESSION["accountid"];
        $session_userid = $_SESSION["userid"];
        $session_groupid = (int)$_SESSION["groupid"];

        if ( $session_groupid != 1 && $session_groupid != 2 && $session_groupid != 6)
        {
            echo '[]';
        }
        else
        {
            $return_str = load_maingroup($project_id, $session_accountid, $session_userid, $session_groupid);
            echo json_encode($return_str);
        }
    }
    else
    if (isset($_GET['loadallmaingroup']))
    {
        $project_id = 1;

        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $return_str = load_allmaingroupname($project_id);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadmaingroupname']))
    {
        $project_id = 1;
        $maingroupids = '';

        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];
        if (isset($_GET['maingroupids']))
            $maingroupids = $_GET['maingroupids'];

        $return_str = load_maingroupname($project_id, $maingroupids);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loaddatasubgroup']))
    {
        $project_id = 1;
        $type = '';

        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        if (isset($_GET['type']))
            $type = $_GET['type'];

        $session_userid = $_SESSION["userid"];
        $session_groupid = $_SESSION["groupid"];

        $return_str = loadsubgroup($project_id, $session_userid, $type);
        echo json_encode($return_str);

    }
    else
    if (isset($_GET['usercount']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        // $session_accountid = $_SESSION["accountid"];
        $session_userid = $_SESSION["userid"];
        $session_maingroups = $_SESSION["maingroups"];
        $count = getSubGroupUserCount_ById($project_id, $session_userid, $session_maingroups);

        $json_array = [];
        $json_array['USER_NUM'] = $count;
        echo json_encode($json_array);
    }
    else
    if (isset($_GET['loaduser']))
    {

        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $subgroup = '';
        if (isset($_GET['subgroup']))
            $subgroup = $_GET['subgroup'];

        $start = 0;
        $count = 0;

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            $start = (int)$_GET['s'];
            $count = (int)$_GET['count'];
        }

        if (isset($_GET['accountid']))
        {
            $query_accountid = $_GET["accountid"];
            $session_accountid = $_SESSION["accountid"];
            if ( strcmp($query_accountid,  $session_accountid) == 0 )
            {
                $return_str = loadsingleuser($project_id, $query_accountid);
                echo json_encode($return_str);
            }
            else
            {
                echo '[]';
            }
        }
        else
        {
            $session_accountid = $_SESSION["accountid"];
            $session_userid = $_SESSION["userid"];
            $session_maingroups = $_SESSION["maingroups"];
            $return_str = loaduser($project_id, $session_userid, $start, $count, $subgroup, $session_maingroups);
            echo json_encode($return_str);
        }
    }
    else
    if (isset($_GET['loaduserbaseinfo']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $start = 0;
        $count = 0;
        $userid = '';

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            $start = (int)$_GET['s'];
            $count = (int)$_GET['count'];
        }

        if (isset($_GET['userid']))
        {
            $userid = $_GET['userid'];
        }

        $session_accountid = $_SESSION["accountid"];
        $session_userid = $_SESSION["userid"];
        $session_maingroups = $_SESSION["maingroups"];
        $return_str = loaduserbaseinfo($project_id, $session_userid, $start, $count, $session_maingroups, $userid);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loaduserpoistioninfo']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $start = 0;
        $count = 0;

        if (isset($_GET['s']) && isset($_GET['count']))
        {
            $start = (int)$_GET['s'];
            $count = (int)$_GET['count'];
        }

        $session_accountid = $_SESSION["accountid"];
        $session_maingroups = $_SESSION["maingroups"];
        $return_str = loaduserpoistioninfo($project_id, $session_accountid, $start, $count, $session_maingroups);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['queryusers']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $session_maingroups = $_SESSION["maingroups"];

        if( isset($_GET['userid']) )
        {
            $userid = $_GET['userid'];

            if (strlen($userid) == 0)
                echo '[]';
            else
            {
                $status = queryUsers($project_id, $userid, $session_maingroups);
                echo json_encode($status);
            }
        }
        else
        if( isset($_GET['account']) )
        {
            $account = $_GET['account'];
            if (strlen($account) == 0)
                echo '[]';
            else
            {
                $status = queryUsersByAccount($project_id, $account);
                echo json_encode($status);
            }
        }
        else
        {
            echo '[]';
        }
    }
    else
    if (isset($_GET['loadusername']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        if (isset($_GET['account']))
        {
            $query_account = $_GET["account"];
            $accounts = explode(",", $query_account);
            $return_str = loadusername($project_id, $accounts);
            echo json_encode($return_str);
        }
        else
        if (isset($_GET['user']))
        {
            $query_user = $_GET["user"];
            $userids = explode(",", $query_user);
            $return_str = loadusername_byuserids($project_id, $userids);
            echo json_encode($return_str);
        }

    }
    else
    if (isset($_GET['subgroupusercount']))
    {
        $project_id = 1;
        $accountid = '';

        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        if (isset($_GET['accountid']))
            $accountid = $_GET['accountid'];

        $count = 0;
        if (strlen($accountid) > 0)
        {
            $session_maingroups = $_SESSION["maingroups"];
            $count = getSubGroupUserCount($project_id, $accountid, $session_maingroups);
        }

        $json_array = [];
        $json_array['USER_NUM'] = $count;
        echo json_encode($json_array);
    }
    else
    if (isset($_GET['loadsubgroupuser']))
    {
        $project_id = 1;
        $accountid = '';

        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        if (isset($_GET['accountid']))
            $accountid = $_GET['accountid'];

        $session_accountid = $_SESSION["accountid"];
        $session_maingroups = $_SESSION["maingroups"];

        if (strlen($session_accountid) > 0)
        {
            $return_str = loadsubgroupuser($project_id, $session_accountid, $session_maingroups);
            echo json_encode($return_str);
        }
        else
        {
            echo '[]';
        }
    }
    else
    if (isset($_GET['loadweekschedule']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $subgroupids = $_GET['subgroupids'];

        $subgroupidList = explode(',', $subgroupids);

        $cnt = 0;
        $json_array = [];
        foreach($subgroupidList as $subgroupid)
        {
            $schedule = load_schedule($project_id, (int)$subgroupid);
            if ( $schedule != null )
            {
                $json_array[$cnt] = $schedule;
                $cnt++;
            }
        }
        $json_string = json_encode($json_array);
        echo $json_string;
    }
    else
    if (isset($_GET['loadedgecontrol']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $count = 0;
        $offset = 0;
        $showonline = 1;
        $showoffline = 1;

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['s']))
        {
            $offset = (int)$_GET['s'];
        }
        if (isset($_GET['showonline']))
        {
            $showonline = (int)$_GET['showonline'];
        }
        if (isset($_GET['showoffline']))
        {
            $showoffline = (int)$_GET['showoffline'];
        }

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['start'] = $offset;
        $json_array['count'] = $count;
        $json_array['showonline'] = $showonline;
        $json_array['showoffline'] = $showoffline;
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];
        $json_string = json_encode($json_array);
        $status = getServerInfo('edgedevicetatus='.$json_string);
        echo $status;
    }
    else
    if (isset($_GET['loadsensorinfo']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $return_str = loadsensorinfo($project_id);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadsensordata']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $names = '';
        if (isset($_GET['names']))
        {
            $names = $_GET['names'];
        }


        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['names'] = $names;
        $json_string = json_encode($json_array);
        $status = getServerInfo('sensordevicedata='.$json_string);
        echo $status;
    }
    else
    if (isset($_GET['loadsensorrawdata']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $names = '';
        $day = '';
        $count = 0;
        $offset = 0;
        if (isset($_GET['names']))
            $names = $_GET['names'];
        if (isset($_GET['day']))
            $day = $_GET['day'];
        if (isset($_GET['count']))
            $count = (int)$_GET['count'];
        if (isset($_GET['s']))
            $offset = (int)$_GET['s'];

        $sensordatas = loadsensorrawdata($project_id, $names, $day, $offset, $count);

        //
        // convert to csv
        //
        foreach( $sensordatas as $sensordata )
        {
            echo $sensordata['datetime'];
            echo ',';
            echo $sensordata['value'];
            echo '<br>';
        }
    }
    else
    if (isset($_GET['loadsensordevice']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $count = 0;
        $offset = 0;
        $showonline = 1;
        $showoffline = 1;
        $nodeid = '';

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['s']))
        {
            $offset = (int)$_GET['s'];
        }
        if (isset($_GET['showonline']))
        {
            $showonline = (int)$_GET['showonline'];
        }
        if (isset($_GET['showoffline']))
        {
            $showoffline = (int)$_GET['showoffline'];
        }
        if (isset($_GET['nodeid']))
        {
            $nodeid = $_GET['nodeid'];
        }

        $session_maingroups = $_SESSION["maingroups"];
        $session_groupid = $_SESSION["groupid"];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['start'] = $offset;
        $json_array['count'] = $count;
        $json_array['showonline'] = $showonline;
        $json_array['showoffline'] = $showoffline;
        $json_array['groupid'] = $session_groupid;
        $json_array['maingroups'] = $session_maingroups;
        $json_array['nodeid'] = $nodeid;
        $json_string = json_encode($json_array);
        $status = getServerInfo('sensordevicetatus='.$json_string);
        writeLogFile([$status], 'logfiletest.txt');
        echo $status;
    }
    else
    if (isset($_GET['loadsensordevicebaseinfo']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $session_maingroups = $_SESSION["maingroups"];
        $session_groupid = $_SESSION["groupid"];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['groupid'] = $session_groupid;
        $json_array['maingroups'] = $session_maingroups;
        $json_string = json_encode($json_array);
        $status = getServerInfo('sensordevicebaseinfo='.$json_string);
        echo $status;
    }
    else
    if (isset($_GET['loaduserregister']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $count = 0;
        $offset = 0;

        if (isset($_GET['count']))
        {
            $count = (int)$_GET['count'];
        }
        if (isset($_GET['s']))
        {
            $offset = (int)$_GET['s'];
        }

        $return_str = loaduserregisterdata($project_id, $offset, $count);
        echo json_encode($return_str);

    }
    else
    if (isset($_GET['get_air_remain']))
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['areaid'] = $_GET['areaid'];
        $json_string = json_encode($json_array);
        $status = getServerInfo('get_air_remain='.$json_string);
        echo $status;
    }
    else
    {
        $project_id = 1;
        if (isset($_GET['project_id']))
            $project_id = (int)$_GET['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('status='.$json_string);
            //$status_str = getServerInfo('loadloragateway');
        # return device status
        // $status_str = getServerInfo('status');
        $status = packageJSON($status_str);
        echo json_encode($status);
    }
}

function VILSNODES_do_PUT()
{
}

function VILSNODES_do_POST()
{
    $json = file_get_contents('php://input');
    $obj = json_decode($json);

    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    if (isset($_POST['EDIT_MODE']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['macaddress'] = $_POST['MAC'];
        if (isset($_POST['NODEID']))
            $json_array['nodeid'] = $_POST['NODEID'];
        if (isset($_POST['TAFID']))
            $json_array['tafid'] = $_POST['TAFID'];
        if (isset($_POST['BUILDINGID']))
            $json_array['buildingid'] = $_POST['BUILDINGID'];
        if (isset($_POST['BUILDINGFLOOR']))
            $json_array['buildingfloor'] = $_POST['BUILDINGFLOOR'];
        if (isset($_POST['NODENAME']))
            $json_array['nodename'] = $_POST['NODENAME'];
        if (isset($_POST['POSX']))
            $json_array['posX'] = $_POST['POSX'];
        if (isset($_POST['POSY']))
            $json_array['posY'] = $_POST['POSY'];
        if (isset($_POST['POSZ']))
            $json_array['posZ'] = $_POST['POSZ'];
        if (isset($_POST['LORAMACADDRESS']))
            $json_array['LoRAMacaddress'] = $_POST['LORAMACADDRESS'];
        if (isset($_POST['maplayerid']))
            $json_array['maplayerid'] = $_POST['maplayerid'];

        $json_string = json_encode($json_array);

        $send_string = $_POST['TYPE'] . 'info=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['EDIT_NODE']))
    {
        $maplayerid = 1;
        if (isset($_GET['maplayerid']))
            $maplayerid = (int)$_GET['maplayerid'];

        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['maplayerid'] = $maplayerid;
        $json_array['type'] = $_POST['TYPE'];
        $json_array['nodeid'] = $_POST['NODEID'];
        $json_array['posX'] = $_POST['POSX'];
        $json_array['posY'] = $_POST['POSY'];
        $json_array['posZ'] = $_POST['POSZ'];
        $json_array['Latitude'] = $_POST['LAT'];
        $json_array['Longitude'] = $_POST['LNG'];

        $json_string = json_encode($json_array);

        $send_string = 'nodeinfo=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['DELETE_NODE']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['type'] = $_POST['TYPE'];
        $json_array['macaddress'] = $_POST['MAC'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];

        $json_string = json_encode($json_array);

        $send_string = 'deletenode=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['DELETE_GATEWAY']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['gatewayid'] = $_POST['ID'];
        $json_array['maingroups'] = $_SESSION["maingroups"];
        $json_array['groupid'] = $_SESSION["groupid"];

        $json_string = json_encode($json_array);

        $send_string = 'deletegateway=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['DELETE_ALL_OFFLINE_DEVICE']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['TYPE'] = $_POST['TYPE'];

        $json_string = json_encode($json_array);

        $send_string = 'deleteallofflinedevice=' . $json_string;
        sendMessageToServer($send_string);
    }
    // else
    // if (isset($_POST['UPLOAD_NODE']))
    // {
    //     $status_str = getServerInfo('uploadnodes');
    //     $status = packageJSON($status_str);
    //     echo "Upload finished!";
    // }
    else
    if (isset($_POST['DUMP_RANGING_RESULT']))
    {
        //$send_string = 'startdumprangingresult='. $_POST['DUMP_RANGING_RESULT'];
        //sendMessageToServer($send_string);
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['timeout'] = $_POST['DUMP_RANGING_RESULT'];

        $json_string = json_encode($json_array);

        $send_string = 'startdumprangingresult=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['dumplogtype']))
    {
        //$send_string = 'startdumprangingresult='. $_POST['DUMP_RANGING_RESULT'];
        //sendMessageToServer($send_string);
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['logtype'] = $_POST['dumplogtype'];

        $json_string = json_encode($json_array);

        $send_string = 'printsystemlog=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['UPDATE_BLACKLIST']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_array['list'] = $_POST['LIST'];

        $json_string = json_encode($json_array);

        $send_string = 'blacklist=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['UPDATE_WHITELIST']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_array['list'] = $_POST['LIST'];

        $json_string = json_encode($json_array);

        $send_string = 'whitelist=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['CLEAR_BLACKLIST']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_string = json_encode($json_array);

        $send_string = 'clearblacklist=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['CLEAR_WHITELIST']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_string = json_encode($json_array);

        $send_string = 'clearwhitelist=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['UPDATE_JOINRANGINGRANGE']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_array['joinRangingRange'] = $_POST['RANGE'];

        $json_string = json_encode($json_array);

        $send_string = 'joinRangingRange=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['UPDATE_JOINNOTRANGINGRANGE']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_array['joinNotRangingRange'] = $_POST['RANGE'];

        $json_string = json_encode($json_array);

        $send_string = 'joinNotRangingRange=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['UPDATE_NODE_RESIDENT']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_array['type'] = $_POST['TYPE'];
        $json_array['serno'] = $_POST['SERNO'];

        $json_string = json_encode($json_array);

        $send_string = 'nodeResident=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['UPDATE_NODE_EMPLOYEE']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_array['type'] = $_POST['TYPE'];
        $json_array['empid'] = $_POST['EMPID'];

        $json_string = json_encode($json_array);

        $send_string = 'nodeEmployee=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['UPDATE_LOCATOR_BUTTONTYPE']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_array['type'] = $_POST['TYPE'];
        $json_array['buttontype'] = $_POST['BUTTONTYPE'];

        $json_string = json_encode($json_array);

        $send_string = 'locatorButtontype=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['UPDATE_LOCATOR_SENSORALERT']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_array['type'] = $_POST['TYPE'];
        $json_array['sensoralert'] = $_POST['SENSORALERT'];

        $json_string = json_encode($json_array);

        $send_string = 'locatorSensorAlert=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['UPDATE_NODE_PEROJECT']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_array['type'] = $_POST['TYPE'];
        $json_array['target_projectid'] = $_POST['target_projectid'];

        $json_string = json_encode($json_array);

        $send_string = 'updatenodeprj=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['UPDATE_NODE_USER']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_array['type'] = $_POST['TYPE'];
        $json_array['userid'] = $_POST['userid'];

        $json_string = json_encode($json_array);

        $send_string = 'updatenodeuser=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['RESET_M4']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];

        $json_string = json_encode($json_array);

        $send_string = 'resetm4='. $json_string;
        sendMessageToServer($send_string);
        // $send_string = 'resetm4='. $_POST['MAC'];
        // sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['RESET_M0']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];

        $json_string = json_encode($json_array);

        $send_string = 'resetm0='. $json_string;
        sendMessageToServer($send_string);
        // $send_string = 'resetm0='. $_POST['MAC'];
        // sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['update_aoa_normal_vector']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['mac'] = $_POST['MAC'];
        $json_array['posX'] = $_POST['POSX'];
        $json_array['posY'] = $_POST['POSY'];
        $json_array['posZ'] = $_POST['POSZ'];
        $json_array['roll'] = $_POST['ROLL'];

        $json_string = json_encode($json_array);

        $send_string = 'aoanormalvector='. $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['SAVE_BODYINFO_WEIGHT']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['nodeid'] = $_POST['nodeid'];
        $json_array['weight'] = $_POST['weight'];
        $json_string = json_encode($json_array);

        $send_string = 'savebodyinfoweight='. $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['SAVE_BODYINFO_HEARTBEAT']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['nodeid'] = $_POST['nodeid'];
        $json_array['heartbeat'] = $_POST['heartbeat'];
        $json_string = json_encode($json_array);

        $send_string = 'savebodyinfoheartbeat='. $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['SAVE_BODYINFO_ALL']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['accountid'] = $_POST['accountid'];
        $json_array['recorderid'] = $_POST['recorderid'];
        $json_array['weight'] = $_POST['weight'];
        $json_array['heartbeat'] = $_POST['heartbeat'];
        $json_array['bloodsugar'] = $_POST['bloodsugar'];
        $json_array['systolicbloodpressure'] = $_POST['systolicbloodpressure'];
        $json_array['diastolicbloodpressure'] = $_POST['diastolicbloodpressure'];
        $json_array['bodytemperature'] = $_POST['bodytemperature'];
        $json_array['bodyfat'] = $_POST['bodyfat'];
        $json_array['bloodoxygen'] = $_POST['bloodoxygen'];
        $json_array['datetime'] = $_POST['datetime'];
        $json_string = json_encode($json_array);

        $send_string = 'savebodyinfoall='. $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['updateAoAPowerTagInfo']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['AoAPowerTag_1'] = $_POST['AoAPowerTag_1'];
        $json_array['AoAPowerTag_2'] = $_POST['AoAPowerTag_2'];
        $json_array['AoAPowerTag_3'] = $_POST['AoAPowerTag_3'];
        $json_array['AoAPowerTag_aoaid'] = $_POST['AoAPowerTag_aoaid'];
        $json_string = json_encode($json_array);

        $send_string = 'PowerTagCalculateInfo='. $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['fixposition']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['fixposition'] = $_POST['fixposition'];
        $json_array['updatenode'] = $_POST['updatenode'];
        $json_string = json_encode($json_array);

        $send_string = 'fixposition='. $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['fixdevice']))
    {
        $json_array = [];
        $json_array['project_id'] = $_POST['project_id'];
        $json_array['fixdevice'] = $_POST['fixdevice'];
        $json_array['updatenode'] = $_POST['updatenode'];
        $json_string = json_encode($json_array);

        $send_string = 'fixdevice='. $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (isset($_POST['create_new_maintenance']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['nodetype'] = 1;
        $record['nodeid'] = "";
        $record['name'] = $_POST['name'];
        $record['type'] = $_POST['type'];
        $record['unit'] = $_POST['unit'];
        $record['manager'] = $_POST['manager'];
        $record['assetlife'] = $_POST['assetlife'];
        $record['maintenancedate'] = $_POST['maintenancedate'];
        $record['status'] = $_POST['status'];

        save_new_maintenance($project_id, $record);

        $json_string = json_encode($record);
        echo $json_string;
    }
    else
    if (isset($_POST['create_new_user']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['name'] = $_POST['name'];
        $record['account'] = $_POST['account'];
        $record['unit'] = $_POST['unit'];
        $record['title'] = $_POST['title'];
        $record['gender'] = $_POST['gender'];
        $record['group'] = $_POST['group'];
        $record['datagroups'] = $_POST['datagroups'];
        $record['subgroups'] = $_POST['subgroups'];
        $record['rfidcards'] = $_POST['rfidcards'];
        $record['nfccards'] = $_POST['nfccards'];
        $record['uwbcards'] = $_POST['uwbcards'];
        $record['sensors'] = $_POST['sensors'];
        $record['imageuid'] = $_POST['imageuid'];
        $record['maingroups'] = $_SESSION['maingroups'];

        $maingroups = explode(",", $_SESSION['maingroups']);
        if (count($maingroups) > 0)
            $record['maingroupid'] = $maingroups[0];
        else
            $record['maingroupid'] = '1';

        list($count, $userid) = check_account_existed($project_id, $record['account']);

        // if (!$existed)
        // {
            list( $newuserid, $rfidcards, $assigned_rfidcards, $nfccards, $assigned_nfccards) = save_new_user($project_id, $record);
            $record['userid'] = $newuserid;
            $record['existed'] = ($count > 0);
            $record['rfidcards'] = $rfidcards;
            $record['nfccards'] = $nfccards;
            $record['assigned_rfidcards'] = $assigned_rfidcards;
            $record['assigned_nfccards'] = $assigned_nfccards;

            // $record['userid'] = save_new_user($project_id, $record);

            $json_string = json_encode($record);
            $send_string = 'create_new_user='. $json_string;
            sendMessageToServer($send_string);
            echo $json_string;
        // }
        // else
        // {
        //     $record['userid'] = -1;
        //     $json_string = json_encode($record);
        //     echo $json_string;
        // }
    }
    else
    if (isset($_POST['update_user']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['userid'] = $_POST['userid'];
        $record['name'] = $_POST['name'];
        //$record['account'] = $_POST['account'];
        //$record['unit'] = $_POST['unit'];
        $record['title'] = $_POST['title'];
        //$record['gender'] = $_POST['gender'];
        $record['group'] = $_POST['group'];
        $record['datagroups'] = $_POST['datagroups'];
        $record['subgroups'] = $_POST['subgroups'];
        $record['rfidcards'] = $_POST['rfidcards'];
        $record['nfccards'] = $_POST['nfccards'];
        $record['uwbcards'] = $_POST['uwbcards'];
        $record['sensors'] = $_POST['sensors'];
        $record['imageuid'] = $_POST['imageuid'];
        $record['maingroups'] = $_SESSION['maingroups'];

        list( $rfidcards, $assigned_rfidcards, $nfccards, $assigned_nfccards) = update_user($project_id, $record);
        $record['rfidcards'] = $rfidcards;
        $record['nfccards'] = $nfccards;
        $record['assigned_rfidcards'] = $assigned_rfidcards; // 重複註冊
        $record['assigned_nfccards'] = $assigned_nfccards; // 重複註冊

        // update_user($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'update_user='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['reset_user_password']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['userid'] = (int)$_POST['userid'];
        $record['account'] = $_POST['account'];
        $record['password'] = $_POST['password'];

        $json_string = json_encode($record);
        $send_string = 'reset_user_password='. $json_string;
        sendMessageToServer($send_string);
        echo $json_string;
    }
    else
    if (isset($_POST['reset_user_topic']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['userid'] = (int)$_POST['userid'];
        $record['account'] = $_POST['account'];

        $json_string = json_encode($record);
        $send_string = 'reset_user_topic='. $json_string;
        sendMessageToServer($send_string);
        echo $json_string;
    }
    else
    if (isset($_POST['delete_user']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['userid'] = (int)$_POST['userid'];
        $record['account'] = $_POST['account'];

        delete_user($project_id, $record['userid']);

        $json_string = json_encode($record);
        $send_string = 'delete_user='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['searchuser']))
    {
        // $groupid = (int)$_SESSION["groupid"];
        // if ( $groupid == 1 || $groupid == 2 )
        // {
        //     $search_result = search_user($project_id, $_POST['text'], $_POST['s'], $_POST['count']);
        // }
        // else
        // {
            // $session_accountid = $_SESSION["accountid"];
            $session_userid = $_SESSION["userid"];
            $session_maingroups = $_SESSION["maingroups"];
            $search_result = search_subgroup_user($project_id, $session_userid, $_POST['text'], $_POST['s'], $_POST['count'], $session_maingroups);
        // }

        $json_string = json_encode($search_result);
        echo $json_string;
    }
    else
    if (isset($_POST['create_new_usergroup']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['groupname'] = $_POST['groupname'];
        $record['permission'] = $_POST['permission'];

        $record['groupid'] = save_new_usergroup($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'update_usergroup='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['update_usergroup']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['groupid'] = $_POST['groupid'];
        $record['groupname'] = $_POST['groupname'];
        $record['permission'] = $_POST['permission'];

        update_usergroup($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'update_usergroup='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['delete_usergroup']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['groupid'] = (int)$_POST['groupid'];

        delete_usergroup($project_id, $record['groupid']);

        $json_string = json_encode($record);
        $send_string = 'update_usergroup='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['create_new_usersubgroup']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['groupname'] = $_POST['groupname'];
        $record['permission'] = $_POST['permission'];
        $record['accountids'] = $_POST['accountids'];
        $record['userids'] = $_POST['userids'];
        $record['imageuid'] = $_POST['imageuid'];

        $maingroups = explode(",", $_SESSION['maingroups']);
        if (count($maingroups) > 0)
            $record['maingroupid'] = $maingroups[0];
        else
            $record['maingroupid'] = '1';

        $session_userid = $_SESSION["userid"];

        $record['groupid'] = save_new_usersubgroup($project_id, $session_userid, $record);

        $json_string = json_encode($record);
        $send_string = 'update_usersubgroup='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['update_usersubgroup']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['subgroupid'] = $_POST['groupid'];
        $record['subgroupname'] = $_POST['groupname'];
        $record['permission'] = $_POST['permission'];
        $record['imageuid'] = $_POST['imageuid'];
        $record['accountids'] = $_POST['accountids'];
        $record['userids'] = $_POST['userids'];
        $record['maingroupid'] = $_POST['maingroupid'];
        $record['type'] = 0;
        $record['validdate'] = '9999-12-31';
        $record['curriculumrecordid'] = 0;

        $session_userid = $_SESSION["userid"];

        update_usersubgroup($project_id, $session_userid, $record);

        $json_string = json_encode($record);
        $send_string = 'update_usersubgroup='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['delete_usersubgroup']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['groupid'] = (int)$_POST['groupid'];

        delete_usersubgroup($project_id, $record['groupid']);

        $json_string = json_encode($record);
        $send_string = 'update_usersubgroup='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['update_usermaingroup']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['groupid'] = (int)$_POST['groupid'];
        $record['groupname'] = $_POST['groupname'];
        $record['permission'] = $_POST['permission'];
        $record['accountids'] = $_POST['accountids'];
        $record['userids'] = $_POST['userids'];
        $record['imageuid'] = $_POST['imageuid'];
        $record['autojoin'] = (int)$_POST['autojoin'];
        $record['welcommessage'] = $_POST['welcommessage'];

        update_usermaingroup($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'update_usermaingroup='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if (isset($_POST['delete_usermaingroup']))
    {
        $record = [];
        $record['project_id'] = $project_id;
        $record['groupid'] = (int)$_POST['groupid'];

        delete_usermaingroup($project_id, $record['groupid']);

        $json_string = json_encode($record);
        $send_string = 'update_usermaingroup='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    //if (isset($_POST['bodyinfopdfreport']))
    if ($obj->bodyinfopdfreport == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['nodeid'] = $obj->nodeid;
        $json_array['nodename'] = $obj->nodename;
        $json_array['startdate'] = $obj->startdate;
        $json_array['enddate'] = $obj->enddate;
        $json_array['weight'] = $obj->weight;
        $json_array['heartbeat'] = $obj->heartbeat;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('bodyinfopdfreport='.$json_string);
        echo $status_str;
    }
    else
    if ($obj->userbodyinfopdfreport == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['accountid'] = $obj->accountid;
        $json_array['username'] = $obj->username;
        $json_array['startdate'] = $obj->startdate;
        $json_array['enddate'] = $obj->enddate;
        $json_array['weight'] = $obj->weight;
        $json_array['heartbeat'] = $obj->heartbeat;
        $json_array['bloodsugar'] = $obj->bloodsugar;
        $json_array['systolicbloodpressure'] = $obj->systolicbloodpressure;
        $json_array['diastolicbloodpressure'] = $obj->diastolicbloodpressure;
        $json_array['bodytemperature'] = $obj->bodytemperature;
        $json_array['bodyfat'] = $obj->bodyfat;
        $json_array['bloodoxygen'] = $obj->bloodoxygen;

        $json_string = json_encode($json_array);
        $status_str = getServerInfo('userbodyinfopdfreport='.$json_string);
        echo $status_str;
    }
    else
    if(isset($_POST['loadtagrollcallpdfreport']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['areaid'] = $_POST['areaid'];
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('tagrollcallpdfreport='.$json_string);
        echo $status_str;
    }
    else
    if(isset($_POST['loadtagrollcallxlsxreport']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['areaid'] = $_POST['areaid'];
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('tagrollcallxlsxreport='.$json_string);
        echo $status_str;
    }
    else
    if(isset($_POST['loaduserinfoxlsxreport']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        // $session_accountid = $_SESSION["accountid"];
        $session_userid = $_SESSION["userid"];
        $session_groupid = $_SESSION["groupid"];
        $session_maingroups = $_SESSION["maingroups"];
        $users = loaduser($project_id, $session_userid, 0, 0, '', $session_maingroups);

        $count = 0;
        $targetusers = [];
        foreach( $users as $user )
        {
            $targetusers[$count] = $user['userid'];
            $count = $count + 1;
        }

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['users'] = $targetusers;
        $json_array['groupid'] = $session_groupid;
        $json_array['maingroups'] = $session_maingroups;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('userinfoxlsxreport='.$json_string);
        echo $status_str;
    }
    else
    if(isset($_POST['loaduserrollcallresultxlsxreport']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $session_accountid = $_SESSION["accountid"];
        $rollcallid = $_POST['rollcallid'];
        $eventid = $_POST['eventid'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['accountid'] = $session_accountid;
        $json_array['rollcallid'] = $rollcallid;
        $json_array['eventid'] = $eventid;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('userrollcallresultxlsxreport='.$json_string);
        echo $status_str;
    }
    else
    if(isset($_POST['loadunknownnfccardsxlsxreport']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('unknownnfccardsxlsxreport='.$json_string);
        echo $status_str;
    }
    else
    if(isset($_POST['loadunknownuhfcardsxlsxreport']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('unknownuhfcardsxlsxreport='.$json_string);
        echo $status_str;
    }
    else
    if(isset($_POST['update_edgecontrol']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $record = [];
        $record['project_id'] = $project_id;
        $record['alive'] = $_POST['alive'];
        $record['macaddress'] = $_POST['macaddress'];
        $record['name'] = $_POST['name'];

        update_edgecontrol($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'update_edgecontrol='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if(isset($_POST['delete_edgecontrol']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $record = [];
        $record['project_id'] = $project_id;
        $record['macaddress'] = $_POST['macaddress'];

        if ($_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            remove_edgecontrol($project_id, $record);
        }
        else
        {
            delete_edgecontrol($project_id, $record);
        }

        $json_string = json_encode($record);
        $send_string = 'update_edgecontrol='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if(isset($_FILES['uploadxlsxfile']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $filename = $_FILES['uploadxlsxfile']['name'];

        global $GL_CONFIG;
        $target_Path = $GL_CONFIG['IMAGE_UPLOAD_PATH'];

        $upload_img_name = $_FILES['uploadxlsxfile']['name'];
        $target_Path = $target_Path.basename( $upload_img_name );
        move_uploaded_file( $_FILES['uploadxlsxfile']['tmp_name'], $target_Path );

        $url_img_file = '';
        if(strlen($upload_img_name) > 0)
        {
            $url_img_file = '/upload/' . $upload_img_name;
        }

        $session_accountid = $_SESSION["accountid"];
        $session_userid = $_SESSION["userid"];
        $session_groupid = $_SESSION["groupid"];
        $session_maingroups = $_SESSION["maingroups"];

        $json_array = [];
        $json_array['project_id'] = $project_id;
        $json_array['filename'] = $filename;
        $json_array['fileurl'] = $url_img_file;
        $json_array['target_Path'] = $target_Path;
        $json_array['tmp_name'] = $_FILES['uploadxlsxfile']['tmp_name'];
        $json_array['fileinfo'] = $_POST['fileinfo'];
        $json_array['accountid'] = $session_accountid;
        $json_array['userid'] = $session_userid;
        $json_array['groupid'] = $session_groupid;
        $json_array['maingroups'] = $session_maingroups;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('uploadxlsxfile='.$json_string);
        echo $status_str;
    }
    else
    if(isset($_POST['update_weekschedule']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        update_weekschedule($project_id, (int)$_POST['subgroupid'], $_POST['schedule']);
    }
    else
    if(isset($_POST['update_sensor']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $record = [];
        $record['project_id'] = $project_id;
        $record['alive'] = $_POST['alive'];
        $record['id'] = $_POST['id'];
        $record['name'] = $_POST['name'];
        $record['lat'] = $_POST['lat'];
        $record['lng'] = $_POST['lng'];

        update_sensordevice($project_id, $record);

        $json_string = json_encode($record);
        $send_string = 'update_sensordevice='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if(isset($_POST['delete_sensor']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $record = [];
        $record['project_id'] = $project_id;
        $record['id'] = $_POST['id'];

        if ($_SESSION["groupid"] == '5' || $_SESSION["groupid"] == '6')
        {
            remove_sensordevice($project_id, $record);
        }
        else
        {
            delete_sensordevice($project_id, $record);
        }

        $json_string = json_encode($record);
        $send_string = 'update_sensordevice='. $json_string;
        sendMessageToServer($send_string);

        echo $json_string;
    }
    else
    if(isset($_POST['verify_new_device']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $record = [];
        $record['project_id'] = $project_id;

        $type = $_POST['type'];
        $verifykey = $_POST['verifykey'];

        $user_maingroupid = 1;
        $user_maingroups = explode(",", $_SESSION['maingroups']);
        if (count($user_maingroups) > 0)
            $user_maingroupid = $user_maingroups[0];

        list($verify_error, $verify_pass) = verifyDeviceKey($type, $verifykey, $user_maingroupid);

        // if( strlen($verify_pass) > 0 )
        // {
        //     $_SESSION['type'] = 2;
        // }

        $record['verify_error'] = $verify_error;
        $record['verify_pass'] = $verify_pass;

        $json_string = json_encode($record);
        echo $json_string;
    }
    else
    if(isset($_POST['verify_new_sensor']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $record = [];
        $record['project_id'] = $project_id;

        $verifykey = $_POST['verifykey'];
        $accountid = $_SESSION['accountid'];
        $userid = $_SESSION['userid'];

        $user_maingroupid = 1;
        $user_maingroups = explode(",", $_SESSION['maingroups']);
        if (count($user_maingroups) > 0)
            $user_maingroupid = $user_maingroups[0];

        list($verify_error, $verify_pass) = verifySensorKey($verifykey, $accountid, $userid, $user_maingroupid);

        if( strlen($verify_pass) > 0 )
        {
            $_SESSION['type'] = 2;
        }

        $record['verify_error'] = $verify_error;
        $record['verify_pass'] = $verify_pass;

        $json_string = json_encode($record);
        echo $json_string;
    }
    else
    if(isset($_POST['register_userregister']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $register_email = $_POST['email'];

        list($existed_email, $registerinfo) = check_account_email_existed(1, $register_email);

        if ( $existed_email )
        {
            if ( $registerinfo['valid'] == 0 )
            {

                //
                // 確認啟動
                //
                $registerinfo['valid'] = 1;
                update_register(1, $registerinfo);

                //
                // create new account to
                //
                list($userid, $datagroups) = create_new_account(1, $registerinfo);

                create_new_UserSubgroupUIBlock($project_id, $datagroups);

                $record = [];
                $record['project_id'] = 1;
                $record['userid'] = strval($userid);
                $record['account'] = $register_email;

                $json_string = json_encode($record);
                $send_string = 'update_user='. $json_string;
                sendMessageToServer($send_string);

                echo '{"success":"true","status":"new"}';
            }
            else
            {
                echo '{"success":"false","status":"existed"}';
            }
        }
        else
        {
            echo '{"success":"false","status":"not existed"}';
        }
    }
    else
    if(isset($_POST['delete_userregister']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $register_email = $_POST['email'];

        delete_register($project_id, $register_email);

        echo '{"success":"true"}';
    }
    else
    if(isset($_POST['reset_air_remain']))
    {
        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $record = [];
        $record['project_id'] = $project_id;
        $record['userid'] = $_POST['userid'];
        $record['areaid'] = $_POST['areaid'];
        $record['remain'] = $_POST['remain'];

        $json_string = json_encode($record);
        $send_string = 'reset_air_remain='. $json_string;
        sendMessageToServer($send_string);

        echo '{"success":"true"}';
    }
}

function VILSNODES_do_DELETE()
{
}

function get_tagbodyinfo_bydate($project_id, $targetid, $StartDate, $EndDate)
{
    $WeightinfoList = get_bodyinfo_Weight_bydate($project_id, $targetid, $StartDate, $EndDate);
    $HeartBeatinfoList = get_bodyinfo_HeartBeat_bydate($project_id, $targetid, $StartDate, $EndDate);

    $Weightinfo = [];
    $Weightinfo['weight'] = $WeightinfoList;
    $HeartBeatinfo = [];
    $HeartBeatinfo['heartbeat'] = $HeartBeatinfoList;


    $taginfo = [];
    $taginfo[0] = $Weightinfo;
    $taginfo[1] = $HeartBeatinfo;
    return $taginfo;
}

function get_bodyinfo_Weight_bydate($project_id, $targetid, $StartDate, $EndDate)
{
    $WeightinfoList = [];

    $db = new DB($project_id, 'base');

    $datacnt = 0;
    $db->query("SELECT * FROM BodyInfo WHERE nodeid='$targetid' AND weight IS NOT NULL AND datetime BETWEEN '$StartDate' AND '$EndDate' ORDER BY datetime DESC ");
    while($result = $db->fetch_array())
    {
        $info = [];
        $info['weight'] = $result['weight'];
        $info['datetime'] = $result['datetime'];

        $WeightinfoList[$datacnt] = $info;
        $datacnt++;
    }

    return $WeightinfoList;
}

function get_bodyinfo_HeartBeat_bydate($project_id, $targetid, $StartDate, $EndDate)
{
    $HeartBeatinfoList = [];

    $db = new DB($project_id, 'base');

    $datacnt = 0;
    $db->query("SELECT * FROM BodyInfo WHERE nodeid='$targetid' AND heartbeat IS NOT NULL AND datetime BETWEEN '$StartDate' AND '$EndDate' ORDER BY datetime DESC ");
    while($result = $db->fetch_array())
    {
        $info = [];
        $info['heartbeat'] = $result['heartbeat'];
        $info['datetime'] = $result['datetime'];

        $HeartBeatinfoList[$datacnt] = $info;
        $datacnt++;
    }

    return $HeartBeatinfoList;
}

function get_tagbodyinfo($project_id, $targetid, $count)
{
    $WeightinfoList = get_bodyinfo_Weight($project_id, $targetid, $count);
    $HeartBeatinfoList = get_bodyinfo_HeartBeat($project_id, $targetid, $count);

    $Weightinfo = [];
    $Weightinfo['weight'] = $WeightinfoList;
    $HeartBeatinfo = [];
    $HeartBeatinfo['heartbeat'] = $HeartBeatinfoList;


    $taginfo = [];
    $taginfo[0] = $Weightinfo;
    $taginfo[1] = $HeartBeatinfo;
    return $taginfo;
}

function get_bodyinfo_Weight($project_id, $targetid, $count)
{
    $WeightinfoList = [];

    $db = new DB($project_id, 'base');

    $datacnt = 0;
    $db->query("SELECT * FROM BodyInfo WHERE nodeid='$targetid' AND weight IS NOT NULL ORDER BY datetime DESC LIMIT $count ");
    while($result = $db->fetch_array())
    {
        $info = [];
        $info['weight'] = $result['weight'];
        $info['datetime'] = $result['datetime'];

        $WeightinfoList[$datacnt] = $info;
        $datacnt++;
    }

    return $WeightinfoList;
}

function get_bodyinfo_HeartBeat($project_id, $targetid, $count)
{
    $HeartBeatinfoList = [];

    $db = new DB($project_id, 'base');

    $datacnt = 0;
    $db->query("SELECT * FROM BodyInfo WHERE nodeid='$targetid' AND heartbeat IS NOT NULL ORDER BY datetime DESC LIMIT $count ");
    while($result = $db->fetch_array())
    {
        $info = [];
        $info['heartbeat'] = $result['heartbeat'];
        $info['datetime'] = $result['datetime'];

        $HeartBeatinfoList[$datacnt] = $info;
        $datacnt++;
    }

    return $HeartBeatinfoList;
}

function get_userbodyinfo_bydate($project_id, $accountid, $StartDate, $EndDate)
{
    $db = new DB($project_id, 'base');

    $info = [];
    $info['weight'] = array();
    $info['heartbeat'] = array();
    $info['bloodsugar'] = array();
    $info['systolicbloodpressure'] = array();
    $info['diastolicbloodpressure'] = array();
    $info['bodytemperature'] = array();
    $info['bodyfat'] = array();
    $info['bloodoxygen'] = array();

    // $db->query("SELECT * FROM BodyInfo WHERE BINARY accountid='$accountid' AND datadatetime BETWEEN '$StartDate' AND '$EndDate' ORDER BY datadatetime DESC ");

    // 每五分鐘一筆
    // $db->query("SELECT date(datadatetime) as time_day, hour(datadatetime) as time_hour, minute(datadatetime) as time_minute, " .
    //            "avg(bloodoxygen) as bloodoxygen, avg(heartbeat) as heartbeat, avg(weight) as weight, avg(bloodsugar) as bloodsugar, " .
    //            "avg(systolicbloodpressure) as systolicbloodpressure, avg(diastolicbloodpressure) as diastolicbloodpressure, avg(bodytemperature) as bodytemperature, " .
    //            "avg(bodyfat) as bodyfat, avg(bloodoxygen) as bloodoxygen " .
    //            "FROM BodyInfo where BINARY accountid='$accountid' AND datadatetime BETWEEN '$StartDate' AND '$EndDate' " .
    //            "AND mod(minute(datadatetime),5)=0 GROUP BY time_day, time_hour, time_minute");

    // 每1分鐘一筆
       $db->query("SELECT max(dbId) as id, date(datadatetime) as time_day, hour(datadatetime) as time_hour, minute(datadatetime) as time_minute, " .
                  "avg(bloodoxygen) as bloodoxygen, avg(heartbeat) as heartbeat, avg(weight) as weight, avg(bloodsugar) as bloodsugar, " .
                  "avg(systolicbloodpressure) as systolicbloodpressure, avg(diastolicbloodpressure) as diastolicbloodpressure, avg(bodytemperature) as bodytemperature, " .
                  "avg(bodyfat) as bodyfat " .
                  "FROM BodyInfo where BINARY accountid='$accountid' AND datadatetime BETWEEN '$StartDate' AND '$EndDate' " .
                  "AND mod(minute(datadatetime),1)=0  GROUP BY time_day, time_hour, time_minute  ORDER BY id DESC LIMIT 50 ");


    while($result = $db->fetch_array())
    {
        put_to_data_array($info, $result, 'weight');
        put_to_data_array($info, $result, 'heartbeat');
        put_to_data_array($info, $result, 'bloodsugar');
        put_to_data_array($info, $result, 'systolicbloodpressure');
        put_to_data_array($info, $result, 'diastolicbloodpressure');
        put_to_data_array($info, $result, 'bodytemperature');
        put_to_data_array($info, $result, 'bodyfat');
        put_to_data_array($info, $result, 'bloodoxygen');
    }

    return $info;
}

function get_userbodybaseinfo_bydate($project_id, $accountid, $StartDate, $EndDate)
{
    $db = new DB($project_id, 'base');

    $info = [];
    $info['heartbeat'] = array();
    $info['bloodoxygen'] = array();
    // 每1分鐘一筆
   $db->query("SELECT max(dbId) as id, date(datadatetime) as time_day, hour(datadatetime) as time_hour, minute(datadatetime) as time_minute, " .
              "avg(bloodoxygen) as bloodoxygen, avg(heartbeat) as heartbeat  " .
              "FROM BodyInfo where BINARY accountid='$accountid' AND datadatetime BETWEEN '$StartDate' AND '$EndDate' " .
              "AND mod(minute(datadatetime),1)=0  GROUP BY time_day, time_hour, time_minute ORDER BY id DESC LIMIT 20 ");

    while($result = $db->fetch_array())
    {
        put_to_data_array($info, $result, 'bloodoxygen');
        put_to_data_array($info, $result, 'heartbeat');
    }

    return $info;
}

function put_to_data_array(&$info, &$result, $targetinfo)
{
    if ( $result[$targetinfo] != null )
    {
        $data = [];
        $data['data'] = (int)$result[$targetinfo];

        if ($data['data'] != 0)
        {
            $time_day = $result['time_day'];
            $time_hour = $result['time_hour'];
            $time_minute = $result['time_minute'];
            $data['datetime'] = $time_day.' '.sprintf("%02d", $time_hour).':'.sprintf("%02d", $time_minute).':00';
            // $data['datetime'] = $result['datadatetime'];
            // array_push($info[$targetinfo], $data);
            array_unshift($info[$targetinfo], $data);
        }
    }
}

function loadmaintenancerecord($project_id, $start, $count)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $datacnt = 0;
    if ( $count == 0 )
        $db->query("SELECT * FROM DeviceMaintenanceRecord ORDER BY datetime DESC ");
    else
        $db->query("SELECT * FROM DeviceMaintenanceRecord ORDER BY datetime DESC LIMIT $start,$count ");

    while($result = $db->fetch_array())
    {
        $record = [];
        $record['nodetype'] = $result['nodetype'];
        $record['nodeid'] = $result['nodeid'];
        $record['name'] = $result['name'];
        $record['type'] = $result['type'];
        $record['unit'] = $result['unit'];
        $record['manager'] = $result['manager'];
        $record['assetlife'] = $result['assetlife'];
        $record['maintenancedate'] = $result['maintenancedate'];
        $record['status'] = $result['status'];
        $record['datetime'] = $result['DateTime'];

        $records[$datacnt] = $record;
        $datacnt++;
    }

    return $records;
}

function save_new_maintenance($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $db->query("INSERT INTO DeviceMaintenanceRecord(nodetype, nodeid, name, type, unit, manager, assetlife, maintenancedate, status) " .
                "VALUES (".$record['nodetype'].", '".$record['nodeid']."', '".$record['name']."', '".$record['type']."', '".$record['unit']."', '".$record['manager'].
                "', '".$record['assetlife']."', '".$record['maintenancedate']."', '".$record['status']."')");
}

function update_weekschedule($project_id, $subgroupid, $schedule)
{
    $db = new DB($project_id, 'base');

    $db->query("INSERT INTO weekschedule(subgroupid, jsonstring) VALUES ($subgroupid, '$schedule') ON DUPLICATE KEY UPDATE jsonstring=VALUES(jsonstring) ");
}

function load_schedule($project_id, $subgroupid)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM weekschedule WHERE subgroupid=$subgroupid ");

    $schedule = null;
    while($result = $db->fetch_array())
    {
        $schedule = [];
        $schedule['subgroupid'] = $result['subgroupid'];
        $schedule['jsonstring'] = json_decode($result['jsonstring']);
    }
    return $schedule;
}

function update_edgecontrol($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $alive = $record['alive'];
    $macaddress = $record['macaddress'];
    $name = $record['name'];

    $db->query("UPDATE EDGEDevice SET name='$name', alive=$alive WHERE macaddress='$macaddress' ");
}

function remove_edgecontrol($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $macaddress = $record['macaddress'];

    $db->query("UPDATE EDGEDevice SET maingroupid=1 WHERE macaddress='$macaddress' ");
}

function delete_edgecontrol($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $macaddress = $record['macaddress'];

    $db->query("DELETE FROM EDGEDevice WHERE macaddress='$macaddress' ");
}

function update_sensordevice($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $alive = $record['alive'];
    $id = $record['id'];
    $name = $record['name'];
    $lat = $record['lat'];
    $lng = $record['lng'];

    $db->query("UPDATE sensordevice SET name='$name', Longitude='$lng', Latitude='$lat', alive=$alive WHERE id='$id' ");
}

function remove_sensordevice($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $id = $record['id'];

    $db->query("UPDATE sensordevice SET maingroupid=1 WHERE id='$id' ");
}

function delete_sensordevice($project_id, $record)
{
    $db = new DB($project_id, 'base');

    $id = $record['id'];

    $db->query("DELETE FROM sensordevice WHERE id='$id' ");
}

function loadsensorinfo($project_id)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT name as stringvalue FROM sensorinfo GROUP BY stringvalue ");

    $datacnt = 0;
    $sensorinfo = [];
    while($result = $db->fetch_array())
    {
        $sensorinfo[$datacnt] = $result['stringvalue'];
        $datacnt = $datacnt + 1;
    }
    return $sensorinfo;
}

function loadsensorrawdata($project_id, $names, $day, $offset, $count)
{
    $db = new DB($project_id, 'base');

    $namelist = explode(",", $names);
    // $names = '\'' . implode("','", $namelist).'\'';

    $datacnt = 0;
    $sensordatas = [];
    foreach( $namelist as $name )
    {
        //$db->query("SELECT * FROM sensorinfo WHERE name='$name' AND date(datetime) = CURDATE() order by datetime desc limit 100 ");
        if ($count > 0)
            $db->query("SELECT * FROM sensorinfo WHERE name='$name' AND day='$day' order by datetime desc limit $offset,$count");
        else
            $db->query("SELECT * FROM sensorinfo WHERE name='$name' AND day='$day' order by datetime desc ");

        while($result = $db->fetch_array())
        {
            $sensordata = [];
            $sensordata['name'] = $result['name'];
            $sensordata['day'] = $result['day'];
            $sensordata['value'] = $result['value'];
            $sensordata['jsonvalue'] = $result['jsonvalue'];
            $sensordata['Longitude'] = $result['Longitude'];
            $sensordata['Latitude'] = $result['Latitude'];
            $sensordata['datetime'] = $result['datetime'];
            $sensordatas[$datacnt] = $sensordata;
            $datacnt = $datacnt + 1;
        }
    }

    return $sensordatas;
}

function verifyDeviceKey($type, $verifykey, $user_maingroupid)
{
    $verify_error = '';
    $verify_pass = '';

    if ( strlen($verifykey) != 8)
    {
        $verify_error = '請輸入正確機器碼!';
    }
    else
    {
        $count = 0;
        $device = [];
        $device_maingroupid_orig = 1;

        $NODE_TYPE_ANCHOR = 0;
        $NODE_TYPE_TAG = 1;
        $NODE_TYPE_COORD = 2;
        $NODE_TYPE_LOCATOR = 3;

        if( $type == 'tag' )
        {
            list($count, $device, $device_maingroupid_orig) = check_device_key($NODE_TYPE_TAG, $verifykey, $user_maingroupid);
        }
        else
        if( $type == 'anchor' )
        {
            list($count, $device, $device_maingroupid_orig) = check_device_key($NODE_TYPE_ANCHOR, $verifykey, $user_maingroupid);
        }
        else
        if( $type == 'coord' )
        {
            list($count, $device, $device_maingroupid_orig) = check_device_key($NODE_TYPE_COORD, $verifykey, $user_maingroupid);
        }
        else
        if( $type == 'locator' )
        {
            list($count, $device, $device_maingroupid_orig) = check_device_key($NODE_TYPE_LOCATOR, $verifykey, $user_maingroupid);
        }
        else
        if( $type == 'celllocator' )
        {
            list($count, $device, $device_maingroupid_orig) = check_device_key($NODE_TYPE_LOCATOR, $verifykey, $user_maingroupid);
        }
        else
        if( $type == 'wifilocator' )
        {
            list($count, $device, $device_maingroupid_orig) = check_wifilocator_key($verifykey, $user_maingroupid);
        }
        else
        if( $type == 'edgecontol' )
        {
            list($count, $device, $device_maingroupid_orig) = check_EDGEDevice_key($verifykey, $user_maingroupid);
        }
        else
        if( $type == 'uhfreader' )
        {
            list($count, $device, $device_maingroupid_orig) = check_RFIDDevice_key($verifykey, $user_maingroupid);
        }
        else
        if( $type == 'loragateway' )
        {
            list($count, $device, $device_maingroupid_orig) = check_LoRaGatewayInfo_key($verifykey, $user_maingroupid);
        }
        if ( $count > 0 )
        {
            if ( $device_maingroupid_orig != 1 )
            {
                $verify_error = '您輸入的機器碼已被關連，請先確認已解除關連或輸入正確!';
            }
            else
            {
                if ( $user_maingroupid != 1 )
                {
                    $verify_pass = '此裝置已成功註冊!';
                }
                else
                {
                    $verify_error = '目前使用者無法註冊此裝置!';
                }
            }
        }
        else
        {
            $verify_error = '您輸入的機器碼並非有效機器碼';
        }
    }
    return array($verify_error, $verify_pass);
}

function verifySensorKey($verifykey, $accountid, $userid, $user_maingroupid)
{
    $verify_error = '';
    $verify_pass = '';

    if ( strlen($verifykey) != 8)
    {
        $verify_error = '請輸入正確機器碼!';
    }
    else
    {
        list($count, $device) = check_sensor_key($verifykey);
        if ( $count > 0 )
        {
            $device_maingroupid = $device['maingroupid'];
            if ( $device_maingroupid != 1 )
            {
                $verify_error = '您輸入的機器碼已被關連，請先確認已解除關連或輸入正確!';
            }
            else
            {
                if ( $user_maingroupid != 1 )
                {
                    //
                    // register this device to this maingroup
                    //
                    update_sensor_maingroup(1, $device['id'], $user_maingroupid);

                    $json_string = json_encode($device);
                    $send_string = 'update_sensordevice='. $json_string;
                    sendMessageToServer($send_string);

                    //
                    // set User type = 2; // 0: vilscenter user 1:已註冊, 2:已購買點名裝置
                    //
                    // update_user_type(1, $accountid, $userid, 2);

                    $verify_pass = '此裝置已成功註冊!';
                }
                else
                {
                    $verify_error = '目前使用者無法註冊此裝置!';
                }
            }
        }
        else
        {
            $verify_error = '您輸入的機器碼並非有效機器碼';
        }
    }

    return array($verify_error, $verify_pass);
}

function check_sensor_key($verifykey)
{
    $verify_ok = false;

    $db = new DB(1, 'base');

    $db->query("SELECT * from sensordevice WHERE verifykey='$verifykey' ");

    $device = [];

    $count = 0;
    if($result = $db->fetch_array())
    {
        $device['alive'] = $result['alive'];
        $device['id'] = $result['id'];
        $device['name'] = $result['name'];
        $device['type'] = $result['type'];
        $device['Version'] = $result['Version'];
        $device['maingroupid'] = $result['maingroupid'];

        $count++;
    }

    return array($count, $device);
}

function update_sensor_maingroup($project_id, $id, $maingroupid)
{
    $db = new DB($project_id, 'base');
    $db->query("UPDATE sensordevice SET maingroupid=$maingroupid WHERE id='$id' ");
}

function check_device_key($NODE_TYPE, $verifykey, $user_maingroupid)
{
    $verify_ok = false;

    $db = new DB(1, 'base');

    $db->query("SELECT * from NodeInfo WHERE verifykey='$verifykey' AND nodetype=$NODE_TYPE ");

    $device = [];
    $count = 0;
    $device_maingroupid_orig = 1;

    if($result = $db->fetch_array())
    {
        $device['macaddress'] = $result['macaddress'];
        $device['maingroupid'] = $result['maingroupid'];
        $device_maingroupid_orig = $result['maingroupid'];
        $count++;
    }

    if ( $count > 0 && $device_maingroupid_orig == 1 &&  $user_maingroupid != 1)
    {
        //
        // register this device to this maingroup
        //
        $macaddress = $device['macaddress'];
        $device['maingroupid'] = $user_maingroupid;
        $db->query("UPDATE NodeInfo SET maingroupid=$user_maingroupid WHERE macaddress='$macaddress' AND nodetype=$NODE_TYPE ");

        // Maybe has AOA
        $db->query("UPDATE AoANodeInfo SET maingroupid=$user_maingroupid WHERE macaddress='$macaddress'");

        $json_string = json_encode($device);
        $send_string = 'update_nodemaingroup='. $json_string;
        sendMessageToServer($send_string);
    }

    return array($count, $device, $device_maingroupid_orig);
}

function check_wifilocator_key($verifykey, $user_maingroupid)
{
    $verify_ok = false;

    $db = new DB(1, 'base');

    $db->query("SELECT * from NFCDevice WHERE verifykey='$verifykey' ");

    $device = [];
    $count = 0;
    $device_maingroupid_orig = 1;

    if($result = $db->fetch_array())
    {
        $device['deviceid'] = $result['deviceid'];
        $device['maingroupid'] = $result['maingroupid'];
        $device_maingroupid_orig = $result['maingroupid'];
        $count++;
    }

    if ( $count > 0 && $device_maingroupid_orig == 1 &&  $user_maingroupid != 1)
    {
        //
        // register this device to this maingroup
        //
        $id = $device['deviceid'];
        $device['maingroupid'] = $ruser_maingroupid;
        $db->query("UPDATE NFCDevice SET maingroupid=$user_maingroupid WHERE deviceid='$id' ");

        $json_string = json_encode($device);
        $send_string = 'update_nfclocator='. $json_string;
        sendMessageToServer($send_string);
    }

    return array($count, $device, $device_maingroupid_orig);
}

function check_EDGEDevice_key($verifykey, $user_maingroupid)
{
    $verify_ok = false;

    $db = new DB(1, 'base');

    $db->query("SELECT * from EDGEDevice WHERE verifykey='$verifykey' ");

    $device = [];
    $count = 0;
    $device_maingroupid_orig = 1;

    if($result = $db->fetch_array())
    {
        $device['deviceid'] = $result['deviceid'];
        $device['maingroupid'] = $result['maingroupid'];
        $device_maingroupid_orig = $result['maingroupid'];
        $count++;
    }

    if ( $count > 0 && $device_maingroupid_orig == 1 &&  $user_maingroupid != 1)
    {
        //
        // register this device to this maingroup
        //
        $id = $device['deviceid'];
        $device['maingroupid'] = $user_maingroupid;
        $db->query("UPDATE EDGEDevice SET maingroupid=$user_maingroupid WHERE deviceid='$id' ");

        $json_string = json_encode($device);
        $send_string = 'update_edgecontrol='. $json_string;
        sendMessageToServer($send_string);
    }

    return array($count, $device, $device_maingroupid_orig);
}

function check_RFIDDevice_key($verifykey, $user_maingroupid)
{
    $verify_ok = false;

    $db = new DB(1, 'base');

    $db->query("SELECT * from RFIDDevice WHERE verifykey='$verifykey' ");

    $device = [];
    $count = 0;
    $device_maingroupid_orig = 1;

    if($result = $db->fetch_array())
    {
        $device['deviceid'] = $result['deviceid'];
        $device['maingroupid'] = $result['maingroupid'];
        $device_maingroupid_orig = $result['maingroupid'];
        $count++;
    }

    if ( $count > 0 && $device_maingroupid_orig == 1 &&  $user_maingroupid != 1)
    {
        //
        // register this device to this maingroup
        //
        $id = $device['deviceid'];
        $device['maingroupid'] = $user_maingroupid;
        $db->query("UPDATE RFIDDevice SET maingroupid=$user_maingroupid WHERE deviceid='$id' ");

        $json_string = json_encode($device);
        $send_string = 'update_rfiddevice='. $json_string;
        sendMessageToServer($send_string);
    }

    return array($count, $device, $device_maingroupid_orig);
}

function check_LoRaGatewayInfo_key($verifykey, $user_maingroupid)
{
    $verify_ok = false;

    $db = new DB(1, 'base');

    $db->query("SELECT * from LoRaGatewayInfo WHERE verifykey='$verifykey' ");

    $device = [];
    $count = 0;
    $device_maingroupid_orig = 1;

    if($result = $db->fetch_array())
    {
        $device['gwid'] = $result['gwid'];
        $device['maingroupid'] = $result['maingroupid'];
        $device_maingroupid_orig = $result['maingroupid'];
        $count++;
    }

    if ( $count > 0 && $device_maingroupid_orig == 1 &&  $user_maingroupid != 1)
    {
        //
        // register this device to this maingroup
        //
        $id = $device['gwid'];
        $device['maingroupid'] = $user_maingroupid;
        $db->query("UPDATE LoRaGatewayInfo SET maingroupid=$user_maingroupid WHERE gwid='$id' ");

        $json_string = json_encode($device);
        $send_string = 'update_LoRaGatewayInfo='. $json_string;
        sendMessageToServer($send_string);
    }

    return array($count, $device, $device_maingroupid_orig);
}

?>
