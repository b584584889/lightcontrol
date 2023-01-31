<?php
    include ('checksession.php');

    $path = ltrim($_SERVER['REQUEST_URI'], '/');    // Trim leading slash(es)
    $elements = explode('/', $path);                // Split path on slashes

    require_once('config.php');

    if(empty($elements[0]))
    {
        echo '';
    }
    else
    if(array_shift($elements))// index.php
    {
        if ( isset($elements[0]) )
            $target = $elements[0];
        else
            $target = $elements;

        if ( $target != 'log' )
        {
            echo '';
        }
        else
        {
            if (!empty($elements[1]))
            {
                //echo 'elements[1]:'.$elements[1];

                $queryArray = explode('?', $elements[1]);

                $action = $queryArray[0];// view?file=vilscenter_20200716_173518_log.txt
                if ( $action != 'view' )
                {
                    echo '';
                }
                else
                {
                    $queryFile = explode('=', $queryArray[1])[1];
                    //echo 'queryFile[0]:'.$queryFile[0];
                    //echo '<br>queryFile[1]:'.$queryFile[1];

                    global $GL_CONFIG;
                    $string = file_get_contents($GL_CONFIG['WEB_LOG_PATH'].$queryFile, true);
                    $lines = explode("\n", $string);
                    foreach($lines as $line)
                    {
                        echo $line . '<br>';
                    }
                }
            }
            else
            {
                global $GL_CONFIG;
                $path = realpath($GL_CONFIG['WEB_LOG_PATH']);
                //foreach (new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path)) as $filename)

                $files = array();
                foreach (new RecursiveDirectoryIterator($path) as $filepathname)
                {
                    $filename = str_replace($path.'/', "", $filepathname);
                    if ( strlen($filename) > 3 )
                    {
                        $files[filemtime($filepathname)] = $filepathname;
                        // $file_datetime = date('Y-m-d H:i:s', filemtime($filepathname));
                        // echo "$file_datetime \t $filename </br>";
                    }
                }

                // sort
                krsort($files);

                // find the last modification
                //$reallyLastModified = end($files);

                foreach($files as $file)
                {
                    //$lastModified = date('Y-m-d H:i:s',filemtime($file));

                    $filename = str_replace($path.'/', "", $file);
                    $filesize = filesize($file);

                    //if(strlen($file)-strpos($file,".swf")== 4)
                    //{
                       // if ($file == $reallyLastModified)
                       // {
                       //   // do stuff for the real last modified file
                       // }
                       echo "<tr><td><a href=log/view?file=$filename target=\"_blank\">$filename</a></td><td>&nbsp;&nbsp;&nbsp;$filesize bytes</td></tr><br>";
                    //}
                }
            }
        }
    }

?>
