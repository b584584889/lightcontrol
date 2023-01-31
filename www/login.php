<?php
    session_start();

    $path = ltrim($_SERVER['REQUEST_URI'], '/');    // Trim leading slash(es)
    $elements = explode('/', $path);                // Split path on slashes

    if (!isset($_SESSION['PAGE_CONFIG']))
    {
        $_SESSION['PAGE_CONFIG'] = 'NORGAY';
    }

    if(!empty($elements[0]))
    {
        $requestarray = explode('?',$elements[0]);
        if ( isset($requestarray[1]) )
        {
            $paramarray = explode('&',$requestarray[1]);
            foreach($paramarray as $param)
            {
                $paramdata = explode('=',$param);

                if (count($paramdata) == 2)
                {
                    if ($paramdata[0] == 'site')

                    {
                        $PAGE_CONFIG = $paramdata[1];
                        $_SESSION['PAGE_CONFIG'] = $PAGE_CONFIG;
                    }
                }
            }
        }
    }

    require_once("config.php");
    require_once("./php/common.php");
    require_once("./php/User.php");

    //unset($_SESSION["authenticated"]);

    $clientIP = $_SERVER['REMOTE_ADDR'];
    $SO_user = $_SESSION['SO_user'];
    $login_enter_error = false;
    $login_need_userid = false;

    $login_account = '';
    $login_password = '';

    if ( !empty( $_POST ) )
    {
        
        if ( isset( $_POST['syncERPaccount'] ) )
        {
            $json_array = [];
            $json_array['project_id'] = $_POST['project_id'];
            $json_string = json_encode($json_array);

            sendMessageToServer('syncERPaccount='.$json_string);
            // echo 'sendMessageToServer()';
            exit;
        }
        else
        if ( isset( $_POST['user'] ) )
        {
            $SO_user = $_POST['user'];
        }
        else
        if ( isset( $_POST['account'] ) && isset( $_POST['password'] ) )
        {
            $userid = '';
            if ( isset( $_POST['userid'] ) )
                $userid = $_POST['userid'];

            $login_account = $_POST['account'];
            $login_password = $_POST['password'];

            $login_account = str_replace(' ', '', $login_account);
            $login_password = str_replace(' ', '', $login_password);
            $userid = str_replace(' ', '', $userid);

            list($count, $existeduserid) = check_account_existed(1, $login_account);

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
                $json_array['account'] = $login_account;
                $json_array['password'] = $login_password;


               if ( $count > 1 )
                    $json_array['userid'] = $userid;
                else
                    $json_array['userid'] = $existeduserid;
                $json_string = json_encode($json_array);
                $return_str = getServerInfo('checkaccountlogin='.$json_string);
                $return_json = json_decode($return_str);

                if ( strcmp("true", $return_json->{'valid'}) == 0)
                {
                    echo "<script>console.log('11');</script>";
                    // 寫入 Session 變數值
                    $_SESSION['authenticated'] = true;
                    $_SESSION['accountid'] = $login_account;
                    $_SESSION['accountname'] = $return_json->{'name'};
                    $_SESSION['groupid'] = $return_json->{'groupid'};
                    $_SESSION['type'] = $return_json->{'type'};
                    $_SESSION['userid'] = $return_json->{'userid'};
                    $_SESSION['maingroups'] = $return_json->{'maingroups'};
                    $_SESSION['subgroups'] = $return_json->{'subgroups'};
                    $_SESSION['login_time'] = date('Y-m-d h:i:s');
                    $_SESSION['PAGE_CONFIG'] = $PAGE_CONFIG;

                    // 檢查在 $_SESSION 全域變數中，是否有之前設定好的網址重導 Session 變數
                    // if(isset($_SESSION[UrlRedirect]))
                    // {
                    //     $redir = $_SESSION[UrlRedirect];
                    // }
                    // else
                    // {
                    //     $redir = '/index.php';
                    // }
                    $redir = '/index.php';

                    // 重導到相關頁面
                    header("Location: $redir");
                    exit;
                }
                else
                {
                    //echo '<br>帳號或密碼錯誤!<br>請重新輸入.';
                    $login_enter_error = true;
                    echo "<script>console.log('22');</script>";
                }
            }
        }
    }
    else
    if ( isset( $_GET['user'] ) )
    {
        $SO_user = $_GET['user'];
    }
    else
    if ( isset( $_GET['account'] ) )
    {
        $login_account = $_GET['account'];
        $login_account = str_replace(' ', '', $login_account);
    }

    if ( strlen($SO_user) > 0 )
    {
        $ERPIP = get_ERP_IP();
        $SSOIP = get_SSO_IP();
        if (strcmp($clientIP, $ERPIP) == 0 || strcmp($clientIP, $SSOIP) == 0)
        {
            $user_data = get_User_data($SO_user);

            $datacnt = count($user_data);
            if ($datacnt > 0)
            {

                // 寫入 Session 變數值
                $_SESSION['authenticated'] = true;
                $_SESSION['accountid'] = $SO_user;
                $_SESSION['accountname'] = $user_data['name'];
                $_SESSION['groupid'] = $user_data['groupid'];
                $_SESSION['userid'] = $user_data['userid'];
                $_SESSION['maingroups'] = $user_data['maingroups'];
                $_SESSION['subgroups'] = $user_data['subgroups'];
                $_SESSION['login_time'] = date('Y-m-d h:i:s');
                $_SESSION['PAGE_CONFIG'] = $PAGE_CONFIG;
                $redir = '/index.php';

                // 重導到相關頁面
                header("Location: $redir");
                exit;
            }
        }
    }

?>
<!DOCTYPE HTML>
<html lang="en">
    <?php
        include ('vils_html_head.php');
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

        <form action="/login.php" method="post">
          <div class="form-group has-feedback">
            <input type="text" name="account" class="form-control" value="<?php echo $login_account; ?>"  placeholder=<?php echo $login_sign_in_account;?> required>
            <span class="glyphicon glyphicon-user form-control-feedback"></span>
          </div>
          <div class="form-group has-feedback">
            <input type="password" name="password" class="form-control" value="<?php echo $login_password; ?>" placeholder=<?php echo $login_sign_in_password;?> required>
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
            <div class="col-xs-2">
                <span class='logo-mini'>
                    <div class='image'>
                      <img src='/images/<?php echo $main_logo_small_file; ?> ' class='img-circle' alt='VILS Logo' id='logo' width="40" height="40">
                    </div>
                </span>
            </div>
            <!-- /.col -->

            <div class="col-xs-3">
            <?php
                if ($GL_CONFIG_ENABLE_EMAIL_ACCOUNT)
                {
                    echo '  <a href="register.php">申請帳號</a>';
                }
            ?>
            </div>

            <div class="col-xs-3">
            </div>
            <!-- /.col -->
            <div class="col-xs-4">
              <button type="submit" class="btn btn-primary btn-block btn-flat pull-right"><?php echo $login_sign_in_loginbtn;?></button>
            </div>
            <!-- /.col -->
          </div>
        </form>

      </div>
      <!-- /.login-box-body -->
    </div>
    <!-- /.login-box -->
    <div align="center"><h4><span class="label">Your IP: </span><span class="label label-success"><?php echo $clientIP;?></span></h4></div>

  <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <!-- <script src="/js/vils.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</body>

</html>
