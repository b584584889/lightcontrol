<?php
//ini_set('mysql.connect_timeout', 300);
//ini_set('default_socket_timeout', 300);

class DB
{
    var $_dbConn = 0;
    var $_queryResource = 0;

    function __construct($projectid, $type)
    {
        global $_DB;

        $dbname = '';
        if ($type == 'base')
        {
            $dbname = $_DB['dbname'];
        }
        // else
        // if ($type == 'ts')
        // {
        //     $dbname = $_DB['dbname_ts'];
        // }
        else
        if ($type == 'fp')
        {
            $dbname = $_DB['dbname_fp'];
        }
        else
        if ($type == 'track')
        {
            $dbname = $_DB['dbname_track'];
        }
        else
        if ($type == 'config')
        {
            $dbname = $_DB['dbname_config'];
        }
        else
        if ($type == 'arinspection')
        {
            $dbname = $_DB['dbname_arinspection'];
        }

        if ( $projectid != 1 )
        {
            $dbname = $dbname . '_' . $projectid;
        }

        return DB::connect_db($_DB['host'], $_DB['username'], $_DB['password'], $dbname);
    }

    function connect_db($host, $user, $pwd, $dbname)
    {
        $dbConn = mysqli_connect($host, $user, $pwd, $dbname);

        // Check connection
        if (mysqli_connect_errno())
        {
            echo "Failed to connect to MySQL: " . mysqli_connect_error() . "\n";
        }
        if (! $dbConn)
            die ("MySQL Connect Error!");

        //mysqli_query($dbConn, "SET NAMES utf8");
        //mysqli_query($dbConn, "SET CHARACTER_SET_CLIENT=utf8");
        //mysqli_query($dbConn, "SET CHARACTER_SET_RESULTS=utf8");

        $this->_dbConn = $dbConn;
        return true;
    }

    function query($sql)
    {
        if (! $queryResource = mysqli_query($this->_dbConn, $sql))
        {
            //die ("MySQL Query Error ". mysqli_error($this->_dbConn) . "\n");
        }
        $this->_queryResource = $queryResource;
        return $queryResource;
    }

    function affected_rows()
    {
        return mysqli_affected_rows($this->_dbConn);
    }

    /** Get array return by MySQL */
    function fetch_array()
    {
        return mysqli_fetch_array($this->_queryResource, MYSQLI_ASSOC);
    }

    function get_num_rows()
    {
        return mysqli_num_rows($this->_queryResource);
    }

    /** Get the cuurent id */
    function get_insert_id()
    {
        return mysqli_insert_id($this->_dbConn);
    }

    function close()
    {
        return mysqli_close($this->_dbConn);
    }

}
?>
