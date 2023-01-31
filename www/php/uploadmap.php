<?php

session_start();

require_once("../config.php");
require_once("common.php");

header('Content-Type: application/json; charset=utf-8');

if (isset($_FILES['imageUpload']))
{
    global $GL_CONFIG;
    $target_Path = $GL_CONFIG['IMAGE_UPLOAD_PATH'];

    $upload_img_name = $_FILES['imageUpload']['name'];
    //$target_FilePath = $target_Path.basename( $upload_img_name );
    $target_FilePath = $target_Path . $upload_img_name;
    move_uploaded_file( $_FILES['imageUpload']['tmp_name'], $target_FilePath );

    if(strlen($upload_img_name) > 0)
    {
        $url_img_file = '/upload/' . $upload_img_name;

        $project_id = 1;
        if (isset($_POST['project_id']))
            $project_id = (int)$_POST['project_id'];

        $maplayerid = 1;
        if (isset($_POST['maplayerid']))
            $maplayerid = (int)$_POST['maplayerid'];

        $maplayer = getMapLayer($project_id, $maplayerid);

        $maplayer['project_id'] = $project_id;
        $maplayer['FILE'] = $url_img_file;
        $maplayer['MAPWIDTH'] = $_POST['upload_img_width'];
        $maplayer['MAPHEIGHT'] = $_POST['upload_img_height'];
        updateMapLayer($maplayer);

        echo json_encode($maplayer);
    }
}

?>
