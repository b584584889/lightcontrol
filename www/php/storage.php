<?php

session_start();

require_once("../config.php");
require_once("common.php");
require_once("User.php");

$method = $_SERVER['REQUEST_METHOD'];

switch ($method)
{
    case 'GET':
        STORAGE_do_GET();
    break;
    case 'PUT':
    break;
    case 'POST':
        STORAGE_do_POSt();
    break;
    case 'DELETE':
    break;
}

function STORAGE_do_GET()
{
    $project_id = 1;
    if (isset($_GET['project_id']))
        $project_id = (int)$_GET['project_id'];

    $session_accountid = $_SESSION["accountid"];
    $session_userid = $_SESSION["userid"];
    $session_groupid = $_SESSION["groupid"];

    if (isset($_GET['loadfile']))
    {
        $subgroups = '';

        // $targetgroupids = load_view_access_subgroupids($project_id, $session_accountid);
        $targetgroupids = load_view_access_subgroupids_ById($project_id, $session_userid);

        // $targetgroupidsring = implode(",", $targetgroupids);
        // echo $targetgroupidsring;

        // if ($session_groupid == 3)
        // {
        //     $subgroups = getUserSubGroups($project_id, $session_accountid);
        // }
        // else
        // {
        //     // get user datagroup
        //     $subgroups = getUserDataGroups($project_id, $session_accountid);
        // }
        // $subgroupids = explode(",", $subgroups);

        $records = query_files($project_id, $session_accountid, $session_userid, $targetgroupids);

        echo json_encode($records);
    }
    else
    if (isset($_GET['loadsinglefile']))
    {
        if (isset($_GET['fileuid']))
        {
            $records = getstoragefileFromuid($project_id, $_GET['fileuid']);

            echo json_encode($records);
        }
        else
        {
            echo '[]';
        }
    }
    else
    if (isset($_GET['loadimage']))
    {
        $subgroups = '';
        if ($session_groupid == 3)
        {
            // $subgroups = getUserSubGroups($project_id, $session_accountid);
            $subgroups = getUserSubGroups_ById($project_id, $session_userid);
        }
        else
        {
            // get user datagroup
            // $subgroups = getUserDataGroups($project_id, $session_accountid);
            $subgroups = getUserDataGroups_ById($project_id, $session_userid);
        }
        $subgroupids = explode(",", $subgroups);
        $records = query_images($project_id, $session_accountid, $session_userid, $subgroupids);

        echo json_encode($records);
    }
    else
    if (isset($_GET['loadsingleimage']))
    {
        if (isset($_GET['imageuid']))
        {
            $records = getstorageimageFromuid($project_id, $_GET['imageuid']);

            echo json_encode($records);
        }
        else
        {
            echo '[]';
        }
    }
    else
    if (isset($_GET['loadnotifyimage']))
    {
        $messagehashkey = $_GET['messagehashkey'];

        $records = getstorageimageFrommessagehashkey($project_id, $messagehashkey);
        echo json_encode($records);
    }
}


function STORAGE_do_POST()
{
    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    if (isset($_POST['delete_image']))
    {
        $imageid = 0;
        if (isset($_POST['imageid']))
        {
            $imageid = $_POST['imageid'];

            delete_image($project_id, $imageid);
        }
    }
    else
    if (isset($_POST['update_image']))
    {
        $imageid = 0;
        if (isset($_POST['imageid']))
        {
            $imageid = $_POST['imageid'];
            $name = $_POST['name'];
            $description = $_POST['description'];
            $subgroups = $_POST['subgroups'];

            update_image($project_id, $imageid, $name, $description, $subgroups);
        }
    }
    else
    if (isset($_POST['delete_file']))
    {
        $fileid = 0;
        if (isset($_POST['fileid']))
        {
            $fileid = $_POST['fileid'];

            delete_file($project_id, $fileid);
        }
    }
    else
    if (isset($_POST['update_file']))
    {
        $fileid = 0;
        if (isset($_POST['fileid']))
        {
            $fileid = $_POST['fileid'];
            $name = $_POST['name'];
            $description = $_POST['description'];
            $subgroups = $_POST['subgroups'];

            update_file($project_id, $fileid, $name, $description, $subgroups);
        }
    }
}

