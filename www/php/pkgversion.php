<?php

    session_start();

    require_once("common.php");

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
        if (isset($_GET['fetchsystemtime']))
        {
            date_default_timezone_set('Asia/Taipei');

            $json_orig = [];
            $json_orig['systemtime'] = date('Y/m/d H:i:s', time());

            echo json_encode($json_orig);
        }
        else
        {
            $app_version = getServerInfo('appversion');

            global $APP_BIN_FILE;
            $vilscenterapp = $APP_BIN_FILE;
            exec("ps aux | grep \"${vilscenterapp}\" | grep -v grep | awk '{ print $2 }' | head -1", $out);

            $json_orig = [];
            $json_orig['version'] = $app_version;
            $json_orig['pid'] = $out[0];

            echo json_encode($json_orig);
        }
    }

    function VILSNODES_do_PUT()
    {

    }

    function VILSNODES_do_POST()
    {

    }

    function VILSNODES_do_DELETE()
    {

    }



?>
