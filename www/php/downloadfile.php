<?php

session_start();

require_once("common.php");
require_once("User.php");
require_once("storage.php");
require_once("pushnotify.php");

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
        DOWNLOADFILE_do_GET();
    break;
    case 'PUT':
    break;
    case 'POST':
        DOWNLOADFILE_do_POST();
    break;
    case 'DELETE':
    break;
}

function DOWNLOADFILE_do_GET()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    if (isset($_GET['fileid']))
    {
        $fileid = (int)$_GET['fileid'];

        $storagefile = getstoragefile($project_id, $fileid);

        create_download_content($storagefile['filepath'], $storagefile['filename']);
    }
    else
    if (isset($_GET['imageid']))
    {
        $imageid = (int)$_GET['imageid'];
        if (isset($_GET['loadthumb']))
        {
            $storageimage = getstorageimage($project_id, $imageid);

            create_download_content($storageimage['thumbpath'], $storageimage['filename']);
        }
        else
        {
            $storageimage = getstorageimage($project_id, $imageid);

            create_download_content($storageimage['imagepath'], $storageimage['imagename']);
        }
    }
    else
    if (isset($_GET['imageuid']))
    {
        $imageuid = (int)$_GET['imageuid'];
        if (isset($_GET['loadthumb']))
        {
            $storageimage = getstorageimageFromuid($project_id, $imageuid);

            create_download_content($storageimage['thumbpath'], $storageimage['filename']);
        }
        else
        if (isset($_GET['loadcomment']))
        {
            //echo "可以手動輸入理量測, 血糖,收縮壓,舒張壓,體溫,體脂率,血氧";
            $storageimage = getstorageimageFromuid($project_id, $imageuid);
            echo $storageimage['description'];
        }
        else
        {
            $storageimage = getstorageimageFromuid($project_id, $imageuid);

            create_download_content($storageimage['imagepath'], $storageimage['imagename']);
        }
    }
    else
    if (isset($_GET['topic']))
    {
        $topic = $_GET['topic'];
        $TargetTopic = getTargetTopic($project_id, $topic);
        //$userid = getUserIdFromTopic($project_id, $topic);
        if (isset($_GET['type']))
        {
            if ($_GET['type'] == 'getgroupinfo')
            {
                get_groupinfo($project_id, $TargetTopic);
            }
            else
            {
                echo '';
                exit;
            }
        }
        else
        {
            $loadthumb = 0;
            if (isset($_GET['loadthumb']))
                $loadthumb = 1;
            create_imagelink($project_id, $TargetTopic, $loadthumb);
        }
    }
}

function DOWNLOADFILE_do_POST()
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    if (isset($_POST['type']))
    {
        if ($_POST['type'] == 'updateuserphoto')
        {
            $imageuid = '';
            $topic = '';
            if (isset($_POST['imageuid']))
                $imageuid = $_POST['imageuid'];
            if (isset($_POST['topic']))
                $topic = $_POST['topic'];

            $userid = getUserIdFromTopic($project_id, $topic);
            update_user_imageuid($project_id, $userid, $imageuid);
        }
    }
}

function get_groupinfo($project_id, $TargetTopic)
{
    header('Content-Type: application/json');

    $target = $TargetTopic['target'];
    if (strpos($target, '_SUBGROUP_') !== false)
    {
        $subgroupid = $TargetTopic['accountid'];
        $subgroup = load_subgroup_ById($project_id, $subgroupid);

        $jsondata = [];
        $jsondata['id'] = $subgroup['subgroupid'];
        $jsondata['name'] = $subgroup['subgroupname'];
        $jsondata['description'] = $subgroup['permission'];
        $jsondata['imageuid'] = $subgroup['imageuid'];
        $jsondata['maingroupid'] = $subgroup['maingroupid'];
        $jsondata['welcommessage'] = $subgroup['permission'];
        $json_string = json_encode($jsondata);
        echo $json_string;
    }
    else
    if (strpos($target, '_MAINGROUP_') !== false)
    {
        $maingroupid = $TargetTopic['accountid'];
        $maingroup = load_maingroup_ById($project_id, $maingroupid);

        $jsondata = [];
        $jsondata['id'] = $maingroup['id'];
        $jsondata['name'] = $maingroup['name'];
        $jsondata['description'] = $maingroup['permission'];
        $jsondata['imageuid'] = $maingroup['imageuid'];
        $jsondata['autojoin'] = $maingroup['autojoin'];
        $jsondata['welcommessage'] = $maingroup['welcommessage'];
        $json_string = json_encode($jsondata);
        echo $json_string;
    }
}

function create_imagelink($project_id, $TargetTopic, $loadthumb)
{
    $userid = $TargetTopic['userid'];
    $imageuid = '';
    //if (strlen($userid) == 0)
    if ($TargetTopic['userid'] == '0')
    {
        // check SUBGROUP, MAINGROUP
        $target = $TargetTopic['target'];
        if (strpos($target, '_SUBGROUP_') !== false)
        {
            $subgroupid = $TargetTopic['accountid'];
            // echo 'SUBGROUP=['.$subgroupid.']';
            $imageuid = getSubGroupImageuidFromId($project_id, $subgroupid);
            // echo 'SUBGROUP=['.$subgroupid.'] imageuid='.$imageuid;
        }
        else
        if (strpos($target, '_MAINGROUP_') !== false)
        {
            $maingroupid = $TargetTopic['accountid'];
            // echo 'MAINGROUP=['.$maingroupid.']';
            $imageuid = getMainGroupImageuidFromId($project_id, $maingroupid);
            // echo 'MAINGROUP=['.$maingroupid.'] imageuid='.$imageuid;
        }
        else
        {
            echo '';
            exit;
        }
    }
    else
    {
        $imageuid = getUserImageuidFromUserid($project_id, $userid);
    }

    if (strlen($imageuid) == 0)
    {
        echo '';
        exit;
    }
    else
    if ($loadthumb == 1)
    {
        $storageimage = getstorageimageFromuid($project_id, $imageuid);

        create_download_content($storageimage['thumbpath'], $storageimage['filename']);
    }
    else
    {
        $storageimage = getstorageimageFromuid($project_id, $imageuid);

        create_download_content($storageimage['imagepath'], $storageimage['imagename']);
    }
}

function create_download_content($filename, $targetfilename)
{
    $known_mime_types = array(
            "htm" => "text/html",
            "exe" => "application/octet-stream",
            "zip" => "application/zip",
            "doc" => "application/msword",
            "jpg" => "image/jpg",
            "php" => "text/plain",
            "xls" => "application/vnd.ms-excel",
            "ppt" => "application/vnd.ms-powerpoint",
            "gif" => "image/gif",
            "pdf" => "application/pdf",
            "txt" => "text/plain",
            "html"=> "text/html",
            "png" => "image/png",
            "jpeg"=> "image/jpg"
        );

    $file_extension = strtolower(substr(strrchr($filename,"."),1));

    if(array_key_exists($file_extension, $known_mime_types))
    {
        $mime_type = $known_mime_types[$file_extension];
    }
    else
    {
        $mime_type = "application/force-download";
    };

    header('Pragma: public');
    header('Expires: 0');
    header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
    header('Cache-Control: private', false); // required for certain browsers
    header('Content-Type: ' . $mime_type);

    header('Content-Disposition: attachment; filename="'. $targetfilename . '";');
    header('Content-Transfer-Encoding: binary');
    header('Content-Length: ' . filesize($filename));

    readfile($filename);

    exit;
}

?>
