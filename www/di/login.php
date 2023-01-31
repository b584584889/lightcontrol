<?php
    session_start();

    require_once("config.php");
    require_once("../php/common.php");
    require_once("../php/User.php");

    // unset($_SESSION["authenticated"]);

    $clientIP = $_SERVER['REMOTE_ADDR'];
    $SO_user = $_SESSION['SO_user'];
    $login_enter_error = false;
    $login_need_userid = false;

    $login_account = '';
    $login_password = '';

    if ( !empty( $_POST ) )
    {
        if ( isset( $_POST['account'] ) && isset( $_POST['password'] ) && isset( $_POST['token'] ) )
        {
            //====================================================
            $userid = '';
            if ( isset( $_POST['userid'] ) )
                $userid = $_POST['userid'];

            $login_account = $_POST['account'];
            $login_password = $_POST['password'];

            list($count, $existeduserid) = check_account_existed(1, $_POST['account']);

            $valid = true;
            if ( ($count > 1) && (strlen($userid) == 0) )
            {
                $login_enter_error = true;
                $login_need_userid = true;

                $valid = false;
            }

            if ( $valid )
            {
                $json_array = [];
                $json_array['account'] = $_POST['account'];
                $json_array['password'] = $_POST['password'];
                if ( $count > 1 )
                    $json_array['userid'] = $_POST['userid'];
                else
                    $json_array['userid'] = $existeduserid;
                $json_string = json_encode($json_array);
                $return_str = getServerInfo('queryaccountloginkey='.$json_string);
                $return_json = json_decode($return_str);
                if ( strcmp("true", $return_json->{'valid'}) == 0)
                {
                    $json_array = [];
                    $json_array['account'] = $_POST['account'];
                    $json_array['userid'] = $_POST['userid'];
                    $json_array['count'] = $count;
                    // $json_array['password'] = $_POST['password'];
                    $json_array['valid'] = $return_json->{'valid'};
                    $json_array['key'] = $return_json->{'key'};

                    $json_string = json_encode($json_array);
                    echo $json_string;
                    exit;
                }
                else
                {
                    $json_array = [];
                    $json_array['account'] = $_POST['account'];
                    // $json_array['password'] = $_POST['password'];
                    // $json_array['token'] = $_POST['token'];
                    $json_array['valid'] = 'false';
                    $json_array['key'] = '';
                    $json_array['errorcode'] = '0'; // not match

                    $json_string = json_encode($json_array);

                    echo $json_string;
                    exit;
                }
            }
            else
            {
                $json_array = [];
                $json_array['account'] = $_POST['account'];
                // $json_array['password'] = $_POST['password'];
                // $json_array['token'] = $_POST['token'];
                $json_array['valid'] = 'false';
                $json_array['key'] = '';
                $json_array['errorcode'] = '1'; //need user id

                $json_string = json_encode($json_array);

                echo $json_string;
                exit;
            }
            //====================================================
            // $json_array = [];
            // $json_array['account'] = $_POST['account'];
            // $json_array['password'] = $_POST['password'];
            // $json_array['token'] = $_POST['token'];
            // $json_string = json_encode($json_array);
            // $return_str = getServerInfo('queryaccountloginkey='.$json_string);
            // $return_json = json_decode($return_str);
            // if ( strcmp("true", $return_json->{'valid'}) == 0)
            // {
            //     $json_array = [];
            //     $json_array['account'] = $_POST['account'];
            //     // $json_array['password'] = $_POST['password'];
            //     $json_array['valid'] = $return_json->{'valid'};
            //     $json_array['key'] = $return_json->{'key'};
            //
            //     $json_string = json_encode($json_array);
            //     echo $json_string;
            //     exit;
            // }
            // else
            // {
            //     $json_array = [];
            //     $json_array['account'] = $_POST['account'];
            //     // $json_array['password'] = $_POST['password'];
            //     // $json_array['token'] = $_POST['token'];
            //     $json_array['valid'] = 'false';
            //     $json_array['key'] = '';
            //
            //     $json_string = json_encode($json_array);
            //
            //     echo $json_string;
            //     exit;
            // }
            //====================================================
        }
        else
        if ( isset( $_POST['account'] ) && isset( $_POST['key'] ) && isset( $_POST['token'] ) )
        {
            $json_array = [];
            $json_array['account'] = $_POST['account'];
            $json_array['key'] = $_POST['key'];
            $json_array['token'] = $_POST['token'];
            $json_string = json_encode($json_array);
            $return_str = getServerInfo('checkaccountloginkey='.$json_string);
            $return_json = json_decode($return_str);
            if ( strcmp("true", $return_json->{'valid'}) == 0)
            {
                $login_enter_error = false;
            }
            else
            {
                $login_enter_error = true;
            }
        }
        else
        if ( isset( $_POST['account'] ) && isset( $_POST['password'] ) )
        {
            //======================================
            $userid = '';
            if ( isset( $_POST['userid'] ) )
                $userid = $_POST['userid'];

            $login_account = $_POST['account'];
            $login_password = $_POST['password'];

            list($count, $existeduserid) = check_account_existed(1, $_POST['account']);

            $valid = true;
            if ( ($count > 1) && (strlen($userid) == 0) )
            {
                $login_enter_error = true;
                $login_need_userid = true;

                $valid = false;
            }

            if ( $valid )
            {
                $json_array = [];
                $json_array['account'] = $_POST['account'];
                $json_array['password'] = $_POST['password'];
                if ( $count > 1 )
                    $json_array['userid'] = $_POST['userid'];
                else
                    $json_array['userid'] = $existeduserid;
                $json_string = json_encode($json_array);
                $return_str = getServerInfo('checkaccountlogin='.$json_string);
                $return_json = json_decode($return_str);
                if ( strcmp("true", $return_json->{'valid'}) == 0)
                {
                    $login_enter_error = false;
                }
                else
                {
                    $login_enter_error = true;
                }
            }
            //======================================
            // $json_array = [];
            // $json_array['account'] = $_POST['account'];
            // $json_array['password'] = $_POST['password'];
            // $json_string = json_encode($json_array);
            // $return_str = getServerInfo('checkaccountlogin='.$json_string);
            // $return_json = json_decode($return_str);
            // if ( strcmp("true", $return_json->{'valid'}) == 0)
            // {
            //     $login_enter_error = false;
            // }
            // else
            // {
            //     $login_enter_error = true;
            // }
            //======================================
        }
        else
        if ( isset( $_POST['account'] ) && isset( $_POST['key'] ) )
        {
            $return_json = checkValidKey($_POST['account'], $_POST['key']);

            $json_array = [];
            $json_array['account'] = $_POST['account'];
            if ( strcmp("true", $return_json->{'valid'}) == 0)
                $json_array['valid'] = 'true';
            else
                $json_array['valid'] = 'false';

            $json_string = json_encode($json_array);

            echo $json_string;
            exit;
        }
        else
        if ( isset( $_POST['querytopic'] ) )
        {
            $account = $_POST['account'];
            $userid = $_POST['userid'];
            $maingroupid = $_POST['maingroupid'];

            list($count, $existeduserid) = check_maingroup_account_existed(1, $maingroupid, $account, $userid);

            $json_array = [];
            $json_array['account'] = $account;
            $json_array['status'] = 0;

            if ( $count > 1 )
            {
                $json_array['status'] = 1;// 需輸入識別碼
            }
            else
            if ( $count == 1 )
            {
                $json_array['status'] = 0;
                $json_array['userid'] = $existeduserid;

                //
                $targettopic = getTopicFromUserid(1, $existeduserid);
                $json_array['target'] = $targettopic['target'];
                $json_array['topic'] = $targettopic['topic'];
                $json_array['name'] = $targettopic['name'];
            }
            else
            {
                $json_array['status'] = 2;// 無此帳號在此主集合
            }

            $json_string = json_encode($json_array);
            echo $json_string;
            exit;
        }
        else
        if ( isset( $_POST['createtopic'] ) )
        {
            $project_id = 1;

            $maingroupid = $_POST['maingroupid'];

            $subgroupid = loadSubGroupIdByMainGroupId($project_id, $maingroupid);

            $record = [];
            $record['project_id'] = $project_id;
            $record['name'] = $_POST['account'];
            $record['account'] = $_POST['account'];
            $record['unit'] = '';
            $record['title'] = '';
            $record['gender'] = '';
            $record['group'] = '3';
            $record['datagroups'] = '';
            $record['subgroups'] = $subgroupid;
            $record['rfidcards'] = '';
            $record['nfccards'] = '';
            $record['uwbcards'] = '';
            $record['sensors'] = '';
            $record['imageuid'] = '';
            $record['maingroups'] = $_POST['maingroupid'];
            $record['maingroupid'] = $_POST['maingroupid'];

            list($count, $userid) = check_account_existed($project_id, $record['account']);

            list( $newuserid, $rfidcards, $assigned_rfidcards, $nfccards, $assigned_nfccards) = save_new_user($project_id, $record);

            $record['userid'] = $newuserid;
            $record['existed'] = ($count > 0);
            $record['rfidcards'] = $rfidcards;
            $record['nfccards'] = $nfccards;
            $record['assigned_rfidcards'] = $assigned_rfidcards;
            $record['assigned_nfccards'] = $assigned_nfccards;

            $json_string = json_encode($record);
            $send_string = 'create_new_user='. $json_string;
            sendMessageToServer($send_string);


            //
            // create QR Code
            //
            $query_array = [];
            $query_array['project_id'] = $project_id;
            $query_array['type'] = "USER";
            $query_array['id'] = $newuserid;
            $query_array['accountid'] = $_POST['account'];
            $query_array['maingroupid'] = $_POST['maingroupid'];

            $json_query = json_encode($query_array);
            $query_result = getServerInfo('loadqrcode='.$json_query);

            $decode_obj = json_decode($query_result);

            $json_array = [];
            $json_array['account'] = $_POST['account'];
            $json_array['userid'] = $newuserid;
            $json_array['status'] = 0;
            $json_array['target'] = $decode_obj->target;
            $json_array['topic'] = $decode_obj->topic;
            $json_array['name'] = $decode_obj->name;
            $json_string = json_encode($json_array);
            echo $json_string;
            exit;
        }

        if ( !$login_enter_error)
        {
            // 寫入 Session 變數值
            $_SESSION['authenticated'] = true;
            $_SESSION['accountid'] = $_POST['account'];
            $_SESSION['accountname'] = $return_json->{'name'};
            $_SESSION['groupid'] = $return_json->{'groupid'};
            $_SESSION['type'] = $return_json->{'type'};
            $_SESSION['userid'] = $return_json->{'userid'};
            $_SESSION['maingroups'] = $return_json->{'maingroups'};
            $_SESSION['subgroups'] = $return_json->{'subgroups'};
            $_SESSION['login_time'] = date('Y-m-d h:i:s');

            $redir = $DI_PATH.'/index.php';

            // 重導到相關頁面
            header("Location: $redir");
            exit;
        }
    }
    else
    if ( isset( $_GET['account'] ) && isset( $_GET['key'] ) )
    {
        $return_json = checkValidKey($_GET['account'], $_GET['key']);

        if ( strcmp("true", $return_json->{'valid'}) == 0)
        {
            // 寫入 Session 變數值
            $_SESSION['authenticated'] = true;
            $_SESSION['accountid'] = $_GET['account'];
            $_SESSION['accountname'] = $return_json->{'name'};
            $_SESSION['groupid'] = $return_json->{'groupid'};
            $_SESSION['type'] = $return_json->{'type'};
            $_SESSION['userid'] = $return_json->{'userid'};
            $_SESSION['maingroups'] = $return_json->{'maingroups'};
            $_SESSION['subgroups'] = $return_json->{'subgroups'};
            $_SESSION['login_time'] = date('Y-m-d h:i:s');

            $redir = $DI_PATH.'/index.php';

            // 重導到相關頁面
            header("Location: $redir");
            exit;
        }
        else
        {
            unset($_SESSION["authenticated"]);
            $_SESSION['accountid'] = $_GET['account'];

            $json_array = [];
            $json_array['account'] = $_GET['account'];
            $json_array['valid'] = 'false';
            $json_string = json_encode($json_array);
            echo $json_string;
            exit;
        }
    }
    else
    if ( isset( $_GET['account'] ) )
    {
        unset($_SESSION["authenticated"]);
        $_SESSION['accountid'] = $_GET['account'];
    }

    function checkValidKey($account, $key)
    {
        $json_array = [];
        $json_array['account'] = $account;
        $json_array['key'] = $key;
        $json_string = json_encode($json_array);
        $return_str = getServerInfo('checkaccountloginkey='.$json_string);
        return $return_json = json_decode($return_str);
    }

    include ('config.php');
