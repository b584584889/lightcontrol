<?php

session_start();

require_once("common.php");

header('Content-Type: application/json');

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
       COURSE_do_GET();
    break;
    case 'PUT':
        COURSE_do_PUT();
    break;
    case 'POST':
        COURSE_do_POST();
    break;
    case 'DELETE':
        COURSE_do_DELETE();
    break;
}

function COURSE_do_GET()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    if (isset($_GET['loadcourses']))
    {
        $maingroups = $_SESSION['maingroups'];

        $return_str = loadcourses($project_id, $maingroups);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['loadcourserollcallrecord']))
    {
        $maingroups = $_SESSION['maingroups'];

        $return_str = loadcourserollcallrecord($project_id, $_GET['courseId'], $_GET['infoOfDate']);
        echo json_encode($return_str);
    }
    else
    if (isset($_GET['checkrecentrollcallrecord']))
    {
        if (isset($_GET['recentdays']))
        {
            $courseId = $_GET['courseId'];
            $startdate = $_GET['startdate'];
            $recentdays = (int)$_GET['recentdays'];

            if (strlen($startdate) > 10)
                $startdate = substr( $startdate, 0, 10 );

            $return_str = checkrecentrollcallrecord($project_id, $courseId, $startdate, $recentdays);
            echo json_encode($return_str);
        }
    }
}

function COURSE_do_PUT()
{
}

function COURSE_do_POST()
{
    $json = file_get_contents('php://input');
    $obj = json_decode($json);

    if (is_array($obj))
    {
        $issues = [];
        $issues['is_array'] = 1;
        echo json_encode($issues);
    }
    else
    if ($obj->CREATE_COURSE == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['resourceId'] = $obj->resourceId;
        $json_array['courseId'] = $obj->courseId;
        $json_array['title'] = $obj->title;
        $json_array['description'] = $obj->description;
        $json_array['allDay'] = $obj->allDay;
        $json_array['start'] = $obj->start;
        $json_array['end'] = $obj->end;
        $json_array['url'] = $obj->url;
        $json_array['startTime'] = $obj->startTime;
        $json_array['endTime'] = $obj->endTime;
        $json_array['daysOfWeek'] = $obj->daysOfWeek;
        $json_array['startRecur'] = $obj->startRecur;
        $json_array['endRecur'] = $obj->endRecur;
        $json_array['backgroundColor'] = $obj->backgroundColor;
        $json_array['borderColor'] = $obj->borderColor;

        $maingroups = explode(",", $_SESSION['maingroups']);

        if (count($maingroups) > 0)
            $json_array['maingroupid'] = $maingroups[0];
        else
            $json_array['maingroupid'] = '1';

        $json_string = json_encode($json_array);
        $send_string = 'createcourse=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($json_array);
    }
    else
    if ($obj->DELETE_COURSE == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['todayDate'] = $obj->todayDate;
        $json_array['courseId'] = $obj->courseId;

        $json_string = json_encode($json_array);
        $send_string = 'deletecourse=' . $json_string;
        sendMessageToServer($send_string);

        echo json_encode($json_array);
    }
    else
    if ($obj->loadcoursepdfreport == 1)
    {
        $json_array = [];
        $json_array['project_id'] = $obj->project_id;
        $json_array['courseId'] = $obj->courseId;
        $json_array['date'] = $obj->date;
        $json_string = json_encode($json_array);
        $status_str = getServerInfo('loadcoursepdfreport='.$json_string);
        echo $status_str;
    }
    else
    {
        $issues = [];
        $issues['action'] = 'none';
        echo json_encode($issues);
    }

}

function COURSE_do_DELETE()
{
    // $project_id = 1;
    // if (isset($_GET['project_id']))
    //     $project_id = (int)$_GET['project_id'];
    //
    // $event = [];
    // if (isset($_GET['PathID']))
    // {
    //     $json_array = [];
    //     $json_array['project_id'] =  $project_id;
    //     $json_array['GroupID'] = (int)$_GET['GroupID'];
    //     $json_array['PathID'] = (int)$_GET['PathID'];
    //
    //     $json_string = json_encode($json_array);
    //     $send_string = 'deletecheckpointpath=' . $json_string;
    //     sendMessageToServer($send_string);
    //
    //     echo json_encode($json_array);
    // }
    // else
    // {
    //     echo json_encode($event);
    // }
}