function query_files($project_id, $accountid, $userid, $subgroupids)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM StorageFile WHERE uploadtype=0 ORDER BY updatetime DESC ");

    $addall = false;
    foreach ($subgroupids as $groupid)
    {
        if ( $groupid == '99999' )
        {
            $addall = true;
        }
    }

    $datacnt = 0;
    while($result = $db->fetch_array())
    {
        if ($addall == false)
        {
            $canadd = false;
            $file_subgroups = $result['subgroups'];
            if (strlen($file_subgroups) == 0 )
            {
                // only myself can see
                if ( $result['userid'] == 0 )
                {
                    if ( $result['accountid'] == $accountid )
                    {
                        $canadd = true;
                    }
                }
                else
                if ( $result['userid'] == $userid )
                {
                    $canadd = true;
                }
            }
            else
            {
                $cur_subgroups = explode(",", $file_subgroups);

                foreach ($subgroupids as $groupid)
                {
                    foreach ($cur_subgroups as $subgroupid)
                    {
                        if ( $groupid == $subgroupid || $subgroupid == '99999' )
                        {
                            $canadd = true;
                            break;
                        }
                    }
                    if ($canadd)
                    {
                        break;
                    }
                }
            }

            if ($canadd == false)
            {
                continue;
            }
        }

        $record = [];
        $record['id'] = $result['id'];
        $record['name'] = $result['name'];
        $record['description'] = $result['description'];
        $record['filename'] = $result['filename'];
        //$record['fileuid'] = $result['fileuid'];
        $record['filetype'] = $result['filetype'];
        $record['filesize'] = $result['filesize'];
        //$record['filepath'] = $result['filepath'];
        $record['accountid'] = $result['accountid'];
        $record['userid'] = $result['userid'];
        $record['subgroups'] = $result['subgroups'];
        $record['datetime'] = $result['datetime'];
        $record['updatetime'] = $result['updatetime'];

        $records[$datacnt] = $record;
        $datacnt++;
    }

    return $records;
}

function query_images($project_id, $accountid, $userid, $subgroupids)
{
    $records = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM StorageImage WHERE uploadtype=0 ORDER BY updatetime DESC ");

    $addall = false;
    foreach ($subgroupids as $groupid)
    {
        if ( $groupid == '99999' )
        {
            $addall = true;
        }
    }

    $datacnt = 0;
    while($result = $db->fetch_array())
    {
        if ($addall == false)
        {
            $canadd = false;
            $image_subgroups = $result['subgroups'];
            if (strlen($image_subgroups) == 0 )
            {
                // only myself can see
                if ( $result['userid'] == 0 )
                {
                    if ( $result['accountid'] == $accountid )
                    {
                        $canadd = true;
                    }
                }
                else
                if ( $result['userid'] == $userid )
                {
                    $canadd = true;
                }
            }
            else
            {
                $cur_subgroups = explode(",", $image_subgroups);

                foreach ($subgroupids as $groupid)
                {
                    foreach ($cur_subgroups as $subgroupid)
                    {
                        if ( $groupid == $subgroupid || $subgroupid == '99999')
                        {
                            $canadd = true;
                            break;
                        }
                    }
                    if ($canadd)
                    {
                        break;
                    }
                }
            }

            if ($canadd == false)
            {
                continue;
            }
        }

        $record = [];
        $record['id'] = $result['id'];
        $record['name'] = $result['name'];
        $record['description'] = $result['description'];
        $record['imagename'] = $result['imagename'];
        //$record['imageuid'] = $result['imageuid'];
        $record['imagetype'] = $result['imagetype'];
        $record['imagewidth'] = $result['imagewidth'];
        $record['imageheight'] = $result['imageheight'];
        $record['imagesize'] = $result['imagesize'];
        //$record['imagepath'] = $result['imagepath'];
        $record['thumbpath'] = $result['thumbpath'];
        $record['accountid'] = $result['accountid'];
        $record['userid'] = $result['userid'];
        $record['subgroups'] = $result['subgroups'];
        $record['datetime'] = $result['datetime'];
        $record['updatetime'] = $result['updatetime'];

        $records[$datacnt] = $record;
        $datacnt++;
    }

    return $records;
}

function getstoragefile($project_id, $fileid)
{
    $storagefile = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM StorageFile WHERE id=$fileid ");

    while($result = $db->fetch_array())
    {
        $storagefile['id'] = $result['id'];
        $storagefile['name'] = $result['name'];
        $storagefile['description'] = $result['description'];
        $storagefile['filename'] = $result['filename'];
        $storagefile['fileuid'] = $result['fileuid'];
        $storagefile['filetype'] = $result['filetype'];
        $storagefile['filesize'] = $result['filesize'];
        $storagefile['filepath'] = $result['filepath'];
        $storagefile['accountid'] = $result['accountid'];
        $storagefile['userid'] = $result['userid'];
        $storagefile['subgroups'] = $result['subgroups'];
        $storagefile['datetime'] = $result['datetime'];
        $storagefile['updatetime'] = $result['updatetime'];
    }

    return $storagefile;
}

function getstoragefileFromuid($project_id, $fileuid)
{
    $storagefile = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM StorageFile WHERE fileuid=$fileuid ");

    while($result = $db->fetch_array())
    {
        $storagefile['id'] = $result['id'];
        $storagefile['name'] = $result['name'];
        $storagefile['description'] = $result['description'];
        $storagefile['filename'] = $result['filename'];
        $storagefile['fileuid'] = $result['fileuid'];
        $storagefile['filetype'] = $result['filetype'];
        $storagefile['filesize'] = $result['filesize'];
        $storagefile['filepath'] = $result['filepath'];
        $storagefile['accountid'] = $result['accountid'];
        $storagefile['userid'] = $result['userid'];
        $storagefile['subgroups'] = $result['subgroups'];
        $storagefile['datetime'] = $result['datetime'];
        $storagefile['updatetime'] = $result['updatetime'];
    }

    return $storagefile;
}

