<?php

session_start();

require_once("../config.php");
require_once("common.php");
require_once("User.php");

if (isset($_FILES['fileUpload']))
{
    global $GL_CONFIG;
    $target_Path = $GL_CONFIG['STORGAE_FILE_UPLOAD_PATH'];

    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    $upload_file_name = $_FILES['fileUpload']['name'];
    $target_Path = $target_Path.$project_id.'/';

    if(!file_exists($target_Path))
    {
        mkdir($target_Path, 0777);
    }

    //==============================================
    $tmp_name = $_FILES['fileUpload']['tmp_name'];
    $size = $_FILES['fileUpload']['size'];
    $name = $_FILES['fileUpload']['name'];
    $sports = $_REQUEST['file'];
    $part = (string)$_REQUEST['num'];
    $uid = (string)$_REQUEST['uid'];

    // $sports = $_GET['file'];
    // $part = (string)$_GET['num'];
    // $uid = (string)$_GET['uid'];
    //$part = split("/\=/", $part);
    //$target_file = $target_path .$part. $sports;

    // Open temp file
    //$target_FilePath = $target_Path.basename( $part.$sports );
    //$out = fopen($target_FilePath, "wb");
    $target_FilePath = $target_Path.basename( $uid.'_'.$sports );
    $out = fopen($target_FilePath, "ab");

    if ( $out )
    {
        // Read binary input stream and append it to temp file
        $in = fopen($tmp_name, "rb");
        if ( $in )
        {
            while ( $buff = fread( $in, 1048576 ) )
            {
                fwrite($out, $buff);
            }
        }
        fclose($in);
        fclose($out);
    }
    //==============================================
    // $target_FilePath = $target_Path.basename( $upload_file_name );
    // move_uploaded_file( $_FILES['fileUpload']['tmp_name'], $target_FilePath );
    //==============================================

    $uploadfile = [];
    //$uploadfile['name'] = $upload_file_name;
    //$uploadfile['FilePath'] = $target_FilePath;
    $uploadfile['File'] = $sports;
    $uploadfile['FileSize'] = filesize($target_FilePath);

    echo json_encode($uploadfile);
}
else
if (isset($_FILES['imageUpload']))
{
    global $GL_CONFIG;
    $target_Path = $GL_CONFIG['STORGAE_IMAGE_UPLOAD_PATH'];

    $project_id = 1;
    if (isset($_POST['project_id']))
        $project_id = (int)$_POST['project_id'];

    $upload_file_name = $_FILES['imageUpload']['name'];
    $target_Path = $target_Path.$project_id.'/';

    if(!file_exists($target_Path))
    {
        mkdir($target_Path, 0777);
    }

    //==============================================
    $tmp_name = $_FILES['imageUpload']['tmp_name'];
    $size = $_FILES['imageUpload']['size'];
    $name = $_FILES['imageUpload']['name'];
    $sports = $_REQUEST['file'];
    $part = (string)$_REQUEST['num'];
    $uid = (string)$_REQUEST['uid'];

    $target_FilePath = $target_Path.basename( $uid.'_'.$sports );
    $out = fopen($target_FilePath, "ab");

    if ( $out )
    {
        // Read binary input stream and append it to temp file
        $in = fopen($tmp_name, "rb");
        if ( $in )
        {
            while ( $buff = fread( $in, 1048576 ) )
            {
                fwrite($out, $buff);
            }
        }
        fclose($in);
        fclose($out);
    }

    $uploadfile = [];
    //$uploadfile['name'] = $upload_file_name;
    //$uploadfile['FilePath'] = $target_FilePath;
    $uploadfile['File'] = $sports;
    $uploadfile['FileSize'] = filesize($target_FilePath);

    echo json_encode($uploadfile);
}
else
if (isset($_POST['createnewfile']))
{
    $project_id = $_POST['project_id'];
    $name = (string)$_POST['name'];
    $description = (string)$_POST['description'];
    $filename = (string)$_POST['filename'];
    $fileuid = (string)$_POST['fileuid'];
    $filetype = (string)$_POST['filetype'];
    $filesize = (int)$_POST['filesize'];
    $subgroups = (string)$_POST['subgroups'];

    $uploadfile = [];
    $uploadfile['accountid'] = $_SESSION["accountid"];
    $uploadfile['project_id'] = $project_id;
    $uploadfile['filename'] = $filename;
    $uploadfile['fileuid'] = $fileuid;
    $uploadfile['name'] = $name;
    $uploadfile['description'] = $description;

    global $GL_CONFIG;
    $target_Path = $GL_CONFIG['STORGAE_FILE_UPLOAD_PATH'];
    $target_Path = $target_Path.$project_id.'/';
    $target_FilePath = $target_Path.basename( $fileuid.'_'.$filename );

    $uploadtype = 0;
    if (strlen($subgroups) == 0)
    {
        if ( isset($_POST['leaveform']) )
        {
            // only read from self
            $uploadtype = 1; // leaveform
            $subgroups = '';
        }
        else
        {
            // get user datagroup
            $subgroups = getUserDataGroups($project_id, $_SESSION["accountid"]);
        }
    }

    create_storagefile_record($project_id, $name, $description, $filename, $fileuid, $filetype, $filesize, $target_FilePath, $_SESSION["accountid"], $_SESSION["userid"], $subgroups, $uploadtype, "", "");

    echo json_encode($uploadfile);
}
else
if (isset($_POST['createnewimage']))
{
    $project_id = $_POST['project_id'];
    $name = (string)$_POST['name'];
    $description = (string)$_POST['description'];
    $filename = (string)$_POST['filename'];
    $fileuid = (string)$_POST['fileuid'];
    $filetype = (string)$_POST['filetype'];
    $filesize = (int)$_POST['filesize'];
    $subgroups = (string)$_POST['subgroups'];

    $uploadfile = [];
    $uploadfile['accountid'] = $_SESSION["accountid"];
    $uploadfile['project_id'] = $project_id;
    $uploadfile['name'] = $name;
    $uploadfile['description'] = $description;
    $uploadfile['filename'] = $filename;
    $uploadfile['fileuid'] = $fileuid;

    global $GL_CONFIG;
    $target_Path = $GL_CONFIG['STORGAE_IMAGE_UPLOAD_PATH'];
    $target_Path = $target_Path.$project_id.'/';

    $target_FilePath = $target_Path.basename( $fileuid.'_'.$filename );
    $thumb_FilePath = $target_Path.basename( $fileuid.'_thumb_'.$filename );

    createThumbnail($target_FilePath, $thumb_FilePath, 180);

    list($image_width, $image_height, $original_type) = getimagesize($target_FilePath);

    $uploadtype = 0;
    if (strlen($subgroups) == 0)
    {
        if ( isset($_POST['leaveform']) )
        {
            // only read from self
            $uploadtype = 1; // leaveform
            $subgroups = '';
        }
        else
        {
            // get user datagroup
            $subgroups = getUserDataGroups($project_id, $_SESSION["accountid"]);
        }
    }

    create_storageimage_record($project_id, $name, $description, $filename, $fileuid, $filetype, $image_width, $image_height, $filesize, $target_FilePath, $thumb_FilePath, $_SESSION["accountid"], $_SESSION["userid"], $subgroups, $uploadtype, "", "");

    echo json_encode($uploadfile);
}
else
if (isset($_POST['createnewimagefromapp']))
{
    $project_id = 1;
    $messagehashkey = (string)$_POST['messagehashkey'];
    $topic = (string)$_POST['topic'];
    $name = (string)$_POST['name'];
    $description = urldecode((string)$_POST['description']);
    $filename = (string)$_POST['filename'];
    $fileuid = (string)$_POST['fileuid'];
    $filetype = (string)$_POST['filetype'];
    $filesize = (int)$_POST['filesize'];
    $subgroups = '';

    list($accountid, $userid) = getAccountInfoFromTopic($project_id, $topic);

    $uploadfile = [];
    $uploadfile['accountid'] = $accountid;
    $uploadfile['messagehashkey'] = $messagehashkey;
    $uploadfile['topic'] = $topic;
    $uploadfile['project_id'] = $project_id;
    $uploadfile['name'] = $name;
    $uploadfile['description'] = $description;
    $uploadfile['filename'] = $filename;
    $uploadfile['fileuid'] = $fileuid;

    global $GL_CONFIG;
    $target_Path = $GL_CONFIG['STORGAE_IMAGE_UPLOAD_PATH'];
    $target_Path = $target_Path.$project_id.'/';

    $target_FilePath = $target_Path.basename( $fileuid.'_'.$filename );
    $thumb_FilePath = $target_Path.basename( $fileuid.'_thumb_'.$filename );

    createThumbnail($target_FilePath, $thumb_FilePath, 180);

    list($image_width, $image_height, $original_type) = getimagesize($target_FilePath);

    $uploadfile['image_width'] = $image_width;
    $uploadfile['image_height'] = $image_height;

    $uploadtype = 2; // from app
    create_storageimage_record($project_id, $name, $description, $filename, $fileuid, $filetype, $image_width, $image_height, $filesize, $target_FilePath, $thumb_FilePath, $accountid, $userid, $subgroups, $uploadtype, $messagehashkey, $topic);

    echo json_encode($uploadfile);

}

