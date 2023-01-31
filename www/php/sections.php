<?php

session_start();

require_once("common.php");

header('Content-Type: application/json');

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
        SECTIONS_do_GET();
    break;
    case 'PUT':
        SECTIONS_do_PUT();
    break;
    case 'POST':
        SECTIONS_do_POST();
    break;
    case 'DELETE':
        SECTIONS_do_DELETE();
    break;
}

function SECTIONS_do_GET()
{
    $project_id = "1";
    if (isset($_GET['project_id']))
        $project_id = $_GET['project_id'];

    if (isset($_GET['loadsections']))
    {
        $session_maingroups = $_SESSION["maingroups"];
        $session_groupid = $_SESSION["groupid"];

        $return_str = loadsections($project_id, $session_groupid, $session_maingroups);
        echo json_encode($return_str);
    }
}

function SECTIONS_do_PUT()
{
}

function SECTIONS_do_POST()
{
    $json = file_get_contents('php://input');
    $obj = json_decode($json);

    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    if (isset($_POST['delete_group']))
    {
        $group_id = (int)$_POST['group_id'];

        $json_array = [];
        $json_array['GroupID'] = $group_id;
        $json_array['project_id'] = $project_id;
        $json_string = json_encode($json_array);

        $send_string = 'deletesection=' . $json_string;
        sendMessageToServer($send_string);
    }
    else
    if (!is_array($obj))
    {
        $section_type = (int)$obj->section_type;
        $project_id = $obj->project_id;

        if ($section_type == 0)
        {
            $json_array = [];
            $json_array['project_id'] = $project_id;
            $json_array['SectionType'] = $section_type;
            $json_array['nodeIDList'] = $obj->anchor;
            $json_array['AreaID'] = $obj->area;
            $json_array['Param1'] = (int)$obj->range;
            $json_array['Param2'] = (int)$obj->alerttype;
            $json_array['maingroups'] = $_SESSION["maingroups"];

            $json_string = json_encode($json_array);

            $send_string = 'createsection=' . $json_string;
            sendMessageToServer($send_string);
        }
        else
        if ($section_type == 1)
        {
            $nodeIDList = $obj->anchors;
            $frompoint = $obj->frompoint;
            $topoint = $obj->topoint;

            $FP_Params = explode(",",  $frompoint);
            $TP_Params = explode(",",  $topoint);
            $Param1 = (int)$FP_Params[0];
            $Param2 = (int)$FP_Params[1];
            $Param3 = (int)$FP_Params[2];
            $Param4 = (int)$TP_Params[0];
            $Param5 = (int)$TP_Params[1];
            $Param6 = (int)$TP_Params[2];

            if ( sizeof($FP_Params) == 3 && sizeof($TP_Params) == 3 )
            {
                $json_array = [];
                $json_array['project_id'] = $project_id;
                $json_array['SectionType'] = $section_type;
                $json_array['nodeIDList'] = $nodeIDList;
                $json_array['AreaID'] = "";
                $json_array['Param1'] = $Param1;
                $json_array['Param2'] = $Param2;
                $json_array['Param3'] = $Param3;
                $json_array['Param4'] = $Param4;
                $json_array['Param5'] = $Param5;
                $json_array['Param6'] = $Param6;
                $json_array['maingroups'] = $_SESSION["maingroups"];

                $json_string = json_encode($json_array);

                $send_string = 'createsection=' . $json_string;
                sendMessageToServer($send_string);
            }
        }
        else
        {
            $issues = [];
            echo json_encode($issues);
            return;
        }

        # send notification to server
        //sendMessageToServer('updatesection');

        echo json_encode($obj);
    }
    else
    {
        $issues = [];
        echo json_encode($issues);
    }
}

function SECTIONS_do_DELETE()
{
    $event = [];
    if (isset($_GET['group_id']))
    {
        $group_id = (int)$_GET['group_id'];

        $json_array = [];
        $json_array['GroupID'] = $group_id;
        $json_array['project_id'] = $_GET['project_id'];
        $json_string = json_encode($json_array);

        $send_string = 'deletesection=' . $json_string;
        sendMessageToServer($send_string);
    }

    # send notification to server
    // sendMessageToServer('updatesection');

    echo json_encode($event);
}

function loadsections($project_id, $session_groupid, $maingroupids)
{
    $sections = [];

    $db = new DB($project_id, 'base');

    $QueryString = '';
    if ($session_groupid == '5' || $session_groupid == '6')
    {
        // maingroup user
        $QueryString = "AND maingroupid in($maingroupids)";
    }

    $db->query("SELECT * FROM SectionGroup WHERE PROJECT_ID=". $project_id ." ". $QueryString);
    while($result = $db->fetch_array())
    {
        $section = [];
        $section['GroupID'] = $result['GroupID'];
        $section['PROJECT_ID'] = $result['PROJECT_ID'];
        $section['SectionType'] = $result['SectionType'];
        $section['AreaID'] = $result['AreaID'];
        $section['Param1'] = $result['Param1'];
        $section['Param2'] = $result['Param2'];
        $section['Param3'] = $result['Param3'];
        $section['Param4'] = $result['Param4'];
        $section['Param5'] = $result['Param5'];
        $section['Param6'] = $result['Param6'];
        $section['maingroupid'] = $result['maingroupid'];
        $section['nodeIDList'] = $result['nodeIDList'];

        $sections[$result['GroupID']] = $section;
    }
    //$db->close();

    return $sections;
}

?>
