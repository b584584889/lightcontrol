<?php

session_start();

if (!isset($_SESSION['PAGE_CONFIG']))
{
    $_SESSION['PAGE_CONFIG'] = 'NORGAY';
}

include ('checksession.php');

$path = ltrim($_SERVER['REQUEST_URI'], '/');    // Trim leading slash(es)
$elements = explode('/', $path);                // Split path on slashes

require_once('config.php');

if ($_SESSION["groupid"] == 3)
{
    // "普通用戶", "只能看DI, 不能看網頁"
    header("Location: /di/");
}

require_once("php/common.php");

if(empty($elements[0]))
{                       // No path elements means home
    //ShowHomepage();
    echo 'normal';
}
else
if(array_shift($elements))// index.php
{
    if ( isset($elements[0]) )
        $target = $elements[0];
    else
        $target = $elements;

    $project_id = '1';

    if (empty($elements[1]))
    {
        if (is_numeric($target))
        {
            $project_id = $target;
        }
    }
    else
    {
        $project_id = explode('?',$elements[1])[0];// 1?RangeStartDate=2017:09:11:01:33:22&RangeEndDate=2017:09:13:01:33:22
    }

    // check valid project
    if (checkValidProject((int)$project_id) == TRUE)
    {

    }
    else
    {
        $project_id = '1';
    }

    switch($target)             // Pop off first item and switch
    {
    case 'log':
        include ('vilslog.php');
        break;
    case 'login':
        include ('login.php');
        break;
    case 'vils':
        include ('vils.php');
        break;
    case 'vilsdashboard':
        if ($GL_CONFIG_ENABLE_DASHBOARD)
            include ('vilsdashboard.php');
        else
            header("Location: /");
        break;

    case 'vilsdevices':
        if ($GL_CONFIG_ENABLE_NODELIST || $_SESSION['type'] == 2)
            include ('vilsdevices.php');
        else
            header("Location: /");
        break;

    case 'vilsrfiddevices':
        if ($GL_CONFIG_ENABLE_RFIDDEVICELIST)
            include ('vilsrfiddevices.php');
        else
            header("Location: /");
        break;

    case 'vilsrfidcards':
        if ($GL_CONFIG_ENABLE_RFIDDEVICELIST)
            include ('vilsrfidcards.php');
        else
            header("Location: /");
        break;

    case 'vilsuserinfo':
        if ($GL_CONFIG_ENABLE_USEINFO)
            include ('vilsuserinfo.php');
        else
            header("Location: /");
        break;

    case 'vilsuserregister':
        if ($GL_CONFIG_ENABLE_USEINFO)
            include ('vilsuserregister.php');
        else
            header("Location: /");
        break;

    case 'vilsusergroupinfo':
        if ($GL_CONFIG_ENABLE_USEINFO)
            include ('vilsusergroupinfo.php');
        else
            header("Location: /");
        break;

    case 'vilsusermaingroupinfo':
        if ($GL_CONFIG_ENABLE_USEINFO)
            include ('vilsusermaingroupinfo.php');
        else
            header("Location: /");
        break;

    case 'vilsusersubgroupinfo':
        if ($GL_CONFIG_ENABLE_USEINFO)
            include ('vilsusersubgroupinfo.php');
        else
            header("Location: /");
        break;

    case 'vilscardrollcall':
        if ($GL_CONFIG_ENABLE_CARDROLLCALL)
            include ('vilscardrollcall.php');
        else
            header("Location: /");
        break;

    case 'vilscardrollcallevent':
        if ($GL_CONFIG_ENABLE_CARDROLLCALL)
            include ('vilscardrollcallevent.php');
        else
            header("Location: /");
        break;

    case 'vilscardrollcallresult':

        $rollcallid = $_GET['rollcallid'];
        $eventid = $_GET['eventid'];

        if ($GL_CONFIG_ENABLE_CARDROLLCALL)
            include ('vilscardrollcallresult.php');
        else
            header("Location: /");
        break;

    case 'vilsuserrollcallresult':

        $rollcallid = $_GET['rollcallid'];
        $eventid = $_GET['eventid'];

        if ($GL_CONFIG_ENABLE_CARDROLLCALL)
            include ('vilsuserrollcallresult.php');
        else
            header("Location: /");
        break;

    case 'vilscardrollcallrecord':
        if ($GL_CONFIG_ENABLE_CARDROLLCALL)
            include ('vilscardrollcallrecord.php');
        else
            header("Location: /");
        break;

    case 'vilsuserrollcallrecord':
        if ($GL_CONFIG_ENABLE_CARDROLLCALL)
            include ('vilsuserrollcallrecord.php');
        else
            header("Location: /");
        break;

    case 'vilscardscannfcresult':
        if ($GL_CONFIG_ENABLE_CARDROLLCALL)
            include ('vilscardscannfcresult.php');
        else
            header("Location: /");
        break;

    case 'vilscardscanuhfresult':
        if ($GL_CONFIG_ENABLE_CARDROLLCALL)
            include ('vilscardscanuhfresult.php');
        else
            header("Location: /");
        break;

    case 'vilsbodyinfo':
        if ($GL_CONFIG_ENABLE_BODYINFO)
            include ('vilsbodyinfo.php');
        else
            header("Location: /");
        break;

    case 'vilsbodyinforecord':
        if ($GL_CONFIG_ENABLE_BODYINFO)
            include ('vilsbodyinforecord.php');
        else
            header("Location: /");
        break;

    case 'vilsbodyinfomanualinput':
        if ($GL_CONFIG_ENABLE_BODYINFO)
            include ('vilsbodyinfomanualinput.php');
        else
            header("Location: /");
        break;

    case 'vilsbodyinfoforacare':
        if ($GL_CONFIG_ENABLE_FORACARE)
            include ('vilsbodyinfoforacare.php');
        else
            header("Location: /");
        break;

    case 'vilspushnotification':

        $nodeid = $_GET['nodeid'];
        $title = $_GET['title'];
        $message = $_GET['message'];

        if ($GL_CONFIG_ENABLE_PUSHNOTIFICATION)
            include ('vilspushnotification.php');
        else
            header("Location: /");
        break;

    case 'vilspushnotificationrecords':
        if ($GL_CONFIG_ENABLE_PUSHNOTIFICATION)
            include ('vilspushnotificationrecords.php');
        else
            header("Location: /");
        break;

    case 'vilssurvey':
        if ($GL_CONFIG_ENABLE_SURVEY)
            include ('vilssurvey.php');
        else
            header("Location: /");
        break;

    case 'vilssurveyrecords':
        if ($GL_CONFIG_ENABLE_SURVEY)
            include ('vilssurveyrecords.php');
        else
            header("Location: /");
        break;

    case 'vilssurveyresult':

        $surveyid = $_GET['surveyid'];
        $hashkey = $_GET['hashkey'];

        if ($GL_CONFIG_ENABLE_SURVEY)
            include ('vilssurveyresult.php');
        else
            header("Location: /");
        break;

    case 'vilsassist':
        if ($GL_CONFIG_ENABLE_ASSIST)
            include ('vilsassist.php');
        else
            header("Location: /");
        break;

    case 'vilsassistrecords':
        if ($GL_CONFIG_ENABLE_ASSIST)
            include ('vilsassistrecords.php');
        else
            header("Location: /");
        break;

    case 'vilsassistrecorddetail':

        $assistid = $_GET['assistid'];
        $assisteventid = $_GET['assisteventid'];

        if ($GL_CONFIG_ENABLE_ASSIST)
            include ('vilsassistrecorddetail.php');
        else
            header("Location: /");
        break;

    case 'vilsbillboard':
        if ($GL_CONFIG_ENABLE_BILLBOARD)
            include ('vilsbillboard.php');
        else
            header("Location: /");
        break;

    case 'vilsbillboardrecords':
        if ($GL_CONFIG_ENABLE_BILLBOARD)
            include ('vilsbillboardrecords.php');
        else
            header("Location: /");
        break;

    case 'vilscurriculum':
        if ($GL_CONFIG_ENABLE_CURRICULUM)
            include ('vilscurriculum.php');
        else
            header("Location: /");
        break;

    case 'vilscurriculumrecords':
        if ($GL_CONFIG_ENABLE_CURRICULUM)
            include ('vilscurriculumrecords.php');
        else
            header("Location: /");
        break;

    case 'vilslocation':
        if ($GL_CONFIG_ENABLE_LOCATION)
            include ('vilslocation.php');
        else
            header("Location: /");

        break;
    case 'vilsranging':
        if ($GL_CONFIG_ENABLE_RANGING)
            include ('vilsranging.php');
        else
            header("Location: /");
        break;

    case 'vils2dmap':
        $nodeid = $_GET['nodeid'];
        if ($GL_CONFIG_ENABLE_2DMAP)
            include ('vils2dmap.php');
        else
            header("Location: /");
        break;

    case 'vils2dmapdashboard':
        $nodeid = $_GET['nodeid'];
        if ($GL_CONFIG_ENABLE_2DMAP)
            include ('vils2dmapdashboard.php');
        else
            header("Location: /");
        break;

    case 'vilsfieldcontrol':
        if ($GL_CONFIG_ENABLE_FIELDCONTROLL)
            include ('vilsfieldcontrol.php');
        else
            header("Location: /");
        break;

    case 'vilsfirefightingcreate':
        if ($GL_CONFIG_ENABLE_FIREFIGHTING)
            include ('vilsfirefightingcreate.php');
        else
            header("Location: /");
        break;

    case 'vilsfirefightingrecord':
        if ($GL_CONFIG_ENABLE_FIREFIGHTING)
            include ('vilsfirefightingrecord.php');
        else
            header("Location: /");
        break;

    case 'vilsfirefightinghistory':
        if ($GL_CONFIG_ENABLE_FIREFIGHTING)
            include ('vilsfirefightinghistory.php');
        else
            header("Location: /");
        break;

    case 'vilsfirefightingreport':
        if ($GL_CONFIG_ENABLE_FIREFIGHTING)
            include ('vilsfirefightingreport.php');
        else
            header("Location: /");
        break;

    case 'vilsversionhistory':
        if ($GL_CONFIG_ENABLE_VERSION_HISTORY)
            include ('vilsversionhistory.php');
        else
            header("Location: /");
        break;

    case 'vils3dmap':
        if ($GL_CONFIG_ENABLE_3DMAP)
            include ('vils3dmap.php');
        else
            header("Location: /");
        break;

    case 'vilsactivity':
        if ($GL_CONFIG_ENABLE_ACTIVITY)
            include ('vilsactivity.php');
        else
            header("Location: /");
        break;

    case 'vilsmaintenance':
        if ($GL_CONFIG_ENABLE_MAINTENANCE)
            include ('vilsmaintenance.php');
        else
            header("Location: /");
        break;

    case 'vilssections':
        if ($GL_CONFIG_ENABLE_SECTION)
            include ('vilssections.php');
        else
            header("Location: /");
        break;

    // case 'vilscheckpoint':
    //     include ('vilscheckpointpath.php');
    //     break;
    // case 'vilscheckpointrecord':
    //     include ('vilscheckpointrecord.php');
    //     break;
    // case 'vilscheckpointhistory':
    //     include ('vilscheckpointhistory.php');
    //     break;
    case 'vilsinspectionreport':
        if ($GL_CONFIG_ENABLE_INSPECTION)
            include ('vilsinspectionreport.php');
        else
            header("Location: /");
        break;

    case 'vilsinspection':
        if ($GL_CONFIG_ENABLE_INSPECTION)
            include ('vilsinspectionpath.php');
        else
            header("Location: /");
        break;

    case 'vilsinspectionrecord':
        if ($GL_CONFIG_ENABLE_INSPECTION)
            include ('vilsinspectionrecord.php');
        else
            header("Location: /");
        break;

    case 'vilsinspectionhistory':
        if ($GL_CONFIG_ENABLE_INSPECTION)
            include ('vilsinspectionhistory.php');
        else
            header("Location: /");
        break;

    case 'vilsfencealert':
        if ($GL_CONFIG_ENABLE_FENCE)
            include ('vilsfencealert.php');
        else
            header("Location: /");
        break;

    case 'vilsstorageimage':
        if ($GL_CONFIG_ENABLE_FILESTORAGE)
            include ('vilsstorageimage.php');
        else
            header("Location: /");
        break;

    case 'vilseventreport':
        include ('vilseventreport.php');
        break;

    case 'vilspowermeterview':
        include ('vilspowermeterview.php');
        break;

    case 'vilslightdevicecontrol':
        include ('vilslightdevicecontrol.php');
        break;

    case 'vilswatchlocation':
        include ('vilswatchlocation.php');
        break;

    case 'vilsstoragefile':
        if ($GL_CONFIG_ENABLE_FILESTORAGE)
            include ('vilsstoragefile.php');
        else
            header("Location: /");
        break;

    case 'vilssettings':
        if ($GL_CONFIG_ENABLE_SETTING)
            include ('vilssettings.php');
        else
            header("Location: /");
        break;

    case 'vilsdisetting':
        if ($GL_CONFIG_ENABLE_SETTING)
            include ('vilsdisetting.php');
        else
            header("Location: /");
        break;

    case 'vilspushnotify':
        if ($GL_CONFIG_ENABLE_SETTING && $GL_CONFIG_ENABLE_PUSHNOTIFICATION)
            include ('vilspushnotify.php');
        else
            header("Location: /");
        break;

    case 'vilsfootprinttags':
        if ($GL_CONFIG_ENABLE_FOOTPRINT)
            include ('vilsfootprinttags.php');
        else
            header("Location: /");
        break;

    case 'vilsfootprinttaggps':
        $date = $_GET['date'];
        $type = $_GET['type'];
        $nodeid = $_GET['nodeid'];
        $macaddress = $_GET['macaddress'];

        if ($GL_CONFIG_ENABLE_FOOTPRINT)
            include ('vilsfootprinttaggps.php');
        else
            header("Location: /");
        break;

    case 'vilsfootprintcourses':
        if ($GL_CONFIG_ENABLE_FOOTPRINT)
            include ('vilsfootprintcourses.php');
        else
            header("Location: /");
        break;

    case 'vilsfootprintareas':
        if ($GL_CONFIG_ENABLE_FOOTPRINT)
            include ('vilsfootprintareas.php');
        else
            header("Location: /");
        break;

    case 'vilsfootprintinout':
        if ($GL_CONFIG_ENABLE_FOOTPRINT)
            include ('vilsfootprintinout.php');
        else
            header("Location: /");
        break;

    case 'vilsfootprintgps':
        if ($GL_CONFIG_ENABLE_FOOTPRINT)
            include ('vilsfootprintgps.php');
        else
            header("Location: /");
        break;

    case 'vilsfootprint':
        if ($GL_CONFIG_ENABLE_FOOTPRINT)
            include ('vilsfootprint.php');
        else
            header("Location: /");
        break;

    case 'vilscourses':
        if ($GL_CONFIG_ENABLE_COURSE)
            include ('vilscourses.php');
        else
            header("Location: /");
        break;

    case 'vilsweekschedule':
        if ($GL_CONFIG_ENABLE_WEEK_SCHEDULE)
            include ('vilsweekschedule.php');
        else
            header("Location: /");
        break;

    case 'vilsapplicationformleave':
        if ($GL_CONFIG_ENABLE_APPLICATION_FORM)
            include ('vilsapplicationformleave.php');
        else
            header("Location: /");
        break;

    case 'vilsevents':
        if ($GL_CONFIG_ENABLE_EVENT)
            include ('vilsevents.php');
        else
            header("Location: /");
        break;

    case 'vilseventtimeline':
        include ('vilseventtimeline.php');
        break;
    case 'vilsalerteventtimeline':
        include ('vilsalerteventtimeline.php');
        break;

    case 'vilsdiagnosis':
        if ($GL_CONFIG_ENABLE_ENGINEERMODE)
            include ('vilsdiagnosis.php');
        else
            header("Location: /");
        break;

    case 'vilsanalyzer':
        if ($GL_CONFIG_ENABLE_ENGINEERMODE)
            include ('vilsanalyzer.php');
        else
            header("Location: /");
        break;

    case 'vilsrollcall':
        $areaid = $_GET['areaid'];

        include ('vilsrollcall.php');
        break;

    case 'vilssensorreport':
        include ('vilssensorreport.php');
        break;

    case 'vilssensorreportmonthly':
        include ('vilssensorreportmonthly.php');
        break;

    case 'vilsunlockpage':
        include ('vilsunlockpage.php');
        break;
    default:
        include ('vils.php');
    }

}

?>