//function createThumbnail($filepath, $thumbpath, $thumbnail_width, $thumbnail_height, $background=false)
function createThumbnail($filepath, $thumbpath, $thumbnail_width, $background=false)
{
    list($original_width, $original_height, $original_type) = getimagesize($filepath);

    if ($original_type === 1)
    {
        $imgt = "ImageGIF";
        $imgcreatefrom = "ImageCreateFromGIF";
    }
    else
    if ($original_type === 2)
    {
        $imgt = "ImageJPEG";
        $imgcreatefrom = "ImageCreateFromJPEG";
    }
    else
    if ($original_type === 3)
    {
        $imgt = "ImagePNG";
        $imgcreatefrom = "ImageCreateFromPNG";
    }
    else
    {
        return false;
    }

    // echo '$imgt:'.$imgt.'\n';

    $old_image = $imgcreatefrom($filepath);

    $exif = exif_read_data($filepath);
    if(!empty($exif['Orientation']))
    {
        // echo '$exif[Orientation]:'. $exif['Orientation'].'\n';

        switch($exif['Orientation'])
        {
            case 8:
                $old_image = imagerotate($old_image, 90, 0);
                $tmp = $original_width;
                $original_width = $original_height;
                $original_height = $tmp;
                break;
            case 3:
                $old_image = imagerotate($old_image, 180, 0);
                break;
            case 6:
                $old_image = imagerotate($old_image, -90, 0);
                $tmp = $original_width;
                $original_width = $original_height;
                $original_height = $tmp;
                break;
        }
    }

    // echo '$original_width:'.$original_width.' $original_height:'.$original_height.'\n';

    $thumbnail_height = $thumbnail_width;

    if ($original_width > $original_height)
    {
        $new_width = $thumbnail_width;
        $new_height = intval($original_height * $new_width / $original_width);
        $thumbnail_height = $new_height;
    }
    else
    {
        $new_height = $thumbnail_width;
        $new_width = intval($new_height * $original_width / $original_height);
        $thumbnail_height = $new_height;
        $thumbnail_width = $new_width;
    }
    $dest_x = intval(($thumbnail_width - $new_width) / 2);
    $dest_y = intval(($thumbnail_height - $new_height) / 2);

    // echo '$new_height:'.$new_height.' $new_width:'.$new_width.' $thumbnail_height:'.$thumbnail_height.' $thumbnail_width:'.$thumbnail_width.'\n';
    // echo '$dest_x:'.$dest_x.' $dest_y:'.$dest_y.'\n';
    $new_image = imagecreatetruecolor($thumbnail_width, $thumbnail_height); // creates new image, but with a black background

    // figuring out the color for the background
    if(is_array($background) && count($background) === 3)
    {
        list($red, $green, $blue) = $background;
        $color = imagecolorallocate($new_image, $red, $green, $blue);
        imagefill($new_image, 0, 0, $color);
        // apply transparent background only if is a png image
    }
    else
    if($background === 'transparent' && $original_type === 3)
    {
        imagesavealpha($new_image, TRUE);
        $color = imagecolorallocatealpha($new_image, 0, 0, 0, 127);
        imagefill($new_image, 0, 0, $color);
    }

    imagecopyresampled($new_image, $old_image, $dest_x, $dest_y, 0, 0, $new_width, $new_height, $original_width, $original_height);
    $imgt($new_image, $thumbpath);
    return file_exists($thumbpath);
}