?>
<!DOCTYPE HTML>
<html lang="en">
    <?php
        include ('vi_html_head.php');
    ?>

<body class="hold-transition login-page">
    <div class="login-box">
      <div class="login-logo">
        <b><h2><?php echo $login_tilte;?></h2></b></a>
      </div>

      <!-- /.login-logo -->
      <div class="login-box-body">
        <?php
            if ($login_enter_error == true)
            {
                if ( $login_need_userid == true)
                    echo '<div align="center"><h4><span class="label label-danger">帳號需要使用識別碼, 請輸入識別碼.</span></h4></div>';
                else
                    echo '<div align="center"><h4><span class="label label-danger">帳號或密碼錯誤!請重新輸入.</span></h4></div>';
            }
        ?>

        <p class="login-box-msg"><?php echo $login_sign_in_comment;?></p>

        <form action= "<?php echo $DI_PATH ?>/login.php" method="post">
          <div class="form-group has-feedback">
            <input type="text" name="account" class="form-control" placeholder=<?php echo $login_sign_in_account;?> value="<?php echo $_SESSION["accountid"] ?>" required>
            <span class="glyphicon glyphicon-user form-control-feedback"></span>
          </div>
          <div class="form-group has-feedback">
            <input type="password" name="password" class="form-control" placeholder=<?php echo $login_sign_in_password;?> required>
            <span class="glyphicon glyphicon-lock form-control-feedback"></span>
          </div>
          <?php
              if ($GL_CONFIG_ENABLE_EMAIL_ACCOUNT)
              {
                  echo '<div class="form-group has-feedback">';
                  echo '    <input type="text" name="userid" class="form-control" placeholder='; echo $login_sign_in_userid; echo '>';
                  echo '    <span class="glyphicon glyphicon-pencil form-control-feedback"></span>';
                  echo '</div>';
              }
          ?>
          <div class="row">
            <div class="col-xs-8">
                <span class='logo-mini'>
                    <div class='image'>
                      <img src='/images/VILS_logo_s.jpg' class='img-circle' alt='VILS Logo' id='logo'>
                    </div>
                </span>
            </div>
            <!-- /.col -->
            <div class="col-xs-4">
              <button type="submit" class="btn btn-primary btn-block btn-flat"><?php echo $login_sign_in_loginbtn;?></button>
            </div>
            <!-- /.col -->
          </div>
        </form>

      </div>
      <!-- /.login-box-body -->
    </div>
    <!-- /.login-box -->
    <div align="center"><h4><span class="label">Your IP: </span><span class="label label-success"><?php echo $clientIP;?></span></h4></div>

</body>

<script>
  var accountid = "<?php echo $_SESSION["accountid"] ?>";
</script>

</html>