function loadcourses($project_id, $maingroups)
{
    $courses = [];

    $maingroupList = explode(",", $maingroups);

    $db = new DB($project_id, 'base');

    $db->query("SELECT * from Course WHERE PROJECT_ID=" . $project_id);
    while($result = $db->fetch_array())
    {
        $course = [];
        $course['courseId'] = $result['courseId'];
        $course['projectid'] = $result['PROJECT_ID'];
        $course['resourceId'] = $result['resourceId'];
        $course['title'] = $result['title'];
        $course['description'] = $result['description'];
        $course['allDay'] = $result['allDay'];
        $course['start'] = $result['start'];
        $course['end'] = $result['end'];
        $course['startTime'] = $result['startTime'];
        $course['endTime'] = $result['endTime'];
        $course['daysOfWeek'] = $result['daysOfWeek'];
        $course['startRecur'] = $result['startRecur'];
        $course['endRecur'] = $result['endRecur'];
        $course['backgroundColor'] = $result['backgroundColor'];
        $course['borderColor'] = $result['borderColor'];
        $course['maingroupid'] = $result['maingroupid'];
        if ($course['maingroupid'] == null)
            $course['maingroupid'] = 1;

        $valid = false;
        foreach ($maingroupList as $maingroup)
        {
            if ( $course['maingroupid'] == $maingroup )
            {
                $valid = true;
                break;
            }
        }

        if ($valid)
        {
            $courses[$result['courseId']] = $course;
        }
    }
    //$db->close();

    return $courses;
}

function loadcourserollcallrecord($project_id, $courseId, $infoOfDate)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $rollcallrecord = [];

    $db->query("SELECT * from CourseRollCallRecord WHERE infoOfDate='" . $infoOfDate . "' AND courseId='" . $courseId . "'");
    while($result = $db->fetch_array())
    {
        $rollcallrecord['courseId'] = $result['courseId'];
        $rollcallrecord['projectid'] = $result['PROJECT_ID'];
        $rollcallrecord['infoOfDate'] = $result['infoOfDate'];
        $rollcallrecord['issuetime'] = $result['issuetime'];
        $rollcallrecord['tags'] = $FP_Params = explode(",",  $result['tags']);

        $tagcnt = 0;
        foreach($rollcallrecord['tags'] as $nodeid)
        {
            $rollcallrecord['tagname'][$tagcnt] = $nodeid;
            $db->query("SELECT nodename from NodeInfo WHERE nodeid='$nodeid'");
            while($result_tag = $db->fetch_array())
            {
                $rollcallrecord['tagname'][$tagcnt] = $result_tag['nodename'];
                break;
            }
            $tagcnt++;
        }
    }
    //$db->close();

    return $rollcallrecord;
}


function checkrecentrollcallrecord($project_id, $courseId, $startdate, $recentdays)
{
    // global $_DB;
    // $db = new DB();
    // $db->connect_db($_DB['host'], $_DB['username'], $_DB['password'], $_DB['dbname']);
    $db = new DB($project_id, 'base');

    $db->query("SELECT tags, infoOfDate " .
                "FROM CourseRollCallRecord " .
                "WHERE courseId='" . $courseId. "' AND infoOfDate <= '".$startdate."' ORDER BY infoOfDate DESC LIMIT ".$recentdays);


    $rollcallrecord = [];

    $data_count = 0;
    while($result = $db->fetch_array())
    {
        $record = [];
        $record['infoOfDate'] = $result['infoOfDate'];
        $record['tags'] = $FP_Params = explode(",",  $result['tags']);
        $record['tagnames'] = [];

        $index = 0;
        $count = count($record['tags']);
        for($index=0;$index<$count;$index++)
        {
            $tagid = $record['tags'][$index];
            $record['tagnames'][$index] = get_node_name($project_id, $tagid);
        }


        $rollcallrecord[$data_count] = $record;
        $data_count++;
    }
    //$db->close();

    return $rollcallrecord;
}

?>