function create_storagefile_record($project_id, $name, $description, $filename, $fileuid, $filetype, $filesize, $target_FilePath, $accountid, $userid, $subgroups, $uploadtype, $messagehashkey, $topic)
{
    $db = new DB($project_id, 'base');

    $db->query("INSERT INTO StorageFile(name, description, filename, fileuid, filetype, filesize, filepath, accountid, userid, subgroups, uploadtype, messagehashkey, topic) " .
               "VALUES ('$name', '$description', '$filename', '$fileuid', '$filetype', $filesize, '$target_FilePath', '$accountid', $userid, '$subgroups', $uploadtype, '$messagehashkey', '$topic') ");
}

function create_storageimage_record($project_id, $name, $description, $filename, $fileuid, $filetype, $image_width, $image_height, $filesize, $target_FilePath, $thumb_FilePath, $accountid, $userid, $subgroups, $uploadtype, $messagehashkey, $topic)
{
    $db = new DB($project_id, 'base');

    $db->query("INSERT INTO StorageImage(name, description, imagename, imageuid, imagetype, imagewidth, imageheight, imagesize, imagepath, thumbpath, accountid, userid, subgroups, uploadtype, messagehashkey, topic) " .
               "VALUES ('$name', '$description', '$filename', '$fileuid', '$filetype', $image_width, $image_height, $filesize, '$target_FilePath', '$thumb_FilePath', '$accountid', $userid, '$subgroups', $uploadtype, '$messagehashkey', '$topic') ");

}

// function getAccountIdFromTopic($project_id, $topic)
function getAccountInfoFromTopic($project_id, $topic)
{
    $db = new DB($project_id, 'base');

    $db->query("SELECT accountid FROM TargetTopic WHERE topic='$topic' ");

    $accountid = '';
    $userid = 0;
    while($result = $db->fetch_array())
    {
        $accountid = $result['accountid'];
        $userid = $result['userid'];
    }
    return array($accountid, $userid);
}

?>