function getstorageimage($project_id, $imageid)
{
    $storagefile = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM StorageImage WHERE id=$imageid ");

    while($result = $db->fetch_array())
    {
        $storagefile['id'] = $result['id'];
        $storagefile['name'] = $result['name'];
        $storagefile['description'] = $result['description'];
        $storagefile['imagename'] = $result['imagename'];
        $storagefile['imageuid'] = $result['imageuid'];
        $storagefile['imagetype'] = $result['imagetype'];
        $storagefile['imagewidth'] = $result['imagewidth'];
        $storagefile['imageheight'] = $result['imageheight'];
        $storagefile['imagesize'] = $result['imagesize'];
        $storagefile['imagepath'] = $result['imagepath'];
        $storagefile['thumbpath'] = $result['thumbpath'];
        $storagefile['accountid'] = $result['accountid'];
        $storagefile['userid'] = $result['userid'];
        $storagefile['subgroups'] = $result['subgroups'];
        $storagefile['datetime'] = $result['datetime'];
        $storagefile['updatetime'] = $result['updatetime'];
    }

    return $storagefile;
}

function getstorageimageFromuid($project_id, $imageuid)
{
    $storagefile = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM StorageImage WHERE imageuid=$imageuid ");

    while($result = $db->fetch_array())
    {
        $storagefile['id'] = $result['id'];
        $storagefile['name'] = $result['name'];
        $storagefile['description'] = $result['description'];
        $storagefile['imagename'] = $result['imagename'];
        $storagefile['imageuid'] = $result['imageuid'];
        $storagefile['imagetype'] = $result['imagetype'];
        $storagefile['imagewidth'] = $result['imagewidth'];
        $storagefile['imageheight'] = $result['imageheight'];
        $storagefile['imagesize'] = $result['imagesize'];
        $storagefile['imagepath'] = $result['imagepath'];
        $storagefile['thumbpath'] = $result['thumbpath'];
        $storagefile['accountid'] = $result['accountid'];
        $storagefile['userid'] = $result['userid'];
        $storagefile['subgroups'] = $result['subgroups'];
        $storagefile['messagehashkey'] = $result['messagehashkey'];
        $storagefile['topic'] = $result['topic'];
        $storagefile['datetime'] = $result['datetime'];
        $storagefile['updatetime'] = $result['updatetime'];
    }

    return $storagefile;
}

function getstorageimageFrommessagehashkey($project_id, $messagehashkey)
{
    $storagefiles = [];

    $db = new DB($project_id, 'base');

    $db->query("SELECT * FROM StorageImage WHERE messagehashkey='$messagehashkey' ");

    $datacnt = 0;
    while($result = $db->fetch_array())
    {
        $storagefile['id'] = $result['id'];
        $storagefile['name'] = $result['name'];
        $storagefile['description'] = $result['description'];
        $storagefile['imagename'] = $result['imagename'];
        $storagefile['imageuid'] = $result['imageuid'];
        $storagefile['imagetype'] = $result['imagetype'];
        $storagefile['imagewidth'] = $result['imagewidth'];
        $storagefile['imageheight'] = $result['imageheight'];
        $storagefile['imagesize'] = $result['imagesize'];
        $storagefile['imagepath'] = $result['imagepath'];
        $storagefile['thumbpath'] = $result['thumbpath'];
        $storagefile['accountid'] = $result['accountid'];
        $storagefile['userid'] = $result['userid'];
        $storagefile['subgroups'] = $result['subgroups'];
        $storagefile['messagehashkey'] = $result['messagehashkey'];
        $storagefile['topic'] = $result['topic'];
        $storagefile['datetime'] = $result['datetime'];
        $storagefile['updatetime'] = $result['updatetime'];

        $storagefiles[$datacnt] = $storagefile;
        $datacnt++;
    }

    return $storagefiles;
}

function delete_image($project_id, $imageid)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM StorageImage WHERE id=$imageid ");

}

function update_image($project_id, $imageid, $name, $description, $subgroups)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE StorageImage set name='$name', description='$description', subgroups='$subgroups' WHERE id=$imageid ");

}

function delete_file($project_id, $fileid)
{
    $db = new DB($project_id, 'base');

    $db->query("DELETE FROM StorageFile WHERE id=$fileid ");

}

function update_file($project_id, $fileid, $name, $description, $subgroups)
{
    $db = new DB($project_id, 'base');

    $db->query("UPDATE StorageFile set name='$name', description='$description', subgroups='$subgroups' WHERE id=$fileid ");

}

?>
